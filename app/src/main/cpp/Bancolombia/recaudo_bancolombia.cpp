//
// Created by NETCOM on 26/02/2020.
//
#include "jni.h"
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

#define  LOG_TAG    "NETCOM_RECAUDO"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

int globalsizeData;
int isRecaudoNequi = 0;
int globalTransaccionAprobada;
char globalDataRecibida[512 + 1];
char mensajeDeclinada[30 + 1];
int enviarConsultaRecaudoLector(DatosRecaudoAndroid *datosRecaudoAndroid, char *codigoLeido,
                                char *codigoAprobacion) {
    int resultado = 0;

    DatosTokens datosToken;
    memset(&datosToken, 0x00, sizeof(datosToken));
    memcpy(datosToken.factura, codigoLeido, strlen(codigoLeido));
    strcpy(datosToken.indicador1, INDICADOR_LECTOR);
    resultado = enviarConsultaRecaudo(datosToken, datosRecaudoAndroid);
    if (resultado > 0) {
        strcpy(codigoAprobacion, "OK");
    } else {
        if (mensajeDeclinada[0] == 0x00) {
            strcpy(mensajeDeclinada,"TIME OUT");
            LOGI("mensajeDeclinada %s", mensajeDeclinada);
        }
        imprimirDeclinadas(datosVentaBancolombia, mensajeDeclinada);
    }
    limpiarTokenMessage();
    return resultado;
}

int enviarConsultaRecaudoManual(DatosRecaudoAndroid *datosRecaudoAndroid, char *codigoAprobacion) {

    int resultado = 0;
    int indicadorValor = 0;
    int tipoRecaudoManual = 0;
    int indicadorBaseDatos = 0;

    long valorFactura = 0;
    uChar nombreConvenio[50 + 1] = {0x00};
    uChar cuentaRecaudadora[16 + 1] = {0x00};
    uChar facturaAux[SIZE_FACTURA + 1] = {0x00};
    uChar TCRecaudadora[2 + 1] = {0x00};
    uChar NCRecaudadora[16 + 1] = {0x00};
    uChar tarjetaEmpresarial[SIZE_TARJETA_CREDITO + 1] = {0x00};

    DatosTokens datosToken;
    DatosTarjetaSembrada datosTarjetaSembrada;

    memset(&datosToken, 0x00, sizeof(datosToken));
    memset(&datosVentaBancolombia, 0x00, sizeof(datosVentaBancolombia));

    if (strcmp(datosRecaudoAndroid->tipoRecaudo, "00") == 0 ||
        strcmp(datosRecaudoAndroid->tipoRecaudo, "NEQUI") == 0) {
        if (strcmp(datosRecaudoAndroid->tipoRecaudo, "NEQUI") == 0) {
            isRecaudoNequi = 1;
            strcpy(datosVentaBancolombia.numeroRecibo2, "NQ");
        } else {
            isRecaudoNequi = 0;
        }
        LOGI("Es convenio  ");
        tipoRecaudoManual = MANUAL_CODIGO_CONVENIO;
        memcpy(datosToken.indicador2, INDICADOR_NO_APLICA, 1);
        memcpy(TCRecaudadora, "00", TAM_TIPO_CUENTA);
        leftPad(datosVentaBancolombia.codigoComercio, datosRecaudoAndroid->codigoConvenio, 0x30,
                SIZE_COD_SERVICO);
    } else {
        LOGI("Es recaudadora  ");
        LOGI("cuenta  recaudadora %s  ", datosRecaudoAndroid->cuentaRecaudadora);
        tipoRecaudoManual = MANUAL_CUENTA_RECAUDADORA;
        sprintf(datosVentaBancolombia.isQPS, "%01d", MANUAL_CUENTA_RECAUDADORA);
        memcpy(datosToken.indicador2, INDICADOR_CTA_RECAUD, 1);
        memcpy(NCRecaudadora, datosRecaudoAndroid->cuentaRecaudadora,
               strlen(datosRecaudoAndroid->cuentaRecaudadora));
        memcpy(datosVentaBancolombia.textoAdicionalInicializacion,
               datosRecaudoAndroid->cuentaRecaudadora,
               strlen(datosRecaudoAndroid->cuentaRecaudadora));
        memcpy(TCRecaudadora, datosRecaudoAndroid->tipoRecaudo, TAM_TIPO_CUENTA);
    }

    memset(datosVentaBancolombia.tokenVivamos, 0x00, sizeof(datosVentaBancolombia.tokenVivamos));
    memcpy(datosVentaBancolombia.tokenVivamos, datosRecaudoAndroid->numeroFactura,
           SIZE_REFERENCIA_MANUAL);
    strcpy(datosVentaBancolombia.posEntrymode, "021");
    strcpy(datosToken.indicador1, INDICADOR_MANUAL);
    datosTarjetaSembrada = cargarTarjetaSembrada();
    memcpy(datosToken.referencia1, datosVentaBancolombia.tokenVivamos, SIZE_REFERENCIA_MANUAL);
    memcpy(datosToken.referencia2, datosVentaBancolombia.codigoComercio, SIZE_COD_SERVICO);
    armarP6(&datosToken, NCRecaudadora, TCRecaudadora, tipoRecaudoManual, tarjetaEmpresarial);
    resultado = consultaRecaudo(&datosToken, &valorFactura, &indicadorBaseDatos, &indicadorValor,
                                nombreConvenio, NULL);
    LOGI("consulto  %ld", valorFactura);
    datosRecaudoAndroid->indicadorBaseDatos = indicadorBaseDatos;
    datosRecaudoAndroid->indicadorValor = indicadorValor;
    datosRecaudoAndroid->valorFactura = valorFactura;
    memcpy(datosRecaudoAndroid->tokenP6, datosToken.tokensp6, 161);
    memcpy(datosRecaudoAndroid->nombreConvenio, nombreConvenio, strlen(nombreConvenio));
    LOGI("valorFactura %ld", valorFactura);
    LOGI("nombreConvenio %s", nombreConvenio);
    if (resultado > 0) {
        strcpy(codigoAprobacion, "OK");
    } else {
        if (mensajeDeclinada[0] == 0x00) {
            strcpy(mensajeDeclinada,"TIME OUT");
            LOGI("mensajeDeclinada %s", mensajeDeclinada);
        }
        imprimirDeclinadas(datosVentaBancolombia, mensajeDeclinada);
    }
    limpiarTokenMessage();
    return resultado;
}

