//
// Created by NETCOM on 06/03/2020.
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

#define  LOG_TAG    "NETCOM_DINERO_ELECTRONICO"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

ParametrosVenta parametrosVenta;
ParametrosDineroElectronico parametrosCompra;

DatosCierreDineroElectronico cierreEntidades[10 + 1];
int bit5Prendido = 0;
int bit4Prendido = 0;
int bit3Prendido = 0;
int bit4Validacion = 0;
int bit3Validacion = 0;
int bit2Validacion = 0;
int bit1Validacion = 0;
int banderaValidacion = 0;

int _dineroElectronico_(DatosVenta datosVentaPaypass, int TipoTransaccion) {

    int respuesta = 0;
    uChar tipoTransaccion[2 + 1];
    char flujoUnificado[1 + 1];

    memset(tipoTransaccion, 0x00, sizeof(tipoTransaccion));
    memset(flujoUnificado, 0x00, sizeof(flujoUnificado));

    getParameter(TIPO_TRANSACCION_OTP, tipoTransaccion);
    getParameter(FLUJO_UNIFICADO, flujoUnificado);

    if (verificarHabilitacionCaja() == TRUE) {
        switch (atoi(tipoTransaccion)) {

            case TRANSACCION_TEFF_COMPRAS:
            case TRANSACCION_TEFF_COMPRA_REFERENCIA:
                setParameter(TIPO_TRANSACCION_OTP, "01");
                sprintf(globalTramaVenta.tipoTransaccion, "%02d", TRANSACCION_PAGO_ELECTRONICO);
                //respuesta = dineroElectronicoCompra(datosVentaPaypass, 0);
                break;

            case TRANSACCION_TEFF_EFECTIVO:
                setParameter(TIPO_TRANSACCION_OTP, "00");
                sprintf(globalTramaVenta.tipoTransaccion, "%02d", TRANSACCION_RETIRO_OTP);
                //respuesta = dineroElectronicoCompra(datosVentaPaypass, 0);
                break;
        }

    } else {
        sprintf(globalTramaVenta.tipoTransaccion, "%02d",
                TRANSACCION_PAGO_ELECTRONICO); //para dinero electronico sin flujo unificado
        //dineroElectronicoCompra(datosVentaPaypass, 0);
    }
    return respuesta;
}

int dineroElectronicoCompra(DatosVenta datosVentaPaypass, int TipoTransaccion, int tipoTxOtp,
                            int opcion, char *valor) {

    int respuesta = 0;
    uChar mensajeError[21 + 1];
    DatosTransaccionDeclinada datosTransaccionDeclinada;
    unsigned char terminal[64];

    memset(&datosTransaccionDeclinada, 0x00, sizeof(datosTransaccionDeclinada));
    memset(mensajeError, 0x00, sizeof(mensajeError));
    memset(terminal, 0x00, sizeof(terminal));
    memset(globalTramaVenta.acquirerId, 0x00, sizeof(globalTramaVenta.acquirerId));

    //respuesta = capturarDatosTransaccion(tipoTxOtp, opcion, valor);

    return respuesta;
}

int intentosPin(int tipoTransaccion) {

    int respuesta = 0;
    uChar transaccionInicial[2 + 1] = {0x00};
    uChar transaccionFinal[2 + 1] = {0x00};

    if (tipoTransaccion == ENVIO_PAGO_ELECTRONICO) {

        if (bit3Prendido == TRUE) {
            getParameter(TIPO_TRANSACCION, transaccionInicial);
            sprintf(transaccionFinal, "%02d", TRANSACCION_PAGO_ELECTRONICO_ESPECIAL);
            setParameter(TIPO_TRANSACCION, transaccionFinal);
        }

        respuesta = capturarPINEmisores(globalTramaVenta.track2);
    } else {
        respuesta = capturarPINEmisores(globalTramaVenta.track2);
    }
    return respuesta;
}

int capturaDato(int tipoDato, char *dato) {

    uChar varDocumento[10 + 1];
    uChar variableAux[16 + 1];
    uChar titulo[16 + 1];
    uChar texto1[16 + 1];
    uChar texto2[16 + 1];
    int resultado = 0;
    int digitos = 0;

    memset(titulo, 0x00, sizeof(titulo));
    memset(texto1, 0x00, sizeof(texto1));
    memset(texto2, 0x00, sizeof(texto1));

    switch (tipoDato) {
        case 0:
            strcpy(titulo, "NUMERO CELULAR");
            strcpy(texto1, "INGRESE");
            strcpy(texto2, "NUMERO CELULAR");
            digitos = 10;
            break;
        case 1:
            strcpy(titulo, "DOCUMENTO");
            strcpy(texto1, "INGRESE");
            strcpy(texto2, "DOCUMENTO");
            digitos = 10;
            break;
        case 2:
            strcpy(titulo, "TRANSFERENCIA");
            strcpy(texto1, "CODIGO");
            strcpy(texto2, "ENTIDAD DESTINO");
            digitos = 4;
            break;
        default:
            break;
    }

    do {
        memset(variableAux, 0x00, sizeof(variableAux));
        memset(varDocumento, 0x00, sizeof(varDocumento));
        //resultado = getStringKeyboard(titulo, texto1, texto2, digitos, variableAux, DATO_NUMERICO);

        memset(varDocumento, 0x00, strlen(variableAux));

        if (resultado > 0 && strncmp(varDocumento, variableAux, strlen(variableAux)) == 0) {
            //screenMessage("MENSAJE", "DATO INVALIDO", "INGRESE UN VALOR", "DIFERENTE", 2 * 1000);
        } else if (
                (resultado > 0 && strncmp(varDocumento, variableAux, strlen(variableAux)) != 0) ||
                resultado < 0) {
            break;
        }

    } while (1);

    memcpy(dato, variableAux, digitos);

    return resultado;
}


ParametrosDineroElectronico capturarDatosTransaccion(int tipoTxOtp, int opcion, char *valor) {

    DatosBasicosVenta datosBasicos;
    TablaSieteInicializacion informacionEntidades;
    TablaUnoInicializacion tablaUno;
    uChar bitSet[8 + 1];
    uChar banderaValidacion2[8 + 1];
    uChar documento[10 + 1];
    uChar celular[12 + 1];
    uChar auxBuffer[12 + 1];
    uChar variableAux[16 + 1];
    uChar fiidOTP[4 + 1];
    uChar codigoComercio[23 + 1];

    long totalVenta = 0;
    int respuesta = 1;
    int tamanoDocumento = 0;
    char bin[4 + 1];
    char fiidAuxiliar[3 + 1] = {0x00};
    char fiid[4 + 1] = {0x00};
    char flujoUnificado[1 + 1];
    char itemTipoDocumento[50 + 1] = {0x00};
    int iRet = 0;

    banderaValidacion = 0;

    bit5Prendido = 0;
    bit4Prendido = 0;
    bit3Prendido = 0;
    bit4Validacion = 0;
    bit3Validacion = 0;
    bit2Validacion = 0;
    bit1Validacion = 0;
    LOGI("opcion %d", opcion);
    informacionEntidades = obtenerOpcionMenu(opcion, valor);
    memset(&parametrosCompra, 0x00, sizeof(parametrosCompra));
    getParameter(FLUJO_UNIFICADO, flujoUnificado);
    getParameter(CODIGO_COMERCIO, codigoComercio);
    tablaUno = _desempaquetarTablaCeroUno_();

    memcpy(globalTramaVenta.codigoComercio, codigoComercio, 23);
    memcpy(parametrosCompra.binAsignado, informacionEntidades.binAsignado,
           strlen(informacionEntidades.binAsignado));

    if (informacionEntidades.tipoTransaccion != -11 && informacionEntidades.tipoTransaccion != -3) {
        bitSet[0] = informacionEntidades.tipoTransaccion;
        calcularBitsEncendidos(bitSet);

        _convertBCDToASCII_(fiid, informacionEntidades.codigoFiid, 4);
        memcpy(parametrosCompra.fiddOtp, fiid, 4);
        memcpy(fiidAuxiliar, fiid + 1, 3);
        LOGI("fiidAuxiliar %s", fiidAuxiliar);
        banderaValidacion2[0] = informacionEntidades.banderaValidacion;

        memcpy(globalTramaVenta.field61, informacionEntidades.codigoFiid, TAM_CARD_NAME);
        _convertBCDToASCII_(fiidOTP, globalTramaVenta.field61, 4);

        if (strncmp(fiidOTP, "0812", 4) == 0 || strncmp(fiidOTP, "8999", 4) == 0 ||
            strncmp(fiidOTP, "7999", 4) == 0) {
            LOGI("fiidOTP A%s", fiidOTP);
            bit5Prendido = bitSet[3] - '0';
            LOGI("bit5Prendido %d", bit5Prendido);
            bit4Prendido = bitSet[4] - '0';
            LOGI("bit4Prendido %d", bit4Prendido);
        } else {
            LOGI("fiidOTP B %s", fiidOTP);
            bit4Prendido = bitSet[3] - '0';
            LOGI("bit4Prendido %d", bit4Prendido);
            bit3Prendido = bitSet[2] - '0';
            LOGI("bit3Prendido %d", bit3Prendido);
        }

        calcularBitsEncendidos(banderaValidacion2);
        bit4Validacion = banderaValidacion2[4] - '0';
        bit3Validacion = banderaValidacion2[5] - '0';
        bit2Validacion = banderaValidacion2[6] - '0';
        bit1Validacion = banderaValidacion2[7] - '0';

        memcpy(globalTramaVenta.cardName, informacionEntidades.nombreBanco, TAM_CARD_NAME);
        LOGI("cardName %s", globalTramaVenta.cardName);
        respuesta = informacionEntidades.codigoEntidad;

    } else if (informacionEntidades.tipoTransaccion == -3) {
        respuesta = -3;
    } else {
        //respuesta = capturarDatoServicio("DINERO ELECTRONICO", "INGRESE CODIGO", "DE LA ENTIDAD", 4, fiid);

        if (respuesta > 0) {
            memcpy(fiidAuxiliar, fiid + 1, 3);
            memcpy(globalTramaVenta.cardName, "DAVIPLATA", TAM_CARD_NAME);
            bit3Prendido = TRUE;
            banderaValidacion = 3;
        }
    }

    if (strncmp(informacionEntidades.nombreBanco, "BTERA.OTP", 9) == 0) {
        switch (tipoTxOtp) {
            case COMPRA_OTP:
                sprintf(globalTramaVenta.tipoTransaccion, "%02d", TRANSACCION_PAGO_ELECTRONICO);
                break;
            case RETIRO_OTP:
                sprintf(globalTramaVenta.tipoTransaccion, "%02d", TRANSACCION_RETIRO_OTP);
                break;
        }
    } else {
        sprintf(globalTramaVenta.tipoTransaccion, "%02d", TRANSACCION_PAGO_ELECTRONICO);
    }

    if (strncmp(fiidOTP, "8999", 4) != 0 ||
        atoi(globalTramaVenta.tipoTransaccion) != TRANSACCION_RETIRO_OTP) {

        ///////CELULAR///////////////////
        if ((respuesta > 0) &&
            ((bit4Prendido == TRUE || bit5Prendido == TRUE) && (bit2Validacion == TRUE))) {
            parametrosCompra.permiteCelular = 1;
        }

        ///////DOCUMENTO///////////////////
        if ((respuesta > 0) &&
            ((bit4Prendido == TRUE || bit5Prendido == TRUE) && (bit1Validacion == TRUE))) {

            //respuesta = capturaDato(DATO_NUMERO_DOCUMENTO, documento);
            parametrosCompra.permiteDocumento = 1;
            memset(globalTramaVenta.fiel42, 0x30, sizeof(globalTramaVenta.fiel42));
            tamanoDocumento = strncmp(fiidOTP, "8999", 4) == 0 ? 10 : 12;
            tamanoDocumento = strncmp(fiidOTP, "7999", 4) == 0 ? 10 : tamanoDocumento;
            leftPad(globalTramaVenta.fiel42, documento, 0x30, tamanoDocumento);
            if (bit4Validacion == TRUE && respuesta > 0) {
                strcpy(itemTipoDocumento, "TIPO DOCUMENTO;");
                strcat(itemTipoDocumento, "2;");
                strcat(itemTipoDocumento, "CEDULA;");
                strcat(itemTipoDocumento, "C.EXTRANJERIA;");
                memcpy(parametrosCompra.itemsTipoDocumento, itemTipoDocumento,
                       strlen(itemTipoDocumento));

                switch (respuesta) {
                    case C_CIUDADANIA:
                        memcpy(globalTramaVenta.isQPS, "2", 1);
                        break;
                    case C_EXTRANJERIA:
                        memcpy(globalTramaVenta.isQPS, "3", 1);
                        break;
                }
            } else {
                strcpy(itemTipoDocumento, "NOMENU;");
                memcpy(parametrosCompra.itemsTipoDocumento, itemTipoDocumento,
                       strlen(itemTipoDocumento));
                LOGI("items  %s", parametrosCompra.itemsTipoDocumento);
            }
        } else {
            strcpy(itemTipoDocumento, "NOMENU;");
            memcpy(parametrosCompra.itemsTipoDocumento, itemTipoDocumento,
                   strlen(itemTipoDocumento));
            LOGI("items  %s", parametrosCompra.itemsTipoDocumento);
        }

    }
    return parametrosCompra;
}

void construccionTrack(char *bin, char *fiid, char *celular, char *track) {

    char binEmisores[6 + 1];
    char fiidEmisores[4 + 1];
    char celularEmisores[10 + 1];
    char auxiliarCelular[10 + 1];
    LOGI("celular track2  %s ",celular);
    memset(binEmisores, 0x00, sizeof(binEmisores));
    memset(fiidEmisores, 0x00, sizeof(fiidEmisores));
    memset(celularEmisores, 0x00, sizeof(celularEmisores));
    memset(fiidEmisores, 0x00, sizeof(fiidEmisores));
    memset(auxiliarCelular, 0x30, sizeof(auxiliarCelular));

    if (bit3Prendido == TRUE) {
        memcpy(binEmisores, "881017", 6);
    } else if (bit4Prendido == TRUE) {
        memcpy(binEmisores, bin, 6);
    }

    memcpy(fiidEmisores, fiid, 3);
    if (strlen(celular) > 0) {
        leftPad(celularEmisores, celular, 0x30, 10);
        LOGI("celularEmisores %s ",celularEmisores);

    } else {
        memcpy(celularEmisores, auxiliarCelular, 10);
        LOGI("celularEmisores %s ",celularEmisores);
    }

    sprintf(track, "%s%s%s%s%s", binEmisores, fiidEmisores, celularEmisores, "D",
            "601200000000");
    memcpy(globalTramaVenta.ultimosCuatro, celularEmisores + 6, 4);
}

