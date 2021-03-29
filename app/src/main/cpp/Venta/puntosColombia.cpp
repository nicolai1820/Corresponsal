//
// Created by ronald on 21/04/2020.
//
#include "lealtad.h"
#include "TIPO_DATOS.h"
#include "configuracion.h"
#include "Utilidades.h"
#include "venta.h"
#include "Mensajeria.h"
#include "comunicaciones.h"
#include "Archivos.h"
#include "stdlib.h"
#include "Cajas.h"
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
#include <Inicializacion.h>
#include <native-lib.h>
#include "android/log.h"

#define  LOG_TAG    "NETCOM_PUNTOS_COLOMBIA"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

int puntosColombia(DatosBasicosVenta datosBasicosVenta) {

    int items = 0;
    int resultado = 0;
    int opcionLealtad = 0;

    char galones[10 + 1];
    char variableAux[11 + 1];
    char codigoCajero[6 + 1];
    char galonesActivos[1 + 1];
    char linea1[50 + 1];
    char linea2[50 + 1];
    char linea3[50 + 1];
    char linea4[50 + 1];
    char linea5[50 + 1];
    char monto[12 + 1];
    char tipoDocumento[2 + 1];
    char puntosRedimidos[10 + 1];
    char tipoTransaccion[2 + 1];
    char dataCampo93[120 + 1];
    char hora[4 + 1];
    char fecha[6 + 1];
    char isPuntosColombia[2 + 1];
    char lineaResumen[100 + 1] = {0x00};

    DatosLifeMiles datosLifeMiles;
    TransferenciaDatos datosPColombia;

    TablaUnoInicializacion tablaUno;

    memset(&globalTramaVenta, 0x00, sizeof(globalTramaVenta));
    memset(&datosLifeMiles, 0x00, sizeof(datosLifeMiles));
    memset(&tablaUno, 0x00, sizeof(tablaUno));
    memset(&datosPColombia, 0x00, sizeof(datosPColombia));

    setParameter(PRODUCTO_PUNTOS_COLOMBIA, "01");

    if (verificarHabilitacionCaja() == TRUE) {
        memcpy(globalTramaVenta.tipoCuenta, datosLifeMiles.tipoCuenta, TAM_TIPO_CUENTA);
        if (strcmp(globalTramaVenta.tipoCuenta, "01") == 0) {
            //resultado = transaccionPeticionDatosCaja(&globalTramaVenta,
            //                                       TRANSACCION_TEFF_ACUMULACION_PUNTOS_COL);
        } else {
            //resultado = transaccionPeticionDatosCaja(&globalTramaVenta, TRANSACCION_TEFF_LEALTAD);
        }
        strcat(globalTramaVenta.iva, "00");
    } else {
        memcpy(globalTramaVenta.totalVenta, datosBasicosVenta.totalVenta, TAM_COMPRA_NETA);
        memcpy(globalTramaVenta.compraNeta, datosBasicosVenta.compraNeta, TAM_COMPRA_NETA);
        memcpy(globalTramaVenta.iva, datosBasicosVenta.iva, TAM_IVA);
        memcpy(globalTramaVenta.inc, datosBasicosVenta.inc, TAM_IVA);
        memcpy(globalTramaVenta.baseDevolucion, datosBasicosVenta.baseDevolucion, TAM_COMPRA_NETA);
        //memcpy(globalTramaVenta.track2, datosLifeMiles.track2, TAM_TRACK2);
        //memcpy(globalTramaVenta.tipoCuenta, datosLifeMiles.tipoCuenta, TAM_TIPO_CUENTA);
    }

    tablaUno = _desempaquetarTablaCeroUno_();

    memcpy(globalTramaVenta.codigoComercio, tablaUno.receiptLine3,
           sizeof(globalTramaVenta.codigoComercio));
    generarInvoice(globalTramaVenta.numeroRecibo);

    getParameter(NUMERO_TERMINAL, globalTramaVenta.terminalId);
    getParameter(ACUMULAR_LIFEMILES, isPuntosColombia);

    if (atoi(isPuntosColombia) == 1) {
        items = 1;
    } else {
        items = 2;
    }

    if ((verificarHabilitacionCaja() == TRUE && resultado > 0) ||
        verificarHabilitacionCaja() != TRUE) {

        if (verificarHabilitacionCaja() == TRUE) {
            if (strcmp(globalTramaVenta.tipoCuenta, "01") == 0) {
                //resultado = acumularPuntosColombia(&globalTramaVenta);
            } else {
                //resultado = redimirPuntosColombia(&globalTramaVenta);
            }
        } else {
            sprintf(lineaResumen, "%s", "ACUMULACION;29;REDENCION;28;");
            showMenu("PUNTOS COLOMBIA;", items, lineaResumen, 100);
        }
    }
    return resultado;
}

