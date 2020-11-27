//
// Created by NETCOM on 02/03/2020.
//
#include <string>
#include <stdio.h>
#include <Utilidades.h>
#include <Mensajeria.h>
#include <configuracion.h>
#include <Inicializacion.h>
#include <Archivos.h>
#include <native-lib.h>
#include "Tipo_datos.h"
#include "comunicaciones.h"
#include "android/log.h"
#include "Cajas.h"
#include "venta.h"

#define  LOG_TAG    "mysocket"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

int cargarCamposEstructuraDatosVenta(char * datosVenta, int Campo) {

    TEFFFieldMessage tipoCampo;
    int respuesta = 1;

    memset(&tipoCampo, 0x00, sizeof(tipoCampo));

    tipoCampo = getFiledTEFF(Campo);
    int longitudDefinida = 0;

    longitudDefinida = definirLongitudCampo(Campo);

    if (tipoCampo.field == Campo) {

        if (longitudDefinida > 0) {
            memcpy(datosVenta, tipoCampo.valueField, longitudDefinida);
        } else {
            memcpy(datosVenta, tipoCampo.valueField, tipoCampo.totalBytes);
        }

    } else {
        respuesta = -1;
    }
    return respuesta;
}

/**
 * @brief Funcion que se encarga de validar el codigo de respuesta y
 *        colocar los mensajes correspondientes.
 * @return 1 ok , -1 ko
 * @author .
 */
int validacionCodigoRespuestaTEFF(void) {

    int codigoRespuesta = 1;
    int respuesta = 0;
    codigoRespuesta = getCodigoRespuestaTEFF();
    respuesta = ((codigoRespuesta == 0) ? 1 : -1);

    if (respuesta < 0) {

        if (codigoRespuesta == TEFF_DECLINADA) {
            //screenMessage("COMUNICACION", "CAJA", "DECLINADA", "", 2 * 1000);
        } else if (codigoRespuesta == TEFF_ENTIDAD_NO_RESPONDE) {
            //screenMessage("COMUNICACION", "ENTIDAD NO RESPONDE", "", "", 2 * 1000);
        }
    }

    return respuesta;
}

int validarCodigoTransaccionTEFF(int codigoTransaccion) {

    int codigoTransaccionAuxiliar = 0;
    int respuesta = 0;

    codigoTransaccionAuxiliar = getCodigoTransaccionTEFF();

    if (codigoTransaccionAuxiliar == TRANSACCION_TEFF_LEALTAD
        || codigoTransaccionAuxiliar == TRANSACCION_TEFF_SINCRONIZACION
        || codigoTransaccionAuxiliar == TRANSACCION_TEFF_ACUMULACION_PUNTOS_COL) { //COINCIDE  CON TIPO DE TRANSACCION LIFEMILES
        respuesta = 1;
    } else {
        respuesta = ((codigoTransaccionAuxiliar == codigoTransaccion) ? 1 : -1);
        if (respuesta < 0) {
            //screenMessage("COMUNICACION", "CAJA", "RESPONDE DIFERENTE", "", 2 * 1000);
        }
    }

    return respuesta;
}

/**
 * @brief Funcion que se encarga de validar el codigo de transaccion y
 *        colocar los mensajes correspondientes.
 * @return  1 ok , -1 ko
 * @author .
 */
int validarTipoMensajeTEFF(void) {

    int tipoMensaje = 0;
    int respuesta = -1;

    tipoMensaje = getTipoMensajeTEFF();

    respuesta = (tipoMensaje == MENSAJE_TEFF_REQUERIMIENTO) || (tipoMensaje == MENSAJE_TEFF_RESPUESTA);
    if (respuesta < 0) {
        //screenMessage("COMUNICACION", "CAJA", "RESPONDE DIFERENTE", "", 2 * 1000);
        respuesta = -1;
    }
    return respuesta;
}

int _verificarHabilitacionCaja_(void) {

    int respuesta = 0;
    char auxiliarRespuesta[3 + 1];

    memset(auxiliarRespuesta, 0x00, sizeof(auxiliarRespuesta));
    getParameter(CAJA_REGISTRADORA, auxiliarRespuesta);
    respuesta = ((atoi(auxiliarRespuesta) == TRUE) ? 1 : -1);

    return respuesta;
}

/**
 * @brief Funcion que verifica si el codigo de cajero esta habilitado
 *        y validad en el caso que este habilitado, si es enviado por
 *        la caja
 * @param codigoCajero: si el codigo de cajero es encontrado salida del
 *        dato.
 * @return 1 ok , -1 ko
 * @author .
 */
int validarCodigoCajero(char *codigoCajero) {

    int activadoCodigoCajero = 0;
    int respuesta = 1;

    activadoCodigoCajero = verificarCodigoCajero();

    if (activadoCodigoCajero == TRUE) {

        respuesta = cargarCamposEstructuraDatosVenta(codigoCajero, FIELD_TEFF_CODIGO_CAJERO);

        if (respuesta < 0) {

            respuesta = -1;

        }

    } else {
        respuesta = cargarCamposEstructuraDatosVenta(codigoCajero, FIELD_TEFF_CODIGO_CAJERO);

        if (respuesta > 0) {
            respuesta = -1;

        } else {
            respuesta = 1;
        }
    }

    if (respuesta < 0) {
        //screenMessage("COMUNICACION", "ERROR", "CODIGO CAJERO", "", 2 * 1000);
    }

    return respuesta;
}

int verificarCodigoCajero(void) {

    char auxiliarRespuesta[3 + 1];
    int respuesta = 1;

    memset(auxiliarRespuesta, 0x00, sizeof(auxiliarRespuesta));
    getParameter(CODIGO_CAJERO_CAJA, auxiliarRespuesta);
    respuesta = ((atoi(auxiliarRespuesta) == TRUE) ? 1 : -1);

    return respuesta;
}


void extraerPanAsteriscos(char *track2, int lentrack2, char* outPan, int tipoAsteriaco) {

    int n = 0;
    int i = 0;
    //char panAuxiliar [LEN_TEFF_PAN + 1];
    char panAuxiliar[30 + 1];
    char ultimosCuatro[5 + 1];

    memset(panAuxiliar, 0x00, sizeof(panAuxiliar));
    memset(ultimosCuatro, 0x00, sizeof(ultimosCuatro));
    for (n = 0; n < lentrack2; n++) {

        if (track2[n] == 'D' || track2[n] == 'd') {
            break;

        }
    }
    memcpy(panAuxiliar, track2, n);
    memcpy(ultimosCuatro, track2 + n - 4, 4);

    if (tipoAsteriaco == 0) {
        for (i = 0; i < 6; i++) {
            panAuxiliar[6 + i] = '*';

        }
    } else {
        for (i = 0; i < 2; i++) {
            panAuxiliar[6 + i] = '*';

        }
        memcpy(panAuxiliar + 8, ultimosCuatro, 4);

    }

    memcpy(outPan, panAuxiliar, LEN_TEFF_PAN);

}

/**
 * @brief	realiza la petición para las cajas serial
 * @param 	datos
 * @param 	tipoTransaccionCaja
 * @return	> 1 exitosa
 * @author
 */
int peticionCajaSerial(DatosVenta *datos, int tipoTransaccionCaja) {

    int respuesta = 1;
    int verificar = 0;
    int sincronizar = 0;
    int continuar = TRUE;

    setParameter(USO_CAJA_REGISTRADORA, "0");

    setParameter(CAJA_NO_RESPONDE, "0");
    respuesta = 0;

    if (tipoTransaccionCaja != TRANSACCION_TEFF_COMPRAS_CLESS) {
        //screenMessage("COMUNICACION", "CAJA", "CONECTANDO", "", 1);
    } else {
        tipoTransaccionCaja = TRANSACCION_TEFF_COMPRAS;
    }

    verificar = verificarArchivo(DISCO_NETCOM, RESPALDO_CAJA);
    if (verificar > 0) {
        borrarArchivo(DISCO_NETCOM, RESPALDO_CAJA);
    }

    sincronizar = verificarArchivo(DISCO_NETCOM, ARCHIVO_SINCRONIZACION_CAJA);

    if (sincronizar > 0) {

        //sincronizar = getchAcceptCancel("CAJA REGISTRADORA", "SINCRONIZACION", "PENDIENTE", "DESEA CONTINUAR?", 30 * 1000);

        if (sincronizar > 0) {
            borrarArchivo(DISCO_NETCOM, ARCHIVO_SINCRONIZACION_CAJA);
            if(tipoTransaccionCaja != TRANSACCION_TEFF_COMPRAS_CLESS){
                //screenMessage("COMUNICACION", "CAJA", "CONECTANDO", "", 1);
            }else {
                tipoTransaccionCaja = TRANSACCION_TEFF_COMPRAS;
            }

        } else {
            continuar = 0;
            respuesta = -1;
        }
    }

    if (continuar == TRUE) {

        switch (tipoTransaccionCaja) {
            case TRANSACCION_TEFF_COMPRAS:
            case TRANSACCION_TEFF_TARJETA_MI_COMPRA:
                respuesta = transaccionPeticionDatosCompra(datos);
                break;
            case TRANSACCION_TEFF_ANULACIONES:
                respuesta = transaccionPeticionDatosAnulacion(datos);
                break;
            case TRANSACCION_TEFF_REDENCION:
                respuesta = transaccionPeticionDatosRedimirBono(datos);
                break;
            case TRANSACCION_TEFF_HABILITACIONES:
                respuesta = transaccionPeticionDatosHabilitarBono(datos);
                break;
            case TRANSACCION_TEFF_RECARGA_BONO:
                respuesta = transaccionPeticionDatosRecargaBono(datos);
                break;
            case TRANSACCION_TEFF_SINCRONIZACION:
                break;
            case TRANSACCION_TEFF_LEALTAD:
            case TRANSACCION_TEFF_ACUMULACION_PUNTOS_COL:
                //////// Se ajusta por peticon de REDEBAN ID 5366 del mantis y orlando envia correo ////////
                respuesta = transaccionPeticionDatosLealtad(datos, TRANSACCION_TEFF_COMPRAS);
                break;
            case TRANSACCION_TEFF_RETIRO_SIN_TARJETA:
                respuesta = transaccionPeticionRetiro(datos);
                break;
            case TRANSACCION_TEFF_EFECTIVO:
                respuesta = transaccionPeticionEfectivo(datos);
                break;
            case TRANSACCION_TEFF_TARJETA_PRIVADA:
                respuesta = transaccionPeticionTarjetaPrivada(datos);
                break;
            case TRANSACCION_TEFF_DEPOSITO_BCL:
                respuesta = transaccionPeticionDeposito(datos);
                break;
            case TRANSACCION_TEFF_PAGO_TARJETA_PRIVADA:
                respuesta = transaccionPeticionPagoTarjeta(datos);
                break;
            case TRANSACCION_TEFF_RECAUDO_LECTOR_BARRAS:
                respuesta = transaccionPeticionRecaudoManual(datos, TRANSACCION_TEFF_RECAUDO_LECTOR_BARRAS);
                break;
            case TRANSACCION_TEFF_RECAUDO_MANUAL:
                respuesta = transaccionPeticionRecaudoManual(datos, TRANSACCION_TEFF_RECAUDO_MANUAL);
                break;
            case TRANSACCION_TEFF_RECAUDO_TEMPRESARIAL:
                respuesta = transaccionPeticionRecaudoManual(datos, TRANSACCION_TEFF_RECAUDO_TEMPRESARIAL);
                break;
            case TRANSACCION_TEFF_PAGO_TCREDITO:
                respuesta = transaccionPeticionRecaudoManual(datos, TRANSACCION_TEFF_PAGO_TCREDITO);
                break;
            case TRANSACCION_TEFF_PAGO_CARTERA:
                respuesta = transaccionPeticionRecaudoManual(datos, TRANSACCION_TEFF_PAGO_CARTERA);
                break;
            case TRANSACCION_TEFF_TRANSFERENCIA_BCL:
                respuesta = transaccionPeticionTransferencia(datos, TRANSACCION_TEFF_TRANSFERENCIA_BCL);
                break;
            case TRANSACCION_TEFF_RETIRO_EFECTIVO:
                respuesta = transaccionPeticionRetiroEfectivo(datos);
                break;
            case TRANSACCION_TEFF_CONSULTA_SALDO_BCL:
                respuesta = transaccionPeticionConsultaSaldo(datos, TRANSACCION_TEFF_CONSULTA_SALDO_BCL);
                break;
            case TRANSACCION_TEFF_ENVIAR_GIRO:
            case TRANSACCION_TEFF_RECLAMAR_GIRO:
            case TRANSACCION_TEFF_CANCELAR_GIRO:
                respuesta = transaccionPeticionEnvioGiro(datos, tipoTransaccionCaja);
                break;
            case TRANSACCION_TEFF_ANULACION:
                respuesta = transaccionAnulacionBancolombia(datos);
                break;
            default:
                break;
        }
        if (respuesta > 0) {
            if (respuesta != 17) { //se valida para cuando sea por lectura directa y cambio de transaccion por caja
                setParameter(USO_CAJA_REGISTRADORA, "1");
            }

            if (tipoTransaccionCaja != TRANSACCION_TEFF_ANULACIONES) {
                guardarTransaccionCajaAnulacion(*datos);
            }
        }

    }

    //}

    return respuesta;
}

int respuestaCajaSerial(DatosVenta datos, int tipoTransaccionCaja) {

    int respuesta = 1;
    int verificar = 0;

    uChar usoCaja[1 + 1];

    memset(usoCaja, 0x00, sizeof(usoCaja));
    getParameter(USO_CAJA_REGISTRADORA, usoCaja);

    if ((getParameter(CAJA_NO_RESPONDE, "1") == TRUE)) {
        setParameter(CAJA_NO_RESPONDE, "0");
        return 1;
    }

    if (verificar > 0) {

        cargarDatosCajaAnulada(&datos);

    }

    if (tipoTransaccionCaja != TRANSACCION_TEFF_COMPRAS && strncmp(datos.codigoRespuesta, "03", 2) != 0) {
        //screenMessage("COMUNICACION", "CAJA", "RESPUESTA", "", 1);
    }

    switch (tipoTransaccionCaja) {
        case TRANSACCION_TEFF_COMPRAS:
            if (atoi(usoCaja) == 1) {
                //screenMessage("COMUNICACION", "CAJA", "RESPUESTA", "", 1);
                setParameter(USO_CAJA_REGISTRADORA, "0");
                getParameter(USO_CAJA_REGISTRADORA, usoCaja);
                respuesta = transaccionRespuestaDatos(datos, TRANSACCION_TEFF_COMPRAS);
            }
            break;

        case TRANSACCION_TEFF_DEPOSITO_BCL:
        case TRANSACCION_TEFF_RETIRO_SIN_TARJETA:
        case TRANSACCION_TEFF_TRANSFERENCIA_BCL:
        case TRANSACCION_TEFF_PAGO_TCREDITO:
        case TRANSACCION_TEFF_PAGO_CARTERA:
        case TRANSACCION_TEFF_RECAUDO_MANUAL:
        case TRANSACCION_TEFF_RECAUDO_TEMPRESARIAL:
        case TRANSACCION_TEFF_RECAUDO_LECTOR_BARRAS:
        case TRANSACCION_TEFF_CONSULTA_SALDO_BCL:
        case TRANSACCION_TEFF_ENVIAR_GIRO:
        case TRANSACCION_TEFF_RECLAMAR_GIRO:
        case TRANSACCION_TEFF_CANCELAR_GIRO:
            respuesta = transaccionRespuestaDatos(datos, tipoTransaccionCaja);
            break;
        case TRANSACCION_TEFF_CONSULTA_CUPO_BCL	:
            respuesta = transaccionRespuestaCupo(NULL, datos, TRANSACCION_TEFF_CONSULTA_CUPO_BCL,1);
            break;
        case TRANSACCION_TEFF_ANULACION:
            respuesta = transaccionRespuestaDatos(datos, TRANSACCION_TEFF_ANULACION);
            break;
        case TRANSACCION_TEFF_ANULACIONES:
            respuesta = 1;
            if (strncmp(datos.codigoRespuesta, "03", 2) != 0) {
                respuesta = transaccionRespuestaDatos(datos, TRANSACCION_TEFF_ANULACIONES);

            }
            break;

        case TRANSACCION_TEFF_REDENCION:

            respuesta = transaccionRespuestaDatosBono(datos, TRANSACCION_TEFF_REDENCION);
            break;
        case TRANSACCION_TEFF_HABILITACIONES:

            if (strncmp(datos.codigoRespuesta, "03", 2) != 0) {
                respuesta = transaccionRespuestaDatosBono(datos, TRANSACCION_TEFF_HABILITACIONES);
            }

            break;

        case TRANSACCION_TEFF_RECARGA_BONO:
            if (strncmp(datos.codigoRespuesta, "03", 2) != 0) {
                respuesta = transaccionRespuestaDatosBono(datos, TRANSACCION_TEFF_RECARGA_BONO);

            }
            break;

        case TRANSACCION_TEFF_SINCRONIZACION:

            respuesta = transaccionRespuestaDatos(datos, TRANSACCION_TEFF_SINCRONIZACION);
            break;
        case TRANSACCION_TEFF_REDIMIR_FALABELLA:
            respuesta = transaccionRespuestaRedimirFalabella(datos);
            break;
        case TRANSACCION_TEFF_LEALTAD:
            respuesta = transaccionRespuestaDatosLealtad(datos, FALSE);
            break;

        case TRANSACCION_TEFF_TARJETA_MI_COMPRA:
            respuesta = transaccionRespuestaDatos(datos, TRANSACCION_TEFF_TARJETA_MI_COMPRA);

            break;
        case TRANSACCION_TEFF_TARJETA_PRIVADA:
            respuesta = transaccionRespuestaDatosPrivada(datos, TRANSACCION_TEFF_TARJETA_PRIVADA);
            break;

        case TRANSACCION_TEFF_RETIRO_AVAL:
            respuesta = transaccionRespuestaDatosPrivada(datos, TRANSACCION_TEFF_RETIRO_AVAL);
            break;
        default:
            break;
    }

    return respuesta;

}

/**
 * @brief Funcion que se encarga de verificar, validar  y desempaquetar la trama
 *         de respuesta enviada por la caja.
 * @param bufferOut : trama de respuesta caja
 * @param longitud :longitud de la trama de respuesta
 * @param tipoTransaccionCaja
 * @return 1 ok , -1 ko
 * @author .
 */
int unpackRespuestaCaja(char *bufferOut, int longitud, int tipoTransaccionCaja) {

    int respuesta = 0;

    respuesta = ((bufferOut[0] == ACK) ? 1 : -1);
    if (respuesta < 0) {
        //screenMessage("COMUNICACION", "CAJA", "NACK RECIBIDO", "", 2 * 1000);
    } else if (bufferOut[1] == NACK
               && (tipoTransaccionCaja == TRANSACCION_TEFF_HABILITACIONES
                   || tipoTransaccionCaja == TRANSACCION_TEFF_REDENCION
                   || tipoTransaccionCaja == TRANSACCION_TEFF_RECARGA_BONO
                   || tipoTransaccionCaja == TRANSACCION_TEFF_BONO_VIRTUAL)) {
        //screenMessage("HABILITACION BONOS", "BONO", "NO COINCIDE", "", 2 * 1000);
        respuesta = -20;
    }

    if (respuesta > 0) {
        respuesta = confirmacionLRC(bufferOut, longitud);
    }

    if (respuesta > 0) {
        unpackMessageTEFF(bufferOut, longitud);
    }

    if (respuesta > 0) {
        respuesta = validarTipoMensajeTEFF();
    }

    if (respuesta > 0 && (tipoTransaccionCaja != TRANSACCION_TEFF_IDENTIFICAR_TRANSACCION_REALIZAR)) {
        respuesta = validarCodigoTransaccionTEFF(tipoTransaccionCaja);
    }

    if (respuesta > 0) {
        respuesta = validacionCodigoRespuestaTEFF();
    }

    return respuesta;
}

/**
 * @brief Funcion que se encarga de verificar, validar  y desempaquetar la trama
 *         de respuesta enviada por la caja para una compra.
 * @param datos
 * @return 1 ok , -1 ko
 * @author .
 */