int capturarPINEmisores(char *track) {
    int n = 0;
    int resultado = 0;
    char pan[20 + 1] = {0x00};
    uChar fiidOTP[4 + 1] = {0x00};

    _convertBCDToASCII_(fiidOTP, globalTramaVenta.field61, 4);
    LOGI("capturar pin emisores  %s", globalTramaVenta.field61);
    memcpy(parametrosCompra.fiddOtp, fiidOTP, strlen(fiidOTP));
    //////// SE EXTRAE EL PAN /////////////
    for (n = 0; n < strlen(globalTramaVenta.track2); n++) {
        if (track[n] == 'D' || track[n] == 'd') {
            break;
        }
    }

    memcpy(pan, track, n);
    LOGI("capturarPINEmisores fiidOTP  %s", fiidOTP);
    if (strncmp(fiidOTP, "8999", 4) != 0 && strncmp(fiidOTP, "7999", 4) != 0) {
        resultado = 1;
    } else {
        if (strncmp(fiidOTP, "7999", 4) != 0) {
            resultado = 3;
        } else {
            resultado = 2;
        }
    }
    if (verificarHabilitacionCaja() == TRUE) {
        if (resultado > 0 && strncmp(fiidOTP, "7999", 4) == 0) {
            memset(globalTramaVenta.track2, 0x00, sizeof(globalTramaVenta.track2));
            strcpy(globalTramaVenta.track2, globalTramaVenta.textoAdicionalInicializacion + 6);
            strcat(globalTramaVenta.track2, "0000000");
            //resultado = transaccionPeticionDatosCaja(&datosVenta, TRANSACCION_TEFF_COMPRAS);

        }
    }

    return resultado;
}

int consultaCompraDineroElectronico(char *pinblock, int intentosPin) {

    int respuesta = 0;
    memset(globalTramaVenta.processingCode, 0x00, sizeof(globalTramaVenta.processingCode));
    memcpy(globalTramaVenta.processingCode, "361000", 6);
    armarTramaDineroElectronico(pinblock, ENVIO_SOLICITUD_PAGO_ELECTRONICO, 0, intentosPin);
    respuesta = procesarTransaccion(ENVIO_SOLICITUD_PAGO_ELECTRONICO);

    return respuesta;
}

int procesarCompraDineroElectronico(char *pinblock, int intentosPin) {

    int idx = 0;
    int iniUlt4 = 0;
    int respuesta = 0;
    int intentos = 0;
    int tipoCuentaAux = 0;
    int codigoOTPAux = 0;
    int indice = 2;
    int longitudOtp = 1;
    uChar tipoCuenta[2 + 1];
    uChar codigoOTP[2 + 1];
    uChar fiidOTP[4 + 1];

    DatosTarjeta leerTrack;

    memset(globalTramaVenta.processingCode, 0x00, sizeof(globalTramaVenta.processingCode));
    memset(tipoCuenta, 0x00, sizeof(tipoCuenta));
    memset(codigoOTP, 0x00, sizeof(codigoOTP));
    memset(&leerTrack, 0x00, sizeof(leerTrack));
    memset(fiidOTP, 0x00, sizeof(fiidOTP));


    _convertBCDToASCII_(fiidOTP, globalTramaVenta.field61, 4);
    LOGI("fiidOTP  %s", fiidOTP);
    if (strncmp(fiidOTP, "7999", 4) == 0) {
        indice = 0;
        longitudOtp = 2;
    }
    memcpy(tipoCuenta, globalTramaVenta.tipoCuenta, 2);

    memcpy(codigoOTP, globalTramaVenta.textoAdicionalInicializacion + indice, longitudOtp);
    LOGI("codigootp  %s", codigoOTP);

    tipoCuentaAux = atoi(tipoCuenta);
    codigoOTPAux = atoi(codigoOTP);
    LOGI("codigoOTPAux  %d", codigoOTPAux);
    switch (codigoOTPAux) {
        case COD_AHORRO:
            codigoOTPAux = 10;
            break;
        case COD_CORRIENTE:
            codigoOTPAux = 20;
            break;
        case COD_CREDITO:
        case COD_TUYA:
            codigoOTPAux = 30;
            break;
        default:
            codigoOTPAux = 00;
            break;
    }

    if (atoi(globalTramaVenta.tipoCuenta) == 0) {
        LOGI("tipo transacion  %s", globalTramaVenta.tipoTransaccion);
        if ((atoi(globalTramaVenta.tipoTransaccion) == TRANSACCION_PAGO_ELECTRONICO)
            || (atoi(globalTramaVenta.tipoTransaccion) == TRANSACCION_CONSULTA_PAGO_ELECTRONICO)
            || (atoi(globalTramaVenta.tipoTransaccion) == TRANSACCION_PAGO_ELECTRONICO_ESPECIAL) ||
            bit4Prendido == TRUE) {
            if (strncmp(fiidOTP, "7999", 4) != 0) {
                if (bit3Prendido == TRUE) {
                    memcpy(globalTramaVenta.processingCode, "004100", 6); //esta en 10 se coloca 41
                } else if (bit4Prendido == TRUE) {
                    LOGI("tipo de cuenta es D  ");
                    memcpy(globalTramaVenta.processingCode, "004100", 6); ////para mirara
                } else if (bit5Prendido == TRUE) {
                    LOGI("tipo de cuenta es E  ");
                    sprintf(globalTramaVenta.processingCode, "00%02d00",
                            codigoOTPAux); //// interoperabilidad
                    sprintf(globalTramaVenta.tipoCuenta, "%02d", codigoOTPAux);
                }

            } else {
                if (codigoOTPAux == 30) {
                    LOGI("codigoOTPAux  %d", codigoOTPAux);
                    sprintf(globalTramaVenta.processingCode, "00%02d00",
                            codigoOTPAux); //// interoperabilidad
                    LOGI("globalTramaVenta.processingCode  %s", globalTramaVenta.processingCode);
                    sprintf(globalTramaVenta.tipoCuenta, "%02d", codigoOTPAux);
                } else {
                    strcpy(globalTramaVenta.processingCode, "000000");
                }
            }

        } else if (atoi(globalTramaVenta.tipoTransaccion) == TRANSACCION_RETIRO_OTP) {
            LOGI("se va por retiro  ");
            memcpy(globalTramaVenta.processingCode, "014100", 6);
        }

    } else {
        if (atoi(globalTramaVenta.tipoTransaccion) == TRANSACCION_PAGO_ELECTRONICO) {
            memset(globalTramaVenta.processingCode, 0x00, sizeof(globalTramaVenta.processingCode));
            if (bit3Prendido == TRUE) {
                sprintf(globalTramaVenta.tipoCuenta, "%02d", tipoCuentaAux);
                sprintf(globalTramaVenta.processingCode, "00%02d00", tipoCuentaAux);
            } else if (bit4Prendido == TRUE) {
                sprintf(globalTramaVenta.tipoCuenta, "%02d", tipoCuentaAux);
                sprintf(globalTramaVenta.processingCode, "00%02d00", tipoCuentaAux);
            } else {
                sprintf(globalTramaVenta.tipoCuenta, "%02d", tipoCuentaAux);
                sprintf(globalTramaVenta.processingCode, "00%02d00", tipoCuentaAux);
            }
        } else if (atoi(globalTramaVenta.tipoTransaccion) == TRANSACCION_RETIRO_OTP) {
            sprintf(globalTramaVenta.tipoCuenta, "%02d", tipoCuentaAux);
            sprintf(globalTramaVenta.processingCode, "01%02d00", tipoCuentaAux);
        } else {
            strcpy(globalTramaVenta.processingCode, "000000");
        }
    }

    do {
        LOGI("armar la trama ");
        armarTramaDineroElectronico(pinblock, ENVIO_PAGO_ELECTRONICO, intentos, intentosPin);
        respuesta = procesarTransaccion(ENVIO_PAGO_ELECTRONICO);
        LOGI("Proceso dinero electronico ");
        if (strncmp(fiidOTP, "7999", 4) == 0) {
            /// El pan
            for (idx = 0; idx < strlen(globalTramaVenta.track2); idx++) {
                if (globalTramaVenta.track2[idx] == 'D' || globalTramaVenta.track2[idx] == 'd') {
                    break;
                }
            }
            iniUlt4 = idx - 4;
            memset(globalTramaVenta.ultimosCuatro, 0x00, sizeof(globalTramaVenta.ultimosCuatro));
            memcpy(globalTramaVenta.ultimosCuatro, globalTramaVenta.track2 + iniUlt4, 4);
        }


        if (respuesta == 2) {

            intentos++;
            if (intentos == MAX_INTENTOS_VENTA) {
                //screenMessage("COMUNICACION", "REINTENTE", "TRANSACCION", "", 2 * 1000);
                respuesta = 0;
            }
        }

    } while (respuesta == 2 && intentos < MAX_INTENTOS_VENTA);

    if (bit3Prendido != TRUE || bit4Prendido != TRUE) {
        procesarDatosTarjeta(&leerTrack);
    }

    return respuesta;
}

int procesarTransaccion(int tipoTransaccion) {

    ResultISOPack resultadoIsoPackMessage;
    uChar dataRecibida[1024 + 1];
    int resultadoTransaccion = -1;
    int generaReverso = 0;
    int sizeData = 0;

    memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
    memset(dataRecibida, 0x00, sizeof(dataRecibida));

    resultadoIsoPackMessage = packISOMessage();

    if (tipoTransaccion == ENVIO_PAGO_ELECTRONICO ||
        tipoTransaccion == ENVIO_ANULACION_PAGO_ELECTRONICO) {
        generaReverso = REVERSO_GENERADO;
    }

    if (resultadoIsoPackMessage.responseCode > 0) {

        resultadoTransaccion = realizarTransaccion(resultadoIsoPackMessage.isoPackMessage,
                                                   resultadoIsoPackMessage.totalBytes,
                                                   dataRecibida,
                                                   atoi(globalTramaVenta.tipoTransaccion),
                                                   CANAL_DEMANDA,
                                                   generaReverso);
        LOGI("resultadoTransaccion %d ", resultadoTransaccion);

        if (resultadoTransaccion > 0) {
            sizeData = resultadoTransaccion;

            resultadoTransaccion = unpackRespuestaDineroElectronico(tipoTransaccion, dataRecibida,
                                                                    sizeData);

        } else {
            resultadoTransaccion = 0;
        }
    }

    return resultadoTransaccion;
}

int unpackRespuestaDineroElectronico(int tipoTransaccion, char *dataRecibida, int sizeData) {

    ISOFieldMessage isoFieldMessage;
    uChar codigoRespuesta[2 + 1];
    char Track2[TAM_TRACK2 + 1];
    char codigoAprobacion[6 + 1];
    char mensajeError[20 + 1] = {0x00};
    int resultado = 1;
    int tipoCodigoRespuesta = 0;
    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(&codigoRespuesta, 0x00, sizeof(codigoRespuesta));
    memset(Track2, 0x00, sizeof(Track2));
    memset(codigoAprobacion, 0x00, sizeof(codigoAprobacion));
    LOGI("unpack ");

    unpackISOMessage(dataRecibida, sizeData);

    isoFieldMessage = getField(35);
    memcpy(Track2, isoFieldMessage.valueField, isoFieldMessage.totalBytes);
    memcpy(globalTramaVenta.track2, Track2, isoFieldMessage.totalBytes);

    isoFieldMessage = getField(38);
    memcpy(codigoAprobacion, isoFieldMessage.valueField, isoFieldMessage.totalBytes);
    memcpy(globalTramaVenta.codigoAprobacion, codigoAprobacion, 6);

    isoFieldMessage = getField(39);
    memcpy(codigoRespuesta, isoFieldMessage.valueField, isoFieldMessage.totalBytes);
    memcpy(globalTramaVenta.codigoRespuesta, codigoRespuesta, 2);
    LOGI("respuesta   %s", globalTramaVenta.codigoRespuesta);
    if (strcmp(codigoRespuesta, "00") == 0) {

        if (tipoTransaccion == ENVIO_CONSULTA_PAGO_ELECTRONICO_CLESS) {
            memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
            isoFieldMessage = getField(4);
            memset(globalTramaVenta.totalVenta, 0x00, sizeof(globalTramaVenta.totalVenta));
            memcpy(globalTramaVenta.totalVenta, isoFieldMessage.valueField,
                   isoFieldMessage.totalBytes);
        }

        if (atoi(globalTramaVenta.tipoTransaccion) != TRANSACCION_CONSULTA_PAGO_ELECTRONICO
            && tipoTransaccion != ENVIO_CONSULTA_PAGO_ELECTRONICO_CLESS) {
            LOGI("verificando llegada trama ");
            resultado = verificacionLlegadaTrama(&globalTramaVenta);
            LOGI("Sale  ");
        }

        if (resultado < 1) {
            resultado = -1;

            if (tipoTransaccion == ENVIO_PAGO_ELECTRONICO ||
                tipoTransaccion == ENVIO_ANULACION_PAGO_ELECTRONICO) {
                resultado = generarReverso();

                if (resultado == 1) {
                    resultado = 2; // Volver a enviar la anulacion
                }
            }
        }
    } else {

        borrarArchivo(discoNetcom, ARCHIVO_REVERSO);
        tipoCodigoRespuesta = atoi(codigoRespuesta);
        LOGI("tipoCodigoRespuesta   %d", tipoCodigoRespuesta);
        switch (tipoCodigoRespuesta) {
            case 16:
            case 11:
                //screenMessage("MENSAJE", "CLAVE INVALIDA", "GENERE UNA NUEVA", "CLAVE", 2 * 1000);
                break;
            case 66:
            case 97:
                //screenMessage("MENSAJE", "", "CEDULA INVALIDA", "", 2 * 1000);
                break;
            case 14:
                //screenMessage("MENSAJE", "#MOVIL / MONEDERO", "INVALIDO", "", 2 * 1000);
                break;
            case 61:
                //screenMessage("MENSAJE", "EXCEDE MONTO ", "COMPRA", "", 2 * 1000);
                break;
            case 55:
                //screenMessage("MENSAJE", " ", "CLAVE ERRADA", "", 2 * 1000);
                break;
            default:
                if (strcmp(codigoRespuesta, "O1") == 0) {
                    //screenMessage("MENSAJE", "USUARIO", "NO", "EXISTE", 2 * 1000);
                } else if (strcmp(codigoRespuesta, "O2") == 0) {
                    //screenMessage("MENSAJE", "OTP", "USADO", "", 2 * 1000);
                } else if (strcmp(codigoRespuesta, "O3") == 0) {
                    //screenMessage("MENSAJE", "OTP", "NO", "EXISTE", 2 * 1000);
                } else if (strcmp(codigoRespuesta, "O4") == 0) {
                    //screenMessage("MENSAJE", "OTP", "EXPIRADO", "", 2 * 1000);
                } else if (strcmp(codigoRespuesta, "O5") == 0) {
                    //screenMessage("MENSAJE", "OTP", "INVALIDO", "", 2 * 1000);
                } else {
                    errorRespuestaTransaccion(codigoRespuesta, mensajeError);
                }
                break;
        }
        LOGI("error   %d", mensajeError);
        resultado = -1;
        if (strcmp(codigoRespuesta, "55") == 0) {
            resultado = PEDIR_NUEVAMENTE_PIN;
        } else if (strcmp(codigoRespuesta, "75") == 0) {
            resultado = MAXIMO_INTENTOS_PIN;
        }
    }

    return resultado;
}

