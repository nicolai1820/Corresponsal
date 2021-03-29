//
// Created by NETCOM on 18/02/2020.
//

/*
 * Cierre.c
 *
 *  Created on: 2/12/2013
 *      Author: Calidad03
 */


#include "Definiciones.h"
#include "Tipo_datos.h"
#include <string>
#include <stdio.h>
#include <Utilidades.h>
#include <Archivos.h>
#include "configuracion.h"
#include "Mensajeria.h"
#include "bancolombia.h"
#include "Inicializacion.h"
#include <native-lib.h>
#include <comunicaciones.h>
#include "android/log.h"

#define  LOG_TAG    "NETCOM_BANCOLOMBIA_CIERRE"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

void ejecutarOpcionesCierreBancolombia(void);
int cierreCnbBancolombia(int tipoCierre);
int confirmarEnviarCierre(void);
int validarCierreTotal(void);
void calcularCierreBancolombia(int tipoCierre);
void imprimirReporteCierre(int tipoCierre);
void cambiarEstadoCierre(void);
int enviarCierre(void);
ResultISOPack empaquetarCierreVenta(DatosCierreVenta datosCierreVenta, char * codigoProceso);
int desempaquetarCierreVenta(char * inputData, int cantidadBytes);
int cierreDescuadrado(int totalTransacciones);
int empaquetarTramaVenta(DatosCnbBancolombia datosVenta, char * outputData);
int verificarExistenciaTransacciones(void);
DatosCierreCnbBancolombia datosCierreCnbRbm;
DatosCierreVenta datosCierreVenta;
int maximoId = 0;
uChar saldoEfectivo[20 + 1];
int modoDatafono = 0;

const ITEM_MENU menuCierre[] = {
        { "PARCIAL", "1" },
        { "GENERAL", "2" } };

/**
 * @brief Controla el cierre para CNB RBM.
 */
void ejecutarOpcionesCierreBancolombia(void) {

    char claveActual[4 + 1];
    int resultado = 0;

    memset(claveActual, 0x00, sizeof(claveActual));

    getParameter(VALOR_CLAVE_SUPERVISOR, claveActual);
    //resultado = capturarClave("CLAVE SUPERVISOR", "INGRESE", "CLAVE", claveActual, 4);

    if (resultado > 0) {
        do {
            // resultado = showMenu("CIERRE CORRESPONSALES", 0, 0, 2, menuCierre, 30 * 1000);

            if (resultado > 0) {
                resultado = cierreCnbBancolombia(resultado);
            }
        } while (resultado == -1);
    }
}

int cierreCnbBancolombia(int tipoCierre) {
    long tam = 0;
    int resultado = 1;
    int tamJournal = 0;
    char datos[2 + 1];
    uChar terminalCnb[8 + 1];
    uChar cambioTerminalCnb[1 + 1];
    uChar cnbInicializado[1 + 1];
    uChar estadoInicializacion[2 + 1];

    memset(datos, 0x00, sizeof(datos));
    memset(terminalCnb, 0x00, sizeof(terminalCnb));
    memset(cambioTerminalCnb, 0x00, sizeof(cambioTerminalCnb));
    memset(cnbInicializado, 0x00, sizeof(cnbInicializado));
    memset(estadoInicializacion, 0x00, sizeof(estadoInicializacion));

    tam = tamArchivo(discoNetcom, (char*)JOURNAL_CNB_BANCOLOMBIA);
    tamJournal = sizeof(DatosCnbBancolombia);
    maximoId = tam / tamJournal;
    LOGI("tipoCierre %d ",tipoCierre);
    if (tipoCierre == CNB_CIERRE_TOTAL) {
        resultado = validarCierreTotal();
    }

    if (resultado > 0) {

        if (maximoId > 0) {
            calcularCierreBancolombia(tipoCierre);

            modoDatafono = verificarModoCNB();

            imprimirReporteCierre(tipoCierre);
            LOGI("salio de imprimir cierre  ");
            pprintf((char*)"final");
        } else {
            getParameter(CNB_TERMINAL, terminalCnb);

            if (terminalCnb[0] != 0x00) {
                getParameter(CNB_INICIALIZADO, cnbInicializado);
            }
            getParameter(CNB_CAMBIO_TERMINAL, cambioTerminalCnb);
            getParameter(TERMINAL_INICIALIZADA, estadoInicializacion);

            //llamado desde codigo terminal del cnb

            if (strcmp(cambioTerminalCnb, "1") == 0) {
                resultado = 1;
                //llamado desde codigo terminal/DESHABILITAR CNB MIXTO
                if (terminalCnb[0] != 0x00 && strcmp(cnbInicializado, "1") == 0) {
                    enviarStringToJava((char *)"CIERRE SIN TRANSACCIONES CIERRE NO PERMITIDO", (char *)"mostrarInicializacion");
                } else if (terminalCnb[0] == 0x00 && strcmp(estadoInicializacion, "1") == 0) {
                    enviarStringToJava((char *)"CIERRE SIN TRANSACCIONES CIERRE NO PERMITIDO", (char *)"mostrarInicializacion");
                }
            } else {
                resultado = 0;
            }
        }
    }

    if ((resultado > 0) && (tipoCierre == CNB_CIERRE_TOTAL)) {
        // Confirmar datos
        resultado = confirmarEnviarCierre();

        //Enviar datos
        if (resultado == 0) {
            resultado = enviarCierre();
        }
    }

    return resultado;
}

int confirmarEnviarCierre(void) {

    int resultado = 0;
    uChar valorFormato[12 + 1];
    uChar linea1[22 + 1];
    uChar linea2[22 + 1];
    uChar linea3[22 + 1];
    uChar linea4[19 + 1];
    char lineaResumen[100 + 1] = {0x00};
    long valor = 0;

    memset(valorFormato, 0x00, sizeof(valorFormato));
    memset(linea1, 0x00, sizeof(linea1));
    memset(linea2, 0x00, sizeof(linea2));
    memset(linea3, 0x00, sizeof(linea3));
    memset(linea4, 0x00, sizeof(linea4));

    valor = (datosCierreCnbRbm.totalRetiroTarjeta + datosCierreCnbRbm.totalTransferencias);
    memset(valorFormato, 0x00, sizeof(valorFormato));
    formatString(0x00, valor, (char*)"", 0, valorFormato, 1);

    strcpy(linea1, "Trans. Tarjeta:");
    strcpy(linea2, valorFormato);

    memset(valorFormato, 0x00, sizeof(valorFormato));
    valor = (datosCierreCnbRbm.totalAnuladasTransferencia + datosCierreCnbRbm.totalAnuladasDeposito
             + datosCierreCnbRbm.totalAnuladasRecaudo + datosCierreCnbRbm.totalAnuladasPagoCartera
             + datosCierreCnbRbm.totalAnuladasPagoTarjeta);
    memset(valorFormato, 0x00, sizeof(valorFormato));
    formatString(0x00, valor, (char*)"", 0, valorFormato, 1);

    strcpy(linea3, "Anulaciones:");
    strcpy(linea4, valorFormato);

    //resultado = confirmarDatos("TOTAL", linea1, linea2, linea3, linea4);
    strcpy(lineaResumen, linea1);
    strcat(lineaResumen," ");
    strcat(lineaResumen, linea2);
    strcat(lineaResumen," ");
    strcat(lineaResumen, linea3);
    strcat(lineaResumen, " ");
    strcat(lineaResumen, linea4);
    enviarStringToJava(lineaResumen, (char *)"mostrarInicializacion");
    return resultado;
}

