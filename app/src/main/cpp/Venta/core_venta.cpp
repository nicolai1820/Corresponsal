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
#include "android/log.h"

#define  LOG_TAG    "NETCOM_COREVENTA"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
int enviarTransaccion(DatosVenta *tramaVenta, int transaccionQPS, char * pinblock, uChar *tokenDC,
                     DatosCampo61Eco campo61Eco, int intentosPin, uChar *tokenDP, int isDivididas, char * campo11TEff) {


    int resultadoTransaccion = 0;
    int resultadoVenta = 0;
    int lengthTextoAdicional = 0;
    int verificar;
    uChar codigoRespuesta[2 + 1];
    uChar dataRecibida[512];
    uChar field48[36 + 1]; /// iva + base  + inc
    uChar variableAux[36 + 1];
    uChar nii[TAM_NII + 1];
    uChar campoQps[26 + 1];
    ResultISOPack resultadoIsoPackMessage;
    ResultISOUnpack resultadoUnpack;
    ISOFieldMessage isoFieldMessage;
    ISOHeader isoHeader8583;
    int iRet = 0;
    TablaUnoInicializacion tablaUno;
    int numIntentos = 0;
    uChar imprimirQPS[1 + 1];
    char impresionRecibo[1 + 1];
    int resultadoCaja = 0;
    char tipoTransaccionAuxiliar[2 + 1];
    ResultTokenPack resultTokenPack;
    char campo63[12 + 1];
    unsigned char terminal[64];
    DatosLifeMiles datosLifeMiles;
    char lealtadHabilitado[1 + 1];
    char datosRecibo[TAM_JOURNAL_COMERCIO];
    char puntosColombiaActivo[2 + 1];
    char auxiliarMonto[12 + 1];
    char numeroCelular[10 + 1];

    memset(lealtadHabilitado, 0x00, sizeof(lealtadHabilitado));
    memset(&datosLifeMiles, 0x00, sizeof(datosLifeMiles));
    memset(impresionRecibo, 0x00, sizeof(impresionRecibo));
    memset(codigoRespuesta, 0x00, sizeof(codigoRespuesta));
    memset(dataRecibida, 0x00, sizeof(dataRecibida));
    memset(field48, 0x00, sizeof(field48));
    memset(variableAux, 0x00, sizeof(variableAux));
    memset(nii, 0x00, sizeof(nii));
    memset(campoQps, 0x00, sizeof(campoQps));
    memset(imprimirQPS, 0x00, sizeof(imprimirQPS));
    memset(tipoTransaccionAuxiliar, 0x00, sizeof(tipoTransaccionAuxiliar));
    memset(&resultTokenPack, 0x00, sizeof(resultTokenPack));
    memset(campo63, 0x00, sizeof(campo63));
    memset(terminal, 0x00, sizeof(terminal));
    memset(datosRecibo, 0x00, sizeof(datosRecibo));
    memset(puntosColombiaActivo, 0x00, sizeof(puntosColombiaActivo));
    memset(auxiliarMonto, 0x00, sizeof(auxiliarMonto));
    memset(numeroCelular, 0x00, sizeof(numeroCelular));

    getParameter(TIPO_TRANSACCION_AUX, tipoTransaccionAuxiliar);

    /** Obtener Terminal Id **/
    getParameter(NUMERO_TERMINAL, tramaVenta->terminalId);
    getParameter(BIG_BANCOLOMBIA, lealtadHabilitado);
    getParameter(PUNTOS_COLOMBIA_ACTIVO, puntosColombiaActivo);

    if (intentosPin == 0) {
        strcat(tramaVenta->totalVenta, "00");
        strcat(tramaVenta->compraNeta, "00");
    }

    sprintf(variableAux, "%010ld00", atol(tramaVenta->iva));
    sprintf(variableAux + 12, "%010ld00", atol(tramaVenta->baseDevolucion));
    sprintf(variableAux + 24, "%010ld00", atol(tramaVenta->inc));

    _convertASCIIToBCD_(field48, variableAux, 36);

    generarSystemTraceNumber(tramaVenta->systemTrace, "");
    generarInvoice(tramaVenta->numeroRecibo);

    getParameter(NII, nii);
    memset(tramaVenta->modoDatafono,0x00, sizeof(tramaVenta->modoDatafono));
    getParameter(IMPRIMIR_PANEL_FIRMA, tramaVenta->modoDatafono);

    verificar = verificarArchivo("/HOST", RELACION_PUNTOS_COLOMBIA);
    do {

        memset(&isoHeader8583, 0x00, sizeof(isoHeader8583));
        memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
        memset(&resultadoUnpack, 0x00, sizeof(resultadoUnpack));
        memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));

        isoHeader8583.TPDU = 60;
        memcpy(isoHeader8583.destination, nii, TAM_NII + 1);
        memcpy(isoHeader8583.source, nii, TAM_NII + 1);
        setHeader(isoHeader8583);

        memcpy(tramaVenta->nii, nii + 1, TAM_NII);

        setMTI(tramaVenta->msgType);
        setField(3, tramaVenta->processingCode, 6);
        setField(4, tramaVenta->totalVenta, 12);
        setField(11, tramaVenta->systemTrace, 6);
        setField(22, tramaVenta->posEntrymode, 3);
        setField(24, tramaVenta->nii, TAM_NII);
        setField(25, tramaVenta->posConditionCode, 2);
        setField(35, tramaVenta->track2, strlen(tramaVenta->track2));
        setField(41, tramaVenta->terminalId, 8);
        setField(42, tramaVenta->fiel42, strlen(tramaVenta->fiel42));
        setField(48, field48, 18);
        if (strlen(pinblock) > 0) {
            setField(52, pinblock, LEN_PIN_DATA);
        }

        if (strncmp(tokenDC + 2, "DC", 2) == 0) {
            memset(tramaVenta->estadoTarifa, 0x00, TAM_ESTADO_TARIFA + 1);
            memcpy(tramaVenta->estadoTarifa, "DC", TAM_ESTADO_TARIFA);
            memcpy(tramaVenta->tokenVivamos, tokenDC, 44);
            setField(47, tokenDC, 44);
            setField(60, tokenDC, 44);
        } else if (strncmp(tokenDP + 2, "DP", 2) == 0) {
            memset(tramaVenta->estadoTarifa, 0x00, TAM_ESTADO_TARIFA + 1);
            memcpy(tramaVenta->estadoTarifa, "DP", TAM_ESTADO_TARIFA);
            memcpy(tramaVenta->tokenVivamos, tokenDP, 39);
            setField(47, tokenDP, 39);
            setField(60, tokenDP, 39);
        } else if (strcmp(tramaVenta->estadoTarifa, "DT") == 0) {
            setField(60, tramaVenta->tokenVivamos, 19);
        }

        ////// ENVIAR TEXTO ADICIONAL CAMPO 63 ////////
        lengthTextoAdicional = strlen(tramaVenta->textoAdicional);

        if ((strcmp(tramaVenta->estadoTarifa, "LT") == 0) && strlen(tramaVenta->aux1) > 0) {
            //resultTokenPack = armarTokenQB(tramaVenta->aux1);
            memcpy(campo63, tramaVenta->aux1, strlen(tramaVenta->aux1));
            setField(63, campo63, strlen(campo63));
        } else if (lengthTextoAdicional > 0) {
            setField(63, tramaVenta->textoAdicional, lengthTextoAdicional);
        } else if (strlen(campo11TEff) > 0) {
            setField(63, campo11TEff, 7);
        }

        tablaUno = _desempaquetarTablaCeroUno_();
        if (tablaUno.options1.tipProcessing == 1) {

            if (numIntentos == 0 && intentosPin == 0) {
                strcat(tramaVenta->propina, "00");
            }

            leftPad(tramaVenta->field57, tramaVenta->propina, 0x30, 12);
            setField(57, tramaVenta->field57, 12);
        }

        if (transaccionQPS == 1) {
            _convertASCIIToBCD_(campoQps, "10000000000000000000000000", 13);
            setField(61, campoQps, 13);
        }

        setField(62, tramaVenta->numeroRecibo, 6);

        resultadoIsoPackMessage = packISOMessage();

        if (resultadoIsoPackMessage.responseCode > 0) {
            if (atol(tramaVenta->totalVenta) != 0) {
                if (strcmp(tramaVenta->estadoTarifa, "DC") == 0) {
                    //se crea archivo de reverso login dcc para cuando la transaccion DCC es reversada
                   // borrarArchivo(discoNetcom, ARCHIVO_DCC_LOGGIN_REVERSO);
                    //escribirArchivoDccLoggin(*tramaVenta, NO_CODIGO, campo61Eco);
                }

                resultadoTransaccion = realizarTransaccion(resultadoIsoPackMessage.isoPackMessage,
                                                           resultadoIsoPackMessage.totalBytes, dataRecibida, TRANSACCION_VENTA, CANAL_PERMANENTE,
                                                           REVERSO_GENERADO);
            } else {
                //screenMessage("COMUNICACION", "ERROR MONTO", "NO PERMITIDO", "REINTENTE TRANSACCION", 3 * 1000);
            }
        }

        if (resultadoTransaccion > 0) {

            resultadoUnpack = unpackISOMessage(dataRecibida, resultadoTransaccion);

            isoFieldMessage = getField(39);
            memcpy(codigoRespuesta, isoFieldMessage.valueField, isoFieldMessage.totalBytes);
            ////////CODIGO RESPUESTA CAJAS////////
            memcpy(tramaVenta->codigoRespuesta, codigoRespuesta, 2);

            if (strcmp(codigoRespuesta, "00") == 0) {

                if (strlen(tramaVenta->iva) > 0 && (numIntentos == 0) && (intentosPin == 0 || intentosPin == 1)) {
                    strcat(tramaVenta->iva, "00");
                }

                if (strlen(tramaVenta->baseDevolucion) > 0 && (numIntentos == 0)
                    && (intentosPin == 0 || intentosPin == 1)) {
                    strcat(tramaVenta->baseDevolucion, "00");
                }

                if (strlen(tramaVenta->inc) > 0 && (numIntentos == 0) && (intentosPin == 0 || intentosPin == 1)) {
                    strcat(tramaVenta->inc, "00");
                }

                resultadoVenta = verificacionLlegadaTrama(tramaVenta);

                    DatosVenta datosVenta;
                    memset(&datosVenta, 0x00, sizeof(datosVenta));
                    memcpy(&datosVenta, tramaVenta, sizeof(datosVenta));

                   imprimirTicketVenta(*tramaVenta, RECIBO_COMERCIO, RECIBO_NO_DUPLICADO);

                    if (resultadoVenta > 0) {
                        leerArchivo(DISCO_NETCOM, CELULAR_SMS, numeroCelular);
                        memcpy(tramaVenta->tarjetaHabiente + 41, numeroCelular, 10);
                        borrarArchivo(DISCO_NETCOM, CELULAR_SMS);
                        borrarArchivo(DISCO_NETCOM, REVERSO_FIRMA_IP);
                        resultadoVenta = 1;
                    }
                    else {
                        numIntentos = MAX_INTENTOS_VENTA;
                        memset(tramaVenta->codigoRespuesta, 0x00, sizeof(tramaVenta->codigoRespuesta));
                        memcpy(tramaVenta->codigoRespuesta, "01", strlen("01"));
                        if (verificarHabilitacionCaja() == TRUE) {
                            //inicioCaja = FALSE;
                            //responderVentaCaja(*tramaVenta);
                        }
                    }


                if (resultadoVenta == 1) {
                    sprintf(tramaVenta->tipoTransaccion, "%02d", TRANSACCION_VENTA);
                    sprintf(tramaVenta->estadoTransaccion, "%d", TRANSACCION_ACTIVA);

                    if (atoi(tramaVenta->isQPS) != 2 && atoi(tramaVenta->isQPS) != 3
                        && strcmp(tramaVenta->isQPS, "Q") != 0) {
                        iRet = ventaQps(tramaVenta, resultadoUnpack);
                    }

                    if (numIntentos == 0) {
                        _verificarConsultaInformacion_(tramaVenta, dataRecibida, resultadoTransaccion);
                    }

                    _guardarTransaccion_(*tramaVenta);

                    if (strcmp(tramaVenta->estadoTarifa, "DC") == 0) {
                        borrarArchivo(discoNetcom, ARCHIVO_DCC_LOGGIN_REVERSO);
                        borrarArchivo(discoNetcom, ARCHIVO_DCC_LOGGIN);
                        memcpy(campo61Eco.fielId14, ONLINE_AUTHORIZED_TRANSACTION, sizeof(campo61Eco.fielId14));
                        //escribirArchivoDccLoggin(*tramaVenta, NO_CODIGO, campo61Eco);
                    }

                    borrarArchivo(discoNetcom, ARCHIVO_REVERSO);

                    if (verificarHabilitacionCaja() == TRUE) {
                        //inicioCaja = FALSE;
                        //responderVentaCaja(*tramaVenta);
                    }

                    getParameter(IMPRESION_RECIBO_CAJA, impresionRecibo);
                    if (strncmp(impresionRecibo, "01", 2) != 0) {
                        //screenMessage("VENTA", tramaVenta->codigoAprobacion, "NUMERO DE APROBACION", "", 1 * 1000);
                    } else {
                        //screenMessage("VENTA", tramaVenta->codigoAprobacion, "NUMERO DE APROBACION", "", 0);
                    }

                    //// Verificar Puntos Del Emisor ////
                    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
                    isoFieldMessage = getField(60);
                    if (isoFieldMessage.totalBytes > 0) {
                        verificarPuntosEmisor(tramaVenta, isoFieldMessage.valueField, isoFieldMessage.totalBytes);
                    }
                    resultadoCaja = verificarHabilitacionCaja();

                    if (resultadoCaja == TRUE) {
                        iRet = strncmp(impresionRecibo, "01", 2);
                    } else {
                        iRet = 0;
                    }

                    if (iRet == 0) {
                        if (atoi(tramaVenta->isQPS) != 2 && atoi(tramaVenta->isQPS) != 3
                            && strcmp(tramaVenta->isQPS, "Q") != 0) {
                            iRet = ventaQps(tramaVenta, resultadoUnpack);
                        }
                        if (iRet == 0) {
                                imprimirTicketVenta(*tramaVenta, RECIBO_COMERCIO, RECIBO_NO_DUPLICADO);
                               // resultadoVenta = getchAcceptCancel("RECIBO", "IMPRIMIR", "SEGUNDO", "RECIBO ?", 30 * 1000);

                                if (resultadoVenta != __BACK_KEY) {
                                   imprimirTicketVenta(*tramaVenta, RECIBO_CLIENTE, RECIBO_NO_DUPLICADO);
                                }


                            if (verificar == FS_OK) {
                                borrarArchivo("/HOST", MONTO_PENDIENTE_PUNTOS_COLOMBIA);
                            }
                            iRet = 1;

                            setParameter(IMPRIMIR_PANEL_FIRMA, "0");

                            if (strcmp(tramaVenta->estadoTarifa, "DC") == 0) {
                               // realizarLogginDcc();
                            }

                        } else {

                            getParameter(IMPRIMIR_QPS, imprimirQPS);
                            if (atoi(imprimirQPS) == 1) {
                                imprimirTicketVenta(*tramaVenta, RECIBO_QPS, RECIBO_NO_DUPLICADO);
                            }

                        }
                    }

                    if (strncmp((const char*) terminal, "LINK2500", 8) != 0) {
                        if ((atoi(lealtadHabilitado) == 1 || atoi(puntosColombiaActivo) == 1)
                             && isDivididas == 1) {
                            setParameter(ACUMULAR_LIFEMILES, "1");
                            if (atoi(puntosColombiaActivo) == 1) {
                                memcpy(auxiliarMonto, tramaVenta->totalVenta, strlen(tramaVenta->totalVenta) - 2);
                                memset(tramaVenta->totalVenta, 0x00, sizeof(tramaVenta->totalVenta));
                                memcpy(tramaVenta->totalVenta, auxiliarMonto, TAM_COMPRA_NETA);
                            }
                            memcpy(datosLifeMiles.totalVenta, tramaVenta->totalVenta, TAM_COMPRA_NETA);
                            memcpy(datosLifeMiles.compraNeta, tramaVenta->compraNeta, TAM_COMPRA_NETA);
                            memcpy(datosLifeMiles.iva, tramaVenta->iva, TAM_IVA);
                            memcpy(datosLifeMiles.inc, tramaVenta->inc, TAM_IVA);
                            memcpy(datosLifeMiles.baseDevolucion, tramaVenta->baseDevolucion, TAM_COMPRA_NETA);
                            memcpy(datosLifeMiles.track2, tramaVenta->track2, TAM_TRACK2);
                            memcpy(datosLifeMiles.tipoCuenta, tramaVenta->tipoCuenta, TAM_TIPO_CUENTA);
                            //netcomServices(APP_TYPE_LEALTAD, INICIAR_LIFEMILES, sizeof(DatosLifeMiles),(char *) &datosLifeMiles);
                        }
                    }

                    numIntentos = MAX_INTENTOS_VENTA;
                } else {
                    resultadoTransaccion = generarReverso();
                    memset(tramaVenta->codigoRespuesta, 0x00, sizeof(tramaVenta->codigoRespuesta));
                    memcpy(tramaVenta->codigoRespuesta, "01", strlen("01"));

                    if (resultadoTransaccion > 0) {
                        numIntentos++;
                    } else if (verificarHabilitacionCaja() == TRUE) {
                        //inicioCaja = FALSE;
                        //responderVentaCaja(*tramaVenta);
                    }

                    if (numIntentos == MAX_INTENTOS_VENTA) {
                        //screenMessage("COMUNICACION", "REINTENTE", "TRANSACCION", "", 2 * 1000);
                        iRet = -1;
                    }

                }
            } else {
                memcpy(tramaVenta->codigoRespuesta, "01", 2);
                borrarArchivo(DISCO_NETCOM, ARCHIVO_REVERSO);
                borrarArchivo(DISCO_NETCOM, ARCHIVO_DCC_LOGGIN_REVERSO);
                errorRespuestaTransaccion(codigoRespuesta,"mensajeError");

                iRet = -1;
                numIntentos = MAX_INTENTOS_VENTA;
                if (strcmp(codigoRespuesta, "55") == 0) {
                    iRet = -3;
                }

                else if (strcmp(codigoRespuesta, "75") == 0) {
                    iRet = -4;
                }

                if ((verificarHabilitacionCaja() == TRUE) && (iRet != -3)) {
                    //inicioCaja = FALSE;
                    memset(tramaVenta->numeroRecibo, 0x00, strlen(tramaVenta->numeroRecibo));
                    //responderVentaCaja(*tramaVenta);
                }

            }
        }

    } while (resultadoTransaccion > 0 && numIntentos < MAX_INTENTOS_VENTA);

    if (resultadoTransaccion > 0 && atoi(tipoTransaccionAuxiliar) != TRANSACCION_CUENTAS_DIVIDIDAS) {
        if (verificar == FS_OK) {
            leerArchivo("/HOST", RELACION_PUNTOS_COLOMBIA, datosRecibo);
            //netcomServices(APP_TYPE_LEALTAD, DUPLICADO_LEALTAD, TAM_JOURNAL_COMERCIO, &datosRecibo);
        }
        //verificarServicioIPFinal(*tramaVenta);
    } else {
        if (resultadoTransaccion > 0 && atoi(tramaVenta->codigoRespuesta) == 0) {
            iRet = 1;
        } else {
            iRet = -1;
        }
    }

    return iRet;
}