void armarTramaDineroElectronico(char *pinblock, int transaccionDineroElectronico, int intentos,
                                 int intentosPin) {

    ISOHeader isoHeader8583;
    TablaUnoInicializacion tablaUno;
    DatosEmpaquetados datos;
    DatosIniQrCode datosOTP;
    uChar nii[TAM_NII + 1];
    uChar p59[34 + 1];
    uChar tipoTransaccion[TAM_TIPO_TRANSACCION + 1];
    uChar field47[36 + 1]; /// iva + base  + inc
    uChar variableAux[36 + 1];
    uChar propina[12 + 1];
    uChar terminal[8 + 1];
    uChar bufferAuxiliar[36 + 1];
    uChar variableAux2[TAM_FIELD_42 + 1];
    uChar stringAux[20 + 1];
    uChar niiBiOtp[TAM_NII + 1];
    uChar fiidOTP[4 + 1];
    uChar codigoComercio[TAM_ID_COMERCIO + 1];
    int tamano = 0;
//	int documento = 0;

    memset(&isoHeader8583, 0x00, sizeof(isoHeader8583));
    memset(&datos, 0x00, sizeof(datos));
    memset(nii, 0x00, sizeof(nii));
    memset(p59, 0x30, sizeof(p59));
    memset(tipoTransaccion, 0x00, sizeof(tipoTransaccion));
    memset(field47, 0x00, sizeof(field47));
    memset(variableAux, 0x00, sizeof(variableAux));
    memset(propina, 0x00, sizeof(propina));
    memset(&tablaUno, 0x00, sizeof(tablaUno));
    memset(terminal, 0x00, sizeof(terminal));
    memset(bufferAuxiliar, 0x00, sizeof(bufferAuxiliar));
    memset(variableAux2, 0x00, sizeof(variableAux2));
    memset(stringAux, 0x00, sizeof(stringAux));
    memset(&datosOTP, 0x00, sizeof(&datosOTP));
    memset(niiBiOtp, 0x00, sizeof(niiBiOtp));
    memset(fiidOTP, 0x00, sizeof(fiidOTP));
    memset(codigoComercio, 0x00, sizeof(codigoComercio));
    LOGI("armar la trama ");
    tablaUno = _desempaquetarTablaCeroUno_();
    datosOTP = datoInteroperabilidad();
    LOGI("saco datos otp");
    getParameter(NII, nii);
    getParameter(NUMERO_TERMINAL, terminal);
    getParameter(CODIGO_COMERCIO, codigoComercio);
    _convertBCDToASCII_(fiidOTP, globalTramaVenta.field61, 4);
    memcpy(globalTramaVenta.terminalId, terminal, 8);
    LOGI("terminal %s", globalTramaVenta.terminalId);
    memcpy(globalTramaVenta.posEntrymode, "021", 3); ///011

    memset(globalTramaVenta.numeroRecibo, 0x00, sizeof(globalTramaVenta.numeroRecibo));
    generarInvoice(globalTramaVenta.numeroRecibo);
    LOGI("recibo %s", globalTramaVenta.numeroRecibo);

    memset(globalTramaVenta.systemTrace, 0x00, sizeof(globalTramaVenta.systemTrace));
    generarSystemTraceNumber(globalTramaVenta.systemTrace, "");

    if (intentos == 0 && intentosPin == 0
        && ((transaccionDineroElectronico == ENVIO_SOLICITUD_PAGO_ELECTRONICO) ||
            (bit4Prendido == TRUE))) {
        if (intentos <= 0) {

            strcat(globalTramaVenta.totalVenta, "00");
            strcat(globalTramaVenta.compraNeta, "00");

            sprintf(variableAux, "%010ld00", atol(globalTramaVenta.iva));

            if (strlen(globalTramaVenta.iva) > 0) {
                strcat(globalTramaVenta.iva, "00");
            }

            sprintf(variableAux + 12, "%010ld00", atol(globalTramaVenta.baseDevolucion));

            if (strlen(globalTramaVenta.baseDevolucion) > 0) {
                strcat(globalTramaVenta.baseDevolucion, "00");
            }

            sprintf(variableAux + 24, "%010ld00", atol(globalTramaVenta.inc));

            if (strlen(globalTramaVenta.inc) > 0) {
                strcat(globalTramaVenta.inc, "00");
            }

            sprintf(propina, "%010ld00", atol(globalTramaVenta.propina));

            if (strlen(globalTramaVenta.propina) > 0) {
                strcat(globalTramaVenta.propina, "00");
            }
        }
    } else {
        if ((intentos <= 0) && (intentosPin == 0)) {

            memset(bufferAuxiliar, 0x00, sizeof(bufferAuxiliar));
            strcat(globalTramaVenta.totalVenta, "00");
            strcat(globalTramaVenta.compraNeta, "00");
            if (strlen(globalTramaVenta.iva) > 0) {
                if (atoi(globalTramaVenta.tipoTransaccion) !=
                    TRANSACCION_CONSULTA_PAGO_ELECTRONICO) {
                    strcat(globalTramaVenta.iva, "00");
                }
                memcpy(bufferAuxiliar, globalTramaVenta.iva, strlen(globalTramaVenta.iva) - 2);
            }

            sprintf(variableAux, "%010ld00", atol(bufferAuxiliar));
            memset(bufferAuxiliar, 0x00, sizeof(bufferAuxiliar));

            if (strlen(globalTramaVenta.baseDevolucion) > 0) {
                if (atoi(globalTramaVenta.tipoTransaccion) !=
                    TRANSACCION_CONSULTA_PAGO_ELECTRONICO) {
                    strcat(globalTramaVenta.baseDevolucion, "00");
                }
                memcpy(bufferAuxiliar, globalTramaVenta.baseDevolucion,
                       strlen(globalTramaVenta.baseDevolucion) - 2);
            }

            sprintf(variableAux + 12, "%010ld00", atol(bufferAuxiliar));
            memset(bufferAuxiliar, 0x00, sizeof(bufferAuxiliar));

            if (strlen(globalTramaVenta.inc) > 0) {
                if (atoi(globalTramaVenta.tipoTransaccion) !=
                    TRANSACCION_CONSULTA_PAGO_ELECTRONICO) {
                    strcat(globalTramaVenta.inc, "00");
                }
                memcpy(bufferAuxiliar, globalTramaVenta.inc, strlen(globalTramaVenta.inc) - 2);
            }

            sprintf(variableAux + 24, "%010ld00", atol(bufferAuxiliar));
            memset(bufferAuxiliar, 0x00, sizeof(bufferAuxiliar));

            if (strlen(globalTramaVenta.propina) > 0) {
                if (atoi(globalTramaVenta.tipoTransaccion) !=
                    TRANSACCION_CONSULTA_PAGO_ELECTRONICO) {
                    strcat(globalTramaVenta.propina, "00");
                }
                memcpy(bufferAuxiliar, globalTramaVenta.propina,
                       strlen(globalTramaVenta.propina) - 2);
            }

            sprintf(propina, "%010ld00", atol(bufferAuxiliar));
        } else {
            memset(bufferAuxiliar, 0x00, sizeof(bufferAuxiliar));
            if (strlen(globalTramaVenta.iva) > 0) {
                memcpy(bufferAuxiliar, globalTramaVenta.iva, strlen(globalTramaVenta.iva) - 2);
            }

            sprintf(variableAux, "%010ld00", atol(bufferAuxiliar));
            memset(bufferAuxiliar, 0x00, sizeof(bufferAuxiliar));

            if (strlen(globalTramaVenta.baseDevolucion) > 0) {
                memcpy(bufferAuxiliar, globalTramaVenta.baseDevolucion,
                       strlen(globalTramaVenta.baseDevolucion) - 2);
            }
            sprintf(variableAux + 12, "%010ld00", atol(bufferAuxiliar));
            memset(bufferAuxiliar, 0x00, sizeof(bufferAuxiliar));

            if (strlen(globalTramaVenta.inc) > 0) {
                memcpy(bufferAuxiliar, globalTramaVenta.inc, strlen(globalTramaVenta.inc) - 2);
            }

            sprintf(variableAux + 24, "%010ld00", atol(bufferAuxiliar));
            memset(bufferAuxiliar, 0x00, sizeof(bufferAuxiliar));

            if (strlen(globalTramaVenta.propina) > 0) {
                memcpy(bufferAuxiliar, globalTramaVenta.propina,
                       strlen(globalTramaVenta.propina) - 2);
            }
            sprintf(propina, "%010ld00", atol(bufferAuxiliar));
        }
    }

    if (transaccionDineroElectronico == ENVIO_SOLICITUD_PAGO_ELECTRONICO) {
        memcpy(globalTramaVenta.msgType, "0100", TAM_MTI);
        sprintf(tipoTransaccion, "%d", TRANSACCION_CONSULTA_PAGO_ELECTRONICO);
        leftPad(globalTramaVenta.tipoTransaccion, tipoTransaccion, 0x30, TAM_TIPO_TRANSACCION);
    } else if (bit3Prendido == TRUE) {
        memcpy(globalTramaVenta.msgType, "0200", TAM_MTI);
        sprintf(tipoTransaccion, "%d", TRANSACCION_PAGO_ELECTRONICO_ESPECIAL);
        leftPad(globalTramaVenta.tipoTransaccion, tipoTransaccion, 0x30, TAM_TIPO_TRANSACCION);

        if ((intentos <= 0) && (intentosPin == 0)) {

            memcpy(stringAux, globalTramaVenta.totalVenta, strlen(globalTramaVenta.totalVenta) - 2);
            memset(globalTramaVenta.totalVenta, 0x00, strlen(globalTramaVenta.totalVenta));
            memcpy(globalTramaVenta.totalVenta, stringAux, strlen(stringAux));
            memcpy(stringAux, globalTramaVenta.compraNeta, strlen(globalTramaVenta.compraNeta) - 2);
            memset(globalTramaVenta.compraNeta, 0x00, strlen(globalTramaVenta.compraNeta));
            memcpy(globalTramaVenta.compraNeta, stringAux, strlen(stringAux));
        }
    } else {
        memcpy(globalTramaVenta.msgType, "0200", TAM_MTI);
        memset(globalTramaVenta.posEntrymode, 0x00, sizeof(globalTramaVenta.posEntrymode));
        memcpy(globalTramaVenta.posEntrymode, "011", 3); ///011
    }

    memcpy(globalTramaVenta.posConditionCode, "00", TAM_POS_CONDITION);
    memcpy(niiBiOtp, datosOTP.niiQr, TAM_NII + 1);

    isoHeader8583.TPDU = 60;

    if (strncmp(fiidOTP, "8999", 4) != 0 && strncmp(fiidOTP, "7999", 4) != 0) {
        memcpy(globalTramaVenta.nii, nii + 1, TAM_NII);
        memcpy(isoHeader8583.destination, nii, TAM_NII + 1);
        memcpy(isoHeader8583.source, nii, TAM_NII + 1);
    } else {
        memcpy(globalTramaVenta.nii, niiBiOtp + 1, TAM_NII);
        memcpy(isoHeader8583.destination, niiBiOtp, TAM_NII + 1);
        memcpy(isoHeader8583.source, niiBiOtp, TAM_NII + 1);
    }

    setHeader(isoHeader8583);

    setMTI(globalTramaVenta.msgType);

    setField(3, globalTramaVenta.processingCode, TAM_CODIGO_PROCESO);
    setField(4, globalTramaVenta.totalVenta, 12);
    setField(11, globalTramaVenta.systemTrace, TAM_SYSTEM_TRACE);
    setField(22, globalTramaVenta.posEntrymode, TAM_ENTRY_MODE);

    if (strncmp(fiidOTP, "8999", 4) != 0 && strncmp(fiidOTP, "7999", 4) != 0) {
        setField(24, globalTramaVenta.nii, TAM_NII);
        setField(35, globalTramaVenta.track2, strlen(globalTramaVenta.track2));
    } else {
        setField(24, niiBiOtp, TAM_NII + 1);
    }

    setField(25, globalTramaVenta.posConditionCode, TAM_POS_CONDITION);
    setField(41, globalTramaVenta.terminalId, TAM_TERMINAL_ID);

//	documento = (strlen(datosVenta.fiel42) > 0) ? 1:0;

    if (atoi(globalTramaVenta.fiel42) == 0) {
        memcpy(globalTramaVenta.fiel42, "000000000000000", 12);
    }

    if (atoi(globalTramaVenta.isQPS) == 0) {
        memcpy(globalTramaVenta.isQPS, "0", 1);
    }

    datos = armarCampo42(globalTramaVenta);

    if (banderaValidacion == 3 || bit3Prendido == TRUE
        || strncmp(fiidOTP, "8999", 4) == 0
        || strncmp(fiidOTP, "0812", 4) == 0
        || strncmp(fiidOTP, "7999", 4) == 0) {

        if (strncmp(fiidOTP, "7999", 4) != 0) {
            memset(globalTramaVenta.fiel42, 0x30, sizeof(globalTramaVenta.fiel42));
        }
        if (atoi(globalTramaVenta.tipoCuenta) != 30
            && atoi(globalTramaVenta.tipoCuenta) != 43
            && atoi(globalTramaVenta.tipoCuenta) != 45
            && atoi(globalTramaVenta.tipoCuenta) != 49
            && strncmp(fiidOTP, "7999", 4) != 0) {
            memcpy(globalTramaVenta.fiel42, "00", 2);
            memcpy(globalTramaVenta.fiel42 + 2, datos.buffer + 2, 13);
        } else {
            memcpy(globalTramaVenta.fiel42, datos.buffer, 15);
        }
        setField(42, globalTramaVenta.fiel42, TAM_FIELD_42);
    } else {
        memset(globalTramaVenta.fiel42, 0x00, sizeof(globalTramaVenta.fiel42));
    }

    memset(bufferAuxiliar, 0x00, sizeof(bufferAuxiliar));
    if (intentosPin == 0) {
        datos = armarCampo42(globalTramaVenta);
    }
    _convertASCIIToBCD_(bufferAuxiliar, variableAux, 36);

    setField(48, bufferAuxiliar, 18);

    if (transaccionDineroElectronico != ENVIO_ANULACION_PAGO_ELECTRONICO) {

        if (strncmp(fiidOTP, "8999", 4) != 0 && strncmp(fiidOTP, "7999", 4) != 0) {
            memset(globalTramaVenta.tokenVivamos, 0x00, sizeof(globalTramaVenta.tokenVivamos));
            memcpy(globalTramaVenta.tokenVivamos, pinblock, strlen(pinblock));
            setField(52, pinblock, strlen(pinblock));
        } else {
            uChar variableAuxOTP[8 + 1];
            memset(variableAuxOTP, 0x00, sizeof(variableAuxOTP));
            LOGI("pin 47 %s", globalTramaVenta.textoAdicionalInicializacion);
            tamano = strlen(globalTramaVenta.textoAdicionalInicializacion);
            _convertASCIIToBCD_(variableAuxOTP, globalTramaVenta.textoAdicionalInicializacion,
                                strlen(globalTramaVenta.textoAdicionalInicializacion));
            setField(47, variableAuxOTP, tamano / 2);
            setField(57, globalTramaVenta.acquirerId, sizeof(globalTramaVenta.acquirerId));
            setField(63, codigoComercio, strlen(codigoComercio));
        }


    }

    if (tablaUno.options1.tipProcessing == 1) {
        if (strncmp(fiidOTP, "8999", 4) != 0 && strncmp(fiidOTP, "7999", 4) != 0) {
            setField(57, propina, TAM_COMPRA_NETA);
        } else {
            setField(54, propina, TAM_COMPRA_NETA);
        }
    } else if (tablaUno.options1.tipProcessing == 1 && strncmp(fiidOTP, "8999", 4) != 0 &&
               strncmp(fiidOTP, "7999", 4) != 0) {
        setField(57, globalTramaVenta.acquirerId, sizeof(globalTramaVenta.acquirerId));
    }

    setField(62, globalTramaVenta.numeroRecibo, 6);

    if (transaccionDineroElectronico == ENVIO_SOLICITUD_PAGO_ELECTRONICO) {

        memset(p59, 0x30, sizeof(p59));
        memcpy(p59, "6666", 4);
        memcpy(p59 + 24, globalTramaVenta.acquirerId, 10);
        setField(59, p59, 34);
    }

}