int enviarConsultaRecaudo(DatosTokens datosToken, DatosRecaudoAndroid *datosRecaudoAndroid) {

    int resultado = 0;
    int indicadorValor = 0;
    int tipoRecaudoManual = 0;
    int indicadorBaseDatos = 0;

    long valorFactura = 0;

    uChar cuentaRecaudadora[16 + 1] = {0x00};
    uChar facturaAux[SIZE_FACTURA + 1] = {0x00};
    uChar TCRecaudadora[SIZE_CUENTA + 1] = {0x00};
    uChar nombreConvenio[TAM_P6_NOMBRE_CONVENIO + 1] = {0x00};


    memcpy(facturaAux, datosToken.factura, SIZE_FACTURA);
    armarP6(&datosToken, cuentaRecaudadora, TCRecaudadora, tipoRecaudoManual, facturaAux);
    resultado = consultaRecaudo(&datosToken, &valorFactura, &indicadorBaseDatos, &indicadorValor,
                                nombreConvenio, NULL);


    LOGI("consulto  consulta  ");
    datosRecaudoAndroid->indicadorBaseDatos = indicadorBaseDatos;
    datosRecaudoAndroid->indicadorValor = indicadorValor;
    datosRecaudoAndroid->valorFactura = valorFactura;
    memcpy(datosRecaudoAndroid->tokenP6, datosToken.tokensp6, 161);
    memcpy(datosRecaudoAndroid->nombreConvenio, nombreConvenio, strlen(nombreConvenio));
    LOGI("valor factura  %ld  ", valorFactura);
    LOGI("datosRecaudoAndroid->tokenP6 %s  ", datosRecaudoAndroid->tokenP6);
    memcpy(datosRecaudoAndroid->nombreConvenio, nombreConvenio, strlen(nombreConvenio));
    return resultado;
}

int consultaRecaudo(DatosTokens *datosTokens, long *valorFactura, int *indicadorBaseDatos,
                    int *indicadorValor,
                    uChar *nombreConvenio, RecaudoMulticorresponsal *datosRecaudo) {

    int iRet = 0;
    uChar totalVentaAux[TAM_COMPRA_NETA + 1];

    memset(totalVentaAux, 0x00, sizeof(totalVentaAux));

    memcpy(totalVentaAux, datosVentaBancolombia.totalVenta, TAM_COMPRA_NETA);
    memcpy(datosVentaBancolombia.totalVenta, "0000000000", TAM_COMPRA_NETA);
    armarTramaRecaudo(0, *datosTokens, MENSAJE_CONSULTA);
    memcpy(datosTokens->tokensp6 + 2, datosVentaBancolombia.numeroRecibo, TAM_NUMERO_RECIBO);

    iRet = procesarConsulta(datosTokens->tokensp6, valorFactura, indicadorBaseDatos, indicadorValor,
                            nombreConvenio, datosRecaudo);
    LOGI("proceso consulta  %d", iRet);
    return iRet;

}

int enviarRecaudo(DatosTokens datosToken, int tipoRecaudoCaja, char *totalVenta) {

    int resultado = 0;
    int intentosVentas = 0;
    int indice = 0;
    uChar nombreConvenio[30 + 1];
    uChar tokenP6[SIZE_TOKEN_P6 + 1];

    memset(tokenP6, 0x00, sizeof(tokenP6));
    strcpy(datosToken.indicador1, INDICADOR_LECTOR);
    memcpy(datosVentaBancolombia.totalVenta, totalVenta, 12);

   // do {

        armarTramaRecaudo(intentosVentas, datosToken, MENSAJE_PAGO);
        memcpy(datosToken.tokensp6 + 2, datosVentaBancolombia.numeroRecibo, TAM_NUMERO_RECIBO);
        LOGI("resultado procesarTx %s ", datosToken.tokensp6);
        resultado = procesarTransaccionRecaudo(datosToken.tokensp6, tipoRecaudoCaja);

        if (resultado > 0) {

            extraerTokens((char *) "P6", tokenP6);
            memset(nombreConvenio, 0x00, sizeof(nombreConvenio));

            indice = 60;

            if (strlen(datosVentaBancolombia.tokenVivamos) == 0) {
                memset(datosVentaBancolombia.tokenVivamos, 0x00,
                       sizeof(datosVentaBancolombia.tokenVivamos));
                memcpy(datosVentaBancolombia.tokenVivamos, tokenP6 + 8, SIZE_REFERENCIA_MANUAL);
            }
            indice += SIZE_REFERENCIA_MANUAL;

            if (strlen(datosVentaBancolombia.codigoComercio) == 0) {
                memset(datosVentaBancolombia.codigoComercio, 0x00,
                       sizeof(datosVentaBancolombia.codigoComercio));
                memcpy(datosVentaBancolombia.codigoComercio, tokenP6 + 32, SIZE_COD_SERVICO);
            }
            indice += SIZE_COD_SERVICO;

            memcpy(nombreConvenio, tokenP6 + 37, 30);
            indice += 30;

            memset(datosVentaBancolombia.field57, 0x00, sizeof(datosVentaBancolombia.field57));
            memcpy(datosVentaBancolombia.field57, nombreConvenio, strlen(nombreConvenio));
        }

        if (resultado == -2) {
            intentosVentas++;
        }

        if (intentosVentas == MAX_INTENTOS_VENTA) {
            //screenMessage("COMUNICACION", "REINTENTE", "TRANSACCION", "", 2 * 1000);
            resultado = -1;
            //responderCajaDeclinadaRecaudo(tipoRecaudoCaja);
            imprimirDeclinadas(datosVentaBancolombia, "REINTENTE TRANSACCION");
        }

   // } while (resultado == -2 && intentosVentas < MAX_INTENTOS_VENTA);

    if (strlen(datosVentaBancolombia.ARQC) > 0) { // Solo entra si es CHIP
        //memset(bufferAuxiliarCustomEMV, 0x00, sizeof(bufferAuxiliarCustomEMV));
        //cargaTagsRespuestaTransacionEMV(bufferAuxiliarCustomEMV, &sizeCustomEMV, globalresultadoUnpack, resultado);

        if (resultado == 1) {
            globalTransaccionAprobada = TRUE;
        }
    }
    if (resultado > 0) {
        mostrarAprobacionBancolombia(&datosVentaBancolombia);
    }
    limpiarTokenMessage();
    return resultado;
}