/**
 * @brief Verifica si se puede hacer el cierre total
 * @return 2: Si puede continuar con el cierre total (2 == CNB_CIERRE_TOTAL), -1: si no puede.
 */
int validarCierreTotal(void) {

    int idx = 0;
    DatosCnbBancolombia datosVenta;
    int resultado = 1;
    char lineaResumen[100 +1] = {0x00};

    for (idx = 0; idx < maximoId; idx++) {
        memset(&datosVenta, 0x00, sizeof(datosVenta));

        datosVenta = buscarReciboCNBByID(idx);
        LOGI("datosVenta.estadoCierre %s  ",datosVenta.estadoCierre);
        LOGI("datosVenta.tipoTransaccion %s  ",datosVenta.tipoTransaccion);
        if (atoi(datosVenta.estadoCierre) != CNB_CIERRE_PARCIAL) {
            screenMessageFiveLine("MENSAJE", "DEBE REALIZAR", "PRIMERO", "CIERRE PARCIAL", "","",lineaResumen);
            enviarStringToJava(lineaResumen, (char *)"mostrarInicializacion");
            resultado = -1;
            break;
        }
    }
    return resultado;
}

void calcularCierreBancolombia(int tipoCierre) {

    DatosCnbBancolombia datosVenta;
    int idx = 0;
    int tipoTransaccion = 0;
    char valorVenta[10 + 1];
//	int posicion = 0;

    memset(&datosCierreCnbRbm, 0x00, sizeof(datosCierreCnbRbm));
    memset(&datosCierreVenta, 0x00, sizeof(datosCierreVenta));
    memset(&datosVenta, 0x00, sizeof(datosVenta));
    memset(valorVenta, 0x00, sizeof(valorVenta));

    for (idx = 0; idx < maximoId; idx++) {
//		posicion = sizeof(DatosCnbBancolombia) * idx;

        memset(&datosVenta, 0x00, sizeof(datosVenta));

        datosVenta = buscarReciboCNBByID(idx);

        tipoTransaccion = atoi(datosVenta.tipoTransaccion);

        memset(valorVenta, 0x00, sizeof(valorVenta));

        if (strlen(datosVenta.totalVenta) > 2) {
            memcpy(valorVenta, datosVenta.totalVenta, strlen(datosVenta.totalVenta) - 2); //-2 por la parte decimal
        } else {
            strcpy(valorVenta, "0");
        }

        if (tipoTransaccion == TRANSACCION_BCL_TRANSFERENCIA) {

            if (atoi(datosVenta.estadoCierre) != tipoCierre) {

                if (atoi(datosVenta.estadoTransaccion) == 1) {
                    datosCierreCnbRbm.cantidadTransferencias++;
                    datosCierreCnbRbm.totalTransferencias += atol(valorVenta);

                    datosCierreVenta.totalVentaDebito += atol(valorVenta);
                    datosCierreVenta.cantidadVentaDebito++;
                } else {
                    datosCierreCnbRbm.cantidadAnuladasTransferencia++;
                    datosCierreCnbRbm.totalAnuladasTransferencia += atol(valorVenta);
                }
            }

        } else if (tipoTransaccion == TRANSACCION_BCL_RETIRO) { //// RETIRO CON TARJETA

            if (atoi(datosVenta.estadoCierre) != tipoCierre) {

                datosCierreCnbRbm.cantidadRetiroTarjeta++;
                datosCierreCnbRbm.totalRetiroTarjeta += atol(valorVenta);
            }

            if (strncmp(datosVenta.tipoCuenta, "30", 2) == 0) {
                datosCierreVenta.totalVentaCredito += atol(valorVenta);
                datosCierreVenta.cantidadVentaCredito++;
            } else {
                datosCierreVenta.totalVentaDebito += atol(valorVenta);
                datosCierreVenta.cantidadVentaDebito++;
            }

        } else if (tipoTransaccion == TRANSACCION_BCL_RETIRO_EFECTIVO) {

            if (atoi(datosVenta.estadoCierre) != tipoCierre) {

                datosCierreCnbRbm.cantidadRetiroEfectivo++;
                datosCierreCnbRbm.totalRetiroEfectivo += atol(valorVenta);
            }

            datosCierreVenta.totalVentaDebito += atol(valorVenta);
            datosCierreVenta.cantidadVentaDebito++;
        } else if (tipoTransaccion == TRANSACCION_BCL_DEPOSITO) {

            if (atoi(datosVenta.estadoCierre) != tipoCierre) {

                if (atoi(datosVenta.estadoTransaccion) == 1) {
                    datosCierreCnbRbm.cantidadDeposito++;
                    datosCierreCnbRbm.totalDeposito += atol(valorVenta);

                    datosCierreVenta.totalVentaDebito += atol(valorVenta);
                    datosCierreVenta.cantidadVentaDebito++;
                } else {
                    datosCierreCnbRbm.cantidadAnuladasDeposito++;
                    datosCierreCnbRbm.totalAnuladasDeposito += atol(valorVenta);
                }
            }

        } else if (tipoTransaccion == TRANSACCION_BCL_RECAUDO) {

            if (atoi(datosVenta.estadoCierre) != tipoCierre) {
                if (atoi(datosVenta.estadoTransaccion) == 1) {
                    datosCierreCnbRbm.cantidadRecaudo++;
                    datosCierreCnbRbm.totalRecaudo += atol(valorVenta);

                    datosCierreVenta.totalVentaDebito += atol(valorVenta);
                    datosCierreVenta.cantidadVentaDebito++;
                } else {
                    datosCierreCnbRbm.cantidadAnuladasRecaudo++;
                    datosCierreCnbRbm.totalAnuladasRecaudo += atol(valorVenta);
                }
            }

        } else if (tipoTransaccion == TRANSACCION_BCL_PAGO_CARTERA) {
            if (atoi(datosVenta.estadoCierre) != tipoCierre) {
                if (atoi(datosVenta.estadoTransaccion) == 1) {
                    datosCierreCnbRbm.cantidadPagoCartera++;
                    datosCierreCnbRbm.totalPagoCartera += atol(valorVenta);

                    datosCierreVenta.totalVentaDebito += atol(valorVenta);
                    datosCierreVenta.cantidadVentaDebito++;
                } else {
                    datosCierreCnbRbm.cantidadAnuladasPagoCartera++;
                    datosCierreCnbRbm.totalAnuladasPagoCartera += atol(valorVenta);
                }
            }

        }

        else if (tipoTransaccion == TRANSACCION_BCL_PAGO_TARJETA) {

            if (atoi(datosVenta.estadoCierre) != tipoCierre) {
                if (atoi(datosVenta.estadoTransaccion) == 1) {
                    datosCierreCnbRbm.cantidadPagoTarjetaCredito++;
                    datosCierreCnbRbm.totalPagoTarjetaCredito += atol(valorVenta);

                    datosCierreVenta.totalVentaDebito += atol(valorVenta);
                    datosCierreVenta.cantidadVentaDebito++;
                } else {
                    datosCierreCnbRbm.cantidadAnuladasPagoTarjeta++;
                    datosCierreCnbRbm.totalAnuladasPagoTarjeta += atol(valorVenta);
                }
            }

        } else if (tipoTransaccion == TRANSACCION_BCL_SALDO) {

            if (atoi(datosVenta.tipoProducto) == 32) {
                datosCierreCnbRbm.cantidadCupos++;
            } else {
                datosCierreCnbRbm.cantidadConsultas++;
            }
        } else if (tipoTransaccion == TRANSACCION_BCL_GIROS) {

            if (atoi(datosVenta.estadoCierre) != tipoCierre) {
                if (atoi(datosVenta.estadoTransaccion) == 1) {
                    datosCierreCnbRbm.cantidadEnvioGiros++;
                    datosCierreCnbRbm.totalEnvioGiros += atol(valorVenta);

                    datosCierreVenta.totalVentaDebito += atol(valorVenta);
                    datosCierreVenta.cantidadVentaDebito++;
                }
            }
        } else if (tipoTransaccion == TRANSACCION_BCL_RECLAMAR_GIROS) {

            if (atoi(datosVenta.estadoCierre) != tipoCierre) {
                if (atoi(datosVenta.estadoTransaccion) == 1) {
                    datosCierreCnbRbm.cantidadReclamacionGiros++;
                    datosCierreCnbRbm.totalReclamacionGiros += atol(valorVenta);

                    datosCierreVenta.totalVentaDebito += atol(valorVenta);
                    datosCierreVenta.cantidadVentaDebito++;
                }
            }
        } else if (tipoTransaccion == TRANSACCION_BCL_CANCELAR_GIROS) {

            if (atoi(datosVenta.estadoCierre) != tipoCierre) {
                if (atoi(datosVenta.estadoTransaccion) == 1) {
                    datosCierreCnbRbm.cantidadCancelacionGiros++;
                    datosCierreCnbRbm.totalCancelacionGiros += atol(valorVenta);

                    datosCierreVenta.totalVentaDebito += atol(valorVenta);
                    datosCierreVenta.cantidadVentaDebito++;
                }
            }

        }
    }
}

