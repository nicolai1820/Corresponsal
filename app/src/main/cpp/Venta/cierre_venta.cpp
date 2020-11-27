//
// Created by NETCOM on 04/03/2020.
//

#include <jni.h>
#include <string>
#include <sys/stat.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include "Utilidades.h"
#include "Mensajeria.h"
#include "Inicializacion.h"
#include "android/log.h"
#include "Archivos.h"
#include "configuracion.h"
#include "Tipo_datos.h"
#include "native-lib.h"
#include "comunicaciones.h"
#include "venta.h"
#include "bancolombia.h"
#include "android/log.h"

#define  LOG_TAG    "NETCOM_CIERRE"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

DatosGrupo datosGrupo[MAX_GRUPO + 1];
DatosBonos datosBonos;
DatosOtrosCierre datosPCR;
DatosOtrosCierre datosPSP;
DatosOtrosCierre datosGASOPASS;
DatosOtrosCierre datosRecargaCelular;
DatosOtrosCierre datosAvanceEfectivo;
TotalesCelucompra datosCeluCompra;
TotalesQrcode datosQrcode;
TotalesConsultaEstado datosConsultaEstado;
TotalesRecargaCelular totalesRecarga;
TotalesBigBancolombia datosBigBancolombia[30 + 1];
DatosOtrosCierre datosDineroElectronico;
DatosOtrosCierre datosDineroElectronicoRetiro;
DatosOtrosCierre recaudoCNBAval;
DatosOtrosCierre totalesPaypass;
DatosOtrosCierre totalesPayWave;
DatosOtrosCierre tarjetaPrivada;
DatosOtrosCierre tarjetaPrivadaEfectivo;
DatosOtrosCierre tarjetaPrivadaCombinado;
DatosOtrosCierre totalesTarjetaPrivada;
DatosOtrosCierre datosTarjetaMiCompra;
DatosLealtad datosLealtad;
TotalesBigBancolombia datosPColombia;
DatosLealtad datosLealtadBig[30 + 1];
DatosOtrosCierre datosPagoImpuestos;
DatosOtrosCierre datosFocoSodexo;
DatosLealtad datosPuntosColombia;

int totalTransaccionesDineroElectronico = 0;
int totalTransaccionesDineroElectronicoRetiro = 0;
int totalTransaccionesGASOPASS = 0;
int totalTransaccionesTarjetaMiCompra = 0;

DatosComercioMultiPos datosCierreComercios[MAX_COMERCIOS_MULTIPOS + 1];
int totalComerciosMultiPos = 0;
int tasaAero = 0;

int countFieldGrupo = 0;

int _cierreVenta_(void) {

    int maximoId = 0;
    int iRet = 0;
    int verificarFile = 0;
    long tam = 0;
    int resultado = 0;
    int cantidadBytes = 1;
    int cierre = 0;
    int tamJournal = 0;
    char cierreAuto[1 + 1];
    char dataRecibida[512 + 1];
    char datos[2 + 1];
    char modo[2 + 1];
    DatosCierreVenta datosCierreVenta;
    ResultISOPack resultadoIsoPackMessage;
    uChar estadoInicializacion[2 + 1];

    memset(datos, 0x00, sizeof(datos));
    memset(estadoInicializacion, 0x00, sizeof(estadoInicializacion));
    memset(modo, 0x00, sizeof(modo));

    getParameter(PROCESO_CIERRE, cierreAuto);

    if (verificarHabilitacionObjeto(CNB_REALIZAR_CIERRE_PARCIAL) == TRUE
        && verificarHabilitacionObjeto(HABILITACION_IMPRIMIR_CIERRE_AVAL) == TRUE &&
        atoi(cierreAuto) == 0) {
        LOGI("Realizar primero cierre parcial  ");
        resultado = -3;
    } else {

        getParameter(IMPRIMIR_CIERRE, datos);

        tam = tamArchivo(discoNetcom, (char *)JOURNAL);
        tamJournal = sizeof(DatosVenta);

        restarTamAjusteLealtadUltimoRecibo(&tam);

        maximoId = tam / tamJournal;
        LOGI("solicitando maximoID  %d", maximoId);
        if (maximoId > 0) {

            memset(dataRecibida, 0x00, sizeof(dataRecibida));
            memset(&datosCierreVenta, 0x00, sizeof(datosCierreVenta));
            memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
            datosCierreVenta = calcularCierreVenta(maximoId,
                                                   1); // no totaliza anuladas fuera de linea
            resultadoIsoPackMessage = empaquetarCierreVentas(datosCierreVenta, (char *)"920000");

            if (resultadoIsoPackMessage.responseCode > 0) {
                getParameter(IMPRIMIR_CIERRE, datos);

                cantidadBytes = realizarTransaccion(resultadoIsoPackMessage.isoPackMessage,
                                                    resultadoIsoPackMessage.totalBytes,
                                                    dataRecibida, TRANSACCION_CIERRE,
                                                    CANAL_PERMANENTE,
                                                    REVERSO_NO_GENERADO);
                //cantidadBytes = 1;

                if (cantidadBytes > 0) {
                    LOGI("desempaquetarCierreVentas  %d", cantidadBytes);
                    resultado = desempaquetarCierreVentas(dataRecibida, cantidadBytes);

                    getParameter(MODO_DATAFONO, modo);
                    //resultado = -1;
                    if (resultado == 1) {

                        verificarFile = verificarArchivo(discoNetcom,
                                                         DATOS_QRCODE);
                        iRet = (verificarFile == FS_OK) ? 1 : 0;

                        if (iRet > 0) {
                            borrarArchivo(discoNetcom, (char *)DATOS_QRCODE);
                            enviarStringToJava("TRANSACCION QR ELIMINADAS ", (char *)"mostrarInicializacion");
                        }

                        //imprimir totales.
                        LOGI("CIERRE EXITOSO  ");
                        enviarStringToJava("CIERRE EXITOSO  ", (char *)"mostrarInicializacion");

                        if (strncmp(datos, "1", 1) == 0) {

                            datosLealtad.cantidadAcumuladas = 0;
                            datosLealtad.cantidadLealtad = 0;
                            datosLealtad.totalAcumuladas = 0;
                            datosLealtad.bigBancolombia = 0;
                            datosLealtad.cantidadRedimidasBig = 0;
                            datosLealtad.totalRedimidasBig = 0;
                            datosLealtad.totalRedimidas = 0;
                            datosLealtad.cantidadRedimidas = 0;
                            datosPuntosColombia.cantidadAcumuladas = 0;
                            datosPuntosColombia.cantidadRedimidas = 0;
                            datosPuntosColombia.cantidadLealtad = 0;
                            datosPuntosColombia.totalAcumuladas = 0;
                            datosPuntosColombia.totalRedimidas = 0;
                            datosPuntosColombia.bigBancolombia = 0;
                            datosPuntosColombia.cantidadRedimidasBig = 0;
                            calcularCierreVenta(maximoId, 1);

                            if (atoi(modo) != MODO_PAGO) {
                                LOGI("imprimirReporteCierreVentas B");
                                imprimirReporteCierreVentas(FALSE);
                            }

                        }
                        //imprimirReporteAuditoria(maximoId);
                        borrarArchivo(discoNetcom, (char *)JOURNAL);
                        borrarArchivo(discoNetcom, (char *)JOURNAL_DCC);
                    } else if (resultado == -1) {
                        //cierre descuadrado
                        //imprimir totales.
                        //enviar transacciones
                        cierre = cierreDescuadradoVenta(maximoId);
                        enviarStringToJava("CIERRE DESCUADRADO FINALIZADO", (char *)"mostrarInicializacion");
                        if (cierre > 0) {
                            if (strncmp(datos, "1", 1) == 0) {

                                datosLealtad.cantidadAcumuladas = 0;
                                datosLealtad.cantidadLealtad = 0;
                                datosLealtad.totalAcumuladas = 0;
                                datosLealtad.bigBancolombia = 0;
                                datosLealtad.cantidadRedimidasBig = 0;
                                datosLealtad.totalRedimidasBig = 0;
                                datosPuntosColombia.cantidadAcumuladas = 0;
                                datosPuntosColombia.cantidadRedimidas = 0;
                                datosPuntosColombia.cantidadLealtad = 0;
                                datosPuntosColombia.totalAcumuladas = 0;
                                datosPuntosColombia.totalRedimidas = 0;
                                datosPuntosColombia.bigBancolombia = 0;
                                datosPuntosColombia.cantidadRedimidasBig = 0;
                                calcularCierreVenta(maximoId, 1);

                                if (atoi(modo) != MODO_PAGO) {
                                    LOGI("imprimirReporteCierreVentas C");
                                    imprimirReporteCierreVentas(FALSE);
                                    pprintf((char *) "final");
                                }

                            }
                            resultado = 1;
                            //imprimirReporteAuditoria(maximoId);
                            borrarArchivo(discoNetcom, (char *)JOURNAL);
                            borrarArchivo(discoNetcom, (char *)JOURNAL_DCC);
                        }

                    } else {
                        resultado = -2;
                    }
                }
            }

        } else {

            getParameter(TERMINAL_INICIALIZADA, estadoInicializacion);
            if (strcmp(estadoInicializacion, "1") == 0) {
                enviarStringToJava("SIN TRANSACCIONES CIERRE NO PERMITIDO", (char *)"mostrarInicializacion");
                //screenMessage("CIERRE", "SIN TRANSACCIONES", "CIERRE NO", "PERMITIDO", 2 * 1000);
            }
            resultado = 1;
        }
    }

    return resultado;

}