void armarTramaAnulacionDineroElectronico(int intentos) {

    ISOHeader isoHeader8583;
    TablaUnoInicializacion tablaUno;
    DatosIniQrCode datosOTP;
    uChar nii[TAM_NII + 1];
    uChar p59[34 + 1];
    uChar tipoTransaccion[TAM_TIPO_TRANSACCION + 1];
    uChar field47[36 + 1]; /// iva + base  + inc
    uChar variableAux[36 + 1];
    uChar propina[12 + 1];
    uChar terminal[8 + 1];
    uChar bufferAuxiliar[36 + 1];
    uChar niiBiOtp[TAM_NII + 1];
    uChar fiidOTP[4 + 1];
    uChar codigoComercio[TAM_ID_COMERCIO + 1];
    int tamano = 0;
    char auxiliarRRN[12 + 1];
    char auxiliarField37[12 + 1];

    memset(&bufferAuxiliar, 0x00, sizeof(bufferAuxiliar));
    memset(&isoHeader8583, 0x00, sizeof(isoHeader8583));
    memset(nii, 0x00, sizeof(nii));
    memset(p59, 0x30, sizeof(p59));
    memset(tipoTransaccion, 0x00, sizeof(tipoTransaccion));
    memset(field47, 0x00, sizeof(field47));
    memset(variableAux, 0x00, sizeof(variableAux));
    memset(propina, 0x00, sizeof(propina));
    memset(&tablaUno, 0x00, sizeof(tablaUno));
    memset(terminal, 0x00, sizeof(terminal));
    memset(niiBiOtp, 0x00, sizeof(niiBiOtp));
    memset(&datosOTP, 0x00, sizeof(&datosOTP));
    memset(fiidOTP, 0x00, sizeof(fiidOTP));
    memset(codigoComercio, 0x00, sizeof(codigoComercio));
    memset(auxiliarRRN, 0x00, sizeof(auxiliarRRN));
    memset(auxiliarField37, 0x00, sizeof(auxiliarField37));

    tablaUno = _desempaquetarTablaCeroUno_();
    datosOTP = datoInteroperabilidad();
    _convertBCDToASCII_(fiidOTP, globalTramaVenta.field61, 4);

    getParameter(NII, nii);
    getParameter(NUMERO_TERMINAL, terminal);
    getParameter(CODIGO_COMERCIO, codigoComercio);

    memcpy(globalTramaVenta.terminalId, terminal, 8);
    memset(globalTramaVenta.processingCode, 0x00, sizeof(globalTramaVenta.processingCode));

    if (strncmp(fiidOTP, "8999", 4) != 0 && strncmp(fiidOTP, "0812", 4) != 0 &&
        strncmp(fiidOTP, "7999", 4) != 0) {
        memcpy(globalTramaVenta.processingCode, "204100", 6);
    } else if (strncmp(fiidOTP, "0812", 4) == 0) {
        sprintf(globalTramaVenta.processingCode, "20%02d00", atoi(globalTramaVenta.tipoCuenta));
    } else if (strncmp(fiidOTP, "8999", 4) == 0 || strncmp(fiidOTP, "7999", 4) == 0) {
        sprintf(globalTramaVenta.processingCode, "04%02d00", atoi(globalTramaVenta.tipoCuenta));
    }


    if (intentos == 0) {
        memset(globalTramaVenta.systemTrace, 0x00, sizeof(globalTramaVenta.systemTrace));
        generarSystemTraceNumber(globalTramaVenta.systemTrace, "");

    }

    sprintf(variableAux, "%012ld", atol(globalTramaVenta.iva));
    sprintf(variableAux + 12, "%012ld", atol(globalTramaVenta.baseDevolucion));
    sprintf(variableAux + 24, "%012ld", atol(globalTramaVenta.inc));
    sprintf(propina, "%012ld", atol(globalTramaVenta.propina));

    memcpy(globalTramaVenta.msgType, "0200", TAM_MTI);
    memcpy(globalTramaVenta.nii, nii + 1, TAM_NII);
    memcpy(globalTramaVenta.posConditionCode, "00", TAM_POS_CONDITION);

    if (atoi(globalTramaVenta.tipoTransaccion) == TRANSACCION_PAGO_ELECTRONICO_ESPECIAL) {
        sprintf(tipoTransaccion, "%d", TRANSACCION_ANULACION);
    }

    if (atoi(globalTramaVenta.tipoTransaccion) == TRANSACCION_PAGO_ELECTRONICO) {
        sprintf(tipoTransaccion, "%d", TRANSACCION_ANULACION);
    }

    leftPad(globalTramaVenta.tipoTransaccion, tipoTransaccion, 0x30, TAM_TIPO_TRANSACCION);
    memcpy(niiBiOtp, datosOTP.niiQr + 1, TAM_NII);

    isoHeader8583.TPDU = 60;

    if (strncmp(fiidOTP, "8999", 4) != 0 && strncmp(fiidOTP, "7999", 4) != 0) {
        memcpy(globalTramaVenta.nii, nii + 1, TAM_NII);
        memcpy(isoHeader8583.destination, nii, TAM_NII + 1);
        memcpy(isoHeader8583.source, nii, TAM_NII + 1);
    } else {
        memcpy(globalTramaVenta.nii, datosOTP.niiQr + 1, TAM_NII);
        memcpy(isoHeader8583.destination, datosOTP.niiQr, TAM_NII + 1);
        memcpy(isoHeader8583.source, datosOTP.niiQr, TAM_NII + 1);
    }


    setHeader(isoHeader8583);
    setMTI(globalTramaVenta.msgType);
    setField(3, globalTramaVenta.processingCode, TAM_CODIGO_PROCESO);
    setField(4, globalTramaVenta.totalVenta, 12);
    setField(11, globalTramaVenta.systemTrace, TAM_SYSTEM_TRACE);
    setField(22, globalTramaVenta.posEntrymode, TAM_ENTRY_MODE);
    setField(25, globalTramaVenta.posConditionCode, TAM_POS_CONDITION);

    if (strncmp(fiidOTP, "7999", 4) != 0) {
        setField(37, globalTramaVenta.rrn, 6);
    } else {
        _rightTrim_(auxiliarRRN, globalTramaVenta.rrn, 0x20);
        leftPad(auxiliarField37, auxiliarRRN, 0x30, 12);
        setField(37, auxiliarField37, 12);
    }


    if (strncmp(fiidOTP, "8999", 4) != 0 && strncmp(fiidOTP, "7999", 4) != 0) {
        setField(24, globalTramaVenta.nii, TAM_NII);
        setField(35, globalTramaVenta.track2, strlen(globalTramaVenta.track2));
    } else {
        setField(24, niiBiOtp, TAM_NII + 1);
    }

    setField(38, globalTramaVenta.codigoAprobacion, 6);
    setField(41, globalTramaVenta.terminalId, TAM_TERMINAL_ID);
    setField(42, globalTramaVenta.fiel42, strlen(globalTramaVenta.fiel42));

    _convertASCIIToBCD_(bufferAuxiliar, variableAux, 36);
    setField(48, bufferAuxiliar, 18);

    if (strncmp(fiidOTP, "8999", 4) == 0 || strncmp(fiidOTP, "7999", 4) == 0) {
        uChar variableAuxOTP[12 + 1];
        memset(variableAuxOTP, 0x00, sizeof(variableAuxOTP));
        tamano = strlen(globalTramaVenta.textoAdicionalInicializacion);
        _convertASCIIToBCD_(variableAuxOTP, globalTramaVenta.textoAdicionalInicializacion,
                            sizeof(variableAuxOTP));
        setField(47, variableAuxOTP, tamano / 2);
    }

    if (tablaUno.options1.tipProcessing == 1) {
        if (strncmp(fiidOTP, "8999", 4) != 0 && strncmp(fiidOTP, "7999", 4) != 0) {
            setField(57, propina, TAM_COMPRA_NETA);
        } else {
            setField(54, propina, TAM_COMPRA_NETA);
            setField(57, globalTramaVenta.acquirerId, sizeof(globalTramaVenta.acquirerId));
        }
    } else if (tablaUno.options1.tipProcessing != 1
               && strncmp(fiidOTP, "8999", 4) != 0
               && strncmp(fiidOTP, "0812", 4) != 0
               && strncmp(fiidOTP, "7999", 4) != 0) {
        setField(57, globalTramaVenta.acquirerId, sizeof(globalTramaVenta.acquirerId));
    }

    setField(62, globalTramaVenta.numeroRecibo, 6);

    if (strncmp(fiidOTP, "8999", 4) == 0 || strncmp(fiidOTP, "7999", 4) == 0) {
        setField(63, codigoComercio, strlen(codigoComercio));
    }
}