void imprimirReporteCierre(int tipoCierre) {

    TablaUnoInicializacion tablaUno;
    uChar lineaSalida[100 + 1];
    uChar valor[20 + 1];
    uChar terminal[8 + 1];
    uChar codigoBanco[SIZE_CODIGO_BANCO_CNB + 1];
    long total = 0;
    int cantidad = 0;
    uChar auxiliar[20 + 1];
    long sumaEfectivo = 0;

    memset(&tablaUno, 0x00, sizeof(tablaUno));
    memset(codigoBanco, 0x00, sizeof(codigoBanco));
    memset(auxiliar, 0x00, sizeof(auxiliar));

    //logo
    //imprimirEncabezado();POR AHORA VUELA

    if (modoDatafono == MODO_MIXTO) {
        tablaUno = desempaquetarCNB();
        getParameter(CNB_TERMINAL, terminal);
    } else {
        tablaUno = _desempaquetarTablaCeroUno_();
        getTerminalId(terminal);
    }

    getParameter(CNB_BANCO, codigoBanco);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "%s\n", tablaUno.defaultMerchantName); // nombre comercio
    pprintf(lineaSalida);
    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "%s\n\n", tablaUno.receiptLine2); // direccion comercio
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "Terminal:       %s\n", terminal);
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "Corresponsal: %s\n", tablaUno.receiptLine3);
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "Banco:              %s", codigoBanco);
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));

    if (tipoCierre == CNB_CIERRE_TOTAL) {
        sprintf(lineaSalida,"%s", "      CIERRE TOTAL     ");
    } else {
        sprintf(lineaSalida,"%s", "     CIERRE PARCIAL     ");
    }

    pprintf(lineaSalida);
    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida,"%s", "        TOTALES         ");
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida,"%s", "DESCRIP.  CANT  TOT(+/-)");
    pprintf(lineaSalida);


    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    formatString(0x00, datosCierreCnbRbm.totalTransferencias, (char *)"", 0, valor, 1);
    sprintf(lineaSalida, "TRANSFERENCIA         %3d%8s%15s", datosCierreCnbRbm.cantidadTransferencias, " ", valor);
    pprintf(lineaSalida);

    pprintf("------------------------------------------------");

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    memset(auxiliar, 0x00, sizeof(auxiliar));
    formatString(0x00, (datosCierreCnbRbm.totalRetiroTarjeta + datosCierreCnbRbm.totalRetiroEfectivo), "", 0, valor, 1);
    strcat(auxiliar, "-");
    strcat(auxiliar, valor);
    sprintf(lineaSalida, "RETIRO                %3d%8s%15s",
            (datosCierreCnbRbm.cantidadRetiroTarjeta + datosCierreCnbRbm.cantidadRetiroEfectivo), " ", auxiliar);
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    memset(auxiliar, 0x00, sizeof(auxiliar));
    formatString(0x00, datosCierreCnbRbm.totalDeposito, "", 0, valor, 1);
    strcat(auxiliar, "+");
    strcat(auxiliar, valor);
    sprintf(lineaSalida, "DEPOSITO              %3d%8s%15s", datosCierreCnbRbm.cantidadDeposito, " ", auxiliar);
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    memset(auxiliar, 0x00, sizeof(auxiliar));
    formatString(0x00, datosCierreCnbRbm.totalRecaudo, "", 0, valor, 1);
    strcat(auxiliar, "+");
    strcat(auxiliar, valor);
    sprintf(lineaSalida, "RECAUDO               %3d%8s%15s", datosCierreCnbRbm.cantidadRecaudo, " ", auxiliar);
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    memset(auxiliar, 0x00, sizeof(auxiliar));
    formatString(0x00, datosCierreCnbRbm.totalPagoTarjetaCredito, "", 0, valor, 1);
    strcat(auxiliar, "+");
    strcat(auxiliar, valor);
    sprintf(lineaSalida, "PAGO T. CREDITO       %3d%8s%15s", datosCierreCnbRbm.cantidadPagoTarjetaCredito, " ",
            auxiliar);
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    memset(auxiliar, 0x00, sizeof(auxiliar));
    formatString(0x00, datosCierreCnbRbm.totalPagoCartera, "", 0, valor, 1);
    strcat(auxiliar, "+");
    strcat(auxiliar, valor);
    sprintf(lineaSalida, "PAGO CARTERA          %3d%8s%15s", datosCierreCnbRbm.cantidadPagoCartera, " ", auxiliar);
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    memset(auxiliar, 0x00, sizeof(auxiliar));
    formatString(0x00, datosCierreCnbRbm.totalEnvioGiros, "", 0, valor, 1);
    strcat(auxiliar, "+");
    strcat(auxiliar, valor);
    sprintf(lineaSalida, "ENVIO GIRO            %3d%8s%15s", datosCierreCnbRbm.cantidadEnvioGiros, " ", auxiliar);
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    memset(auxiliar, 0x00, sizeof(auxiliar));
    formatString(0x00, datosCierreCnbRbm.totalReclamacionGiros, "", 0, valor, 1);
    strcat(auxiliar, "-");
    strcat(auxiliar, valor);
    sprintf(lineaSalida, "RECLAMO GIRO          %3d%8s%15s", datosCierreCnbRbm.cantidadReclamacionGiros, " ", auxiliar);
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    memset(auxiliar, 0x00, sizeof(auxiliar));
    formatString(0x00, datosCierreCnbRbm.totalCancelacionGiros, "", 0, valor, 1);
    strcat(auxiliar, "-");
    strcat(auxiliar, valor);
    sprintf(lineaSalida, "CANCELACION GIRO      %3d%8s%15s", datosCierreCnbRbm.cantidadCancelacionGiros, " ", auxiliar);
    pprintf(lineaSalida);

    pprintf("------------------------------------------------");

    sumaEfectivo = datosCierreCnbRbm.totalDeposito + datosCierreCnbRbm.totalRecaudo
                   + datosCierreCnbRbm.totalPagoTarjetaCredito + datosCierreCnbRbm.totalPagoCartera
                   + datosCierreCnbRbm.totalEnvioGiros;

    if (sumaEfectivo < (datosCierreCnbRbm.totalRetiroTarjeta + datosCierreCnbRbm.totalRetiroEfectivo
                        + datosCierreCnbRbm.totalReclamacionGiros + datosCierreCnbRbm.totalCancelacionGiros)) {
        total = (datosCierreCnbRbm.totalRetiroTarjeta + datosCierreCnbRbm.totalRetiroEfectivo
                 + datosCierreCnbRbm.totalReclamacionGiros + datosCierreCnbRbm.totalCancelacionGiros) - sumaEfectivo;
    } else {
        total = sumaEfectivo - (datosCierreCnbRbm.totalRetiroTarjeta + datosCierreCnbRbm.totalRetiroEfectivo
                                + datosCierreCnbRbm.totalReclamacionGiros + datosCierreCnbRbm.totalCancelacionGiros);
    }

    cantidad = datosCierreCnbRbm.cantidadDeposito + datosCierreCnbRbm.cantidadRecaudo
               + datosCierreCnbRbm.cantidadPagoTarjetaCredito + datosCierreCnbRbm.cantidadPagoCartera
               + datosCierreCnbRbm.cantidadRetiroEfectivo + datosCierreCnbRbm.cantidadRetiroTarjeta
               + datosCierreCnbRbm.cantidadEnvioGiros + datosCierreCnbRbm.cantidadReclamacionGiros
               + datosCierreCnbRbm.cantidadCancelacionGiros;

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    formatString(0x00, total, "", 0, valor, 1);
    memset(saldoEfectivo, 0x00, sizeof(saldoEfectivo));
    if (sumaEfectivo < (datosCierreCnbRbm.totalRetiroTarjeta + datosCierreCnbRbm.totalRetiroEfectivo
                        + datosCierreCnbRbm.totalReclamacionGiros + datosCierreCnbRbm.totalCancelacionGiros)) {
        strcat(saldoEfectivo, "-");
    }

    strcat(saldoEfectivo, valor);
    sprintf(lineaSalida, "SALDO EFECTIVO        %3d%8s%15s", cantidad, " ", saldoEfectivo);
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "CONSULTAS DE SALDO    %8s%15d", " ", datosCierreCnbRbm.cantidadConsultas);
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "CONSULTAS CUPO CUENTA CB %5s%15d", " ", datosCierreCnbRbm.cantidadCupos);
    pprintf(lineaSalida);

    //screenMessage("MENSAJE", "REPORTE", "CORRESPONSALES", "NO BANCARIOS", 2 * 1000);
    LOGI("imprimirDetalleCierreCNB IN  ");
    imprimirDetalleCierreCNB(tipoCierre);
    LOGI("imprimirDetalleCierreCNB oUT  ");
}