int transaccionPeticionDatosCompra(DatosVenta *datos) {

    int respuesta = 0;
    int respuestaAuxiliar = 0;
    int respondeCaja = 0;
    int longitud = 0;
    char pan[LEN_TEFF_PAN + 1];
    char descuento[4 + 1];
    char bufferOut[2024 + 1];
    ResultISOPack packMessage;
    int resultadoCampo = 0;
    int tipoDatoAjustar = DATOS_SALIDA;
    int cajaAra = 0;
    char auxiliar[30 + 1];
    char modoCajaBancolombia[2 + 1];
    char isPuntosColombia[2 + 1];
    char TransaccionOTP[2 + 1];
    TablaUnoInicializacion tablaUno;

    DatosQrDescuentos datosQrDescuento;
    memset(pan, 0x00, sizeof(pan));
    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(&packMessage, 0x00, sizeof(packMessage));
    memset(bufferOut, 0x00, sizeof(bufferOut));
    memset(modoCajaBancolombia, 0x00, sizeof(modoCajaBancolombia));
    memset(descuento, 0x00, sizeof(descuento));
    memset(isPuntosColombia, 0x00, sizeof(isPuntosColombia));
    memset(TransaccionOTP, 0x00, sizeof(TransaccionOTP));
    memset(&datosQrDescuento, 0x00, sizeof(datosQrDescuento));
    memset(&tablaUno, 0x00, sizeof(tablaUno));

    tablaUno = _desempaquetarTablaCeroUno_();

    getParameter(TIPO_INICIA_TRANSACCION, modoCajaBancolombia);
    getParameter(PRODUCTO_PUNTOS_COLOMBIA, isPuntosColombia);
    getParameter(TIPO_TRANSACCION_OTP, TransaccionOTP);

    setTipoMensajeTEFF(MENSAJE_TEFF_REQUERIMIENTO);
    setCodigoTransaccionTEFF(TRANSACCION_TEFF_COMPRAS);
    setCodigoRespuestaTEFF(NO_HAY_CODIGO_DE_RESPUESTA);

    validarPosEntryMode(2, datos->posEntrymode);

    //if (strcmp(modoCajaBancolombia, CAJA_JMR) != 0) {

    if (atoi(datos->track2) != 0) {
        extraerPanAsteriscos(datos->track2, strlen(datos->track2), pan,
                             ASTERISCOS_4_ULTIMOS);
    } else {
        datosQrDescuento = binQrDescuentos(TransaccionOTP);
        memcpy(pan, datosQrDescuento.bin, 6);
        strcat(pan, "**0000");
    }

    setFieldTEFF(FIELD_TEFF_ASTERISCOS_4_ULTIMOS_TARJETA, pan,
                 LEN_TEFF_PAN);
    //}
    if (cajaAra == 1) {
        tipoDatoAjustar = DATOS_ENTRADA_MONTOS;
        setFieldTEFF(FIELD_TEFF_DATA_ADICIONAL, auxiliar, 0);
    } else {
        setFieldTEFF(FIELD_TEFF_REFERENCIA_ADICIONAL, auxiliar, 0);
    }
    if (verificarCodigoCajero() > 0) {
        setFieldTEFF(FIELD_TEFF_CODIGO_CAJERO, auxiliar, 0);
    }

    setFieldTEFF(FIELD_TEFF_MONTO, auxiliar, 0);

    if (atoi(isPuntosColombia) == 1) {
        setFieldTEFF(FIELD_TEFF_TIPO_AUTORIZADOR, auxiliar, 0);
        setFieldTEFF(FIELD_TEFF_NUMERO_DOCUMENTO, auxiliar, 0);
    }

    setFieldTEFF(FIELD_TEFF_IVA, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_BASE_DEVOLUCION, auxiliar, 0);

    if(tablaUno.options1.tipProcessing > 0 && cajaAra == 0){
        setFieldTEFF(FIELD_TEFF_PROPINA, auxiliar, 0);
    }

    if (cajaAra == 0) {
        setFieldTEFF(FIELD_TEFF_INC, auxiliar, 0);
        setFieldTEFF(FIELD_TEFF_NUMERO_FACTURA_ENVIADO_CAJA, auxiliar, 0);
    } else {
        setFieldTEFF(FIELD_TEFF_RRN, auxiliar, 0);//inc para ARA
    }


    if (labelEspecialesDescuento(datos->track2) == TRUE) {
        setFieldTEFF(FIELD_TEFF_DESCUENTO_TRANSACCION, auxiliar, 0);
    }

    packMessage = packMessageTEFF();

    //respuesta = realizarTransaccionCaja(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 200, 30 * 100);
    respuesta = realizarTransaccionMedioComunicacion(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 30*100);

    respondeCaja = respuesta;

    if (respondeCaja == 0) {
        setParameter(CAJA_NO_RESPONDE, "1");
        //screenMessage("COMUNICACION", "CAJA", "SIN RESPUESTA", "", 2 * 1000);

    } else {

        memset(auxiliar, 0x00, sizeof(auxiliar));
        auxiliar[0] = ACK;
        //realizarTransaccionCaja(auxiliar, 1, bufferOut, 200, 10);
        realizarTransaccionMedioComunicacion(auxiliar, 1, bufferOut,10);
    }

    if (respuesta > 1) {
        longitud = respuesta;
        respuesta = unpackRespuestaCaja(bufferOut, longitud, TRANSACCION_TEFF_COMPRAS);
    } else {
        respuesta = 0;
    }

    if (respuesta > 0) {

        respuesta = validarCodigoCajero(datos->codigoCajero);

    }

    if (respuesta > 0) {

        memset(auxiliar, 0x00, sizeof(auxiliar));
        cargarCamposEstructuraDatosVenta(auxiliar, FIELD_TEFF_REFERENCIA_ADICIONAL);
        memcpy(datos->textoAdicional, auxiliar, 7);//desarrollo nuevo campo tuya texto adicional

        memset(auxiliar, 0x00, sizeof(auxiliar));
        cargarCamposEstructuraDatosVenta(auxiliar, FIELD_TEFF_MONTO);
        memcpy(datos->totalVenta, auxiliar, TAM_COMPRA_NETA);
        ajustarDatosTransaccion(datos->totalVenta, strlen(datos->totalVenta), tipoDatoAjustar);

        memset(auxiliar, 0x00, sizeof(auxiliar));
        cargarCamposEstructuraDatosVenta(auxiliar, FIELD_TEFF_IVA);
        memcpy(datos->iva, auxiliar, TAM_COMPRA_NETA);
        ajustarDatosTransaccion(datos->iva, strlen(datos->iva), tipoDatoAjustar);

        memset(auxiliar, 0x00, sizeof(auxiliar));
        cargarCamposEstructuraDatosVenta(auxiliar, FIELD_TEFF_BASE_DEVOLUCION);
        memcpy(datos->baseDevolucion, auxiliar, TAM_COMPRA_NETA);
        ajustarDatosTransaccion(datos->baseDevolucion, strlen(datos->baseDevolucion), tipoDatoAjustar);

        memset(auxiliar, 0x00, sizeof(auxiliar));
        cargarCamposEstructuraDatosVenta(auxiliar, FIELD_TEFF_PROPINA);
        memcpy(datos->propina, auxiliar, TAM_COMPRA_NETA);
        ajustarDatosTransaccion(datos->propina, TAM_COMPRA_NETA, tipoDatoAjustar);

        cargarCamposEstructuraDatosVenta(datos->facturaCaja, FIELD_TEFF_NUMERO_FACTURA_ENVIADO_CAJA);

        ajustarDatosTransaccion(datos->facturaCaja, strlen(datos->facturaCaja), tipoDatoAjustar);

        if (cajaAra == 0) {
            cargarCamposEstructuraDatosVenta(datos->inc, FIELD_TEFF_INC);
        } else {
            memset(auxiliar, 0x00, sizeof(auxiliar));
            cargarCamposEstructuraDatosVenta(auxiliar, FIELD_TEFF_RRN);
            if(atoi(auxiliar) > 0){
                memcpy(datos->inc, auxiliar, TAM_COMPRA_NETA);
            }
        }
        ajustarDatosTransaccion(datos->inc, strlen(datos->inc), tipoDatoAjustar);

        resultadoCampo = cargarCamposEstructuraDatosVenta(auxiliar, FIELD_TEFF_NUMERO_CUENTA);

        if (resultadoCampo > 0) {
            memcpy(datos->tokenVivamos, auxiliar, SIZE_CUENTA);
        }

        memset(auxiliar, 0x00, sizeof(auxiliar));
        cargarCamposEstructuraDatosVenta(auxiliar, FIELD_TEFF_NUMERO_TELEFONO);

        if (atoi(auxiliar) > 0) {
            borrarArchivo(discoNetcom, CELULAR_SMS);
            escribirArchivo(discoNetcom, CELULAR_SMS, auxiliar, 10);
        }

        respuesta = verificarMontosInconsistentes(datos);
        //MODIFICAR POR GETPARAMETER
        /*if (verificarHabilitacionObjeto(TRANSACCION_VIVAMOS) == TRUE && respuesta > 0) {
            respuestaAuxiliar = cargarCamposEstructuraDatosVenta(datos->tokenVivamos, FIELD_TEFF_DESCUENTO_TRANSACCION);
            if (respuestaAuxiliar == TRUE) {
                ajustarDatosTransaccion(datos->tokenVivamos, strlen(datos->tokenVivamos), DATOS_SALIDA);

            } else {
                memcpy(datos->tokenVivamos, "0", strlen("0"));
            }

        }*/
        if (respuesta > 0) {
            respuesta = validarCampo69(datos);
        }

        if (validarDescuentosCaja(datos->tokenVivamos, descuento) == TRUE && respuesta > 0) {

            memset(datos->estadoTarifa, 0x00, sizeof(datos->estadoTarifa));
            memcpy(datos->estadoTarifa, "DT", 2);

        }
    }

    if (respuesta <= 0) {

        setParameter(USO_CAJA_REGISTRADORA, "1");

    }

    return respuesta;
}

int validarCampo69(DatosVenta *datos) {

    char documento[12 + 1];
    char documentoAuxiliar[12 + 1];
    int campoActivo = 0;
    long valor = 0;
    int respuesta = 1;
    int lecturaDirectaChip = 0;
    char auxiliar[30 + 1];
    char bufferAuxiliar[sizeof(DatosVenta) + 2];
    DatosVenta datosTransaccion;
    ResultTokenPack resultTokenPack;
    boolean lecturaDirectaMulticorresponsal = FALSE;

    memset(bufferAuxiliar, 0x00, sizeof(bufferAuxiliar));
    memset(&datosTransaccion, 0x00, sizeof(datosTransaccion));
    memset(&resultTokenPack, 0x00, sizeof(resultTokenPack));
    memset(documentoAuxiliar, 0x00, sizeof(documentoAuxiliar));
    memset(documento, 0x00, sizeof(documento));

    setParameter(USO_CAJA_REGISTRADORA, "1");
    campoActivo = cargarCamposEstructuraDatosVenta(documento, FIELD_TEFF_TIPO_DOCUMENTO);

    if (campoActivo == TRUE) {

        if (strncmp(datos->estadoTarifa, "MV", 2) == 0) {
            respuesta = 17; //TAG_CUST_PROCESSING_ERROR
        } else if (strncmp(datos->estadoTarifa, "BD", 2) == 0) {
            respuesta = -17;
        }

        leftTrim(documentoAuxiliar, documento, 0x30);
        valor = atol(documentoAuxiliar);
        setParameter(USO_CAJA_REGISTRADORA, "1");

        switch (valor) {

            case 94:
            case 95:
                memset(auxiliar, 0x00, sizeof(auxiliar));
                cargarCamposEstructuraDatosVenta(auxiliar, FIELD_TEFF_TIPO_DOCUMENTO);
                memcpy(datos->estadoTarifa, auxiliar, 2); // OJO YENY
                break;
            case 98:
                respuesta = -17;
                break;
            case 99:
                break;

        }

    }
    return respuesta;
}
/**
 * @brief Funcion que se encarga de entregar los datos de respuesta de la
 *        transaccion de compra y anualcion a la caja
 * @param datos
 * @param transaccion             TRANSACCION_TEFF_COMPRAS
 *                                TRANSACCION_TEFF_ANULACIONES
 *                                TRANSACCION_TEFF_RECARGA_BONO
 *
 * @return 1 ok , -1 ko
 * @author .
 */

int transaccionRespuestaDatos(DatosVenta datos, int transaccion) {
    int respuesta = 0;
    char pan[LEN_TEFF_PAN + 1];
    char bufferOut[512 + 1];
    char tipoCuenta[2 + 1];
    char auxiliar[30 + 1];
    int modoDatafono = 0;
    TEFFFieldMessage tipoCampo;
    uChar modoCajaBancolombia[1 + 1];
    uChar isCupoEspecialista[1 + 1];
    char flujoUnificado[2 + 1];
    int cajaAra = 0;
    TablaUnoInicializacion tablaUno;
    ResultISOPack packMessage;

    ResultISOPack packMessageRespaldo;
    memset(pan, 0x00, sizeof(pan));
    memset(&tablaUno, 0x00, sizeof(tablaUno));
    memset(&packMessage, 0x00, sizeof(packMessage));
    memset(&packMessageRespaldo, 0x00, sizeof(packMessageRespaldo));
    memset(bufferOut, 0x00, sizeof(bufferOut));
    memset(tipoCuenta, 0x00, sizeof(tipoCuenta));
    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(modoCajaBancolombia, 0x00, sizeof(modoCajaBancolombia));
    memset(isCupoEspecialista, 0x00, sizeof(isCupoEspecialista));
    memset(flujoUnificado, 0x00, sizeof(flujoUnificado));

    tipoCampo = getFiledTEFF(69);

    modoDatafono = verificarModoCNB();
    getParameter(TIPO_INICIA_TRANSACCION, modoCajaBancolombia);
    getParameter(PUERTO_SALIDA, isCupoEspecialista);
    getParameter(FLUJO_UNIFICADO, flujoUnificado);

    if (/*verificarModoJMR() == 1 && */atoi(flujoUnificado) == 1) {
        cajaAra = 1;
        strcat(datos.iva, "00");
        strcat(datos.baseDevolucion, "00");
    } else if(/*verificarModoJMR() == 1 && */atoi(flujoUnificado) == 0) {
        cajaAra = 2;
    }

    if ((modoDatafono == MODO_MIXTO)
        && (T_BANCOLOMBIA(transaccion) || strncmp(datos.codigoGrupo, OPERACION_RETIRO_PRINCIPAL, 2) == 0
            || strncmp(datos.codigoGrupo, OPERACION_RETIRO_SECUNDARIA, 2) == 0
            || strncmp(datos.codigoGrupo, OPERACION_ANULACION_BANCOL, 2) == 0)) {
        tablaUno = desempaquetarCNB();
    } else {
        tablaUno = _desempaquetarTablaCeroUno_();
    }

    if (transaccion == TRANSACCION_TEFF_DEPOSITO_BCL || T_BANCOLOMBIA(transaccion)) // ||
    {

        if (transaccion == TRANSACCION_TEFF_DEPOSITO_BCL || transaccion == TRANSACCION_TEFF_ANULACION) {

            strcat(datos.totalVenta, "00");
            strcat(datos.compraNeta, "00");
            //strcat(datos.iva, "00");
        } else {

//			strcat(datos.totalVenta, "00");
//			strcat(datos.compraNeta, "00");
//			strcat(datos.iva, "00");
        }
    }

    if (transaccion == TRANSACCION_TEFF_TARJETA_MI_COMPRA) {
        setTipoMensajeTEFF(MENSAJE_TEFF_NO_REQUIERE_RESPUESTA);
    } else {
        setTipoMensajeTEFF(MENSAJE_TEFF_RESPUESTA);
    }

    setCodigoTransaccionTEFF(transaccion);

    if (cajaAra == 0) {

        validacionCodigoRespuestaCaja(datos.codigoRespuesta);
        setCodigoRespuestaTEFF(atoi(datos.codigoRespuesta));

        setFieldTEFF(FIELD_TEFF_CODIGO_RESPUESTA, datos.codigoRespuesta,
                     LEN_TEFF_CODIGO_RESPUESTA);

        validarPosEntryMode(2, datos.posEntrymode);

        ajustarDatosTransaccion(datos.codigoAprobacion,
                                LEN_TEFF_CODIGO_AUTORIZACION, DATOS_ENTRADA);
        setFieldTEFF(FIELD_TEFF_CODIGO_AUTORIZACION, datos.codigoAprobacion,
                     LEN_TEFF_CODIGO_AUTORIZACION);
    }
    if (cajaAra != 2) {
        if (atoi(datos.track2) != 0) {
            extraerPanAsteriscos(datos.track2, strlen(datos.track2), pan,
                                 ASTERISCOS_4_ULTIMOS);
        } else {
            memcpy(pan, "000000**0000", LEN_TEFF_PAN);
        }

        setFieldTEFF(FIELD_TEFF_ASTERISCOS_4_ULTIMOS_TARJETA, pan,
                     LEN_TEFF_ASTERISCOS_4_ULTIMOS_TARJETA);

        if (verificarCodigoCajero() > 0) {
            memset(auxiliar, 0x00, sizeof(auxiliar));
            _rightPad_(auxiliar, datos.codigoCajero, 0x20,
                     LEN_TEFF_CODIGO_CAJERO);
            setFieldTEFF(FIELD_TEFF_CODIGO_CAJERO, auxiliar,
                         LEN_TEFF_CODIGO_CAJERO);
        }

        memcpy(tipoCuenta, datos.tipoCuenta, LEN_TEFF_TIPO_CUENTA);
        tiposDeCuentaCaja(tipoCuenta, atoi(datos.creditoRotativo));
        setFieldTEFF(FIELD_TEFF_TIPO_CUENTA, tipoCuenta, LEN_TEFF_TIPO_CUENTA);

        if (transaccion != TRANSACCION_TEFF_ENVIAR_GIRO
            && transaccion != TRANSACCION_TEFF_RECLAMAR_GIRO
            && transaccion != TRANSACCION_TEFF_CANCELAR_GIRO) {
            memset(auxiliar, 0x00, sizeof(auxiliar));
            _rightPad_(auxiliar, datos.cardName, 0x20, LEN_TEFF_FRANQUICIA);
            setFieldTEFF(FIELD_TEFF_FRANQUICIA, auxiliar, LEN_TEFF_FRANQUICIA);
        }
    }

    if (cajaAra == 0) {
        ajustarDatosTransaccion(datos.totalVenta, LEN_TEFF_MONTO,
                                DATOS_ENTRADA_MONTOS);
        setFieldTEFF(FIELD_TEFF_MONTO, datos.totalVenta, LEN_TEFF_MONTO);

        memset(auxiliar, 0x00, sizeof(auxiliar));
        leftPad(auxiliar, datos.totalVenta, 0x30, LEN_TEFF_MONTO);
        setFieldTEFF(FIELD_TEFF_MONTO, auxiliar, LEN_TEFF_MONTO);
    }


    if (transaccion != TRANSACCION_TEFF_DEPOSITO_BCL
        && transaccion != TRANSACCION_TEFF_RETIRO_SIN_TARJETA
        && transaccion != TRANSACCION_TEFF_TRANSFERENCIA_BCL
        && transaccion != TRANSACCION_TEFF_PAGO_CARTERA
        && transaccion != TRANSACCION_TEFF_PAGO_TCREDITO
        && transaccion != TRANSACCION_TEFF_RECAUDO_MANUAL
        && transaccion != TRANSACCION_TEFF_RECAUDO_LECTOR_BARRAS
        && transaccion != TRANSACCION_TEFF_RECAUDO_TEMPRESARIAL
        && transaccion != TRANSACCION_TEFF_ENVIAR_GIRO
        && transaccion != TRANSACCION_TEFF_RECLAMAR_GIRO
        && transaccion != TRANSACCION_TEFF_CANCELAR_GIRO) {
        if (cajaAra != 2) {
            if (atoi(datos.codigoRespuesta) != 1) {

                ajustarDatosTransaccion(datos.iva, LEN_TEFF_MONTO,DATOS_ENTRADA_MONTOS);
                setFieldTEFF(FIELD_TEFF_IVA, datos.iva, LEN_TEFF_IVA);
                ajustarDatosTransaccion(datos.baseDevolucion, LEN_TEFF_MONTO, DATOS_ENTRADA_MONTOS);
                setFieldTEFF(FIELD_TEFF_BASE_DEVOLUCION, datos.baseDevolucion, LEN_TEFF_BASE_DEVOLUCION);

                ajustarDatosTransaccion(datos.cuotas, LEN_TEFF_CUOTAS,DATOS_ENTRADA);
                setFieldTEFF(FIELD_TEFF_CUOTAS, datos.cuotas, LEN_TEFF_CUOTAS);
                if ((tablaUno.options1.tipProcessing > 0)
                    && (atoi(datos.propina) > 0)) {
                    ajustarDatosTransaccion(datos.propina, LEN_TEFF_PROPINA, DATOS_ENTRADA);
                    setFieldTEFF(FIELD_TEFF_PROPINA, datos.propina,
                                 strlen(datos.propina) - 2);

                }
            } else {

                leftPad(auxiliar, datos.iva, 0x30, LEN_TEFF_IVA);
                setFieldTEFF(FIELD_TEFF_IVA, auxiliar, LEN_TEFF_IVA);
                leftPad(auxiliar, datos.baseDevolucion, 0x30,
                        LEN_TEFF_BASE_DEVOLUCION);
                setFieldTEFF(FIELD_TEFF_BASE_DEVOLUCION, auxiliar,
                             LEN_TEFF_BASE_DEVOLUCION);
                if ((tablaUno.options1.tipProcessing > 0)
                    && (atoi(datos.propina) > 0)) {
                    leftPad(auxiliar, datos.propina, 0x30, LEN_TEFF_PROPINA);
                    setFieldTEFF(FIELD_TEFF_PROPINA, auxiliar,
                                 LEN_TEFF_PROPINA);
                }
            }
        }
    }
    if (atoi(tipoCampo.valueField) == 91
        || atoi(tipoCampo.valueField) == 90) {

        leftPad(auxiliar, datos.facturaCaja, 0x30,
                LEN_TEFF_NUMERO_FACTURA_MULTIPOS);
        setFieldTEFF(FIELD_TEFF_NUMERO_FACTURA_ENVIADO_CAJA, auxiliar,
                     LEN_TEFF_NUMERO_FACTURA_MULTIPOS);
        leftPad(auxiliar, datos.codigoComercio, 0x00,
                LEN_TEFF_NUMERO_DOCUMENTO);
        setFieldTEFF(FIELD_TEFF_NUMERO_DOCUMENTO, auxiliar,
                     LEN_TEFF_NUMERO_DOCUMENTO);
    }

    if (transaccion == TRANSACCION_TEFF_TRANSFERENCIA_BCL) {
        setFieldTEFF(FIELD_TEFF_NUMERO_CUENTA, datos.textoAdicional,
                     strlen(datos.textoAdicional));
        setFieldTEFF(FIELD_TEFF_NUMERO_FACTURA, datos.nombreComercio,
                     strlen(datos.nombreComercio));
    }

    if (transaccion == TRANSACCION_TEFF_RECAUDO_MANUAL) {
        setFieldTEFF(FIELD_TEFF_CODIGO_CONVENIO, datos.codigoComercio,
                     strlen(datos.codigoComercio));
        setFieldTEFF(FIELD_TEFF_DESCRIPCION_CONVENIO, datos.field57,
                     strlen(datos.field57));
        memset(datos.facturaCaja,0x30,sizeof(datos.facturaCaja));
        setFieldTEFF(FIELD_TEFF_NUMERO_FACTURA, datos.facturaCaja,
                     strlen(datos.facturaCaja));
    } else if (transaccion == TRANSACCION_TEFF_RECAUDO_LECTOR_BARRAS
               || transaccion == TRANSACCION_TEFF_RECAUDO_TEMPRESARIAL) {
        setFieldTEFF(FIELD_TEFF_CODIGO_CONVENIO, datos.codigoComercio,
                     strlen(datos.codigoComercio));
        setFieldTEFF(FIELD_TEFF_DESCRIPCION_CONVENIO, datos.field57,
                     strlen(datos.field57));
        memset(auxiliar, 0x00, sizeof(auxiliar));
        leftTrim(auxiliar, datos.tokenVivamos, ' ');

        setFieldTEFF(FIELD_TEFF_NUMERO_FACTURA, auxiliar, strlen(auxiliar));
    } else if (verificarAnulacionBclRecaudo(transaccion,
                                            datos.tipoTransaccion)) {
        setFieldTEFF(FIELD_TEFF_CODIGO_CONVENIO, datos.codigoComercio,
                     strlen(datos.codigoComercio));

        memset(auxiliar, 0x00, sizeof(auxiliar));
        leftTrim(auxiliar, datos.tokenVivamos, ' ');
        setFieldTEFF(FIELD_TEFF_NUMERO_FACTURA, auxiliar, strlen(auxiliar));
    }
    if (cajaAra != 2) {
        ajustarDatosTransaccion(datos.numeroRecibo,
                                LEN_TEFF_NUMERO_RECIBO_DATAFONO, DATOS_ENTRADA);
        setFieldTEFF(FIELD_TEFF_NUMERO_RECIBO_DATAFONO, datos.numeroRecibo,
                     LEN_TEFF_NUMERO_RECIBO_DATAFONO);
        ajustarDatosTransaccion(datos.rrn, LEN_TEFF_RRN, DATOS_ENTRADA);

        //if (strncmp(datos.track2, "881031", 6) == 0) {
        if (atoi(isCupoEspecialista) == 1) {
            setFieldTEFF(FIELD_TEFF_TIPO_DOCUMENTO, datos.acquirerId,
                         LEN_TEFF_TIPO_DOCUMENTO);
            setFieldTEFF(FIELD_TEFF_NUMERO_DOCUMENTO, datos.fiel42 + 2,
                         strlen(datos.fiel42));
        }
        //}
    }

    if (cajaAra == 0) {
        setFieldTEFF(FIELD_TEFF_RRN, datos.rrn, LEN_TEFF_RRN);
    }
    if (cajaAra != 2) {
        ajustarDatosTransaccion(datos.terminalId, LEN_TEFF_NUMERO_TERMINAL,
                                DATOS_ENTRADA);
        setFieldTEFF(FIELD_TEFF_NUMERO_TERMINAL, datos.terminalId,
                     LEN_TEFF_NUMERO_TERMINAL);
        if (strcmp(tablaUno.receiptLine3, datos.codigoComercio)
            != 0&& modoDatafono != MODO_MIXTO) {
            _rightPad_(auxiliar, datos.codigoComercio, 0x00,
                     LEN_TEFF_CODIGO_ESTABLECIMIENTO);
            setFieldTEFF(FIELD_TEFF_CODIGO_ESTABLECIMIENTO, auxiliar,
                         LEN_TEFF_CODIGO_ESTABLECIMIENTO);
        } else {
            setFieldTEFF(FIELD_TEFF_CODIGO_ESTABLECIMIENTO,
                         tablaUno.receiptLine3, LEN_TEFF_CODIGO_ESTABLECIMIENTO);
        }

        if (transaccion == TRANSACCION_TEFF_RETIRO_SIN_TARJETA) {
            setFieldTEFF(FIELD_TEFF_NUMERO_CUENTA, datos.tokenVivamos, 11);
        } else if (transaccion == TRANSACCION_TEFF_DEPOSITO_BCL
                   || verificarAnulacionBclDeposito(transaccion,
                                                    datos.tipoTransaccion)) {
            ajustarDatosTransaccion(datos.propina, SIZE_CUENTA, DATOS_ENTRADA);
            setFieldTEFF(FIELD_TEFF_NUMERO_CUENTA, datos.propina, SIZE_CUENTA);
        }

        if (strlen(datos.txnDate) > 0) {
            ajusteFecha(datos.txnDate);
        } else {

            memset(datos.txnTime, 0x00, sizeof(datos.txnTime));
            memset(datos.txnDate, 0x00, sizeof(datos.txnDate));
            horaFechaTerminal(datos.txnTime, datos.txnDate);
        }
        setFieldTEFF(FIELD_TEFF_FECHA, datos.txnDate, LEN_TEFF_FECHA);
        setFieldTEFF(FIELD_TEFF_HORA, datos.txnTime, LEN_TEFF_HORA);

        if (transaccion == TRANSACCION_TEFF_PAGO_TCREDITO
            || transaccion == TRANSACCION_TEFF_PAGO_CARTERA
            || verificarAnulacionBclPago(transaccion,
                                         datos.tipoTransaccion)) {
            setFieldTEFF(FIELD_TEFF_NUMERO_FACTURA, datos.tokenVivamos,
                         strlen(datos.tokenVivamos));
        }

        armarCamposGiros(transaccion, datos);
    }

    if (cajaAra == 2) {
        setFieldTEFF(FIELD_TEFF_DATA_ADICIONAL, datos.aux1, 20);
    }

    packMessage = packMessageTEFF();
    packMessageRespaldo = packMessage;
    borrarArchivo(DISCO_NETCOM, ARCHIVO_SINCRONIZACION_CAJA);
    //respuesta = realizarTransaccionCaja(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 200, 10 * 100);
    respuesta = realizarTransaccionMedioComunicacion(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 10*100);

    if (respuesta <= 0) {
        setParameter(CAJA_NO_RESPONDE, "1");
       // screenMessage("COMUNICACION", "CAJA", "SIN RESPUESTA", "", 2 * 1000);
    }
    respuesta = ((bufferOut[0] == ACK) ? 1 : -1);

    if (respuesta <= 0 && transaccion != TRANSACCION_TEFF_SINCRONIZACION) {
        guardarTransaccionCajasSincronizacion(packMessage.isoPackMessage, packMessage.totalBytes);
    }
    setParameter(PUERTO_SALIDA, "0");
    return respuesta;
}

