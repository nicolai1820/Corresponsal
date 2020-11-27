//
// Created by NETCOM on 02/03/2020.
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

#define  LOG_TAG    "NETCOM_ANULACION_CNBBCL"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

void procesoAnularTransaccion(DatosCnbBancolombia datosVenta) {

    uChar stringAux[12 + 1];
    uChar monto[12 + 1];
    uChar linea1[24 + 1];
    uChar linea2[24 + 1];
    uChar montoAux[12 + 1];
//	int tipoCuplicado = 0;
    int iRet = 0;

    memset(monto, 0x00, sizeof(monto));
    memset(linea1, 0x00, sizeof(linea1));
    memset(linea2, 0x00, sizeof(linea2));
    memset(montoAux, 0x00, sizeof(montoAux));
    memset(stringAux, 0x00, sizeof(stringAux));

    iRet = atoi(datosVenta.estadoTransaccion) == TRANSACCION_ACTIVA ? 1 : 0;

    if (iRet > 0) {
        if ((atoi(datosVenta.tipoTransaccion) == TRANSACCION_BCL_RETIRO)
            || (atoi(datosVenta.tipoTransaccion) == TRANSACCION_BCL_RETIRO_EFECTIVO)
            || (atoi(datosVenta.tipoTransaccion) == TRANSACCION_BCL_SALDO)
            || (atoi(datosVenta.tipoTransaccion) == TRANSACCION_BCL_GIROS)
            || (atoi(datosVenta.tipoTransaccion) == TRANSACCION_BCL_RECLAMAR_GIROS)
            || (atoi(datosVenta.tipoTransaccion) == TRANSACCION_BCL_CANCELAR_GIROS)) {

            //screenMessage("MENSAJE", "NO SOPORTADA", "CANCELANDO", "LA TRANSACCION", 2 * 1000);

            if (verificarHabilitacionCaja() == TRUE /*&& verificarCajaIPCB() == 0*/) {
                memset(datosVentaBancolombia.codigoRespuesta, 0x00,
                       sizeof(datosVentaBancolombia.codigoRespuesta));
                memcpy(datosVentaBancolombia.codigoRespuesta, "01", 2);
                memcpy(datosVentaBancolombia.rrn, datosVenta.rrn, TAM_RRN);
                memcpy(datosVentaBancolombia.tipoTransaccion, datosVenta.tipoTransaccion,
                       TAM_TIPO_TRANSACCION);
                // transaccionRespuestaDatosCaja(datosVentaBancolombia, TRANSACCION_TEFF_ANULACION);
            }

            return;
        }

        if (atoi(datosVenta.tipoTransaccion) == TRANSACCION_BCL_DEPOSITO) {
            memcpy(datosVentaBancolombia.propina, datosVenta.cuentaDestino,
                   strlen(datosVenta.cuentaDestino));
        } else if (atoi(datosVenta.tipoTransaccion) == TRANSACCION_BCL_PAGO_CARTERA
                   || atoi(datosVenta.tipoTransaccion) == TRANSACCION_BCL_PAGO_TARJETA) {
            memcpy(datosVentaBancolombia.tokenVivamos, datosVenta.numeroReferencia,
                   strlen(datosVenta.numeroReferencia));
        } else if (atoi(datosVenta.tipoTransaccion) == TRANSACCION_BCL_RECAUDO) {
            memcpy(datosVentaBancolombia.codigoComercio, datosVenta.convenio,
                   strlen(datosVenta.convenio));

            memset(datosVentaBancolombia.tokenVivamos, 0x00,
                   sizeof(datosVentaBancolombia.tokenVivamos));
            leftTrim(datosVentaBancolombia.tokenVivamos, datosVenta.numeroReferencia, '0');
        }

//		tipoCuplicado = iRet; // Comercio o Cliente
        memcpy(stringAux, datosVenta.totalVenta, strlen(datosVenta.totalVenta) - 2);
        formatString(0x00, 0, stringAux, strlen(stringAux), monto, 2);
        sprintf(linea1, "RECIBO: %.6s", datosVenta.numeroRecibo);
        sprintf(linea2, "VALOR:$%s", monto);
        memset(datosVentaBancolombia.totalVenta, 0x00, sizeof(datosVentaBancolombia.totalVenta));
        memcpy(datosVentaBancolombia.totalVenta, datosVenta.totalVenta,
               sizeof(datosVenta.totalVenta));
        memcpy(montoAux, datosVentaBancolombia.totalVenta, 12);
        strcat(datosVentaBancolombia.totalVenta, "00");

        //iRet = getchAcceptCancel("ANULACION", linea1, linea2, "Desea Continuar?", 20 * 1000);
        if (iRet <= 0 && verificarHabilitacionCaja()) {

            memset(datosVentaBancolombia.codigoRespuesta, 0x00,
                   sizeof(datosVentaBancolombia.codigoRespuesta));
            memcpy(datosVentaBancolombia.codigoRespuesta, "01", 2);
            memcpy(datosVentaBancolombia.tipoTransaccion, datosVenta.tipoTransaccion,
                   TAM_TIPO_TRANSACCION);
            //transaccionRespuestaDatosCaja(datosVentaBancolombia, TRANSACCION_TEFF_ANULACION);

        }
    } else {
        // screenMessage("ANULACION", "RECIBO YA ANULADO", "CANCELANDO", "LA TRANSACCION", T_OUT(2));
    }

    if (iRet > 0) {
//        iRet = enviarAnulacion(datosVenta);
    }

    if (iRet > 0) {
        extraerCamposAprobacionAnulacion(&datosVenta);

        if (verificarHabilitacionCaja() == TRUE) {
            memset(datosVentaBancolombia.codigoRespuesta, 0x00,
                   sizeof(datosVentaBancolombia.codigoRespuesta));
            memset(datosVentaBancolombia.totalVenta, 0x00,
                   sizeof(datosVentaBancolombia.totalVenta));
            memcpy(datosVentaBancolombia.totalVenta, montoAux, 12);
            memcpy(datosVentaBancolombia.codigoRespuesta, "00", 2);
            memcpy(datosVentaBancolombia.rrn, datosVenta.rrn, TAM_RRN);
            memcpy(datosVentaBancolombia.tipoTransaccion, datosVenta.tipoTransaccion,
                   TAM_TIPO_TRANSACCION);

            /* if (verificarCajaIPCB() == 1) {
                 memset(datosVentaBancolombia.codigoAprobacion, 0x00, sizeof(datosVentaBancolombia.codigoAprobacion));
                 memset(datosVentaBancolombia.terminalId, 0x00, sizeof(datosVentaBancolombia.terminalId));
                 memset(datosVentaBancolombia.track2, 0x00, sizeof(datosVentaBancolombia.track2));
                 memset(datosVentaBancolombia.cardName, 0x00, sizeof(datosVentaBancolombia.cardName));
                 memcpy(datosVentaBancolombia.codigoAprobacion, datosVenta.codigoAprobacion, 6);
                 memcpy(datosVentaBancolombia.terminalId,datosVenta.terminalId, 8);
                 memcpy(datosVentaBancolombia.track2, datosVenta.track2, 16);
             }*/

            // iRet = transaccionRespuestaDatosCaja(datosVentaBancolombia, TRANSACCION_TEFF_ANULACION);


            if (iRet < 0 /*&& verificarCajaIPCB() == 1*/) {
                memcpy(datosVentaBancolombia.codigoRespuesta, "WS", 2);
                if (atoi(datosVentaBancolombia.tipoTransaccion) == TRANSACCION_BCL_DEPOSITO) {
                    memset(datosVentaBancolombia.totalVenta, 0x00,
                           sizeof(datosVentaBancolombia.totalVenta));
                    memcpy(datosVentaBancolombia.totalVenta, montoAux, strlen(montoAux) - 2);
                }
                imprimirDeclinadas(datosVentaBancolombia, (char *) "ERROR CON EL WS_AN");
            }
            strcat(datosVentaBancolombia.totalVenta, "00");
        }
        //control de cambios login de operador
        setParameter(USER_CNB_LOGON, "1");
        mostrarAprobacionAnulacion(&datosVenta);
    }
}