void imprimirDetalleCierreCNB(int tipoCierre) {

    uChar lineaSalida[100 + 1];
    uChar terminal[8 + 1]=  {0x00};
    uChar codigoBanco[SIZE_CODIGO_BANCO_CNB + 1];
    uChar valor[20 + 1];
    TablaUnoInicializacion tablaUno;

    memset(&tablaUno, 0x00, sizeof(tablaUno));
    memset(codigoBanco, 0x00, sizeof(codigoBanco));
    memset(valor, 0x00, sizeof(valor));

    //logo
    //imprimirEncabezado();

    if (modoDatafono == MODO_MIXTO) {
        tablaUno = desempaquetarCNB();
        getParameter(CNB_TERMINAL, terminal);
    } else {
        tablaUno = _desempaquetarTablaCeroUno_();
        getTerminalId(terminal);
    }

    getParameter(CNB_BANCO, codigoBanco);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "%s", tablaUno.defaultMerchantName); // nombre comercio
    pprintf(lineaSalida);
    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "%s", tablaUno.receiptLine2); // direccion comercio
    pprintf(lineaSalida);
    LOGI("comercio oUT  ");
    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "Terminal:       %s", terminal);
    pprintf(lineaSalida);
    LOGI("terminal  ");
    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "Corresponsal: %s", tablaUno.receiptLine3);
    pprintf(lineaSalida);
    LOGI("corresponal  ");
    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "Banco:              %s", codigoBanco);
    pprintf(lineaSalida);
    LOGI("banco  ");
    memset(lineaSalida, 0x00, sizeof(lineaSalida));

    if (tipoCierre == CNB_CIERRE_TOTAL) {
        sprintf(lineaSalida,"%s", "      CIERRE TOTAL      ");
    } else {
        sprintf(lineaSalida,"%s" ,"     CIERRE PARCIAL     ");
    }
    LOGI("tipo de cierre  ");
    pprintf(lineaSalida);
    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "%s" ,"       DETALLES        ");
    pprintf(lineaSalida);


    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "%s" ,"FECHA    DESCRIP.        RECIBO       TOTAL(+/-)  ");
    pprintf(lineaSalida);
    LOGI("cabezote ");
    detallesTransaccion((datosCierreCnbRbm.cantidadRetiroTarjeta + datosCierreCnbRbm.cantidadRetiroEfectivo),
                        (datosCierreCnbRbm.totalRetiroTarjeta + datosCierreCnbRbm.totalRetiroEfectivo), TRANSACCION_BCL_RETIRO,
                        "RETIRO", tipoCierre, 0, 0);
    LOGI("retiro  ");
    detallesTransaccion(datosCierreCnbRbm.cantidadDeposito, datosCierreCnbRbm.totalDeposito,
                        TRANSACCION_BCL_DEPOSITO, "DEPOSITO", tipoCierre, datosCierreCnbRbm.cantidadAnuladasDeposito,
                        datosCierreCnbRbm.totalAnuladasDeposito);
    imprimirAnuladas(datosCierreCnbRbm.cantidadAnuladasDeposito, datosCierreCnbRbm.totalAnuladasDeposito,
                     TRANSACCION_BCL_DEPOSITO, tipoCierre);
    LOGI("deposito  ");
    detallesTransaccion(datosCierreCnbRbm.cantidadRecaudo, datosCierreCnbRbm.totalRecaudo,
                        TRANSACCION_BCL_RECAUDO, "RECAUDO", tipoCierre, datosCierreCnbRbm.cantidadAnuladasRecaudo,
                        datosCierreCnbRbm.totalAnuladasRecaudo);
    imprimirAnuladas(datosCierreCnbRbm.cantidadAnuladasRecaudo, datosCierreCnbRbm.totalAnuladasRecaudo,
                     TRANSACCION_BCL_RECAUDO, tipoCierre);
    LOGI("recaudo  ");
    detallesTransaccion(datosCierreCnbRbm.cantidadPagoTarjetaCredito,
                        datosCierreCnbRbm.totalPagoTarjetaCredito, TRANSACCION_BCL_PAGO_TARJETA, "PAGO T. CREDITO", tipoCierre,
                        datosCierreCnbRbm.cantidadAnuladasPagoTarjeta, datosCierreCnbRbm.totalAnuladasPagoTarjeta);
    imprimirAnuladas(datosCierreCnbRbm.cantidadAnuladasPagoTarjeta, datosCierreCnbRbm.totalAnuladasPagoTarjeta,
                     TRANSACCION_BCL_PAGO_TARJETA, tipoCierre);
    LOGI("pago tc  ");
    detallesTransaccion(datosCierreCnbRbm.cantidadPagoCartera, datosCierreCnbRbm.totalPagoCartera,
                        TRANSACCION_BCL_PAGO_CARTERA, "PAGO CARTERA", tipoCierre, datosCierreCnbRbm.cantidadAnuladasPagoCartera,
                        datosCierreCnbRbm.totalAnuladasPagoCartera);
    imprimirAnuladas(datosCierreCnbRbm.cantidadAnuladasPagoCartera, datosCierreCnbRbm.totalAnuladasPagoCartera,
                     TRANSACCION_BCL_PAGO_CARTERA, tipoCierre);
    LOGI("pago cartera  ");
    detallesTransaccion(datosCierreCnbRbm.cantidadTransferencias, datosCierreCnbRbm.totalTransferencias,
                        TRANSACCION_BCL_TRANSFERENCIA, "TRANSFERENCIA", tipoCierre, datosCierreCnbRbm.cantidadAnuladasTransferencia,
                        datosCierreCnbRbm.totalAnuladasTransferencia);
    imprimirAnuladas(datosCierreCnbRbm.cantidadAnuladasTransferencia,
                     datosCierreCnbRbm.totalAnuladasTransferencia, TRANSACCION_BCL_TRANSFERENCIA, tipoCierre);
    LOGI("transferencias  ");
    detallesTransaccion(datosCierreCnbRbm.cantidadEnvioGiros, datosCierreCnbRbm.totalEnvioGiros,
                        TRANSACCION_BCL_GIROS, "ENVIO GIRO", tipoCierre, 0, 0);
    LOGI("envio giros   ");
    detallesTransaccion(datosCierreCnbRbm.cantidadReclamacionGiros, datosCierreCnbRbm.totalReclamacionGiros,
                        TRANSACCION_BCL_RECLAMAR_GIROS, "RECLAMO GIRO", tipoCierre, 0, 0);
    LOGI("reclamar girros  ");
    detallesTransaccion(datosCierreCnbRbm.cantidadCancelacionGiros, datosCierreCnbRbm.totalCancelacionGiros,
                        TRANSACCION_BCL_CANCELAR_GIROS, "CANCELAR GIRO", tipoCierre, 0, 0);
    LOGI("cancelar giros   ");
    //screenMessage("MENSAJE", "ACTUALIZANDO REPORTES", "UN MOMENTO", "POR FAVOR", 2 * 1000);

    // Realizar cambio de estado
    LOGI("realizar cambio de cierre  ");
    cambiarEstadoCierre();

    //screenMessage("MENSAJE", "", "**CIERRE**", "FINALIZADO", 2 * 1000);
}