/**
 * @brief Funcion que se encarga de verificar, validar  y desempaquetar la trama
 *        de respuesta enviada por la caja para una anulacion.
 * @param datos
 * @return 1 ok , -1 ko
 * @author .
 */
int transaccionPeticionDatosAnulacion(DatosVenta *datos) {
    int respuesta = 0;
    int respondeCaja = 0;
    int longitud = 0;
    int claveValida = 0;
    char auxiliar[30 + 1];
    char bufferOut[512 + 1];
    char claveSupervisor[4 + 1];

    ResultISOPack packMessage;

    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(&packMessage, 0x00, sizeof(packMessage));
    memset(bufferOut, 0x00, sizeof(bufferOut));
    memset(claveSupervisor, 0x00, sizeof(claveSupervisor));

    setTipoMensajeTEFF(MENSAJE_TEFF_REQUERIMIENTO);
    setCodigoTransaccionTEFF(TRANSACCION_TEFF_ANULACIONES);
    setCodigoRespuestaTEFF(NO_HAY_CODIGO_DE_RESPUESTA);

    if (verificarCodigoCajero() > 0) {
        setFieldTEFF(FIELD_TEFF_CODIGO_CAJERO, auxiliar, 0);

    }

    setFieldTEFF(FIELD_TEFF_NUMERO_RECIBO_DATAFONO, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_NUMERO_FACTURA_ENVIADO_CAJA, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_CLAVE_SUPERVISOR, auxiliar, 0);

    packMessage = packMessageTEFF();

    respuesta = realizarTransaccionAnulacionMedioComunicacion(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut,100000);
    respondeCaja = respuesta;
    if (respondeCaja == 0) {
        setParameter(CAJA_NO_RESPONDE, "1");
        //screenMessage("COMUNICACION", "CAJA", "SIN RESPUESTA", "", 2 * 1000);
    }

    if (respuesta > 0) {
        longitud = respuesta;
        respuesta = unpackRespuestaCaja(bufferOut, longitud, TRANSACCION_TEFF_ANULACIONES);

    }

    if (respuesta > 0) {
        respuesta = validarCodigoCajero(datos->codigoCajero);
    }

    if (respuesta > 0) {

        cargarCamposEstructuraDatosVenta(claveSupervisor, FIELD_TEFF_CLAVE_SUPERVISOR);
        respuesta = validarClaveSupervisor(claveSupervisor, TRANSACCION_TEFF_ANULACIONES);
        claveValida = respuesta;
    }

    if (respuesta > 0) {
        memset(auxiliar, 0x00, sizeof(auxiliar));
        cargarCamposEstructuraDatosVenta(datos->numeroRecibo, FIELD_TEFF_NUMERO_RECIBO_DATAFONO);
        cargarCamposEstructuraDatosVenta(datos->facturaCaja, FIELD_TEFF_NUMERO_FACTURA_ENVIADO_CAJA);
    }

    if (respondeCaja > 0) {

        memcpy(datos->codigoRespuesta, "01", 2);
        memset(auxiliar, 0x00, sizeof(auxiliar));
        auxiliar[0] = ACK;
        realizarTransaccionAnulacionMedioComunicacion(auxiliar, 1, bufferOut, 0);
    }

    if (claveValida != TRUE) {

        memset(&packMessage, 0x00, sizeof(packMessage));
        packMessage = tramaRespuestaClaveInvalida(TRANSACCION_TEFF_ANULACIONES);
        realizarTransaccionAnulacionMedioComunicacion(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 0);
        memcpy(datos->codigoRespuesta, "03", 2);
    }

    return respuesta;
}

int transaccionPeticionDatosHabilitarBono(DatosVenta *datos) {
    int respuesta = 0;
    int respondeCaja = 0;
    int longitud = 0;
    int claveValida = 0;
    int idx = 0;
    char pan[LEN_TEFF_PAN + 1];
    char auxiliar[30 + 1];
    char bufferOut[512 + 1];
    char claveSupervisor[4 + 1];
    char consultarHabilitada[1 + 1];

    ResultISOPack packMessage;

    memset(pan, 0x00, sizeof(pan));
    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(&packMessage, 0x00, sizeof(packMessage));
    memset(bufferOut, 0x00, sizeof(bufferOut));
    memset(claveSupervisor, 0x00, sizeof(claveSupervisor));
    memset(consultarHabilitada, 0x00, sizeof(consultarHabilitada));

    getParameter(HABILITAR_CONSULTA_BONOS, consultarHabilitada);


    setTipoMensajeTEFF(MENSAJE_TEFF_REQUERIMIENTO);
    setCodigoTransaccionTEFF(TRANSACCION_TEFF_HABILITACIONES);
    setCodigoRespuestaTEFF(NO_HAY_CODIGO_DE_RESPUESTA);

    validarPosEntryMode(2, datos->posEntrymode);

    if (verificarCodigoCajero() > 0) {
        setFieldTEFF(FIELD_TEFF_CODIGO_CAJERO, auxiliar, 0);

    }

    setFieldTEFF(FIELD_TEFF_CLAVE_SUPERVISOR, auxiliar, 0);

    if (getParameter(HABILITACION_BONOS_KIOSCO, "1") == TRUE){
        setFieldTEFF(FIELD_TEFF_MONTO, auxiliar, 0);
    }

    if (atoi(consultarHabilitada) == 1) {
        setFieldTEFF(FIELD_TEFF_MONTO, datos->totalVenta,
                     strlen(datos->totalVenta));

        for (idx = 0; idx < strlen(datos->track2); idx++) {
            if (datos->track2[idx] == 'D' || datos->track2[idx] == 'd') {
                break;
            }
        }
        memcpy(pan, datos->track2, idx);
        setFieldTEFF(FIELD_TEFF_NUMERO_BONO, pan, idx);
    }

    packMessage = packMessageTEFF();

    respuesta = realizarTransaccionMedioComunicacion(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 30*100);

    respondeCaja = respuesta;
    if (respondeCaja == 0) {
        setParameter(CAJA_NO_RESPONDE, "1");
        //screenMessage("COMUNICACION", "CAJA", "SIN RESPUESTA", "", 2 * 1000);
    }

    if (respuesta > 0) {
        longitud = respuesta;
        respuesta = unpackRespuestaCaja(bufferOut, longitud, TRANSACCION_TEFF_HABILITACIONES);

    }

    if (respuesta > 0) {
        respuesta = validarCodigoCajero(datos->codigoCajero);
    }

    if (respuesta > 0) {

        cargarCamposEstructuraDatosVenta(claveSupervisor, FIELD_TEFF_CLAVE_SUPERVISOR);
        cargarCamposEstructuraDatosVenta(datos->totalVenta, FIELD_TEFF_MONTO);
        ajustarDatosTransaccion(datos->totalVenta, strlen(datos->totalVenta), DATOS_SALIDA);
        /*
        if (verificarHabilitacionObjeto(HABILITACION_BONOS_KIOSCO) == TRUE) {
            respuesta = 1;
        } else {

            respuesta = validarClaveSupervisorHabilitacionBono(claveSupervisor);

        }
        */
        claveValida = respuesta;
    }
    if (respondeCaja > 0) {
        memset(auxiliar, 0x00, sizeof(auxiliar));
        auxiliar[0] = ACK;
        //realizarTransaccionCaja(auxiliar, 1, bufferOut, 200, 10);
        realizarTransaccionMedioComunicacion(auxiliar, 1, bufferOut,10);
        memcpy(datos->codigoRespuesta, "01", 2);
    }
    if (respuesta > 0) {
        if (claveValida != TRUE) {

            if (validarCodigoCajero(datos->codigoCajero) >= 0) {

                memcpy(datos->codigoRespuesta, "03", 2);
                memset(&packMessage, 0x00, sizeof(packMessage));
                packMessage = tramaRespuestaClaveInvalida(TRANSACCION_TEFF_HABILITACIONES);
                //realizarTransaccionCaja(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 200, 30 * 100);
                realizarTransaccionMedioComunicacion(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 30*100);

            }
        }
    }

    return respuesta;
}

int transaccionRespuestaDatosBono(DatosVenta datos, int transaccionBono) {

    int respuesta = 0;
    char pan[LEN_TEFF_PAN + 1];
    char bufferOut[512 + 1];
    char auxiliar[30 + 1];
    char numeroBono[LEN_TEFF_NUMERO_BONO + 1];
    TablaUnoInicializacion tablaUno;
    ResultISOPack packMessage;
    ResultISOPack packMessageRespaldo;

    memset(pan, 0x00, sizeof(pan));
    memset(&tablaUno, 0x00, sizeof(tablaUno));
    memset(&packMessage, 0x00, sizeof(packMessage));
    memset(&packMessageRespaldo, 0x00, sizeof(packMessageRespaldo));
    memset(bufferOut, 0x00, sizeof(bufferOut));
    memset(auxiliar, 0x30, sizeof(auxiliar));
    memset(numeroBono, 0x00, sizeof(numeroBono));

    tablaUno = _desempaquetarTablaCeroUno_();

    setTipoMensajeTEFF(MENSAJE_TEFF_RESPUESTA);
    setCodigoTransaccionTEFF(transaccionBono);
    validacionCodigoRespuestaCaja(datos.codigoRespuesta);
    setCodigoRespuestaTEFF(atoi(datos.codigoRespuesta));
    setFieldTEFF(FIELD_TEFF_CODIGO_RESPUESTA, datos.codigoRespuesta, LEN_TEFF_CODIGO_RESPUESTA);
    ajustarDatosTransaccion(datos.codigoAprobacion, LEN_TEFF_CODIGO_AUTORIZACION, DATOS_ENTRADA);
    setFieldTEFF(FIELD_TEFF_CODIGO_AUTORIZACION, datos.codigoAprobacion, LEN_TEFF_CODIGO_AUTORIZACION);
    validarPosEntryMode(2, datos.posEntrymode);

    if (transaccionBono == TRANSACCION_TEFF_RECARGA_BONO) {
        extraerPanAsteriscos(datos.tokenVivamos, strlen(datos.tokenVivamos), pan, ASTERISCOS_4_ULTIMOS);
    } else {
        extraerPanAsteriscos(datos.track2, strlen(datos.track2), pan, ASTERISCOS_4_ULTIMOS);
    }

    setFieldTEFF(FIELD_TEFF_ASTERISCOS_4_ULTIMOS_TARJETA, pan, LEN_TEFF_ASTERISCOS_4_ULTIMOS_TARJETA);

    if (verificarCodigoCajero() > 0) {

        setFieldTEFF(FIELD_TEFF_CODIGO_CAJERO, datos.codigoCajero, LEN_TEFF_CODIGO_CAJERO);
    }

    ajustarDatosTransaccion(datos.totalVenta, LEN_TEFF_MONTO, DATOS_ENTRADA_MONTOS);
    setFieldTEFF(FIELD_TEFF_MONTO, datos.totalVenta, LEN_TEFF_MONTO);
    ajustarDatosTransaccion(datos.numeroRecibo, LEN_TEFF_NUMERO_RECIBO_DATAFONO, DATOS_ENTRADA);
    setFieldTEFF(FIELD_TEFF_NUMERO_RECIBO_DATAFONO, datos.numeroRecibo, LEN_TEFF_NUMERO_RECIBO_DATAFONO);
    setFieldTEFF(FIELD_TEFF_CUOTAS, auxiliar, LEN_TEFF_CUOTAS);
    ajustarDatosTransaccion(datos.rrn, LEN_TEFF_RRN, DATOS_ENTRADA);
    setFieldTEFF(FIELD_TEFF_RRN, datos.rrn, LEN_TEFF_RRN);
    setFieldTEFF(FIELD_TEFF_NUMERO_TERMINAL, datos.terminalId, LEN_TEFF_NUMERO_TERMINAL);
    setFieldTEFF(FIELD_TEFF_CODIGO_ESTABLECIMIENTO, tablaUno.receiptLine3, LEN_TEFF_CODIGO_ESTABLECIMIENTO);

    if (strlen(datos.txnDate) > 0) {
        ajusteFecha(datos.txnDate);
    } else {
        memset(datos.txnTime, 0x00, sizeof(datos.txnTime));
        memset(datos.txnDate, 0x00, sizeof(datos.txnDate));
        horaFechaTerminal(datos.txnTime, datos.txnDate);
    }

    setFieldTEFF(FIELD_TEFF_FECHA, datos.txnDate, LEN_TEFF_FECHA);
    setFieldTEFF(FIELD_TEFF_HORA, datos.txnTime, LEN_TEFF_HORA);

    if (transaccionBono == TRANSACCION_TEFF_HABILITACIONES) {
        setFieldTEFF(FIELD_TEFF_VALOR_HABILITACION_BONO, datos.totalVenta, LEN_TEFF_VALOR_HABILITACION_BONO);
    } else {
        setFieldTEFF(FIELD_TEFF_VALOR_REDENCION_BONO, datos.totalVenta, LEN_TEFF_VALOR_REDENCION_BONO);
    }

    memset(auxiliar, 0x00, sizeof(auxiliar));

    if (transaccionBono == TRANSACCION_TEFF_RECARGA_BONO) {
        memcpy(auxiliar, datos.tokenVivamos, strlen(datos.tokenVivamos));
    } else {
        memcpy(auxiliar, datos.track2, strcspn(datos.track2, "D"));
    }

    _rightPad_(numeroBono, auxiliar, 0x30, LEN_TEFF_NUMERO_BONO);
    setFieldTEFF(FIELD_TEFF_NUMERO_BONO, numeroBono, LEN_TEFF_NUMERO_BONO);
    packMessage = packMessageTEFF();
    packMessageRespaldo = packMessage;
    //respuesta = realizarTransaccionCaja(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 200, 10 * 100);
    respuesta = realizarTransaccionMedioComunicacion(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 10*100);

    if (respuesta <= 0) {
        setParameter(CAJA_NO_RESPONDE, "1");
        //screenMessage("COMUNICACION", "CAJA", "SIN RESPUESTA", "", 2 * 1000);
    }
    respuesta = ((bufferOut[0] == ACK) ? 1 : -1);

    if (respuesta <= 0 && transaccionBono != TRANSACCION_TEFF_SINCRONIZACION) {
        guardarTransaccionCajasSincronizacion(packMessage.isoPackMessage, packMessage.totalBytes);
    }

    return respuesta;
}

/**
 * @brief Funcion que se encarga de verificar, validar  y desempaquetar la trama
 *        de respuesta enviada por la caja para redimr el bono.
 * @param datos
 * @return 1 ok , -1 ko
 * @author .
 */
int transaccionPeticionDatosRedimirBono(DatosVenta *datos) {
    int respuesta = 0;
    int respondeCaja = 0;
    int longitud = 0;
    char auxiliar[30 + 1];
    char bufferOut[512 + 1];

    ResultISOPack packMessage;

    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(&packMessage, 0x00, sizeof(packMessage));
    memset(bufferOut, 0x00, sizeof(bufferOut));

    setTipoMensajeTEFF(MENSAJE_TEFF_REQUERIMIENTO);
    setCodigoTransaccionTEFF(TRANSACCION_TEFF_REDENCION);
    setCodigoRespuestaTEFF(NO_HAY_CODIGO_DE_RESPUESTA);

    validarPosEntryMode(2, datos->posEntrymode);

    if (verificarCodigoCajero() > 0) {
        setFieldTEFF(FIELD_TEFF_CODIGO_CAJERO, auxiliar, 0);

    }

    setFieldTEFF(FIELD_TEFF_MONTO, auxiliar, 0);

    packMessage = packMessageTEFF();

    respuesta = realizarTransaccionMedioComunicacion(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 100000);

    respondeCaja = respuesta;

    if (respondeCaja == 0) {
        setParameter(CAJA_NO_RESPONDE, "1");
        //screenMessage("COMUNICACION", "CAJA", "SIN RESPUESTA", "", 2 * 1000);
    }

    if (respuesta > 0) {
        longitud = respuesta;
        respuesta = unpackRespuestaCaja(bufferOut, longitud, TRANSACCION_TEFF_REDENCION);

    }

    if (respuesta > 0) {
        respuesta = validarCodigoCajero(datos->codigoCajero);
    }

    if (respuesta > 0) {

        cargarCamposEstructuraDatosVenta(datos->totalVenta, FIELD_TEFF_MONTO);
        ajustarDatosTransaccion(datos->totalVenta, strlen(datos->totalVenta), DATOS_SALIDA);
    }
    if (respondeCaja > 0) {
        memcpy(datos->codigoRespuesta, "01", 2);
        memset(auxiliar, 0x00, sizeof(auxiliar));
        auxiliar[0] = ACK;
        realizarTransaccionAnulacionMedioComunicacion(auxiliar, 1, bufferOut,10);
    }

    return respuesta;
}

int transaccionPeticionDatosRecargaBono(DatosVenta *datos) {
    int respuesta = 0;
    int respondeCaja = 0;
    int longitud = 0;
    int claveValida = 0;
    char auxiliar[30 + 1];
    char bufferOut[512 + 1];
    char claveSupervisor[4 + 1];

    ResultISOPack packMessage;

    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(&packMessage, 0x00, sizeof(packMessage));
    memset(bufferOut, 0x00, sizeof(bufferOut));
    memset(claveSupervisor, 0x00, sizeof(claveSupervisor));

    setTipoMensajeTEFF(MENSAJE_TEFF_REQUERIMIENTO);
    setCodigoTransaccionTEFF(TRANSACCION_TEFF_RECARGA_BONO);
    setCodigoRespuestaTEFF(NO_HAY_CODIGO_DE_RESPUESTA);

    validarPosEntryMode(2, datos->posEntrymode);

    if (verificarCodigoCajero() > 0) {
        setFieldTEFF(FIELD_TEFF_CODIGO_CAJERO, auxiliar, 0);

    }

    setFieldTEFF(FIELD_TEFF_MONTO, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_CLAVE_SUPERVISOR, auxiliar, 0);

    packMessage = packMessageTEFF();

    //respuesta = realizarTransaccionCaja(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 200, 100000);
    respuesta = realizarTransaccionMedioComunicacion(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 100000);
    respondeCaja = respuesta;
    if (respondeCaja == 0) {
        setParameter(CAJA_NO_RESPONDE, "1");
        //screenMessage("COMUNICACION", "CAJA", "SIN RESPUESTA", "", 2 * 1000);
    }

    if (respuesta > 0) {
        longitud = respuesta;
        respuesta = unpackRespuestaCaja(bufferOut, longitud, TRANSACCION_TEFF_RECARGA_BONO);

    }

    if (respuesta > 0) {

        respuesta = validarCodigoCajero(datos->codigoCajero);
    }

    if (respuesta > 0) {

        cargarCamposEstructuraDatosVenta(claveSupervisor, FIELD_TEFF_CLAVE_SUPERVISOR);
        respuesta = validarClaveSupervisor(claveSupervisor, TRANSACCION_TEFF_RECARGA_BONO);
        claveValida = respuesta;

    }
    memcpy(datos->codigoRespuesta, "01", 2);
    if (respuesta > 0) {

        cargarCamposEstructuraDatosVenta(datos->totalVenta, FIELD_TEFF_MONTO);
        ajustarDatosTransaccion(datos->totalVenta, strlen(datos->totalVenta), DATOS_SALIDA);
    }
    if (respondeCaja > 0) {

        memset(auxiliar, 0x00, sizeof(auxiliar));
        auxiliar[0] = ACK;
        //realizarTransaccionCaja(auxiliar, 1, bufferOut, 200, 10);
        realizarTransaccionMedioComunicacion(auxiliar, 1, bufferOut,10);
    }

    if (claveValida != TRUE) {

        memset(&packMessage, 0x00, sizeof(packMessage));
        packMessage = tramaRespuestaClaveInvalida(TRANSACCION_TEFF_RECARGA_BONO);
        //realizarTransaccionCaja(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 200, 200);
        realizarTransaccionMedioComunicacion(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 200);
    }
    return respuesta;
}

/**
 * @brief Funcion que se encarga de verificar, validar  y desempaquetar la trama
 *        de respuesta enviada por la caja para Recargas en efectivo.
 * @param datos
 * @return 1 ok , -1 ko
 * @author .
 */
int transaccionPeticionRecargasEfectivo(DatosVenta *datos) {
    int respuesta = 0;
    int longitud = 0;
    char auxiliar[30 + 1];
    char bufferOut[512 + 1];

    ResultISOPack packMessage;

    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(&packMessage, 0x00, sizeof(packMessage));
    memset(bufferOut, 0x00, sizeof(bufferOut));

    setTipoMensajeTEFF(MENSAJE_TEFF_REQUERIMIENTO);
    setCodigoTransaccionTEFF(TRANSACCION_TEFF_RECARGA_EFECTIVO);
    setCodigoRespuestaTEFF(NO_HAY_CODIGO_DE_RESPUESTA);

    if (verificarCodigoCajero() > 0) {
        setFieldTEFF(FIELD_TEFF_CODIGO_CAJERO, auxiliar, 0);

    }

    setFieldTEFF(FIELD_TEFF_MONTO, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_CODIGO_OPERADOR, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_CLAVE_USUARIO, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_NUMERO_FACTURA, auxiliar, 0);

    packMessage = packMessageTEFF();

    //respuesta = realizarTransaccionCaja(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 200, 30 * 100);
    respuesta = realizarTransaccionMedioComunicacion(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 30 * 100);
    if (respuesta > 0) {
        longitud = respuesta;
        respuesta = unpackRespuestaCaja(bufferOut, longitud, TRANSACCION_TEFF_HABILITACIONES);

    }

    if (respuesta > 0) {
        respuesta = validarCodigoCajero(datos->codigoCajero);
    }

    if (respuesta > 0) {

    }

    memset(auxiliar, 0x00, sizeof(auxiliar));
    auxiliar[0] = ACK;
    //realizarTransaccionCaja(auxiliar, 1, bufferOut, 200, 10);
    realizarTransaccionMedioComunicacion(auxiliar, 1, bufferOut,10);

    return respuesta;
}

/**
 * @brief Funcion encargada de em`paquetar la trama de avance efectivo.
 * @param datos
 * @return 1 ok , -1 ko
 */
int transaccionPeticionEfectivo(DatosVenta *datos) {
    int respuesta = 0;
    int respondeCaja = 0;
    int longitud = 0;
    long compraNeta = 0;
    char pan[LEN_TEFF_PAN + 1];
    char auxiliar[30 + 1];
    char auxiliarMonto[12 + 1];
    char bufferOut[512 + 1];
    ResultISOPack packMessage;

    memset(pan, 0x00, sizeof(pan));
    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(&packMessage, 0x00, sizeof(packMessage));
    memset(bufferOut, 0x00, sizeof(bufferOut));
    memset(auxiliarMonto, 0x00, sizeof(auxiliarMonto));

    setTipoMensajeTEFF(MENSAJE_TEFF_REQUERIMIENTO);
    setCodigoTransaccionTEFF(TRANSACCION_TEFF_COMPRAS);
    setCodigoRespuestaTEFF(NO_HAY_CODIGO_DE_RESPUESTA);

    validarPosEntryMode(2, datos->posEntrymode);

    if (verificarCodigoCajero() > 0) {
        setFieldTEFF(FIELD_TEFF_CODIGO_CAJERO, auxiliar, 0);

    }

    extraerPanAsteriscos(datos->track2, strlen(datos->track2), pan, ASTERISCOS_4_ULTIMOS);
    setFieldTEFF(FIELD_TEFF_ASTERISCOS_4_ULTIMOS_TARJETA, pan, LEN_TEFF_PAN);
    setFieldTEFF(FIELD_TEFF_MONTO, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_IVA, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_BASE_DEVOLUCION, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_INC, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_NUMERO_FACTURA_ENVIADO_CAJA, auxiliar, 0);

    packMessage = packMessageTEFF();
    //respuesta = realizarTransaccionCaja(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 200, 30 * 100);
    respuesta = realizarTransaccionMedioComunicacion(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 30 * 100);

    respondeCaja = respuesta;

    if (respondeCaja == 0) {
        setParameter(CAJA_NO_RESPONDE, "1");
        //screenMessage("COMUNICACION", "CAJA", "SIN RESPUESTA", "", 2 * 1000);
    }

    if (respuesta > 0) {
        longitud = respuesta;
        respuesta = unpackRespuestaCaja(bufferOut, longitud, TRANSACCION_TEFF_COMPRAS);

    }

    if (respuesta > 0) {
        respuesta = validarCodigoCajero(datos->codigoCajero);
    }

    if (respuesta > 0) {

        cargarCamposEstructuraDatosVenta(datos->totalVenta, FIELD_TEFF_MONTO);
        ajustarDatosTransaccion(datos->totalVenta, strlen(datos->totalVenta), DATOS_SALIDA);
        cargarCamposEstructuraDatosVenta(datos->iva, FIELD_TEFF_IVA);
        ajustarDatosTransaccion(datos->iva, strlen(datos->iva), DATOS_SALIDA);
        cargarCamposEstructuraDatosVenta(datos->baseDevolucion, FIELD_TEFF_BASE_DEVOLUCION);
        ajustarDatosTransaccion(datos->baseDevolucion, strlen(datos->baseDevolucion), DATOS_SALIDA);
        cargarCamposEstructuraDatosVenta(datos->facturaCaja, FIELD_TEFF_NUMERO_FACTURA_ENVIADO_CAJA);
        ajustarDatosTransaccion(datos->facturaCaja, strlen(datos->facturaCaja), DATOS_SALIDA);
        cargarCamposEstructuraDatosVenta(datos->inc, FIELD_TEFF_INC);
        ajustarDatosTransaccion(datos->inc, strlen(datos->inc), DATOS_SALIDA);

        sprintf(auxiliarMonto, "%ld", compraNeta);
        memcpy(datos->compraNeta, auxiliarMonto, 12);

        respuesta = verificarMontosInconsistentes(datos);
    }

    if (respondeCaja > 0) {
        memset(auxiliar, 0x00, sizeof(auxiliar));
        auxiliar[0] = ACK;
        //realizarTransaccionCaja(auxiliar, 1, bufferOut, 200, 10);
        realizarTransaccionMedioComunicacion(auxiliar, 1, bufferOut,10);
    }

    if (respuesta < 0) {
        setParameter(USO_CAJA_REGISTRADORA, "1");
    }

    return respuesta;
}