int realizarConsultaReciboAnulacionBanbolombia(char *numeroRecibo) {

    int iRet = 0;
    long lengthJournal = 0;
    char numeroReciboAux[6 + 1] = {0x00};
    char lineaResumen[100 + 1] = {0x00};
    uChar dataDuplicado[sizeof(DatosCnbBancolombia)];

    memset(&globalDatosBancolombia, 0x00, sizeof(globalDatosBancolombia));
    memset(dataDuplicado, 0x00, sizeof(dataDuplicado));

    lengthJournal = tamArchivo(discoNetcom, (char *) JOURNAL_CNB_BANCOLOMBIA);
    LOGI("lengthJournal  %d", lengthJournal);

    if (lengthJournal > 0) {
        if (numeroRecibo[0] == 0x00) {
            lengthJournal -= sizeof(DatosCnbBancolombia);
            iRet = buscarArchivo(discoNetcom, (char *) JOURNAL_CNB_BANCOLOMBIA, dataDuplicado,
                                 lengthJournal,
                                 sizeof(DatosCnbBancolombia));
            LOGI("iRet  %d", iRet);
            if (iRet >= 0) {
                memcpy(&globalDatosBancolombia, dataDuplicado, sizeof(dataDuplicado));
                LOGI("tipoDe producto anular   %s", globalDatosBancolombia.tipoProducto);
            }

        } else {
            leftPad(numeroReciboAux, numeroRecibo, '0', 6);
            iRet = buscarRecibo(numeroReciboAux, &globalDatosBancolombia, ORIGEN_ANULACION);
        }
        LOGI("numero recibo  %s", globalDatosBancolombia.numeroRecibo);
        if (atoi(globalDatosBancolombia.estadoTransaccion) == 1 && iRet >= 0) {
            uChar stringAux[12 + 1];
            uChar monto[12 + 1];
            uChar linea1[24 + 1];
            uChar linea2[24 + 1];
            uChar montoAux[12 + 1];

            memset(monto, 0x00, sizeof(monto));
            memset(linea1, 0x00, sizeof(linea1));
            memset(linea2, 0x00, sizeof(linea2));
            memset(montoAux, 0x00, sizeof(montoAux));
            memset(stringAux, 0x00, sizeof(stringAux));

            iRet = atoi(globalDatosBancolombia.estadoTransaccion) == TRANSACCION_ACTIVA ? 1 : 0;
            LOGI("numero trabsaccion  %s", globalDatosBancolombia.tipoTransaccion);
            if (iRet > 0) {
                if ((atoi(globalDatosBancolombia.tipoTransaccion) == TRANSACCION_BCL_RETIRO)
                    || (atoi(globalDatosBancolombia.tipoTransaccion) ==
                        TRANSACCION_BCL_RETIRO_EFECTIVO)
                    || (atoi(globalDatosBancolombia.tipoTransaccion) == TRANSACCION_BCL_SALDO)
                    || (atoi(globalDatosBancolombia.tipoTransaccion) == TRANSACCION_BCL_GIROS)
                    ||
                    (atoi(globalDatosBancolombia.tipoTransaccion) == TRANSACCION_BCL_RECLAMAR_GIROS)
                    || (atoi(globalDatosBancolombia.tipoTransaccion) ==
                        TRANSACCION_BCL_CANCELAR_GIROS)) {

                    screenMessageFiveLine((char *) " MENSAJE ", (char *) "  NO SOPORTADA ",
                                          (char *) " CANCELANDO ",
                                          (char *) " LA TRANSACCION ", (char *) "", (char *) "",
                                          lineaResumen);
                    enviarStringToJava(lineaResumen, (char *) "showScreenMessage");

/*                            if (verificarHabilitacionCaja() == TRUE && verificarCajaIPCB() == 0) {
                                memset(datosVentaBancolombia.codigoRespuesta, 0x00, sizeof(datosVentaBancolombia.codigoRespuesta));
                                memcpy(datosVentaBancolombia.codigoRespuesta, "01", 2);
                                memcpy(datosVentaBancolombia.rrn, datosVenta.rrn, TAM_RRN);
                                memcpy(datosVentaBancolombia.tipoTransaccion, datosVenta.tipoTransaccion, TAM_TIPO_TRANSACCION);
                                transaccionRespuestaDatosCaja(datosVentaBancolombia, TRANSACCION_TEFF_ANULACION);
                            }

                            return;*/

                    iRet = 0;
                    return iRet;
                }

                if (atoi(globalDatosBancolombia.tipoTransaccion) == TRANSACCION_BCL_DEPOSITO) {
                    memcpy(datosVentaBancolombia.propina, globalDatosBancolombia.cuentaDestino,
                           strlen(globalDatosBancolombia.cuentaDestino));
                } else if (
                        atoi(globalDatosBancolombia.tipoTransaccion) == TRANSACCION_BCL_PAGO_CARTERA
                        || atoi(globalDatosBancolombia.tipoTransaccion) ==
                           TRANSACCION_BCL_PAGO_TARJETA) {
                    memcpy(datosVentaBancolombia.tokenVivamos,
                           globalDatosBancolombia.numeroReferencia,
                           strlen(globalDatosBancolombia.numeroReferencia));
                } else if (atoi(globalDatosBancolombia.tipoTransaccion) ==
                           TRANSACCION_BCL_RECAUDO) {
                    memcpy(datosVentaBancolombia.codigoComercio, globalDatosBancolombia.convenio,
                           strlen(globalDatosBancolombia.convenio));

                    memset(datosVentaBancolombia.tokenVivamos, 0x00,
                           sizeof(datosVentaBancolombia.tokenVivamos));
                    leftTrim(datosVentaBancolombia.tokenVivamos,
                             globalDatosBancolombia.numeroReferencia, '0');
                }

                memcpy(stringAux, globalDatosBancolombia.totalVenta,
                       strlen(globalDatosBancolombia.totalVenta) - 2);
                formatString(0x00, 0, stringAux, strlen(stringAux), monto, 2);
                sprintf(linea1, "RECIBO: %.6s", globalDatosBancolombia.numeroRecibo);
                sprintf(linea2, "VALOR:$%s", monto);
                memset(datosVentaBancolombia.totalVenta, 0x00,
                       sizeof(datosVentaBancolombia.totalVenta));
                memcpy(datosVentaBancolombia.totalVenta, globalDatosBancolombia.totalVenta,
                       sizeof(globalDatosBancolombia.totalVenta));
                memcpy(montoAux, datosVentaBancolombia.totalVenta, 12);
                strcat(datosVentaBancolombia.totalVenta, "00");

//                        iRet = getchAcceptCancel("ANULACION", linea1, linea2, "Desea Continuar?", 20 * 1000);

                screenMessageFiveLine((char *) "ANULACION", linea1, linea2,
                                      (char *) "Desea Continuar?",
                                      globalDatosBancolombia.posEntrymode, (char *) "",
                                      lineaResumen);
                enviarStringToJava(lineaResumen, (char *) "showScreenMessage");

                if (iRet <= 0 && verificarHabilitacionCaja()) {

                    memset(datosVentaBancolombia.codigoRespuesta, 0x00,
                           sizeof(datosVentaBancolombia.codigoRespuesta));
                    memcpy(datosVentaBancolombia.codigoRespuesta, "01", 2);
                    memcpy(datosVentaBancolombia.tipoTransaccion,
                           globalDatosBancolombia.tipoTransaccion, TAM_TIPO_TRANSACCION);
//                            transaccionRespuestaDatosCaja(datosVentaBancolombia, TRANSACCION_TEFF_ANULACION);

                }
            } else {
                screenMessageFiveLine((char *) "ANULACION", (char *) "RECIBO YA ANULADO",
                                      (char *) "CANCELANDO",
                                      (char *) "LA TRANSACCION", (char *) "", (char *) "",
                                      lineaResumen);
                enviarStringToJava(lineaResumen, (char *) "showScreenMessage");
            }
        } else {
            if (atoi(globalDatosBancolombia.estadoTransaccion) == 0 && iRet >= 0 && globalDatosBancolombia.numeroRecibo[0] != 0x00){
                screenMessageFiveLine((char *) " ANULACION  ", (char *) "RECIBO YA ANULADO ",
                                      (char *) "CANCELANDO",
                                      (char *) "LA TRANSACCION", (char *) "", (char *) "",
                                      lineaResumen);
                enviarStringToJava(lineaResumen, (char *) "showScreenMessage");
            } else {
                screenMessageFiveLine((char *) " ANULACION  ", (char *) "RECIBO NO EXISTE ",
                                      (char *) "CANCELANDO",
                                      (char *) "LA TRANSACCION", (char *) "", (char *) "",
                                      lineaResumen);
                enviarStringToJava(lineaResumen, (char *) "showScreenMessage");
            }
        }
    } else {
        screenMessageFiveLine((char *) "ANULACION", (char *) "RECIBO YA ANULADO",
                              (char *) "CANCELANDO",
                              (char *) "LA TRANSACCION", (char *) "", (char *) "", lineaResumen);
        enviarStringToJava(lineaResumen, (char *) "showScreenMessage");
    }

    return iRet;
}