void detallesTransaccion(int cantidad, long total, int tipoTransaccion, uChar *descripcion,
                         int tipoCierre, int cantidadAnuladas, long totalAnuladas) {

    uChar lineaSalida[100 + 1];
    DatosCnbBancolombia datosVenta;
    uChar valor[20 + 1];
    int idx = 0;
    uChar valorVenta[TAM_COMPRA_NETA + 1];
    int iRet = 0;

    if ((cantidad + cantidadAnuladas) > 0) {

        for (idx = 0; idx < maximoId; idx++) {

            memset(&datosVenta, 0x00, sizeof(datosVenta));
            memset(valor, 0x00, sizeof(valor));

            datosVenta = buscarReciboCNBByID(idx);

            if ((atoi(datosVenta.tipoTransaccion) == tipoTransaccion) && (atoi(datosVenta.estadoCierre) != tipoCierre)
                && (atoi(datosVenta.estadoTransaccion) == 1)) {

                memset(lineaSalida, 0x00, sizeof(lineaSalida));
                memset(valorVenta, 0x00, sizeof(valorVenta));
                if(atoi(datosVenta.totalVenta) > 0){
                    memcpy(valorVenta, datosVenta.totalVenta, strlen(datosVenta.totalVenta) - 2); //-2 por la parte decimal
                }

                memset(valor, 0x00, sizeof(valor));
                formatString(0x00, atol(valorVenta), (char*)"", 0, valor, 1);


                if (strcmp(datosVenta.codigoRespuesta, "WS") == 0) {
                    sprintf(lineaSalida, "%.2s/%.2s  %-15s%3s%6s%4s%13s", datosVenta.txnDate,
                            datosVenta.txnDate + 2, descripcion, "NN", datosVenta.numeroRecibo, " ", valor);
                } else {
                    sprintf(lineaSalida, "%.2s/%.2s  %-15s%3s%6s%4s%13s", datosVenta.txnDate,
                            datosVenta.txnDate + 2, descripcion, "  ", datosVenta.numeroRecibo, " ", valor);
                }

                pprintf(lineaSalida);


                iRet++;
            }

            if (cantidad == iRet) { ///// SE SALE CUANDO LLEGA AL MAXIMO DE TRANSACCION POR TIPO
                break;
            }
        }

        if ((iRet > 0) && (total > 0)) {
            pprintf("------------------------------------------------");
            memset(lineaSalida, 0x00, sizeof(lineaSalida));
            memset(valor, 0x00, sizeof(valor));

            formatString(0x00, total, "", 0, valor, 1);
            sprintf(lineaSalida, "TOTAL                    %3d%7s%13s", cantidad, " ", valor);

            pprintf(lineaSalida);
        }

    }

}



