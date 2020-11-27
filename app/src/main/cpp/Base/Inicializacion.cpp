//
// Created by NETCOM on 26/11/2018.
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
#include <venta.h>
#include <bancolombia.h>
#include "Utilidades.h"
#include "Mensajeria.h"
#include "Inicializacion.h"
#include "android/log.h"
#include "Archivos.h"
#include "configuracion.h"
#include "Tipo_datos.h"
#include "native-lib.h"
#include "comunicaciones.h"

#define  LOG_TAG    "NETCOM_INICIALIZACION"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)


//INICIALIZACION//

char tipoCanal[2 + 1];
int isCnb = 0;
//int conectado = 0;
//char pathNetcom[60 + 1];
const char *internalStoragePath;
RangoBinesEncontrados RangoBinesEncontrados_[12 + 1];

int invocarInicializacionGeneral(char *codigoProcesoInicial) {

    int resultado = 0;
    char tamanoTabla[2 + 1];
    char codigoProceso[6 + 1];
    char nombreArchivo[20 + 1];
    char nombreArchivoActual[20 + 1];
    FILE *pxFid = NULL;
    int contador = 1;
    uChar tablaCero = 0x00;
    uChar tablaUno = 0x01;
    uChar dataRecibidaAux[1024];
    uChar data[25 + 1];
    uChar consecutivo[6 + 1];
    uChar numeroTabla;
    uChar numeroTabla_;
    uChar numeroTerminalAux[SIZE_NUMERO_TERMINAL + 1];
    uChar inicializacionCambioTerminal[1 + 1];
    ResultISOPack resultadoIsoPackMessage;
    ResultISOUnpack resultadoUnpack;
    ISOHeader isoHeader8583;
    ISOFieldMessage isoFieldMessage;
    TablaCuatroInicializacion tablaCuatro;
    uChar terminal[8 + 1];
    uChar texto[15 + 1];
    uChar message[80 + 1];
    uChar modoQr[3 + 1];
    uChar working3DES[16 + 1];
    char niiMultipos[4 + 1];
    char rutaFile[60 + 1];

    memset(niiMultipos, 0x00, sizeof(niiMultipos));
    memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
    memset(&isoHeader8583, 0x00, sizeof(isoHeader8583));
    memset(dataRecibidaAux, 0x00, sizeof(dataRecibidaAux));
    memset(codigoProceso, 0x00, sizeof(codigoProceso));
    memset(tamanoTabla, 0x00, sizeof(tamanoTabla));
    memset(&resultadoUnpack, 0x00, sizeof(resultadoUnpack));
    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(consecutivo, 0x00, sizeof(consecutivo));
    memset(nombreArchivo, 0x00, sizeof(nombreArchivo));
    memset(nombreArchivoActual, 0x00, sizeof(nombreArchivoActual));
    memset(terminal, 0x00, sizeof(terminal));
    memset(&tablaCuatro, 0x00, sizeof(tablaCuatro));
    memset(texto, 0x00, sizeof(texto));
    memset(message, 0x00, sizeof(message));
    memset(numeroTerminalAux, 0x00, sizeof(numeroTerminalAux));
    memset(inicializacionCambioTerminal, 0x00, sizeof(inicializacionCambioTerminal));
    memset(data, 0x00, sizeof(data));
    memset(modoQr, 0x00, sizeof(modoQr));
    memset(working3DES, 0x00, sizeof(working3DES));
    memset(rutaFile, 0x00, sizeof(rutaFile));

    getParameter(NUMERO_TERMINAL, terminal);
    getParameter(NUMERO_TERMINAL_AUXILIAR, numeroTerminalAux);

    uChar nii[TAM_NII + 1];
    memset(nii, 0x00, sizeof(nii));

    getParameter(NII, nii);

    isoHeader8583.TPDU = 60;
    memcpy(isoHeader8583.destination, nii, TAM_NII + 1);
    memcpy(isoHeader8583.source, nii, TAM_NII + 1);
    setHeader(isoHeader8583);

    generarSystemTraceNumber(consecutivo, pathNetcom);

    setMTI((char *) "0800");
    setField(3, codigoProcesoInicial, 6);
    setField(11, consecutivo, 6);
    setField(24, nii + 1, 3);
    setField(41, terminal, 8);
    setField(60, (char *) "EFT30_NET01", 11);
    resultadoIsoPackMessage = packISOMessage();
    setParameter(VERIFICAR_PAPEL_TERMINAL, (char *) "00");

    getParameter(INICIALIZACION_CAMBIO_TERMINAL, inicializacionCambioTerminal);

    if (strcmp(inicializacionCambioTerminal, (char *) "1") == 0) {
        respaldoTablas();
        setParameter(TERMINAL_INICIALIZADA, (char *) "2");
    }

    memset(inicializacionCambioTerminal, 0x00, sizeof(inicializacionCambioTerminal));

    resultado = realizarTransaccion(resultadoIsoPackMessage.isoPackMessage,
                                    resultadoIsoPackMessage.totalBytes,
                                    dataRecibidaAux, TRANSACCION_INICIALIZACION, CANAL_PERMANENTE,
                                    REVERSO_NO_GENERADO);

    //release();
    LOGI("resultado transaccion %d ", resultado);
    if (resultado > 0) {

        respaldoTablas();
        uChar varAux[1 + 1];

        memset(varAux, 0x00, sizeof(varAux));

        getParameter(TERMINAL_INICIALIZADA, varAux);

        setParameter(TERMINAL_INICIALIZADA_ANTERIOR, varAux);

        if (strcmp(varAux, "3") !=
            0) { // si no viene de una telecarga borre hora y fecha de la inicializacion
            memset(data, 0x30, sizeof(data));
            setParameter(TIME_ULTIMA_TELECARGA_EXITOSA, data);
        }

        setParameter(TERMINAL_INICIALIZADA, (char *) "2");

        contador++;
        resultadoUnpack = unpackISOMessage(dataRecibidaAux, resultado);

        isoFieldMessage = getField(3);
        strcpy(codigoProceso, isoFieldMessage.valueField);
        memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
        isoFieldMessage = getField(60);
        numeroTabla_ = isoFieldMessage.valueField[0];
        numeroTabla = isoFieldMessage.valueField[3]; //01
        /// TABLA 1 ///
        if (numeroTabla == tablaUno && numeroTabla_ == tablaCero) {
            memset(nombreArchivo, 0x00, sizeof(nombreArchivo));
            sprintf(nombreArchivo, TABLA_03_D, numeroTabla);
            memset(rutaFile, 0x00, sizeof(rutaFile));

            escribirArchivo(discoNetcom, nombreArchivo, isoFieldMessage.valueField,
                            isoFieldMessage.totalBytes);
            //cerrarArchivo();
            strcpy(nombreArchivoActual, nombreArchivo);

            ///////////////////////////////////////////////////////////

            while (strcmp(codigoProceso, "930001") == 0) {

                memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
                memset(&isoHeader8583, 0x00, sizeof(isoHeader8583));
                memset(&resultadoUnpack, 0x00, sizeof(resultadoUnpack));
                memset(codigoProceso, 0x00, sizeof(codigoProceso));
                memset(dataRecibidaAux, 0x00, sizeof(dataRecibidaAux));
                memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
                memset(consecutivo, 0x00, sizeof(consecutivo));
                memset(message, 0x00, sizeof(message));

                isoHeader8583.TPDU = 60;
                memcpy(isoHeader8583.destination, nii, TAM_NII + 1);
                memcpy(isoHeader8583.source, nii, TAM_NII + 1);
                setHeader(isoHeader8583);

                generarSystemTraceNumber(consecutivo, pathNetcom);

                setMTI((char *) "0800");
                setField(3, (char *) "930001", 6);
                setField(11, consecutivo, 6);
                setField(24, nii + 1, 3);
                setField(41, terminal, 8);
                setField(60, (char *) "EFT30_NET01", 11);

                resultadoIsoPackMessage = packISOMessage();
                setParameter(VERIFICAR_PAPEL_TERMINAL, (char *) "00");

                if (contador < 400) {
                    resultado = realizarTransaccion(resultadoIsoPackMessage.isoPackMessage,
                                                    resultadoIsoPackMessage.totalBytes,
                                                    dataRecibidaAux, TRANSACCION_INICIALIZACION,
                                                    CANAL_PERMANENTE, REVERSO_NO_GENERADO);
                    char varAux[25+ 1] = {0x00};
                    //sprintf(varAux, "%d", contador);
                    sprintf(varAux, "%s %d" ,"INICIALIZANDO... ", contador);
                    enviarStringToJava(varAux, (char *)"mostrarInicializacion");
                 //release();
                } else {
                    resultado = -1;
                }

                if (resultado > 0) {
                    contador++;
                    resultadoUnpack = unpackISOMessage(dataRecibidaAux, resultado);

                    isoFieldMessage = getField(3);
                    strcpy(codigoProceso, isoFieldMessage.valueField);

                    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
                    isoFieldMessage = getField(60);
                    numeroTabla = isoFieldMessage.valueField[0];

                    memset(nombreArchivo, 0x00, sizeof(nombreArchivo));
                    if (strcmp(codigoProceso, "942000") == 0) {
                        strcpy(nombreArchivo, TABLA_PARAMETROS);

                    } else {
                        sprintf(nombreArchivo, TABLA_03_D, numeroTabla);
                    }

                    if (strcmp(nombreArchivoActual, nombreArchivo) != 0) {
                        strcpy(nombreArchivoActual, nombreArchivo);

                        if (pxFid != NULL) {
                            cerrarArchivo(pxFid);
                        }

                        pxFid = abrirArchivo(discoNetcom, nombreArchivo, (char *) "a");
                    }

                    operacionArchivo(pxFid, 2, isoFieldMessage.valueField, 0,
                                     isoFieldMessage.totalBytes);

                } else {
                    getParameter(INICIALIZACION_CAMBIO_TERMINAL, inicializacionCambioTerminal);
                    if (strcmp(inicializacionCambioTerminal, "1") == 0) {
                        setParameter(INICIALIZACION_CAMBIO_TERMINAL, (char *) "0");
                        getParameter(NUMERO_TERMINAL_AUXILIAR, numeroTerminalAux);
                        LOGI("seteo 1 %s ", numeroTerminalAux);
                        setParameter(NUMERO_TERMINAL, numeroTerminalAux);
                    }
                    resultado = -1;
                    strcpy(codigoProceso, "999999");
                    break;
                }
            }
            cerrarArchivo(pxFid);
            //otras tablas necesarias
            if (resultado > 0) {
                if (verificarArchivo(discoNetcom, TABLA_PARAMETROS) == FS_OK) {

                    resultado = desempaquetarTablaParametros();

                    if (resultado < 0) {
                        memset(terminal, 0x00, sizeof(terminal));
                        getParameter(TERMINAL_INICIALIZADA, terminal);
                        setParameter(TERMINAL_INICIALIZADA, terminal);

                        getParameter(INICIALIZACION_CAMBIO_TERMINAL, inicializacionCambioTerminal);
                        if (strcmp(inicializacionCambioTerminal, "1") == 0) {
                            setParameter(INICIALIZACION_CAMBIO_TERMINAL, (char *) "0");
                            getParameter(NUMERO_TERMINAL_AUXILIAR, numeroTerminalAux);
                            LOGI("seteo 2 %s ", numeroTerminalAux);
                            setParameter(NUMERO_TERMINAL, numeroTerminalAux);
                        }
                    }

                } else {

                    setParameter(MONTO_TRANSACCION_LIMITE, (char *) "0");
                    setParameter(MONTO_CVM_LIMITE, (char *) "0");
                    setParameter(MODO_PAYPASS, (char *) "0");

                    if (resultado > 0) {
                        borrarArchivo(discoNetcom, (char *) INICIALIZACION_MULTIPOS);
                        getParameter(NII_MULTIPOS, niiMultipos);

                        if (resultado > 0 && atoi(niiMultipos) > 0) {
                            //inicializarMultiPos();
                        } else if (atoi(niiMultipos) == 0) {

                            resultado = 1;
                        } else {
                            resultado = 1;
                        }
                    }
                }

                inicializacionConsultaInformacion();

                //desempaquetarAidEmv();

                setParameter(TERMINAL_INICIALIZADA, (char *) "1");
                setParameter(TIPO_INICIALIZACION, (char *) "M");
                if (atoi(codigoProcesoInicial) == atoi(INICIALIZACION_AUTOMATICA)) {
                    setParameter(TIPO_INICIALIZACION, (char *) "A");
                }

                configurarTime();
                /////// CONFIGURAR DATOS DE COMUNICACION //////////
                tablaCuatro = _desempaquetarTablaCuatro_();

                configurarDatosComunicacion(tablaCuatro);

                ///////// WORKING KEY 3DES ////////////
                memcpy(working3DES, tablaCuatro.encryptedPinKey, 8);
                memcpy(working3DES + 8, tablaCuatro.encryptedPinKey2, 8);

                ///////////////////////////////////////////////////
                borrarTablas(1);

                setParameter(WORKING_KEY_LOGON_3DES, (char *) "0");

                _inicializacionSerial_();
                /// Primero debe cerrar la primera terminal antes de inicializar la segunda
                // resultado = verificarModoCNB();
                // if (resultado == MODO_MIXTO) {
                //_inicializacionCNB_(INICIALIZACION_MANUAL, texto, 1);
                // }

                ///////// INICIALIZACION QRCODE  ////////
                getParameter(MODO_QR, modoQr);
                resultado = atol(modoQr);
                if (resultado == 1) {
                    //_inicializacionQR_(INICIALIZACION_MANUAL, texto, 1);
                }
                ///////////////////////////////////////////
                //cerrarSocket();

                memset(numeroTerminalAux, 0x00, sizeof(numeroTerminalAux));
                getParameter(NUMERO_TERMINAL, numeroTerminalAux);
                setParameter(NUMERO_TERMINAL_AUXILIAR, numeroTerminalAux);

                habilitarMultiplesCorresponsales();

                enviarStringToJava((char *)" INICIALIZACION_EXITOSA", (char *)"mostrarInicializacion");
                //buzzer(10);
                //printMessage(strlen(INICIALIZACION_EXITOSA), INICIALIZACION_EXITOSA);
                //printMessage(strlen(REINICIAR_TERMINAL), REINICIAR_TERMINAL);
                //Telium_Exit(19);

            } else {
                //cerrarSocket();
                getParameter(INICIALIZACION_CAMBIO_TERMINAL, inicializacionCambioTerminal);
                if (strcmp(inicializacionCambioTerminal, "1") == 0) {
                    setParameter(INICIALIZACION_CAMBIO_TERMINAL, (char *) "0");
                    getParameter(NUMERO_TERMINAL_AUXILIAR, numeroTerminalAux);
                    LOGI("seteo 3 %s ", numeroTerminalAux);
                    setParameter(NUMERO_TERMINAL, numeroTerminalAux);
                }
                _restaurarInicializacion_();
                resultado = -1;
            }
            //////////////////////////////////////////////
        } else {
            memset(terminal, 0x00, sizeof(terminal));
            getParameter(TERMINAL_INICIALIZADA, terminal);
            setParameter(TERMINAL_INICIALIZADA, terminal);
            getParameter(INICIALIZACION_CAMBIO_TERMINAL, inicializacionCambioTerminal);
            if (strcmp(inicializacionCambioTerminal, "1") == 0) {
                setParameter(INICIALIZACION_CAMBIO_TERMINAL, (char *) "0");
                getParameter(NUMERO_TERMINAL_AUXILIAR, numeroTerminalAux);
                LOGI("seteo 4 %s ", numeroTerminalAux);
                setParameter(NUMERO_TERMINAL, numeroTerminalAux);
            }
            _restaurarInicializacion_();
            resultado = -1;
        }
    } else {

        enviarStringToJava((char *)"LA INICIALIZACION NO FUE EXITOSA", (char *)"mostrarInicializacion");

        memset(terminal, 0x00, sizeof(terminal));
        getParameter(TERMINAL_INICIALIZADA, terminal);
        setParameter(TERMINAL_INICIALIZADA, terminal);

        getParameter(INICIALIZACION_CAMBIO_TERMINAL, inicializacionCambioTerminal);
        if (strcmp(inicializacionCambioTerminal, "1") == 0) {
            setParameter(INICIALIZACION_CAMBIO_TERMINAL, (char *) "0");
            getParameter(NUMERO_TERMINAL_AUXILIAR, numeroTerminalAux);
            LOGI("seteo 5 %s ", numeroTerminalAux);
            setParameter(NUMERO_TERMINAL, numeroTerminalAux);
        }
    }
    setParameter(VERIFICAR_PAPEL_TERMINAL, (char *) "01");

    return resultado;
}

void configurarTime(void) {

    TablaUnoInicializacion configuracionTerminal;
    uChar timeTerminal[12 + 1];

    memset(timeTerminal, 0x00, sizeof(timeTerminal));
    memset(&configuracionTerminal, 0x00, sizeof(configuracionTerminal));
    configuracionTerminal = _desempaquetarTablaCeroUno_();
    _convertBCDToASCII_(timeTerminal, configuracionTerminal.dateandTime, 12);

    setParameter(TIME_ULTIMA_INICIALIZACION_EXITOSA, timeTerminal);

    Date date;

    memcpy(date.year, timeTerminal, 2);
    memcpy(date.month, timeTerminal + 2, 2);
    memcpy(date.day, timeTerminal + 4, 2);
    memcpy(date.hour, timeTerminal + 6, 2);
/*    memcpy(date.minute, timeTerminal + 8, 2);
    memcpy(date.second, timeTerminal + 10, 2);
    Telium_Write_date(&date);*/

}

void respaldoTablas(void) {

    int resultado = 0;
    int indiceAid = 0;
    char nombreActual[20 + 1];
    char nombreNuevo[20 + 1];
    int numeroTabla = 0;
    char rutaFile[60 + 1];
    char rutaFileNuevo[60 + 1];

    for (numeroTabla = 1; numeroTabla < 100; numeroTabla++) {
        memset(nombreActual, 0x00, sizeof(nombreActual));
        memset(nombreNuevo, 0x00, sizeof(nombreNuevo));
        sprintf(nombreActual, TABLA_03_D, numeroTabla);
        sprintf(nombreNuevo, TABLA_03_D_R, numeroTabla);

        resultado = verificarArchivo(pathNetcom, nombreActual);
        if (resultado == FS_OK) {
            renombrarArchivo(pathNetcom, nombreActual, nombreNuevo);
        }
    }

    renombrarArchivo(pathNetcom, (char *) TABLA_PARAMETROS, (char *) TABLA_PARAMETROS_R);
    renombrarArchivo(pathNetcom, (char *) CAKEYS, (char *) CAKEYS_R);
    renombrarArchivo(pathNetcom, (char *) TABLA_EXCEPCIONES, (char *) TABLA_EXCEPCIONES_R);
    renombrarArchivo(pathNetcom, (char *) INICIALIZACION_MULTIPOS,
                     (char *) INICIALIZACION_MULTIPOS_R);
    renombrarArchivo(pathNetcom, (char *) WORKING_MULTIPOS, (char *) WORKING_MULTIPOS_R);
    renombrarArchivo(pathNetcom, (char *) TX_BONOS, (char *) TX_BONOS_R);
    renombrarArchivo(pathNetcom, (char *) INICIALIZACION_PUNTOS, (char *) INICIALIZACION_PUNTOS_R);
    renombrarArchivo(pathNetcom, (char *) INICIALIZACION_EMISORES,
                     (char *) INICIALIZACION_EMISORES_R);
    renombrarArchivo(pathNetcom, (char *) INICIALIZACION_BI_EMISORES,
                     (char *) INICIALIZACION_BI_EMISORES_R);
    renombrarArchivo(pathNetcom, (char *) TABLA_CI, (char *) TABLA_CI_R);
    renombrarArchivo(pathNetcom, (char *) TABLA_CNB, (char *) TABLA_CNB_R);
    renombrarArchivo(pathNetcom, (char *) TABLA_DCC, (char *) TABLA_DCC_R);
    renombrarArchivo(pathNetcom, (char *) RUTA_TABLA_MULTICORRESPONSAL,
                     (char *) RUTA_TABLA_MULTICORRESPONSAL_R);

    for (indiceAid = 1; indiceAid < 25; indiceAid++) {
        memset(nombreActual, 0x00, sizeof(nombreActual));
        memset(nombreNuevo, 0x00, sizeof(nombreNuevo));
        sprintf(nombreActual, "/AID%d.PAR", indiceAid);
        sprintf(nombreNuevo, "/AID%d.PARR", indiceAid);
        resultado = verificarArchivo(pathNetcom, nombreActual);
        if (resultado == FS_OK) {
            renombrarArchivo(pathNetcom, nombreActual, nombreNuevo);
        }
    }

    borrarTablas(0);

}