DatosCierreVenta calcularCierreVenta(int totalTransacciones, int incluyeAnuladasOffline) {

    int idx = 0;
    int indiceGrupo = 0;
    int indiceSubgrupo = 0;
    char valor[10 + 1];
    char valorVenta[10 + 1];
    char valorIva[10 + 1];
    char valorInc[10 + 1];
    char valorPropina[10 + 1];
    char valorBase[10 + 1];
    char convenioBig[16 + 1];
    char fiidBig[2 + 1];
    DatosVenta datosVenta;
    DatosCierreVenta datosCierreVenta;
    char auxCodigo[2 + 1];
    TablaSeisInicializacion tablaSeis;
    int tipoTransaccion = 0;
    int estadoTransaccion = 0;
    int sumarAnuladas = 0;

    memset(&datosCierreVenta, 0x00, sizeof(datosCierreVenta));
    memset(convenioBig, 0x00, sizeof(convenioBig));
    memset(fiidBig, 0x00, sizeof(fiidBig));

    limpiarDatos();
    for (idx = 0; idx < totalTransacciones; idx++) {
        memset(&datosVenta, 0x00, sizeof(datosVenta));
        sumarAnuladas = 1;

        datosVenta = buscarReciboByID(idx);

        tipoTransaccion = atoi(datosVenta.tipoTransaccion);
        estadoTransaccion = atoi(datosVenta.estadoTransaccion);

        if (strlen(datosVenta.numeroRecibo) <= 0 || strlen(datosVenta.codigoAprobacion) <= 0) {
            tipoTransaccion = -1;
        }

        if (tipoTransaccion == TRANSACCION_VENTA) { //venta

            memset(valor, 0x00, sizeof(valor));
            memset(valorVenta, 0x00, sizeof(valorVenta));
            memset(valorIva, 0x00, sizeof(valorIva));
            memset(valorInc, 0x00, sizeof(valorInc));
            memset(valorPropina, 0x00, sizeof(valorPropina));
            memset(valorBase, 0x00, sizeof(valorBase));

            memcpy(valorVenta, datosVenta.totalVenta,
                   strlen(datosVenta.totalVenta) - 2); //-2 por la parte decimal

            if (strlen(datosVenta.iva) > 2) {
                memcpy(valorIva, datosVenta.iva,
                       strlen(datosVenta.iva) - 2); //-2 por la parte decimal
            }

            if (strlen(datosVenta.inc) > 2) {
                memcpy(valorInc, datosVenta.inc,
                       strlen(datosVenta.inc) - 2); //-2 por la parte decimal
            }

            if (strlen(datosVenta.propina) > 2) {
                memcpy(valorPropina, datosVenta.propina,
                       strlen(datosVenta.propina) - 2); //-2 por la parte decimal
            }

            if (strlen(datosVenta.baseDevolucion) > 2) {
                memcpy(valorBase, datosVenta.baseDevolucion,
                       strlen(datosVenta.baseDevolucion) - 2); //-2 por la parte decimal
            }

            //guardar en grupo y subgrupo correspondiente
            indiceGrupo = getIdGrupo(datosVenta.codigoGrupo);

            memset(valor, 0x00, sizeof(valor));
            memcpy(valor, datosVenta.compraNeta,
                   strlen(datosVenta.compraNeta) - 2); //-2 por la parte decimal

            if (strlen(datosGrupo[indiceGrupo].nombreGrupo) == 0) {
                if (strncmp(datosVenta.codigoGrupo, "06", 2) == 0) {
                    memcpy(tablaSeis.texto, "TOTAL VISA DEBITO", 17);
                    memcpy(datosGrupo[indiceGrupo].nombreGrupo, tablaSeis.texto, 20);
                    memcpy(datosVenta.cardName, "VISA DEBITO", TAM_CARD_NAME);
                } else {
                    memset(auxCodigo, 0x00, sizeof(auxCodigo));
                    memset(&tablaSeis, 0x00, sizeof(tablaSeis));
                    _convertASCIIToBCD_(auxCodigo, datosVenta.codigoGrupo, 2);
                    tablaSeis = desempaquetarTablaSeis(auxCodigo[0]);
                    memcpy(datosGrupo[indiceGrupo].nombreGrupo, tablaSeis.texto, 20);

                    if (strncmp(datosVenta.cardName, "MASTERDEBIT", 11) == 0
                        || strncmp(datosVenta.cardName, "MASTER DEBIT", 12) == 0) {
                        memcpy(datosVenta.cardName, "MASTERCARD", TAM_CARD_NAME);
                    }
                }
            }

            memcpy(datosGrupo[indiceGrupo].codigoGrupo, datosVenta.codigoGrupo, TAM_GRUPO);

            indiceSubgrupo = getIdSubgrupo(indiceGrupo, datosVenta.codigoSubgrupo);
            memcpy(datosGrupo[indiceGrupo].datosSubgrupo[indiceSubgrupo].codigoSubgrupo,
                   datosVenta.codigoSubgrupo,
                   TAM_SUBGRUPO);

            memcpy(datosGrupo[indiceGrupo].datosSubgrupo[indiceSubgrupo].nombreSubgrupo,
                   datosVenta.cardName,
                   TAM_CARD_NAME);

            if (estadoTransaccion == TRANSACCION_ACTIVA) {
                datosGrupo[indiceGrupo].datosSubgrupo[indiceSubgrupo].cantidadVenta++;
                datosGrupo[indiceGrupo].datosSubgrupo[indiceSubgrupo].totalVenta += atol(valor);
                datosGrupo[indiceGrupo].datosSubgrupo[indiceSubgrupo].totalIva += atol(valorIva);
                datosGrupo[indiceGrupo].datosSubgrupo[indiceSubgrupo].totalInc += atol(valorInc);
                datosGrupo[indiceGrupo].datosSubgrupo[indiceSubgrupo].totalPropina += atol(
                        valorPropina);
                datosGrupo[indiceGrupo].datosSubgrupo[indiceSubgrupo].totalBase += atol(valorBase);

                if (strncmp(datosVenta.isQPS, "1", 1) == 0) {
                    datosGrupo[indiceGrupo].datosSubgrupo[indiceSubgrupo].cantidadQPS++;
                    datosGrupo[indiceGrupo].datosSubgrupo[indiceSubgrupo].totalQPS += atol(
                            valorVenta);
                }

                if (datosVenta.isQPS[0] == 'P') {
                    totalesPaypass.cantidadVentaDebito++;
                    totalesPaypass.totalVentaDebito += atol(valorVenta);
                }

                if (datosVenta.isQPS[0] == 'V') {
                    totalesPayWave.cantidadVentaDebito++;
                    totalesPayWave.totalVentaDebito += atol(valorVenta);
                }

                if (strncmp(datosVenta.tipoCuenta, "30", 2) == 0) {
                    datosCierreVenta.totalVentaCredito += atol(valorVenta);
                    datosCierreVenta.cantidadVentaCredito++;
                } else {
                    datosCierreVenta.totalVentaDebito += atol(valorVenta);
                    datosCierreVenta.cantidadVentaDebito++;
                }

            } else {
                if (strlen(datosVenta.rrn) == 0 && incluyeAnuladasOffline == 0) { //fuera de linea
                    sumarAnuladas = 0;
                }

                if (sumarAnuladas == 1) {
                    datosGrupo[indiceGrupo].datosSubgrupo[indiceSubgrupo].cantidadAnulacion++;
                    datosGrupo[indiceGrupo].datosSubgrupo[indiceSubgrupo].totalAnulacion += atol(
                            valorVenta);

                    if (strncmp(datosVenta.tipoCuenta, "30", 2) == 0) {
                        datosCierreVenta.totalAnuladasCredito += atol(valorVenta);
                        datosCierreVenta.cantidadAnuladasCredito++;
                    } else {
                        datosCierreVenta.totalAnuladasDebito += atol(valorVenta);
                        datosCierreVenta.cantidadAnuladasDebito++;

                    }

                }

            }

        } else if (IS_BONO(atoi(datosVenta.tipoTransaccion)) == 1) { // se realiza calculo de bono

            memset(valorVenta, 0x00, sizeof(valorVenta));
            memcpy(valorVenta, datosVenta.totalVenta,
                   strlen(datosVenta.totalVenta) - 2); //-2 por la parte decimal

            if (estadoTransaccion == TRANSACCION_ACTIVA) {

                if (tipoTransaccion == TRANSACCION_HABILITAR_BONO) {
                    datosBonos.cantidadHabilitacion++;
                    datosBonos.totalHabilitacion += atol(valorVenta);
                } else if (tipoTransaccion == TRANSACCION_REDENCION_BONO) {
                    datosBonos.cantidadRedencion++;
                    datosBonos.totalRedencion += atol(valorVenta);
                    datosCierreVenta.totalVentaDebito += atol(valorVenta);
                    datosCierreVenta.cantidadVentaDebito++;
                } else if (tipoTransaccion == TRANSACCION_RECARGAR_BONO) {
                    datosBonos.cantidadRecarga++;
                    datosBonos.totalRecarga += atol(valorVenta);
                }

            } else {
                datosBonos.cantidadAnulacion++;
                datosBonos.totalAnulacion += atol(valorVenta);
            }

        } else if (tipoTransaccion == TRANSACCION_PCR) {

            memset(valorVenta, 0x00, sizeof(valorVenta));
            memcpy(valorVenta, datosVenta.totalVenta,
                   strlen(datosVenta.totalVenta) - 2); //-2 por la parte decimal

            if (estadoTransaccion == TRANSACCION_ACTIVA) {

                if (strncmp(datosVenta.tipoCuenta, "30", 2) == 0) {
                    datosPCR.totalVentaCredito += atol(valorVenta);
                    datosPCR.cantidadVentaCredito++;
                } else {
                    datosPCR.totalVentaDebito += atol(valorVenta);
                    datosPCR.cantidadVentaDebito++;
                }
            }

        } else if (tipoTransaccion == TRANSACCION_PSP) {

            memset(valorVenta, 0x00, sizeof(valorVenta));
            memcpy(valorVenta, datosVenta.totalVenta,
                   strlen(datosVenta.totalVenta) - 2); //-2 por la parte decimal

            if (estadoTransaccion == TRANSACCION_ACTIVA) {
                datosPSP.totalVentaCredito += atol(valorVenta);
                datosPSP.cantidadVentaCredito++;
            }

        } else if (tipoTransaccion == TRANSACCION_RECARGA) {

            memset(valorVenta, 0x00, sizeof(valorVenta));
            memcpy(valorVenta, datosVenta.totalVenta,
                   strlen(datosVenta.totalVenta) - 2); //-2 por la parte decimal

            if (estadoTransaccion == TRANSACCION_ACTIVA) {

                if ((strncmp(datosVenta.tipoCuenta, "30", 2) == 0)
                    || (strncmp(datosVenta.estadoTarifa, "02", 2) == 0)) {
                    datosRecargaCelular.totalVentaCredito += atol(valorVenta);
                    datosRecargaCelular.cantidadVentaCredito++;
                } else {
                    datosRecargaCelular.totalVentaDebito += atol(valorVenta);
                    datosRecargaCelular.cantidadVentaDebito++;
                }

                totalizarRecargaCelular(datosVenta.codigoSubgrupo, datosVenta.codigoGrupo,
                                        atol(valorVenta));

            }

        } else if (IS_MULTIPOS(atoi(datosVenta.tipoTransaccion)) == 1) {
            llenarDatosComercio(datosVenta);
        } else if ((tipoTransaccion == TRANSACCION_PAGO_ELECTRONICO)
                   || tipoTransaccion == TRANSACCION_PAGO_ELECTRONICO_ESPECIAL) {

            indiceGrupo = getIdGrupo(datosVenta.codigoGrupo);
            if (strlen(datosGrupo[indiceGrupo].nombreGrupo) == 0) {
                memset(auxCodigo, 0x00, sizeof(auxCodigo));
                memset(&tablaSeis, 0x00, sizeof(tablaSeis));
                _convertASCIIToBCD_(auxCodigo, datosVenta.codigoGrupo, 2);
                tablaSeis = desempaquetarTablaSeis(auxCodigo[0]);
                memcpy(datosGrupo[indiceGrupo].nombreGrupo, tablaSeis.texto, 20);
            }
            memcpy(datosGrupo[indiceGrupo].codigoGrupo, datosVenta.codigoGrupo, TAM_GRUPO);
            totalTransaccionesDineroElectronico++;

            if (estadoTransaccion == TRANSACCION_ACTIVA) {
                memset(valorVenta, 0x00, sizeof(valorVenta));
                memcpy(valorVenta, datosVenta.totalVenta, strlen(datosVenta.totalVenta) - 2);
                datosDineroElectronico.totalVentaDebito += atol(valorVenta);
                datosDineroElectronico.cantidadVentaDebito++;
            }

        } else if (tipoTransaccion == TRANSACCION_RETIRO_OTP) {

            indiceGrupo = getIdGrupo(datosVenta.codigoGrupo);
            if (strlen(datosGrupo[indiceGrupo].nombreGrupo) == 0) {
                memset(auxCodigo, 0x00, sizeof(auxCodigo));
                memset(&tablaSeis, 0x00, sizeof(tablaSeis));
                _convertASCIIToBCD_(auxCodigo, datosVenta.codigoGrupo, 2);
                tablaSeis = desempaquetarTablaSeis(auxCodigo[0]);
                memcpy(datosGrupo[indiceGrupo].nombreGrupo, tablaSeis.texto, 20);
            }
            memcpy(datosGrupo[indiceGrupo].codigoGrupo, datosVenta.codigoGrupo, TAM_GRUPO);
            totalTransaccionesDineroElectronicoRetiro++;

            if (estadoTransaccion == TRANSACCION_ACTIVA) {
                memset(valorVenta, 0x00, sizeof(valorVenta));
                memcpy(valorVenta, datosVenta.totalVenta, strlen(datosVenta.totalVenta) - 2);
                datosDineroElectronicoRetiro.totalVentaDebito += atol(valorVenta);
                datosDineroElectronicoRetiro.cantidadVentaDebito++;
            }

        } else if (tipoTransaccion == TRANSACCION_CONSULTA_QR) {

            indiceGrupo = getIdGrupo(datosVenta.codigoGrupo);

            memset(valorVenta, 0x00, sizeof(valorVenta));
            memset(valorIva, 0x00, sizeof(valorIva));
            memset(valorInc, 0x00, sizeof(valorInc));
            memset(valorBase, 0x00, sizeof(valorBase));
            memset(valor, 0x00, sizeof(valor));
            memset(valorPropina, 0x00, sizeof(valorPropina));

            memcpy(valorVenta, datosVenta.totalVenta, strlen(datosVenta.totalVenta) - 2);
            datosQrcode.cantidadTransacciones += 1;

            if (strlen(datosGrupo[indiceGrupo].nombreGrupo) == 0) {
                memcpy(datosGrupo[indiceGrupo].nombreGrupo, "TOTAL QR      ", 20);
            }
            memcpy(datosGrupo[indiceGrupo].codigoGrupo, datosVenta.codigoGrupo, TAM_GRUPO);

            if (estadoTransaccion == TRANSACCION_ACTIVA) {

                datosQrcode.totalDebitos += atol(valorVenta);
                datosQrcode.cantidadDebitos++;

                if (strlen(datosVenta.iva) >= 1) {
                    memcpy(valorIva, datosVenta.iva, strlen(datosVenta.iva));
                }
                LOGI("inc reportes %s", datosVenta.inc);
                if (strlen(datosVenta.inc) >= 1) {
                    memcpy(valorInc, datosVenta.inc, strlen(datosVenta.inc));
                }

                if (strlen(datosVenta.baseDevolucion) >= 1) {
                    memcpy(valorBase, datosVenta.baseDevolucion, strlen(datosVenta.baseDevolucion));
                }

                if (strlen(datosVenta.propina) >= 1) {
                    memcpy(valorPropina, datosVenta.propina, strlen(datosVenta.propina));
                }

                memcpy(valor, datosVenta.compraNeta, strlen(datosVenta.compraNeta));

                datosQrcode.totalCompraNeta += atol(valor);
                datosQrcode.totalIva += atol(valorIva);
                datosQrcode.totalInc += atol(valorInc);
                datosQrcode.totalBase += atol(valorBase);
                datosQrcode.totalPropina += atol(valorPropina);

            } else {
                datosCierreVenta.totalAnuladasDebito += atol(valorVenta);
                datosCierreVenta.cantidadAnuladasDebito++;
                datosQrcode.cantidadAnuladas += 1;
                datosQrcode.totalAnuladas += atol(valorVenta);
            }
        } else if (tipoTransaccion == TRANSACCION_CODIGO_ESTATICO) {

            memset(valorVenta, 0x00, sizeof(valorVenta));
            memset(valorIva, 0x00, sizeof(valorIva));
            memset(valorInc, 0x00, sizeof(valorInc));
            memset(valorBase, 0x00, sizeof(valorBase));
            memset(valor, 0x00, sizeof(valor));
            memset(valorPropina, 0x00, sizeof(valorPropina));

            memcpy(valorVenta, datosVenta.totalVenta, strlen(datosVenta.totalVenta) - 2);
            datosConsultaEstado.cantidadTransacciones += 1;

            if (estadoTransaccion == TRANSACCION_ACTIVA) {

                datosConsultaEstado.totalDebitos += atol(valorVenta);
                datosConsultaEstado.cantidadDebitos++;

                if (strlen(datosVenta.iva) > 2) {
                    memcpy(valorIva, datosVenta.iva, strlen(datosVenta.iva) - 2);
                }

                if (strlen(datosVenta.inc) > 2) {
                    memcpy(valorInc, datosVenta.inc, strlen(datosVenta.inc) - 2);
                }

                if (strlen(datosVenta.baseDevolucion) > 2) {
                    memcpy(valorBase, datosVenta.baseDevolucion,
                           strlen(datosVenta.baseDevolucion) - 2);
                }

                if (strlen(datosVenta.propina) > 2) {
                    memcpy(valorPropina, datosVenta.propina, strlen(datosVenta.propina) - 2);
                }

                memcpy(valor, datosVenta.compraNeta, strlen(datosVenta.compraNeta) - 2);

                datosConsultaEstado.totalCompraNeta += atol(valor);
                datosConsultaEstado.totalIva += atol(valorIva);
                datosConsultaEstado.totalInc += atol(valorInc);
                datosConsultaEstado.totalBase += atol(valorBase);
                datosConsultaEstado.totalPropina += atol(valorPropina);

            }
            //			else{
            //				datosCierreVenta.totalAnuladasDebito += atol(valorVenta);
            //				datosCierreVenta.cantidadAnuladasDebito ++;
            //				datosConsultaEstado.cantidadAnuladas += 1;
            //				datosConsultaEstado.totalAnuladas += atol(valorVenta);
            //			}
        } else if (atoi(datosVenta.tipoTransaccion) == TRANSACCION_GASO_PASS) {

            totalTransaccionesGASOPASS++;
            memset(valorVenta, 0x00, sizeof(valorVenta));
            memcpy(valorVenta, datosVenta.totalVenta, strlen(datosVenta.totalVenta) - 2);
            if (estadoTransaccion == TRANSACCION_ACTIVA) {

                if (strncmp(datosVenta.tipoCuenta, "30", 2) == 0) {
                    datosGASOPASS.totalVentaCredito += atol(valorVenta);
                    datosGASOPASS.cantidadVentaCredito++;
                } else {
                    datosGASOPASS.totalVentaDebito += atol(valorVenta);
                    datosGASOPASS.cantidadVentaDebito++;
                }
            }
        } else if (atoi(datosVenta.tipoTransaccion) == TRANSACCION_TARJETA_MI_COMPRA) {

            totalTransaccionesTarjetaMiCompra++;
            memset(valorVenta, 0x00, sizeof(valorVenta));
            memcpy(valorVenta, datosVenta.totalVenta, strlen(datosVenta.totalVenta) - 2);
            if (estadoTransaccion == TRANSACCION_ACTIVA) {

                if (strncmp(datosVenta.tipoCuenta, "30", 2) == 0) {
                    datosTarjetaMiCompra.totalVentaCredito += atol(valorVenta);
                    datosTarjetaMiCompra.cantidadVentaCredito++;
                } else {
                    datosTarjetaMiCompra.totalVentaDebito += atol(valorVenta);
                    datosTarjetaMiCompra.cantidadVentaDebito++;
                }
            }
        } else if (tipoTransaccion == TRANSACCION_EFECTIVO) {

            memset(valorVenta, 0x00, sizeof(valorVenta));
            memcpy(valorVenta, datosVenta.totalVenta, strlen(datosVenta.totalVenta) - 2);
            if (estadoTransaccion == TRANSACCION_ACTIVA) {
                datosAvanceEfectivo.totalVentaCredito += atol(valorVenta);
                datosAvanceEfectivo.cantidadVentaCredito++;
            } else {

                //se guarda en estas variables pero son de las anuladas.
                datosAvanceEfectivo.totalVentaDebito += atol(valorVenta);
                datosAvanceEfectivo.cantidadVentaDebito++;
            }

        } else if (tipoTransaccion == TRANSACCION_PAGO_IMPUESTOS) {

            memset(valorVenta, 0x00, sizeof(valorVenta));
            memcpy(valorVenta, datosVenta.totalVenta, strlen(datosVenta.totalVenta) - 2);
            if (estadoTransaccion == TRANSACCION_ACTIVA) {
                datosPagoImpuestos.totalVentaCredito += atol(valorVenta);
                datosPagoImpuestos.cantidadVentaCredito++;
            } else {

                //se guarda en estas variables pero son de las anuladas.
                datosPagoImpuestos.totalVentaDebito += atol(valorVenta);
                datosPagoImpuestos.cantidadVentaDebito++;
            }

        } else if (tipoTransaccion == TRANSACCION_FOCO_SODEXO) {

            memset(valorVenta, 0x00, sizeof(valorVenta));
            memcpy(valorVenta, datosVenta.totalVenta, strlen(datosVenta.totalVenta) - 2);
            if (estadoTransaccion == TRANSACCION_ACTIVA) {
                datosFocoSodexo.totalVentaCredito += atol(valorVenta);
                datosFocoSodexo.cantidadVentaCredito++;
            } else {

                //se guarda en estas variables pero son de las anuladas.
                datosFocoSodexo.totalVentaDebito += atol(valorVenta);
                datosFocoSodexo.cantidadVentaDebito++;
            }


        } else if (tipoTransaccion == TRANSACCION_CELUCOMPRA) {
            memset(valorVenta, 0x00, sizeof(valorVenta));
            memset(valorIva, 0x00, sizeof(valorIva));
            memset(valorInc, 0x00, sizeof(valorInc));
            memset(valorBase, 0x00, sizeof(valorBase));
            memset(valor, 0x00, sizeof(valor));
            memset(valorPropina, 0x00, sizeof(valorPropina));

            memcpy(valorVenta, datosVenta.totalVenta, strlen(datosVenta.totalVenta) - 2);
            datosCeluCompra.cantidadTransacciones += 1;
            if (estadoTransaccion == TRANSACCION_ACTIVA) {

                datosCeluCompra.totalDebitos += atol(valorVenta);
                datosCeluCompra.cantidadDebitos++;

                if (strlen(datosVenta.iva) > 2) {
                    memcpy(valorIva, datosVenta.iva, strlen(datosVenta.iva) - 2);
                }

                if (strlen(datosVenta.inc) > 2) {
                    memcpy(valorInc, datosVenta.inc, strlen(datosVenta.inc) - 2);
                }

                if (strlen(datosVenta.baseDevolucion) > 2) {
                    memcpy(valorBase, datosVenta.baseDevolucion,
                           strlen(datosVenta.baseDevolucion) - 2);
                }

                if (strlen(datosVenta.propina) > 2) {
                    memcpy(valorPropina, datosVenta.propina, strlen(datosVenta.propina) - 2);
                }

                memcpy(valor, datosVenta.compraNeta, strlen(datosVenta.compraNeta) - 2);

                datosCeluCompra.totalCompraNeta += atol(valor);
                datosCeluCompra.totalIva += atol(valorIva);
                datosCeluCompra.totalInc += atol(valorInc);
                datosCeluCompra.totalBase += atol(valorBase);
                datosCeluCompra.totalPropina += atol(valorPropina);

            } else {
                datosCierreVenta.totalAnuladasDebito += atol(valorVenta);
                datosCierreVenta.cantidadAnuladasDebito++;
                datosCeluCompra.cantidadAnuladas += 1;
                datosCeluCompra.totalAnuladas += atol(valorVenta);
            }
        } else if ((tipoTransaccion == TRANSACCION_PAGO_FACTURAS_AVAL)) {

            if (estadoTransaccion == TRANSACCION_ACTIVA) {
                memset(valorVenta, 0x00, sizeof(valorVenta));
                memcpy(valorVenta, datosVenta.totalVenta, strlen(datosVenta.totalVenta) - 2);
                recaudoCNBAval.totalVentaDebito += atol(valorVenta);
                recaudoCNBAval.cantidadVentaDebito++;
            }

        } else if ((tipoTransaccion == TRANSACCION_RETIRO_CNB_AVAL)) {

            if (estadoTransaccion == TRANSACCION_ACTIVA) {

                memset(valorVenta, 0x00, sizeof(valorVenta));
                memcpy(valorVenta, datosVenta.totalVenta, strlen(datosVenta.totalVenta) - 2);
                recaudoCNBAval.totalVentaCredito += atol(valorVenta);
                recaudoCNBAval.cantidadVentaCredito++;

            }

        } else if ((tipoTransaccion == TRANSACCION_DEPOSITO_EFECTIVO_CNB_AVAL)) {

            if (estadoTransaccion == TRANSACCION_ACTIVA) {

                memset(valorVenta, 0x00, sizeof(valorVenta));
                memcpy(valorVenta, datosVenta.totalVenta, strlen(datosVenta.totalVenta) - 2);
                recaudoCNBAval.totalVentaDebito += atol(valorVenta);
                recaudoCNBAval.cantidadVentaDebito++;

            }
        } else if ((tipoTransaccion == TRANSACCION_TRANSFERENCIA_AVAL)) {

            if (estadoTransaccion == TRANSACCION_ACTIVA) {

                memset(valorVenta, 0x00, sizeof(valorVenta));
                memcpy(valorVenta, datosVenta.totalVenta, strlen(datosVenta.totalVenta) - 2);
                recaudoCNBAval.totalVentaDebito += atol(valorVenta);
                recaudoCNBAval.cantidadVentaDebito++;

            }

        } else if ((tipoTransaccion == TRANSACCION_P_PRODUCTO_TARJETA_AVAL)) {

            if (estadoTransaccion == TRANSACCION_ACTIVA) {

                memset(valorVenta, 0x00, sizeof(valorVenta));
                memcpy(valorVenta, datosVenta.totalVenta, strlen(datosVenta.totalVenta) - 2);
                recaudoCNBAval.totalVentaDebito += atol(valorVenta);
                recaudoCNBAval.cantidadVentaDebito++;

            }

        } else if ((tipoTransaccion == TRANSACCION_P_PRODUCTO_NO_TARJETA_AVAL)) {

            if (estadoTransaccion == TRANSACCION_ACTIVA) {

                memset(valorVenta, 0x00, sizeof(valorVenta));
                memcpy(valorVenta, datosVenta.totalVenta, strlen(datosVenta.totalVenta) - 2);
                recaudoCNBAval.totalVentaDebito += atol(valorVenta);
                recaudoCNBAval.cantidadVentaDebito++;

            }
        } else if (tipoTransaccion == TRANSACCION_TARJETA_PRIVADA) {

            memset(valorVenta, 0x00, sizeof(valorVenta));
            memcpy(valorVenta, datosVenta.totalVenta, strlen(datosVenta.totalVenta) - 2);

            totalesTarjetaPrivada.totalVentaCredito += atol(valorVenta);
            totalesTarjetaPrivada.cantidadVentaCredito++;

            if (strcmp(datosVenta.iva, "31") == 0 ||
                strcmp(datosVenta.iva, "53") == 0) { // 1/2 de pago:tarjeta
                tarjetaPrivada.totalVentaCredito += atol(valorVenta);
                tarjetaPrivada.cantidadVentaCredito++;
            } else if (strcmp(datosVenta.iva, "11") == 0) { // 1/2 : documento
                tarjetaPrivadaEfectivo.totalVentaCredito += atol(valorVenta);
                tarjetaPrivadaEfectivo.cantidadVentaCredito++;
            } else if (strcmp(datosVenta.iva, "77") == 0) { // 1/2 :combinado
                tarjetaPrivadaCombinado.totalVentaCredito += atol(valorVenta);
                tarjetaPrivadaCombinado.cantidadVentaCredito++;
            }
        } else if (tipoTransaccion == TRANSACCION_ACUMULAR_LEALTAD) {
            if (estadoTransaccion == TRANSACCION_ACTIVA) {
                memset(valorVenta, 0x00, sizeof(valorVenta));
                memcpy(valorVenta, datosVenta.totalVenta, strlen(datosVenta.totalVenta) - 2);

                //se toman estos campos de la estructura solo para almacenar allí las variables, no suman
                datosLealtad.cantidadLealtad++;
                datosLealtad.cantidadAcumuladas++;
                datosLealtad.totalAcumuladas += atol(valorVenta);
            }
        } else if (tipoTransaccion == TRANSACCION_REDENCION_LIFEMILES) {

            if (estadoTransaccion == TRANSACCION_ACTIVA) {
                memset(valorVenta, 0x00, sizeof(valorVenta));
                memcpy(valorVenta, datosVenta.totalVenta, strlen(datosVenta.totalVenta) - 2);

                //se toman estos campos de la estructura solo para almacenar allí las variables, no suman
                datosLealtad.cantidadLealtad++;
                datosLealtad.cantidadRedimidas++;
                datosLealtad.totalRedimidas += atol(valorVenta);
            }
        } else if (tipoTransaccion == TRANSACCION_AJUSTAR_LEALTAD) {

            memset(valorVenta, 0x00, sizeof(valorVenta));
            memcpy(valorVenta, datosVenta.compraNeta, strlen(datosVenta.compraNeta) - 2);
            datosLealtad.totalAcumuladas += atol(valorVenta);
            memset(valorVenta, 0x00, sizeof(valorVenta));
            memcpy(valorVenta, datosVenta.totalVenta, strlen(datosVenta.totalVenta) - 2);
            datosLealtad.totalAcumuladas -= atol(valorVenta);

        } else if (tipoTransaccion == TRANSACCION_BIG_BANCOLOMBIA) {

            char fiid[2 + 1];
            int idx = 0;
            long lengthTablaBig = 0;
            int itemsBig = 0;

            memset(fiid, 0x00, sizeof(fiid));
            memset(valorVenta, 0x00, sizeof(valorVenta));

            lengthTablaBig = tamArchivo(discoNetcom, TX_BIG_BANCOLOMBIA);

            itemsBig = lengthTablaBig / 34;


            memcpy(valorVenta, datosVenta.totalVenta, strlen(datosVenta.totalVenta) - 2);

            for (idx = 0; idx < itemsBig; idx++) {
                invocarMenuBig(convenioBig, fiidBig, idx);

                if (atoi(fiidBig) == atoi(datosVenta.facturaCaja)) {

                    if (estadoTransaccion == TRANSACCION_ACTIVA) {
                        datosLealtad.bigBancolombia = 1;
                        datosLealtadBig[idx].bigBancolombia = 1;
                        datosLealtadBig[idx].cantidadRedimidasBig++;
                        datosLealtadBig[idx].totalRedimidasBig += atol(valorVenta);
                    } else {
                        datosLealtad.bigBancolombia = 1;
                        datosBigBancolombia[idx].cantidadAnuladas += 1;
                        datosBigBancolombia[idx].totalAnuladas += atol(valorVenta);
                    }
                }
            }
        } else if (tipoTransaccion == TRANSACCION_PUNTOS_COLOMBIA) {
            if (estadoTransaccion == TRANSACCION_ACTIVA) {
                memset(valorVenta, 0x00, sizeof(valorVenta));
                memcpy(valorVenta, datosVenta.totalVenta, strlen(datosVenta.totalVenta));
                if (strcmp(datosVenta.estadoTarifa, "A0") == 0) {
                    datosPuntosColombia.cantidadLealtad++;
                    datosPuntosColombia.cantidadAcumuladas++;
                    datosPuntosColombia.totalAcumuladas += atol(valorVenta);
                } else if (strcmp(datosVenta.estadoTarifa, "R0") == 0) {
                    datosPuntosColombia.cantidadLealtad++;
                    datosPuntosColombia.cantidadRedimidas++;
                    datosPuntosColombia.totalRedimidas += atol(valorVenta);
                }
            } else {
                datosPColombia.cantidadAnuladas += 1;
                datosPColombia.totalAnuladas += atol(valorVenta);
            }
        }
    }

    return datosCierreVenta;

}