void mostrarAprobacion(int tipoTransaccion) {
    LOGI("Aprobado tx %s", globalTramaVenta.codigoAprobacion);
    ISOFieldMessage isoFieldMessage;

    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));

    /// BORRAR ARCHIVO REVERSO ///
    borrarArchivo(discoNetcom, ARCHIVO_REVERSO);
    isoFieldMessage = getField(12);
    memcpy(globalTramaVenta.txnTime, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

    isoFieldMessage = getField(13);
    memcpy(globalTramaVenta.txnDate, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

    isoFieldMessage = getField(37);
    memcpy(globalTramaVenta.rrn, isoFieldMessage.valueField + 6, 6);

    isoFieldMessage = getField(38);
    memcpy(globalTramaVenta.codigoAprobacion, isoFieldMessage.valueField,
           isoFieldMessage.totalBytes);
    LOGI("guardando tx %s", globalTramaVenta.codigoAprobacion);
    if (tipoTransaccion != ENVIO_ANULACION_PAGO_ELECTRONICO) {
        sprintf(globalTramaVenta.estadoTransaccion, "%d", TRANSACCION_ACTIVA);
        _guardarTransaccion_(globalTramaVenta);
    }
    //screenMessage("CODIGO APROBACION", datosVenta.codigoAprobacion, "NUMERO DE APROBACION", "", 0);
}

int anulacionDineroElectronico(DatosVenta anulacion) {

    int respuesta = 0;
    int intentos = 0;
    int intentosPin = 0;

    unsigned char terminal[64 + 1];
    uChar pin[12 + 1];
    uChar fiidOTP[4 + 1];

    memset(pin, 0x00, sizeof(pin));
    memset(&globalTramaVenta, 0x00, sizeof(globalTramaVenta));
    memset(fiidOTP, 0x00, sizeof(fiidOTP));
    memset(terminal, 0x00, sizeof(terminal));

    globalTramaVenta = anulacion;

    _convertBCDToASCII_(fiidOTP, globalTramaVenta.field61, 4);

    respuesta = verificarAnulacion(globalTramaVenta.estadoTransaccion, globalTramaVenta.totalVenta,
                                   globalTramaVenta.numeroRecibo);

    if (respuesta > 0) {

        do {

            if (strncmp(fiidOTP, "8999", 4) == 0 || strncmp(fiidOTP, "7999", 4) == 0) {

                do {
                    //respuesta = getStringKeyboard("MENSAJE", "INGRESE SU", "PIN OTP", 12, pin, DATO_PASSWORD);
                    if (respuesta > 0) {

                        if ((IS_IMPAR(strlen(pin)) == 1)) {
                            strcat(pin, "F");
                        }

                        memset(globalTramaVenta.textoAdicionalInicializacion, 0x00,
                               sizeof(globalTramaVenta.textoAdicionalInicializacion));
                        memcpy(globalTramaVenta.textoAdicionalInicializacion, pin, strlen(pin));

                        if (strlen(pin) < 4 && respuesta > 0) {
                            //screenMessage("MENSAJE", "DATO INVALIDO", "MINIMO DE 4", "CARACTERES", 2 * 1000);
                            respuesta = -2;
                            intentosPin++;
                            if (intentosPin == 3) {
                                //screenMessage("MENSAJE", "EXCEDE", "MAXIMO NUMERO ", "DE INTENTOS", 2 * 1000);
                            }
                        } else {
                            intentosPin = 3;
                        }
                    } else {
                        respuesta = -2;
                        intentosPin = 3;
                    }
                } while (intentosPin != 3);
            }
            if (respuesta != -2) {
                armarTramaAnulacionDineroElectronico(intentos);
                respuesta = procesarTransaccion(ENVIO_ANULACION_PAGO_ELECTRONICO);

                if (respuesta == 2) {

                    intentos++;

                    if (intentos == MAX_INTENTOS_VENTA) {
                        //screenMessage("COMUNICACION", "REINTENTE", "TRANSACCION", "", 2 * 1000);
                        respuesta = 0;
                    } else if (respuesta == -4) {
                        respuesta = 2;
                        intentos = 0;
                    }
                }
            } else {
                respuesta = 0;
                intentos = MAX_INTENTOS_VENTA;
            }
        } while (respuesta == 2 && intentos < MAX_INTENTOS_VENTA);

    }

    if (respuesta > 0) {
        cambiarEstadoTransaccion(anulacion.numeroRecibo, globalTramaVenta.rrn,
                                 globalTramaVenta.codigoAprobacion,
                                 globalTramaVenta.txnDate, globalTramaVenta.txnTime);
        strcpy(globalTramaVenta.estadoTransaccion, "0");

//		guardarTransaccion(datosVenta);
        mostrarAprobacion(ENVIO_ANULACION_PAGO_ELECTRONICO);
        realizarImpresion(6);

        if (verificarHabilitacionCaja() == TRUE) {
            memcpy(globalTramaVenta.codigoRespuesta, "00", 2);
            setParameter(USO_CAJA_REGISTRADORA, "1");
            //transaccionRespuestaDatosCaja(datosVenta, TRANSACCION_TEFF_ANULACIONES);
        }
    } else {
        if (verificarHabilitacionCaja() == TRUE) {
            memcpy(globalTramaVenta.codigoRespuesta, "01", 2);
            setParameter(USO_CAJA_REGISTRADORA, "1");
            //transaccionRespuestaDatosCaja(datosVenta, TRANSACCION_TEFF_ANULACIONES);
        }
    }

    return respuesta;

}

int verificarAnulacion(char *estadoTransaccion, char *totalVenta, char *numeroRecibo) {

    char linea1[40 + 1];
    char linea2[40 + 1];
    char monto[15 + 1];
    char monto2[15 + 1];
    int respuesta = 0;

    memset(linea1, 0x00, sizeof(linea1));
    memset(linea2, 0x00, sizeof(linea2));
    memset(monto, 0x00, sizeof(monto));
    memset(monto2, 0x00, sizeof(monto2));

    if (atoi(estadoTransaccion) == TRUE) {

        memcpy(monto, totalVenta, strlen(totalVenta) - 2);
        formatString(0x00, 0, monto, strlen(monto), monto2, 1);
        sprintf(linea1, "RECIBO : %s", numeroRecibo);
        sprintf(linea2, "VALOR  : %s", monto2);

        // respuesta = getchAcceptCancel("ANULACION", linea1, linea2, "Desea Continuar?", 30 * 1000);

    } else {
        //screenMessage("ANULACION", "RECIBO YA ANULADO", "CANCELANDO", "LA TRANSACCION", 2 * 1000);
    }
    return respuesta;
}

void realizarImpresion(int duplicado) {

    int resultado = 0;

    imprimirTicketDineroElectronico(globalTramaVenta, RECIBO_COMERCIO, duplicado);

}

void imprimirTicketDineroElectronico(DatosVenta reciboVenta, int tipoTicket, int duplicado) {

    uChar parMensajePie[250 + 1];
    uChar parPie[40 + 1];
    uChar valorFormato[20 + 1];
    uChar aprob[15 + 1];
    uChar rrnVenta[20 + 1];
    uChar parDuplicado[30 + 1];
    uChar parVersion[20 + 1];
    uChar parVenta[20 + 1];
    uChar parBiOTP[20 + 1];
    uChar textoComercio[30 + 1];
    uChar parTipoRecibo[40 + 1];
    uChar parFactura[20 + 1];
    uChar stringAux[20 + 1];
    uChar parInformacion[TAM_TEXTO_INFORMACION + 1];
    uChar parFirma[50 + 1];
    uChar parCedula[50 + 1];
    uChar parTel[50 + 1];
    uChar parDian[34 + 1];
    uChar parSaludo[35 + 1];
    uChar parTextoCompraNeta[20 + 1];
    uChar parCompraNeta[20 + 1];
    uChar parTextoIva[20 + 1];
    uChar parTextoInc[20 + 1];
    uChar parIva[20 + 1];
    uChar parInc[20 + 1];
    uChar parTextoPropina[20 + 1];
    uChar parPropina[20 + 1];
    uChar parTextoTotal[20 + 1];
    uChar parTextoBase[35 + 1];
    uChar parLogo[35 + 1];
    uChar parCosto[50 + 1];
    uChar cardName[15 + 1];
    uChar parBase[20 + 1];
    uChar totalAnulacion[20 + 1];
    long valor = 0;
    uChar tipoCuenta[12 + 1];
    uChar hora[10 + 1];
    uChar cuotas[9 + 1];
    uChar fechaExpiracion[20 + 1];
    uChar bin[9 + 1];
    uChar parcodigoCajero[12 + 1];
    uChar codigoCajeroVacio[TAM_CODIGO_CAJERO + 1];
    uChar auxiliar[20 + 1];
    uChar fiidOTP[4 + 1];

    int isColsubsidio = 0;
    int isPayPass = 0;
    int existeEmisor = 0;

    TablaUnoInicializacion tablaUno;
    EmisorAdicional emisorAdicional;

    memset(bin, 0x00, sizeof(bin));
    memset(parInformacion, 0x00, sizeof(parInformacion));
    memset(stringAux, 0x00, sizeof(stringAux));
    memset(parFactura, 0x00, sizeof(parFactura));
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
    memset(parBase, 0x00, sizeof(parBase));
    memset(cuotas, 0x00, sizeof(cuotas));
    memset(tipoCuenta, 0x00, sizeof(tipoCuenta));
    memset(parTextoTotal, 0x00, sizeof(parTextoTotal));
    memset(parLogo, 0x00, sizeof(parLogo));
    memset(parCosto, 0x00, sizeof(parCosto));
    memset(parMensajePie, 0x00, sizeof(parMensajePie));
    memset(parPie, 0x00, sizeof(parPie));
    memset(aprob, 0x00, sizeof(aprob));
    memset(rrnVenta, 0x00, sizeof(rrnVenta));
    memset(valorFormato, 0x00, sizeof(valorFormato));
    memset(parDuplicado, 0x00, sizeof(parDuplicado));
    memset(parVersion, 0x00, sizeof(parVersion));
    memset(parVenta, 0x00, sizeof(parVenta));
    memset(parBiOTP, 0x00, sizeof(parBiOTP));
    memset(textoComercio, 0x00, sizeof(textoComercio));
    memset(parTipoRecibo, 0x00, sizeof(parTipoRecibo));
    memset(parFirma, 0x00, sizeof(parFirma));
    memset(parCedula, 0x00, sizeof(parCedula));
    memset(parTel, 0x00, sizeof(parTel));
    memset(parDian, 0x00, sizeof(parDian));
    memset(parSaludo, 0x00, sizeof(parSaludo));
    memset(cardName, 0x00, sizeof(cardName));
    memset(totalAnulacion, 0x00, sizeof(totalAnulacion));
    memset(fechaExpiracion, 0x00, sizeof(fechaExpiracion));
    memset(parcodigoCajero, 0x00, sizeof(parcodigoCajero));
    memset(codigoCajeroVacio, 0x00, sizeof(codigoCajeroVacio));
    memset(auxiliar, 0x00, sizeof(auxiliar));
    strcpy(parCosto, NO_IMPRIMIR);
    strcpy(parDian, "*SUJETO A VERIFICACION POR LA DIAN");
    strcpy(parSaludo, NO_IMPRIMIR);
    memset(fiidOTP, 0x00, sizeof(fiidOTP));

    _convertBCDToASCII_(fiidOTP, reciboVenta.field61, 4);

    memcpy(bin, reciboVenta.track2, 9);
    sprintf(parLogo, "%s%s", "0000", ".BMP");
    existeEmisor = buscarBinEmisor(bin);

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

    if (strncmp(fiidOTP, "8999", 4) != 0) {
        sprintf(parBiOTP, "%-16s", "BILLETERA OTP");
    } else {
        strcpy(parBiOTP, NO_IMPRIMIR);
    }

    if (atoi(reciboVenta.tipoTransaccion) == TRANSACCION_PAGO_ELECTRONICO) {
        sprintf(parTextoCompraNeta, "%-11s", "COMPRA NETA");
    } else {
        sprintf(parTextoCompraNeta, "%-11s", "VALOR NETO");
    }
    sprintf(parTextoPropina, "%-11s", "PROPINA");
    sprintf(parTextoTotal, "%-11s", "TOTAL");
    sprintf(parTextoBase, "%-30s", "BASE DE DEVOLUCION IVA");
    sprintf(cuotas, "%7s", " ");

    memset(tipoCuenta, 0x00, sizeof(tipoCuenta));
    memset(hora, 0x00, sizeof(hora));

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
    strcat(numeroRecibo, reciboVenta.numeroRecibo);

    /// TOTAL VENTA ///
    uChar totalVenta[20 + 1];
    memset(totalVenta, 0x00, sizeof(totalVenta));

    memcpy(valorFormato, reciboVenta.totalVenta, strlen(reciboVenta.totalVenta) - 2);

    valor = atol(valorFormato);
    memset(valorFormato, 0x00, sizeof(valorFormato));
    formatString(0x00, valor, "", 0, valorFormato, 1);
    sprintf(totalVenta, "%12s", valorFormato);

    strcpy(aprob, "\x0F APRO: ");
    strcat(aprob, reciboVenta.codigoAprobacion);

    /// IMPRIME RRN  ///
    sprintf(rrnVenta, "%s", reciboVenta.rrn);

    /// ULTIMOS CUATRO ///
    uChar ultimosCuatro[6 + 1];
    memset(ultimosCuatro, 0x00, sizeof(ultimosCuatro));
    strcpy(ultimosCuatro, "**");
    strcat(ultimosCuatro, reciboVenta.ultimosCuatro);

    /// FECHA ///
    formatoFecha(reciboVenta.txnDate);

    /// HORA ///
    sprintf(hora, "%.2s:%.2s:%.2s", reciboVenta.txnTime, reciboVenta.txnTime + 2,
            reciboVenta.txnTime + 4);

    switch (atoi(reciboVenta.tipoCuenta)) {
        case 0: // PAY PASS
            if ((strcmp(reciboVenta.cardName, "MASTERCARD") != 0) && isPayPass == 1) {
                //			strcpy(textoCredito,NO_IMPRIMIR);
            }

            if (strlen(reciboVenta.cuotas) > 0 && atoi(reciboVenta.cuotas) > 0) {
                sprintf(cuotas, "CUO: %2s", reciboVenta.cuotas);
            } else {
                strcpy(cuotas, "       ");
            }
            break;
        case 10:
            sprintf(tipoCuenta, "%7s", "Ah");
            break;
        case 20:
            sprintf(tipoCuenta, "%7s", "Cc");
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
            sprintf(fechaExpiracion, "%s", "               ");
            break;
        case 41:
            sprintf(tipoCuenta, "%7s", "LT");
            break;
        case 42:
            sprintf(tipoCuenta, "%7s", "CM");
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
            sprintf(fechaExpiracion, "EXP: %-10s", reciboVenta.fechaExpiracion);
            break;
        case 44:
            sprintf(tipoCuenta, "%7s", "BE");
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
            sprintf(fechaExpiracion, "EXP: %-10s", reciboVenta.fechaExpiracion);
            break;
        case 46:
            sprintf(tipoCuenta, "%7s", "RG");
            break;
        case 47:
            sprintf(tipoCuenta, "%7s", "DS");
            break;

        case 48:
            sprintf(tipoCuenta, "%7s", "DB");
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
            sprintf(fechaExpiracion, "EXP: %-10s", reciboVenta.fechaExpiracion);
            break;
        default:
            break;
    }

    memset(valorFormato, 0x00, sizeof(valorFormato));
    formatString(0x00, 0, reciboVenta.totalVenta, strlen(reciboVenta.totalVenta) - 2,
                 valorFormato, 1);
    sprintf(parCompraNeta, "%12s", valorFormato);

    if (strlen(reciboVenta.textoInformacion) > 0) {
        strcpy(parInformacion, reciboVenta.textoInformacion);
    } else {
        strcpy(parInformacion, NO_IMPRIMIR);
    }

    memset(valorFormato, 0x00, sizeof(valorFormato));
    formatString(0x00, 0, reciboVenta.compraNeta, strlen(reciboVenta.compraNeta) - 2,
                 valorFormato, 1);
    sprintf(parCompraNeta, "%12s", valorFormato);
    memset(stringAux, 0x00, sizeof(stringAux));

    if (strlen(reciboVenta.iva) >= 2) {
        memcpy(stringAux, reciboVenta.iva, strlen(reciboVenta.iva) - 2);
        memset(valorFormato, 0x00, sizeof(valorFormato));
        formatString(0x00, 0, stringAux, strlen(stringAux), valorFormato, 1);
        sprintf(parTextoIva, "%-11s", "IVA");
        sprintf(parIva, "%12s", valorFormato);
    } else {
        sprintf(parTextoIva, "%s", NO_IMPRIMIR);
        strcpy(parIva, NO_IMPRIMIR);
    }

    memset(stringAux, 0x00, sizeof(stringAux));
    if (strlen(reciboVenta.inc) >= 2) {
        memcpy(stringAux, reciboVenta.inc, strlen(reciboVenta.inc) - 2);
        memset(valorFormato, 0x00, sizeof(valorFormato));
        formatString(0x00, 0, stringAux, strlen(stringAux), valorFormato, 1);
        sprintf(parTextoInc, "%-11s", "INC");
        sprintf(parInc, "%12s", valorFormato);
    } else {
        sprintf(parTextoInc, "%s", NO_IMPRIMIR);
        strcpy(parInc, NO_IMPRIMIR);
    }

    if (tablaUno.options1.tipProcessing == 1) {

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

    if (strlen(reciboVenta.textoInformacion) > 0) {
        strcpy(parInformacion, reciboVenta.textoInformacion);
    } else {
        strcpy(parInformacion, NO_IMPRIMIR);
    }

    memset(stringAux, 0x00, sizeof(stringAux));
    if (strlen(reciboVenta.baseDevolucion) >= 2) {
        memcpy(stringAux, reciboVenta.baseDevolucion, strlen(reciboVenta.baseDevolucion) - 2);
    }

    memset(valorFormato, 0x00, sizeof(valorFormato));
    formatString(0x00, 0, stringAux, strlen(stringAux), valorFormato, 1);
    sprintf(parBase, "%16s*", valorFormato);

    if (tipoTicket == RECIBO_COMERCIO) {

        strcpy(parPie, "*** COMERCIO ***");
        strcpy(parFirma, "FIRMA: _____________________________________");
        strcpy(parCedula, "C.C:   _____________________________________");
        strcpy(parTel, "TEL:   _____________________________________");
    } else if (tipoTicket == RECIBO_CLIENTE) {

        strcpy(parPie, "*** CLIENTE ***");
        strcpy(parFirma, NO_IMPRIMIR);
        strcpy(parCedula, NO_IMPRIMIR);
        strcpy(parTel, NO_IMPRIMIR);
    }

    if (duplicado == RECIBO_DUPLICADO && reciboVenta.estadoTransaccion[0] == '1') {
        memset(parDuplicado, 0x00, sizeof(parDuplicado));
        strcpy(parDuplicado, "     ** DUPLICADO **");
        _convertBCDToASCII_(fiidOTP, reciboVenta.field61, 4);
        if (strncmp(fiidOTP, "8999", 4) != 0) {
            if ((atoi(reciboVenta.tipoTransaccion)
                 == TRANSACCION_PAGO_ELECTRONICO)
                || (atoi(reciboVenta.tipoTransaccion)
                    == TRANSACCION_PAGO_ELECTRONICO_ESPECIAL)) {
                sprintf(parVenta, "%-16s", "COMPRA DINERO ELEC.");
            } else {
                sprintf(parVenta, "%-16s", "RETIRO DINERO ELEC.");
            }
        } else {
            sprintf(parBiOTP, "%-16s", "BILLETERA OTP");
            sprintf(parVenta, "%-16s", "VENTA");
        }

    } else if ((duplicado == RECIBO_DUPLICADO && reciboVenta.estadoTransaccion[0] == '0') ||
               duplicado == 6) {
        memset(parDuplicado, 0x00, sizeof(parDuplicado));
        if (duplicado == 6) {
            strcpy(parDuplicado, NO_IMPRIMIR);
        } else {
            strcpy(parDuplicado, "     ** DUPLICADO **");
        }

        sprintf(parVenta, "%-16s", "ANULACION");

        memset(valorFormato, 0x00, sizeof(valorFormato));
        memset(totalVenta, 0x00, sizeof(totalVenta));
        formatString(0x00, 0, reciboVenta.totalVenta, strlen(reciboVenta.totalVenta) - 2,
                     valorFormato, 1);
        sprintf(totalAnulacion, "-%s", valorFormato);
        sprintf(totalVenta, "%13s", totalAnulacion);
        memset(parTextoTotal, 0x00, sizeof(parTextoTotal));
        sprintf(parTextoTotal, "%-10s", "ANULACION");

        strcpy(parFirma, NO_IMPRIMIR);
        strcpy(parCedula, NO_IMPRIMIR);
        strcpy(parTel, NO_IMPRIMIR);
        strcpy(parDian, NO_IMPRIMIR);
        strcpy(parBase, NO_IMPRIMIR);
        strcpy(parTextoBase, NO_IMPRIMIR);

    } else {
        if (strncmp(fiidOTP, "8999", 4) != 0) {
            memset(parDuplicado, 0x00, sizeof(parDuplicado));
            strcpy(parDuplicado, NO_IMPRIMIR);
            if ((atoi(reciboVenta.tipoTransaccion)
                 == TRANSACCION_PAGO_ELECTRONICO)
                || (atoi(reciboVenta.tipoTransaccion)
                    == TRANSACCION_PAGO_ELECTRONICO_ESPECIAL)) {
                sprintf(parVenta, "%-16s", "COMPRA DINERO ELEC.");
            } else {
                sprintf(parVenta, "%-16s", "RETIRO DINERO ELEC.");
            }
        } else {
            sprintf(parVenta, "%-16s", "VENTA");
        }

    }

    getParameter(VERSION_SW, parVersion);
    getParameter(REVISION_SW, parVersion + strlen(parVersion));
    getParameter(TEXTO_COMERCIO, textoComercio);

    memset(stringAux, 0x00, sizeof(stringAux));

    memcpy(stringAux, reciboVenta.field57, 4);

    memset(stringAux, 0x00, sizeof(stringAux));
    memset(parFactura, 0x00, sizeof(parFactura));
    memcpy(stringAux, reciboVenta.field57 + 14, 20);

    sprintf(cardName, "%-15s", reciboVenta.cardName);

    memset(stringAux, 0x00, sizeof(stringAux));

    if (strncmp(reciboVenta.codigoCajero, codigoCajeroVacio, 10) == 0
        || strcmp(reciboVenta.codigoCajero, NO_IMPRIMIR_CAJERO) == 0) {
        strcpy(parcodigoCajero, NO_IMPRIMIR);

    } else {
        leftTrim(auxiliar, reciboVenta.codigoCajero, 0x20);
        leftPad(stringAux, auxiliar, 0x30, TAM_CODIGO_CAJERO);
        memcpy(parcodigoCajero, stringAux, LEN_TEFF_CODIGO_CAJERO);
    }

    HasMap hash[] =
            {
                    {"_PAR_FECHA_",       reciboVenta.txnDate},
                    {"_PAR_VERSION_",     parVersion},
                    {"_PAR_HORA_",        hora},
                    {"_PAR_NESTABLE_",    tablaUno.defaultMerchantName},
                    {"_PAR_DESTABLE_",    tablaUno.receiptLine2},
                    {"_PAR_TEXTO_ADM_",   textoComercio},
                    {"_PAR_CUNICO_",      tablaUno.receiptLine3},
                    {"_PAR_TERMINAL_",    reciboVenta.terminalId},
                    {"_PAR_CUOTAS_",      cuotas},
                    {"_PAR_CUENTA_",      tipoCuenta},
                    {"_PAR_TARJETA_",     cardName},
                    {"_PAR_CUATROU_",     ultimosCuatro},
                    {"_PAR_RECIBO_",      numeroRecibo},
                    {"_PAR_RRN_",         rrnVenta},
                    {"_PAR_VENTA_",       parVenta},
                    {"_PAR_BI_OTP_",      parBiOTP},
                    {"_PAR_COMPRA_",      parTextoCompraNeta},
                    {"_PAR_VCOMPRA_",     parCompraNeta},
                    {"_PAR_IVA_",         parTextoIva},
                    {"_PAR_VIVA_",        parIva},
                    {"_PAR_INC_",         parTextoInc},
                    {"_PAR_VINC_",        parInc},
                    {"_PAR_PROPINA_",     parTextoPropina},
                    {"_PAR_VPROPINA_",    parPropina},
                    {"_PAR_TOTAL_",       parTextoTotal},
                    {"_PAR_VTOTAL_",      totalVenta},
                    {"_PAR_BASEDEV_",     parTextoBase},
                    {"_PAR_VBASEDEV_",    parBase},
                    {"_PAR_APRO_",        aprob},
                    {"_PAR_SALUDO_",      parSaludo},
                    {"_PAR_FIRMA_",       parFirma},
                    {"_PAR_CEDULA_",      parCedula},
                    {"_PAR_TELEFONO_",    parTel},
                    {"_PAR_PIE_",         parPie},
                    {"_PAR_DUPLICADO_",   parDuplicado},
                    {"_PAR_DIAN_",        parDian},
                    {"_PAR_INFORMACION_", parInformacion},
                    {"_PAR_CCAJERO_",     parcodigoCajero},
                    {"FIN_HASH",          ""}
            };

    imprimirTicket(hash, parLogo, (char *) "/TDELECT.TPL");

}

int calcularCierreDineroElectronico(int index) {

    long tam = 0;
    int respuesta = 0;
    int tamJournal = 0;
    int maximoId = 0;
    int idx = 0;
    int tipoTransaccion = 0;
    int estadoTransaccion = 0;
    int n = 0;
    int puntero = 0;
    DatosVenta datosVenta;

    char valor[10 + 1];
    char valorVenta[10 + 1];
    char valorIva[10 + 1];
    char valorInc[10 + 1];
    char valorPropina[10 + 1];
    char valorBase[10 + 1];

    tam = tamArchivo(discoNetcom, JOURNAL);
    tamJournal = sizeof(DatosVenta);
    maximoId = tam / tamJournal;

    for (idx = 0; idx < maximoId; idx++) {

        memset(&datosVenta, 0x00, sizeof(datosVenta));
        datosVenta = buscarReciboByID(idx);
        tipoTransaccion = atoi(datosVenta.tipoTransaccion);
        estadoTransaccion = atoi(datosVenta.estadoTransaccion);

        if (tipoTransaccion == TRANSACCION_PAGO_ELECTRONICO ||
            tipoTransaccion == TRANSACCION_PAGO_ELECTRONICO_ESPECIAL) {

            memset(valor, 0x00, sizeof(valor));
            memset(valorVenta, 0x00, sizeof(valorVenta));
            memset(valorIva, 0x00, sizeof(valorIva));
            memset(valorInc, 0x00, sizeof(valorInc));
            memset(valorPropina, 0x00, sizeof(valorPropina));
            memset(valorBase, 0x00, sizeof(valorBase));

            if (strlen(datosVenta.totalVenta) > 2) {
                memcpy(valorVenta, datosVenta.totalVenta, strlen(datosVenta.totalVenta) - 2);
            }

            if (strlen(datosVenta.iva) > 2) {
                memcpy(valorIva, datosVenta.iva, strlen(datosVenta.iva) - 2);
            }

            if (strlen(datosVenta.inc) > 2) {
                memcpy(valorInc, datosVenta.inc, strlen(datosVenta.inc) - 2);
            }

            if (strlen(datosVenta.propina) > 2) {
                memcpy(valorPropina, datosVenta.propina, strlen(datosVenta.propina) - 2);
            }

            if (strlen(datosVenta.baseDevolucion) > 2) {
                memcpy(valorBase, datosVenta.baseDevolucion,
                       strlen(datosVenta.baseDevolucion) - 2);
            }

            if (strlen(datosVenta.compraNeta) > 2) {
                memcpy(valor, datosVenta.compraNeta, strlen(datosVenta.compraNeta) - 2);
            }

            crearEntidad(datosVenta.cardName);
            index = getNumeroEntidades();

            for (n = 0; n < index; n++) {

                puntero = 0;

                if (strstr(cierreEntidades[n].nombreBanco, datosVenta.cardName) != NULL) {
                    respuesta++;
                    puntero = n;

                    n = index;
                }
            }

            if (estadoTransaccion == TRANSACCION_ACTIVA) {

                cierreEntidades[puntero].cantidadVenta++;
                cierreEntidades[puntero].totalVentaNeta += atoi(valor);
                cierreEntidades[puntero].totalIva += atoi(valorIva);
                cierreEntidades[puntero].totalInc += atoi(valorInc);
                cierreEntidades[puntero].totalPropina += atoi(valorPropina);
                cierreEntidades[puntero].totalVenta += atoi(valorVenta);
                cierreEntidades[puntero].totalBase += atoi(valorBase);

            } else {

                cierreEntidades[puntero].cantidadAnulaciones++;
                cierreEntidades[puntero].totalAnulaciones += atoi(valorVenta);
            }

        }
    }

    return respuesta;
}

int calcularCierreDineroElectronicoRetiro(int index) {

    long tam = 0;
    int respuesta = 0;
    int tamJournal = 0;
    int maximoId = 0;
    int idx = 0;
    int tipoTransaccion = 0;
    int estadoTransaccion = 0;
    int n = 0;
    int puntero = 0;
    DatosVenta datosVenta;

    char valor[10 + 1];
    char valorVenta[10 + 1];
    char valorIva[10 + 1];
    char valorInc[10 + 1];
    char valorPropina[10 + 1];
    char valorBase[10 + 1];

    tam = tamArchivo(discoNetcom, JOURNAL);
    tamJournal = sizeof(DatosVenta);
    maximoId = tam / tamJournal;

    for (idx = 0; idx < maximoId; idx++) {

        memset(&datosVenta, 0x00, sizeof(datosVenta));
        datosVenta = buscarReciboByID(idx);
        tipoTransaccion = atoi(datosVenta.tipoTransaccion);
        estadoTransaccion = atoi(datosVenta.estadoTransaccion);

        if (tipoTransaccion == TRANSACCION_RETIRO_OTP) {

            memset(valor, 0x00, sizeof(valor));
            memset(valorVenta, 0x00, sizeof(valorVenta));
            memset(valorIva, 0x00, sizeof(valorIva));
            memset(valorInc, 0x00, sizeof(valorInc));
            memset(valorPropina, 0x00, sizeof(valorPropina));
            memset(valorBase, 0x00, sizeof(valorBase));

            if (strlen(datosVenta.totalVenta) > 2) {
                memcpy(valorVenta, datosVenta.totalVenta, strlen(datosVenta.totalVenta) - 2);
            }

            if (strlen(datosVenta.iva) > 2) {
                memcpy(valorIva, datosVenta.iva, strlen(datosVenta.iva) - 2);
            }

            if (strlen(datosVenta.inc) > 2) {
                memcpy(valorInc, datosVenta.inc, strlen(datosVenta.inc) - 2);
            }

            if (strlen(datosVenta.propina) > 2) {
                memcpy(valorPropina, datosVenta.propina, strlen(datosVenta.propina) - 2);
            }

            if (strlen(datosVenta.baseDevolucion) > 2) {
                memcpy(valorBase, datosVenta.baseDevolucion,
                       strlen(datosVenta.baseDevolucion) - 2);
            }

            if (strlen(datosVenta.compraNeta) > 2) {
                memcpy(valor, datosVenta.compraNeta, strlen(datosVenta.compraNeta) - 2);
            }

            crearEntidad(datosVenta.cardName);
            index = getNumeroEntidades();

            for (n = 0; n < index; n++) {

                puntero = 0;

                if (strstr(cierreEntidades[n].nombreBanco, datosVenta.cardName) != NULL) {
                    respuesta++;
                    puntero = n;

                    n = index;
                }
            }

            if (estadoTransaccion == TRANSACCION_ACTIVA) {
                cierreEntidades[puntero].cantidadVenta++;
                cierreEntidades[puntero].totalVentaNeta += atoi(valor);
                cierreEntidades[puntero].totalIva += atoi(valorIva);
                cierreEntidades[puntero].totalInc += atoi(valorInc);
                cierreEntidades[puntero].totalPropina += atoi(valorPropina);
                cierreEntidades[puntero].totalVenta += atoi(valorVenta);
                cierreEntidades[puntero].totalBase += atoi(valorBase);
            } else {
                cierreEntidades[puntero].cantidadAnulaciones++;
                cierreEntidades[puntero].totalAnulaciones += atoi(valorVenta);
            }
        }
    }
    return respuesta;
}

int crearEntidad(char *entidad) {

    int creado = 0;
    int n = 0;

    for (n = 0; n < MAXIMO_NUMERO_ENTIDADES; n++) {

        if (strstr(cierreEntidades[n].nombreBanco, entidad) != NULL) {
            creado = TRUE;
            break;
        }
    }

    if (creado == 0) {

        for (n = 0; n < MAXIMO_NUMERO_ENTIDADES; n++) {

            if (strlen(cierreEntidades[n].nombreBanco) == 0) {
                memcpy(cierreEntidades[n].nombreBanco, entidad, strlen(entidad));
                break;
            }

        }

    }

    return creado;

}

int getNumeroEntidades(void) {

    int n = 0;

    for (n = 0; n < MAXIMO_NUMERO_ENTIDADES; n++) {

        if (strlen(cierreEntidades[n].nombreBanco) == 0) {
            break;
        }

    }

    return n;
}

void cierreDineroElectronico(DatosBasicosCierre *basicos) {

    int respuesta = 0;
    int numeroEntidades = 0;
    int index = 0;
    int n = 0;
    int cantidad = 0;
    char buffer[12 + 1];

    DatosCierreDineroElectronico auxiliar;

    memset(buffer, 0x00, sizeof(buffer));
    seteaEstructuraEntidades();
    LOGI("calcularCierreDineroElectronico A");
    respuesta = calcularCierreDineroElectronico(index);
    LOGI("respuesta %d",respuesta);
    if (respuesta > 0) {
        numeroEntidades = getNumeroEntidades();
        memset(&auxiliar, 0x00, sizeof(auxiliar));
        for (n = 0; n <= numeroEntidades; n++) {

            if (cierreEntidades[n].cantidadAnulaciones > 0 ||
                cierreEntidades[n].cantidadVenta > 0) {
                LOGI("imprimirCierreDineroElectronico A");
                imprimirCierreDineroElectronico(cierreEntidades[n], cantidad);
                auxiliar.cantidadVenta += cierreEntidades[n].cantidadVenta;
                auxiliar.totalVentaNeta += cierreEntidades[n].totalVentaNeta;
                auxiliar.totalIva += cierreEntidades[n].totalIva;
                auxiliar.totalInc += cierreEntidades[n].totalInc;
                auxiliar.totalPropina += cierreEntidades[n].totalPropina;
                auxiliar.totalVenta += cierreEntidades[n].totalVenta;
                auxiliar.totalBase += cierreEntidades[n].totalBase;
                auxiliar.cantidadAnulaciones += cierreEntidades[n].cantidadAnulaciones;
                auxiliar.totalAnulaciones += cierreEntidades[n].totalAnulaciones;
                cantidad++;

            }
        }
    }

    basicos->cantidadVenta = auxiliar.cantidadVenta;
    sprintf(basicos->datos.compraNeta, "%ld", auxiliar.totalVentaNeta);
    sprintf(basicos->datos.iva, "%ld", auxiliar.totalIva);
    sprintf(basicos->datos.inc, "%ld", auxiliar.totalInc);
    sprintf(basicos->datos.propina, "%ld", auxiliar.totalPropina);
    sprintf(basicos->datos.baseDevolucion, "%ld", auxiliar.totalBase);
    basicos->cantidadAnulaciones = auxiliar.cantidadAnulaciones;
    sprintf(basicos->totalAnulacion, "%ld", auxiliar.totalAnulaciones);

}

void cierreDineroElectronicoRetiro(DatosBasicosCierre *basicos) {

    int respuesta = 0;
    int numeroEntidades = 0;
    int index = 0;
    int n = 0;
    int cantidad = 0;
    char buffer[12 + 1];

    DatosCierreDineroElectronico auxiliar;

    memset(buffer, 0x00, sizeof(buffer));
    seteaEstructuraEntidades();
    LOGI("calcularCierreDineroElectronicoRetiro ");
    respuesta = calcularCierreDineroElectronicoRetiro(index);

    if (respuesta > 0) {
        numeroEntidades = getNumeroEntidades();
        LOGI("numeroEntidades %d ",numeroEntidades);
        memset(&auxiliar, 0x00, sizeof(auxiliar));
        for (n = 0; n <= numeroEntidades; n++) {

            if (cierreEntidades[n].cantidadAnulaciones > 0 ||
                cierreEntidades[n].cantidadVenta > 0) {

                imprimirCierreRetiro(cierreEntidades[n], cantidad);
                auxiliar.cantidadVenta += cierreEntidades[n].cantidadVenta;
                auxiliar.totalVentaNeta += cierreEntidades[n].totalVentaNeta;
                auxiliar.totalIva += cierreEntidades[n].totalIva;
                auxiliar.totalInc += cierreEntidades[n].totalInc;
                auxiliar.totalPropina += cierreEntidades[n].totalPropina;
                auxiliar.totalVenta += cierreEntidades[n].totalVenta;
                auxiliar.totalBase += cierreEntidades[n].totalBase;
                auxiliar.cantidadAnulaciones += cierreEntidades[n].cantidadAnulaciones;
                auxiliar.totalAnulaciones += cierreEntidades[n].totalAnulaciones;
                cantidad++;

            }
        }
    }

    basicos->cantidadVenta = auxiliar.cantidadVenta;
    sprintf(basicos->datos.compraNeta, "%ld", auxiliar.totalVentaNeta);
    sprintf(basicos->datos.iva, "%ld", auxiliar.totalIva);
    sprintf(basicos->datos.inc, "%ld", auxiliar.totalInc);
    sprintf(basicos->datos.propina, "%ld", auxiliar.totalPropina);
    sprintf(basicos->datos.baseDevolucion, "%ld", auxiliar.totalBase);
    basicos->cantidadAnulaciones = auxiliar.cantidadAnulaciones;
    sprintf(basicos->totalAnulacion, "%ld", auxiliar.totalAnulaciones);

}


void imprimirCierreDineroElectronico(DatosCierreDineroElectronico datosCierre, int cantidad) {

    char lineaSalida[60 + 1];
    char valorAnulacion[20 + 1];
    char valor[20 + 1];

    TablaUnoInicializacion tablaUno;

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valorAnulacion, 0x00, sizeof(valorAnulacion));
    memset(&tablaUno, 0x00, sizeof(tablaUno));

    tablaUno = _desempaquetarTablaCeroUno_();

    if (cantidad == 0) {
        memset(lineaSalida, 0x00, sizeof(lineaSalida));
        sprintf(lineaSalida, "%s", "\n  COMPRAS DINERO ELECT.  \n");
        pprintf(lineaSalida);
    }

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memcpy(lineaSalida, datosCierre.nombreBanco, 10);
    strcat(lineaSalida, "\n");
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    formatString(0x00, datosCierre.totalVentaNeta, "", 0, valor, 1);
    sprintf(lineaSalida, "COMPRA       : %3d%16s%13s\n", datosCierre.cantidadVenta, " ", valor);
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    formatString(0x00, datosCierre.totalIva, "", 0, valor, 1);
    sprintf(lineaSalida, "IVA          : %19s%13s\n", " ", valor);
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    formatString(0x00, datosCierre.totalInc, "", 0, valor, 1);
    sprintf(lineaSalida, "INC          : %19s%13s\n", " ", valor);
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    formatString(0x00, datosCierre.totalPropina, "", 0, valor, 1);

    if (tablaUno.options1.tipProcessing == 1) {
        sprintf(lineaSalida, "PROPINA      : %19s%13s\n", " ", valor);
        pprintf(lineaSalida);
    }
    pprintf("-----------------------------------------------\n");
    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    formatString(0x00, datosCierre.totalVenta, "", 0, valor, 1);
    sprintf(lineaSalida, "TOTAL        : %3d%16s%13s\n", datosCierre.cantidadVenta, " ", valor);
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    formatString(0x00, datosCierre.totalAnulaciones, "", 0, valor, 1);
    memset(valorAnulacion, 0x00, sizeof(valorAnulacion));
    if (datosCierre.cantidadAnulaciones > 0) {
        strcat(valorAnulacion, "-");
    }
    strcat(valorAnulacion, valor);
    sprintf(lineaSalida, "ANULACIONES  : %3d%16s%13s\n", datosCierre.cantidadAnulaciones, " ",
            valorAnulacion);
    pprintf(lineaSalida);
    pprintf("-----------------------------------------------\n");
    pprintf("-----------------------------------------------\n");
}

void imprimirCierreRetiro(DatosCierreDineroElectronico datosCierre, int cantidad) {

    char lineaSalida[60 + 1];
    char valorAnulacion[20 + 1];
    char valor[20 + 1];

    TablaUnoInicializacion tablaUno;

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valorAnulacion, 0x00, sizeof(valorAnulacion));
    memset(&tablaUno, 0x00, sizeof(tablaUno));

    tablaUno = _desempaquetarTablaCeroUno_();

    if (cantidad == 0) {
        memset(lineaSalida, 0x00, sizeof(lineaSalida));
        sprintf(lineaSalida, "%s", "\n RETIRO DINERO ELECT.  \n");
        pprintf(lineaSalida);
    }

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memcpy(lineaSalida, datosCierre.nombreBanco, 10);
    strcat(lineaSalida, "\n");
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    formatString(0x00, datosCierre.totalVentaNeta, "", 0, valor, 1);
    sprintf(lineaSalida, "COMPRA       : %3d%16s%13s\n", datosCierre.cantidadVenta, " ", valor);
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    formatString(0x00, datosCierre.totalIva, "", 0, valor, 1);
    sprintf(lineaSalida, "IVA          : %19s%13s\n", " ", valor);
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    formatString(0x00, datosCierre.totalInc, "", 0, valor, 1);
    sprintf(lineaSalida, "INC          : %19s%13s\n", " ", valor);
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    formatString(0x00, datosCierre.totalPropina, "", 0, valor, 1);

    if (tablaUno.options1.tipProcessing == 1) {
        sprintf(lineaSalida, "PROPINA      : %19s%13s\n", " ", valor);
        pprintf(lineaSalida);
    }
    pprintf("-----------------------------------------------\n");
    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    memset(valor, 0x00, sizeof(valor));
    formatString(0x00, datosCierre.totalVenta, "", 0, valor, 1);
    sprintf(lineaSalida, "TOTAL        : %3d%16s%13s\n", datosCierre.cantidadVenta, " ", valor);
    pprintf(lineaSalida);

    pprintf("-----------------------------------------------\n");
    pprintf("-----------------------------------------------\n");
    pprintf(" \n");

}