TablaCuatroInicializacion _desempaquetarTablaCuatro_(void) {
    ///Tabla Adquirente
    //La tabla del adquirente se utiliza para definir
    //los parametros asociados con el comprador o host.

    uChar variableBits[8 + 1];
    uChar dataTablaCuatro[1024];
    uChar working3DES[17 + 1];
    uChar working3DESApos[16 + 1];
    char rutaFile[60 + 1];
    int indice = 3;

    TablaCuatroInicializacion desempaquetarTablaCuatro;
    memset(&desempaquetarTablaCuatro, 0x00, sizeof(desempaquetarTablaCuatro));
    memset(dataTablaCuatro, 0x00, sizeof(dataTablaCuatro));
    memset(working3DES, 0x00, sizeof(working3DES));
    memset(working3DESApos, 0x00, sizeof(working3DESApos));

    leerArchivo(discoNetcom, (char *) TABLA_4_INICIALIZACION, dataTablaCuatro);

    desempaquetarTablaCuatro.acquirerNumber = dataTablaCuatro[indice];
    indice += 1;
    desempaquetarTablaCuatro.acquirerId = dataTablaCuatro[indice];
    indice += 1;
    memcpy(desempaquetarTablaCuatro.acquirerProgramName, dataTablaCuatro + indice, 10);
    indice += 10;
    memcpy(desempaquetarTablaCuatro.acquirerName, dataTablaCuatro + indice, 10);
    indice += 10;
    memcpy(desempaquetarTablaCuatro.primaryTransactionTelephoneNumber, dataTablaCuatro + indice,
           12);
    indice += 12;
    desempaquetarTablaCuatro.primaryTransactionConnectionTime = dataTablaCuatro[indice];
    indice += 1;
    desempaquetarTablaCuatro.primaryTransactionDialAttempts = dataTablaCuatro[indice];
    indice += 1;
    memcpy(desempaquetarTablaCuatro.secondaryTransactionTelephoneNumber, dataTablaCuatro + indice,
           12);
    indice += 12;
    desempaquetarTablaCuatro.secondaryTransactionConnectionTime = dataTablaCuatro[indice];
    indice += 1;
    desempaquetarTablaCuatro.secondaryTransactionDialAttempts = dataTablaCuatro[indice];
    indice += 1;
    memcpy(desempaquetarTablaCuatro.primarySettlementTelephoneNumber, dataTablaCuatro + indice, 12);
    indice += 12;
    desempaquetarTablaCuatro.primarySettlementConnectionTime = dataTablaCuatro[indice];
    indice += 1;
    desempaquetarTablaCuatro.primarySettlementDialAttempts = dataTablaCuatro[indice];
    indice += 1;
    memcpy(desempaquetarTablaCuatro.secondarySettlementTelephoneNumber, dataTablaCuatro + indice,
           12);
    indice += 12;
    desempaquetarTablaCuatro.secondarySettlementConnectionTime = dataTablaCuatro[indice];
    indice += 1;
    desempaquetarTablaCuatro.secondarySettlementDialAttempts = dataTablaCuatro[indice];
    indice += 1;
    desempaquetarTablaCuatro.modemModeTransactions = dataTablaCuatro[indice];
    indice += 1;
    ///////options 1///////
    memset(variableBits, 0x00, sizeof(variableBits));
    memcpy(variableBits, dataTablaCuatro + indice, 1);
    calcularBitsEncendidos(variableBits);
    desempaquetarTablaCuatro.options1.reservedByHypercom0 = variableBits[7] - '0';
    desempaquetarTablaCuatro.options1.reservedByHypercom1 = variableBits[6] - '0';
    desempaquetarTablaCuatro.options1.reservedByHypercom2 = variableBits[5] - '0';
    desempaquetarTablaCuatro.options1.ignoreHostTime = variableBits[4] - '0';
    desempaquetarTablaCuatro.options1.reservedByHypercom4 = variableBits[3] - '0';
    desempaquetarTablaCuatro.options1.reservedByHypercom5 = variableBits[2] - '0';
    desempaquetarTablaCuatro.options1.reservedByHypercom6 = variableBits[1] - '0';
    desempaquetarTablaCuatro.options1.reservedByHypercom7 = variableBits[0] - '0';
    ///////options 1///////

    indice += 1;
    ///////options 2///////
    memset(variableBits, 0x00, sizeof(variableBits));
    memcpy(variableBits, dataTablaCuatro + indice, 1);
    calcularBitsEncendidos(variableBits);
    desempaquetarTablaCuatro.options2.reservedByHypercom0 = variableBits[7] - '0';
    desempaquetarTablaCuatro.options2.reservedByHypercom1 = variableBits[6] - '0';
    desempaquetarTablaCuatro.options2.printVisaIIMerchantNumberInsteadOfAcquirerId =
            variableBits[5] - '0';
    desempaquetarTablaCuatro.options2.sendCurrentBatchNumberInMessages = variableBits[4] - '0';
    desempaquetarTablaCuatro.options2.macKeyExchangeMethod = variableBits[3] - '0';
    desempaquetarTablaCuatro.options2.PinKeyExchangeMethod = variableBits[2] - '0';
    desempaquetarTablaCuatro.options2.disableStatisticsFollowingSettlement = variableBits[1] - '0';
    desempaquetarTablaCuatro.options2.disableNbdHeader = variableBits[0] - '0';
    ///////options 2///////

    indice += 1;
    ///////options 3///////
    memset(variableBits, 0x00, sizeof(variableBits));
    memcpy(variableBits, dataTablaCuatro + indice, 1);
    calcularBitsEncendidos(variableBits);
    desempaquetarTablaCuatro.options3.disable0220Messages = variableBits[7] - '0';
    desempaquetarTablaCuatro.options3.achTotalsReportOnSettlement = variableBits[6] - '0';
    desempaquetarTablaCuatro.options3.nonSettlementAcquirer = variableBits[5] - '0';
    desempaquetarTablaCuatro.options3.hdcMode = variableBits[4] - '0';
    desempaquetarTablaCuatro.options3.unused4 = variableBits[3] - '0';
    desempaquetarTablaCuatro.options3.unused5 = variableBits[2] - '0';
    desempaquetarTablaCuatro.options3.unused6 = variableBits[1] - '0';
    desempaquetarTablaCuatro.options3.unused7 = variableBits[0] - '0';
    ///////options 3///////

    indice += 1;
    desempaquetarTablaCuatro.options4 = dataTablaCuatro[indice];
    indice += 1;
    memcpy(desempaquetarTablaCuatro.NetworkInternationalId, dataTablaCuatro + indice, 2);
    indice += 2;
    memcpy(desempaquetarTablaCuatro.cardAcceptorTerminalId, dataTablaCuatro + indice, 8);
    indice += 8;
    memcpy(desempaquetarTablaCuatro.cardAcceptorIdentificationCode, dataTablaCuatro + indice, 15);
    indice += 15;
    desempaquetarTablaCuatro.timeOut = dataTablaCuatro[indice];
    indice += 1;
    memcpy(desempaquetarTablaCuatro.currentBatchNumber, dataTablaCuatro + indice, 3);
    indice += 3;
    memcpy(desempaquetarTablaCuatro.nextBatchNumber, dataTablaCuatro + indice, 3);
    indice += 3;
    memcpy(desempaquetarTablaCuatro.settleTime, dataTablaCuatro + indice, 4);
    indice += 4;
    memcpy(desempaquetarTablaCuatro.settleDay, dataTablaCuatro + indice, 2);
    indice += 2;
    memcpy(desempaquetarTablaCuatro.encryptedPinKey, dataTablaCuatro + indice, 8);
    indice += 8;
    desempaquetarTablaCuatro.masterPinKeyIndex = dataTablaCuatro[indice];
    indice += 1;
    memcpy(desempaquetarTablaCuatro.encryptedMacKey, dataTablaCuatro + indice, 8);
    indice += 8;
    desempaquetarTablaCuatro.encryptedMacKeyIndex = dataTablaCuatro[indice];
    indice += 1;
    memcpy(desempaquetarTablaCuatro.visaITerminalId, dataTablaCuatro + indice, 23);
    indice += 23;
    desempaquetarTablaCuatro.modemModeSettlement = dataTablaCuatro[indice];
    indice += 1;
    desempaquetarTablaCuatro.maximumAutosettlementAttempts = dataTablaCuatro[indice];
    indice += 1;
    desempaquetarTablaCuatro.settlementLogonId = dataTablaCuatro[indice];
    indice += 3;
    memcpy(desempaquetarTablaCuatro.settlementProtocolName, dataTablaCuatro + indice, 10);
    indice += 10;
    memcpy(desempaquetarTablaCuatro.AcquiringInstitutionIdCode, dataTablaCuatro + indice, 6);
    indice += 31;

    ///// WORKING KEY 3DES ////
    if (dataTablaCuatro[indice] == '2') {
        indice += 1;
        memcpy(desempaquetarTablaCuatro.encryptedPinKey2, dataTablaCuatro + indice, 8);
    } else {
        memcpy(desempaquetarTablaCuatro.encryptedPinKey2, desempaquetarTablaCuatro.encryptedPinKey,
               8);
    }

    /////// WORKING KEY 3DES LOGON? //////////
    getParameter(WORKING_KEY_LOGON_3DES, working3DES);

    if (working3DES[0] == '1') {
        memset(desempaquetarTablaCuatro.encryptedPinKey, 0x00, sizeof(desempaquetarTablaCuatro.encryptedPinKey));
        memset(desempaquetarTablaCuatro.encryptedPinKey2, 0x00, sizeof(desempaquetarTablaCuatro.encryptedPinKey2));
        memcpy(desempaquetarTablaCuatro.encryptedPinKey, working3DES + 1, 8);
        memcpy(desempaquetarTablaCuatro.encryptedPinKey2, working3DES + 9, 8);
    }
    LOGI("WORKING KEY ALI 1 :%s  ",desempaquetarTablaCuatro.encryptedPinKey);
    memcpy( working3DESApos ,desempaquetarTablaCuatro.encryptedPinKey, 8);
    LOGI("WORKING KEY ALI 2 :%s  ",desempaquetarTablaCuatro.encryptedPinKey2);
    memcpy( working3DESApos + 8,desempaquetarTablaCuatro.encryptedPinKey2, 8);
    LOGI("WORKING KEY ALI final :%s  ",working3DESApos);
    setParameter(WORKING_KEY_APOS, working3DESApos);

    char auxiliarWorking[32 + 1] = {0x00};
    _convertBCDToASCII_(auxiliarWorking,working3DESApos, 32 );
    LOGI("working %s ", auxiliarWorking);
    enviarStringToJava((char *)auxiliarWorking, (char *)"mostrarInicializacion");
    //configura los datos de comunicacion
    //configurarDatosComunicacion(desempaquetarTablaCuatro);

    return desempaquetarTablaCuatro;
}


void _restaurarInicializacion_(void) {

    int resultado = 0;
    int iRet = 0;
    int indiceAid = 0;
    char nombreActual[20 + 1];
    char nombreNuevo[20 + 1];
    uChar numeroTabla = 0x00;
    uChar inicializacionCambioTerminal[1 + 1];
    uChar numeroTerminalAux[SIZE_NUMERO_TERMINAL + 1];

    memset(numeroTerminalAux, 0x00, sizeof(numeroTerminalAux));
    memset(inicializacionCambioTerminal, 0x00, sizeof(inicializacionCambioTerminal));

    getParameter(NUMERO_TERMINAL_AUXILIAR, numeroTerminalAux);

    //borrarTablas(0);
    iRet = tamArchivo(pathNetcom, (char *) TABLA_1_INICIALIZACION_R);

    if (iRet > 0) {

        for (numeroTabla = 1; numeroTabla < 100; numeroTabla++) {
            memset(nombreActual, 0x00, sizeof(nombreActual));
            memset(nombreNuevo, 0x00, sizeof(nombreNuevo));
            sprintf(nombreActual, TABLA_03_D_R, numeroTabla);
            sprintf(nombreNuevo, TABLA_03_D, numeroTabla);

            resultado = verificarArchivo(pathNetcom, nombreActual);

            if (resultado == FS_OK) {
                borrarArchivo(pathNetcom, nombreNuevo);
                iRet = renombrarArchivo(pathNetcom, nombreActual, nombreNuevo);
            }
        }

        borrarArchivo(pathNetcom, (char *) TABLA_PARAMETROS);
        renombrarArchivo(pathNetcom, (char *) TABLA_PARAMETROS_R, (char *) TABLA_PARAMETROS);
        borrarArchivo(pathNetcom, (char *) CAKEYS_TXT);
        renombrarArchivo(pathNetcom, (char *) CAKEYS_R, (char *) CAKEYS);
        borrarArchivo(pathNetcom, (char *) TABLA_EXCEPCIONES);
        renombrarArchivo(pathNetcom, (char *) TABLA_EXCEPCIONES_R, (char *) TABLA_EXCEPCIONES);
        borrarArchivo(pathNetcom, (char *) INICIALIZACION_MULTIPOS);
        renombrarArchivo(pathNetcom, (char *) INICIALIZACION_MULTIPOS_R,
                         (char *) INICIALIZACION_MULTIPOS);
        borrarArchivo(pathNetcom, (char *) WORKING_MULTIPOS);
        renombrarArchivo(pathNetcom, (char *) WORKING_MULTIPOS_R, (char *) WORKING_MULTIPOS);
        borrarArchivo(pathNetcom, (char *) TX_BONOS);
        renombrarArchivo(pathNetcom, (char *) TX_BONOS_R, (char *) TX_BONOS);
        borrarArchivo(pathNetcom, (char *) INICIALIZACION_PUNTOS);
        renombrarArchivo(pathNetcom, (char *) INICIALIZACION_PUNTOS_R,
                         (char *) INICIALIZACION_PUNTOS);
        borrarArchivo(pathNetcom, (char *) INICIALIZACION_EMISORES);
        renombrarArchivo(pathNetcom, (char *) INICIALIZACION_EMISORES_R,
                         (char *) INICIALIZACION_EMISORES);
        borrarArchivo(pathNetcom, (char *) INICIALIZACION_BI_EMISORES);
        renombrarArchivo(pathNetcom, (char *) INICIALIZACION_BI_EMISORES_R,
                         (char *) INICIALIZACION_BI_EMISORES);
        borrarArchivo(pathNetcom, (char *) TABLA_CI);
        renombrarArchivo(pathNetcom, (char *) TABLA_CI_R, (char *) TABLA_CI);
        borrarArchivo(pathNetcom, (char *) TABLA_CNB);
        renombrarArchivo(pathNetcom, (char *) TABLA_CNB_R, (char *) TABLA_CNB);
        borrarArchivo(pathNetcom, (char *) TABLA_DCC);
        renombrarArchivo(pathNetcom, (char *) TABLA_DCC_R, (char *) TABLA_DCC);
        borrarArchivo(pathNetcom, (char *) RUTA_TABLA_MULTICORRESPONSAL);

        renombrarArchivo(pathNetcom, (char *) RUTA_TABLA_MULTICORRESPONSAL_R,
                         (char *) RUTA_TABLA_MULTICORRESPONSAL);

        for (indiceAid = 1; indiceAid < 25; indiceAid++) {
            LOGI("en el for fiesta  ");
            memset(nombreActual, 0x00, sizeof(nombreActual));
            memset(nombreNuevo, 0x00, sizeof(nombreNuevo));
            sprintf(nombreActual, "/AID%d.PARR", indiceAid);
            sprintf(nombreNuevo, "/AID%d.PAR", indiceAid);

            resultado = verificarArchivo(pathNetcom, nombreActual);

            if (resultado == FS_OK) {
                borrarArchivo(pathNetcom, nombreNuevo);
                renombrarArchivo(pathNetcom, nombreActual, nombreNuevo);
            }

        }
        LOGI("sale del  for fiesta  ");
        setParameter(TERMINAL_INICIALIZADA, (char *) "1");
        habilitarMultiplesCorresponsales();
        LOGI("habilitarMultiplesCorresponsales ");

        getParameter(INICIALIZACION_CAMBIO_TERMINAL, inicializacionCambioTerminal);
        if (strcmp(inicializacionCambioTerminal, "1") == 0) {
            setParameter(INICIALIZACION_CAMBIO_TERMINAL, (char *) "0");
            getParameter(NUMERO_TERMINAL_AUXILIAR, numeroTerminalAux);
            setParameter(NUMERO_TERMINAL, numeroTerminalAux);
        }


    } else {
        setParameter(TERMINAL_INICIALIZADA, (char *) "0");
    }
    LOGI("Termino de restaurar ");
}


void borrarTablas(int tipoTabla) {

    int resultado = 0;
    int indiceAid = 0;
    char nombreArchivo[20 + 1];
    int numeroTabla = 0;
    char aux[1 + 1];

    memset(aux, 0x00, sizeof(aux));
    if (tipoTabla == 1) {
        strcpy(aux, "R");
    }
    if (tipoTabla == 0) {
        strcpy(aux, "");
    }

    for (numeroTabla = 1; numeroTabla < 100; numeroTabla++) {
        memset(nombreArchivo, 0x00, sizeof(nombreArchivo));
        sprintf(nombreArchivo, "/T-%03d.TXT%s", numeroTabla, aux);

        resultado = verificarArchivo(pathNetcom, nombreArchivo);

        if (resultado == FS_OK) {
            borrarArchivo(pathNetcom, nombreArchivo);
        }
    }

    memset(nombreArchivo, 0x00, sizeof(nombreArchivo));
    sprintf(nombreArchivo, "/TABPAR.TXT%s", aux);
    borrarArchivo(pathNetcom, nombreArchivo);

    memset(nombreArchivo, 0x00, sizeof(nombreArchivo));
    sprintf(nombreArchivo, "/CAKEYS.PAR%s", aux);
    borrarArchivo(pathNetcom, nombreArchivo);

    memset(nombreArchivo, 0x00, sizeof(nombreArchivo));
    sprintf(nombreArchivo, "/EXCEPC.TXT%s", aux);
    borrarArchivo(pathNetcom, nombreArchivo);

    memset(nombreArchivo, 0x00, sizeof(nombreArchivo));
    sprintf(nombreArchivo, "/MULTIP.TXT%s", aux);
    borrarArchivo(pathNetcom, nombreArchivo);

    memset(nombreArchivo, 0x00, sizeof(nombreArchivo));
    sprintf(nombreArchivo, "/WORKIN.TXT%s", aux);
    borrarArchivo(pathNetcom, nombreArchivo);

    memset(nombreArchivo, 0x00, sizeof(nombreArchivo));
    sprintf(nombreArchivo, "/BONOS.TXT%s", aux);
    borrarArchivo(pathNetcom, nombreArchivo);

    memset(nombreArchivo, 0x00, sizeof(nombreArchivo));
    sprintf(nombreArchivo, "/PUNTOS.TXT%s", aux);
    borrarArchivo(pathNetcom, nombreArchivo);

    memset(nombreArchivo, 0x00, sizeof(nombreArchivo));
    sprintf(nombreArchivo, "/EMISOR.TXT%s", aux);
    borrarArchivo(pathNetcom, nombreArchivo);

    memset(nombreArchivo, 0x00, sizeof(nombreArchivo));
    sprintf(nombreArchivo, "/BIEMIS.TXT%s", aux);
    borrarArchivo(pathNetcom, nombreArchivo);

    memset(nombreArchivo, 0x00, sizeof(nombreArchivo));
    sprintf(nombreArchivo, "/Tab-ci.TXT%s", aux);
    borrarArchivo(pathNetcom, nombreArchivo);

    memset(nombreArchivo, 0x00, sizeof(nombreArchivo));
    sprintf(nombreArchivo, "/Ta-CNB.TXT%s", aux);
    borrarArchivo(pathNetcom, nombreArchivo);


    for (indiceAid = 1; indiceAid < 25; indiceAid++) {
        memset(nombreArchivo, 0x00, sizeof(nombreArchivo));
        sprintf(nombreArchivo, "/AID%d.PAR%s", indiceAid, aux);
        borrarArchivo(pathNetcom, nombreArchivo);
    }

}


void configurarDatosComunicacion(TablaCuatroInicializacion tablaCuatro) {

    uChar input[100 + 1];
    uChar inputAux[100 + 1];
    char data[10 + 1];
    //TablaCuatroInicializacion tablaCuatro;

    memset(data, 0x00, sizeof(data));

    //SOCKET PRIMARIO VENTA
    memset(input, 0x00, sizeof(input));
    memset(inputAux, 0x00, sizeof(inputAux));
    _convertBCDToASCII_(input, tablaCuatro.primaryTransactionTelephoneNumber, 24);

    sprintf(inputAux, "%.3s.%.3s.%.3s.%.3s", &input[0], &input[3], &input[6], &input[9]);
    setParameter(IP_VENTA_PRIMARIA, inputAux);

    memset(inputAux, 0x00, sizeof(inputAux));
    sprintf(inputAux, "%.4s", &input[12]);
    setParameter(PUERTO_VENTA_PRIMARIA, inputAux);
    memset(input, 0x00, sizeof(input));
    _convertBCDToASCII_(input, &tablaCuatro.primaryTransactionConnectionTime, 2);
    setParameter(TIMEOUT_VENTA_PRIMARIA, input);
    memset(input, 0x00, sizeof(inputAux));
    _convertBCDToASCII_(input, &tablaCuatro.primaryTransactionDialAttempts, 2);
    setParameter(INTENTOS_VENTA_PRIMARIA, input);
    //printf("\x1B IN1:%s",input);ttestall(0,200);

    //SOCKET SECUNDARIO VENTA
    memset(input, 0x00, sizeof(input));
    memset(inputAux, 0x00, sizeof(inputAux));
    _convertBCDToASCII_(input, tablaCuatro.secondaryTransactionTelephoneNumber, 16);
    sprintf(inputAux, "%.3s.%.3s.%.3s.%.3s", &input[0], &input[3], &input[6], &input[9]);
    setParameter(IP_VENTA_SECUNDARIA, inputAux);
    memset(inputAux, 0x00, sizeof(inputAux));
    sprintf(inputAux, "%.4s", &input[12]);
    setParameter(PUERTO_VENTA_SECUNDARIA, inputAux);
    memset(input, 0x00, sizeof(input));
    _convertBCDToASCII_(input, &tablaCuatro.secondaryTransactionConnectionTime, 2);
    setParameter(TIMEOUT_VENTA_SECUNDARIA, input);
    memset(input, 0x00, sizeof(inputAux));
    _convertBCDToASCII_(input, &tablaCuatro.secondaryTransactionDialAttempts, 2);
    setParameter(INTENTOS_VENTA_SECUNDARIA, input);

    //SOCKET PRIMARIO CIERRE
    memset(input, 0x00, sizeof(input));
    memset(inputAux, 0x00, sizeof(inputAux));
    _convertBCDToASCII_(input, tablaCuatro.primarySettlementTelephoneNumber, 16);
    sprintf(inputAux, "%.3s.%.3s.%.3s.%.3s", &input[0], &input[3], &input[6], &input[9]);
    setParameter(IP_CIERRE_PRIMARIA, inputAux);
    memset(inputAux, 0x00, sizeof(inputAux));
    sprintf(inputAux, "%.4s", &input[12]);
    setParameter(PUERTO_CIERRE_PRIMARIA, inputAux);
    memset(input, 0x00, sizeof(input));
    _convertBCDToASCII_(input, &tablaCuatro.primarySettlementConnectionTime, 2);
    setParameter(TIMEOUT_CIERRE_PRIMARIA, input);
    memset(input, 0x00, sizeof(inputAux));
    _convertBCDToASCII_(input, &tablaCuatro.primarySettlementDialAttempts, 2);
    setParameter(INTENTOS_CIERRE_PRIMARIA, input);

    //SOCKET SECUNDARIO CIERRE
    memset(input, 0x00, sizeof(input));
    memset(inputAux, 0x00, sizeof(inputAux));
    _convertBCDToASCII_(input, tablaCuatro.secondarySettlementTelephoneNumber, 16);
    sprintf(inputAux, "%.3s.%.3s.%.3s.%.3s", &input[0], &input[3], &input[6], &input[9]);
    setParameter(IP_CIERRE_SECUNDARIA, inputAux);
    memset(inputAux, 0x00, sizeof(inputAux));
    sprintf(inputAux, "%.4s", &input[12]);
    setParameter(PUERTO_CIERRE_SECUNDARIA, inputAux);
    memset(input, 0x00, sizeof(input));
    _convertBCDToASCII_(input, &tablaCuatro.secondarySettlementConnectionTime, 2);
    setParameter(TIMEOUT_CIERRE_SECUNDARIA, input);
    memset(input, 0x00, sizeof(inputAux));
    _convertBCDToASCII_(input, &tablaCuatro.secondarySettlementDialAttempts, 2);
    setParameter(INTENTOS_CIERRE_SECUNDARIA, input);

    //TIMEOUT TRANSACCION
    memset(input, 0x00, sizeof(input));
    _convertBCDToASCII_(input, &tablaCuatro.timeOut, 2);
    setParameter(TIMEOUT_TRANSACCION, input);
}

void calcularBitsEncendidos(char *byte) {

    char bitMap[8 + 1];
    int i = 0;

    memset(bitMap, 0x00, sizeof(bitMap));

    for (i = 0; i < 8; i++) {
        if (*(byte + i / 8) & (0x80 >> i % 8))
            bitMap[i] = '1';
        else
            bitMap[i] = '0';
    }
    memcpy(byte, bitMap, 8);
}

void generarSystemTraceNumber(char *numeroRecibo, char *pathNetcom) {//eliminar pathNetrcom

    int contador = 0;
    char dataStrace[6 + 1];

    memset(dataStrace, 0x00, sizeof(dataStrace));
    memset(numeroRecibo, 0x00, sizeof(numeroRecibo));

    getParameter(CONSECUTIVO, dataStrace);
    contador = atoi(dataStrace);
    contador++;
    if (contador >= 999999) {
        contador = 1;
    }
    memset(dataStrace, 0x00, sizeof(dataStrace));
    sprintf(dataStrace, "%06d", contador);
    setParameter(CONSECUTIVO, dataStrace);
    memcpy(numeroRecibo, dataStrace, 6);

}