void imprimirAnuladas(int cantidad, long total, int tipoTransaccion, int tipoCierre) {

    char lineaSalida[60 + 1];
    char valor[20 + 1];

    if (cantidad > 0) {
        memset(lineaSalida, 0x00, sizeof(lineaSalida));
        memset(valor, 0x00, sizeof(valor));

        formatString(0x00, total, (char*)"", 0, valor, 1);
        sprintf(lineaSalida, "ANULACIONES              %3d%7s%13s\n\n", cantidad, " ", valor);

        pprintf(lineaSalida);
    }
}

void cambiarEstadoCierre(void) {

    int posicion = 0;
    int idx = 0;
    char dataDuplicado[TAM_JOURNAL_BANCOLOMBIA + 1];
    char auxiliarTiempo[TAM_TIME + 1];
    char auxiliarfecha[TAM_DATE + 1];

    DatosCnbBancolombia datosVenta;
    memset(auxiliarTiempo, 0x00, sizeof(auxiliarTiempo));
    memset(auxiliarfecha, 0x00, sizeof(auxiliarfecha));

    for (idx = 0; idx < maximoId; idx++) {
        memset(dataDuplicado, 0x00, sizeof(dataDuplicado));
        memset(&datosVenta, 0x00, sizeof(datosVenta));
        buscarArchivo(discoNetcom, (char*)JOURNAL_CNB_BANCOLOMBIA, dataDuplicado, posicion, sizeof(DatosCnbBancolombia));

        memcpy(&datosVenta, dataDuplicado, sizeof(DatosCnbBancolombia));

        if (atoi(datosVenta.estadoCierre) != CNB_CIERRE_PARCIAL) {
            memset(dataDuplicado, 0x00, sizeof(dataDuplicado));

            memset(datosVenta.estadoCierre, 0x00, sizeof(datosVenta.estadoCierre));
            sprintf(datosVenta.estadoCierre, "%d", CNB_CIERRE_PARCIAL);

            memcpy(dataDuplicado, &datosVenta, sizeof(DatosCnbBancolombia));
            actualizarArchivo(discoNetcom, (char*)JOURNAL_CNB_BANCOLOMBIA, dataDuplicado, posicion, sizeof(DatosCnbBancolombia));
        }

        posicion += sizeof(DatosCnbBancolombia);
    }

}

int enviarCierre(void) {

    ResultISOPack resultadoIsoPackMessage;
    char dataRecibida[512 + 1];
    int cantidadBytes = 1;
    int resultado = 0;
    int cierre = 0;
    DatosVenta datosVentaVacio;

    memset(dataRecibida, 0x00, sizeof(dataRecibida));
    memset(&datosVentaVacio, 0x00, sizeof(datosVentaVacio));

    resultadoIsoPackMessage = empaquetarCierreVenta(datosCierreVenta, (char*)"920000");

    if (resultadoIsoPackMessage.responseCode > 0) {
        LOGI("realizar Cierre Bancolombia ");
        cantidadBytes = realizarTransaccion(resultadoIsoPackMessage.isoPackMessage, resultadoIsoPackMessage.totalBytes,
                                            dataRecibida, TRANSACCION_CIERRE, CANAL_PERMANENTE, REVERSO_NO_GENERADO);
        if (cantidadBytes > 0) {
            resultado = desempaquetarCierreVenta(dataRecibida, cantidadBytes);

            if (resultado == 1) {
                borrarArchivo(discoNetcom, (char*)JOURNAL_CNB_BANCOLOMBIA);
                borrarArchivo(discoNetcom, (char*)DIRECTORIO_JOURNALS);

            } else if (resultado == -1) {
                cierre = cierreDescuadrado(maximoId);

                if (cierre > 0) {
                    resultado = 1;
                    borrarArchivo(discoNetcom, (char*)JOURNAL_CNB_BANCOLOMBIA);
                    borrarArchivo(discoNetcom, (char*)DIRECTORIO_JOURNALS);
                }

            } else {
                resultado = -2;
            }
        } else {
            if (cantidadBytes == -3) {
                imprimirDeclinadas(datosVentaVacio, "TIME OUT GENERAL");
                resultado = cantidadBytes;
            }
        }

    }
    return resultado;
}