/**
 * @brief Verifica las entidades que tienen cierre
 * @return
 */
int entidadesDineroElectronico(void) {

    int existeArchivo = 0;
    int lengthTabla = 0;
    int itemsEntidades = 0;
    int indice = 0;
    int posicion = 0;
    int codigoIdentidad = 0;
    int indexAux = 0;
    int respuesta = 0;
    uChar dataEntidades[LEN_BLOQUE_ENTIDADES + 1];

    seteaEstructuraEntidades();
    existeArchivo = verificarArchivo(discoNetcom, TABLA_7_INICIALIZACION);

    if (existeArchivo > 0) {

        lengthTabla = tamArchivo(discoNetcom, TABLA_7_INICIALIZACION);
        itemsEntidades = lengthTabla / LEN_BLOQUE_ENTIDADES;
        for (indice = 0; indice < itemsEntidades; indice++) {

            memset(dataEntidades, 0x00, sizeof(dataEntidades));
            buscarArchivo(discoNetcom, TABLA_7_INICIALIZACION, dataEntidades, posicion,
                          LEN_BLOQUE_ENTIDADES);
            codigoIdentidad = dataEntidades[4];

            if (codigoIdentidad != 0) {

                memcpy(cierreEntidades[indexAux].nombreBanco, dataEntidades + 8, 9);
                indexAux++;
                respuesta = indexAux;

            }
            posicion += LEN_BLOQUE_ENTIDADES;
        }
    }

    return respuesta;
}

