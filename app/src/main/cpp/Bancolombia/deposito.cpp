//
// Created by NETCOM on 10/02/2020.
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

#define  LOG_TAG    "NETCOM_BANCOLOMBIA_DEPOSITO"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

int enviarDeposito(char *cuenta,char *monto, int tipoCuenta, char* codigoAprobacion){

    int resultado = 0;
    int intentosVentas = 0;
    char datoAuxiliar[SIZE_CUENTA + 1] = {0x00};

    DatosCnbBancolombia datosDeposito;

    memset(&datosVentaBancolombia, 0x00, sizeof(datosVentaBancolombia));
    memset(&datosDeposito, 0x00, sizeof(datosDeposito));

    memcpy(datosVentaBancolombia.totalVenta,monto, 12);

    memcpy(datoAuxiliar, cuenta, SIZE_CUENTA);
    leftPad(datosVentaBancolombia.textoAdicional, datoAuxiliar, '0', SIZE_CUENTA);

    memset(datosVentaBancolombia.tipoCuenta, 0x00, sizeof(datosVentaBancolombia.tipoCuenta));
    sprintf(datosVentaBancolombia.tipoCuenta, "%02d", tipoCuenta);
    obtenerTerminalModoCorresponsal(datosVentaBancolombia.terminalId);
    do {
        armarTramaDeposito(&datosDeposito, intentosVentas);

        resultado = procesarTransaccionDeposito(&datosDeposito);

        if (resultado == -2) {
            intentosVentas++;
        }

        if (intentosVentas == 3) {//CAMBIAR POR DEFINE
            resultado = -1;
        }

    } while (resultado == -2 && intentosVentas < 3);

    if(resultado > 0){
        memcpy(codigoAprobacion, datosVentaBancolombia.codigoAprobacion, strlen(datosVentaBancolombia.codigoAprobacion));
   }
    return resultado;
}
/*int enviarDeposito(){


    return 10;
}*/

void armarTramaDeposito(DatosCnbBancolombia *datosDeposito, int intentosVentas) {

    ISOHeader isoHeader8583;
    uChar nii[TAM_NII + 1];
    uChar p59[16];
    DatosTokens datosToken;
    ResultTokenPack resultTokenPack;

    DatosTarjetaSembrada datosTarjetaSembrada;

    uChar tokenQZ[81 + 1];
    int longitud = 0;

    memset(&isoHeader8583, 0x00, sizeof(isoHeader8583));
    memset(nii, 0x00, sizeof(nii));
    memset(p59, 0x30, sizeof(p59));
    memset(&datosToken, 0x00, sizeof(datosToken));
    memset(&resultTokenPack, 0x00, sizeof(resultTokenPack));
    memset(&datosTarjetaSembrada, 0x30, sizeof(datosTarjetaSembrada));
    memset(tokenQZ, 0x00, sizeof(tokenQZ));

    getParameter(NII, nii);

    if (intentosVentas == 0) {
        generarInvoice(datosVentaBancolombia.numeroRecibo);
        generarSystemTraceNumber(datosVentaBancolombia.systemTrace, (char*)"");
        strcat(datosVentaBancolombia.totalVenta, "00");
    }

    sprintf(datosVentaBancolombia.processingCode, "25%2s00", datosVentaBancolombia.tipoCuenta);
    sprintf(datosVentaBancolombia.tipoTransaccion, "%02d", TRANSACCION_BCL_DEPOSITO);
    strcpy(datosVentaBancolombia.posEntrymode, "021");

    longitud = _armarTokenAutorizacion_(tokenQZ);

    memcpy(datosVentaBancolombia.msgType, "0200", TAM_MTI);
    memcpy(datosVentaBancolombia.nii, nii + 1, TAM_NII);
    memcpy(datosVentaBancolombia.posConditionCode, "00", TAM_POS_CONDITION);

    isoHeader8583.TPDU = 60;
    memcpy(isoHeader8583.destination, nii, TAM_NII + 1);
    memcpy(isoHeader8583.source, nii, TAM_NII + 1);
    setHeader(isoHeader8583);


    setMTI(datosVentaBancolombia.msgType);

    setField(3, datosVentaBancolombia.processingCode, TAM_CODIGO_PROCESO);
    setField(4, datosVentaBancolombia.totalVenta, 12);
    setField(11, datosVentaBancolombia.systemTrace, TAM_SYSTEM_TRACE);
    setField(22, datosVentaBancolombia.posEntrymode, TAM_ENTRY_MODE);
    setField(24, datosVentaBancolombia.nii, TAM_NII);
    setField(25, datosVentaBancolombia.posConditionCode, TAM_POS_CONDITION);

    datosTarjetaSembrada = cargarTarjetaSembrada();

    armarTrack(datosVentaBancolombia.track2, datosTarjetaSembrada.pan, atoi(datosVentaBancolombia.tipoTransaccion));
    setField(35, datosVentaBancolombia.track2, strlen(datosVentaBancolombia.track2));

    setField(41, datosVentaBancolombia.terminalId, TAM_TERMINAL_ID);

    //// ARMADO DE TOKENS

    strcpy(datosVentaBancolombia.tarjetaHabiente, datosTarjetaSembrada.tarjetaHabiente);

    strcpy(datosToken.valorKotenR3, TOKEN_DEPOSITO);
    memset(datosVentaBancolombia.iva, 0x00, sizeof(datosDeposito->tipoProducto));
    strcpy(datosVentaBancolombia.iva, datosToken.valorKotenR3);

    strcpy(datosToken.tipoCuentaOrigen, datosTarjetaSembrada.tipoCuenta); //// TIPO DE CUENTA SEMBRADA
    strcpy(datosToken.cuentaOrigen, datosTarjetaSembrada.pan); //// PAN DE LA TARJETA SEMBRADA

    strcpy(datosToken.tipoCuentaDestino, datosVentaBancolombia.tipoCuenta); //// TIPO DE CUENTA SELECCIONADA
    strcpy(datosToken.cuentaDestino, datosVentaBancolombia.textoAdicional);

    empaquetarP60(datosToken, datosVentaBancolombia.tipoTransaccion);
    setField(58, tokenQZ, longitud);
    //// SE RECUPERA EL CAMPO 60
    resultTokenPack = _packTokenMessage_();

    setField(60, resultTokenPack.tokenPackMessage, resultTokenPack.totalBytes);
}



