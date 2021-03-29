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

#define  LOG_TAG    "NETCOM_SALDO_CNBBCL"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
char globalCampo55[1024 + 1];
char globalpinblock[16 + 1];
int globalsizep55;

int enviarTransaccionSaldoBCL(DatosTarjetaAndroid datosTarjetaAndroid, int intentosPin,
                              char *codigoRespuesta) {

    int resultado = 0;
    int intentosVentas = 0;
    int tamPan = 0;
    char ultimos4[4 + 1] = {0x00};
    uChar pan[20 + 1] = {0x00};

    DatosTarjeta datosTarjeta;
    TablaTresInicializacion dataIssuerEMV;

    memset(&datosTarjeta, 0x00, sizeof(datosTarjeta));
    memset(&dataIssuerEMV, 0x00, sizeof(dataIssuerEMV));

    memcpy(datosVentaBancolombia.track2, datosTarjetaAndroid.track2,
           strlen(datosTarjetaAndroid.track2));
    LOGI("tam 55 %d",datosTarjetaAndroid.sizeP55);
    if (datosTarjetaAndroid.sizeP55 > 0) {
        _convertASCIIToBCD_(globalCampo55,datosTarjetaAndroid.data55,(datosTarjetaAndroid.sizeP55));
        globalsizep55 = datosTarjetaAndroid.sizeP55/2;

        //memcpy(globalCampo55, datosTarjetaAndroid.data55, datosTarjetaAndroid.sizeP55);
        memcpy(datosVentaBancolombia.posEntrymode, (char *) "051", 3);
        memcpy(datosVentaBancolombia.AID, datosTarjetaAndroid.aid, strlen(datosTarjetaAndroid.aid));
        memcpy(datosVentaBancolombia.ARQC, datosTarjetaAndroid.arqc,
               strlen(datosTarjetaAndroid.arqc));
        _convertASCIIToBCD_(datosVentaBancolombia.appLabel, datosTarjetaAndroid.apLabel,
                            strlen(datosTarjetaAndroid.apLabel));
        dataIssuerEMV = traerIssuerCNB(pan, datosVentaBancolombia.track2);
        memcpy(datosVentaBancolombia.ultimosCuatro, (pan + strlen(pan)) - 4, 4);
        memcpy(datosVentaBancolombia.tipoCuenta, datosTarjetaAndroid.tipoCuenta,strlen(datosTarjetaAndroid.tipoCuenta));
        comportamientoTarjetaLabelEMV(&datosVentaBancolombia, dataIssuerEMV);
    } else {
        tamPan = strlen(datosTarjetaAndroid.pan);
        memcpy(ultimos4, datosTarjetaAndroid.pan + tamPan - 4, 4);
        memcpy(datosVentaBancolombia.ultimosCuatro, ultimos4, 4 );
        //datosTarjeta = detectarTrack2(datosTarjetaAndroid.track2);
        //validarFallBack(&datosVentaBancolombia, &datosTarjeta);
    }
    if (datosTarjetaAndroid.isPin > 0) {
        _convertASCIIToBCD_(globalpinblock,datosTarjetaAndroid.pinblock,strlen(datosTarjetaAndroid.pinblock));
    }

    getTerminalId(datosVentaBancolombia.terminalId);
    sprintf(datosVentaBancolombia.processingCode, "31%2s00",datosTarjetaAndroid.tipoCuenta);

    sprintf(datosVentaBancolombia.tipoTransaccion, "%02d", TRANSACCION_BCL_SALDO);
    resultado = enviarConsultarSaldo(globalpinblock, intentosVentas);

    /*if(resultado > 0  && datosTarjetaAndroid.sizeP55 == 0){
        LOGI("Aprobado  EMV  %s ", datosVentaBancolombia.codigoAprobacion);
        mostrarAprobacionBancolombia(&datosVentaBancolombia);
        memcpy(codigoRespuesta, datosVentaBancolombia.codigoRespuesta, 6);
    } else {
        memcpy(codigoRespuesta, datosVentaBancolombia.codigoRespuesta, 6);
        LOGI("PreAprobado  EMV  %s ", datosVentaBancolombia.codigoAprobacion);
        preAprobacionEmv();
    }
    setParameter(TIPO_INICIA_TRANSACCION, (char *) "00");
    setVariableGlobalesRetiro();*/
    return resultado;
}