int transaccionPeticionTarjetaPrivada(DatosVenta *datos) {

    int respuesta = 0;
    int respondeCaja = 0;
    int longitud = 0;
    long compraNeta = 0;
    char pan[LEN_TEFF_PAN + 1];
    char auxiliar[30 + 1];
    char auxiliarMonto[12 + 1];
    char bufferOut[512 + 1];
    char facturaAuxiliar[30 + 1];
    ResultISOPack packMessage;
    int tipoDocumento = 0;
    int campoRecibido = 0;

    memset(pan, 0x00, sizeof(pan));
    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(&packMessage, 0x00, sizeof(packMessage));
    memset(bufferOut, 0x00, sizeof(bufferOut));
    memset(auxiliarMonto, 0x00, sizeof(auxiliarMonto));
    memset(facturaAuxiliar, 0x30, sizeof(facturaAuxiliar));

    setTipoMensajeTEFF(MENSAJE_TEFF_REQUERIMIENTO);
    setCodigoTransaccionTEFF(TRANSACCION_TEFF_TARJETA_PRIVADA); //TRANSACCION_TEFF_PAGO_TARJETA_PRIVADA
    setCodigoRespuestaTEFF(NO_HAY_CODIGO_DE_RESPUESTA);

    validarPosEntryMode(1, datos->posEntrymode);

    if (strlen(datos->track2) > 0) {
        extraerPanAsteriscos(datos->track2, strlen(datos->track2), pan, ASTERISCOS_4_ULTIMOS);
        setFieldTEFF(FIELD_TEFF_ASTERISCOS_4_ULTIMOS_TARJETA, pan, LEN_TEFF_PAN);
        tipoDocumento = 1; //ingresó por tarjeta
    }

    if (verificarCodigoCajero() > 0) {
        setFieldTEFF(FIELD_TEFF_CODIGO_CAJERO, auxiliar, 0); //! campo 33
    }

    setFieldTEFF(FIELD_TEFF_MONTO, auxiliar, 0); //! campo 40

    if (tipoDocumento != 1) {
        setFieldTEFF(FIELD_TEFF_CODIGO_CONVENIO, auxiliar, 0); //! campo 45 si es recaudo con tare
    }

    setFieldTEFF(FIELD_TEFF_TIPO_DOCUMENTO, auxiliar, 0); //! campo 69
    setFieldTEFF(FIELD_TEFF_CODIGO_OPERADOR, auxiliar, 0); //! campo 89

    if (tipoDocumento != 1) {
        setFieldTEFF(FIELD_TEFF_NUMERO_FACTURA, auxiliar, 0); //! campo 93
        tipoDocumento = 2; //ingresó por documento
    }

    packMessage = packMessageTEFF();

    //respuesta = realizarTransaccionCaja(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 200, 30 * 100);
    respuesta = realizarTransaccionMedioComunicacion(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 30 * 100);
    respondeCaja = respuesta;

    if (respondeCaja == 0) {
        setParameter(CAJA_NO_RESPONDE, "1");
        //screenMessage("COMUNICACION", "CAJA", "SIN RESPUESTA", "", 2 * 1000);
    }

    if (respuesta > 1) {
        longitud = respuesta;
        respuesta = unpackRespuestaCaja(bufferOut, longitud, TRANSACCION_TEFF_TARJETA_PRIVADA);
    } else {
        respuesta = 0;
    }

    if (respuesta > 0) {
        respuesta = validarCodigoCajero(datos->codigoCajero);
    }

    if (respuesta > 0) {

        if (tipoDocumento == 2) {
            cargarCamposEstructuraDatosVenta(datos->facturaCaja, FIELD_TEFF_NUMERO_FACTURA);

            int tamano = 0;
            tamano = strlen(datos->facturaCaja);
            if(strncmp(facturaAuxiliar,datos->facturaCaja, tamano) == 0 ){
                setParameter(CAJA_NO_RESPONDE, "1");
                //screenMessage("COMUNICACION", "CAJA", "DECLINADA", "", 2 * 1000);
                respuesta = 0;
            }
        }

        cargarCamposEstructuraDatosVenta(datos->totalVenta, FIELD_TEFF_MONTO);
        ajustarDatosTransaccion(datos->totalVenta, strlen(datos->totalVenta), DATOS_SALIDA);
        cargarCamposEstructuraDatosVenta(datos->modoDatafono, FIELD_TEFF_TIPO_DOCUMENTO);
        ajustarDatosTransaccion(datos->modoDatafono, strlen(datos->modoDatafono), DATOS_SALIDA);
        cargarCamposEstructuraDatosVenta(datos->baseDevolucion, FIELD_TEFF_CODIGO_OPERADOR);
        ajustarDatosTransaccion(datos->baseDevolucion, strlen(datos->baseDevolucion), DATOS_SALIDA);

        ajustarDatosTransaccion(datos->facturaCaja, strlen(datos->facturaCaja), DATOS_SALIDA);
        ajustarDatosTransaccion(datos->tokenVivamos, strlen(datos->tokenVivamos), DATOS_SALIDA);

        campoRecibido = cargarCamposEstructuraDatosVenta(datos->tokenVivamos + 1, FIELD_TEFF_CODIGO_CONVENIO);

        if (campoRecibido == 1) { // verificar que el campo 45 se
            datos->tokenVivamos[0] = '1';
        } else {
            memset(datos->tokenVivamos, 0x00, sizeof(datos->tokenVivamos));
            datos->tokenVivamos[0] = '0';
        }

        sprintf(auxiliarMonto, "%ld", compraNeta);
        memcpy(datos->compraNeta, auxiliarMonto, 12);

    }

    if (respondeCaja > 0) {
        memset(auxiliar, 0x00, sizeof(auxiliar));
        auxiliar[0] = ACK;
        //realizarTransaccionCaja(auxiliar, 1, bufferOut, 200, 10);
        realizarTransaccionMedioComunicacion(auxiliar, 1, bufferOut,10);
    }

    return respuesta;
}

int transaccionPeticionRetiroEfectivo(DatosVenta *datos) {

    int respuesta = 0;
    int respondeCaja = 0;
    int longitud = 0;
    long compraNeta = 0;
    char pan[LEN_TEFF_PAN + 1];
    char auxiliar[30 + 1];
    char bufferOut[512 + 1];
    char cuentaAuxiliar[30 + 1];
    int resultadoCampo = 0;
    ResultISOPack packMessage;

    memset(pan, 0x00, sizeof(pan));
    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(&packMessage, 0x00, sizeof(packMessage));
    memset(bufferOut, 0x00, sizeof(bufferOut));
    memset(cuentaAuxiliar, 0x00, sizeof(cuentaAuxiliar));

    setTipoMensajeTEFF(MENSAJE_TEFF_REQUERIMIENTO);
    setCodigoTransaccionTEFF(TRANSACCION_TEFF_RETIRO_SIN_TARJETA);
    setCodigoRespuestaTEFF(NO_HAY_CODIGO_DE_RESPUESTA);

    validarPosEntryMode(2, datos->posEntrymode);

    if (verificarCodigoCajero() > 0) {
        setFieldTEFF(FIELD_TEFF_CODIGO_CAJERO, auxiliar, 0);
    }

    extraerPanAsteriscos(datos->track2, strlen(datos->track2), pan, ASTERISCOS_4_ULTIMOS);
    setFieldTEFF(FIELD_TEFF_ASTERISCOS_4_ULTIMOS_TARJETA, pan, LEN_TEFF_PAN);
    setFieldTEFF(FIELD_TEFF_MONTO, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_NUMERO_FACTURA_ENVIADO_CAJA, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_NUMERO_CUENTA, auxiliar, 0);

    packMessage = packMessageTEFF();

    //respuesta = realizarTransaccionCaja(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 200, 30 * 100);
    respuesta = realizarTransaccionMedioComunicacion(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 30 * 100);
    respondeCaja = respuesta;

    if (respondeCaja == 0) {
        setParameter(CAJA_NO_RESPONDE, "1");
        //screenMessage("COMUNICACION", "CAJA", "SIN RESPUESTA", "", 2 * 1000);
    }

    if (respuesta > 0) {
        longitud = respuesta;

        respuesta = unpackRespuestaCaja(bufferOut, longitud, TRANSACCION_TEFF_RETIRO_SIN_TARJETA);
    }

    if (respuesta > 0) {
        respuesta = validarCodigoCajero(datos->codigoCajero);
    }

    if (respuesta > 0) {

        cargarCamposEstructuraDatosVenta(datos->totalVenta, FIELD_TEFF_MONTO);
        ajustarDatosTransaccion(datos->totalVenta, strlen(datos->totalVenta), DATOS_SALIDA);
        cargarCamposEstructuraDatosVenta(datos->field57, FIELD_TEFF_NUMERO_DOCUMENTO);
        ajustarDatosTransaccion(datos->field57, strlen(datos->field57), LEN_TEFF_NUMERO_DOCUMENTO);
        cargarCamposEstructuraDatosVenta(datos->propina, FIELD_TEFF_NUMERO_CUENTA);
        ajustarDatosTransaccion(datos->propina, LEN_TEFF_NUMERO_CUENTA, DATOS_SALIDA);

        resultadoCampo = cargarCamposEstructuraDatosVenta(auxiliar, FIELD_TEFF_NUMERO_CUENTA);

        if (resultadoCampo > 0) {
            _rightTrim_(cuentaAuxiliar, auxiliar, 0x30);
            leftPad(datos->tokenVivamos, cuentaAuxiliar, 0x30, SIZE_CUENTA);
        }

        if (compraNeta < 0) {
            respuesta = -1;
            //screenMessage("COMUNICACION", "MONTOS", "INCONSISTENTES", "", 2 * 1000);
        }
    }

    if (respondeCaja > 0) {
        memset(auxiliar, 0x00, sizeof(auxiliar));
        auxiliar[0] = ACK;
        //realizarTransaccionCaja(auxiliar, 1, bufferOut, 200, 10);
        realizarTransaccionMedioComunicacion(auxiliar, 1, bufferOut,10);
    }

    return respuesta;
}
/**
 * @brief Funcion que se encarga de verificar, validar  y desempaquetar la trama
 *         de respuesta enviada por la caja para una compra.
 * @param datos
 * @return 1 ok , -1 ko
 * @author Yeny Alexandra Gantivar.
 */
/**
 * @brief Funcion que se encarga de verificar, validar  y desempaquetar la trama
 *         de respuesta enviada por la caja para una compra.
 * @param datos
 * @return 1 ok , -1 ko
 * @author Yeny Alexandra Gantivar.
 */
int transaccionAnulacionBancolombia(DatosVenta *datos) {

    int respuesta = 0;
    int respondeCaja = 0;
    int longitud = 0;
    long compraNeta = 0;
    char pan[LEN_TEFF_PAN + 1];
    char auxiliar[30 + 1];
    char bufferOut[512 + 1];
    uChar recibo[TAM_NUMERO_RECIBO + 1];
    uChar reciboAux[TAM_NUMERO_RECIBO + 1];
    ResultISOPack packMessage;

    memset(pan, 0x00, sizeof(pan));
    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(&packMessage, 0x00, sizeof(packMessage));
    memset(bufferOut, 0x00, sizeof(bufferOut));
    memset(recibo, 0x00, sizeof(recibo));
    memset(reciboAux, 0x00, sizeof(reciboAux));

    setTipoMensajeTEFF(MENSAJE_TEFF_REQUERIMIENTO);
    setCodigoTransaccionTEFF(TRANSACCION_TEFF_ANULACION);
    setCodigoRespuestaTEFF(NO_HAY_CODIGO_DE_RESPUESTA);

    if (verificarCodigoCajero() > 0) {
        setFieldTEFF(FIELD_TEFF_CODIGO_CAJERO, auxiliar, 0);
    }

    extraerPanAsteriscos(datos->track2, strlen(datos->track2), pan, ASTERISCOS_4_ULTIMOS);
    setFieldTEFF(FIELD_TEFF_ASTERISCOS_4_ULTIMOS_TARJETA, pan, LEN_TEFF_PAN);
    setFieldTEFF(FIELD_TEFF_NUMERO_RECIBO_DATAFONO, auxiliar, 0);

    packMessage = packMessageTEFF();

    //respuesta = realizarTransaccionCaja(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 200, 30 * 100);
    respuesta = realizarTransaccionMedioComunicacion(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 30 * 100);
    respondeCaja = respuesta;

    if (respondeCaja == 0) {
        setParameter(CAJA_NO_RESPONDE, "1");
        //screenMessage("COMUNICACION", "CAJA", "SIN RESPUESTA", "", 2 * 1000);
    }

    if (respuesta > 0) {
        longitud = respuesta;

        // Para caja de olimpica se coloca validacion TRANSACCION_TEFF_RETIRO_SIN_TARJETA, vericar
        // si esto afecta las cajas  del exito.

        respuesta = unpackRespuestaCaja(bufferOut, longitud, TRANSACCION_TEFF_ANULACION);
        //respuesta = unpackRespuestaCaja (bufferOut,longitud,0);
    }
    if (respuesta > 0) {
        respuesta = validarCodigoCajero(datos->codigoCajero);
    }

    if (respuesta > 0) {
        cargarCamposEstructuraDatosVenta(recibo, FIELD_TEFF_NUMERO_RECIBO_DATAFONO);
        ajustarDatosTransaccion(recibo, strlen(recibo), DATOS_SALIDA);
        _rightTrim_(reciboAux, recibo, 0x20);
        leftPad(datos->numeroRecibo, reciboAux, '0', TAM_NUMERO_RECIBO);
        //	cargarCamposEstructuraDatosVenta(datos->numeroRecibo,FIELD_TEFF_NUMERO_RECIBO_DATAFONO);
        //	ajustarDatosTransaccion(datos->numeroRecibo,strlen(datos->numeroRecibo),DATOS_SALIDA);

        if (compraNeta < 0) {
            respuesta = -1;
            //screenMessage("COMUNICACION", "MONTOS", "INCONSISTENTES", "", 2 * 1000);
        }
    }

    if (respondeCaja > 0) {
        memset(auxiliar, 0x00, sizeof(auxiliar));
        auxiliar[0] = ACK;
        //realizarTransaccionCaja(auxiliar, 1, bufferOut, 200, 10);
        realizarTransaccionMedioComunicacion(auxiliar, 1, bufferOut,10);
    }

    return respuesta;
}

int transaccionPeticionDeposito(DatosVenta *datos) {

    int respuesta = 0;
    int respondeCaja = 0;
    int longitud = 0;
    long compraNeta = 0;
    char pan[LEN_TEFF_PAN + 1];
    char auxiliar[30 + 1];
    char bufferOut[512 + 1];
    ResultISOPack packMessage;

    memset(pan, 0x00, sizeof(pan));
    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(&packMessage, 0x00, sizeof(packMessage));
    memset(bufferOut, 0x00, sizeof(bufferOut));

    setTipoMensajeTEFF(MENSAJE_TEFF_REQUERIMIENTO);
    setCodigoTransaccionTEFF(TRANSACCION_TEFF_DEPOSITO_BCL);
    setCodigoRespuestaTEFF(NO_HAY_CODIGO_DE_RESPUESTA);

    validarPosEntryMode(2, datos->posEntrymode);

    if (verificarCodigoCajero() > 0) {
        setFieldTEFF(FIELD_TEFF_CODIGO_CAJERO, auxiliar, 0);
    }

    extraerPanAsteriscos(datos->track2, strlen(datos->track2), pan, ASTERISCOS_4_ULTIMOS);
    setFieldTEFF(FIELD_TEFF_ASTERISCOS_4_ULTIMOS_TARJETA, pan, LEN_TEFF_PAN);
    setFieldTEFF(FIELD_TEFF_MONTO, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_NUMERO_FACTURA_ENVIADO_CAJA, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_NUMERO_CUENTA, auxiliar, 0);

    packMessage = packMessageTEFF();

    //respuesta = realizarTransaccionCaja(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 200, 30 * 100);
    respuesta = realizarTransaccionMedioComunicacion(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 30 * 100);
    respondeCaja = respuesta;

    if (respondeCaja == 0) {
        setParameter(CAJA_NO_RESPONDE, "1");
        //screenMessage("COMUNICACION", "CAJA", "SIN RESPUESTA", "", 2 * 1000);
    }

    if (respuesta > 0) {
        longitud = respuesta;
        respuesta = unpackRespuestaCaja(bufferOut, longitud, TRANSACCION_TEFF_DEPOSITO_BCL);
    }

    if (respuesta > 0) {
        respuesta = validarCodigoCajero(datos->codigoCajero);
    }

    if (respuesta > 0) {

        cargarCamposEstructuraDatosVenta(datos->totalVenta, FIELD_TEFF_MONTO);
        ajustarDatosTransaccion(datos->totalVenta, strlen(datos->totalVenta), DATOS_SALIDA);
        cargarCamposEstructuraDatosVenta(datos->field57, FIELD_TEFF_NUMERO_DOCUMENTO);
        ajustarDatosTransaccion(datos->field57, strlen(datos->field57), LEN_TEFF_NUMERO_DOCUMENTO);
        cargarCamposEstructuraDatosVenta(datos->propina, FIELD_TEFF_NUMERO_CUENTA);
        ajustarDatosTransaccion(datos->propina, LEN_TEFF_NUMERO_CUENTA, DATOS_SALIDA);

        if (compraNeta < 0) {
            respuesta = -1;
            //screenMessage("COMUNICACION", "MONTOS", "INCONSISTENTES", "", 2 * 1000);
        }
    }
    if (respondeCaja > 0) {
        memset(auxiliar, 0x00, sizeof(auxiliar));
        auxiliar[0] = ACK;
        //realizarTransaccionCaja(auxiliar, 1, bufferOut, 200, 10);
        realizarTransaccionMedioComunicacion(auxiliar, 1, bufferOut,10);
    }

    return respuesta;
}

int transaccionPeticionPagoTarjeta(DatosVenta *datos) {
    int respuesta = 0;
    int respondeCaja = 0;
    int longitud = 0;
    long compraNeta = 0;
    char pan[LEN_TEFF_PAN + 1];
    char auxiliar[30 + 1];
    char auxiliarMonto[12 + 1];
    char bufferOut[512 + 1];
    ResultISOPack packMessage;

    memset(pan, 0x00, sizeof(pan));
    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(&packMessage, 0x00, sizeof(packMessage));
    memset(bufferOut, 0x00, sizeof(bufferOut));
    memset(auxiliarMonto, 0x00, sizeof(auxiliarMonto));

    setTipoMensajeTEFF(MENSAJE_TEFF_REQUERIMIENTO);
    setCodigoTransaccionTEFF(TRANSACCION_TEFF_PAGO_TARJETA_PRIVADA); //TRANSACCION_TEFF_PAGO_TARJETA_PRIVADA
    setCodigoRespuestaTEFF(NO_HAY_CODIGO_DE_RESPUESTA);

    validarPosEntryMode(2, datos->posEntrymode);

    if (verificarCodigoCajero() > 0) {
        setFieldTEFF(FIELD_TEFF_CODIGO_CAJERO, auxiliar, 0);
    }

    setFieldTEFF(FIELD_TEFF_MONTO, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_TIPO_DOCUMENTO, auxiliar, 0);

    setFieldTEFF(FIELD_TEFF_CODIGO_OPERADOR, auxiliar, 0);

    if (strlen(datos->track2) > 0) {
        extraerPanAsteriscos(datos->track2, strlen(datos->track2), pan, ASTERISCOS_4_ULTIMOS);
        setFieldTEFF(FIELD_TEFF_ASTERISCOS_4_ULTIMOS_TARJETA, pan, LEN_TEFF_PAN);
    } else {
        setFieldTEFF(FIELD_TEFF_NUMERO_FACTURA, auxiliar, 0);
    }

    setFieldTEFF(FIELD_TEFF_CODIGO_CONVENIO, auxiliar, 0);




    packMessage = packMessageTEFF();

    //respuesta = realizarTransaccionCaja(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 200, 30 * 100);
    respuesta = realizarTransaccionMedioComunicacion(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 30 * 100);
    respondeCaja = respuesta;

    if (respondeCaja == 0) {
        setParameter(CAJA_NO_RESPONDE, "1");
        //screenMessage("COMUNICACION", "CAJA", "SIN RESPUESTA", "", 2 * 1000);
    }

    if (respuesta > 0) {
        longitud = respuesta;
        respuesta = unpackRespuestaCaja(bufferOut, longitud, TRANSACCION_TEFF_PAGO_TARJETA_PRIVADA);
    }

    if (respuesta > 0) {
        respuesta = validarCodigoCajero(datos->codigoCajero);
    }

    if (respuesta > 0) {

        //cambiar
        cargarCamposEstructuraDatosVenta(datos->totalVenta, FIELD_TEFF_MONTO);
        ajustarDatosTransaccion(datos->totalVenta, strlen(datos->totalVenta), DATOS_SALIDA);
        cargarCamposEstructuraDatosVenta(datos->iva, FIELD_TEFF_TIPO_DOCUMENTO);
        ajustarDatosTransaccion(datos->iva, strlen(datos->iva), DATOS_SALIDA);
        cargarCamposEstructuraDatosVenta(datos->baseDevolucion, FIELD_TEFF_CODIGO_OPERADOR);
        ajustarDatosTransaccion(datos->baseDevolucion, strlen(datos->baseDevolucion), DATOS_SALIDA);
        cargarCamposEstructuraDatosVenta(datos->facturaCaja, FIELD_TEFF_NUMERO_FACTURA);
        ajustarDatosTransaccion(datos->facturaCaja, strlen(datos->facturaCaja), DATOS_SALIDA);

        sprintf(auxiliarMonto, "%ld", compraNeta);
        memcpy(datos->compraNeta, auxiliarMonto, 12);

    }
    if (respondeCaja > 0) {
        memset(auxiliar, 0x00, sizeof(auxiliar));
        auxiliar[0] = ACK;
        //realizarTransaccionCaja(auxiliar, 1, bufferOut, 200, 10);
        realizarTransaccionMedioComunicacion(auxiliar, 1, bufferOut,10);
    }

    return respuesta;
}
int transaccionPeticionTransferencia(DatosVenta *datos, int transaccion) {

    int respuesta = 0;
    int respondeCaja = 0;
    int longitud = 0;
    long compraNeta = 0;
    char pan[LEN_TEFF_PAN + 1];
    char auxiliar[30 + 1];
    char auxiliarMonto[12 + 1];
    char bufferOut[512 + 1];
    ResultISOPack packMessage;
    uChar numeroCuenta[TAM_TEXTO_ADICIONAL + 1];
    uChar numeroCuentaAux[TAM_TEXTO_ADICIONAL + 1];

    memset(pan, 0x00, sizeof(pan));
    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(&packMessage, 0x00, sizeof(packMessage));
    memset(bufferOut, 0x00, sizeof(bufferOut));
    memset(auxiliarMonto, 0x00, sizeof(auxiliarMonto));
    memset(numeroCuenta, 0x00, sizeof(numeroCuenta));
    memset(numeroCuentaAux, 0x00, sizeof(numeroCuentaAux));

    setTipoMensajeTEFF(MENSAJE_TEFF_REQUERIMIENTO);
    setCodigoTransaccionTEFF(transaccion); //TRANSACCION_TEFF_PAGO_TARJETA_PRIVADA


    setCodigoRespuestaTEFF(NO_HAY_CODIGO_DE_RESPUESTA);

    validarPosEntryMode(2, datos->posEntrymode);

    if (verificarCodigoCajero() > 0) {
        setFieldTEFF(FIELD_TEFF_CODIGO_CAJERO, auxiliar, 0);
    }

    if (strlen(datos->track2) > 0) {
        extraerPanAsteriscos(datos->track2, strlen(datos->track2), pan, ASTERISCOS_4_ULTIMOS);
        setFieldTEFF(FIELD_TEFF_ASTERISCOS_4_ULTIMOS_TARJETA, pan, LEN_TEFF_PAN);
    }
    setFieldTEFF(FIELD_TEFF_MONTO, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_NUMERO_FACTURA_ENVIADO_CAJA, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_NUMERO_CUENTA, auxiliar, 0);

    packMessage = packMessageTEFF();

    //respuesta = realizarTransaccionCaja(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 200, 30 * 100);
    respuesta = realizarTransaccionMedioComunicacion(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 30 * 100);
    respondeCaja = respuesta;

    if (respondeCaja == 0) {
        setParameter(CAJA_NO_RESPONDE, "1");
        //screenMessage("COMUNICACION", "CAJA", "SIN RESPUESTA", "", 2 * 1000);
    }

    if (respuesta > 0) {
        longitud = respuesta;
        respuesta = unpackRespuestaCaja(bufferOut, longitud, transaccion);
//		respuesta = unpackRespuestaCaja (bufferOut,longitud,TRANSACCION_TEFF_COMPRAS );
    }
    if (respuesta > 0) {
        respuesta = validarCodigoCajero(datos->codigoCajero);
    }

    if (respuesta > 0) {
        //cambiar
        cargarCamposEstructuraDatosVenta(datos->totalVenta, FIELD_TEFF_MONTO);
        ajustarDatosTransaccion(datos->totalVenta, strlen(datos->totalVenta), DATOS_SALIDA);

        cargarCamposEstructuraDatosVenta(numeroCuentaAux, FIELD_TEFF_NUMERO_CUENTA);
        ajustarDatosTransaccion(numeroCuentaAux, strlen(numeroCuentaAux), DATOS_SALIDA);
        _rightTrim_(numeroCuenta, numeroCuentaAux, 0x20);

        leftPad(datos->textoAdicional, numeroCuenta, '0', SIZE_CUENTA);

        memset(numeroCuenta, 0x00, sizeof(numeroCuenta));
        memset(numeroCuentaAux, 0x00, sizeof(numeroCuentaAux));

        cargarCamposEstructuraDatosVenta(numeroCuentaAux, FIELD_TEFF_NUMERO_FACTURA);
        ajustarDatosTransaccion(numeroCuentaAux, strlen(numeroCuentaAux), DATOS_SALIDA);
        _rightTrim_(numeroCuenta, numeroCuentaAux, 0x20);
        leftPad(datos->nombreComercio, numeroCuenta, '0', SIZE_CUENTA);

        if (strcmp(datos->nombreComercio, "00000000000") == 0) {
            memset(datos->nombreComercio, 0x00, SIZE_CUENTA);
        }

        sprintf(auxiliarMonto, "%ld", compraNeta);
        memcpy(datos->compraNeta, auxiliarMonto, 12);

    }
    if (respondeCaja > 0) {
        memset(auxiliar, 0x00, sizeof(auxiliar));
        auxiliar[0] = ACK;

        realizarTransaccionMedioComunicacion(auxiliar, 1, bufferOut,10);
    }

    return respuesta;
}