int acumularPuntosColombia(int tipoDocumento, char *documento, char *codigoCajero, char *galones) {

    int salida = 0;
    int intentos = 0;
    int resultado = 0;
    int opcionLealtad = 0;
    int verificar = 0;

    char linea1[50 + 1];
    char linea2[50 + 1];
    char linea3[50 + 1];
    char linea4[50 + 1];
    char tipoDocCaja[2 + 1];
    char variableAux[12 + 1];
    char galonesActivos[1 + 1];
    char puntosRedimidos[10 + 1];
    char montoAuxiliar[12 + 1];
    char valorFormato[20 + 1];
    DatosVenta datosReverso;

    memset(linea1, 0x00, sizeof(linea1));
    memset(linea2, 0x00, sizeof(linea2));
    memset(linea3, 0x00, sizeof(linea3));
    memset(linea4, 0x00, sizeof(linea4));
    memset(tipoDocCaja, 0x00, sizeof(tipoDocCaja));
    memset(variableAux, 0x00, sizeof(variableAux));
    memset(montoAuxiliar, 0x00, sizeof(montoAuxiliar));
    memset(galonesActivos, 0x00, sizeof(galonesActivos));
    memset(puntosRedimidos, 0x00, sizeof(puntosRedimidos));
    memset(valorFormato, 0x00, sizeof(valorFormato));
    memset(&datosReverso, 0x00, sizeof(datosReverso));

    getParameter(GALONES_ACTIVOS, galonesActivos);

    if (verificarHabilitacionCaja() == TRUE) {
        memcpy(tipoDocCaja, globalTramaVenta.aux1, 2);
        memcpy(variableAux, globalTramaVenta.aux1 + 2, 10);
        memcpy(codigoCajero, globalTramaVenta.codigoCajero, sizeof(codigoCajero));
        opcionLealtad = atoi(tipoDocCaja);
    }
    LOGI("tipoDocumento %d", tipoDocumento);
    sprintf(globalTramaVenta.aux1, "%s%d", "0", tipoDocumento);
    LOGI("globalTramaVenta.aux1 %s", globalTramaVenta.aux1);
    if (strlen(variableAux) == 0) {
        LOGI("documento %s", documento);
        leftPad(globalTramaVenta.aux1 + 2, documento, 0x30, 10);
    }

    leftPad(globalTramaVenta.codigoCajero, codigoCajero, 0x20, 10);
    LOGI("globalTramaVenta.codigoCajero %s", codigoCajero);
    sprintf(puntosRedimidos, "%s", "0");
    memset(puntosRedimidos, 0x00, sizeof(puntosRedimidos));
    sprintf(puntosRedimidos, "%s", galones);

    leftPad(globalTramaVenta.aux1 + 24, puntosRedimidos, 0x30, 10);
    leftPad(globalTramaVenta.aux1 + 12, globalTramaVenta.totalVenta, 0x30, 12);

    memcpy(globalTramaVenta.estadoTarifa, "RV", 2);

    verificar = verificarArchivo(discoNetcom, REVERSO_PUNTOS_COLOMBIA);

    resultado = 1;
    //if (verificar == FS_OK) {
      //  leerArchivo(discoNetcom, REVERSO_PUNTOS_COLOMBIA, (char *) &datosReverso);
        //resultado = netcomServices(APP_TYPE_LEALTAD, ANULACION_PUNTOS_COLOMBIA,
        //TAM_JOURNAL_COMERCIO, (char *) &datosReverso);PENDIENTE POR FAVOR
        //resultado = 1;
    //}
    if (resultado > 0) {
        LOGI("solicitudPuntosColombia ");
        resultado = solicitudPuntosColombia(&globalTramaVenta, ACUMULACION_PC);
        LOGI("resultado %d", resultado);
    }


    if (resultado > 0) {
        memset(globalTramaVenta.estadoTarifa, 0x00, sizeof(globalTramaVenta.estadoTarifa));
    }

    if (resultado > 0) {
        memcpy(globalTramaVenta.tipoTransaccion, "20", 2);
        memcpy(globalTramaVenta.estadoTarifa, "A0", 2);
        sprintf(globalTramaVenta.estadoTransaccion, "%d", TRANSACCION_ACTIVA);
    }

    if (resultado > 0) {
        _guardarTransaccion_(globalTramaVenta);
        //screenMessage("PUNTOS COLOMBIA", "TRANSACCION APROBADA", "NUEVO SALDO DE PUNTOS:",
        //globalTramaVenta->textoAdicional, 2 * 1000);
        // imprimirPuntosColombia(*globalTramaVenta, 0, 0);
    }

    if (verificarHabilitacionCaja() == TRUE) {

        setParameter(USO_CAJA_REGISTRADORA, "1");
        strcpy(globalTramaVenta.tipoCuenta, "41");
        strcpy(globalTramaVenta.cardName, "PTOSCOLMB");

        if (resultado > 0) {
            strcpy(globalTramaVenta.codigoRespuesta, "00");
        } else {
            strcpy(globalTramaVenta.codigoRespuesta, "01");
            if (strlen(globalTramaVenta.iva) > 0) {
                memcpy(montoAuxiliar, globalTramaVenta.iva, strlen(globalTramaVenta.iva) - 2);
                memset(globalTramaVenta.iva, 0x00, sizeof(globalTramaVenta.iva));
                memcpy(globalTramaVenta.iva, montoAuxiliar, sizeof(globalTramaVenta.iva));
                memset(montoAuxiliar, 0x00, sizeof(montoAuxiliar));
            }
        }

        strcat(globalTramaVenta.totalVenta, "00");
        memcpy(montoAuxiliar, globalTramaVenta.totalVenta, sizeof(montoAuxiliar));
        transaccionRespuestaDatosCaja(globalTramaVenta, TRANSACCION_TEFF_COMPRAS);
        memset(globalTramaVenta.totalVenta, 0x00, sizeof(globalTramaVenta.totalVenta));
        memcpy(globalTramaVenta.totalVenta, montoAuxiliar, strlen(montoAuxiliar) - 2);
    }
    return resultado;
}

