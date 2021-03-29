//
// Created by NETCOM on 09/03/2020.
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
#include "android/log.h"

#define  LOG_TAG    "NETCOM_QR_CODE"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

#define  PROCESINGCODE                "008800"
#define  PROCESINGCODE_RBM            "320000"
#define  CONSULTA_EMVCO                1
#define  CONSULTA_F1                2

int armarTramaSolicitudPaypass(DatosBasicosVenta datosBasicosVenta) {

    int resultado = 0;
    uChar nii[TAM_NII + 1];
    uChar codigoComercio[TAM_ID_COMERCIO + 1];

    //usoCaja = 0;

    memset(nii, 0x00, sizeof(nii));
    memset(codigoComercio, 0x00, sizeof(codigoComercio));
    memset(&globalTramaVenta, 0x00, sizeof(globalTramaVenta));
    LOGI("setear qrcode  ");
    setParameter(VERSION_QRCODE, (char *) "0");
    direccionamientoQRcode(globalTramaVenta.terminalId, nii, globalTramaVenta.processingCode,
                           globalTramaVenta.posEntrymode,
                           codigoComercio);
    LOGI("direccionamientoQRcode sale  ");
    strcpy(globalTramaVenta.totalVenta, datosBasicosVenta.totalVenta);
    strcpy(globalTramaVenta.iva, datosBasicosVenta.iva);
    strcpy(globalTramaVenta.inc, datosBasicosVenta.inc);
    strcpy(globalTramaVenta.baseDevolucion, datosBasicosVenta.baseDevolucion);
    strcpy(globalTramaVenta.compraNeta, datosBasicosVenta.compraNeta);
    strcpy(globalTramaVenta.propina, datosBasicosVenta.propina);
    //strcpy(datosVentaPaypass.codigoCajero, datosVentaPaypass.codigoCajero);
    //strcpy(datosVentaPaypass.facturaCaja, datosVentaPaypass.facturaCaja);

    armarCamposQrCode(&globalTramaVenta, nii, codigoComercio, 1);

    resultado = realizarSolicitudQR(0);
    releaseSocket();

    return resultado;
}

int armarCamposQrCode(DatosVenta *datosVenta, char *nii, char *codigoComercio, int inicioPaypass) {

    int resultado = 0;
    int tipoQrcode = 0;
    int tamCampo48 = 0;
    char field48[44 + 1];
    char variableAux[44 + 1];
    char tipoTransaccion[TAM_TIPO_TRANSACCION + 1];

    ISOHeader isoHeader8583;

    memset(field48, 0x00, sizeof(field48));
    memset(variableAux, 0x00, sizeof(variableAux));
    memset(&isoHeader8583, 0x00, sizeof(isoHeader8583));
    memset(tipoTransaccion, 0x00, sizeof(tipoTransaccion));

    isoHeader8583.TPDU = 60;
    memcpy(isoHeader8583.destination, nii, TAM_NII + 1);
    memcpy(isoHeader8583.source, nii, TAM_NII + 1);
    setHeader(isoHeader8583);
    generarSystemTraceNumber(datosVenta->systemTrace, (char *)"");
    LOGI("systemTrace %s ", datosVenta->systemTrace);
    sprintf(tipoTransaccion, "%d", TRANSACCION_SOLICITUD_QR);
    leftPad(datosVenta->tipoTransaccion, tipoTransaccion, 0x30, TAM_TIPO_TRANSACCION);

    tipoQrcode = verificarTipoQrcode();

    if (tipoQrcode == IS_QRCODE_EMVCO) {
        sprintf(variableAux, "%s", "02");
        LOGI("variableAux %s ", variableAux);
        sprintf(variableAux + 2, "%010ld00", atol(datosVenta->iva));
        LOGI("variableAux %s ", variableAux);
        sprintf(variableAux + 14, "%010ld00", atol(datosVenta->baseDevolucion));
        LOGI("variableAux %s ", variableAux);
        strcat(variableAux, "02");
        LOGI("variableAux %s ", variableAux);
        sprintf(variableAux + 28, "%010ld00", atol(datosVenta->inc));
        LOGI("variableAux %s ", variableAux);
        memcpy(field48, variableAux, 40);
        LOGI("field48 %s ", field48);
        tamCampo48 = 40;
    } else {
        sprintf(variableAux, "%010ld00", atol(datosVenta->iva));
        sprintf(variableAux + 12, "%010ld00", atol(datosVenta->baseDevolucion));
        sprintf(variableAux + 24, "%010ld00", atol(datosVenta->inc));
        _convertASCIIToBCD_(field48, variableAux, 36);
        tamCampo48 = 18;
    }

    strcat(datosVenta->totalVenta, "00");

    memcpy(datosVenta->msgType, "0100", TAM_MTI);
    memcpy(datosVenta->nii, nii + 1, TAM_NII);
    memcpy(datosVenta->posConditionCode, "00", TAM_POS_CONDITION);
    memcpy(datosVenta->codigoComercio, codigoComercio + 1, TAM_ID_COMERCIO);

    setMTI(datosVenta->msgType);

    setField(3, datosVenta->processingCode, TAM_CODIGO_PROCESO);
    setField(4, datosVenta->totalVenta, TAM_COMPRA_NETA);
    setField(11, datosVenta->systemTrace, TAM_SYSTEM_TRACE);
    setField(22, datosVenta->posEntrymode, TAM_ENTRY_MODE);
    setField(24, datosVenta->nii, TAM_NII);
    setField(25, datosVenta->posConditionCode, TAM_POS_CONDITION);
    setField(41, datosVenta->terminalId, TAM_TERMINAL_ID);

    if (tipoQrcode == IS_QRCODE_RBM) {
        memset(variableAux, 0x00, sizeof(variableAux));
        leftPad(variableAux, codigoComercio, 0x30, TAM_FIELD_42);
        setField(42, variableAux, TAM_FIELD_42);
    }
    setField(48, field48, tamCampo48);

    if (tipoQrcode == IS_QRCODE_RBM) {
        memset(variableAux, 0x00, sizeof(variableAux));
        leftPad(variableAux, datosVenta->propina, 0x30, 10);
        strcat(variableAux, "00");
        setField(57, variableAux, TAM_COMPRA_NETA);
    } else {
        memset(variableAux, 0x00, sizeof(variableAux));
        strcpy(variableAux, "02");
        leftPad(variableAux + 2, datosVenta->propina, 0x30, 10);
        strcat(variableAux, "00");
        setField(54, variableAux, 14);
        setField(60, (char *)"12", 2);
        memset(variableAux, 0x00, sizeof(variableAux));
        leftPad(variableAux, codigoComercio, 0x30, TAM_FIELD_42);
        setField(61, variableAux, TAM_FIELD_42);
    }
    LOGI("resultado %d ", resultado);
    return resultado;
}