ResultISOPack empaquetarCierreVenta(DatosCierreVenta datosCierreVenta, char * codigoProceso) {

    char systemTrace[12 + 1];
    char terminalID[12 + 1];
    //char nii[4 + 1];
    char batch[24 + 1];
    char field63[100 + 1];
    int cantidadVentaCredito = 0;
    long totalVentaCredito = 0;
    int cantidadVentaDebito = 0;
    long totalVentaDebito = 0;
    int indice = 0;
    ResultISOPack resultadoIsoPackMessage;
    ISOHeader isoHeader8583;

    memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
    memset(&isoHeader8583, 0x00, sizeof(isoHeader8583));
    memset(systemTrace, 0x00, sizeof(systemTrace));
    memset(terminalID, 0x00, sizeof(terminalID));
    memset(batch, 0x00, sizeof(batch));
    memset(field63, 0x00, sizeof(field63));

    //Credito
    cantidadVentaCredito += datosCierreVenta.cantidadVentaCredito;

    totalVentaCredito += datosCierreVenta.totalVentaCredito;
    ////////////////////////////////////////////////////////////////////////////

    //Debito
    cantidadVentaDebito += datosCierreVenta.cantidadVentaDebito;

    totalVentaDebito += datosCierreVenta.totalVentaDebito;
    ///////////////////////////////////////////////////////////////////////////

    sprintf(field63 + indice, "%03d", cantidadVentaCredito);
    indice += 3;
    sprintf(field63 + indice, "%010ld00", totalVentaCredito);
    indice += 12;

    //sprintf(field63 + indice,"%03d",datosCierreVenta.cantidadAnuladasCredito);
    sprintf(field63 + indice, "%03d", 0);
    indice += 3;
    //se ajusta cierre para que no envie las anulaciones al tanden
    //sprintf(field63 + indice,"%010ld00",datosCierreVenta.totalAnuladasCredito);
    sprintf(field63 + indice, "%010d00", 0);
    indice += 12;

    sprintf(field63 + indice, "%03d", cantidadVentaDebito);
    indice += 3;
    sprintf(field63 + indice, "%010ld00", totalVentaDebito);
    indice += 12;

    //sprintf(field63 + indice,"%03d",datosCierreVenta.cantidadAnuladasDebito);
    sprintf(field63 + indice, "%03d", 0);
    indice += 3;
    //sprintf(field63 + indice,"%010ld00",datosCierreVenta.totalAnuladasDebito);
    sprintf(field63 + indice, "%010d00", 0);
    indice += 12;

    sprintf(field63 + indice, "%03d", 0);
    indice += 3;
    sprintf(field63 + indice, "%010d00", 0);
    indice += 12;

    sprintf(field63 + indice, "%03d", 0);
    indice += 3;
    sprintf(field63 + indice, "%010d00", 0);
    indice += 12;

    generarSystemTraceNumber(systemTrace, (char*)"");
    generarBatch(batch);

    if (modoDatafono == MODO_MIXTO) {
        getParameter(CNB_TERMINAL, terminalID);
    } else {
        getTerminalId(terminalID);
    }

    uChar nii[TAM_NII + 1];
    memset(nii, 0x00, sizeof(nii));

    getParameter(NII, nii);

    isoHeader8583.TPDU = 60;
    memcpy(isoHeader8583.destination, nii, TAM_NII + 1);
    memcpy(isoHeader8583.source, nii, TAM_NII + 1);
    setHeader(isoHeader8583);

    setMTI((char*)"0500");
    setField(3, codigoProceso, 6);
    setField(11, systemTrace, 6);
    setField(24, nii + 1, 3);
    setField(41, terminalID, 8);
    setField(42, (char*)"000000000000000", 15);
    setField(60, batch, 6);
    setField(63, field63, indice);

    resultadoIsoPackMessage = packISOMessage();

    return resultadoIsoPackMessage;
}

int desempaquetarCierreVenta(char * inputData, int cantidadBytes) {

    ResultISOPack resultadoIsoPackMessage;
    ResultISOUnpack resultadoUnpack;
    ISOFieldMessage isoFieldMessage;
    int resultado = 0;
    char linea1[20 + 1];
    char linea2[20 + 1];
    char lineaAuxiliar[100 + 1] = {0x00};
    char lineaResumen[100 + 1] = {0x00};

    memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
    memset(&resultadoUnpack, 0x00, sizeof(resultadoUnpack));
    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(linea1, 0x00, sizeof(linea1));
    memset(linea2, 0x00, sizeof(linea2));

    resultadoUnpack = unpackISOMessage(inputData, cantidadBytes);
    if (resultadoUnpack.responseCode > 0) {
        isoFieldMessage = getField(39);
        if (strcmp(isoFieldMessage.valueField, "00") == 0) {
            memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
            isoFieldMessage = getField(63);
            //*BATCH BALANCED2    *BATCH COMPLETE2
            LOGI("memcopy1  de  Cierre Bancolombia  %s",isoFieldMessage.valueField);
            LOGI("memcopy2  de  Cierre Bancolombia  %s",isoFieldMessage.valueField + 20);
            memcpy(lineaAuxiliar, isoFieldMessage.valueField, 16);
            memcpy(linea1, lineaAuxiliar, strlen(lineaAuxiliar));

            memset(lineaAuxiliar,0x00,sizeof(lineaAuxiliar));
            memcpy(lineaAuxiliar, isoFieldMessage.valueField + 20, isoFieldMessage.totalBytes - 16);
            memcpy(linea2, lineaAuxiliar, strlen(lineaAuxiliar));
            strcpy(lineaResumen, linea1);
            strcat(lineaResumen," ");
            strcat(lineaResumen, linea2);
            enviarStringToJava(lineaResumen, (char *)"mostrarInicializacion");
            //screenMessage("CIERRE", "CIERRE", linea1, linea2, 5 * 1000);
            resultado = 1;

        } else if (strcmp(isoFieldMessage.valueField, "95") == 0) {
            memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
            isoFieldMessage = getField(63);
            //*UPLOAD STARTED*     ERR:   27450.00
            LOGI("memcopy1  de  Cierre upload  %s",isoFieldMessage.valueField);
            memset(lineaAuxiliar,0x00,sizeof(lineaAuxiliar));
            memcpy(lineaAuxiliar, isoFieldMessage.valueField + 20, isoFieldMessage.totalBytes - 16);
            memcpy(linea1, lineaAuxiliar, strlen(lineaAuxiliar));

            memset(lineaAuxiliar,0x00,sizeof(lineaAuxiliar));
            memcpy(lineaAuxiliar, isoFieldMessage.valueField + 20, isoFieldMessage.totalBytes - 16);
            memcpy(linea2, lineaAuxiliar, strlen(lineaAuxiliar));
            LOGI("memcopy2  de  Cierre upload  %s",isoFieldMessage.valueField + 20);

            //screenMessage("CIERRE", "CIERRE", linea1, linea2, 5 * 1000);
            strcpy(lineaResumen, linea1);
            strcat(lineaResumen," ");
            strcat(lineaResumen, linea2);
            enviarStringToJava(lineaResumen, (char *)"mostrarInicializacion");
            resultado = -1;
        }

    }

    return resultado;
}

int cierreDescuadrado(int totalTransacciones) {

    int idx = 0;
    int catidadBytes = 0;
    int resultado = 1;
    DatosCnbBancolombia datosVenta;
    char outputData[512 + 1];
    char inputData[512 + 1];
    ResultISOPack resultadoIsoPackMessage;

    for (idx = 0; idx < totalTransacciones; idx++) {

        memset(&datosVenta, 0x00, sizeof(datosVenta));
        datosVenta = buscarReciboCNBByID(idx);

        if (IS_SUMAN_CIERRE( atoi(datosVenta.tipoTransaccion) ) == 1) {

            memset(outputData, 0x00, sizeof(outputData));
            memset(inputData, 0x00, sizeof(inputData));
            catidadBytes = empaquetarTramaVenta(datosVenta, outputData);

            if (catidadBytes > 0) {
                resultado = realizarTransaccion(outputData, catidadBytes, inputData, TRANSACCION_CIERRE,
                                                CANAL_PERMANENTE, REVERSO_NO_GENERADO);
                if (resultado < 0) {
                    resultado = -1;
                    idx = totalTransacciones;
                    break;
                }

            }
        }
    }

    if (resultado > 0) {
        memset(&datosCierreVenta, 0x00, sizeof(datosCierreVenta));
        memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
        memset(outputData, 0x00, sizeof(outputData));
        memset(inputData, 0x00, sizeof(inputData));

        calcularCierreBancolombia(CNB_CIERRE_TOTAL);

        resultadoIsoPackMessage = empaquetarCierreVenta(datosCierreVenta, (char*)"960000");
        resultado = realizarTransaccion(resultadoIsoPackMessage.isoPackMessage, resultadoIsoPackMessage.totalBytes,
                                        outputData, TRANSACCION_CIERRE, CANAL_PERMANENTE, REVERSO_NO_GENERADO);
    }

    return resultado;

}

