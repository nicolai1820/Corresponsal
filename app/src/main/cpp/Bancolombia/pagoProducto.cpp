//
// Created by NETCOM on 13/02/2020.
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

ResultISOUnpack globalresultadoUnpack;

int enviarTransPago(char *referencia,char *monto, int tipoProducto) {

    int maximo = 0;
    int resultado = 0;
    int intentosVentas = 0;

    char datoAuxiliar[SIZE_TARJETA_CREDITO + 1] = {0x00};
    setVariableGlobalesRetiro();
    DatosCnbBancolombia datosPagoProducto;

    memset(&datosPagoProducto, 0x00, sizeof(datosPagoProducto));
    memset(&datosVentaBancolombia, 0x00, sizeof(datosVentaBancolombia));

    memcpy(datosVentaBancolombia.totalVenta,monto, 12);

    if (tipoProducto == 1) {
        maximo = SIZE_TARJETA_CREDITO;
        sprintf(datosVentaBancolombia.tipoTransaccion, "%02d", TRANSACCION_BCL_PAGO_TARJETA);
        memcpy(datosPagoProducto.tipoProducto,TOKEN_PAGO_TARJETA,2);
    } else if (tipoProducto == 2) {
        maximo = SIZE_CARTERA;
        sprintf(datosVentaBancolombia.tipoTransaccion, "%02d", TRANSACCION_BCL_PAGO_CARTERA);
        memcpy(datosPagoProducto.tipoProducto,TOKEN_PAGO_CARTERA,2);
    }

    memcpy(datoAuxiliar, referencia, strlen(referencia));
    leftPad(datosVentaBancolombia.tokenVivamos, datoAuxiliar, '0', maximo);

    do {
        armarTramaPagoProducto(datosPagoProducto,intentosVentas);

        resultado = procesarTransaccionPagoProducto(datosPagoProducto);

        if (resultado == -2) {
            intentosVentas++;
        }

        if (intentosVentas == 3) {

            if (verificarHabilitacionCaja()) {

                memset(datosVentaBancolombia.codigoRespuesta, 0x00, sizeof(datosVentaBancolombia.codigoRespuesta));
                memcpy(datosVentaBancolombia.codigoRespuesta, "01", 2);
                //transaccionRespuestaDatosCaja(datosVentaBancolombia, establecerTipoPagoCaja(tipoProducto));
            }

            resultado = -1;
        }

    } while (resultado == -2 && intentosVentas < 3);

    if(resultado > 0){
        mostrarAprobacionBancolombia(&datosVentaBancolombia);
    }
    return resultado;
}

void armarTramaPagoProducto(DatosCnbBancolombia datosPagoProducto,int intentosVentas) {

    int longitud = 0;

    ResultTokenPack resultTokenPack;
    DatosTarjetaSembrada datosTarjetaSembrada;
    DatosTokens datosToken;
    char indicador[SIZE_INDICADOR + 1];
    char tokenQZ[81 + 1];

    memset(&resultTokenPack, 0x00, sizeof(resultTokenPack));
    memset(&datosTarjetaSembrada, 0x30, sizeof(datosTarjetaSembrada));
    memset(&datosToken, 0x00, sizeof(datosToken));
    memset(indicador, 0x00, sizeof(indicador));
    memset(tokenQZ, 0x00, sizeof(tokenQZ));

    ///// DATOS PARA EL TOKEN R3
    strcpy(datosToken.valorKotenR3, datosPagoProducto.tipoProducto);

    memset(datosVentaBancolombia.iva, 0x00, sizeof(datosVentaBancolombia.iva));
    strcpy(datosVentaBancolombia.iva, datosPagoProducto.tipoProducto);

    if (strcmp(datosPagoProducto.tipoProducto, TOKEN_PAGO_TARJETA) == 0) {
        strcpy(indicador, INDICADOR_TCREDITO);
    } else {
        strcpy(indicador, INDICADOR_CBANCO);
    }

    datosTarjetaSembrada = cargarTarjetaSembrada();
    longitud = _armarTokenAutorizacion_(tokenQZ);
    memcpy(datosVentaBancolombia.msgType, "0200", TAM_MTI);
    strcpy(datosVentaBancolombia.posEntrymode, "021");

    memset(datosVentaBancolombia.tipoCuenta, 0x00, sizeof(datosVentaBancolombia.tipoCuenta));
    memset(datosVentaBancolombia.processingCode, 0x00, sizeof(datosVentaBancolombia.processingCode));

    strcpy(datosVentaBancolombia.tipoCuenta, datosTarjetaSembrada.tipoCuenta);
    sprintf(datosVentaBancolombia.processingCode, "57%s00", datosVentaBancolombia.tipoCuenta);

    empaquetarEncabezado(&datosVentaBancolombia, datosTarjetaSembrada.pan, intentosVentas);

    ///// DATOS PARA EL TOKEN QF
    strcpy(datosToken.tipoCuentaOrigen, datosTarjetaSembrada.tipoCuenta); //// TIPO DE CUENTA SEMBRADA
    strcpy(datosToken.cuentaOrigen, datosTarjetaSembrada.pan); //// PAN DE LA TARJETA SEMBRADA

    ///// DATOS PARA EL TOKEN P6
    strcpy(datosToken.indicador1, INDICADOR_MANUAL);
    strcpy(datosToken.indicador2, indicador);

    memset(datosToken.factura, 0x00, sizeof(datosToken.factura));
    memcpy(datosToken.factura, datosPagoProducto.cuentaDestino, SIZE_REFERENCIA);
    memcpy(datosPagoProducto.tipoTransaccion, datosVentaBancolombia.tipoTransaccion,strlen(datosVentaBancolombia.tipoTransaccion));
    ///// EMPAQUETA EL CAMPO 60 CON LOS DATOS DE DATOSTOKENS
    empaquetarP60(datosToken, datosPagoProducto.tipoTransaccion);

    //// SE RECUPERA EL CAMPO 60
    resultTokenPack = _packTokenMessage_();
    setField(58, tokenQZ, longitud);
    setField(60, resultTokenPack.tokenPackMessage, resultTokenPack.totalBytes);
}

