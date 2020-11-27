//
// Created by NETCOM on 03/03/2020.
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
#include "android/log.h"

#define  LOG_TAG    "NETCOM_CAJAS_IP"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
int peticionCajaIP(DatosVenta *datos, int tipoTransaccionCaja) {

    int retorno = 0;
    int indice = 0;
    int tpdu = 0;
    int indiceRecibido = 0;
    uChar terminal[SIZE_ID_TEFF + 1];
    uChar tcpIpActual[SIZE_DOMINIO + 1];
    uChar dataEnviada[512];
    uChar recibida[2048];
    uChar bufferCaja[512];
    uChar dataTransaccion[1024];
    uChar datoshex2[512 + 1];
    int servicioWso2 = 0;

    memset(terminal, 0x00, sizeof(terminal));
    memset(tcpIpActual, 0x00, sizeof(tcpIpActual));
    memset(dataEnviada, 0x00, sizeof(dataEnviada));
    memset(recibida, 0x00, sizeof(recibida));
    memset(bufferCaja, 0x00, sizeof(bufferCaja));
    memset(dataTransaccion, 0x00, sizeof(dataTransaccion));
    memset(datoshex2, 0x00, sizeof(datoshex2));

    retorno = traerDatosConsumoRest(terminal, tcpIpActual);

    if (retorno > 0) {
        retorno = generarTramaPeticionIP(dataTransaccion, *datos, tipoTransaccionCaja);
    }

    if (retorno > 0) {
        indice = armarTpdu(dataEnviada, PRODUCTO_CAJA_IP);
        indiceRecibido = indice;
    }
    if (indice > 4) {
        tpdu = 1;
        indiceRecibido += 2;
    }

    if (retorno) {
        indice = armarPeticionDatos(dataEnviada, dataTransaccion, indice);

        if (tpdu == 1) {
            agregarLongitud(indice, dataEnviada);
            indice += 2;
        }
        //mostrarMensajeConectando();

        retorno = realizarTransaccion(dataEnviada, indice, recibida, TRANSACCION_CAJAS_IP, CANAL_DEMANDA,
                                      REVERSO_NO_GENERADO);
        retorno = retorno > 0 ? retorno : ERROR_NO_RESPONDE;
    } else {
        retorno = ERROR_NO_RESPONDE;
    }

    if (retorno > 0) {
        retorno = identificarRespuestaHttp(recibida  + indiceRecibido) == TRUE ? retorno : FALSE;
    }

    if (retorno > 0) {
        if (servicioWso2 == 0) {
            retorno = extraerDatosCaja(recibida + indiceRecibido, bufferCaja);
        } else {
            retorno = extraerDatosCajaWso2(recibida + indiceRecibido, bufferCaja);
        }
    }

    if (retorno > 0) {
            retorno = convertirCadenaHex(bufferCaja, datoshex2);
            retorno = unpackRespuestaCaja(datoshex2, retorno, TRANSACCION_TEFF_IDENTIFICAR_TRANSACCION_REALIZAR);

    } else {
        //mostrarMensajeSinRespuesta();
        retorno = ERROR_NO_RESPONDE;
    }

    if (retorno > 0) {
        retorno = cargarDatosPeticionTransaccion(datos, tipoTransaccionCaja);
    }

    return retorno;

}

int traerDatosConsumoRest(uChar *terminal, uChar *host) {

    int retorno = 0;
    uChar tcpIpActual[SIZE_DOMINIO + 1];

    memset(tcpIpActual, 0x00, sizeof(tcpIpActual));

    getParameter(URL_TEFF, tcpIpActual);

    retorno = strlen(tcpIpActual) > 0 ? TRUE : FALSE;

    if (retorno == 1) {
        strcpy(host, tcpIpActual);
        getParameter(NUMERO_TERMINAL, terminal);
        retorno = strlen(terminal) > 0 ? TRUE : FALSE;
    }

    return retorno;
}

int identificarRespuestaHttp(uChar *buffer) {
    return strncmp(buffer + 9, HTTP_EXITOSO, strlen(HTTP_EXITOSO)) == 0 ? TRUE : FALSE;
}

int desempaquetarMensajeIP(uChar *mensajeCaja, int tipoTransaccion) {

    int retorno = 0;
    uChar datoshex[512 + 1];
    uChar resultadoLRC = 0x00;

    memset(datoshex, 0x00, sizeof(datoshex));

    retorno = convertirCadenaHex(mensajeCaja, datoshex);

    if (retorno > 0) {

        if (datoshex[0] == STX) {
            resultadoLRC = calcularLRC(datoshex + 1, retorno - 2);
            //borrar
            retorno = resultadoLRC == datoshex[retorno - 1] ? retorno : FALSE;

            if (retorno == FALSE) {
                //screenMessageOptimizado("COMUNICACION", "MENSAJE", "DE CAJA", "INVALIDO (LRC)", 800);
            }
        } else {
            retorno = 0;
        }
    }

    if (retorno > 0) {
        retorno = armarTramaEstandarSolicitudCaja(datoshex, retorno);
        unpackMessageTEFF(datoshex, retorno);
    }

    if (retorno > 0) {
        if (tipoTransaccion == TRANSACCION_TEFF_EFECTIVO) {
            tipoTransaccion = TRANSACCION_TEFF_COMPRAS;
        }

        tipoTransaccion = tipoTransaccion == TRANSACCION_TEFF_EFECTIVO ? TRANSACCION_TEFF_COMPRAS : tipoTransaccion;
        retorno = validarCodigoTransaccionTEFF(tipoTransaccion);
    }

    return retorno;
}

int convertirCadenaHex(uChar *datosAscii, uChar *datosHex) {

    int i = 0;
    int j = 0;
    int longitud = 0;
    uChar caracter[2 + 1];

    memset(caracter, 0x00, sizeof(caracter));

    longitud = strlen(datosAscii);

    for (i = 0; i < longitud; i++) {

        if ((i % 2) == 0) {
            memset(caracter, 0x00, sizeof(caracter));
        }

        if (datosAscii[i] >= '0' && datosAscii[i] <= '9') {
            if ((i % 2) == 0) {
                caracter[0] = datosAscii[i] << 4;
            } else {
                caracter[0] = caracter[0] | (datosAscii[i] & 0x0F);
            }
        } else if (datosAscii[i] >= 'a' && datosAscii[i] <= 'f') {
            if ((i % 2) == 0) {
                caracter[0] = (datosAscii[i] - 0x57) << 4;
            } else {
                caracter[0] = caracter[0] | ((datosAscii[i] - 0x57) & 0x0F);
            }
        } else if (datosAscii[i] >= 'A' && datosAscii[i] <= 'F') {
            if ((i % 2) == 0) {
                caracter[0] = (datosAscii[i] - 0x37) << 4;
            } else {
                caracter[0] = caracter[0] | ((datosAscii[i] - 0x37) & 0x0F);
            }
        }

        if ((i % 2) != 0) {
            memcpy(datosHex + j, caracter, 1);
            j++;
        }
    }

    return j;
}

int extraerDatosCaja(uChar *buffer, uChar *bufferCaja) {

    int retorno = 0;
    char *p = NULL;

    retorno = verificarCodigoRespuestaCajasIP(buffer);

    if (retorno == 1) {
        p = strstr(buffer, "\"Cod:00,Msj:");
        retorno = strlen(p) > 0 ? strlen(p) - strlen("\"Cod:00,Msj:") - 1 : 0;
    }

    if (retorno > 0) {
        strncpy(bufferCaja, p + strlen("\"Cod:00,Msj:"), retorno);
    }

    return retorno;
}

int armarTramaEstandarSolicitudCaja(uChar *datoshex, int tam) {

    int indice = 0;
    uChar datoshexAux[512 + 1];
    uChar tamAscii[4 + 1];
    uChar tamBCD[2 + 1];

    memset(datoshexAux, 0x00, sizeof(datoshexAux));
    memset(tamAscii, 0x00, sizeof(tamAscii));
    memset(tamBCD, 0x00, sizeof(tamBCD));

    redefinirTamLlegada(datoshex);
    memcpy(datoshexAux, datoshex, tam);

    memset(datoshex, 0x00, tam);

    datoshex[indice] = ACK;
    indice++;

    memcpy(datoshex + indice, datoshexAux, 3);
    indice += 3;

    memcpy(datoshex + indice, "600000000011", strlen("600000000011"));
    indice += strlen("600000000011");

    memcpy(datoshex + indice, datoshexAux + 3, 2);
    indice += 2;

    memcpy(datoshex + indice, "000", 3);
    indice += 3;

    memcpy(datoshex + indice, datoshexAux + 5, tam - 5);
    indice += tam - 5;

    return indice;
}

int cargarDatosPeticionTransaccion(DatosVenta *datos, int tipoTransaccion) {

    int retorno = 0;

    switch (tipoTransaccion) {
        case TRANSACCION_TEFF_COMPRAS:
            retorno = cargarDatosVentaCajasIP(datos);
            break;
        case TRANSACCION_TEFF_ANULACIONES:
            retorno = cargarDatosAnulacionCajasIP(datos);
            break;
        case TRANSACCION_TEFF_HABILITACIONES:
            retorno = cargarDatosHabilitacionBonoCajasIP(datos);
            break;
        case TRANSACCION_TEFF_REDENCION:
            retorno = cargarDatosRedencionBonoCajasIP(datos);
            break;
        case TRANSACCION_TEFF_RECARGA_BONO:
            retorno = cargarDatosRecargaBonoCajasIP(datos);
            break;
        case TRANSACCION_TEFF_EFECTIVO:
            retorno = cargarDatosEfectivoCajasIP(datos);
            break;
        case TRANSACCION_TEFF_LEALTAD:
            retorno = cargarDatosLealtadCajasIP(datos);
            break;
        default:
            break;
    }

    return retorno;
}

int cargarDatosVentaCajasIP(DatosVenta *datos) {

    int retorno = 0;
    int resultadoCampo = 0;
    int respuestaAuxiliar = 0;
    char auxiliar[30 + 1];

    verificarLLegadaCodigoCajero(datos->codigoCajero);

    memset(auxiliar, 0x00, sizeof(auxiliar));
    cargarCamposEstructuraDatosVenta(auxiliar, FIELD_TEFF_MONTO);
    memcpy(datos->totalVenta, auxiliar, TAM_COMPRA_NETA);
    ajustarDatosTransaccion(datos->totalVenta, strlen(datos->totalVenta), DATOS_SALIDA);

    memset(auxiliar, 0x00, sizeof(auxiliar));
    cargarCamposEstructuraDatosVenta(auxiliar, FIELD_TEFF_IVA);
    memcpy(datos->iva, auxiliar, TAM_COMPRA_NETA);
    ajustarDatosTransaccion(datos->iva, strlen(datos->iva), DATOS_SALIDA);

    memset(auxiliar, 0x00, sizeof(auxiliar));
    cargarCamposEstructuraDatosVenta(auxiliar, FIELD_TEFF_BASE_DEVOLUCION);
    memcpy(datos->baseDevolucion, auxiliar, TAM_COMPRA_NETA);
    ajustarDatosTransaccion(datos->baseDevolucion, strlen(datos->baseDevolucion), DATOS_SALIDA);

    memset(auxiliar, 0x00, sizeof(auxiliar));
    cargarCamposEstructuraDatosVenta(auxiliar, FIELD_TEFF_PROPINA);
    memcpy(datos->propina, auxiliar, TAM_COMPRA_NETA);
    ajustarDatosTransaccion(datos->propina, TAM_COMPRA_NETA, DATOS_SALIDA);

    memset(auxiliar, 0x00, sizeof(auxiliar));
    cargarCamposEstructuraDatosVenta(datos->facturaCaja, FIELD_TEFF_NUMERO_FACTURA_ENVIADO_CAJA);

    ajustarDatosTransaccion(datos->facturaCaja, strlen(datos->facturaCaja), DATOS_SALIDA);
    memcpy(auxiliar, datos->facturaCaja, strlen(datos->facturaCaja));
    memset(datos->facturaCaja, 0x00, sizeof(datos->facturaCaja));

    _rightTrim_(datos->facturaCaja, auxiliar, ' ');

    if (datos->facturaCaja[0] == 0x20) {
        memset(datos->facturaCaja, 0x00, sizeof(datos->facturaCaja));
    }

    cargarCamposEstructuraDatosVenta(datos->inc, FIELD_TEFF_INC);
    ajustarDatosTransaccion(datos->inc, strlen(datos->inc), DATOS_SALIDA);

    resultadoCampo = cargarCamposEstructuraDatosVenta(auxiliar, FIELD_TEFF_NUMERO_CUENTA);

    if (resultadoCampo > 0) {
        memcpy(datos->tokenVivamos, auxiliar, SIZE_CUENTA);
    }

    retorno = verificarMontosInconsistentes(datos);

   /* if (verificarHabilitacionObjeto(TRANSACCION_VIVAMOS) == TRUE && retorno > 0) {
        respuestaAuxiliar = cargarCamposEstructuraDatosVenta(datos->tokenVivamos, FIELD_TEFF_DESCUENTO_TRANSACCION);
        if (respuestaAuxiliar == TRUE) {
            ajustarDatosTransaccion(datos->tokenVivamos, strlen(datos->tokenVivamos), DATOS_SALIDA);

        } else {
            memcpy(datos->tokenVivamos, "0", strlen("0"));
        }
    }*/

    if (retorno > 0) {
        retorno = validarCampo69(datos);
    }

    return retorno;
}

int respuestaCajaIP(DatosVenta datos, int tipoTransaccionCaja) {

    int retorno = 0;
    int indice = 0;
    int tpdu = 0;
    int indiceRecibido = 0;
    int retrasmicion = 0;
    int conttx = 0;
    int tamanoMensaje = 2;

    uChar bufferSalida[2500];
    uChar dataEnviada[2500];
    uChar recibida[2048];
    uChar terminal[SIZE_ID_TEFF + 1];
    uChar tcpIpActual[SIZE_DOMINIO + 1];
    uChar cajaRespuestaIP[1 + 1];
    char cajaFlujoMedio[2 + 1];
    char valRetrasmi[2 + 1];
    char valContador[2 + 1];
    char mensaje [20 + 1];
    char rechazo [20 + 1];

    char *mensajeRespuesta = NULL;

    int servicioWso2 = 0;

    memset(bufferSalida, 0x00, sizeof(bufferSalida));
    memset(dataEnviada, 0x00, sizeof(dataEnviada));
    memset(recibida, 0x00, sizeof(recibida));
    memset(terminal, 0x00, sizeof(terminal));
    memset(tcpIpActual, 0x00, sizeof(tcpIpActual));
    memset(cajaRespuestaIP, 0x00, sizeof(cajaRespuestaIP));
    memset(valRetrasmi, 0x00, sizeof(valRetrasmi));
    memset(rechazo, 0x00, sizeof(rechazo));
    memset(mensaje, 0x00, sizeof(mensaje));
    memset(valContador, 0x00, sizeof(valContador));

    memset(cajaFlujoMedio, 0x00, sizeof(cajaFlujoMedio));

    getParameter(RESPUESTA_CAJA_IP, cajaRespuestaIP);
    getParameter(CAJAS_IP_CORRESPONSAL, cajaFlujoMedio);

    if (atoi(cajaFlujoMedio) == 1) {
        //procesarArchivoConfiguraciones(valRetrasmi, 7);

        retrasmicion = atoi(valRetrasmi);
        if (retrasmicion > 0) {
            retrasmicion++;
        }
    }
    retorno = cargarDatosRespuestaIP(bufferSalida, datos, tipoTransaccionCaja);

    if (retorno > 0) {
        if (atoi(cajaRespuestaIP) == 1) {
            retorno = traerDatosConsumoRestRespuestaFinal(terminal, tcpIpActual);
        } else {
            retorno = traerDatosConsumoRest(terminal, tcpIpActual);
            indice = armarTpdu(dataEnviada, PRODUCTO_CAJA_IP);
            indiceRecibido = indice;
        }
    }
    if (indice > 4) {
        tpdu = 1;
        indiceRecibido += 2;
    }
    if (retorno > 0) {

        indice = armarRespuestaDatos(dataEnviada, bufferSalida, indice);

        if (tpdu == 1) {
            agregarLongitud(indice, dataEnviada);
            indice += 2;
        }

        if (atoi(cajaRespuestaIP) == 1) {
            //mostrarMensajeServicio();
        } else {
            //mostrarMensajeRespuesta();

        }

        borrarArchivo(discoNetcom, (char*)ARCHIVO_SINCRONIZACION_CAJA);
        //GTL_Traces_TraceDebugBuffer (1024, dataEnviada, "Netcom Respuesta caja : ");

        if (atoi(cajaFlujoMedio) == 0) {
            conttx = retrasmicion;
        }

        do {
            if (atoi(cajaFlujoMedio) == 1) {
                if (conttx == 0) {
                    //screenMessageOptimizado("COMUNICACION", "ENVIANDO", "RESPUESTA", "SERVIDOR", 1);
                } else {
                    sprintf(valContador, "%d", conttx);
                    //screenMessageOptimizado("COMUNICACION", "", "RETRASMITIENDO", valContador, 3 * 1000);
                }
            }
            retorno = realizarTransaccion(dataEnviada, indice, recibida, TRANSACCION_CAJAS_IP, CANAL_DEMANDA,
                                          REVERSO_NO_GENERADO);
            if (retorno > 0) {
                conttx = retrasmicion;
            } else {
                conttx++;
            }
        } while (conttx < retrasmicion);

        //GTL_Traces_TraceDebugBuffer (1024, recibida, "Netcom Respuesta Sip : ");
        retorno = retorno > 0 ? retorno : ERROR_NO_RESPONDE;
        if (retorno == ERROR_NO_RESPONDE && atoi(cajaFlujoMedio) == 1) {
            //screenMessageOptimizado("COMUNICACION", "OPERACION NO", "PUDO SER NOTIFICADA ", "AL WS", 2 * 1000);
        }
    } else {
        retorno = ERROR_CONFIGURACION;
    }

    if (retorno > 0) {
        retorno = identificarRespuestaHttp(recibida + indiceRecibido) == TRUE ? retorno : FALSE;
        if (retorno > 0) {
            mensajeRespuesta = strstr(recibida + indiceRecibido, HTTP_EXITOSO);
            strcpy(rechazo, "Rechazo:");
            memcpy(rechazo + 8, mensajeRespuesta, 4);
            mensajeRespuesta = NULL;
        }
    }

    if (retorno > 0) {
        if (servicioWso2 == 0) {
            retorno = verificarCodigoRespuestaCajasIP(recibida + indiceRecibido);
        } else {
            retorno = 1;
        }

        if (servicioWso2 == 0) {
            if (retorno > 0) {
                retorno = validarACKCajasIp(recibida + indiceRecibido);

                if (retorno <= 0 && tipoTransaccionCaja != TRANSACCION_TEFF_SINCRONIZACION) {
                    guardarTransaccionCajasSincronizacion(dataEnviada, strlen(dataEnviada));
                }
            } else {
                retorno = ERROR_NO_RESPONDE;
            }
        }
    } else {
        if (retorno <= 0 && retorno != ERROR_NO_RESPONDE && atoi(cajaFlujoMedio) == 1) {
            mensajeRespuesta = strstr(recibida + indiceRecibido, "\"Cod:") + strlen("\"Cod:");

            if (strlen(rechazo) == 0) {
                mensajeRespuesta =  (char*)" DESCONOCIDO";
                tamanoMensaje = strlen(mensajeRespuesta);
            }
            strcpy(mensaje, "Motivo:");
            memcpy(mensaje + 7, mensajeRespuesta, tamanoMensaje);
            //screenMessageOptimizado("SERVICIOS", rechazo, mensaje, "", 2 * 1500);
        }
    }

    if (retorno == ERROR_NO_RESPONDE) {

        borrarArchivo(discoNetcom,  (char*)ARCHIVO_SINCRONIZACION_CAJA);

        escribirArchivo(discoNetcom, (char*) ARCHIVO_SINCRONIZACION_CAJA, dataEnviada, strlen(dataEnviada));
        //mostrarMensajeSinRespuesta();
    }

    if (retorno == 0 &&  atoi(cajaFlujoMedio) == 1) {
        retorno = ERROR_NO_RESPONDE;
    }

    return retorno;
}

/**
 * @brief	convierte la buffer en su representacion ascii nibble a nibble
 * @param 	datosAscii
 * @param 	datosHex
 * @return	> 0, conversión exitosa
 * @author
 */
int convertirHexCadena(uChar *datosAscii, uChar *datosHex, int tamDatoshex) {

    int i = 0;
    int j = 0;

    for (i = 0; i < tamDatoshex; i++) {
        definirCaracterAscii(datosAscii + j, ((datosHex[i] & 0xF0) >> 4));
        j++;
        definirCaracterAscii(datosAscii + j, (datosHex[i] & 0x0F));
        j++;
    }

    return j;
}

void definirCaracterAscii(uChar *caracterAscii, uChar caracterhex) {

    int datoEntero = 0;

    datoEntero = (uChar) (caracterhex);

    if (datoEntero >= 0 && datoEntero <= 9) {
        sprintf(caracterAscii, "%d", datoEntero);
    } else {
        caracterAscii[0] = caracterhex + 0x37;
    }
}

int cargarDatosRespuestaIP(uChar *bufferSalida, DatosVenta datos, int tipoTransaccion) {

    int retorno = 0;
    uChar datoshex[2048];

    memset(datoshex, 0x00, sizeof(datoshex));

    switch (tipoTransaccion) {
        case TRANSACCION_TEFF_COMPRAS:
        case TRANSACCION_TEFF_RETIRO_SIN_TARJETA:
        case TRANSACCION_TEFF_PAGO_TCREDITO:
        case TRANSACCION_TEFF_PAGO_CARTERA:
        case TRANSACCION_TEFF_DEPOSITO_BCL:
        case TRANSACCION_TEFF_TRANSFERENCIA_BCL:
        case TRANSACCION_TEFF_RECAUDO_MANUAL:
        case TRANSACCION_TEFF_RECAUDO_LECTOR_BARRAS:
        case TRANSACCION_TEFF_RECAUDO_TEMPRESARIAL:
        case TRANSACCION_TEFF_ANULACION:
        case TRASACCION__TEFF_NOTIFICACION_SMS:
        case TRANSACCION_TEFF_ENVIAR_GIRO:
        case TRANSACCION_TEFF_RECLAMAR_GIRO:
        case TRANSACCION_TEFF_CANCELAR_GIRO:
        case TRANSACCION_TEFF_CONSULTA_SALDO_BCL:
            retorno = generarDatosRespuestaIPCompra(datoshex, datos, tipoTransaccion);
            break;
        case TRANSACCION_TEFF_CONSULTA_CUPO_BCL:
            retorno = transaccionRespuestaCupo(datoshex, datos,tipoTransaccion, 2);
            break;
        case TRANSACCION_TEFF_ANULACIONES:
            retorno = generarDatosRespuestaIPAnulacion(datoshex, datos);
            break;
        case TRANSACCION_TEFF_HABILITACIONES:
        case TRANSACCION_TEFF_REDENCION:
        case TRANSACCION_TEFF_RECARGA_BONO:
            retorno = generarDatosRespuestaIPBonos(datoshex, datos, tipoTransaccion);
            break;
        case TRANSACCION_TEFF_LEALTAD:
            retorno = generarDatosRespuestaIPLealtad(datoshex, datos);
            break;
        default:
            break;
    }

    if (retorno > 0) {
        retorno = convertirHexCadena(bufferSalida, datoshex, retorno);
    }

    return retorno;
}