void armarTramaRecaudo(int intentosVentas, DatosTokens datosToken, int tipoMensaje) {

    int longitud = 0;

    ResultTokenPack resultTokenPack;
    DatosTarjetaSembrada datosTarjetaSembrada;
    uChar tokenQZ[81 + 1];
    uChar iniciaTransaccion[2 + 1];

    memset(&resultTokenPack, 0x00, sizeof(resultTokenPack));
    memset(&datosTarjetaSembrada, 0x30, sizeof(datosTarjetaSembrada));
    memset(tokenQZ, 0x00, sizeof(tokenQZ));
    memset(iniciaTransaccion, 0x00, sizeof(iniciaTransaccion));

    getParameter(TIPO_INICIA_TRANSACCION, iniciaTransaccion);

    datosTarjetaSembrada = cargarTarjetaSembrada();

    strcpy(datosVentaBancolombia.ultimosCuatro, datosTarjetaSembrada.ultimosCuatro);
    strcpy(datosVentaBancolombia.tipoCuenta, datosTarjetaSembrada.tipoCuenta);
    strcpy(datosVentaBancolombia.tarjetaHabiente, datosTarjetaSembrada.tarjetaHabiente);
    sprintf(datosVentaBancolombia.tipoTransaccion, "%02d", TRANSACCION_BCL_RECAUDO);

    memcpy(datosVentaBancolombia.posEntrymode, "021", TAM_ENTRY_MODE);

    if (verificarHabilitacionCaja() > 0 /*&& verificarCajaIPCB() == */
        && strcmp(iniciaTransaccion, RECAUDO_CAJA_NUEVO_SERVIDOR) != 0) { // recaudo antiguo
        sprintf(datosVentaBancolombia.processingCode, "51%2s00", datosVentaBancolombia.tipoCuenta);
        memcpy(datosVentaBancolombia.msgType, "0200", TAM_MTI);
    } else {
        switch (tipoMensaje) {
            case MENSAJE_CONSULTA:
                sprintf(datosVentaBancolombia.processingCode, "58%2s00",
                        datosVentaBancolombia.tipoCuenta);
                memcpy(datosVentaBancolombia.msgType, "0100", TAM_MTI);
                break;
            case MENSAJE_PAGO:
                sprintf(datosVentaBancolombia.processingCode, "59%2s00",
                        datosVentaBancolombia.tipoCuenta);
                memcpy(datosVentaBancolombia.msgType, "0200", TAM_MTI);
                break;
            default:
                break;
        }
    }
    longitud = _armarTokenAutorizacion_(tokenQZ);

    empaquetarEncabezado(&datosVentaBancolombia, datosTarjetaSembrada.pan, intentosVentas);

    ///// DATOS PARA EL TOKEN R3
    strcpy(datosToken.valorKotenR3, TOKEN_RECAUDO);
    LOGI("datosToken.valorKotenR3  %s", datosToken.valorKotenR3);
    memset(datosVentaBancolombia.iva, 0x00, sizeof(datosVentaBancolombia.iva));
    strcpy(datosVentaBancolombia.iva, datosToken.valorKotenR3);

    ///// DATOS PARA EL TOKEN QF
    strcpy(datosToken.tipoCuentaOrigen,
           datosTarjetaSembrada.tipoCuenta); //// TIPO DE CUENTA SEMBRADA
    LOGI("datosToken.tipoCuentaOrigen  %s", datosToken.tipoCuentaOrigen);
    strcpy(datosToken.cuentaOrigen, datosTarjetaSembrada.pan); //// PAN DE LA TARJETA SEMBRADA
    LOGI("datosToken.cuentaOrigen  %s", datosToken.cuentaOrigen);
    ///// EMPAQUETA EL CAMPO 60 CON LOS DATOS DE DATOSTOKENS
    empaquetarP60(datosToken, datosVentaBancolombia.tipoTransaccion);

    //// SE RECUPERA EL CAMPO 60
    resultTokenPack = _packTokenMessage_();
    setField(58, tokenQZ, longitud);
    setField(60, resultTokenPack.tokenPackMessage, resultTokenPack.totalBytes);
}

int procesarTransaccionRecaudo(uChar *tokenP6Enviado, int tipoRecaudoCaja) {

    ResultISOPack resultadoIsoPackMessage;
    int resultadoTransaccion = -1;
    DatosTransaccionDeclinada datosTransaccionDeclinada;
    uChar tokenP6Recibido[SIZE_TOKEN_P6 + 1];
    uChar iniciaTransaccion[2 + 1];
    uChar mensajeDeclinada[21 + 1] = {0x00};

    memset(globalDataRecibida, 0x00, sizeof(globalDataRecibida));
    memset(&globalresultadoIsoPack, 0x00, sizeof(globalresultadoIsoPack));
    memset(&datosTransaccionDeclinada, 0x00, sizeof(datosTransaccionDeclinada));
    memset(tokenP6Recibido, 0x00, sizeof(tokenP6Recibido));
    memset(iniciaTransaccion, 0x00, sizeof(iniciaTransaccion));

    globalresultadoIsoPack = packISOMessage();
    getParameter(TIPO_INICIA_TRANSACCION, iniciaTransaccion);

    verificarHabilitacionCaja();

    /*if (resultadoIsoPackMessage.responseCode > 0) {
        interpretarTrama(resultadoIsoPackMessage.isoPackMessage,
                         resultadoIsoPackMessage.totalBytes * 2);
        resultadoTransaccion = realizarTransaccion(resultadoIsoPackMessage.isoPackMessage,
                                                   resultadoIsoPackMessage.totalBytes,
                                                   globalDataRecibida,
                                                   atoi(datosVentaBancolombia.tipoTransaccion),
                                                   CANAL_PERMANENTE, REVERSO_GENERADO);

        if (resultadoTransaccion > 0) {
            globalsizeData = resultadoTransaccion;

            resultadoTransaccion = unpackDataRecaudo(tokenP6Recibido, tipoRecaudoCaja);

            if (resultadoTransaccion > 0
                && (verificarHabilitacionCaja() <= 0/)) {
                LOGI("comparar tokens pago % d", resultadoTransaccion);
                resultadoTransaccion = compararTokens(tokenP6Recibido, tokenP6Enviado,
                                                      COMPARACION_PAGO, (char *) " ", NULL);
                LOGI("resultadoPago % d", resultadoTransaccion);
                if (resultadoTransaccion <= 0) {
                    imprimirDeclinadas(datosVentaBancolombia, "TRANSACCION REVERSADA");
                    resultadoTransaccion = generarReverso();

                    if (resultadoTransaccion == -3) {
                        imprimirDeclinadas(datosVentaBancolombia, "TIME OUT GENERAL");
                    } else if (resultadoTransaccion == -1) {
                        imprimirDeclinadas(datosVentaBancolombia, "REVERSO PENDIENTE");
                    } else if (resultadoTransaccion > 0) {
                        resultadoTransaccion = -2;
                    }
                } else {
                    borrarArchivo(discoNetcom, (char *) ARCHIVO_REVERSO);
                }
            }
        } else {

            memset(datosVentaBancolombia.codigoRespuesta, 0x00,
                   sizeof(datosVentaBancolombia.codigoRespuesta));
            memcpy(datosVentaBancolombia.codigoRespuesta, "01", 2);

            if (resultadoTransaccion == -3) {
                strcpy(mensajeDeclinada, "TIME OUT GENERAL");
            } else {
                strcpy(mensajeDeclinada, "ERROR SIN CONEXION");
            }

            LOGI("imprimir declinada  ");
            imprimirDeclinadas(datosVentaBancolombia, mensajeDeclinada);

            resultadoTransaccion = 0;
        }
    }*/

    return resultadoTransaccion;
}

