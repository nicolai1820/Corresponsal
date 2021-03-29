//
// Created by NETCOM on 26/02/2020.
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

#define  LOG_TAG    "NETCOM_TRANSFERENCIA"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
int enviarTransferencia(DatosTarjetaAndroid datosTarjetaAndroid, char *tpCuentaOrg,char * TipoCuentaD, char * cuentaDestino,   char * otraCuenta,  char * monto) {

    int resultado = 0;
    int intentosVentas = 0;
    char data[50 + 1];
    uChar pan[20 + 1] = {0x00};
    int indice = 0;
    int tamPan = 0;
    char ultimos4[4 + 1] = {0x00};
    uChar numeroCuenta[SIZE_NUMERO_CUENTA + 1];
    DatosTarjeta datosTarjeta;
    TablaTresInicializacion dataIssuerEMV;

    memset(&datosTarjeta, 0x00, sizeof(datosTarjeta));
    memset(&dataIssuerEMV, 0x00, sizeof(dataIssuerEMV));

    memcpy(datosVentaBancolombia.tipoCuenta,datosTarjetaAndroid.tipoCuenta, 2);
    memcpy(datosVentaBancolombia.codigoGrupo,TipoCuentaD, 2);
    memcpy(datosVentaBancolombia.totalVenta,monto, 12);

    leftPad(datosVentaBancolombia.textoAdicional, cuentaDestino, '0', SIZE_CUENTA);

    memcpy(datosVentaBancolombia.track2, datosTarjetaAndroid.track2, strlen(datosTarjetaAndroid.track2));
    memcpy(datosVentaBancolombia.totalVenta, monto, 12);
    LOGI("otraCuenta %d",otraCuenta);
    if (strcmp(otraCuenta,"00000") != 0) {
        ///// CAPTURAR DATOS DE CUENTA DESTINO ////
        memset(datosVentaBancolombia.tipoCuenta, 0x00, sizeof(datosVentaBancolombia.tipoCuenta));
        memset(datosVentaBancolombia.nombreComercio, 0x00, sizeof(datosVentaBancolombia.nombreComercio));

        memcpy(datosVentaBancolombia.tipoCuenta, datosTarjetaAndroid.tipoCuenta, 2);
        leftPad(datosVentaBancolombia.nombreComercio, otraCuenta, '0', SIZE_CUENTA);
    } else {
        memcpy(datosVentaBancolombia.tipoCuenta, datosTarjetaAndroid.tipoCuenta, 2);
    }
    if(strlen(datosTarjetaAndroid.data55) > 0){
        _convertASCIIToBCD_(globalCampo55,datosTarjetaAndroid.data55,strlen(datosTarjetaAndroid.data55));
        memcpy(datosVentaBancolombia.posEntrymode, (char*)"051", 3);
        memcpy(datosVentaBancolombia.AID, datosTarjetaAndroid.aid, strlen(datosTarjetaAndroid.aid));
        memcpy(datosVentaBancolombia.ARQC, datosTarjetaAndroid.arqc,strlen(datosTarjetaAndroid.arqc));
        _convertASCIIToBCD_(datosVentaBancolombia.appLabel,datosTarjetaAndroid.apLabel,strlen(datosTarjetaAndroid.apLabel));
        dataIssuerEMV = traerIssuerCNB(pan, datosVentaBancolombia.track2);
        memcpy(datosVentaBancolombia.ultimosCuatro, (pan + strlen(pan)) - 4, 4);
        comportamientoTarjetaLabelEMV(&datosVentaBancolombia, dataIssuerEMV);
        globalsizep55 = strlen(datosTarjetaAndroid.data55)/2;
    } else {
        tamPan = strlen(datosTarjetaAndroid.pan);
        memcpy(ultimos4, datosTarjetaAndroid.pan + tamPan - 4, 4);
        memcpy(datosVentaBancolombia.ultimosCuatro, ultimos4, 4 );
        //datosTarjeta = detectarTrack2(datosTarjetaAndroid.track2);
        //validarFallBack(&datosVentaBancolombia, &datosTarjeta);
    }

    if(datosTarjetaAndroid.isPin > 0){
        _convertASCIIToBCD_(globalpinblock,datosTarjetaAndroid.pinblock,strlen(datosTarjetaAndroid.pinblock));
    }
    sprintf(datosVentaBancolombia.processingCode, "41%2s00", datosVentaBancolombia.tipoCuenta);
    sprintf(datosVentaBancolombia.tipoTransaccion, "%02d", TRANSACCION_BCL_TRANSFERENCIA);
    do {
        armarTramaTransferencia(intentosVentas);

        resultado = procesarTransaccionTransferencia();

        if (resultado > 0) {
            memset(data, 0x00, sizeof(data));

            extraerTokens((char*)"QF", data);

            //// Si no llega el tokens, entonces debe imprimir lo que se capturo en la cuenta destino
            if (strlen(data) > 0) {
                ////// SE FRAGMENTA EL TOKEN
                //////// DATOS DE LA CUENTA ORIGEN
                indice += TAM_TIPO_CUENTA; // No se toman por que en la respuesta este dato no vienen en esta posicion.

                ////// segun app de producccion el tipo de cta y la cuenta van en el mismo campo
                memset(datosVentaBancolombia.nombreComercio, 0x00, sizeof(datosVentaBancolombia.nombreComercio));
                memcpy(datosVentaBancolombia.nombreComercio, data + indice, SIZE_NUMERO_CUENTA);
                indice += SIZE_NUMERO_CUENTA;

                //////// DATOS DE LA CUENTA DESTINO
                indice += TAM_TIPO_CUENTA; // No se toman por que en la respuesta este dato no vienen en esta posicion.

                memset(datosVentaBancolombia.textoAdicional, 0x00, sizeof(datosVentaBancolombia.textoAdicional));
                memcpy(datosVentaBancolombia.textoAdicional, data + indice, SIZE_NUMERO_CUENTA);
                indice += SIZE_NUMERO_CUENTA;
            }

            else {
                memset(numeroCuenta, 0x00, sizeof(numeroCuenta));
                memcpy(numeroCuenta, datosVentaBancolombia.nombreComercio, SIZE_CUENTA);

                memset(datosVentaBancolombia.nombreComercio, 0x00, sizeof(datosVentaBancolombia.nombreComercio));
                leftPad(datosVentaBancolombia.nombreComercio, numeroCuenta, '0', SIZE_NUMERO_CUENTA);

                memset(numeroCuenta, 0x00, sizeof(numeroCuenta));
                memcpy(numeroCuenta, datosVentaBancolombia.textoAdicional, SIZE_CUENTA);

                memset(datosVentaBancolombia.textoAdicional, 0x00, sizeof(datosVentaBancolombia.textoAdicional));
                leftPad(datosVentaBancolombia.textoAdicional, numeroCuenta, '0', SIZE_NUMERO_CUENTA);
            }

        }

        if (resultado == -2) {
            intentosVentas++;
        }

        if (intentosVentas == MAX_INTENTOS_VENTA) {
            //screenMessage("COMUNICACION", "REINTENTE", "TRANSACCION", "", 2 * 1000);

            //responderCajaTransferenciaDeclinada();
            //imprimirDeclinadaTransferencia("REINTENTE TRANSACCION");
            resultado = -1;
        }

    } while (resultado == -2 && intentosVentas < MAX_INTENTOS_VENTA);

    if (strlen(datosVentaBancolombia.ARQC) > 0) { // Solo entra si es CHIP
        // memset(bufferAuxiliarCustomEMV, 0x00, sizeof(bufferAuxiliarCustomEMV));

        globalresultadoUnpack = unpackISOMessage(globalDataRecibida, globalsizeData);

        //cargaTagsRespuestaTransacionEMV(bufferAuxiliarCustomEMV, &sizeCustomEMV, globalresultadoUnpack, resultado);
        if (resultado == 1) {
            globalTransaccionAprobada = TRUE;
        }
    }
    if(resultado > 0  && datosTarjetaAndroid.sizeP55 == 0){
        LOGI("Aprobado  EMV  %s ", datosVentaBancolombia.codigoAprobacion);
        mostrarAprobacionBancolombia(&datosVentaBancolombia);
    } else if(resultado > 0 && datosTarjetaAndroid.sizeP55 > 0 ){
        preAprobacionEmv();
    }
    setVariableGlobalesRetiro();
    return resultado;
}