int desempaquetarTablaParametros(void) {

    int resultado = 0;
    int indicadorMultiPos = 0;
    uChar tablaPar[10 + 1];
    uChar modo[2 + 1];
    DatosIniQrCode datosQr;
    char terminalAuxiliar[10 + 1];
    char numeroTerminal[SIZE_NUMERO_TERMINAL + 1];
    char codigoBanco[SIZE_CODIGO_BANCO_CNB + 1];
    char niiMultipos[4 + 1];

    memset(niiMultipos, 0x00, sizeof(niiMultipos));
    memset(&datosQr, 0x00, sizeof(datosQr));
    memset(tablaPar, 0x00, sizeof(tablaPar));
    memset(modo, 0x00, sizeof(modo));
    memset(terminalAuxiliar, 0x20, sizeof(terminalAuxiliar));
    memset(numeroTerminal, 0x00, sizeof(numeroTerminal));
    memset(codigoBanco, 0x00, sizeof(codigoBanco));
    getParameter(CNB_TERMINAL, numeroTerminal);
    getParameter(CNB_BANCO, codigoBanco);

    setParameter(HABILITACION_VIVAMOS, (char *) "0");
    setParameter(HABILITACION_MODO_CNB_CITIBANK, (char *) "0");

    resultado = leerArchivo(discoNetcom, (char *) TABLA_PARAMETROS, tablaPar);

    if (resultado > 0) {
        ////////////////// INICIALIZANDO PUNTOS ////////////////
        if (tablaPar[4] & 0x80) {
            resultado = inicializacionPuntos();
            enviarStringToJava((char *)" INICIALIZANDO PUNTOS", (char *)"mostrarInicializacion");
            usleep(10000);
        }
        ////////////// INICIALIZANDO BONOS ////////////////
        if ((tablaPar[4] & 0x40) && resultado > 0) {
            //printMessage(strlen(INICIALIZANDO_PUNTOS), INICIALIZANDO_BONOS);
            resultado = inicializacionBonos();
            enviarStringToJava((char *)"INICIALIZANDO BONOS", (char *)"mostrarInicializacion");
            usleep(10000);
        }
        ////////////// INICIALIZANDO EMISORES ///////////////
        if ((tablaPar[4] & 0x08) && resultado > 0) {
            //printMessage(strlen(INICIALIZANDO_PUNTOS), INICIALIZANDO_EMISORES);
            resultado = inicializacionEmisores();
            enviarStringToJava((char *)"INICIALIZANDO EMISORES", (char *)"mostrarInicializacion");
            usleep(10000);
        }
        if (resultado > 0) {
            ////////////// INICIALIZANDO EXCEPCIONES EMV //////////////////
            //printMessage(strlen(MESSAGE_EXCEPCIONES), MESSAGE_EXCEPCIONES);
            LOGI("Inicializando excepciones   ");
            resultado = inicializarExcepciones(TRANSACCION_INICIALIZACION);
            enviarStringToJava((char *)"INICIALIZANDO EXCEPCIONES", (char *)"mostrarInicializacion");
            usleep(10000);
        }
        LOGI("salio excepciones   ");
        if (tablaPar[6] & 0x20) {
            inicializarMulticorresponsal();
        }

        ////////////// INICIALIZANDO QRCODE ////////////////
        if (tablaPar[6] & 0x10 || tablaPar[6] & 0x08) {
            inicializarQRCODE();
            enviarStringToJava((char *)"Inicializando Qrcode", (char *)"mostrarInicializacion");
            if (tablaPar[6] & 0x08) { // SOLO ESTA ACTIVADO  TERPEL
                setParameter(MODO_CONSULTA, (char *) "1");
            } else {
                setParameter(MODO_CONSULTA, (char *) "0");
            }
            if (tablaPar[6] & 0x10) { // SOLO ESTA ACTIVADO QR
                datosQr = capturarDatosQrCodeRbm();
                if (strncmp(terminalAuxiliar, datosQr.terminal, 5) != 0) {
                    setParameter(MODO_QR, (char *) "1");
                } else {
                    setParameter(MODO_QR, (char *) "0");
                }
            } else {
                setParameter(MODO_QR, (char *) "0");
            }
        } else {
            setParameter(MODO_QR, (char *) "0");
            setParameter(MODO_CONSULTA, (char *) "0");
        }
        LOGI("Inicializando Flujo ");
        /////////// INICIALIZANDO FLUJO UNIFICADO /////////////
        if (tablaPar[6] & 0x04) {
            setParameter(FLUJO_UNIFICADO, (char *) "1");
            enviarStringToJava((char *)"FLUJO UNIFICADO ACTIVADO", (char *)"mostrarInicializacion");
            usleep(10000);
        } else {
            setParameter(FLUJO_UNIFICADO, (char *)"0");
            enviarStringToJava((char *)"FLUJO UNIFICADO DESACTIVADO", (char *)"mostrarInicializacion");
            usleep(10000);
        }

        /////////// INICIALIZANDO BIG - BANCOLOMBIA  /////////////

        setParameter(BIG_BANCOLOMBIA, (char *) "0");

        if (tablaPar[6] & 0x02) {
            inicializarBigBancolombia();
            enviarStringToJava((char *)"LIFEMILES", (char *)"mostrarInicializacion");
            usleep(10000);
            setParameter(BIG_BANCOLOMBIA, (char *) "1");
        }

        ////////////// INICIALIZANDO MULTIPOS ////////////////
        //Pregunta si desea Inicializar MultiPos.
        if ((tablaPar[4] & 0x04) && resultado > 0) {
            //resultado = getchAcceptCancel("INICIALIZACION", "DESEA INICIALIZAR", "MULTI POS", "CONFIRMAR?", 30 * 1000);
            indicadorMultiPos = -1;
            borrarArchivo(discoNetcom, (char *) INICIALIZACION_MULTIPOS);

            getParameter(NII_MULTIPOS, niiMultipos);
            if (resultado > 0 && atoi(niiMultipos) > 0) {
                indicadorMultiPos = 1;
                inicializarMultiPos();
            } else if (atoi(niiMultipos) == 0) {
                //screenMessage("COMUNICACION", "", "CONECTANDO", "", 1 * 1000);
                //screenMessage("COMUNICACION", "", "PROCESANDO", "", 1 * 1000);
                resultado = 1;
            } else {
                resultado = 1;
            }
        }
        if (resultado > 0) {
            // Inicializa MultiPos Directamente sin Preguntar.

            if (tablaPar[5] & 0x80 && indicadorMultiPos == 0) {
                borrarArchivo(discoNetcom, (char *) INICIALIZACION_MULTIPOS);
                LOGI("Inicializando multipos ");
                inicializarMultiPos();
            }

            /////////// CLUB VIVAMOS /////////////
            if (tablaPar[5] & 0x0C) {
                setParameter(HABILITACION_VIVAMOS, (char *) "1");
            }

            if (tablaPar[5] & 0x40) { // Se verifica si es CNB RBM
                sprintf(modo, "%02x", MODO_CNB_RBM);
                setParameter(HABILITACION_MODO_CNB, modo);
            }

            if (tablaPar[4] & 0x02) { // Se verifica si es CNB AVAL
                sprintf(modo, "%02x", MODO_CNB_AVAL);
                setParameter(HABILITACION_MODO_CNB_AVAL, modo);
            } else {
                setParameter(HABILITACION_MODO_CNB_AVAL, (char *) "00");
            }

            //tablaPar[5] = 0x02;
            if (tablaPar[5] & 0x02) { // Se verifica si esta habilitado CNB CitiBank
                setParameter(HABILITACION_MODO_CNB_CITIBANK, (char *) "1");
            } else {
                setParameter(HABILITACION_MODO_CNB_CITIBANK, (char *) "0");
            }

            if (((strlen(numeroTerminal) <= 0) &&
                 (strncmp(codigoBanco, "0000", SIZE_CODIGO_BANCO_CNB) == 0))
                || ((isCnb == 0) && strlen(numeroTerminal) <= 0)) {

                if (tablaPar[5] & 0x10) { // Se verifica si esta habilitado CNB Bancolombia
                    setParameter(HABILITACION_MODO_CNB_BCL, (char *) "1");
                    setParameter(HABILITACION_MODO_CNB_CORRESPONSAL, (char *) "0");
                    LOGI("Inicializando inicializarMulticorresponsal A ");
                    inicializarMulticorresponsal();
                    if (verificarArchivo(discoNetcom, RUTA_TABLA_MULTICORRESPONSAL) != FS_OK) {
                        setParameter(CNB_BANCO, (char *) "0000");
                    }

                } else if (tablaPar[6] & 0x01) { // Se verifica bancolombia modo JMC
                    setParameter(HABILITACION_MODO_CNB_BCL, (char *) BCL_JMR);
                    LOGI("Inicializando inicializarMulticorresponsal B ");
                    inicializarMulticorresponsal();
                    //llamarMetodoJava("Bancolombia Ara");
                    if (verificarArchivo(discoNetcom, RUTA_TABLA_MULTICORRESPONSAL) != FS_OK) {
                        enviarStringToJava((char *)"CB  NO CONFIGURADO", (char *)"mostrarInicializacion");
                        setParameter(CNB_BANCO, (char *) "0000");
                    }
                } else if (tablaPar[7] &
                           0x80) { // Se verifica si esta habilitado CNB CORRESPONSALES
                    setParameter(HABILITACION_MODO_CNB_CORRESPONSAL, (char *) "1");
                    setParameter(HABILITACION_MODO_CNB_BCL, (char *) "0");
                    LOGI("Inicializando inicializarMulticorresponsal C ");
                    inicializarMulticorresponsal();
                    //llamarMetodoJava("Corresponsales");
                    if (verificarArchivo(discoNetcom, RUTA_TABLA_MULTICORRESPONSAL) != FS_OK) {
                        enviarStringToJava((char *)"CB  NO CONFIGURADO", (char *)"mostrarInicializacion");
                        // screenMessage("INICIALIZACION", "CB", "NO CONFIGURADO", "", 2 * 1000);
                        setParameter(CNB_BANCO, (char *) "0000");
                    }
                } else {
                    setParameter(HABILITACION_MODO_CNB_BCL,
                                 (char *) "0");            /// BANCOLOMBIA
                    setParameter(HABILITACION_MODO_CNB_CORRESPONSAL,
                                 (char *) "0");   /// CORRESPONSALES
                }
            }

            if (tablaPar[7] & 0x40) {
                sprintf(tipoCanal, "%d", GPRS_SSL);
            } else {
                sprintf(tipoCanal, "%d", GPRS_TCP_IP);
            }

            if (validarDCC(tablaPar[6]) > 0) {
                inicializarDCC();
                //llamarMetodoJava("DCC");
            }

            //////////// VALIDAR DCC //////////////
            /////////// BONOS KIOSCO /////////////
            if (tablaPar[5] & 0x01) {
                setParameter(HABILITACION_BONOS_KIOSCO, (char *) "1");
            } else {
                setParameter(HABILITACION_BONOS_KIOSCO, (char *) "0");
            }
        }
        //// INICIALIZACION PAYPASS /////
        if (tablaPar[4] & 0x20 && resultado > 0) {

            resultado = inicializarPaypass();

            if (resultado > 0) {
                setParameter(MODO_PAYPASS, (char *) "1");
            } else {
                setParameter(MONTO_TRANSACCION_LIMITE, (char *) "0");
                setParameter(MONTO_CVM_LIMITE, (char *) "0");
                setParameter(MODO_PAYPASS, (char *) "0");
            }
        }

        /////////// INICIALIZANDO MULTICORRESPONSAL /////////////
        if (tablaPar[6] & 0x20) {
            setParameter(MULTICORRESPONSAL_ACTIVO, (char *) "1");
        } else {
            setParameter(MULTICORRESPONSAL_ACTIVO, (char *) "0");
        }

        ///////////////INICIALIZACION BONOS SIN TEFF//////////////
        if (tablaPar[7] & 0x80) {
            setParameter(BONO_TEFF_ACTIVO, (char *) "1");
        } else {
            setParameter(BONO_TEFF_ACTIVO, (char *) "0");
        }

        ///////////////PUNTOS COLOMBIA//////////////
        if (tablaPar[7] & 0x10) {
            setParameter(PUNTOS_COLOMBIA_ACTIVO, (char *) "1");
        } else {
            setParameter(PUNTOS_COLOMBIA_ACTIVO, (char *) "0");
        }

    } else {
        //printMessage(strlen(ERROR_PARAMETROS), ERROR_PARAMETROS);
    }

    return (resultado);
}


int inicializarPaypass(void) {

    uChar consecutivo[6 + 1];
    uChar terminal[8 + 1];
    uChar dataRecibidaAux[512 + 1];
    uChar nii[TAM_NII + 1];
    uChar data[8 + 1];
    uChar stringAux[8 + 1];
    int resultado = 0;
    ResultISOPack resultadoIsoPackMessage;
    ResultISOUnpack resultadoUnpack;
    ISOHeader isoHeader8583;
    ISOFieldMessage isoFieldMessage;

    memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
    memset(&isoHeader8583, 0x00, sizeof(isoHeader8583));
    memset(&resultadoUnpack, 0x00, sizeof(resultadoUnpack));
    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(consecutivo, 0x00, sizeof(consecutivo));
    memset(terminal, 0x00, sizeof(terminal));
    memset(dataRecibidaAux, 0x00, sizeof(dataRecibidaAux));
    memset(nii, 0x00, sizeof(nii));
    memset(data, 0x00, sizeof(data));
    memset(stringAux, 0x00, sizeof(stringAux));

    getParameter(NII, nii);

    isoHeader8583.TPDU = 60;
    memcpy(isoHeader8583.destination, nii, TAM_NII + 1);
    memcpy(isoHeader8583.source, nii, TAM_NII + 1);
    setHeader(isoHeader8583);

    generarSystemTraceNumber(consecutivo, pathNetcom);
    getParameter(NUMERO_TERMINAL, terminal);
    getParameter(NII, nii);

    setMTI((char *) "0800");
    setField(3, (char *) "948000", 6);
    setField(11, consecutivo, 6);
    setField(24, nii + 1, 3);
    setField(41, terminal, 8);
    setField(60, (char *) "EFT30_NET01", 11);

    resultadoIsoPackMessage = packISOMessage();

    setParameter(VERIFICAR_PAPEL_TERMINAL, (char *) "00");
    enviarStringToJava((char *)"INICIALIZANDO PAYPASS", (char *)"mostrarInicializacion");
    resultado = realizarTransaccion(resultadoIsoPackMessage.isoPackMessage,
                                    resultadoIsoPackMessage.totalBytes,
                                    dataRecibidaAux, TRANSACCION_INICIALIZACION, CANAL_PERMANENTE,
                                    REVERSO_NO_GENERADO);

    if (resultado > 0) {
        resultadoUnpack = unpackISOMessage(dataRecibidaAux, resultado);
        memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
        isoFieldMessage = getField(60);

        if (isoFieldMessage.totalBytes > 0) {
            sprintf(data, "%02x%02x%02x%02x", isoFieldMessage.valueField[3],
                    isoFieldMessage.valueField[4],
                    isoFieldMessage.valueField[5], isoFieldMessage.valueField[6]);
            leftTrim(stringAux, data, '0');
            setParameter(MONTO_CVM_LIMITE, stringAux);
            memset(data, 0x00, sizeof(data));
            memset(stringAux, 0x00, sizeof(stringAux));
            sprintf(data, "%02x%02x%02x%02x", isoFieldMessage.valueField[7],
                    isoFieldMessage.valueField[8],
                    isoFieldMessage.valueField[9], isoFieldMessage.valueField[10]);
            leftTrim(stringAux, data, '0');
            setParameter(MONTO_TRANSACCION_LIMITE, stringAux);

        } else {
            setParameter(MONTO_TRANSACCION_LIMITE, (char *) "0");
            setParameter(MONTO_CVM_LIMITE, (char *) "0");
        }
    }
    return (resultado);
}

int inicializacionBiEmisores() {

    uChar nii[TAM_NII + 1];
    memset(nii, 0x00, sizeof(nii));
    int respuesta = 0;

    getParameter(NII, nii);

    respuesta = transaccionInicializacion((char *) "945100", (char *) "945101",
                                          (char *) INICIALIZACION_BI_EMISORES, nii);

    return (respuesta);
}

void inicializarDCC(void) {

    ResultISOPack resultadoIsoPackMessage;
    ResultISOUnpack resultadoUnpack;
    ISOHeader isoHeader8583;
    ISOFieldMessage isoFieldMessage;
    uChar dataRecibidaAux[512];
    uChar consecutivo[6 + 1];
    uChar terminal[8 + 1];
    int resultado = 0;

    memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
    memset(&isoHeader8583, 0x00, sizeof(isoHeader8583));
    memset(&resultadoUnpack, 0x00, sizeof(resultadoUnpack));
    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(dataRecibidaAux, 0x00, sizeof(dataRecibidaAux));
    memset(terminal, 0x00, sizeof(terminal));
    memset(consecutivo, 0x00, sizeof(consecutivo));

    borrarArchivo(discoNetcom, (char *) TABLA_DCC);

    //if (estadoTerminalConfigurado() == 1) {

    getParameter(NUMERO_TERMINAL, terminal);

    uChar nii[TAM_NII + 1];
    memset(nii, 0x00, sizeof(nii));

    getParameter(NII, nii);

    isoHeader8583.TPDU = 60;
    memcpy(isoHeader8583.destination, nii, TAM_NII + 1);
    memcpy(isoHeader8583.source, nii, TAM_NII + 1);
    setHeader(isoHeader8583);

    generarSystemTraceNumber(consecutivo, pathNetcom);

    setMTI((char *) "0800");
    setField(3, (char *) "947200", 6);
    setField(11, consecutivo, 6);
    setField(24, nii + 1, 3);
    setField(41, terminal, 8);
    setField(60, (char *) "EFT30_NET01", 11);
    resultadoIsoPackMessage = packISOMessage();

    ////////// ENVIANDO TRANSACCION //////////
    setParameter(VERIFICAR_PAPEL_TERMINAL, (char *) "00");

    resultado = realizarTransaccion(resultadoIsoPackMessage.isoPackMessage,
                                    resultadoIsoPackMessage.totalBytes,
                                    dataRecibidaAux, TRANSACCION_INICIALIZACION, CANAL_PERMANENTE,
                                    REVERSO_NO_GENERADO);

    ////////// RECIBIENDO TRANSACCION //////////
    if (resultado > 0) {
        resultadoUnpack = unpackISOMessage(dataRecibidaAux, resultado);

        isoFieldMessage = getField(60);
        escribirArchivo(discoNetcom, (char *) TABLA_DCC, isoFieldMessage.valueField,
                        isoFieldMessage.totalBytes);
    }
    //}
}

int validarDCC(uChar byteDCC) {

    int indiceDCC = 0;
    int iRet = 0;
    uChar niiDCC[4 + 1];

    if (byteDCC & 0x80) {
        memset(niiDCC, 0x00, sizeof(niiDCC));
        strcpy(niiDCC, "0011");
        indiceDCC++;
    }

    if (byteDCC & 0x40) {
        memset(niiDCC, 0x00, sizeof(niiDCC));
        strcpy(niiDCC, "0012");
        indiceDCC++;
    }

    //////// Setear NII DCC /////////
    if (indiceDCC == 1) {
        setParameter(NII_DCC, niiDCC);
        iRet = 1;
    } else {
        setParameter(NII_DCC, (char *) "0000");
    }

    return iRet;
}

void inicializarMulticorresponsal() {

    ResultISOPack resultadoIsoPackMessage;
    ResultISOUnpack resultadoUnpack;
    ISOHeader isoHeader8583;
    ISOFieldMessage isoFieldMessage;
    uChar dataRecibidaAux[512];
    uChar consecutivo[6 + 1];
    uChar terminal[8 + 1];
    int resultado = 0;
    char modoCnb[1 + 1];
    char modoCnbCORRESPONSALES[1 + 1];

    memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
    memset(&isoHeader8583, 0x00, sizeof(isoHeader8583));
    memset(&resultadoUnpack, 0x00, sizeof(resultadoUnpack));
    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(dataRecibidaAux, 0x00, sizeof(dataRecibidaAux));
    memset(terminal, 0x00, sizeof(terminal));
    memset(consecutivo, 0x00, sizeof(consecutivo));
    memset(modoCnb, 0x00, sizeof(modoCnb));
    memset(modoCnbCORRESPONSALES, 0x00, sizeof(modoCnbCORRESPONSALES));

    borrarArchivo(discoNetcom, (char *) RUTA_TABLA_MULTICORRESPONSAL);

    //if (estadoTerminalConfigurado() == 1) {

    getParameter(NUMERO_TERMINAL, terminal);

    uChar nii[TAM_NII + 1];
    memset(nii, 0x00, sizeof(nii));

    getParameter(NII, nii);

    isoHeader8583.TPDU = 60;
    memcpy(isoHeader8583.destination, nii, TAM_NII + 1);
    memcpy(isoHeader8583.source, nii, TAM_NII + 1);
    setHeader(isoHeader8583);

    generarSystemTraceNumber(consecutivo, pathNetcom);

    setMTI((char *) "0800");
    setField(3, (char *) "947500", 6);
    setField(11, consecutivo, 6);
    setField(24, nii + 1, 3);
    setField(41, terminal, 8);
    setField(60, (char *) "EFT30_NET01", 11);
    resultadoIsoPackMessage = packISOMessage();

    ////////// ENVIANDO TRANSACCION //////////
    setParameter(VERIFICAR_PAPEL_TERMINAL, (char *) "00");
    resultado = realizarTransaccion(resultadoIsoPackMessage.isoPackMessage,
                                    resultadoIsoPackMessage.totalBytes,
                                    dataRecibidaAux, TRANSACCION_INICIALIZACION, CANAL_PERMANENTE,
                                    REVERSO_NO_GENERADO);

    ////////// RECIBIENDO TRANSACCION //////////
    if (resultado > 0) {
        resultadoUnpack = unpackISOMessage(dataRecibidaAux, resultado);
        if (resultado > 36) {
            setParameter(CNB_BANCO, (char *) "0000");
            borrarArchivo(discoNetcom, (char *) TARJETA_CNB);
            borrarArchivo(discoNetcom, (char *) RUTA_TABLA_MULTICORRESPONSAL);
            isoFieldMessage = getField(60);
            LOGI("escribir %s ",isoFieldMessage.valueField);
            escribirArchivo(discoNetcom, RUTA_TABLA_MULTICORRESPONSAL,
                            isoFieldMessage.valueField,
                            isoFieldMessage.totalBytes);
            LOGI("escribio %d",isoFieldMessage.totalBytes);
            getParameter(HABILITACION_MODO_CNB_CORRESPONSAL, modoCnbCORRESPONSALES);
            getParameter(HABILITACION_MODO_CNB_BCL, modoCnb);
            if (atoi(modoCnb) == 1 || atoi(modoCnbCORRESPONSALES) == 1 || atoi(modoCnb) == 2) {
                LOGI("traerInicializacionBcl in");
                traerInicializacionBcl();
            }
        }
    }
    //}
}