int verificacionLlegadaTrama(DatosVenta * tramaVenta) {

    int resultado = 0;
    int validacion = 1;
    long variableAux = 0;
    uChar stringAux[20];
    uChar fiidOTP[4 + 1];
    ISOFieldMessage isoFieldMessage;
    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(stringAux, 0x00, sizeof(stringAux));
    memset(fiidOTP, 0x00, sizeof(fiidOTP));

    _convertBCDToASCII_(fiidOTP, tramaVenta->field61, 4);
    isoFieldMessage = getField(12);
    memcpy(tramaVenta->txnTime, isoFieldMessage.valueField, 6);
    isoFieldMessage = getField(13);
    memcpy(tramaVenta->txnDate, isoFieldMessage.valueField, 4);
    isoFieldMessage = getField(37);
    memcpy(tramaVenta->rrn, isoFieldMessage.valueField + 6, 6);
    variableAux = atol(tramaVenta->rrn);
    isoFieldMessage = getField(38);
    memcpy(tramaVenta->codigoAprobacion, isoFieldMessage.valueField, 6);

    if (tramaVenta->txnTime[0] != 0x00 && tramaVenta->txnDate != 0x00 && variableAux != 0
        && tramaVenta->codigoAprobacion[0] != 0x00) {

        /// Validar Codigo Proceso-Monto-System Trace-N. Terminal.
        isoFieldMessage = getField(3);

        if (strncmp(fiidOTP, "7999", 4) != 0) {
            validacion = strncmp(isoFieldMessage.valueField, tramaVenta->processingCode, isoFieldMessage.totalBytes);
        } else {
            validacion = 0;
            memcpy(tramaVenta->tipoCuenta, isoFieldMessage.valueField + 2, 2);
        }

        if (validacion == 0) {
            isoFieldMessage = getField(4);
            variableAux = atol(isoFieldMessage.valueField);
            if (variableAux == atol(tramaVenta->totalVenta)) {
                validacion = 0;
            } else {
                validacion = -1;
            }
        }

        if (validacion == 0) {
            isoFieldMessage = getField(11);
            validacion = strncmp(isoFieldMessage.valueField, tramaVenta->systemTrace, isoFieldMessage.totalBytes);
        }

        if (validacion == 0) {
            if (strncmp(fiidOTP, "7999", 4) == 0) {
                isoFieldMessage = getField(35);
                memcpy(tramaVenta->track2, isoFieldMessage.valueField, 16);
            }
        }
        if (validacion == 0) {
            isoFieldMessage = getField(41);
            validacion = strncmp(isoFieldMessage.valueField, tramaVenta->terminalId, isoFieldMessage.totalBytes);
        }

        isoFieldMessage = getField(60);
        if (isoFieldMessage.totalBytes > 0) {
            verificarTokenQi(tramaVenta, isoFieldMessage.valueField,
                             isoFieldMessage.totalBytes);
        }


    }

    if (validacion == 0) {
        resultado = 1;
    } else {
        resultado = -1;
    }

    return resultado;
}
int ventaQps(DatosVenta * datosVenta, ResultISOUnpack resultadoUnpack) {

    int idx = 0;
    int iRet = 0;
    uChar imprimirQPS[1 + 1];
    ISOFieldMessage isoFieldMessage;

    memset(imprimirQPS, 0x00, sizeof(imprimirQPS));
    datosVenta->isQPS[0] = '0';

    for (idx = 0; idx < resultadoUnpack.totalField; idx++) {
        if (resultadoUnpack.listField[idx] == 61) {
            isoFieldMessage = getField(61);
            if ((isoFieldMessage.valueField[13] & 0x10) == 0x10) {
                datosVenta->isQPS[0] = '1';
            }
            break;
        }
    }

    if (datosVenta->isQPS[0] == '1') {
        iRet = 1;
    }

    return iRet;
}