int enviarAnulacion(DatosCnbBancolombia *datosVenta) {

    int resultado = 0;
    int intentosVentas = 0;

    do {
        armarTramaAnulacion(datosVenta, intentosVentas);

        resultado = procesarTransaccionAnulacion(*datosVenta);

        if (resultado == -2) {
            intentosVentas++;
        }

        if (intentosVentas == MAX_INTENTOS_VENTA) {
            //screenMessage("COMUNICACION", "REINTENTE", "TRANSACCION", "", 2 * 1000);
            resultado = -1;
        }

    } while (resultado == -2 && intentosVentas < MAX_INTENTOS_VENTA);
    LOGI("resultado anulacion  %d", resultado);
    if (resultado > 0) {
        LOGI("fecha antes  %s", datosVenta->txnDate);
        extraerCamposAprobacionAnulacion(datosVenta);
        LOGI("fecha despues  %s", datosVenta->txnDate);
        if (verificarHabilitacionCaja() == TRUE) {
            memset(datosVentaBancolombia.codigoRespuesta, 0x00,
                   sizeof(datosVentaBancolombia.codigoRespuesta));
            memset(datosVentaBancolombia.totalVenta, 0x00,
                   sizeof(datosVentaBancolombia.totalVenta));
            //memcpy(datosVentaBancolombia.totalVenta,montoAux, 12);
            memcpy(datosVentaBancolombia.codigoRespuesta, "00", 2);
            memcpy(datosVentaBancolombia.rrn, datosVenta->rrn, TAM_RRN);
            memcpy(datosVentaBancolombia.tipoTransaccion, datosVenta->tipoTransaccion,
                   TAM_TIPO_TRANSACCION);

            /* if (verificarCajaIPCB() == 1) {
                 memset(datosVentaBancolombia.codigoAprobacion, 0x00, sizeof(datosVentaBancolombia.codigoAprobacion));
                 memset(datosVentaBancolombia.terminalId, 0x00, sizeof(datosVentaBancolombia.terminalId));
                 memset(datosVentaBancolombia.track2, 0x00, sizeof(datosVentaBancolombia.track2));
                 memset(datosVentaBancolombia.cardName, 0x00, sizeof(datosVentaBancolombia.cardName));
                 memcpy(datosVentaBancolombia.codigoAprobacion, datosVenta.codigoAprobacion, 6);
                 memcpy(datosVentaBancolombia.terminalId,datosVenta.terminalId, 8);
                 memcpy(datosVentaBancolombia.track2, datosVenta.track2, 16);
             }*/

            // iRet = transaccionRespuestaDatosCaja(datosVentaBancolombia, TRANSACCION_TEFF_ANULACION);


            if (resultado < 0 /*&& verificarCajaIPCB() == 1*/) {
                memcpy(datosVentaBancolombia.codigoRespuesta, "WS", 2);
                if (atoi(datosVentaBancolombia.tipoTransaccion) == TRANSACCION_BCL_DEPOSITO) {
                    memset(datosVentaBancolombia.totalVenta, 0x00,
                           sizeof(datosVentaBancolombia.totalVenta));
                    //memcpy(datosVentaBancolombia.totalVenta, montoAux, strlen(montoAux) - 2);
                }
                imprimirDeclinadas(datosVentaBancolombia, (char *) "ERROR CON EL WS_AN");
            }
            strcat(datosVentaBancolombia.totalVenta, "00");
        }
        //control de cambios login de operador
        setParameter(USER_CNB_LOGON, "1");
        LOGI("resultado mostrar aprobacion anulacion  ");
        mostrarAprobacionAnulacion(datosVenta);
    }
    return resultado;
}