int procesarTransaccionDeposito(DatosCnbBancolombia *datosDeposito) {

    ResultISOPack resultadoIsoPackMessage;
    int resultadoTransaccion = -1;
    //DatosTransaccionDeclinada datosTransaccionDeclinada;
    char dataRecibida[512];

    memset(dataRecibida, 0x00, sizeof(dataRecibida));
    memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
    //memset(&datosTransaccionDeclinada, 0x00, sizeof(datosTransaccionDeclinada));

    verificarHabilitacionCaja();

    resultadoIsoPackMessage = packISOMessage();

    if (resultadoIsoPackMessage.responseCode > 0) {

        resultadoTransaccion = realizarTransaccion(resultadoIsoPackMessage.isoPackMessage,
                                                   resultadoIsoPackMessage.totalBytes, dataRecibida, atoi(datosVentaBancolombia.tipoTransaccion), CANAL_DEMANDA,
                                                   REVERSO_GENERADO);
        memcpy(datosDeposito->tipoTransaccion, datosVentaBancolombia.tipoTransaccion, 2);
        if (resultadoTransaccion > 0) {
            // desarmar la trama
            resultadoTransaccion = unpackData(datosDeposito, dataRecibida, resultadoTransaccion);
        } else {

            memset(datosVentaBancolombia.codigoRespuesta, 0x00, sizeof(datosVentaBancolombia.codigoRespuesta));
            memcpy(datosVentaBancolombia.codigoRespuesta, "01", 2);

            if (verificarHabilitacionCaja() == TRUE) {
                //  transaccionRespuestaDatosCaja(datosVentaBancolombia, TRANSACCION_TEFF_DEPOSITO_BCL);
            }

            if (resultadoTransaccion == -3) {
                imprimirDeclinadasCNB(*datosDeposito, "TIME OUT GENERAL");
            } else {
                imprimirDeclinadasCNB(*datosDeposito, "ERROR SIN CONEXION");
            }

            resultadoTransaccion = 0;
        }
    }

    return resultadoTransaccion;
}

