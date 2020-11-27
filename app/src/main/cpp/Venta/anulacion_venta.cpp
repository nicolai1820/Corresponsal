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
#include <Cajas.h>
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

#define  LOG_TAG    "NETCOM_ANULACION"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define ERROR_TRACK     -32


int cajaActiva = 0;

char systemTrace[12 + 1];

void _anulacion_(int tipoTransaccion) {

    int iRet = 0;
    char calveActual[4 + 1];
    uChar numeroRecibo[TAM_NUMERO_RECIBO + 1];

    memset(numeroRecibo, 0x00, sizeof(numeroRecibo));

    iRet = verificarArchivo(discoNetcom, DIRECTORIO_JOURNALS);

    if (iRet == FS_OK) {
        if (verificarHabilitacionCaja() == TRUE) {
            _anulacionNumeroRecibo_(numeroRecibo);
        } else {
            memset(calveActual, 0x00, sizeof(calveActual));
            getParameter(VALOR_CLAVE_SUPERVISOR, calveActual);
            //iRet = capturarClave("CLAVE SUPERVISOR", "INGRESE", "CLAVE", calveActual, 4);
            if (iRet > 0) {
                //iRet = menuSeleccionAnulacion(tipoTransaccion);
            }
        }
    } else {
        // screenMessage("ANULACION", "SIN TRANSACCIONES", "OPCION NO", "PERMITIDA", 2 * 1000);
    }
}

/**
 * Busca en el Journal las transacciones del tipo especifico.
 * Si entro a esta funcion, se da por verificado la existencia del archivo.
 * @param tipoTransaccion
 * @return 1 si encontro almenos encuentra una transaccion. 0 si no encuentra
 */
int _buscarTransaccionesTipo_(int tipoTransaccion) {
    int resultado = 0;
    int iRet = 0;
    int posicion = 0;
    char dataDuplicado[TAM_JOURNAL + 1];
    DatosVenta datosVenta;

    do {
        memset(dataDuplicado, 0x00, sizeof(dataDuplicado));
        memset(&datosVenta, 0x00, sizeof(datosVenta));

        resultado = buscarArchivo(discoNetcom, (char *)JOURNAL, dataDuplicado, posicion,
                                  sizeof(DatosVenta));

        datosVenta = obtenerDatosVenta(dataDuplicado);

        if (atoi(datosVenta.tipoTransaccion) == tipoTransaccion) {
            resultado = 2;
            iRet = 1;
        }
        posicion += sizeof(DatosVenta);
    } while (resultado == 0);

    return iRet;
}

DatosVenta ultimoReciboAnulacion(void) {

    int resultado = 0;
    int iRet = 0;
    long tamanoJournal = 0;
    char dataDuplicado[TAM_JOURNAL + 1];
    char recibo[6 + 1];
    char monto[15 + 1];
    char monto2[15 + 1];
    DatosVenta datosVenta;
    char linea1[40 + 1];
    char linea2[40 + 1];

    memset(recibo, 0x00, sizeof(recibo));
    memset(dataDuplicado, 0x00, sizeof(dataDuplicado));
    memset(monto, 0x00, sizeof(monto));
    memset(monto2, 0x00, sizeof(monto2));
    memset(&datosVenta, 0x00, sizeof(datosVenta));
    memset(linea1, 0x00, sizeof(linea1));
    memset(linea2, 0x00, sizeof(linea2));

    tamanoJournal = tamArchivo(discoNetcom, (char *)JOURNAL);
    if (tamanoJournal > 0) {
        tamanoJournal -= sizeof(DatosVenta);
        resultado = buscarArchivo(discoNetcom, (char *)JOURNAL, dataDuplicado, tamanoJournal,
                                  sizeof(DatosVenta));
        if (resultado > 0) {
            datosVenta = obtenerDatosVenta(dataDuplicado);

            verificarUltimoRecibo(&datosVenta);
            if ((atoi(datosVenta.tipoTransaccion) == TRANSACCION_VENTA)
                || (IS_MULTIPOS(atoi(datosVenta.tipoTransaccion)) == 1)
                || (atoi(datosVenta.tipoTransaccion) == TRANSACCION_GASO_PASS)
                || (atoi(datosVenta.tipoTransaccion) == TRANSACCION_EFECTIVO)
                || (atoi(datosVenta.tipoTransaccion) == TRANSACCION_BIG_BANCOLOMBIA)
                || atoi(datosVenta.tipoTransaccion) == TRANSACCION_PAGO_IMPUESTOS
                || (atoi(datosVenta.tipoTransaccion) == TRANSACCION_FOCO_SODEXO)) {
                if (atoi(datosVenta.estadoTransaccion) == 1) {
                    /////// DATOS DE TARJETA - PERMITE COSTO TRANSACCION ?///////
                    memset(linea1, 0x00, sizeof(linea1));
                    memcpy(monto, datosVenta.totalVenta, strlen(datosVenta.totalVenta) - 2);
                    formatString(0x00, 0, monto, strlen(monto), monto2, 2);
                    sprintf(linea1, "RECIBO : %s", datosVenta.numeroRecibo);
                    sprintf(linea2, "VALOR  : $ %s", monto2);
                    //iRet = getchAcceptCancel("ANULACION", linea1, linea2, "Desea Continuar?", 30 * 1000);

                    if (iRet > 0) {
                        anularTransaccion(datosVenta);
                    }

                } else {
                    //screenMessage("ANULACION", "RECIBO YA ANULADO", "CANCELANDO", "LA TRANSACCION", 2 * 1000);
                }
            } else if (atoi(datosVenta.tipoTransaccion) == TRANSACCION_RECARGAR_BONO) {
                //ultimoReciboAnulacionBono();

                cajaActiva = FALSE;

                ////////////////// anulacion habilitacion ////////////////////////
            } else if (atoi(datosVenta.tipoTransaccion) == TRANSACCION_HABILITAR_BONO) {
//					ultimoReciboAnuHabilitacion();

                cajaActiva = FALSE;

                ////////////////// anulacion habilitacion ////////////////////////

            } else if ((atoi(datosVenta.tipoTransaccion) == TRANSACCION_PAGO_ELECTRONICO)
                       || (atoi(datosVenta.tipoTransaccion) ==
                           TRANSACCION_PAGO_ELECTRONICO_ESPECIAL)) {
                //anulacionDineroElectronico(datosVenta);
            } else if (atoi(datosVenta.tipoTransaccion) == TRANSACCION_CELUCOMPRA) {
                //netcomServices(APP_TYPE_CELUCOMPRA, ANULACION_CELUCOMPRA, sizeof(datosVenta), (char *) &datosVenta);
            } else if (atoi(datosVenta.tipoTransaccion) == TRANSACCION_CONSULTA_QR) {
                //netcomServices(APP_TYPE_QR, ANULACION_QR, sizeof(datosVenta), (char *) &datosVenta);
            } else {
                //screenMessage("ANULACION", "NO SOPORTADA", "CANCELANDO", "LA TRANSACCION", 2 * 1000);
            }
        }
    }
    return datosVenta;
}