int _dineroElectronicoActivo_(void) {

    int activo = 0;

    activo = ((verificarArchivo(DISCO_NETCOM, TABLA_7_INICIALIZACION) > 0) ? 1 : -1);
    activo = 1;

    return activo;
}

/**
 * @brief Setea estructura entidades
 */
void seteaEstructuraEntidades(void) {
    int n = 0;

    for (n = 0; n < MAXIMO_NUMERO_ENTIDADES; n++) {
        memset(cierreEntidades[n].nombreBanco, 0x00, sizeof(cierreEntidades));
        cierreEntidades[n].cantidadAnulaciones = 0;
        cierreEntidades[n].cantidadVenta = 0;
        cierreEntidades[n].totalAnulaciones = 0;
        cierreEntidades[n].totalInc = 0;
        cierreEntidades[n].totalIva = 0;
        cierreEntidades[n].totalVenta = 0;
        cierreEntidades[n].totalVentaNeta = 0;
    }

}

/**
 * @brief esta funcion realiza el set en las variables bit4,bit3,bandera
 *        que permite saber el tipo de flujo que debe seguir la transaccion
 * @param bit4
 * @param bit3
 * @param bandera
 */
void setComportamientoTransaccion(int bit4, int bit3, int bandera) {
    bit4Prendido = bit4;
    bit3Prendido = bit3;
    banderaValidacion = bandera;
}

/**
 * @brief esta funcion realiza el get en las variables bit4,bit3,bandera
 *        que permite saber el tipo de flujo que debe seguir la transaccion
 * @param bit4
 * @param bit3
 * @param bandera
 */
void getComportamientoTransaccion(int *bit4, int *bit3, int *bandera) {

    *bit4 = bit4Prendido;
    *bit3 = bit3Prendido;
    *bandera = banderaValidacion;
}

DatosEmpaquetados armarCampo42(DatosVenta datosVenta) {

    int indice = 0;
    uChar fiidOTP[4 + 1];

    DatosEmpaquetados datos;

    memset(&datos, 0x00, sizeof(&datos));
    memset(fiidOTP, 0x00, sizeof(fiidOTP));
    _convertBCDToASCII_(fiidOTP, globalTramaVenta.field61, 4);
    memcpy(datos.buffer, datosVenta.cuotas, 2);
    indice += 2;

    if (strncmp(fiidOTP, "8999", 4) == 0 || strncmp(fiidOTP, "7999", 4) == 0) {
        memcpy(datos.buffer + indice, "EEE", 3);
        indice += 3;
        memcpy(datos.buffer + indice, datosVenta.fiel42, 10);

    } else {
        memcpy(datos.buffer + indice, datosVenta.isQPS, 1);
        indice += 1;
        memcpy(datos.buffer + indice, datosVenta.fiel42, 12);
    }

    datos.totalBytes = indice;
    return datos;
}

int procesarDatosTarjeta(DatosTarjeta *leerTrack) {

    int respuesta = 0;
    int existeEmisor = 0;
    char outBuffer[80 + 1];
    char bufferAuxiliar[30 + 1];
    char pan[19 + 1];
    char fecha[6 + 1];
    char bin[20 + 1];
    DatosTarjeta datosTarjeta;
    EmisorAdicional emisorAdicional;
    int idx = 0;
    int formato = 0;

    memset(outBuffer, 0x00, sizeof(outBuffer));
    memset(bufferAuxiliar, 0x00, sizeof(bufferAuxiliar));
    memset(pan, 0x00, sizeof(pan));
    memset(fecha, 0x00, sizeof(fecha));
    memset(&datosTarjeta, 0x00, sizeof(datosTarjeta));
    memset(&leerTrack->dataIssuer, 0x00, sizeof(leerTrack->dataIssuer));
    memset(bin, 0x00, sizeof(bin));
    memset(&emisorAdicional, 0x00, sizeof(emisorAdicional));

    memcpy(outBuffer, globalTramaVenta.track2, sizeof(globalTramaVenta.track2));
    respuesta = strlen(globalTramaVenta.track2);

    if (respuesta > 0) {

        ///El track2
        memset(globalTramaVenta.track2, 0x00,
               sizeof(globalTramaVenta.track2)); //sizeof(datosVenta.track2)
        memcpy(globalTramaVenta.track2, outBuffer, respuesta - 1);
        memcpy(leerTrack->track2, outBuffer, respuesta - 1);

        /// El pan
        for (idx = 0; idx < respuesta; idx++) {
            if (outBuffer[idx] == 'D' || outBuffer[idx] == 'd') {
                formato = 1;
                break;

            }
        }

        if (formato == 1) {
            memcpy(leerTrack->pan, globalTramaVenta.track2, idx);
            memcpy(leerTrack->fechaExpiracion, globalTramaVenta.track2 + idx + 1, 4);
            memcpy(globalTramaVenta.fechaExpiracion, globalTramaVenta.track2 + idx + 1, 4);
            /// Ultimos 4

            idx = strlen(leerTrack->pan);
            idx -= 4;
            memcpy(globalTramaVenta.ultimosCuatro, leerTrack->pan + idx, 4);

            memcpy(bin, leerTrack->pan, 9);
            leerTrack->dataIssuer = _traerIssuer_(atol(bin));

        } else {
            respuesta = -1;
        }

        if (leerTrack->dataIssuer.respuesta == 1 && respuesta > 0) {
            existeEmisor = buscarBinEmisor(bin);
            if (existeEmisor > 0) {
                emisorAdicional = traerEmisorAdicional(existeEmisor);
                if (emisorAdicional.existeIssuerEmisor == 1) {
                    memset(leerTrack->dataIssuer.cardName, 0x00,
                           sizeof(leerTrack->dataIssuer.cardName));
                    memcpy(leerTrack->dataIssuer.cardName, emisorAdicional.nombreEmisor, 15);
                    leerTrack->isEmisorAdicional = 1;
                }
            }
            leerTrack->respuesta = _TRUE_;
            memcpy(globalTramaVenta.codigoGrupo, leerTrack->dataIssuer.reservedForFutureUse,
                   TAM_GRUPO);
            memcpy(globalTramaVenta.codigoSubgrupo, leerTrack->dataIssuer.reservedForFutureUse2,
                   TAM_SUBGRUPO);

        }
    }
    return respuesta;
}