ResultISOPack empaquetarCierreVentas(DatosCierreVenta datosCierreVenta, char *codigoProceso) {

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

    //TablaCuatroInicializacion tablaCuatro;

    memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
    memset(&isoHeader8583, 0x00, sizeof(isoHeader8583));
    memset(systemTrace, 0x00, sizeof(systemTrace));
    memset(terminalID, 0x00, sizeof(terminalID));
    //memset(nii,0x00,sizeof(nii));
    memset(batch, 0x00, sizeof(batch));
    memset(field63, 0x00, sizeof(field63));

    //Credito
    cantidadVentaCredito += datosCierreVenta.cantidadVentaCredito;
    cantidadVentaCredito += datosPCR.cantidadVentaCredito;
    cantidadVentaCredito += datosRecargaCelular.cantidadVentaCredito;
    cantidadVentaCredito += datosPSP.cantidadVentaCredito;
    cantidadVentaCredito += datosGASOPASS.cantidadVentaCredito;
    cantidadVentaCredito += datosAvanceEfectivo.cantidadVentaCredito;
    cantidadVentaCredito += totalesTarjetaPrivada.cantidadVentaCredito;
    cantidadVentaCredito += datosTarjetaMiCompra.cantidadVentaCredito;
    cantidadVentaCredito += datosFocoSodexo.cantidadVentaCredito;

    totalVentaCredito += datosCierreVenta.totalVentaCredito;
    totalVentaCredito += datosPCR.totalVentaCredito;
    totalVentaCredito += datosRecargaCelular.totalVentaCredito;
    totalVentaCredito += datosPSP.totalVentaCredito;
    totalVentaCredito += datosGASOPASS.totalVentaCredito;
    totalVentaCredito += datosAvanceEfectivo.totalVentaCredito;
    totalVentaCredito += totalesTarjetaPrivada.totalVentaCredito; //!
    totalVentaCredito += datosTarjetaMiCompra.totalVentaCredito;
    totalVentaCredito += datosFocoSodexo.totalVentaCredito;
    ////////////////////////////////////////////////////////////////////////////

    //Debito
    cantidadVentaDebito += datosCierreVenta.cantidadVentaDebito;
    cantidadVentaDebito += datosPCR.cantidadVentaDebito;
    cantidadVentaDebito += datosRecargaCelular.cantidadVentaDebito;
    cantidadVentaDebito += datosPSP.cantidadVentaDebito;
    cantidadVentaDebito += datosDineroElectronico.cantidadVentaDebito;
    cantidadVentaDebito += datosGASOPASS.cantidadVentaDebito;
    cantidadVentaDebito += datosTarjetaMiCompra.cantidadVentaDebito;
    cantidadVentaDebito += datosFocoSodexo.cantidadVentaDebito;

    totalVentaDebito += datosCierreVenta.totalVentaDebito;
    totalVentaDebito += datosPCR.totalVentaDebito;
    totalVentaDebito += datosRecargaCelular.totalVentaDebito;
    totalVentaDebito += datosPSP.totalVentaDebito;
    totalVentaDebito += datosDineroElectronico.totalVentaDebito;
    totalVentaDebito += datosGASOPASS.totalVentaDebito;
    totalVentaDebito += datosTarjetaMiCompra.totalVentaDebito;
    totalVentaDebito += datosFocoSodexo.totalVentaDebito;
    //totalVentaDebito	 += recaudoCNBAval.totalVentaDebito;

    ///////////////////////////////////////////////////////////////////////////

    sprintf(field63 + indice, "%03d", cantidadVentaCredito);
    indice += 3;
    sprintf(field63 + indice, "%010ld00", totalVentaCredito);
    indice += 12;

    sprintf(field63 + indice, "%03d", 0);
    indice += 3;
    sprintf(field63 + indice, "%010d00", 0);
    indice += 12;

    sprintf(field63 + indice, "%03d", cantidadVentaDebito);
    indice += 3;
    sprintf(field63 + indice, "%010ld00", totalVentaDebito);
    indice += 12;

    sprintf(field63 + indice, "%03d", 0);
    indice += 3;
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

    generarSystemTraceNumber(systemTrace, "");
    generarBatch(batch);
    getParameter(NUMERO_TERMINAL, terminalID);

    uChar nii[TAM_NII + 1];
    memset(nii, 0x00, sizeof(nii));

    getParameter(NII, nii);

    isoHeader8583.TPDU = 60;
    memcpy(isoHeader8583.destination, nii, TAM_NII + 1);
    memcpy(isoHeader8583.source, nii, TAM_NII + 1);
    setHeader(isoHeader8583);

    setMTI("0500");
    setField(3, codigoProceso, 6);
    setField(11, systemTrace, 6);
    setField(24, nii + 1, 3);
    setField(41, terminalID, 8);
    setField(42, "000000000000000", 15);
    setField(60, batch, 6);
    setField(63, field63, indice);

    resultadoIsoPackMessage = packISOMessage();

    return resultadoIsoPackMessage;

}