void armarTramaTransferencia(int intentosVentas) {

    int indice = 0;
    int longitud = 0;

    ResultTokenPack resultTokenPack;
    DatosTarjetaSembrada datosTarjetaSembrada;
    DatosTokens datosToken;
    char tokenQZ[81 + 1];

    memset(&resultTokenPack, 0x00, sizeof(resultTokenPack));
    memset(&datosTarjetaSembrada, 0x30, sizeof(datosTarjetaSembrada));
    memset(&datosToken, 0x00, sizeof(datosToken));
    memset(tokenQZ, 0x00, sizeof(tokenQZ));

    ///// DATOS PARA EL TOKEN R3
    strcpy(datosToken.valorKotenR3, TOKEN_TRANSFERENCIA);

    memset(datosVentaBancolombia.iva, 0x00, sizeof(datosVentaBancolombia.iva));
    strcpy(datosVentaBancolombia.iva, datosToken.valorKotenR3);

    datosTarjetaSembrada = cargarTarjetaSembrada();

    memcpy(datosVentaBancolombia.msgType, "0200", TAM_MTI);

    empaquetarEncabezado(&datosVentaBancolombia, (char*)"", intentosVentas);
    longitud = _armarTokenAutorizacion_(tokenQZ);

    ///// DATOS PARA EL TOKEN QF

    ////////  SE EMPAQUETA LOS DATOS DE LA CUENTA ORIGEN /////
    memset(datosToken.tipoCuentaOrigen, 0x00, sizeof(datosToken.tipoCuentaOrigen));
    memcpy(datosToken.tipoCuentaOrigen, datosVentaBancolombia.tipoCuenta, SIZE_TIPO_CUENTA);
    LOGI("datosToken.tipoCuentaOrigen %s",datosToken.tipoCuentaOrigen);
    memset(datosToken.cuentaOrigen, 0x00, sizeof(datosToken.cuentaOrigen));
    memcpy(datosToken.cuentaOrigen, datosVentaBancolombia.nombreComercio, SIZE_CUENTA);

    ////////  SE EMPAQUETA LOS DATOS DE LA CUENTA DESTINO /////
    memset(datosToken.tipoCuentaDestino, 0x00, sizeof(datosToken.tipoCuentaDestino));
    memcpy(datosToken.tipoCuentaDestino, datosVentaBancolombia.codigoGrupo, SIZE_TIPO_CUENTA);
    LOGI("datosToken.tipoCuentaDestino %s",datosToken.tipoCuentaDestino);
    memset(datosToken.cuentaDestino, 0x00, sizeof(datosToken.cuentaDestino));
    memcpy(datosToken.cuentaDestino, datosVentaBancolombia.textoAdicional, SIZE_CUENTA);

    ///// DATOS PARA EL TOKEN P6
    strcpy(datosToken.indicador2, INDICADOR_CTA_CNB);
    memcpy(datosToken.referencia1, datosTarjetaSembrada.tipoCuenta, TAM_TIPO_CUENTA);
    indice += TAM_TIPO_CUENTA;

    memcpy(datosToken.referencia1 + indice, datosTarjetaSembrada.pan, 16);

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
            setField(53, (char*)SEGURIDAD_PIN_EMV, 16);
        }
    }
    setField(58, tokenQZ, longitud);
}