int procesarTransaccionPagoProducto(DatosCnbBancolombia datosPagoProducto) {

    ResultISOPack resultadoIsoPackMessage;
    int resultadoTransaccion = -1;
    char dataRecibida[512] = {0x00};
    //DatosTransaccionDeclinada datosTransaccionDeclinada;

    memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
    // memset(&datosTransaccionDeclinada, 0x00, sizeof(datosTransaccionDeclinada));

    resultadoIsoPackMessage = packISOMessage();

    if (resultadoIsoPackMessage.responseCode > 0) {

        resultadoTransaccion = realizarTransaccion(resultadoIsoPackMessage.isoPackMessage,
                                                   resultadoIsoPackMessage.totalBytes, dataRecibida, atoi(datosVentaBancolombia.tipoTransaccion),
                                                   CANAL_DEMANDA, REVERSO_GENERADO);

        if (resultadoTransaccion > 0) {
            // desarmar la trama
            resultadoTransaccion = unpackDataPagoProducto(&datosPagoProducto, dataRecibida, resultadoTransaccion);
        } else {

            memset(datosPagoProducto.codigoRespuesta, 0x00, sizeof(datosPagoProducto.codigoRespuesta));
            memcpy(datosPagoProducto.codigoRespuesta, "01", 2);

            if (verificarHabilitacionCaja() == TRUE) {
                //transaccionRespuestaDatosCaja(datosVentaBancolombia, establecerTipoPagoCaja(tipoProducto));
            }

            if (resultadoTransaccion == -3) {
                imprimirDeclinadas(datosVentaBancolombia, "TIME OUT GENERAL");
            } else {
                imprimirDeclinadas(datosVentaBancolombia, "ERROR SIN CONEXION");
            }

            resultadoTransaccion = 0;
        }
    }

    return resultadoTransaccion;
}

int unpackDataPagoProducto(DatosCnbBancolombia *datosPagoProducto, char *dataRecibida, int cantidadBytes) {

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
    memcpy(datosVentaBancolombia.codigoRespuesta, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

    if (strcmp(codigoRespuesta, "00") == 0) {
        convertirDatosBancolombia(datosVentaBancolombia,datosPagoProducto);
        resultado = verificacionTramabancolombia(datosPagoProducto);

        if (resultado > 0) {
            resultado = 1;
        } else if (resultado < 1) {

            resultado = generarReverso();

            if (verificarHabilitacionCaja() == TRUE && (resultado == -3 || resultado == -1)) {
                memset(datosVentaBancolombia.codigoRespuesta, 0x00, sizeof(datosVentaBancolombia.codigoRespuesta));
                memcpy(datosVentaBancolombia.codigoRespuesta, "01", 2);
                //transaccionRespuestaDatosCaja(datosVentaBancolombia, establecerTipoPagoCaja(tipoProducto));
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
        borrarArchivo(discoNetcom, (char*)ARCHIVO_REVERSO);
        errorRespuestaTransaccion(codigoRespuesta, mensajeError);
        resultado = 0;

        if (verificarHabilitacionCaja() == TRUE) {
            memset(datosVentaBancolombia.codigoRespuesta, 0x00, sizeof(datosVentaBancolombia.codigoRespuesta));
            memcpy(datosVentaBancolombia.codigoRespuesta, "01", 2);
            //transaccionRespuestaDatosCaja(datosVentaBancolombia, establecerTipoPagoCaja(tipoProducto));
        }
        imprimirDeclinadas(datosVentaBancolombia, mensajeError);
    }

    return resultado;
}