int unpackDataRecaudo(uChar *tokenP6Recibido, int tipoRecaudoCaja) {

    ISOFieldMessage isoFieldMessage;
    ResultTokenUnpack resultTokenUnpack;
    TokenMessage tokenMessage;
    uChar codigoRespuesta[2 + 1];
    int resultado = 2;
    uChar mensajeError[21 + 1];
    DatosCnbBancolombia datosRecaudo;
    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(&codigoRespuesta, 0x00, sizeof(codigoRespuesta));
    memset(mensajeError, 0x00, sizeof(mensajeError));
    memset(&resultTokenUnpack, 0x00, sizeof(resultTokenUnpack));
    memset(&tokenMessage, 0x00, sizeof(tokenMessage));

    verificarHabilitacionCaja();

    /// VALIDACIONES TRANSACCION ///
    globalresultadoUnpack = unpackISOMessage(globalDataRecibida, globalsizeData);

    isoFieldMessage = getField(39);
    memcpy(codigoRespuesta, isoFieldMessage.valueField, isoFieldMessage.totalBytes);
    memcpy(datosVentaBancolombia.codigoRespuesta, isoFieldMessage.valueField,
           isoFieldMessage.totalBytes);

    if (strcmp(codigoRespuesta, "00") == 0) {
        memcpy(datosRecaudo.processingCode, datosVentaBancolombia.processingCode,
               sizeof(datosVentaBancolombia.processingCode));
        memcpy(datosRecaudo.systemTrace, datosVentaBancolombia.systemTrace,
               sizeof(datosVentaBancolombia.systemTrace));
        memcpy(datosRecaudo.terminalId, datosVentaBancolombia.terminalId,
               sizeof(datosVentaBancolombia.terminalId));
        memcpy(datosRecaudo.totalVenta, datosVentaBancolombia.totalVenta,
               sizeof(datosVentaBancolombia.totalVenta));
        resultado = verificacionTramabancolombia(&datosRecaudo);

        if (resultado > 0) {

            resultado = 1;

            memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));

            isoFieldMessage = getField(60);

            if (isoFieldMessage.totalBytes > 0) {

                resultTokenUnpack = unpackTokenMessage(isoFieldMessage.valueField,
                                                       isoFieldMessage.totalBytes);
                tokenMessage = _getTokenMessage_((char *) "P6");

                if (tokenMessage.totalBytes > 0) {
                    memcpy(tokenP6Recibido, tokenMessage.valueToken, tokenMessage.totalBytes);
                    LOGI("tokenP6Recibido en umpack %s  ", tokenP6Recibido);
                    resultado = 1;
                } else {

                }
            }

        } else if (resultado < 1) {
            resultado = generarReverso();

            if (verificarHabilitacionCaja() == TRUE && (resultado == -3 || resultado == -1)) {
                //responderCajaDeclinadaRecaudo(tipoRecaudoCaja);
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
        borrarArchivo(discoNetcom, (char *) ARCHIVO_REVERSO);
        errorRespuestaTransaccion(codigoRespuesta, mensajeError);
        resultado = 0;

        //getMensajeError(codigoRespuesta, mensajeError);

        memset(datosVentaBancolombia.numeroRecibo, 0x00,
               sizeof(datosVentaBancolombia.numeroRecibo));
        //responderCajaDeclinadaRecaudo(tipoRecaudoCaja);
        imprimirDeclinadas(datosVentaBancolombia, mensajeError);
    }

    return resultado;
}