int realizarSolicitudQR(int anulacion) {

    int iRet = 0;
    int tipoQr = 0;
    uChar datosRecibidos[1324 + 1] = {0x00};
    DatosTerminalQr datoTerminal;
    DatosEmpaquetados datosConsultaQr;
    DatosIniQrCode datoIntentos;
    DatosVenta datosCaja;
    ResultISOPack resultISOPack;

    memset(&datoTerminal, 0x00, sizeof(datoTerminal));
    memset(&datosConsultaQr, 0x00, sizeof(datosConsultaQr));
    memset(&datoIntentos, 0x00, sizeof(datoIntentos));
    memset(&datosCaja, 0x00, sizeof(datosCaja));
    memset(&resultISOPack, 0x00, sizeof(resultISOPack));

    resultISOPack = packISOMessage();
    //screenMessage("COMUNICACION", "CONSULTANDO", "ESTADO", "TRANSACION QR", 1 * 1000);
    iRet = realizarTransaccion(resultISOPack.isoPackMessage, resultISOPack.totalBytes,
                               datosRecibidos,
                               TRANSACCION_SOLICITUD_QR, CANAL_PERMANENTE, REVERSO_NO_GENERADO);

    if (iRet > 0) {
        if (anulacion == 1) {
            iRet = desempaquetarSolicitud(datosRecibidos, TRANSACCION_ANULACION, iRet);
        } else {
            iRet = desempaquetarSolicitud(datosRecibidos, TRANSACCION_SOLICITUD_QR, iRet);
        }

    }
    /* if (verificarHabilitacionCaja() == TRUE && usoCaja == 0) {
         memcpy(&datosCaja, &datosVenta, sizeof(datosVenta));

         tipoQr = verificarTipoQrcode();
         if (tipoQr == IS_QRCODE_EMVCO) {
             getTerminalId(datosCaja.terminalId);
         } else {
             datoTerminal = capturarDatoTerminal();
             datoIntentos = capturarDatosQrCodeRbm();
             memcpy(datosCaja.terminalId, datoIntentos.terminal, sizeof(datosCaja.terminalId));
            // procesarArchivoConfiguraciones(datosCaja.posEntrymode, 9);
         }

         memset(datosCaja.cardName, 0x00, sizeof(datosCaja.cardName));
         memset(datosCaja.codigoAprobacion, 0x00, sizeof(datosCaja.codigoAprobacion));

         strcpy(datosCaja.codigoComercio, datoTerminal.codigoComercio);
         strcpy(datosCaja.track2, datosConsultaQr.bin);
         strcat(datosCaja.track2, "00");
         strcat(datosCaja.track2, datosConsultaQr.ultimosCuatro);
         if (atoi(datosCaja.iva) > 0) {

             strcat(datosCaja.iva, "00");
             strcat(datosCaja.baseDevolucion, "00");
         }
         if (strcmp(datosCaja.codigoAprobacion, "00") != 0) {
             memcpy(datosCaja.codigoRespuesta, "01", 2);
         }

         //transaccionRespuestaDatosCaja(datosCaja, TRANSACCION_TEFF_COMPRAS);
         usoCaja = 1;
     }*/
    return iRet;
}