int inicializarMultiPos(void) {

    uChar consecutivo[6 + 1];
    //uChar data [20 + 1];
    uChar numeroTerminal[8 + 1];
    uChar dataRecibidaAux[512 + 1];
    uChar codigoProceso[6 + 1];
    uChar p62[8 + 1];
    uChar p62Aux[8 + 1];
    uChar codigoRespuesta[2 + 1];
    int resultado = 0;
    int contador = 1;
    char message[80 + 1];
    int inicializacionMultiPos = 0;
    ResultISOPack resultadoIsoPackMessage;
    ResultISOUnpack resultadoUnpack;
    ISOHeader isoHeader8583;
    ISOFieldMessage isoFieldMessage;

    memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
    memset(&isoHeader8583, 0x00, sizeof(isoHeader8583));
    memset(&resultadoUnpack, 0x00, sizeof(resultadoUnpack));
    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(consecutivo, 0x00, sizeof(consecutivo));
    //memset(data,0x00,sizeof(data));
    memset(numeroTerminal, 0x00, sizeof(numeroTerminal));
    memset(dataRecibidaAux, 0x00, sizeof(dataRecibidaAux));
    memset(p62, 0x00, sizeof(p62));
    memset(codigoProceso, 0x00, sizeof(codigoProceso));
    memset(codigoRespuesta, 0x00, sizeof(codigoRespuesta));
    memset(message, 0x00, sizeof(message));

    uChar nii[TAM_NII + 1];
    memset(nii, 0x00, sizeof(nii));

    getParameter(NII_MULTIPOS, nii);
    getParameter(NUMERO_TERMINAL, numeroTerminal);

    isoHeader8583.TPDU = 60;
    memcpy(isoHeader8583.destination, nii, TAM_NII + 1);
    memcpy(isoHeader8583.source, nii, TAM_NII + 1);
    setHeader(isoHeader8583);

    generarSystemTraceNumber(consecutivo, pathNetcom);

    setMTI((char *) "0800");
    setField(3, (char *) "970000", 6);
    setField(11, consecutivo, 6);
    setField(24, nii + 1, 3);
    setField(41, numeroTerminal, 8);
    setField(62, (char *) "000000", 6);

    resultadoIsoPackMessage = packISOMessage();

    /// SE ENVIA TRANSACCION MULTIPOS///
    setParameter(VERIFICAR_PAPEL_TERMINAL, (char *) "00");

    resultado = realizarTransaccion(resultadoIsoPackMessage.isoPackMessage,
                                    resultadoIsoPackMessage.totalBytes,
                                    dataRecibidaAux, TRANSACCION_INICIALIZACION, CANAL_PERMANENTE,
                                    REVERSO_NO_GENERADO);

    if (resultado > 0) {
        resultadoUnpack = unpackISOMessage(dataRecibidaAux, resultado);


        contador++;
        memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
        isoFieldMessage = getField(3);
        memcpy(codigoProceso, isoFieldMessage.valueField, isoFieldMessage.totalBytes);
        memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
        isoFieldMessage = getField(39);
        memcpy(codigoRespuesta, isoFieldMessage.valueField, isoFieldMessage.totalBytes);
    }
    inicializacionMultiPos = 1;

    memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
    memset(&resultadoUnpack, 0x00, sizeof(resultadoUnpack));
    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(consecutivo, 0x00, sizeof(consecutivo));
    memset(dataRecibidaAux, 0x00, sizeof(dataRecibidaAux));
    memset(p62, 0x00, sizeof(p62));
    isoFieldMessage = getField(62);
    memcpy(p62, isoFieldMessage.valueField + 2, 4);
    memset(p62Aux, 0x00, sizeof(p62Aux));
    memcpy(p62Aux, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

    isoFieldMessage = getField(60);

    if (isoFieldMessage.totalBytes > 0 && resultado > 0) {
        escribirArchivo(discoNetcom, (char *) INICIALIZACION_MULTIPOS, isoFieldMessage.valueField,
                        isoFieldMessage.totalBytes);
    }
    /// SE RECIBE TRANSACCION MULTIPOS///

    while ((strcmp(codigoProceso, "970000") == 0) && (strcmp(codigoRespuesta, "00") == 0)) {

        inicializacionMultiPos = 1;

        memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
        memset(&resultadoUnpack, 0x00, sizeof(resultadoUnpack));
        memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
        memset(consecutivo, 0x00, sizeof(consecutivo));

        memset(dataRecibidaAux, 0x00, sizeof(dataRecibidaAux));
        memset(p62, 0x00, sizeof(p62));
        isoFieldMessage = getField(62);
        memcpy(p62, isoFieldMessage.valueField + 2, 4);
        memset(p62Aux, 0x00, sizeof(p62Aux));
        memcpy(p62Aux, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

        memset(codigoProceso, 0x00, sizeof(codigoProceso));
        memset(codigoRespuesta, 0x00, sizeof(codigoRespuesta));

        if (strncmp(p62, p62 + 2, 2) == 0) {
            logonMultipos(codigoProceso, codigoRespuesta);
        } else {
            generarSystemTraceNumber(consecutivo, pathNetcom);
            setMTI((char *) "0800");
            setField(3, (char *) "970000", 6);
            setField(11, consecutivo, 6);
            setField(24, nii + 1, 3);
            setField(41, numeroTerminal, 8);
            setField(62, p62Aux, 6);

            resultadoIsoPackMessage = packISOMessage();
            setParameter(VERIFICAR_PAPEL_TERMINAL, (char *) "00");

            resultado = realizarTransaccion(resultadoIsoPackMessage.isoPackMessage,
                                            resultadoIsoPackMessage.totalBytes,
                                            dataRecibidaAux, TRANSACCION_INICIALIZACION,
                                            CANAL_PERMANENTE, REVERSO_NO_GENERADO);

            if (resultado > 0) {
                resultadoUnpack = unpackISOMessage(dataRecibidaAux, resultado);

                contador++;
                isoFieldMessage = getField(60);
                escribirArchivo(discoNetcom, (char *) INICIALIZACION_MULTIPOS,
                                isoFieldMessage.valueField,
                                isoFieldMessage.totalBytes);
                memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
                isoFieldMessage = getField(3);
                memcpy(codigoProceso, isoFieldMessage.valueField, isoFieldMessage.totalBytes);
                memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
                isoFieldMessage = getField(39);
                memcpy(codigoRespuesta, isoFieldMessage.valueField, isoFieldMessage.totalBytes);
            }
        }
    }

    if (strcmp(codigoRespuesta, "00") == 0) {
        //printMessage(strlen(INI_MULTIPOS_CORRECTA), INI_MULTIPOS_CORRECTA);
    }
    if (inicializacionMultiPos == 0) {
        //printMessage(strlen(MULTIPOS_NO_DISPONIBLE), MULTIPOS_NO_DISPONIBLE);
        resultado = 1;
    }
    //SE SALE DEL WHILE, SE VERIFICAN LAS TRAMAS
    // SI HUBO MULTIPOS SE GENERA LA INICIALIZACION SERIAL
    //_inicializacionSerial_();
    return (resultado);

}

void inicializarBigBancolombia() {

    ResultISOPack resultadoIsoPackMessage;
    ResultISOUnpack resultadoUnpack;
    ISOHeader isoHeader8583;
    ISOFieldMessage isoFieldMessage;
    uChar dataRecibidaAux[512];
    uChar consecutivo[6 + 1];
    uChar terminal[8 + 1];
    int resultado = 0;
    char buffer[16 + 1];

    memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
    memset(&isoHeader8583, 0x00, sizeof(isoHeader8583));
    memset(&resultadoUnpack, 0x00, sizeof(resultadoUnpack));
    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(dataRecibidaAux, 0x00, sizeof(dataRecibidaAux));
    memset(terminal, 0x00, sizeof(terminal));
    memset(consecutivo, 0x00, sizeof(consecutivo));
    memset(buffer, 0x00, sizeof(buffer));

    borrarArchivo(discoNetcom, (char *) "/Big_Ban.txt");

    //if (estadoTerminalConfigurado() == 1) {

    getParameter(NUMERO_TERMINAL, terminal);

    uChar nii[TAM_NII + 1];
    memset(nii, 0x00, sizeof(nii));

    getParameter(NII, nii);

    isoHeader8583.TPDU = 60;
    memcpy(isoHeader8583.destination, nii, TAM_NII + 1);
    memcpy(isoHeader8583.source, nii, TAM_NII + 1);
    setHeader(isoHeader8583);

    generarSystemTraceNumber(consecutivo, pathNetcom);

    setMTI((char *) "0800");
    setField(3, (char *) "948100", 6);
    setField(11, consecutivo, 6);
    setField(24, nii + 1, 3);
    setField(41, terminal, 8);
    setField(60, (char *) "EFT30_NET01", 11);
    resultadoIsoPackMessage = packISOMessage();

    ////////// ENVIANDO TRANSACCION //////////
    setParameter(VERIFICAR_PAPEL_TERMINAL, (char *) "00");

    resultado = realizarTransaccion(resultadoIsoPackMessage.isoPackMessage,
                                    resultadoIsoPackMessage.totalBytes,
                                    dataRecibidaAux, TRANSACCION_INICIALIZACION, CANAL_PERMANENTE,
                                    REVERSO_NO_GENERADO);

    ////////// RECIBIENDO TRANSACCION //////////
    if (resultado > 0) {

        resultadoUnpack = unpackISOMessage(dataRecibidaAux, resultado);
        isoFieldMessage = getField(60);
        if (isoFieldMessage.totalBytes > 0) {
            escribirArchivo(discoNetcom, (char *) "/Big_Ban.txt", isoFieldMessage.valueField,
                            isoFieldMessage.totalBytes);
        } else {
            borrarArchivo(discoNetcom, (char *) "/Big_Ban.txt");
            setParameter(BIG_BANCOLOMBIA, (char *) "0");
        }
    }
    //}
}


DatosIniQrCode capturarDatosQrCodeRbm(void) {

    char buffer[20 + 1];
    int indice = 0;
    DatosIniQrCode datosQr;

    memset(&buffer, 0x00, sizeof(buffer));
    memset(&datosQr, 0x00, sizeof(datosQr));
    LOGI("capturarDatosQrCodeRbm in    ");
    buscarArchivo(discoNetcom, (char *) TABLA_INICIALIZACION_QRCODE, buffer, 0, 20);
    LOGI("capturarDatosQrCodeRbm out    ");
    indice += (4);
    memcpy(datosQr.terminal, buffer + indice, TAM_TERMINAL_ID);
    indice += (TAM_TERMINAL_ID);
    memcpy(datosQr.tiempoIntentos, buffer + indice, TAM_NII);
    indice += (TAM_NII);
    memcpy(datosQr.numeroIntentos, buffer + indice, 1);
    indice += (1);
    memcpy(datosQr.niiQr, buffer + indice, 4);

    return datosQr;
}

DatosQrCodeEmvco capturarDatosQrCode(void) {

    char buffer[16 + 1];
    int indice = 0;
    DatosQrCodeEmvco datosQr;

    memset(&buffer, 0x00, sizeof(&buffer));
    memset(&datosQr, 0x00, sizeof(datosQr));

    buscarArchivo(discoNetcom, (char *)TABLA_INICIALIZACION_QRCODE_EMVCO, buffer, 0, 20);

    indice += 4;
    memcpy(datosQr.tiempoConsulta, buffer + indice, 2);
    indice += 2;
    memcpy(datosQr.numeroIntentos, buffer + indice, 1);
    indice += 1;
    memcpy(datosQr.tiempoVisualizacion, buffer + indice, 2);
    indice += 2;
    memcpy(datosQr.niiGeneracionQr, buffer + indice, 4);
    indice += 4;
    memcpy(datosQr.niiTransaccional, buffer + indice, 4);

    //setParameter(NUMERO_TERMINAL_QR, datosQr.terminal);

    return datosQr;
}

void inicializarQRCODE(void) {

    ResultISOPack resultadoIsoPackMessage;
    ResultISOUnpack resultadoUnpack;
    ISOHeader isoHeader8583;
    ISOFieldMessage isoFieldMessage;
    uChar dataRecibidaAux[512];
    uChar consecutivo[6 + 1];
    uChar terminal[8 + 1];
    int resultado = 0;
    char buffer[16 + 1];

    memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
    memset(&isoHeader8583, 0x00, sizeof(isoHeader8583));
    memset(&resultadoUnpack, 0x00, sizeof(resultadoUnpack));
    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(dataRecibidaAux, 0x00, sizeof(dataRecibidaAux));
    memset(terminal, 0x00, sizeof(terminal));
    memset(consecutivo, 0x00, sizeof(consecutivo));
    memset(buffer, 0x00, sizeof(buffer));

    borrarArchivo(discoNetcom, (char *) TABLA_INICIALIZACION_QRCODE);

    //if (estadoTerminalConfigurado() == 1) {

    getParameter(NUMERO_TERMINAL, terminal);

    uChar nii[TAM_NII + 1];
    memset(nii, 0x00, sizeof(nii));

    getParameter(NII, nii);

    isoHeader8583.TPDU = 60;
    memcpy(isoHeader8583.destination, nii, TAM_NII + 1);
    memcpy(isoHeader8583.source, nii, TAM_NII + 1);
    setHeader(isoHeader8583);

    generarSystemTraceNumber(consecutivo, pathNetcom);

    setMTI((char *) "0800");
    setField(3, (char *) "947600", 6);
    setField(11, consecutivo, 6);
    setField(24, nii + 1, 3);
    setField(41, terminal, 8);
    setField(60, (char *) "EFT30_NET01", 11);
    resultadoIsoPackMessage = packISOMessage();

    ////////// ENVIANDO TRANSACCION //////////
    setParameter(VERIFICAR_PAPEL_TERMINAL, (char *) "00");

    resultado = realizarTransaccion(resultadoIsoPackMessage.isoPackMessage,
                                    resultadoIsoPackMessage.totalBytes,
                                    dataRecibidaAux, TRANSACCION_INICIALIZACION, CANAL_PERMANENTE,
                                    REVERSO_NO_GENERADO);

    ////////// RECIBIENDO TRANSACCION //////////
    if (resultado > 0) {
        resultadoUnpack = unpackISOMessage(dataRecibidaAux, resultado);
        isoFieldMessage = getField(60);
        if (isoFieldMessage.totalBytes > 0) {
            escribirArchivo(discoNetcom, (char *) TABLA_INICIALIZACION_QRCODE,
                            isoFieldMessage.valueField,
                            isoFieldMessage.totalBytes);
        } else {
            borrarArchivo(discoNetcom, (char *) TABLA_INICIALIZACION_QRCODE);
            setParameter(MODO_QR, (char *) "0");
        }

    }
    //}
}

int inicializarExcepciones(int tipoInicializacion) {

    uChar nii[TAM_NII + 1];
    memset(nii, 0x00, sizeof(nii));
    int resultado = 0;

    getParameter(NII, nii);

    resultado = transaccionInicializacion((char *) "946500", (char *) "946501",
                                          (char *) TABLA_EXCEPCIONES, nii);

    return (resultado);
}


int inicializacionEmisores(void) {

    uChar nii[TAM_NII + 1];
    memset(nii, 0x00, sizeof(nii));
    int respuesta = 0;

    getParameter(NII, nii);

    respuesta = transaccionInicializacion((char *) "945000", (char *) "945001",
                                          (char *) INICIALIZACION_EMISORES, nii);

    if (respuesta > 0) {
        respuesta = inicializacionBiEmisores();
    }
    return (respuesta);
}

int inicializacionBonos(void) {

    uChar nii[TAM_NII + 1];
    memset(nii, 0x00, sizeof(nii));
    int resultado = 0;

    getParameter(NII, nii);

    resultado = transaccionInicializacion((char *) "944000", (char *) "944001", (char *) TX_BONOS,
                                          nii);
    return (resultado);
}

int inicializacionPuntos(void) {

    uChar nii[TAM_NII + 1];
    memset(nii, 0x00, sizeof(nii));
    int resultado = 0;

    getParameter(NII, nii);

    resultado = transaccionInicializacion((char *) "943000", (char *) "943001",
                                          (char *) INICIALIZACION_PUNTOS, nii);
    return (resultado);

}


void logonMultipos(uChar *codigoProceso, uChar *codigoRespuesta) {

    uChar consecutivo[6 + 1];
    uChar data[20 + 1];
    uChar dataRecibidaAux[512 + 1];
    uChar workingKey[8 + 1];
    int resultado = 0;
    ResultISOPack resultadoIsoPackMessage;
    ResultISOUnpack resultadoUnpack;
    ISOHeader isoHeader8583;
    ISOFieldMessage isoFieldMessage;

    memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
    memset(&isoHeader8583, 0x00, sizeof(isoHeader8583));
    memset(&resultadoUnpack, 0x00, sizeof(resultadoUnpack));
    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(consecutivo, 0x00, sizeof(consecutivo));
    memset(data, 0x00, sizeof(data));
    memset(dataRecibidaAux, 0x00, sizeof(dataRecibidaAux));
    memset(workingKey, 0x00, sizeof(workingKey));

    uChar nii[TAM_NII + 1];
    memset(nii, 0x00, sizeof(nii));

    getParameter(NII_MULTIPOS, nii);

    isoHeader8583.TPDU = 60;
    memcpy(isoHeader8583.destination, nii, TAM_NII + 1);
    memcpy(isoHeader8583.source, nii, TAM_NII + 1);
    setHeader(isoHeader8583);

    generarSystemTraceNumber(consecutivo, pathNetcom);

    setMTI((char *) "0800");
    setField(3, (char *) "920000", 6);
    setField(11, consecutivo, 6);
    setField(24, nii + 1, 3);
    getParameter(NUMERO_TERMINAL, data);
    setField(41, data, 8);

    resultadoIsoPackMessage = packISOMessage();

    // SE ENVIA LOGON MULTIPOS //
    setParameter(VERIFICAR_PAPEL_TERMINAL, (char *) "00");

    resultado = realizarTransaccion(resultadoIsoPackMessage.isoPackMessage,
                                    resultadoIsoPackMessage.totalBytes,
                                    dataRecibidaAux, TRANSACCION_INICIALIZACION, CANAL_PERMANENTE,
                                    REVERSO_NO_GENERADO);

    // SE RECIBE LOGON MULTIPOS //
    if (resultado > 0) {
        resultadoUnpack = unpackISOMessage(dataRecibidaAux, resultado);
        isoFieldMessage = getField(62);
        /// WORKING KEY MULTIPOS
        _convertASCIIToBCD_(workingKey, isoFieldMessage.valueField, isoFieldMessage.totalBytes);
        borrarArchivo(discoNetcom, (char *) WORKING_MULTIPOS);
        escribirArchivo(discoNetcom, (char *) WORKING_MULTIPOS, workingKey,
                        (isoFieldMessage.totalBytes / 2));
        memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
        isoFieldMessage = getField(3);
        memcpy(codigoProceso, isoFieldMessage.valueField, isoFieldMessage.totalBytes);
        memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
        isoFieldMessage = getField(39);
        memcpy(codigoRespuesta, isoFieldMessage.valueField, isoFieldMessage.totalBytes);
    }
}

int traerInicializacionBcl() {

    char fidd[4 + 1];
    char tipoCuenta[2 + 1];
    char panVirtual[37 + 1];
    char numeroTarjeta[37 + 1];
    char datosBancolombia[256 + 1]; //!!temporal;

    memset(fidd, 0x00, sizeof(fidd));
    memset(tipoCuenta, 0x00, sizeof(tipoCuenta));
    memset(panVirtual, 0x00, sizeof(panVirtual));
    memset(numeroTarjeta, 0x00, sizeof(numeroTarjeta));
    memset(datosBancolombia, 0x00, sizeof(datosBancolombia));

    if (verificarArchivo(discoNetcom, RUTA_TABLA_MULTICORRESPONSAL) == FS_OK) {
        leerArchivo(discoNetcom, (char *) RUTA_TABLA_MULTICORRESPONSAL, datosBancolombia);
        LOGI("datosBancolombia %s", datosBancolombia);
        memcpy(fidd, datosBancolombia + 3, 4);
        LOGI("fidd %s", fidd);
        memcpy(tipoCuenta, datosBancolombia + 45, 1);
        LOGI("tipoCuenta %s", tipoCuenta);
        memcpy(panVirtual, datosBancolombia + 26, 16);
        LOGI("panVirtual %s", panVirtual);
        strcat(tipoCuenta, "0");
        setParameter(CNB_BANCO, fidd);
        setParameter(CNB_TIPO_CUENTA, tipoCuenta);
        memcpy(panVirtual + strlen(panVirtual), "D101210100000", 13);

        memset(tipoCuenta, 0x00, sizeof(tipoCuenta));
        sprintf(tipoCuenta, "%02d", CNB_TIPO_TARJETA_VIRTUAL);
        LOGI("tipoCuenta %s",tipoCuenta);
        setParameter(CNB_TIPO_TARJETA, tipoCuenta);
        escribirArchivo(discoNetcom, (char *) TARJETA_CNB, panVirtual, strlen(panVirtual));
    }
    LOGI("salio  ");
    return 1;
}

int transaccionInicializacion(uChar *codigoProcesoInicial, uChar *codigoVerificacion,
                              uChar *nombreArchivo, uChar *nii) {

    uChar consecutivo[6 + 1];
    uChar data[20 + 1];
    uChar codigoProceso[8 + 1];
    uChar dataRecibidaAux[2048 + 1];
    int resultado = 0;
    ResultISOPack resultadoIsoPackMessage;
    ResultISOUnpack resultadoUnpack;
    ISOHeader isoHeader8583;
    ISOFieldMessage isoFieldMessage;

    memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
    memset(&isoHeader8583, 0x00, sizeof(isoHeader8583));
    memset(&resultadoUnpack, 0x00, sizeof(resultadoUnpack));
    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(consecutivo, 0x00, sizeof(consecutivo));
    memset(data, 0x00, sizeof(data));
    memset(dataRecibidaAux, 0x00, sizeof(dataRecibidaAux));
    memset(codigoProceso, 0x00, sizeof(codigoProceso));

    isoHeader8583.TPDU = 60;
    memcpy(isoHeader8583.destination, nii, TAM_NII + 1);
    memcpy(isoHeader8583.source, nii, TAM_NII + 1);
    setHeader(isoHeader8583);

    generarSystemTraceNumber(consecutivo, "");

    setMTI((char *) "0800");
    setField(3, codigoProcesoInicial, 6);
    setField(11, consecutivo, 6);
    setField(24, nii + 1, 3);
    getParameter(NUMERO_TERMINAL, data);
    setField(41, data, 8);
    memset(data, 0x00, sizeof(data));
    strcpy(data, "EFT30_NET01");
    setField(60, data, 11);
    resultadoIsoPackMessage = packISOMessage();

    setParameter(VERIFICAR_PAPEL_TERMINAL, (char *) "00");

    resultado = realizarTransaccion(resultadoIsoPackMessage.isoPackMessage,
                                    resultadoIsoPackMessage.totalBytes,
                                    dataRecibidaAux, TRANSACCION_INICIALIZACION, CANAL_PERMANENTE,
                                    REVERSO_NO_GENERADO);

    if (resultado > 0) {
        resultadoUnpack = unpackISOMessage(dataRecibidaAux, resultado);
        isoFieldMessage = getField(3);
        memcpy(codigoProceso, isoFieldMessage.valueField, isoFieldMessage.totalBytes);
        memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
        isoFieldMessage = getField(60);
        if (isoFieldMessage.totalBytes > 0) {
            borrarArchivo(discoNetcom, nombreArchivo);
            escribirArchivo(discoNetcom, nombreArchivo, isoFieldMessage.valueField,
                            isoFieldMessage.totalBytes);
        } else {
            //printMessage(strlen(ERROR_FIELD_60), ERROR_FIELD_60);
        }
    }

    while (strncmp(codigoProceso, codigoVerificacion, 6) != 0) {
        memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
        memset(&resultadoUnpack, 0x00, sizeof(resultadoUnpack));
        memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
        memset(consecutivo, 0x00, sizeof(consecutivo));
        memset(data, 0x00, sizeof(data));
        memset(dataRecibidaAux, 0x00, sizeof(dataRecibidaAux));
        memset(codigoProceso, 0x00, sizeof(codigoProceso));

        generarSystemTraceNumber(consecutivo, pathNetcom);
        setHeader(isoHeader8583);
        setMTI((char *) "0800");
        setField(3, codigoVerificacion, 6);
        setField(11, consecutivo, 6);
        setField(24, nii + 1, 3);
        memset(data, 0x00, sizeof(data));
        getParameter(NUMERO_TERMINAL, data);
        setField(41, data, 8);
        memset(data, 0x00, sizeof(data));
        strcpy(data, "EFT30_NET01");
        setField(60, data, 11);
        resultadoIsoPackMessage = packISOMessage();

        setParameter(VERIFICAR_PAPEL_TERMINAL, (char *) "00");

        resultado = realizarTransaccion(resultadoIsoPackMessage.isoPackMessage,
                                        resultadoIsoPackMessage.totalBytes,
                                        dataRecibidaAux, TRANSACCION_INICIALIZACION,
                                        CANAL_PERMANENTE, REVERSO_NO_GENERADO);

        if (resultado > 0) {
            resultadoUnpack = unpackISOMessage(dataRecibidaAux, resultado);
            isoFieldMessage = getField(3);
            LOGI("resultado  %d", resultado);
            LOGI("Inicializando isoFieldMessage.totalBytes   %d", isoFieldMessage.totalBytes);
            if (isoFieldMessage.totalBytes > 0){
                memcpy(codigoProceso, isoFieldMessage.valueField, isoFieldMessage.totalBytes);
            }
            memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
            isoFieldMessage = getField(60);
            if (isoFieldMessage.totalBytes > 0) {
                escribirArchivo(discoNetcom, nombreArchivo, isoFieldMessage.valueField,
                                isoFieldMessage.totalBytes);
            } else {
                //printMessage(strlen(ERROR_FIELD_60), ERROR_FIELD_60);
            }
        }
    }
    return (resultado);
}

void _inicializacionSerial_(void) {

    //NO_SERIE Tmp;
    uChar consecutivo[6 + 1];
    uChar data[20 + 1];
    uChar Tmp[10 + 1];
    uChar campo60[80 + 1];
    uChar dataRecibidaAux[512 + 1];
    uChar workingKey[8 + 1];
    uChar versionSoftware[12 + 1];
    uChar passwordUser[4 + 1];
    uChar passwordAdmin[6 + 1];
    uChar dataAux[64];
    uChar claveCambiada[1 + 1];
    int resultado = 0;
    int existeTelecarga = 0;
    int existeInicializacion = 0;
    int idx = 0;
    ResultISOPack resultadoIsoPackMessage;
    ResultISOUnpack resultadoUnpack;
    ISOHeader isoHeader8583;
    ISOFieldMessage isoFieldMessage;
    TablaUnoInicializacion TablaUno;
    unsigned char terminal[64];
    char *token;
    char parametrosFile[10][20 + 1] = {0x00};

    memset(Tmp, 0, sizeof(Tmp));
    memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
    memset(&resultadoUnpack, 0x00, sizeof(resultadoUnpack));
    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(dataRecibidaAux, 0x00, sizeof(dataRecibidaAux));
    memset(&isoHeader8583, 0x00, sizeof(isoHeader8583));
    memset(consecutivo, 0x00, sizeof(consecutivo));
    memset(&TablaUno, 0x00, sizeof(TablaUno));
    memset(workingKey, 0x00, sizeof(workingKey));
    memset(campo60, 0x00, sizeof(campo60));
    memset(data, 0x00, sizeof(data));
    memset(versionSoftware, 0x00, sizeof(versionSoftware));
    memset(passwordUser, 0x00, sizeof(passwordUser));
    memset(passwordAdmin, 0x00, sizeof(passwordAdmin));
    memset(claveCambiada, 0x00, sizeof(claveCambiada));
    memset(terminal, 0x00, sizeof(terminal));

    TablaUno = _desempaquetarTablaCeroUno_();

    getParameter(TITULO_CAMBIO_CLAVE, claveCambiada);

    sprintf(passwordAdmin, "%02x%02x%02x", TablaUno.maintenancePassword[0],
            TablaUno.maintenancePassword[1],
            TablaUno.maintenancePassword[2]);
    setParameter(VALOR_CLAVE_ADMINISTRADOR, passwordAdmin);

    if (atoi(claveCambiada) == 0) {
        sprintf(passwordUser, "%02x%02x", TablaUno.password[0],
                TablaUno.password[1]);
        setParameter(VALOR_CLAVE_SUPERVISOR, passwordUser);
    }

    setParameter(VALOR_CLAVE_BONOS, passwordUser);
    uChar nii[TAM_NII + 1];
    memset(nii, 0x00, sizeof(nii));

    getParameter(NII, nii);

    isoHeader8583.TPDU = 60;
    memcpy(isoHeader8583.destination, nii, TAM_NII + 1);
    memcpy(isoHeader8583.source, nii, TAM_NII + 1);
    setHeader(isoHeader8583);

    generarSystemTraceNumber(consecutivo, pathNetcom);

    setMTI((char *) "0800");
    setField(3, (char *) "940000", 6);
    setField(11, consecutivo, 6);
    setField(24, nii + 1, 3);
    getParameter(NUMERO_TERMINAL, data);

    if (isCnb == 1) {
        memset(data, 0x00, sizeof(data));
        getParameter(CNB_TERMINAL, data);
    }

    setField(41, data, 8);
    /// COMPLETAR P60 ///
    int indice = 0;
    memset(data, 0x00, sizeof(data));
    memset(dataAux, 0x00, sizeof(dataAux));

    leerArchivo(discoNetcom,"/PARAMTERMINAL.txt",dataAux);

    token = strtok(dataAux, ";");
    memcpy(parametrosFile[idx],token, strlen(token));
    while( token != NULL ) {
        idx++;
        token = strtok(NULL, ";");
        if(token != NULL){
            memcpy(parametrosFile[idx],token, strlen(token));
        }
    }

    memcpy(dataAux,  parametrosFile[1], 8);
    leftPad(data, dataAux, 0x30, 16);

    ///////// SERIAL DEL TERMINAL /////////|
    memcpy(campo60, data, 16);

    indice += 16;
    ///////// VERSION DEL TERMINAL /////////
    memset(dataAux, 0x00, sizeof(dataAux));
    memset(data, 0x00, sizeof(data));

    getParameter(VERSION_SW, dataAux);
    getParameter(REVISION_SW, data);
    sprintf(versionSoftware, "%.10s%s", dataAux, data);
    LOGI("versionSoftware %s ", versionSoftware);
    strcpy(campo60 + indice, versionSoftware);

    indice += 11;

    ///////// TIPO COMUNICACION /////////
    memset(dataAux, 0x00, sizeof(dataAux));
    getParameter(TIPO_COMUNICACION, dataAux);
    switch (dataAux[0]) {
        case '1':
            campo60[indice] = 'D';
            break;
        case '2':
            campo60[indice] = 'I';
            break;
        case '3':
            campo60[indice] = 'I';
            break;
        case '4':
            campo60[indice] = 'G';
            break;
    }
    LOGI("INICIALIZACION_EXITOSA 2.4");
    indice += 1;
    ///////// NUMERO PLACA /////////
    memset(data, 0x00, sizeof(data));
    getParameter(NUMERO_PLACA, data);
    LOGI("PLACA %s ", data);
    memcpy(campo60 + indice, data, 10);
    indice += 10;
    ///////// CODIGO COMERCIO /////////
    memcpy(campo60 + indice, TablaUno.receiptLine3, 10);
    indice += 10;

    uChar varAux[1 + 1];

    memset(varAux, 0x00, sizeof(varAux));

    getParameter(TERMINAL_INICIALIZADA_ANTERIOR, varAux);

    ///////// TELECARGA MANUAL O AUTOMATICA /////////
    memset(data, 0x00, sizeof(data));
    getParameter(TIPO_INICIALIZACION, data);
    campo60[indice] = data[0]; //telecarga automatica o manual   cambiar
    memset(dataAux, 0x00, sizeof(dataAux));
    dataAux[0] = data[0]; //telecarga automatica o manual   cambiar
    if (dataAux[0] == 'M') {
        existeInicializacion = 1;
        memset(data, 0x30, sizeof(data));
        setParameter(TIME_ULTIMA_TELECARGA_EXITOSA, data);
    }
    indice += 1;
    ///////// FECHA ULTIMA TELECARGA EXITOSA /////////
    memset(data, 0x00, sizeof(data));
    getParameter(TIME_ULTIMA_TELECARGA_EXITOSA, data);
    memcpy(campo60 + indice, data, 10); //revisar fecha telecarga ultima exitosa cambiar
    indice += 10;
    memcpy(campo60 + indice, "00", 2);
    indice += 2;
    ///////// FECHA ULTIMA INICIALIZACION EXITOSA /////////
    memset(data, 0x00, sizeof(data));
    getParameter(TIME_ULTIMA_INICIALIZACION_EXITOSA, data);
    memcpy(campo60 + indice, data, 10); //revisar fecha inicializacion ultima exitosa cambiar
    indice += 10;
    memcpy(campo60 + indice, "00", 2);
    indice += 2;

    if (strncmp(data, "0000000000", 10) != 0) {
        existeInicializacion = 1;
    }
    ///////// TELECARGA EXITOSA O FALLIDA /////////
    memset(data, 0x00, sizeof(data));
    getParameter(ESTADO_TELECARGA, data);
    existeTelecarga = atoi(data);

    memset(data, 0x00, sizeof(data));
    getParameter(TIPO_TELECARGA, data);
    existeInicializacion = atoi(data);
    if (existeInicializacion == 1) { // si solo inicializo
        campo60[indice] = 'I'; // Solo Inicializa
    } else {
        if (existeTelecarga == 1) {
            campo60[indice] = 'S'; // TELECARGA EXITOSA
        } else {
            campo60[indice] = 'N'; // TELECARGA FALLO
        }
    }

    setField(60, campo60, 74);
    LOGI("campo60 %s ", campo60);
    resultadoIsoPackMessage = packISOMessage();

    setParameter(VERIFICAR_PAPEL_TERMINAL, (char *) "00");

    resultado = realizarTransaccion(resultadoIsoPackMessage.isoPackMessage,
                                    resultadoIsoPackMessage.totalBytes,
                                    dataRecibidaAux, TRANSACCION_INICIALIZACION, CANAL_PERMANENTE,
                                    REVERSO_NO_GENERADO);
    if (resultado > 0) {
        resultadoUnpack = unpackISOMessage(dataRecibidaAux, resultado);
        //VERIFICAR QUE VENGAN LOS CAMPOS CORRESPONDIENTES//
        // CAMPOS: 3,12,13,24,41 ///
        //printMessage(strlen(SERIAL_EXITOSO), SERIAL_EXITOSO);
    }
    LOGI("INICIALIZACION_EXITOSA 2.7");
}


TablaUnoInicializacion _desempaquetarTablaCeroUno_(void) {

    TablaUnoInicializacion desempaquetarTablaUno;
    uChar variableBits[9]; //Manejo de bits
    uChar dataTablaUno[1024];
    uChar dataAux[23 + 1];
    uChar horaCierre[6 + 1];

    int indice = 6;
    memset(&desempaquetarTablaUno, 0x00, sizeof(desempaquetarTablaUno));
    memset(variableBits, 0x00, sizeof(variableBits));
    memset(dataTablaUno, 0x00, sizeof(dataTablaUno));
    memset(&horaCierre, 0x00, sizeof(horaCierre));
    leerArchivo(discoNetcom, (char *) TABLA_1_INICIALIZACION, dataTablaUno);
    desempaquetarTablaUno.dllTraking = dataTablaUno[indice];
    indice += 1;
    desempaquetarTablaUno.initializationControl = dataTablaUno[indice];
    indice += 1;
    desempaquetarTablaUno.printerBaudRate = dataTablaUno[indice];
    indice += 1;
    desempaquetarTablaUno.ecrBaudRate = dataTablaUno[indice];
    indice += 1;
    memcpy(desempaquetarTablaUno.dateandTime, dataTablaUno + indice, 6);
    indice += 6;
    ///////telephoneDialOptions///////
    memcpy(variableBits, dataTablaUno + indice, 1);
    calcularBitsEncendidos(variableBits);
    desempaquetarTablaUno.telephoneDialOptions.unusedCero = variableBits[7] - '0';
    desempaquetarTablaUno.telephoneDialOptions.unusedUno = variableBits[6] - '0';
    desempaquetarTablaUno.telephoneDialOptions.unusedDos = variableBits[5] - '0';
    desempaquetarTablaUno.telephoneDialOptions.autoAnswer = variableBits[4] - '0';
    desempaquetarTablaUno.telephoneDialOptions.referralDialing = variableBits[3] - '0';
    desempaquetarTablaUno.telephoneDialOptions.memoryDialing = variableBits[2] - '0';
    desempaquetarTablaUno.telephoneDialOptions.tollDialing = variableBits[1] - '0';
    desempaquetarTablaUno.telephoneDialOptions.keyboardDialing = variableBits[0] - '0';

    memset(dataAux, 0x00, sizeof(dataAux));
    sprintf(dataAux, "%02x", desempaquetarTablaUno.telephoneDialOptions.unusedUno);

    setParameter(IMPRESION_RECIBO_CAJA, dataAux);
    indice += 1;
    ///////telephoneDialOptions///////
    memcpy(desempaquetarTablaUno.password, dataTablaUno + indice, 2);
    indice += 2;
    memcpy(desempaquetarTablaUno.helpDeskPhone, dataTablaUno + indice, 12);
    indice += 12;
    ///////Options 1///////
    memset(variableBits, 0x00, sizeof(variableBits));
    memcpy(variableBits, dataTablaUno + indice, 1);
    calcularBitsEncendidos(variableBits);
    desempaquetarTablaUno.options1.amountDualEntry = variableBits[7] - '0';
    desempaquetarTablaUno.options1.displayMagneticStripeReaderInput = variableBits[6] - '0';
    desempaquetarTablaUno.options1.tipProcessing = variableBits[5] - '0';
    desempaquetarTablaUno.options1.cashierProcessing = variableBits[4] - '0';
    desempaquetarTablaUno.options1.lodgingProcessing = variableBits[3] - '0';
    desempaquetarTablaUno.options1.printTimeOnReceipt = variableBits[2] - '0';
    desempaquetarTablaUno.options1.businessDateFormat = variableBits[1] - '0';
    desempaquetarTablaUno.options1.reconciliationMethod = variableBits[0] - '0';
    indice += 1;
    ///////Options 1///////
    ///////Options 2///////
    memset(variableBits, 0x00, sizeof(variableBits));
    memcpy(variableBits, dataTablaUno + indice, 1);
    calcularBitsEncendidos(variableBits);
    desempaquetarTablaUno.options2.reserved = variableBits[7] - '0';
    desempaquetarTablaUno.options2.printerUsed = variableBits[6] - '0';
    desempaquetarTablaUno.options2.printTotalOnReceipt = variableBits[5] - '0';
    desempaquetarTablaUno.options2.tillProcessing = variableBits[4] - '0';
    desempaquetarTablaUno.options2.refundTransactionsProcessedOffline = variableBits[3] - '0';
    desempaquetarTablaUno.options2.voidTransactionsProcessedOffline = variableBits[2] - '0';
    desempaquetarTablaUno.options2.enableReceiptPreprint = variableBits[1] - '0';
    desempaquetarTablaUno.options2.CardVerifyKeyOrDebitKey = variableBits[0] - '0';
    indice += 1;
    ///////Options 2///////
    ///////Options 3///////
    memset(variableBits, 0x00, sizeof(variableBits));
    memcpy(variableBits, dataTablaUno + indice, 1);
    calcularBitsEncendidos(variableBits);
    desempaquetarTablaUno.options3.unusedCero = variableBits[7] - '0';
    desempaquetarTablaUno.options3.electronicCardReaderInterface = variableBits[6] - '0';
    desempaquetarTablaUno.options3.amountVerifyOnPinPad = variableBits[5] - '0';
    desempaquetarTablaUno.options3.printCardVerifyReceipt = variableBits[4] - '0';
    desempaquetarTablaUno.options3.takeImprintPromptManualEntry = variableBits[3] - '0';
    desempaquetarTablaUno.options3.checkReaderInstalled = variableBits[2] - '0';
    desempaquetarTablaUno.options3.cardReadFromPinPad = variableBits[1] - '0';
    desempaquetarTablaUno.options3.unusedSiete = variableBits[0] - '0';
    indice += 1;
    ///////Options 3///////
    ///////Options 4///////
    memset(variableBits, 0x00, sizeof(variableBits));
    memcpy(variableBits, dataTablaUno + indice, 1);
    calcularBitsEncendidos(variableBits);
    desempaquetarTablaUno.options4.adjustByCashier = variableBits[7] - '0';
    desempaquetarTablaUno.options4.promptServerNotCashier = variableBits[6] - '0';
    desempaquetarTablaUno.options4.batchReviewByCashier = variableBits[5] - '0';
    desempaquetarTablaUno.options4.tenderCashPrompt = variableBits[4] - '0';
    desempaquetarTablaUno.options4.reserved4 = variableBits[3] - '0';
    desempaquetarTablaUno.options4.reserved5 = variableBits[2] - '0';
    desempaquetarTablaUno.options4.duplicateProcessing = variableBits[1] - '0';
    desempaquetarTablaUno.options4.unused = variableBits[0] - '0';
    indice += 1;
    ///////Options 4///////
    memset(dataAux, 0x00, sizeof(dataAux));
    memcpy(dataAux, dataTablaUno + indice, 23);
    rightTrim_(dataAux, 0x20);
    leftTrim_(dataAux, 0x20);
    strcpy(desempaquetarTablaUno.receiptLine2, dataAux);
    indice += 23;
    memset(dataAux, 0x00, sizeof(dataAux));
    memcpy(dataAux, dataTablaUno + indice, 23);
    rightTrim_(dataAux, 0x20);
    leftTrim_(dataAux, 0x20);

    memcpy(desempaquetarTablaUno.receiptLine3, dataAux, TAM_ID_COMERCIO);
    indice += 23;
    memset(dataAux, 0x00, sizeof(dataAux));
    memcpy(dataAux, dataTablaUno + indice, 23);
    rightTrim_(dataAux, 0x20);
    leftTrim_(dataAux, 0x20);
    strcpy(desempaquetarTablaUno.defaultMerchantName, dataAux);
    indice += 23;
    desempaquetarTablaUno.currencySymbol = dataTablaUno[indice];
    indice += 1;
    desempaquetarTablaUno.transactionCurrencyDigits = dataTablaUno[indice];
    indice += 1;
    desempaquetarTablaUno.currencyDecimalPointPosition = dataTablaUno[indice];
    indice += 1;
    desempaquetarTablaUno.languageIndicator = dataTablaUno[indice];
    indice += 1;
    desempaquetarTablaUno.settlementCurrencyDigits = dataTablaUno[indice];
    indice += 1;
    desempaquetarTablaUno.checkService = dataTablaUno[indice];
    indice += 1;
    desempaquetarTablaUno.checkGuaranteeIssuerTableId = dataTablaUno[indice];
    indice += 1;
    desempaquetarTablaUno.checkGuaranteeAcquirerTableId = dataTablaUno[indice];
    indice += 1;
    ///////Local Terminal Options///////
    memset(variableBits, 0x00, sizeof(variableBits));
    memcpy(variableBits, dataTablaUno + indice, 1);
    calcularBitsEncendidos(variableBits);
    desempaquetarTablaUno.localTerminalOptions.keyboardLocked = variableBits[7] - '0';
    desempaquetarTablaUno.localTerminalOptions.voidPassword = variableBits[6] - '0';
    desempaquetarTablaUno.localTerminalOptions.refundPassword = variableBits[5] - '0';
    desempaquetarTablaUno.localTerminalOptions.adjustmentPassword = variableBits[4] - '0';
    desempaquetarTablaUno.localTerminalOptions.reportPassword = variableBits[3] - '0';
    desempaquetarTablaUno.localTerminalOptions.unused5 = variableBits[2] - '0';
    desempaquetarTablaUno.localTerminalOptions.unused6 = variableBits[1] - '0';
    desempaquetarTablaUno.localTerminalOptions.unused7 = variableBits[0] - '0';
    ///////Local Terminal Options///////
    indice += 1;
    desempaquetarTablaUno.defaultTransaction = dataTablaUno[indice];
    indice += 1;
    desempaquetarTablaUno.debitIssuerId = dataTablaUno[indice];
    indice += 1;
    desempaquetarTablaUno.debitAcquirerId = dataTablaUno[indice];
    indice += 1;
    desempaquetarTablaUno.phoneLineHold = dataTablaUno[indice];
    indice += 1;
    memcpy(desempaquetarTablaUno.reservedForFutureUse, dataTablaUno + indice, 2);
    indice += 2;
    desempaquetarTablaUno.pinPadType = dataTablaUno[indice];
    indice += 1;
    desempaquetarTablaUno.printerType = dataTablaUno[indice];
    indice += 1;
    memcpy(desempaquetarTablaUno.cashBackLimit, dataTablaUno + indice, 2);
    indice += 2;
    memcpy(desempaquetarTablaUno.maintenancePassword, dataTablaUno + indice, 3);
    memcpy(horaCierre, dataTablaUno + indice, 3);
    setParameter(HORA_CIERRE, horaCierre);
    getParameter(HORA_CIERRE, horaCierre);
    indice += 3;
    desempaquetarTablaUno.nmsCallSchedule = dataTablaUno[indice];
    indice += 1;
    ///////check Prompts1///////
    memset(variableBits, 0x00, sizeof(variableBits));
    memcpy(variableBits, dataTablaUno + indice, 1);
    calcularBitsEncendidos(variableBits);
    desempaquetarTablaUno.checkPrompts1.enterAddress = variableBits[7] - '0';
    desempaquetarTablaUno.checkPrompts1.driversLicenseNumber = variableBits[6] - '0';
    desempaquetarTablaUno.checkPrompts1.enterCheckNumber = variableBits[5] - '0';
    desempaquetarTablaUno.checkPrompts1.enterTransitNumber = variableBits[4] - '0';
    desempaquetarTablaUno.checkPrompts1.enterRouteNumber = variableBits[3] - '0';
    desempaquetarTablaUno.checkPrompts1.enterLastFourDigitsOfAccount = variableBits[2] - '0';
    desempaquetarTablaUno.checkPrompts1.enterCheckAccountNumber = variableBits[1] - '0';
    desempaquetarTablaUno.checkPrompts1.enterMicrNumber = variableBits[0] - '0';
    ///////check Prompts1///////
    indice += 1;
    ///////check Prompts2///////
    memset(variableBits, 0x00, sizeof(variableBits));
    memcpy(variableBits, dataTablaUno + indice, 1);
    calcularBitsEncendidos(variableBits);
    desempaquetarTablaUno.checkPrompts2.expirationDateMMYY = variableBits[7] - '0';
    desempaquetarTablaUno.checkPrompts2.idlePrompt = variableBits[6] - '0';
    desempaquetarTablaUno.checkPrompts2.enterTransCode = variableBits[5] - '0';
    desempaquetarTablaUno.checkPrompts2.enterIdNumber = variableBits[4] - '0';
    desempaquetarTablaUno.checkPrompts2.birthdateMMDDYY = variableBits[3] - '0';
    desempaquetarTablaUno.checkPrompts2.enterPhoneNumber = variableBits[2] - '0';
    desempaquetarTablaUno.checkPrompts2.enterZipCode = variableBits[1] - '0';
    desempaquetarTablaUno.checkPrompts2.stateCode = variableBits[0] - '0';
    ///////check Prompts2///////
    indice += 1;
    ///////check Prompts3///////
    memset(variableBits, 0x00, sizeof(variableBits));
    memcpy(variableBits, dataTablaUno + indice, 1);
    calcularBitsEncendidos(variableBits);
    desempaquetarTablaUno.checkPrompts3.notUsed0 = variableBits[7] - '0';
    desempaquetarTablaUno.checkPrompts3.notUsed1 = variableBits[6] - '0';
    desempaquetarTablaUno.checkPrompts3.notUsed2 = variableBits[5] - '0';
    desempaquetarTablaUno.checkPrompts3.notUsed3 = variableBits[4] - '0';
    desempaquetarTablaUno.checkPrompts3.notUsed4 = variableBits[3] - '0';
    desempaquetarTablaUno.checkPrompts3.notUsed5 = variableBits[2] - '0';
    desempaquetarTablaUno.checkPrompts3.notUsed6 = variableBits[1] - '0';
    desempaquetarTablaUno.checkPrompts3.enterCheckNumberIfNotScanned = variableBits[0] - '0';
    ///////check Prompts3///////
    indice += 1;
    memcpy(desempaquetarTablaUno.merchantSurcharge, dataTablaUno + indice, 2);
    indice += 2;
    ///////Options 5///////
    memset(variableBits, 0x00, sizeof(variableBits));
    memcpy(variableBits, dataTablaUno + indice, 1);
    calcularBitsEncendidos(variableBits);
    desempaquetarTablaUno.options5.hostSelectionType = variableBits[7] - '0';
    desempaquetarTablaUno.options5.noHostSelectPromptAtSettlement = variableBits[6] - '0';
    desempaquetarTablaUno.options5.notUsed2 = variableBits[5] - '0';
    desempaquetarTablaUno.options5.notUsed3 = variableBits[4] - '0';
    desempaquetarTablaUno.options5.notUsed4 = variableBits[3] - '0';
    desempaquetarTablaUno.options5.notUsed5 = variableBits[2] - '0';
    desempaquetarTablaUno.options5.notUsed6 = variableBits[1] - '0';
    desempaquetarTablaUno.options5.notUsed7 = variableBits[0] - '0';
    ///////Options 5///////
    indice += 1;
    memcpy(desempaquetarTablaUno.reservedForFutureUse2, dataTablaUno + indice, 8);

    indice += 7;
    memcpy(desempaquetarTablaUno.maintenancePassword, dataTablaUno + indice, 3);

    memset(dataAux, 0x00, sizeof(dataAux));

    setParameter(CODIGO_COMERCIO, desempaquetarTablaUno.receiptLine3);
    setParameter(NOMBRE_COMERCIO, desempaquetarTablaUno.defaultMerchantName);
    setParameter(DIRECCION_COMERCIO, desempaquetarTablaUno.receiptLine2);

    sprintf(dataAux, "%02x", desempaquetarTablaUno.ecrBaudRate);
    setParameter(MAXIMO_IVA, dataAux);
    setParameter(MAXIMO_INC, dataAux);

    return desempaquetarTablaUno;
}


void leftTrim_(uChar *dataLeft, uChar caracter) {

    uChar dataAux[50 + 1];
    int indice = 0;
    int contador = 0;
    int tamSring = 0;

    memset(dataAux, 0x00, sizeof(dataAux));

    tamSring = strlen(dataLeft);

    for (indice = 0; indice < tamSring; indice++) {
        if (dataLeft[indice] == caracter) {
            contador++;
        } else {
            break;
        }
    }

    memcpy(dataAux, dataLeft + contador, (tamSring - contador));
    memset(dataLeft, 0x00, sizeof(dataLeft));
    strcpy(dataLeft, dataAux);

}


void rightTrim_(uChar *dataRight, uChar caracter) {

    int indice = 0;
    int tamString = 0;

    tamString = strlen(dataRight);

    for (indice = (tamString - 1); indice >= 0; indice--) {
        if (dataRight[indice] == caracter) {
            dataRight[indice] = 0x00;
        } else {
            break;
        }
    }
}


void _leftPad_(uChar *destination, uChar *source, char fill, int len) {

    int longitud = 0;
    int indice = 0;

    memset(destination, 0x00, sizeof(destination));
    strcpy(destination, source);
    longitud = strlen(source);

    if (longitud < len) {
        indice = len - longitud;
        memset(destination, fill, len);
        memcpy(destination + indice, source, longitud);
    }
}

int _armarTokenAutorizacion_(char *buffer) {

    int idx = 0;
    int longitud = 0;

    unsigned char *p;
    unsigned int local_addr = 190249041;
    //T_DGPRS_GET_INFORMATION infoGPRS;

    uChar szTemp[32];
    uChar filler[10 + 1];
    uChar serial[18 + 1];
    uChar usuario[10 + 1];
    unsigned char dataAux[20 + 1];
    uChar mac_addr[12 + 1];
    uChar tokenQZ[80 + 1];
    uChar auxiliar[25 + 1];
    uChar tipoComunicacion[2 + 1];
    uChar consecutivo[6 + 1];
    char dataLeida[100 + 1] = {0x00};
    LOGI("dataLeida ");
    char parametrosFile[10][20 + 1] = {0x00};
    LOGI("dataLeida A");
    char *token;
    ResultTokenPack resultTokenPackQZ;

    memset(filler, 0x00, sizeof(filler));
    memset(serial, 0x00, sizeof(serial));
    memset(usuario, 0x00, sizeof(usuario));
    memset(dataAux, 0x00, sizeof(dataAux));
    memset(tokenQZ, 0x00, sizeof(tokenQZ));
    memset(mac_addr, 0x00, sizeof(mac_addr));
    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(tipoComunicacion, 0x00, sizeof(tipoComunicacion));
    memset(consecutivo, 0x00, sizeof(consecutivo));

    getParameter(USER_CNB, usuario);
    getParameter(CONSECUTIVO_CNB, consecutivo);

    getParameter(TIPO_COMUNICACION, tipoComunicacion);
    LOGI("dataLeida b");
    leerArchivo(discoNetcom,"/PARAMTERMINAL.txt",dataLeida);
    LOGI("dataLeida %s ", dataLeida);
    token = strtok(dataLeida, ";");
    memcpy(parametrosFile[idx],token, strlen(token));
    LOGI("parametrosFile[0] %s ", parametrosFile[idx]);
    while( token != NULL ) {
        idx++;
        token = strtok(NULL, ";");
        if(token != NULL){
            memcpy(parametrosFile[idx],token, strlen(token));
        }
    }
    LOGI("parametrosFile[0] %s ", parametrosFile[0]);
    LOGI("parametrosFile[1] %s ", parametrosFile[1]);
    LOGI("parametrosFile[2] %s ", parametrosFile[2]);

    memcpy(serial,  parametrosFile[1], 8);
    leftPad(auxiliar, serial, 0x20, 18);
    strcpy(tokenQZ, auxiliar);

    memset(auxiliar, 0x00, sizeof(auxiliar));

    leftPad(mac_addr,parametrosFile[2],0x30,12);
    LOGI("mac_addr %s ", mac_addr);
    strcat(tokenQZ, mac_addr);

    strcpy(szTemp,  parametrosFile[0]);//buscar funcion para obtener serial
    strcat(tokenQZ, szTemp);

    memset(auxiliar, 0x00, sizeof(auxiliar));
    leftPad(auxiliar, usuario, 0x20, 10);
    strcat(tokenQZ, auxiliar);
    strcat(tokenQZ, consecutivo);

    memset(auxiliar, 0x00, sizeof(auxiliar));
    leftPad(auxiliar, filler, 0x20, 10);
    strcat(tokenQZ, auxiliar);

    _setTokenMessage_((char *) "QZ", tokenQZ, 68);

    resultTokenPackQZ = _packTokenMessage_();

    longitud = resultTokenPackQZ.totalBytes;
    memcpy(buffer, resultTokenPackQZ.tokenPackMessage, resultTokenPackQZ.totalBytes);


    return longitud;
}

TablaTresInicializacion _traerIssuer_(long bin) {

    int resultadoIssuer = 0;
    int respuestaBin = 0;
    int posicion = 0;
    int indiceBin = 0;
    int lengthFile = 0;
    int index = 0;
    int idx = 0;
    int lengthTabla2 = 37;
    uChar tablaDosDesempaquetar[lengthTabla2 + 1];
    uChar rangoBajo[10 + 1];
    uChar rangoAlto[10 + 1];
    TablaTresInicializacion dataTarjeta;

    memset(&dataTarjeta, 0x00, sizeof(dataTarjeta));
    memset(&RangoBinesEncontrados_, 0x00, sizeof(RangoBinesEncontrados_));

    lengthFile = tamArchivo(discoNetcom, (char *) TABLA_2_INICIALIZACION);

    if (lengthFile >= lengthTabla2) {

        uChar dataTabla2[lengthFile + 1];
        memset(dataTabla2, 0x00, sizeof(dataTabla2));

        leerArchivo(discoNetcom, (char *) TABLA_2_INICIALIZACION, dataTabla2);

        index = lengthFile / lengthTabla2;

        for (idx = 0; idx < index; idx++) {
            // Seteamos variables a null
            memset(tablaDosDesempaquetar, 0x00, sizeof(tablaDosDesempaquetar));
            memset(rangoBajo, 0x00, sizeof(rangoBajo));
            memset(rangoAlto, 0x00, sizeof(rangoAlto));

            // Leemos Tabla de Bines
            memcpy(tablaDosDesempaquetar, dataTabla2 + posicion, lengthTabla2);

            if (resultadoIssuer < 0) {
                dataTarjeta.respuesta = -1;
                break;
            }
            // Seteamos variables con los datos requeridos
            memcpy(rangoBajo, tablaDosDesempaquetar + 4, 5);
            memcpy(rangoAlto, tablaDosDesempaquetar + 9, 5);
            // Buscamos Bines
            respuestaBin = buscarBin(rangoBajo, rangoAlto, bin);

            if (respuestaBin == 1 && indiceBin <= 12) {
                strcpy(RangoBinesEncontrados_[indiceBin].panRangeHigh, rangoAlto);
                strcpy(RangoBinesEncontrados_[indiceBin].panRangeLow, rangoBajo);
                RangoBinesEncontrados_[indiceBin].issuerTableId = tablaDosDesempaquetar[14];
                RangoBinesEncontrados_[indiceBin].acquirerTableID = tablaDosDesempaquetar[15];
                indiceBin++;
            }
            posicion += lengthTabla2;
        }
    }

    if (indiceBin > 0) {
        respuestaBin = buscarBinOptimo(RangoBinesEncontrados_, indiceBin);
        dataTarjeta = buscarIssuer(RangoBinesEncontrados_[respuestaBin].issuerTableId);
        dataTarjeta.respuesta = 1;
    } else {
        dataTarjeta.respuesta = -1;
    }

    return dataTarjeta;
}

int buscarBin(uChar *rangoBajo, uChar *rangoAlto, long bin) {

    int resultado = -1;
    uChar rangoBajo_[10 + 1];
    uChar rangoAlto_[10 + 1];

    memset(rangoBajo_, 0x00, sizeof(rangoBajo_));
    memset(rangoAlto_, 0x00, sizeof(rangoAlto_));

    _convertBCDToASCII_(rangoBajo_, rangoBajo, 9);
    _convertBCDToASCII_(rangoAlto_, rangoAlto, 9);

    rangoBajo_[9] = 0x00;
    rangoAlto_[9] = 0x00;

    if (bin >= atol(rangoBajo_) && bin <= atol(rangoAlto_)) {
        memset(rangoBajo, 0x00, sizeof(rangoBajo));
        memset(rangoAlto, 0x00, sizeof(rangoAlto));
        strcpy(rangoBajo, rangoBajo_);
        strcpy(rangoAlto, rangoAlto_);
        resultado = 1;
    }

    return resultado;
}

int buscarBinOptimo(RangoBinesEncontrados *rangoBin, int cantidadBines) {

    //selecciona el rango mas pequeño q lo contenga
    int idx = 0;
    long menor = 999999999;
    int resultado = 0;
    long diferencia = 0;

    for (idx = 0; idx < cantidadBines; idx++) {

        diferencia = atol(rangoBin[idx].panRangeHigh) - atol(rangoBin[idx].panRangeLow);

        if (diferencia <= menor) {
            menor = diferencia;
            resultado = idx;
        }
    }
    return resultado;
}

TablaTresInicializacion buscarIssuer(uChar issuerTableId) {

    int posicion = 0;
    int lengthTabla3 = 53;
    int lengthFile = 0;
    int index = 0;
    int idx = 0;
    uChar tablaTresDesempaquetar[lengthTabla3 + 1];
    uChar issuerTableId_;
    TablaTresInicializacion dataIssuer;

    memset(&dataIssuer, 0x00, sizeof(dataIssuer));

    lengthFile = tamArchivo(discoNetcom, (char *) TABLA_3_INICIALIZACION);

    if (lengthFile >= lengthTabla3) {

        uChar dataTabla3[lengthFile + 1];
        memset(dataTabla3, 0x00, sizeof(dataTabla3));

        leerArchivo(discoNetcom, (char *) TABLA_3_INICIALIZACION, dataTabla3);

        index = lengthFile / lengthTabla3;

        for (idx = 0; idx < index; idx++) {

            memset(tablaTresDesempaquetar, 0x00, sizeof(tablaTresDesempaquetar));

            /** Leer data tabla 3 **/
            memcpy(tablaTresDesempaquetar, dataTabla3 + posicion, lengthTabla3);

            issuerTableId_ = tablaTresDesempaquetar[4];

            if (issuerTableId_ == issuerTableId) {
                dataIssuer = dataTablaTres(tablaTresDesempaquetar);
                break;
            }
            posicion += 56;
        }
    }

    return dataIssuer;
}

TablaTresInicializacion dataTablaTres(uChar *tablaTres) {

    TablaTresInicializacion dataIssuer;
    uChar variableBits[8 + 1];
    int posicion = 3;

    memset(&dataIssuer, 0x00, sizeof(dataIssuer));

    dataIssuer.issuerNumber = tablaTres[posicion];
    posicion += 1;
    dataIssuer.issuerTableId = tablaTres[posicion];
    posicion += 1;
    memcpy(dataIssuer.cardName, tablaTres + posicion, 10);
    posicion += 10;
    memcpy(dataIssuer.referralPhoneNumber, tablaTres + posicion, 12);
    posicion += 12;
    ///////Options 1///////
    memset(variableBits, 0x00, sizeof(variableBits));
    memcpy(variableBits, tablaTres + posicion, 1);
    calcularBitsEncendidos(variableBits);
    //printf("\x1B""oP1:%s",variableBits);ttestall(0,800);
    dataIssuer.options1.selectAccountType = variableBits[7] - '0';
    dataIssuer.options1.pinEntry = variableBits[6] - '0';
    dataIssuer.options1.manualPanAllowed = variableBits[5] - '0';
    dataIssuer.options1.expirationDateRequired = variableBits[4] - '0';
    dataIssuer.options1.offLineEntryAllowed = variableBits[3] - '0';
    dataIssuer.options1.voiceReferalAllowed = variableBits[2] - '0';
    dataIssuer.options1.descriptorsRequired = variableBits[1] - '0';
    dataIssuer.options1.adjustAllowed = variableBits[0] - '0';
    ///////Options 1///////
    posicion += 1;
    ///////Options 2///////
    memset(variableBits, 0x00, sizeof(variableBits));
    memcpy(variableBits, tablaTres + posicion, 1);
    calcularBitsEncendidos(variableBits);
    dataIssuer.options2.performMod10CheckOnPAN = variableBits[7] - '0';
    dataIssuer.options2.invoiceNumberRequired = variableBits[6] - '0';
    dataIssuer.options2.printReceipt = variableBits[5] - '0';
    dataIssuer.options2.captureTransactions = variableBits[4] - '0';
    dataIssuer.options2.checkExpirationDate = variableBits[3] - '0';
    dataIssuer.options2.beMyGuest = variableBits[2] - '0';
    dataIssuer.options2.blockRefunds = variableBits[1] - '0';
    dataIssuer.options2.blockCardVerify = variableBits[0] - '0';
    ///////Options 2///////
    posicion += 1;
    ///////Options 3///////
    memset(variableBits, 0x00, sizeof(variableBits));
    memcpy(variableBits, tablaTres + posicion, 1);
    calcularBitsEncendidos(variableBits);
    dataIssuer.options3.blockVoids = variableBits[7] - '0';
    dataIssuer.options3.cashBack = variableBits[6] - '0';
    dataIssuer.options3.printSignatureNotRequired = variableBits[5] - '0';
    dataIssuer.options3.printNoRefundsAllowed = variableBits[4] - '0';
    dataIssuer.options3.refundProcessingOffline = variableBits[3] - '0';
    dataIssuer.options3.voidProcessingOffline = variableBits[2] - '0';
    dataIssuer.options3.allowCashOnlyTransaction = variableBits[1] - '0';
    dataIssuer.options3.generateSixDigitAuthCode = variableBits[0] - '0';
    ///////Options 3///////
    posicion += 1;
    ///////Options 4///////
    memset(variableBits, 0x00, sizeof(variableBits));
    memcpy(variableBits, tablaTres + posicion, 1);
    calcularBitsEncendidos(variableBits);
    dataIssuer.options4.debitTransaction = variableBits[7] - '0';
    dataIssuer.options4.verifyLastFourPANDigits = variableBits[6] - '0';
    dataIssuer.options4.authorizeOverFloorLimit = variableBits[5] - '0';
    dataIssuer.options4.autoReprintFailedReceipt = variableBits[4] - '0';
    dataIssuer.options4.ignoreTrack1Error = variableBits[3] - '0';
    dataIssuer.options4.promptForTax = variableBits[2] - '0';
    dataIssuer.options4.promptForFiveDigitCreditPlan = variableBits[1] - '0';
    dataIssuer.options4.promptForProductCodes = variableBits[0] - '0';
    ///////Options 4///////
    posicion += 1;
    dataIssuer.defaultAccount = tablaTres[posicion];
    posicion += 1;
    memcpy(dataIssuer.reservedForFutureUse, tablaTres + posicion, 2);
    posicion += 2;
    memcpy(dataIssuer.floorLimit, tablaTres + posicion, 2);
    posicion += 2;
    dataIssuer.reauthorizationMarginPercentage = tablaTres[posicion];
    posicion += 1;
    ///////pin Entry Transactions///////
    memset(variableBits, 0x00, sizeof(variableBits));
    memcpy(variableBits, tablaTres + posicion, 1);
    calcularBitsEncendidos(variableBits);
    dataIssuer.pinEntryTransactions.pinDisabledForDebit = variableBits[7] - '0';
    dataIssuer.pinEntryTransactions.pinDisabledForReturn = variableBits[6] - '0';
    dataIssuer.pinEntryTransactions.pinDisabledForVoid = variableBits[5] - '0';
    dataIssuer.pinEntryTransactions.pinDisabledForAdjust = variableBits[4] - '0';
    dataIssuer.pinEntryTransactions.pinDisabledForCashback = variableBits[3] - '0';
    dataIssuer.pinEntryTransactions.pinDisabledForBalanceInquiry = variableBits[2] - '0';
    dataIssuer.pinEntryTransactions.pinDisabledForPreauthorization = variableBits[1] - '0';
    dataIssuer.pinEntryTransactions.unused = variableBits[0] - '0';
    ///////pin Entry Transactions///////
    posicion += 1;
    ///////Additional Prompts Select///////
    memset(variableBits, 0x00, sizeof(variableBits));
    memcpy(variableBits, tablaTres + posicion, 1);
    calcularBitsEncendidos(variableBits);
    dataIssuer.additionalPromptsSelect.additionalPrompt1 = variableBits[7] - '0'; // DOCUMENTO
    dataIssuer.additionalPromptsSelect.additionalPrompt2 = variableBits[6] - '0'; // CVC2
    dataIssuer.additionalPromptsSelect.additionalPrompt3 = variableBits[5] - '0'; // LIBRE
    dataIssuer.additionalPromptsSelect.additionalPrompt4 =
            variableBits[4] - '0'; // COSTO TRANSACCION
    dataIssuer.additionalPromptsSelect.additionalPrompt5 = variableBits[3] - '0'; // LIBRE
    dataIssuer.additionalPromptsSelect.additionalPrompt6 = variableBits[2] - '0'; // QPS
    dataIssuer.additionalPromptsSelect.additionalPrompt7 =
            variableBits[1] - '0'; // SELECCION TIPO DOCUMENTO
    dataIssuer.additionalPromptsSelect.additionalPrompt8 = variableBits[0] - '0'; // CMR EXTENDIDO
    ///////Additional Prompts Select///////
    posicion += 1;
    dataIssuer.cardType = tablaTres[posicion];
    posicion += 1;
    dataIssuer.downPaymentIndicator = tablaTres[posicion];
    posicion += 1;
    ///////options 5///////
    memset(variableBits, 0x00, sizeof(variableBits));
    memcpy(variableBits, tablaTres + posicion, 1);
    calcularBitsEncendidos(variableBits);
    dataIssuer.options5.doNotCaptureDebitTransactions = variableBits[7] - '0';
    dataIssuer.options5.etbProcessing = variableBits[6] - '0';
    dataIssuer.options5.generationNumberPrompt = variableBits[5] - '0';
    dataIssuer.options5.additionalRefundAmountPrompts = variableBits[4] - '0';
    dataIssuer.options5.purchaseOrderNumberPrompt = variableBits[3] - '0';
    dataIssuer.options5.barAmountPrompt = variableBits[2] - '0';
    dataIssuer.options5.unused = variableBits[1] - '0';
    dataIssuer.options5.disablePrintingRrn = variableBits[0] - '0';
    ///////options 5///////
    posicion += 1;
    ///////options 6///////
    memset(variableBits, 0x00, sizeof(variableBits));
    memcpy(variableBits, tablaTres + posicion, 1);
    calcularBitsEncendidos(variableBits);
    dataIssuer.options6.AvsEnabledOnSale = variableBits[7] - '0';
    dataIssuer.options6.cardholderAddressRequired = variableBits[5] - '0';
    dataIssuer.options6.cardholderZipCodeRequired = variableBits[6] - '0';
    dataIssuer.options6.unused3 = variableBits[4] - '0';
    dataIssuer.options6.unused4 = variableBits[3] - '0';
    dataIssuer.options6.unused5 = variableBits[2] - '0';
    dataIssuer.options6.unused6 = variableBits[1] - '0';
    dataIssuer.options6.unused7 = variableBits[0] - '0';
    ///////options 6///////
    posicion += 1;
    memcpy(dataIssuer.reservedForFutureUse2, tablaTres + posicion, 4);
    return dataIssuer;
}


int buscarBinEmisor(uChar *binEmisor) {

    int resultado = -1;
    int control = 0;
    int verificarTablaEmisores = 0;
    long lengthBinesEmisores = 0;
    long binAux = 0;
    uChar rangoBajo[10 + 1];
    uChar rangoAlto[10 + 1];
    uChar bloqueBinesEmisores[LENGTH_BINES_EMISORES + 1];

    verificarTablaEmisores = verificarArchivo(discoNetcom, BI_EMISORES_ADICIONALES);

    if (verificarTablaEmisores == FS_OK) {
        lengthBinesEmisores = tamArchivo(discoNetcom, (char *) BI_EMISORES_ADICIONALES);

        binAux = atol(binEmisor);

        do {
            memset(rangoBajo, 0x00, sizeof(rangoBajo));
            memset(rangoAlto, 0x00, sizeof(rangoAlto));
            memset(bloqueBinesEmisores, 0x00, sizeof(bloqueBinesEmisores));
            /////// COPIAMOS BLOQUE DE BINES ///////
            buscarArchivo(discoNetcom, (char *) BI_EMISORES_ADICIONALES, bloqueBinesEmisores,
                          control, LENGTH_BINES_EMISORES);
            memcpy(rangoBajo, bloqueBinesEmisores + 4, 5);
            memcpy(rangoAlto, bloqueBinesEmisores + 9, 5);
            /////// BUSCAMOS SI ESTA DENTRO DEL RANGO DE BINES /////////
            resultado = buscarBin(rangoBajo, rangoAlto, binAux);

            if (resultado == 1) {
                /////// RETORNAMOS INDICE EMISOR ////////
                resultado = bloqueBinesEmisores[3];
                break;
            }
            control += LENGTH_BINES_EMISORES;
        } while (control < lengthBinesEmisores);
    }
    return resultado;
}

EmisorAdicional traerEmisorAdicional(int indiceEmisor) {

    EmisorAdicional datosEmisor;
    uChar bloqueEmisorAdicional[LENGTH_BLOQUE_EMISORES + 1];
    uChar variableAux[15 + 1];
    long lengthDatosEmisor = 0;
    int control = 0;

    lengthDatosEmisor = tamArchivo(discoNetcom, (char *) EMISORES_ADICIONALES);

    memset(variableAux, 0x00, sizeof(variableAux));
    memset(&datosEmisor, 0x00, sizeof(datosEmisor));
    //////// RECORREMOS LA TABLA DE ISSUER DE EMISORES ADICIONALES ///////
    do {
        memset(bloqueEmisorAdicional, 0x00, sizeof(bloqueEmisorAdicional));
        buscarArchivo(discoNetcom, (char *) EMISORES_ADICIONALES, bloqueEmisorAdicional, control,
                      LENGTH_BLOQUE_EMISORES);
        //////// VERIFICAMOS SI ES EL EMISOR //////////
        if (bloqueEmisorAdicional[3] == indiceEmisor) {
            ////// COPIAMOS LOS DATOS DEL EMISOR ///////////
            datosEmisor.indiceEmisor = bloqueEmisorAdicional[3];
            datosEmisor.indiceImagen = bloqueEmisorAdicional[19];
            memcpy(variableAux, bloqueEmisorAdicional + 4, 15);
            _rightTrim_(datosEmisor.nombreEmisor, variableAux, ' ');
            datosEmisor.existeIssuerEmisor = 1;
            break;
        }
        control += LENGTH_BLOQUE_EMISORES;
    } while (control < lengthDatosEmisor);

    return datosEmisor;
}

TablaUnoInicializacion desempaquetarCNB(void) {

    TablaUnoInicializacion desempaquetarTablaUno;
    uChar variableBits[9]; //Manejo de bits
    uChar dataTablaUno[1024];
    uChar dataAux[23 + 1];

    int indice = 6;
    memset(&desempaquetarTablaUno, 0x00, sizeof(desempaquetarTablaUno));
    memset(variableBits, 0x00, sizeof(variableBits));
    memset(dataTablaUno, 0x00, sizeof(dataTablaUno));

    leerArchivo(discoNetcom, (char *) TABLA_CNB, dataTablaUno);

    desempaquetarTablaUno.dllTraking = dataTablaUno[indice];
    indice += 1;
    desempaquetarTablaUno.initializationControl = dataTablaUno[indice];
    indice += 1;
    desempaquetarTablaUno.printerBaudRate = dataTablaUno[indice];
    indice += 1;
    desempaquetarTablaUno.ecrBaudRate = dataTablaUno[indice];
    indice += 1;
    memcpy(desempaquetarTablaUno.dateandTime, dataTablaUno + indice, 6);
    indice += 6;
    ///////telephoneDialOptions///////
    memcpy(variableBits, dataTablaUno + indice, 1);
    calcularBitsEncendidos(variableBits);
    desempaquetarTablaUno.telephoneDialOptions.unusedCero = variableBits[7] - '0';
    desempaquetarTablaUno.telephoneDialOptions.unusedUno = variableBits[6] - '0';
    desempaquetarTablaUno.telephoneDialOptions.unusedDos = variableBits[5] - '0';
    desempaquetarTablaUno.telephoneDialOptions.autoAnswer = variableBits[4] - '0';
    desempaquetarTablaUno.telephoneDialOptions.referralDialing = variableBits[3] - '0';
    desempaquetarTablaUno.telephoneDialOptions.memoryDialing = variableBits[2] - '0';
    desempaquetarTablaUno.telephoneDialOptions.tollDialing = variableBits[1] - '0';
    desempaquetarTablaUno.telephoneDialOptions.keyboardDialing = variableBits[0] - '0';
    indice += 1;
    ///////telephoneDialOptions///////
    memcpy(desempaquetarTablaUno.password, dataTablaUno + indice, 2);
    indice += 2;
    memcpy(desempaquetarTablaUno.helpDeskPhone, dataTablaUno + indice, 12);
    indice += 12;
    ///////Options 1///////
    memset(variableBits, 0x00, sizeof(variableBits));
    memcpy(variableBits, dataTablaUno + indice, 1);
    calcularBitsEncendidos(variableBits);
    desempaquetarTablaUno.options1.amountDualEntry = variableBits[7] - '0';
    desempaquetarTablaUno.options1.displayMagneticStripeReaderInput = variableBits[6] - '0';
    desempaquetarTablaUno.options1.tipProcessing = variableBits[5] - '0';
    desempaquetarTablaUno.options1.cashierProcessing = variableBits[4] - '0';
    desempaquetarTablaUno.options1.lodgingProcessing = variableBits[3] - '0';
    desempaquetarTablaUno.options1.printTimeOnReceipt = variableBits[2] - '0';
    desempaquetarTablaUno.options1.businessDateFormat = variableBits[1] - '0';
    desempaquetarTablaUno.options1.reconciliationMethod = variableBits[0] - '0';
    indice += 1;
    ///////Options 1///////
    ///////Options 2///////
    memset(variableBits, 0x00, sizeof(variableBits));
    memcpy(variableBits, dataTablaUno + indice, 1);
    calcularBitsEncendidos(variableBits);
    desempaquetarTablaUno.options2.reserved = variableBits[7] - '0';
    desempaquetarTablaUno.options2.printerUsed = variableBits[6] - '0';
    desempaquetarTablaUno.options2.printTotalOnReceipt = variableBits[5] - '0';
    desempaquetarTablaUno.options2.tillProcessing = variableBits[4] - '0';
    desempaquetarTablaUno.options2.refundTransactionsProcessedOffline = variableBits[3] - '0';
    desempaquetarTablaUno.options2.voidTransactionsProcessedOffline = variableBits[2] - '0';
    desempaquetarTablaUno.options2.enableReceiptPreprint = variableBits[1] - '0';
    desempaquetarTablaUno.options2.CardVerifyKeyOrDebitKey = variableBits[0] - '0';
    indice += 1;
    ///////Options 2///////
    ///////Options 3///////
    memset(variableBits, 0x00, sizeof(variableBits));
    memcpy(variableBits, dataTablaUno + indice, 1);
    calcularBitsEncendidos(variableBits);
    desempaquetarTablaUno.options3.unusedCero = variableBits[7] - '0';
    desempaquetarTablaUno.options3.electronicCardReaderInterface = variableBits[6] - '0';
    desempaquetarTablaUno.options3.amountVerifyOnPinPad = variableBits[5] - '0';
    desempaquetarTablaUno.options3.printCardVerifyReceipt = variableBits[4] - '0';
    desempaquetarTablaUno.options3.takeImprintPromptManualEntry = variableBits[3] - '0';
    desempaquetarTablaUno.options3.checkReaderInstalled = variableBits[2] - '0';
    desempaquetarTablaUno.options3.cardReadFromPinPad = variableBits[1] - '0';
    desempaquetarTablaUno.options3.unusedSiete = variableBits[0] - '0';
    indice += 1;
    ///////Options 3///////
    ///////Options 4///////
    memset(variableBits, 0x00, sizeof(variableBits));
    memcpy(variableBits, dataTablaUno + indice, 1);
    calcularBitsEncendidos(variableBits);
    desempaquetarTablaUno.options4.adjustByCashier = variableBits[7] - '0';
    desempaquetarTablaUno.options4.promptServerNotCashier = variableBits[6] - '0';
    desempaquetarTablaUno.options4.batchReviewByCashier = variableBits[5] - '0';
    desempaquetarTablaUno.options4.tenderCashPrompt = variableBits[4] - '0';
    desempaquetarTablaUno.options4.reserved4 = variableBits[3] - '0';
    desempaquetarTablaUno.options4.reserved5 = variableBits[2] - '0';
    desempaquetarTablaUno.options4.duplicateProcessing = variableBits[1] - '0';
    desempaquetarTablaUno.options4.unused = variableBits[0] - '0';
    indice += 1;
    ///////Options 4///////
    memset(dataAux, 0x00, sizeof(dataAux));
    memcpy(dataAux, dataTablaUno + indice, 23);
    rightTrim_(dataAux, 0x20);
    leftTrim_(dataAux, 0x20);

    memcpy(desempaquetarTablaUno.receiptLine2, dataAux, strlen(dataAux));

    indice += 23;
    memset(dataAux, 0x00, sizeof(dataAux));
    memcpy(dataAux, dataTablaUno + indice, 23);
    rightTrim_(dataAux, 0x20);
    leftTrim_(dataAux, 0x20);
    memcpy(desempaquetarTablaUno.receiptLine3, dataAux, TAM_ID_COMERCIO);
    indice += 23;
    memset(dataAux, 0x00, sizeof(dataAux));
    memcpy(dataAux, dataTablaUno + indice, 23);
    rightTrim_(dataAux, 0x20);
    leftTrim_(dataAux, 0x20);
    strcpy(desempaquetarTablaUno.defaultMerchantName, dataAux);
    indice += 23;
    desempaquetarTablaUno.currencySymbol = dataTablaUno[indice];
    indice += 1;
    desempaquetarTablaUno.transactionCurrencyDigits = dataTablaUno[indice];
    indice += 1;
    desempaquetarTablaUno.currencyDecimalPointPosition = dataTablaUno[indice];
    indice += 1;
    desempaquetarTablaUno.languageIndicator = dataTablaUno[indice];
    indice += 1;
    desempaquetarTablaUno.settlementCurrencyDigits = dataTablaUno[indice];
    indice += 1;
    desempaquetarTablaUno.checkService = dataTablaUno[indice];
    indice += 1;
    desempaquetarTablaUno.checkGuaranteeIssuerTableId = dataTablaUno[indice];
    indice += 1;
    desempaquetarTablaUno.checkGuaranteeAcquirerTableId = dataTablaUno[indice];
    indice += 1;
    ///////Local Terminal Options///////
    memset(variableBits, 0x00, sizeof(variableBits));
    memcpy(variableBits, dataTablaUno + indice, 1);
    calcularBitsEncendidos(variableBits);
    desempaquetarTablaUno.localTerminalOptions.keyboardLocked = variableBits[7] - '0';
    desempaquetarTablaUno.localTerminalOptions.voidPassword = variableBits[6] - '0';
    desempaquetarTablaUno.localTerminalOptions.refundPassword = variableBits[5] - '0';
    desempaquetarTablaUno.localTerminalOptions.adjustmentPassword = variableBits[4] - '0';
    desempaquetarTablaUno.localTerminalOptions.reportPassword = variableBits[3] - '0';
    desempaquetarTablaUno.localTerminalOptions.unused5 = variableBits[2] - '0';
    desempaquetarTablaUno.localTerminalOptions.unused6 = variableBits[1] - '0';
    desempaquetarTablaUno.localTerminalOptions.unused7 = variableBits[0] - '0';
    ///////Local Terminal Options///////
    indice += 1;
    desempaquetarTablaUno.defaultTransaction = dataTablaUno[indice];
    indice += 1;
    desempaquetarTablaUno.debitIssuerId = dataTablaUno[indice];
    indice += 1;
    desempaquetarTablaUno.debitAcquirerId = dataTablaUno[indice];
    indice += 1;
    desempaquetarTablaUno.phoneLineHold = dataTablaUno[indice];
    indice += 1;
    memcpy(desempaquetarTablaUno.reservedForFutureUse, dataTablaUno + indice, 2);
    indice += 2;
    desempaquetarTablaUno.pinPadType = dataTablaUno[indice];
    indice += 1;
    desempaquetarTablaUno.printerType = dataTablaUno[indice];
    indice += 1;
    memcpy(desempaquetarTablaUno.cashBackLimit, dataTablaUno + indice, 2);
    indice += 2;
    memcpy(desempaquetarTablaUno.maintenancePassword, dataTablaUno + indice, 3);
    indice += 3;
    desempaquetarTablaUno.nmsCallSchedule = dataTablaUno[indice];
    indice += 1;
    ///////check Prompts1///////
    memset(variableBits, 0x00, sizeof(variableBits));
    memcpy(variableBits, dataTablaUno + indice, 1);
    calcularBitsEncendidos(variableBits);
    desempaquetarTablaUno.checkPrompts1.enterAddress = variableBits[7] - '0';
    desempaquetarTablaUno.checkPrompts1.driversLicenseNumber = variableBits[6] - '0';
    desempaquetarTablaUno.checkPrompts1.enterCheckNumber = variableBits[5] - '0';
    desempaquetarTablaUno.checkPrompts1.enterTransitNumber = variableBits[4] - '0';
    desempaquetarTablaUno.checkPrompts1.enterRouteNumber = variableBits[3] - '0';
    desempaquetarTablaUno.checkPrompts1.enterLastFourDigitsOfAccount = variableBits[2] - '0';
    desempaquetarTablaUno.checkPrompts1.enterCheckAccountNumber = variableBits[1] - '0';
    desempaquetarTablaUno.checkPrompts1.enterMicrNumber = variableBits[0] - '0';
    ///////check Prompts1///////
    indice += 1;
    ///////check Prompts2///////
    memset(variableBits, 0x00, sizeof(variableBits));
    memcpy(variableBits, dataTablaUno + indice, 1);
    calcularBitsEncendidos(variableBits);
    desempaquetarTablaUno.checkPrompts2.expirationDateMMYY = variableBits[7] - '0';
    desempaquetarTablaUno.checkPrompts2.idlePrompt = variableBits[6] - '0';
    desempaquetarTablaUno.checkPrompts2.enterTransCode = variableBits[5] - '0';
    desempaquetarTablaUno.checkPrompts2.enterIdNumber = variableBits[4] - '0';
    desempaquetarTablaUno.checkPrompts2.birthdateMMDDYY = variableBits[3] - '0';
    desempaquetarTablaUno.checkPrompts2.enterPhoneNumber = variableBits[2] - '0';
    desempaquetarTablaUno.checkPrompts2.enterZipCode = variableBits[1] - '0';
    desempaquetarTablaUno.checkPrompts2.stateCode = variableBits[0] - '0';
    ///////check Prompts2///////
    indice += 1;
    ///////check Prompts3///////
    memset(variableBits, 0x00, sizeof(variableBits));
    memcpy(variableBits, dataTablaUno + indice, 1);
    calcularBitsEncendidos(variableBits);
    desempaquetarTablaUno.checkPrompts3.notUsed0 = variableBits[7] - '0';
    desempaquetarTablaUno.checkPrompts3.notUsed1 = variableBits[6] - '0';
    desempaquetarTablaUno.checkPrompts3.notUsed2 = variableBits[5] - '0';
    desempaquetarTablaUno.checkPrompts3.notUsed3 = variableBits[4] - '0';
    desempaquetarTablaUno.checkPrompts3.notUsed4 = variableBits[3] - '0';
    desempaquetarTablaUno.checkPrompts3.notUsed5 = variableBits[2] - '0';
    desempaquetarTablaUno.checkPrompts3.notUsed6 = variableBits[1] - '0';
    desempaquetarTablaUno.checkPrompts3.enterCheckNumberIfNotScanned = variableBits[0] - '0';
    ///////check Prompts3///////
    indice += 1;
    memcpy(desempaquetarTablaUno.merchantSurcharge, dataTablaUno + indice, 2);
    indice += 2;
    ///////Options 5///////
    memset(variableBits, 0x00, sizeof(variableBits));
    memcpy(variableBits, dataTablaUno + indice, 1);
    calcularBitsEncendidos(variableBits);
    desempaquetarTablaUno.options5.hostSelectionType = variableBits[7] - '0';
    desempaquetarTablaUno.options5.noHostSelectPromptAtSettlement = variableBits[6] - '0';
    desempaquetarTablaUno.options5.notUsed2 = variableBits[5] - '0';
    desempaquetarTablaUno.options5.notUsed3 = variableBits[4] - '0';
    desempaquetarTablaUno.options5.notUsed4 = variableBits[3] - '0';
    desempaquetarTablaUno.options5.notUsed5 = variableBits[2] - '0';
    desempaquetarTablaUno.options5.notUsed6 = variableBits[1] - '0';
    desempaquetarTablaUno.options5.notUsed7 = variableBits[0] - '0';
    ///////Options 5///////
    indice += 1;
    memcpy(desempaquetarTablaUno.reservedForFutureUse2, dataTablaUno + indice, 8);

    memset(dataAux, 0x00, sizeof(dataAux));

    return desempaquetarTablaUno;
}

TablaSeisInicializacion desempaquetarTablaSeis(uChar indiceGrupo) {

    uChar tabla6[22 + 1];
    int indice = 0;
    int posicion = 0;
    int tamanoTabla6 = 0;

    TablaSeisInicializacion tablaSeis;
    memset(&tablaSeis, 0x00, sizeof(tablaSeis));
    tamanoTabla6 = tamArchivo(discoNetcom, (char *)TABLA_6_INICIALIZACION);
    uChar todatabla6[tamanoTabla6 + 1];
    leerArchivo(discoNetcom, (char *)TABLA_6_INICIALIZACION, todatabla6);

    memcpy(tablaSeis.texto, "TOTAL FRANQUICIA", 16);
    do {
        indice = 4;
        memset(tabla6, 0x00, sizeof(tabla6));
        buscarArchivo(discoNetcom, (char *)TABLA_6_INICIALIZACION, tabla6, posicion, 22);

        tablaSeis.banderaAgrupacion = tabla6[indice];

        if (indiceGrupo == tablaSeis.banderaAgrupacion) {
            indice += 1;
            memcpy(tablaSeis.texto, tabla6 + indice, 20);

            rightTrim_(tablaSeis.texto, 0x20);

            tablaSeis.consecutivoRegistro = '1';
            break;
        }
        posicion += 25;
    } while (posicion < tamanoTabla6);

    return tablaSeis;
}

TablaConsultaInformacion traerConsultaInformacion(uChar *bin) {

    TablaConsultaInformacion consultaInformacion;
    uChar variableBits[8 + 1];
    DatosConsulta dataConsulta;
    char bufferConsulta[13 + 1];
    char binAux[10 + 1];
    int posicion = 0;
    int resultado = 0;

    resultado = tamArchivo((char *)DISCO_NETCOM, (char *)TABLA_CI);
    memset(&consultaInformacion, 0x00, sizeof(consultaInformacion));

    if (resultado >= 13) {
        do {
            memset(&consultaInformacion, 0x00, sizeof(consultaInformacion));
            memset(&dataConsulta, 0x00, sizeof(dataConsulta));
            memset(bufferConsulta, 0x00, sizeof(bufferConsulta));
            memset(binAux, 0x00, sizeof(binAux));

            resultado = buscarArchivo((char *)DISCO_NETCOM, (char *)TABLA_CI, bufferConsulta, posicion, 13);

            dataConsulta = obtenerDatosConsulta(bufferConsulta);
            _convertBCDToASCII_(binAux, dataConsulta.bin, 10);

            if (strncmp(binAux, bin, 6) == 0 ||
                strncmp(binAux, "000000", 6)) { //para el bin de la tarjeta.
                memset(variableBits, 0x00, sizeof(variableBits));
                memcpy(variableBits, dataConsulta.bitmapTipoTx, 1);
                calcularBitsEncendidos(variableBits);
                if (strncmp(binAux, bin, 6) == 0) {
                    consultaInformacion.optionsTransaccion.compra = variableBits[0] - 0x30;
                    consultaInformacion.optionsTransaccion.psp = variableBits[1] - 0x30;
                    consultaInformacion.optionsTransaccion.pcr = variableBits[2] - 0x30;
                    consultaInformacion.optionsTransaccion.consultaRecarga = variableBits[3] - 0x30;
                    consultaInformacion.optionsTransaccion.recarga = variableBits[4] - 0x30;
                    consultaInformacion.optionsTransaccion.efectivo = variableBits[5] - 0x30;
                    consultaInformacion.optionsTransaccion.saldo = variableBits[6] - 0x30;
                    consultaInformacion.optionsTransaccion.transferencia = variableBits[7] - 0x30;
                }

                memset(variableBits, 0x00, sizeof(variableBits));
                memcpy(variableBits, dataConsulta.bitmapTipoTx + 1, 1);
                calcularBitsEncendidos(variableBits);

                if (strncmp(binAux, "000000", 6) != 0) {
                    consultaInformacion.optionsTransaccion.retiroSinTarjeta =
                            variableBits[0] - 0x30;
                    consultaInformacion.optionsTransaccion.deposito = variableBits[1] - 0x30;
                    consultaInformacion.optionsTransaccion.transferenciaSimple =
                            variableBits[2] - 0x30;
                    consultaInformacion.optionsTransaccion.pagoProducto = variableBits[3] - 0x30;
                    consultaInformacion.optionsTransaccion.unused1 = variableBits[4] - 0x30;
                    consultaInformacion.optionsTransaccion.unused2 = variableBits[5] - 0x30;
                    consultaInformacion.optionsTransaccion.unused3 = variableBits[6] - 0x30;
                    consultaInformacion.optionsTransaccion.unused4 = variableBits[7] - 0x30;

                }

                memset(variableBits, 0x00, sizeof(variableBits));
                memcpy(variableBits, dataConsulta.bitmapTipoProducto, 1);

                calcularBitsEncendidos(variableBits);
                consultaInformacion.optionsProducto.modoComercio = variableBits[0] - 0x30;
                consultaInformacion.optionsProducto.modoPuntoPago = variableBits[1] - 0x30;
                consultaInformacion.optionsProducto.modoCNBRBM = variableBits[2] - 0x30;
                consultaInformacion.optionsProducto.modoCNBBancolombia = variableBits[3] - 0x30;
                if (strncmp(binAux, "000000", 6) != 0) {
                    consultaInformacion.optionsProducto.modoCNBAval = variableBits[4] - 0x30;
                }
                consultaInformacion.optionsProducto.modoUnused1 = variableBits[5] - 0x30;
                consultaInformacion.optionsProducto.modoUnused2 = variableBits[6] - 0x30;
                consultaInformacion.optionsProducto.modoUnused3 = variableBits[7] - 0x30;

                memset(variableBits, 0x00, sizeof(variableBits));
                memcpy(variableBits, dataConsulta.tipoMensaje, 1);

                consultaInformacion.tipoMensaje = variableBits[0];

                break;
            }

            posicion += 13;

        } while (resultado == 0);

    }

    return consultaInformacion;
}

DatosConsulta obtenerDatosConsulta(char *data) {

    DatosConsulta dataConsulta;

    memset(&dataConsulta, 0x00, sizeof(dataConsulta));
    memcpy(dataConsulta.bin, data + 4, 5);
    memcpy(dataConsulta.tipoMensaje, data + 9, 1);
    memcpy(dataConsulta.bitmapTipoTx, data + 10, 2);
    memcpy(dataConsulta.bitmapTipoProducto, data + 12, 1);

    return dataConsulta;
}

ConfiguracionExcepciones excepcionesEmv(long bin) {

    int posicion = 0;
    int respuesta = 0;
    int lengthDataExcepciones = 15;
    int lengthFile = 0;
    int index = 0;
    int idx = 0;
    uChar dataEmv[lengthDataExcepciones + 1];
    uChar rangoBajo[5 + 1];
    uChar rangoAlto[5 + 1];
    uChar bitSet[8 + 1];
    ConfiguracionExcepciones configuracionExcepciones;

    memset(&configuracionExcepciones, 0x00, sizeof(configuracionExcepciones));

    lengthFile = tamArchivo(discoNetcom, (char *)TABLA_EXCEPCIONES);

    if (lengthFile >= lengthDataExcepciones) {

        uChar dataExcepciones[lengthFile + 1];
        memset(dataExcepciones, 0x00, sizeof(dataExcepciones));
        leerArchivo(discoNetcom, (char *)TABLA_EXCEPCIONES, dataExcepciones);

        index = lengthFile / lengthDataExcepciones;

        for (idx = 0; idx < index; idx++) {

            memset(dataEmv, 0x00, sizeof(dataEmv));
            memset(rangoBajo, 0x00, sizeof(rangoBajo));
            memset(rangoAlto, 0x00, sizeof(rangoAlto));
            /** Copiar Data Excepciones **/
            memcpy(dataEmv, dataExcepciones + posicion, lengthDataExcepciones);

            ///// RANGOS DE TARJETAS /////
            memcpy(rangoBajo, dataEmv + 3, 5);
            memcpy(rangoAlto, dataEmv + 8, 5);

            /////// VERIFICAR SI SE ENCUENTRA EN EL RANGO DE LAS EXCEPCIONES //////
            respuesta = buscarBin(rangoBajo, rangoAlto, bin);
            if (respuesta == 1) {
                //// CONFIGURACION 1 DE LAS EXCEPCIONES DE EMV ////////
                memset(bitSet, 0x00, sizeof(bitSet));
                bitSet[0] = dataEmv[13];
                calcularBitsEncendidos(bitSet);
                configuracionExcepciones.comportamientoExcepciones1.obligaBanda = bitSet[7] - '0';
                configuracionExcepciones.comportamientoExcepciones1.seleccionCuentaDefault =
                        bitSet[6] - '0';
                configuracionExcepciones.comportamientoExcepciones1.gasoPass = bitSet[5] - '0';
                configuracionExcepciones.comportamientoExcepciones1.seleccionCuentaMultiservicios =
                        bitSet[4] - '0';
                configuracionExcepciones.comportamientoExcepciones1.bit4 =
                        bitSet[3] - '0'; //CREDITO ROTATIVO
                configuracionExcepciones.comportamientoExcepciones1.bit5 =
                        bitSet[2] - '0'; //FALABELLA
                configuracionExcepciones.comportamientoExcepciones1.bit6 =
                        bitSet[1] - '0'; //LEALTAD
                configuracionExcepciones.comportamientoExcepciones1.bit7 = bitSet[0] - '0'; //FOCO
                //// CONFIGURACION 2 DE LAS EXCEPCIONES DE EMV ////////
                memset(bitSet, 0x00, sizeof(bitSet));
                bitSet[0] = dataEmv[14];
                calcularBitsEncendidos(bitSet);
                configuracionExcepciones.comportamientoExcepciones2.bit0 = bitSet[7] - '0';
                configuracionExcepciones.comportamientoExcepciones2.bit1 = bitSet[6] - '0';
                configuracionExcepciones.comportamientoExcepciones2.bit2 = bitSet[5] - '0';
                configuracionExcepciones.comportamientoExcepciones2.bit3 = bitSet[4] - '0';
                configuracionExcepciones.comportamientoExcepciones2.bit4 = bitSet[3] - '0';
                configuracionExcepciones.comportamientoExcepciones2.bit5 = bitSet[2] - '0';
                configuracionExcepciones.comportamientoExcepciones2.bit6 = bitSet[1] - '0';
                configuracionExcepciones.comportamientoExcepciones2.bit7 = bitSet[0] - '0';
                break;
            }
            posicion += lengthDataExcepciones;
        }
    }

    return configuracionExcepciones;
}

TablaSieteInicializacion invocarMenuEntidades(void) {

    uChar dataEntidades[LEN_BLOQUE_ENTIDADES + 1];
    TablaSieteInicializacion informacionEntidades;
    uChar bitSet[8 + 1];
    char posicionAuxiliar[4 + 1];
    char items[1024 + 1] = {0x00};
    int existeArchivo = 0;
    int lengthTabla = 0;
    int itemsEntidades = 0;
    int indice = 0;
    int posicion = 0;
    int opcion = 0;
    int index = 0;
    int indexAux = 0;
    int codigoIdentidad = 0;
    int dineroElectronico = 0;
    int activoDineroElectronico = 0;

    memset(bitSet, 0x00, sizeof(bitSet));

    memset(&informacionEntidades, 0x00, sizeof(informacionEntidades));
    memset(dataEntidades, 0x00, sizeof(dataEntidades));
    memset(posicionAuxiliar, 0x00, sizeof(posicionAuxiliar));

    existeArchivo = verificarArchivo(discoNetcom, TABLA_7_INICIALIZACION);
    if (existeArchivo >= 0) {

        lengthTabla = tamArchivo(discoNetcom, (char *)TABLA_7_INICIALIZACION);

        ///// NUMERO DE ITEMS DEL MENU ///////
        itemsEntidades = lengthTabla / LEN_BLOQUE_ENTIDADES;
        ITEM_MENU menuEntidades[itemsEntidades + 1];
        memset(&menuEntidades, 0x00, sizeof(menuEntidades));
        strcpy(items, ";");
        for (indice = 0; indice < itemsEntidades; indice++) {

            memset(dataEntidades, 0x00, sizeof(dataEntidades));

            buscarArchivo(discoNetcom, (char *)TABLA_7_INICIALIZACION, dataEntidades, posicion,
                          LEN_BLOQUE_ENTIDADES);
            codigoIdentidad = dataEntidades[4];
            dineroElectronico = 0;
            memcpy(bitSet, dataEntidades + 23, 1);

            calcularBitsEncendidos(bitSet);

            if (((bitSet[3] - '0') == TRUE) || ((bitSet[2] - '0') == TRUE)) {
                dineroElectronico = TRUE;
                activoDineroElectronico = TRUE;
            }

            if (codigoIdentidad != 0 && dineroElectronico == TRUE) {
                memset(menuEntidades[indexAux].textoPantalla, 0x00,
                       sizeof(menuEntidades[indexAux].textoPantalla));
                memcpy(menuEntidades[indexAux].textoPantalla, dataEntidades + 8, 9);
                strcat(items, menuEntidades[indexAux].textoPantalla);
                strcat(items, ";");
                sprintf(menuEntidades[indexAux].valor, "%d", posicion);
                strcat(items, menuEntidades[indexAux].valor);
                strcat(items, ";");
                indexAux++;
            } else if (codigoIdentidad == 0) {
                indice = itemsEntidades;
            }
            posicion += LEN_BLOQUE_ENTIDADES;
        }

        if (activoDineroElectronico == TRUE) {
            showMenu((char *)"ENTIDADES BANCARIAS;", itemsEntidades, items, 1024);

            if (opcion == -3) {
                informacionEntidades.tipoTransaccion = -3;
            }

        } else {

            informacionEntidades.tipoTransaccion = -11;
        }
    } else {
        informacionEntidades.tipoTransaccion = -11;
    }

    return informacionEntidades;
}

void habilitarMultiplesCorresponsales(void) {

    int idx = 0;
    int iteraciones = 0;
    int i = 0;
    int multiplesCorresponsales = 0;
    int CNB_B = 0;
    int CNB_T_C = 0;
    int CNB_T_T = 0;
    int tamSembrada = 0;
    long tamDatosMulticorresponsal = 0;
    char datosMulticorresponsal[256 + 1] = {0x00};
    uChar fiid[TAM_FIID + 1];
    uChar codigoUnico[TAM_CODIGO_UNICO + 1];
    uChar panVirtual[TAM_TRACK2 + 1];
    uChar panVirtualAuxiliar[TAM_TRACK2 + 1];
    uChar tipoCuenta[TAM_TIPO_CUENTA_MULTICORRESPONSAL + 1];
    uChar tipoCuentaLimpio[TAM_TIPO_CUENTA + 1] = {0x00};
    uChar tablaPar[10 + 1] = {0x00};
    uChar habilitacionCorresponsal[TAM_FIID + 1];
    uChar tarjetaSembrada[69 + 1];

    if (leerArchivo(discoNetcom, (char *)TABLA_PARAMETROS, tablaPar) > 0) {
        if (!(tablaPar[6] & 0x20) && !(tablaPar[5] & 0x10) && !(tablaPar[6] & 0x01) &&
            !tablaPar[7] & 0x80) {
            borrarArchivo(discoNetcom, (char *)RUTA_TABLA_MULTICORRESPONSAL);
            setParameter(CNB_BANCO, (char *)"0000");
        }
    }

    if (verificarArchivo(discoNetcom, RUTA_TABLA_MULTICORRESPONSAL) == FS_OK) {
        tamDatosMulticorresponsal = leerArchivo(discoNetcom, (char *)RUTA_TABLA_MULTICORRESPONSAL,
                                                datosMulticorresponsal);
    }

    if (tamDatosMulticorresponsal > 0) {
        iteraciones = tamDatosMulticorresponsal / TAM_DATOS_MULTICORRESPONSAL;
    }

    for (i = 0; i < iteraciones; ++i) {
        if (strncmp(datosMulticorresponsal + (46 * i + 3), "0007", 4) == 0
            || strncmp(datosMulticorresponsal + (46 * i + 3), "0903", 4) == 0) {
            multiplesCorresponsales++;
        }
    }

    if (multiplesCorresponsales > 1) {

        // se limpian datos de CNB CLASICO;
        setParameter(CNB_BANCO, (char *)"0000");
        setParameter(CNB_TIPO_CUENTA, tipoCuentaLimpio);
        borrarArchivo(discoNetcom, (char *)TARJETA_CNB);

        for (i = 0; i < iteraciones; i++) {
            idx += 3;

            if (strncmp(datosMulticorresponsal + idx, "0007", 4) == 0
                || strncmp(datosMulticorresponsal + idx, "0903", 4) == 0) {
                memset(fiid, 0x00, sizeof(fiid));
                memset(codigoUnico, 0x00, sizeof(codigoUnico));
                memset(panVirtual, 0x00, sizeof(panVirtual));
                memset(tipoCuenta, 0x00, sizeof(tipoCuenta));

                memcpy(fiid, datosMulticorresponsal + idx, TAM_FIID);
                idx += TAM_FIID;
                memcpy(codigoUnico, datosMulticorresponsal + idx, TAM_CODIGO_UNICO);
                idx += TAM_CODIGO_UNICO;
                memcpy(panVirtual, datosMulticorresponsal + idx, TAM_PAN_VIRTUAL);
                idx += TAM_PAN_VIRTUAL;
                memcpy(tipoCuenta, datosMulticorresponsal + idx,
                       TAM_TIPO_CUENTA_MULTICORRESPONSAL - 1);
                strcat(tipoCuenta, "0");
                idx += TAM_TIPO_CUENTA_MULTICORRESPONSAL - 1;

                memset(tarjetaSembrada, 0x00, sizeof(tarjetaSembrada));

                if (strncmp(fiid, "0007", 4) == 0) {
                    CNB_B = CNB_B_BANCOLOMBIA;
                    CNB_T_C = CNB_T_C_BANCOLOMBIA;
                    CNB_T_T = CNB_T_T_BANCOLOMBIA;
                    sprintf(tarjetaSembrada, "%s", TARJ_SEMB_BANCOLOMBIA);
                } else if (strncmp(fiid, "0903", 4) == 0) {
                    CNB_B = CNB_B_AVAL;
                    CNB_T_C = CNB_T_C_AVAL;
                    CNB_T_T = CNB_T_T_AVAL;
                    sprintf(tarjetaSembrada, "%s", TARJ_SEMB_AVAL);
                } else if (strncmp(fiid, "0013", 4) == 0) {
                    CNB_B = CNB_B_CORRESPONSAL;
                    CNB_T_C = CNB_T_C_CORRESPONSAL;
                    CNB_T_T = CNB_T_T_CORRESPONSAL;
                    sprintf(tarjetaSembrada, "%s", TARJ_SEMB_CORRESPONSALES);
                }

                if (panVirtual[0] != 0x20 && (tipoCuenta[0] == 0x31 || tipoCuenta[0] == 0x32)) {

                    borrarArchivo(discoNetcom, tarjetaSembrada);
                    setParameter(CNB_B, fiid);
                    setParameter(CNB_T_C, tipoCuenta);

                    memset(tipoCuenta, 0x00, sizeof(tipoCuenta));
                    sprintf(tipoCuenta, "%02d", CNB_TIPO_TARJETA_VIRTUAL);
                    setParameter(CNB_T_T, tipoCuenta);

                    memset(panVirtualAuxiliar, 0x00, sizeof(panVirtualAuxiliar));
                    _rightTrim_(panVirtualAuxiliar, panVirtual, 0x20);
                    //!!memcpy( numeroTarjeta + strlen(numeroTarjeta), "D101210100000",  13 );
                    memcpy(panVirtualAuxiliar + strlen(panVirtualAuxiliar), "D101210100000", 13);
                    escribirArchivo(discoNetcom, tarjetaSembrada, panVirtualAuxiliar,
                                    strlen(panVirtualAuxiliar));
                } else {

                    memset(tipoCuenta, 0x00, sizeof(tipoCuenta));

                    memset(habilitacionCorresponsal, 0x00, sizeof(habilitacionCorresponsal));
                    getParameter(CNB_B, habilitacionCorresponsal);

                    if (strcmp(habilitacionCorresponsal, "0000") == 0) {
                        setParameter(CNB_B, (char *)"0000");
                        setParameter(CNB_T_C, tipoCuenta);
                        setParameter(CNB_PEDIR_PIN, (char *)"00");
                        borrarArchivo(discoNetcom, tarjetaSembrada);
                        setParameter(USER_CNB_LOGON, (char *)"0");
                    } else {

                        tamSembrada = tamArchivo(discoNetcom, tarjetaSembrada);

                        if (tamSembrada > 0) {
                            leerArchivo(discoNetcom, tarjetaSembrada, panVirtualAuxiliar);
                        }

                        if (strncmp(panVirtualAuxiliar + strlen(panVirtualAuxiliar) -
                                    strlen("D101210100000"),
                                    "D101210100000", strlen("D101210100000")) == 0) {
                            setParameter(CNB_B, (char *)"0000");
                            setParameter(CNB_T_C, tipoCuenta);
                            setParameter(CNB_PEDIR_PIN, (char *)"00");
                            borrarArchivo(discoNetcom, tarjetaSembrada);
                            setParameter(USER_CNB_LOGON, (char *)"0");
                        }
                    }
                }

            } else {
                idx += TAM_FIID;
                idx += TAM_CODIGO_UNICO;
                idx += TAM_PAN_VIRTUAL;
                idx += TAM_TIPO_CUENTA_MULTICORRESPONSAL - 1;
            }
        }
    } else {

        setParameter(CNB_B_BANCOLOMBIA, (char *)"0000");
        setParameter(CNB_B_AVAL, (char *)"0000");
    }
}

void inicializacionConsultaInformacion(void) {

    int resultado = 0;

    uChar terminal[8 + 1] = {0x00};
    uChar tablaPar[10 + 1] = {0x00};
    uChar consecutivo[6 + 1] = {0x00};
    uChar dataRecibidaAux[512] = {0x00};

    char codigoProceso[6 + 1] = {0x00};

    ResultISOPack resultadoIsoPackMessage;
    ResultISOUnpack resultadoUnpack;
    ISOHeader isoHeader8583;
    ISOFieldMessage isoFieldMessage;

    memset(&isoHeader8583, 0x00, sizeof(isoHeader8583));
    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(&resultadoUnpack, 0x00, sizeof(resultadoUnpack));
    memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));

    resultado = leerArchivo(discoNetcom, (char *)TABLA_PARAMETROS, tablaPar);
    if (resultado > 0 && tablaPar[5] & 0x20) {
        getParameter(NUMERO_TERMINAL, terminal);
        strcpy(codigoProceso, "947000");

        do {
            isoHeader8583.TPDU = 60;
            strcpy(isoHeader8583.destination, "0001");
            strcpy(isoHeader8583.source, "0001");
            generarSystemTraceNumber(consecutivo, (char *)"");
            setHeader(isoHeader8583);
            setMTI((char *)"0800");
            setField(3, codigoProceso, 6);
            setField(11, consecutivo, 6);
            setField(24, (char *)"001", 3);
            setField(41, terminal, 8);
            setField(60, (char *) "EFT30_NET01", 11);
            resultadoIsoPackMessage = packISOMessage();

            ////////// ENVIANDO TRANSACCION //////////

            //printMessage(strlen(INICIALIZANDO_INFORMACION), INICIALIZANDO_INFORMACION);
            setParameter(VERIFICAR_PAPEL_TERMINAL, (char *) "00");
            resultado = realizarTransaccion(resultadoIsoPackMessage.isoPackMessage,
                                            resultadoIsoPackMessage.totalBytes,
                                            dataRecibidaAux, TRANSACCION_INICIALIZACION,
                                            CANAL_PERMANENTE, REVERSO_NO_GENERADO);

            ////////// RECIBIENDO TRANSACCION //////////
            memset(codigoProceso, 0x00, sizeof(codigoProceso));
            if (resultado > 0) {
                resultadoUnpack = unpackISOMessage(dataRecibidaAux, resultado);
                isoFieldMessage = getField(60);
                borrarArchivo(discoNetcom, (char *) TABLA_CI);
                escribirArchivo(discoNetcom, (char *) TABLA_CI, isoFieldMessage.valueField,
                                isoFieldMessage.totalBytes);
                memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
                isoFieldMessage = getField(3);
                strcpy(codigoProceso, isoFieldMessage.valueField);

            }

        } while (strcmp(codigoProceso, "947001") == 0);
    }

}