int generarDatosRespuestaIPCompra(uChar *bufferSalida, DatosVenta datos, int transaccion) {

    char pan[LEN_TEFF_PAN + 1];
    char tipoCuenta[SIZE_TIPO_CUENTA + 1];
    char cajaCorresponsal[2 + 1];
    char transaccionTeff[2 + 1];
    uChar auxiliar[30 + 1];
    ResultISOPack packMessage;
    TablaUnoInicializacion tablaUno;
    int modoDatafono = 0;
    char firmaHabilitada[2 + 1];
    char compraReferenciada[2 + 1];
    char catalogos[1 + 1];
    char outBuffer[100 + 1];

    memset(pan, 0x00, sizeof(pan));
    memset(tipoCuenta, 0x00, sizeof(tipoCuenta));
    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(&packMessage, 0x00, sizeof(packMessage));
    memset(&tablaUno, 0x00, sizeof(tablaUno));
    memset(compraReferenciada, 0x00, sizeof(compraReferenciada));
    memset(firmaHabilitada, 0x00, sizeof(firmaHabilitada));
    memset(catalogos, 0x00, sizeof(catalogos));
    memset(outBuffer, 0x00, sizeof(outBuffer));

    getParameter(HABILITA_FIRMA_IP, firmaHabilitada);
    getParameter(CAJAS_IP_CORRESPONSAL, cajaCorresponsal);
    getParameter(COMPRA_CON_REFERENCIA, compraReferenciada);
    getParameter(CATALOGOS_DIGITALES, catalogos);

    modoDatafono = verificarModoCNB();

    if ((modoDatafono == MODO_MIXTO)
        && (T_BANCOLOMBIA(transaccion) || strncmp(datos.codigoGrupo, OPERACION_RETIRO_PRINCIPAL, 2) == 0
            || strncmp(datos.codigoGrupo, OPERACION_RETIRO_SECUNDARIA, 2) == 0
            || strncmp(datos.codigoGrupo, OPERACION_ANULACION_BANCOL, 2) == 0)) {
        tablaUno = desempaquetarCNB();
    } else {
        tablaUno = _desempaquetarTablaCeroUno_();
    }

    if (atoi(cajaCorresponsal) == 1) {

        if (transaccion == TRANSACCION_TEFF_DEPOSITO_BCL) {
            strcat(datos.totalVenta, "00");
            strcat(datos.compraNeta, "00");
            strcat(datos.iva, "00");
        }
    }

    setTipoMensajeTEFF(MENSAJE_TEFF_RESPUESTA);

    validarPosEntryMode(2, datos.posEntrymode);

    if (transaccion == TRASACCION__TEFF_NOTIFICACION_SMS) {
        setCodigoTransaccionTEFF(TRASACCION__TEFF_NOTIFICACION_SMS);
    } else {
        setCodigoTransaccionTEFF(TRANSACCION_TEFF_COMPRAS);
    }

    validacionCodigoRespuestaCaja(datos.codigoRespuesta);
    setCodigoRespuestaTEFF(atoi(datos.codigoRespuesta));

    setFieldTEFF(FIELD_TEFF_CODIGO_RESPUESTA, datos.codigoRespuesta, LEN_TEFF_CODIGO_RESPUESTA);

    ajustarDatosTransaccion(datos.codigoAprobacion, LEN_TEFF_CODIGO_AUTORIZACION, DATOS_ENTRADA);

    setFieldTEFF(FIELD_TEFF_CODIGO_AUTORIZACION, datos.codigoAprobacion, LEN_TEFF_CODIGO_AUTORIZACION);

    generarDatosRespuestaAdicionales(datos);

    if (atoi(datos.track2) != 0) {
        extraerPanAsteriscos(datos.track2, strlen(datos.track2), pan,
                             ASTERISCOS_4_ULTIMOS);
    } else if (atoi(datos.tipoTransaccion) == 20) {
        memcpy(pan, datos.aux1 + 2, 6);
        strcat(pan, "**");
        memcpy(pan + 8, datos.aux1 + 8, 2);
    } else {
        memcpy(pan, "000000**0000", LEN_TEFF_PAN);
    }

    if (transaccion != TRANSACCION_TEFF_RECAUDO_MANUAL && transaccion != TRANSACCION_TEFF_RECAUDO_LECTOR_BARRAS) {
        setFieldTEFF(FIELD_TEFF_ASTERISCOS_4_ULTIMOS_TARJETA, pan, LEN_TEFF_ASTERISCOS_4_ULTIMOS_TARJETA);
    } else if (atoi(cajaCorresponsal) == 1) {
        setFieldTEFF(FIELD_TEFF_ASTERISCOS_4_ULTIMOS_TARJETA, pan, LEN_TEFF_ASTERISCOS_4_ULTIMOS_TARJETA);
    }

    if (atoi(catalogos) == 1) {

        leerArchivo(discoNetcom, (char*)LECTURA_CATALOGOS_REFERENCIADA, outBuffer);
        if (strlen(outBuffer) > 0) {
            borrarArchivo(discoNetcom, (char*)LECTURA_CATALOGOS_REFERENCIADA);
            memcpy(auxiliar, outBuffer, strlen(outBuffer));
            setFieldTEFF(FIELD_TEFF_CODIGO_CAJERO, auxiliar, strlen(auxiliar));
        }
    } else {
        if (verificarCodigoCajero() > 0 && strcmp(datos.codigoCajero, NO_IMPRIMIR_CAJERO) != 0) {
            setFieldTEFF(FIELD_TEFF_CODIGO_CAJERO, datos.codigoCajero, LEN_TEFF_CODIGO_CAJERO);
        }
    }

    if (transaccion != TRANSACCION_TEFF_RECAUDO_MANUAL && transaccion != TRANSACCION_TEFF_RECAUDO_LECTOR_BARRAS) {
        memcpy(tipoCuenta, datos.tipoCuenta, LEN_TEFF_TIPO_CUENTA);
        tiposDeCuentaCaja(tipoCuenta, atoi(datos.creditoRotativo));
        setFieldTEFF(FIELD_TEFF_TIPO_CUENTA, tipoCuenta, LEN_TEFF_TIPO_CUENTA);

        if (transaccion != TRANSACCION_TEFF_ENVIAR_GIRO && transaccion != TRANSACCION_TEFF_RECLAMAR_GIRO
            && transaccion != TRANSACCION_TEFF_CANCELAR_GIRO) {
            if (atoi(datos.codigoRespuesta) == 0) {
                setFieldTEFF(FIELD_TEFF_FRANQUICIA, datos.cardName, LEN_TEFF_FRANQUICIA);
            }
        }
    }

    ajustarDatosTransaccion(datos.totalVenta, LEN_TEFF_MONTO, DATOS_ENTRADA_MONTOS);

    setFieldTEFF(FIELD_TEFF_MONTO, datos.totalVenta, LEN_TEFF_MONTO);

    if ((transaccion != TRANSACCION_TEFF_DEPOSITO_BCL && transaccion != TRANSACCION_TEFF_RETIRO_SIN_TARJETA
         && transaccion != TRANSACCION_TEFF_TRANSFERENCIA_BCL && transaccion != TRANSACCION_TEFF_PAGO_CARTERA
         && transaccion != TRANSACCION_TEFF_PAGO_TCREDITO && transaccion != TRANSACCION_TEFF_RECAUDO_MANUAL
         && transaccion != TRANSACCION_TEFF_RECAUDO_LECTOR_BARRAS
         && transaccion != TRANSACCION_TEFF_RECAUDO_TEMPRESARIAL && transaccion != TRANSACCION_TEFF_ENVIAR_GIRO
         && transaccion != TRANSACCION_TEFF_RECLAMAR_GIRO && transaccion != TRANSACCION_TEFF_CANCELAR_GIRO)
        && atoi(cajaCorresponsal) == 0) {
        ajustarDatosTransaccion(datos.iva, LEN_TEFF_IVA, DATOS_ENTRADA_MONTOS);
        setFieldTEFF(FIELD_TEFF_IVA, datos.iva, LEN_TEFF_IVA);


            ajustarDatosTransaccion(datos.baseDevolucion, LEN_TEFF_BASE_DEVOLUCION,
                                    DATOS_ENTRADA_MONTOS);
            setFieldTEFF(FIELD_TEFF_BASE_DEVOLUCION, datos.baseDevolucion,
                         LEN_TEFF_BASE_DEVOLUCION);
            ajustarDatosTransaccion(datos.inc, LEN_TEFF_INC, DATOS_ENTRADA_MONTOS);
            setFieldTEFF(FIELD_TEFF_INC, datos.inc, LEN_TEFF_INC);
            ajustarDatosTransaccion(datos.propina, LEN_TEFF_PROPINA, DATOS_ENTRADA_MONTOS);
            setFieldTEFF(FIELD_TEFF_PROPINA, datos.propina, LEN_TEFF_PROPINA);



    }

    if (transaccion == TRANSACCION_TEFF_TRANSFERENCIA_BCL && atoi(datos.codigoRespuesta) == 0) {
        setFieldTEFF(FIELD_TEFF_NUMERO_CUENTA, datos.textoAdicional, strlen(datos.textoAdicional));
        setFieldTEFF(FIELD_TEFF_NUMERO_FACTURA, datos.nombreComercio, strlen(datos.nombreComercio));
    }

    if (transaccion == TRANSACCION_TEFF_RECAUDO_MANUAL && atoi(datos.codigoRespuesta) == 0) {
        setFieldTEFF(FIELD_TEFF_CODIGO_OPERADOR, datos.codigoComercio, strlen(datos.codigoComercio));
        setFieldTEFF(FIELD_TEFF_CUOTAS, datos.facturaCaja, strlen(datos.facturaCaja));
    } else if ((transaccion == TRANSACCION_TEFF_RECAUDO_LECTOR_BARRAS
                || transaccion == TRANSACCION_TEFF_RECAUDO_TEMPRESARIAL) && atoi(datos.codigoRespuesta) == 0) {
        setFieldTEFF(FIELD_TEFF_CODIGO_OPERADOR, datos.codigoComercio, strlen(datos.codigoComercio));
        memset(auxiliar, 0x00, sizeof(auxiliar));

        leftTrim(auxiliar, datos.tokenVivamos, ' ');
        setFieldTEFF(FIELD_TEFF_CUOTAS, auxiliar, strlen(auxiliar));

    } else if (verificarAnulacionBclRecaudo(transaccion, datos.tipoTransaccion) && atoi(datos.codigoRespuesta) == 0) {
        setFieldTEFF(FIELD_TEFF_CODIGO_CONVENIO, datos.codigoComercio, strlen(datos.codigoComercio));

        memset(auxiliar, 0x00, sizeof(auxiliar));
        leftTrim(auxiliar, datos.tokenVivamos, ' ');
        setFieldTEFF(FIELD_TEFF_CUOTAS, auxiliar, strlen(auxiliar));
    }

    ajustarDatosTransaccion(datos.numeroRecibo, LEN_TEFF_NUMERO_RECIBO_DATAFONO, DATOS_ENTRADA);
    setFieldTEFF(FIELD_TEFF_NUMERO_RECIBO_DATAFONO, datos.numeroRecibo, LEN_TEFF_NUMERO_RECIBO_DATAFONO);

    if (atoi(compraReferenciada) == 1) {
        setFieldTEFF(FIELD_TEFF_NUMERO_FACTURA_ENVIADO_CAJA, datos.facturaCaja, strlen(datos.facturaCaja));
    }

    if ((transaccion != TRANSACCION_TEFF_DEPOSITO_BCL && transaccion != TRANSACCION_TEFF_RETIRO_SIN_TARJETA
         && transaccion != TRANSACCION_TEFF_TRANSFERENCIA_BCL && transaccion != TRANSACCION_TEFF_PAGO_CARTERA
         && transaccion != TRANSACCION_TEFF_PAGO_TCREDITO && transaccion != TRANSACCION_TEFF_RECAUDO_MANUAL
         && transaccion != TRANSACCION_TEFF_RECAUDO_LECTOR_BARRAS
         && transaccion != TRANSACCION_TEFF_RECAUDO_TEMPRESARIAL && transaccion != TRANSACCION_TEFF_ENVIAR_GIRO
         && transaccion != TRANSACCION_TEFF_RECLAMAR_GIRO && transaccion != TRANSACCION_TEFF_CANCELAR_GIRO)
        && atoi(cajaCorresponsal) == 0) {
        leftPad(auxiliar, datos.cuotas, '0', 2);
        memset(datos.cuotas, 0x00, sizeof(datos.cuotas));
        strcpy(datos.cuotas, auxiliar);
        ajustarDatosTransaccion(datos.cuotas, LEN_TEFF_CUOTAS, DATOS_ENTRADA);
        setFieldTEFF(FIELD_TEFF_CUOTAS, datos.cuotas, LEN_TEFF_CUOTAS);
    }

    if (atoi(cajaCorresponsal) == 1) {
        sprintf(transaccionTeff, "%d", transaccion);
        if (transaccion == 0) {
            strcat(transaccionTeff, "0");
        }
        setFieldTEFF(FIELD_TEFF_MODO_FLUJO, "MF", LEN_TEFF_MODO_FLUJO);
        setFieldTEFF(FIELD_TEFF_TIPO_DOCUMENTO, transaccionTeff, LEN_TEFF_CUOTAS);
        armarCamposGiros(transaccion, datos);
    }

    if (atoi(datos.codigoRespuesta) == 0) {

        ajustarDatosTransaccion(datos.rrn, LEN_TEFF_RRN, DATOS_ENTRADA);

        setFieldTEFF(FIELD_TEFF_RRN, datos.rrn, LEN_TEFF_RRN);

        if (transaccion == TRANSACCION_TEFF_RECAUDO_LECTOR_BARRAS || transaccion == TRANSACCION_TEFF_RECAUDO_MANUAL) {
            setFieldTEFF(FIELD_TEFF_CODIGO_CORRESPONSAL,  (char*)"00", LEN_TEFF_CODIGO_BANCO);

        }

        ajustarDatosTransaccion(datos.terminalId, LEN_TEFF_NUMERO_TERMINAL, DATOS_ENTRADA);
        setFieldTEFF(FIELD_TEFF_NUMERO_TERMINAL, datos.terminalId, LEN_TEFF_NUMERO_TERMINAL);

        _rightPad_(auxiliar, tablaUno.receiptLine3, ' ', LEN_TEFF_CODIGO_ESTABLECIMIENTO);
        setFieldTEFF(FIELD_TEFF_CODIGO_ESTABLECIMIENTO, auxiliar, LEN_TEFF_CODIGO_ESTABLECIMIENTO);

        if (transaccion == TRANSACCION_TEFF_RETIRO_SIN_TARJETA) {
            setFieldTEFF(FIELD_TEFF_NUMERO_CUENTA, datos.tokenVivamos, 11);
        } else if (transaccion == TRANSACCION_TEFF_DEPOSITO_BCL
                   || verificarAnulacionBclDeposito(transaccion, datos.tipoTransaccion)) {
            ajustarDatosTransaccion(datos.propina, SIZE_CUENTA, DATOS_ENTRADA);
            setFieldTEFF(FIELD_TEFF_NUMERO_CUENTA, datos.propina, SIZE_CUENTA);
        }

        if (transaccion == TRANSACCION_TEFF_PAGO_TCREDITO || transaccion == TRANSACCION_TEFF_PAGO_CARTERA
            || transaccion == TRANSACCION_TEFF_RECAUDO_LECTOR_BARRAS
            || transaccion == TRANSACCION_TEFF_RECAUDO_MANUAL
            || transaccion == TRANSACCION_TEFF_RECAUDO_TEMPRESARIAL
            || verificarAnulacionBclPago(transaccion, datos.tipoTransaccion) == TRUE) {
            setFieldTEFF(FIELD_TEFF_NUMERO_FACTURA, datos.tokenVivamos, strlen(datos.tokenVivamos));
        }
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
    packMessage = packMessageTEFF();

    memcpy(bufferSalida, packMessage.isoPackMessage, packMessage.totalBytes);

    return packMessage.totalBytes;
}

int armarTramaRespuestaCajaIP(uChar *datosHex, int tam) {

    int indiceDatosHex = 0;
    int indiceDatosHexAux = 0;
    uChar datosHexAux[2500 + 1];

    memset(datosHexAux, 0x00, sizeof(datosHexAux));

    memcpy(datosHexAux, datosHex, tam);

    memset(datosHex, 0x00, tam);

    memcpy(datosHex, datosHexAux, 3);
    indiceDatosHex += 3;
    indiceDatosHexAux += 3;
    indiceDatosHexAux += 12;
    memcpy(datosHex + indiceDatosHex, datosHexAux + indiceDatosHexAux, 3);
    indiceDatosHex += 3;
    indiceDatosHexAux += 3;
    indiceDatosHexAux++;
    memcpy(datosHex + indiceDatosHex, datosHexAux + indiceDatosHexAux, tam - indiceDatosHexAux);
    indiceDatosHex = tam - 13;

    redefinirLongitudLRC(datosHex, indiceDatosHex);

    return indiceDatosHex;
}

int redefinirLongitudLRC(uChar *dataHex, int tam) {

    int retorno = 0;
    uChar longitudBuffer[2 + 1];
    uChar longitudString[4 + 1];

    memset(longitudBuffer, 0x00, sizeof(longitudBuffer));
    memset(longitudString, 0x00, sizeof(longitudString));

    if (dataHex[0] == ACK) {
        sprintf(longitudString, "%04d", tam - 6);
    } else {
        sprintf(longitudString, "%04d", tam - 5);
    }

    _convertASCIIToBCD_(longitudBuffer, longitudString, 4);

    if (dataHex[0] == ACK) {
        memcpy(dataHex + 2, longitudBuffer, 2);
    } else {
        memcpy(dataHex + 1, longitudBuffer, 2);
    }

    if (dataHex[0] == ACK) {
        dataHex[tam - 1] = calcularLRC(dataHex + 2, tam - 3);
    } else {
        dataHex[tam - 1] = calcularLRC(dataHex + 1, tam - 2);
    }

    return retorno;
}

int verificarCodigoRespuestaCajasIP(uChar *buffer) {
    int resultado = 0;
    char *ret;
    ret = strstr(buffer, "\"Cod:");

    if (ret != NULL) {
        resultado =	strncmp(strstr(buffer, "\"Cod:") + strlen("\"Cod:"), "00", 2)== 0 ? 1 : 0;
    }

    return resultado;
}

int validarACKCajasIp(uChar *buffer) {

    return strncmp(strstr(buffer, "\"Cod:00,Msj:") + strlen("\"Cod:00,Msj:"), "06", 2) == 0 ? TRUE : FALSE;
}

int verificarCofiguracionURI(void) {

    int retorno = 0;
    char tcpIpActual[SIZE_DOMINIO + 1];
    uChar tipoCaja[1 + 1];

    memset(tipoCaja, 0x00, sizeof(tipoCaja));

    getParameter(TIPO_CAJA, tipoCaja);

    memset(tcpIpActual, 0x00, sizeof(tcpIpActual));
    getParameter(URL_TEFF, tcpIpActual);
    retorno = strlen(tcpIpActual) > 0 ? TRUE : FALSE;

    if (retorno && strcmp(tipoCaja, CAJA_IP) == 0) {
        memset(tcpIpActual, 0x00, sizeof(tcpIpActual));
        getParameter(IP_CAJAS, tcpIpActual);
        retorno = strlen(tcpIpActual) > 0 ? TRUE : FALSE;
    }

    if (retorno != TRUE) {
        //screenMessageOptimizado("MENSAJE", "CONFIGURE", "PARAMETOS", "CAJAS IP", 800);
        retorno = ERROR_CONFIGURACION;
    }

    return retorno;
}

void redefinirTamLlegada(uChar *bufferLlegada) {

    int tamReal = 0;
    uChar tamAscii[4 + 1];
    uChar tamBCD[2 + 1];

    memset(tamAscii, 0x00, sizeof(tamAscii));
    memset(tamBCD, 0x00, sizeof(tamBCD));

    memcpy(tamBCD, bufferLlegada + 1, 2);

    _convertBCDToASCII_(tamAscii, tamBCD, 4);

    tamReal = atoi(tamAscii) + strlen("600000000011000");

    memset(tamAscii, 0x00, sizeof(tamAscii));
    sprintf(tamAscii, "%04d", tamReal);
    _convertASCIIToBCD_(tamBCD, tamAscii, 4);

    memcpy(bufferLlegada + 1, tamBCD, 2);
}

int cargarDatosAnulacionCajasIP(DatosVenta *datos) {

    int claveValida = 0;
    int respuesta = 0;
    char claveSupervisor[SIZE_CLAVE_SUPERVIDOR + 1];
    char auxiliar[30 + 1];

    memset(claveSupervisor, 0x00, sizeof(claveSupervisor));
    memset(auxiliar, 0x00, sizeof(auxiliar));

    cargarCamposEstructuraDatosVenta(claveSupervisor, FIELD_TEFF_CLAVE_SUPERVISOR);
    respuesta = validarClaveSupervisor(claveSupervisor, TRANSACCION_TEFF_ANULACIONES);
    claveValida = respuesta;

    if (respuesta > 0) {

        verificarLLegadaCodigoCajero(datos->codigoCajero);
        memset(auxiliar, 0x00, sizeof(auxiliar));
        cargarCamposEstructuraDatosVenta(auxiliar, FIELD_TEFF_NUMERO_RECIBO_DATAFONO);
        _rightTrim_(datos->numeroRecibo, auxiliar, ' ');
        ajustarDatosTransaccion(datos->numeroRecibo, strlen(datos->numeroRecibo), DATOS_SALIDA);
        memset(auxiliar, 0x00, sizeof(auxiliar));
        cargarCamposEstructuraDatosVenta(auxiliar, FIELD_TEFF_NUMERO_FACTURA_ENVIADO_CAJA);
        memcpy(datos->facturaCaja, auxiliar, strlen(auxiliar));
        ajustarDatosTransaccion(datos->facturaCaja, strlen(datos->facturaCaja), DATOS_SALIDA);
    }

    if (claveValida != TRUE) {
        respuesta = respuestaClaveInvalidaIP(TRANSACCION_TEFF_ANULACIONES);
        respuesta = respuesta > 0 ? ERROR_NO_RESPONDE : respuesta;
    }

    return respuesta;
}

int generarDatosRespuestaIPAnulacion(uChar *bufferSalida, DatosVenta datos) {

    char pan[LEN_TEFF_PAN + 1];
    char tipoCuenta[TAM_TIPO_CUENTA + 1];
    TablaUnoInicializacion tablaUno;
    ResultISOPack packMessage;

    memset(pan, 0x00, sizeof(pan));
    memset(tipoCuenta, 0x00, sizeof(tipoCuenta));
    memset(&tablaUno, 0x00, sizeof(tablaUno));
    memset(&packMessage, 0x00, sizeof(packMessage));

    setCodigoTransaccionTEFF(TRANSACCION_TEFF_ANULACIONES);
    validacionCodigoRespuestaCaja(datos.codigoRespuesta);
    setCodigoRespuestaTEFF(atoi(datos.codigoRespuesta));
    tablaUno = _desempaquetarTablaCeroUno_();

    setFieldTEFF(FIELD_TEFF_CODIGO_RESPUESTA, datos.codigoRespuesta, LEN_TEFF_CODIGO_RESPUESTA);

    ajustarDatosTransaccion(datos.codigoAprobacion, LEN_TEFF_CODIGO_AUTORIZACION, DATOS_ENTRADA);
    setFieldTEFF(FIELD_TEFF_CODIGO_AUTORIZACION, datos.codigoAprobacion, LEN_TEFF_CODIGO_AUTORIZACION);

    generarDatosRespuestaAdicionales(datos);
    extraerPanAsteriscos(datos.track2, strlen(datos.track2), pan, ASTERISCOS_4_ULTIMOS);

    setFieldTEFF(FIELD_TEFF_ASTERISCOS_4_ULTIMOS_TARJETA, pan, LEN_TEFF_ASTERISCOS_4_ULTIMOS_TARJETA);

    if (verificarCodigoCajero() > 0) {

        setFieldTEFF(FIELD_TEFF_CODIGO_CAJERO, datos.codigoCajero, LEN_TEFF_CODIGO_CAJERO);
    }

    memcpy(tipoCuenta, datos.tipoCuenta, LEN_TEFF_TIPO_CUENTA);
    tiposDeCuentaCaja(tipoCuenta, atoi(datos.creditoRotativo));
    setFieldTEFF(FIELD_TEFF_TIPO_CUENTA, tipoCuenta, LEN_TEFF_TIPO_CUENTA);
    setFieldTEFF(FIELD_TEFF_FRANQUICIA, datos.cardName, LEN_TEFF_FRANQUICIA);
    ajustarDatosTransaccion(datos.totalVenta, LEN_TEFF_MONTO, DATOS_ENTRADA_MONTOS);
    setFieldTEFF(FIELD_TEFF_MONTO, datos.totalVenta, LEN_TEFF_MONTO);

//	if (transaccion != TRANSACCION_TEFF_DEPOSITO_BCL) {

    ajustarDatosTransaccion(datos.iva, LEN_TEFF_MONTO, DATOS_ENTRADA_MONTOS);
    setFieldTEFF(FIELD_TEFF_IVA, datos.iva, LEN_TEFF_IVA);
    ajustarDatosTransaccion(datos.baseDevolucion, LEN_TEFF_MONTO, DATOS_ENTRADA_MONTOS);
    setFieldTEFF(FIELD_TEFF_BASE_DEVOLUCION, datos.baseDevolucion, LEN_TEFF_BASE_DEVOLUCION);
//	}

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

    memcpy(bufferSalida, packMessage.isoPackMessage, packMessage.totalBytes);

    return packMessage.totalBytes;
}

int cargarDatosHabilitacionBonoCajasIP(DatosVenta *datos) {

    int respuesta = 0;
    int claveValida = 0;
    char claveSupervisor[SIZE_CLAVE_SUPERVIDOR + 1];

    memset(claveSupervisor, 0x00, sizeof(claveSupervisor));

    cargarCamposEstructuraDatosVenta(claveSupervisor, FIELD_TEFF_CLAVE_SUPERVISOR);
    cargarCamposEstructuraDatosVenta(datos->totalVenta, FIELD_TEFF_MONTO);
    ajustarDatosTransaccion(datos->totalVenta, strlen(datos->totalVenta), DATOS_SALIDA);

    verificarLLegadaCodigoCajero(datos->codigoCajero);

    claveValida = respuesta;

    if (claveValida != TRUE) {
        respuesta = respuestaClaveInvalidaIP(TRANSACCION_TEFF_ANULACIONES);
        respuesta = respuesta > 0 ? ERROR_NO_RESPONDE : respuesta;
    }
    return respuesta;
}

int generarDatosRespuestaIPBonos(uChar *bufferSalida, DatosVenta datos, int transaccionBono) {

    char pan[LEN_TEFF_PAN + 1];
    char bufferOut[512 + 1];
    char tipoCuenta[2 + 1];
    char tipoCuentaAuxiliar[2 + 1];
    char auxiliar[30 + 1];
    char auxiliarMontos[15 + 1];
    char numeroBono[LEN_TEFF_NUMERO_BONO + 1];
    char franquicia[LEN_TEFF_FRANQUICIA + 1];
    TablaUnoInicializacion tablaUno;
    ResultISOPack packMessage;
    ResultISOPack packMessageRespaldo;

    memset(pan, 0x00, sizeof(pan));
    memset(&tablaUno, 0x00, sizeof(tablaUno));
    memset(&packMessage, 0x00, sizeof(packMessage));
    memset(&packMessageRespaldo, 0x00, sizeof(packMessageRespaldo));
    memset(bufferOut, 0x00, sizeof(bufferOut));
    memset(tipoCuenta, 0x00, sizeof(tipoCuenta));
    memset(tipoCuentaAuxiliar, 0x00, sizeof(tipoCuentaAuxiliar));
    memset(auxiliar, 0x30, sizeof(auxiliar));
    memset(auxiliarMontos, 0x00, sizeof(auxiliarMontos));
    memset(numeroBono, 0x00, sizeof(numeroBono));
    memset(franquicia, 0x00, sizeof(franquicia));

    tablaUno = _desempaquetarTablaCeroUno_();

    setTipoMensajeTEFF(MENSAJE_TEFF_RESPUESTA);
    setCodigoTransaccionTEFF(transaccionBono);
    validacionCodigoRespuestaCaja(datos.codigoRespuesta);
    setCodigoRespuestaTEFF(atoi(datos.codigoRespuesta));
    setFieldTEFF(FIELD_TEFF_CODIGO_RESPUESTA, datos.codigoRespuesta, LEN_TEFF_CODIGO_RESPUESTA);
    ajustarDatosTransaccion(datos.codigoAprobacion, LEN_TEFF_CODIGO_AUTORIZACION, DATOS_ENTRADA);
    setFieldTEFF(FIELD_TEFF_CODIGO_AUTORIZACION, datos.codigoAprobacion, LEN_TEFF_CODIGO_AUTORIZACION);

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

    ////////////////////////////////////////////////////////////////
    memcpy(bufferSalida, packMessage.isoPackMessage, packMessage.totalBytes);

    return packMessage.totalBytes;
}

int generarTramaPeticionIP(uChar *bufferSalida, DatosVenta datos, int tipoTransaccionCaja) {

    int retorno = 0;
    uChar datoshex[256];

    memset(datoshex, 0x00, sizeof(datoshex));

    switch (tipoTransaccionCaja) {
        case TRANSACCION_TEFF_COMPRAS:
            retorno = generarTramaPeticionIPCompra(datoshex, datos);
            break;
        case TRANSACCION_TEFF_ANULACIONES:
            retorno = generarTramaPeticionIPAnulacion(datoshex, datos);
            break;
        case TRANSACCION_TEFF_HABILITACIONES:
            retorno = generarTramaPeticionIPHabilitacionBono(datoshex, datos);
            break;
        case TRANSACCION_TEFF_REDENCION:
            retorno = generarTramaPeticionRedencionBono(datoshex, datos);
            break;
        case TRANSACCION_TEFF_RECARGA_BONO:
            retorno = generarTramaPeticionIPRecargaBono(datoshex, datos);
            break;
        case TRANSACCION_TEFF_EFECTIVO:
            retorno = generarTramaPeticionIPEfectivo(datoshex, datos);
            break;
        case TRANSACCION_TEFF_LEALTAD:
            retorno = generarTramaPeticionIPLealtad(datoshex, datos);
            break;
        default:
            break;
    }

    if (retorno > 0) {
        retorno = convertirHexCadena(bufferSalida, datoshex, retorno);
    }

    return retorno;
}

int generarTramaPeticionIPCompra(uChar *bufferSalida, DatosVenta datos) {

    char pan[LEN_TEFF_PAN + 1];
    char descuento[4 + 1];
    ResultISOPack packMessage;
    char auxiliar[30 + 1];
    char compraReferencia[1 + 1];
    char catalogos[1 + 1];
    char outBuffer[100 + 1];

    memset(pan, 0x00, sizeof(pan));
    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(&packMessage, 0x00, sizeof(packMessage));
    memset(descuento, 0x00, sizeof(descuento));
    memset(compraReferencia, 0x00, sizeof(compraReferencia));
    memset(catalogos, 0x00, sizeof(catalogos));
    memset(outBuffer, 0x00, sizeof(outBuffer));

    getParameter(COMPRA_CON_REFERENCIA, compraReferencia);
    getParameter(CATALOGOS_DIGITALES, catalogos);

    setTipoMensajeTEFF(MENSAJE_TEFF_REQUERIMIENTO);
    setCodigoTransaccionTEFF(TRANSACCION_TEFF_COMPRAS);
    setCodigoRespuestaTEFF(NO_HAY_CODIGO_DE_RESPUESTA);

    validarPosEntryMode(2, datos.posEntrymode);

    if (atoi(datos.track2) != 0) {
        extraerPanAsteriscos(datos.track2, strlen(datos.track2), pan, ASTERISCOS_4_ULTIMOS);
    } else {
        memcpy(pan, "000000**0000", LEN_TEFF_PAN);
    }

    setFieldTEFF(FIELD_TEFF_ASTERISCOS_4_ULTIMOS_TARJETA, pan, LEN_TEFF_PAN);

    if (atoi(catalogos) == 1) {

        leerArchivo(discoNetcom, (char*)LECTURA_CATALOGOS_REFERENCIADA, outBuffer);

        if (strlen(outBuffer) > 0) {
            memcpy(auxiliar, outBuffer, strlen(outBuffer));
        }
    }

    if (verificarCodigoCajero() > 0) {
        setFieldTEFF(FIELD_TEFF_CODIGO_CAJERO, auxiliar, strlen(auxiliar));
    }

    if (atoi(compraReferencia) == 1) {
        setFieldTEFF(FIELD_TEFF_REFERENCIA_COMPRA, datos.codigoCajero, 10);
        setFieldTEFF(FIELD_TEFF_CODIGO_ESTABLECIMIENTO, datos.codigoComercio, 10);
    }

    setFieldTEFF(FIELD_TEFF_MONTO, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_IVA, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_BASE_DEVOLUCION, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_INC, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_NUMERO_FACTURA_ENVIADO_CAJA, auxiliar, 0);

    if (labelEspecialesDescuento(datos.track2) == TRUE) {
        setFieldTEFF(FIELD_TEFF_DESCUENTO_TRANSACCION, auxiliar, 0);
    }

    packMessage = packMessageTEFF();

    memcpy(bufferSalida, packMessage.isoPackMessage, packMessage.totalBytes);

    return packMessage.totalBytes;
}

int generarTramaPeticionIPAnulacion(uChar *bufferSalida, DatosVenta datos) {

    char pan[LEN_TEFF_PAN + 1];
    char auxiliar[30 + 1];
    char bufferOut[512 + 1];
    char claveSupervisor[4 + 1];
    ResultISOPack packMessage;

    memset(pan, 0x00, sizeof(pan));
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

    memcpy(bufferSalida, packMessage.isoPackMessage, packMessage.totalBytes);

    return packMessage.totalBytes;
}

int generarTramaPeticionIPHabilitacionBono(uChar *bufferSalida, DatosVenta datos) {

    char pan[LEN_TEFF_PAN + 1];
    char auxiliar[30 + 1];
    char claveSupervisor[4 + 1];
    ResultISOPack packMessage;

    memset(pan, 0x00, sizeof(pan));
    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(&packMessage, 0x00, sizeof(packMessage));
    memset(claveSupervisor, 0x00, sizeof(claveSupervisor));

    setTipoMensajeTEFF(MENSAJE_TEFF_REQUERIMIENTO);
    setCodigoTransaccionTEFF(TRANSACCION_TEFF_HABILITACIONES);
    setCodigoRespuestaTEFF(NO_HAY_CODIGO_DE_RESPUESTA);

    if (verificarCodigoCajero() > 0) {
        setFieldTEFF(FIELD_TEFF_CODIGO_CAJERO, auxiliar, 0);
    }

    setFieldTEFF(FIELD_TEFF_CLAVE_SUPERVISOR, auxiliar, 0);

    /*if (verificarHabilitacionObjeto(HABILITACION_BONOS_KIOSCO) == TRUE) {
        setFieldTEFF(FIELD_TEFF_MONTO, auxiliar, 0);
    }*/

    packMessage = packMessageTEFF();

    memcpy(bufferSalida, packMessage.isoPackMessage, packMessage.totalBytes);

    return packMessage.totalBytes;
}

void sincronizacionIP(void) {

    int retorno = 0;
    char dataEnviada[1024 + 1];
    uChar recibida[2500 + 1];
    uChar bufferSalida[512];
    uChar terminal[SIZE_ID_TEFF + 1];
    uChar tcpIpActual[SIZE_DOMINIO + 1];

    memset(dataEnviada, 0x00, sizeof(dataEnviada));
    memset(recibida, 0x00, sizeof(recibida));
    memset(bufferSalida, 0x00, sizeof(bufferSalida));
    memset(terminal, 0x00, sizeof(terminal));
    memset(tcpIpActual, 0x00, sizeof(tcpIpActual));

    retorno = verificarArchivo(discoNetcom, ARCHIVO_SINCRONIZACION_CAJA);

    if (retorno > 0) {

        retorno = traerDatosConsumoRest(terminal, tcpIpActual);

        if (retorno > 0) {

            retorno = realizarTransaccion(bufferSalida, strlen(bufferSalida), recibida, TRANSACCION_CAJAS_IP,
                                          CANAL_DEMANDA, REVERSO_NO_GENERADO);

            retorno = retorno > 0 ? retorno : ERROR_NO_RESPONDE;
        } else {
            retorno = ERROR_CONFIGURACION;
        }

        if (retorno > 0) {
            retorno = identificarRespuestaHttp(recibida) == TRUE ? retorno : FALSE;
        }

        if (retorno > 0) {
            retorno = validarACKCajasIp(recibida);
            retorno = retorno >= 0 ? retorno : ERROR_NO_RESPONDE;
        }

        if (retorno == ERROR_NO_RESPONDE) {
            //mostrarMensajeSinRespuesta();
        } else {
            borrarArchivo(discoNetcom,  (char*)ARCHIVO_SINCRONIZACION_CAJA);
        }
    } else {
       // mostrarMensajeSinDatosSincronizacion();
    }
}

int generarTramaPeticionRedencionBono(uChar *bufferSalida, DatosVenta datos) {

    char auxiliar[30 + 1];
    ResultISOPack packMessage;

    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(&packMessage, 0x00, sizeof(packMessage));

    setTipoMensajeTEFF(MENSAJE_TEFF_REQUERIMIENTO);
    setCodigoTransaccionTEFF(TRANSACCION_TEFF_REDENCION);
    setCodigoRespuestaTEFF(NO_HAY_CODIGO_DE_RESPUESTA);

    if (verificarCodigoCajero() > 0) {
        setFieldTEFF(FIELD_TEFF_CODIGO_CAJERO, auxiliar, 0);
    }

    setFieldTEFF(FIELD_TEFF_MONTO, auxiliar, 0);

    packMessage = packMessageTEFF();

    memcpy(bufferSalida, packMessage.isoPackMessage, packMessage.totalBytes);

    return packMessage.totalBytes;
}

int cargarDatosRedencionBonoCajasIP(DatosVenta *datos) {

    int retorno = -1;

    verificarLLegadaCodigoCajero(datos->codigoCajero);
    cargarCamposEstructuraDatosVenta(datos->totalVenta, FIELD_TEFF_MONTO);
    ajustarDatosTransaccion(datos->totalVenta, strlen(datos->totalVenta), DATOS_SALIDA);

    if (atol(datos->totalVenta) > 0) {
        retorno = 1;
    } else {
        memcpy(datos->codigoRespuesta, "01", TAM_CODIGO_RESPUESTA);
        //screenMessageOptimizado("COMUNICACION", "MONTOS", "INCONSISTENTES", "", T_OUT(1));
    }
    return retorno;
}

int generarTramaPeticionIPRecargaBono(uChar *bufferSalida, DatosVenta datos) {

    char auxiliar[30 + 1];

    ResultISOPack packMessage;

    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(&packMessage, 0x00, sizeof(packMessage));

    setTipoMensajeTEFF(MENSAJE_TEFF_REQUERIMIENTO);
    setCodigoTransaccionTEFF(TRANSACCION_TEFF_RECARGA_BONO);
    setCodigoRespuestaTEFF(NO_HAY_CODIGO_DE_RESPUESTA);

    setFieldTEFF(FIELD_TEFF_MONTO, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_CLAVE_SUPERVISOR, auxiliar, 0);

    packMessage = packMessageTEFF();

    memcpy(bufferSalida, packMessage.isoPackMessage, packMessage.totalBytes);

    return packMessage.totalBytes;
}

int cargarDatosRecargaBonoCajasIP(DatosVenta *datos) {

    int respuesta = 0;
    int claveValida = 0;
    uChar claveSupervisor[SIZE_CLAVE_SUPERVIDOR + 1];

    memset(claveSupervisor, 0x00, sizeof(claveSupervisor));

    cargarCamposEstructuraDatosVenta(claveSupervisor, FIELD_TEFF_CLAVE_SUPERVISOR);
    respuesta = validarClaveSupervisor(claveSupervisor, TRANSACCION_TEFF_RECARGA_BONO);
    claveValida = respuesta;
    memcpy(datos->codigoRespuesta, "01", 2);

    if (respuesta > 0) {
        verificarLLegadaCodigoCajero(datos->codigoCajero);

        cargarCamposEstructuraDatosVenta(datos->totalVenta, FIELD_TEFF_MONTO);
        ajustarDatosTransaccion(datos->totalVenta, strlen(datos->totalVenta), DATOS_SALIDA);
    }

    if (claveValida != TRUE) {
        respuesta = respuestaClaveInvalidaIP(TRANSACCION_TEFF_ANULACIONES);
        respuesta = respuesta > 0 ? ERROR_NO_RESPONDE : respuesta;
    }
    return respuesta;
}

void verificarLLegadaCodigoCajero(uChar *codigoCajero) {

    int existeCodigo = FALSE;
    char auxiliar[TAM_CODIGO_CAJERO + 1];
    char auxiliar2[TAM_CODIGO_CAJERO + 1];
    int i = 0;

    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(auxiliar2, 0x00, sizeof(auxiliar2));

    cargarCamposEstructuraDatosVenta(auxiliar, FIELD_TEFF_CODIGO_CAJERO);

    for (i = 0; i < strlen(auxiliar); i++) {
        if (auxiliar[i] != 0x20) {
            existeCodigo = TRUE;
            break;
        }
    }

    if (existeCodigo) {
        leftTrim(auxiliar2, auxiliar, ' ');
        memset(auxiliar, 0x00, sizeof(auxiliar));
        memcpy(codigoCajero, auxiliar2, strlen(auxiliar2));
    } else {

        strcpy(codigoCajero, NO_IMPRIMIR_CAJERO);
    }
}

int respuestaClaveInvalidaIP(int tipoTransaccion) {

    int retorno = 0;
    int indice = 0;
    int tpdu = 0;
    int indiceRecibido = 0;
    uChar datoshex[256];
    uChar bufferSalida[512];
    uChar dataEnviada[512];
    uChar recibida[2048];
    uChar terminal[SIZE_ID_TEFF + 1];
    uChar tcpIpActual[SIZE_DOMINIO + 1];
    ResultISOPack packMessage;

    memset(datoshex, 0x00, sizeof(datoshex));
    memset(bufferSalida, 0x00, sizeof(bufferSalida));
    memset(dataEnviada, 0x00, sizeof(dataEnviada));
    memset(recibida, 0x00, sizeof(recibida));
    memset(terminal, 0x00, sizeof(terminal));
    memset(tcpIpActual, 0x00, sizeof(tcpIpActual));
    memset(&packMessage, 0x00, sizeof(packMessage));

    packMessage = tramaRespuestaClaveInvalida(tipoTransaccion);
    retorno = packMessage.totalBytes;

    if (retorno > 0) {
        memcpy(datoshex, packMessage.isoPackMessage, packMessage.totalBytes);
        retorno = armarTramaRespuestaCajaIP(datoshex, retorno);
    }

    if (retorno > 0) {
        retorno = convertirHexCadena(bufferSalida, datoshex, retorno);
    }

    if (retorno > 0) {
        retorno = traerDatosConsumoRest(terminal, tcpIpActual);
    }

    if (retorno > 0) {

        indice = armarTpdu(dataEnviada, PRODUCTO_CAJA_IP);
        indiceRecibido = indice;
        if (indice > 4) {
            tpdu = 1;
            indiceRecibido += 2;
        }
        indice = armarRespuestaDatos(dataEnviada, bufferSalida, indice);

        if (tpdu == 1) {
            agregarLongitud(indice, dataEnviada);
            indice += 2;
        }
        //mostrarMensajeRespuesta();

        borrarArchivo(discoNetcom,  (char*)ARCHIVO_SINCRONIZACION_CAJA);

        retorno = realizarTransaccion(dataEnviada, indice, recibida, TRANSACCION_CAJAS_IP, CANAL_DEMANDA,
                                      REVERSO_NO_GENERADO);
        retorno = retorno > 0 ? retorno : ERROR_NO_RESPONDE;
    } else {
        retorno = ERROR_CONFIGURACION;
    }

    if (retorno > 0) {
        retorno = identificarRespuestaHttp(recibida + indiceRecibido) == TRUE ? retorno : FALSE;
    }
    if (retorno > 0) {
        retorno = verificarCodigoRespuestaCajasIP(recibida + indiceRecibido);

        if (retorno > 0) {
            retorno = validarACKCajasIp(recibida + indiceRecibido);

            if (retorno <= 0) {
                guardarTransaccionCajasSincronizacion(dataEnviada, strlen(dataEnviada));
            }
        } else {
            retorno = ERROR_NO_RESPONDE;
        }
    }

    if (retorno == ERROR_NO_RESPONDE) {

        borrarArchivo(discoNetcom,  (char*)ARCHIVO_SINCRONIZACION_CAJA);

        escribirArchivo(discoNetcom,  (char*)ARCHIVO_SINCRONIZACION_CAJA, dataEnviada, strlen(dataEnviada));
    }

    return retorno;
}

int generarTramaPeticionIPEfectivo(uChar *bufferSalida, DatosVenta datos) {

    char pan[LEN_TEFF_PAN + 1];
    char auxiliar[30 + 1];
    ResultISOPack packMessage;

    memset(pan, 0x00, sizeof(pan));
    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(&packMessage, 0x00, sizeof(packMessage));

    setTipoMensajeTEFF(MENSAJE_TEFF_REQUERIMIENTO);
    setCodigoTransaccionTEFF(TRANSACCION_TEFF_COMPRAS);
    setCodigoRespuestaTEFF(NO_HAY_CODIGO_DE_RESPUESTA);

    extraerPanAsteriscos(datos.track2, strlen(datos.track2), pan, ASTERISCOS_4_ULTIMOS);
    setFieldTEFF(FIELD_TEFF_ASTERISCOS_4_ULTIMOS_TARJETA, pan, LEN_TEFF_PAN);
    setFieldTEFF(FIELD_TEFF_MONTO, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_IVA, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_BASE_DEVOLUCION, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_INC, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_NUMERO_FACTURA_ENVIADO_CAJA, auxiliar, 0);

    packMessage = packMessageTEFF();

    memcpy(bufferSalida, packMessage.isoPackMessage, packMessage.totalBytes);

    return packMessage.totalBytes;
}

int cargarDatosEfectivoCajasIP(DatosVenta *datos) {

    int respuesta = 0;
    long compraNeta = 0;
    char auxiliarMonto[TAM_COMPRA_NETA + 1];

    memset(auxiliarMonto, 0x00, sizeof(auxiliarMonto));

    verificarLLegadaCodigoCajero(datos->codigoCajero);

    respuesta = cargarCamposEstructuraDatosVenta(datos->totalVenta, FIELD_TEFF_MONTO);
    ajustarDatosTransaccion(datos->totalVenta, strlen(datos->totalVenta), DATOS_SALIDA);

    if (respuesta > 0) {
        sprintf(auxiliarMonto, "%ld", compraNeta);
        memcpy(datos->compraNeta, auxiliarMonto, 12);
        respuesta = cargarCamposEstructuraDatosVenta(datos->iva, FIELD_TEFF_IVA);
        ajustarDatosTransaccion(datos->iva, strlen(datos->iva), DATOS_SALIDA);
    }

    if (respuesta > 0) {
        respuesta = cargarCamposEstructuraDatosVenta(datos->baseDevolucion, FIELD_TEFF_BASE_DEVOLUCION);
        ajustarDatosTransaccion(datos->baseDevolucion, strlen(datos->baseDevolucion), DATOS_SALIDA);
    }

    if (respuesta > 0) {
        respuesta = cargarCamposEstructuraDatosVenta(datos->facturaCaja, FIELD_TEFF_NUMERO_FACTURA_ENVIADO_CAJA);
        ajustarDatosTransaccion(datos->facturaCaja, strlen(datos->facturaCaja), DATOS_SALIDA);
    }

    if (respuesta > 0) {
        respuesta = cargarCamposEstructuraDatosVenta(datos->inc, FIELD_TEFF_INC);
        ajustarDatosTransaccion(datos->inc, strlen(datos->inc), DATOS_SALIDA);
    }

    if (respuesta > 0) {
        respuesta = verificarMontosInconsistentes(datos);
    }

    return respuesta;
}

int generarTramaPeticionIPLealtad(uChar *bufferSalida, DatosVenta datos) {

    char isPuntosColombia[2 + 1];
    uChar auxiliar[30 + 1];
    ResultISOPack resultIsoPack;

    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(isPuntosColombia, 0x00, sizeof(isPuntosColombia));

    getParameter(PRODUCTO_PUNTOS_COLOMBIA, isPuntosColombia);

    setTipoMensajeTEFF(MENSAJE_TEFF_REQUERIMIENTO);
    setCodigoTransaccionTEFF(TRANSACCION_TEFF_LEALTAD);
    setCodigoRespuestaTEFF(NO_HAY_CODIGO_DE_RESPUESTA);

    setFieldTEFF(FIELD_TEFF_MONTO, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_IVA, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_INC, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_NUMERO_DOCUMENTO, auxiliar, 0);
    setFieldTEFF(FIELD_TEFF_CLAVE_SUPERVISOR, auxiliar, 0);

    resultIsoPack = packMessageTEFF();

    memcpy(bufferSalida, resultIsoPack.isoPackMessage, resultIsoPack.totalBytes);
    return resultIsoPack.totalBytes;
}

int cargarDatosLealtadCajasIP(DatosVenta *datos) {

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

    verificarLLegadaCodigoCajero(datos->codigoCajero);

    cargarCamposEstructuraDatosVenta(claveSupervisor, FIELD_TEFF_CLAVE_SUPERVISOR);
    retorno = validarClaveSupervisor(claveSupervisor, TRANSACCION_TEFF_LEALTAD);
    claveValida = retorno;

    if (retorno > 0) {
        cargarCamposLealtad(datos);
        retorno = verificarMontosInconsistentes(datos);

        /*if (verificarHabilitacionObjeto(TRANSACCION_VIVAMOS) == TRUE && retorno > 0) {
            retornoAux = cargarCamposEstructuraDatosVenta(datos->tokenVivamos, FIELD_TEFF_DESCUENTO_TRANSACCION);
            if (retornoAux == TRUE) {
                ajustarDatosTransaccion(datos->tokenVivamos, strlen(datos->tokenVivamos), DATOS_SALIDA);
            } else {
                memcpy(datos->tokenVivamos, "0", strlen("0"));
            }
        }*/

        if (validarDescuentosCaja(datos->tokenVivamos, descuento) == TRUE && retorno > 0) {
            memset(datos->estadoTarifa, 0x00, sizeof(datos->estadoTarifa));
            memcpy(datos->estadoTarifa, "DT", 2);
        }
    }

    if (claveValida != TRUE) {
        retorno = respuestaClaveInvalidaIP(TRANSACCION_TEFF_ANULACIONES);
        retorno = retorno > 0 ? ERROR_NO_RESPONDE : retorno;
    }

    return retorno;
}

int generarDatosRespuestaIPLealtad(uChar *bufferSalida, DatosVenta datos) {

    char pan[LEN_TEFF_PAN + 1];
    char tipoCuenta[2 + 1];
    char auxiliar[30 + 1];
    TablaUnoInicializacion tablaUno;
    ResultISOPack packMessage;

    ResultISOPack packMessageRespaldo;

    memset(pan, 0x00, sizeof(pan));
    memset(&tablaUno, 0x00, sizeof(tablaUno));
    memset(&packMessage, 0x00, sizeof(packMessage));
    memset(&packMessageRespaldo, 0x00, sizeof(packMessageRespaldo));
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
    generarDatosRespuestaAdicionales(datos);
    extraerPanAsteriscos(datos.track2, strlen(datos.track2), pan, ASTERISCOS_4_ULTIMOS);

    setFieldTEFF(FIELD_TEFF_ASTERISCOS_4_ULTIMOS_TARJETA, pan, LEN_TEFF_ASTERISCOS_4_ULTIMOS_TARJETA);

    if (verificarCodigoCajero() > 0) {
        setFieldTEFF(FIELD_TEFF_CODIGO_CAJERO, datos.codigoCajero, LEN_TEFF_CODIGO_CAJERO);
    }

    ajustarDatosTransaccion(datos.totalVenta, LEN_TEFF_MONTO, DATOS_ENTRADA_MONTOS);
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
    memcpy(bufferSalida, packMessage.isoPackMessage, packMessage.totalBytes);

    return packMessage.totalBytes;
}

int solicitudFlujoUnificado(int origenConsulta, uChar *monto, int *cantidadFacturasRecaudo) {

    char terminal[SIZE_ID_TEFF + 1];
    char tcpIpActual[SIZE_DOMINIO + 1];
    char auxiliar[30 + 1];
    char datoshex[256];
    char dataEnviada[512];
    char dataTransaccion[1024];
    uChar bufferCaja[1024];
    uChar recibida[1500];
    char cantidadFacturas[2 + 1];
    char catalogos[1 + 1];
    char outBuffer [100 + 1];
    int respuesta = 0;
    char datoshex2[256];
    int servicioWso2 = 0;
    int indice = 0;
    int indiceRecibido = 0;
    int tpdu = 0;
    int contador = 0;
    int noResponder = 0;
    ResultISOPack packMessage;

    int tipoTransaccionCaja = 0;

    memset(terminal, 0x00, sizeof(terminal));
    memset(tcpIpActual, 0x00, sizeof(tcpIpActual));
    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(&packMessage, 0x00, sizeof(packMessage));
    memset(datoshex, 0x00, sizeof(datoshex));
    memset(dataEnviada, 0x00, sizeof(dataEnviada));
    memset(dataTransaccion, 0x00, sizeof(dataTransaccion));
    memset(bufferCaja, 0x00, sizeof(bufferCaja));
    memset(recibida, 0x00, sizeof(recibida));
    memset(cantidadFacturas, 0x00, sizeof(cantidadFacturas));
    memset(catalogos, 0x00, sizeof(catalogos));
    memset(outBuffer,0x00,sizeof(outBuffer));
    memset(datoshex2, 0x00, sizeof(datoshex2));

    respuesta = traerDatosConsumoRest(terminal, tcpIpActual);

    getParameter(CATALOGOS_DIGITALES, catalogos);

    if (respuesta > 0) {

        setTipoMensajeTEFF(MENSAJE_TEFF_REQUERIMIENTO);
        setCodigoTransaccionTEFF(TRANSACCION_TEFF_IDENTIFICAR_TRANSACCION_REALIZAR);
        setCodigoRespuestaTEFF(NO_HAY_CODIGO_DE_RESPUESTA);

        if (atoi(catalogos) == 1) {
               //INTENTAR INVOCAR LA CAMARA DESDE AQUI


            if (strlen(outBuffer) > 0) {
                borrarArchivo(discoNetcom, (char*)LECTURA_CATALOGOS_REFERENCIADA);
                escribirArchivo(discoNetcom, (char*)LECTURA_CATALOGOS_REFERENCIADA, outBuffer, strlen(outBuffer));
                memcpy(auxiliar, outBuffer, strlen(outBuffer));
                respuesta = 1;
            } else {
                respuesta = 0;
            }
        }

        if (respuesta > 0) {

            validarPosEntryMode(1, NULL);

            if (verificarCodigoCajero() > 0) {
                setFieldTEFF(FIELD_TEFF_CODIGO_CAJERO, auxiliar, 0);
            }

            setFieldTEFF(FIELD_TEFF_MONTO, auxiliar, 0);
            setFieldTEFF(FIELD_TEFF_TIPO_DOCUMENTO, auxiliar, 0);

            if (origenConsulta == CONSULTA_MULTICORRESPONSAL) {
                setFieldTEFF(FIELD_TEFF_NUMERO_FACTURA, auxiliar, 0);
            }

            setParameter(CAJA_NO_RESPONDE,  (char*)"0");
            packMessage = packMessageTEFF();
            memcpy(datoshex, packMessage.isoPackMessage, packMessage.totalBytes);
            respuesta = packMessage.totalBytes;

        }

        if (respuesta > 0) {
            respuesta = convertirHexCadena(dataTransaccion, datoshex, respuesta);
        }

        indice = armarTpdu(dataEnviada, PRODUCTO_CAJA_IP);
        indiceRecibido = indice;
        if (indice > 4) {
            tpdu = 1;
            indiceRecibido += 2;
        }
        if (respuesta) {
            indice = armarPeticionDatos(dataEnviada, dataTransaccion, indice);

            if (tpdu == 1) {
                agregarLongitud(indice, dataEnviada);
                indice += 2;
            }

//            mostrarMensajeConectando();
//			GTL_Traces_TraceDebugBuffer (512, dataEnviada, "Netcom Envio caja ip : ");
            respuesta = realizarTransaccion(dataEnviada, indice, recibida, TRANSACCION_CAJAS_IP,
                                            CANAL_DEMANDA, REVERSO_NO_GENERADO);
            respuesta = respuesta > 0 ? respuesta : ERROR_NO_RESPONDE;
        } else {
            respuesta = ERROR_NO_RESPONDE;
        }

        if (respuesta > 0) {
            respuesta = identificarRespuestaHttp(recibida + indiceRecibido) == TRUE ? respuesta : FALSE;
        }

        if (respuesta > 0) {
            if (servicioWso2 == 0) {
                respuesta = extraerDatosCaja(recibida + indiceRecibido, bufferCaja);
            } else {
                respuesta = extraerDatosCajaWso2(recibida + indiceRecibido, bufferCaja);
            }
        }

        if (respuesta > 0) {

                respuesta = convertirCadenaHex(bufferCaja, datoshex2);
                respuesta = unpackRespuestaCaja(datoshex2, respuesta,
                                                TRANSACCION_TEFF_IDENTIFICAR_TRANSACCION_REALIZAR);

        } else {
            if (noResponder != 1) {
                respuesta = ERROR_NO_RESPONDE;
            } else {
                //screenMessageOptimizado("COMUNICACION", "TRANSACCION", "CANCELADA", "", 500);
            }
        }

        if (respuesta > 0) {

            cargarCamposEstructuraDatosVenta(monto, FIELD_TEFF_MONTO);
            ajustarDatosTransaccion(monto, strlen(monto), DATOS_SALIDA);

            cargarCamposEstructuraDatosVenta(cantidadFacturas, FIELD_TEFF_NUMERO_FACTURA);
            ajustarDatosTransaccion(cantidadFacturas, strlen(cantidadFacturas), DATOS_SALIDA);

            *cantidadFacturasRecaudo = atoi(cantidadFacturas);

            respuesta = getCodigoTransaccionTEFF();

            switch (respuesta) {
                case TRANSACCION_TEFF_SINCRONIZACION:
                    respuesta = TRANSACCION_TEFF_LEALTAD_LIFEMILES;
                    break;
                case TRANSACCION_TEFF_LEALTAD:
                    respuesta = TRANSACCION_TEFF_ACUMULACION_LIFEMILLES;
                    break;
                case TRANSACCION_TEFF_ACUMULACION_PUNTOS_COL:
                    respuesta = TRANSACCION_TEFF_ACUMULACION_PUNTOSCOL;
                    break;
                default:
                    respuesta = validarCampoCorresponsal(FIELD_TEFF_TIPO_DOCUMENTO);
                    if (respuesta == 99) {
                        respuesta = TRANSACCION_TEFF_EFECTIVO;
                    } else if (respuesta == 96) {
                        //screenMessage("COMUNICACION", "CAJA", "OPERACION NO", "VALIDA", 2 * 1000);
                        respuesta = -1;
                    } else if (respuesta == 9) {
                        respuesta = TRANSACCION_TEFF_LEALTAD_LIFEMILES;
                    } else if (respuesta == 31) {
                        respuesta = TRANSACCION_TEFF_COMPRA_REFERENCIA;
                    } else {
                        respuesta = TRANSACCION_TEFF_COMPRAS;
                    }
                    break;
            }
        }

    }
    return respuesta;
}

int verificarCofiguracionURIFirma(void) {

    int retorno = 0;
    char tcpIpActual[SIZE_DOMINIO + 1];
    uChar numeroTerminalActual[SIZE_ID_TEFF + 1];
    uChar tipoCaja[1 + 1];

    memset(tipoCaja, 0x00, sizeof(tipoCaja));

    getParameter(TIPO_CAJA, tipoCaja);
    getParameter(ID_FIRMA, numeroTerminalActual); //CAJA_DOMINIO // CAJA_IP

    retorno = strlen(numeroTerminalActual) > 0 ? TRUE : FALSE;

    if (retorno) {
        memset(tcpIpActual, 0x00, sizeof(tcpIpActual));
        getParameter(URL_FIRMA, tcpIpActual);
        retorno = strlen(tcpIpActual) > 0 ? TRUE : FALSE;
    }

    if (retorno && strcmp(tipoCaja, CAJA_IP) == 0) {
        memset(tcpIpActual, 0x00, sizeof(tcpIpActual));
        getParameter(IP_FIRMA, tcpIpActual);
        retorno = strlen(tcpIpActual) > 0 ? TRUE : FALSE;
    }

    if (retorno != TRUE) {
        //screenMessageOptimizado("MENSAJE", "CONFIGURE", "PARAMETOS", "CAJAS IP", 800);
        retorno = ERROR_CONFIGURACION;
    }

    return retorno;
}

/**
 * Funcion que prepara el envio de la firma por un servicio web
 * @param datos
 * @param tipoTransaccionCaja
 * @return
 */

int envioFirmaIP(DatosVenta datos, int tipoTransaccionCaja ,int sincronizacion) {

    int retorno = 0;
    int crearRv = 0;
    uChar bufferSalida[2500];
    uChar terminal[SIZE_ID_TEFF + 1];
    uChar recibo[SIZE_ID_TEFF + 1];
    uChar tcpIpActual[SIZE_DOMINIO + 1];
    char panelDeFirma[2 + 1];
    int error = 0;

    int partes = 0;
    int iCode = 0;
    char firmaIp[1 + 1];
    char firmoOK[1 + 1];
    char valRetrasmi[2 + 1];
    char storedAndForward[2 + 1];

    memset(firmoOK, 0x00, sizeof(firmoOK));
    memset(bufferSalida, 0x00, sizeof(bufferSalida));
    memset(terminal, 0x00, sizeof(terminal));
    memset(recibo, 0x00, sizeof(recibo));
    memset(tcpIpActual, 0x00, sizeof(tcpIpActual));
    memset(firmaIp, 0x00, sizeof(firmaIp));
    memset(panelDeFirma, 0x00, sizeof(panelDeFirma));
    memset(storedAndForward, 0x30, sizeof(storedAndForward));
    memset(valRetrasmi, 0x00, sizeof(valRetrasmi));

    getParameter(HABILITA_FIRMA_IP, firmaIp);

    //procesarArchivoConfiguraciones(valRetrasmi, 7);

    if (atoi(valRetrasmi) > 0) {
        //procesarArchivoConfiguraciones(storedAndForward, 8);
    }

    memcpy(recibo, datos.numeroRecibo, TAM_NUMERO_RECIBO); //Prueba_recibo
    if (atoi(firmaIp) == 1) {
        setParameter(HABILITA_FIRMA_IP,  (char*)"3");
        //retorno = cargarDatosRespuestaIP(bufferSalida, datos, tipoTransaccionCaja);
        retorno = armarReciboDinamico(bufferSalida, datos, tipoTransaccionCaja);

    } else {
        //screenMessageOptimizado("SERVIDOR DE FIRMA", "ERROR DE CONEXION", "CON EL SERVIDOR", "REVISE PARAMETROS", 3 * 1500);
    }

    if (retorno > 0) {
        retorno = traerDatosConsumoRestFirma(terminal, tcpIpActual);
    }

    if (strlen(datos.terminalId) < 8) {
        getParameter(NUMERO_TERMINAL, datos.terminalId);
    }
    getParameter(IMPRIMIR_PANEL_FIRMA, panelDeFirma);
    setParameter(IMPRIMIR_PANEL_FIRMA,  (char*)"0");

    if (retorno > 0) {
        if (atoi(panelDeFirma) == 1) {
            crearRv = 1;
        } else {
            //screenMessageOptimizado("COMUNICACION", "ENVIANDO", "RECIBO AL ", "SERVIDOR", 1);
        }
        borrarArchivo(discoNetcom,  (char*)REVERSO_FIRMA_IP);
        escribirArchivo(discoNetcom,  (char*)REVERSO_FIRMA_IP, (char*)&datos, sizeof(DatosVenta));
        retorno = respuestaFirmaIP(bufferSalida, datos.terminalId, tcpIpActual, crearRv);
    } else {
        retorno = ERROR_CONFIGURACION;
        //screenMessageOptimizado("SERVIDOR DE FIRMA", "ERROR DE CONEXION", "CON EL SERVIDOR", "REVISE PARAMETROS", 3 * 1500);
    }

    if (retorno > 0) {
        if (atol(panelDeFirma) == 1) {
            if (sincronizacion == 0) {
              //PANTALLA DE PANEL DE FIRMA
            }
        }

        if (atol(panelDeFirma) == 1) {
            partes = verificarFirma(firmoOK);

            if (partes > 0 && atoi(firmoOK) == 1) {

                retorno = respuestaFirmaIP(bufferSalida, datos.terminalId, tcpIpActual, 0);
            } else {
                retorno = ERROR_NO_FIRMO;
            }
        }
    }
    if (retorno > 0 && atoi(panelDeFirma) == 1) {
        retorno = solicitarEnviarFirma(datos.terminalId, recibo, tipoTransaccionCaja, partes, datos.posEntrymode);
    }
    if (retorno > 0) {
        borrarArchivo( (char*)"/HOST",  (char*)"/HOST/SIGNA.BMP");
        borrarArchivo(discoNetcom, (char*) RECIBO_DIGITAL);
    }
    if ((retorno <= 0 && atoi(storedAndForward) == 0)
        || (retorno != ERROR_NO_RESPONDE && retorno <= 0 && atoi(storedAndForward) == 1)) {

        error = retorno;
        retorno = generarReverso();
        if (retorno > 0) {
            reversarTransaccionFirma(error);
        }
        borrarArchivo( (char*)"/HOST",  (char*)"/HOST/SIGNA.BMP");
        retorno = ERROR_NO_RESPONDE;
    } else if (retorno == ERROR_NO_RESPONDE) {
        setParameter(HABILITA_FIRMA_IP,  (char*)"1");
        renombrarFileASincronizar(1);
        if (sincronizacion == 0) {
            retorno = 1;
        }
    }
    return retorno;
}

int factorizarTrama(uChar *auxiliar, int dato) {

    int idx = 0;
    int counter = 0;
    int contandoCeros = 0;
    int contadorF = 0;
    char datoMenor[4 + 1];
    char factorMenor[1 + 1];
    uChar auxiliar2[27000 + 1 ];
    char datoComprimido[10 + 1];
    unsigned char factor = '0';

    memset(auxiliar2, 0x00, sizeof(auxiliar2));
    memset(datoMenor, 0x00, sizeof(datoMenor));
    memset(factorMenor, 0x00, sizeof(factorMenor));

    factor = '0';
    strcpy(factorMenor, "0");
    for (idx = 0; idx < strlen(auxiliar); idx++) {
        memset(datoComprimido, 0x00, sizeof(datoComprimido));

        if (auxiliar[idx]  == 'F') {
            contadorF++;
        }

        if (auxiliar[idx] == factor) {
            counter++;
            if (counter > 3) {
                contandoCeros = 1;
            } else {
                strcat(datoMenor, factorMenor);
            }
        } else {
            if (contandoCeros == 1) {
                contandoCeros = 0;
                memset(datoMenor, 0x00, sizeof(datoMenor));
                sprintf(datoComprimido, "(%d)", counter);
                counter = 0;
                idx--;
                strcat(auxiliar2, datoComprimido);
            } else if (counter > 0 && counter < 4) {
                strcat(auxiliar2, datoMenor);
                memset(datoMenor, 0x00, sizeof(datoMenor));
                counter = 0;
                idx--;
            } else {
                datoComprimido[0] = auxiliar[idx];
                strcat(auxiliar2, datoComprimido);
            }
            contandoCeros = 0;
        }

    }
    if (contandoCeros == 1 && dato == 1) {
        contandoCeros = 0;
        memset(datoMenor, 0x00, sizeof(datoMenor));
        if (dato == 1) {
            sprintf(datoComprimido, "(%d)", counter);
        }
        strcat(auxiliar2, datoComprimido);
        counter = 0;
    } else if (strlen(datoMenor) > 0 && contandoCeros == 0) {
        strcat(auxiliar2, datoMenor);
    }

    memset(auxiliar, 0x00, sizeof(auxiliar));
    memcpy(auxiliar, auxiliar2, sizeof(auxiliar2));
    memset(auxiliar2, 0x00, sizeof(auxiliar2));
    memset(datoMenor, 0x00, sizeof(datoMenor));
    memset(factorMenor, 0x00, sizeof(factorMenor));

    factor = 'F';
    strcpy(factorMenor, "F");
    for (idx = 0; idx < strlen(auxiliar); idx++) {
        memset(datoComprimido, 0x00, sizeof(datoComprimido));

        if (auxiliar[idx] == factor) {
            counter++;
            if (counter > 3) {
                contandoCeros = 1;
            } else {
                strcat(datoMenor, factorMenor);
            }
        } else {
            if (contandoCeros == 1) {
                contandoCeros = 0;
                memset(datoMenor, 0x00, sizeof(datoMenor));
                sprintf(datoComprimido, "{%d}", counter);
                counter = 0;
                idx--;
                strcat(auxiliar2, datoComprimido);
            } else if (counter > 0 && counter < 4) {
                strcat(auxiliar2, datoMenor);
                memset(datoMenor, 0x00, sizeof(datoMenor));
                counter = 0;
                idx--;
            } else {
                datoComprimido[0] = auxiliar[idx];
                strcat(auxiliar2, datoComprimido);
            }
            contandoCeros = 0;
        }

    }
    if (contandoCeros == 1 && dato == 2) {
        contandoCeros = 0;
        memset(datoMenor, 0x00, sizeof(datoMenor));
        if (dato == 2) {
            sprintf(datoComprimido, "{%d}", counter);
        }
        strcat(auxiliar2, datoComprimido);
        counter = 0;
    } else if (strlen(datoMenor) > 0 && contandoCeros == 0) {
        strcat(auxiliar2, datoMenor);
    }

    memset(auxiliar, 0x00, sizeof(auxiliar));
    memcpy(auxiliar, auxiliar2, sizeof(auxiliar2));

    return contadorF;
}

int traerDatosConsumoRestFirma(uChar *terminal, uChar *host) {

    int retorno = 0;
    uChar tcpIpActual[SIZE_DOMINIO + 1];

    memset(tcpIpActual, 0x00, sizeof(tcpIpActual));

    getParameter(URL_FIRMA, tcpIpActual);

    retorno = strlen(tcpIpActual) > 0 ? TRUE : FALSE;

    if (retorno == 1) {
        strcpy(host, tcpIpActual);
        getParameter(ID_FIRMA, terminal);
        retorno = strlen(terminal) > 0 ? TRUE : FALSE;
    }

    return retorno;
}

int validarPanelDeFirma(DatosVenta *datosVenta) {

    int iRet = 0;
    char panelFirma[2 + 1];
    char tipoTransaccion[2 + 1];

    memset(panelFirma, 0x00, sizeof(panelFirma));
    memset(tipoTransaccion, 0x00, sizeof(tipoTransaccion));

    getParameter(IMPRIMIR_PANEL_FIRMA, panelFirma);
    getParameter(TIPO_TRANSACCION, tipoTransaccion);

    if (atoi(datosVenta->tipoCuenta) == 10 || atoi(datosVenta->tipoCuenta) == 20 || atoi(datosVenta->tipoCuenta) == 0
        || atoi(datosVenta->tipoCuenta) == 30) {
        if ((strncmp(datosVenta->cardName, "MAESTRO", 7) != 0 && strcmp(datosVenta->cardName, "V.ELECTRON") != 0
             && strncmp(datosVenta->cardName, "VISA DEBIT", 10) != 0
             && strncmp(datosVenta->cardName, "MASTER DEBIT", 12) != 0
             && strncmp(datosVenta->cardName, "MASTERD", 7) != 0
             && strncmp(datosVenta->cardName, "MASTERDEBIT", 11) != 0) || atoi(panelFirma) == 0) {
            if (atoi(panelFirma) != 3) {
                iRet = 1;
            } else {
                iRet = 0;
            }
        } else {
            iRet = 0;
        }
    } else {
        if (atoi(panelFirma) == 1) {
            iRet = 1;
        }
    }

    if (iRet > 0) {
        setParameter(IMPRIMIR_PANEL_FIRMA,  (char*)"01");
    } else {
        setParameter(IMPRIMIR_PANEL_FIRMA,  (char*)"00");
    }

    return iRet;
}

int traerDatosConsumoRestRespuestaFinal(uChar *terminal, uChar *host) {

    int retorno = 0;
    uChar tcpIpActual[SIZE_DOMINIO + 1];

    memset(tcpIpActual, 0x00, sizeof(tcpIpActual));

    getParameter(URL_TEFF2, tcpIpActual);

    retorno = strlen(tcpIpActual) > 0 ? TRUE : FALSE;

    if (retorno == 1) {
        strcpy(host, tcpIpActual);
        getParameter(ID_TEFF2, terminal);
        retorno = strlen(terminal) > 0 ? TRUE : FALSE;
    }

    return retorno;
}

void generarDatosRespuestaAdicionales(DatosVenta datos) {

    int tam = 0;
    char version[20 + 1];
    char revision[2 + 1];
    char tipoEntrada[3 + 1];
    char panelFirma[2 + 1];
    char textoPagare[2 + 1];
    char comercio[50 + 1];
    char numeroCelular[10 + 1];
    char millasAcumuladas[30 + 1];
    char tipoTransaccion[2 + 1];
    uChar puntosAcumulados[12 + 1];
    TablaUnoInicializacion tablaUno;
    unsigned char terminal[64];

    memset(version, 0x00, sizeof(version));
    memset(revision, 0x00, sizeof(revision));
    memset(panelFirma, 0x00, sizeof(panelFirma));
    memset(tipoEntrada, 0x00, sizeof(tipoEntrada));
    memset(textoPagare, 0x00, sizeof(textoPagare));
    memset(&tablaUno, 0x00, sizeof(tablaUno));
    memset(comercio, 0x00, sizeof(comercio));
    memset(millasAcumuladas, 0x00, sizeof(millasAcumuladas));
    memset(tipoTransaccion, 0x00, sizeof(tipoTransaccion));
    memset(terminal, 0x00, sizeof(terminal));
    memset(numeroCelular, 0x00, sizeof(numeroCelular));

    tablaUno = _desempaquetarTablaCeroUno_();
    getParameter(TIPO_TRANSACCION, tipoTransaccion);

    tam = tamArchivo(discoNetcom, (char*)CELULAR_SMS);
    if (tam > 0) {
        leerArchivo(discoNetcom,  (char*)CELULAR_SMS, datos.tarjetaHabiente + 41);
    }
    memcpy(numeroCelular, datos.tarjetaHabiente + 41, 10);

    memcpy(comercio, tablaUno.defaultMerchantName, 24);
    strcat(comercio, "\n");
    strcat(comercio, tablaUno.receiptLine2);

        getParameter(VERSION_SW, version);
        getParameter(REVISION_SW, revision);

        strcat(version, revision);

        memcpy(panelFirma, datos.modoDatafono, 2);

        if (atoi(panelFirma) == 1) {
            strcpy(textoPagare, "01");
        } else {
            strcpy(textoPagare, "00");
        }
        if (atoi(datos.posEntrymode) == 21  && atoi(datos.tipoTransaccion) != TRANSACCION_CONSULTA_QR ) {
            strcpy(tipoEntrada, "01");
        } else if (atoi(datos.posEntrymode) == 51) {
            strcpy(tipoEntrada, "02");
        } else if (atoi(datos.posEntrymode) == 911) {
            strcpy(tipoEntrada, "03");
        } else {
            if (atoi(datos.tipoTransaccion) == TRANSACCION_CONSULTA_QR) {
                strcpy(tipoEntrada, "04");
            } else if(atoi(datos.tipoTransaccion) == TRANSACCION_PAGO_ELECTRONICO){
                strcpy(tipoEntrada, "05");
            }else {
                strcpy(tipoEntrada, "06");
            }
        }

        if(atoi(tipoTransaccion) != 3){
            setFieldTEFF(FIELD_TEFF_TIPO_DE_TRANSACCION, datos.tipoTransaccion, strlen(datos.tipoTransaccion));
        } else {
            setFieldTEFF(FIELD_TEFF_TIPO_DE_TRANSACCION, tipoTransaccion, strlen(tipoTransaccion));
        }
        setFieldTEFF(FIELD_TEFF_TIPO_ENTRADA, tipoEntrada, LEN_TEFF_TIPO_ENTRADA_TRANSACCION);
        if (strlen(datos.ARQC) > 0) {
            setFieldTEFF(FIELD_TEFF_ARQC, datos.ARQC, LEN_TEFF_ARQC);
        }
        if (strlen(datos.AID) > 0) {
            setFieldTEFF(FIELD_TEFF_AID, datos.AID, strlen(datos.AID));
        }
        if (strlen(datos.appLabel) > 0) {
            setFieldTEFF(FIELD_TEFF_CARDNAME, datos.appLabel, strlen(datos.appLabel));
        } else {
            setFieldTEFF(FIELD_TEFF_CARDNAME, datos.cardName, strlen(datos.cardName));
        }
        if (strlen(datos.fechaExpiracion) > 0) {
            setFieldTEFF(FIELD_TEFF_FECHA_EXPEDICION, datos.fechaExpiracion, strlen(datos.fechaExpiracion));
        }
        setFieldTEFF(FIELD_TEFF_VERSION_SW, version, strlen(version));
        setFieldTEFF(FIELD_TEFF_PANEL_FIRMA, panelFirma, LEN_TEFF_PANEL_DE_FIRMA);
        setFieldTEFF(FIELD_TEFF_TEXTO_PAGARE, textoPagare, 2);
        verificarDCC(&datos);
        setFieldTEFF(FIELD_TEFF_NOMBRE_COMERCIO, comercio, strlen(comercio));
        setFieldTEFF(FIELD_TEFF_INDICADOR_FIRMA, panelFirma, 2);

        if (atoi(datos.tipoTransaccion) == TRANSACCION_ACUMULAR_LEALTAD) {
            leftTrim(puntosAcumulados, datos.tokenVivamos, '0');
            strcpy(millasAcumuladas, "MILLAS ACUMULADAS ");
            strcat(millasAcumuladas, puntosAcumulados);
            setFieldTEFF(FIELD_TEFF_NUMERO_DE_MILLAS, millasAcumuladas, strlen(millasAcumuladas));
        }
        if (strlen(datos.inc) >= 2) {
            setFieldTEFF(FIELD_TEFF_INC, datos.inc, strlen(datos.inc) - 2);
        }

        if (strlen(numeroCelular) > 0) {
            setFieldTEFF(FIELD_TEFF_NUMERO_TELEFONO, numeroCelular, 10);
        }

}
int extraerDatosCajaWso2(uChar *buffer, uChar *bufferCaja) {

    int retorno = 0;
    char *p = NULL;

    setParameter(TIPO_TRANSACCION_CITIBANK,  (char*)"1");
    p = strstr(buffer, "<ns:return>");
    retorno = strlen(p) > 0 ? strlen(p) - strlen("<ns:return>") - 36 : 0;

    if (retorno > 0) {
        strncpy(bufferCaja, p + strlen("<ns:return>"), retorno);
    }

    return retorno;
}

int verificarDCC(DatosVenta *reciboVenta){

    int iRet = 0;
    char simbolo[30 + 1];
    char simbolo2[30 + 1];
    char aux1Dcc[1 +1];
    char tasaDeCambio[30 + 1];
    char parTextoLocal[30 + 1];
    char parTextoExtranjera[30 + 1];
    char valorFormato[20 + 1];

    memset(simbolo, 0x00, sizeof(simbolo));
    memset(simbolo2, 0x00, sizeof(simbolo2));
    memset(tasaDeCambio, 0x00, sizeof(tasaDeCambio));
    memset(aux1Dcc, 0x00, sizeof(aux1Dcc));
    memset(parTextoLocal, 0x00, sizeof(parTextoLocal));
    memset(parTextoExtranjera, 0x00, sizeof(parTextoExtranjera));
    memset(valorFormato, 0x00, sizeof(valorFormato));

    if (strncmp(reciboVenta->isQPS, "2", 1) == 0 || strncmp(reciboVenta->isQPS, "3", 1) == 0
        || strncmp(reciboVenta->aux1 + 49, "2", 1) == 0 || strncmp(reciboVenta->aux1 + 49, "3", 1) == 0) {
        strcpy(tasaDeCambio, "TASA CAMBIO");
        strcat(tasaDeCambio, " 1COP /");
        strcat(tasaDeCambio, reciboVenta->acquirerId);

        memcpy(aux1Dcc, reciboVenta->aux1 + 49, 1);

        if (strcmp(reciboVenta->isQPS, "3") == 0 || strcmp(aux1Dcc, "3") == 0) {
            strcat(simbolo, "[X]COP ");
            strcat(simbolo2, "[ ]");
        } else {
            strcat(simbolo, "[ ]COP ");
            strcat(simbolo2, "[X]");
        }

        memset(valorFormato, 0x00, sizeof(valorFormato));
        formatString(0x00, 0, reciboVenta->totalVenta, strlen(reciboVenta->totalVenta) - 2, valorFormato, 2);
        strcat(simbolo, valorFormato);
        strcpy(parTextoLocal, simbolo);
        strcat(simbolo2, reciboVenta->nombreComercio);
        strcpy(parTextoExtranjera, simbolo2);

        setFieldTEFF(FIELD_TEFF_TASA_DE_CAMBIO, tasaDeCambio, strlen(tasaDeCambio));
        setFieldTEFF(FIELD_TEFF_PESOS_DCC, parTextoLocal, strlen(parTextoLocal));
        setFieldTEFF(FIELD_TEFF_MONEDA_EXTRANJERA, parTextoExtranjera, strlen(parTextoExtranjera));
        iRet = 1;
    }
    return iRet;
}

int armarTpdu(char* data, int producto) {
    int indice = 0;
    int clave  = 0;
    int tamTpdu = 7;
    char niiFirma[4 + 1];
    char eftsecFirma[1 + 1];
    char eftsecteff[1 + 1];
    char eftsecPuntos[1 + 1];

    uChar Nii[4 + 1];
    uChar iso[12 + 1];
    uChar tpdu[7 + 1];

    memset(iso, 0x00, sizeof(iso));
    memset(tpdu, 0x00, sizeof(tpdu));
    memset(niiFirma, 0x00, sizeof(niiFirma));
    memset(eftsecteff, 0x00, sizeof(eftsecteff));
    memset(eftsecFirma, 0x00, sizeof(eftsecFirma));
    memset(eftsecPuntos, 0x00, sizeof(eftsecPuntos));

    if (producto == PRODUCTO_CAJA_IP || producto == PRODUCTO_TEST_WEB_EFTSEC || producto == PRODUCTO_TEST_WEB) {
        clave = NII_TEFF;
    } else if (producto == PRODUCTO_FIRMA_IP) {
        clave = NII_FIRMA;
    } else if (producto == PRODUCTO_PCOLOMBIA) {
        clave = NII_PUNTOS_COLOMBIA;
    }

    getParameter(clave, niiFirma);
    getParameter(EFTSEC_FIRMA, eftsecFirma);
    getParameter(EFTSEC_TEFF, eftsecteff);
    getParameter(EFTSEC_PUNTOS_COLOMBIA, eftsecPuntos);

    strcpy(Nii, niiFirma);

    sprintf(iso, "%d%s%s", 60, Nii, Nii);
    tamTpdu = 5;
//	}

    _convertASCIIToBCD_(tpdu, iso, 12);
    if (atoi(Nii) > 0) {
        memcpy(data, tpdu, tamTpdu);
        indice += tamTpdu;
    }
    return indice;
}

int armarPeticionDatos(char *data, char* dataTransaccion, int indice) {

    char terminal[SIZE_ID_TEFF + 1];
    char tcpIpActual[SIZE_DOMINIO + 1];
    char metodo[60 + 1];

    memset(terminal, 0x00, sizeof(terminal));
    memset(metodo, 0x00, sizeof(metodo));
    memset(tcpIpActual, 0x00, sizeof(tcpIpActual));

    leerArchivo(discoNetcom, (char*)METODO_SOLICITUD_CAJA,metodo);

    traerDatosConsumoRest(terminal, tcpIpActual);

    memcpy(data + indice, metodo, strlen(metodo));
    indice += strlen(metodo);
    memcpy(data + indice, "?Terminal=", 10);
    indice += 10;
    memcpy(data + indice, terminal, 8);
    indice += 8;
    memcpy(data + indice, "&Data=", 6);
    indice += 6;
    memcpy(data + indice, dataTransaccion, strlen(dataTransaccion));
    indice += strlen(dataTransaccion);
    memcpy(data + indice, " HTTP/1.1\r\nContent-Length: 0\r\nHost: ", 36);
    indice += 36;
    memcpy(data + indice, tcpIpActual, strlen(tcpIpActual));
    indice += strlen(tcpIpActual);
    memcpy(data + indice, "\r\n\r\n", 8);
    indice += 4;
    return indice;
}

int armarRespuestaDatos(char *dataEnviada, char* bufferSalida, int indice) {

    char terminal[SIZE_ID_TEFF + 1];
    char tcpIpActual[SIZE_DOMINIO + 1];
    char metodo[60 + 1];
    char firmaIP[1 + 1];

    memset(terminal, 0x00, sizeof(terminal));
    memset(tcpIpActual, 0x00, sizeof(tcpIpActual));
    memset(metodo, 0x00, sizeof(metodo));
    memset(firmaIP, 0x00, sizeof(firmaIP));

    getParameter(HABILITA_FIRMA_IP, firmaIP);
    leerArchivo(discoNetcom, (char*)METODO_CAJA_RESPUESTA,metodo);

    if (atoi(firmaIP) == 3) {
        traerDatosConsumoRestFirma(terminal, tcpIpActual);
    } else {
        traerDatosConsumoRest(terminal, tcpIpActual);
    }

    memset(terminal, 0x00, sizeof(terminal));
    getParameter(NUMERO_TERMINAL, terminal);

    memcpy(dataEnviada + indice, metodo, strlen(metodo));
    indice += strlen(metodo);
    memcpy(dataEnviada + indice, "?Terminal=", 10);
    indice += 10;
    memcpy(dataEnviada + indice, terminal, 8); //NETCOM01
    indice += 8;
    memcpy(dataEnviada + indice, "&Data=", 6);
    indice += 6;
    memcpy(dataEnviada + indice, bufferSalida, strlen(bufferSalida));
    indice += strlen(bufferSalida);
    memcpy(dataEnviada + indice, " HTTP/1.1\r\nContent-Length: 0\r\nHost: ", 36);
    indice += 36;
    memcpy(dataEnviada + indice, tcpIpActual, strlen(tcpIpActual));
    indice += strlen(tcpIpActual);
    memcpy(dataEnviada + indice, "\r\n\r\n", 8);
    indice += 4;

    return indice;
}

int solicitudPuntosColombia(DatosVenta *datos, int tipotransaccion) {

    char terminal[SIZE_ID_TEFF + 1];
    char datoshex[256];
    char dataEnviada[1024];
    char dataTransaccion[1024];
    uChar bufferCaja[512];
    uChar recibida[2048];
    char tipoDocumento[2 + 1];
    char documento[10 + 1];
    char documentoAux[10 + 1];
    char valorRedimidos[12 + 1];
    char puntosRedimidos[12 + 1];
    int respuesta = 0;
    int indice = 0;
    int tpdu = 0;
    int transaccionEnviar = TRANSACCION_PUNTOS_COLOMBIA;
    int indiceRecibido = 0;
    char suma2[6 + 1];
    char puntos[12 + 1];
    char auxiliar[20 + 1];
    char dataCampo93[120 + 1];
    char fecha[6 + 1];
    char hora[4 + 1];
    char dominio[20 + 1];
    char linea1[18 + 1];
    char linea2[18 + 1];
    char linea3[18 + 1];
    char metodoSolicitud [60 + 1];

    //DATE date;
    ResultISOPack packMessage;

    int tipoTransaccionCaja = tipotransaccion;

    if (tipoTransaccionCaja == REVERSO_PC) {
        transaccionEnviar = TRANSACCION_REVERSO_PUNTOS_COLOMBIA;
        tipoTransaccionCaja = ANULACION_PC;
    }

    memset(terminal, 0x00, sizeof(terminal));
    memset(&packMessage, 0x00, sizeof(packMessage));
    memset(datoshex, 0x00, sizeof(datoshex));
    memset(dataEnviada, 0x00, sizeof(dataEnviada));
    memset(dataTransaccion, 0x00, sizeof(dataTransaccion));
    memset(bufferCaja, 0x00, sizeof(bufferCaja));
    memset(recibida, 0x00, sizeof(recibida));
    memset(puntos, 0x00, sizeof(puntos));
    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(dataCampo93, 0x00, sizeof(dataCampo93));
    memset(fecha, 0x00, sizeof(fecha));
    memset(hora, 0x00, sizeof(hora));
    memset(tipoDocumento, 0x00, sizeof(tipoDocumento));
    memset(documento, 0x00, sizeof(documento));
    memset(documentoAux, 0x00, sizeof(documentoAux));
    memset(valorRedimidos, 0x00, sizeof(valorRedimidos));
    memset(puntosRedimidos, 0x00, sizeof(puntosRedimidos));
    memset(dominio, 0x00, sizeof(dominio));
    memset(linea1, 0x00, sizeof(linea1));
    memset(linea2, 0x00, sizeof(linea2));
    memset(linea3, 0x00, sizeof(linea3));
    memset(suma2, 0x00, sizeof(suma2));
    memset(metodoSolicitud, 0x00, sizeof(metodoSolicitud));
    getParameter(DOMINIO_PUNTOS_COLOMBIA, dominio);
    LOGI("leer archivo ");

    leerArchivo(discoNetcom, (char*)METODO_SOLICITUD_PUNTOS_COLOMBIA,metodoSolicitud);
    LOGI("leyo archivo ");
    setParameter(PRODUCTO_PUNTOS_COLOMBIA,  (char*)"01");
    //Telium_Read_date(&date);
    //sprintf(hora, "%.2s%.2s", date.hour, date.minute);
    //sprintf(fecha, "%.2s%.2s%.2s", date.year, date.month, date.day);

    setFieldTEFF(FIELD_TEFF_FECHA, fecha, 6); //fecha
    setFieldTEFF(FIELD_TEFF_HORA, hora, 4); //hora

    if (tipoTransaccionCaja != ANULACION_PC) {
        strcpy(dataCampo93, datos->codigoComercio);
        strcat(dataCampo93, datos->terminalId);
        strcat(dataCampo93, fecha);
        strcat(dataCampo93, hora);
        strcat(dataCampo93, datos->numeroRecibo);

        memcpy(datos->textoInformacion, dataCampo93, sizeof(dataCampo93));
    }

    suma2[0] = datos->numeroRecibo2[0] + hora[2];
    suma2[1] = datos->numeroRecibo2[1] + hora[3];
    suma2[2] = datos->numeroRecibo2[2] + hora[2];
    suma2[3] = datos->numeroRecibo2[3] + hora[3];

    memset(datos->numeroRecibo2, 0x00, sizeof(datos->numeroRecibo2));
    memcpy(datos->numeroRecibo2, suma2, 4);
    leftPad(tipoDocumento, datos->aux1, 0x30, 2);
    memcpy(documentoAux, datos->aux1 + 2, 10);
    leftTrim(documento, documentoAux, 0x30);
    memcpy(valorRedimidos, datos->aux1 + 12, 12);
    memcpy(puntosRedimidos, datos->aux1 + 24, 10);

    //respuesta = traerDatosConsumoRest(terminal, tcpIpActual);
    respuesta = 1;
    if (respuesta > 0) {

        setTipoMensajeTEFF(MENSAJE_TEFF_REQUERIMIENTO);
        setCodigoTransaccionTEFF(tipoTransaccionCaja);
        setCodigoRespuestaTEFF(NO_HAY_CODIGO_DE_RESPUESTA);
        strcpy(dataCampo93, datos->textoInformacion);

        switch (tipoTransaccionCaja) {
            case CONSULTA_PC:
                //setFieldTEFF(FIELD_TEFF_NUMERO_TERMINAL, datos->terminalId, LEN_TEFF_NUMERO_TERMINAL);
                setFieldTEFF(FIELD_TEFF_TIPO_DOCUMENTO, tipoDocumento, 2); //tipo de documento
                //setFieldTEFF(FIELD_TEFF_NUMERO_DOCUMENTO, "73670300", 8); // documento
                setFieldTEFF(FIELD_TEFF_NUMERO_DOCUMENTO, documento, strlen(documento)); // documento
                setFieldTEFF(FIELD_TEFF_CODIGO_ESTABLECIMIENTO, datos->codigoComercio, 10);
                //setFieldTEFF(FIELD_TEFF_CODIGO_ESTABLECIMIENTO, "0010203040", 10);
                setFieldTEFF(FIELD_TEFF_NUMERO_FACTURA, datos->codigoComercio, 10); //codigo de establecimiento
                break;
            case REDENCION_PC:
                setFieldTEFF(FIELD_TEFF_TIPO_DOCUMENTO, tipoDocumento, 2); //tipo de documento
                setFieldTEFF(FIELD_TEFF_NUMERO_DOCUMENTO, documento, strlen(documento)); // documento
                setFieldTEFF(FIELD_TEFF_CODIGO_CAJERO, datos->codigoCajero, 10); //cajero
                setFieldTEFF(FIELD_TEFF_MONTO, valorRedimidos, 12);
                setFieldTEFF(FIELD_TEFF_NUMERO_TERMINAL, datos->terminalId, 8); //terminal
                setFieldTEFF(FIELD_TEFF_CODIGO_ESTABLECIMIENTO, datos->codigoComercio, 10);
                setFieldTEFF(FIELD_TEFF_VALOR_REDENCION_BONO, puntosRedimidos, 10);
                setFieldTEFF(FIELD_TEFF_CODIGO_OPERADOR, datos->numeroRecibo2, 4); //Pin con encripcion SR
                setFieldTEFF(FIELD_TEFF_NUMERO_FACTURA, dataCampo93, strlen(dataCampo93)); //data campo93
                //setFieldTEFF(FIELD_TEFF_INFO_ADICIONAL_OPERACION, datos->numeroRecibo2, 4);//Pin con encripcion SR
                break;
            case ACUMULACION_PC:
                setFieldTEFF(FIELD_TEFF_CODIGO_CAJERO, datos->codigoCajero, 10); //cajero
                setFieldTEFF(FIELD_TEFF_MONTO, valorRedimidos, 12);
                setFieldTEFF(FIELD_TEFF_TIPO_DOCUMENTO, tipoDocumento, 2); //tipo de documento
                //setFieldTEFF(FIELD_TEFF_NUMERO_DOCUMENTO, "73670300", 8); // documento
                setFieldTEFF(FIELD_TEFF_NUMERO_DOCUMENTO, documento, strlen(documento)); // documento
                setFieldTEFF(FIELD_TEFF_NUMERO_TERMINAL, datos->terminalId, 8); //terminal
                setFieldTEFF(FIELD_TEFF_VALOR_REDENCION_BONO, puntosRedimidos, 10); //galones
                setFieldTEFF(FIELD_TEFF_NUMERO_FACTURA, dataCampo93, strlen(dataCampo93)); //data campo93
                break;
            case ANULACION_PC:
            case REVERSO_PC:
                setFieldTEFF(FIELD_TEFF_CODIGO_CAJERO, datos->codigoCajero, 10); //cajero
                setFieldTEFF(FIELD_TEFF_NUMERO_TERMINAL, datos->terminalId, 8); //terminal
                setFieldTEFF(FIELD_TEFF_CODIGO_ESTABLECIMIENTO, datos->codigoComercio, 10);
                setFieldTEFF(FIELD_TEFF_NUMERO_FACTURA, dataCampo93, strlen(dataCampo93));
                break;
            default:
                break;
        }


        packMessage = packMessageTEFF();
        memcpy(datoshex, packMessage.isoPackMessage, packMessage.totalBytes);
        respuesta = packMessage.totalBytes;

        if (respuesta > 0) {
            respuesta = armarTramaRespuestaCajaIP(datoshex, respuesta);
        }
        if (respuesta > 0) {
            respuesta = convertirHexCadena(dataTransaccion, datoshex, respuesta);
        }

        if (respuesta > 0) {
            indice = armarTpdu(dataEnviada, PRODUCTO_PCOLOMBIA);
            indiceRecibido = indice;
            if (indice > 4) {
                tpdu = 1;
                indiceRecibido += 2;
            }
            memcpy(dataEnviada + indice, metodoSolicitud, strlen(metodoSolicitud));
            indice += strlen(metodoSolicitud);
            memcpy(dataEnviada + indice, "?Terminal=", 10);
            indice += 10;
            memcpy(dataEnviada + indice, datos->terminalId, 8); //NETCOM01
            indice += 8;
            memcpy(dataEnviada + indice, "&Data=", 6);
            indice += 6;
            memcpy(dataEnviada + indice, dataTransaccion, strlen(dataTransaccion));
            indice += strlen(dataTransaccion);
            memcpy(dataEnviada + indice, " HTTP/1.1\r\nContent-Length: 0\r\nHost: ", 36);
            indice += 36;
            memcpy(dataEnviada + indice, dominio, strlen(dominio));
            LOGI("dominio A %s", dominio);
            indice += strlen(dominio);
            memcpy(dataEnviada + indice, "\r\n\r\n", 8);
            indice += 4;
            interpretarTrama(dataEnviada, indice * 2);
                if (tipotransaccion == REVERSO_PC) {
                //screenMessageOptimizado("COMUNICACION", "REVERSO", "PUNTOS", "COLOMBIA", 250);
            } else {
                //screenMessageOptimizado("COMUNICACION", "SERVICIO", "CONECTANDO", "", 250);
            }

            if (tpdu == 1) {
                agregarLongitud(indice, dataEnviada);
                LOGI("data Enviada B %s", dataEnviada);
                indice += 2;
            }
            if (tipoTransaccionCaja == ACUMULACION_PC || tipoTransaccionCaja == REDENCION_PC) {
                borrarArchivo(discoNetcom,  (char*)REVERSO_AUXILIAR_PUNTOS_COLOMBIA);
                escribirArchivo(discoNetcom,  (char*)REVERSO_AUXILIAR_PUNTOS_COLOMBIA, (char *) datos, sizeof(DatosVenta));
                if (tipoTransaccionCaja == ACUMULACION_PC) {
                    setParameter(NOMBRE_REVERSO,  (char*)"ACUMULACION");
                } else if (tipoTransaccionCaja == REDENCION_PC) {
                    setParameter(NOMBRE_REVERSO,  (char*)"REDENCION");
                }
            }

            respuesta = realizarTransaccion(dataEnviada, indice, recibida, transaccionEnviar, CANAL_DEMANDA,
                                            REVERSO_GENERADO);

            respuesta = respuesta > 0 ? respuesta : ERROR_NO_RESPONDE;
        } else {
            respuesta = ERROR_NO_RESPONDE;
        }

        if (respuesta > 0) {
            respuesta = identificarRespuestaHttp(recibida + indiceRecibido) == TRUE ? respuesta : FALSE;
        }

        if (respuesta > 0) {
            respuesta = extraerDatosCajaPuntosColombia(recibida + indiceRecibido, bufferCaja);
        } else {
            //screenMessageOptimizado("MENSAJE ", "ERROR SERVER ", "VALIDAR ", "CONFIGURACION", 2 * 1500);
        }

        if (respuesta > 0) {
            respuesta = desempaquetarMensajeIP(bufferCaja, tipoTransaccionCaja);
        } else {
            if (respuesta == ERROR_NO_RESPONDE) {
                //screenMessageOptimizado("COMUNICACION", "SERVICIO", "NO RESPONDE", "", 500);
            }
            respuesta = ERROR_NO_RESPONDE;
        }

        if (respuesta > 0) {

            cargarCamposEstructuraDatosVenta(auxiliar, FIELD_TEFF_CODIGO_RESPUESTA);

            if (atoi(auxiliar) == 0) {

                borrarArchivo(discoNetcom,  (char*)REVERSO_PUNTOS_COLOMBIA);
                memset(auxiliar, 0x00, sizeof(auxiliar));
                cargarCamposEstructuraDatosVenta(auxiliar, FIELD_TEFF_CODIGO_AUTORIZACION);
                memcpy(datos->codigoAprobacion, auxiliar, strlen(auxiliar));
                memset(auxiliar, 0x00, sizeof(auxiliar));

                cargarCamposEstructuraDatosVenta(auxiliar, FIELD_TEFF_TEXTO_PAGARE);
                memcpy(datos->tarjetaHabiente, auxiliar, strlen(auxiliar));
                //memcpy(datos->tarjetaHabiente, "Ronald J Baez Torres", 20);
                memset(auxiliar, 0x00, sizeof(auxiliar));

                cargarCamposEstructuraDatosVenta(auxiliar, FIELD_TEFF_MONTO);
                //memcpy(datos->ARQC, "25000", 12);
                memcpy(datos->ARQC, auxiliar, strlen(auxiliar));
                memset(auxiliar, 0x00, sizeof(auxiliar));

                cargarCamposEstructuraDatosVenta(auxiliar, FIELD_TEFF_NUMERO_DOCUMENTO);
                //memcpy(datos->textoAdicional, "324",3);
                memcpy(datos->tokenVivamos, auxiliar, strlen(auxiliar));
                memset(auxiliar, 0x00, sizeof(auxiliar));

                cargarCamposEstructuraDatosVenta(auxiliar, FIELD_TEFF_VALOR_REDENCION_BONO);
                //memcpy(datos->textoAdicional, "3571",4);
                memcpy(datos->textoAdicional, auxiliar, strlen(auxiliar));
                memset(auxiliar, 0x00, sizeof(auxiliar));

                ajustarDatosTransaccion(datos->ARQC, 12, DATOS_SALIDA);

                ajustarDatosTransaccion(datos->textoAdicional, strlen(datos->textoAdicional), DATOS_SALIDA);
                ajustarDatosTransaccion(datos->aux1 + 24, 10, DATOS_SALIDA);
                ajustarDatosTransaccion(datos->tokenVivamos, strlen(datos->tokenVivamos), DATOS_SALIDA);

                /// FECHA ///
                //Telium_Read_date(&date);
                //sprintf(datos->txnTime, "%.2s%.2s%.2s", date.hour, date.minute, date.second);
                //sprintf(datos->txnDate, "%.2s%.2s", date.month, date.day);

            } else {

                if (tipotransaccion != REVERSO_PC) {
                    borrarArchivo(discoNetcom,  (char*)REVERSO_PUNTOS_COLOMBIA);
                }

                if (strcmp(auxiliar, "01") == 0) {
                    strcpy(linea1, "SERVIDOR PC");
                    strcpy(linea2, "NO");
                    strcpy(linea3, "RESPONDE");
                } else if (strcmp(auxiliar, "02") == 0 || strcmp(auxiliar, "03") == 0 || strcmp(auxiliar, "04") == 0) {
                    strcpy(linea1, "FALLA COMUNICACION");
                    strcpy(linea2, "SERVIDOR PC");
                    strcpy(linea3, "INTENTE NUEVAMENTE");
                } else if (strcmp(auxiliar, "05") == 0) {
                    strcpy(linea1, "");
                    strcpy(linea2, "ESTRUCTURA");
                    strcpy(linea3, "INCORRECTA");
                } else if (strcmp(auxiliar, "06") == 0) {
                    strcpy(linea1, "OPERACION");
                    strcpy(linea2, "NO PUEDE");
                    strcpy(linea3, "REALIZARSE");
                } else if (strcmp(auxiliar, "07") == 0) {
                    strcpy(linea1, "COMERCIO");
                    strcpy(linea2, "NO");
                    strcpy(linea3, "HABILITADO");
                } else if (strcmp(auxiliar, "08") == 0) {
                    strcpy(linea1, "MAS DE ");
                    strcpy(linea2, "UN");
                    strcpy(linea3, "DOCUMENTO");
                } else if (strcmp(auxiliar, "09") == 0) {
                    strcpy(linea1, "");
                    strcpy(linea2, "IDENTIFICACION");
                    strcpy(linea3, "INVALIDA");
                } else if (strcmp(auxiliar, "10") == 0) {
                    strcpy(linea1, "CODIGO");
                    strcpy(linea2, "DE LOCALIZACION");
                    strcpy(linea3, "INCORRECTA");
                } else if (strcmp(auxiliar, "11") == 0) {
                    strcpy(linea1, "INFORMACION");
                    strcpy(linea2, "NO ");
                    strcpy(linea3, "EXISTE");
                } else if (strcmp(auxiliar, "12") == 0) {
                    strcpy(linea1, "PLAN");
                    strcpy(linea2, "DE PRECIOS");
                    strcpy(linea3, "INCORRECTOS");
                } else if (strcmp(auxiliar, "13") == 0) {
                    strcpy(linea1, "USUARIO");
                    strcpy(linea2, "O PASSWORD");
                    strcpy(linea3, "INVALIDO");
                } else if (strcmp(auxiliar, "14") == 0) {
                    strcpy(linea1, "");
                    strcpy(linea2, "CERTIFICADO");
                    strcpy(linea3, "INVALIDO");
                } else if (strcmp(auxiliar, "15") == 0) {
                    strcpy(linea1, "TRANSACCION");
                    strcpy(linea2, "NO PUEDE");
                    strcpy(linea3, "SER CANCELADA");
                } else if (strcmp(auxiliar, "C1") == 0) {
                    strcpy(linea1, "CONTACTESE CON");
                    strcpy(linea2, "EL EMISOR");
                } else if (strcmp(auxiliar, "C2") == 0) {
                    strcpy(linea1, "ESTRUCTURA");
                    strcpy(linea2, "INCORRECTA");
                } else if (strcmp(auxiliar, "C3") == 0) {
                    strcpy(linea1, "METODO NO");
                    strcpy(linea2, "PERMITIDO");
                } else if (strcmp(auxiliar, "C4") == 0) {
                    strcpy(linea1, "INTENTE MAS ");
                    strcpy(linea2, "TARDE");
                } else {
                    strcpy(linea1, "TRANSACCION");
                    strcpy(linea2, "DECLINADA");
                }

                //screenMessageOptimizado("PUNTOS COLOMBIA", linea1, linea2, linea3, 1 * 1000);
                respuesta = -1;
            }
        }
    }
    return respuesta;
}
int extraerDatosCajaPuntosColombia(uChar *buffer, uChar *bufferCaja) {

    int retorno = 0;
    char *p = NULL;

    retorno = verificarCodigoRespuestaPuntosColombia(buffer);

    if (retorno == 1) {
        p = strstr(buffer, "\"Cod:00,Msj:");
        retorno = strlen(p) > 0 ? strlen(p) - strlen("\"Cod:00,Msj:") - 1 : 0;
    }

    if (retorno > 0) {
        strncpy(bufferCaja, p + strlen("\"Cod:00,Msj:"), retorno);
    }

    return retorno;
}
int verificarCodigoRespuestaPuntosColombia(uChar *buffer) {

    int iRet = 0;

    char linea1[18 + 1];
    char linea2[18 + 1];
    char linea3[18 + 1];

    memset(linea1, 0x00, sizeof(linea1));
    memset(linea2, 0x00, sizeof(linea2));
    memset(linea3, 0x00, sizeof(linea3));

    if (strncmp(strstr(buffer, "\"Cod:") + strlen("\"Cod:"), "00", 2) == 0) {
        iRet = 1;
    } else if (strncmp(strstr(buffer, "\"Cod:") + strlen("\"Cod:"), "01", 2) == 0) {
        strcpy(linea1, "SERVIDOR PC");
        strcpy(linea2, "NO");
        strcpy(linea3, "RESPONDE");
        iRet = 2;
    } else if (strncmp(strstr(buffer, "\"Cod:") + strlen("\"Cod:"), "02", 2) == 0) {
        strcpy(linea1, "FALLA COMUNICACION");
        strcpy(linea2, "SERVIDOR PC");
        strcpy(linea3, "INTENTE NUEVAMENTE");
        iRet = 2;
    } else if (strncmp(strstr(buffer, "\"Cod:") + strlen("\"Cod:"), "03", 2) == 0) {
        strcpy(linea1, "FALLA COMUNICACION");
        strcpy(linea2, "SERVIDOR PC");
        strcpy(linea3, "INTENTE NUEVAMENTE");
        iRet = 2;
    } else if (strncmp(strstr(buffer, "\"Cod:") + strlen("\"Cod:"), "04", 2) == 0) {
        strcpy(linea1, "FALLA COMUNICACION");
        strcpy(linea2, "SERVIDOR PC");
        strcpy(linea3, "INTENTE NUEVAMENTE");
        iRet = 2;
    } else if (strncmp(strstr(buffer, "\"Cod:") + strlen("\"Cod:"), "05", 2) == 0) {
        strcpy(linea1, "");
        strcpy(linea2, "ESTRUCTURA");
        strcpy(linea3, "INCORRECTA");
        iRet = 2;
    } else if (strncmp(strstr(buffer, "\"Cod:") + strlen("\"Cod:"), "06", 2) == 0) {
        strcpy(linea1, "OPERACION");
        strcpy(linea2, "NO PUEDE");
        strcpy(linea3, "REALIZARSE");
        iRet = 2;
    } else if (strncmp(strstr(buffer, "\"Cod:") + strlen("\"Cod:"), "07", 2) == 0) {
        strcpy(linea1, "COMERCIO");
        strcpy(linea2, "NO");
        strcpy(linea3, "HABILITADO");
        iRet = 2;
    } else if (strncmp(strstr(buffer, "\"Cod:") + strlen("\"Cod:"), "08", 2) == 0) {
        strcpy(linea1, "MAS DE ");
        strcpy(linea2, "UN");
        strcpy(linea3, "DOCUMENTO");
        iRet = 2;
    } else if (strncmp(strstr(buffer, "\"Cod:") + strlen("\"Cod:"), "09", 2) == 0) {
        strcpy(linea1, "");
        strcpy(linea2, "IDENTIFICACION");
        strcpy(linea3, "INVALIDA");
        iRet = 2;
    } else if (strncmp(strstr(buffer, "\"Cod:") + strlen("\"Cod:"), "10", 2) == 0) {
        strcpy(linea1, "CODIGO");
        strcpy(linea2, "DE LOCALIZACION");
        strcpy(linea3, "INCORRECTA");
        iRet = 2;
    } else if (strncmp(strstr(buffer, "\"Cod:") + strlen("\"Cod:"), "11", 2) == 0) {
        strcpy(linea1, "INFORMACION");
        strcpy(linea2, "NO ");
        strcpy(linea3, "EXISTE");
        iRet = 2;
    } else if (strncmp(strstr(buffer, "\"Cod:") + strlen("\"Cod:"), "12", 2) == 0) {
        strcpy(linea1, "PLAN");
        strcpy(linea2, "DE PRECIOS");
        strcpy(linea3, "INCORRECTOS");
        iRet = 2;
    } else if (strncmp(strstr(buffer, "\"Cod:") + strlen("\"Cod:"), "13", 2) == 0) {
        strcpy(linea1, "USUARIO");
        strcpy(linea2, "O PASSWORD");
        strcpy(linea3, "INVALIDO");
        iRet = 2;
    } else if (strncmp(strstr(buffer, "\"Cod:") + strlen("\"Cod:"), "14", 2) == 0) {
        strcpy(linea1, "");
        strcpy(linea2, "CERTIFICADO");
        strcpy(linea3, "INVALIDO");
        iRet = 2;
    } else if (strncmp(strstr(buffer, "\"Cod:") + strlen("\"Cod:"), "15", 2) == 0) {
        strcpy(linea1, "TRANSACCION");
        strcpy(linea2, "NO PUEDE");
        strcpy(linea3, "SER CANCELADA");
        iRet = 2;
    } else {
        iRet = -20;
    }

    if (iRet == 2) {
        //screenMessageOptimizado("PUNTOS COLOMBIA", linea1, linea2, linea3, 1 * 1000);
        borrarArchivo(discoNetcom,  (char*)REVERSO_PUNTOS_COLOMBIA);
        iRet = -1;
    }
    return iRet;
}

int reversarTransaccionFirma(int error) {

    int idx = 0;
    int iRet = 0;
    int indice = 0;
    int indiceRecibido = 0;

    char linea1[50 + 1];
    char linea2[50 + 1];
    char linea3[50 + 1];
    char dataEnviada[2500 + 1];
    char datosLeidos[sizeof(DatosVenta)];
    char recibida[1024 + 1];
    DatosVenta datos;

    memset(&datos, 0x00, sizeof(datos));
    memset(linea1, 0x00, sizeof(linea1));
    memset(linea2, 0x00, sizeof(linea2));
    memset(linea3, 0x00, sizeof(linea3));
    memset(recibida, 0x00, sizeof(recibida));
    memset(datosLeidos, 0x00, sizeof(datosLeidos));
    memset(dataEnviada, 0x00, sizeof(dataEnviada));

    strcpy(linea1, "TRANSACCION");
    strcpy(linea2, "NO EXITOSA");

    indice = tamArchivo(discoNetcom,  (char*)REVERSO_FIRMA_IP);
    leerArchivo(discoNetcom,  (char*)REVERSO_FIRMA_IP, datosLeidos);
    memcpy(&datos, datosLeidos, sizeof(datos));

    memset(datos.codigoRespuesta, 0x00, sizeof(datos.codigoRespuesta));
    memset(datos.textoAdicional, 0x00, sizeof(datos.textoAdicional));
    memcpy(datos.codigoRespuesta, "88",2);

    if(error == ERROR_NO_FIRMO){
        strcpy(linea3, "SE REQUIERE FIRMA");
    } else if(error == RETIRO_CHIP) {
        strcpy(linea3, "RETIRO TARJETA");
    } else if (error == ERROR_CON_CAJA) {
        memset(linea2, 0x00, sizeof(linea2));
        strcpy(linea2, "ERROR");
        strcpy(linea3, "VERIFIQUE CAJA");
        memset(datos.codigoRespuesta, 0x00, sizeof(datos.codigoRespuesta));
        memcpy(datos.codigoRespuesta, "89", 2);
        memcpy(datos.textoAdicional, "VERIFIQUE CAJA", 14);
    } else if (error == ERROR_REVERSO_PENDIENTE) {
        strcpy(linea3, "");
    } else {
        strcpy(linea3, "ERROR SERVER");
        memcpy(datos.textoAdicional, "ERROR SERVER", 12);
    }

    if(error != ERROR_REVERSO_PENDIENTE){
        //screenMessage("COMUNICACION", linea1, linea2, linea3, 2 * 1000);
    }

    if (error != ERROR_NO_FIRMO) {
        indiceRecibido = armarTramaReversoSip(datos, TRANSACCION_TEFF_COMPRAS, dataEnviada);

        if (indice < 0) {
            indice = 272;
        }

        setParameter(HABILITA_FIRMA_IP,  (char*)"3");
        iRet = realizarTransaccion(dataEnviada, indice, recibida, TRANSACCION_CAJAS_IP, CANAL_DEMANDA,
                                   REVERSO_NO_GENERADO);
        if (iRet > 0) {
            borrarArchivo(discoNetcom, (char*) REVERSO_FIRMA_IP);
        }
        setParameter(HABILITA_FIRMA_IP,  (char*)"1");
    } else {
        borrarArchivo(discoNetcom,  (char*)REVERSO_FIRMA_IP);
    }

    iRet = iRet > 0 ? iRet : ERROR_NO_RESPONDE;

    if (iRet > 0) {
        iRet = identificarRespuestaHttp(recibida + indiceRecibido) == TRUE ? iRet : FALSE;
    }

    if (iRet > 0) {
        iRet = verificarCodigoRespuestaCajasIP(recibida + indiceRecibido);
    }
    if (iRet > 0) {
        iRet = validarACKCajasIp(recibida + indiceRecibido);
    }
    if (iRet > 0) {
        borrarArchivo(discoNetcom,  (char*)REVERSO_FIRMA_IP);
    }

    iRet = -1;
    iRet = ERROR_NO_RESPONDE;

    return iRet;
}

void convertirIntegerToHex(int integer, unsigned char * buffer) {
    buffer[0] = integer / 256;
    buffer[1] = integer - (buffer[0] * 256);
}

int agregarLongitud(int  indice, char* dataSend) {

    int iRet = 0;
    unsigned char longitud[4 + 1];
    char dataAuxiliar[6000 + 1];
    char eftsecFirma[1 + 1];

    memset(eftsecFirma, 0x00, sizeof(eftsecFirma));
    memset(longitud, 0x00, sizeof(longitud));
    memset(dataAuxiliar, 0x00, sizeof(dataAuxiliar));
    memcpy(dataAuxiliar, dataSend, indice);

    getParameter(EFTSEC_FIRMA,eftsecFirma);

    if (atoi(eftsecFirma) == 0) {
        convertirIntegerToHex(indice, longitud);
        memset(dataSend, 0x00, sizeof(dataSend));

        memcpy(dataSend, longitud, 2);
        memcpy(dataSend + 2, dataAuxiliar, indice);
    }

    LOGI("indice B %d", indice);
    return iRet;
}

int solicitarEnviarFirma(char* terminal2, char * recibo, int tipoTransaccionCaja, int partes, char * posEntrymode) {

    int tpdu = 0;
    int tamano = 0;
    int indice = 0;
    int tamano2 = 0;
    int retorno = 0;
    int posicion = 0;
    int resultado = 0;
    int porcentaje = 0;
    int indiceRecibido = 0;
    int retrasmicion = 0;
    int conttx = 0;

    uChar auxiliar[6000];
    uChar recibida[3000];
    uChar dataEnviada[2048];
    uChar tablaActual[2 + 1];
    uChar contadorPantalla[2 + 1];
    char terminal[8 + 1];
    char paquetes[3 + 1];
    char panelDeFirma[1 + 1];
    char valRetrasmi[2 + 1];
    char valContador[20 + 1];
    char *mensajeRespuesta = NULL;
    char mensaje [20 + 1];
    char rechazo [20 + 1];
    char metodo[60 + 1];

    char tcpIpActual[SIZE_DOMINIO + 1];

    memset(recibida, 0x00, sizeof(recibida));
    memset(terminal, 0x00, sizeof(terminal));
    memset(paquetes, 0x00, sizeof(paquetes));
    memset(tcpIpActual, 0x00, sizeof(tcpIpActual));
    memset(tablaActual, 0x00, sizeof(tablaActual));
    memset(panelDeFirma, 0x00, sizeof(panelDeFirma));
    memset(contadorPantalla, 0x00, sizeof(contadorPantalla));
    memset(rechazo, 0x00, sizeof(rechazo));
    memset(mensaje, 0x00, sizeof(mensaje));
    memset(metodo, 0x00, sizeof(metodo));

    getParameter(IMPRIMIR_PANEL_FIRMA, panelDeFirma);
    //procesarArchivoConfiguraciones(valRetrasmi, 7);
    leerArchivo(discoNetcom, (char*)METODO_ENVIO_FIRMA,metodo);

    retrasmicion = atoi(valRetrasmi);
    retrasmicion++;

    resultado = tamArchivo( (char*)"/HOST",  (char*)"/HOST/CPM.txt");

    do {
        indice = 0;
        memset(dataEnviada, 0x00, sizeof(dataEnviada));
        memset(auxiliar, 0x00, sizeof(auxiliar));
        sprintf(paquetes, "%d", partes);

        tamano = resultado / partes;
        if ((posicion + 1) == partes) {
            tamano2 = tamano;
            tamano = resultado - (posicion * tamano);
            buscarArchivo( (char*)"/HOST",  (char*)"/HOST/CPM.txt", auxiliar, posicion * tamano2, tamano);
        } else {
            buscarArchivo( (char*)"/HOST",  (char*)"/HOST/CPM.txt", auxiliar, posicion * tamano, tamano);
        }

        posicion++;

        retorno = 1;
        if (retorno > 0) {
            retorno = traerDatosConsumoRestFirma(terminal, tcpIpActual);
        }

        if (retorno > 0) {

            indice = armarTpdu(dataEnviada, PRODUCTO_FIRMA_IP);
            indiceRecibido = indice;
            if (indice > 4) {
                tpdu = 1;
                indiceRecibido += 2;
            }
            memcpy(dataEnviada + indice, metodo, strlen(metodo));
            indice += strlen(metodo);
            memcpy(dataEnviada + indice, "?Terminal=", 10);
            indice += 10;
            memcpy(dataEnviada + indice, terminal2, 8);
            indice += 8;
            memcpy(dataEnviada + indice, "&Recibo=", 8);
            indice += 8;
            memcpy(dataEnviada + indice, recibo, 6);
            indice += 6;
            memcpy(dataEnviada + indice, "&Paquetes=", 10);
            indice += 10;
            memcpy(dataEnviada + indice, paquetes, strlen(paquetes));
            indice += strlen(paquetes);
            memcpy(dataEnviada + indice, "&Paquete=", 9);
            indice += 9;
            sprintf(tablaActual, "%d", posicion);
            memcpy(dataEnviada + indice, tablaActual, strlen(tablaActual));
            indice += strlen(tablaActual);
            memcpy(dataEnviada + indice, "&Data=", 6);
            indice += 6;
            memcpy(dataEnviada + indice, auxiliar, strlen(auxiliar));
            indice += strlen(auxiliar);
            memcpy(dataEnviada + indice, " HTTP/1.1\r\nContent-Length: 0\r\nHost: ", 36);
            indice += 36;
            memcpy(dataEnviada + indice, tcpIpActual, strlen(tcpIpActual));
            indice += strlen(tcpIpActual);
            memcpy(dataEnviada + indice, "\r\n\r\n", 8);

            sprintf(contadorPantalla, "%s%s%s", tablaActual,"/",paquetes);

            indice += 4;
            if (tpdu == 1) {
                agregarLongitud(indice, dataEnviada);
                indice += 2;
            }
            borrarArchivo(discoNetcom, (char*)ARCHIVO_SINCRONIZACION_CAJA);
            memset(auxiliar, 0x00, sizeof(auxiliar));
            memcpy(auxiliar , dataEnviada, indice);//respaldo en caso de reintento

            do {
                if (conttx > 0) {
                    sprintf(valContador, "%s%d", "REINTENTO ", conttx);
                }
                porcentaje = (atoi(tablaActual) * 100) / atoi(paquetes);
                sprintf(contadorPantalla, "%d%s", porcentaje, "%");
                if (conttx == 0) {
                    //screenMessageOptimizado("COMUNICACION", "ENVIANDO FIRMA", contadorPantalla, "", 1);
                } else {
                    //screenMessageOptimizado("COMUNICACION", "RENVIANDO FIRMA", contadorPantalla, valContador, 2 * 1000);
                    memset(dataEnviada, 0x00, sizeof(dataEnviada));
                    memcpy(dataEnviada , auxiliar, indice);
                }
                setParameter(HABILITA_FIRMA_IP,(char*) "3");
                retorno = realizarTransaccion(dataEnviada, indice, recibida, TRANSACCION_CAJAS_IP, CANAL_DEMANDA,
                                              REVERSO_NO_GENERADO);
                if (retorno > 0) {
                    conttx = retrasmicion;
                } else {
                    conttx++;
                }
            } while (conttx < retrasmicion);
            conttx = 0;
            indice = 0;
            retorno = retorno > 0 ? retorno : ERROR_NO_RESPONDE;
        } else {
            retorno = ERROR_CONFIGURACION;
        }
        if (retorno == ERROR_NO_RESPONDE) {
            //screenMessageOptimizado("COMUNICACION", "FIN", "RETRASMISION","MENSAJE PENDIENTE" , 2*1000);
        }
        if (retorno > 0) {
            retorno = identificarRespuestaHttp(recibida + indiceRecibido) == TRUE ? retorno : FALSE;
            if (retorno > 0) {
                mensajeRespuesta = strstr(recibida + indiceRecibido, HTTP_EXITOSO);
                strcpy(rechazo, "Rechazo:");
                memcpy(rechazo + 8, mensajeRespuesta, 4);
                mensajeRespuesta = NULL;
            }
        }

        if (retorno > 0) {
            retorno = verificarCodigoRespuestaCajasIP(recibida + indiceRecibido);
        } else {
            //screenMessageOptimizado("MENSAJE ", "ERROR SERVER ", "VALIDAR ", "CONFIGURACION", 2 * 1500);
        }
    } while (posicion < partes && retorno > 0);
    setParameter(HABILITA_FIRMA_IP, (char*)"1");

    if (retorno <= 0 && retorno != ERROR_NO_RESPONDE) {
        mensajeRespuesta = strstr(recibida + indiceRecibido, "\"Cod:") + strlen("\"Cod:");
        strcpy(mensaje, "Motivo:");
        memcpy(mensaje + 7, mensajeRespuesta, 2);
        //screenMessageOptimizado("SERVICIOS", rechazo, mensaje, "", 2 * 1500);
    }

    return retorno;
}
int verificarFirma(char * firmoOK) {

    int idx = 0;
    int tamano = 0;
    int firmaOK = 0;
    int partes = 0;
    int posicion = 0;
    int resultado = 0;

    uChar auxiliar[6000];
    uChar bufferSalida[6000];
    uChar factorizada[27000];

    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(factorizada, 0x00, sizeof(factorizada));
    memset(bufferSalida, 0x00, sizeof(bufferSalida));

    if (verificarArchivo( (char*)"/HOST",  (char*)"/HOST/SIGNA.BMP") == FS_OK) {
        resultado = tamArchivo( (char*)"/HOST",  (char*)"/HOST/SIGNA.BMP");
        partes = 35;
        tamano = resultado / partes;

        borrarArchivo( (char*)"/HOST",  (char*)"/HOST/CPM.txt");
        borrarArchivo( (char*)"/HOST",  (char*)"/HOST/BCDASS.txt");

        for (idx = 0; idx < (resultado / tamano); idx++) {

            memset(auxiliar, 0x00, sizeof(auxiliar));
            memset(bufferSalida, 0x00, sizeof(bufferSalida));
            buscarArchivo( (char*)"/HOST",  (char*)"/HOST/SIGNA.BMP", bufferSalida, posicion, tamano);

            _convertBCDToASCII_(auxiliar, bufferSalida, tamano);
            _convertBCDToASCII_(auxiliar + tamano, bufferSalida + (tamano / 2), tamano);
            escribirArchivo( (char*)"/HOST",  (char*)"/HOST/BCDASS.txt", auxiliar, strlen(auxiliar));
            posicion += tamano;

        }
        if (resultado > (idx * tamano)) {

            tamano = resultado - posicion;
            memset(auxiliar, 0x00, sizeof(auxiliar));
            memset(bufferSalida, 0x00, sizeof(bufferSalida));
            buscarArchivo( (char*)"/HOST", (char*) "/HOST/SIGNA.BMP", bufferSalida, posicion, tamano);
            _convertBCDToASCII_(auxiliar, bufferSalida, tamano);
            _convertBCDToASCII_(auxiliar + tamano, bufferSalida + (tamano / 2), tamano);
            escribirArchivo( (char*)"/HOST",  (char*)"/HOST/BCDASS.txt", auxiliar, strlen(auxiliar));

        }

        leerArchivo( (char*)"/HOST",  (char*)"/HOST/BCDASS.txt", factorizada);
        resultado = factorizarTrama(factorizada, 1);
        if (resultado > 10) {
            firmaOK = 1;
            sprintf(firmoOK,"%d",firmaOK );
        }
        escribirArchivo( (char*)"/HOST", (char*) "/HOST/CPM.txt", factorizada, strlen(factorizada));
        if (firmaOK == 1) {
            posicion = 0;
            resultado = tamArchivo( (char*)"/HOST", (char*) "/HOST/CPM.txt");
            if (resultado > 499) {
                partes = resultado / 500;
            } else {
                partes = 1;
            }
        }
    }
    return partes;
}

int respuestaFirmaIP(char * bufferSalida, char * terminal, char * tcpIpActual, int crearReverso) {

    int tpdu = 0;
    int indice = 0;
    int indice2 = 0;
    int retorno = 0;
    int indiceRecibido = 0;
    int retrasmicion = 0;
    int conttx = 0;
    int tamanoMensaje = 2;

    uChar dataEnviada[2500];
    uChar recibida[1024];
    char valRetrasmi[2 + 1];
    char valContador[2 + 1];
    char mensaje [20 + 1];
    char rechazo [20 + 1];
    char metodo[60 + 1];

    memset(dataEnviada, 0x00, sizeof(dataEnviada));
    memset(recibida, 0x00, sizeof(recibida));
    memset(valRetrasmi, 0x00, sizeof(valRetrasmi));
    memset(valContador, 0x00, sizeof(valContador));
    memset(rechazo, 0x00, sizeof(rechazo));
    memset(mensaje, 0x00, sizeof(mensaje));
    memset(metodo, 0x00, sizeof(metodo));

    leerArchivo(discoNetcom, (char*)METODO_CAJA_RESPUESTA,metodo);

    //procesarArchivoConfiguraciones(valRetrasmi, 7);

    retrasmicion = atoi(valRetrasmi);

    if (retrasmicion > 0) {
        retrasmicion++;
    }

    indice = armarTpdu(dataEnviada, PRODUCTO_FIRMA_IP);
    indiceRecibido = indice;
    if (indice > 4) {
        tpdu = 1;
        indiceRecibido += 2;
    }
    memcpy(dataEnviada + indice, metodo, strlen(metodo));
    indice += strlen(metodo);
    memcpy(dataEnviada + indice, "?Terminal=", 10);
    indice += 10;
    memcpy(dataEnviada + indice, terminal, 8); //NETCOM01
    indice += 8;
    memcpy(dataEnviada + indice, "&Data=", 6);
    indice += 6;
    indice2 = indice;
    memcpy(dataEnviada + indice, bufferSalida, strlen(bufferSalida));
    indice += strlen(bufferSalida);
    memcpy(dataEnviada + indice, " HTTP/1.1\r\nContent-Length: 0\r\nHost: ", 36);
    indice += 36;
    memcpy(dataEnviada + indice, tcpIpActual, strlen(tcpIpActual));
    indice += strlen(tcpIpActual);
    memcpy(dataEnviada + indice, "\r\n\r\n", 8);
    indice += 4;

    if (tpdu == 1) {
        agregarLongitud(indice, dataEnviada);
        indice += 2;
        indice2 += 2;
    }

    if (crearReverso == 0) {
        borrarArchivo(discoNetcom,  (char*)ARCHIVO_SINCRONIZACION_CAJA);
    }

    retorno = 1;

    if (crearReverso == 0) {
        do {
            if (conttx <= 0) {
                //screenMessageOptimizado("COMUNICACION", "ENVIANDO", "RESPUESTA", "SERVIDOR", 1);
            } else {
                sprintf(valContador, "%d", conttx);
                //screenMessageOptimizado("COMUNICACION", "RETRASMITIENDO", "RESPUESTA SERVIDOR", valContador, 3 * 1000);
                setParameter(HABILITA_FIRMA_IP,  (char*)"3");
            }

            retorno = realizarTransaccion(dataEnviada, indice, recibida, TRANSACCION_CAJAS_IP, CANAL_DEMANDA,
                                          REVERSO_NO_GENERADO);

            if (retorno > 0) {
                conttx = retrasmicion;
            } else {
                conttx++;
            }

        } while (conttx < retrasmicion);
        setParameter(HABILITA_FIRMA_IP,  (char*)"1");
        if (retorno == ERROR_NO_RESPONDE) {
            //screenMessageOptimizado("COMUNICACION", "FIN", "RETRASMISION", "MENSAJE PENDIENTE", 2 * 1000);
        }
    }

    char *mensajeRespuesta = NULL;

    if (crearReverso == 0) {
        retorno = retorno > 0 ? retorno : ERROR_NO_RESPONDE;
        if (retorno > 0) {
            retorno = identificarRespuestaHttp(recibida + indiceRecibido) == TRUE ? retorno : FALSE;
            if (retorno > 0) {
                mensajeRespuesta = strstr(recibida + indiceRecibido, HTTP_EXITOSO);
                strcpy(rechazo, "Rechazo:");
                memcpy(rechazo + 8, mensajeRespuesta, 4);
                mensajeRespuesta = NULL;
            }
        }
        if (retorno > 0) {
            retorno = verificarCodigoRespuestaCajasIP(recibida + indiceRecibido);
        }
        if (retorno <= 0 && retorno != ERROR_NO_RESPONDE) {
            mensajeRespuesta = strstr(recibida + indiceRecibido, "\"Cod:") + strlen("\"Cod:");
            if (strlen(rechazo) == 0) {
                mensajeRespuesta =  (char*)" DESCONOCIDO";
                tamanoMensaje = strlen(mensajeRespuesta);
            }
            strcpy(mensaje, "Motivo:");
            memcpy(mensaje + 7, mensajeRespuesta, tamanoMensaje);
            //screenMessageOptimizado("SERVICIOS", rechazo, mensaje, "", 2 * 1500);
        }
    }
    return retorno;
}

int armarTramaReversoSip(DatosVenta datos, int tipoTransaccionCaja, uChar* dataEnviada) {

    int indice = 0;
    int retorno = 0;
    int tpdu = 0;
    int indiceRecibido = 0;
    uChar bufferSalida[2500 + 1];
    uChar terminal[SIZE_ID_TEFF + 1];
    uChar tcpIpActual[SIZE_DOMINIO + 1];

    memset(bufferSalida, 0x00, sizeof(bufferSalida));
    memset(dataEnviada, 0x00, sizeof(dataEnviada));
    memset(terminal, 0x00, sizeof(terminal));
    memset(tcpIpActual, 0x00, sizeof(tcpIpActual));

    setParameter(HABILITA_FIRMA_IP, (char*)"03");
    retorno = cargarDatosRespuestaIP(bufferSalida, datos, tipoTransaccionCaja);
    setParameter(HABILITA_FIRMA_IP, (char*)"01");

    if (retorno > 0) {
        retorno = traerDatosConsumoRest(terminal, tcpIpActual);
        indice = armarTpdu(dataEnviada,PRODUCTO_FIRMA_IP);
        indiceRecibido = indice;
    }

    if (indice > 4) {
        tpdu = 1;
        indiceRecibido += 2;
    }
    if (retorno > 0) {
        setParameter(HABILITA_FIRMA_IP, (char*)"3");
        indice = armarRespuestaDatos(dataEnviada, bufferSalida, indice);
        setParameter(HABILITA_FIRMA_IP, (char*)"1");
        if (tpdu == 1) {
            agregarLongitud(indice, dataEnviada);
            indice += 2;
        }
    }

    return indiceRecibido;
}

void renombrarFileASincronizar(int tipoTransaccionCaja) {

    char tipoTx[2 + 1];
    char auxTipotx[2  + 1];
    char datosLeidos[sizeof(DatosVenta)];

    memset(tipoTx, 0x00, sizeof(tipoTx));
    memset(auxTipotx, 0x00, sizeof(auxTipotx));
    memset(datosLeidos, 0x00, sizeof(datosLeidos));

    sprintf(tipoTx, "%d",tipoTransaccionCaja);
    leftPad(auxTipotx, tipoTx, 0x30,2);

    leerArchivo(discoNetcom,(char*)REVERSO_FIRMA_IP , datosLeidos);
    borrarArchivo(discoNetcom, (char*)SINCRONIZACION_SERVICIO_IP);
    escribirArchivo(discoNetcom,(char*) SINCRONIZACION_SERVICIO_IP, auxTipotx, 2);
    escribirArchivo(discoNetcom,(char*) SINCRONIZACION_SERVICIO_IP, datosLeidos, sizeof(DatosVenta));
    borrarArchivo(discoNetcom, (char*)REVERSO_FIRMA_IP);
}
int armarReciboDinamico(uChar *bufferSalida, DatosVenta datos, int tipoTransaccion) {

    int idx = 0;
    int idy = 0;
    int idz = 0;
    int tam = 0;
    int indiceActual = 0;
    int tamano = 0;
    int retorno = 0;
    int continuaImprimiendo = 1;
    uChar datoshex[2500];

    char version[20 + 1];
    char revision[20 + 1];
    char textoCvm[3 + 1];
    char comercio[50 + 1];
    char campo98[1100 + 1];
    char auxiliar[20 + 1];
    char lineaImprimir[70 + 1];
    char pan[LEN_TEFF_PAN + 1];
    char auxiliarTexto[70 + 1];
    char millasAcumuladas[30 + 1];
    char puntosAcumulados[12 + 1];
    char tipoCuenta[SIZE_TIPO_CUENTA + 1];
    char alineadoIzquierda[] = "[0X1A]";
    char alineadoDerecha[] = "[0X1B]";
    char alineadoCentro[] = "[0X1C]";
    char alineadoDefecto[6 + 1] = {0x00};
    char dataRecibo[1500 + 1];
    char desconcatenar[80 + 1];
    char numeroCelular[10 + 1];

    char lineasRecibo[45][80 + 1];

    uChar parIdComercio[20 + 1];

    TablaUnoInicializacion tablaUno;

    ResultISOPackRecibo packMessage;

    memset(pan, 0x00, sizeof(pan));
    memset(version, 0x00, sizeof(version));
    memset(campo98, 0x00, sizeof(campo98));
    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(textoCvm, 0x00, sizeof(textoCvm));
    memset(comercio, 0x00, sizeof(comercio));
    memset(&tablaUno, 0x00, sizeof(tablaUno));
    memset(tipoCuenta, 0x00, sizeof(tipoCuenta));
    memset(dataRecibo, 0x00,sizeof(dataRecibo));
    memset(lineasRecibo, 0x00,sizeof(lineasRecibo));
    memset(&packMessage, 0x00, sizeof(packMessage));
    memset(lineaImprimir, 0x00, sizeof(lineaImprimir));
    memset(desconcatenar, 0x00, sizeof(desconcatenar));
    memset(parIdComercio, 0x00, sizeof(parIdComercio));
    memset(millasAcumuladas, 0x00, sizeof(millasAcumuladas));
    memset(puntosAcumulados, 0x00, sizeof(puntosAcumulados));
    memset(numeroCelular, 0x00, sizeof(numeroCelular));

    memcpy(comercio, tablaUno.defaultMerchantName, 24);
    strcat(comercio, "\n");
    strcat(comercio, tablaUno.receiptLine2);

    tablaUno = _desempaquetarTablaCeroUno_();

    setTipoMensajeTEFF(MENSAJE_TEFF_RESPUESTA);

    validacionCodigoRespuestaCaja(datos.codigoRespuesta);
    setCodigoRespuestaTEFF(atoi(datos.codigoRespuesta));
    setFieldTEFF(FIELD_TEFF_CODIGO_RESPUESTA, datos.codigoRespuesta, LEN_TEFF_CODIGO_RESPUESTA);

    ajustarDatosTransaccion(datos.codigoAprobacion, LEN_TEFF_CODIGO_AUTORIZACION, DATOS_ENTRADA);
    setFieldTEFF(FIELD_TEFF_CODIGO_AUTORIZACION, datos.codigoAprobacion, LEN_TEFF_CODIGO_AUTORIZACION);
    setFieldTEFF(FIELD_TEFF_NOMBRE_COMERCIO, comercio, strlen(comercio));
    if (atoi(datos.tipoTransaccion) == TRANSACCION_ACUMULAR_LEALTAD) {
        leftTrim(puntosAcumulados, datos.tokenVivamos, '0');
        strcpy(millasAcumuladas, "MILLAS ACUMULADAS ");
        strcat(millasAcumuladas, puntosAcumulados);
        setFieldTEFF(FIELD_TEFF_NUMERO_DE_MILLAS, millasAcumuladas, strlen(millasAcumuladas));
    }
    if (atoi(datos.track2) != 0) {
        extraerPanAsteriscos(datos.track2, strlen(datos.track2), pan, ASTERISCOS_4_ULTIMOS);
    } else if (atoi(datos.tipoTransaccion) == 20) {
        memcpy(pan, datos.aux1 + 2, 6);
        strcat(pan, "**");
        memcpy(pan + 8, datos.aux1 + 8, 2);
    } else {
        memcpy(pan, "000000**0000", LEN_TEFF_PAN);
    }

    tam = tamArchivo(discoNetcom, (char*) CELULAR_SMS);
    if (tam > 0) {
        leerArchivo(discoNetcom, (char*) CELULAR_SMS, datos.tarjetaHabiente + 41);
    }

    memcpy(numeroCelular, datos.tarjetaHabiente + 41, 10);

    memcpy(tipoCuenta, datos.tipoCuenta, LEN_TEFF_TIPO_CUENTA);
    tiposDeCuentaCaja(tipoCuenta, atoi(datos.creditoRotativo));

    setFieldTEFF(FIELD_TEFF_ASTERISCOS_4_ULTIMOS_TARJETA, pan, LEN_TEFF_ASTERISCOS_4_ULTIMOS_TARJETA);

    ajustarDatosTransaccion(datos.totalVenta, LEN_TEFF_MONTO, DATOS_ENTRADA_MONTOS);

    ajusteFecha(datos.txnDate);

    setFieldTEFF(FIELD_TEFF_TIPO_CUENTA, tipoCuenta, LEN_TEFF_TIPO_CUENTA);
    setFieldTEFF(FIELD_TEFF_FRANQUICIA, datos.cardName, LEN_TEFF_FRANQUICIA);
    setFieldTEFF(FIELD_TEFF_MONTO, datos.totalVenta, LEN_TEFF_MONTO);
    setFieldTEFF(FIELD_TEFF_NUMERO_RECIBO_DATAFONO, datos.numeroRecibo, strlen(datos.numeroRecibo));

    if (strlen(numeroCelular) > 0) {
        setFieldTEFF(FIELD_TEFF_NUMERO_TELEFONO, numeroCelular, 10);
    }

    setFieldTEFF(FIELD_TEFF_CODIGO_ESTABLECIMIENTO, tablaUno.receiptLine3, LEN_TEFF_CODIGO_ESTABLECIMIENTO);
    setFieldTEFF(FIELD_TEFF_FECHA, datos.txnDate, LEN_TEFF_FECHA);
    setFieldTEFF(FIELD_TEFF_HORA, datos.txnTime, LEN_TEFF_HORA);

    tamano = tamArchivo(discoNetcom,  (char*)RECIBO_DIGITAL);

    leerArchivo(discoNetcom,  (char*)RECIBO_DIGITAL,dataRecibo);

    setParameter(IMPRIMIR_PANEL_FIRMA,  (char*)"0");
    do {
        memset(desconcatenar, 0x00, sizeof(desconcatenar));

        while (dataRecibo[idx] != '\n') {
            if( dataRecibo[idx] != 0x10){
                desconcatenar[idy] = dataRecibo[idx];
                idy++;
            }
            idx++;
        }
        memcpy(lineasRecibo[idz], desconcatenar, strlen(desconcatenar));
        if (strncmp(lineasRecibo[idz] + 24, "FIRMA", 5) == 0) {
            memset(textoCvm, 0x00, sizeof(textoCvm));
            strcpy(textoCvm, "01");
            setParameter(IMPRIMIR_PANEL_FIRMA,  (char*)"1");
        }

//		if (strncmp(lineasRecibo[idz] + 24, "C.UNICO", 7) == 0) {
//			inicio = idz;
//		}

        idy = 0;
        idx++;
        idz++;

    } while (idx  < tamano);
    idy = 1;

    do {
        memset(auxiliarTexto, 0x00, sizeof(auxiliarTexto));
        if (strncmp(lineasRecibo[idy] + 24, "C.UNICO", 7) == 0) {
            memcpy(parIdComercio, lineasRecibo[idy] + 24, 20);
            armarLineaRecibo(lineaImprimir, CON_SALTO, alineadoIzquierda, LETRA_PEQUENA, FORMATO_ESTANDAR,
                             parIdComercio, campo98);
            memcpy(auxiliarTexto, lineasRecibo[idy] + 59, 20);

            armarLineaRecibo(lineaImprimir, SIN_SALTO, alineadoDerecha, ULTIMA_LETRA, ULTIMO_FORMATO, auxiliarTexto,
                             campo98);
            idy++;
        } else if (strncmp(lineasRecibo[idy] + 40, "EXP: ", 5) == 0) {
            memcpy(auxiliarTexto, lineasRecibo[idy] + 24, 12);
            armarLineaRecibo(lineaImprimir, CON_SALTO, alineadoIzquierda, ULTIMA_LETRA, ULTIMO_FORMATO, auxiliarTexto,
                             campo98);
            memset(auxiliarTexto, 0x00, sizeof(auxiliarTexto));
            memcpy(auxiliarTexto, lineasRecibo[idy] + 40, 9);
            armarLineaRecibo(lineaImprimir, SIN_SALTO, alineadoCentro, ULTIMA_LETRA, ULTIMO_FORMATO, auxiliarTexto,
                             campo98);
            memset(auxiliarTexto, 0x00, sizeof(auxiliarTexto));
            memcpy(auxiliarTexto, lineasRecibo[idy] + 56, 16);
            armarLineaRecibo(lineaImprimir, SIN_SALTO, alineadoDerecha, ULTIMA_LETRA, ULTIMO_FORMATO, auxiliarTexto,
                             campo98);
            idy++;
        } else if (strncmp(lineasRecibo[idy] + 24, "** ", 3) == 0) {
            memcpy(auxiliarTexto, lineasRecibo[idy] + 24, 40);
            armarLineaRecibo(lineaImprimir, CON_SALTO, alineadoCentro, ULTIMA_LETRA, ULTIMO_FORMATO, auxiliarTexto,
                             campo98);
            idy++;
        } else if (strncmp(lineasRecibo[idy] + 24, "*** MINUTOS ", 12) == 0) {
            memcpy(auxiliarTexto, lineasRecibo[idy] + 24, 40);
            armarLineaRecibo(lineaImprimir, CON_SALTO, alineadoCentro, ULTIMA_LETRA, ULTIMO_FORMATO, auxiliarTexto,
                             campo98);
            idy++;
        } else if (strncmp(lineasRecibo[idy] + 24, "ARQC:", 5) == 0) {
            memcpy(auxiliarTexto, lineasRecibo[idy] + 24, 40);
            armarLineaRecibo(lineaImprimir, CON_SALTO, alineadoCentro, ULTIMA_LETRA, ULTIMO_FORMATO, auxiliarTexto,
                             campo98);
            idy++;
        } else if (strncmp(lineasRecibo[idy] + 24, "AID:", 4) == 0) {
            memcpy(auxiliarTexto, lineasRecibo[idy] + 24, 40);
            armarLineaRecibo(lineaImprimir, CON_SALTO, alineadoCentro, ULTIMA_LETRA, ULTIMO_FORMATO, auxiliarTexto,
                             campo98);
            idy++;
        } else if (strncmp(lineasRecibo[idy] + 24, "AP LABEL:", 9) == 0) {
            memcpy(auxiliarTexto, lineasRecibo[idy] + 24, 40);
            armarLineaRecibo(lineaImprimir, CON_SALTO, alineadoCentro, ULTIMA_LETRA, ULTIMO_FORMATO, auxiliarTexto,
                             campo98);
            idy++;
        } else if (strncmp(lineasRecibo[idy] + 24, "ID CAJERO:", 10) == 0) {
            memcpy(auxiliarTexto, lineasRecibo[idy] + 24, 20);
            armarLineaRecibo(lineaImprimir, CON_SALTO, alineadoIzquierda, ULTIMA_LETRA, ULTIMO_FORMATO, auxiliarTexto,
                             campo98);
            idy++;
        } else if(strncmp(lineasRecibo[idy] + 24, "No DOCUMENTO:", 13) == 0){
            memcpy(auxiliarTexto, lineasRecibo[idy] + 24, 30);
            armarLineaRecibo(lineaImprimir, CON_SALTO, alineadoIzquierda, ULTIMA_LETRA, ULTIMO_FORMATO, auxiliarTexto,
                             campo98);
            idy++;
        }else if(strncmp(lineasRecibo[idy] + 24, "Incluye Comision:", 17) == 0){
            memcpy(auxiliarTexto, lineasRecibo[idy] + 24, 30);
            armarLineaRecibo(lineaImprimir, CON_SALTO, alineadoIzquierda, ULTIMA_LETRA, ULTIMO_FORMATO, auxiliarTexto,
                             campo98);
            idy++;
        } else if(strncmp(lineasRecibo[idy] + 24, "       MONEDA DE LA TRANSACCION", 31) == 0){
            memcpy(auxiliarTexto, lineasRecibo[idy] + 31, 50);
            armarLineaRecibo(lineaImprimir, CON_SALTO, alineadoCentro, ULTIMA_LETRA, ULTIMO_FORMATO, auxiliarTexto,
                             campo98);
            idy++;
        } else if (strncmp(lineasRecibo[idy] + 24, "[ ]", 3) == 0) {
            memcpy(auxiliarTexto, lineasRecibo[idy] + 24, 20);
            armarLineaRecibo(lineaImprimir, CON_SALTO, alineadoIzquierda, ULTIMA_LETRA, ULTIMO_FORMATO, auxiliarTexto,
                             campo98);
            idy++;
        } else if (strncmp(lineasRecibo[idy] + 24, "[X]", 3) == 0) {
            memcpy(auxiliarTexto, lineasRecibo[idy] + 24, 20);
            armarLineaRecibo(lineaImprimir, CON_SALTO, alineadoIzquierda, ULTIMA_LETRA, ULTIMO_FORMATO, auxiliarTexto,
                             campo98);
            idy++;
        } else if (strncmp(lineasRecibo[idy] + 24, "TASA CAMBIO", 11) == 0) {
            memcpy(auxiliarTexto, lineasRecibo[idy] + 24, 11);
            armarLineaRecibo(lineaImprimir, CON_SALTO, alineadoIzquierda, ULTIMA_LETRA, ULTIMO_FORMATO, auxiliarTexto,
                             campo98);
            idy++;
        } else if (strncmp(lineasRecibo[idy] + 24, "**", 2) == 0) {
            memcpy(auxiliarTexto, lineasRecibo[idy] + 24, 6);
            armarLineaRecibo(lineaImprimir, CON_SALTO, alineadoIzquierda, ULTIMA_LETRA, ULTIMO_FORMATO, auxiliarTexto,
                             campo98);
            memset(auxiliarTexto, 0x00, sizeof(auxiliarTexto));
            memcpy(auxiliarTexto, lineasRecibo[idy] + 39, 15);
            armarLineaRecibo(lineaImprimir, SIN_SALTO, alineadoCentro, ULTIMA_LETRA, ULTIMO_FORMATO, auxiliarTexto,
                             campo98);
            memset(auxiliarTexto, 0x00, sizeof(auxiliarTexto));
            memcpy(auxiliarTexto, lineasRecibo[idy] + 61, 11);
            armarLineaRecibo(lineaImprimir, SIN_SALTO, alineadoDerecha, ULTIMA_LETRA, ULTIMO_FORMATO, auxiliarTexto,
                             campo98);
            idy++;
        } else if (strncmp(lineasRecibo[idy] + 44, "APRO: ", 6) == 0) {
            memcpy(auxiliarTexto, lineasRecibo[idy] + 24, 16);
            armarLineaRecibo(lineaImprimir, CON_SALTO, alineadoIzquierda, ULTIMA_LETRA, ULTIMO_FORMATO, auxiliarTexto,
                             campo98);
            memset(auxiliarTexto, 0x00, sizeof(auxiliarTexto));
            memcpy(auxiliarTexto, lineasRecibo[idy] + 44, 12);
            armarLineaRecibo(lineaImprimir, SIN_SALTO, alineadoDerecha, ULTIMA_LETRA, ULTIMO_FORMATO, auxiliarTexto,
                             campo98);
            idy++;
        } else if (strncmp(lineasRecibo[idy] + 24, (char*) "COMPRA NETA", 11) == 0) {
            adecuarRenglon(lineasRecibo[idy], (char*) "COMPRA NETA", 24, 11, 35, 13, campo98);
            continuaImprimiendo = 0;
            idy++;
        } else if (strncmp(lineasRecibo[idy] + 24, (char*) "IVA", 3) == 0) {
            adecuarRenglon(lineasRecibo[idy], (char*) "IVA", 24, 11, 35, 13, campo98);
            idy++;
        } else if (strncmp(lineasRecibo[idy] + 24, (char*) "INC", 3) == 0) {
            adecuarRenglon(lineasRecibo[idy],  (char*)"INC", 24, 11, 35, 13, campo98);
            idy++;
        } else if (strncmp(lineasRecibo[idy] + 24,  (char*)"PROPINA", 7) == 0) {
            adecuarRenglon(lineasRecibo[idy],  (char*)"PROPINA", 24, 11, 35, 13, campo98);
            idy++;
        } else if (strncmp(lineasRecibo[idy] + 24,  (char*)"VAL. AVANCE", 11) == 0) {
            adecuarRenglon(lineasRecibo[idy], (char*) "VAL. AVANCE", 24, 11, 35, 13, campo98);
            idy++;
            continuaImprimiendo = 0;
        } else if (strncmp(lineasRecibo[idy] + 24,  (char*)"VALOR", 5) == 0
                   && strncmp(lineasRecibo[idy] + 18,  (char*)"[0X3C]", 6) != 0) {
            adecuarRenglon(lineasRecibo[idy],  (char*)"VALOR", 24, 11, 35, 13, campo98);
            idy++;
            continuaImprimiendo = 0;
        } else if (strncmp(lineasRecibo[idy] + 24,  (char*)"P.ACUM", 6) == 0) {
            adecuarRenglon(lineasRecibo[idy],  (char*)"P.ACUM", 24, 11, 35, 13, campo98);
            idy++;
            continuaImprimiendo = 0;
        } else if (strncmp(lineasRecibo[idy] + 24,  (char*)"1COP", 4) == 0) {
            adecuarRenglon(lineasRecibo[idy],  (char*)"1COP", 24, 11, 35, 13, campo98);
            idy++;
            continuaImprimiendo = 0;
        } else if (strncmp(lineasRecibo[idy] + 24,  (char*)"T.AEROP", 7) == 0) {
            adecuarRenglon(lineasRecibo[idy], (char*) "T.AEROP", 24, 11, 35, 13, campo98);
            idy++;
        } else if (strncmp(lineasRecibo[idy] + 24, (char*) "P.REDIM", 7) == 0) {
            adecuarRenglon(lineasRecibo[idy],  (char*)"P.REDIM", 24, 11, 35, 13, campo98);
            idy++;
            continuaImprimiendo = 0;
        } else if (strncmp(lineasRecibo[idy] + 24, (char*) "T.PUNTOS:", 9) == 0) {
            adecuarRenglon(lineasRecibo[idy],  (char*)"T.PUNTOS:", 24, 11, 35, 13, campo98);
            idy = idz;
            continuaImprimiendo = 0;
        } else if (strncmp(lineasRecibo[idy] + 24,  (char*)"RETIRO EFEC", 11) == 0) {
            adecuarRenglon(lineasRecibo[idy], (char*) "RETIRO EFEC", 24, 11, 35, 13, campo98);
            idy++;
            continuaImprimiendo = 0;
        } else if (strncmp(lineasRecibo[idy] + 24,  (char*)"REC EFEC", 8) == 0) {
            adecuarRenglon(lineasRecibo[idy],  (char*)"REC EFEC", 24, 11, 35, 13, campo98);
            idy++;
            continuaImprimiendo = 0;
        } else if (strncmp(lineasRecibo[idy] + 24, (char*) "RECIBO", 6) == 0) {
            memcpy(auxiliarTexto, lineasRecibo[idy] + 24, 14);

            if (strncmp(lineasRecibo[idy] + 38, " ", 1) == 0) {
                armarLineaRecibo(lineaImprimir, CON_SALTO, alineadoCentro, ULTIMA_LETRA, ULTIMO_FORMATO, auxiliarTexto,
                                 campo98);

                memset(auxiliarTexto, 0x00, sizeof(auxiliarTexto));
                memcpy(auxiliarTexto, lineasRecibo[idy] + 43, 10);
                armarLineaRecibo(lineaImprimir, SIN_SALTO, alineadoCentro, ULTIMA_LETRA, ULTIMO_FORMATO, auxiliarTexto,
                                 campo98);
                memset(auxiliarTexto, 0x00, sizeof(auxiliarTexto));
                memcpy(auxiliarTexto, lineasRecibo[idy] + 60, 12);
                armarLineaRecibo(lineaImprimir, SIN_SALTO, alineadoDerecha, ULTIMA_LETRA, ULTIMO_FORMATO, auxiliarTexto,
                                 campo98);
            } else {
                armarLineaRecibo(lineaImprimir, CON_SALTO, alineadoCentro, ULTIMA_LETRA, ULTIMO_FORMATO, auxiliarTexto,
                                 campo98);
            }

            idy++;
        } else if (strncmp(lineasRecibo[idy] + 24,  (char*)"RECARGA EN LINEA", 16) == 0) {
            strcat(campo98, lineasRecibo[idy]);
            idy++;
            continuaImprimiendo = 0;
        } else if (strncmp(lineasRecibo[idy] + 24,  (char*)"LLAMAME", 7) == 0) {
            strcat(campo98, lineasRecibo[idy]);
            idy++;
            continuaImprimiendo = 0;
        } else if (strncmp(lineasRecibo[idy] + 18, (char*) "[0X3C]", 6) != 0) { //LINEA FONDO NEGRO DE TOTAL
            if (lineasRecibo[idy][25] != 0) {
                strcat(campo98, lineasRecibo[idy]);
            }
            idy++;
        } else {
            if (strncmp(lineasRecibo[idy] + 24, "    ", 4) != 0) {
                memcpy(auxiliarTexto, lineasRecibo[idy] + 24, 11);
                armarLineaRecibo(lineaImprimir, CON_SALTO, alineadoIzquierda, ULTIMA_LETRA, FORMATO_FONDO_NEGRO,
                                 auxiliarTexto, campo98);
                memset(auxiliarTexto, 0x00, sizeof(auxiliarTexto));
                memcpy(auxiliarTexto, lineasRecibo[idy] + 35, 13);
                armarLineaRecibo(lineaImprimir, SIN_SALTO, alineadoDerecha, ULTIMA_LETRA, ULTIMO_FORMATO, auxiliarTexto,
                                 campo98);
            } else {
                memcpy(auxiliarTexto, lineasRecibo[idy] + 24, 20);
                armarLineaRecibo(lineaImprimir, CON_SALTO, alineadoCentro, ULTIMA_LETRA, FORMATO_FONDO_NEGRO,
                                 auxiliarTexto, campo98);
            }

            indiceActual = idy;
            idy = idz;
        }
    } while (idy < idz);

    if (continuaImprimiendo == 1) {
        indiceActual++;
        while ((indiceActual) < idz) {
            if (strncmp(lineasRecibo[indiceActual] + 24, (char*) "FIRMA:", 6) != 0) {
                if (strncmp(lineasRecibo[indiceActual] + 24,  (char*)"MILLAS ACUMULADAS", 17) == 0) {
                    adecuarRenglon(lineasRecibo[indiceActual],  (char*)"MILLAS ACUMULADAS", 24, 17, 41, 13, campo98);
                    indiceActual++;
                } else if (strncmp(lineasRecibo[indiceActual] + 24, "MILLAS DE ESTA TRANSACCION:", 18) == 0) {
                    adecuarRenglon(lineasRecibo[indiceActual],  (char*)"MILLAS DE ESTA TRANSACCION", 24, 27, 51, 10, campo98);
                    indiceActual++;
                } else {
                    memset(auxiliarTexto, 0x00, sizeof(auxiliarTexto));
                    memcpy(auxiliarTexto, lineasRecibo[indiceActual] + 24, 30);
                    memcpy(alineadoDefecto, lineasRecibo[indiceActual] + 6, 6);
                    armarLineaRecibo(lineaImprimir, CON_SALTO, alineadoDefecto, ULTIMA_LETRA, ULTIMO_FORMATO,
                                     auxiliarTexto, campo98);
                    indiceActual++;
                }
            } else {
                indiceActual = idz;
            }
        }
    }

    getParameter(VERSION_SW, version);
    getParameter(REVISION_SW, revision);

    strcat(version, revision);
    strcpy(textoCvm, "00");
    setFieldTEFF(FIELD_TEFF_CVM_RECIBO_DINAMICO, textoCvm, 2);
    setFieldTEFF(FIELD_TEFF_RECIBO_DINAMICO, campo98, strlen(campo98));
    setFieldTEFF(FIELD_TEFF_VERSION_RECIBO_DINAMICO, version + 3, 9);

    packMessage = packMessageTEFFReciboDinamico();

    memcpy(datoshex, packMessage.isoPackMessage, packMessage.totalBytes);

    retorno = armarTramaRespuestaCajaIP(datoshex, packMessage.totalBytes);

    if (retorno > 0) {
        retorno = convertirHexCadena(bufferSalida, datoshex, retorno);
    }

    return retorno;
}


void armarLineaRecibo(char *lineaImprimir, int salto, char *alineacion, int tipoLetra, int tipoFormato, char *valscr, char *campo98) {

    char letra[10 + 1];
    char formato[10 + 1];
    char saltoLinea[] = "[0X0A]";

    memset(letra, 0x00, sizeof(letra));
    memset(formato, 0x00, sizeof(formato));
    memset(lineaImprimir, 0x00, sizeof(lineaImprimir));

    switch (tipoFormato) {
        case 1:
            strcpy(formato, "[0X3A]");
            break;
        case 2:
            strcpy(formato, "[0X3B]");
            break;
        case 3:
            strcpy(formato, "[0X3C]");
            break;
        default:
            break;
    }

    switch (tipoLetra) {
        case 1:
            strcpy(letra, "[0X2A]");
            break;
        case 2:
            strcpy(letra, "[0X2B]");
            break;
        case 3:
            strcpy(letra, "[0X2C]");
            break;
        default:
            break;
    }

    if (salto == TRUE) {
        strcpy(lineaImprimir, saltoLinea);
    }

    strcat(lineaImprimir, alineacion);

    if (tipoLetra > 0) {
        strcat(lineaImprimir, letra);
    }

    if (tipoFormato > 0) {
        strcat(lineaImprimir, formato);
    }

    strcat(lineaImprimir, valscr);
    strcat(campo98, lineaImprimir);
}

void armarMontosReciboDinamico( char * texto, char * campo98, char * textoScr, int tipoLetra, int tipoFormato) {

    char lineaImprimir[50 + 1];
    char auxiliarMontos[12 + 1];
    char formatoMontos[12 + 1];
    char alineadoDerecha[] = "[0X1B]";
    char alineadoIzquierda[] = "[0X1A]";

    int cerosDecimales = 0;

    memset(formatoMontos, 0x00, sizeof(formatoMontos));

    if (strlen(textoScr) > 0) {
        cerosDecimales = 2;
    }
    armarLineaRecibo(lineaImprimir, CON_SALTO, alineadoIzquierda, tipoLetra, tipoFormato, texto, campo98);
    memset(auxiliarMontos, 0x00, sizeof(auxiliarMontos));
    memcpy(auxiliarMontos, textoScr, strlen(textoScr) - cerosDecimales);
    formatString(0x00, 0, auxiliarMontos, strlen(auxiliarMontos), formatoMontos, 1);
    armarLineaRecibo(lineaImprimir, SIN_SALTO, alineadoDerecha, 0, 0, formatoMontos, campo98);
}

int adecuarRenglon(char * lineaSource, char * compare, int indice1, int longitud1, int indice2, int longitud2,
                   char * campo98) {

    char auxiliarTexto[50 + 1];
    char lineaImprimir[50 + 1];
    char alineadoDerecha[] = "[0X1B]";
    char alineadoIzquierda[] = "[0X1A]";

    memset(auxiliarTexto, 0x00, sizeof(auxiliarTexto));


    memcpy(auxiliarTexto, lineaSource + indice1, longitud1);
    armarLineaRecibo(lineaImprimir, CON_SALTO, alineadoIzquierda, ULTIMA_LETRA, ULTIMO_FORMATO, auxiliarTexto, campo98);
    memset(auxiliarTexto, 0x00, sizeof(auxiliarTexto));
    memcpy(auxiliarTexto, lineaSource + indice2, longitud2);
    armarLineaRecibo(lineaImprimir, SIN_SALTO, alineadoDerecha, ULTIMA_LETRA, ULTIMO_FORMATO, auxiliarTexto, campo98);
    return 0;
}

int _testServicioWeb_() {

    int iRet = 0;
    int tpdu = 0;
    int indice = 0;
    int producto = PRODUCTO_TEST_WEB;

    int indiceRecibido = 0;
    char terminal[SIZE_ID_TEFF + 1];
    char tcpIpActual[SIZE_DOMINIO + 1];
    char metodo[60 + 1];
    char data[512 + 1];
    char recibida[4096 + 1];
    char *mensajeRespuesta = NULL;
    char *mensaje = NULL;
    char dataLeida[160 + 1];
    char eftsecTeff[1 + 1];
    char tcpIp[SIZE_DOMINIO + 1];
    char puertoFirma[5 + 1];
    char niiTest[4 + 1];

    LineaUrlFirma productoWeb;

    memset(data, 0x00, sizeof(data));
    memset(niiTest, 0x00, sizeof(niiTest));
    memset(metodo, 0x00, sizeof(metodo));
    memset(tcpIp, 0x00, sizeof(tcpIp));
    memset(puertoFirma, 0x00, sizeof(puertoFirma));
    memset(recibida, 0x00, sizeof(recibida));
    memset(terminal, 0x00, sizeof(terminal));
    memset(eftsecTeff, 0x00, sizeof(eftsecTeff));
    memset(dataLeida, 0x00, sizeof(dataLeida));
    memset(tcpIpActual, 0x00, sizeof(tcpIpActual));

    memset(&productoWeb, 0x00, sizeof(productoWeb));

    getParameter(NII_TEFF, niiTest);
    getParameter(EFTSEC_TEFF, eftsecTeff);
    getParameter(IP_CAJAS, tcpIp);
    getParameter(PUERTO_TEFF, puertoFirma);

    leerArchivo(discoNetcom, METODO_SOLICITUD_SERVICIO_WEB, (char *) &productoWeb);

    traerDatosConsumoRest(terminal, tcpIpActual);

    if (atoi(productoWeb.encriptado) == 1) {
        producto = PRODUCTO_TEST_WEB_EFTSEC;
        setParameter(EFTSEC_TEFF,  (char*)"1");
    }

    setParameter(IP_CAJAS, productoWeb.ipDestino);
    setParameter(NII_TEFF, productoWeb.nii);
    setParameter(PUERTO_TEFF, productoWeb.puerto);

    indice = armarTpdu(data, producto);

    indiceRecibido = indice;
    if (indice > 4) {
        tpdu = 1;
        indiceRecibido += 2;
    }

    memcpy(data + indice, productoWeb.metodo, strlen(productoWeb.metodo));
    indice += strlen(productoWeb.metodo);
    memcpy(data + indice, "?Terminal=", 10);
    indice += 10;
    memcpy(data + indice, terminal, 8);
    indice += 8;
    memcpy(data + indice, "&Data=", 6);
    indice += 6;
    memcpy(data + indice, "TEST", 4);
    indice += 4;
    memcpy(data + indice, " HTTP/1.1\r\nContent-Length: 0\r\nHost: ", 36);
    indice += 36;
    memcpy(data + indice, productoWeb.host, strlen(productoWeb.host));
    indice += strlen(productoWeb.host);
    memcpy(data + indice, "\r\n\r\n", 8);
    indice += 4;

    if (tpdu == 1) {
        agregarLongitud(indice, data);
        indice += 2;
    }

    //screenMessageOptimizado("COMUNICACION", "TEST ", "WEB", "", 500);

    iRet = realizarTransaccion(data, indice, recibida, TRANSACCION_CAJAS_IP, CANAL_DEMANDA, REVERSO_NO_GENERADO);

    iRet = iRet > 0 ? iRet : ERROR_NO_RESPONDE;

    if (iRet > 0) {
        iRet = identificarRespuestaHttp(recibida + indiceRecibido) == TRUE ? iRet : FALSE;
    }

    if (iRet > 0) {
        mensajeRespuesta = strstr(recibida + indiceRecibido, "\"Cod:") + strlen("\"Cod:");
        mensaje = strstr(mensajeRespuesta, "Msj:") + strlen("Msj:");
        //screenMessageOptimizado("SERVICIOS", mensaje, "", "", 2 * 1500);
    } else {
        //screenMessageOptimizado("MENSAJE ", "ERROR SERVER ", "VALIDAR ", "CONFIGURACION", 2 * 1500);
    }

    setParameter(NII_TEFF, niiTest);
    setParameter(EFTSEC_TEFF, eftsecTeff);
    setParameter(IP_CAJAS, tcpIp);
    setParameter(PUERTO_TEFF, puertoFirma);

    return iRet;
}