int compararTokens(uChar *tokenRecibido, uChar *tokenEnviado, int tipoComparacion,
                   uChar *nombreConvenio, RecaudoMulticorresponsal *datosRecaudo) {

    int idxRecibido = 0;
    int idxEnviado = 0;
    int caja = 0;
    int iRet = 0;
    boolean diferencia = TRUE;
    uChar indicador1[TAM_P6_IND_1 + 1]; //INDICADOR_MANUAL, INDICADOR_LECTOR, INDICADOR_TEMPRESARIAL
    boolean manualCodigoConvenio = FALSE;

    if (verificarHabilitacionCaja() == TRUE) {
        caja = 1;
    }
/*
    if (caja == TRUE) {
        if (verificarCajaIPCB() == 1) {
            caja = 0;
        }
    }*/

    memset(indicador1, 0x00, sizeof(indicador1));
    LOGI("enviando %s recibido %s", tokenRecibido + idxRecibido, tokenEnviado + idxEnviado);

    diferencia = strncmp(tokenRecibido + idxRecibido, tokenEnviado + idxEnviado,
                         TAM_P6_IND_1); //INDIC 1, INGRESO MANUAL
    if (diferencia == FALSE) {
        idxRecibido += TAM_P6_IND_1;
        idxEnviado += TAM_P6_IND_1;
        LOGI("diferencia 1");
        diferencia = strncmp(tokenRecibido + idxRecibido, tokenEnviado + idxEnviado,
                             SIZE_INDICADOR); //INDIC 2, MANUAL CON RECAUDADORA
        memcpy(indicador1, tokenEnviado, TAM_P6_IND_1);

        if (strcmp(indicador1, INDICADOR_MANUAL) == 0) {
            if (strncmp(tokenEnviado + idxEnviado, "0", TAM_P6_IND_2) == 0) {
                manualCodigoConvenio = TRUE;
            }
        }
    }

    if (diferencia == FALSE) {
        idxRecibido += TAM_P6_IND_2;
        idxEnviado += TAM_P6_IND_2;
        LOGI("diferencia 2");
        diferencia = strncmp(tokenRecibido + idxRecibido, tokenEnviado + idxEnviado,
                             TAM_P6_NUM_RECIBO); //NUMERO DE RECIBO

    }

    if (diferencia == FALSE) {
        idxRecibido += TAM_P6_NUM_RECIBO;
        idxEnviado += TAM_P6_NUM_RECIBO;
        if (strcmp(indicador1, INDICADOR_MANUAL) == 0
            || (strcmp(indicador1, INDICADOR_LECTOR) == 0 && tipoComparacion == COMPARACION_PAGO)) {
            LOGI("diferencia 3");
            diferencia = strncmp(tokenRecibido + idxRecibido, tokenEnviado + idxEnviado,
                                 TAM_P6_NUM_REFERENCIA); //REFERENCIA
        }
    }

    if (caja == 1) {
        if (strcmp(indicador1, INDICADOR_LECTOR) == 0 && tipoComparacion == COMPARACION_CONSULTA) {
            memcpy(datosRecaudo->facturaPago, tokenRecibido + idxRecibido, TAM_P6_NUM_REFERENCIA);
        }
    }

    if (diferencia == FALSE) {
        idxRecibido += TAM_P6_NUM_REFERENCIA;
        idxEnviado += TAM_P6_NUM_REFERENCIA;

        if (((strcmp(indicador1, INDICADOR_MANUAL) == 0 && tipoComparacion == COMPARACION_PAGO) ||
             manualCodigoConvenio)
            || (strcmp(indicador1, INDICADOR_LECTOR) == 0 && tipoComparacion == COMPARACION_PAGO)) {
            LOGI("diferencia 4");

            diferencia = strncmp(tokenRecibido + idxRecibido, tokenEnviado + idxEnviado,
                                 TAM_P6_COD_CONVENIO); //CODIGO CONVENIO
        }

        if (caja == 1) {
            if (((strcmp(indicador1, INDICADOR_LECTOR) == 0) || (manualCodigoConvenio == FALSE))
                && tipoComparacion == COMPARACION_CONSULTA) {
                memcpy(datosRecaudo->codigoConvenio, tokenRecibido + idxRecibido,
                       TAM_P6_COD_CONVENIO);
            }
        }
    }

    if (diferencia == FALSE) {
        idxRecibido += TAM_P6_COD_CONVENIO;
        LOGI("diferencia 5");
        if (tipoComparacion == COMPARACION_CONSULTA) {
            memcpy(nombreConvenio, tokenRecibido + idxRecibido, TAM_P6_NOMBRE_CONVENIO);
        }

        idxRecibido += TAM_P6_NOMBRE_CONVENIO;

        idxRecibido += TAM_P6_IND_BASE_DATOS;
        idxRecibido += TAM_P6_IND_VALOR;
        idxRecibido += TAM_P6_VALOR_FACTURA;
        idxEnviado += TAM_P6_COD_CONVENIO;
        idxEnviado += TAM_P6_ESPACIO_45;

        if (strcmp(indicador1, INDICADOR_MANUAL) == 0
            && (tipoComparacion == COMPARACION_PAGO ||
                manualCodigoConvenio == FALSE)) { //&& tipoComparacion == COMPARACION_PAGO
            LOGI("diferencia 6");

            diferencia = strncmp(tokenRecibido + idxRecibido, tokenEnviado + idxEnviado,
                                 TAM_P6_TC_RECAUDADORA); //TIPO CUENTA RECAUDADORA
        } else if ((strcmp(indicador1, INDICADOR_LECTOR) == 0 ||
                    strcmp(indicador1, INDICADOR_TEMPRESARIAL) == 0)
                   && tipoComparacion == COMPARACION_PAGO) {
            LOGI("diferencia 7");

            diferencia = strncmp(tokenRecibido + idxRecibido, tokenEnviado + idxEnviado,
                                 TAM_P6_TC_RECAUDADORA); //PARA LECTOR
        }
    }

    if (diferencia == FALSE) {
        idxRecibido += TAM_P6_TC_RECAUDADORA;
        idxEnviado += TAM_P6_TC_RECAUDADORA;
        if (strcmp(indicador1, INDICADOR_MANUAL) == 0
            && (tipoComparacion == COMPARACION_PAGO || manualCodigoConvenio == FALSE)) {
            LOGI("diferencia 8");

            diferencia = strncmp(tokenRecibido + idxRecibido, tokenEnviado + idxEnviado,
                                 TAM_P6_CTA_RECAUDADORA); //NUMERO CUENTA RECAUDADORA
        } else if ((strcmp(indicador1, INDICADOR_LECTOR) == 0 ||
                    strcmp(indicador1, INDICADOR_TEMPRESARIAL) == 0)
                   && tipoComparacion == COMPARACION_PAGO) {
            LOGI("diferencia 9");

            diferencia = strncmp(tokenRecibido + idxRecibido, tokenEnviado + idxEnviado,
                                 TAM_P6_CTA_RECAUDADORA); //PARA LECTOR
        }
    }

    if (diferencia == FALSE) {
        idxRecibido += TAM_P6_CTA_RECAUDADORA;
        idxEnviado += TAM_P6_CTA_RECAUDADORA;

        if (tipoComparacion == COMPARACION_PAGO) {

            if (strcmp(indicador1, INDICADOR_TEMPRESARIAL) == 0) {

                memcpy(tokensP6 + idxEnviado, tokenRecibido + idxRecibido,
                       TAM_P6_NUM_TRANZABILIDAD);
            } else {
                LOGI("diferencia 10");

                diferencia = strncmp(tokenRecibido + idxRecibido, tokenEnviado + idxEnviado,
                                     TAM_P6_NUM_TRANZABILIDAD); //NUMERO TRANZABILIDAD
            }
        }
    }

    if (diferencia == FALSE) {

        idxRecibido += TAM_P6_NUM_TRANZABILIDAD;
        idxEnviado += TAM_P6_NUM_TRANZABILIDAD;

        if (tipoComparacion == COMPARACION_CONSULTA) {
            if (strcmp(indicador1, INDICADOR_MANUAL) == 0) {
                idxRecibido += TAM_P6_NUM_PADRE;
                idxEnviado += TAM_P6_NUM_PADRE;
                LOGI("diferencia 11");

                diferencia = strncmp(tokenRecibido + idxRecibido,
                                     datosVentaBancolombia.tokenVivamos,
                                     strlen(datosVentaBancolombia.tokenVivamos)); //NUMERO TRANZABILIDAD
            }
        } else if (strcmp(indicador1, INDICADOR_LECTOR) == 0 ||
                   strcmp(indicador1, INDICADOR_MANUAL) == 0) {
            LOGI("diferencia 12");
            diferencia = strncmp(tokenRecibido + idxRecibido, tokenEnviado + idxEnviado,
                                 TAM_P6_NUM_FACTURA); //NUMERO TRANZABILIDAD
        }
    }

    if (strcmp(indicador1, INDICADOR_TEMPRESARIAL) == 0) {
        memcpy(tokensP6, tokenRecibido, SIZE_TOKEN_P6);
        diferencia = FALSE;
    }

    iRet = diferencia == FALSE ? 1 : 0;
    LOGI("salio de tokens ");
    return iRet;
}