int transaccionPeticionRecaudoManual(DatosVenta *datos, int transaccion) {

    int respuesta = 0;
    int respondeCaja = 0;
    int longitud = 0;
    long compraNeta = 0;
    char pan[LEN_TEFF_PAN + 1];
    char auxiliar[30 + 1];
    char auxiliarMonto[12 + 1];
    char bufferOut[512 + 1];
    ResultISOPack packMessage;
    uChar factura[20 + 1];
    uChar facturaAux[20 + 1];

    memset(pan, 0x00, sizeof(pan));
    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(&packMessage, 0x00, sizeof(packMessage));
    memset(bufferOut, 0x00, sizeof(bufferOut));
    memset(auxiliarMonto, 0x00, sizeof(auxiliarMonto));
    memset(factura, 0x00, sizeof(factura));
    memset(facturaAux, 0x00, sizeof(facturaAux));


    setTipoMensajeTEFF(MENSAJE_TEFF_REQUERIMIENTO);
    setCodigoTransaccionTEFF(transaccion); //TRANSACCION_TEFF_PAGO_TARJETA_PRIVADA
    setCodigoRespuestaTEFF(NO_HAY_CODIGO_DE_RESPUESTA);

    validarPosEntryMode(2, datos->posEntrymode);

    if (verificarCodigoCajero() > 0) {
        setFieldTEFF(FIELD_TEFF_CODIGO_CAJERO, auxiliar, 0);
    }

    if (strlen(datos->track2) > 0) {
        extraerPanAsteriscos(datos->track2, strlen(datos->track2), pan, ASTERISCOS_4_ULTIMOS);
        setFieldTEFF(FIELD_TEFF_ASTERISCOS_4_ULTIMOS_TARJETA, pan, LEN_TEFF_PAN);
    }
    setFieldTEFF(FIELD_TEFF_MONTO, auxiliar, 0);
    if (transaccion != TRANSACCION_TEFF_PAGO_TCREDITO && transaccion != TRANSACCION_TEFF_PAGO_CARTERA) {

        if (transaccion != TRANSACCION_TEFF_RECAUDO_LECTOR_BARRAS) {
            setFieldTEFF(FIELD_TEFF_CODIGO_CONVENIO, auxiliar, 0);
        }

        setFieldTEFF(FIELD_TEFF_NUMERO_FACTURA_ENVIADO_CAJA, auxiliar, 0);
    }

    if (transaccion != TRANSACCION_TEFF_RECAUDO_LECTOR_BARRAS) {
        setFieldTEFF(FIELD_TEFF_NUMERO_FACTURA, auxiliar, 0);
    }

    packMessage = packMessageTEFF();

    //respuesta = realizarTransaccionCaja(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 200, 30 * 100);
    respuesta = realizarTransaccionMedioComunicacion(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 30 * 100);
    respondeCaja = respuesta;

    if (respondeCaja == 0) {
        setParameter(CAJA_NO_RESPONDE, "1");
        //screenMessage("COMUNICACION", "CAJA", "SIN RESPUESTA", "", 2 * 1000);
    }

    if (respuesta > 0) {
        longitud = respuesta;
        respuesta = unpackRespuestaCaja(bufferOut, longitud, transaccion);
    }

    if (respuesta > 0) {
        respuesta = validarCodigoCajero(datos->codigoCajero);
    }

    if (respuesta > 0) {

        //cambiar
        cargarCamposEstructuraDatosVenta(datos->totalVenta, FIELD_TEFF_MONTO);
        ajustarDatosTransaccion(datos->totalVenta, strlen(datos->totalVenta), DATOS_SALIDA);
        cargarCamposEstructuraDatosVenta(datos->codigoComercio, FIELD_TEFF_CODIGO_CONVENIO);
        ajustarDatosTransaccion(datos->codigoComercio, strlen(datos->codigoComercio), DATOS_SALIDA);
        cargarCamposEstructuraDatosVenta(datos->facturaCaja, FIELD_TEFF_NUMERO_FACTURA);
        ajustarDatosTransaccion(datos->facturaCaja, strlen(datos->facturaCaja), DATOS_SALIDA);
        if (transaccion == TRANSACCION_TEFF_PAGO_TCREDITO || transaccion == TRANSACCION_TEFF_PAGO_CARTERA) {
            cargarCamposEstructuraDatosVenta(factura, FIELD_TEFF_NUMERO_FACTURA);
            ajustarDatosTransaccion(factura, strlen(factura), DATOS_SALIDA);
            _rightTrim_(facturaAux, factura, 0x20);
            leftPad(datos->tokenVivamos, facturaAux, '0', 19);
        }
        sprintf(auxiliarMonto, "%ld", compraNeta);
        memcpy(datos->compraNeta, auxiliarMonto, 12);
    }

    if (respondeCaja > 0) {
        memset(auxiliar, 0x00, sizeof(auxiliar));
        auxiliar[0] = ACK;
        //realizarTransaccionCaja(auxiliar, 1, bufferOut, 200, 10);
        realizarTransaccionMedioComunicacion(auxiliar, 1, bufferOut,10);
    }

    return respuesta;
}

ResultISOPack tramaRespuestaClaveInvalida(int transaccion) {

    char auxiliar[30 + 1];
    char codigoRespuesta[2 + 1];
    char numeroTerminal[SIZE_NUMERO_TERMINAL + 1];
    TablaUnoInicializacion tablaUno;
    ResultISOPack packMessage;

    memset(&tablaUno, 0x00, sizeof(tablaUno));
    memset(&packMessage, 0x00, sizeof(packMessage));
    memset(numeroTerminal, 0x00, sizeof(numeroTerminal));
    memset(auxiliar, 0x30, sizeof(auxiliar));
    memset(codigoRespuesta, 0x00, sizeof(codigoRespuesta));
    tablaUno = _desempaquetarTablaCeroUno_();

    setTipoMensajeTEFF(MENSAJE_TEFF_RESPUESTA);
    setCodigoTransaccionTEFF(transaccion);
    setCodigoRespuestaTEFF(CLAVE_INVALIDAD_TEFF);
    sprintf(codigoRespuesta, "%02d", CLAVE_INVALIDAD_TEFF);
    setFieldTEFF(FIELD_TEFF_CODIGO_RESPUESTA, codigoRespuesta, LEN_TEFF_CODIGO_RESPUESTA);
    setFieldTEFF(FIELD_TEFF_CODIGO_AUTORIZACION, auxiliar, LEN_TEFF_CODIGO_AUTORIZACION);
    setFieldTEFF(FIELD_TEFF_ASTERISCOS_4_ULTIMOS_TARJETA, auxiliar, LEN_TEFF_ASTERISCOS_4_ULTIMOS_TARJETA);
    setFieldTEFF(FIELD_TEFF_TIPO_CUENTA, auxiliar, LEN_TEFF_TIPO_CUENTA);
    setFieldTEFF(FIELD_TEFF_FRANQUICIA, auxiliar, LEN_TEFF_FRANQUICIA);
    setFieldTEFF(FIELD_TEFF_MONTO, auxiliar, LEN_TEFF_MONTO);
    setFieldTEFF(FIELD_TEFF_IVA, auxiliar, LEN_TEFF_IVA);
    setFieldTEFF(FIELD_TEFF_BASE_DEVOLUCION, auxiliar, LEN_TEFF_BASE_DEVOLUCION);
    setFieldTEFF(FIELD_TEFF_NUMERO_RECIBO_DATAFONO, auxiliar, LEN_TEFF_NUMERO_RECIBO_DATAFONO);
    setFieldTEFF(FIELD_TEFF_CUOTAS, auxiliar, LEN_TEFF_CUOTAS);
    setFieldTEFF(FIELD_TEFF_RRN, auxiliar, LEN_TEFF_RRN);
    getParameter(NUMERO_TERMINAL, numeroTerminal);
    setFieldTEFF(FIELD_TEFF_NUMERO_TERMINAL, numeroTerminal, LEN_TEFF_NUMERO_TERMINAL);
    setFieldTEFF(FIELD_TEFF_CODIGO_ESTABLECIMIENTO, tablaUno.receiptLine3, LEN_TEFF_CODIGO_ESTABLECIMIENTO);
    setFieldTEFF(FIELD_TEFF_FECHA, auxiliar, LEN_TEFF_FECHA);
    setFieldTEFF(FIELD_TEFF_HORA, auxiliar, LEN_TEFF_HORA);

    packMessage = packMessageTEFF();

    return packMessage;
}

int tiposDeCuentaCaja(char* cuenta, int validacionEspecial) {

    char tipoCuenta[20 + 1];

    memset(tipoCuenta, 0x00, sizeof(tipoCuenta));

    switch (atoi(cuenta)) {
        case 0:
            sprintf(tipoCuenta, "%s", "00");//paypass
            break;
        case 10:
            sprintf(tipoCuenta, "%s", "AH");

            break;
        case 20:
            sprintf(tipoCuenta, "%s", "CC");

            break;
        case 30:
            sprintf(tipoCuenta, "%s", "CR");

            break;
        case 40:
            if (validacionEspecial == 8) {
                sprintf(tipoCuenta, "%s", "SP");

            } else {
                sprintf(tipoCuenta, "%s", "Ah");

            }

            break;
        case 41:
            sprintf(tipoCuenta, "%s", "LT");

            break;
        case 42:
            sprintf(tipoCuenta, "%s", "CM");

            break;
        case 43:
            sprintf(tipoCuenta, "%s", "CU");

            break;
        case 44:
            sprintf(tipoCuenta, "%s", "BE");
            break;

        case 45:
            sprintf(tipoCuenta, "%s", "RT");
            break;
        case 46:
            sprintf(tipoCuenta, "%s", "RG");
            break;
        case 47:
            sprintf(tipoCuenta, "%s", "DS");
            break;

        case 48:
            sprintf(tipoCuenta, "%s", "DB");
            break;

        case 49:
            sprintf(tipoCuenta, "%s", "BC");
            break;

        default:
            sprintf(tipoCuenta, "%s", "CR");
            break;
    }

    memcpy(cuenta, tipoCuenta, 2);
    return 1;

}

int ajustarDatosTransaccion(char *buffer, int len, int inOut) {

    char auxiliar[130 + 1];
    char auxiliarBuffer[130 + 1];
    char auxiliarMontos[15 + 1];
    int longitud = 0;
    int respuesta = -1;

    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(auxiliarBuffer, 0x00, sizeof(auxiliarBuffer));
    memset(auxiliarMontos, 0x00, sizeof(auxiliarMontos));

    longitud = strlen(buffer);

    if (longitud > 1) {

        if (inOut == DATOS_ENTRADA_MONTOS) {

            if (longitud > 2) {
                memcpy(auxiliar, buffer, strlen(buffer) - 2);
                sprintf(auxiliarMontos, "%012ld", atol(auxiliar));
                memcpy(buffer, auxiliarMontos, len);
            } else {
                memset(auxiliar, 0x30, sizeof(auxiliar));
                memcpy(buffer, auxiliar, len);
            }

        } else if (inOut == DATOS_ENTRADA) {
            memcpy(auxiliar, buffer, strlen(buffer));
            leftPad(auxiliarBuffer, auxiliar, 0x30, len);
            memcpy(buffer, auxiliarBuffer, len);
        } else if (inOut == DATOS_SALIDA) {
            leftTrim(auxiliarBuffer, buffer, 0x30);
            memset(buffer, 0x00, len);
            if (strlen(auxiliarBuffer) > 0) {
                memcpy(buffer, auxiliarBuffer, strlen(auxiliarBuffer));
            } else {
                memcpy(buffer, "0", 1);
            }

        }
        respuesta = 1;
    }
    return respuesta;
}

void validacionCodigoRespuestaCaja(char *codigoRespuesta) {
    char auxiliarCodigoRespuesta[2 + 1];

    memset(auxiliarCodigoRespuesta, 0x00, sizeof(auxiliarCodigoRespuesta));

    switch (atoi(codigoRespuesta)) {
        case 0:
            sprintf(auxiliarCodigoRespuesta, "%s", "00");
            break;
        case 75:
            sprintf(auxiliarCodigoRespuesta, "%s", "02");
            break;
        case 88:
            sprintf(auxiliarCodigoRespuesta, "%s", "88");
            break;
        case 89:
            sprintf(auxiliarCodigoRespuesta, "%s", "89");
            break;
        default:
            sprintf(auxiliarCodigoRespuesta, "%s", "01");
            break;
    }
    memcpy(codigoRespuesta, auxiliarCodigoRespuesta, LEN_TEFF_CODIGO_RESPUESTA);
}

void ajusteFecha(char *fecha) {

    //Telium_Date_t date;

    char auxiliarFecha[6 + 1];
    char fechaYear[6 + 1] = {0x00};
    //memset(&date, 0x00, sizeof(date));
    memset(auxiliarFecha, 0x00, sizeof(auxiliarFecha));
    //Telium_Read_date(&date);
    obtenerFecha(fechaYear);
    memcpy(auxiliarFecha,fechaYear, 2);
    memcpy(auxiliarFecha + 2, fecha, 4);
    memcpy(fecha, auxiliarFecha, LEN_TEFF_FECHA);

}


int resincronizacionCaja(ResultISOPack packMessage) {
    int respuesta = 1;
    char bufferOut[512 + 1];

    memset(bufferOut, 0x00, sizeof(bufferOut));
    respuesta = realizarTransaccionMedioComunicacion(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 30 * 100);

    if (respuesta < 0) {
        setParameter(CAJA_NO_RESPONDE, "1");
        //screenMessage("COMUNICACION", "CAJA", "SIN RESPUESTA", "", 2 * 1000);

    }

    if (respuesta > 0) {
        respuesta = ((bufferOut[0] == ACK) ? 1 : -1);

        if (respuesta < 0) {
            //screenMessage("COMUNICACION", "CAJA", "NACK RECIBIDO", "", 2 * 1000);
        }
    }

    return respuesta;
}

int confirmacionLRC(char* inBuffer, int longitud) {

    int respuesta = -1;
    int size = 0;
    char bufferAuxiliar[2024 + 1];
    char longitudBCD[2 + 1];
    char longitudaAuxiliar[2 + 1];
    int longitudMensaje = 0;
    uChar respuestaLRC;
    uChar resultadoLRC;
    int mostrarMensaje = TRUE;

    memset(bufferAuxiliar, 0x00, sizeof(bufferAuxiliar));
    memset(longitudBCD, 0x00, sizeof(longitudBCD));
    memset(longitudaAuxiliar, 0x00, sizeof(longitudaAuxiliar));

    if ((inBuffer[0] == ACK) && (inBuffer[1] == STX)) {

        size += 2;
        memcpy(longitudBCD, inBuffer + size, 2);
        size += 2;
        _convertBCDToASCII_(longitudaAuxiliar, longitudBCD, 4);
        longitudMensaje = atoi(longitudaAuxiliar);
        respuestaLRC = inBuffer[longitudMensaje + size + 1];
        size++;
        memcpy(bufferAuxiliar, inBuffer + 2, longitudMensaje + 3);
        resultadoLRC = calcularLRC(bufferAuxiliar, longitudMensaje + 3);
        respuesta = ((respuestaLRC == resultadoLRC) ? 1 : -1);

        if (respuesta < 0) {
            mostrarMensaje = FALSE;
            //screenMessage("COMUNICACION", "MENSAJE", "DE CAJA", "INVALIDO (LRC)", 2 * 1000);
        }

    }
    if (respuesta < 0 && mostrarMensaje == TRUE) {

        setParameter(CAJA_NO_RESPONDE, "1");
        //screenMessage("COMUNICACION", "CAJA", "SIN RESPUESTA", "", 2 * 1000);
    }

    return respuesta;
}

int validarClaveSupervisor(char *claveSupervisor, int tipoClave) {

    char claveActual[4 + 1];
    int respuesta = 0;

    memset(claveActual, 0x00, sizeof(claveActual));

    if (tipoClave == TRANSACCION_TEFF_RECARGA_BONO) {
        getParameter(VALOR_CLAVE_BONOS, claveActual);
    } else {
        getParameter(VALOR_CLAVE_SUPERVISOR, claveActual);
    }

    respuesta = ((strcmp(claveSupervisor, claveActual) != 0) ? -1 : 1);
    if (respuesta < 0) {
        //screenMessage("MENSAJE", "", "CLAVE ERRADA", "CAJA", 2 * 1000);
    }

    return respuesta;
}

int cargarDatosCajaAnulada(DatosVenta* datosVentaCaja) {

    int resultado = 0;
    char dataCaja[TAM_JOURNAL + 1];
    DatosVenta datosVenta;

    memset(dataCaja, 0x00, sizeof(dataCaja));
    memset(&datosVenta, 0x00, sizeof(datosVenta));

    resultado = verificarArchivo(discoNetcom, RESPALDO_CAJA);

    if (resultado > 0) {

        resultado = leerArchivo(discoNetcom, RESPALDO_CAJA, dataCaja);
    }

    if (resultado > 0) {

        datosVenta = obtenerDatosVenta(dataCaja);

        memset(datosVentaCaja->codigoCajero, 0x00, sizeof(datosVentaCaja->codigoCajero));
        memcpy(datosVentaCaja->codigoCajero, datosVenta.codigoCajero, sizeof(datosVentaCaja->codigoCajero));

        memset(datosVentaCaja->totalVenta, 0x00, sizeof(datosVentaCaja->totalVenta));
        memcpy(datosVentaCaja->totalVenta, datosVenta.totalVenta, sizeof(datosVentaCaja->totalVenta));
        strcat(datosVentaCaja->totalVenta, "00");

        memset(datosVentaCaja->iva, 0x00, sizeof(datosVentaCaja->iva));
        memcpy(datosVentaCaja->iva, datosVenta.iva, sizeof(datosVentaCaja->iva));
        strcat(datosVentaCaja->iva, "00");

        memset(datosVentaCaja->inc, 0x00, sizeof(datosVentaCaja->inc));
        memcpy(datosVentaCaja->inc, datosVenta.inc, sizeof(datosVentaCaja->inc));
        strcat(datosVentaCaja->inc, "00");

        memset(datosVentaCaja->facturaCaja, 0x00, sizeof(datosVentaCaja->facturaCaja));
        memcpy(datosVentaCaja->facturaCaja, datosVenta.facturaCaja, sizeof(datosVentaCaja->facturaCaja));
    }

    return resultado;
}

void guardarTransaccionCajaAnulacion(DatosVenta tramaVenta) {

    int tamCaja = 0;
    char cajaVenta[TAM_JOURNAL + 1];

    tamCaja = sizeof(tramaVenta);
    memcpy(cajaVenta, &tramaVenta, sizeof(tramaVenta));
    escribirArchivo(discoNetcom, RESPALDO_CAJA, cajaVenta, tamCaja);

}

void guardarTransaccionCajasSincronizacion(char *buffer, int longitud) {

    escribirArchivo(discoNetcom, ARCHIVO_SINCRONIZACION_CAJA, buffer, longitud);

}

void sincronizacionSerial(void) {
    int longitud = 0;
    int idx = 0;
    char dataEnviado[512 + 1];
    int sincronizar = 0;
    int respondeCaja = 0;
    char bufferOut[512 + 1];
    DatosVenta datos;
    ResultISOUnpack resultadoUnpack;
    TEFFFieldMessage tipoCampo;
    ResultISOPack packMessage;

    memset(&packMessage, 0x00, sizeof(packMessage));
    memset(&tipoCampo, 0x00, sizeof(tipoCampo));
    memset(dataEnviado, 0x00, sizeof(dataEnviado));
    memset(&resultadoUnpack, 0x00, sizeof(resultadoUnpack));
    memset(&bufferOut, 0x00, sizeof(bufferOut));
    memset(&datos, 0x00, sizeof(datos));

    setParameter(USO_CAJA_REGISTRADORA, "1");
    sincronizar = verificarArchivo(discoNetcom, ARCHIVO_SINCRONIZACION_CAJA);

    if (sincronizar == FS_OK) {

        longitud = leerArchivo(discoNetcom, ARCHIVO_SINCRONIZACION_CAJA, dataEnviado);
        bufferOut[0] = ACK;
        memcpy(bufferOut + 1, dataEnviado, longitud);

        if (longitud > 0) {

            resultadoUnpack = unpackMessageTEFF(bufferOut, longitud + 1);

            for (idx = 0; idx < resultadoUnpack.totalField; idx++) {
                memset(&tipoCampo, 0x00, sizeof(tipoCampo));

                tipoCampo = getFiledTEFF(resultadoUnpack.listField[idx]);
                setFieldTEFF(tipoCampo.field, tipoCampo.valueField, tipoCampo.totalBytes);

            }

        }

        setTipoMensajeTEFF(MENSAJE_TEFF_REQUERIMIENTO);
        setCodigoTransaccionTEFF(TRANSACCION_TEFF_SINCRONIZACION);
        setCodigoRespuestaTEFF(NO_HAY_CODIGO_DE_RESPUESTA);

        packMessage = packMessageTEFF();

        memset(bufferOut, 0x00, sizeof(bufferOut));
        //respondeCaja = realizarTransaccionCaja(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 200, 0);
        respondeCaja = realizarTransaccionMedioComunicacion(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 30 * 100);

        if (respondeCaja == 0) {

            setParameter(CAJA_NO_RESPONDE, "1");
            //screenMessage("COMUNICACION", "CAJA", "SIN RESPUESTA", "", 2 * 1000);

        } else {

            borrarArchivo(DISCO_NETCOM, ARCHIVO_SINCRONIZACION_CAJA);

        }
    } else {

        //screenMessage("COMUNICACION", "NADA", "PARA SINCRONIZAR", "", 2 * 1000);
        memcpy(datos.codigoRespuesta, "01", 2);

        transaccionRespuestaDatosCaja(datos, TRANSACCION_TEFF_SINCRONIZACION);

    }

}

int validarDescuentosCaja(char *token, char *tipo) {

    char descuento[12 + 1];
    char descuentoAuxiliar[12 + 1];
    int descuentoActivo = 0;

    ResultTokenPack resultTokenPack;

    memset(descuento, 0x00, sizeof(descuento));
    memset(&resultTokenPack, 0x00, sizeof(resultTokenPack));
    memset(descuentoAuxiliar, 0x00, sizeof(descuentoAuxiliar));

    descuentoActivo = cargarCamposEstructuraDatosVenta(descuento, FIELD_TEFF_DESCUENTO_TRANSACCION);

    if (descuentoActivo == TRUE) {

        leftPad(descuentoAuxiliar, descuento, 0x20, 12);

        _setTokenMessage_("QS", descuentoAuxiliar, 12);
        resultTokenPack = _packTokenMessage_();

        ////////empaqueta el token QS/////////
        memcpy(token, resultTokenPack.tokenPackMessage, resultTokenPack.totalBytes);

    } else {

        memcpy(token, "0", strlen("0"));
        memcpy(tipo, "0", 2);
    }

    return descuentoActivo;
}