int unpackData(DatosCnbBancolombia *datosDeposito, char *dataRecibida, int cantidadBytes) {

    ISOFieldMessage isoFieldMessage;
    uChar codigoRespuesta[2 + 1];
    int resultado = 2;
    uChar mensajeError[21 + 1];

    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(&codigoRespuesta, 0x00, sizeof(codigoRespuesta));
    memset(mensajeError, 0x00, sizeof(mensajeError));

    /// VALIDACIONES TRANSACCION ///
    globalresultadoUnpack = unpackISOMessage(dataRecibida, cantidadBytes);

    isoFieldMessage = getField(39);
    memcpy(codigoRespuesta, isoFieldMessage.valueField, isoFieldMessage.totalBytes);
    memcpy(datosDeposito->tipoTransaccion, datosVentaBancolombia.tipoTransaccion, 2);

    memcpy(datosVentaBancolombia.codigoRespuesta, isoFieldMessage.valueField, isoFieldMessage.totalBytes);
    if (strcmp(codigoRespuesta, "00") == 0) {
        memcpy(datosDeposito->processingCode,datosVentaBancolombia.processingCode,sizeof(datosVentaBancolombia.processingCode));
        memcpy(datosDeposito->systemTrace,datosVentaBancolombia.systemTrace,sizeof(datosVentaBancolombia.systemTrace));
        memcpy(datosDeposito->terminalId,datosVentaBancolombia.terminalId,sizeof(datosVentaBancolombia.terminalId));
        memcpy(datosDeposito->totalVenta,datosVentaBancolombia.totalVenta,sizeof(datosVentaBancolombia.totalVenta));
        resultado = verificacionTramabancolombia(datosDeposito);
        if (resultado > 0) {
            ///// SE VALIDA EL CAMPO 61 PARA EL COSTO DE LA TRANSACCION ////

            //verificarCosto(datosDeposito->field61);

            //datos para respuesta a la caja
            /*memcpy(datosVentaBancolombia.codigoAprobacion, datosDeposito->codigoAprobacion,
                   strlen(datosDeposito->codigoAprobacion));
            memcpy(datosVentaBancolombia.rrn, datosDeposito->rrn, strlen(datosDeposito->rrn));
            memcpy(datosVentaBancolombia.numeroRecibo, datosDeposito->numeroRecibo, TAM_NUMERO_RECIBO);
            memcpy(datosVentaBancolombia.tipoCuenta, datosDeposito->tipoCuenta, TAM_TIPO_CUENTA);
            sprintf(datosVentaBancolombia.tipoTransaccion, "%02d", TRANSACCION_BCL_DEPOSITO);*/

            //datos para respuesta a la caja

            if (verificarHabilitacionCaja() == TRUE) {
                memcpy(datosVentaBancolombia.codigoRespuesta, "00", 2);
                setParameter(USO_CAJA_REGISTRADORA, "1");
                // resultado = transaccionRespuestaDatosCaja(datosVentaBancolombia, TRANSACCION_TEFF_DEPOSITO_BCL);

                if (resultado < 0/* && verificarCajaIPCB() == 1*/) {
                    memcpy(datosDeposito->codigoRespuesta, "WS", 2);
                    imprimirDeclinadas(datosVentaBancolombia, "ERROR CON EL WS");
                }
            }
            //control de cambios login de operador
            setParameter(USER_CNB_LOGON, (char*)"1");
            mostrarAprobacionDeposito(datosDeposito);

        } else if (resultado < 1) {
            resultado = generarReverso();

            if (verificarHabilitacionCaja() == TRUE && (resultado == -3 || resultado == -1)) {
                memset(datosVentaBancolombia.codigoRespuesta, 0x00, sizeof(datosVentaBancolombia.codigoRespuesta));
                memcpy(datosVentaBancolombia.codigoRespuesta, "01", 2);

                //transaccionRespuestaDatosCaja(datosVentaBancolombia, TRANSACCION_TEFF_DEPOSITO_BCL);
            }

            if (resultado == -3) {
                imprimirDeclinadasCNB(*datosDeposito, "TIME OUT GENERAL");
            } else if (resultado == -1) {
                imprimirDeclinadasCNB(*datosDeposito, "REVERSO PENDIENTE");
            }

            if (resultado > 0) {
                resultado = -2;
            }
        }
    } else {
        borrarArchivo(discoNetcom, (char*)ARCHIVO_REVERSO);
        errorRespuestaTransaccion(codigoRespuesta, mensajeError);
        resultado = 0;

        if (verificarHabilitacionCaja() == TRUE) {
            memset(datosVentaBancolombia.codigoRespuesta, 0x00, sizeof(datosVentaBancolombia.codigoRespuesta));
            memcpy(datosVentaBancolombia.codigoRespuesta, "01", 2);

            //transaccionRespuestaDatosCaja(datosVentaBancolombia, TRANSACCION_TEFF_DEPOSITO_BCL);
        }
        imprimirDeclinadasCNB(*datosDeposito, mensajeError);
    }

    return resultado;
}