void extraerCamposAprobacionAnulacion(DatosCnbBancolombia *datosVenta) {
    ISOFieldMessage isoFieldMessage;

    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));

    isoFieldMessage = getField(12);
    memset(datosVenta->txnTime, 0x00, sizeof(datosVenta->txnTime));
    memcpy(datosVenta->txnTime, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

    isoFieldMessage = getField(13);
    memset(datosVenta->txnDate, 0x00, sizeof(datosVenta->txnDate));
    memcpy(datosVenta->txnDate, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

    isoFieldMessage = getField(37);
    memset(datosVenta->rrn, 0x00, sizeof(datosVenta->rrn));
    memcpy(datosVenta->rrn, isoFieldMessage.valueField + 6, 6);

    isoFieldMessage = getField(38);
    memset(datosVenta->codigoAprobacion, 0x00, sizeof(datosVenta->codigoAprobacion));
    memcpy(datosVenta->codigoAprobacion, isoFieldMessage.valueField, isoFieldMessage.totalBytes);
}

void mostrarAprobacionAnulacion(DatosCnbBancolombia *datosVenta) {

    ISOFieldMessage isoFieldMessage;
    char pan[20 + 1];
    uChar modo[2 + 1];
    int resultado = 0;
    DatosCnbBancolombia datosBancolombia;
    char impresionRecibo[1 + 1];
    int resultadoCaja = 0;
    int iRet = 0;

    memset(&datosBancolombia, 0x00, sizeof(datosBancolombia));
    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(&pan, 0x00, sizeof(pan));
    memset(modo, 0x00, sizeof(modo));

    ///// SE VALIDA EL CAMPO 61 PARA EL COSTO DE LA TRANSACCION ////
    verificarCosto(datosVenta->field61);


    /// BORRAR ARCHIVO REVERSO ///
    borrarArchivo(discoNetcom, ARCHIVO_REVERSO);
    sprintf(datosVenta->estadoTransaccion, "%d", TRANSACCION_ANULADA);
    LOGI("estado tx  %s", datosVenta->estadoTransaccion);
    cambiarEstadoTransaccionBancolombia(datosVenta->numeroRecibo, datosVenta->rrn,
                                        datosVenta->codigoAprobacion,
                                        datosVenta->txnDate, datosVenta->txnTime);
    LOGI("fecha tx  %s", datosVenta->txnDate);
    LOGI("hora tx  %s", datosVenta->txnTime);
    memset(datosVenta->estadoTransaccion, 0x00, sizeof(datosVenta->estadoTransaccion));
    strcpy(datosVenta->estadoTransaccion, "0");

    _guardarDirectorioJournals_(TRANS_CNB_BANCOLOMBIA, datosVenta->numeroRecibo,
                                datosVenta->estadoTransaccion);

    //screenMessage("CODIGO APROBACION", datosVenta->codigoAprobacion, "NUMERO DE APROBACION", "", 0);
    resultadoCaja = verificarHabilitacionCaja();

    if (resultadoCaja == TRUE) {
        getParameter(IMPRESION_RECIBO_CAJA, impresionRecibo);
        iRet = strncmp(impresionRecibo, "01", 2);
    } else {
        iRet = 0;
    }

    if (iRet == 0) {
        LOGI("error es al imprimir tx  %s", datosVenta->estadoTransaccion);
        memcpy(datosVentaBancolombia.codigoRespuesta,"00", 2);
        memcpy(globalDatosBancolombia.codigoRespuesta,"00", 2);
        imprimirTicketBancolombia(*datosVenta, RECIBO_COMERCIO, 6);
        //resultado = getchAcceptCancel("RECIBO", "IMPRIMIR", "SEGUNDO", "RECIBO ?", 30 * 1000);
        if (resultado != __BACK_KEY) {
            //imprimirTicketBancolombia(*datosVenta, RECIBO_CLIENTE, 6);
        }
    }

}

void armarTramaAnulacion(DatosCnbBancolombia *datosVenta, int intentosVentas) {

    ISOHeader isoHeader8583;
    uChar nii[TAM_NII + 1];
    DatosTokens datosToken;
    char variableAux[36];
    char systemTrace[12 + 1];
    char codigoProceso[6 + 1];
    ResultTokenPack resultTokenPack;
    uChar tipoTransaccion[2 + 1];
    uChar tokenQZ[81 + 1];

    int longitud = 0;

    memset(&isoHeader8583, 0x00, sizeof(isoHeader8583));
    memset(nii, 0x00, sizeof(nii));
    memset(variableAux, 0x00, sizeof(variableAux));
    memset(systemTrace, 0x00, sizeof(systemTrace));
    memset(codigoProceso, 0x00, sizeof(codigoProceso));
    memset(&datosToken, 0x00, sizeof(datosToken));
    memset(&resultTokenPack, 0x00, sizeof(resultTokenPack));
    memset(tipoTransaccion, 0x00, sizeof(tipoTransaccion));
    memset(tokenQZ, 0x00, sizeof(tokenQZ));

    getParameter(NII, nii);

    if (intentosVentas == 0) {

        generarSystemTraceNumber(systemTrace, "");
    }

    memcpy(datosVenta->nii, nii + 1, TAM_NII);

    longitud = _armarTokenAutorizacion_(tokenQZ);

    isoHeader8583.TPDU = 60;
    memcpy(isoHeader8583.destination, nii, TAM_NII + 1);
    memcpy(isoHeader8583.source, nii, TAM_NII + 1);
    setHeader(isoHeader8583);

    setMTI("0200");

    if (atoi(datosVenta->tipoTransaccion) == TRANSACCION_BCL_RECAUDO) {

        if (strncmp(datosVenta->modoDatafono, "1", 1) == 0) {
            strcpy(codigoProceso, "27");
        } else {
            strcpy(codigoProceso, "28");
        }
    } else {
        strcpy(codigoProceso, "27");
    }

    memcpy(codigoProceso + 2, datosVenta->processingCode + 2, 4);

    memset(datosVenta->processingCode, 0x00, TAM_CODIGO_PROCESO);
    strcpy(datosVenta->processingCode, codigoProceso);

    if (intentosVentas == 0) {
        strcpy(datosVenta->systemTrace, systemTrace);
    }

    setField(3, codigoProceso, TAM_CODIGO_PROCESO);
    setField(4, datosVenta->totalVenta, 12);
    setField(11, systemTrace, TAM_SYSTEM_TRACE);
    setField(22, datosVenta->posEntrymode, TAM_ENTRY_MODE);
    setField(24, datosVenta->nii, TAM_NII);
    setField(25, datosVenta->posConditionCode, TAM_POS_CONDITION);
    setField(35, datosVenta->track2, strlen(datosVenta->track2));

    memset(variableAux, 0x00, sizeof(variableAux));
    leftPad(variableAux, datosVenta->rrn, 0x30, 12);
    setField(37, variableAux, 12);
    setField(38, datosVenta->codigoAprobacion, 6);

    setField(41, datosVenta->terminalId, TAM_TERMINAL_ID);
    setField(42, "000000000000000", 15);

    ///// DATOS PARA EL TOKEN R3
    strcpy(datosToken.valorKotenR3, datosVenta->tipoProducto);
    LOGI("token R3 %s", datosVenta->tipoProducto);
    memcpy(datosToken.tokensQF, datosVenta->tokensQF, SIZE_TOKEN_QF);

    if (strlen(datosVenta->tokensP6) != 0) {
        memcpy(datosToken.tokensp6, datosVenta->tokensP6, SIZE_TOKEN_P6);
    } else {
        memset(datosToken.tokensp6, 0x30, SIZE_TOKEN_P6);
    }
    setField(58, tokenQZ, longitud);
    //// LA ANULACION DEBE ENVIAR EL NUMERO DE RECIBO
    memcpy(datosToken.referencia1, datosVenta->numeroRecibo, TAM_NUMERO_RECIBO);

    ///// DATOS PARA EL TOKEN P6

    ///// EMPAQUETA EL CAMPO 60 CON LOS DATOS DE DATOSTOKENS
    sprintf(tipoTransaccion, "%02d", TRANSACCION_BCL_ANULACION);
    empaquetarP60(datosToken, tipoTransaccion);

    //// SE RECUPERA EL CAMPO 60
    resultTokenPack = _packTokenMessage_();

    setField(60, resultTokenPack.tokenPackMessage, resultTokenPack.totalBytes);

}

void
cambiarEstadoTransaccionBancolombia(char *numeroRecibo, char *rrn, char *aprobacion, char *fecha,
                                    char *hora) {

    int resultado = 0;
    int posicion = 0;
    char dataDuplicado[TAM_JOURNAL_BANCOLOMBIA + 1];
    char auxiliarTiempo[TAM_TIME + 1];
    char auxiliarfecha[TAM_DATE + 1];

    DatosCnbBancolombia datosVenta;
    memset(auxiliarTiempo, 0x00, sizeof(auxiliarTiempo));
    memset(auxiliarfecha, 0x00, sizeof(auxiliarfecha));
    do {
        memset(dataDuplicado, 0x00, sizeof(dataDuplicado));
        memset(&datosVenta, 0x00, sizeof(datosVenta));
        resultado = buscarArchivo(discoNetcom, JOURNAL_CNB_BANCOLOMBIA, dataDuplicado, posicion,
                                  sizeof(DatosCnbBancolombia));

        memcpy(&datosVenta, dataDuplicado, sizeof(DatosCnbBancolombia));

        if (strncmp(datosVenta.numeroRecibo, numeroRecibo, TAM_NUMERO_RECIBO) == 0) {
            memset(dataDuplicado, 0x00, sizeof(dataDuplicado));

            memcpy(datosVenta.txnTime, hora, strlen(hora));
            memcpy(datosVenta.txnDate, fecha, strlen(fecha));
            memcpy(datosVenta.codigoAprobacion, aprobacion, strlen(aprobacion));
            memcpy(datosVenta.rrn, rrn, strlen(rrn));
            memcpy(datosVenta.estadoTransaccion, "0", 1);

            sprintf(datosVenta.estadoCierre, "%2d",
                    0); // Se debe cambiar el estado del cierre para que la vuelva a tener en cuenta

            memcpy(dataDuplicado, &datosVenta, sizeof(DatosCnbBancolombia));
            actualizarArchivo(discoNetcom, JOURNAL_CNB_BANCOLOMBIA, dataDuplicado, posicion,
                              sizeof(DatosCnbBancolombia));
            resultado = 1;
        }

        posicion += sizeof(DatosCnbBancolombia);
    } while (resultado == 0);
}

int procesarTransaccionAnulacion(DatosCnbBancolombia datosVenta) {

    ResultISOPack resultadoIsoPackMessage;
    int resultadoTransaccion = -1;
    DatosTransaccionDeclinada datosTransaccionDeclinada;
    uChar numeroReciboTokenP6[TAM_NUMERO_RECIBO + 1];

    memset(globalDataRecibida, 0x00, sizeof(globalDataRecibida));
    memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
    memset(&datosTransaccionDeclinada, 0x00, sizeof(datosTransaccionDeclinada));
    memset(numeroReciboTokenP6, 0x00, sizeof(numeroReciboTokenP6));

    resultadoIsoPackMessage = packISOMessage();

    if (resultadoIsoPackMessage.responseCode > 0) {

        resultadoTransaccion = realizarTransaccion(resultadoIsoPackMessage.isoPackMessage,
                                                   resultadoIsoPackMessage.totalBytes,
                                                   globalDataRecibida, TRANSACCION_ANULACION,
                                                   CANAL_DEMANDA,
                                                   REVERSO_GENERADO);

        if (resultadoTransaccion > 0) {
            globalsizeData = resultadoTransaccion;

            resultadoTransaccion = unpackDataAnulacion(datosVenta, numeroReciboTokenP6);

            if (numeroReciboTokenP6[0] != 0x00 &&
                strcmp(datosVenta.numeroRecibo, numeroReciboTokenP6) != 0) {

                //screenMessage("MENSAJE", "TRANSACCION", "REVERSADA", "", T_OUT(2));
                imprimirDeclinadas(datosVentaBancolombia, "TRANSACCION REVERSADA");

                resultadoTransaccion = generarReverso();

                if (resultadoTransaccion == -3) {
                    imprimirDeclinadas(datosVentaBancolombia, "TIME OUT GENERAL");
                } else if (resultadoTransaccion == -1) {
                    imprimirDeclinadas(datosVentaBancolombia, "REVERSO PENDIENTE");
                } else if (resultadoTransaccion > 0) {
                    resultadoTransaccion = -2;
                }
            }
        } else {

            memset(datosVentaBancolombia.codigoRespuesta, 0x00,
                   sizeof(datosVentaBancolombia.codigoRespuesta));
            memcpy(datosVentaBancolombia.codigoRespuesta, "01", 2);

            if (verificarHabilitacionCaja() == TRUE) {

                memcpy(datosVentaBancolombia.tipoTransaccion, datosVenta.tipoTransaccion,
                       TAM_TIPO_TRANSACCION);
                //transaccionRespuestaDatosCaja(datosVentaBancolombia, TRANSACCION_TEFF_ANULACION);
            }

            if (resultadoTransaccion == -3) {
                imprimirDeclinadasCNB(datosVenta, "TIME OUT GENERAL");
            } else {
                imprimirDeclinadasCNB(datosVenta, "ERROR SIN CONEXION");
            }

            resultadoTransaccion = 0;
        }
    }

    return resultadoTransaccion;
}

int unpackDataAnulacion(DatosCnbBancolombia datosVenta, uChar *numeroReciboTokenP6) {

    ISOFieldMessage isoFieldMessage;
    uChar codigoRespuesta[2 + 1];
    int resultado = 2;
    uChar mensajeError[21 + 1];
    ResultTokenUnpack resultTokenUnpack;
    TokenMessage tokenMessage;

    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(&codigoRespuesta, 0x00, sizeof(codigoRespuesta));
    memset(mensajeError, 0x00, sizeof(mensajeError));

    /// VALIDACIONES TRANSACCION ///
    globalresultadoUnpack = unpackISOMessage(globalDataRecibida, globalsizeData);

    isoFieldMessage = getField(39);
    memcpy(codigoRespuesta, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

    if (strcmp(codigoRespuesta, "00") == 0) {

        resultado = verificacionTramabancolombia(&datosVenta);

        if (resultado > 0) {
            resultado = 1;

            if (atoi(datosVenta.tipoTransaccion) == TRANSACCION_BCL_RECAUDO) {
                memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
                isoFieldMessage = getField(60);

                if (isoFieldMessage.totalBytes > 0) {
                    memset(&resultTokenUnpack, 0x00, sizeof(resultTokenUnpack));
                    memset(&tokenMessage, 0x00, sizeof(tokenMessage));
                    resultTokenUnpack = unpackTokenMessage(isoFieldMessage.valueField,
                                                           isoFieldMessage.totalBytes);
                    tokenMessage = _getTokenMessage_("P6");
                    if (tokenMessage.totalBytes > 0) {
                        strncpy(numeroReciboTokenP6, tokenMessage.valueToken + 2,
                                TAM_NUMERO_RECIBO);
                    }
                }
            }
        } else if (resultado < 1) {
            resultado = generarReverso();

            if (verificarHabilitacionCaja() == TRUE && (resultado == -3 || resultado == -1)) {
                memset(datosVentaBancolombia.codigoRespuesta, 0x00,
                       sizeof(datosVentaBancolombia.codigoRespuesta));
                memcpy(datosVentaBancolombia.codigoRespuesta, "01", 2);
                memcpy(datosVentaBancolombia.tipoTransaccion, datosVenta.tipoTransaccion,
                       TAM_TIPO_TRANSACCION);
                //transaccionRespuestaDatosCaja(datosVentaBancolombia, TRANSACCION_TEFF_ANULACION);
            }

            if (resultado == -3) {
                imprimirDeclinadasCNB(datosVenta, "TIME OUT GENERAL");
            } else if (resultado == -1) {
                imprimirDeclinadasCNB(datosVenta, "REVERSO PENDIENTE");
            }

            if (resultado > 0) {
                resultado = -2;
            }
        }
    } else {
        borrarArchivo(discoNetcom, ARCHIVO_REVERSO);
        errorRespuestaTransaccion(codigoRespuesta, mensajeError);
        resultado = 0;

        if (strcmp(codigoRespuesta, "55") == 0) {
            resultado = -4;
        } else if (strcmp(codigoRespuesta, "75") == 0) {
            resultado = -3;
        }

        //// SI LA RESPUESTA ES 55 O 75 Y ES DEFERENTE DE CHIP, ENTONCES NO DEBE IMPRIMIR
        if ((resultado == -4) && (strlen(datosVenta.AID) == 0)) {
            return resultado;
        }

        if (verificarHabilitacionCaja() == TRUE) {
            memset(datosVentaBancolombia.codigoRespuesta, 0x00,
                   sizeof(datosVentaBancolombia.codigoRespuesta));
            memcpy(datosVentaBancolombia.codigoRespuesta, "01", 2);
            memcpy(datosVentaBancolombia.tipoTransaccion, datosVenta.tipoTransaccion,
                   TAM_TIPO_TRANSACCION);
            // transaccionRespuestaDatosCaja(datosVentaBancolombia, TRANSACCION_TEFF_ANULACION);
        }
        imprimirDeclinadasCNB(datosVenta, mensajeError);
    }

    return resultado;
}