void _anulacionNumeroRecibo_(uChar *numeroRecibo) {

    int iRet = 1;
    int verificar = 0;
    int cajaAnulacion = 0;
    int tamJournal = TAM_DIR_JOURNALS * 2;

    long tamDirJournal = 0;

    char numeroReciboAux[6 + 1];
    char monto[12 + 1];
    char monto2[12 + 1];
    char variableAux[20 + 1];
    char variableAux2[20 + 1];
    char numeroCelular[10 + 1];

    unsigned char terminal[8 + 1];

    DatosVenta datosVenta;
    DatosVenta datosReverso;
    DatosJournals datosJournals;
    TablaTresInicializacion dataIssuer;

    if (atoi(globalTramaVenta.numeroRecibo) > 0) {
        LOGI("globalTramaVenta.numeroRecibo %s ", globalTramaVenta.numeroRecibo);
        memset(&datosVenta, 0x00, sizeof(datosVenta));
        memcpy(&datosVenta, &globalTramaVenta, sizeof(globalTramaVenta));
        iRet = 1;
        goto A;
    } else {
        memset(&globalTramaVenta, 0x00, sizeof(globalTramaVenta));
    }
    memset(&datosVenta, 0x00, sizeof(datosVenta));
    memset(numeroReciboAux, 0x00, sizeof(numeroReciboAux));
    memset(&dataIssuer, 0x00, sizeof(dataIssuer));
    memset(monto, 0x00, sizeof(monto));
    memset(monto2, 0x00, sizeof(monto2));
    memset(variableAux, 0x00, sizeof(variableAux));
    memset(variableAux2, 0x00, sizeof(variableAux2));
    memset(&datosJournals, 0x00, sizeof(datosJournals));
    memset(&datosReverso, 0x00, sizeof(datosReverso));
    memset(terminal, 0x00, sizeof(terminal));
    memset(numeroCelular, 0x00, sizeof(numeroCelular));


    cajaActiva = 0;

    if (verificarHabilitacionCaja() == TRUE && numeroRecibo[0] == 0x00) {
        cajaActiva = TRUE;
        //iRet = transaccionPeticionDatosCaja(&datosVenta, TRANSACCION_TEFF_ANULACIONES);

        if (iRet == ERROR_NO_RESPONDE) {
            sprintf(datosVenta.codigoRespuesta, "%02d", ERROR_NO_RESPONDE);
        } else {
            memcpy(datosVenta.codigoRespuesta, "01", 2);
        }

        leftTrim(numeroRecibo, datosVenta.numeroRecibo, ' ');
        //		sprintf(numeroRecibo,"%06d",atoi(datosVenta.numeroRecibo));
    } else if (numeroRecibo[0] == 0x00) {
        tamDirJournal = tamArchivo(discoNetcom, (char *)DIRECTORIO_JOURNALS);
        iRet = buscarArchivo(discoNetcom, (char *)DIRECTORIO_JOURNALS, (char *) &datosJournals,
                             tamDirJournal - TAM_DIR_JOURNALS, tamJournal);
        memcpy(numeroRecibo, datosJournals.numeroRecibo, TAM_NUMERO_RECIBO);
        if (strcmp(datosJournals.estadoTransaccion, "0") == 0) {
            memset(&datosJournals, 0x00, sizeof(datosJournals));

            strcpy(numeroReciboAux, numeroRecibo);

            memset(numeroRecibo, 0x00, sizeof(numeroRecibo));

            buscarArchivo(discoNetcom, (char *)DIRECTORIO_JOURNALS, (char *) &datosJournals,
                          (tamDirJournal - (2 * TAM_DIR_JOURNALS)), TAM_DIR_JOURNALS);
            memcpy(numeroRecibo, datosJournals.numeroRecibo, TAM_NUMERO_RECIBO);
            if (strcmp(numeroRecibo, numeroReciboAux) == 0) {
                //screenMessage("ANULACION", "RECIBO YA ANULADO", "CANCELANDO", "LA TRANSACCION", 2 * 1000);
                iRet = 0;
            }
        }
    }

    leftPad(numeroReciboAux, numeroRecibo, '0', 6);

    datosJournals = buscarReciboMulticorresponsal(numeroReciboAux);
    if (datosJournals.journal == TRANS_MULT_BANCOLOMBIA ||
        datosJournals.journal == TRANS_MULT_AVAL) {
        //screenMessage("MENSAJE", "TRANSACCION", "NO TIENE", "ANULACION", 2 * 1000);
        iRet = 0;
    } else if (datosJournals.journal == TRANS_CNB_BANCOLOMBIA) {
        //netcomServices(APP_TYPE_CNB_BANCOLOMBIA, ANULACION_RECIBO_BANCOLOMBIA, TAM_NUMERO_RECIBO, numeroRecibo);
        cajaActiva = FALSE;
        //screenMessage("MENSAJE", "TRANSACCION", "NO PERMITIDA", "", 2 * 1000);
        iRet = 0;
    } else if (datosJournals.journal == TRANS_CNB_CORRESPONSALES) {
        //screenMessage("MENSAJE", "TRANSACCION", "NO PERMITIDA", "", 2 * 1000);
        iRet = 0;
    } else if (strlen(datosJournals.numeroRecibo) <= 0 && iRet > 0) {
        if (strcmp(numeroRecibo, "ERRADA") != 0) {
            //screenMessage("ANULACION", "NO EXISTE", "RECIBO", "", 2 * 1000);
            if (verificarHabilitacionCaja() == TRUE) {
                cajaAnulacion = FALSE;
                cajaActiva = TRUE;
            }
        }
        iRet = 0;
    }

    if (iRet > 0) {

        leftPad(numeroReciboAux, numeroRecibo, '0', 6);

        buscarArchivo(discoNetcom, (char *)JOURNAL, (char *) &datosVenta, datosJournals.posicionTransaccion,
                      sizeof(DatosVenta));

        memcpy(datosVenta.codigoRespuesta, "01", 2); //se setea codigo de respuesta en 1 para caja

        if (atoi(datosVenta.tipoTransaccion) != 0) {

            if (atoi(datosVenta.tipoTransaccion) == TRANSACCION_VENTA
                || IS_MULTIPOS(atoi(datosVenta.tipoTransaccion))
                   == 1 || atoi(datosVenta.tipoTransaccion) == TRANSACCION_GASO_PASS
                || atoi(datosVenta.tipoTransaccion) == TRANSACCION_EFECTIVO
                || atoi(datosVenta.tipoTransaccion) == TRANSACCION_TARJETA_MI_COMPRA
                || (atoi(datosVenta.tipoTransaccion) == TRANSACCION_BIG_BANCOLOMBIA)
                || atoi(datosVenta.tipoTransaccion) == TRANSACCION_PAGO_IMPUESTOS
                || atoi(datosVenta.tipoTransaccion) == TRANSACCION_REDENCION_LIFEMILES
                || atoi(datosVenta.tipoTransaccion) == TRANSACCION_ACUMULAR_LEALTAD) {

                if (atoi(datosVenta.estadoTransaccion) == 1) {
                    /////// DATOS DE TARJETA - PERMITE COSTO TRANSACCION ?///////

                    if (strlen(datosVenta.codigoAprobacion) > 0) {
                        if (strlen(datosVenta.tipoTransaccion) > 0) {
                            memcpy(monto, datosVenta.totalVenta, strlen(datosVenta.totalVenta) - 2);
                            memcpy(numeroCelular, datosVenta.tarjetaHabiente + 41, 10);
                            formatString(0x00, 0, monto, strlen(monto), monto2, 2);
                            memset(variableAux, 0x00, sizeof(variableAux));
                            sprintf(variableAux, "RECIBO: %.6s", datosVenta.numeroRecibo);
                            sprintf(variableAux2, "VALOR:$%s", monto2);


                            memcpy(&globalTramaVenta, &datosVenta, sizeof(datosVenta));
                            //iRet = getchAcceptCancel("ANULACION", variableAux, variableAux2, "Desea Continuar?", 20 * 1000);
                            A:
                            LOGI("llegamos A");
                            if (iRet > 0) {
                                LOGI("estado tx %s", datosVenta.estadoTransaccion);
                                if (atoi(datosVenta.estadoTransaccion) == TRANSACCION_ACTIVA) {
                                    anularTransaccion(datosVenta);
                                    cajaAnulacion = TRUE;

                                } else {
                                    //screenMessage("ANULACION", "RECIBO YA ANULADO", "CANCELANDO", "LA TRANSACCION", 2 * 1000);
                                }
                            } else {
                                if (verificarHabilitacionCaja() == TRUE) {
                                    cajaAnulacion = FALSE;
                                    cajaActiva = TRUE;
                                }
                            }
                        } else {
                            // screenMessage("ANULACION", "SIN TRANSACCIONES", "OPCION NO", "PERMITIDA", 2 * 1000);
                        }
                    }
                } else {
                    //screenMessage("ANULACION", "RECIBO YA ANULADO", "CANCELANDO", "LA TRANSACCION", 2 * 1000);
                    if (verificarHabilitacionCaja() == TRUE) {
                        cajaAnulacion = FALSE;
                        cajaActiva = TRUE;
                    }
                }
            } else if (atoi(datosVenta.tipoTransaccion) == TRANSACCION_RECARGAR_BONO) {

                //otroReciboAnulacionBono(numeroRecibo);

                cajaAnulacion = FALSE;
                cajaActiva = FALSE;
                ///////////////////anulacion habilitacion////////////////////
            } else if (atoi(datosVenta.tipoTransaccion) == TRANSACCION_HABILITAR_BONO) {

                //otroReciboAnuHabilitacion(numeroRecibo);
                cajaAnulacion = FALSE;
                cajaActiva = FALSE;
                ///////////////////anulacion habilitacion////////////////////

            } else if (atoi(datosVenta.tipoTransaccion) == TRANSACCION_PAGO_ELECTRONICO
                       ||
                       atoi(datosVenta.tipoTransaccion) == TRANSACCION_PAGO_ELECTRONICO_ESPECIAL) {
                //anulacionDineroElectronico(datosVenta);
            } else if (atoi(datosVenta.tipoTransaccion) == TRANSACCION_CELUCOMPRA) {
                //netcomServices(APP_TYPE_CELUCOMPRA, ANULACION_CELUCOMPRA, sizeof(datosVenta), (char *) &datosVenta);
            } else if (atoi(datosVenta.tipoTransaccion) == TRANSACCION_CONSULTA_QR) {
                //netcomServices(APP_TYPE_QR, ANULACION_QR, sizeof(datosVenta), (char *) &datosVenta);
            } else if (atoi(datosVenta.tipoTransaccion) == TRANSACCION_PUNTOS_COLOMBIA) {

                memcpy(monto, datosVenta.totalVenta, strlen(datosVenta.totalVenta));
                formatString(0x00, 0, monto, strlen(monto), monto2, 2);
                memset(variableAux, 0x00, sizeof(variableAux));
                sprintf(variableAux, "RECIBO: %.6s", datosVenta.numeroRecibo);
                sprintf(variableAux2, "VALOR:$%s", monto2);
                //iRet = getchAcceptCancel("ANULACION", variableAux, variableAux2, "Desea Continuar?",20 * 1000);

                if (iRet > 0) {
                    verificar = verificarArchivo(discoNetcom, REVERSO_PUNTOS_COLOMBIA);

                    if (verificar == FS_OK) {
                        leerArchivo(discoNetcom, (char *)REVERSO_PUNTOS_COLOMBIA, (char *) &datosReverso);
                        //iRet = netcomServices(APP_TYPE_LEALTAD, ANULACION_PUNTOS_COLOMBIA, TAM_JOURNAL_COMERCIO,(char*) &datosReverso);
                    }
                    if (iRet > 0) {
                        // netcomServices(APP_TYPE_LEALTAD, ANULACION_PUNTOS_COLOMBIA, sizeof(datosVenta), (char *) &datosVenta);
                    }
                }

            } else {
                // screenMessage("ANULACION", "NO SOPORTADA", "CANCELANDO", "LA TRANSACCION", 2 * 1000);
            }

        }
    }

    if (verificarHabilitacionCaja() == TRUE && cajaAnulacion != TRUE && cajaActiva == TRUE

        && atoi(datosVenta.tipoTransaccion) != TRANSACCION_CONSULTA_QR
        && atoi(datosVenta.tipoTransaccion) != TRANSACCION_PAGO_ELECTRONICO
        && atoi(datosVenta.tipoTransaccion) != TRANSACCION_PUNTOS_COLOMBIA) {
        responderAnulacionCaja(datosVenta);
    }
}