void mostrarAprobacionDeposito(DatosCnbBancolombia *datosDeposito) {

    ISOFieldMessage isoFieldMessage;
    uChar pan[20 + 1];
    uChar modo[2 + 1];
    int resultado = 0;
    uChar impresionRecibo[1 + 1];
    int resultadoCaja = 0;
    int iRet = 0;

    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(pan, 0x00, sizeof(pan));
    memset(modo, 0x00, sizeof(modo));

    memset(datosDeposito->AID, 0x00, sizeof(datosDeposito->AID));
    memset(datosDeposito->ARQC, 0x00, sizeof(datosDeposito->ARQC));
    memset(datosDeposito->appLabel, 0x00, sizeof(datosDeposito->appLabel));

//	verificarConsultaInformacion( &datosVenta, globalDataRecibida, globalsizeData);

    ///// SE VALIDA EL CAMPO 61 PARA EL COSTO DE LA TRANSACCION ////
    verificarCosto(datosVentaBancolombia.field61);

    /// BORRAR ARCHIVO REVERSO ///
    borrarArchivo(discoNetcom, (char*)ARCHIVO_REVERSO);
    sprintf(datosVentaBancolombia.estadoTransaccion, "%d", 1);

    isoFieldMessage = getField(12);
    memset(datosVentaBancolombia.txnTime, 0x00, sizeof(datosVentaBancolombia.txnTime));
    memcpy(datosVentaBancolombia.txnTime, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

    isoFieldMessage = getField(13);
    memset(datosVentaBancolombia.txnDate, 0x00, sizeof(datosDeposito->txnDate));
    memcpy(datosVentaBancolombia.txnDate, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

    isoFieldMessage = getField(37);
    memset(datosVentaBancolombia.rrn, 0x00, sizeof(datosDeposito->rrn));
    memcpy(datosVentaBancolombia.rrn, isoFieldMessage.valueField + 6, 6);

    isoFieldMessage = getField(38);
    memset(datosVentaBancolombia.codigoAprobacion, 0x00, sizeof(datosVentaBancolombia.codigoAprobacion));
    memcpy(datosVentaBancolombia.codigoAprobacion, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

    // // SE COPIA LA VARIABLE GLOBAL QUE CONTIENE EL TOKEN COMPLETO
    convertirDatosBancolombia(datosVentaBancolombia, datosDeposito);
    memcpy(datosDeposito->tokensQF, tokensQF, SIZE_TOKEN_QF);

    guardarTransaccionBancolombia(discoNetcom, (char*)JOURNAL_CNB_BANCOLOMBIA, *datosDeposito);

    _guardarDirectorioJournals_(TRANS_CNB_BANCOLOMBIA, datosDeposito->numeroRecibo, datosDeposito->estadoTransaccion);

    imprimirTicketBancolombia(*datosDeposito, RECIBO_COMERCIO, RECIBO_NO_DUPLICADO);
    validarActualizacionTelecarga(globalresultadoUnpack);
}

void verificarCosto(uChar * costoTransaccion) {

    int indice = 3; /// LOS 3 PRIMEROS CONTIENEN EL PROCESSING CODE
    ISOFieldMessage isoFieldMessage;
    int idx = 0;

    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(costoTransaccion, 0x00, sizeof(costoTransaccion));

    for (idx = 0; idx < globalresultadoUnpack.totalField; idx++) {
        if (globalresultadoUnpack.listField[idx] == 61) {
            isoFieldMessage = getField(61);
            if (isoFieldMessage.totalBytes == 13) {
                memcpy(costoTransaccion, isoFieldMessage.valueField + indice, 10);
            }

            break;
        }
    }

}