int redimirPuntosColombiaConsulta(int tipoDocumento, char *documento, char *codigoCajero) {

    int salida = 0;
    int intentos = 0;
    int resultado = 0;
    int opcionLealtad = 0;
    int cajaActiva = 0;
    int verificar = 0;

    char monto[12 + 1];
    char linea1[50 + 1];
    char linea2[50 + 1];
    char linea3[50 + 1];
    char linea4[50 + 1];
    char galones[10 + 1];
    char tipoDocCaja[2 + 1];
    char variableAux[11 + 1];
    char galonesActivos[1 + 1];
    char montoAuxiliar[12 + 1];
    char puntosRedimidos[10 + 1];
    char valorFormato[20 + 1];
    char valorFormato2[20 + 1];
    char lineaResumen[100 + 1];
    DatosVenta datosReverso;

    TransferenciaDatos datosPColombia;
    TablaUnoInicializacion tablaUno;

    memset(monto, 0x00, sizeof(monto));
    memset(linea1, 0x00, sizeof(linea1));
    memset(linea2, 0x00, sizeof(linea2));
    memset(linea3, 0x00, sizeof(linea3));
    memset(linea4, 0x00, sizeof(linea4));
    memset(galones, 0x00, sizeof(galones));
    memset(tipoDocCaja, 0x00, sizeof(tipoDocCaja));
    memset(montoAuxiliar, 0x00, sizeof(montoAuxiliar));
    memset(galonesActivos, 0x00, sizeof(galonesActivos));
    memset(&datosPColombia, 0x00, sizeof(datosPColombia));
    memset(puntosRedimidos, 0x00, sizeof(puntosRedimidos));
    memset(&tablaUno, 0x00, sizeof(tablaUno));
    memset(valorFormato, 0x00, sizeof(valorFormato));
    memset(valorFormato2, 0x00, sizeof(valorFormato2));
    memset(&datosReverso, 0x00, sizeof(datosReverso));

    if (verificarHabilitacionCaja() == TRUE) {
        memcpy(tipoDocCaja, globalTramaVenta.aux1, 2);
        memcpy(codigoCajero, globalTramaVenta.codigoCajero, sizeof(codigoCajero));
        cajaActiva = 1;
    }
    memcpy(globalTramaVenta.tipoTransaccion, "20", 2);

    tablaUno = _desempaquetarTablaCeroUno_();
    memcpy(globalTramaVenta.codigoComercio, tablaUno.receiptLine3,
           sizeof(globalTramaVenta.codigoComercio));

    if (atoi(tipoDocCaja) == 0) {

        sprintf(globalTramaVenta.aux1, "%s%d", "0", tipoDocumento);
        leftPad(globalTramaVenta.aux1 + 2, documento, 0x30, 10);
        LOGI("globalTramaVenta.aux1 %s", globalTramaVenta.aux1);
        resultado = 1;
    } else {
        resultado = 1;
    }

    if (resultado > 0) {
        leftPad(globalTramaVenta.codigoCajero, codigoCajero, 0x20, 10);
    }

    verificar = verificarArchivo(discoNetcom, REVERSO_PUNTOS_COLOMBIA);

    // (verificar == FS_OK) {
      //  leerArchivo(discoNetcom, REVERSO_PUNTOS_COLOMBIA, (char *) &datosReverso);
        //resultado = netcomServices(APP_TYPE_LEALTAD, ANULACION_PUNTOS_COLOMBIA,
        //TAM_JOURNAL_COMERCIO, (char *) &datosReverso);
        //resultado = 1; //PENDIENTE HABILITAR
    //}
    if (resultado > 0) {
        LOGI("solicitudPuntosColombia redencion %s ", globalTramaVenta.totalVenta);
        resultado = solicitudPuntosColombia(&globalTramaVenta, CONSULTA_PC);
    }


    if (resultado > 0) {
        formatString(0x00, 0, globalTramaVenta.textoAdicional,
                     strlen(globalTramaVenta.textoAdicional),
                     valorFormato, 2);
        formatString(0x00, 0, globalTramaVenta.ARQC, strlen(globalTramaVenta.ARQC), valorFormato2,
                     1);
        strcpy(linea1, "Puntos:");
        strcat(linea1, valorFormato);
        strcpy(linea2, "Pesos: ");
        strcat(linea2, valorFormato2); //pesos en puntos
        screenMessageFiveLine("SALDO ACTUAL", linea1, linea2, "", " ", " ", lineaResumen);
        enviarStringToJava(lineaResumen, "showScreenMessage");
    }

    return resultado;
}