int desempaquetarCierreVentas(char *inputData, int cantidadBytes) {

    ResultISOPack resultadoIsoPackMessage;
    ResultISOUnpack resultadoUnpack;
    ISOFieldMessage isoFieldMessage;
    int resultado = 0;
    char linea1[50 + 1];
    char linea2[50 + 1];
    char lineaResumen[100 + 1] = {0x00};
    //*BATCH BALANCED2    *BATCH COMPLETE2

    memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
    memset(&resultadoUnpack, 0x00, sizeof(resultadoUnpack));
    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(linea1, 0x00, sizeof(linea1));
    memset(linea2, 0x00, sizeof(linea2));
    resultadoUnpack = unpackISOMessage(inputData, cantidadBytes);
    LOGI("resultadoUnpack.responseCode %d", resultadoUnpack.responseCode);

    if (resultadoUnpack.responseCode > 0) {

        isoFieldMessage = getField(39);

        if (strcmp(isoFieldMessage.valueField, "00") == 0) {

            if (verificarArchivo(discoNetcom, FILE_CELUCOMPRA) > 0) {
                borrarArchivo(discoNetcom, FILE_CELUCOMPRA);
            }

            memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
            isoFieldMessage = getField(63);

            if ((isoFieldMessage.totalBytes > 0) && (strlen(isoFieldMessage.valueField) > 0)) {
                LOGI("BATH BALANCE");
                //*BATCH BALANCED2    *BATCH COMPLETE2
                memcpy(linea1, isoFieldMessage.valueField, 16);
                memcpy(linea2, isoFieldMessage.valueField + 20, isoFieldMessage.totalBytes - 16);
            }
            LOGI("desempaquetarCierreVentas  %s", linea1);
            LOGI("desempaquetarCierreVentas  %s", linea2);
            strcpy(lineaResumen, linea1);
            strcat(lineaResumen," ");
            strcat(lineaResumen, linea2);
            LOGI("desempaquetarCierreVentas  %s", lineaResumen);
            enviarStringToJava(lineaResumen, (char *)"mostrarInicializacion");
            resultado = 1;

        } else if (strcmp(isoFieldMessage.valueField, "95") == 0) {
            //borrarArchivo(DISCO_NETCOM,ARCHIVO_REVERSO);
            memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
            isoFieldMessage = getField(63);

            if ((isoFieldMessage.totalBytes > 0) && (strlen(isoFieldMessage.valueField) > 0)) {
                LOGI("UPLOAD BALANCE  %s", isoFieldMessage.valueField);
                memcpy(linea1, isoFieldMessage.valueField, 16);
                LOGI("B  %s", isoFieldMessage.valueField + 20);
                memcpy(linea2, isoFieldMessage.valueField + 20, isoFieldMessage.totalBytes - 16);

            }
            strcpy(lineaResumen, linea1);
            strcat(lineaResumen," ");
            strcat(lineaResumen, linea2);
            enviarStringToJava(lineaResumen, (char *)"mostrarInicializacion");
            resultado = -1;
        }

    }
    LOGI("resultado  %d", resultado);
    return resultado;
}

int getIdGrupo(char *codigoGrupo) {

    int idx = 0;
    int id = 0;
    int indiceActual = -1;

    for (idx = 0; idx < MAX_GRUPO; idx++) {
        if (strncmp(datosGrupo[idx].codigoGrupo, codigoGrupo, TAM_GRUPO) == 0) {
            indiceActual = idx; // indice donde se encontro.
            break;
        }
    }

    if (indiceActual == -1) {
        id = countFieldGrupo++; // no estba seteado previamente
    } else {
        id = indiceActual; //estaba seteado, entonces se devuelve el id donde fue seteado
    }

    return id;
}

int getIdSubgrupo(int idGrupo, char *codigoSubgrupo) {

    int idx = 0;
    int id = 0;
    int indiceActual = -1;

    for (idx = 0; idx < 10; idx++) {
        if (strncmp(datosGrupo[idGrupo].datosSubgrupo[idx].codigoSubgrupo, codigoSubgrupo,
                    TAM_SUBGRUPO) == 0) {
            indiceActual = idx; // indice donde se encontro.
            break;
        }
    }
    if (indiceActual == -1) {
        id = datosGrupo[idGrupo].totalIdSubgrupo++;
    } else {
        id = indiceActual; //estaba seteado, entonces se devuelve el id donde fue seteado
    }

    return id;
}

int cierreDescuadradoVenta(int totalTransacciones) {

    int idx = 0;
    int catidadBytes = 0;
    int resultado = 1;
    DatosVenta datosVenta;
    char outputData[512 + 1];
    char inputData[512 + 1];
    char transaccion[TAM_TIPO_TRANSACCION + 1];
    DatosCierreVenta datosCierreVenta;
    ResultISOPack resultadoIsoPackMessage;

    memset(transaccion, 0x00, sizeof(transaccion));
    sprintf(transaccion, "%02d", TRANSACCION_VENTA);

    for (idx = 0; idx < totalTransacciones; idx++) {

        memset(&datosVenta, 0x00, sizeof(datosVenta));
        datosVenta = buscarReciboByID(idx);
        //printf("\x1B - %d-%s-%s-%s",idx, datosVenta.estadoTransaccion,datosVenta.tipoTransaccion,transaccion);ttestall(0,200);
        if (strncmp(datosVenta.estadoTransaccion, "1", 1) == 0) {

            if (IS_MULTIPOS(atoi(datosVenta.tipoTransaccion)) != 1 &&
                IS_AVAL(atoi(datosVenta.tipoTransaccion)) != 1
                && IS_NO_CIERRE(atoi(datosVenta.tipoTransaccion)) != 1) {
                memset(outputData, 0x00, sizeof(outputData));
                memset(inputData, 0x00, sizeof(inputData));
                catidadBytes = empaquetarTramaVenta(datosVenta, outputData);
                if (catidadBytes > 0) {
                    resultado = realizarTransaccion(outputData, catidadBytes, inputData,
                                                    TRANSACCION_CIERRE,
                                                    CANAL_PERMANENTE, REVERSO_NO_GENERADO);
                    if (resultado < 0) {
                        resultado = -1;
                        idx = totalTransacciones;
                        break;
                    }
                }
            }
        }
    }

    if (resultado > 0) {
        memset(&datosCierreVenta, 0x00, sizeof(datosCierreVenta));
        memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
        memset(outputData, 0x00, sizeof(outputData));
        memset(inputData, 0x00, sizeof(inputData));
        datosCierreVenta = calcularCierreVenta(totalTransacciones,
                                               0); //no totaliza anuladas fuera de linea
        resultadoIsoPackMessage = empaquetarCierreVentas(datosCierreVenta, "960000");
        resultado = realizarTransaccion(resultadoIsoPackMessage.isoPackMessage,
                                        resultadoIsoPackMessage.totalBytes,
                                        outputData, TRANSACCION_CIERRE, CANAL_PERMANENTE,
                                        REVERSO_NO_GENERADO);
    }

    return resultado;

}

int empaquetarTramaVenta(DatosVenta datosVenta, char *outputData) {

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

    generarSystemTraceNumber(systemTrace, "");
    memset(variableAux, 0x00, sizeof(variableAux));
    leftPad(variableAux, datosVenta.rrn, 0x30, 12);

    sprintf(field60, "0200%s            ", datosVenta.rrn);

    isoHeader8583.TPDU = 60;
    strcpy(isoHeader8583.destination, "0001");
    strcpy(isoHeader8583.source, "0001");
    setHeader(isoHeader8583);

    setMTI("0320");
    setField(3, datosVenta.processingCode, 6);
    setField(4, datosVenta.totalVenta, 12);
    setField(11, systemTrace, 6);
    setField(12, datosVenta.txnTime, 6);
    setField(13, datosVenta.txnDate, 4);
    setField(22, datosVenta.posEntrymode, 3);
    setField(24, datosVenta.nii, 3);
    setField(25, "00", 2);
    setField(35, datosVenta.track2, strlen(datosVenta.track2));
    setField(37, variableAux, 12);
    setField(38, datosVenta.codigoAprobacion, strlen(datosVenta.codigoAprobacion));
    setField(41, datosVenta.terminalId, 8);
    setField(42, datosVenta.fiel42, 15);
    setField(60, field60, 22);
    setField(62, datosVenta.numeroRecibo, 6);

    resultadoIsoPackMessage = packISOMessage();

    if (resultadoIsoPackMessage.responseCode > 0) {
        cantidadBytes = resultadoIsoPackMessage.totalBytes;
        memcpy(outputData, resultadoIsoPackMessage.isoPackMessage,
               resultadoIsoPackMessage.totalBytes);
    }

    return cantidadBytes;
}