void armarTrama(char *pinblock, int intentosVenta, int intentosPin) {

    int longitud = 0;

    ISOHeader isoHeader8583;
    uChar nii[TAM_NII + 1];
    uChar p59[16];
    uChar auxiliar[12 + 1];
    uChar buffer[2 + 1];
    uChar tokens[40 + 1];
    ResultTokenPack resultTokenPack;
    DatosTokens datosToken;
    uChar tokenQZ[81 + 1];
    DatosTarjetaSembrada datosTarjetaSembrada;

    memset(&isoHeader8583, 0x00, sizeof(isoHeader8583));
    memset(nii, 0x00, sizeof(nii));
    memset(p59, 0x30, sizeof(p59));
    memset(auxiliar, 0x30, sizeof(auxiliar));
    memset(buffer, 0x00, sizeof(buffer));
    memset(tokens, 0x00, sizeof(tokens));
    memset(&resultTokenPack, 0x00, sizeof(resultTokenPack));
    memset(&datosToken, 0x00, sizeof(datosToken));
    memset(tokenQZ, 0x00, sizeof(tokenQZ));
    memset(&datosTarjetaSembrada, 0x00, sizeof(datosTarjetaSembrada));

    getParameter(NII, nii);

    if (intentosVenta == 0) {
        generarSystemTraceNumber(datosVentaBancolombia.systemTrace, (char *) "");
        generarInvoice(datosVentaBancolombia.numeroRecibo);
    }

    memcpy(datosVentaBancolombia.msgType, "0100", TAM_MTI);
    memcpy(datosVentaBancolombia.nii, nii + 1, TAM_NII);
    memcpy(datosVentaBancolombia.posConditionCode, "00", TAM_POS_CONDITION);
    longitud = _armarTokenAutorizacion_(tokenQZ);

    if (intentosPin == -1) {
        datosTarjetaSembrada = cargarTarjetaSembrada();
        memcpy(datosVentaBancolombia.track2, datosTarjetaSembrada.track2,
               sizeof(datosVentaBancolombia.track2));
    }


    isoHeader8583.TPDU = 60;
    memcpy(isoHeader8583.destination, nii, TAM_NII + 1);
    memcpy(isoHeader8583.source, nii, TAM_NII + 1);
    setHeader(isoHeader8583);

    setMTI(datosVentaBancolombia.msgType);

    setField(3, datosVentaBancolombia.processingCode, TAM_CODIGO_PROCESO);
    setField(11, datosVentaBancolombia.systemTrace, TAM_SYSTEM_TRACE);
    setField(22, datosVentaBancolombia.posEntrymode, TAM_ENTRY_MODE);
    setField(24, datosVentaBancolombia.nii, TAM_NII);
    setField(25, datosVentaBancolombia.posConditionCode, TAM_POS_CONDITION);
    setField(35, datosVentaBancolombia.track2, strlen(datosVentaBancolombia.track2));
    setField(41, datosVentaBancolombia.terminalId, TAM_TERMINAL_ID);

    if (intentosPin != -1) {
        if (strlen(pinblock) > 0) {
            setField(52, pinblock, LEN_PIN_DATA);
        }
    }

    if (globalsizep55 > 0) {
        setField(55, globalCampo55, globalsizep55);
        setField(4, auxiliar, 12);
        if (strlen(pinblock) < 0) {
            setField(53, (char *) SEGURIDAD_PIN_EMV, 16);
        }
    }

    if (intentosPin == -1) {
        strcpy(datosToken.valorKotenR3, TOKEN_CONSULTA_CUPO);

    } else {
        strcpy(datosToken.valorKotenR3, TOKEN_SALDO);
    }

    memset(datosVentaBancolombia.iva, 0x00, sizeof(datosVentaBancolombia.iva));
    strcpy(datosVentaBancolombia.iva, datosToken.valorKotenR3);


    strcpy(datosToken.tipoCuentaOrigen, datosVentaBancolombia.tipoCuenta);
    strcpy(datosToken.indicador2, INDICADOR_CTA_CNB);
    setField(58, tokenQZ, longitud);
    empaquetarP60(datosToken, datosVentaBancolombia.tipoTransaccion);

    //// SE RECUPERA EL CAMPO 60
    resultTokenPack = _packTokenMessage_();

    setField(60, resultTokenPack.tokenPackMessage, resultTokenPack.totalBytes);
}