int
procesarConsulta(uChar *tokenP6, long *valorFactura, int *indicadorBaseDatos, int *indicadorValor,
                 uChar *nombreConvenio, RecaudoMulticorresponsal *datosRecaudo) {

    ResultISOPack resultadoIsoPackMessage;
    int resultadoTransaccion = -1;
    DatosTransaccionDeclinada datosTransaccionDeclinada;
    uChar globalDataRecibida[512 + 1];
    uChar tokenP6Recibido[SIZE_TOKEN_P6 + 1];
    uChar var[SIZE_TOKEN_P6 + 1];

    memset(globalDataRecibida, 0x00, sizeof(globalDataRecibida));
    memset(&globalresultadoIsoPack, 0x00, sizeof(globalresultadoIsoPack));
    memset(&datosTransaccionDeclinada, 0x00, sizeof(datosTransaccionDeclinada));
    memset(tokenP6Recibido, 0x00, sizeof(tokenP6Recibido));
    memset(var, 0x00, sizeof(var));

    globalresultadoIsoPack = packISOMessage();

   /*if (resultadoIsoPackMessage.responseCode > 0) {

        resultadoTransaccion = realizarTransaccion(resultadoIsoPackMessage.isoPackMessage,
                                                   resultadoIsoPackMessage.totalBytes,
                                                   globalDataRecibida,
                                                   atoi(datosVentaBancolombia.tipoTransaccion),
                                                   CANAL_PERMANENTE, REVERSO_NO_GENERADO);
        LOGI("resultadoTransaccion % d", resultadoTransaccion);

        if (resultadoTransaccion > 0) {
            resultadoTransaccion = desempaquetarConsultaRecaudo(globalDataRecibida,
                                                                resultadoTransaccion,
                                                                tokenP6Recibido);
        }
        LOGI("desempaquetarConsultaRecaudo % d", resultadoTransaccion);
        if (resultadoTransaccion > 0) {
            resultadoTransaccion = compararTokens(tokenP6Recibido, tokenP6, COMPARACION_CONSULTA,
                                                  nombreConvenio, datosRecaudo);

            if (resultadoTransaccion > 0) {
                cargarIndicadoresDB(tokenP6Recibido, valorFactura, indicadorBaseDatos,
                                    indicadorValor);
                LOGI("cargo indicadores  ");
            } else {
                //screenMessage("MENSAJE", "TRANSACCION", "NO", "PERMITIDA", T_OUT(2));
                //strcpy(mensajeDeclinada, "TRANSACCION NO PERMITIDA");
            }
        }

        if (resultadoTransaccion > 0) {
            LOGI("copiar token  ");
            memcpy(tokenP6, tokenP6Recibido, SIZE_TOKEN_P6);

        }
    }*/

    return resultadoTransaccion;
}

int desempaquetarConsultaRecaudo(uChar *datosRecibidos, int tam, uChar *tokenP6Recibido) {

    ResultISOUnpack resultadoUnpack;
    ResultTokenUnpack resultTokenUnpack;
    ISOFieldMessage isoFieldMessage;
    TokenMessage tokenMessage;
    uChar codigoRespuesta[TAM_CODIGO_RESPUESTA + 1];
    uChar tokens[256 + 1];
    uChar mensajeError[21 + 1] = {0x00};
    int iRet = 0;

    memset(&resultadoUnpack, 0x00, sizeof(resultadoUnpack));
    memset(&resultTokenUnpack, 0x00, sizeof(resultTokenUnpack));
    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(&tokenMessage, 0x00, sizeof(tokenMessage));
    memset(codigoRespuesta, 0x00, sizeof(codigoRespuesta));
    memset(tokens, 0x00, sizeof(tokens));

    resultadoUnpack = unpackISOMessage(datosRecibidos, tam);

    isoFieldMessage = getField(39);

    memcpy(codigoRespuesta, isoFieldMessage.valueField, isoFieldMessage.totalBytes);
    memcpy(datosVentaBancolombia.codigoRespuesta, isoFieldMessage.valueField,
           isoFieldMessage.totalBytes);
    LOGI("respuesta de la consulta  % s", datosVentaBancolombia.codigoRespuesta);
    if (strcmp(codigoRespuesta, "00") == 0) {

        memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));

        isoFieldMessage = getField(60);
        memcpy(tokens, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

        if (isoFieldMessage.totalBytes > 0) {
            resultTokenUnpack = unpackTokenMessage(isoFieldMessage.valueField,
                                                   isoFieldMessage.totalBytes);
            tokenMessage = _getTokenMessage_((char *) "P6");

            if (tokenMessage.totalBytes > 0) {
                memcpy(tokenP6Recibido, tokenMessage.valueToken, tokenMessage.totalBytes);
                LOGI("tokenP6Recibido en unpackConsulta  % s", tokenP6Recibido);
                iRet = 1;
            }
        }
    } else {
        errorRespuestaTransaccion(codigoRespuesta, mensajeError);
        memcpy(mensajeDeclinada, mensajeError,strlen(mensajeError));
        //getMensajeError(codigoRespuesta, mensajeDeclinada);
    }
    return iRet;
}