DatosVenta buscarReciboAnulacion(char *numeroRecibo) {

    int resultado = 0;
    int posicion = 0;
    char dataDuplicado[TAM_JOURNAL + 1];
    char recibo[6 + 1];
    int controlRecibo = 0;
    DatosVenta datosVenta;

    do {
        memset(dataDuplicado, 0x00, sizeof(dataDuplicado));
        memset(&datosVenta, 0x00, sizeof(datosVenta));
        memset(recibo, 0x00, sizeof(recibo));
        resultado = buscarArchivo(discoNetcom, (char *)JOURNAL, dataDuplicado, posicion,
                                  sizeof(DatosVenta));
        datosVenta = obtenerDatosVenta(dataDuplicado);
        if (strncmp(datosVenta.numeroRecibo, numeroRecibo, TAM_NUMERO_RECIBO) == 0) {
            controlRecibo = 1;
            resultado = 2;
        }
        posicion += sizeof(DatosVenta);
    } while (resultado == 0);

    if (controlRecibo != 1) {
        memset(&datosVenta, 0x00, sizeof(datosVenta));
        //screenMessage("RECIBO", "RECIBO NO HALLADO", "VERIFIQUE NUMERO", "DE RECIBO", 2 * 1000);
    }
    return datosVenta;
}