int labelEspecialesDescuento(char *track) {
    int respuesta = FALSE;

    TablaTresInicializacion tablaTres;
    uChar bin[9 + 1];

    memset(bin, 0x00, sizeof(bin));
    memcpy(bin, track, 9);

    memset(&tablaTres, 0x00, sizeof(tablaTres));

    tablaTres = _traerIssuer_(atol(bin));
    respuesta = isIssuerEspecial(tablaTres.cardName);

    return respuesta;
}

/**
 * @brief translada datos del datafono a la caja para la tarjeta privada
 * @param datosVenta contiene información de la venta
 * @param transaccion, indica la transaccion "tarjetaPrivada"
 * @return iRet entero.
 * @author
 */
int transaccionRespuestaDatosPrivada(DatosVenta datos, int transaccion) {
    int respuesta = 0;
    char pan[LEN_TEFF_PAN + 1];
    char bufferOut[512 + 1];
    char tipoCuenta[2 + 1];
    TablaUnoInicializacion tablaUno;
    ResultISOPack packMessage;

    memset(pan, 0x00, sizeof(pan));
    memset(&tablaUno, 0x00, sizeof(tablaUno));
    memset(&packMessage, 0x00, sizeof(packMessage));
    memset(bufferOut, 0x00, sizeof(bufferOut));
    memset(tipoCuenta, 0x00, sizeof(tipoCuenta));

    tablaUno = _desempaquetarTablaCeroUno_();
    setTipoMensajeTEFF(MENSAJE_TEFF_RESPUESTA);
    setCodigoTransaccionTEFF(transaccion);

    validacionCodigoRespuestaCaja(datos.codigoRespuesta);

    setCodigoRespuestaTEFF(atoi(datos.codigoRespuesta));
    setFieldTEFF(FIELD_TEFF_CODIGO_RESPUESTA, datos.codigoRespuesta, LEN_TEFF_CODIGO_RESPUESTA);
    ajustarDatosTransaccion(datos.codigoAprobacion, LEN_TEFF_CODIGO_AUTORIZACION, DATOS_ENTRADA);
    setFieldTEFF(FIELD_TEFF_CODIGO_AUTORIZACION, datos.codigoAprobacion, LEN_TEFF_CODIGO_AUTORIZACION);
    extraerPanAsteriscos(datos.track2, strlen(datos.track2), pan, ASTERISCOS_4_ULTIMOS);
    setFieldTEFF(FIELD_TEFF_ASTERISCOS_4_ULTIMOS_TARJETA, pan, LEN_TEFF_ASTERISCOS_4_ULTIMOS_TARJETA);

    validarPosEntryMode(2, datos.posEntrymode);

    if (verificarCodigoCajero() > 0) {
        setFieldTEFF(FIELD_TEFF_CODIGO_CAJERO, datos.codigoCajero, LEN_TEFF_CODIGO_CAJERO);
    }

    memcpy(tipoCuenta, datos.tipoCuenta, LEN_TEFF_TIPO_CUENTA);
    tiposDeCuentaCaja(tipoCuenta, atoi(datos.creditoRotativo));
    setFieldTEFF(FIELD_TEFF_TIPO_CUENTA, tipoCuenta, LEN_TEFF_TIPO_CUENTA);
    setFieldTEFF(FIELD_TEFF_FRANQUICIA, datos.cardName, LEN_TEFF_FRANQUICIA);
    ajustarDatosTransaccion(datos.totalVenta, LEN_TEFF_MONTO, DATOS_ENTRADA_MONTOS);
    setFieldTEFF(FIELD_TEFF_MONTO, datos.totalVenta, LEN_TEFF_MONTO);
    ajustarDatosTransaccion(datos.iva, LEN_TEFF_MONTO, DATOS_ENTRADA_MONTOS);
    setFieldTEFF(FIELD_TEFF_IVA, datos.iva, LEN_TEFF_IVA);
    ajustarDatosTransaccion(datos.baseDevolucion, LEN_TEFF_MONTO, DATOS_ENTRADA_MONTOS);
    setFieldTEFF(FIELD_TEFF_BASE_DEVOLUCION, datos.baseDevolucion, LEN_TEFF_BASE_DEVOLUCION);

    ajustarDatosTransaccion(datos.numeroRecibo, LEN_TEFF_NUMERO_RECIBO_DATAFONO, DATOS_ENTRADA);
    setFieldTEFF(FIELD_TEFF_NUMERO_RECIBO_DATAFONO, datos.numeroRecibo, LEN_TEFF_NUMERO_RECIBO_DATAFONO);
    ajustarDatosTransaccion(datos.cuotas, LEN_TEFF_CUOTAS, DATOS_ENTRADA);
    setFieldTEFF(FIELD_TEFF_CUOTAS, datos.cuotas, LEN_TEFF_CUOTAS);
    ajustarDatosTransaccion(datos.rrn, LEN_TEFF_RRN, DATOS_ENTRADA);
    setFieldTEFF(FIELD_TEFF_RRN, datos.rrn, LEN_TEFF_RRN);
    ajustarDatosTransaccion(datos.terminalId, LEN_TEFF_NUMERO_TERMINAL, DATOS_ENTRADA);
    setFieldTEFF(FIELD_TEFF_NUMERO_TERMINAL, datos.terminalId, LEN_TEFF_NUMERO_TERMINAL);
    setFieldTEFF(FIELD_TEFF_CODIGO_ESTABLECIMIENTO, tablaUno.receiptLine3, LEN_TEFF_CODIGO_ESTABLECIMIENTO);

    if (strlen(datos.txnDate) > 0) {
        ajusteFecha(datos.txnDate);
    } else {
        memset(datos.txnTime, 0x00, sizeof(datos.txnTime));
        memset(datos.txnDate, 0x00, sizeof(datos.txnDate));
        horaFechaTerminal(datos.txnTime, datos.txnDate);
    }

    setFieldTEFF(FIELD_TEFF_FECHA, datos.txnDate, LEN_TEFF_FECHA);
    setFieldTEFF(FIELD_TEFF_HORA, datos.txnTime, LEN_TEFF_HORA);

    packMessage = packMessageTEFF();
    borrarArchivo(DISCO_NETCOM, ARCHIVO_SINCRONIZACION_CAJA);

    //respuesta = realizarTransaccionCaja(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 200, 10 * 100);
    respuesta = realizarTransaccionMedioComunicacion(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 30 * 100);
    if (respuesta <= 0) {
        setParameter(CAJA_NO_RESPONDE, "1");
        //screenMessage("COMUNICACION", "CAJA", "SIN RESPUESTA", "", 2 * 1000);
    }

    respuesta = ((bufferOut[0] == ACK) ? 1 : -1);

    if (respuesta <= 0 && transaccion != TRANSACCION_TEFF_SINCRONIZACION) {
        guardarTransaccionCajasSincronizacion(packMessage.isoPackMessage, packMessage.totalBytes);
    }

    return respuesta;
}

/** @brief función encargada de verificar la existencia de montos inconsistentes
 * 	@param datos, estructura datosVenta con la información necesaria para la verificacion
 * 	@return
 * 	-1 para montos inconsistentes
 * 	1 para montos normales.
 * 	@ autor
 */
int verificarMontosInconsistentes(DatosVenta * datos) {

    long compraNeta = 0;
    long compraTotal = 0;
    long sumaImpuestos = 0;
    char auxiliarMonto[12 + 1];
    char auxiliar[12 + 1];
    int respuesta = 1;
    char propinaMaxPermitido[2 + 1];
    TablaUnoInicializacion tablaUno;

    DatosOperacion datosOperacion;
    int montoPropina = 0;
    int porcentajePropina = 0;

    memset(auxiliarMonto, 0x00, sizeof(auxiliarMonto));
    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(&datosOperacion, 0x00, sizeof(datosOperacion));
    memset(propinaMaxPermitido, 0x00, sizeof(propinaMaxPermitido));
    memset(&tablaUno, 0x00, sizeof(tablaUno));

    tablaUno = _desempaquetarTablaCeroUno_();

    sprintf(propinaMaxPermitido, "%02x", tablaUno.printerBaudRate);

    porcentajePropina = atoi(propinaMaxPermitido);

    sumaImpuestos = /*(atol(datos->propina) +*/atol(datos->iva) + atol(datos->inc); //ojo incidencia 5152 debate
    compraNeta = (atol(datos->totalVenta) - sumaImpuestos);
    compraTotal = (compraNeta + sumaImpuestos + atol(datos->propina));

    if (atol(datos->totalVenta) < 0 || atol(datos->iva) < 0 || atol(datos->inc) < 0 || atol(datos->propina) < 0) {
        respuesta = -1;

        //screenMessage("COMUNICACION", "MONTOS", "INCONSISTENTES", "", 2 * 1000);

        if (atol(datos->totalVenta) < 0) {
            sprintf(datos->totalVenta, "%ld", atol(datos->totalVenta) * 1000);
        } else if (atol(datos->totalVenta) > 0) {
            strcat(datos->totalVenta, "00");
        }

        if (atol(datos->iva) < 0) {
            sprintf(datos->iva, "%ld", atol(datos->iva) * 1000);
        } else if (atol(datos->iva) > 0) {
            strcat(datos->iva, "00");
        }

        if (atol(datos->inc) < 0) {
            sprintf(datos->inc, "%ld", atol(datos->inc) * 1000);
        } else if (atol(datos->inc) > 0) {
            strcat(datos->inc, "00");
        }

        if (atol(datos->propina) < 0) {
            sprintf(datos->propina, "%ld", atol(datos->propina) * 1000);
        } else if (atol(datos->propina) > 0) {
            strcat(datos->propina, "00");
        }

    } else if (compraNeta <= 0) {
        respuesta = -1;
        //screenMessage("COMUNICACION", "MONTOS", "INCONSISTENTES", "", 2 * 1000);

        strcat(datos->totalVenta, "00");
        strcat(datos->iva, "00");
        strcat(datos->baseDevolucion, "00");
        strcat(datos->propina, "00");
        strcat(datos->inc, "00");
    }

    sprintf(auxiliarMonto, "%ld", compraNeta);
    memcpy(datos->compraNeta, auxiliarMonto, 12);

    memset(auxiliarMonto, 0x00, sizeof(auxiliarMonto));
    sprintf(auxiliarMonto, "%ld", compraTotal);
    memcpy(datos->totalVenta, auxiliarMonto, 12);
    datosOperacion = dividir((atol(datos->totalVenta) - atol(datos->propina))* porcentajePropina, 100, 0, 1);
    montoPropina = atoi(datosOperacion.cociente);

    if (atoi(datos->propina) > montoPropina && tablaUno.options1.tipProcessing > 0) {
        respuesta = -1;
        //screenMessage("COMUNICACION", "PROPINA", "INVALIDA", "", 2 * 1000);
    }
    return respuesta;
}

int validarClaveSupervisorHabilitacionBono(char *claveSupervisor) {

    char claveActual[4 + 1];
    int respuesta = 0;

    memset(claveActual, 0x00, sizeof(claveActual));

    getParameter(VALOR_CLAVE_SUPERVISOR, claveActual);

    respuesta = ((strcmp(claveSupervisor, claveActual) != 0) ? -1 : 1);
    if (respuesta < 0) {
        //screenMessage("MENSAJE", "", "ERROR CLAVE", "SUPERVISOR", 2 * 1000);
    }

    return respuesta;
}

int definirLongitudCampo(int campo) {

    int tam = 0;

    switch (campo) {
        case FIELD_TEFF_REFERENCIA_ADICIONAL:
            tam = 7;
            break;
        case FIELD_TEFF_MONTO:
            tam = LEN_TEFF_MONTO;
            break;
        case FIELD_TEFF_IVA:
            tam = LEN_TEFF_IVA;
            break;
        case FIELD_TEFF_BASE_DEVOLUCION:
            tam = LEN_TEFF_BASE_DEVOLUCION;
            break;
        case FIELD_TEFF_NUMERO_FACTURA_ENVIADO_CAJA:
            tam = LEN_TEFF_NUMERO_FACTURA_ENVIADO_CAJA;
            break;
        case FIELD_TEFF_INC:
            break;
        default:
            tam = 0;
            break;
    }

    return tam;
}
/**
 * @brief	realiza una petición de datos a la caja para la transacción de Lealtad.
 * @param	datosVenta
 * @return
 */
int transaccionPeticionDatosLealtad(DatosVenta *datosVenta, int tipoTransaccion) {

    int retorno = 0;
    int retornoAux = 0;
    int claveValida = FALSE;
    uChar auxiliar[30 + 1];
    uChar bufferOut[512 + 1];
    uChar descuento[4 + 1];
    uChar claveSupervisor[SIZE_CLAVE_SUPERVIDOR + 1];
    ResultISOPack resultIsoPack;

    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(bufferOut, 0x00, sizeof(bufferOut));
    memset(descuento, 0x00, sizeof(descuento));
    memset(claveSupervisor, 0x00, sizeof(claveSupervisor));
    memset(&resultIsoPack, 0x00, sizeof(resultIsoPack));

    setTipoMensajeTEFF(MENSAJE_TEFF_REQUERIMIENTO);
    setCodigoTransaccionTEFF(tipoTransaccion);
    setCodigoRespuestaTEFF(NO_HAY_CODIGO_DE_RESPUESTA);

    validarPosEntryMode(2, datosVenta->posEntrymode);

    if (verificarCodigoCajero() > 0) {
        setFieldTEFF(FIELD_TEFF_CODIGO_CAJERO, auxiliar, 0);
    }

    setFieldTEFF(FIELD_TEFF_MONTO, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_IVA, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_INC, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_NUMERO_DOCUMENTO, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_CLAVE_SUPERVISOR, auxiliar, 0);

    resultIsoPack = packMessageTEFF();

    //retorno = realizarTransaccionCaja(resultIsoPack.isoPackMessage, resultIsoPack.totalBytes, bufferOut, 200, 30 * 100);
    retorno = realizarTransaccionMedioComunicacion(resultIsoPack.isoPackMessage, resultIsoPack.totalBytes, bufferOut, 30 * 100);
    if (retorno == 0) {
        setParameter(CAJA_NO_RESPONDE, "1");
        //screenMessage("COMUNICACION", "CAJA", "SIN RESPUESTA", "", 2 * 1000);
    } else {
        memset(auxiliar, 0x00, sizeof(auxiliar));
        auxiliar[0] = ACK;
        //realizarTransaccionCaja(auxiliar, 1, bufferOut, 200, 10);
        realizarTransaccionMedioComunicacion(auxiliar, 1, bufferOut,10);
    }

    if (retorno > 1) {
        retorno = unpackRespuestaCaja(bufferOut, retorno, TRANSACCION_TEFF_LEALTAD);
    } else {
        retorno = 0;
    }

    if (retorno > 0) {
        retorno = validarCodigoCajero(datosVenta->codigoCajero);
    }

    if (retorno > 0) {
        cargarCamposEstructuraDatosVenta(claveSupervisor, FIELD_TEFF_CLAVE_SUPERVISOR);
        retorno = validarClaveSupervisor(claveSupervisor, TRANSACCION_TEFF_LEALTAD);
        claveValida = retorno;
    }

    if (retorno > 0) {
        cargarCamposLealtad(datosVenta);
        retorno = verificarMontosInconsistentes(datosVenta);


        //REEMPLAZAR POR GETPARAMETER
        /*if (verificarHabilitacionObjeto(TRANSACCION_VIVAMOS) == TRUE && retorno > 0) {
            retornoAux = cargarCamposEstructuraDatosVenta(datosVenta->tokenVivamos, FIELD_TEFF_DESCUENTO_TRANSACCION);
            if (retornoAux == TRUE) {
                ajustarDatosTransaccion(datosVenta->tokenVivamos, strlen(datosVenta->tokenVivamos), DATOS_SALIDA);
            } else {
                memcpy(datosVenta->tokenVivamos, "0", strlen("0"));
            }
        }*/

        if (validarDescuentosCaja(datosVenta->tokenVivamos, descuento) == TRUE && retorno > 0) {
            memset(datosVenta->estadoTarifa, 0x00, sizeof(datosVenta->estadoTarifa));
            memcpy(datosVenta->estadoTarifa, "DT", 2);
        }
    }

    if (retorno <= 0) {
        setParameter(USO_CAJA_REGISTRADORA, "1");
    }

    if (claveValida != TRUE) {
        memcpy(datosVenta->codigoRespuesta, "01", TAM_CODIGO_RESPUESTA);
        transaccionRespuestaDatosLealtad(*datosVenta, TRUE);
    }

    return retorno;
}

/**
 * @brief toma los campo de la trama de la caja y los asigna a la estructura DatosVenta
 * @param datosVenta
 */
void cargarCamposLealtad(DatosVenta *datosVenta) {

    char auxiliar[30 + 1];
    char auxiliar2[30 + 1];
    char isPuntosColombia[2 + 1];

    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(auxiliar2, 0x00, sizeof(auxiliar2));
    memset(isPuntosColombia, 0x00, sizeof(isPuntosColombia));

    getParameter(PRODUCTO_PUNTOS_COLOMBIA, isPuntosColombia);

    cargarCamposEstructuraDatosVenta(datosVenta->totalVenta, FIELD_TEFF_MONTO);
    ajustarDatosTransaccion(datosVenta->totalVenta, strlen(datosVenta->totalVenta), DATOS_SALIDA);

    cargarCamposEstructuraDatosVenta(datosVenta->iva, FIELD_TEFF_IVA);
    ajustarDatosTransaccion(datosVenta->iva, strlen(datosVenta->iva), DATOS_SALIDA);

    cargarCamposEstructuraDatosVenta(datosVenta->inc, FIELD_TEFF_INC);
    ajustarDatosTransaccion(datosVenta->inc, strlen(datosVenta->inc), DATOS_SALIDA);

    if (atoi(isPuntosColombia) == 1) {
        memset(auxiliar, 0x00, sizeof(auxiliar));

        cargarCamposEstructuraDatosVenta(auxiliar, FIELD_TEFF_TIPO_DOCUMENTO);
        sprintf(datosVenta->aux1, "%s", auxiliar);

        memset(auxiliar, 0x00, sizeof(auxiliar));
        cargarCamposEstructuraDatosVenta(auxiliar, FIELD_TEFF_NUMERO_DOCUMENTO);
        _rightTrim_(auxiliar2, auxiliar, 0x20);
        leftPad(datosVenta->aux1 + 2, auxiliar2, 0x30, 10);
    } else {
        cargarCamposEstructuraDatosVenta(datosVenta->field57, FIELD_TEFF_NUMERO_DOCUMENTO);
        ajustarDatosTransaccion(datosVenta->field57, strlen(datosVenta->field57), DATOS_SALIDA);
    }

}

int transaccionRespuestaDatosLealtad(DatosVenta datos, int validacionClave) {

    int respuesta = 0;
    char pan[LEN_TEFF_PAN + 1];
    char bufferOut[512 + 1];
    char tipoCuenta[2 + 1];
    char auxiliar[30 + 1];
    TablaUnoInicializacion tablaUno;
    ResultISOPack packMessage;

    ResultISOPack packMessageRespaldo;

    memset(pan, 0x00, sizeof(pan));
    memset(&tablaUno, 0x00, sizeof(tablaUno));
    memset(&packMessage, 0x00, sizeof(packMessage));
    memset(&packMessageRespaldo, 0x00, sizeof(packMessageRespaldo));
    memset(bufferOut, 0x00, sizeof(bufferOut));
    memset(tipoCuenta, 0x00, sizeof(tipoCuenta));
    memset(auxiliar, 0x00, sizeof(auxiliar));

    tablaUno = _desempaquetarTablaCeroUno_();

    setTipoMensajeTEFF(MENSAJE_TEFF_RESPUESTA);

    setCodigoTransaccionTEFF(TRANSACCION_TEFF_LEALTAD);
    validacionCodigoRespuestaCaja(datos.codigoRespuesta);
    setCodigoRespuestaTEFF(atoi(datos.codigoRespuesta));

    setFieldTEFF(FIELD_TEFF_CODIGO_RESPUESTA, datos.codigoRespuesta, LEN_TEFF_CODIGO_RESPUESTA);
    ajustarDatosTransaccion(datos.codigoAprobacion, LEN_TEFF_CODIGO_AUTORIZACION, DATOS_ENTRADA);
    setFieldTEFF(FIELD_TEFF_CODIGO_AUTORIZACION, datos.codigoAprobacion, LEN_TEFF_CODIGO_AUTORIZACION);

    validarPosEntryMode(2, datos.posEntrymode);

    extraerPanAsteriscos(datos.track2, strlen(datos.track2), pan, ASTERISCOS_4_ULTIMOS);

    setFieldTEFF(FIELD_TEFF_ASTERISCOS_4_ULTIMOS_TARJETA, pan, LEN_TEFF_ASTERISCOS_4_ULTIMOS_TARJETA);

    if (verificarCodigoCajero() > 0) {
        setFieldTEFF(FIELD_TEFF_CODIGO_CAJERO, datos.codigoCajero, LEN_TEFF_CODIGO_CAJERO);
    }

    memcpy(tipoCuenta, datos.tipoCuenta, LEN_TEFF_TIPO_CUENTA);
    tiposDeCuentaCaja(tipoCuenta, atoi(datos.creditoRotativo));
    setFieldTEFF(FIELD_TEFF_TIPO_CUENTA, tipoCuenta, LEN_TEFF_TIPO_CUENTA);

    memset(auxiliar, 0x00, sizeof(auxiliar));
    _rightPad_(auxiliar, datos.cardName, 0x20, LEN_TEFF_FRANQUICIA);
    setFieldTEFF(FIELD_TEFF_FRANQUICIA, auxiliar, LEN_TEFF_FRANQUICIA);

    ajustarDatosTransaccion(datos.totalVenta, LEN_TEFF_MONTO,
                            DATOS_ENTRADA_MONTOS);
    setFieldTEFF(FIELD_TEFF_MONTO, datos.totalVenta, LEN_TEFF_MONTO);

    ajustarDatosTransaccion(datos.iva, LEN_TEFF_MONTO, DATOS_ENTRADA_MONTOS);
    setFieldTEFF(FIELD_TEFF_IVA, datos.iva, LEN_TEFF_IVA);

    ajustarDatosTransaccion(datos.numeroRecibo, LEN_TEFF_NUMERO_RECIBO_DATAFONO, DATOS_ENTRADA);
    setFieldTEFF(FIELD_TEFF_NUMERO_RECIBO_DATAFONO, datos.numeroRecibo, LEN_TEFF_NUMERO_RECIBO_DATAFONO);
    ajustarDatosTransaccion(datos.cuotas, LEN_TEFF_CUOTAS, DATOS_ENTRADA);
    setFieldTEFF(FIELD_TEFF_CUOTAS, datos.cuotas, LEN_TEFF_CUOTAS);
    ajustarDatosTransaccion(datos.rrn, LEN_TEFF_RRN, DATOS_ENTRADA);

    setFieldTEFF(FIELD_TEFF_RRN, datos.rrn, LEN_TEFF_RRN);
    ajustarDatosTransaccion(datos.terminalId, LEN_TEFF_NUMERO_TERMINAL, DATOS_ENTRADA);
    setFieldTEFF(FIELD_TEFF_NUMERO_TERMINAL, datos.terminalId, LEN_TEFF_NUMERO_TERMINAL);
    setFieldTEFF(FIELD_TEFF_CODIGO_ESTABLECIMIENTO, tablaUno.receiptLine3, LEN_TEFF_CODIGO_ESTABLECIMIENTO);

    if (strlen(datos.txnDate) > 0) {
        ajusteFecha(datos.txnDate);
    } else {
        memset(datos.txnTime, 0x00, sizeof(datos.txnTime));
        memset(datos.txnDate, 0x00, sizeof(datos.txnDate));
        horaFechaTerminal(datos.txnTime, datos.txnDate);
    }
    setFieldTEFF(FIELD_TEFF_FECHA, datos.txnDate, LEN_TEFF_FECHA);
    setFieldTEFF(FIELD_TEFF_HORA, datos.txnTime, LEN_TEFF_HORA);

    if (atoi(datos.codigoGrupo) == LEALTAD_VENTA) {
        setFieldTEFF(FIELD_TEFF_NUMERO_BONO, datos.track2, strlen(datos.track2));
    }

    packMessage = packMessageTEFF();
    packMessageRespaldo = packMessage;

    borrarArchivo(discoNetcom, ARCHIVO_SINCRONIZACION_CAJA);

    //respuesta = realizarTransaccionCaja(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 200, 10 * 100);
    respuesta = realizarTransaccionMedioComunicacion(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 30 * 100);
    if (respuesta <= 0) {
        setParameter(CAJA_NO_RESPONDE, "1");
    }

    if (respuesta <= 0 && validacionClave == FALSE) {
        setParameter(CAJA_NO_RESPONDE, "1");
        //screenMessage("COMUNICACION", "CAJA", "SIN RESPUESTA", "", 2 * 1000);
    }

    respuesta = ((bufferOut[0] == ACK) ? 1 : -1);

    if (respuesta <= 0 && validacionClave == FALSE) {
        guardarTransaccionCajasSincronizacion(packMessage.isoPackMessage, packMessage.totalBytes);
    }

    return respuesta;
}