void imprimirReporteCierreVentas(int rTotales) {

    int idx = 0;
    int idy = 0;
    long subTotal = 0;
    long total = 0;
    long finalTotalVenta = 0;
    long finalTotalIva = 0;
    long finalTotalInc = 0;
    long finalTotalPropina = 0;
    int finalCantidadVenta = 0;
    int finalCantidadAnulaciones = 0;
    long finalTotalAnulaciones = 0;
    long finalGranTotalVenta = 0;
    long finalGranTotalIva = 0;
    long finalGranTotalInc = 0;
    long finalGranTotalPropina = 0;
    long finalGranTotalTransacionVenta = 0;
    long finalGranTotalTransacionAnulacion = 0;
    long finalTotalBase = 0;
    int finalGranCantidadTransacionVenta = 0;
    int finalGranCantidadTransacionAnulacion = 0;
    int finalCantidadQPS = 0;
    long finalTotalQPS = 0;
    long totalPagos = 0;
    char lineaSalida[60 + 1];
    char terminal[10 + 1];
    char valor[20 + 1];
    char valorAnulacion[20 + 1];
    char convenioBig[16 + 1];
    char fiidBig[4 + 1];
    TablaUnoInicializacion tablaUno;
    DatosBasicosCierre basicosCierre;

    uChar *data = NULL;

    memset(terminal, 0x00, sizeof(terminal));
    memset(&tablaUno, 0x00, sizeof(tablaUno));
    memset(&basicosCierre, 0x00, sizeof(basicosCierre));
    memset(convenioBig, 0x00, sizeof(convenioBig));
    memset(fiidBig, 0x00, sizeof(fiidBig));
    getParameter(NUMERO_TERMINAL, terminal);

    //logo
    //imprimirEncabezado();

    tablaUno = _desempaquetarTablaCeroUno_();

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "%s\n", tablaUno.defaultMerchantName); // nombre comercio
    pprintf(lineaSalida);
    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "%s\n", tablaUno.receiptLine2); // direccion comercio
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));

    if (rTotales == TRUE) {
        sprintf(lineaSalida, "%s", "   REPORTE DE TOTALES   \n");
    } else {
        sprintf(lineaSalida, "%s", "   REPORTE DE CIERRE    \n");
    }

    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "COD: %s\n", tablaUno.receiptLine3);
    pprintf(lineaSalida);


    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "TERMINAL: %s\n", terminal);
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "%s", "         TOTAL TRANSACCIONES POR EMISOR       \n");
    pprintf(lineaSalida);

    if ((tablaUno.options1.tipProcessing == 1) &&
        (tablaUno.telephoneDialOptions.keyboardDialing == 1)) {
        tasaAero = 1;
    }

    ordenarGrupoFranquicia();
    for (idx = 0; idx < countFieldGrupo; idx++) {
        finalTotalVenta = 0;
        finalTotalIva = 0;
        finalTotalInc = 0;
        finalTotalPropina = 0;
        finalCantidadVenta = 0;
        finalCantidadAnulaciones = 0;
        finalTotalAnulaciones = 0;
        for (idy = 0; idy < datosGrupo[idx].totalIdSubgrupo; idy++) {
            subTotal = datosGrupo[idx].datosSubgrupo[idy].totalVenta +
                       datosGrupo[idx].datosSubgrupo[idy].totalIva
                       + datosGrupo[idx].datosSubgrupo[idy].totalInc +
                       datosGrupo[idx].datosSubgrupo[idy].totalPropina;
            finalTotalVenta += datosGrupo[idx].datosSubgrupo[idy].totalVenta;
            finalTotalIva += datosGrupo[idx].datosSubgrupo[idy].totalIva;
            finalTotalInc += datosGrupo[idx].datosSubgrupo[idy].totalInc;
            finalTotalPropina += datosGrupo[idx].datosSubgrupo[idy].totalPropina;
            finalCantidadVenta += datosGrupo[idx].datosSubgrupo[idy].cantidadVenta;
            finalCantidadAnulaciones += datosGrupo[idx].datosSubgrupo[idy].cantidadAnulacion;
            finalTotalAnulaciones += datosGrupo[idx].datosSubgrupo[idy].totalAnulacion;
            finalTotalBase += datosGrupo[idx].datosSubgrupo[idy].totalBase;
            finalTotalQPS += datosGrupo[idx].datosSubgrupo[idy].totalQPS;
            finalCantidadQPS += datosGrupo[idx].datosSubgrupo[idy].cantidadQPS;


            pprintf(datosGrupo[idx].datosSubgrupo[idy].nombreSubgrupo);

            memset(lineaSalida, 0x00, sizeof(lineaSalida));
            memset(valor, 0x00, sizeof(valor));
            formatString(0x00, datosGrupo[idx].datosSubgrupo[idy].totalVenta, "", 0, valor, 1);
            sprintf(lineaSalida, "COMPRA       : %3d%16s%13s\n",
                    datosGrupo[idx].datosSubgrupo[idy].cantidadVenta, " ",
                    valor);
            pprintf(lineaSalida);
            memset(lineaSalida, 0x00, sizeof(lineaSalida));
            memset(valor, 0x00, sizeof(valor));
            formatString(0x00, datosGrupo[idx].datosSubgrupo[idy].totalIva, "", 0, valor, 1);
            sprintf(lineaSalida, "IVA          : %19s%13s\n", " ", valor);
            pprintf(lineaSalida);
            memset(lineaSalida, 0x00, sizeof(lineaSalida));
            memset(valor, 0x00, sizeof(valor));
            formatString(0x00, datosGrupo[idx].datosSubgrupo[idy].totalInc, "", 0, valor, 1);
            sprintf(lineaSalida, "INC          : %19s%13s\n", " ", valor);
            pprintf(lineaSalida);
            memset(lineaSalida, 0x00, sizeof(lineaSalida));
            memset(valor, 0x00, sizeof(valor));
            formatString(0x00, datosGrupo[idx].datosSubgrupo[idy].totalPropina, "", 0, valor, 1);

            if (tasaAero == 1) {
                sprintf(lineaSalida, "T.AEROP      : %19s%13s\n", " ", valor);
                pprintf(lineaSalida);
            } else if (tablaUno.options1.tipProcessing == 1) {
                sprintf(lineaSalida, "PROPINA      : %19s%13s\n", " ", valor);
                pprintf(lineaSalida);
            }
            memset(lineaSalida, 0x00, sizeof(lineaSalida));
            sprintf(lineaSalida, "%s", "-----------------------------------------------\n");
            pprintf(lineaSalida);

            memset(lineaSalida, 0x00, sizeof(lineaSalida));
            memset(valor, 0x00, sizeof(valor));
            formatString(0x00, subTotal, "", 0, valor, 1);
            sprintf(lineaSalida, "SUBTOTAL     : %3d%16s%13s\n",
                    datosGrupo[idx].datosSubgrupo[idy].cantidadVenta, " ",
                    valor);
            pprintf(lineaSalida);
            memset(lineaSalida, 0x00, sizeof(lineaSalida));
            memset(valor, 0x00, sizeof(valor));
            formatString(0x00, datosGrupo[idx].datosSubgrupo[idy].totalAnulacion, "", 0, valor, 1);
            memset(valorAnulacion, 0x00, sizeof(valorAnulacion));
            if (datosGrupo[idx].datosSubgrupo[idy].totalAnulacion > 0) {
                strcat(valorAnulacion, "-");
            }
            strcat(valorAnulacion, valor);
            sprintf(lineaSalida, "ANULACIONES  : %3d%16s%13s\n",
                    datosGrupo[idx].datosSubgrupo[idy].cantidadAnulacion,
                    " ", valorAnulacion);
            pprintf(lineaSalida);
        }

        total = finalTotalVenta + finalTotalIva + finalTotalPropina + finalTotalInc;

        if ((total > 0) || (finalTotalAnulaciones > 0)) {

            pprintf(datosGrupo[idx].nombreGrupo);

            memset(lineaSalida, 0x00, sizeof(lineaSalida));
            memset(valor, 0x00, sizeof(valor));
            formatString(0x00, finalTotalVenta, "", 0, valor, 1);
            sprintf(lineaSalida, "COMPRA       : %3d%16s%13s\n", finalCantidadVenta, " ", valor);
            pprintf(lineaSalida);
            memset(lineaSalida, 0x00, sizeof(lineaSalida));
            memset(valor, 0x00, sizeof(valor));
            formatString(0x00, finalTotalIva, "", 0, valor, 1);
            sprintf(lineaSalida, "IVA          : %19s%13s\n", " ", valor);
            pprintf(lineaSalida);
            memset(lineaSalida, 0x00, sizeof(lineaSalida));
            memset(valor, 0x00, sizeof(valor));
            formatString(0x00, finalTotalInc, "", 0, valor, 1);
            sprintf(lineaSalida, "INC          : %19s%13s\n", " ", valor);
            pprintf(lineaSalida);
            memset(lineaSalida, 0x00, sizeof(lineaSalida));
            memset(valor, 0x00, sizeof(valor));
            formatString(0x00, finalTotalPropina, "", 0, valor, 1);

            if (tasaAero == 1) {
                sprintf(lineaSalida, "T.AEROP      : %19s%13s\n", " ", valor);
                pprintf(lineaSalida);
            } else if (tablaUno.options1.tipProcessing == 1) {
                sprintf(lineaSalida, "PROPINA      : %19s%13s\n", " ", valor);
                pprintf(lineaSalida);
            }
            memset(lineaSalida, 0x00, sizeof(lineaSalida));
            sprintf(lineaSalida, "%s", "-----------------------------------------------\n");
            pprintf(lineaSalida);

            memset(lineaSalida, 0x00, sizeof(lineaSalida));
            memset(valor, 0x00, sizeof(valor));
            formatString(0x00, total, "", 0, valor, 1);
            sprintf(lineaSalida, "TOTAL        : %3d%16s%13s\n", finalCantidadVenta, " ", valor);
            pprintf(lineaSalida);
            memset(lineaSalida, 0x00, sizeof(lineaSalida));
            memset(valor, 0x00, sizeof(valor));
            formatString(0x00, finalTotalAnulaciones, "", 0, valor, 1);
            memset(valorAnulacion, 0x00, sizeof(valorAnulacion));
            if (finalTotalAnulaciones > 0) {
                strcat(valorAnulacion, "-");
            }
            strcat(valorAnulacion, valor);
            sprintf(lineaSalida, "ANULACIONES  : %3d%16s%13s\n", finalCantidadAnulaciones, " ",
                    valorAnulacion);
            pprintf(lineaSalida);

            memset(lineaSalida, 0x00, sizeof(lineaSalida));
            sprintf(lineaSalida, "%s", "-----------------------------------------------\n");
            pprintf(lineaSalida);

            memset(lineaSalida, 0x00, sizeof(lineaSalida));
            sprintf(lineaSalida, "%s", "-----------------------------------------------\n");
            pprintf(lineaSalida);


            finalGranCantidadTransacionVenta += finalCantidadVenta;
            finalGranTotalTransacionVenta += finalTotalVenta;
            finalGranTotalIva += finalTotalIva;
            finalGranTotalInc += finalTotalInc;
            finalGranTotalPropina += finalTotalPropina;
            finalGranCantidadTransacionAnulacion += finalCantidadAnulaciones;
            finalGranTotalTransacionAnulacion += finalTotalAnulaciones;

        }

    }

    if (totalTransaccionesDineroElectronico > 0) {
        LOGI("totalTransaccionesDineroElectronico A");
        cierreDineroElectronico(&basicosCierre);
        LOGI("totalTransaccionesDineroElectronico B");
        finalGranCantidadTransacionVenta += basicosCierre.cantidadVenta;
        finalGranTotalTransacionVenta += atol(basicosCierre.datos.compraNeta);
        finalGranTotalIva += atol(basicosCierre.datos.iva);
        finalGranTotalInc += atol(basicosCierre.datos.inc);
        finalGranTotalPropina += atol(basicosCierre.datos.propina);
        finalGranCantidadTransacionAnulacion += basicosCierre.cantidadAnulaciones;
        finalGranTotalTransacionAnulacion += atol(basicosCierre.totalAnulacion);
        finalTotalBase += atol(basicosCierre.datos.baseDevolucion);

    }

    /////DINERO ELECTRONICO RETIRO
    LOGI("Reportes de dinero electronico A");
    if (totalTransaccionesDineroElectronicoRetiro > 0) {
        LOGI("Reportes de dinero electronico B");
        cierreDineroElectronicoRetiro(&basicosCierre);
    }

    ///GASOPASS
    if (totalTransaccionesGASOPASS > 0) {

        memset(&basicosCierre, 0x00, sizeof(basicosCierre));
        //cierreGasoPass(1, &basicosCierre);
        finalGranCantidadTransacionVenta += basicosCierre.cantidadVenta;
        finalGranTotalTransacionVenta += atol(basicosCierre.datos.compraNeta);
        finalGranTotalIva += atol(basicosCierre.datos.iva);
        finalGranTotalInc += atol(basicosCierre.datos.inc);
        finalGranTotalPropina += atol(basicosCierre.datos.propina);
        finalGranCantidadTransacionAnulacion += basicosCierre.cantidadAnulaciones;
        finalGranTotalTransacionAnulacion += atol(basicosCierre.totalAnulacion);
        finalTotalBase += atol(basicosCierre.datos.baseDevolucion);

    }

    ///TARJETAMICOMPRA
    if (totalTransaccionesTarjetaMiCompra > 0) {

        memset(&basicosCierre, 0x00, sizeof(basicosCierre));

        basicosCierre.datos.tipoPagoDivididas = 1;
        //netcomServices(APP_TYPE_TARJETA_MI_COMPRA, CIERRE_MI_COMPRA, sizeof(basicosCierre), &basicosCierre);
        basicosCierre.datos.tipoPagoDivididas = 0;
        finalGranCantidadTransacionVenta += basicosCierre.cantidadVenta;
        finalGranTotalTransacionVenta += atol(basicosCierre.datos.compraNeta);
        finalGranTotalIva += atol(basicosCierre.datos.iva);
        finalGranTotalInc += atol(basicosCierre.datos.inc);
        finalGranTotalPropina += atol(basicosCierre.datos.propina);
        finalGranCantidadTransacionAnulacion += basicosCierre.cantidadAnulaciones;
        finalGranTotalTransacionAnulacion += atol(basicosCierre.totalAnulacion);
        finalTotalBase += atol(basicosCierre.datos.baseDevolucion);

    }

    ///// QRCODE /////
    if (datosQrcode.cantidadTransacciones > 0) {
        cierreQrcode();
        finalGranCantidadTransacionVenta += datosQrcode.cantidadDebitos;
        finalGranTotalTransacionVenta += datosQrcode.totalCompraNeta;
        finalGranTotalIva += datosQrcode.totalIva;
        finalGranTotalInc += datosQrcode.totalInc;
        finalGranTotalPropina += datosQrcode.totalPropina;
        finalGranCantidadTransacionAnulacion += datosQrcode.cantidadAnuladas;
        finalGranTotalTransacionAnulacion += datosQrcode.totalAnuladas;
        finalTotalBase += datosQrcode.totalBase;
    }

    ///// CONSULTA ESTADO /////
    if (datosConsultaEstado.cantidadTransacciones > 0) {
        cierreConsultaEstado();
        finalGranCantidadTransacionVenta += datosConsultaEstado.cantidadDebitos;
        finalGranTotalTransacionVenta += datosConsultaEstado.totalCompraNeta;
        finalGranTotalIva += datosConsultaEstado.totalIva;
        finalGranTotalInc += datosConsultaEstado.totalInc;
        finalGranTotalPropina += datosConsultaEstado.totalPropina;
        finalGranCantidadTransacionAnulacion += datosConsultaEstado.cantidadAnuladas;
        finalGranTotalTransacionAnulacion += datosConsultaEstado.totalAnuladas;
        finalTotalBase += datosConsultaEstado.totalBase;
    }

    ///// DESCRIPCION CELUCOMPRA /////
    if (datosCeluCompra.cantidadTransacciones > 0) {
        cierreCeluCompra();
        finalGranCantidadTransacionVenta += datosCeluCompra.cantidadDebitos;
        finalGranTotalTransacionVenta += datosCeluCompra.totalCompraNeta;
        finalGranTotalIva += datosCeluCompra.totalIva;
        finalGranTotalInc += datosCeluCompra.totalInc;
        finalGranTotalPropina += datosCeluCompra.totalPropina;
        finalGranCantidadTransacionAnulacion += datosCeluCompra.cantidadAnuladas;
        finalGranTotalTransacionAnulacion += datosCeluCompra.totalAnuladas;
        finalTotalBase += datosCeluCompra.totalBase;
    }

    finalGranTotalVenta = finalGranTotalTransacionVenta + finalGranTotalIva + finalGranTotalInc +
                          finalGranTotalPropina;

    pprintf("      GRAN TOTAL      ");

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    formatString(0x00, finalGranTotalTransacionVenta, "", 0, valor, 1);
    sprintf(lineaSalida, "COMPRA       : %3d%16s%13s", finalGranCantidadTransacionVenta, " ",
            valor);
    pprintf(lineaSalida);
    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    formatString(0x00, finalGranTotalIva, "", 0, valor, 1);
    sprintf(lineaSalida, "IVA          : %19s%13s", " ", valor);
    pprintf(lineaSalida);
    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    formatString(0x00, finalGranTotalInc, "", 0, valor, 1);
    sprintf(lineaSalida, "INC          : %19s%13s", " ", valor);
    pprintf(lineaSalida);
    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    formatString(0x00, finalGranTotalPropina, "", 0, valor, 1);

    if (tasaAero == 1) {
        sprintf(lineaSalida, "T.AEROP      : %19s%13s", " ", valor);
        pprintf(lineaSalida);
    } else if ((tablaUno.options1.tipProcessing == 1) || (datosConsultaEstado.totalPropina > 0)) {
        sprintf(lineaSalida, "PROPINA      : %19s%13s", " ", valor);
        pprintf(lineaSalida);
    }

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "%s", "-----------------------------------------------\n");
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    formatString(0x00, finalGranTotalVenta, "", 0, valor, 1);
    sprintf(lineaSalida, "TOTAL        : %3d%16s%13s", finalGranCantidadTransacionVenta, " ",
            valor);
    pprintf(lineaSalida);
    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    formatString(0x00, finalGranTotalTransacionAnulacion, "", 0, valor, 1);
    memset(valorAnulacion, 0x00, sizeof(valorAnulacion));
    if (finalGranTotalTransacionAnulacion > 0) {
        strcat(valorAnulacion, "-");
    }
    strcat(valorAnulacion, valor);
    sprintf(lineaSalida, "ANULACIONES  : %3d%16s%13s", finalGranCantidadTransacionAnulacion, " ",
            valorAnulacion);
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    formatString(0x00, finalTotalBase, "", 0, valor, 1);
    sprintf(lineaSalida, "TOTAL BASE DEVOLUCION %12s%13s", " ", valor);
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "%s", "-----------------------------------------------");
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "%s", "-----------------------------------------------");
    pprintf(lineaSalida);

    if (datosLealtad.cantidadLealtad > 0) {

        pprintf("       LEALTAD");

        memset(lineaSalida, 0x00, sizeof(lineaSalida));
        memset(valor, 0x00, sizeof(valor));
        formatString(0x00, datosLealtad.totalAcumuladas, "", 0, valor, 1);
        sprintf(lineaSalida, "ACUMULACIONES : %3d%16s%13s", datosLealtad.cantidadAcumuladas, " ",
                valor);
        pprintf(lineaSalida);

        memset(lineaSalida, 0x00, sizeof(lineaSalida));
        memset(valor, 0x00, sizeof(valor));
        formatString(0x00, datosLealtad.totalRedimidas, "", 0, valor, 1);
        sprintf(lineaSalida, "REDENCIONES   : %3d%16s%13s", datosLealtad.cantidadRedimidas, " ",
                valor);
        pprintf(lineaSalida);

        memset(lineaSalida, 0x00, sizeof(lineaSalida));
        memset(valor, 0x00, sizeof(valor));
        formatString(0x00, datosLealtad.totalPendientes, "", 0, valor, 1);
        sprintf(lineaSalida, "PEN ACUMUL    : %3d%16s%13s", datosLealtad.cantidadPendientes, " ",
                valor);
        pprintf(lineaSalida);
    }
    if (datosPuntosColombia.cantidadLealtad > 0 || datosPColombia.totalAnuladas > 0) {

        pprintf("       PUNTOS COLOMBIA");

        memset(lineaSalida, 0x00, sizeof(lineaSalida));
        memset(valor, 0x00, sizeof(valor));
        formatString(0x00, datosPuntosColombia.totalAcumuladas, "", 0, valor, 1);
        sprintf(lineaSalida, "ACUMULACIONES : %3d%16s%13s",
                datosPuntosColombia.cantidadAcumuladas, " ",
                valor);
        pprintf(lineaSalida);

        memset(lineaSalida, 0x00, sizeof(lineaSalida));
        memset(valor, 0x00, sizeof(valor));
        formatString(0x00, datosPuntosColombia.totalRedimidas, "", 0, valor, 1);
        sprintf(lineaSalida, "REDENCIONES   : %3d%16s%13s", datosPuntosColombia.cantidadRedimidas,
                " ", valor);
        pprintf(lineaSalida);

        memset(valorAnulacion, 0x00, sizeof(valorAnulacion));
        memset(valor, 0x00, sizeof(valor));
        formatString(0x00, datosPColombia.totalAnuladas, "", 0, valor, 1);
        if (datosPColombia.totalAnuladas > 0) {
            strcat(valorAnulacion, "-");
        }
        strcat(valorAnulacion, valor);
        sprintf(lineaSalida, "ANULACIONES   : %3d%16s%13s", datosPColombia.cantidadAnuladas, " ",
                valorAnulacion);
        pprintf(lineaSalida);

    }
    if (datosLealtad.bigBancolombia == 1) {

        idx = 0;
        long lengthTablaBig = 0;
        int itemsBig = 0;

        lengthTablaBig = tamArchivo(discoNetcom, TX_BIG_BANCOLOMBIA);

        itemsBig = lengthTablaBig / 34;

        pprintf("      LEALTAD PLUS");


        for (idx = 0; idx < itemsBig; idx++) {

            if (datosLealtadBig[idx].cantidadRedimidasBig > 0 ||
                datosLealtadBig[idx].cantidadAcumuladas > 0
                || datosBigBancolombia[idx].totalAnuladas > 0) {


                invocarMenuBig(convenioBig, fiidBig, idx);

                pprintf(convenioBig);

                memset(lineaSalida, 0x00, sizeof(lineaSalida));
                memset(valor, 0x00, sizeof(valor));
                formatString(0x00, datosLealtadBig[idx].totalRedimidasBig, "", 0, valor, 1);
                sprintf(lineaSalida, "REDENCIONES   : %3d%16s%13s",
                        datosLealtadBig[idx].cantidadRedimidasBig, " ",
                        valor);
                pprintf(lineaSalida);

                memset(lineaSalida, 0x00, sizeof(lineaSalida));
                memset(valor, 0x00, sizeof(valor));
                datosLealtadBig[idx].totalAcumuladas = 0;
                formatString(0x00, datosLealtadBig[idx].totalAcumuladas, "", 0, valor, 1);
                sprintf(lineaSalida, "ACUMULACIONES : %3d%16s%13s", 0, " ", valor);
                pprintf(lineaSalida);

                memset(valorAnulacion, 0x00, sizeof(valorAnulacion));
                memset(valor, 0x00, sizeof(valor));
                formatString(0x00, datosBigBancolombia[idx].totalAnuladas, "", 0, valor, 1);
                if (datosBigBancolombia[idx].totalAnuladas > 0) {
                    strcat(valorAnulacion, "-");
                }
                strcat(valorAnulacion, valor);
                sprintf(lineaSalida, "ANULACIONES   : %3d%16s%13s",
                        datosBigBancolombia[idx].cantidadAnuladas, " ",
                        valorAnulacion);
                pprintf(lineaSalida);

            }

        }
    }

    if (totalesTarjetaPrivada.cantidadVentaCredito > 0) {

        ////////////////////////////////////////////////////////////////////////////
        pprintf("      T. PRIVADA");

        memset(lineaSalida, 0x00, sizeof(lineaSalida));
        memset(valor, 0x00, sizeof(valor));
        formatString(0x00, tarjetaPrivada.totalVentaCredito, "", 0, valor, 1);
        sprintf(lineaSalida, "TARJETA       : %3d%16s%13s", tarjetaPrivada.cantidadVentaCredito,
                " ", valor);
        pprintf(lineaSalida);

        memset(lineaSalida, 0x00, sizeof(lineaSalida));
        memset(valor, 0x00, sizeof(valor));
        formatString(0x00, tarjetaPrivadaEfectivo.totalVentaCredito, "", 0, valor, 1);
        sprintf(lineaSalida, "EFECTIVO      : %3d%16s%13s",
                tarjetaPrivadaEfectivo.cantidadVentaCredito, " ", valor);
        pprintf(lineaSalida);

        memset(lineaSalida, 0x00, sizeof(lineaSalida));
        memset(valor, 0x00, sizeof(valor));
        formatString(0x00, tarjetaPrivadaCombinado.totalVentaCredito, "", 0, valor, 1);
        sprintf(lineaSalida, "COMBINADO     : %3d%16s%13s",
                tarjetaPrivadaCombinado.cantidadVentaCredito, " ", valor);
        pprintf(lineaSalida);

    }

    if (datosBonos.cantidadRedencion > 0 || datosBonos.cantidadHabilitacion > 0) {

        pprintf("     TOTAL BONOS");

        memset(lineaSalida, 0x00, sizeof(lineaSalida));
        memset(valor, 0x00, sizeof(valor));
        formatString(0x00, datosBonos.totalRedencion, "", 0, valor, 1);

        sprintf(lineaSalida, "REDENCIONES   : %3d%9s%20s", datosBonos.cantidadRedencion, " ",
                valor);
        pprintf(lineaSalida);

        memset(lineaSalida, 0x00, sizeof(lineaSalida));
        memset(valor, 0x00, sizeof(valor));
        formatString(0x00, datosBonos.totalHabilitacion, "", 0, valor, 1);
        sprintf(lineaSalida, "HABILITACIONES: %3d%9s%20s", datosBonos.cantidadHabilitacion, " ",
                valor);
        pprintf(lineaSalida);

    }

    if (datosBonos.cantidadRecarga > 0 || datosBonos.cantidadAnulacion > 0) {

        pprintf("  BONOS RECARGABLES");

        memset(lineaSalida, 0x00, sizeof(lineaSalida));
        memset(valor, 0x00, sizeof(valor));
        formatString(0x00, datosBonos.totalRecarga, "", 0, valor, 1);
        sprintf(lineaSalida, "RECARGAS      : %3d%16s%13s", datosBonos.cantidadRecarga, " ",
                valor);
        pprintf(lineaSalida);

        memset(lineaSalida, 0x00, sizeof(lineaSalida));
        memset(valor, 0x00, sizeof(valor));
        formatString(0x00, datosBonos.totalAnulacion, "", 0, valor, 1);
        memset(valorAnulacion, 0x00, sizeof(valorAnulacion));
        if (datosBonos.totalAnulacion > 0) {
            strcat(valorAnulacion, "-");
        }
        strcat(valorAnulacion, valor);
        sprintf(lineaSalida, "ANULACIONES   : %3d%16s%13s", datosBonos.cantidadAnulacion, " ",
                valorAnulacion);
        pprintf(lineaSalida);

    }

    if ((datosPCR.cantidadVentaCredito > 0) || (datosPCR.cantidadVentaDebito > 0)) {
        pprintf("         PCR");

        memset(lineaSalida, 0x00, sizeof(lineaSalida));
        memset(valor, 0x00, sizeof(valor));

        totalPagos = datosPCR.totalVentaCredito + datosPCR.totalVentaDebito;

        formatString(0x00, totalPagos, "", 0, valor, 1);
        sprintf(lineaSalida, "PCR           : %3d%16s%13s",
                (datosPCR.cantidadVentaCredito + datosPCR.cantidadVentaDebito), " ", valor);
        pprintf(lineaSalida);
    }

    if ((datosRecargaCelular.cantidadVentaCredito > 0) ||
        (datosRecargaCelular.cantidadVentaDebito > 0)) {

        if (totalesRecarga.cantidadRecargaCelularEfectivo > 0) {
            pprintf("   RECARGA EFECTIVO");

            memset(lineaSalida, 0x00, sizeof(lineaSalida));
            memset(valor, 0x00, sizeof(valor));

            formatString(0x00, totalesRecarga.totalRecargaCelularEfectivo, "", 0, valor, 1);

            sprintf(lineaSalida, "REC EFECTIVO  : %3d%16s%13s",
                    totalesRecarga.cantidadRecargaCelularEfectivo, " ",
                    valor);
            pprintf(lineaSalida);
        }

        if (totalesRecarga.cantidadComcel > 0 || totalesRecarga.cantidadMovistar > 0 ||
            totalesRecarga.cantidadTigo > 0
            || totalesRecarga.totalUff > 0 || totalesRecarga.totalDirectv > 0) {
            pprintf("RECARGAS");

            if (totalesRecarga.cantidadMovistar > 0) {
                memset(lineaSalida, 0x00, sizeof(lineaSalida));
                memset(valor, 0x00, sizeof(valor));

                formatString(0x00, totalesRecarga.totalMovistar, "", 0, valor, 1);

                sprintf(lineaSalida, "MOVISTAR      : %3d%16s%13s",
                        totalesRecarga.cantidadMovistar, " ", valor);
                pprintf(lineaSalida);
            }

            if (totalesRecarga.cantidadComcel > 0) {
                memset(lineaSalida, 0x00, sizeof(lineaSalida));
                memset(valor, 0x00, sizeof(valor));

                formatString(0x00, totalesRecarga.totalComcel, "", 0, valor, 1);

                sprintf(lineaSalida, "COMCEL S.A    : %3d%16s%13s", totalesRecarga.cantidadComcel,
                        " ", valor);
                pprintf(lineaSalida);
            }

            if (totalesRecarga.cantidadTigo > 0) {
                memset(lineaSalida, 0x00, sizeof(lineaSalida));
                memset(valor, 0x00, sizeof(valor));

                formatString(0x00, totalesRecarga.totalTigo, "", 0, valor, 1);

                sprintf(lineaSalida, "TIGO          : %3d%16s%13s", totalesRecarga.cantidadTigo,
                        " ", valor);
                pprintf(lineaSalida);
            }

            if (totalesRecarga.cantidadUff > 0) {
                memset(lineaSalida, 0x00, sizeof(lineaSalida));
                memset(valor, 0x00, sizeof(valor));

                formatString(0x00, totalesRecarga.totalUff, "", 0, valor, 1);

                sprintf(lineaSalida, "UFF!MOVIL     : %3d%16s%13s", totalesRecarga.cantidadUff,
                        " ", valor);
                pprintf(lineaSalida);
            }
            if (totalesRecarga.cantidadDirectv > 0) {
                memset(lineaSalida, 0x00, sizeof(lineaSalida));
                memset(valor, 0x00, sizeof(valor));
                formatString(0x00, totalesRecarga.totalDirectv, "", 0, valor, 1);
                sprintf(lineaSalida, "DIRECTV       : %3d%16s%13s",
                        totalesRecarga.cantidadDirectv, " ", valor);
                pprintf(lineaSalida);
            }
        }
    }

    if (datosAvanceEfectivo.totalVentaDebito > 0 || datosAvanceEfectivo.cantidadVentaCredito > 0) {

        pprintf("       EFECTIVO    ");

        memset(lineaSalida, 0x00, sizeof(lineaSalida));
        memset(valor, 0x00, sizeof(valor));
        formatString(0x00, datosAvanceEfectivo.totalVentaCredito, "", 0, valor, 1);
        sprintf(lineaSalida, "EFECTIVO      : %3d%16s%13s",
                datosAvanceEfectivo.cantidadVentaCredito, " ", valor);
        pprintf(lineaSalida);
        memset(valorAnulacion, 0x00, sizeof(valorAnulacion));
        memset(valor, 0x00, sizeof(valor));
        formatString(0x00, datosAvanceEfectivo.totalVentaDebito, "", 0, valorAnulacion, 1);
        if (datosAvanceEfectivo.cantidadVentaDebito > 0) {
            strcat(valor, "-");
        }
        strcat(valor, valorAnulacion);
        memset(lineaSalida, 0x00, sizeof(lineaSalida));

        sprintf(lineaSalida, "ANULACIONES   : %3d%15s%14s",
                datosAvanceEfectivo.cantidadVentaDebito, " ", valor);
        pprintf(lineaSalida);
    }

    if (datosPagoImpuestos.totalVentaDebito > 0 || datosPagoImpuestos.cantidadVentaCredito > 0) {

        pprintf("       IMPUESTOS    ");

        memset(lineaSalida, 0x00, sizeof(lineaSalida));
        memset(valor, 0x00, sizeof(valor));
        formatString(0x00, datosPagoImpuestos.totalVentaCredito, "", 0, valor, 1);
        sprintf(lineaSalida, "IMPUESTOS     : %3d%16s%13s",
                datosPagoImpuestos.cantidadVentaCredito, " ", valor);
        pprintf(lineaSalida);
        memset(valorAnulacion, 0x00, sizeof(valorAnulacion));
        memset(valor, 0x00, sizeof(valor));
        formatString(0x00, datosPagoImpuestos.totalVentaDebito, "", 0, valorAnulacion, 1);
        if (datosPagoImpuestos.cantidadVentaDebito > 0) {
            strcat(valor, "-");
        }
        strcat(valor, valorAnulacion);
        memset(lineaSalida, 0x00, sizeof(lineaSalida));

        sprintf(lineaSalida, "ANULACIONES   : %3d%15s%14s",
                datosPagoImpuestos.cantidadVentaDebito, " ", valor);
        pprintf(lineaSalida);
    }

    if (datosFocoSodexo.totalVentaDebito > 0
        || datosFocoSodexo.cantidadVentaCredito > 0) {

        pprintf("       WIZEO       ");

        memset(lineaSalida, 0x00, sizeof(lineaSalida));
        memset(valor, 0x00, sizeof(valor));
        formatString(0x00, datosFocoSodexo.totalVentaCredito, "", 0, valor, 1);
        sprintf(lineaSalida, "WIZEO         : %3d%16s%13s",
                datosFocoSodexo.cantidadVentaCredito, " ", valor);
        pprintf(lineaSalida);
        memset(valorAnulacion, 0x00, sizeof(valorAnulacion));
        memset(valor, 0x00, sizeof(valor));
        formatString(0x00, datosFocoSodexo.totalVentaDebito, "", 0,
                     valorAnulacion, 1);
        if (datosFocoSodexo.cantidadVentaDebito > 0) {
            strcat(valor, "-");
        }
        strcat(valor, valorAnulacion);
        memset(lineaSalida, 0x00, sizeof(lineaSalida));

        sprintf(lineaSalida, "ANULACIONES   : %3d%15s%14s",
                datosFocoSodexo.cantidadVentaDebito, " ", valor);
        pprintf(lineaSalida);
    }
    /// Imprimir Totales PayPass
    imprimirTotalesPaypass();
    imprimirTotalesPayWave();

    if (finalCantidadQPS > 0) {
        pprintf("         QPS          ");

        memset(lineaSalida, 0x00, sizeof(lineaSalida));
        memset(valor, 0x00, sizeof(valor));
        formatString(0x00, finalTotalQPS, "", 0, valor, 1);
        sprintf(lineaSalida, "QPS          : %3d%16s%13s", finalCantidadQPS, " ", valor);
        pprintf(lineaSalida);
    }


    if (totalComerciosMultiPos > 0) {
        //cierreMultiPos(1, datosCierreComercios, totalComerciosMultiPos, tasaAero);
        limpiarDatosMultiPos();
    }

    if (recaudoCNBAval.totalVentaDebito > 0 || recaudoCNBAval.totalVentaCredito > 0) {
        //netcomServices(APP_TYPE_CNB_AVAL, SERVICIO_CIERRE, 1, &data);
    }

}