void anularTransaccion(DatosVenta datosVenta) {

    char dataRecibida[512 + 1];
    uChar codigoProcesoAux[6 + 1];
    uChar bin[9 + 1];
    uChar bufferAux[TAM_J_DCC + 1];
    int cantidadBytes = 0;
    int iRet = 0;
    int numIntentos = 0;
    int resultadoCosto = 0;
    int resultado = 0;
    ResultISOPack resultadoIsoPackMessage;
    DatosVenta datosVentaEnvio;
    TablaTresInicializacion dataIssuer;
    long binTarjeta = 0;
    int isOffLine = 0;
    char auxiliarNumeroRecibo[TAM_NUMERO_RECIBO + 1];
    char impresionRecibo[1 + 1];
    int resultadoCaja = 0;
    DatosCampo61Eco campo61Eco;
    int indice = 0;
    int posicion = 0;
    int isTarjeta = 0;
    char estadoTj[1 + 1];
    char pinBlock[16];

    uChar issuerCountryCode[TAM_ISSUER_COUNTRY_CODE + 1];
    uChar reciboDCC[TAM_NUMERO_RECIBO + 1];

    memset(auxiliarNumeroRecibo, 0x00, sizeof(auxiliarNumeroRecibo));
    memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
    memset(dataRecibida, 0x00, sizeof(dataRecibida));
    memset(&datosVentaEnvio, 0x00, sizeof(datosVentaEnvio));
    memcpy(&datosVentaEnvio, &datosVenta, sizeof(datosVenta));
    memset(pinBlock, 0x00, sizeof(pinBlock));

    if (strncmp(datosVenta.processingCode, "000000", 6) == 0 && strlen(datosVenta.rrn) == 0) {
        isOffLine = 1;
    }

    if (isOffLine == 0) {
        //se realiza consulta de costo
        resultadoCosto = _TRUE_;
        memset(&dataIssuer, 0x00, sizeof(dataIssuer));
        memset(bin, 0x00, sizeof(bin));
        memcpy(codigoProcesoAux, datosVenta.processingCode, 6);
        memcpy(bin, datosVenta.track2, 9);
        binTarjeta = atol(bin);
        dataIssuer = _traerIssuer_(binTarjeta);
        if (dataIssuer.additionalPromptsSelect.additionalPrompt4 == 1) {
            LOGI("codigoProcesoAux %s ",codigoProcesoAux);
            memset(datosVenta.processingCode, 0x00, sizeof(datosVenta.processingCode));
            sprintf(datosVenta.processingCode, "02%.4s", codigoProcesoAux + 2);
            // resultadoCosto = consultarCosto(&datosVenta);SI VA EN ANDROID
        }

        if (resultadoCosto == _TRUE_) {
            do {
                LOGI("costo es true ");

                memset(datosVenta.processingCode, 0x00, sizeof(datosVenta.processingCode));
                memcpy(datosVenta.processingCode, codigoProcesoAux, 6);
                //empaqueta trama
                //isTarjeta = verificarAnulacionTarjeta(&datosVenta, estadoTj);

                if (isTarjeta == 1) {
                    cajaActiva = FALSE;
                }
                if (isTarjeta == 0) {
                    /*iRet = solicitarPinAnulacionBanda(datosVenta.posEntrymode,
                                                      datosVenta.modoDatafono,
                                                      datosVenta.track2, datosVenta.cardName,
                                                      datosVenta.totalVenta, pinBlock);*/
                    iRet = 1;
                    if (iRet > 0) {
                        LOGI("Empaquetar anulacion ");
                        resultadoIsoPackMessage = empaquetarAnulacion(&datosVentaEnvio,
                                                                      numIntentos);
                    }
                } else {
                    memset(datosVenta.tipoTransaccion, 0x00, sizeof(datosVenta.tipoTransaccion));
                    strcpy(datosVenta.tipoTransaccion, "02");
                    cantidadBytes = 1;
                    iRet = atoi(estadoTj);
                }

                if (resultadoIsoPackMessage.responseCode > 0 || isTarjeta == 1) {

                    if (isTarjeta == 0) {
                        LOGI("Enviar anulacion ");
                        cantidadBytes = enviarAnulacion(resultadoIsoPackMessage, dataRecibida);
                    }
                    if (cantidadBytes > 0 || isTarjeta == 1) {
                        //CAMBIAR ESTADO EN EL JOURNAL

                        if (isTarjeta == 0) {
                            iRet = desempaquetarAnulacion(&datosVentaEnvio, dataRecibida,
                                                          cantidadBytes);
                        }
                        if (iRet == 1) {
                            if (atoi(datosVenta.tipoTransaccion) == TRANSACCION_ACUMULAR_LEALTAD
                                || atoi(datosVenta.tipoTransaccion) ==
                                   TRANSACCION_REDENCION_LIFEMILES) {
                                sprintf(datosVenta.tipoTransaccion, "%d",
                                        TRANSACCION_AJUSTAR_LEALTAD);
                            }
                            cambiarEstadoTransaccion(datosVenta.numeroRecibo, datosVentaEnvio.rrn,
                                                     datosVentaEnvio.codigoAprobacion,
                                                     datosVentaEnvio.txnDate,
                                                     datosVentaEnvio.txnTime);

                            memset(datosVenta.estadoTransaccion, 0x00,
                                   sizeof(datosVenta.estadoTransaccion));
                            strcpy(datosVenta.estadoTransaccion, "0");

                            _guardarDirectorioJournals_(TRANS_COMERCIO, datosVenta.numeroRecibo,
                                                        datosVenta.estadoTransaccion);
                            memcpy(datosVenta.codigoRespuesta, datosVentaEnvio.codigoRespuesta, 2);
                            memcpy(datosVenta.codigoAprobacion, datosVentaEnvio.codigoAprobacion,
                                   TAM_COD_APROBACION);
                            memcpy(datosVenta.rrn, datosVentaEnvio.rrn, TAM_RRN);

                            if (strcmp(datosVenta.estadoTarifa, "DC") == 0) {
                                do {
                                    memset(issuerCountryCode, 0x00, sizeof(issuerCountryCode));
                                    memset(reciboDCC, 0x00, sizeof(reciboDCC));
                                    memset(&campo61Eco, 0x00, sizeof(campo61Eco));
                                    memset(bufferAux, 0x00, sizeof(bufferAux));
                                    buscarArchivo(discoNetcom, (char *)JOURNAL_DCC, bufferAux, indice,
                                                  sizeof(campo61Eco));
                                    memcpy(reciboDCC, bufferAux, sizeof(reciboDCC));
                                    if (strncmp(datosVenta.numeroRecibo, reciboDCC,
                                                TAM_NUMERO_RECIBO) == 0) {
                                        posicion += TAM_NUMERO_RECIBO + 1;
                                        memcpy(campo61Eco.fielId6, bufferAux + posicion, TAM_61_6);
                                        posicion += TAM_61_6 + 1;
                                        memcpy(campo61Eco.fielId9, bufferAux + posicion, TAM_61_9);
                                        posicion += TAM_61_9 + 1;
                                        memcpy(campo61Eco.fielId10, bufferAux + posicion,
                                               TAM_61_10);
                                        posicion += TAM_61_10 + 1;
                                        memcpy(campo61Eco.fielId14, VOID_TRANSACTION, TAM_61_14);
                                        posicion += TAM_61_14 + 1;
                                        memcpy(campo61Eco.fielId20, bufferAux + posicion,
                                               TAM_61_20);
                                        posicion += TAM_61_20 + 1;
                                        memcpy(campo61Eco.fielId21, bufferAux + posicion,
                                               TAM_61_21);
                                        posicion += TAM_61_21 + 1;
                                        memcpy(issuerCountryCode, bufferAux + posicion,
                                               sizeof(issuerCountryCode));

                                        borrarArchivo(discoNetcom, (char *)ARCHIVO_DCC_LOGGIN);
                                        //escribirArchivoDccLoggin(datosVenta, issuerCountryCode, campo61Eco);
                                        resultado = 2;
                                    }
                                    indice += sizeof(campo61Eco);

                                } while (resultado == 0);
                            }

                            borrarArchivo(discoNetcom, (char *)ARCHIVO_REVERSO);

                            if (verificarModoJMR() == 1 && isTarjeta == 0) {
                                setParameter(USO_CAJA_REGISTRADORA, (char *)"1");
                                //transaccionRespuestaDatosCaja(datosVenta, TRANSACCION_TEFF_COMPRAS);
                            }

                            //SE TOMA EL CODIGO DE RESPUESTA PARA CAJAS
                            memset(datosVenta.codigoRespuesta, 0x00,
                                   sizeof(datosVenta.codigoRespuesta));
                            memcpy(datosVenta.codigoRespuesta, datosVentaEnvio.codigoRespuesta, 2);

                            memset(datosVenta.txnDate, 0x00, sizeof(datosVenta.txnDate));
                            strcpy(datosVenta.txnDate, datosVentaEnvio.txnDate);

                            memset(datosVenta.txnTime, 0x00, sizeof(datosVenta.txnTime));
                            strcpy(datosVenta.txnTime, datosVentaEnvio.txnTime);

                            memcpy(auxiliarNumeroRecibo, datosVenta.numeroRecibo,
                                   sizeof(datosVenta.numeroRecibo));
                            //setParameter(NUMERO_ULTIMA_ANULADA,auxiliarNumeroRecibo);
                            resultadoCaja = verificarHabilitacionCaja();

                            if (resultadoCaja == TRUE) {
                                getParameter(IMPRESION_RECIBO_CAJA, impresionRecibo);
                                iRet = strncmp(impresionRecibo, "01", 2);
                            } else {
                                iRet = 0;
                            }

                            if (iRet == 0) {


                                if (atoi(datosVenta.isQPS)
                                    == 1 && atoi(datosVenta.tipoTransaccion) !=
                                            TRANSACCION_BIG_BANCOLOMBIA) {
                                    imprimirTicketVenta(datosVenta, RECIBO_QPS, 6);
                                } else if (atoi(datosVenta.tipoTransaccion) ==
                                           TRANSACCION_BIG_BANCOLOMBIA) {
                                    //netcomServices(APP_TYPE_LEALTAD, IMPRIMIR_BIG_BANCOLOMBIA, sizeof(datosVenta),(char *) &datosVenta);
                                } else {
                                    if (datosVenta.estadoTarifa[0] == 'T') {
                                        //imprimirTicketDescuentos(datosVenta, RECIBO_COMERCIO, 6);
                                    } else if (atoi(datosVenta.tipoTransaccion) ==
                                               TRANSACCION_EFECTIVO) {
                                        //imprimirTicketRetiro(datosVenta, RECIBO_COMERCIO, 6);
                                    } else if (atoi(datosVenta.tipoTransaccion) ==
                                               TRANSACCION_PAGO_IMPUESTOS) {
                                        //imprimirTicketRetiro(datosVenta, RECIBO_COMERCIO, 6);
                                    } else {
                                        imprimirTicketVenta(datosVenta, RECIBO_COMERCIO, 6);
                                    }

                                    // iRet = getchAcceptCancel("RECIBO", "IMPRIMIR", "SEGUNDO", "RECIBO ?",30 * 1000);

                                    if (iRet != __BACK_KEY) {
                                        if (datosVenta.estadoTarifa[0] == 'T') {
                                            //imprimirTicketDescuentos(datosVenta, RECIBO_CLIENTE, 6);
                                        } else if (atoi(datosVenta.tipoTransaccion) ==
                                                   TRANSACCION_EFECTIVO) {
                                            // imprimirTicketRetiro(datosVenta, RECIBO_CLIENTE, 6);
                                        } else if (atoi(datosVenta.tipoTransaccion) ==
                                                   TRANSACCION_PAGO_IMPUESTOS) {
                                            // imprimirTicketRetiro(datosVenta, RECIBO_COMERCIO, 6);
                                        } else {
                                            //_imprimirTicketVenta_(datosVenta, RECIBO_CLIENTE, 6);
                                        }
                                    }
                                    //Para realizar login de anulacion DCC
                                    if (strcmp(datosVenta.estadoTarifa, "DC") == 0) {

                                        // realizarLogginDcc();
                                    }
                                }
                                setParameter(IMPRIMIR_PANEL_FIRMA, (char *)"0");

                            }

                            if (verificarHabilitacionCaja() == TRUE && cajaActiva == TRUE) {
                                cajaActiva = FALSE;
                                responderAnulacionCaja(datosVenta);
                            }

                        } else if (iRet == 2) {

                            numIntentos++;

                            if (numIntentos == MAX_INTENTOS_VENTA) {
                                //screenMessage("COMUNICACION", "REINTENTE", "TRANSACCION", "", 2 * 1000);
                                iRet = 0;
                            }
                        }
                    } else {
                        numIntentos++;
                    }
                }
            } while (iRet == 2 && numIntentos < MAX_INTENTOS_VENTA);

        }

    } else {
        anularTransaccionOffLine(datosVenta);
    }

    if (verificarHabilitacionCaja() == TRUE && cajaActiva == TRUE) {
        if (atoi(datosVenta.posEntrymode) != 911 && (atoi(datosVenta.posEntrymode) != 51)) {
            responderAnulacionCaja(datosVenta);
        }
    }
    releaseSocket();
}

