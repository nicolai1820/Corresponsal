//
// Created by NETCOM on 25/02/2020.
//

#include <string>
#include <stdio.h>
#include <Utilidades.h>
#include <Mensajeria.h>
#include <configuracion.h>
#include <bancolombia.h>
#include <Inicializacion.h>
#include <Archivos.h>
#include <native-lib.h>
#include "Tipo_datos.h"
#include "comunicaciones.h"
#include "android/log.h"

#define  LOG_TAG    "NETCOM RETIRO_BANCOLOMBIA"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
/* ========================================================================	*/
/* Variables Globales                                                       */
/* ========================================================================	*/
char bufferAuxiliarCustomEMV[1024 + 1];
int tipoRetiroCaja = 0;
int modoJMR = 0;
int cajaIP = 0;


int enviarTransaccionRetiroTarjetaBCL(DatosTarjetaAndroid datosTarjetaAndroid, int intentosPin, char * codigoRespuesta, char* tipoCuenta, char * cuentaDestino,char * monto, int otraCuenta){

    int resultado = 0;
    int tamPan = 0;
    uChar pan[20 + 1] = {0x00};
    char ultimos4[4 + 1] = {0x00};
    DatosTarjeta datosTarjeta;
    TablaTresInicializacion dataIssuerEMV;

    memset(&datosTarjeta, 0x00, sizeof(datosTarjeta));
    memset(&dataIssuerEMV, 0x00, sizeof(dataIssuerEMV));

    memcpy(datosVentaBancolombia.track2, datosTarjetaAndroid.track2, strlen(datosTarjetaAndroid.track2));
    memcpy(datosVentaBancolombia.tipoCuenta, datosTarjetaAndroid.tipoCuenta, 2);
    memcpy(datosVentaBancolombia.totalVenta, monto, 12);

    if(otraCuenta == 1){
        leftPad(datosVentaBancolombia.tokenVivamos, cuentaDestino, '0', SIZE_CUENTA);
        sprintf(datosVentaBancolombia.numeroRecibo2, "%02d", RETIRO_OTRA_CTA);
    }
    LOGI("monto %s ", monto);
    if((datosTarjetaAndroid.sizeP55) > 0){
        _convertASCIIToBCD_(globalCampo55,datosTarjetaAndroid.data55,strlen(datosTarjetaAndroid.data55));
        memcpy(datosVentaBancolombia.posEntrymode, (char*)"051", 3);
        memcpy(datosVentaBancolombia.AID, datosTarjetaAndroid.aid, strlen(datosTarjetaAndroid.aid));
        memcpy(datosVentaBancolombia.ARQC, datosTarjetaAndroid.arqc,strlen(datosTarjetaAndroid.arqc));
        _convertASCIIToBCD_(datosVentaBancolombia.appLabel,datosTarjetaAndroid.apLabel,strlen(datosTarjetaAndroid.apLabel));

        dataIssuerEMV = traerIssuerCNB(pan, datosVentaBancolombia.track2);
        LOGI("pan 2 %s ", pan);

        memcpy(datosVentaBancolombia.ultimosCuatro, (pan + strlen(pan)) - 4, 4);
        LOGI("ultimos 4 %s ", datosVentaBancolombia.ultimosCuatro);
        comportamientoTarjetaLabelEMV(&datosVentaBancolombia, dataIssuerEMV);
        globalsizep55 = strlen(datosTarjetaAndroid.data55)/2;
    } else {
        LOGI("pan andride  %s ", datosTarjetaAndroid.pan);
        tamPan = strlen(datosTarjetaAndroid.pan);
        memcpy(ultimos4, datosTarjetaAndroid.pan + tamPan - 4, 4);
        memcpy(datosVentaBancolombia.ultimosCuatro, ultimos4, 4 );
        //datosTarjeta = detectarTrack2(datosTarjetaAndroid.track2);
        LOGI("salio   %s ", datosTarjeta.pan);
        //validarFallBack(&datosVentaBancolombia, &datosTarjeta);
    }
    LOGI("is pin  %d ", datosTarjetaAndroid.isPin);
    if(datosTarjetaAndroid.isPin > 0){
        _convertASCIIToBCD_(globalpinblock,datosTarjetaAndroid.pinblock,strlen(datosTarjetaAndroid.pinblock));
    }

    getTerminalId(datosVentaBancolombia.terminalId);
    sprintf(datosVentaBancolombia.processingCode, "41%2s00", datosVentaBancolombia.tipoCuenta);

    sprintf(datosVentaBancolombia.tipoTransaccion, "%02d", TRANSACCION_BCL_RETIRO);

    resultado = enviarRetiro(TRANSACCION_TEFF_COMPRAS);

    if (resultado == -4) {
        resultado = 0;
        intentosPin = 1;
    } else if (resultado == -3) {
        resultado = -1;
    } else {
        intentosPin = 0;
    }

    if(resultado > 0  && datosTarjetaAndroid.sizeP55 == 0){
        LOGI("Aprobado  EMV  %s ", datosVentaBancolombia.codigoAprobacion);
        mostrarAprobacionBancolombia(&datosVentaBancolombia);
    } else if(resultado > 0 && datosTarjetaAndroid.sizeP55 > 0){
        preAprobacionEmv();
    }

    setParameter(TIPO_INICIA_TRANSACCION, (char *) "00");
    setVariableGlobalesRetiro();

    return resultado;
}