int codigoTerminalCnb(char *numeroTerminal) {

    int respuesta = 0;
    int longitud = 0;
    int iRetCierreCnb = 0;
    int iRetCierreComercio = 0;
    int modoCNB = 0;
    uChar texto[15 + 1];
    uChar cnb[2 + 1];
    uChar data[1 + 1];

    memset(cnb, 0x00, sizeof(cnb));
    memset(texto, 0x00, sizeof(texto));
    memset(data, 0x00, sizeof(data));

    getParameter(CNB_INICIALIZADO, data);


    modoCNB = getModoCNB(texto);

    setParameter(CNB_INICIA, cnb);

    if (strlen(numeroTerminal) != 0) {

        setParameter(CNB_CAMBIO_TERMINAL, (char *)"1");
        iRetCierreCnb = _cierreCnb_();
        setParameter(CNB_CAMBIO_TERMINAL, (char *)"0");

        if (iRetCierreCnb > 0) {
            respuesta = setParameter(CNB_TERMINAL, numeroTerminal);
            if (respuesta < 0) {
                respuesta = -1;
            } else {
                memset(texto, 0x00, sizeof(texto));
                //se inicializa solo el cnb por solicitud de Rbm.
                memset(texto, 0x00, sizeof(texto));
                getModoCNB(texto);
                respuesta = _inicializacionCNB_((char *)INICIALIZACION_MANUAL, texto, 0);
            }
        }
    } else {
        //cierre comercio
        iRetCierreComercio = _cierreVenta_();

        //cierre CNB
        setParameter(CNB_CAMBIO_TERMINAL, (char *)"1");
        iRetCierreCnb = _cierreCnb_();

        if (iRetCierreCnb > 0 && iRetCierreComercio > 0) {
            setParameter(CNB_CAMBIO_TERMINAL, (char *)"0");

            memset(numeroTerminal, 0x00, sizeof(numeroTerminal));
            setParameter(CNB_TERMINAL, numeroTerminal);

            setParameter(CNB_INICIA, (char *)"00");
            respuesta = 0;
            if (verificarArchivo(discoNetcom, (char *)JOURNAL) != FS_OK
                && verificarArchivo(discoNetcom, (char *)JOURNAL_CNB_RBM) != FS_OK
                && verificarArchivo(discoNetcom, (char *)JOURNAL_CNB_BANCOLOMBIA) != FS_OK
                && verificarArchivo(discoNetcom, (char *)JOURNAL_CNB_CITIBANK) != FS_OK
                && verificarArchivo(discoNetcom, (char *)JOURNAL_MULT_BCL) != FS_OK
                && verificarArchivo(discoNetcom, (char *)JOURNAL_MCNB_AVAL) != FS_OK
                && verificarArchivo(discoNetcom, (char *)JOURNAL_CNB_CORRESPONSALES) != FS_OK) {

                borrarArchivo(discoNetcom, (char *)DIRECTORIO_JOURNALS);
            }
        }
    }

    return respuesta;
}