ResultISOPack empaquetarAnulacion(DatosVenta *datosVenta, int intentos) {

    char field[36 + 1];
    char field48[36 + 1];
    char field54[36 + 1];
    char variableAux[36];
    char codigoProceso[6 + 1];
    char auxiliarValores[12 + 1];
    char validarValores[12 + 1];
    char campo42Auxiliar[TAM_FIELD_42 + 1];

    ISOHeader isoHeader8583;

    ResultISOPack resultadoIsoPackMessage;

    memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
    memset(&isoHeader8583, 0x00, sizeof(isoHeader8583));
    memset(field48, 0x00, sizeof(field48));
    memset(field54, 0x00, sizeof(field54));
    memset(variableAux, 0x00, sizeof(variableAux));
    memset(codigoProceso, 0x00, sizeof(codigoProceso));
    memset(field, 0x00, sizeof(field));
    memset(campo42Auxiliar, 0x00, sizeof(campo42Auxiliar));

    if (intentos == 0) {
        memset(systemTrace, 0x00, sizeof(systemTrace));
        generarSystemTraceNumber(systemTrace, (char *)"");

    }

    memset(validarValores, 0x00, sizeof(validarValores));
    memset(validarValores, 0x30, strlen(datosVenta->iva));

    if (strncmp(validarValores, datosVenta->iva, strlen(datosVenta->iva)) != 0) {
        memset(auxiliarValores, 0x00, sizeof(auxiliarValores));
        leftPad(auxiliarValores, datosVenta->iva, 0x30, TAM_IVA);
    } else {
        memset(auxiliarValores, 0x30, 12);
    }
    sprintf(field, "%s", auxiliarValores);

    memset(validarValores, 0x00, sizeof(validarValores));
    memset(validarValores, 0x30, strlen(datosVenta->baseDevolucion));

    if (strncmp(validarValores, datosVenta->baseDevolucion, strlen(datosVenta->baseDevolucion)) !=
        0) { //strlen(globalTramaVenta.baseDevolucion) > 0
        memset(auxiliarValores, 0x00, sizeof(auxiliarValores));
        leftPad(auxiliarValores, datosVenta->baseDevolucion, 0x30, TAM_IVA);
    } else {
        memset(auxiliarValores, 0x30, 12);
    }
    sprintf(field + 12, "%s", auxiliarValores);

    memset(validarValores, 0x00, sizeof(validarValores));
    memset(validarValores, 0x30, strlen(datosVenta->inc));

    if (strncmp(validarValores, datosVenta->inc, strlen(datosVenta->inc)) !=
        0) {
        memset(auxiliarValores, 0x00, sizeof(auxiliarValores));
        leftPad(auxiliarValores, datosVenta->inc, 0x30, TAM_IVA);
        LOGI("auxiliarValores A %s ", auxiliarValores);
    } else {
        memset(auxiliarValores, 0x30, 12);
        LOGI("auxiliarValores  B %s ", auxiliarValores);
    }

    sprintf(field + 24, "%s", auxiliarValores);
    LOGI("field %s ", field);
    uChar nii[TAM_NII + 1];
    memset(nii, 0x00, sizeof(nii));

    if (IS_MULTIPOS(atoi(datosVenta->tipoTransaccion)) == 1) {
        getParameter(NII_MULTIPOS, nii);
    } else {
        getParameter(NII, nii);
    }

    isoHeader8583.TPDU = 60;
    memcpy(isoHeader8583.destination, nii, TAM_NII + 1);
    memcpy(isoHeader8583.source, nii, TAM_NII + 1);
    setHeader(isoHeader8583);

    memset(variableAux, 0x00, sizeof(variableAux));
    leftPad(variableAux, datosVenta->rrn, 0x30, 12);

    if (atoi(datosVenta->tipoTransaccion) == TRANSACCION_RECARGAR_BONO) {
        memcpy(codigoProceso, "203000", TAM_CODIGO_PROCESO);
    } else if (IS_MULTIPOS(atoi(datosVenta->tipoTransaccion)) == 1) {
        strcpy(codigoProceso, "05");
        memcpy(codigoProceso + 2, datosVenta->processingCode + 2, 4);
    } else if (atoi(datosVenta->tipoTransaccion) == TRANSACCION_EFECTIVO) {
        strcpy(codigoProceso, "45");
        memcpy(codigoProceso + 2, datosVenta->processingCode + 2, 4);
    } else if (atoi(datosVenta->tipoTransaccion) == TRANSACCION_PAGO_IMPUESTOS) {
        strcpy(codigoProceso, "47");
        memcpy(codigoProceso + 2, datosVenta->processingCode + 2, 4);
    } else if (atoi(datosVenta->tipoTransaccion) == TRANSACCION_ACUMULAR_LEALTAD) {
        strcpy(codigoProceso, "44");
        memcpy(codigoProceso + 2, datosVenta->processingCode + 2, 4);
    } else {
        strcpy(codigoProceso, "02");
        memcpy(codigoProceso + 2, datosVenta->processingCode + 2, 4);
    }

    setMTI((char *)"0200");
    setField(3, codigoProceso, 6); //SEGUN TIPO DE CUENTA
    setField(4, datosVenta->totalVenta, 12); // MONTO JOURNAL
    setField(11, systemTrace, 6); // SYSTEM TRACE

    if (IS_MULTIPOS(atoi(datosVenta->tipoTransaccion)) == 1) {
        setField(12, datosVenta->txnTime, 6);
        setField(13, datosVenta->txnDate, 4);
    }

    setField(22, datosVenta->posEntrymode, 3);
    setField(24, nii + 1, 3);
    setField(25, (char *)"00", 2);
    setField(35, datosVenta->track2, strlen(datosVenta->track2));
    setField(37, variableAux, 12);
    setField(38, datosVenta->codigoAprobacion, 6);
    setField(41, datosVenta->terminalId, 8);

    if (datosVenta->fiel42[0] != 0x00) {
        if (atoi(datosVenta->tipoTransaccion) == TRANSACCION_BIG_BANCOLOMBIA) {
            setField(42, datosVenta->fiel42, 15);
            setField(60, datosVenta->field57, 22);
            setField(63, datosVenta->field61, 16);
        } else if (atoi(datosVenta->tipoTransaccion) == TRANSACCION_ACUMULAR_LEALTAD
                   || atoi(datosVenta->tipoTransaccion) == TRANSACCION_REDENCION_LIFEMILES) {
            setField(42, datosVenta->fiel42, strlen(datosVenta->fiel42));
            setField(63, datosVenta->textoAdicional, 25);
        } else {
            memcpy(campo42Auxiliar, datosVenta->fiel42, 2);
            memcpy(campo42Auxiliar + 2, "0000000000000", 13);
            setField(42, campo42Auxiliar, 15);
        }

    }

    if (IS_MULTIPOS(atoi(datosVenta->tipoTransaccion)) == 1) {

        setField(47, field, 36);
        setField(48, datosVenta->codigoComercio, TAM_ID_COMERCIO);

        if (datosVenta->propina[0] != 0x00) {
            memset(field54, 0x00, sizeof(field54));
            leftPad(field54, datosVenta->propina, 0x30, 12);
            setField(54, field54, strlen(field54));
        }
    } else {
        _convertASCIIToBCD_(field48, field, 36);
        setField(48, field48, 18); //BASE IVA //48

        if (datosVenta->propina[0] != 0x00) {
            memset(variableAux, 0x00, sizeof(variableAux));
            leftPad(variableAux, datosVenta->propina, 0x30, TAM_COMPRA_NETA);
            setField(57, variableAux, TAM_COMPRA_NETA);
        }
    }

    ////// TOKEN PARA DCC //////
    if (strcmp(datosVenta->estadoTarifa, "DC") == 0) {
        setField(47, datosVenta->tokenVivamos, 44);
        setField(60, datosVenta->tokenVivamos, 44);
    } else if (strcmp(datosVenta->estadoTarifa, "DP") == 0) {
        setField(47, datosVenta->tokenVivamos, 39);
        setField(60, datosVenta->tokenVivamos, 39);
    }

    setField(62, datosVenta->numeroRecibo, 6); //numero de recibo a anular

    ////// TOKEN VIVAMOS //////
    if (datosVenta->estadoTarifa[0] == 'T') {
        setField(60, datosVenta->tokenVivamos, 56);
    }

    resultadoIsoPackMessage = packISOMessage();

    memset(datosVenta->processingCode, 0x00, sizeof(datosVenta->processingCode));
    memset(datosVenta->systemTrace, 0x00, sizeof(datosVenta->systemTrace));
    strcpy(datosVenta->processingCode, codigoProceso);
    strcpy(datosVenta->systemTrace, systemTrace);

    return resultadoIsoPackMessage;

}