void cierreCeluCompra(void) {

    uChar lineaSalida[48 + 1];
    uChar valorAnulacion[48 + 1];
    uChar valor[48 + 1];

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));

    pprintf("       CELUCOMPRA\n");

    formatString(0x00, datosCeluCompra.totalCompraNeta, "", 0, valor, 1);
    sprintf(lineaSalida, "COMPRA       : %3d%16s%13s\n", datosCeluCompra.cantidadDebitos, " ",
            valor);
    pprintf(lineaSalida);
    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    formatString(0x00, datosCeluCompra.totalIva, "", 0, valor, 1);
    sprintf(lineaSalida, "IVA          : %19s%13s\n", " ", valor);
    pprintf(lineaSalida);
    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    formatString(0x00, datosCeluCompra.totalInc, "", 0, valor, 1);
    sprintf(lineaSalida, "INC          : %19s%13s\n", " ", valor);
    pprintf(lineaSalida);
    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    formatString(0x00, datosCeluCompra.totalPropina, "", 0, valor, 1);

    sprintf(lineaSalida, "PROPINA      : %19s%13s\n", " ", valor);

    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "%s", "-----------------------------------------------\n");
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    formatString(0x00, datosCeluCompra.totalDebitos, "", 0, valor, 1);
    sprintf(lineaSalida, "TOTAL        : %3d%16s%13s\n", datosCeluCompra.cantidadDebitos, " ",
            valor);
    pprintf(lineaSalida);
    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    formatString(0x00, datosCeluCompra.totalAnuladas, "", 0, valor, 1);
    memset(valorAnulacion, 0x00, sizeof(valorAnulacion));
    if (datosCeluCompra.cantidadAnuladas > 0) {
        strcat(valorAnulacion, "-");
    }
    strcat(valorAnulacion, valor);
    sprintf(lineaSalida, "ANULACIONES  : %3d%16s%13s\n", datosCeluCompra.cantidadAnuladas, " ",
            valorAnulacion);
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "%s", "-----------------------------------------------\n");
    pprintf(lineaSalida);
}