int procesarTransaccionTransferencia(void) {

    ResultISOPack resultadoIsoPackMessage;
    int resultadoTransaccion = -1;
    DatosTransaccionDeclinada datosTransaccionDeclinada;
    int resultadoCaja = 0;

    resultadoCaja = verificarHabilitacionCaja();

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
            resultadoTransaccion = unpackDataTransferencia();
        } else {

            //responderCajaTransferenciaDeclinada();

            if (resultadoTransaccion == -3) {
                //imprimirDeclinadaTransferencia("TIME OUT GENERAL");
            } else {
                //imprimirDeclinadaTransferencia("ERROR SIN CONEXION");
            }
            resultadoTransaccion = 0;
        }
    }

    return resultadoTransaccion;
}

int unpackDataTransferencia(void) {

    ISOFieldMessage isoFieldMessage;
    uChar codigoRespuesta[2 + 1];
    int resultado = 2;
    uChar mensajeError[21 + 1];
    int resultadoCaja = 0;
    DatosCnbBancolombia datosTransferencia;

    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(&codigoRespuesta, 0x00, sizeof(codigoRespuesta));
    memset(&datosTransferencia, 0x00, sizeof(datosTransferencia));
    memset(mensajeError, 0x00, sizeof(mensajeError));

    resultadoCaja = verificarHabilitacionCaja();

    /// VALIDACIONES TRANSACCION ///
    globalresultadoUnpack = unpackISOMessage(globalDataRecibida, globalsizeData);

    isoFieldMessage = getField(39);
    memcpy(codigoRespuesta, isoFieldMessage.valueField, isoFieldMessage.totalBytes);
    memcpy(datosVentaBancolombia.codigoRespuesta, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

    if (strcmp(codigoRespuesta, "00") == 0) {
        memcpy(datosTransferencia.processingCode,datosVentaBancolombia.processingCode,sizeof(datosVentaBancolombia.processingCode));
        memcpy(datosTransferencia.systemTrace,datosVentaBancolombia.systemTrace,sizeof(datosVentaBancolombia.systemTrace));
        memcpy(datosTransferencia.terminalId,datosVentaBancolombia.terminalId,sizeof(datosVentaBancolombia.terminalId));
        resultado = verificacionTramabancolombia(&datosTransferencia);

        if (resultado > 0) {
            resultado = 1;
        } else if (resultado < 1) {
            resultado = generarReverso();

            if (verificarHabilitacionCaja() == TRUE && (resultado == -3 || resultado == -1)) {
                memcpy(datosVentaBancolombia.codigoRespuesta, "01", 2);
                //transaccionRespuestaDatosCaja(datosVentaBancolombia, TRANSACCION_TEFF_TRANSFERENCIA_BCL);
                //procesoCajaActivo = FALSE;
            }

            if (resultado == -3) {
                //imprimirDeclinadaTransferencia("TIME OUT GENERAL");
            } else if (resultado == -1) {
                //imprimirDeclinadaTransferencia("REVERSO PENDIENTE");
            }

            if (resultado > 0) {
                resultado = -2;
            }
        }
    } else {
        borrarArchivo(discoNetcom, (char*)ARCHIVO_REVERSO);
        errorRespuestaTransaccion(codigoRespuesta, mensajeError);
        resultado = 0;

        if (strcmp(codigoRespuesta, "55") == 0) {
            resultado = -4;
        } else if (strcmp(codigoRespuesta, "75") == 0) {
            resultado = -3;
        }

        //// SI LA RESPUESTA ES 55 O 75 Y ES DEFERENTE DE CHIP, ENTONCES NO DEBE IMPRIMIR
        if ((resultado == -4) && (strlen(datosVentaBancolombia.AID) == 0)) {
            return resultado;
        }

        // getMensajeError(codigoRespuesta, mensajeError);

        // responderCajaTransferenciaDeclinada();
        imprimirDeclinadas(datosVentaBancolombia, mensajeError);
    }

    return resultado;
}