int desempaquetarAnulacion(DatosVenta *datosVenta, char *inputData, int cantidadBytes) {

    ResultISOPack resultadoIsoPackMessage;
    ResultISOUnpack resultadoUnpack;
    ISOFieldMessage isoFieldMessage;
    int resultado = 0;

    memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
    memset(&resultadoUnpack, 0x00, sizeof(resultadoUnpack));
    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));

    resultadoUnpack = unpackISOMessage(inputData, cantidadBytes);
    isoFieldMessage = getField(39);
    ///se toma el codigo de respuesta para caja registradoras
    memcpy(datosVenta->codigoRespuesta, isoFieldMessage.valueField, isoFieldMessage.totalBytes);
    if (strcmp(isoFieldMessage.valueField, "00") == 0) {
        if (validarRespuesta(datosVenta->processingCode, datosVenta->totalVenta,
                             datosVenta->systemTrace,
                             datosVenta->terminalId, inputData, cantidadBytes) == 1) {
            //borrarArchivo(DISCO_NETCOM,ARCHIVO_REVERSO);
            //screenMessage("","TRANSACCION","EXITOSA","**********",2*1000);
            memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
            isoFieldMessage = getField(38);
            memset(datosVenta->codigoAprobacion, 0x00, sizeof(datosVenta->codigoAprobacion));
            memcpy(datosVenta->codigoAprobacion, isoFieldMessage.valueField,
                   isoFieldMessage.totalBytes);

            memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
            isoFieldMessage = getField(37);
            memset(datosVenta->rrn, 0x00, sizeof(datosVenta->rrn));
            memcpy(datosVenta->rrn, isoFieldMessage.valueField + 6, 6);

            memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));

            isoFieldMessage = getField(12);
            memset(datosVenta->txnTime, 0x00, sizeof(datosVenta->txnTime));
            memcpy(datosVenta->txnTime, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

            isoFieldMessage = getField(13);
            memset(datosVenta->txnDate, 0x00, sizeof(datosVenta->txnDate));
            memcpy(datosVenta->txnDate, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

            resultado = 1;
        } else {

            resultado = generarReverso();
            if (resultado == 1) {
                resultado = 2; // Volver a enviar la anulacion
            }
        }

    } else {
        borrarArchivo(discoNetcom, (char *)ARCHIVO_REVERSO);
        errorRespuestaTransaccion(isoFieldMessage.valueField, (char *)"");

        imprimirTicketDeclina(*datosVenta);

    }

    return resultado;

}

int enviarAnulacion(ResultISOPack resultadoIsoPackMessage, char *dataRecibida) {

    int cantidadBytes = 0;

    cantidadBytes = realizarTransaccion(resultadoIsoPackMessage.isoPackMessage,
                                        resultadoIsoPackMessage.totalBytes,
                                        dataRecibida, TRANSACCION_ANULACION, CANAL_DEMANDA,
                                        REVERSO_GENERADO);

    return cantidadBytes;
}

int validarRespuesta(char *p3Inicial, char *p4Inicial, char *p11Inicial, char *p41Inicial,
                     char *inputData,
                     int longitud) {

    ISOFieldMessage isoFieldMessage;
    int resultado = 1;

    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));

    isoFieldMessage = getField(3);
    if (strcmp(p3Inicial, isoFieldMessage.valueField) != 0) {
        resultado = -1;
    }

    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    isoFieldMessage = getField(4);
    if (atol(p4Inicial) != atol(isoFieldMessage.valueField)) {
        resultado = -1;
    }

    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    isoFieldMessage = getField(11);
    if (strcmp(p11Inicial, isoFieldMessage.valueField) != 0) {
        resultado = -1;
    }

    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    isoFieldMessage = getField(41);
    if (strcmp(p41Inicial, isoFieldMessage.valueField) != 0) {
        resultado = -1;
    }

//	resultado = 1;
    return resultado;
}

void
cambiarEstadoTransaccion(char *numeroRecibo, char *rrn, char *aprobacion, char *fecha, char *hora) {

    int resultado = 0;
    int posicion = 0;
    char dataDuplicado[TAM_JOURNAL + 1];

    DatosVenta datosVenta;
    do {
        memset(dataDuplicado, 0x00, sizeof(dataDuplicado));
        memset(&datosVenta, 0x00, sizeof(datosVenta));
        resultado = buscarArchivo(discoNetcom, (char *)JOURNAL, dataDuplicado, posicion,
                                  sizeof(DatosVenta));
        datosVenta = obtenerDatosVenta(dataDuplicado);

        if (strncmp(datosVenta.numeroRecibo, numeroRecibo, TAM_NUMERO_RECIBO) == 0) {
            //se cambia el estado
            posicion = posicion + (TAM_TIPO_TRANSACCION + 1);
            actualizarArchivo(discoNetcom, (char *)JOURNAL, (char *)"0", posicion, 1);

            //se cambia el rrn
            posicion += (TAM_ESTADO_TRANSACCION + 1);
            posicion += (TAM_TIPO_CUENTA + 1);
            posicion += (TAM_NUMERO_RECIBO + 1);
            posicion += (TAM_SYSTEM_TRACE + 1);
            posicion += (TAM_MTI + 1);
            posicion += (TAM_CODIGO_PROCESO + 1);
            posicion += (TAM_COMPRA_NETA + 1);
            posicion += (TAM_IVA + 1);
            posicion += (TAM_COMPRA_NETA + 1);

            actualizarArchivo(discoNetcom, (char *)JOURNAL, hora, posicion, strlen(hora));
            posicion += (TAM_TIME + 1);
            actualizarArchivo(discoNetcom, (char *)JOURNAL, fecha, posicion, strlen(fecha));
            posicion += (TAM_DATE + 1);
            posicion += (TAM_ENTRY_MODE + 1);
            posicion += (TAM_NII + 1);
            posicion += (TAM_POS_CONDITION + 1);
            posicion += (TAM_TRACK2 + 1);
            posicion += (TAM_TERMINAL_ID + 1);
            posicion += (TAM_ACQIRER_ID + 1);
            posicion += (TAM_FIELD_57 + 1);
            posicion += (TAM_FIELD_61 + 1);
            actualizarArchivo(discoNetcom, (char *)JOURNAL, rrn, posicion, strlen(rrn));
            //se cambia el codigo aprobacion
            posicion += (TAM_RRN + 1);
            posicion += (TAM_CODIGO_PROCESO + 1);
            posicion += (TAM_CARD_NAME + 1);
            actualizarArchivo(discoNetcom, (char *)JOURNAL, aprobacion, posicion, strlen(aprobacion));

            resultado = 1;
        }

        posicion += sizeof(DatosVenta);
    } while (resultado == 0);


}