int retiroEfectivo(char * monto, char *cuenta, char * pinBlock) {

    int resultado = 0;
    int intentosPin = 0;
    DatosTarjetaSembrada datosTarjetaSembrada;
    uChar stringAux[20 + 1];
    int resultadoCaja = 0;
//	int tipoCuentaCaja = 0;
    uChar pan[19 + 1];

    if (modoJMR == FALSE) {
        memset(&datosVentaBancolombia, 0x00, sizeof(datosVentaBancolombia));
    }

    setVariableGlobalesRetiro();
    memset(stringAux, 0x00, sizeof(stringAux));
    memset(pan, 0x00, sizeof(pan));
    memset(&datosTarjetaSembrada, 0x30, sizeof(datosTarjetaSembrada));

    memcpy(datosVentaBancolombia.totalVenta,monto, 12);
    leftPad(datosVentaBancolombia.tokenVivamos,cuenta,'0',SIZE_CUENTA);
    LOGI("numero de cuenta %s",datosVentaBancolombia.tokenVivamos);
    resultadoCaja = verificarHabilitacionCaja();
    memset(datosVentaBancolombia.numeroRecibo2, 0x00, sizeof(datosVentaBancolombia.numeroRecibo2));
    sprintf(datosVentaBancolombia.numeroRecibo2, "%02d", RETIRO_EFECTIVO);
    memset(globalpinblock,0x00, sizeof(globalpinblock));

    _convertASCIIToBCD_(globalpinblock,pinBlock,strlen(pinBlock));

    //memcpy(globalpinblock, pinBlock, strlen(pinBlock));
    if(resultadoCaja == TRUE) {
        /* if(verificarCajaIPCB() == 1) {
             resultadoCaja = 0;
         }*/
    }

    if (resultadoCaja == TRUE && modoJMR == FALSE ) {

        datosTarjetaSembrada = cargarTarjetaSembrada();

        //resultado = transaccionPeticionDatosCaja(&datosVentaBancolombia, TRANSACCION_TEFF_RETIRO_EFECTIVO);
        if (resultado > 0) {
//			tipoCuentaCaja = atol(datosVentaBancolombia.estadoTarifa);
            memset(datosVentaBancolombia.numeroRecibo2, 0x00, sizeof(datosVentaBancolombia.numeroRecibo2));
            sprintf(datosVentaBancolombia.numeroRecibo2, "%02d", RETIRO_EFECTIVO);
            if (resultado > 0) {
                // resultado = confirmarRetiro(datosVentaBancolombia.totalVenta, RETIRO_EFECTIVO,datosVentaBancolombia.tokenVivamos);
            }

            if (resultado <= 0) {
                /*screenMessage("MENSAJE", "CANCELADO", "POR USUARIO", "", 2 * 1000);*/
                if (verificarHabilitacionCaja() == TRUE) {
                    //reponderCajaRetirosDeclinados(TRANSACCION_TEFF_RETIRO_SIN_TARJETA);
                }
                imprimirDeclinadas(datosVentaBancolombia, "CANCELADO POR USUARIO");
            }
        }
    } else {
        resultado = 1;//temporal Android
        if (resultado <= 0) {
            sprintf(datosVentaBancolombia.tipoTransaccion, "%02d", TRANSACCION_BCL_RETIRO_EFECTIVO);
            if (verificarHabilitacionCaja() == TRUE) {
                //reponderCajaRetirosDeclinados(TRANSACCION_TEFF_RETIRO_SIN_TARJETA);
            }
            imprimirDeclinadas(datosVentaBancolombia, "CANCELADO POR USUARIO");
        }
    }
    if (resultado > 0) {
        sprintf(datosVentaBancolombia.tipoCuenta, "%02d", 10);

        obtenerTerminalModoCorresponsal(datosVentaBancolombia.terminalId);
        sprintf(datosVentaBancolombia.processingCode, "41%2s00", datosVentaBancolombia.tipoCuenta);
        strcpy(datosVentaBancolombia.posEntrymode, "021");
        sprintf(datosVentaBancolombia.tipoTransaccion, "%02d", TRANSACCION_BCL_RETIRO_EFECTIVO);

        datosTarjetaSembrada = cargarTarjetaSembrada();

        do {
            ///// PARA LOS RETIROS, SIEMPRE SE DEBE SOLICITAR PIN
            if (intentosPin > 0) {
                memcpy(stringAux, datosVentaBancolombia.totalVenta, strlen(datosVentaBancolombia.totalVenta) - 2);
                memset(datosVentaBancolombia.totalVenta, 0x00, sizeof(datosVentaBancolombia.totalVenta));
                memcpy(datosVentaBancolombia.totalVenta, stringAux, strlen(stringAux));
            }

            memcpy(pan, datosVentaBancolombia.tokenVivamos + 1, 10);
            LOGI("numero de pan %s",pan);
            armarTrack(datosVentaBancolombia.track2, pan, atoi(datosVentaBancolombia.tipoTransaccion));
            memset(pan, 0x00, sizeof(pan));
            memcpy(pan, datosVentaBancolombia.track2, 16);

            //resultado = capturarPinCnbBancolombia(globalpinblock, pan, datosTarjetaSembrada.cardName,datosVentaBancolombia.totalVenta);

            if (resultado <= 0) {
                //screenMessage("MENSAJE", "CANCELADO", "POR USUARIO", "", 2 * 1000);
                if (verificarHabilitacionCaja() == TRUE) {
                    //reponderCajaRetirosDeclinados(TRANSACCION_TEFF_RETIRO_SIN_TARJETA);
                }
                imprimirDeclinadas(datosVentaBancolombia, "CANCELADO POR USUARIO");
            }

            if (resultado > 0) {
                LOGI("  Entro de enviar retiro");
                resultado = enviarRetiro(TRANSACCION_TEFF_RETIRO_SIN_TARJETA);
                LOGI("  Salio de enviar retiro");
            }

            if (resultado == -4) {
                resultado = 0;
                intentosPin = 1;
            } else if (resultado == -3) {
                resultado = -1;
            } else {
                intentosPin = 0;
            }

        } while (resultado > 0 && intentosPin == 1);
    }

    if ((resultado > 0)) {

        ///// SIEMPRE VA A TOMAR ESTE TIPO DE TRANSACCION PARA QUE NO AFECTE EN EN DETALLE DEL CIERRE
        memset(datosVentaBancolombia.tipoTransaccion, 0x00, sizeof(datosVentaBancolombia.tipoTransaccion));
        sprintf(datosVentaBancolombia.tipoTransaccion, "%02d", TRANSACCION_BCL_RETIRO);

        if (verificarHabilitacionCaja() == TRUE) {
            memcpy(datosVentaBancolombia.codigoRespuesta, "00", 2);
            setParameter(USO_CAJA_REGISTRADORA, "1");
            //resultado = transaccionRespuestaDatosCaja(datosVentaBancolombia, TRANSACCION_TEFF_RETIRO_SIN_TARJETA);
            if (resultado < 0 /*&& verificarCajaIPCB() == 1*/) {
                memcpy(datosVentaBancolombia.codigoRespuesta, "WS", 2);
                imprimirDeclinadas(datosVentaBancolombia, "ERROR CON EL WS");
            }
        }
        mostrarAprobacionBancolombia(&datosVentaBancolombia);
    }
    setParameter(TIPO_INICIA_TRANSACCION, (char *) "00");
    setVariableGlobalesRetiro();
    return resultado;
}