int pagoTotal(DatosVenta *globalTramaVenta, int cajaActiva) {

    int residuo = 0;
    int puntosTotales = 0;
    int resultado = 0;
    int salida = 0;
    int verificar = 0;

    unsigned long puntosRedimed = 0;
    unsigned long long dato1 = 0;
    unsigned long dato2 = 0;
    unsigned long dato3 = 0;

    char pin[4 + 1];
    char monto[12 + 1];
    char linea1[50 + 1];
    char linea2[50 + 1];
    char linea3[50 + 1];
    char linea4[50 + 1];
    char variableAux[11 + 1];
    char codigoCajero[10 + 1];
    char montoAuxiliar[12 + 1];
    char puntosRedimidos[10 + 1];
    char valorFormato[20 + 1];
    char lineaResumen[100 + 1] = {0x00};
    DatosVenta datosReverso;

    memset(linea1, 0x00, sizeof(linea1));
    memset(linea2, 0x00, sizeof(linea2));
    memset(linea3, 0x00, sizeof(linea3));
    memset(linea4, 0x00, sizeof(linea4));
    memset(monto, 0x00, sizeof(monto));
    memset(codigoCajero, 0x00, sizeof(codigoCajero));
    memset(variableAux, 0x00, sizeof(variableAux));
    memset(montoAuxiliar, 0x00, sizeof(montoAuxiliar));
    memset(puntosRedimidos, 0x00, sizeof(puntosRedimidos));
    memset(valorFormato, 0x00, sizeof(valorFormato));
    memset(&datosReverso, 0x00, sizeof(datosReverso));

    leftPad(globalTramaVenta->aux1 + 12, globalTramaVenta->totalVenta, 0x30, 12);
    memcpy(monto, globalTramaVenta->ARQC, sizeof(monto));
    memcpy(variableAux, globalTramaVenta->aux1 + 2, 10);
    memcpy(codigoCajero, globalTramaVenta->codigoCajero, 10);

    dato1 = atoi(globalTramaVenta->totalVenta) * 10;
    dato2 = atoi(globalTramaVenta->ARQC);
    dato3 = atoi(globalTramaVenta->textoAdicional);

    puntosRedimed = (dato1 * dato3) / dato2;
    residuo = puntosRedimed % 10;
    if (residuo > 0) {
        puntosTotales = (puntosRedimed / 10) + 1;
    } else {
        puntosTotales = (puntosRedimed / 10);
    }

    sprintf(puntosRedimidos, "%d", puntosTotales);
    leftPad(globalTramaVenta->aux1 + 24, puntosRedimidos, 0x30, 10);
    formatString(0x00, 0, globalTramaVenta->totalVenta, strlen(globalTramaVenta->totalVenta),
                 valorFormato, 2);
    strcpy(linea1, "MONTO A PAGAR:$");
    strcat(linea1, valorFormato);
    strcpy(linea2, "PUNTOS: ");
    strcat(linea2, puntosRedimidos);
    strcpy(linea3, "DOC: C.C ");
    strcat(linea3, variableAux);
    strcpy(linea4, "COD.CAJERO: ");
    strcat(linea4, codigoCajero);

    screenMessageFiveLine("RESUMEN REDENCION", linea1, linea2, linea3, linea4, "", lineaResumen);
    enviarStringToJava(lineaResumen, "showScreenMessage");
    return resultado;
}