void anularTransaccionOffLine(DatosVenta datosVenta) {

    int iRet = 0;


    if (strncmp(datosVenta.estadoTransaccion, "1", 1) == 0) {
        cambiarEstadoTransaccion(datosVenta.numeroRecibo, datosVenta.rrn,
                                 datosVenta.codigoAprobacion,
                                 datosVenta.txnDate, datosVenta.txnTime);
        memset(datosVenta.estadoTransaccion, 0x00, sizeof(datosVenta.estadoTransaccion));
        strcpy(datosVenta.estadoTransaccion, "0");
        //screenMessage("ANULACION", "", "APROBADA", "OFFLINE", 2 * 1000);

        //imprimirTicketVenta(datosVenta, RECIBO_COMERCIO, 6);//SI VA ORIGINAL ANDROID
        // iRet = getchAcceptCancel("RECIBO", "IMPRIMIR", "SEGUNDO", "RECIBO ?", 30 * 1000);
        if (iRet != __BACK_KEY) {
            //imprimirTicketVenta(datosVenta, RECIBO_CLIENTE, 6);//SI VA ORIGINAL ANDROID
        }

    } else {
        //screenMessage("ANULACION", "RECIBO YA ANULADO", "CANCELANDO", "LA TRANSACCION", 2 * 1000);
    }

}

int optenerPosicionTransaccion(char *numeroRecibo, char *datosVentaPosicion) {

    int resultado = 0;
    int respuesta = -1;
    int posicion = 0;
    char dataDuplicado[TAM_JOURNAL + 1];
    DatosVenta datosVenta;

    do {
        memset(dataDuplicado, 0x00, sizeof(dataDuplicado));
        memset(&datosVenta, 0x00, sizeof(datosVenta));
        resultado = buscarArchivo(discoNetcom, (char *)JOURNAL, dataDuplicado, posicion,
                                  sizeof(DatosVenta));
        datosVenta = obtenerDatosVenta(dataDuplicado);

        if (strncmp(datosVenta.numeroRecibo, numeroRecibo, TAM_NUMERO_RECIBO) == 0) {
            respuesta = posicion;
            resultado = 1;
        }

        posicion += sizeof(DatosVenta);
    } while (resultado == 0);

    memcpy(datosVentaPosicion, dataDuplicado, sizeof(DatosVenta));

    return respuesta;
}

int verificarUltimoRecibo(DatosVenta *ultimaVenta) {

    DatosVenta datosVenta;
    int tamanoJournal = 0;
    int respuesta = 0;
    int reciboUltimo = 0;
    int reciboAnterior = 0;
    char dataDuplicado[TAM_JOURNAL + 1];

    memset(&datosVenta, 0x00, sizeof(datosVenta));
    memset(dataDuplicado, 0x00, sizeof(dataDuplicado));
    tamanoJournal = tamArchivo(discoNetcom, (char *)JOURNAL);
    if (tamanoJournal > 0) {

        tamanoJournal -= (2 * sizeof(DatosVenta));
        buscarArchivo(discoNetcom, (char *)JOURNAL, dataDuplicado, tamanoJournal, sizeof(DatosVenta));
        datosVenta = obtenerDatosVenta(dataDuplicado);
    }

    reciboUltimo = atoi(ultimaVenta->numeroRecibo);
    reciboAnterior = atoi(datosVenta.numeroRecibo);

    if (reciboUltimo > reciboAnterior) {
        respuesta = 1;
    } else {
        memcpy(ultimaVenta, &datosVenta, sizeof(DatosVenta));
    }

    return respuesta;
}

void imprimirTicketDeclina(DatosVenta datosVenta) {

    DatosTransaccionDeclinada datosTransaccionDeclinada;
    int imprimir = 0;
    char transaccion[21 + 1];

    memset(transaccion, 0x00, sizeof(transaccion));

    switch (atoi(datosVenta.tipoTransaccion)) {
        case TRANSACCION_EFECTIVO_RETIRO:
        case TRANSACCION_EFECTIVO_DEPOSITO:
        case TRANSACCION_EFECTIVO:
            imprimir = 1;
            strcpy(transaccion, "EFECTIVO");
            break;
        default:
            break;
    }

    if (imprimir == 1) {

        memset(&datosTransaccionDeclinada, 0x00, sizeof(datosTransaccionDeclinada));
        strcpy(datosTransaccionDeclinada.textoTransaccion, transaccion);
        strcpy(datosTransaccionDeclinada.cardName, datosVenta.cardName);
        strcpy(datosTransaccionDeclinada.tipoCuenta, datosVenta.tipoCuenta);
        strcpy(datosTransaccionDeclinada.cuotas, datosVenta.cuotas);
        strcpy(datosTransaccionDeclinada.tarjetaHabiente, datosVenta.tarjetaHabiente);
        strcpy(datosTransaccionDeclinada.fechaExpiracion, datosVenta.fechaExpiracion);
        //getMensajeError(datosVenta.codigoRespuesta, datosTransaccionDeclinada.mensajeError);
        //imprimirTransaccionDeclinada(datosTransaccionDeclinada);
    }

}

void responderAnulacionCaja(DatosVenta datos) {

    if (atoi(datos.codigoRespuesta) != ERROR_NO_RESPONDE &&
        atoi(datos.codigoRespuesta) != ERROR_RESPONDE_INCORRECTO
        && atoi(datos.codigoRespuesta) != ERROR_CONFIGURACION) {
        //transaccionRespuestaDatosCaja(datos, TRANSACCION_TEFF_ANULACIONES);
    }
}

int verificarModoJMR(void) {

    uChar modoBancolombia[1 + 1];
    int respuesta = 0;

    memset(modoBancolombia, 0x00, sizeof(modoBancolombia));

    getParameter(HABILITACION_MODO_CNB_BCL, modoBancolombia);

    if (strcmp(modoBancolombia, BCL_JMR) == 0 && verificarHabilitacionCaja() == TRUE) {
        respuesta = 1;
    }
    return respuesta;
}

int menuSeleccionAnulacion(int tipoTransaccion) {

    int iRet = 0;
    uChar numeroRecibo[TAM_NUMERO_RECIBO + 1];
    uChar numeroReciboAux[TAM_NUMERO_RECIBO + 1];
    DatosJournals datosJournals;

    memset(numeroRecibo, 0x00, sizeof(numeroRecibo));
    memset(numeroReciboAux, 0x00, sizeof(numeroReciboAux));
    memset(&datosJournals, 0x00, sizeof(datosJournals));

    //iRet = showMenu("ANULACION", 0, 0, 2, menuAnulacion, 30 * 1000);

    if (iRet > 0) {

        if (iRet == 2) {
            do {
                //iRet = getStringKeyboard("ANULACION", "INGRESE", "NUMERO RECIBO", 6, numeroReciboAux, DATO_NUMERICO);

                if (iRet > 0 && strlen(numeroReciboAux) == 0) {
                    //screenMessage("ANULACION", "", "DIGITE UN RECIBO", "PARA ANULAR", 2 * 1000);
                }

            } while (iRet > 0 && strlen(numeroReciboAux) == 0);

            leftPad(numeroRecibo, numeroReciboAux, 0x30, 6);
        }

        if (iRet > 0) {
            _anulacionNumeroRecibo_(numeroRecibo);
        }
    }
    return iRet;
}