int retiroOtraCuenta(void) {

    int resultado = 0;
    int tipoCuenta = 0;
    int resultadoCaja = 0;

    resultadoCaja = verificarHabilitacionCaja();

    if(resultadoCaja == TRUE){
        /* if(verificarCajaIPCB() == 1){
             resultadoCaja = 0;
         }*/
    }

    //tipoCuenta = seleccionCuentaBanda("", 2, 0, '0', 0);
    if (tipoCuenta > 0) {
        resultado = 1;
    }

    if (resultado > 0 && tipoCuenta > 0) {
        sprintf(datosVentaBancolombia.tipoCuenta, "%02d", tipoCuenta);

        obtenerTerminalModoCorresponsal(datosVentaBancolombia.terminalId);
        sprintf(datosVentaBancolombia.processingCode, "41%2s00", datosVentaBancolombia.tipoCuenta);
    }

    if ((resultadoCaja != TRUE)  || modoJMR == TRUE) {

        if (resultado > 0) {
            //resultado = capturaDatosEfectivo(RETIRO_OTRA_CTA);
        }
    }
    return resultado;
}

/**
 * @brief Muestra el flujo de captura de numero de cta, monto y confirmacion.
 * @param tipoRetiro: Indica quien llamo al flujo, 1 para "efectivo", 2 para "otra cuenta" ( tarjeta )
 * @return 1: OK
 */