int verificarTokenQi(DatosVenta * datosVenta, uChar * data, int lengthData){

    ResultTokenUnpack resultTokenUnpack;
    TokenMessage tokenMessage;
    int iRet = 0;

    memset(&resultTokenUnpack, 0x00, sizeof(resultTokenUnpack));
    memset(&tokenMessage, 0x00, sizeof(tokenMessage));

    resultTokenUnpack = unpackTokenMessage(data, lengthData);
    tokenMessage = _getTokenMessage_("QI");

    if (tokenMessage.totalBytes > 0) {
        memset(datosVenta->textoInformacion, 0x00, sizeof(datosVenta->textoInformacion));
        memcpy(datosVenta->textoInformacion, tokenMessage.valueToken, 13);
        memcpy(datosVenta->isQPS, "Q", sizeof("Q"));
        iRet = 1;
    }

    return iRet;
}

void imprimirTicketVenta(DatosVenta reciboVenta, int tipoTicket, int duplicado) {

    uChar parFirma[50 + 1];
    uChar parEspacio[50 + 1];
    uChar parCedula[50 + 1];
    uChar parTel[50 + 1];
    uChar parPie[40 + 1];
    uChar valorFormato[20 + 1];
    uChar parBase[20 + 1];
    uChar valorIva[20 + 1];
    uChar aprob[15 + 1];
    uChar rrnVenta[20 + 1];
    uChar stringAux[20 + 1];
    uChar parDuplicado[30 + 1];
    uChar parVersion[20 + 1];
    uChar parCardName[TAM_CARD_NAME + 1];
    uChar fechaExpiracion[20 + 1];
    uChar cuotas[9 + 1];
    uChar AID[TAM_AID + 1];
    uChar ARQC[TAM_ARQC + 1];
    uChar parVenta[20 + 1];
    uChar cardName[15 + 1];
    uChar textoComercio[30 + 1];
    uChar parSaludo[35 + 1];
    uChar textoAdicional[48 + 1];
    uChar parDian[34 + 1];
    uChar bin[9 + 1];
    uChar parTextoCompraNeta[20 + 1];
    uChar parCompraNeta[20 + 1];
    uChar parTextoIva[20 + 1];
    uChar parTextoInc[20 + 1];
    uChar parIva[20 + 1];
    uChar parInc[20 + 1];
    uChar parTextoPropina[20 + 1];
    uChar parPropina[20 + 1];
    uChar parTextoTotal[20 + 1];
    uChar parLogo[35 + 1];
    uChar parTextoBase[40 + 1];
    uChar parCosto[50 + 1];
    uChar parTextoCredito[300 + 1];
    uChar parTextoDCC[300 + 1];
    uChar parInformacion[TAM_TEXTO_INFORMACION + 1];
    uChar parComercio[TAM_COMERCIO + 1];
    uChar parIdComercio[20 + 1];
    uChar parTarifa[50 + 1];
    uChar parTarifa2[50 + 1];
    uChar parcodigoCajero[11 + 1];
    uChar parCajaRecibo[11 + 1];
    uChar parTextoTCambio[20 + 1];
    uChar parTCambio[20 + 1];
    uChar parTCambio2[20 + 1];
    uChar parTCambioM[20 + 1];
    uChar parDCCSeleccion[50 + 1];
    uChar parTextoExtranjera[20 + 1];
    uChar parTextoLocal[20 + 1];
    uChar parTextoMOrigen[20 + 1];
    uChar parMOrigen[20 + 1];
    uChar datoImpuestoIva[2 + 1];
    uChar datoImpuestoInc[2 + 1];
    uChar imprimirPanel[1 + 1];
    uChar aprTComision [20 + 1];
    uChar comisionPlanet [5 + 1];
    uChar codigoCajeroVacio[TAM_CODIGO_CAJERO + 1];
    uChar cajaReciboVacio[TAM_CODIGO_CAJERO + 1];
    uChar auxiliar[20 + 1];
    uChar tipoCuenta[12 + 1];
    uChar hora[10 + 1];
    uChar tagTsi [15 + 1];

    EmisorAdicional emisorAdicional;
    TablaUnoInicializacion tablaUno;
    int isColsubsidio = 0;
    int isPayPass = 0;
    int isOffLine = 0;
    int existeEmisor = 0;
    int imprimeIva = 1;
    int isActualizacionFoco = 0;

    uChar textoCredito[] =
            "PAGARE INCONDICIONALMENTE A LA VISTA Y A LA ORDEN DEL ACREEDOR EL VALOR TOTAL DE ESTE PAGARE JUNTO CON LOS INTERESES A LAS TASAS MAXIMAS PERMITIDAS POR LA LEY.";
    uChar textoDCCPPRO18[] =
            "I have been offered a choice of payment currencies.  This currency  conversion service is offered by this merchant.";

    memset(tipoCuenta, 0x00, sizeof(tipoCuenta));
    memset(hora, 0x00, sizeof(hora));
    memset(codigoCajeroVacio, 0x00, sizeof(codigoCajeroVacio));
    memset(cajaReciboVacio, 0x00, sizeof(cajaReciboVacio));
    memset(parTextoCompraNeta, 0x00, sizeof(parTextoCompraNeta));
    memset(parCompraNeta, 0x00, sizeof(parCompraNeta));
    memset(parTextoIva, 0x00, sizeof(parTextoIva));
    memset(parTextoInc, 0x00, sizeof(parTextoInc));
    memset(parIva, 0x00, sizeof(parIva));
    memset(parInc, 0x00, sizeof(parInc));
    memset(parTextoPropina, 0x00, sizeof(parTextoPropina));
    memset(parPropina, 0x00, sizeof(parPropina));
    memset(parTextoTotal, 0x00, sizeof(parTextoTotal));
    memset(parTextoBase, 0x00, sizeof(parTextoBase));
    memset(parLogo, 0x00, sizeof(parLogo));
    memset(parDian, 0x00, sizeof(parDian));
    memset(parTextoDCC, 0x00, sizeof(parTextoDCC));
    memset(parSaludo, 0x00, sizeof(parSaludo));
    memset(parFirma, 0x00, sizeof(parFirma));
    memset(parEspacio, 0x00, sizeof(parEspacio));
    memset(parCedula, 0x00, sizeof(parCedula));
    memset(parTel, 0x00, sizeof(parTel));
    memset(parPie, 0x00, sizeof(parPie));
    memset(parTextoCredito, 0x00, sizeof(parTextoCredito));
    memset(aprob, 0x00, sizeof(aprob));
    memset(rrnVenta, 0x00, sizeof(rrnVenta));
    memset(valorFormato, 0x00, sizeof(valorFormato));
    memset(valorIva, 0x00, sizeof(valorIva));
    memset(parBase, 0x00, sizeof(parBase));
    memset(parDuplicado, 0x00, sizeof(parDuplicado));
    memset(parVersion, 0x00, sizeof(parVersion));
    memset(fechaExpiracion, 0x00, sizeof(fechaExpiracion));
    memset(cuotas, 0x00, sizeof(cuotas));
    memset(AID, 0x00, sizeof(AID));
    memset(ARQC, 0x00, sizeof(ARQC));
    memset(parCardName, 0x00, sizeof(parCardName));
    memset(parVenta, 0x00, sizeof(parVenta));
    memset(cardName, 0x00, sizeof(cardName));
    memset(textoComercio, 0x00, sizeof(textoComercio));
    memset(textoAdicional, 0x00, sizeof(textoAdicional));
    memset(parInformacion, 0x00, sizeof(parInformacion));
    memset(parCosto, 0x00, sizeof(parCosto));
    memset(parComercio, 0x00, sizeof(parComercio));
    memset(parIdComercio, 0x00, sizeof(parIdComercio));
    memset(parTarifa, 0x00, sizeof(parTarifa));
    memset(parTarifa2, 0x00, sizeof(parTarifa2));
    memset(parcodigoCajero, 0x00, sizeof(parcodigoCajero));
    memset(parCajaRecibo, 0x00, sizeof(parCajaRecibo));
    memset(parTextoTCambio, 0x00, sizeof(parTextoTCambio));
    memset(parTCambio, 0x00, sizeof(parTCambio));
    memset(parTextoExtranjera, 0x00, sizeof(parTextoExtranjera));
    memset(parTextoLocal, 0x00, sizeof(parTextoLocal));
    memset(parTextoMOrigen, 0x00, sizeof(parTextoMOrigen));
    memset(parMOrigen, 0x00, sizeof(parMOrigen));
    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(bin, 0x00, sizeof(bin));
    memset(datoImpuestoIva, 0x00, sizeof(datoImpuestoIva));
    memset(datoImpuestoInc, 0x00, sizeof(datoImpuestoInc));
    memset(imprimirPanel, 0x00, sizeof(imprimirPanel));
    memset(aprTComision, 0x00, sizeof(aprTComision));
    memset(comisionPlanet, 0x00, sizeof(comisionPlanet));
    memset(tagTsi, 0x00, sizeof(tagTsi));

    strcpy(AID, NO_IMPRIMIR);
    strcpy(ARQC, NO_IMPRIMIR);
    strcpy(parCardName, NO_IMPRIMIR);
    strcpy(parSaludo, NO_IMPRIMIR);
    strcpy(parCosto, NO_IMPRIMIR);
    strcpy(textoAdicional, NO_IMPRIMIR);
    strcpy(parComercio, NO_IMPRIMIR);
    strcpy(parTarifa, NO_IMPRIMIR);
    strcpy(parTarifa2, NO_IMPRIMIR);
    strcpy(parTextoDCC, NO_IMPRIMIR);
    strcpy(parTextoTCambio, NO_IMPRIMIR);
    strcpy(parDCCSeleccion, NO_IMPRIMIR);
    strcpy(parTCambio, NO_IMPRIMIR);
    strcpy(parTCambio2, NO_IMPRIMIR);
    strcpy(parTCambioM, NO_IMPRIMIR);
    strcpy(parTextoMOrigen, NO_IMPRIMIR);
    strcpy(parMOrigen, NO_IMPRIMIR);
    strcpy(parTextoLocal, NO_IMPRIMIR);
    strcpy(parTextoExtranjera, NO_IMPRIMIR);
    strcpy(aprTComision, NO_IMPRIMIR);
    strcpy(tagTsi, NO_IMPRIMIR);

    getParameter(IVA_AUTOMATICO, datoImpuestoIva);
    getParameter(INC_AUTOMATICO, datoImpuestoInc);
    setParameter(RECIBO_PENDIENTE, "1");
    getParameter(IMPRIMIR_PANEL_FIRMA, imprimirPanel);
    //memcpy(imprimirPanel, reciboVenta.modoDatafono, strlen(reciboVenta.modoDatafono));

    memcpy(bin, reciboVenta.track2, 9);
    sprintf(parLogo, "%s%s", "0000", ".BMP");
    existeEmisor = buscarBinEmisor(bin);

    if(strcmp(reciboVenta.processingCode,"880000") == 0){
        isActualizacionFoco = 1;
    }

    if (existeEmisor > 0) {
        memset(&emisorAdicional, 0x00, sizeof(emisorAdicional));
        emisorAdicional = traerEmisorAdicional(existeEmisor);
        if (emisorAdicional.existeIssuerEmisor == 1) {
            memset(parLogo, 0x00, sizeof(parLogo));
            memset(bin, 0x00, sizeof(bin));
            _convertBCDToASCII_(bin, &emisorAdicional.indiceImagen, 2);
            existeEmisor = atoi(bin);
            sprintf(parLogo, "%04d%s", existeEmisor, ".BMP");
        }
    }

    /////ES COLSUBSIDIO/////
    if (strncmp(bin, "8800", 4) == 0) {
        isColsubsidio = TRUE;
    }

    if (isActualizacionFoco == 0) {
        if (atoi(reciboVenta.tipoTransaccion) != TRANSACCION_ACUMULAR_LEALTAD
            && atoi(reciboVenta.tipoTransaccion) != TRANSACCION_AJUSTAR_LEALTAD) {
            sprintf(parTextoCompraNeta, "%-11s", "COMPRA NETA");
        }
    }

    sprintf(parTextoPropina, "%-11s", "PROPINA");
    sprintf(parTextoTotal, "%-10s", "TOTAL");
    sprintf(parTextoBase, "%-30s", "BASE DE DEVOLUCION IVA");

    sprintf(fechaExpiracion, "%15s", " ");
    sprintf(cuotas, "%7s", " ");

    /// VERIFICAR SI ES DE PAYPASS POS-ENTRY-MODE ///
    if (strcmp(reciboVenta.posEntrymode, "071") == 0 || strcmp(reciboVenta.posEntrymode, "911") == 0) {
        isPayPass = 1;
    }
    /// VERIFICAR SI ES OFF_LINE ///
    if (isPayPass == 1 && strlen(reciboVenta.rrn) == 0) {
        isOffLine = 1;
    }

    /// TRAE DATOS TABLA UNO INICIALIZACION ///

    memset(&tablaUno, 0x00, sizeof(tablaUno));
    tablaUno = _desempaquetarTablaCeroUno_();

    /// TRAE DATOS TABLA CUATRO INICIALIZACION ///
    TablaCuatroInicializacion tablaCuatro;
    memset(&tablaCuatro, 0x00, sizeof(tablaCuatro));
    tablaCuatro = _desempaquetarTablaCuatro_();

    /// System Trace ///
    uChar numeroRecibo[15 + 1];
    memset(numeroRecibo, 0x00, sizeof(numeroRecibo));
    strcpy(numeroRecibo, "RECIBO: ");
    LOGI(" imprimir ticketQr %s", reciboVenta.numeroRecibo);
    strcat(numeroRecibo, reciboVenta.numeroRecibo);

    /// TOTAL VENTA ///
    uChar totalVenta[20 + 1];
    uChar totalAnulacion[20 + 1];

    if (isActualizacionFoco == 0) {
        memset(totalVenta, 0x00, sizeof(totalVenta));
        formatString(0x00, 0, reciboVenta.totalVenta,
                     strlen(reciboVenta.totalVenta) - 2, valorFormato, 1);
        sprintf(totalVenta, "%13s", valorFormato);
    }
    LOGI(" aprobacion %s", reciboVenta.codigoAprobacion);
    strcpy(aprob, "APRO: ");
    strcat(aprob, reciboVenta.codigoAprobacion);

    /// IMPRIME RRN SOLO SI LA TRANSACCION ES ON_LINE ///
    if (isOffLine == 0) {
        sprintf(rrnVenta, "RRN: %s", reciboVenta.rrn);
    }

    /// ULTIMOS CUATRO ///
    uChar ultimosCuatro[6 + 1];
    memset(ultimosCuatro, 0x00, sizeof(ultimosCuatro));

    if (atoi(reciboVenta.ultimosCuatro) != 0) {
        LOGI(" aprobacion %s", reciboVenta.ultimosCuatro);
        strcpy(ultimosCuatro, "**");
        strcat(ultimosCuatro, reciboVenta.ultimosCuatro);
    } else {
        strcat(ultimosCuatro, "      ");
    }

    /// FECHA ///
    formatoFecha(reciboVenta.txnDate);

    /// HORA ///
    sprintf(hora, "%.2s:%.2s:%.2s", reciboVenta.txnTime, reciboVenta.txnTime + 2, reciboVenta.txnTime + 4);

    ///// COSTO TRANSACCION ///////
    if (strlen(reciboVenta.field61) > 0 && atoi(reciboVenta.field61) > 0) {
        memset(parCosto, 0x00, sizeof(parCosto));
        memset(stringAux, 0x00, sizeof(stringAux));
        formatString(0x30, 0, reciboVenta.field61, strlen(reciboVenta.field61), stringAux, 2);
        sprintf(parCosto, "La operacion tiene un costo de:$ %s", stringAux);
    }
    LOGI(" tipoCuenta %s", reciboVenta.tipoCuenta);
    switch (atoi(reciboVenta.tipoCuenta)) {
        case 0: // PAY PASS
            if ((strcmp(reciboVenta.cardName, "MASTERCARD") != 0) && isPayPass == 1) {
                strcpy(textoCredito, NO_IMPRIMIR);
            }
            break;
        case 10:
            sprintf(tipoCuenta, "%7s", "Ah");
            memset(textoCredito, 0x00, sizeof(textoCredito));
            strcpy(textoCredito, NO_IMPRIMIR);
            break;
        case 20:
            sprintf(tipoCuenta, "%7s", "Cc");
            memset(textoCredito, 0x00, sizeof(textoCredito));
            strcpy(textoCredito, NO_IMPRIMIR);
            break;
        case 30:
            sprintf(tipoCuenta, "%7s", "Cr");
            memset(fechaExpiracion, 0x00, sizeof(fechaExpiracion));
            memset(cuotas, 0x00, sizeof(cuotas));

            if (strlen(reciboVenta.cuotas) > 0 && atoi(reciboVenta.cuotas) > 0) {
                sprintf(cuotas, "CUO: %2s", reciboVenta.cuotas);
            } else {
                strcpy(cuotas, "       ");
            }
            sprintf(fechaExpiracion, "EXP: %-10s", reciboVenta.fechaExpiracion);
            break;
        case 40:
            if (isColsubsidio == TRUE) {
                sprintf(tipoCuenta, "%7s", "Cr");
            } else if (atoi(reciboVenta.creditoRotativo) == 8) {
                sprintf(tipoCuenta, "%7s", "SP");
            } else {
                sprintf(tipoCuenta, "%7s", "Ah");
            }
            memset(fechaExpiracion, 0x00, sizeof(fechaExpiracion));
            memset(cuotas, 0x00, sizeof(cuotas));
            if (strlen(reciboVenta.cuotas) > 0 && atoi(reciboVenta.cuotas) > 0) {
                sprintf(cuotas, "CUO: %2s", reciboVenta.cuotas);
            } else {
                strcpy(cuotas, "       ");
            }
            memset(textoCredito, 0x00, sizeof(textoCredito));
            strcpy(textoCredito, NO_IMPRIMIR);
            sprintf(fechaExpiracion,"%s", "               ");
            break;
        case 41:
            sprintf(tipoCuenta, "%7s", "LT");
            memset(textoCredito, 0x00, sizeof(textoCredito));
            strcpy(textoCredito, NO_IMPRIMIR);
            break;
        case 42:
            sprintf(tipoCuenta, "%7s", "CM");
            memset(textoCredito, 0x00, sizeof(textoCredito));
            strcpy(textoCredito, NO_IMPRIMIR);
            break;
        case 43:
            sprintf(tipoCuenta, "%7s", "CU");
            memset(fechaExpiracion, 0x00, sizeof(fechaExpiracion));
            memset(cuotas, 0x00, sizeof(cuotas));
            if (strlen(reciboVenta.cuotas) > 0 && atoi(reciboVenta.cuotas) > 0) {
                sprintf(cuotas, "CUO: %2s", reciboVenta.cuotas);
            } else {
                strcpy(cuotas, "       ");
            }
            memset(textoCredito, 0x00, sizeof(textoCredito));
            strcpy(textoCredito, NO_IMPRIMIR);
            sprintf(fechaExpiracion, "EXP: %-10s", reciboVenta.fechaExpiracion);
            break;
        case 44:
            sprintf(tipoCuenta, "%7s", "BE");
            memset(textoCredito, 0x00, sizeof(textoCredito));
            strcpy(textoCredito, NO_IMPRIMIR);
            break;

        case 45:
            sprintf(tipoCuenta, "%7s", "RT");
            memset(fechaExpiracion, 0x00, sizeof(fechaExpiracion));
            memset(cuotas, 0x00, sizeof(cuotas));
            if (strlen(reciboVenta.cuotas) > 0 && atoi(reciboVenta.cuotas) > 0) {
                sprintf(cuotas, "CUO: %2s", reciboVenta.cuotas);
            } else {
                strcpy(cuotas, "       ");
            }
            memset(textoCredito, 0x00, sizeof(textoCredito));
            strcpy(textoCredito, NO_IMPRIMIR);
            sprintf(fechaExpiracion, "EXP: %-10s", reciboVenta.fechaExpiracion);
            break;
        case 46:
            sprintf(tipoCuenta, "%7s", "RG");
            memset(textoCredito, 0x00, sizeof(textoCredito));
            strcpy(textoCredito, NO_IMPRIMIR);
            break;
        case 47:
            sprintf(tipoCuenta, "%7s", "DS");
            memset(textoCredito, 0x00, sizeof(textoCredito));
            strcpy(textoCredito, NO_IMPRIMIR);
            break;

        case 48:
            sprintf(tipoCuenta, "%7s", "DB");
            memset(textoCredito, 0x00, sizeof(textoCredito));
            strcpy(textoCredito, NO_IMPRIMIR);
            break;

        case 49:
            sprintf(tipoCuenta, "%7s", "BC");
            memset(fechaExpiracion, 0x00, sizeof(fechaExpiracion));
            memset(cuotas, 0x00, sizeof(cuotas));
            if (strlen(reciboVenta.cuotas) > 0 && atoi(reciboVenta.cuotas) > 0) {
                sprintf(cuotas, "CUO: %2s", reciboVenta.cuotas);
            } else {
                strcpy(cuotas, "       ");
            }
            memset(textoCredito, 0x00, sizeof(textoCredito));
            strcpy(textoCredito, NO_IMPRIMIR);
            sprintf(fechaExpiracion, "EXP: %-10s", reciboVenta.fechaExpiracion);
            break;

        default:
            break;
    }
    LOGI(" salio de tipo de cuenta ");
    if (isIssuerEspecial(reciboVenta.cardName) == 1) {
        memset(cuotas, 0x00, sizeof(cuotas));
        sprintf(cuotas, "CUO: %2s", reciboVenta.cuotas);
    }

    memset(valorFormato, 0x00, sizeof(valorFormato));

    if (isActualizacionFoco == 0) {
        if (atoi(reciboVenta.tipoTransaccion) != TRANSACCION_ACUMULAR_LEALTAD
            && atoi(reciboVenta.tipoTransaccion)
               != TRANSACCION_AJUSTAR_LEALTAD) {
            formatString(0x00, 0, reciboVenta.compraNeta,
                         strlen(reciboVenta.compraNeta) - 2, valorFormato, 1);
            sprintf(parCompraNeta, "%12s", valorFormato);
            memset(stringAux, 0x00, sizeof(stringAux));
        }
    }


    if ((atoi(datoImpuestoIva) == 0 && (atoi(datoImpuestoInc) > 0))) {
        memset(reciboVenta.iva, 0x00, sizeof(reciboVenta.iva));
    }
    LOGI(" Iva ");
    if ((strlen(reciboVenta.iva) >= 2) && imprimeIva != 0 && atoi(reciboVenta.tipoTransaccion) != TRANSACCION_AJUSTAR_LEALTAD) {
        memcpy(stringAux, reciboVenta.iva, strlen(reciboVenta.iva) - 2);
        memset(valorFormato, 0x00, sizeof(valorFormato));
        formatString(0x00, 0, stringAux, strlen(stringAux), valorFormato, 1);
        sprintf(parTextoIva, "%-11s", "IVA");
        sprintf(parIva, "%12s", valorFormato);
    } else {
        sprintf(parTextoIva, "%s",NO_IMPRIMIR);
        strcpy(parIva, NO_IMPRIMIR);
    }

    memset(stringAux, 0x00, sizeof(stringAux));
    LOGI(" INC ");
    if (strlen(reciboVenta.inc) >= 2 && atoi(reciboVenta.tipoTransaccion) != TRANSACCION_AJUSTAR_LEALTAD) {
        memcpy(stringAux, reciboVenta.inc, strlen(reciboVenta.inc) - 2);
        memset(valorFormato, 0x00, sizeof(valorFormato));
        formatString(0x00, 0, stringAux, strlen(stringAux), valorFormato, 1);
        sprintf(parTextoInc, "%-11s", "INC");
        sprintf(parInc, "%12s", valorFormato);
    } else {
        sprintf(parTextoInc, "%s",NO_IMPRIMIR);
        strcpy(parInc, NO_IMPRIMIR);
    }
    LOGI(" texto informacion ");
    if (strlen(reciboVenta.textoInformacion) > 0 && strlen(reciboVenta.isQPS) == 0) {
        strcpy(parInformacion, reciboVenta.textoInformacion);
    } else {
        strcpy(parInformacion, NO_IMPRIMIR);
    }

    if (strcmp(reciboVenta.isQPS, "Q") == 0) {
        memcpy(parCosto, reciboVenta.textoInformacion,
               50);
    }

    memset(stringAux, 0x00, sizeof(stringAux));
    if (strlen(reciboVenta.baseDevolucion) >= 2) {
        memcpy(stringAux, reciboVenta.baseDevolucion, strlen(reciboVenta.baseDevolucion) - 2);
    }

    memset(valorFormato, 0x00, sizeof(valorFormato));
    formatString(0x00, 0, stringAux, strlen(stringAux), valorFormato, 1);
    sprintf(parBase, "%16s*", valorFormato);
    LOGI(" panel de firma  ");
    if (tipoTicket == RECIBO_COMERCIO) {
        if (atoi(reciboVenta.tipoCuenta) == 10 || atoi(reciboVenta.tipoCuenta) == 20
            || atoi(reciboVenta.tipoCuenta) == 0 || atoi(reciboVenta.tipoCuenta) == 30) {
            if ((strncmp(reciboVenta.cardName, "MAESTRO", 7) != 0 && strcmp(reciboVenta.cardName, "V.ELECTRON") != 0
                 && strncmp(reciboVenta.cardName, "VISA DEBIT", 10) != 0 && strncmp(reciboVenta.cardName, "MASTER DEBIT", 12) != 0
                 && strncmp(reciboVenta.cardName, "MASTERDEBIT", 11) != 0
                 && strncmp(reciboVenta.cardName, "MASTERD", 7) != 0
                 && strncmp(reciboVenta.cardName, "TD SERFI", 8) != 0
                 && strncmp(reciboVenta.cardName, "DINERSCLUB", 10) != 0)
                || atoi(imprimirPanel) == 0) {
                if(atoi(imprimirPanel) != 3){

                    strcpy(parTextoCredito, textoCredito);
                    strcpy(parFirma, "FIRMA: _____________________________________");
                    strcpy(parCedula, "C.C:   _____________________________________");
                    strcpy(parTel, "TEL:   _____________________________________");
                }else{
                    strcpy(parTextoCredito, textoCredito);
                    strcpy(parFirma, NO_IMPRIMIR);
                    strcpy(parCedula, NO_IMPRIMIR);
                    strcpy(parTel, NO_IMPRIMIR);
                }
            } else {
                if(atoi(imprimirPanel) != 3){
                    strcpy(parTextoCredito, textoCredito);
                    strcpy(parFirma, "       TRANSACCION AUTENTICADA CON PIN");
                    strcpy(parCedula, NO_IMPRIMIR);
                    strcpy(parTel, NO_IMPRIMIR);
                }else{
                    strcpy(parTextoCredito, textoCredito);
                    strcpy(parFirma, NO_IMPRIMIR);
                    strcpy(parCedula, NO_IMPRIMIR);
                    strcpy(parTel, NO_IMPRIMIR);
                }
            }
        } else {
            if(atoi(imprimirPanel) == 1){
                strcpy(parTextoCredito, textoCredito);
                strcpy(parFirma, "FIRMA: _____________________________________");
                strcpy(parCedula, "C.C:   _____________________________________");
                strcpy(parTel, "TEL:   _____________________________________");
            }
        }
        strcpy(parPie, "*** COMERCIO ***");
    } else if (tipoTicket == RECIBO_CLIENTE) {
        strcpy(parTextoCredito, NO_IMPRIMIR);
        strcpy(parTextoDCC, NO_IMPRIMIR);
        strcpy(parEspacio, NO_IMPRIMIR);
        strcpy(parFirma, NO_IMPRIMIR);
        strcpy(parCedula, NO_IMPRIMIR);
        strcpy(parTel, NO_IMPRIMIR);
        strcpy(parPie, "*** CLIENTE ***");
    } else if (tipoTicket == RECIBO_QPS) {
        strcpy(parTextoCredito, NO_IMPRIMIR);
        strcpy(parFirma, NO_IMPRIMIR);
        strcpy(parCedula, NO_IMPRIMIR);
        strcpy(parTel, NO_IMPRIMIR);
        strcpy(parPie, NO_IMPRIMIR);
    }

    if (atoi(reciboVenta.tipoTransaccion) == TRANSACCION_TARIFA_ADMIN) {
        setParameter(IMPRIMIR_PANEL_FIRMA, "0");
    }
    LOGI(" propina  ");
    if (tablaUno.options1.tipProcessing == 1) {

        if (tablaUno.telephoneDialOptions.keyboardDialing == 1) {
            sprintf(parTextoPropina, "%-11s", "T.AEROP");
        }

        memset(stringAux, 0x00, sizeof(stringAux));
        if (strlen(reciboVenta.propina) >= 2) {
            memcpy(stringAux, reciboVenta.propina, strlen(reciboVenta.propina) - 2);
        }
        memset(valorFormato, 0x00, sizeof(valorFormato));
        formatString(0x00, 0, stringAux, strlen(stringAux), valorFormato, 1);
        sprintf(parPropina, "%12s", valorFormato);
    } else {
        strcpy(parPropina, NO_IMPRIMIR);
    }
    LOGI(" dian  ");
    //memset(parDian, 0x00, sizeof(parDian));
    //strcpy(parDian, "*SUJETO A VERIFICACION POR LA DIAN");

    if (duplicado == RECIBO_DUPLICADO && reciboVenta.estadoTransaccion[0] == '1') {
        memset(parDuplicado, 0x00, sizeof(parDuplicado));
        strcpy(parDuplicado, "     ** DUPLICADO **");

        if (reciboVenta.isQPS[0] == '1') {
            sprintf(parVenta, "%-16s", "VENTA QPS");

            strcpy(parTextoCredito, NO_IMPRIMIR);
            strcpy(parFirma, NO_IMPRIMIR);
            strcpy(parCedula, NO_IMPRIMIR);
            strcpy(parTel, NO_IMPRIMIR);
            strcpy(parPie, NO_IMPRIMIR);
        } else if (atoi(reciboVenta.creditoRotativo) == 1) {
            sprintf(parVenta, "%-16s", "C.ROTATIVO");
        } else {
            sprintf(parVenta, "%-16s", "VENTA");
        }

    } else if ((duplicado == RECIBO_DUPLICADO && reciboVenta.estadoTransaccion[0] == '0') || duplicado == 6) { //6 anulacion
        memset(parDuplicado, 0x00, sizeof(parDuplicado));
        if (duplicado == 6) {
            strcpy(parDuplicado, NO_IMPRIMIR);
        } else {
            strcpy(parDuplicado, "     ** DUPLICADO **");
        }

        if (atoi(reciboVenta.tipoTransaccion) == TRANSACCION_AJUSTAR_LEALTAD) {
            sprintf(parVenta, "%-16s", "LEALTAD");

            memset(valorFormato, 0x00, sizeof(valorFormato));
            memset(totalVenta, 0x00, sizeof(totalVenta));
            formatString(0x00, 0, reciboVenta.totalVenta, strlen(reciboVenta.totalVenta) - 2, valorFormato, 1);
            sprintf(totalAnulacion, "-%s", valorFormato);
            sprintf(totalVenta, "%13s", totalAnulacion);
            memset(parTextoTotal, 0x00, sizeof(parTextoTotal));
            sprintf(parTextoTotal, "%-10s", "AJUSTE");
        } else {
            sprintf(parVenta, "%-16s", "ANULACION");

            memset(valorFormato, 0x00, sizeof(valorFormato));
            memset(totalVenta, 0x00, sizeof(totalVenta));
            formatString(0x00, 0, reciboVenta.totalVenta, strlen(reciboVenta.totalVenta) - 2, valorFormato, 1);
            sprintf(totalAnulacion, "-%s", valorFormato);
            sprintf(totalVenta, "%13s", totalAnulacion);
            memset(parTextoTotal, 0x00, sizeof(parTextoTotal));
            sprintf(parTextoTotal, "%-10s", "ANULACION");
        }
        memset(reciboVenta.AID, 0x00, sizeof(reciboVenta.AID));
        memset(reciboVenta.ARQC, 0x00, sizeof(reciboVenta.ARQC));
        strcpy(AID, NO_IMPRIMIR);
        strcpy(ARQC, NO_IMPRIMIR);
        strcpy(parCardName, NO_IMPRIMIR);
        strcpy(parDian, NO_IMPRIMIR);
        strcpy(parBase, NO_IMPRIMIR);
        strcpy(parTextoBase, NO_IMPRIMIR);

    } else if (tipoTicket == RECIBO_QPS) {
        LOGI(" qps  ");
        memset(parDuplicado, 0x00, sizeof(parDuplicado));
        strcpy(parDuplicado, NO_IMPRIMIR);
        sprintf(parVenta, "%-16s", "VENTA QPS");
    } else if (atoi(reciboVenta.creditoRotativo) == 1) {
        LOGI(" rotativo  ");
        memset(parDuplicado, 0x00, sizeof(parDuplicado));
        strcpy(parDuplicado, NO_IMPRIMIR);
        sprintf(parVenta, "%-16s", "C.ROTATIVO");
    } else {
        LOGI(" linea 1176  ");
        memset(parDuplicado, 0x00, sizeof(parDuplicado));
        strcpy(parDuplicado, NO_IMPRIMIR);
        sprintf(parVenta, "%-16s", "VENTA");
    }
    LOGI(" texto adicional  ");
    ////// TEXTO ADICIONAL ///////
    if (strlen(reciboVenta.textoAdicional) == 0 || atoi(reciboVenta.tipoTransaccion) == TRANSACCION_AJUSTAR_LEALTAD) {
        strcpy(textoAdicional, NO_IMPRIMIR);
    } else {
        sprintf(textoAdicional, "%s %s", reciboVenta.textoAdicionalInicializacion, reciboVenta.textoAdicional);
    }
    LOGI(" arqc  ");
    if (strlen(reciboVenta.ARQC) > 0 && reciboVenta.estadoTransaccion[0] == '1') {
        memset(ARQC, 0x00, sizeof(ARQC));
        strcpy(ARQC, reciboVenta.ARQC);
    }
    LOGI(" aid  ");
    if (strlen(reciboVenta.AID) > 0 && reciboVenta.estadoTransaccion[0] == '1') {
        memset(AID, 0x00, sizeof(AID));
        memcpy(AID, reciboVenta.AID, sizeof(reciboVenta.AID));
    }
    LOGI(" arqc 2  ");
    if (strlen(reciboVenta.ARQC) > 0 || strlen(reciboVenta.AID) > 0) {

        if ((reciboVenta.estadoTransaccion[0] == '0') || duplicado == 6) {
            memset(parSaludo, 0x00, sizeof(parSaludo));
            strcpy(parSaludo, NO_IMPRIMIR);

        }
        LOGI(" estado tx  ");
        if (reciboVenta.estadoTransaccion[0] == '1') {
            strcpy(parCardName, reciboVenta.appLabel);
        }
    }
    LOGI("linea 2211");
    if (reciboVenta.isQPS[0] == '1') {
        memset(parSaludo, 0x00, sizeof(parSaludo));
        strcpy(parSaludo, "AUTORIZADO SIN FIRMA");
    }
    LOGI("cardname %s",reciboVenta.cardName);
    sprintf(cardName, "%-15s", reciboVenta.cardName);

    getParameter(VERSION_SW, parVersion);
    getParameter(REVISION_SW, parVersion + strlen(parVersion));
    getParameter(TEXTO_COMERCIO, textoComercio);

    //////////// SI ES MULTIPOS ///////////////
    if (strlen(reciboVenta.codigoComercio) > 0) {
        LOGI("codigo cajero %s",reciboVenta.codigoComercio);
        strcpy(parIdComercio, reciboVenta.codigoComercio);

        /////// SI ES RECIBO DE MULTIPOS ///////
        if (atoi(reciboVenta.tipoTransaccion) == TRANSACCION_MULTIPOS) {
            memset(stringAux, 0x00, sizeof(stringAux));
            getParameter(TARIFA_ADMIN, stringAux);

            strcpy(parComercio, reciboVenta.nombreComercio);

            if (atoi(stringAux) != FALSE) {
                if (atol(reciboVenta.numeroRecibo2) > 1) {
                    sprintf(parTarifa, "%s%s", "Tarifa Administrativa Asociada al Recibo: ", reciboVenta.numeroRecibo2);
                } else {
                    estadoTarifa(reciboVenta, parTarifa2);
                }
            }
        } else if (atoi(reciboVenta.tipoTransaccion) == TRANSACCION_TARIFA_ADMIN) {

            /////// SI ES RECIBO DE TARIFA ADMIN ///////
            strcpy(parComercio, NO_IMPRIMIR);
            sprintf(parTextoCompraNeta, "%-11s", "T.ADMIN.AGE");
            strcpy(parPropina, NO_IMPRIMIR);
            sprintf(parTarifa, "%s%s", "Compra Multicomercio Asociada al Recibo: ", reciboVenta.numeroRecibo2);
        } else {
            strcpy(parComercio, NO_IMPRIMIR);
            strcpy(parIdComercio, tablaUno.receiptLine3);
        }
    } else {
        strcpy(parComercio, NO_IMPRIMIR);
        strcpy(parIdComercio, tablaUno.receiptLine3);
    }

    memset(stringAux, 0x00, sizeof(stringAux));
    LOGI("codigo cajero");
    if (strncmp(reciboVenta.codigoCajero, codigoCajeroVacio, 10) == 0
        || strcmp(reciboVenta.codigoCajero, NO_IMPRIMIR_CAJERO) == 0
        || atoi(reciboVenta.tipoTransaccion) == TRANSACCION_AJUSTAR_LEALTAD) {
        strcpy(parcodigoCajero, NO_IMPRIMIR);
    } else {
        leftTrim(auxiliar, reciboVenta.codigoCajero, 0x20);
        leftPad(stringAux, auxiliar, 0x30, TAM_CODIGO_CAJERO);
        memcpy(parcodigoCajero, stringAux, LEN_TEFF_CODIGO_CAJERO);
    }

    if (strncmp(reciboVenta.facturaCaja, cajaReciboVacio, 10) == 0) {
        strcpy(parCajaRecibo, NO_IMPRIMIR);
    } else {
        memset(stringAux, 0x00, sizeof(stringAux));
        memset(auxiliar, 0x00, sizeof(auxiliar));
        leftTrim(auxiliar, reciboVenta.facturaCaja, 0x20);
        leftPad(stringAux, auxiliar, 0x30, TAM_CODIGO_CAJERO);
        memcpy(parCajaRecibo, stringAux, LEN_TEFF_NUMERO_FACTURA_ENVIADO_CAJA);
    }

    if (imprimeIva != 1) {
        memset(parDian, 0x00, sizeof(parDian));
        memset(parTextoBase, 0x00, sizeof(parTextoBase));
        memset(parBase, 0x00, sizeof(parBase));
        strcpy(parDian, NO_IMPRIMIR);
        strcpy(parTextoBase, NO_IMPRIMIR);
        strcpy(parBase, NO_IMPRIMIR);
    }
    LOGI("texto DCc");
    ////// TEXTO PARA DCC //////
    if (strcmp(reciboVenta.estadoTarifa, "DC") == 0) {
        memset(parTextoDCC, 0x00, sizeof(parTextoDCC));
        memset(parTextoTCambio, 0x00, sizeof(parTextoTCambio));
        memset(parTCambio, 0x00, sizeof(parTCambio));
        memset(parTextoMOrigen, 0x00, sizeof(parTextoMOrigen));
        memset(parMOrigen, 0x00, sizeof(parMOrigen));

        strcpy(parTextoDCC, NO_IMPRIMIR);
        sprintf(parTextoTCambio, "%-11s", "Tasa Cambio");
        sprintf(parTCambio, "%12s", reciboVenta.acquirerId);
        sprintf(parTextoMOrigen, "%-6s", "V. ORG");
        sprintf(parMOrigen, "%17s", reciboVenta.nombreComercio);
    }
    LOGI("linea 1302");
    if (strncmp(reciboVenta.isQPS, "2", 1) == 0 || strncmp(reciboVenta.isQPS, "3", 1) == 0
        ||strncmp(reciboVenta.aux1+49, "2", 1) == 0 ||strncmp(reciboVenta.aux1+49, "3", 1) == 0){
        if (strcmp(reciboVenta.aux1+50, "W")== 0){
            sprintf(fechaExpiracion, "%s","               ");
            strcpy(tipoCuenta, "  ");
            if (strlen(reciboVenta.textoAdicional) > 0) {
                memset(tagTsi, 0x00, sizeof(tagTsi));
                strcpy(tagTsi, reciboVenta.textoAdicional);
            }
        }

        uChar simbolo[30 + 1];
        uChar simbolo2[30 + 1];
        uChar totalAnulacionDCC[20 + 1];
        char simbolo3 [10 + 1];
        char aux1Dcc[1 +1];

        memset(simbolo, 0x00, sizeof(simbolo));
        memset(simbolo2, 0x00, sizeof(simbolo2));
        memset(parTextoDCC, 0x00, sizeof(parTextoDCC));
        memset(parTextoMOrigen, 0x00, sizeof(parTextoMOrigen));
        memset(parDCCSeleccion, 0x00, sizeof(parDCCSeleccion));
        memset(parMOrigen, 0x00, sizeof(parMOrigen));
        memset(parTextoTCambio, 0x00, sizeof(parTextoTCambio));
        memset(parTCambio2, 0x00, sizeof(parTCambio2));
        memset(parTCambioM, 0x00, sizeof(parTCambioM));
        memset(parTCambioM, 0x00, sizeof(parTCambioM));
        memset(parTCambio, 0x00, sizeof(parTCambio));
        memset(simbolo3, 0x00, sizeof(simbolo3));
        memset(aux1Dcc, 0x00, sizeof(aux1Dcc));

        memcpy(simbolo2, reciboVenta.nombreComercio, 3);
        sprintf(parTextoTCambio, "%-11s", "TASA CAMBIO");
        strcat(simbolo, "1COP /");
        sprintf(parTCambio2, "%-7s", simbolo);
        sprintf(parTCambioM, "%16s", reciboVenta.acquirerId);

        sprintf(parTextoMOrigen, "%-7s", NO_IMPRIMIR);
        sprintf(parMOrigen, "%16s", reciboVenta.nombreComercio);

        if (strncmp(reciboVenta.cardName, "VISA", 4) == 0) {
            memset(comisionPlanet, 0x00, sizeof(comisionPlanet));
            memcpy(comisionPlanet, reciboVenta.aux1 + 40, 5);
            memset(aprTComision, 0x00, sizeof(aprTComision));
            sprintf(aprTComision, "Incluye Comision: %s", comisionPlanet);
        } else {
            strcpy(aprTComision, NO_IMPRIMIR);
        }
        sprintf(parDCCSeleccion, "%-11s", "       MONEDA DE LA TRANSACCION SELECCIONADA");

        memset(simbolo, 0x00, sizeof(simbolo));
        memset(simbolo2, 0x00, sizeof(simbolo2));
        memcpy(aux1Dcc, reciboVenta.aux1+49, 1);

        if (strcmp(reciboVenta.isQPS, "3") == 0 || strcmp(aux1Dcc,"3") == 0 ) {
            strcat(simbolo, "[X]COP ");
            strcat(simbolo2, "[ ]");
        } else {
            strcat(simbolo, "[ ]COP ");
            strcat(simbolo2, "[X]");
        }

        memset(valorFormato, 0x00, sizeof(valorFormato));
        formatString(0x00, 0, reciboVenta.totalVenta, strlen(reciboVenta.totalVenta) - 2, valorFormato, 2);
        strcat(simbolo, valorFormato);
        sprintf(parTextoLocal, "%-11s", simbolo);
        strcat(simbolo2, reciboVenta.nombreComercio);
        sprintf(parTextoExtranjera, "%-11s", simbolo2);

        memset(totalVenta, 0x00, sizeof(totalVenta));

        LOGI(" linea 1371  ");

        if (strcmp(reciboVenta.isQPS, "3") == 0 || strcmp(reciboVenta.aux1 + 49, "3") == 0
            || strcmp(aux1Dcc,"3") == 0 ) {
            if ((duplicado == RECIBO_DUPLICADO
                 && reciboVenta.estadoTransaccion[0] == '0')
                || duplicado == 6) {

                memset(parTextoTotal, 0x00, sizeof(parTextoTotal));
                sprintf(parTextoTotal, "%-14s", "ANULACION-COP");
                memset(simbolo, 0x00, sizeof(simbolo));
                strcat(simbolo, valorFormato);
                sprintf(totalVenta, "%9s", valorFormato);

            } else {
                memset(simbolo, 0x00, sizeof(simbolo));
                memset(parTextoTotal, 0x00, sizeof(parTextoTotal));
                sprintf(parTextoTotal, "%-10s", "TOTAL COP");
                strcpy(simbolo, valorFormato);
                sprintf(totalVenta, "%13s", simbolo);
            }
        } else {
            if ((duplicado == RECIBO_DUPLICADO
                 && reciboVenta.estadoTransaccion[0] == '0')
                || duplicado == 6) {
                sprintf(totalAnulacionDCC, "-%s", reciboVenta.nombreComercio);
                sprintf(totalVenta, "%13s", totalAnulacionDCC);
            } else {
                sprintf(totalVenta, "%13s", reciboVenta.nombreComercio);
            }

        }

        if ((strcmp(reciboVenta.cardName, "MASTERCARD") != 0)) {
            strcpy(parTextoDCC, textoDCCPPRO18);
        } else {
            strcpy(parTextoDCC, NO_IMPRIMIR);
        }
        strcpy(parCosto, NO_IMPRIMIR);

    } else if ((strcmp(reciboVenta.estadoTarifa, "DC") != 0) && (strncmp(reciboVenta.isQPS, "2", 1) != 0)
               && strncmp(reciboVenta.isQPS, "3", 1) != 0 && strcmp(reciboVenta.aux1 + 49, "0") == 0) {
        strcpy(parTextoTCambio, NO_IMPRIMIR);
        strcpy(parTCambio, NO_IMPRIMIR);
        strcpy(parTCambio2, NO_IMPRIMIR);
        strcpy(parTCambioM, NO_IMPRIMIR);
        strcpy(parTextoMOrigen, NO_IMPRIMIR);
        strcpy(parMOrigen, NO_IMPRIMIR);
        strcpy(parTextoLocal, NO_IMPRIMIR);
        strcpy(parTextoExtranjera, NO_IMPRIMIR);
        strcpy(parDCCSeleccion, NO_IMPRIMIR);
        strcpy(parTextoDCC, NO_IMPRIMIR);
    }

    ///// Texto Para Puntos Del Emisor ///////
    uChar line1Puntos[60 + 1];
    uChar line2Puntos[60 + 1];
    uChar line3Puntos[60 + 1];

    memset(line1Puntos, 0x00, sizeof(line1Puntos));
    memset(line2Puntos, 0x00, sizeof(line2Puntos));
    memset(line3Puntos, 0x00, sizeof(line3Puntos));

    getPuntosEmisor(line1Puntos, line2Puntos, line3Puntos, reciboVenta.acquirerId, reciboVenta.tokenVivamos,
                    reciboVenta.estadoTarifa, tipoTicket);

    if (atoi(reciboVenta.tipoTransaccion) == TRANSACCION_TARJETA_MI_COMPRA
        || atoi(reciboVenta.tipoTransaccion) == TRANSACCION_AJUSTAR_LEALTAD) {
        memset(parPropina, 0x00, sizeof(parPropina));
        memset(parTextoPropina, 0x00, sizeof(parTextoPropina));
        strcpy(parPropina, NO_IMPRIMIR);
        strcpy(parTextoPropina, NO_IMPRIMIR);
    }

    uChar verificarIvaAuto[2 + 1];
    uChar verificarIncAuto[2 + 1];

    memset(verificarIvaAuto, 0x00, sizeof(verificarIvaAuto));
    memset(verificarIncAuto, 0x00, sizeof(verificarIncAuto));

    getParameter(IVA_AUTOMATICO, verificarIvaAuto);
    getParameter(INC_AUTOMATICO, verificarIncAuto);

    if (strncmp(verificarIvaAuto, "00", 2) == 0 && strncmp(verificarIncAuto, "00", 2) != 0) {
        memset(parDian, 0x00, sizeof(parDian));
        memset(parBase, 0x00, sizeof(parBase));
        memset(parTextoBase, 0x00, sizeof(parTextoBase));

        strcpy(parDian, NO_IMPRIMIR);
        strcpy(parBase, NO_IMPRIMIR);
        strcpy(parTextoBase, NO_IMPRIMIR);
    }

    if (strlen(reciboVenta.fiel42) > 0 && atoi(reciboVenta.cuotas) > 0) {
        sprintf(cuotas, "CUO: %2s", reciboVenta.cuotas);
    }

    if(strcmp(reciboVenta.estadoTarifa,"LT") == 0) {
        strcpy(parTextoCredito, NO_IMPRIMIR);
    }

    if (isActualizacionFoco == 1) {
        strcpy(parTextoCompraNeta, NO_IMPRIMIR);
        strcpy(parPropina, NO_IMPRIMIR);
        strcpy(parTextoTotal, NO_IMPRIMIR);
        sprintf(parVenta, "%-16s", "ACTUALIZACION");
        strcpy(parPie, NO_IMPRIMIR);
    }
    HasMap hash[] = {

            { "_PAR_FECHA_", reciboVenta.txnDate },
            { "_PAR_VERSION_", parVersion },
            { "_PAR_HORA_", hora },
            { "_PAR_NESTABLE_", tablaUno.defaultMerchantName },
            { "_PAR_DESTABLE_", tablaUno.receiptLine2 },
            { "_PAR_TEXTO_ADM_", textoComercio },
            { "_PAR_COMERCIO_", parComercio },
            { "_PAR_CUNICO_", parIdComercio },
            { "_PAR_TERMINAL_", reciboVenta.terminalId },
            { "_PAR_TARJETA_", cardName },
            { "_PAR_CUATROU_", ultimosCuatro },
            { "_PAR_RECIBO_", numeroRecibo },
            { "_PAR_RRN_", rrnVenta },
            { "_PAR_ADICIONAL_", textoAdicional },
            { "_PAR_VENTA_", parVenta },
            { "_PAR_COMPRA_", parTextoCompraNeta },
            { "_PAR_VCOMPRA_", parCompraNeta },
            { "_PAR_IVA_", parTextoIva },
            { "_PAR_VIVA_", parIva },
            { "_PAR_INC_", parTextoInc },
            { "_PAR_VINC_", parInc },
            { "_PAR_PROPINA_", parTextoPropina },
            { "_PAR_VPROPINA_", parPropina },

            { "_PAR_T_CAMBIO_", parTextoTCambio },
            { "_PAR_VT_CAMBIO_", parTCambio },
            { "_PAR_VTDCC_CAMBIO_", parTCambio2 },
            { "_PAR_VTM_CAMBIO_", parTCambioM },
            { "_PAR_TM_ORIGEN_", parTextoMOrigen },
            { "_PAR_VM_ORIGEN_", parMOrigen },
            { "_PAR_COMISION_", aprTComision },

            { "_PAR_TOTAL_", parTextoTotal },
            { "_PAR_VTOTAL_", totalVenta },
            { "_PAR_BASEDEV_", parTextoBase },
            { "_PAR_VBASEDEV_", parBase },
            { "_PAR_ARQC_", ARQC },
            { "_PAR_TSI_", tagTsi},
            { "_PAR_AID_", AID },
            { "_PAR_LABEL_", parCardName },
            { "_PAR_APRO_", aprob },
            { "_PAR_CUENTA_", tipoCuenta },
            { "_PAR_SALUDO_", parSaludo },
            { "_PAR_ESPACIO_", parEspacio },
            { "_PAR_FIRMA_", parFirma },
            { "_PAR_CEDULA_", parCedula },
            { "_PAR_TELEFONO_", parTel },
            { "_PAR_PIE_", parPie },
            { "_PAR_EXP_", fechaExpiracion },
            { "_PAR_CUOTAS_", cuotas },
            { "_PAR_CREDITO_", parTextoCredito },
            { "_PAR_TDCC_", parTextoDCC },
            { "_PAR_DUPLICADO_", parDuplicado },

            { "_PAR_DCC_SELECCION_", parDCCSeleccion },
            { "_PAR_M_LOCAL_", parTextoLocal },
            { "_PAR_M_EXTRANJERA_", parTextoExtranjera },

            { "_PAR_PUNTOS1_", line1Puntos },
            { "_PAR_PUNTOS2_", line2Puntos },
            { "_PAR_PUNTOS3_", line3Puntos },

            { "_PAR_DIAN_", parDian },
            { "_PAR_COSTO_", parCosto },
            { "_PAR_INFORMACION_", parInformacion },
            { "_PAR_TARJ_HABIENTE_", reciboVenta.tarjetaHabiente },
            { "_PAR_TARIFA_", parTarifa },
            { "_PAR_TCANCELADA_", parTarifa2 },
            { "_PAR_CCAJERO_", parcodigoCajero },
            { "_PAR_CRECIBO_", parCajaRecibo },

            { "FIN_HASH", "" } };
    imprimirTicket(hash, parLogo, "/TICKET.TPL");
    setParameter(RECIBO_PENDIENTE, "0");
}