void cargarIndicadoresDB(uChar *tokenRecibido, long *valorFactura, int *indicadorBaseDatos,
                         int *indicadorValor) {

    int idxRecibido = 0;

    uChar valor[TAM_P6_VALOR_FACTURA + 1];
    uChar indBaseDatos[TAM_P6_IND_BASE_DATOS + 1];
    uChar indValor[TAM_P6_IND_VALOR + 1];

    memset(valor, 0x00, sizeof(valor));
    memset(indBaseDatos, 0x00, sizeof(indBaseDatos));
    memset(indValor, 0x00, sizeof(indValor));

    idxRecibido += TAM_P6_IND_1;
    idxRecibido += TAM_P6_IND_2;
    idxRecibido += TAM_P6_NUM_RECIBO;
    idxRecibido += TAM_P6_NUM_REFERENCIA;
    LOGI("tokenRecibido en indicadores  %s  ", tokenRecibido);
    memcpy(datosVentaBancolombia.codigoComercio, tokenRecibido + idxRecibido, TAM_P6_COD_CONVENIO);
    LOGI("convenio Recibido %s  ", datosVentaBancolombia.codigoComercio);
    idxRecibido += TAM_P6_COD_CONVENIO;
    idxRecibido += TAM_P6_NOMBRE_CONVENIO;

    memcpy(indBaseDatos, tokenRecibido + idxRecibido, TAM_P6_IND_BASE_DATOS);
    *indicadorBaseDatos = atoi(indBaseDatos);
    LOGI("indicadorBaseDatos %s  ", indicadorBaseDatos);
    idxRecibido += TAM_P6_IND_BASE_DATOS;

    memcpy(indValor, tokenRecibido + idxRecibido, TAM_P6_IND_VALOR);
    *indicadorValor = atoi(indValor);
    LOGI("indValor %s  ", indicadorValor);
    idxRecibido += TAM_P6_IND_VALOR;

    memcpy(valor, tokenRecibido + idxRecibido, TAM_P6_VALOR_FACTURA - 2);
    LOGI("valor %s  ", valor);
    LOGI("tokenRecibido + idxRecibido %s  ", tokenRecibido + idxRecibido);
    *valorFactura = atol(valor);
    LOGI("valorFactura %ld  ", valorFactura);
}

void armarP6(DatosTokens *datosTokens, uChar *cuentaRecaudadora, uChar *TCRecaudadora,
             int tipoRecaudoManual,
             uChar *tarjetaEmpresarial) {

    uChar buffer[160 + 1];
    int indice = 0;

    uChar numeroRecibo[TAM_NUMERO_RECIBO + 1];
    uChar referencia[24 + 1];
    uChar referenciaAux[24 + 1];
    uChar informacionFactura[128 + 1];
    uChar codigoConvenio[5 + 1];
    uChar espacios[61 + 1];
    uChar cuentaRecaudadoraAux[24 + 1];

    memset(buffer, 0x00, sizeof(buffer));
    memset(numeroRecibo, 0x30, sizeof(numeroRecibo));
    memset(referencia, 0x30, sizeof(referencia)); //debe enviarse en ceros para diferentes a manual
    memset(informacionFactura, 0x00, sizeof(informacionFactura));
    memset(codigoConvenio, 0x00, sizeof(codigoConvenio));
    memset(espacios, 0x20, sizeof(espacios)); //se requiere enviar en 0x20
    memset(cuentaRecaudadoraAux, 0x00, sizeof(cuentaRecaudadoraAux));
    memset(referenciaAux, 0x00, sizeof(referenciaAux)); //bug_2707

    ///// Indica que no hay nada para el tokens P6
    if (strlen(datosTokens->indicador1) == 0 && strlen(datosTokens->indicador2) == 0) {
        return;
    }

    formatearDatos(datosTokens);

    cargarVariable(buffer, &indice, datosTokens->indicador1, SIZE_INDICADOR);
    cargarVariable(buffer, &indice, datosTokens->indicador2, SIZE_INDICADOR);
    cargarVariable(buffer, &indice, numeroRecibo, TAM_NUMERO_RECIBO);

    if (strcmp(datosTokens->indicador1, INDICADOR_MANUAL) == 0) {

        //en datosVentaBancolombia.tokenVivamos se encuentra la referencia digitada por el usuario.
        _rightPad_(referencia, datosVentaBancolombia.tokenVivamos, ' ', TAM_P6_NUM_REFERENCIA);
        if (tipoRecaudoManual == MANUAL_CUENTA_RECAUDADORA) {
            memcpy(codigoConvenio, "000000", 6);
        } else if (tipoRecaudoManual == MANUAL_CODIGO_CONVENIO) {
            memcpy(codigoConvenio, datosTokens->referencia2 + (SIZE_REFERENCIA - 5),
                   5); // los ultimos 5 digitos contienen el codigo del convenio
        }
    } else if (strcmp(datosTokens->indicador1, INDICADOR_LECTOR) == 0
               || strcmp(datosTokens->indicador1, INDICADOR_TEMPRESARIAL) == 0) {
        memcpy(codigoConvenio, datosTokens->referencia2 + (SIZE_REFERENCIA - 5),
               5); // los ultimos 5 digitos contienen el codigo del convenio
    }

    cargarVariable(buffer, &indice, referencia, TAM_P6_NUM_REFERENCIA);

    if (strcmp(datosTokens->indicador1, INDICADOR_TEMPRESARIAL) == 0) {
        leftPad(buffer + indice, tarjetaEmpresarial, '0', 128);
    } else if (strcmp(datosTokens->indicador1, INDICADOR_LECTOR) == 0) {
        leftPad(buffer + indice, tarjetaEmpresarial, ' ', 128);
    } else {
        cargarVariable(buffer, &indice, codigoConvenio, 5);
        cargarVariable(buffer, &indice, espacios, 45);
        cargarVariable(buffer, &indice, TCRecaudadora, TAM_TIPO_CUENTA);
        leftPad(cuentaRecaudadoraAux, cuentaRecaudadora, '0', 16);
        cargarVariable(buffer, &indice, cuentaRecaudadoraAux, 16);
        cargarVariable(buffer, &indice, espacios, 60);
    }

    memcpy(datosTokens->tokensp6, buffer, SIZE_TOKEN_P6);
}