int _inicializacionCNB_(char * codigoProcesoInicial, char *texto, int tipoAutomatico) {

    int resultado = 0;
    char nombreArchivo[20 + 1];
    uChar tablaCero = 0x00;
    uChar tablaUno = 0x01;
    uChar dataRecibidaAux[512];
    uChar consecutivo[6 + 1];
    uChar numeroTabla;
    uChar numeroTabla_;
    ResultISOPack resultadoIsoPackMessage;
    ResultISOUnpack resultadoUnpack;
    ISOHeader isoHeader8583;
    ISOFieldMessage isoFieldMessage;
    char terminal[8 + 1];
    char message[30 + 1];
    int cierraCanal = CANAL_DEMANDA;

    memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
    memset(&isoHeader8583, 0x00, sizeof(isoHeader8583));
    memset(dataRecibidaAux, 0x00, sizeof(dataRecibidaAux));
    memset(&resultadoUnpack, 0x00, sizeof(resultadoUnpack));
    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(consecutivo, 0x00, sizeof(consecutivo));
    memset(nombreArchivo, 0x00, sizeof(nombreArchivo));
    memset(terminal, 0x00, sizeof(terminal));
    memset(message, 0x00, sizeof(message));

    borrarArchivo(discoNetcom, (char *)TABLA_CNB);

    getParameter(CNB_TERMINAL, terminal);
    isCnb = 1;
    uChar nii[TAM_NII + 1];
    memset(nii, 0x00, sizeof(nii));

    getParameter(NII, nii);

    isoHeader8583.TPDU = 60;
    memcpy(isoHeader8583.destination, nii, TAM_NII + 1);
    memcpy(isoHeader8583.source, nii, TAM_NII + 1);
    setHeader(isoHeader8583);

    generarSystemTraceNumber(consecutivo, (char *) "");

    setMTI((char *)"0800");
    setField(3, codigoProcesoInicial, 6);
    setField(11, consecutivo, 6);
    setField(24, nii + 1, 3);
    setField(41, terminal, 8);
    setField(60, (char *)"EFT30_NET01", 11);
    resultadoIsoPackMessage = packISOMessage();

    if (tipoAutomatico == 1) {
        cierraCanal = CANAL_PERMANENTE;
    }

    resultado = realizarTransaccion(resultadoIsoPackMessage.isoPackMessage, resultadoIsoPackMessage.totalBytes,
                                    dataRecibidaAux, TRANSACCION_INICIALIZACION, cierraCanal, REVERSO_NO_GENERADO);

    if (resultado > 0) {

        resultadoUnpack = unpackISOMessage(dataRecibidaAux, resultado);

        memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
        isoFieldMessage = getField(60);
        numeroTabla_ = isoFieldMessage.valueField[0];
        numeroTabla = isoFieldMessage.valueField[3]; //01

        /// TABLA 1 ///
        if (numeroTabla == tablaUno && numeroTabla_ == tablaCero) {
            memset(nombreArchivo, 0x00, sizeof(nombreArchivo));
            sprintf(nombreArchivo,"%s" ,TABLA_CNB);
            escribirArchivo(discoNetcom, nombreArchivo, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

            setParameter(CNB_INICIALIZADO, (char *)"1");
            if (strcmp(texto, "CNB BANCOLOMBIA") == 0 || strcmp(texto, "CB BBVA") == 0) {

                inicializarMulticorresponsal();
                _inicializacionSerial_();
                if (verificarArchivo(discoNetcom, RUTA_TABLA_MULTICORRESPONSAL) != FS_OK) {
                    resultado = -2;
                    tipoAutomatico = 1;
                    setParameter(CNB_TERMINAL, (char *)"");
                    memset(terminal, 0x00, sizeof(terminal));
                    getParameter(CNB_INICIALIZADO, terminal);
                    setParameter(CNB_INICIALIZADO, terminal);
                }
            } else {
                _inicializacionSerial_();
            }
/*            if (tipoAutomatico != 1) {
                buzzer(10);
                sprintf(message, MESSAGE_CNB, texto);
                printMessage(strlen(message), message);
                printMessage(strlen(REINICIAR_TERMINAL), REINICIAR_TERMINAL);
                Telium_Exit(19);

            }*/
        } else {
            resultado = -3;
            memset(terminal, 0x00, sizeof(terminal));
            getParameter(CNB_INICIALIZADO, terminal);
            setParameter(CNB_INICIALIZADO, terminal);
        }

    } else {
        resultado = -3;
        memset(terminal, 0x00, sizeof(terminal));
        getParameter(CNB_INICIALIZADO, terminal);
        setParameter(CNB_INICIALIZADO, terminal);
    }
    return resultado;
}

int  inicializacionParametros(void) {
    LOGI("Netcom inicializando Parametros " );
    int resultado = 0;

    uChar dataRecibidaAux[512] = {0x00};
    uChar consecutivo[6 + 1] = {0x00};
    uChar terminal[8 + 1] = {0x00};
    ResultISOPack resultadoIsoPackMessage;
    ResultISOUnpack resultadoUnpack;
    ISOHeader isoHeader8583;
    ISOFieldMessage isoFieldMessage;
    memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
    memset(&isoHeader8583, 0x00, sizeof(isoHeader8583));
    memset(&resultadoUnpack, 0x00, sizeof(resultadoUnpack));
    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));

    setParameter(TIPO_TRANSACCION,"01");

    if (estadoTerminalConfigurado() == 1) {
        getParameter(NUMERO_TERMINAL, terminal);
        LOGI("Terminal configurado %s " ,terminal);

        uChar nii[TAM_NII + 1];
        memset(nii, 0x00, sizeof(nii));

        getParameter(NII, nii);
        LOGI("NII  configurado %s " ,nii);
        isoHeader8583.TPDU = 60;
        memcpy(isoHeader8583.destination, nii, TAM_NII + 1);
        memcpy(isoHeader8583.source, nii, TAM_NII + 1);
        setHeader(isoHeader8583);

        generarSystemTraceNumber(consecutivo,"");

        setMTI("0800");
        setField(3, "942000", 6);
        setField(11, consecutivo, 6);
        setField(24, nii + 1, 3);
        setField(41, terminal, 8);
        setField(60, "EFT30_NET01", 11);
        resultadoIsoPackMessage = packISOMessage();

        ////////// ENVIANDO TRANSACCION //////////
        setParameter(VERIFICAR_PAPEL_TERMINAL, (char *)"00");
        resultado = realizarTransaccion(resultadoIsoPackMessage.isoPackMessage, resultadoIsoPackMessage.totalBytes,
                                        dataRecibidaAux, TRANSACCION_INICIALIZACION, CANAL_PERMANENTE, REVERSO_NO_GENERADO);

        ////////// RECIBIENDO TRANSACCION //////////
        if (resultado > 0) {
            resultadoUnpack = unpackISOMessage(dataRecibidaAux, resultado);
            isoFieldMessage = getField(60);
            borrarArchivo(discoNetcom, (char *)TABLA_PARAMETROS);
            escribirArchivo(discoNetcom,(char *) TABLA_PARAMETROS, isoFieldMessage.valueField, isoFieldMessage.totalBytes);
            desempaquetarTablaParametros();

            habilitarMultiplesCorresponsales();
            LOGI("Habilitar multiples corresponsales " );
        }

        //inicializa consulta de la informacion
        inicializacionConsultaInformacion();
        LOGI("Habilitar consulta de la informacion " );
        _inicializacionSerial_();
        LOGI("_inicializacionSerial_" );
        ///////// INICIALIZACION QRCODE  ////////
        uChar texto[15 + 1];
        uChar modoQr[3 + 1];

        memset(texto, 0x00, sizeof(texto));
        memset(modoQr, 0x00, sizeof(modoQr));

        getParameter(MODO_QR, modoQr);
        //resultado = atol(modoQr);

        //if (resultado == 1) {
            //inicializacionQR(INICIALIZACION_MANUAL, texto, 1);
            LOGI("qrcode pendiente" );
        //}
        setParameter(TIPO_TRANSACCION,(char *)"02");
        releaseSocket();
        LOGI("resultado iniparametros %d" ,resultado);
    }
    return  resultado;
}