int empaquetarTramaVenta(DatosCnbBancolombia datosVenta, char * outputData) {

    int cantidadBytes = 0;
    ResultISOPack resultadoIsoPackMessage;
    ISOHeader isoHeader8583;
    char field60[22 + 1];
    char systemTrace[6 + 1];
    char variableAux[12 + 1];

    memset(field60, 0x00, sizeof(field60));
    memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
    memset(&isoHeader8583, 0x00, sizeof(isoHeader8583));
    memset(systemTrace, 0x00, sizeof(systemTrace));

    generarSystemTraceNumber(systemTrace, (char*)"");
    memset(variableAux, 0x00, sizeof(variableAux));
    _leftPad_(variableAux, datosVenta.rrn, 0x30, 12);

    sprintf(field60, "0200%s            ", datosVenta.rrn);

    isoHeader8583.TPDU = 60;
    strcpy(isoHeader8583.destination, "0001");
    strcpy(isoHeader8583.source, "0001");
    setHeader(isoHeader8583);

    setMTI((char*)"0320");
    setField(3, datosVenta.processingCode, 6);
    setField(4, datosVenta.totalVenta, 12);
    setField(11, systemTrace, 6);
    setField(12, datosVenta.txnTime, 6);
    setField(13, datosVenta.txnDate, 4);
    setField(22, datosVenta.posEntrymode, 3);
    setField(24, datosVenta.nii, 3);
    setField(25, (char*)"00", 2);
    setField(35, datosVenta.track2, strlen(datosVenta.track2));
    setField(37, variableAux, 12);
    setField(38, datosVenta.codigoAprobacion, strlen(datosVenta.codigoAprobacion));
    setField(41, datosVenta.terminalId, 8);
    setField(60, field60, 22);
    setField(62, datosVenta.numeroRecibo, 6);

    resultadoIsoPackMessage = packISOMessage();

    if (resultadoIsoPackMessage.responseCode > 0) {
        cantidadBytes = resultadoIsoPackMessage.totalBytes;
        memcpy(outputData, resultadoIsoPackMessage.isoPackMessage, resultadoIsoPackMessage.totalBytes);
    }

    return cantidadBytes;
}

int verificarExistenciaTransacciones(void) {

    int exitenTransacciones = 0;

    exitenTransacciones = datosCierreCnbRbm.cantidadAnuladasDeposito + datosCierreCnbRbm.cantidadAnuladasPagoCartera
                          + datosCierreCnbRbm.cantidadAnuladasPagoTarjeta + datosCierreCnbRbm.cantidadAnuladasRecaudo
                          + datosCierreCnbRbm.cantidadAnuladasTransferencia + datosCierreCnbRbm.cantidadConsultas
                          + datosCierreCnbRbm.cantidadDeposito + datosCierreCnbRbm.cantidadPagoCartera
                          + datosCierreCnbRbm.cantidadPagoCartera + datosCierreCnbRbm.cantidadPagoTarjetaCredito
                          + datosCierreCnbRbm.cantidadRecaudo + datosCierreCnbRbm.cantidadRetiroEfectivo
                          + datosCierreCnbRbm.cantidadRetiroTarjeta + datosCierreCnbRbm.cantidadTransferencias;

    return exitenTransacciones;
}

/**
 * @brief función encargada de hacer el cierre del cnb_bancolombia desde la
 *        opcion de cambio de terminal cnb
 * @return iRet indica cierre exitoso o no
 * @author Edinson Cáceres Parra
 * @return
 */
int cierreCnbTerminalBancolombia(void) {

    int iRet = 0;
    int tamJournal = 0;

    tamJournal = tamArchivo(discoNetcom, (char*)JOURNAL_CNB_BANCOLOMBIA);

    iRet = cierreCnbBancolombia(CNB_CIERRE_PARCIAL);

    if (iRet > 0 && tamJournal > 0) {
        iRet = cierreCnbBancolombia(CNB_CIERRE_TOTAL);
    }

    return iRet;
}

int _cierreCnb_(void) {

    int iRet = 0;
    uChar cnb[2 + 1];
    uChar *buffer = NULL;

    memset(cnb, 0x00, sizeof(cnb));

    getParameter(HABILITACION_MODO_CNB, cnb);

/*    switch (atoi(cnb)) {
        case MODO_CNB_RBM:
            if (verificarObjetosTerminal(OBJECT_TYPE_APPLI, APP_TYPE_CNBRBM) >= 1) {
                iRet = netcomServices(APP_TYPE_CNBRBM, CIERRE_CNB_RBM, 1, &buffer);
            } else {
                screenMessage("MENSAJE", "TRANSACCION", "NO PERMITIDA", "", 2 * 1000);
                iRet = 0;
            }
            break;
        case MODO_CNB_CITIBANK:
            if (verificarObjetosTerminal(OBJECT_TYPE_DLL, APP_TYPE_CNB_CITIBANK) >= 1) {
                iRet = cierreCnbTerminal();
            } else {
                screenMessage("MENSAJE", "TRANSACCION", "NO PERMITIDA", "", 2 * 1000);
                iRet = 0;
            }
            break;
        case MODO_CNB_BANCOLOMBIA:
            if (verificarObjetosTerminal(OBJECT_TYPE_APPLI, APP_TYPE_CNB_BANCOLOMBIA) >= 1) {*/
//    iRet = netcomServices(APP_TYPE_CNB_BANCOLOMBIA, CIERRE_CNB_BANCOLOMBIA, 1, &buffer);

    iRet = cierreCnbTerminalBancolombia();

    /*          } else {
                  screenMessage("MENSAJE", "TRANSACCION", "NO PERMITIDA", "", 2 * 1000);
                  iRet = 0;
              }
              break;
          case MODO_CNB_CORRESPONSAL:
              if (verificarObjetosTerminal(OBJECT_TYPE_APPLI, APP_TYPE_CNB_CORRESPONSALES) > 0) {
                  iRet = netcomServices(APP_TYPE_CNB_CORRESPONSALES, CIERRE_CNB_CORRESPONSAL, 0, buffer);
              } else {
                  screenMessage("MENSAJE", "TRANSACCION", "NO PERMITIDA", "", 2 * 1000);
                  iRet = 0;
              }
              break;
          default:
              iRet = 0;
              break;
      }*/
    return iRet;

}