int capturaDatosEfectivo(int tipoRetiro) {

    int resultado = 0;
    uChar titulo[15 + 1];
    uChar texto[15 + 1];
    uChar auxiliar[12 + 1];
    uChar cardName[TAM_CARD_NAME + 1];
    uChar numeroCuenta[SIZE_CUENTA];
    uChar flujoUnificado[2 + 1];
    uChar pan[19 + 1];

    memset(pan, 0x00, sizeof(pan));
    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(flujoUnificado, 0x00, sizeof(flujoUnificado));
    memset(titulo, 0x00, sizeof(titulo));
    memset(texto, 0x00, sizeof(texto));
    memset(cardName, 0x00, sizeof(cardName));
    memset(numeroCuenta, 0x00, sizeof(numeroCuenta));

    memset(datosVentaBancolombia.numeroRecibo2, 0x00, sizeof(datosVentaBancolombia.numeroRecibo2));
    sprintf(datosVentaBancolombia.numeroRecibo2, "%02d", tipoRetiro);

    getParameter(FLUJO_UNIFICADO,flujoUnificado);

    if (tipoRetiro == RETIRO_EFECTIVO) {
        strcpy(titulo, "RETIRO EFECTIVO");
        strcpy(texto, "VALOR RETIRO");
    } else {
        strcpy(titulo, "RETIRO");
        strcpy(texto, "INGRESE MONTO");
        strcpy(cardName, datosVentaBancolombia.cardName);
    }

    //////////// REFERENCIA ////////////
    //resultado = capturarDato(titulo, "NUMERO", "DE CUENTA", 1, SIZE_CUENTA, DATO_NUMERICO, 1, numeroCuenta);

    if (resultado > 0) {
        memset(datosVentaBancolombia.tokenVivamos, 0x00, sizeof(datosVentaBancolombia.tokenVivamos));
        leftPad(datosVentaBancolombia.tokenVivamos, numeroCuenta, '0', SIZE_CUENTA);

        //////////// MONTO ////////////
        if (modoJMR == FALSE) {

            memset(datosVentaBancolombia.totalVenta, 0x00,
                   sizeof(datosVentaBancolombia.totalVenta));
            //resultado = capturarMonto("MONTO BASE", texto, cardName, 8, datosVentaBancolombia.totalVenta,"99999999", "1");
        }

    }

    /*if (modoJMR == TRUE && strcmp(flujoUnificado, "1") == 0 && resultado > 0) {
        memcpy(pan, datosVentaBancolombia.tokenVivamos + 1, 10);
        armarTrack(datosVentaBancolombia.track2, pan, TRANSACCION_BCL_RETIRO_EFECTIVO);
        resultado = transaccionPeticionDatosCaja(&datosVentaBancolombia, TRANSACCION_TEFF_RETIRO_ARA);
    }*/

    /*
    if (resultado > 0) {
        resultado = confirmarRetiro(datosVentaBancolombia.totalVenta, tipoRetiro, datosVentaBancolombia.tokenVivamos);
    }*/

    return resultado;
}

void retiroConTarjeta(int leerTarjeta, DatosTarjeta leerTrack, int tipoRetiro) {

    int resultado = 0;
    int intentosPin = 0;
    uChar stringAux[20 + 1];
    int resultadoCaja = 0;
    int tipoCuentaCaja = 0;
    uChar montoAux[TAM_COMPRA_NETA + 1];

    //memset(globalpinblock, 0x00, sizeof(globalpinblock));
    memset(&datosVentaBancolombia, 0x00, sizeof(datosVentaBancolombia));
    memset(stringAux, 0x00, sizeof(stringAux));
    memset(montoAux, 0x00, sizeof(montoAux));
    memcpy(datosVentaBancolombia.codigoRespuesta, "01", 2);

    if (tipoRetiro == RETIRO_CUENTA_PRINCIPAL) {
        memcpy(datosVentaBancolombia.codigoGrupo, OPERACION_RETIRO_PRINCIPAL, 2);
    } else if (tipoRetiro == RETIRO_CUENTA_SECUNDARIA) {
        memcpy(datosVentaBancolombia.codigoGrupo, OPERACION_RETIRO_SECUNDARIA, 2);
    }

    //resultado = procesarTarjeta(leerTarjeta, &leerTrack, TRANSACCION_BCL_RETIRO, &datosVentaBancolombia);

    resultadoCaja = verificarHabilitacionCaja();

    /*if(resultadoCaja == TRUE){
        if(verificarCajaIPCB() == 1){
            resultadoCaja = 0;
        }
    }*/

    if ((resultadoCaja == TRUE) && (resultado > 0)) {


        //resultado = transaccionPeticionDatosCaja(&datosVentaBancolombia, TRANSACCION_TEFF_COMPRAS);
        if (resultado > 0) {
            tipoCuentaCaja = atol(datosVentaBancolombia.estadoTarifa);
            //resultado = capturarDatosCaja(tipoCuentaCaja, leerTrack.dataIssuer.issuerNumber);

            //	procesoCajaActivo = TRUE;
            //se setea codigo de respuesta para caja registradora
            memcpy(datosVentaBancolombia.codigoRespuesta, "01", 2);

            if (resultado <= 0) {
                if (verificarHabilitacionCaja() == TRUE) {
                    //reponderCajaRetirosDeclinados(TRANSACCION_TEFF_COMPRAS);
                }

                imprimirDeclinadas(datosVentaBancolombia, "CANCELADO POR USUARIO");
            }

        }
    } else {
        if (resultado > 0) {
            //resultado = capturarDatos(leerTrack.dataIssuer.issuerNumber);
        }

        if (resultado <= 0 && leerTrack.isChip == 0) {

            if (verificarHabilitacionCaja() > 0 /*&& IDENTIFICAR_ERROR_LECTURA(resultado) == 1*/) {
                //reponderCajaRetirosDeclinados(TRANSACCION_TEFF_COMPRAS);
            }

            imprimirDeclinadas(datosVentaBancolombia, "CANCELADO POR USUARIO");
        }
    }

    if (resultado > 0) {
        do {
            ///// PARA LOS RETIROS, SIEMPRE SE DEBE SOLICITAR PIN
            if (intentosPin > 0) {
                memcpy(stringAux, datosVentaBancolombia.totalVenta, strlen(datosVentaBancolombia.totalVenta) - 2);
                memset(datosVentaBancolombia.totalVenta, 0x00, sizeof(datosVentaBancolombia.totalVenta));
                memcpy(datosVentaBancolombia.totalVenta, stringAux, strlen(stringAux));
            }

            /*resultado = capturarPinCnbBancolombia(globalpinblock, leerTrack.pan, datosVentaBancolombia.cardName,
                                                  datosVentaBancolombia.totalVenta);*/
            intentosPin++;
            if ((resultado <= 0) && (intentosPin > 0)) {
                // screenMessage("MENSAJE", "CANCELADO", "POR USUARIO", "", 2 * 1000);
                if (verificarHabilitacionCaja() == TRUE) {
                    //reponderCajaRetirosDeclinados(TRANSACCION_TEFF_COMPRAS);
                }

                imprimirDeclinadas(datosVentaBancolombia, "CANCELADO POR USUARIO");
            }

            if (resultado > 0) {
                resultado = enviarRetiro(TRANSACCION_TEFF_COMPRAS);
            }

            if (resultado == -4) {
                resultado = 0;
                intentosPin = 1;
            } else if (resultado == -3) {
                resultado = -1;
            } else {
                intentosPin = 0;
            }

        } while (resultado > 0 && intentosPin == 1);
    }

    if ((resultado > 0)) {

        if (verificarHabilitacionCaja() == TRUE) {

            memcpy(datosVentaBancolombia.codigoRespuesta, "00", 2);
            setParameter(USO_CAJA_REGISTRADORA, "1");
            //resultado = transaccionRespuestaDatosCaja(datosVentaBancolombia, TRANSACCION_TEFF_COMPRAS);
            if (resultado < 0 /*&& verificarCajaIPCB() == 1*/) {
                memcpy(datosVentaBancolombia.codigoRespuesta, "WS", 2);
                imprimirDeclinadas(datosVentaBancolombia, "ERROR CON EL WS");
            }
        }
        mostrarAprobacionBancolombia(&datosVentaBancolombia);
    }
}