int seleccionarCuenta(DatosTarjeta *leerTrack, DatosVenta *datosVenta) {

    int resultado = 1;
    int pideCuotas = 0;
    long panTarjeta = 0;
    uChar panString[9 + 1];

    ConfiguracionExcepciones configuracionExcepciones;

    memset(&configuracionExcepciones, 0x00, sizeof(configuracionExcepciones));
    memset(panString, 0x00, sizeof(panString));

    memcpy(panString, leerTrack->pan, 9);

    panTarjeta = atol(panString);

    leerTrack->configuracionExcepciones = excepcionesEmv(panTarjeta);

    resultado = validarMultiServicios(&pideCuotas, datosVenta, leerTrack);
    if (resultado == 40) {
        if (parametrosVenta.permiteIngresoUltmosCuatro == 1) {
            //resultado = capturarUltimosCuatro(datosVenta->ultimosCuatro);
            parametrosVenta.permiteIngresoUltmosCuatro = 0;
        }
    }
    if (resultado > 0) {
        configuracionExcepciones = excepcionesEmv(panTarjeta);
        resultado = validarSeleccionCuenta(&pideCuotas, datosVenta, leerTrack,
                                           configuracionExcepciones.comportamientoExcepciones1.bit4);
        if (resultado > 0) {
            if ((pideCuotas) == 1) {
                leerTrack->dataIssuer.options3.printNoRefundsAllowed = 1;
            }
        }
    }

    // Se actualiza los campo de la structura ParametrosVenta
    /////// PERMITE SELECCION DE CUENTA ////////
    if (leerTrack->dataIssuer.options1.selectAccountType == 1) {
        parametrosVenta.permiteSeleccionCuenta = 1;
    }

    ////// PERMITE INGRESO DE PIN ///////
    if (leerTrack->dataIssuer.options1.pinEntry == 1) {
        parametrosVenta.permiteIngresoPin = 1;
    }

    ////// PERMITE INGRESO CUOTAS ? ////////
    if ((leerTrack->dataIssuer.options3.printNoRefundsAllowed == 1) && (pideCuotas == 1)) {
        parametrosVenta.permiteIngresoCoutas = 1;
    } else {
        parametrosVenta.permiteIngresoCoutas = 0;
    }
    ////// PERMITE INGRESO DE PIN ///////
    if (leerTrack->dataIssuer.additionalPromptsSelect.additionalPrompt1 == 1) {
        parametrosVenta.permiteIngresoDocumento = 1;
    }
    return resultado;
}

DatosIniQrCode datoInteroperabilidad(void) {

    char buffer[20 + 1];
    int indice = 0;
    DatosIniQrCode datosOTP;

    memset(&buffer, 0x00, sizeof(&buffer));
    memset(&datosOTP, 0x00, sizeof(datosOTP));

    buscarArchivo(discoNetcom, TABLA_INICIALIZACION_QRCODE, buffer, 0, 20);
    LOGI("buffer %s", buffer);
    indice += (4);
    memcpy(datosOTP.terminal, buffer + indice, TAM_TERMINAL_ID);
    LOGI("terminal %s", datosOTP.terminal);
    indice += (TAM_TERMINAL_ID);
    memcpy(datosOTP.tiempoIntentos, buffer + indice, TAM_NII);
    indice += (TAM_NII);
    memcpy(datosOTP.numeroIntentos, buffer + indice, 1);
    indice += (1);
    memcpy(datosOTP.niiQr, buffer + indice, 4);
    LOGI("NII %s", datosOTP.niiQr);
    return datosOTP;
}

TablaSieteInicializacion obtenerOpcionMenu(int opcion, char *valor) {

    int index = 0;
    char posicionAuxiliar[4 + 1] = {0x00};

    uChar dataEntidades[LEN_BLOQUE_ENTIDADES + 1] = {0x00};
    TablaSieteInicializacion informacionEntidades;

    memset(&informacionEntidades, 0x00, sizeof(informacionEntidades));

    if (opcion >= 0) {
        memset(dataEntidades, 0x00, sizeof(dataEntidades));
        memcpy(posicionAuxiliar, valor, strlen(valor));
        LOGI("entra a buscar file ");
        buscarArchivo(discoNetcom, TABLA_7_INICIALIZACION, dataEntidades, atoi(posicionAuxiliar),
                      LEN_BLOQUE_ENTIDADES);
        LOGI("sale de  buscar file ");
        index += 4;
        LOGI("dataEntidades[index] %d", dataEntidades[index]);
        informacionEntidades.codigoEntidad = dataEntidades[index];
        LOGI("codigoEntidad %d", informacionEntidades.codigoEntidad);
        index += 1;
        memcpy(informacionEntidades.codigoFiid, dataEntidades + index, 2);
        LOGI("fidd %s", informacionEntidades.codigoFiid);
        index += 2;

        informacionEntidades.banderaValidacion = dataEntidades[index];
        LOGI("bandera validacion %d", informacionEntidades.banderaValidacion);
        index += 1;

        memcpy(informacionEntidades.nombreBanco, dataEntidades + index, 9);
        LOGI(" nombre banco %s", informacionEntidades.nombreBanco);
        index += 9;
        memcpy(informacionEntidades.binAsignado, dataEntidades + index, 6);
        LOGI(" bins asignado %s", informacionEntidades.binAsignado);
        index += 6;
        informacionEntidades.tipoTransaccion = dataEntidades[index];
        LOGI(" tipoTransaccion %d", informacionEntidades.tipoTransaccion);

    } else {
        informacionEntidades.tipoTransaccion = -11;
    }
    return informacionEntidades;
}

void solicitarMenuEntidades() {
    memset(&parametrosCompra, 0x00, sizeof(parametrosCompra));
    memset(&globalTramaVenta, 0x00, sizeof(globalTramaVenta));
    invocarMenuEntidades();
    sprintf(globalTramaVenta.tipoTransaccion, "%02d", TRANSACCION_PAGO_ELECTRONICO);
}

ParametrosDineroElectronico
armarTrack2DineroElectronico(int tipoDocumento, char *documento, char *celular) {

    int respuesta = 0;
    int tamanoDocumento = 0;

    uChar auxBuffer[12 + 1] = {0x00};
    char fiidAuxiliar[3 + 1] = {0x00};
    uChar flujoUnificado[2 + 1] = {0x00};


    DatosTarjeta leerTrack;
    ParametrosDineroElectronico parametrosAux;
    memset(&leerTrack, 0x00, sizeof(leerTrack));
    memset(&parametrosAux, 0x00, sizeof(parametrosAux));

    getParameter(FLUJO_UNIFICADO, flujoUnificado);

    if (strncmp(parametrosCompra.fiddOtp, "8999", 4) != 0 ||
        atoi(globalTramaVenta.tipoTransaccion) != TRANSACCION_RETIRO_OTP) {
        if (strlen(celular) > 0) {
            memcpy(globalTramaVenta.acquirerId, celular, strlen(celular));
        }
    }
    memset(globalTramaVenta.fiel42, 0x30, sizeof(globalTramaVenta.fiel42));

    if (strlen(documento) > 0) {
        tamanoDocumento = strncmp(parametrosCompra.fiddOtp, "8999", 4) == 0 ? 10 : 12;
        tamanoDocumento = strncmp(parametrosCompra.fiddOtp, "7999", 4) == 0 ? 10 : tamanoDocumento;
        leftPad(globalTramaVenta.fiel42, documento, 0x30, tamanoDocumento);
    }

    switch (tipoDocumento) {
        case C_CIUDADANIA:
            memcpy(globalTramaVenta.isQPS, "2", 1);
            break;
        case C_EXTRANJERIA:
            memcpy(globalTramaVenta.isQPS, "3", 1);
            break;
    }

    if (strncmp(flujoUnificado, "1", 1) == 0) {
        if (bit1Validacion == TRUE && bit2Validacion == TRUE) {
            LOGI("copiar documento %s", documento);
            memcpy(auxBuffer, documento, strlen(documento));
        } else {
            if (bit2Validacion == TRUE) {
                LOGI("copiar celular %s", celular);
                memcpy(auxBuffer, celular, strlen(celular));
            } else if (bit1Validacion == TRUE) {
                LOGI("copiar documento %s", documento);
                memcpy(auxBuffer, documento, strlen(documento));
            }
        }
    } else {
        if (banderaValidacion == 3 || bit3Prendido == TRUE || banderaValidacion == 2) {
            LOGI("copiar celular %s", celular);
            memcpy(auxBuffer, celular, strlen(celular));
        } else if (banderaValidacion == 1) {
            LOGI("copiar documento %s", documento);
            memcpy(auxBuffer, documento, strlen(documento));
        }
    }
    memset(globalTramaVenta.track2, 0x00, sizeof(globalTramaVenta.track2));
    LOGI("Armar track2 ");
    construccionTrack(parametrosCompra.binAsignado, fiidAuxiliar, auxBuffer,
                      globalTramaVenta.track2);
    respuesta = 1;
    LOGI("salio  track2  %s", globalTramaVenta.track2);
    if (strncmp(flujoUnificado, "1", 1) == 0) {

        if (respuesta > 0) {
            respuesta = procesarDatosTarjeta(&leerTrack);

            if (respuesta > 0) {
                memcpy(globalTramaVenta.cardName, leerTrack.dataIssuer.cardName,
                       strlen(leerTrack.dataIssuer.cardName));
                parametrosVenta = traerParametrosVenta(leerTrack);
            }
            LOGI("bit3Validacion  %d respuesta %d", bit3Validacion, respuesta);
            LOGI("parametrosCompra.fiddOtp  %s", parametrosCompra.fiddOtp);
            if ((respuesta > 0 && bit3Validacion == TRUE) ||
                (respuesta > 0 && strncmp(parametrosCompra.fiddOtp, "7999", 4) == 0)) {

                if (strncmp(parametrosCompra.fiddOtp, "7999", 4) != 0) {
                    respuesta = seleccionarCuenta(&leerTrack, &globalTramaVenta);
                    parametrosCompra.permiteSeleccionCuenta = 1;
                    LOGI("permite seleccion de cuenta");
                    memcpy(globalTramaVenta.cuotas, "00", 2);
                } else {
                    respuesta = 1;
                    parametrosVenta.permiteIngresoCoutas = TRUE;
                    parametrosCompra.permiteCuotas = 1;
                    LOGI("permite cuotas ");
                }
            }
        }
    }
    memcpy(parametrosAux.fiddOtp, parametrosCompra.fiddOtp, strlen(parametrosCompra.fiddOtp));
    parametrosCompra.tipoPinSolicitado = verificarSolicitudPin();
    LOGI("tipo de pin %d", parametrosAux.tipoPinSolicitado);
    memcpy(&parametrosAux, &parametrosCompra, sizeof(parametrosCompra));
    return parametrosAux;
}


int verificarSolicitudPin() {

    int respuesta = 0;
    //respuesta = intentosPin(ENVIO_SOLICITUD_PAGO_ELECTRONICO);

    respuesta = intentosPin(ENVIO_PAGO_ELECTRONICO);

    if (respuesta == 0) {
        respuesta = -2;
    }
    LOGI("respuesta  %d", respuesta);
    return respuesta;
}

int realizarProcesoTransaccionDineroElectronico(char *pinblock, int intentosPin, char *tipoCuenta,
                                                char *cuotas) {

    int resultado = 0;

    if (strlen(cuotas) > 0) {
        LOGI("cuotas %s", cuotas);
        memcpy(globalTramaVenta.cuotas, cuotas, strlen(cuotas));
    }
    if (strlen(tipoCuenta) > 0) {
        LOGI("tipo de cuenta  %s", tipoCuenta);
        memcpy(globalTramaVenta.tipoCuenta, tipoCuenta, strlen(tipoCuenta));
    } else {
        strcpy(globalTramaVenta.tipoCuenta, "00");
    }

    if ((IS_IMPAR(strlen(pinblock)) == 1)) {
        strcat(pinblock, "F");
    }
    LOGI("pinblock  %s", pinblock);
    memcpy(globalTramaVenta.textoAdicionalInicializacion, pinblock, strlen(pinblock));
    resultado = procesarCompraDineroElectronico(pinblock, intentosPin);

    if (resultado > 0) {
        mostrarAprobacion(ENVIO_PAGO_ELECTRONICO);
        realizarImpresion(RECIBO_NO_DUPLICADO);
        setParameter(ACUMULAR_LIFEMILES, "1");

    }
    return resultado;
}

void obtencionDatosBasicosVenta(DatosBasicosVenta datosBasicos) {
    strcpy(globalTramaVenta.iva, datosBasicos.iva);
    strcpy(globalTramaVenta.inc, datosBasicos.inc);
    strcpy(globalTramaVenta.baseDevolucion, datosBasicos.baseDevolucion);
    strcpy(globalTramaVenta.propina, datosBasicos.propina);
    strcpy(globalTramaVenta.compraNeta, datosBasicos.compraNeta);
    strcpy(globalTramaVenta.totalVenta, datosBasicos.totalVenta);
    LOGI("total adquirido  %s", globalTramaVenta.totalVenta);
    strcat(globalTramaVenta.totalVenta, "00");
    strcat(globalTramaVenta.compraNeta, "00");
    strcat(globalTramaVenta.baseDevolucion, "00");
    strcat(globalTramaVenta.inc, "00");
    strcat(globalTramaVenta.iva, "00");
    strcat(globalTramaVenta.propina, "00");
}