int transaccionRespuestaRedimirFalabella(DatosVenta datos) {

    int respuesta = 0;
    int longTrack1 = 0;
    char pan[LEN_TEFF_PAN + 1];
    char bufferOut[512 + 1];
    uChar campo93[TAM_TEXTO_INFORMACION + 1];
    uChar campo94[TAM_TRACK2 + 1];
    TablaUnoInicializacion tablaUno;
    ResultISOPack packMessage;

    memset(pan, 0x00, sizeof(pan));
    memset(&tablaUno, 0x00, sizeof(tablaUno));
    memset(&packMessage, 0x00, sizeof(packMessage));
    memset(bufferOut, 0x00, sizeof(bufferOut));
    tablaUno = _desempaquetarTablaCeroUno_();

    setTipoMensajeTEFF(MENSAJE_TEFF_NO_REQUIERE_RESPUESTA);
    setCodigoTransaccionTEFF(TRANSACCION_TEFF_REDIMIR_FALABELLA);
    setCodigoRespuestaTEFF(NO_HAY_CODIGO_DE_RESPUESTA);

    extraerPanAsteriscos(datos.track2, strlen(datos.track2), pan, ASTERISCOS_4_ULTIMOS);

    setFieldTEFF(FIELD_TEFF_ASTERISCOS_4_ULTIMOS_TARJETA, pan, LEN_TEFF_ASTERISCOS_4_ULTIMOS_TARJETA);

    if (verificarCodigoCajero() > 0) {
        setFieldTEFF(FIELD_TEFF_CODIGO_CAJERO, datos.codigoCajero, LEN_TEFF_CODIGO_CAJERO);
    }

    ajustarDatosTransaccion(datos.terminalId, LEN_TEFF_NUMERO_TERMINAL, DATOS_ENTRADA);
    setFieldTEFF(FIELD_TEFF_NUMERO_TERMINAL, datos.terminalId, LEN_TEFF_NUMERO_TERMINAL);
    setFieldTEFF(FIELD_TEFF_CODIGO_ESTABLECIMIENTO, tablaUno.receiptLine3, LEN_TEFF_CODIGO_ESTABLECIMIENTO);

    if (strlen(datos.txnDate) > 0) {
        ajusteFecha(datos.txnDate);
    } else {
        memset(datos.txnTime, 0x00, sizeof(datos.txnTime));
        memset(datos.txnDate, 0x00, sizeof(datos.txnDate));
        horaFechaTerminal(datos.txnTime, datos.txnDate);
    }
    setFieldTEFF(FIELD_TEFF_FECHA, datos.txnDate, LEN_TEFF_FECHA);
    setFieldTEFF(FIELD_TEFF_HORA, datos.txnTime, LEN_TEFF_HORA);

    longTrack1 = strlen(datos.textoInformacion);

    if (isdigit((int)datos.textoInformacion[longTrack1 - 2]) <= 0) {
        datos.textoInformacion[longTrack1 - 2] = '\0';
    } else if (isdigit((int)datos.textoInformacion[longTrack1 - 1]) <= 0) {
        datos.textoInformacion[longTrack1 - 1] = '\0';
    }

//	longTrack1 = strlen(datos.textoInformacion);

    memset(campo93, 0x00, sizeof(campo93));
    memset(campo94, 0x00, sizeof(campo94));

    memset(campo93, '*', strlen(datos.textoInformacion));
    strncpy(campo93, datos.textoInformacion, 1);
    memset(campo94, '*', strlen(datos.track2));

    setFieldTEFF(FIELD_TEFF_NUMERO_FACTURA, campo93, strlen(campo93));
    setFieldTEFF(FIELD_TEFF_INFO_ADICIONAL_OPERACION, campo94, strlen(campo94));

    packMessage = packMessageTEFF();

    borrarArchivo(discoNetcom, ARCHIVO_SINCRONIZACION_CAJA);

    setParameter(CAJA_NO_RESPONDE, "0");
    //screenMessage("COMUNICACION", "CAJA", "CONECTANDO", "", 2 * 1000);

    //respuesta = realizarTransaccionCaja(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 200, 10 * 100);
    respuesta = realizarTransaccionMedioComunicacion(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 10 * 100);
    if (respuesta <= 0) {
        setParameter(CAJA_NO_RESPONDE, "1");
        //screenMessage("COMUNICACION", "CAJA", "SIN RESPUESTA", "", 2 * 1000);
    }

    respuesta = ((bufferOut[0] == ACK) ? 1 : -1);

    if (respuesta <= 0) {
        guardarTransaccionCajasSincronizacion(packMessage.isoPackMessage, packMessage.totalBytes);
    }

    return respuesta;
}

/**
 * @brief	realiza la petición para las cajas
 * @param 	datos
 * @param 	tipoTransaccionCaja
 * @return	> 1 exitosa
 * @author
 */
int _transaccionPeticionDatosCaja_(DatosVenta *datos, int tipoTransaccionCaja) {

    int retorno = 0;
    uChar tipoCaja[SIZE_TIPO_CAJA + 1];

    memset(tipoCaja, 0x00, sizeof(tipoCaja));

    getParameter(TIPO_CAJA, tipoCaja);

    if (strcmp(tipoCaja, CAJA_SERIAL) == 0) {
        retorno = peticionCajaSerial(datos, tipoTransaccionCaja);
    } else {
        /*if (verificarCofiguracionURI()) {
            retorno = peticionCajaIP(datos, tipoTransaccionCaja);
        }*/
    }

    return retorno;
}

int transaccionRespuestaDatosCaja(DatosVenta datos, int tipoTransaccionCaja) {

    int retorno = 0;
    uChar tipoCaja[SIZE_TIPO_CAJA + 1];
    uChar cajaRespuestaIP[1 + 1];
    uChar firmaIP[2 + 1];
    uChar isKiosko[2 + 1];
    uChar inicioMenuVer[5 + 1];
    unsigned char terminal[8];

    memset(terminal, 0x00, sizeof(terminal));
    memset(cajaRespuestaIP, 0x00, sizeof(cajaRespuestaIP));
    memset(tipoCaja, 0x00, sizeof(tipoCaja));
    memset(firmaIP, 0x00, sizeof(firmaIP));
    memset(isKiosko, 0x00, sizeof(isKiosko));
    memset(inicioMenuVer, 0x00, sizeof(inicioMenuVer));

    getParameter(TIPO_CAJA, tipoCaja);
    getParameter(HABILITA_FIRMA_IP, firmaIP);
    getParameter(RESPUESTA_CAJA_IP, cajaRespuestaIP);
    getParameter(TIPO_TRANSACCION_CITIBANK, isKiosko);
    getParameter(NUMERO_PBX, inicioMenuVer);

        if (atoi(cajaRespuestaIP) == 2 || tipoTransaccionCaja == TRASACCION__TEFF_NOTIFICACION_SMS) {
            if (atoi(cajaRespuestaIP) == 2) {
                setParameter(RESPUESTA_CAJA_IP, "1");
            }

            //retorno = respuestaCajaIP(datos, tipoTransaccionCaja);
        } else {
            if (strcmp(tipoCaja, CAJA_SERIAL) == 0) {
                retorno = respuestaCajaSerial(datos, tipoTransaccionCaja);
            } else {

                if ((/*(verificarCofiguracionURI() && atoi(firmaIP) == 0) ||*/ atoi(isKiosko) == 1 || atoi(isKiosko) == 2
                     || (atoi(firmaIP) == 1 && atoi(datos.codigoRespuesta) == 1)) /*&& atoi(datos.codigoRespuesta) == 0*/) {
                    //retorno = respuestaCajaIP(datos, tipoTransaccionCaja);
                }
            }
        }


    return retorno;
}


void _sincronizacionCaja_(void) {

    uChar tipoCaja[SIZE_TIPO_CAJA + 1];

    memset(tipoCaja, 0x00, sizeof(tipoCaja));

    getParameter(TIPO_CAJA, tipoCaja);

    if (strcmp(tipoCaja, CAJA_SERIAL) == 0) {
        sincronizacionSerial();
    } else {
        /*if (verificarCofiguracionURI()) {
            sincronizacionIP();
        }*/
    }
}

int verificarAnulacionBclDeposito(int tipoTransaccion, uChar *tipoTransaccionOrigen) {
    int anulacionBclDeposito = FALSE;

    if (tipoTransaccion == TRANSACCION_TEFF_ANULACION && atoi(tipoTransaccionOrigen) == TRANSACCION_BCL_DEPOSITO) {
        anulacionBclDeposito = TRUE;
    }

    return anulacionBclDeposito;
}

int verificarAnulacionBclPago(int tipoTransaccion, uChar *tipoTransaccionOrigen) {
    int anulacionBclPago = FALSE;

    if (tipoTransaccion == TRANSACCION_TEFF_ANULACION
        && (atoi(tipoTransaccionOrigen) == TRANSACCION_BCL_PAGO_CARTERA
            || atoi(tipoTransaccionOrigen) == TRANSACCION_BCL_PAGO_TARJETA)) {
        anulacionBclPago = TRUE;
    }

    return anulacionBclPago;
}

/**
 * @brief verifica si es una anulacion de recaudo bancolombia
 * @param tipoTransaccion
 * @param tipoTransaccionOrigen
 * @return
 * @author
 */
int verificarAnulacionBclRecaudo(int tipoTransaccion, uChar *tipoTransaccionOrigen) {
    int anulacionBclPago = FALSE;

    if (tipoTransaccion == TRANSACCION_TEFF_ANULACION && (atoi(tipoTransaccionOrigen) == TRANSACCION_BCL_RECAUDO)) {
        anulacionBclPago = TRUE;
    }

    return anulacionBclPago;
}

/**
 * @brief Funcion que elimina los ceros de los centavos cuando la transaccion es declinada
 * @param *datos
 * @return datosajustados
 * @autor  Ronald Baez
 */
DatosVenta _aJustarDatosCajaDeclinado_(DatosVenta *datos) {
    uChar stringAux[20 + 1];
    DatosVenta datosajustados;
    memset(stringAux, 0x00, strlen(stringAux));
    memset(datos->numeroRecibo, 0x00, strlen(datos->numeroRecibo));
    memcpy(stringAux, datos->iva, strlen(datos->iva) - 2);
    memset(datos->iva, 0x00, sizeof(datos->iva));
    memcpy(datos->iva, stringAux, strlen(stringAux));
    memcpy(stringAux, datos->inc, strlen(datos->inc) - 2);
    memset(datos->inc, 0x00, sizeof(datos->inc));
    memcpy(datos->inc, stringAux, strlen(stringAux));

    if(atoi(datos->propina) > 0) {

        memcpy(stringAux, datos->propina, strlen(datos->propina) - 2);
        memset(datos->propina, 0x00, sizeof(datos->propina));
        memcpy(datos->propina, stringAux, strlen(stringAux));
    }
    return datosajustados;
}


int _transaccionEnvioFirmaIP_(DatosVenta datos, int tipoTransaccionCaja) {

    int retorno = 0;

    /*if (verificarCofiguracionURIFirma()) {
        retorno = envioFirmaIP(datos, tipoTransaccionCaja ,0);
    }*/

    return retorno;
}

DatosQrDescuentos binQrDescuentos(char * TransaccionOTP) {

    int lineasArchivo = 0;
    int indice = 0;
    int posicion = 0;
    long lengthArchivoQrDescuento = 0;
    char buffer[LEN_QR_DESCUENTO + 1];

    DatosQrDescuentos datosQrDescuento;

    memset(&datosQrDescuento, 0x00, sizeof(&datosQrDescuento));

    lengthArchivoQrDescuento = tamArchivo("/HOST", "/HOST/QRBINES.TXT");
    lineasArchivo = lengthArchivoQrDescuento / LEN_QR_DESCUENTO;

    for (indice = 0; indice < lineasArchivo; indice++) {

        memset(buffer, 0x00, sizeof(buffer));

        buscarArchivo("/HOST", "/HOST/QRBINES.TXT", buffer, posicion, LEN_QR_DESCUENTO);

        sprintf(datosQrDescuento.tipo, "%.2s", buffer);
        sprintf(datosQrDescuento.bin, "%.6s", buffer + 3);

        posicion += LEN_QR_DESCUENTO;

        if (atoi(TransaccionOTP) == atoi(datosQrDescuento.tipo)) {
            indice = lineasArchivo;
        }

    }

    return datosQrDescuento;
}

int realizarTransaccionMedioComunicacion(uChar * packMessage, int totalBytes, uChar * recibida, int timeOut) {
    int respuesta = 0;
    uChar tipoCajaLan[2 + 1];

    memset(tipoCajaLan, 0x00, sizeof(tipoCajaLan));

    getParameter(TIPO_CAJA_USB, tipoCajaLan);

    if (atoi(tipoCajaLan) == 2) {
        if (totalBytes > 1) {
            respuesta = realizarTransaccion(packMessage, totalBytes, recibida, TRANSACCION_CAJAS_IP, CANAL_DEMANDA,
                                            REVERSO_NO_GENERADO);
        }
    } else {
       //respuesta = realizarTransaccionCaja(packMessage, totalBytes, recibida, 200, timeOut);//ojo es la comunicacion serial
    }
    return respuesta;
}

int realizarTransaccionAnulacionMedioComunicacion(uChar * packMessage, int totalBytes, uChar * recibida, int timeOut) {
    int respuesta = 0;
    uChar tipoCajaLan[2 + 1];

    memset(tipoCajaLan, 0x00, sizeof(tipoCajaLan));

    getParameter(TIPO_CAJA_USB, tipoCajaLan);

    if (atoi(tipoCajaLan) == 2) {
        if (totalBytes > 1) {
            respuesta = realizarTransaccion(packMessage, totalBytes, recibida, TRANSACCION_CAJAS_IP, CANAL_DEMANDA,
                                            REVERSO_NO_GENERADO);
        }
    } else {
        //respuesta = realizarTransaccionCajaAnulacion(packMessage, totalBytes, recibida, 200, timeOut);//ojo es la comunicacion serial
    }
    return respuesta;
}

int transaccionPeticionEnvioGiro(DatosVenta *datos, int transaccion) {

    int respuesta = 0;
    int respondeCaja = 0;
    int longitud = 0;
    long compraNeta = 0;
    char auxiliar[30 + 1];
    char auxiliarMonto[12 + 1];
    char bufferOut[512 + 1];
    ResultISOPack packMessage;
    uChar factura[20 + 1];

    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(&packMessage, 0x00, sizeof(packMessage));
    memset(bufferOut, 0x00, sizeof(bufferOut));
    memset(auxiliarMonto, 0x00, sizeof(auxiliarMonto));
    memset(factura, 0x00, sizeof(factura));

    setTipoMensajeTEFF(MENSAJE_TEFF_REQUERIMIENTO);
    setCodigoTransaccionTEFF(transaccion); //TRANSACCION_TEFF_PAGO_TARJETA_PRIVADA
    setCodigoRespuestaTEFF(NO_HAY_CODIGO_DE_RESPUESTA);

    validarPosEntryMode(2, datos->posEntrymode);

    setFieldTEFF(FIELD_TEFF_TIPO_DOCUMENTO_BENEFICIARIO, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_DOCUMENTO_BENEFICIARIO, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_MONTO, auxiliar, 0);

    if (transaccion != TRANSACCION_TEFF_RECLAMAR_GIRO) {
        setFieldTEFF(FIELD_TEFF_TIPO_DOCUMENTO, auxiliar, 0);
        setFieldTEFF(FIELD_TEFF_NUMERO_DOCUMENTO, auxiliar, 0);
        if (transaccion != TRANSACCION_TEFF_CANCELAR_GIRO) {
            setFieldTEFF(FIELD_TEFF_TELEFONO_BENEFICIARIO, auxiliar, 0);
            setFieldTEFF(FIELD_TEFF_MONTO_GIRO, auxiliar, 0);
            setFieldTEFF(FIELD_TEFF_MONTO_COMISION, auxiliar, 0);
            setFieldTEFF(FIELD_TEFF_IVA, auxiliar, 0);
            setFieldTEFF(FIELD_TEFF_NUMERO_TELEFONO, auxiliar, 0);
        }
    }

    packMessage = packMessageTEFF();

    //respuesta = realizarTransaccionCaja(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 200, 30 * 100);
    respondeCaja = respuesta;

    if (respondeCaja == 0) {
        setParameter(CAJA_NO_RESPONDE, "1");
        //screenMessage("COMUNICACION", "CAJA", "SIN RESPUESTA", "", 2 * 1000);
    }

    if (respuesta > 0) {
        longitud = respuesta;
        respuesta = unpackRespuestaCaja(bufferOut, longitud, transaccion);
    }

    if (respuesta > 0) {

        //cambiar
        cargarCamposEstructuraDatosVenta(datos->totalVenta, FIELD_TEFF_MONTO);
        ajustarDatosTransaccion(datos->totalVenta, strlen(datos->totalVenta), DATOS_SALIDA);

        cargarCamposEstructuraDatosVenta(datos->iva, FIELD_TEFF_IVA);
        ajustarDatosTransaccion(datos->iva, strlen(datos->iva), DATOS_SALIDA);

        cargarCamposEstructuraDatosVenta(datos->baseDevolucion, FIELD_TEFF_MONTO_GIRO);
        ajustarDatosTransaccion(datos->baseDevolucion, strlen(datos->baseDevolucion), DATOS_SALIDA);

        cargarCamposEstructuraDatosVenta(datos->facturaCaja, FIELD_TEFF_NUMERO_TELEFONO);
        ajustarDatosTransaccion(datos->facturaCaja, strlen(datos->facturaCaja), DATOS_SALIDA);

        cargarCamposEstructuraDatosVenta(datos->appLabel, FIELD_TEFF_TELEFONO_BENEFICIARIO);
        ajustarDatosTransaccion(datos->appLabel, strlen(datos->appLabel), DATOS_SALIDA);

        cargarCamposEstructuraDatosVenta(datos->codigoCajero, FIELD_TEFF_CODIGO_CAJERO);
        ajustarDatosTransaccion(datos->codigoCajero, strlen(datos->codigoCajero), DATOS_SALIDA);

        cargarCamposEstructuraDatosVenta(datos->propina, FIELD_TEFF_MONTO_COMISION);
        ajustarDatosTransaccion(datos->propina, strlen(datos->propina), DATOS_SALIDA);

        cargarCamposEstructuraDatosVenta(datos->aux1, FIELD_TEFF_NUMERO_DOCUMENTO);
        ajustarDatosTransaccion(datos->aux1, 15, DATOS_SALIDA);

        cargarCamposEstructuraDatosVenta(datos->aux1 + 15, FIELD_TEFF_DOCUMENTO_BENEFICIARIO);
        ajustarDatosTransaccion(datos->aux1 + 15, strlen(datos->aux1 + 15), DATOS_SALIDA);

        cargarCamposEstructuraDatosVenta(datos->numeroRecibo2, FIELD_TEFF_TIPO_DOCUMENTO);
        //ajustarDatosTransaccion(datos->numeroRecibo2, 2, DATOS_SALIDA);

        cargarCamposEstructuraDatosVenta(datos->numeroRecibo2 + 2, FIELD_TEFF_TIPO_DOCUMENTO_BENEFICIARIO);
        //ajustarDatosTransaccion(datos->numeroRecibo2 + 2, 2, DATOS_SALIDA);

        sprintf(auxiliarMonto, "%ld", compraNeta);
        memcpy(datos->compraNeta, auxiliarMonto, 12);
    }

    if (respondeCaja > 0) {
        memset(auxiliar, 0x00, sizeof(auxiliar));
        auxiliar[0] = ACK;
        //realizarTransaccionCaja(auxiliar, 1, bufferOut, 200, 10);
    }

    return respuesta;
}

void armarCamposGiros(int transaccion, DatosVenta datos) {

    char auxiliar[30 + 1];

    memset(auxiliar, 0x00, sizeof(auxiliar));

    if (transaccion == TRANSACCION_TEFF_ENVIAR_GIRO || transaccion == TRANSACCION_TEFF_RECLAMAR_GIRO
        || transaccion == TRANSACCION_TEFF_CANCELAR_GIRO) {

        if (transaccion == TRANSACCION_TEFF_ENVIAR_GIRO) {
            setFieldTEFF(FIELD_TEFF_TIPO_DOCUMENTO_BENEFICIARIO, datos.numeroRecibo2 + 2, 2);
            memset(auxiliar, 0x00, sizeof(auxiliar));
            leftPad(auxiliar, datos.compraNeta, 0x30, LEN_TEFF_MONTO);
            setFieldTEFF(FIELD_TEFF_MONTO, auxiliar, LEN_TEFF_MONTO);
        } else {
            setFieldTEFF(FIELD_TEFF_TIPO_DOCUMENTO_BENEFICIARIO, "00", 2);
        }

        memset(auxiliar, 0x00, sizeof(auxiliar));
        leftPad(auxiliar, datos.aux1 + 15, 0x30, 11);
        setFieldTEFF(FIELD_TEFF_DOCUMENTO_BENEFICIARIO, auxiliar, 11);

        if (transaccion == TRANSACCION_TEFF_ENVIAR_GIRO){
            setFieldTEFF(FIELD_TEFF_TIPO_DOCUMENTO, datos.numeroRecibo2, 2);
        } else {
            setFieldTEFF(FIELD_TEFF_TIPO_DOCUMENTO, "00", 2);
        }

        memset(auxiliar, 0x00, sizeof(auxiliar));
        leftPad(auxiliar, datos.aux1, 0x30, 11);
        setFieldTEFF(FIELD_TEFF_NUMERO_DOCUMENTO, auxiliar, 11);

        memset(auxiliar, 0x00, sizeof(auxiliar));
        leftPad(auxiliar, datos.iva, 0x30, LEN_TEFF_MONTO);
        setFieldTEFF(FIELD_TEFF_IVA, auxiliar, LEN_TEFF_IVA);
        if (strlen(datos.appLabel) > 0) {
            setFieldTEFF(FIELD_TEFF_TELEFONO_BENEFICIARIO, datos.appLabel, strlen(datos.appLabel)); //mientras mas variables
        } else {
            setFieldTEFF(FIELD_TEFF_TELEFONO_BENEFICIARIO, "0000000000", 10); //mientras mas variables
        }

        memset(auxiliar, 0x00, sizeof(auxiliar));
        if (transaccion == TRANSACCION_TEFF_CANCELAR_GIRO) {
            leftPad(auxiliar, datos.compraNeta, 0x30, LEN_TEFF_MONTO);
        } else  {
            leftPad(auxiliar, datos.totalVenta, 0x30, LEN_TEFF_MONTO);
        }
        setFieldTEFF(FIELD_TEFF_MONTO_GIRO, auxiliar, LEN_TEFF_MONTO);

        memset(auxiliar, 0x00, sizeof(auxiliar));
        leftPad(auxiliar, datos.propina, 0x30, LEN_TEFF_MONTO);
        setFieldTEFF(FIELD_TEFF_MONTO_COMISION, auxiliar, LEN_TEFF_MONTO);

        if (strlen(datos.facturaCaja) > 0) {
            setFieldTEFF(FIELD_TEFF_NUMERO_TELEFONO, datos.facturaCaja, strlen(datos.facturaCaja));
        } else {
            setFieldTEFF(FIELD_TEFF_NUMERO_TELEFONO, "0000000000", 10);
        }
    }
}