/*int capturarDatos(char tipoTarjeta) {

    int resultado = 0;
    int tipoCuenta = 0;
    int itemMenu = 0;
    char cardName[TAM_CARD_NAME + 1];

    memset(cardName, 0x00, sizeof(cardName));

    itemMenu =  (modoJMR == TRUE) ? 3 : 4;
    if (modoJMR == FALSE
        || strcmp(datosVentaBancolombia.codigoGrupo,
                  OPERACION_RETIRO_PRINCIPAL) == 0) {

        tipoCuenta = seleccionCuentaBanda("", itemMenu, 0, tipoTarjeta,
                                          datosVentaBancolombia.cardName);
        if (tipoCuenta > 0) {
            resultado = 1;
        }

        if (tipoCuenta == 30) {
            resultado = capturarUltimosCuatro(
                    datosVentaBancolombia.ultimosCuatro);
        }

    }

    ///// OTRA CUENTA
    if (tipoCuenta == 5 || strcmp(datosVentaBancolombia.codigoGrupo,  OPERACION_RETIRO_SECUNDARIA) == 0) {
        resultado = retiroOtraCuenta();
    } else {
        if (resultado > 0 && tipoCuenta > 0) {
            sprintf(datosVentaBancolombia.tipoCuenta, "%02d", tipoCuenta);

            obtenerTerminalModoCorresponsal(datosVentaBancolombia.terminalId);
            sprintf(datosVentaBancolombia.processingCode, "41%2s00", datosVentaBancolombia.tipoCuenta);
        }

        if (resultado > 0 && tipoCuenta > 0) {
            if (strlen(datosVentaBancolombia.appLabel) > 0) {
                memcpy(cardName, datosVentaBancolombia.appLabel, sizeof(datosVentaBancolombia.appLabel));
            } else {
                memcpy(cardName, datosVentaBancolombia.cardName, sizeof(datosVentaBancolombia.cardName));
            }
        }

        if (resultado > 0 && tipoCuenta > 0 && modoJMR == FALSE) {
            //////////// MONTO ////////////
            memset(datosVentaBancolombia.totalVenta, 0x00, sizeof(datosVentaBancolombia.totalVenta));
            resultado = capturarMonto("MONTO BASE", "INGRESE MONTO", cardName, 8, datosVentaBancolombia.totalVenta,
                                      "99999999", "1");
        }

        if (resultado > 0 && tipoCuenta > 0) {
            resultado = confirmarRetiro(datosVentaBancolombia.totalVenta, 0, "");
        }
    }

    return resultado;
}*/

/*
int confirmarRetiro(uChar *totalVenta, int tipoRetiro, uChar *dato) {

    int resultado = 0;
    uChar valorFormato[12 + 1];
    uChar linea1[20 + 1];
    uChar linea2[19 + 1];
    uChar linea3[20 + 1];
    uChar linea4[19 + 1];
    long valor = 0;

    memset(valorFormato, 0x00, sizeof(valorFormato));
    memset(linea1, 0x00, sizeof(linea1));
    memset(linea2, 0x00, sizeof(linea2));
    memset(linea3, 0x00, sizeof(linea3));
    memset(linea4, 0x00, sizeof(linea4));

    memcpy(valorFormato, totalVenta, strlen(totalVenta));
    valor = atol(valorFormato);
    memset(valorFormato, 0x00, sizeof(valorFormato));
    formatString(0x00, valor, "", 0, valorFormato, 1);

    if (tipoRetiro == RETIRO_EFECTIVO || tipoRetiro == RETIRO_OTRA_CTA) {
        if (tipoRetiro == 1) {
            sprintf(linea1, "CLIENTE:");
        } else {
            sprintf(linea1, "CUENTA:");
        }

        sprintf(linea2, dato);

        sprintf(linea3, "MONTO:  %s", valorFormato);
    } else {
        sprintf(linea2, "MONTO:  %s", valorFormato);
    }

    resultado = confirmarDatos("CONFIRMACION RETIRO", linea1, linea2, linea3, linea4);

    return resultado;
}*/