void cierreQrcode(void) {

    uChar lineaSalida[48 + 1];
    uChar valorAnulacion[48 + 1];
    uChar valor[48 + 1];
    TablaUnoInicializacion tablaUno;

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    memset(&tablaUno, 0x00, sizeof(tablaUno));

    tablaUno = _desempaquetarTablaCeroUno_();

    pprintf("        CODIGO QR\n");

    formatString(0x00, datosQrcode.totalCompraNeta, "", 0, valor, 1);
    sprintf(lineaSalida, "COMPRA       : %3d%16s%13s\n", datosQrcode.cantidadDebitos, " ", valor);
    pprintf(lineaSalida);
    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    formatString(0x00, datosQrcode.totalIva, "", 0, valor, 1);
    sprintf(lineaSalida, "IVA          : %19s%13s\n", " ", valor);
    pprintf(lineaSalida);
    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    formatString(0x00, datosQrcode.totalInc, "", 0, valor, 1);
    sprintf(lineaSalida, "INC          : %19s%13s\n", " ", valor);
    pprintf(lineaSalida);
    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    formatString(0x00, datosQrcode.totalPropina, "", 0, valor, 1);

    if (tablaUno.options1.tipProcessing == 1) {
        sprintf(lineaSalida, "PROPINA      : %19s%13s\n", " ", valor);
        pprintf(lineaSalida);
    }

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "%s", "-----------------------------------------------\n");
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    formatString(0x00, datosQrcode.totalDebitos, "", 0, valor, 1);
    sprintf(lineaSalida, "TOTAL        : %3d%16s%13s\n", datosQrcode.cantidadDebitos, " ", valor);
    pprintf(lineaSalida);
    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    formatString(0x00, datosQrcode.totalAnuladas, "", 0, valor, 1);
    memset(valorAnulacion, 0x00, sizeof(valorAnulacion));
    if (datosQrcode.cantidadAnuladas > 0) {
        strcat(valorAnulacion, "-");
    }
    strcat(valorAnulacion, valor);
    sprintf(lineaSalida, "ANULACIONES  : %3d%16s%13s\n", datosQrcode.cantidadAnuladas, " ",
            valorAnulacion);
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "%s", "-----------------------------------------------\n");
    pprintf(lineaSalida);

}

void cierreConsultaEstado(void) {

    uChar lineaSalida[48 + 1];
    uChar valorAnulacion[48 + 1];
    uChar valor[48 + 1];

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));

    pprintf("    CONSULTA ESTADO\n");

    formatString(0x00, datosConsultaEstado.totalCompraNeta, "", 0, valor, 1);
    sprintf(lineaSalida, "COMPRA       : %3d%16s%13s\n", datosConsultaEstado.cantidadDebitos, " ",
            valor);
    pprintf(lineaSalida);
    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    formatString(0x00, datosConsultaEstado.totalIva, "", 0, valor, 1);
    sprintf(lineaSalida, "IVA          : %19s%13s\n", " ", valor);
    pprintf(lineaSalida);
    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    formatString(0x00, datosConsultaEstado.totalInc, "", 0, valor, 1);
    sprintf(lineaSalida, "INC          : %19s%13s\n", " ", valor);
    pprintf(lineaSalida);
    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    formatString(0x00, datosConsultaEstado.totalPropina, "", 0, valor, 1);
    sprintf(lineaSalida, "PROPINA      : %19s%13s\n", " ", valor);
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "%s", "-----------------------------------------------\n");
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    formatString(0x00, datosConsultaEstado.totalDebitos, "", 0, valor, 1);
    sprintf(lineaSalida, "TOTAL        : %3d%16s%13s\n", datosConsultaEstado.cantidadDebitos, " ",
            valor);
    pprintf(lineaSalida);
    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    formatString(0x00, datosConsultaEstado.totalAnuladas, "", 0, valor, 1);
    memset(valorAnulacion, 0x00, sizeof(valorAnulacion));
    if (datosConsultaEstado.cantidadAnuladas > 0) {
        strcat(valorAnulacion, "-");
    }
    strcat(valorAnulacion, valor);
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "%s", "-----------------------------------------------\n");
    pprintf(lineaSalida);

}

void imprimirReporteAuditoria(void) {
    int idx = 0;
    int idy = 0;
    int cantidadVentas = 0;
    long totalVentas = 0;
    int granCantidadVentas = 0;
    long granTotalVentas = 0;
    DatosVenta datosVenta;
    uChar lineaSalida[50 + 1];
    uChar terminal[10 + 1];
    uChar valor[20 + 1];
    uChar formatoValor[20 + 1];
    uChar signo[1 + 1];
    int totalTransacciones;
    long tam;
    int tamJournal;
    int maximoId;
    TablaUnoInicializacion tablaUno;
    uChar valorVenta[20 + 1];
    uChar typeTransaccion[2 + 1];
    DatosBasicosCierre basicoCierre;


    memset(&tablaUno, 0x00, sizeof(tablaUno));
    memset(&basicoCierre, 0x00, sizeof(basicoCierre));
    tam = tamArchivo(discoNetcom, JOURNAL);
    tamJournal = sizeof(DatosVenta);
    restarTamAjusteLealtadUltimoRecibo(&tam);
    maximoId = tam / tamJournal;

    if (maximoId > 0) {
        memset(terminal, 0x00, sizeof(terminal));
        getParameter(NUMERO_TERMINAL, terminal);
        //logo
        //imprimirEncabezado();SI VA PENDIENTE

        tablaUno = _desempaquetarTablaCeroUno_();
        memset(lineaSalida, 0x00, sizeof(lineaSalida));
        sprintf(lineaSalida, "%s\n", tablaUno.defaultMerchantName); // nombre comercio
        pprintf(lineaSalida);
        memset(lineaSalida, 0x00, sizeof(lineaSalida));
        sprintf(lineaSalida, "%s\n", tablaUno.receiptLine2); // direccion comercio
        pprintf(lineaSalida);
        sprintf(lineaSalida, "TERMINAL: %s\n", terminal);
        pprintf(lineaSalida);

        memset(lineaSalida, 0x00, sizeof(lineaSalida));
        sprintf(lineaSalida, "%s", "  REPORTE DE AUDITORIA\n");
        pprintf(lineaSalida);

        calcularCierreVenta(maximoId, 1); //Totaliza anuladas offline

        for (idx = 0; idx < countFieldGrupo; idx++) {
            cantidadVentas = 0;
            totalVentas = 0;

            memset(lineaSalida, 0x00, sizeof(lineaSalida));
            sprintf(lineaSalida, "%s\n", datosGrupo[idx].nombreGrupo);
            pprintf(lineaSalida);

            memset(lineaSalida, 0x00, sizeof(lineaSalida));
            sprintf(lineaSalida, "%s", "Tarjeta Recibo VlrCompra\n");
            pprintf(lineaSalida);

            totalTransacciones = maximoId;

            for (idy = 0; idy < totalTransacciones; idy++) {
                memset(&datosVenta, 0x00, sizeof(datosVenta));
                memset(signo, 0x00, sizeof(signo));
                memset(typeTransaccion, 0x00, sizeof(typeTransaccion));
                datosVenta = buscarReciboByID(idy);

                switch (datosVenta.isQPS[0]) {
                    case '1': // Venta Qps
//					typeTransaccion = 'Q';
                        sprintf(typeTransaccion, "%s", "Q");
                        break;
                    case 'P': // Venta PayPass
//					typeTransaccion = "PP";
                        sprintf(typeTransaccion, "%s", "PP");
                        break;
                    case 'V': // Venta PayWave
//					typeTransaccion = 'PW';
                        sprintf(typeTransaccion, "%s", "PW");
                        break;
                    default: // Venta Default
//					typeTransaccion = ' ';
                        sprintf(typeTransaccion, "%s", " ");
                        break;
                }

                if (IS_MULTIPOS(atoi(datosVenta.tipoTransaccion)) == 0
                    && IS_OTRAS_AUDITORIAS(atoi(datosVenta.tipoTransaccion)) ==
                       0) { // Las transacciones de multipos no suman en este cierre

                    if (strncmp(datosGrupo[idx].codigoGrupo, datosVenta.codigoGrupo, TAM_GRUPO) ==
                        0) {
                        memset(valor, 0x00, sizeof(valor));
                        memcpy(valor, datosVenta.totalVenta, strlen(datosVenta.totalVenta) - 2);

                        if (strncmp(datosVenta.estadoTransaccion, "1", 1) == 0) {
                            cantidadVentas++;
                            totalVentas += atol(valor);
                            strcpy(signo, " ");
                        } else {
                            strcpy(signo, "-");
                        }
                        memset(lineaSalida, 0x00, sizeof(lineaSalida));
                        memset(formatoValor, 0x00, sizeof(formatoValor));
                        memset(valorVenta, 0x00, sizeof(valorVenta));
                        formatString(0x00, -1, valor, strlen(valor), formatoValor, 1);
                        sprintf(valorVenta, "%s%s", signo, formatoValor);
                        sprintf(lineaSalida, "**%4s%10s%6s  %2s%8s%13s\n", datosVenta.ultimosCuatro,
                                " ",
                                datosVenta.numeroRecibo, typeTransaccion, " ", valorVenta);
                        pprintf(lineaSalida);

                    }
                }
            }
            granCantidadVentas += cantidadVentas;
            granTotalVentas += totalVentas;
            memset(formatoValor, 0x00, sizeof(formatoValor));
            formatString(0x00, totalVentas, "", 0, formatoValor, 1);
            memset(lineaSalida, 0x00, sizeof(lineaSalida));

            memset(lineaSalida, 0x00, sizeof(lineaSalida));
            sprintf(lineaSalida, "%s", "-----------------------------------------------\n");
            pprintf(lineaSalida);

            sprintf(lineaSalida, "TOTAL%2s%04d%13s\n", " ", cantidadVentas, formatoValor);
            pprintf(lineaSalida);

            memset(lineaSalida, 0x00, sizeof(lineaSalida));
            sprintf(lineaSalida, "%s", "-----------------------------------------------\n");
            pprintf(lineaSalida);

        }

        if (totalTransaccionesGASOPASS > 0) {
            memset(&basicoCierre, 0x00, sizeof(basicoCierre));
            //cierreGasoPass(2, &basicoCierre);
            granCantidadVentas += basicoCierre.cantidadVenta;
            granTotalVentas += atol(basicoCierre.datos.totalVenta);

        }

        if (totalTransaccionesTarjetaMiCompra > 0) {
            memset(&basicoCierre, 0x00, sizeof(basicoCierre));
            basicoCierre.datos.tipoPagoDivididas = 2;
            //netcomServices(APP_TYPE_TARJETA_MI_COMPRA, CIERRE_MI_COMPRA, sizeof(basicoCierre), &basicoCierre); //OJO MIGRACION
            basicoCierre.datos.tipoPagoDivididas = 0;
            granCantidadVentas += basicoCierre.cantidadVenta;
            granTotalVentas += atol(basicoCierre.datos.totalVenta);

        }

        memset(formatoValor, 0x00, sizeof(formatoValor));
        formatString(0x00, granTotalVentas, "", 0, formatoValor, 1);
        memset(lineaSalida, 0x00, sizeof(lineaSalida));
        pprintf("GRAN TOTAL\n");

        memset(lineaSalida, 0x00, sizeof(lineaSalida));
        sprintf(lineaSalida, "%s", "-----------------------------------------------\n");
        pprintf(lineaSalida);
        sprintf(lineaSalida, "TOTAL%2s%04d%13s\n", " ", granCantidadVentas, formatoValor);
        pprintf(lineaSalida);

        memset(lineaSalida, 0x00, sizeof(lineaSalida));
        sprintf(lineaSalida, "%s", "-----------------------------------------------\n");
        pprintf(lineaSalida);
        if (totalComerciosMultiPos > 0) {
            //cierreMultiPos(2, datosCierreComercios, totalComerciosMultiPos, 0);
            limpiarDatosMultiPos();
        }

    } else {
        //screenMessage("CIERRE", "NO EXISTEN", "TRANSACCIONES", "PARA CIERRE", 2 * 1000);
    }

}