void cargarVariable(uChar *buffer, int *indice, uChar *datos, int tam) {
    memcpy(buffer + *indice, datos, tam);
    *indice += tam;
}


int gestionarRecaudoTarjetaEmpresarial(char *track2Empresarial, char *monto) {

    int resultado = 1;
    int resultadoCaja = 0;
    int tipoCuentaCaja = 0;
    int tipoRecaudoManual = 0;
    DatosTokens datosToken;
    uChar transaccion[2 + 1];
    uChar cuentaRecaudadora[16 + 1];
    uChar TCRecaudadora[SIZE_CUENTA + 1];
    uChar mensajeDeclinada[30 + 1] = {0x00};

    DatosTarjetaSembrada datosTarjetaSembrada;

    memset(&datosVentaBancolombia, 0x00, sizeof(datosVentaBancolombia));
    memset(globalpinblock, 0x00, sizeof(globalpinblock));
    memset(&datosToken, 0x00, sizeof(datosToken));
    memset(transaccion, 0x00, sizeof(transaccion));

    ///// SE SETEA ANTES, PARA CUANDO VALLA A PERIFERICOS SEPA LA TRANSACCION PARA VERIFICAR LA TARJETA EMPRESARIAL
    sprintf(transaccion, "%02d", TRANSACCION_BCL_RECAUDO);
    setParameter(TIPO_TRANSACCION, transaccion);

    resultadoCaja = verificarHabilitacionCaja();

    /* if (resultadoCaja == TRUE) {
         if ((verificarCajaIPCB()) == 1) {
             resultadoCaja = 0;
         }
     }*/

    if (resultado > 0) {
        //////////// MONTO ////////////
        if (resultadoCaja == TRUE) {
            strcpy(datosVentaBancolombia.modoDatafono, "1");
            datosTarjetaSembrada = cargarTarjetaSembrada();
            memcpy(datosVentaBancolombia.track2, datosTarjetaSembrada.track2, TAM_TRACK2);
            memcpy(datosVentaBancolombia.cardName, datosTarjetaSembrada.cardName,
                   strlen(datosTarjetaSembrada.cardName));
            //resultado = transaccionPeticionDatosCaja(&datosVentaBancolombia, TRANSACCION_TEFF_RECAUDO_TEMPRESARIAL);
            if (resultado > 0) {
                tipoCuentaCaja = atol(datosVentaBancolombia.estadoTarifa);
                memcpy(datosVentaBancolombia.codigoRespuesta, "01", 2);
            }
        } else {
            //resultado = capturarDato("PAGO DE FACTURAS", "VALOR PAGO", "DE FACTURAS", 0, 8, DATO_MONTO, 1,
            //datosVentaBancolombia.totalVenta);
            memcpy(datosVentaBancolombia.totalVenta, monto, strlen(monto));
            resultado = 1;
        }
    }

    //cargarMensajeDeclinada(resultado, mensajeDeclinada);

    if (resultado <= 0) {

        if (verificarHabilitacionCaja() == TRUE) {
            memset(datosVentaBancolombia.codigoRespuesta, 0x00,
                   sizeof(datosVentaBancolombia.codigoRespuesta));
            memcpy(datosVentaBancolombia.codigoRespuesta, "01", 2);

            //transaccionRespuestaDatosCaja(datosVentaBancolombia,
                                         // TRANSACCION_TEFF_RECAUDO_TEMPRESARIAL);

            if (resultadoCaja == TRUE) {
                imprimirDeclinadas(datosVentaBancolombia, "TRANSACCION CANCELADA");
            }
        } else {
            imprimirDeclinadas(datosVentaBancolombia, mensajeDeclinada);
        }
    }
    ///// TENER EN CUENTA SI PIDE O NO PIN
    if (resultado > 0) {
        ///// DATOS PARA EL TOKEN P6
        strcpy(datosToken.indicador1, INDICADOR_TEMPRESARIAL);

        memset(datosToken.factura, 0x00, sizeof(datosToken.factura));
        memcpy(datosToken.factura, track2Empresarial, strlen(track2Empresarial));

        /////// SE DEBE ELIMINAR EL TRACK QUE SE CAPTURO YA QUE ESTE SE COLOCA EN EL CAMPO DE FACTURA PARA QUE EN EL P35 SE ENVIE LA TARJETA SEMBRADA
        memset(datosVentaBancolombia.track2, 0x00, sizeof(datosVentaBancolombia.track2));

        memset(cuentaRecaudadora, 0x00, sizeof(cuentaRecaudadora));
        memset(TCRecaudadora, 0x00, sizeof(TCRecaudadora));

        armarP6(&datosToken, cuentaRecaudadora, TCRecaudadora, tipoRecaudoManual, track2Empresarial);

        resultado = enviarRecaudo(datosToken, TRANSACCION_TEFF_RECAUDO_TEMPRESARIAL, datosVentaBancolombia.totalVenta);
    }


   // if ((resultado > 0)) {
        /*if (verificarHabilitacionCaja() == TRUE || (verificarCajaIPCB()) == 1) {
            memcpy(datosVentaBancolombia.codigoRespuesta, "00", 2);
            resultado = transaccionRespuestaDatosCaja(datosVentaBancolombia,
                                                      TRANSACCION_TEFF_RECAUDO_TEMPRESARIAL);
            if (resultado < 0 && verificarCajaIPCB() == 1) {
                memcpy(datosVentaBancolombia.codigoRespuesta, "WS", 2);
                imprimirDeclinadas(datosVentaBancolombia, "ERROR CON EL WS");
            }
        }*/
        //mostrarAprobacionBancolombia(&datosVentaBancolombia);
    //}

    return resultado;
}