int enviarRetiro(int tipoTransaccion) {
    int resultado = 0;
    int intentosVentas = 0;

    do {
        armarTramaRetiro(intentosVentas);

        resultado = procesarTransaccionRetiro(tipoTransaccion);

        if (resultado == -2) {
            intentosVentas++;
        }

        if (intentosVentas == MAX_INTENTOS_VENTA) {
            //screenMessageOptimizado("COMUNICACION", "REINTENTE", "TRANSACCION", "", 1000);

            if (verificarHabilitacionCaja() == TRUE) {
                memset(datosVentaBancolombia.codigoRespuesta, 0x00, sizeof(datosVentaBancolombia.codigoRespuesta));
                if(modoJMR == TRUE) {
                    memcpy(datosVentaBancolombia.codigoRespuesta, RESPUESTA_ERROR_CONEXION, 2);
                } else {
                    memcpy(datosVentaBancolombia.codigoRespuesta, "01", 2);
                }

                //transaccionRespuestaDatosCaja(datosVentaBancolombia, tipoTransaccion);

                imprimirDeclinadas(datosVentaBancolombia, "REINTENTE TRANSACCION");
            }

            resultado = -1;
        }

    } while (resultado == -2 && intentosVentas < MAX_INTENTOS_VENTA);

    if (strlen(datosVentaBancolombia.ARQC) > 0) { // Solo entra si es CHIP
        memset(bufferAuxiliarCustomEMV, 0x00, sizeof(bufferAuxiliarCustomEMV));

        globalresultadoUnpack = unpackISOMessage(globalDataRecibida, globalsizeData);

        //cargaTagsRespuestaTransacionEMV(bufferAuxiliarCustomEMV, &sizeCustomEMV, globalresultadoUnpack, resultado);

        /*if (resultado == 1) {
            globalTransaccionAprobada = TRUE;
        }*/
    }

    return resultado;
}

void armarTramaRetiro(int intentosVentas) {

    int longitud = 0;

    ResultTokenPack resultTokenPack;
    DatosTarjetaSembrada datosTarjetaSembrada;
    DatosTokens datosToken;
    uChar pan[19 + 1];
    uChar tokenQZ[81 + 1];

    memset(&resultTokenPack, 0x00, sizeof(resultTokenPack));
    memset(&datosTarjetaSembrada, 0x30, sizeof(datosTarjetaSembrada));
    memset(&datosToken, 0x00, sizeof(datosToken));
    memset(pan, 0x00, sizeof(pan));

    ///// DATOS PARA EL TOKEN R3
    strcpy(datosToken.valorKotenR3, TOKEN_RETIRO);

    datosTarjetaSembrada = cargarTarjetaSembrada();
    memcpy(pan, datosTarjetaSembrada.pan, 19);

    if (atoi(datosVentaBancolombia.tipoTransaccion) == TRANSACCION_BCL_RETIRO_EFECTIVO) {

        memcpy(datosVentaBancolombia.ultimosCuatro,
               datosVentaBancolombia.tokenVivamos + (strlen(datosVentaBancolombia.tokenVivamos) - 4), 4);

        strcpy(datosVentaBancolombia.cardName, datosTarjetaSembrada.cardName);
        strcpy(datosVentaBancolombia.tipoCuenta, "10"); ///// CUANDO ES RETIRO SIN TARJETA SE DEBE ENVIAR SIEMPRE LA CTA AHORROS
        strcpy(datosVentaBancolombia.tarjetaHabiente, datosTarjetaSembrada.tarjetaHabiente);

        ///// DATOS PARA EL TOKEN R3
        strcpy(datosToken.valorKotenR3, TOKEN_RETIRO_EFECTIVO);
    }

    memset(datosVentaBancolombia.iva, 0x00, sizeof(datosVentaBancolombia.iva));
    strcpy(datosVentaBancolombia.iva, datosToken.valorKotenR3);

    if (datosVentaBancolombia.baseDevolucion[0] == 0x00) {
        memset(datosVentaBancolombia.baseDevolucion, 0x30, sizeof(datosVentaBancolombia.baseDevolucion));
    }

    if (atoi(datosVentaBancolombia.numeroRecibo2) != RETIRO_TARJETA) {

        /// SE DEBE ALMACENAR LA CUENTA EN UN CAMPO LIBRE DE DATOS VENTA
        memset(datosToken.cuentaOrigen, 0x00, sizeof(datosToken.cuentaOrigen));
        memcpy(datosToken.cuentaOrigen, datosVentaBancolombia.tokenVivamos, SIZE_CUENTA); //// CUENTA INGRRSADA
        LOGI("datosToken.cuentaOrigen %s", datosToken.cuentaOrigen);
        memset(pan, 0x00, sizeof(pan));
        memcpy(pan, datosToken.cuentaOrigen, SIZE_CUENTA);
    }

    memcpy(datosVentaBancolombia.msgType, "0200", TAM_MTI);
    longitud = _armarTokenAutorizacion_(tokenQZ);

    empaquetarEncabezado(&datosVentaBancolombia, pan, intentosVentas);

    ///// DATOS PARA EL TOKEN QF
    strcpy(datosToken.tipoCuentaOrigen, datosVentaBancolombia.tipoCuenta);

    memset(datosToken.tipoCuentaDestino, 0x00, sizeof(datosToken.tipoCuentaDestino));
    strcpy(datosToken.tipoCuentaDestino, datosTarjetaSembrada.tipoCuenta); //// TIPO DE CUENTA SEMBRADA

    strcpy(datosToken.cuentaDestino, datosTarjetaSembrada.pan); //// PAN DE LA TARJETA SEMBRADA

    ///// EMPAQUETA EL CAMPO 60 CON LOS DATOS DE DATOSTOKENS
    empaquetarP60(datosToken, datosVentaBancolombia.tipoTransaccion);

    //// SE RECUPERA EL CAMPO 60
    resultTokenPack = _packTokenMessage_();

    setField(60, resultTokenPack.tokenPackMessage, resultTokenPack.totalBytes);

   if (strlen(globalpinblock) > 0) {
        setField(52, globalpinblock, LEN_PIN_DATA);
    }
    if (globalsizep55 > 0) {
        if (strlen(globalpinblock) > 0) {
            setField(53, SEGURIDAD_PIN_EMV, 16);
        }
    }
    setField(58, tokenQZ, longitud);
}