int _reporteCierre_(int tipoReporte) {

    long tam = 0;
    int tamJournal = 0;
    int maximoId = 0;
    uChar *buffer = NULL;
    int iRet = 0;

    tam = tamArchivo(discoNetcom, DIRECTORIO_JOURNALS);

    if (tam > 0) {
        tam = tamArchivo(discoNetcom, JOURNAL);
        tamJournal = sizeof(DatosVenta);
        restarTamAjusteLealtadUltimoRecibo(&tam);
        maximoId = tam / tamJournal;

        if (tipoReporte == 1) {

            if (maximoId > 0) {
                calcularCierreVenta(maximoId, 1); //totaliza anuladas offline
                LOGI("imprimirReporteCierreVentas D");
                imprimirReporteCierreVentas(TRUE); //TOTALES
                pprintf((char *) "final");
                iRet = 1;

            } else {
                iRet = 0;
                //screenMessage("MENSAJE", "SIN TRANSACCIONES", "REPORTE NO", "PERMITIDO", T_OUT(2));
            }

            //netcomServices(APP_TYPE_MULTICORRESPONSAL, REPORTE_MULTICORRESPONSAL, 1, &buffer);

        }

        if (tipoReporte == 2) {
            if (maximoId > 0) {
                imprimirReporteAuditoria();
                pprintf((char *) "final");
                iRet = 1;
            } else {
                iRet = 0;
                //screenMessage("MENSAJE", "SIN TRANSACCIONES", "REPORTE NO", "PERMITIDO", T_OUT(2));
            }
        }

    } else {
        iRet = 0;
        //screenMessage("MENSAJE", "SIN TRANSACCIONES", "REPORTE NO", "PERMITIDO", 3 * 1000);
    }
    return iRet;
}

void limpiarDatos(void) {
    int idx = 0;
    int idy = 0;
    countFieldGrupo = 0;
    for (idx = 0; idx < MAX_GRUPO; idx++) {
        memset(&datosGrupo[idx], 0x00, sizeof(datosGrupo[idx]));
        for (idy = 0; idy < 10; idy++) {
            memset(&datosGrupo[idx].datosSubgrupo[idy], 0x00,
                   sizeof(datosGrupo[idx].datosSubgrupo[idy]));
        }

    }

    memset(&datosBonos, 0x00, sizeof(datosBonos));
    memset(&datosPCR, 0x00, sizeof(datosPCR));
    memset(&datosRecargaCelular, 0x00, sizeof(datosRecargaCelular));
    memset(&totalesRecarga, 0x00, sizeof(totalesRecarga));
    memset(&datosPSP, 0x00, sizeof(datosPSP));
    memset(&datosDineroElectronico, 0x00, sizeof(datosDineroElectronico));
    memset(&datosAvanceEfectivo, 0x00, sizeof(datosAvanceEfectivo));
    memset(&datosPagoImpuestos, 0x00, sizeof(datosPagoImpuestos));
    memset(&datosCeluCompra, 0x00, sizeof(datosCeluCompra));
    memset(&datosQrcode, 0x00, sizeof(datosQrcode));
    memset(&datosConsultaEstado, 0x00, sizeof(datosConsultaEstado));
    memset(&tarjetaPrivada, 0x00, sizeof(tarjetaPrivada));
    memset(&tarjetaPrivadaEfectivo, 0x00, sizeof(tarjetaPrivadaEfectivo));
    memset(&tarjetaPrivadaCombinado, 0x00, sizeof(tarjetaPrivadaEfectivo));
    memset(&totalesTarjetaPrivada, 0x00, sizeof(totalesTarjetaPrivada));
    memset(&totalesPaypass, 0x00, sizeof(totalesPaypass));
    memset(&totalesPayWave, 0x00, sizeof(totalesPayWave));
    memset(&recaudoCNBAval, 0x00, sizeof(recaudoCNBAval));
    memset(&datosBigBancolombia, 0x00, sizeof(datosBigBancolombia));
    memset(&datosLealtadBig, 0x00, sizeof(datosLealtadBig));
    memset(&datosFocoSodexo, 0x00, sizeof(datosFocoSodexo));
    memset(&datosPuntosColombia, 0x00, sizeof(datosPuntosColombia));
    memset(&datosLealtad, 0x00, sizeof(datosLealtad));
    memset(&datosPColombia, 0x00, sizeof(datosPColombia));

    totalTransaccionesDineroElectronico = 0;
    totalTransaccionesDineroElectronicoRetiro = 0;
    totalTransaccionesGASOPASS = 0;
    totalTransaccionesTarjetaMiCompra = 0;

}

void ordenarGrupoFranquicia(void) {

    int idx = 0;
    int idy = 0;
    DatosGrupo temp;

    if (countFieldGrupo > 1) {
        for (idx = 1; idx < countFieldGrupo; idx++) {
            for (idy = 0; idy < countFieldGrupo - 1; idy++) {
                if (atoi(datosGrupo[idy].codigoGrupo) > atoi(datosGrupo[idy + 1].codigoGrupo)) {
                    memset(&temp, 0x00, sizeof(temp));
                    memcpy(&temp, &datosGrupo[idy], sizeof(datosGrupo[idy]));
                    memset(&datosGrupo[idy], 0x00, sizeof(datosGrupo[idy]));
                    memcpy(&datosGrupo[idy], &datosGrupo[idy + 1], sizeof(datosGrupo[idy + 1]));
                    memset(&datosGrupo[idy + 1], 0x00, sizeof(datosGrupo[idy + 1]));
                    memcpy(&datosGrupo[idy + 1], &temp, sizeof(datosGrupo[idy + 1]));
                }
            }
        }

    }

}

void totalizarRecargaCelular(uChar *plataforma, uChar *operador, long totalVenta) {

    uChar stringAux[4 + 1];
    int tipoPalaforma = 0;

    memset(stringAux, 0x00, sizeof(stringAux));

    _convertBCDToASCII_(stringAux, operador, 4);

    tipoPalaforma = atoi(plataforma);

    switch (tipoPalaforma) {

        case 0:
            break;
        case 1:
            totalizarRecargaCelularEfectivo(stringAux, totalVenta);
            break;
        case 2:
            totalizarRecargaCelularTarjeta(stringAux, totalVenta);
            break;
        default:
            break;
    }

}

void totalizarRecargaCelularEfectivo(uChar *operador, long totalVenta) {

    totalesRecarga.cantidadRecargaCelularEfectivo++;
    totalesRecarga.totalRecargaCelularEfectivo += totalVenta;

}

void totalizarRecargaCelularTarjeta(uChar *operador, long totalVenta) {

    int tipoOperador = 0;

    tipoOperador = atoi(operador);

    switch (tipoOperador) {

        case TARJETA_MOVISTAR:
            totalesRecarga.cantidadMovistar++;
            totalesRecarga.totalMovistar += totalVenta;
            break;

        case TARJETA_COMCEL:
            totalesRecarga.cantidadComcel++;
            totalesRecarga.totalComcel += totalVenta;
            break;

        case TARJETA_TIGO:
            totalesRecarga.cantidadTigo++;
            totalesRecarga.totalTigo += totalVenta;
            break;

        case TARJETA_UFF:
            totalesRecarga.cantidadUff++;
            totalesRecarga.totalUff += totalVenta;
            break;

        case TARJETA_DIRECTV:
            totalesRecarga.cantidadDirectv++;
            totalesRecarga.totalDirectv += totalVenta;
            break;

        default:
            break;
    }

}

void llenarDatosComercio(DatosVenta datosVenta) {

    int indiceComercio = 0;

    ///////// SE BUSCA EL COMERCIO EN LA ESTRUCTURA /////////////
    indiceComercio = buscarComercioMultiPos(datosVenta.codigoComercio);

    //////// SI NO EXISTE ENTONCES SE AUMENTA EL CONTADOR Y SE GUARDA /////////
    if (indiceComercio == -1) {
        indiceComercio = totalComerciosMultiPos;
        totalComerciosMultiPos++;
    }

    ////////// SE GUARDAN LOS DATOS DEL COMERCIO /////////////
    memcpy(datosCierreComercios[indiceComercio].codigoComercio, datosVenta.codigoComercio,
           TAM_ID_COMERCIO);
    memcpy(datosCierreComercios[indiceComercio].nombreComercio, datosVenta.nombreComercio,
           TAM_COMERCIO);
}

int buscarComercioMultiPos(char *idComercio) {

    int resultado = -1;
    int i = 0;

    for (i = 0; i < totalComerciosMultiPos; i++) {
        if (strcmp(datosCierreComercios[i].codigoComercio, idComercio) == 0) {
            resultado = i;
            break;
        }
    }

    return resultado;
}

void limpiarDatosMultiPos(void) {

    int idx = 0;

    for (idx = 0; idx < MAX_COMERCIOS_MULTIPOS; idx++) {
        memset(&datosCierreComercios[idx], 0x00, sizeof(datosCierreComercios[idx]));
    }

    totalComerciosMultiPos = 0;
}

/**
 * @brief Funcion encargada de imprimir en el reporte de cierre
 * 	      los totales de las ventas realizadas por PayPass.
 * @author
 */
void imprimirTotalesPaypass(void) {

    uChar valor[15 + 1];
    uChar lineaSalida[48 + 1];

    memset(valor, 0x00, sizeof(valor));
    memset(lineaSalida, 0x00, sizeof(lineaSalida));

    if (totalesPaypass.cantidadVentaDebito > 0) {

        pprintf("     TOTALES PAYPASS        \n");

        formatString(0x00, totalesPaypass.totalVentaDebito, "", 0, valor, 1);
        sprintf(lineaSalida, "PAYPASS       : %3d%16s%13s\n", totalesPaypass.cantidadVentaDebito,
                " ", valor);

        pprintf(lineaSalida);
    }

}

/**
 * @brief Funcion encargada de imprimir en el reporte de cierre
 * 	      los totales de las ventas realizadas por PayWave.
 * @author
 */
void imprimirTotalesPayWave(void) {

    uChar valor[15 + 1];
    uChar lineaSalida[48 + 1];

    memset(valor, 0x00, sizeof(valor));
    memset(lineaSalida, 0x00, sizeof(lineaSalida));

    if (totalesPayWave.cantidadVentaDebito > 0) {

        pprintf("     TOTALES PAYWAVE        \n");

        formatString(0x00, totalesPayWave.totalVentaDebito, "", 0, valor, 1);
        sprintf(lineaSalida, "PAYWAVE       : %3d%16s%13s\n", totalesPayWave.cantidadVentaDebito,
                " ", valor);

        pprintf(lineaSalida);
    }

}

void restarTamAjusteLealtadUltimoRecibo(long *tam) {

    DatosVenta datosLealtad;

    memset(&datosLealtad, 0x00, sizeof(datosLealtad));

    buscarArchivo(discoNetcom, JOURNAL, (char *) &datosLealtad, *tam - TAM_JOURNAL_COMERCIO,
                  TAM_JOURNAL_COMERCIO);

    if (atoi(datosLealtad.tipoTransaccion) == TRANSACCION_AJUSTAR_LEALTAD
        && atoi(datosLealtad.estadoTransaccion) == 0) {
        *tam -= TAM_JOURNAL_COMERCIO;
    }

}

void invocarMenuBig(char *convenio, char *fiid, int idx) {

    char dataBig[36 + 1];
    char fiidAux[2 + 1];
    int posicion = 0;
    int indice = 5;

    memset(&dataBig, 0x00, sizeof(dataBig));
    memset(fiidAux, 0x00, sizeof(fiidAux));

    posicion = idx * 34;

    buscarArchivo(discoNetcom, TX_BIG_BANCOLOMBIA, dataBig, posicion, 34);

    memcpy(convenio, dataBig + indice, 16);
    indice += (19);
    memcpy(fiidAux, dataBig + indice, 2);

    _convertBCDToASCII_(fiid, fiidAux, 4);

}

int cierreCorresponsales() {

    uChar *buffer = NULL;
    char lineaResumen[100 + 1]= {0x00};
    int iRet = 1;

    if (verificarArchivo(discoNetcom, JOURNAL_CNB_RBM) == FS_OK) {
        // screenMessage("CIERRE", "REALIZANDO", "CIERRE", "CORRESPONSAL", 2 * 1000);
        // iRet = netcomServices(APP_TYPE_CNBRBM, CIERRE_CNB_RBM, 1, &buffer);
    }

    if (verificarArchivo(discoNetcom, JOURNAL_CNB_BANCOLOMBIA) == FS_OK && iRet > 0) {
        screenMessageFiveLine("CIERRE", "REALIZANDO", "CIERRE", "CORRESPONSAL",""," ",lineaResumen);
        iRet = cierreCnbTerminalBancolombia();
        //iRet = cierreCnbBancolombia(1);

    }

    if (verificarArchivo(discoNetcom, JOURNAL_CNB_CORRESPONSALES) == FS_OK && iRet > 0) {
        //screenMessage("CIERRE", "REALIZANDO", "CIERRE", "CORRESPONSAL", 2 * 1000);
        //iRet = netcomServices(APP_TYPE_CNB_CORRESPONSALES, CIERRE_CNB_CORRESPONSAL, 0, buffer);
    }

    if (verificarArchivo(discoNetcom, JOURNAL_MCNB_AVAL) == FS_OK) {
        //screenMessage("CIERRE", "REALIZANDO", "CIERRE", "CORRESPONSAL", 2 * 1000);
        //iRet = netcomServices(CIERRE_MULTICORRESPONSAL, CIERRE_CNB_RBM, 1, &buffer);
    }

    if (iRet > 0) {
        borrarArchivo(discoNetcom, DIRECTORIO_JOURNALS);
    }

    return iRet;
}