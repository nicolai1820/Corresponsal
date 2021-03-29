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
#include "Definiciones.h"
#include "venta.h"

int consultarSaldo(DatosTarjetaAndroid datosTarjeta, char *saldo) {

    long resultadoSaldo = 0;
    long binTarjeta = 0;
    int tipoCuenta = 0;
    int resultadoTransaccion = 0;
    int resultadoCosto = 0;
    int idx = 0;
    int intentosPin = 0;
    short isSeleccionCuentaDefault = 0;
    short isSeleccionCuentaMultiservicios = 0;
    uChar pinBlock[8 + 1];
    uChar terminalId[8 + 1];
    uChar saldoDisponible[15 + 1];
    uChar nii[4 + 1];
    uChar systemTrace[6 + 1];
    uChar codigoProceso[6 + 1];
    uChar codigoRespuesta[2 + 1];
    uChar variableAux[16 + 1];
    uChar entryMode[3 + 1];
    uChar dataRecibida[512];
    uChar working3DES[16 + 1];
    uChar fechaProceso[10 + 1];
    uChar ultimosCuatro[TAM_CODIGO_PROCESO + 1];
    ResultISOPack resultadoIsoPackMessage;
    ResultISOUnpack resultadoUnpack;
    ISOFieldMessage isoFieldMessage;
    uChar bufferAux[46 + 1];
    ISOHeader isoHeader8583;
    uChar modo[2 + 1];
    char binCafam[6 + 1];
    char mensajeError[100 + 1] = {0x00};

    memset(pinBlock, 0x00, sizeof(pinBlock));
    memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
    memset(&resultadoUnpack, 0x00, sizeof(resultadoUnpack));
    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(&isoHeader8583, 0x00, sizeof(isoHeader8583));
    memset(codigoRespuesta, 0x00, sizeof(codigoRespuesta));
    memset(terminalId, 0x00, sizeof(terminalId));
    memset(nii, 0x00, sizeof(nii));
    memset(dataRecibida, 0x00, sizeof(dataRecibida));
    memset(systemTrace, 0x00, sizeof(systemTrace));
    memset(codigoProceso, 0x00, sizeof(codigoProceso));
    memset(saldoDisponible, 0x00, sizeof(saldoDisponible));
    memset(ultimosCuatro, 0x00, sizeof(ultimosCuatro));
    memset(fechaProceso, 0x00, sizeof(fechaProceso));
    memset(variableAux, 0x00, sizeof(variableAux));
    memset(entryMode, 0x00, sizeof(entryMode));
    memset(working3DES, 0x00, sizeof(working3DES));
    memset(bufferAux, 0x00, sizeof(bufferAux));
    memset(binCafam, 0x00, sizeof(binCafam));

    DatosVenta datosVenta;

    TablaCuatroInicializacion tablaCuatro;
    TablaTresInicializacion dataIssuer;

    memset(&datosVenta, 0x00, sizeof(datosVenta));
    memset(&tablaCuatro, 0x00, sizeof(tablaCuatro));
    memset(&dataIssuer, 0x00, sizeof(dataIssuer));

    //   if (datosTarjeta.isChip != 1 || datosTarjeta.isFallBack == 1) {

    tablaCuatro = _desempaquetarTablaCuatro_();

    getParameter(NUMERO_TERMINAL, terminalId);
    getParameter(NII, nii);

//        resultadoSaldo = datosTarjeta.respuesta;
    resultadoSaldo = 1;
    /*       isSeleccionCuentaDefault =
                   datosTarjeta.configuracionExcepciones.comportamientoExcepciones1.seleccionCuentaDefault;
           isSeleccionCuentaMultiservicios =
                   datosTarjeta.configuracionExcepciones.comportamientoExcepciones1.seleccionCuentaMultiservicios;

           ///////// WORKING KEY 3DES ////////////
           memcpy(working3DES, tablaCuatro.encryptedPinKey, 8);
           memcpy(working3DES + 8, tablaCuatro.encryptedPinKey2, 8);
   */
    /////////// CUENTA POR DEFAULT //////////
//        datosTarjeta.dataIssuer.defaultAccount *= 10;
//        sprintf(codigoProceso, "31%02d00", datosTarjeta.dataIssuer.defaultAccount);
    memcpy(codigoProceso, "311000", 6);

    ///////////////  SI ES TRANSACCION MULTISERVICIOS ///////////////
    memcpy(binCafam, datosTarjeta.track2, 6);

    /*if (resultadoSaldo > 0 && (isSeleccionCuentaMultiservicios == TRUE || strncmp(binCafam, "606125", 6) == 0)) {

        resultadoSaldo = menuMultiservicios(binCafam);

        if (resultadoSaldo > 0) {
            tipoCuenta = resultadoSaldo;
            sprintf(codigoProceso, "31%02d00", tipoCuenta);
            datosTarjeta.dataIssuer.options1.selectAccountType = 0;
            datosTarjeta.dataIssuer.options1.pinEntry = 1;
        }

        isSeleccionCuentaDefault = 0;
        datosTarjeta.dataIssuer.options1.selectAccountType = 0;
    }*/

    /////// SELECCION DE CUENTA //////////////
    if (resultadoSaldo > 0 ) {
        //       if ((resultadoSaldo > 0 && datosTarjeta.dataIssuer.options1.selectAccountType == 1)
        //           || (resultadoSaldo > 0 && isSeleccionCuentaDefault == TRUE)) {
        isSeleccionCuentaDefault = 1;

        long panTarjeta = 0;
        uChar panString[9 + 1];

        memset(panString, 0x00, sizeof(panString));
        memcpy(panString, datosTarjeta.pan, 9);

        panTarjeta = atol(panString);

        //resultadoSaldo = menuTipoDeCuenta(isSeleccionCuentaDefault, panTarjeta);
        if (resultadoSaldo > 0) {
            tipoCuenta = resultadoSaldo;
            sprintf(codigoProceso, "31%02d00", tipoCuenta);
        }
    }
    /////////////// SI ES DE NATURALEZA CREDITO ///////////////
    idx = strlen(datosTarjeta.pan);
    idx -= 4;
    memcpy(ultimosCuatro, datosTarjeta.pan + idx, 4);
/*        if (tipoCuenta == 30) {
            if (datosTarjeta.dataIssuer.options4.verifyLastFourPANDigits == 1) {
                resultadoSaldo = validarUltimosCuatroBanda(ultimosCuatro);
            }
        }*/

    /////////////// CONSULTA DE LA INFORMACION       ///////////////
    if (resultadoSaldo > 0) {
        memset(modo, 0x00, sizeof(modo));
        getParameter(MODO_DATAFONO, modo);

        strcpy(datosVenta.modoDatafono, modo);
        sprintf(datosVenta.tipoTransaccion, "%02d", TRANSACCION_SALDO);
        strcpy(datosVenta.posEntrymode, "021");
        strcpy(datosVenta.track2, datosTarjeta.track2);
        strcpy(datosVenta.processingCode, codigoProceso);

        //resultadoSaldo = consultarInformacion(&datosVenta);//SI VA EN ANDROID
    }

    ////////////// CONSULTA COSTO DE LA TRANSACCION /////////////
    if (resultadoSaldo > 0) {
        memcpy(variableAux, datosVenta.track2, 9);
        binTarjeta = atol(variableAux);
        dataIssuer = _traerIssuer_(binTarjeta);

        resultadoCosto = _TRUE_;
        if (dataIssuer.additionalPromptsSelect.additionalPrompt4 == 1) {
            sprintf(datosVenta.tipoCuenta, "%02d", tipoCuenta);
            memcpy(datosVenta.terminalId, terminalId, 8);
            memcpy(datosVenta.processingCode, codigoProceso, 6);
            //resultadoCosto = consultarCosto(&datosVenta);//SI VA EN ANDROID
        }

        if (resultadoCosto < 0) {
            resultadoSaldo = -1;
        }
    }

    do {

        ///// INGRESAR PIN ////////
/*            if ((datosTarjeta.dataIssuer.options1.pinEntry == 1 || tipoCuenta == 10 || tipoCuenta == 20) && resultadoSaldo > 0) {

                char inTitulo[30 + 1];

                memset(inTitulo, 0x00, sizeof(inTitulo));

                strcpy(inTitulo, "PIN");

                memcpy(bufferAux, datosTarjeta.dataIssuer.cardName, 10);
                memcpy(bufferAux + 10, "", 12);
                memcpy(bufferAux + 22, datosTarjeta.pan, strlen(datosTarjeta.pan));
                memcpy(bufferAux + 42, inTitulo, strlen(inTitulo));


                if ((resultadoSaldo <= 0) && (intentosPin > 0)) {
                    //screenMessage("MENSAJE", "CANCELADO", "POR USUARIO", "", 2 * 1000);
                }
            }*/

        ////// ENVIAR TRANSACCION CONSULTA DE SALDO ////////
        if (resultadoSaldo > 0) {
            /*               if (datosTarjeta.isFallBack == 1) {
                               strcpy(entryMode, "801");
                           } else {
                               strcpy(entryMode, "021");
                           }*/
            generarSystemTraceNumber(systemTrace,(char *)"");

            isoHeader8583.TPDU = 60;
            memcpy(isoHeader8583.destination, nii, TAM_NII + 1);
            memcpy(isoHeader8583.source, nii, TAM_NII + 1);
            setHeader(isoHeader8583);

            setMTI((char *)"0100");
            setField(3, codigoProceso, 6);
            setField(11, systemTrace, 6);
            setField(22, entryMode, 3);
            setField(24, nii + 1, 3);
            setField(25, (char *)"00", 2);
            setField(35, datosTarjeta.track2, strlen(datosTarjeta.track2));
            setField(41, terminalId, 8);
            if (strlen(pinBlock) > 0) {
                setField(52, pinBlock, LEN_PIN_DATA);
            }

            resultadoIsoPackMessage = packISOMessage();

            if (resultadoIsoPackMessage.responseCode > 0) {

                setParameter(VERIFICAR_PAPEL_TERMINAL, (char *)"00");
                resultadoTransaccion = realizarTransaccion(resultadoIsoPackMessage.isoPackMessage,
                                                           resultadoIsoPackMessage.totalBytes, dataRecibida, TRANSACCION_SALDO, CANAL_DEMANDA,
                                                           REVERSO_NO_GENERADO);

                setParameter(VERIFICAR_PAPEL_TERMINAL, (char *)"01");
            }
            /////// SI LA TRANSACCION ES EXITOSA ///////
            if (resultadoTransaccion > 0) {

                resultadoUnpack = unpackISOMessage(dataRecibida, resultadoTransaccion);
                isoFieldMessage = getField(39);
                memcpy(codigoRespuesta, isoFieldMessage.valueField, isoFieldMessage.totalBytes);
                ///// SI EL CODIGO DE RESPUESTA ES OK //////
                if (strcmp(codigoRespuesta, "00") == 0) {
                    uChar stringAux[20 + 1];
                    memset(stringAux, 0x00, sizeof(stringAux));
                    isoFieldMessage = getField(4);
                    memcpy(saldoDisponible, isoFieldMessage.valueField, strlen(isoFieldMessage.valueField) - 2);
                    memset(stringAux, 0x00, sizeof(stringAux));
                    formatString(0x30, 0, saldoDisponible, strlen(saldoDisponible), stringAux, 1);
                    strcpy(saldo, stringAux);
                    //getchAcceptCancel("CONSULTA SALDO", "SU SALDO ES", stringAux, "", 5 * 1000);
                    resultadoTransaccion = 1;
                    intentosPin = 0;

                } else {
                    resultadoTransaccion = 0;
                    errorRespuestaTransaccion(codigoRespuesta, mensajeError);

                    if (strcmp(codigoRespuesta, "55") == 0) {
                        intentosPin = 1;

//                        datosTarjeta.dataIssuer.options1.pinEntry = 1;

                    } else if (strcmp(codigoRespuesta, "75") == 0) {
                        intentosPin = 0;
                    } else {
                        intentosPin = 0;
                    }
                }
            }
        }

    } while (resultadoSaldo > 0 && intentosPin == 1);
    //   }

    return resultadoTransaccion;
}