int pagoParcial(char *monto) {

    int salida = 0;
    int resultado = 0;
    int verificar = 0;
    int cajaActiva = 0;
    unsigned long puntosRedimed = 0;
    unsigned long long dato1 = 0;
    unsigned long dato2 = 0;
    unsigned long dato3 = 0;
    long montoExcedente = 0;

    char pin[4 + 1];
    char linea1[50 + 1];
    char linea2[50 + 1];
    char linea3[50 + 1];
    char linea4[50 + 1];
    char variableAux[11 + 1];
    char valorFormato[20 + 1];
    char codigoCajero[10 + 1];
    char montoAuxiliar[12 + 1];
    char puntosRedimidos[10 + 1];
    char maximoValorIngresado[12 + 1];
    char minimoValorIngresado[12 + 1];
    char lineaResumen[100 + 1] = {0x00};
    DatosVenta datosReverso;
    int residuo = 0;
    int puntosTotales = 0;
    TransferenciaDatos datosPColombia;
    DatosOperacion datosOperacion;

    memset(pin, 0x00, sizeof(pin));
    memset(linea1, 0x00, sizeof(linea1));
    memset(linea2, 0x00, sizeof(linea2));
    memset(linea3, 0x00, sizeof(linea3));
    memset(linea4, 0x00, sizeof(linea4));
    memset(linea4, 0x00, sizeof(linea4));
    memset(valorFormato, 0x00, sizeof(valorFormato));
    memset(montoAuxiliar, 0x00, sizeof(montoAuxiliar));
    memset(&datosPColombia, 0x00, sizeof(datosPColombia));
    memset(puntosRedimidos, 0x00, sizeof(puntosRedimidos));
    memset(&datosOperacion, 0x00, sizeof(datosOperacion));
    memset(minimoValorIngresado, 0x00, sizeof(minimoValorIngresado));
    memset(maximoValorIngresado, 0x00, sizeof(maximoValorIngresado));
    memset(&datosReverso, 0x00, sizeof(datosReverso));

    getParameter(MAXIMO_MONTO, maximoValorIngresado);
    getParameter(MINIMO_MONTO, minimoValorIngresado);

    if ((resultado > 0 && atoi(monto) > atoi(globalTramaVenta.totalVenta)) ||
        atoi(monto) > atoi(globalTramaVenta.ARQC)) {
        resultado = -1;
    } else {
        resultado = 1;
    }

    if (resultado > 0) {
        leftPad(globalTramaVenta.aux1 + 12, monto, 0x30, 12);
        dato1 = atoi(monto) * 10;
        dato2 = atoi(globalTramaVenta.ARQC);
        dato3 = atoi(globalTramaVenta.textoAdicional);

        puntosRedimed = (dato1 * dato3) / dato2;

        //puntosRedimed = (atoi(monto)  *100 * atoi(globalTramaVenta->ARQC)) / atoi(globalTramaVenta->textoAdicional);
        residuo = puntosRedimed % 10;
        if (residuo > 0) {
            puntosTotales = (puntosRedimed / 10) + 1;
        } else {
            puntosTotales = (puntosRedimed / 10);
        }

        sprintf(puntosRedimidos, "%d", puntosTotales);

        leftPad(globalTramaVenta.aux1 + 24, puntosRedimidos, 0x30, 10);
        memcpy(variableAux, globalTramaVenta.aux1 + 2, 10);
        memcpy(codigoCajero, globalTramaVenta.codigoCajero, 10);
    }

    if (resultado > 0 && atoi(monto) > 0) {
        memset(linea1, 0x00, sizeof(linea1));
        memset(linea2, 0x00, sizeof(linea2));
        memset(linea3, 0x00, sizeof(linea3));
        memset(linea4, 0x00, sizeof(linea4));

        formatString(0x00, 0, monto, strlen(monto), valorFormato, 2);

        strcpy(linea1, "MONTO A PAGAR: $ ");
        strcat(linea1, valorFormato);
        strcpy(linea2, "PUNTOS: ");
        strcat(linea2, puntosRedimidos);
        strcpy(linea3, "DOC: C.C ");
        strcat(linea3, variableAux);
        strcpy(linea4, "COD.CAJERO: ");
        strcat(linea4, codigoCajero);

        screenMessageFiveLine("RESUMEN REDENCION", linea1, linea2, linea3, linea4, "",
                              lineaResumen);
        enviarStringToJava(lineaResumen, "showScreenMessage");
    }


    return resultado;
}

int otroMedioPago(int opcion) {

    int resultado = 0;
    int opcionLealtad = 0;
    char nuevoMonto[12 + 1];
    long montoAux = 0;

    TransferenciaDatos datosPColombia;

    memset(&datosPColombia, 0x00, sizeof(datosPColombia));
    memset(nuevoMonto, 0x00, sizeof(nuevoMonto));

    montoAux = atol(globalTramaVenta.totalVenta) - atol(globalTramaVenta.propina);

    sprintf(nuevoMonto, "%ld", montoAux);
    borrarArchivo(discoNetcom, MONTO_PENDIENTE_PUNTOS_COLOMBIA);
    escribirArchivo(discoNetcom, MONTO_PENDIENTE_PUNTOS_COLOMBIA, nuevoMonto, 12);

    switch (opcion) {
        case TARJETA_PC:
            setParameter(TIPO_INICIA_TRANSACCION, "PC");
            resultado = -1;
            break;
        case EFECTIVO:
            strcpy(globalTramaVenta.isQPS, PAGO_EN_EFECTIVO);
            borrarArchivo(discoNetcom, MONTO_PENDIENTE_PUNTOS_COLOMBIA);
            resultado = 1;
            break;
        default:
            //imprimirPuntosColombia(*globalTramaVenta, 0, 0);
            break;
    }
    return resultado;
}

int responderCajaDeclinado(DatosVenta *globalTramaVenta) {

    int resultado = -1;

    char montoAuxiliar[12 + 1];

    memset(montoAuxiliar, 0x00, sizeof(montoAuxiliar));

    setParameter(USO_CAJA_REGISTRADORA, "1");

    strcpy(globalTramaVenta->tipoCuenta, "41");
    strcpy(globalTramaVenta->cardName, "PTOSCOLMB");
    strcpy(globalTramaVenta->codigoRespuesta, "01");

    strcat(globalTramaVenta->totalVenta, "00");

    memcpy(montoAuxiliar, globalTramaVenta->totalVenta, sizeof(montoAuxiliar));
    transaccionRespuestaDatosCaja(*globalTramaVenta, TRANSACCION_TEFF_COMPRAS);
    memset(globalTramaVenta->totalVenta, 0x00, sizeof(globalTramaVenta->totalVenta));
    memcpy(globalTramaVenta->totalVenta, montoAuxiliar, sizeof(globalTramaVenta->totalVenta));

    return resultado;
}