void estadoTarifa(DatosVenta reciboVenta, char * mensaje) {

    if (atoi(reciboVenta.estadoTarifa) == 0) {
        sprintf(mensaje,"%s", "TARIFA ADMINISTRATIVA CANCELADA POR TIMEOUT");
    } else if (atoi(reciboVenta.estadoTarifa) == -5) {
        sprintf(mensaje, "%s","TARIFA ADMINISTRATIVA DECLINADA");
    } else {
        sprintf(mensaje,"%s", "TARIFA ADMINISTRATIVA CANCELADA POR USUARIO");
    }
}

void verificarPuntosEmisor(DatosVenta * datosVenta, uChar * data, int lengthData) {

    ResultTokenUnpack resultTokenUnpack;
    TokenMessage tokenMessage;

    memset(&resultTokenUnpack, 0x00, sizeof(resultTokenUnpack));
    memset(&tokenMessage, 0x00, sizeof(tokenMessage));

    resultTokenUnpack = unpackTokenMessage(data, lengthData);
    tokenMessage = _getTokenMessage_("CP");

    if (tokenMessage.totalBytes > 0) {
        memcpy(datosVenta->acquirerId, tokenMessage.valueToken, 13);
        memcpy(datosVenta->tokenVivamos, tokenMessage.valueToken + 13, 53);
        memcpy(datosVenta->estadoTarifa, "CP", 2);
    }

}