int transaccionPeticionConsultaSaldo(DatosVenta *datos, int transaccion) {

    int respuesta = 0;
    int respondeCaja = 0;
    int longitud = 0;
    long compraNeta = 0;
    char pan[LEN_TEFF_PAN + 1];
    char auxiliar[30 + 1];
    char auxiliarMonto[12 + 1];
    char bufferOut[512 + 1];
    ResultISOPack packMessage;
    uChar factura[20 + 1];
    uChar facturaAux[20 + 1];

    memset(pan, 0x00, sizeof(pan));
    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(&packMessage, 0x00, sizeof(packMessage));
    memset(bufferOut, 0x00, sizeof(bufferOut));
    memset(auxiliarMonto, 0x00, sizeof(auxiliarMonto));
    memset(factura, 0x00, sizeof(factura));
    memset(facturaAux, 0x00, sizeof(facturaAux));

    setTipoMensajeTEFF(MENSAJE_TEFF_REQUERIMIENTO);
    setCodigoTransaccionTEFF(transaccion); //TRANSACCION_TEFF_PAGO_TARJETA_PRIVADA
    setCodigoRespuestaTEFF(NO_HAY_CODIGO_DE_RESPUESTA);

    validarPosEntryMode(2, datos->posEntrymode);

    if (verificarCodigoCajero() > 0) {
        setFieldTEFF(FIELD_TEFF_CODIGO_CAJERO, auxiliar, 0);
    }

    if (strlen(datos->track2) > 0) {
        extraerPanAsteriscos(datos->track2, strlen(datos->track2), pan, ASTERISCOS_4_ULTIMOS);
        setFieldTEFF(FIELD_TEFF_ASTERISCOS_4_ULTIMOS_TARJETA, pan, LEN_TEFF_PAN);
    }
    setFieldTEFF(FIELD_TEFF_MONTO, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_IVA, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_BASE_DEVOLUCION, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_INC, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_NUMERO_FACTURA_ENVIADO_CAJA, auxiliar, 0);

    packMessage = packMessageTEFF();

    respuesta = realizarTransaccionMedioComunicacion(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut,30 * 100);
    respondeCaja = respuesta;

    if (respondeCaja == 0) {
        setParameter(CAJA_NO_RESPONDE, "1");
        //screenMessage("COMUNICACION", "CAJA", "SIN RESPUESTA", "", 2 * 1000);
    }

    if (respuesta > 0) {
        longitud = respuesta;
        respuesta = unpackRespuestaCaja(bufferOut, longitud, transaccion);
    }

    if (respuesta > 0) {
        respuesta = validarCodigoCajero(datos->codigoCajero);
    }

    if (respuesta > 0) {

        //cambiar
        cargarCamposEstructuraDatosVenta(datos->totalVenta, FIELD_TEFF_MONTO);
        ajustarDatosTransaccion(datos->totalVenta, strlen(datos->totalVenta), DATOS_SALIDA);
        cargarCamposEstructuraDatosVenta(datos->codigoComercio, FIELD_TEFF_CODIGO_CONVENIO);
        ajustarDatosTransaccion(datos->codigoComercio, strlen(datos->codigoComercio), DATOS_SALIDA);
        cargarCamposEstructuraDatosVenta(datos->facturaCaja, FIELD_TEFF_NUMERO_FACTURA);
        ajustarDatosTransaccion(datos->facturaCaja, strlen(datos->facturaCaja), DATOS_SALIDA);
        if (transaccion == TRANSACCION_TEFF_PAGO_TCREDITO || transaccion == TRANSACCION_TEFF_PAGO_CARTERA) {
            cargarCamposEstructuraDatosVenta(factura, FIELD_TEFF_NUMERO_FACTURA);
            ajustarDatosTransaccion(factura, strlen(factura), DATOS_SALIDA);
            _rightTrim_(facturaAux, factura, 0x20);
            leftPad(datos->tokenVivamos, facturaAux, '0', 19);
        }
        sprintf(auxiliarMonto, "%ld", compraNeta);
        memcpy(datos->compraNeta, auxiliarMonto, 12);
    }

    if (respondeCaja > 0) {
        memset(auxiliar, 0x00, sizeof(auxiliar));
        auxiliar[0] = ACK;
        realizarTransaccionMedioComunicacion(auxiliar, 1, bufferOut,10);
    }

    return respuesta;
}


int transaccionRespuestaCupo(uChar *bufferSalida, DatosVenta datos, int transaccionCupo, int tipoCaja) {

    int respuesta = 0;
    char pan[LEN_TEFF_PAN + 1];
    char bufferOut[512 + 1];
    TablaUnoInicializacion tablaUno;
    ResultISOPack packMessage;
    ResultISOPack packMessageRespaldo;

    memset(pan, 0x00, sizeof(pan));
    memset(&tablaUno, 0x00, sizeof(tablaUno));
    memset(&packMessage, 0x00, sizeof(packMessage));
    memset(&packMessageRespaldo, 0x00, sizeof(packMessageRespaldo));
    memset(bufferOut, 0x00, sizeof(bufferOut));

    tablaUno = _desempaquetarTablaCeroUno_();

    setTipoMensajeTEFF(MENSAJE_TEFF_RESPUESTA);
    setCodigoTransaccionTEFF(transaccionCupo);
    validacionCodigoRespuestaCaja(datos.codigoRespuesta);
    setCodigoRespuestaTEFF(atoi(datos.codigoRespuesta));

    setFieldTEFF(FIELD_TEFF_CODIGO_RESPUESTA, datos.codigoRespuesta, LEN_TEFF_CODIGO_RESPUESTA);

    extraerPanAsteriscos(datos.track2, strlen(datos.track2), pan, ASTERISCOS_4_ULTIMOS);

    validarPosEntryMode(2, datos.posEntrymode);

    setFieldTEFF(FIELD_TEFF_ASTERISCOS_4_ULTIMOS_TARJETA, pan, LEN_TEFF_ASTERISCOS_4_ULTIMOS_TARJETA);

    ajustarDatosTransaccion(datos.baseDevolucion, LEN_TEFF_MONTO, DATOS_ENTRADA_MONTOS);
    setFieldTEFF(FIELD_TEFF_MONTO, datos.baseDevolucion, LEN_TEFF_MONTO);

    ajustarDatosTransaccion(datos.compraNeta, LEN_TEFF_MONTO, DATOS_ENTRADA_MONTOS);
    setFieldTEFF(FIELD_TEFF_DESCRIPCION_CONVENIO, datos.compraNeta, LEN_TEFF_MONTO);

    ajustarDatosTransaccion(datos.numeroRecibo, LEN_TEFF_NUMERO_RECIBO_DATAFONO, DATOS_ENTRADA);
    setFieldTEFF(FIELD_TEFF_NUMERO_RECIBO_DATAFONO, datos.numeroRecibo, LEN_TEFF_NUMERO_RECIBO_DATAFONO);

    setFieldTEFF(FIELD_TEFF_CODIGO_CORRESPONSAL, "0007", 4);

    setFieldTEFF(FIELD_TEFF_NUMERO_TERMINAL, datos.terminalId, LEN_TEFF_NUMERO_TERMINAL);

    if (strlen(datos.txnDate) > 0) {
        ajusteFecha(datos.txnDate);
    } else {
        memset(datos.txnTime, 0x00, sizeof(datos.txnTime));
        memset(datos.txnDate, 0x00, sizeof(datos.txnDate));
        horaFechaTerminal(datos.txnTime, datos.txnDate);
    }

    setFieldTEFF(FIELD_TEFF_FECHA, datos.txnDate, LEN_TEFF_FECHA);
    setFieldTEFF(FIELD_TEFF_HORA, datos.txnTime, LEN_TEFF_HORA);

    packMessage = packMessageTEFF();
    packMessageRespaldo = packMessage;

    if (tipoCaja == 1) {
        respuesta = realizarTransaccionMedioComunicacion(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut,
                                                         10 * 100);

        if (respuesta <= 0) {
            setParameter(CAJA_NO_RESPONDE, "1");
            //screenMessage("COMUNICACION", "CAJA", "SIN RESPUESTA", "", 2 * 1000);
        }
        respuesta = ((bufferOut[0] == ACK) ? 1 : -1);

        if (respuesta <= 0 && transaccionCupo != TRANSACCION_TEFF_SINCRONIZACION) {
            guardarTransaccionCajasSincronizacion(packMessage.isoPackMessage, packMessage.totalBytes);
        }
    } else if (tipoCaja == 2) {
        memcpy(bufferSalida, packMessage.isoPackMessage, packMessage.totalBytes);
        respuesta = packMessage.totalBytes;
    }

    return respuesta;
}


int validarPosEntryMode(int tipoOperacion, char * posEntry) {

    int respuesta = 0;
    char posEntryMode[3 + 1];

    memset(posEntryMode, 0x00, sizeof(posEntryMode));


    if (tipoOperacion == 1) {
        setParameter(ENVIA_POS_ENTRY_MODE_TEFF, "0");

        respuesta = cargarCamposEstructuraDatosVenta(posEntryMode, FIELD_TEFF_TIPO_ENTRADA);

        if (respuesta > 0) {
            setParameter(ENVIA_POS_ENTRY_MODE_TEFF, "1");
        } else {
            setParameter(ENVIA_POS_ENTRY_MODE_TEFF, "0");
        }
    } else {
        getParameter(ENVIA_POS_ENTRY_MODE_TEFF, posEntryMode);

        if (atoi(posEntryMode) == 1) {
            setFieldTEFF(FIELD_TEFF_TIPO_ENTRADA, posEntry, 3);
        }
    }

    return respuesta;
}

int transaccionRetiroAra(DatosVenta *datos) {
    int respuesta = 0;
    int respondeCaja = 0;
    int longitud = 0;
    char pan[LEN_TEFF_PAN + 1];
    char bufferOut[2024 + 1];
    char auxiliar[30 + 1];
    ResultISOPack packMessage;

    memset(pan, 0x00, sizeof(pan));
    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(&packMessage, 0x00, sizeof(packMessage));
    memset(bufferOut, 0x00, sizeof(bufferOut));

    setTipoMensajeTEFF(MENSAJE_TEFF_REQUERIMIENTO);
    setCodigoTransaccionTEFF(TRANSACCION_TEFF_COMPRAS);
    setCodigoRespuestaTEFF(NO_HAY_CODIGO_DE_RESPUESTA);

    if (atoi(datos->track2) != 0) {
        extraerPanAsteriscos(datos->track2, strlen(datos->track2), pan, ASTERISCOS_4_ULTIMOS);
    }

    setFieldTEFF(FIELD_TEFF_ASTERISCOS_4_ULTIMOS_TARJETA, pan, LEN_TEFF_PAN);

    setFieldTEFF(FIELD_TEFF_DATA_ADICIONAL, auxiliar, 0);

    if (verificarCodigoCajero() > 0) {
        setFieldTEFF(FIELD_TEFF_CODIGO_CAJERO, auxiliar, 0);
    }

    setFieldTEFF(FIELD_TEFF_MONTO, auxiliar, 0);

    setFieldTEFF(FIELD_TEFF_IVA, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_BASE_DEVOLUCION, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_RRN, auxiliar, 0); //inc para ARA

    packMessage = packMessageTEFF();

    respuesta = realizarTransaccionMedioComunicacion(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut,
                                                     30 * 100);

    respondeCaja = respuesta;

    if (respondeCaja == 0) {
        setParameter(CAJA_NO_RESPONDE, "1");
        //screenMessage("COMUNICACION", "CAJA", "SIN RESPUESTA", "", 2 * 1000);

    } else {

        memset(auxiliar, 0x00, sizeof(auxiliar));
        auxiliar[0] = ACK;
        realizarTransaccionMedioComunicacion(auxiliar, 1, bufferOut, 10);
    }

    if (respuesta > 1) {
        longitud = respuesta;
        respuesta = unpackRespuestaCaja(bufferOut, longitud, TRANSACCION_TEFF_RETIRO_ARA);
    } else {
        respuesta = 0;
    }

    if (respuesta > 0) {

        respuesta = validarCodigoCajero(datos->codigoCajero);

    }

    if (respuesta > 0) {


        memset(auxiliar, 0x00, sizeof(auxiliar));
        cargarCamposEstructuraDatosVenta(auxiliar, FIELD_TEFF_MONTO);
        memcpy(datos->totalVenta, auxiliar, TAM_COMPRA_NETA);
        ajustarDatosTransaccion(datos->totalVenta, strlen(datos->totalVenta), DATOS_ENTRADA_MONTOS);

        memset(auxiliar, 0x00, sizeof(auxiliar));
        cargarCamposEstructuraDatosVenta(auxiliar, FIELD_TEFF_IVA);
        memcpy(datos->iva, auxiliar, TAM_COMPRA_NETA);
        ajustarDatosTransaccion(datos->iva, strlen(datos->iva), DATOS_ENTRADA_MONTOS);

        memset(auxiliar, 0x00, sizeof(auxiliar));
        cargarCamposEstructuraDatosVenta(auxiliar, FIELD_TEFF_BASE_DEVOLUCION);
        memcpy(datos->baseDevolucion, auxiliar, TAM_COMPRA_NETA);
        ajustarDatosTransaccion(datos->baseDevolucion, strlen(datos->baseDevolucion), DATOS_ENTRADA_MONTOS);

        cargarCamposEstructuraDatosVenta(datos->inc, FIELD_TEFF_RRN);
        ajustarDatosTransaccion(datos->inc, strlen(datos->inc), DATOS_ENTRADA_MONTOS);

        respuesta = verificarMontosInconsistentes(datos);
    }

    if (respuesta <= 0) {

        setParameter(USO_CAJA_REGISTRADORA, "1");

    }

    return respuesta;

}

void armarRespuestaCajaAra(DatosVenta datos, int tipoCaja) {

    char flujoUnificado[2 + 1];

    memset(flujoUnificado, 0x00, sizeof(flujoUnificado));

    getParameter(FLUJO_UNIFICADO, flujoUnificado);

    ajustarDatosTransaccion(datos.rrn, LEN_TEFF_MONTO, DATOS_ENTRADA);
    setFieldTEFF(FIELD_TEFF_INC, datos.rrn, LEN_TEFF_INC);

    if (strcmp(datos.codigoRespuesta, "00") != 0) {
        setFieldTEFF(FIELD_TEFF_MONTO, "000000000000", LEN_TEFF_MONTO);
        setFieldTEFF(FIELD_TEFF_CODIGO_BANCO, "0000", 4);
        setFieldTEFF(FIELD_TEFF_CODIGO_AUTORIZACION, "000000", LEN_TEFF_CODIGO_AUTORIZACION);
        setFieldTEFF(FIELD_TEFF_RRN, "000000000000", LEN_TEFF_INC);
        setFieldTEFF(FIELD_TEFF_NUMERO_RECIBO_DATAFONO, "000000", LEN_TEFF_NUMERO_RECIBO_DATAFONO);
    } else {
        ajustarDatosTransaccion(datos.codigoAprobacion, LEN_TEFF_CODIGO_AUTORIZACION, DATOS_ENTRADA);
        setFieldTEFF(FIELD_TEFF_CODIGO_AUTORIZACION, datos.codigoAprobacion, LEN_TEFF_CODIGO_AUTORIZACION);
        strcat(datos.totalVenta, "00");
        ajustarDatosTransaccion(datos.totalVenta, LEN_TEFF_MONTO, DATOS_ENTRADA_MONTOS);
        setFieldTEFF(FIELD_TEFF_MONTO, datos.totalVenta, LEN_TEFF_MONTO);
        setFieldTEFF(FIELD_TEFF_CODIGO_BANCO, "0007", 4);
        if (atoi(datos.inc) == 0) {
            memset(datos.inc, 0x30, 12);
        }
        ajustarDatosTransaccion(datos.inc, LEN_TEFF_MONTO, DATOS_ENTRADA);
        if (tipoCaja == 1) {
            setFieldTEFF(FIELD_TEFF_RRN, datos.inc, LEN_TEFF_INC);
        }
    }

    setFieldTEFF(FIELD_TEFF_COD_RESPUESTA_BCL_JMR, datos.codigoRespuesta, LEN_TEFF_CODIGO_RESPUESTA);

    if (atoi(flujoUnificado) == 0) {
        setFieldTEFF(FIELD_TEFF_DATA_ADICIONAL, datos.aux1, 20);
        setParameter(TIPO_INICIA_TRANSACCION, "00");
    }
}
void ajustarMontosAra(char * dato) {

    char auxiliar[12 + 1];

    memset(auxiliar, 0x00, sizeof(auxiliar));
    leftTrim(auxiliar, dato, 0x30);
    memset(dato, 0x00, 12);
    memcpy(dato, auxiliar, strlen(auxiliar));
}

int validarCodigoUnicoTeff(void) {

    int respuesta = 1;
    int campoOK = 0;
    char codigoComercio [23 + 1];
    char codigoComercioTeff [23 + 1];

    memset(codigoComercio, 0x20, sizeof(codigoComercio));
    memset(codigoComercioTeff, 0x00, sizeof(codigoComercioTeff));

    getParameter(CODIGO_COMERCIO, codigoComercio);

    respuesta = cargarCamposEstructuraDatosVenta(codigoComercioTeff, FIELD_TEFF_CODIGO_ESTABLECIMIENTO);

    if (respuesta > 0) {
        campoOK = 1;
    }

    if (campoOK > 0 && atol(codigoComercioTeff) != atol(codigoComercio)) {
        respuesta = -1;
        //screenMessage("MENSAJE", "DATAFONO", "NO ASOCIADO", "A ESTE COMERCIO", 2 * 1000);
    } else {
        respuesta = 1;
    }

    return respuesta;
}


int _sincronizacionEnvioFirmaIP_(int tipoEnvio) {

    int tecla = 0;
    int salida = 0;
    int retorno = 0;
    int existeFirma = 0;
    int tipoTransaccionCaja = 0;
    char hora[10 + 1];
    char tipoTX[2 + 1];
    char fecha[20 + 1];
    char fechaAuxiliar[20 + 1];
    char datosLeidos[sizeof(DatosVenta) + 2];
    char titulo[50 + 1];
    char mensaje1[50 + 1];
    char mensaje2[50 + 1];
    char mensaje3[50 + 1];
    char mensaje4[50 + 1];
    char montoOriginal[12 + 1];

    DatosVenta datosSincronizados;

    memset(hora, 0x00, sizeof(hora));
    memset(fecha, 0x00, sizeof(fecha));
    memset(tipoTX, 0x00, sizeof(tipoTX));
    memset(datosLeidos, 0x00, sizeof(datosLeidos));
    memset(titulo, 0x00, sizeof(titulo));
    memset(mensaje1, 0x00, sizeof(mensaje1));
    memset(mensaje2, 0x00, sizeof(mensaje2));
    memset(mensaje3, 0x00, sizeof(mensaje3));
    memset(mensaje4, 0x00, sizeof(mensaje4));
    memset(fechaAuxiliar, 0x00, sizeof(fechaAuxiliar));
    memset(montoOriginal, 0x00, sizeof(montoOriginal));
    memset(&datosSincronizados, 0x00, sizeof(datosSincronizados));

    //leerArchivo(discoNetcom, SINCRONIZACION_SERVICIO_IP, datosLeidos);

    //existeFirma = tamArchivo(discoNetcom, "/HOST/SIGNA.BMP");

    memcpy(tipoTX, datosLeidos, 2);
    memcpy(&datosSincronizados, datosLeidos + 2, sizeof(datosSincronizados));

    if (tipoEnvio == 1) {
        memcpy(fechaAuxiliar, datosSincronizados.txnDate, sizeof(fechaAuxiliar));
        formatoFecha(datosSincronizados.txnDate);
        memcpy(fecha, datosSincronizados.txnDate, strlen(datosSincronizados.txnDate));
        memset(datosSincronizados.txnDate, 0x00, sizeof(datosSincronizados.txnDate));
        memcpy(datosSincronizados.txnDate, fechaAuxiliar, sizeof(datosSincronizados.txnDate));

        sprintf(hora, "%.2s:%.2s:%.2s", datosSincronizados.txnTime, datosSincronizados.txnTime + 2,
                       datosSincronizados.txnTime + 4);
        strcat(fecha, " ");
        strcat(fecha, hora);
        formatString(0x30, 0, datosSincronizados.totalVenta, strlen(datosSincronizados.totalVenta) - 2, montoOriginal,
                     1);
        strcpy(titulo, "SINCRONIZAR");
        strcpy(mensaje1, "RECIBO: ");
        strcat(mensaje1, datosSincronizados.numeroRecibo);
        strcpy(mensaje2, fecha);
        strcpy(mensaje3, "Monto :");
        strcat(mensaje3, montoOriginal);
        strcpy(mensaje4, "Firma :");
        if (existeFirma > 0) {
            strcat(mensaje4, "SI");
        } else {
            strcat(mensaje4, "NO");
        }
        //screenMessageFiveLine(titulo, mensaje1, mensaje2, mensaje3, mensaje4, "[ROJO]No             Si[VERDE]", 1);
    } else {
        retorno = 1;
    }
    if (retorno > 0) {
        if (existeFirma > 1) {
            setParameter(IMPRIMIR_PANEL_FIRMA, "1");
        }
        do {
            //retorno = chequearCableRed();
        } while (retorno == 0);

        if (retorno > 0) {
            //screenMessage("MENSAJE", "SINCRONIZACION", "PENDIENTE", "ESPERE POR FAVOR", 1 * 1000);
            //retorno = envioFirmaIP(datosSincronizados, tipoTransaccionCaja, 1);
        }

        if (retorno > 0) {
            setParameter(IMPRIMIR_PANEL_FIRMA, "0");
            //screenMessage("MENSAJE", "", "SINCRONIZACION", "EXITOSA", 1 * 1000);
            //borrarArchivo(discoNetcom, SINCRONIZACION_SERVICIO_IP);
            //borrarArchivo("/HOST", "/HOST/SIGNA.BMP");
        } else {
            //screenMessage("MENSAJE", "SINCRONIZACION", "NO COMPLETADA", "INTENTE NUEVAMENTE", 1 * 1000);
            retorno = -2;
        }
    }

    return retorno;
}
int transaccionPeticionRetiro(DatosVenta *datos) {

    int respuesta = 0;
    int respondeCaja = 0;
    int longitud = 0;
    long compraNeta = 0;
    char pan[LEN_TEFF_PAN + 1];
    char auxiliar[30 + 1];
    char auxiliarMonto[12 + 1];
    char bufferOut[512 + 1];
    ResultISOPack packMessage;

    memset(pan, 0x00, sizeof(pan));
    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(&packMessage, 0x00, sizeof(packMessage));
    memset(bufferOut, 0x00, sizeof(bufferOut));
    memset(auxiliarMonto, 0x00, sizeof(auxiliarMonto));

    setTipoMensajeTEFF(MENSAJE_TEFF_REQUERIMIENTO);
    setCodigoTransaccionTEFF(TRANSACCION_TEFF_RETIRO_SIN_TARJETA);
    setCodigoRespuestaTEFF(NO_HAY_CODIGO_DE_RESPUESTA);

    validarPosEntryMode(2, datos->posEntrymode);

    if (verificarCodigoCajero() > 0) {
        setFieldTEFF(FIELD_TEFF_CODIGO_CAJERO, auxiliar, 0);
    }

    setFieldTEFF(FIELD_TEFF_MONTO, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_NUMERO_DOCUMENTO, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_NUMERO_CUENTA, auxiliar, 0);

    packMessage = packMessageTEFF();
   // respuesta = realizarTransaccionCaja(packMessage.isoPackMessage, packMessage.totalBytes, bufferOut, 200, 30 * 100);
    respondeCaja = respuesta;

    if (respondeCaja == 0) {
        setParameter(CAJA_NO_RESPONDE, "1");
        //screenMessage("COMUNICACION", "CAJA", "SIN RESPUESTA", "", 2 * 1000);
    }

    if (respuesta > 0) {
        longitud = respuesta;
        respuesta = unpackRespuestaCaja(bufferOut, longitud, TRANSACCION_TEFF_RETIRO_SIN_TARJETA);
    }

    if (respuesta > 0) {
        respuesta = validarCodigoCajero(datos->codigoCajero);
    }

    if (respuesta > 0) {

        cargarCamposEstructuraDatosVenta(datos->totalVenta, FIELD_TEFF_MONTO);
        ajustarDatosTransaccion(datos->totalVenta, strlen(datos->totalVenta), DATOS_SALIDA);
        cargarCamposEstructuraDatosVenta(datos->field57, FIELD_TEFF_NUMERO_DOCUMENTO);
        ajustarDatosTransaccion(datos->field57, strlen(datos->field57), LEN_TEFF_NUMERO_DOCUMENTO);
        cargarCamposEstructuraDatosVenta(datos->propina, FIELD_TEFF_NUMERO_CUENTA);
        ajustarDatosTransaccion(datos->propina, LEN_TEFF_NUMERO_CUENTA, DATOS_SALIDA);
        //cargarCamposEstructuraDatosVenta(datos->facturaCaja,FIELD_TEFF_NUMERO_FACTURA_ENVIADO_CAJA);
        //ajustarDatosTransaccion(datos->facturaCaja,strlen(datos->facturaCaja),DATOS_SALIDA);

        if (compraNeta < 0) {
            respuesta = -1;
            //screenMessage("COMUNICACION", "MONTOS", "INCONSISTENTES", "", 2 * 1000);
        }

    }
    if (respondeCaja > 0) {
        memset(auxiliar, 0x00, sizeof(auxiliar));
        auxiliar[0] = ACK;
        //realizarTransaccionCaja(auxiliar, 1, bufferOut, 200, 10);

    }

    return respuesta;
}

int validarCampoCorresponsal(int campo) {
    int campoActivo = 0;
    int valor = -1;

    char buffer[12 + 1];
    char bufferAuxiliar[12 + 1];

    memset(buffer, 0x00, sizeof(buffer));
    memset(bufferAuxiliar, 0x00, sizeof(bufferAuxiliar));

    campoActivo = cargarCamposEstructuraDatosVenta(buffer, campo);

    if (campoActivo == TRUE) {

        leftTrim(bufferAuxiliar, buffer, 0x30);
        valor = atol(bufferAuxiliar);
    }

    return valor;
}