int procesarTransaccionRetiro(int tipoTransaccion) {

    ResultISOPack resultadoIsoPackMessage;
    int resultadoTransaccion = -1;
    DatosTransaccionDeclinada datosTransaccionDeclinada;

    memset(globalDataRecibida, 0x00, sizeof(globalDataRecibida));
    memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
    memset(&datosTransaccionDeclinada, 0x00, sizeof(datosTransaccionDeclinada));

    resultadoIsoPackMessage = packISOMessage();

    if (resultadoIsoPackMessage.responseCode > 0) {

        resultadoTransaccion = realizarTransaccion(resultadoIsoPackMessage.isoPackMessage,
                                                   resultadoIsoPackMessage.totalBytes, globalDataRecibida, atoi(datosVentaBancolombia.tipoTransaccion),
                                                   CANAL_DEMANDA, REVERSO_GENERADO);


        if (resultadoTransaccion > 0) {
            globalsizeData = resultadoTransaccion;
            // desarmar la trama
            LOGI("  Entro unpackDataRetiro");
            resultadoTransaccion = unpackDataRetiro(tipoTransaccion);
            LOGI("  salio  unpackDataRetiro");
        } else {

            if (verificarHabilitacionCaja() == TRUE) {
                if(modoJMR == TRUE) {
                    memcpy(datosVentaBancolombia.codigoRespuesta, RESPUESTA_ERROR_CONEXION, TAM_RESPUESTA_JMR);
                }
                //reponderCajaRetirosDeclinados(tipoTransaccion);
            }

            if (resultadoTransaccion == -3) {
                imprimirDeclinadas(datosVentaBancolombia, "TIME OUT GENERAL");
            } else {
                imprimirDeclinadas(datosVentaBancolombia, "ERROR SIN CONEXION");
            }

            resultadoTransaccion = 0;
        }
    }

    return resultadoTransaccion;
}