int recibirOpcionMenuPuntosColombia(int opcionLealtad, int tipoDocumento, char *documento,
                                    char *codigoCajero, char *galones, char *monto) {
    int resultado = 0;
    LOGI("opcionLealtad %d", opcionLealtad);
    switch (opcionLealtad) {
        case ACUMULACION_PC:
            resultado = acumularPuntosColombia(tipoDocumento, documento, codigoCajero, galones);
            break;
        case REDENCION_PC:
            resultado = redimirPuntosColombiaConsulta(tipoDocumento, documento, codigoCajero);
            break;
        default:
            break;
    }
    return resultado;
}

int
redimirPuntosColombiaPago(int opcionPago, int tipoDocumento, char *documento, char *codigoCajero,
                          char *galones, char *monto) {

    int resultado = 0;
    int cajaActiva = 0;

    if (verificarHabilitacionCaja() == TRUE) {
        cajaActiva = 1;
    }
    if (cajaActiva == 0) {

    } else {
        if (atoi(globalTramaVenta.ARQC) >= atoi(globalTramaVenta.totalVenta)) {
            opcionPago = PAGO_TOTAL;
        } else {
            opcionPago = 4;
        }
    }

    switch (opcionPago) {
        case PAGO_TOTAL:
            resultado = pagoTotal(&globalTramaVenta, cajaActiva);
            break;
        case PAGO_PARCIAL:
            resultado = pagoParcial(monto);
            break;
        case OTRO_MEDIO:
            resultado = otroMedioPago(opcionPago);
            break;
        case 4:
            //screenMessage("PUNTOS COLOMBIA", "PUNTOS  ", "INSUFICIENTES", "", 2 * 1000);
            //resultado = responderCajaDeclinado(globalTramaVenta);
            break;
        default:
            break;
    }


    if (resultado > 0) {
        //imprimirPuntosColombia(*globalTramaVenta, 28, 1);
    }
}

int pagoTotalConfirmado(char *pin) {

    int resultado = 0;
    int verificar = 0;
    int cajaActiva = 0;

    char montoAuxiliar[12 + 1] = {0x00};

    DatosVenta datosReverso;
    memset(&datosReverso, 0x00, sizeof(datosReverso));
    if (verificarHabilitacionCaja() == TRUE) {
        cajaActiva = 1;
    }
    verificar = verificarArchivo(discoNetcom, REVERSO_PUNTOS_COLOMBIA);

    if (verificar == FS_OK) {
        leerArchivo(discoNetcom, REVERSO_PUNTOS_COLOMBIA, (char *) &datosReverso);
        //resultado = netcomServices(APP_TYPE_LEALTAD, ANULACION_PUNTOS_COLOMBIA,
        //                  TAM_JOURNAL_COMERCIO,
        //                (char *) &datosReverso);
        resultado = 1;//PROVICIONAL
    }
    if (resultado > 0) {

        memcpy(globalTramaVenta.estadoTarifa, "RV", 2);
        memcpy(globalTramaVenta.numeroRecibo2, pin, 4);
        resultado = solicitudPuntosColombia(&globalTramaVenta, REDENCION_PC);

    }


    if (resultado > 0) {
        memcpy(globalTramaVenta.estadoTarifa, "R0", 2);
        sprintf(globalTramaVenta.estadoTransaccion, "%d", TRANSACCION_ACTIVA);
    }
    if (resultado > 0) {
        _guardarTransaccion_(globalTramaVenta);

        //screenMessage("PUNTOS COLOMBIA", "TRANSACCION APROBADA", "NUMERO DE APROBACION",
        //globalTramaVenta->codigoAprobacion,

        //(2 * 1000);
    }
    if (cajaActiva == 1) {
        setParameter(USO_CAJA_REGISTRADORA, "1");
        strcpy(globalTramaVenta.tipoCuenta, "41");
        strcpy(globalTramaVenta.cardName, "PTOSCOLMB");

        if (resultado > 0) {
            strcpy(globalTramaVenta.codigoRespuesta, "00");
        } else {
            strcpy(globalTramaVenta.codigoRespuesta, "01");
            if (strlen(globalTramaVenta.iva) > 0) {
                memcpy(montoAuxiliar, globalTramaVenta.iva, strlen(globalTramaVenta.iva) - 2);
                memset(globalTramaVenta.iva, 0x00, sizeof(globalTramaVenta.iva));
                memcpy(globalTramaVenta.iva, montoAuxiliar, sizeof(globalTramaVenta.iva));
                memset(montoAuxiliar, 0x00, sizeof(montoAuxiliar));
            }

        }

        strcat(globalTramaVenta.totalVenta, "00");
        memcpy(montoAuxiliar, globalTramaVenta.totalVenta, sizeof(montoAuxiliar));
        transaccionRespuestaDatosCaja(globalTramaVenta, TRANSACCION_TEFF_COMPRAS);
        memset(globalTramaVenta.totalVenta, 0x00, sizeof(globalTramaVenta.totalVenta));
        memcpy(globalTramaVenta.totalVenta, montoAuxiliar, sizeof(globalTramaVenta.totalVenta));
    }
}