int procesarTransaccion(void) {

    //ResultISOPack resultadoIsoPackMessage;
    int resultadoTransaccion = -1;

    memset(&globalresultadoIsoPack, 0x00, sizeof(globalresultadoIsoPack));
    //memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
    memset(globalDataRecibida, 0x00, sizeof(globalsizeData));

    globalresultadoIsoPack = packISOMessage();

    /*if (resultadoIsoPackMessage.responseCode > 0) {

        resultadoTransaccion = realizarTransaccion(resultadoIsoPackMessage.isoPackMessage,
                                                   resultadoIsoPackMessage.totalBytes,
                                                   globalDataRecibida, TRANSACCION_BCL_SALDO,
                                                   CANAL_DEMANDA,
                                                   REVERSO_NO_GENERADO);
        LOGI("resultadoTransaccion %d ", resultadoTransaccion);

        if (resultadoTransaccion > 0) {
            globalsizeData = resultadoTransaccion;
            // desarmar la trama
            resultadoTransaccion = unpackRespuestaSaldo();
        } else {
            if (resultadoTransaccion == -3) {
                imprimirDeclinadas(datosVentaBancolombia, (char *) "TIME OUT GENERAL");
            }

            resultadoTransaccion = 0;
        }
    }*/

    return resultadoTransaccion;
}

int unpackRespuestaSaldo(void) {

    ISOFieldMessage isoFieldMessage;
    uChar codigoRespuesta[2 + 1];
    uChar saldoDisponible[15 + 1];
    int resultado = 2;
    uChar mensajeError[21 + 1];

    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(&codigoRespuesta, 0x00, sizeof(codigoRespuesta));
    memset(saldoDisponible, 0x00, sizeof(saldoDisponible));
    memset(mensajeError, 0x00, sizeof(mensajeError));

    /// VALIDACIONES TRANSACCION ///
    globalresultadoUnpack = unpackISOMessage(globalDataRecibida, globalsizeData);

    isoFieldMessage = getField(39);
    memcpy(codigoRespuesta, isoFieldMessage.valueField, isoFieldMessage.totalBytes);
    //se coloca codigo de respuesta para cajas
    memcpy(datosVentaBancolombia.codigoRespuesta, isoFieldMessage.valueField,
           isoFieldMessage.totalBytes);
    if (strcmp(codigoRespuesta, "00") == 0) {
        resultado = 1;

        isoFieldMessage = getField(4);

        memset(&datosVentaBancolombia.totalVenta, 0x00, sizeof(datosVentaBancolombia.totalVenta));
        leftTrim(datosVentaBancolombia.totalVenta, isoFieldMessage.valueField, '0');
    } else {
        borrarArchivo(discoNetcom, (char *) ARCHIVO_REVERSO);
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

        //getMensajeError(codigoRespuesta, mensajeError);
        imprimirDeclinadas(datosVentaBancolombia, mensajeError);
    }

    isoFieldMessage = getField(60);
    if (isoFieldMessage.totalBytes > 0) {
        verificarTokenQi(isoFieldMessage.valueField, isoFieldMessage.totalBytes);
    }

    return resultado;
}

int gestionarConsultaCupoCB(char *respuesta) {
    int iRet = 0;
    obtenerTerminalModoCorresponsal(datosVentaBancolombia.terminalId);
    memcpy(datosVentaBancolombia.codigoRespuesta, "01", 2);
    memcpy(datosVentaBancolombia.processingCode, "311000", 6);
    memcpy(datosVentaBancolombia.posEntrymode, "021", 3);
    sprintf(datosVentaBancolombia.tipoTransaccion, "%d", TRANSACCION_BCL_SALDO);
    iRet = enviarConsultarSaldo(NULL, -1);
    LOGI("envioConsulta %d",iRet);
    if (iRet > 0) {
        LOGI("A mostrarCupoCb");
        mostrarCupoCb(respuesta);
        LOGI("salio de mostrarCupoCb");
        memset(datosVentaBancolombia.codigoRespuesta, 0x00,
               sizeof(datosVentaBancolombia.codigoRespuesta));
        memcpy(datosVentaBancolombia.codigoRespuesta, "00", 2);
        strcat(datosVentaBancolombia.compraNeta, "00");
        strcat(datosVentaBancolombia.baseDevolucion, "00");
    }
    if (verificarHabilitacionCaja() == TRUE) {
        //transaccionRespuestaDatosCaja(datosVentaBancolombia, TRANSACCION_TEFF_CONSULTA_CUPO_BCL);
    }
    return iRet;
}