int unpackDataRetiro(int tipoTransaccion) {

    ISOFieldMessage isoFieldMessage;
    uChar codigoRespuesta[2 + 1];
    int resultado = 2;
    uChar mensajeError[21 + 1];
    ResultTokenUnpack resultTokenUnpack;
    TokenMessage tokenMessage;
    uChar cuenta[11 + 1];
    DatosCnbBancolombia datosRetiro;

    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(&codigoRespuesta, 0x00, sizeof(codigoRespuesta));
    memset(mensajeError, 0x00, sizeof(mensajeError));
    memset(&resultTokenUnpack, 0x00, sizeof(resultTokenUnpack));
    memset(&tokenMessage, 0x00, sizeof(tokenMessage));
    memset(cuenta, 0x00, sizeof(cuenta));
    memset(&datosRetiro, 0x00, sizeof(datosRetiro));

    /// VALIDACIONES TRANSACCION ///
    globalresultadoUnpack = unpackISOMessage(globalDataRecibida, globalsizeData);

    isoFieldMessage = getField(39);
    memcpy(codigoRespuesta, isoFieldMessage.valueField, isoFieldMessage.totalBytes);
    memcpy(datosVentaBancolombia.codigoRespuesta, isoFieldMessage.valueField, isoFieldMessage.totalBytes);
    LOGI("Codigo de respuesta %s",codigoRespuesta);
    if (strcmp(codigoRespuesta, "00") == 0) {

        memcpy(datosRetiro.processingCode,datosVentaBancolombia.processingCode,sizeof(datosVentaBancolombia.processingCode));
        memcpy(datosRetiro.systemTrace,datosVentaBancolombia.systemTrace,sizeof(datosVentaBancolombia.systemTrace));
        memcpy(datosRetiro.terminalId,datosVentaBancolombia.terminalId,sizeof(datosVentaBancolombia.terminalId));
        resultado = verificacionTramabancolombia(&datosRetiro);
        LOGI("Entro resultado %d", resultado);
        isoFieldMessage = getField(60);
        resultTokenUnpack = unpackTokenMessage(isoFieldMessage.valueField, isoFieldMessage.totalBytes);
        LOGI("Entro getTokenMesage");

        tokenMessage = _getTokenMessage_((char*)"QF");
        LOGI("salio getokenMesage");
        memcpy(cuenta, tokenMessage.valueToken + 10, 11);

        memset(datosVentaBancolombia.tokenVivamos, 0x00, sizeof(datosVentaBancolombia.tokenVivamos));
        leftPad(datosVentaBancolombia.tokenVivamos, cuenta, '0', 11);
        LOGI("Copio token vivamos");

        if (resultado > 0) {
            resultado = 1;
        } else if (resultado < 1) {

            resultado = generarReverso();

            if (verificarHabilitacionCaja() == TRUE && (resultado == -3 || resultado == -1)) {

                if(modoJMR == TRUE){
                    memcpy(datosVentaBancolombia.codigoRespuesta, RESPUESTA_ERROR_CONEXION, TAM_RESPUESTA_JMR);
                }
                //reponderCajaRetirosDeclinados(tipoTransaccion);
            }
            if (resultado == -3) {
                imprimirDeclinadas(datosVentaBancolombia, "TIME OUT GENERAL");
            } else if (resultado == -1) {
                imprimirDeclinadas(datosVentaBancolombia, "REVERSO PENDIENTE");
            }

            if (resultado > 0) {
                resultado = -2;
            }
        }
    } else {
        borrarArchivo(discoNetcom, (char*)ARCHIVO_REVERSO);
        errorRespuestaTransaccion(codigoRespuesta, mensajeError);
        resultado = 0;

/*        if (strcmp(codigoRespuesta, "55") == 0) {
            resultado = -4;
        } else if (strcmp(codigoRespuesta, "75") == 0) {
            resultado = -3;
        }

        //// SI LA RESPUESTA ES 55 O 75 Y ES DEFERENTE DE CHIP, ENTONCES NO DEBE IMPRIMIR
        if ((resultado == -4) && (strlen(datosVentaBancolombia.AID) == 0)) {
            return resultado;
        }

        if (verificarHabilitacionCaja() == TRUE) {
            memset(datosVentaBancolombia.codigoRespuesta, 0x00, sizeof(datosVentaBancolombia.codigoRespuesta));
            if(modoJMR == TRUE){
                memcpy(datosVentaBancolombia.codigoRespuesta, RESPUESTA_DECLINADA, 2);
            } else {
                memcpy(datosVentaBancolombia.codigoRespuesta, "01", 2);
            }
            memset(datosVentaBancolombia.numeroRecibo, 0x00, sizeof(datosVentaBancolombia.numeroRecibo));

            //transaccionRespuestaDatosCaja(datosVentaBancolombia, tipoTransaccion);
        }*/

        imprimirDeclinadas(datosVentaBancolombia, mensajeError);
    }
    LOGI("Llego al final");

    return resultado;
}


void setVariableGlobalesRetiro(void) {

    globalsizeData = 0;
    globalsizep55 = 0;
//	sizeCustomEMV = 0;
    globalTransaccionAprobada = 0;
    memset(globalCampo55, 0x00, sizeof(globalCampo55));
    memset(globalDataRecibida, 0x00, sizeof(globalDataRecibida));
    memset(mensajeDeclinada, 0x00, sizeof(mensajeDeclinada));
    memset(globalpinblock, 0x00, sizeof(globalpinblock));
    memset(&globalresultadoUnpack, 0x00, sizeof(globalresultadoUnpack));
    memset(&bufferAuxiliarCustomEMV, 0x00, sizeof(bufferAuxiliarCustomEMV));
    limpiarTokenMessage();

}
int armarPanRetiroEfectivobancolombia(char * track2, char *pan, char * cardName){

    char datosPanAuxiliar[20 + 1] = {0x00};
    DatosTarjetaSembrada datosTarjetaSembrada;
    leftPad(datosPanAuxiliar, pan, '0', SIZE_CUENTA);
    memset(pan, 0x00, sizeof(pan));
    memcpy(pan, datosPanAuxiliar + 1, 10);
    memset(&datosTarjetaSembrada, 0x00, sizeof(datosTarjetaSembrada));
    armarTrack(track2, pan, TRANSACCION_BCL_RETIRO_EFECTIVO);
    LOGI("hasta qui bien %s",track2);
    memset(pan, 0x00, sizeof(pan));
    memcpy(pan, track2, 16);
    LOGI("este es el pan %s",pan);
    memset(track2, 0x00, sizeof(track2));
    memcpy(track2,pan, 16);
    LOGI("otravez track2 %s",track2);
    datosTarjetaSembrada = cargarTarjetaSembrada();

    memcpy(cardName, datosTarjetaSembrada.cardName, strlen(datosTarjetaSembrada.cardName));
    LOGI("cardname  %s",datosTarjetaSembrada.cardName);
    LOGI("cardname  %s",cardName);
    return 1;
}