int realizarConsultaReciboAnulacion(char *numeroRecibo) {

    int iRet = 1;
    int verificar = 0;
    int cajaAnulacion = 0;
    int tamJournal = TAM_DIR_JOURNALS * 2;
    long tamDirJournal = 0;

    char numeroReciboAux[6 + 1] = {0x00};
    char monto[12 + 1] = {0x00};
    char monto2[12 + 1] = {0x00};
    char variableAux[20 + 1] = {0x00};
    char variableAux2[20 + 1] = {0x00};
    char numeroCelular[10 + 1] = {0x00};
    char lineaResumen[100 + 1] = {0x00};


    DatosVenta datosVenta;
    DatosVenta datosReverso;
    DatosJournals datosJournals;
    TablaTresInicializacion dataIssuer;


    memset(&globalTramaVenta, 0x00, sizeof(globalTramaVenta));
    memset(&datosVenta, 0x00, sizeof(datosVenta));
    memset(&dataIssuer, 0x00, sizeof(dataIssuer));
    memset(&datosJournals, 0x00, sizeof(datosJournals));
    memset(&datosReverso, 0x00, sizeof(datosReverso));

    if (numeroRecibo[0] == 0x00) {
        tamDirJournal = tamArchivo(discoNetcom, (char *)DIRECTORIO_JOURNALS);
        iRet = buscarArchivo(discoNetcom, (char *)DIRECTORIO_JOURNALS, (char *) &datosJournals,
                             tamDirJournal - TAM_DIR_JOURNALS, tamJournal);
        memcpy(numeroRecibo, datosJournals.numeroRecibo, TAM_NUMERO_RECIBO);
        leftPad(numeroReciboAux, numeroRecibo, '0', 6);
    } else {
        leftPad(numeroReciboAux, numeroRecibo, '0', 6);
    }
    LOGI("Numero de recibo  anulacion  %s", numeroReciboAux);

    datosJournals = buscarReciboMulticorresponsal(numeroReciboAux);
    LOGI("datosJournals.journals  %d", datosJournals.journal);
    if (datosJournals.journal == TRANS_MULT_BANCOLOMBIA ||
        datosJournals.journal == TRANS_MULT_AVAL) {
        screenMessageFiveLine((char *)"MENSAJE", (char *)"TRANSACCION", (char *)"NO TIENE",(char *)"ANULACION",(char *)"",(char *)"",lineaResumen);
        iRet = 0;
    } else if (datosJournals.journal == TRANS_CNB_BANCOLOMBIA) {
        //netcomServices(APP_TYPE_CNB_BANCOLOMBIA, ANULACION_RECIBO_BANCOLOMBIA, TAM_NUMERO_RECIBO, numeroRecibo);
        cajaActiva = FALSE;
        //screenMessage("MENSAJE", "TRANSACCION", "NO PERMITIDA", "", 2 * 1000);
        iRet = 0;
    } else if (datosJournals.journal == TRANS_CNB_CORRESPONSALES) {
        //screenMessage("MENSAJE", "TRANSACCION", "NO PERMITIDA", "", 2 * 1000);
        iRet = 0;
    } else if (strlen(datosJournals.numeroRecibo) <= 0 && iRet > 0) {
        LOGI("deberia entrar por aqui %s", numeroRecibo);
        if (strcmp(numeroRecibo, "ERRADA") != 0) {
            LOGI("deberia entrar por aqui tambien %d", datosJournals.journal);
            screenMessageFiveLine((char *)"ANULACION ", (char *)" NO EXISTE ", (char *)" RECIBO ", (char *)"",(char *)"",(char *)"",lineaResumen);
            enviarStringToJava(lineaResumen, (char *) "showScreenMessage");
            if (verificarHabilitacionCaja() == TRUE) {
                cajaAnulacion = FALSE;
                cajaActiva = TRUE;
            }
        }
        iRet = 0;
    }
    if(iRet > 0){
        buscarArchivo(discoNetcom, (char *)JOURNAL, (char *) &datosVenta, datosJournals.posicionTransaccion,
                      sizeof(DatosVenta));

        memcpy(datosVenta.codigoRespuesta, "01", 2); //se setea codigo de respuesta en 1 para caja

        if (atoi(datosVenta.tipoTransaccion) != 0) {

            if (atoi(datosVenta.tipoTransaccion) == TRANSACCION_VENTA
                || IS_MULTIPOS(atoi(datosVenta.tipoTransaccion))
                   == 1 || atoi(datosVenta.tipoTransaccion) == TRANSACCION_GASO_PASS
                || atoi(datosVenta.tipoTransaccion) == TRANSACCION_EFECTIVO
                || atoi(datosVenta.tipoTransaccion) == TRANSACCION_TARJETA_MI_COMPRA
                || (atoi(datosVenta.tipoTransaccion) == TRANSACCION_BIG_BANCOLOMBIA)
                || atoi(datosVenta.tipoTransaccion) == TRANSACCION_PAGO_IMPUESTOS
                || atoi(datosVenta.tipoTransaccion) == TRANSACCION_REDENCION_LIFEMILES
                || atoi(datosVenta.tipoTransaccion) == TRANSACCION_ACUMULAR_LEALTAD) {

                if (atoi(datosVenta.estadoTransaccion) == 1) {
                    /////// DATOS DE TARJETA - PERMITE COSTO TRANSACCION ?///////

                    if (strlen(datosVenta.codigoAprobacion) > 0) {
                        if (strlen(datosVenta.tipoTransaccion) > 0) {
                            memcpy(monto, datosVenta.totalVenta, strlen(datosVenta.totalVenta) - 2);
                            memcpy(numeroCelular, datosVenta.tarjetaHabiente + 41, 10);
                            formatString(0x00, 0, monto, strlen(monto), monto2, 2);
                            memset(variableAux, 0x00, sizeof(variableAux));
                            sprintf(variableAux, "RECIBO: %.6s", datosVenta.numeroRecibo);
                            sprintf(variableAux2, "VALOR:$%s", monto2);
                            memcpy(&globalTramaVenta, &datosVenta, sizeof(datosVenta));
                            screenMessageFiveLine((char *)"ANULACION", variableAux, variableAux2,
                                                  (char *)"Desea Continuar?", datosVenta.posEntrymode, (char *)"",
                                                  lineaResumen);
                            enviarStringToJava(lineaResumen, (char *)"showScreenMessage");
                        } else {
                            screenMessageFiveLine((char *)"ANULACION", (char *)"SIN TRANSACCIONES", (char *)"OPCION NO", (char *)"PERMITIDA",(char *)"",(char *)"",lineaResumen);
                        }
                    }
                } else {
                    screenMessageFiveLine((char *)"ANULACION", (char *)"RECIBO YA ANULADO", (char *)"CANCELANDO",
                                          (char *)"LA TRANSACCION", (char *)"", (char *)"", lineaResumen);
                    enviarStringToJava(lineaResumen, (char *)"showScreenMessage");
                }
            } else if (atoi(datosVenta.tipoTransaccion) == TRANSACCION_RECARGAR_BONO) {

                //otroReciboAnulacionBono(numeroRecibo);

                cajaAnulacion = FALSE;
                cajaActiva = FALSE;
                ///////////////////anulacion habilitacion////////////////////
            } else if (atoi(datosVenta.tipoTransaccion) == TRANSACCION_HABILITAR_BONO) {

                //otroReciboAnuHabilitacion(numeroRecibo);
                cajaAnulacion = FALSE;
                cajaActiva = FALSE;
                ///////////////////anulacion habilitacion////////////////////

            } else if (atoi(datosVenta.tipoTransaccion) == TRANSACCION_PAGO_ELECTRONICO
                       ||
                       atoi(datosVenta.tipoTransaccion) == TRANSACCION_PAGO_ELECTRONICO_ESPECIAL) {
                //anulacionDineroElectronico(datosVenta);
            } else if (atoi(datosVenta.tipoTransaccion) == TRANSACCION_CELUCOMPRA) {
                //netcomServices(APP_TYPE_CELUCOMPRA, ANULACION_CELUCOMPRA, sizeof(datosVenta), (char *) &datosVenta);
            } else if (atoi(datosVenta.tipoTransaccion) == TRANSACCION_CONSULTA_QR) {
                //netcomServices(APP_TYPE_QR, ANULACION_QR, sizeof(datosVenta), (char *) &datosVenta);
            } else if (atoi(datosVenta.tipoTransaccion) == TRANSACCION_PUNTOS_COLOMBIA) {

                memcpy(monto, datosVenta.totalVenta, strlen(datosVenta.totalVenta));
                formatString(0x00, 0, monto, strlen(monto), monto2, 2);
                memset(variableAux, 0x00, sizeof(variableAux));
                sprintf(variableAux, "RECIBO: %.6s", datosVenta.numeroRecibo);
                sprintf(variableAux2, "VALOR:$%s", monto2);
                //iRet = getchAcceptCancel("ANULACION", variableAux, variableAux2, "Desea Continuar?",20 * 1000);

                if (iRet > 0) {
                    verificar = verificarArchivo(discoNetcom, REVERSO_PUNTOS_COLOMBIA);

                    if (verificar == FS_OK) {
                        leerArchivo(discoNetcom, (char *)REVERSO_PUNTOS_COLOMBIA, (char *) &datosReverso);
                        //iRet = netcomServices(APP_TYPE_LEALTAD, ANULACION_PUNTOS_COLOMBIA, TAM_JOURNAL_COMERCIO,(char*) &datosReverso);
                    }
                    if (iRet > 0) {
                        // netcomServices(APP_TYPE_LEALTAD, ANULACION_PUNTOS_COLOMBIA, sizeof(datosVenta), (char *) &datosVenta);
                    }
                }

            } else {
                screenMessageFiveLine((char *)"ANULACION", (char *)"NO SOPORTADA",(char *)"CANCELANDO", (char *)"LA TRANSACCION",(char *)"",(char *)"",lineaResumen);
            }

        }
    }

    return iRet;
}