void getPuntosEmisor(uChar * line1, uChar * line2, uChar * line3, uChar * data1, uChar * data2, uChar * isPuntos,
                     int tipoTicket) {

    uChar datoAux1[15 + 1];
    uChar datoAux2[15 + 1];
    uChar string1[15 + 1];
    uChar string2[15 + 1];
    uChar linea[60 + 1];

    memset(string1, 0x00, sizeof(string1));
    memset(string2, 0x00, sizeof(string2));
    memset(datoAux1, 0x00, sizeof(datoAux1));
    memset(datoAux2, 0x00, sizeof(datoAux2));
    memset(linea, 0x00, sizeof(linea));

    strcpy(line1, NO_IMPRIMIR);
    strcpy(line2, NO_IMPRIMIR);
    strcpy(line3, NO_IMPRIMIR);

    if (strcmp(isPuntos, "CP") == 0 && tipoTicket == RECIBO_CLIENTE) {

        if (data1[0] == 'S') {

            memset(line1, 0x00, 60 + 1);
            memset(line2, 0x00, 60 + 1);
            memset(line3, 0x00, 60 + 1);
            /// Linea 1 Puntos ///
            memcpy(string1, data1 + 1, 6);
            memcpy(string2, data1 + 7, 6);
            leftTrim(datoAux1, string1, '0');
            leftTrim(datoAux2, string2, '0');
            memset(string1, 0x00, sizeof(string1));
            memset(string2, 0x00, sizeof(string2));

            if (strlen(datoAux1) == 0) {
                memset(datoAux1, 0x00, sizeof(datoAux1));
                strcpy(datoAux1, "0");
            }

            if (strlen(datoAux2) == 0) {
                memset(datoAux2, 0x00, sizeof(datoAux2));
                strcpy(datoAux2, "0");
            }

            formatString(0x00, 0, datoAux1, strlen(datoAux1), string1, 2);
            formatString(0x00, 0, datoAux2, strlen(datoAux2), string2, 2);

            sprintf(linea, "Puntos Compra:%s  Total Acum:%s", string1, string2);
            strcpy(line1, linea);

            /// Linea 2 Puntos ///
            memset(string1, 0x00, sizeof(string1));
            memset(datoAux1, 0x00, sizeof(datoAux1));
            memcpy(string1, data2, 6);
            leftTrim(datoAux1, string1, '0');

            if (strlen(datoAux1) == 0) {
                strcpy(datoAux1, "0");
            }
            memset(string1, 0x00, sizeof(string1));
            formatString(0x00, 0, datoAux1, strlen(datoAux1), string1, 2);
            memset(linea, 0x00, sizeof(linea));
            sprintf(linea, "Puntos Vence:%s  Fecha Vence:%.2s/%.2s/%.2s", string1, data2 + 6, data2 + 8, data2 + 10);
            strcpy(line2, linea);

            /// Linea 3 Puntos ///
            memset(linea, 0x00, sizeof(linea));
            sprintf(linea, "%.40s", data2 + 12);
            strcpy(line3, linea);

        }
    }
}