int pagoParcialConfirmado(char *pin, char *monto) {
    int resultado = 0;
    int verificar = 0;
    long montoExcedente = 0;

    DatosVenta datosReverso;
    memset(&datosReverso, 0x00, sizeof(datosReverso));
    if (verificar == FS_OK) {
        leerArchivo(discoNetcom, REVERSO_PUNTOS_COLOMBIA, (char *) &datosReverso);
        //resultado = netcomServices(APP_TYPE_LEALTAD, ANULACION_PUNTOS_COLOMBIA,
        //TAM_JOURNAL_COMERCIO,
        //(char *) &datosReverso);Provisional
        resultado = 1;
    }
    if (resultado > 0) {
        memcpy(globalTramaVenta.estadoTarifa, "RV", 2);
        memcpy(globalTramaVenta.numeroRecibo2, pin, 4);
        resultado = solicitudPuntosColombia(&globalTramaVenta, REDENCION_PC);
    }


    if (resultado > 0) {
        montoExcedente = atoi(globalTramaVenta.totalVenta) - atoi(monto);
        memcpy(globalTramaVenta.estadoTarifa, "R0", 2);
        sprintf(globalTramaVenta.estadoTransaccion, "%d", TRANSACCION_ACTIVA);
        memset(globalTramaVenta.totalVenta, 0x00, sizeof(globalTramaVenta.totalVenta));
        memset(globalTramaVenta.compraNeta, 0x00, sizeof(globalTramaVenta.compraNeta));
        memcpy(globalTramaVenta.totalVenta, monto, 12);

        if (resultado > 0) {
            _guardarTransaccion_(globalTramaVenta);

            //screenMessage("PUNTOS COLOMBIA", "TRANSACCION APROBADA", "NUMERO DE APROBACION",
            //globalTramaVenta.codigoAprobacion, 2 * 1000);
            if (montoExcedente > 1) {
                memset(monto, 0x00, sizeof(monto));
                sprintf(monto, "%ld", montoExcedente);
                memset(globalTramaVenta.totalVenta, 0x00, sizeof(globalTramaVenta.totalVenta));
                memset(globalTramaVenta.compraNeta, 0x00, sizeof(globalTramaVenta.compraNeta));
                leftPad(globalTramaVenta.totalVenta, monto, 0x30, 12);
                sprintf(globalTramaVenta.compraNeta, "%d", atoi(monto));

                //imprimirPuntosColombia(globalTramaVenta, 0, 0);

                borrarArchivo(discoNetcom, RELACION_PUNTOS_COLOMBIA);
                escribirArchivo(discoNetcom, RELACION_PUNTOS_COLOMBIA, (char *) &globalTramaVenta,
                                sizeof(DatosVenta));
            }
        }
    }
    return resultado;
}

int anulacionPuntosColombia(int tipoAnulacionReverso) {

    int resultado = 0;
    DatosVenta datosVenta;
    char dataCampo93[120 + 1];
    char hora[4 + 1];
    int tipoTransaccion = 0;
    char fecha[8 + 1] = {0x00};
    char buffer[TAM_JOURNAL_COMERCIO] = {0x00};
    obtenerFecha(fecha);

    memset(&datosVenta, 0x00, sizeof(datosVenta));
    memset(dataCampo93, 0x00, sizeof(dataCampo93));
    memset(hora, 0x00, sizeof(hora));
    memset(fecha, 0x00, sizeof(fecha));
    leerArchivo(discoNetcom, REVERSO_PUNTOS_COLOMBIA, buffer);
    memcpy(&datosVenta, buffer, TAM_JOURNAL_COMERCIO);

    if (atoi(datosVenta.estadoTransaccion) == 1 || strcmp(datosVenta.estadoTarifa, "RV") == 0) {
        setParameter(PRODUCTO_PUNTOS_COLOMBIA, "01");
        memcpy(dataCampo93, datosVenta.textoInformacion, 10);
        strcat(dataCampo93, datosVenta.terminalId);
        strcat(dataCampo93, fecha);
        strcat(dataCampo93, hora);
        strcat(dataCampo93, datosVenta.numeroRecibo);
        strcat(dataCampo93, datosVenta.terminalId);
        memcpy(dataCampo93 + 42, datosVenta.terminalId, 8);
        memcpy(dataCampo93 + 50, datosVenta.textoInformacion + 18,
               16); //fecha hora systemtraceorigen
        memcpy(dataCampo93 + 66, datosVenta.codigoCajero, 10);
        memcpy(dataCampo93 + 76, datosVenta.textoInformacion, 10);
        memcpy(dataCampo93 + 86, datosVenta.textoInformacion + 18, 10); //fecha hora
        memset(datosVenta.textoInformacion, 0x00, sizeof(datosVenta.textoInformacion));
        memcpy(datosVenta.textoInformacion, dataCampo93, sizeof(dataCampo93));

        tipoTransaccion = ANULACION_PC;

        if (strcmp(datosVenta.estadoTarifa, "RV") == 0) {
            tipoTransaccion = REVERSO_PC;
        }

        resultado = solicitudPuntosColombia(&datosVenta, tipoTransaccion);

        setParameter(PRODUCTO_PUNTOS_COLOMBIA, "00");
    } else {
        //screenMessage("ANULACION", "RECIBO YA ANULADO", "CANCELANDO", "LA TRANSACCION", 2 * 1000);
        resultado = 0;
    }

    if (resultado > 0 && strcmp(datosVenta.estadoTarifa, "RV") == 0) {
        borrarArchivo(discoNetcom, REVERSO_PUNTOS_COLOMBIA);
    } else if (resultado > 0) {
        if (verificarHabilitacionCaja() == TRUE) {
            memset(datosVenta.codigoRespuesta, 0x00, sizeof(datosVenta.codigoRespuesta));
            strcpy(datosVenta.codigoRespuesta, "00");
            transaccionRespuestaDatosCaja(datosVenta, TRANSACCION_TEFF_ANULACIONES);
        }
        if (resultado > 0) {
            //screenMessage("PUNTOS COLOMBIA", "ANULACION APROBADA",
              //            "CODIGO DE APROBACION:", datosVenta.codigoAprobacion,
                //          2 * 1000);
        }

        memcpy(datosVenta.rrn, "000000", 6);

        cambiarEstadoTxPuntosColombia(datosVenta.numeroRecibo, datosVenta.rrn,
                                      datosVenta.codigoAprobacion,
                                      datosVenta.txnDate, datosVenta.txnTime,
                                      datosVenta.textoAdicional);

        memset(datosVenta.estadoTransaccion, 0x00, sizeof(datosVenta.estadoTransaccion));
        strcpy(datosVenta.estadoTransaccion, "0");
        _guardarDirectorioJournals_(TRANS_COMERCIO, datosVenta.numeroRecibo,
                                  datosVenta.estadoTransaccion);
        memset(datosVenta.estadoTransaccion, 0x00, sizeof(datosVenta.estadoTransaccion));
        //imprimirPuntosColombia(datosVenta, 30, 1)
    }

    return resultado;
}