int enviarConsultarSaldo(char *pinblock, int intentosPin) {

    int resultado = 0;
    int intentosVentas = 0;
   // do {
        armarTrama(pinblock, intentosVentas, intentosPin);
        resultado = procesarTransaccion();

   // } while (resultado == -2 && intentosVentas < MAX_INTENTOS_VENTA);
    /*if (strlen(datosVentaBancolombia.ARQC) > 0) { // Solo entra si es CHIP
        //memset(bufferAuxiliarCustomEMV, 0x00, sizeof(bufferAuxiliarCustomEMV));
        //cargaTagsRespuestaTransacionEMV(bufferAuxiliarCustomEMV, &sizeCustomEMV, globalresultadoUnpack, resultado);
        LOGI("entro condicion ARQC %s",datosVentaBancolombia.ARQC);

        if (resultado == 1) {
            globalTransaccionAprobada = TRUE;
        }
    }*/
    return resultado;
}

void mostrarCupoCb(char *respuesta) {
    long valor = 0;

    uChar linea1[24 + 1];
    uChar linea2[24 + 1];
    uChar linea3[24 + 1];
    uChar linea4[24 + 1];
    uChar valorFormato[12 + 1];
    uChar lineaResumen[100 + 1] = {0x00};
    DatosCnbBancolombia datosBancolombia;

    memset(linea1, 0x00, sizeof(linea1));
    memset(linea2, 0x00, sizeof(linea2));
    memset(linea3, 0x00, sizeof(linea3));
    memset(linea4, 0x00, sizeof(linea4));
    memset(valorFormato, 0x00, sizeof(valorFormato));
    memset(&datosBancolombia, 0x00, sizeof(datosBancolombia));

    valor = atoi(datosVentaBancolombia.compraNeta);

    if (valor > 0) {
        formatString(0x30, valor, "", 0, valorFormato, 2);

        memcpy(linea1, "SALDO UTILIZADO", 15);
        sprintf(linea2, "+%s", valorFormato);
        memcpy(linea3, "CUPO DISPONIBLE", 15);
        memset(valorFormato, 0x00, sizeof(valorFormato));
        LOGI("A linea1 % s",linea1);
        valor = atoi(datosVentaBancolombia.baseDevolucion);
        formatString(0x00, valor, "", 0, valorFormato, 2);

        sprintf(linea4, "-%s", valorFormato);
        LOGI("A linea4 % s",linea4);
        strcpy(lineaResumen,"CONSULTA CUPO CB;");
        strcat(lineaResumen, linea1);
        strcat(lineaResumen, linea2);
        strcat(lineaResumen, ";");
        strcat(lineaResumen, linea3);
        strcat(lineaResumen, linea4);
        LOGI("Envio String % s",lineaResumen);
        memcpy(respuesta, lineaResumen, strlen(lineaResumen));
        convertirDatosBancolombia(datosVentaBancolombia, &datosBancolombia);
        memset(datosBancolombia.tipoProducto, 0x00, sizeof(datosBancolombia.tipoProducto));
        memcpy(datosBancolombia.tipoProducto, "32", 2);
        memcpy(datosBancolombia.numeroRecibo, datosVentaBancolombia.numeroRecibo,
               sizeof(datosVentaBancolombia.numeroRecibo));

        guardarTransaccionBancolombia(discoNetcom, JOURNAL_CNB_BANCOLOMBIA, datosBancolombia);

        _guardarDirectorioJournals_(TRANS_CNB_BANCOLOMBIA, datosBancolombia.numeroRecibo,
                                    datosBancolombia.estadoTransaccion);
    }
    LOGI("salio de metodo % s",lineaResumen);
}