int desempaquetarSolicitud(char *dataRecibida, int tipoTransaccion, int cantidadBytes) {

    int resultado = 0;
    int contador = 0;
    int nEvent;
    int tecla = 0;
    int nTimeout;
    int tempoTotal = 0;
    int numTransacciones = 0;
    int iRet = 0;
    int tecladoActivo = 0;
    int teclasDefault = 0;
    int tempoAnterior = 0;
    int tiempoVisualizacion = 60;
    int tiempoConsulta = 700;
    int primerIntento = 0;
    long lengthFile = 0;

    unsigned char range;
    unsigned char product;
    unsigned char terminal[64];
    unsigned long ulBeforeDeselectDisplayTime = 0;
    char cadena[2100 + 1];
    char codigoRespuesta[2 + 1];
    char cadCounter[2 + 1];
    char visualRB[3 + 1];

    ISOFieldMessage isoFieldMessage;
    ResultISOUnpack resultadoUnpack;
    DatosQrCodeEmvco datoConsultaQR;

    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(&resultadoUnpack, 0x00, sizeof(resultadoUnpack));
    memset(&datoConsultaQR, 0x00, sizeof(datoConsultaQR));
    memset(cadena, 0x00, sizeof(cadena));
    memset(codigoRespuesta, 0x00, sizeof(codigoRespuesta));
    memset(cadCounter, 0x00, sizeof(cadCounter));
    memset(visualRB, 0x00, sizeof(visualRB));
    datoConsultaQR = capturarDatosQrCode();

    resultadoUnpack = unpackISOMessage(dataRecibida, cantidadBytes);

    isoFieldMessage = getField(11);
    memcpy(globalTramaVenta.systemTrace, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

    isoFieldMessage = getField(12);
    memcpy(globalTramaVenta.txnTime, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

    isoFieldMessage = getField(13);
    memcpy(globalTramaVenta.txnDate, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

    isoFieldMessage = getField(38);
    memcpy(globalTramaVenta.codigoAprobacion, isoFieldMessage.valueField,
           isoFieldMessage.totalBytes);

    isoFieldMessage = getField(39);
    memcpy(codigoRespuesta, isoFieldMessage.valueField, isoFieldMessage.totalBytes);
    memcpy(globalTramaVenta.codigoRespuesta, isoFieldMessage.valueField,
           isoFieldMessage.totalBytes);

    isoFieldMessage = getField(42);
    memcpy(globalTramaVenta.codigoComercio, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

    isoFieldMessage = getField(62);
    memcpy(cadena, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

    if (strcmp(codigoRespuesta, "00") == 0) {


        if (tipoTransaccion != 3) {
            memcpy(globalTramaVenta.aux1, "Trans. Pend. Aprobar", 20);
            memcpy(globalTramaVenta.processingCode, "008900", 6);
            escribirArchivo(discoNetcom, (char *)DATOS_QRCODE, (char *) &globalTramaVenta,
                            sizeof(globalTramaVenta));
            // screenMessage("CODIGO QR", "", "CODIGO QR GENERADO", "", 0);
        }

        resultado = mostrarQR(cadena);

        /*tiempoVisualizacion = atoi(datoConsultaQR.tiempoVisualizacion) - 10;

        if (tiempoVisualizacion <= 0) {
            getParameter(TIEMPO_VISUALIZACION_QR_RBM, visualRB);
            tiempoVisualizacion = atoi(visualRB);
        }
        if (tiempoVisualizacion < 20) {
            tiempoVisualizacion = 20;
        }
        tiempoVisualizacion = tiempoVisualizacion * 3;
//		if (tipoTransaccion == 78) {
        if (iRet == 0 || iRet == T_VAL_QR) {
            releaseSocket();
            setParameter(INTENTOS_CONSULTA_QR, "0");
            ulBeforeDeselectDisplayTime = GTL_StdTimer_GetCurrent();
            do {
                tempoAnterior = (ulBeforeDeselectDisplayTime / 100);
                ulBeforeDeselectDisplayTime = GTL_StdTimer_GetCurrent();
                nTimeout = (ulBeforeDeselectDisplayTime / 100);
                nTimeout = nTimeout - tempoAnterior;
                tempoTotal = tempoTotal + nTimeout;

                if (teclasDefault == 0) {
                    if (IsColorDisplay() == 0) {
                        teclasDefault = 1;
                    }
                    iRet = mostrarQR(cadena);
                }
                tecladoActivo = 1;
                if (primerIntento == 0) {
                    tiempoConsulta = atoi(datoConsultaQR.numeroIntentos) * 200;
                    primerIntento = 1;
                } else {
                    tiempoConsulta = atoi(datoConsultaQR.tiempoConsulta) * 100;
                }
                if (tiempoConsulta <= 9) {
                    tiempoConsulta = 700;
                }
                nEvent = ttestall(KEYBOARD, tiempoConsulta);

                if (nEvent == KEYBOARD) {
                    tecla = getc(hKeyboard);
                }
                switch (nEvent) {
                    case KEYBOARD:
                        switch (tecla) {
                            case T_VAL:
                                fclose(hKeyboard);
                                tecladoActivo = 0;
                                buzzer(10);

                                iRet = consultaAutoQR(sizeof(datosVenta), (char *) &datosVenta);

                                resultado = 1;
                                contador++;
                                break;
                            case T_ANN:
                                fclose(hKeyboard);
                                tecladoActivo = 0;
                                iRet = getchAcceptCancel("QRCODE", "DESEA CANCELAR ", "LA", "TRANSACCION", 30 * 1000);

                                if (iRet > 0) {
                                    screenMessage("VENTA", "POR FAVOR CONSULTAR", "EN EL MENU F1", "(F1) CONSULTAR QR",
                                                  3 * 1000);
                                    if (tipoTransaccion == 3) {
                                        setParameter(INTENTOS_CONSULTA_QR, "10");
                                    }
                                    break;
                                } else {
                                    if (tipoImagen == IMAGEN_IMPRESORA) {
                                        screenMessage("PROCESO DE COMPRA QR", "ESPERAR CONFIRMACION", "DE TRANSACCION",
                                                      "EN CLIENTE", 1);
                                    }
                                    iRet = 2;
                                }
                                break;
                            default:
                                fclose(hKeyboard);
                                tecladoActivo = 0;
                                setParameter(INTENTOS_CONSULTA_QR, "1");
                                iRet = 2;
                                teclasDefault = 1;
                                break;
                        }

                        break;
                    default:
                        buzzer(3);
                        buzzer(7);
                        buzzer(3);
                        if (tecladoActivo == 1) {
                            tecladoActivo = 0;
                            fclose(hKeyboard);
                        }

                        if (tempoTotal >= tiempoVisualizacion) {
                            setParameter(INTENTOS_CONSULTA_QR, "FN");
                        }
                        iRet = consultaAutoQR(sizeof(datosVenta), (char *) &datosVenta);

                        getParameter(INTENTOS_CONSULTA_QR, cadCounter);

                        if (strcmp(cadCounter, "RT") == 0) {
                            setParameter(INTENTOS_CONSULTA_QR, "0");
                            ulBeforeDeselectDisplayTime = GTL_StdTimer_GetCurrent();
                            tempoTotal = 0;
                        }
                        resultado = 1;
                        contador++;
                        break;
                }

            } while (iRet == 2 && (tempoTotal < tiempoVisualizacion));
        } else {
            memcpy(datosVenta.codigoRespuesta, "01", 2);
            resultado = 1;
            setParameter(INTENTOS_CONSULTA_QR, "10");
        }
    } else {
        errorRespuestaTransaccion(codigoRespuesta);
    }
    /////// NUMERO DE TRANSACCIONES EXISTENTES ////////
    lengthFile = tamArchivo("/NETCOM", DATOS_QRCODE);
    if (lengthFile > 0) {
        numTransacciones = lengthFile / sizeof(DatosVenta);
        if (numTransacciones >= 6) {
            eliminarTransaccion(datosVenta, numTransacciones);
        }
    }
*/

    }

    return resultado;
}

int consultaAutoQR(int sizeData, char *buffer) {

    int posicion = 0;
    int Resultado = 0;
    int numTransacciones = 0;
    long lengthFile = 0;
    DatosVenta informacionQrCode;

    memset(&informacionQrCode, 0x00, sizeof(informacionQrCode));

    ////// MARCACION ANTICIPADA ////////

    lengthFile = tamArchivo(discoNetcom, (char *)DATOS_QRCODE);
    posicion = lengthFile - sizeof(DatosVenta);

    buscarArchivo(discoNetcom, (char *)DATOS_QRCODE, (char *) &informacionQrCode, posicion,
                  sizeof(DatosVenta));

    /////// NUMERO DE TRANSACCIONES EXISTENTES ////////
    lengthFile = tamArchivo(discoNetcom, (char *)DATOS_QRCODE);
    numTransacciones = lengthFile / sizeof(DatosVenta);

    armarTramaConsultaQR(informacionQrCode);

    Resultado = realizarConsultaQR(informacionQrCode, CONSULTA_EMVCO);

    if (Resultado > 0 && Resultado != 2) {
        // Resultado = eliminarDatosQrCode(informacionQrCode, numTransacciones, 0);
    }

    return Resultado;
}

int armarTramaConsultaQR(DatosVenta datosVenta) {

    ISOHeader isoHeader8583;
    uChar nii[TAM_NII + 1];
    uChar codigoComercio[TAM_FIELD_42 + 1];
    uChar tipoTransaccion[TAM_TIPO_TRANSACCION + 1];
    ResultISOPack resultISOPack;
    uChar variableAux[36 + 1];
    DatosQrCodeEmvco datosQr;
    DatosTerminalQr datoTerminal;
    int iRet = 0;

    memset(&isoHeader8583, 0x00, sizeof(isoHeader8583));
    memset(nii, 0x00, sizeof(nii));
    memset(codigoComercio, 0x00, sizeof(codigoComercio));
    memset(tipoTransaccion, 0x00, sizeof(tipoTransaccion));
    memset(&resultISOPack, 0x00, sizeof(resultISOPack));
    memset(variableAux, 0x00, sizeof(variableAux));
    memset(&datosQr, 0x00, sizeof(datosQr));
    memset(&datoTerminal, 0x00, sizeof(datoTerminal));

    getParameter(CODIGO_COMERCIO, datoTerminal.codigoComercio);
    datosQr = capturarDatosQrCode();
    //datoTerminal = capturarDatoTerminal();

    direccionamientoQRcode(datosVenta.terminalId, nii, datosVenta.processingCode,
                           datosVenta.posEntrymode, codigoComercio);

    isoHeader8583.TPDU = 60;
    memcpy(isoHeader8583.destination, nii, TAM_NII + 1);
    memcpy(isoHeader8583.source, nii, TAM_NII + 1);
    setHeader(isoHeader8583);

    strcat(datosVenta.totalVenta, "");
    sprintf(tipoTransaccion, "%d", TRANSACCION_CONSULTA_QR);
    leftPad(datosVenta.tipoTransaccion, tipoTransaccion, 0x30, TAM_TIPO_TRANSACCION);

    memcpy(datosVenta.msgType, "0100", TAM_MTI);
    memcpy(datosVenta.nii, nii + 1, TAM_NII);
    memcpy(datosVenta.posConditionCode, "00", TAM_POS_CONDITION);

    setMTI(datosVenta.msgType);

    setField(3, datosVenta.processingCode, TAM_CODIGO_PROCESO);
    setField(4, datosVenta.totalVenta, TAM_COMPRA_NETA);
    setField(11, datosVenta.systemTrace, TAM_SYSTEM_TRACE);
    setField(12, datosVenta.txnTime, 6);
    setField(13, datosVenta.txnDate, 4);
    setField(22, datosVenta.posEntrymode, TAM_ENTRY_MODE);
    setField(24, datosVenta.nii, TAM_NII);
    setField(25, datosVenta.posConditionCode, TAM_POS_CONDITION);
    setField(41, datosVenta.terminalId, TAM_TERMINAL_ID);

    memset(variableAux, 0x00, sizeof(variableAux));
    leftPad(variableAux, codigoComercio, 0x30, TAM_FIELD_42);
    setField(42, variableAux, TAM_FIELD_42);

    return iRet;
}


DatosTerminalQr capturarDatoTerminal(void) {

    char buffer[164 + 1];
    int indice = 0;
    DatosTerminalQr datoTerminal;

    memset(&buffer, 0x00, sizeof(&buffer));
    memset(&datoTerminal, 0x00, sizeof(&datoTerminal));

    buscarArchivo(discoNetcom, (char *)CODIGO_UNICO_QR, buffer, 0, 165);

    indice += (66);
    memcpy(datoTerminal.codigoComercio, buffer + indice, TAM_ID_COMERCIO);
    indice += (TAM_ID_COMERCIO);
    memcpy(datoTerminal.nombreComercio, buffer + indice, TAM_COMERCIO);

    return datoTerminal;
}

int realizarConsultaQR(DatosVenta datosVenta, int modoConsulta) {

    int iRet = 0;
    int numTransacciones = 0;
    int tipoTransaccion = TRANSACCION_SOLICITUD_QR;

    long lengthFile = 0;

    char contadorConsultas[2 + 1];
    uChar datosRecibidos[512 + 1];

    unsigned char terminal[8];

    DatosEmpaquetados datosQr;
    ResultISOPack resultISOPack;

    memset(&datosQr, 0x00, sizeof(datosQr));
    memset(&resultISOPack, 0x00, sizeof(resultISOPack));
    memset(datosRecibidos, 0x00, sizeof(datosRecibidos));
    memset(contadorConsultas, 0x00, sizeof(contadorConsultas));

    if (modoConsulta == CONSULTA_F1) {
        tipoTransaccion = CONSULTA_F1;
    }

    resultISOPack = packISOMessage();
    if (modoConsulta == CONSULTA_F1) {
        //screenMessage("COMUNICACION", "CONSULTANDO", "TRANSACCION", "....", 1 * 1000);
    }
    if (strcmp(datosVenta.processingCode, "028900") != 0) {
        iRet = realizarTransaccion(resultISOPack.isoPackMessage, resultISOPack.totalBytes,
                                   datosRecibidos,
                                   TRANSACCION_CONSULTA_QR, CANAL_PERMANENTE, REVERSO_NO_GENERADO);
    } else {
        //iRet = anularQR(datosVenta, datosQr);
    }

    if (iRet > 0) {
        // if (modoConsulta == CONSULTA_F1 && strcmp(datosVenta.processingCode, "028900") != 0) {
        //   screenMessage("COMUNICACION", "", "PROCESANDO", "....", 1 * 1000);
        //}

        if (strcmp(datosVenta.processingCode, "028900") != 0) {
            iRet = desempaquetarConsultaQRcode(datosRecibidos, tipoTransaccion, iRet, datosVenta);
        }

        if (modoConsulta == CONSULTA_F1 && strcmp(datosVenta.processingCode, "028900") == 0 &&
            iRet == 2) {
            iRet = 0;
        }

        if (iRet == 0) {
            if (modoConsulta == CONSULTA_F1) {
                //iRet = getchAcceptCancel("QRCODE", "TRANSACCION", "PENDIENTE DE", "CONSULTA", 30 * 1000);
                iRet = -2;
            } else {
                getParameter(INTENTOS_CONSULTA_QR, contadorConsultas);
                if (strcmp(contadorConsultas, "FN") == 0) {
                    //iRet = getchAcceptCancel("QRCODE", "TRANSACCION PENDIENTE", "POR APROBAR", "DESEA REINTENTAR", 60 * 1000);
                    if (iRet > 0) {
                        setParameter(INTENTOS_CONSULTA_QR, (char *)"RT");
                        iRet = 2;
                    } else if (iRet == -3) {
                        setParameter(INTENTOS_CONSULTA_QR, (char *)"RT");
                        iRet = 2;
                    } else {
                        //screenMessage("VENTA", "POR FAVOR CONSULTAR", "EN EL MENU F1", "(F1) CONSULTAR QR", 3 * 1000);
                        setParameter(INTENTOS_CONSULTA_QR, (char *)"10");
                    }
                }
            }
            //iRet = 1;

            if (iRet < 0) {
                if (modoConsulta == CONSULTA_F1) {
                    //screenMessage("VENTA", "POR FAVOR CONSULTAR", "EN EL MENU F1", "(F1) CONSULTAR QR", 3 * 1000);
                }
            } else {
                iRet = 2;
            }
        }
    } else if (iRet == -3) {
        memcpy(datosVenta.codigoRespuesta, "102", 3);
        setParameter(INTENTOS_CONSULTA_QR, (char *)"10");
        //datosVenta = mostrarEstado(datosVenta);
        lengthFile = tamArchivo(discoNetcom, (char *)DATOS_QRCODE);
        numTransacciones = lengthFile / sizeof(DatosVenta);
        //eliminarDatosQrCode(datosVenta, numTransacciones, 1);
    }

    return iRet;
}

int desempaquetarConsultaQRcode(char *dataRecibida, int tipoTransaccion, int cantidadBytes,
                                DatosVenta datosVenta) {

    ISOFieldMessage isoFieldMessage;
    ResultISOUnpack resultadoUnpack;
    uChar codigoRespuesta[2 + 1];
    uChar mostrarError[50 + 1] = {0x00};
    DatosEmpaquetadosQrCode datosConsultaQr;
    DatosTerminalQr datoTerminal;
    int iRet = 0;
    int tipoQr = 0;
    int numTransacciones = 0;
    int segundaIteraccion = 0;

    long lengthFile = 0;
    DatosVenta datosCaja;
    DatosTarjeta leerTrack;
    DatosEmpaquetados datosQr;

    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(&resultadoUnpack, 0x00, sizeof(resultadoUnpack));
    memset(codigoRespuesta, 0x00, sizeof(codigoRespuesta));
    memset(&datosConsultaQr, 0x00, sizeof(datosConsultaQr));
    memset(&datoTerminal, 0x00, sizeof(datoTerminal));
    memset(&datosCaja, 0x00, sizeof(datosCaja));
    memset(&leerTrack, 0x00, sizeof(leerTrack));
    memset(&datosQr, 0x00, sizeof(datosQr));

    tipoQr = verificarTipoQrcode();
    getTerminalId(datosCaja.terminalId);
    LOGI("tipoQr %d",tipoQr);
    resultadoUnpack = unpackISOMessage(dataRecibida, cantidadBytes);

    isoFieldMessage = getField(11);
    memcpy(datosVenta.systemTrace, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

    isoFieldMessage = getField(12);
    memcpy(datosVenta.txnTime, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

    isoFieldMessage = getField(13);
    memcpy(datosVenta.txnDate, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

    isoFieldMessage = getField(24);
    memcpy(datosVenta.nii, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

    isoFieldMessage = getField(25);
    memcpy(datosVenta.codigoRespuesta, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

    isoFieldMessage = getField(37);
    memcpy(datosVenta.rrn, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

    isoFieldMessage = getField(38);
    memcpy(datosVenta.codigoAprobacion, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

    isoFieldMessage = getField(39);
    memcpy(codigoRespuesta, isoFieldMessage.valueField, isoFieldMessage.totalBytes);
    memcpy(datosVenta.codigoRespuesta, isoFieldMessage.valueField, isoFieldMessage.totalBytes);
    LOGI("datosVenta.codigoRespuesta %s",datosVenta.codigoRespuesta);
    if (strcmp(datosVenta.codigoRespuesta, "00") == 0) {

        memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
        isoFieldMessage = getField(63);

        borrarArchivo(discoNetcom, (char *)CONSULTA_QR);
        escribirArchivo(discoNetcom, (char *)CONSULTA_QR, isoFieldMessage.valueField,
                        isoFieldMessage.totalBytes);

        datosConsultaQr = capturarConsultaQR();

        memcpy(datosVenta.ultimosCuatro, datosConsultaQr.ultimosCuatro,
               sizeof(datosConsultaQr.ultimosCuatro));
        memcpy(datosVenta.cuotas, datosConsultaQr.cuotas, TAM_CUOTAS);
        memcpy(datosVenta.cardName, datosConsultaQr.cardName, sizeof(datosConsultaQr.cardName));
        memcpy(datosVenta.tipoCuenta, datosConsultaQr.tipoCuenta, TAM_TIPO_CUENTA);
        memcpy(datosVenta.field57, datosConsultaQr.bin, 6);

        if (strcmp(datosVenta.processingCode, "028900") != 0) {
            generarInvoice(datosVenta.numeroRecibo);
        }

        leerTrack.dataIssuer = _traerIssuer_(atol(datosConsultaQr.bin));
        memcpy(datosVenta.codigoGrupo, "99",
               TAM_GRUPO); //se requiere un codigo de grupo para transacciones QR code para detallar en el cierre
        memcpy(datosVenta.codigoSubgrupo, leerTrack.dataIssuer.reservedForFutureUse2, TAM_SUBGRUPO);
        datosVenta = valorCompraNeta(datosVenta);

        iRet = 1;
        LOGI(" Linea 736");

        if (iRet > 0) {
            if (verificarHabilitacionCaja() == TRUE) {

                memcpy(&datosCaja, &datosVenta, sizeof(DatosVenta));
                datoTerminal = capturarDatoTerminal();
                memset(datosCaja.codigoComercio, 0x00, sizeof(datosCaja.codigoComercio));
                memset(datosCaja.rrn, 0x00, sizeof(datosCaja.rrn));
                memcpy(datosCaja.rrn, datosVenta.rrn + 6, 6);
                memcpy(datosCaja.codigoComercio, datoTerminal.codigoComercio,
                       sizeof(datosCaja.codigoComercio));
                strcpy(datosCaja.track2, datosConsultaQr.bin);
                strcat(datosCaja.track2, "00");
                strcat(datosCaja.track2, datosConsultaQr.ultimosCuatro);
                memcpy(datosVenta.track2, datosCaja.track2, sizeof(datosVenta.track2));

                setParameter(USO_CAJA_REGISTRADORA, (char *)"1");
                if (tipoQr == 1) {
                    //procesarArchivoConfiguraciones(datosCaja.posEntrymode, 9);
                }
                //transaccionRespuestaDatosCaja(datosCaja, TRANSACCION_TEFF_COMPRAS);
                //usoCaja = 1;

                if (atoi(datosCaja.iva) > 0) {
                    strcat(datosCaja.iva, "00");
                    strcat(datosCaja.inc, "00");
                    strcat(datosCaja.baseDevolucion, "00");
                }
            }
            if (strcmp(datosVenta.processingCode, "028900") == 0) {
                cambiarEstadoTransaccion(datosVenta.numeroRecibo, datosVenta.rrn,
                                         datosVenta.codigoAprobacion,
                                         datosVenta.txnDate, datosVenta.txnTime);
                memset(datosVenta.estadoTransaccion, 0x00, sizeof(datosVenta.estadoTransaccion));
                strcpy(datosVenta.estadoTransaccion, "0");
                _guardarDirectorioJournals_(TRANS_COMERCIO, datosVenta.numeroRecibo,
                                            datosVenta.estadoTransaccion);

            }
            setParameter(ACUMULAR_LIFEMILES, (char *)"1");
            LOGI(" imprimir recibo ");
            imprimirReciboQR(datosVenta);
            if (strcmp(datosVenta.processingCode, "028900") != 0) {
                sprintf(datosVenta.tipoTransaccion, "%02d", TRANSACCION_CONSULTA_QR);
                datosVenta.estadoTransaccion[0] = '1';
                _guardarTransaccion_(datosVenta);

            }

            setParameter(INTENTOS_CONSULTA_QR, (char *)"10");
            iRet = 1;
        } else {

            errorRespuestaTransaccion(codigoRespuesta, mostrarError);

            mostrarEstado(datosVenta.codigoRespuesta, datosVenta.aux1);

            lengthFile = tamArchivo(discoNetcom, (char *)DATOS_QRCODE);
            numTransacciones = lengthFile / sizeof(DatosVenta);
            eliminarDatosQrCode(datosVenta, numTransacciones, 1);

            if (strcmp(datosVenta.aux1, "Trans. Pend. Aprobar") != 0) {
                setParameter(INTENTOS_CONSULTA_QR, (char *)"10");
            }

            if (strcmp(codigoRespuesta, "B1") != 0) {
                iRet = 1;
            } else {
                if (strcmp(datosVenta.processingCode, "028900") == 0) {
                    //iRet = anularQR(datosVenta, datosQr);
                    if (iRet == 1) {
                        setParameter(INTENTOS_CONSULTA_QR, (char *)"10");
                    }
                }
            }
        }
    } else {
        if (strcmp(codigoRespuesta, "B1") != 0
            ||
            (tipoTransaccion == CONSULTA_F1 && strcmp(datosVenta.processingCode, "028900") != 0)) {
            errorRespuestaTransaccion(codigoRespuesta, mostrarError);
        }
        mostrarEstado(datosVenta.codigoRespuesta, datosVenta.aux1);

        lengthFile = tamArchivo(discoNetcom, (char *)DATOS_QRCODE);
        numTransacciones = lengthFile / sizeof(DatosVenta);
        eliminarDatosQrCode(datosVenta, numTransacciones, 1);

        if (strcmp(datosVenta.aux1, "Trans. Pend. Aprobar") != 0) {
            setParameter(INTENTOS_CONSULTA_QR, (char *)"10");
        }

        if (strcmp(codigoRespuesta, "B1") != 0) {
            iRet = 1;
        }
    }
    if (segundaIteraccion == 0) {
        if (strcmp(codigoRespuesta, "00") == 0) {
            iRet = 1;
        }
    }
    return iRet;
}

int eliminarDatosQrCode(DatosVenta informacionQrCode, int numTransacciones, int identificador) {

    int idx = 0;
    int posicion = 0;
    int iRet = 1;
    uChar datoAuxiliar[sizeof(DatosVenta) + 1];
    uChar systemTraceAux[6 + 1];
    char tipoRespuesta[20 + 1];

    memset(datoAuxiliar, 0x00, sizeof(datoAuxiliar));
    memset(systemTraceAux, 0x00, sizeof(systemTraceAux));
    memset(tipoRespuesta, 0x00, sizeof(tipoRespuesta));

    if (identificador == 1) {
        for (idx = 0; idx < numTransacciones; idx++) {
            posicion = idx * sizeof(DatosVenta);
            buscarArchivo(discoNetcom, (char *)DATOS_QRCODE, datoAuxiliar, posicion, sizeof(DatosVenta));
            memcpy(systemTraceAux, datoAuxiliar + 15, 6);
            if (strcmp(systemTraceAux, informacionQrCode.systemTrace) != 0) {
                escribirArchivo(discoNetcom, (char *)DATOS_TEQRCODE, datoAuxiliar, sizeof(DatosVenta));
            } else if (identificador == 1) {
                if (strcmp(systemTraceAux, informacionQrCode.systemTrace) == 0) {
                    memset(datoAuxiliar, 0x00, sizeof(datoAuxiliar));
                    memcpy(datoAuxiliar, &informacionQrCode, sizeof(DatosVenta));
                    escribirArchivo(discoNetcom, (char *)DATOS_TEQRCODE, datoAuxiliar, sizeof(DatosVenta));
                }

            }
        }
    } else {
        for (idx = 0; idx < numTransacciones; idx++) {
            posicion = idx * sizeof(DatosVenta);
            buscarArchivo(discoNetcom, (char *)DATOS_QRCODE, datoAuxiliar, posicion, sizeof(DatosVenta));
            memcpy(systemTraceAux, datoAuxiliar + 15, 6);
            memcpy(tipoRespuesta, datoAuxiliar + 806, 20);
            if (strcmp(tipoRespuesta, "Trans. Pend. Aprobar") == 0
                && strcmp(systemTraceAux, informacionQrCode.systemTrace)
                   != 0) {
                escribirArchivo(discoNetcom, (char *)DATOS_TEQRCODE, datoAuxiliar,
                                sizeof(DatosVenta));
            }
        }
    }

    borrarArchivo(discoNetcom, (char *)DATOS_QRCODE);
    renombrarArchivo(discoNetcom, (char *)DATOS_TEQRCODE, (char *)DATOS_QRCODE);
    borrarArchivo(discoNetcom, (char *)DATOS_TEQRCODE);

    return iRet;
}

DatosEmpaquetadosQrCode capturarConsultaQR(void) {

    char buffer[50 + 1];
    int indice = 0;
    int tambuffer = 0;
    uChar tipoCuentaAux[TAM_TIPO_CUENTA + 1];
    DatosEmpaquetadosQrCode datosConsultaQr;

    memset(buffer, 0x00, sizeof(buffer));
    memset(&datosConsultaQr, 0x00, sizeof(datosConsultaQr));
    memset(tipoCuentaAux, 0x00, sizeof(tipoCuentaAux));

    tambuffer = tamArchivo(discoNetcom, (char *)CONSULTA_QR);
    buscarArchivo(discoNetcom, (char *)CONSULTA_QR, buffer, 0, tambuffer);

    indice = 0;
    memcpy(datosConsultaQr.cardName, buffer + indice, 10);
    indice += (10);
    memcpy(datosConsultaQr.bin, buffer + indice, 6);
    indice += (6);
    memcpy(datosConsultaQr.ultimosCuatro, buffer + indice, 4);
    indice += (4);
    memcpy(datosConsultaQr.cuotas, buffer + indice, TAM_CUOTAS);
    indice += (TAM_CUOTAS);
    memcpy(tipoCuentaAux, buffer + indice, TAM_TIPO_CUENTA);

    if (strcmp(tipoCuentaAux, "AH") == 0) {
        strcpy(datosConsultaQr.tipoCuenta, "10");
    } else if (strcmp(tipoCuentaAux, "CC") == 0) {
        strcpy(datosConsultaQr.tipoCuenta, "20");
    } else if (strcmp(tipoCuentaAux, "CR") == 0) {
        strcpy(datosConsultaQr.tipoCuenta, "30");
    } else if (strcmp(tipoCuentaAux, "CR") == 0) {
        strcpy(datosConsultaQr.tipoCuenta, "40");
    } else if (strcmp(tipoCuentaAux, "LT") == 0) {
        strcpy(datosConsultaQr.tipoCuenta, "41");
    } else if (strcmp(tipoCuentaAux, "CM") == 0) {
        strcpy(datosConsultaQr.tipoCuenta, "42");
    } else if (strcmp(tipoCuentaAux, "CU") == 0) {
        strcpy(datosConsultaQr.tipoCuenta, "43");
    } else if (strcmp(tipoCuentaAux, "BE") == 0) {
        strcpy(datosConsultaQr.tipoCuenta, "44");
    } else if (strcmp(tipoCuentaAux, "RT") == 0) {
        strcpy(datosConsultaQr.tipoCuenta, "45");
    } else if (strcmp(tipoCuentaAux, "RG") == 0) {
        strcpy(datosConsultaQr.tipoCuenta, "46");
    } else if (strcmp(tipoCuentaAux, "DS") == 0) {
        strcpy(datosConsultaQr.tipoCuenta, "47");
    } else if (strcmp(tipoCuentaAux, "DB") == 0) {
        strcpy(datosConsultaQr.tipoCuenta, "48");
    } else if (strcmp(tipoCuentaAux, "BC") == 0) {
        strcpy(datosConsultaQr.tipoCuenta, "49");
    }
    memcpy(globalTramaVenta.cardName, datosConsultaQr.cardName, 10);

    return datosConsultaQr;
}

DatosVenta valorCompraNeta(DatosVenta datosVenta) {

    long valorPrueba = 0;
    long valorNeto = 0;
    uChar auxiliarData[12 + 1];
    uChar auxiliarDataPropina[12 + 1];
    uChar auxiliarDataIva[12 + 1];
    uChar auxiliarDataInc[12 + 1];

    memset(auxiliarData, 0x00, sizeof(auxiliarData));
    memset(auxiliarDataPropina, 0x00, sizeof(auxiliarDataPropina));
    memset(auxiliarDataIva, 0x00, sizeof(auxiliarDataIva));
    memset(auxiliarDataInc, 0x00, sizeof(auxiliarDataInc));

    valorPrueba = strlen(datosVenta.totalVenta);
    memcpy(auxiliarData, datosVenta.totalVenta, valorPrueba - 2);

    valorPrueba = strlen(datosVenta.propina);
    memcpy(auxiliarDataPropina, datosVenta.propina, valorPrueba);

    valorNeto = atol(auxiliarData);
    memset(datosVenta.totalVenta, 0x00, sizeof(datosVenta.totalVenta));
    sprintf(datosVenta.totalVenta, "%ld", valorNeto);
    strcat(datosVenta.totalVenta, "00");

    valorPrueba = strlen(datosVenta.iva);
    memcpy(auxiliarDataIva, datosVenta.iva, valorPrueba);
    valorPrueba = strlen(datosVenta.inc);
    memcpy(auxiliarDataInc, datosVenta.inc, valorPrueba);

    valorNeto = atol(auxiliarData) - atol(auxiliarDataIva) - atol(auxiliarDataInc) -
                atol(auxiliarDataPropina);

    memset(datosVenta.compraNeta, 0x00, sizeof(datosVenta.compraNeta));
    sprintf(datosVenta.compraNeta, "%ld", valorNeto);

    return (datosVenta);
}

void mostrarEstado(char *codigoRespuesta, char *datosAuxiliar) {

    int tipoCodigoRespuesta = 0;

    memset(datosAuxiliar, 0x00, sizeof(datosAuxiliar));

    if (strcmp(codigoRespuesta, "B1") == 0) {
        tipoCodigoRespuesta = 100;
    } else if (strcmp(codigoRespuesta, "C0") == 0) {
        tipoCodigoRespuesta = 101;
    } else if (strcmp(codigoRespuesta, "F1") == 0) {
        tipoCodigoRespuesta = 103;
    } else {
        tipoCodigoRespuesta = atoi(codigoRespuesta);
    }
    memset(codigoRespuesta, 0x00, 3);

    switch (tipoCodigoRespuesta) {
        case 0:
            memcpy(datosAuxiliar, "Aprobado", 8);
            break;
        case 1:
            memcpy(datosAuxiliar, "LLame al Emisor", 15);
            break;
        case 2:
            memcpy(datosAuxiliar, "LLamar REF", 10);
            break;
        case 3:
            memcpy(datosAuxiliar, "Marca No Habilitada", 19);
            break;
        case 5:
            memcpy(datosAuxiliar, "Rechazo General", 15);
            break;
        case 12:
            memcpy(datosAuxiliar, "Transaccion Invalida", 20);
            break;
        case 13:
            memcpy(datosAuxiliar, "Monto Invalido", 14);
            break;
        case 14:
            memcpy(datosAuxiliar, "Tarjeta Invalida", 16);
            break;
        case 19:
            memcpy(datosAuxiliar, "Reinicie Transaccion", 20);
            break;
        case 25:
            memcpy(datosAuxiliar, "No Exis/ Comprobante", 20);
            break;
        case 30:
            memcpy(datosAuxiliar, "Formato Invalido", 16);
            break;
        case 31:
            memcpy(datosAuxiliar, "Tarjeta No Soportada", 20);
            break;
        case 41:
        case 43:
            memcpy(datosAuxiliar, "Retenga Y LLame", 15);
            break;
        case 48:
        case 49:
        case 50:
        case 70:
        case 71:
        case 72:
        case 73:
            memcpy(datosAuxiliar, "Rechazo General", 15);
            break;
        case 51:
            memcpy(datosAuxiliar, "Fondos Insuficientes", 20);
            break;
        case 54:
            memcpy(datosAuxiliar, "Tarjeta Vencida", 15);
            break;
        case 55:
            memcpy(datosAuxiliar, "Pin Invalido", 12);
            break;
        case 57:
            memcpy(datosAuxiliar, "Transa. No Permitida", 20);
            break;
        case 58:
            memcpy(datosAuxiliar, "Termi. No Habilitado", 20);
            break;
        case 61:
            memcpy(datosAuxiliar, "Excede Monto limite", 19);
            break;
        case 62:
            memcpy(datosAuxiliar, "Tar. Uso Restringido", 20);
            break;
        case 65:
            memcpy(datosAuxiliar, "Excede Uso Dia", 14);
            break;
        case 66:
            memcpy(datosAuxiliar, "Bono No Habilitado", 18);
            break;
        case 67:
            memcpy(datosAuxiliar, "Bono Bloqueado", 14);
            break;
        case 68:
            memcpy(datosAuxiliar, "Bono Vencido", 12);
            break;
        case 69:
            memcpy(datosAuxiliar, "Bono Ya Habilitado", 18);
            break;
        case 74:
            memcpy(datosAuxiliar, "Cedula Invalida", 15);
            break;
        case 75:
            memcpy(datosAuxiliar, "Excede Intentos PIN", 19);
            break;
        case 80:
            memcpy(datosAuxiliar, "Factura No Existe", 17);
            break;
        case 81:
            memcpy(datosAuxiliar, "Factura Ya Pagada", 17);
            break;
        case 82:
            memcpy(datosAuxiliar, "Factura Vencida", 15);
            break;
        case 83:
            memcpy(datosAuxiliar, "Servi. No Permitido", 19);
            break;
        case 84:
            memcpy(datosAuxiliar, "Tar. No Autorizada", 18);
            break;
        case 85:
            memcpy(datosAuxiliar, "Factura No Pagada", 17);
            break;
        case 86:
            memcpy(datosAuxiliar, "Cuenta Invalida", 15);
            break;
        case 87:
            memcpy(datosAuxiliar, "Excede Monto Diario", 19);
            break;
        case 91:
            memcpy(datosAuxiliar, "Imposible Autorizar", 19);
            break;
        case 92:
            memcpy(datosAuxiliar, "Cancel. Por Usuario", 19);
            break;
        case 97:
            memcpy(datosAuxiliar, "No Pre-Autorizacion", 19);
            break;
        case 98:
            memcpy(datosAuxiliar, "Pre-Autoriz. Vencida", 20);
            break;
        case 99:
            memcpy(datosAuxiliar, "Aprobado", 8);
            break;
        case 100:
            memcpy(datosAuxiliar, "Trans. Pend. Aprobar", 20);
            break;
        case 101:
            memcpy(datosAuxiliar, "Intente Nuevamente", 18);
            break;
        case 102:
            memcpy(datosAuxiliar, "Time Out", 8);
            break;
        case 103:
            memcpy(datosAuxiliar, "Error.Enviando firma", 20);
            break;
        default:
            break;
    }
}

void imprimirReciboQR(DatosVenta datosVenta) {

    int iRet = 0;
    int tipoRecibo = RECIBO_NO_DUPLICADO;

    char lineaResumen[100 + 1] = {0x00};

    if (strcmp(datosVenta.processingCode, "028900") == 0) {
        tipoRecibo = 6;
    }


    iRet = 1;
    screenMessageFiveLine((char *)"COMPRA CON CODIGO QR", datosVenta.codigoAprobacion,
                          (char *)"NUMERO DE APROBACION", (char *)" ",(char *) " ", (char *)" ", lineaResumen);
    LOGI(" imprimir ticketQr ");
    imprimirTicketVenta(datosVenta, RECIBO_COMERCIO, tipoRecibo);

}