void cambiarEstadoTxPuntosColombia(char *numeroRecibo, char * rrn, char * aprobacion, char* fecha, char* hora, char* textoAdicional) {

    int resultado = 0;
    int posicion = 0;
    char dataDuplicado[TAM_JOURNAL + 1];
    char auxiliarTiempo[TAM_TIME + 1];
    char auxiliarfecha[TAM_DATE + 1];

    DatosVenta datosVenta;
    memset(auxiliarTiempo, 0x00, sizeof(auxiliarTiempo));
    memset(auxiliarfecha, 0x00, sizeof(auxiliarfecha));
    do {
        memset(dataDuplicado, 0x00, sizeof(dataDuplicado));
        memset(&datosVenta, 0x00, sizeof(datosVenta));
        resultado = buscarArchivo(discoNetcom, JOURNAL, dataDuplicado, posicion, sizeof(DatosVenta));
        datosVenta = obtenerDatosVenta(dataDuplicado);

        if (strncmp(datosVenta.numeroRecibo, numeroRecibo, TAM_NUMERO_RECIBO) == 0) {
            //se cambia el estado
            posicion = posicion + (TAM_TIPO_TRANSACCION + 1);
            resultado = actualizarArchivo(DISCO_NETCOM, JOURNAL, "0", posicion, 1);

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

            actualizarArchivo(DISCO_NETCOM, JOURNAL, hora, posicion, strlen(hora));
            posicion += (TAM_TIME + 1);
            actualizarArchivo(DISCO_NETCOM, JOURNAL, fecha, posicion, strlen(fecha));
            posicion += (TAM_DATE + 1);
            posicion += (TAM_ENTRY_MODE + 1);
            posicion += (TAM_NII + 1);
            posicion += (TAM_POS_CONDITION + 1);
            posicion += (TAM_TRACK2 + 1);
            posicion += (TAM_TERMINAL_ID + 1);
            posicion += (TAM_ACQIRER_ID + 1);
            posicion += (TAM_FIELD_57 + 1);
            posicion += (TAM_FIELD_61 + 1);
            actualizarArchivo(DISCO_NETCOM, JOURNAL, rrn, posicion, strlen(rrn));
            //se cambia el codigo aprobacion
            posicion += (TAM_RRN + 1);
            posicion += (TAM_CARD_NAME + 1);
            posicion += (TAM_COD_APROBACION + 1);
            resultado = actualizarArchivo(DISCO_NETCOM, JOURNAL, aprobacion, posicion, strlen(aprobacion));
            posicion += (TAM_COD_APROBACION + 1);
            posicion += (TAM_GRUPO + 1);
            posicion += (TAM_SUBGRUPO + 1);
            posicion += (TAM_COMPRA_NETA + 1);
            posicion += (TAM_COMPRA_NETA + 1);
            posicion += (TAM_FIELD_42 + 1);
            posicion += (TAM_AID +1);
            posicion += (TAM_ARQC+1);
            posicion += (TAM_CUOTAS+1);
            resultado = actualizarArchivo(DISCO_NETCOM, JOURNAL, textoAdicional, posicion, strlen(textoAdicional));

            resultado = 1;
        }

        posicion += sizeof(DatosVenta);
    } while (resultado == 0);

    if (resultado == 1) {

    }
}