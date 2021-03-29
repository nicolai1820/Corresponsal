//
// Created by ronald on 18/04/2020.
//

#include <jni.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <sys/time.h>
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/socket.h>
#include "netinet/in.h"
#include <arpa/inet.h>
#include "Archivos.h"
#include "native-lib.h"

#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <memory.h>
#include <pthread.h>
#include <dlfcn.h>
#include "Utilidades.h"
#include "Mensajeria.h"
#include "bancolombia.h"
#include "Definiciones.h"
#include "comunicaciones.h"
#include "venta.h"
#include "android/log.h"
#include "Inicializacion.h"
#include "configuracion.h"
#include "lealtad.h"

#define  LOG_TAG    "NETCOM_LEALTAD"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
DatosVenta datosVenta;
ConveniosBigBancolombia datosLealtadtx;
CombinacionesLifemiles combinacionesLifemiles;


int itemsLealtad = 0;

void iniciarLifeMiles(DatosBasicosVenta datosBasicosVenta) {

    int retorno = 0;
    int resultado = 0;
    int opcionLealtad = 0;
    int itemsLealtad = 0;
    int codigoEntidad = 0;
    long lengthTablaLealtad = 0;
    char indicadorLealtad[1 + 1];
    char lealtadLifemiles[1 + 1];

    memset(indicadorLealtad, 0x00, sizeof(indicadorLealtad));
    memset(lealtadLifemiles, 0x00, sizeof(lealtadLifemiles));
    memset(&datosVenta, 0x00, sizeof(datosVenta));
    memset(&datosLealtadtx, 0x00, sizeof(datosLealtadtx));
    setParameter(ACUMULAR_LIFEMILES, "0");//TEMPORAL
    getParameter(ACUMULAR_LIFEMILES, lealtadLifemiles);

    if (atol(lealtadLifemiles) == 1) {
        memcpy(datosVenta.totalVenta, datosBasicosVenta.compraNeta, TAM_COMPRA_NETA);
    } else {
        memcpy(datosVenta.totalVenta, datosBasicosVenta.totalVenta, TAM_COMPRA_NETA);
    }
    memcpy(datosVenta.compraNeta, datosBasicosVenta.compraNeta, TAM_COMPRA_NETA);
    memcpy(datosVenta.iva, datosBasicosVenta.iva, TAM_IVA);
    memcpy(datosVenta.inc, datosBasicosVenta.inc, TAM_IVA);
    memcpy(datosVenta.baseDevolucion, datosBasicosVenta.baseDevolucion, TAM_COMPRA_NETA);

    verificarArchivo(discoNetcom, TX_BIG_BANCOLOMBIA);

    lengthTablaLealtad = tamArchivo(discoNetcom, TX_BIG_BANCOLOMBIA);
    LOGI("lengthTablaLealtad %ld", lengthTablaLealtad);
    itemsLealtad = lengthTablaLealtad / 34;
    LOGI("lealtadLifemiles %d", itemsLealtad);
    if (atol(lealtadLifemiles) == 0) {
        retorno = invocarMenuLifeMiles();
    }

    if (retorno > 0) { return; }

    LOGI("salio ya  %d", itemsLealtad);
}

int invocarMenuLifeMiles() {

    char dataLealtad[36 + 1];
    char fiidEntidad[4 + 1];
    char lineaResumen[100 + 1] = {0x00};
    long lengthTablaLealtad = 0;
    int posicion = 0;
    int indice = 0;
    int opcion = 0;
    DatosComercioBig informacionLealtad;

    memset(&informacionLealtad, 0x00, sizeof(informacionLealtad));
    memset(fiidEntidad, 0x00, sizeof(fiidEntidad));

    datosLealtadtx = capturarDatosBig();

    _convertBCDToASCII_(fiidEntidad, datosLealtadtx.fiid, 4);

    lengthTablaLealtad = tamArchivo(discoNetcom, TX_BIG_BANCOLOMBIA);

    itemsLealtad = lengthTablaLealtad / 34;
    if (itemsLealtad > 90) {
        itemsLealtad = 90;
    }

    ITEM_MENU menuLealtad[itemsLealtad + 1];
    DatosComercioBig informacionLealtadAux[itemsLealtad + 1];

    memset(&menuLealtad, 0x00, sizeof(menuLealtad));
    memset(&informacionLealtadAux, 0x00, sizeof(informacionLealtadAux));
    strcpy(lineaResumen, ";");
    for (indice = 0; indice < itemsLealtad; indice++) {
        memset(dataLealtad, 0x00, sizeof(dataLealtad));
        buscarArchivo(discoNetcom, TX_BIG_BANCOLOMBIA, dataLealtad, posicion, 34);
        sprintf(informacionLealtadAux[indice].nombreComercio, "%.9s", dataLealtad + 5);
        posicion += 34;
        sprintf(menuLealtad[indice].textoPantalla, "%.25s",
                informacionLealtadAux[indice].nombreComercio);
        strcat(lineaResumen, (menuLealtad[indice].textoPantalla));
        strcat(lineaResumen, ";");
    }
    if (itemsLealtad > 2) {
        showMenu("LEALTAD;", itemsLealtad, lineaResumen, 100);
        opcion = 1;
        LOGI("menu %s", lineaResumen);
    } else {
        strcpy(lineaResumen, ";ACUMULACION;");
        strcat(lineaResumen, "REDENCION;");
        showMenu("LIFEMILES;", 2, lineaResumen, 100);
    }


    LOGI("itemsLealtad %s", lineaResumen);
    return opcion;
}

ConveniosBigBancolombia capturarDatosBig() {

    char buffer[50 + 1];
    char binAsignadoAux[6 + 1];
    char binAsignadoAux2[3 + 1];
    char producto[1 + 1];
    char productoAux[2 + 1];
    char bandera[2 + 1];
    char bandera2[4 + 1];
    int indice = 4;
    int posicion = 0;
    ConveniosBigBancolombia datosBig;

    memset(&datosBig, 0x00, sizeof(datosBig));
    memset(buffer, 0x00, sizeof(buffer));
    memset(binAsignadoAux, 0x00, sizeof(binAsignadoAux));
    memset(binAsignadoAux2, 0x00, sizeof(binAsignadoAux2));
    memset(producto, 0x00, sizeof(producto));
    memset(productoAux, 0x00, sizeof(productoAux));
    memset(bandera, 0x00, sizeof(bandera));
    memset(bandera2, 0x00, sizeof(bandera2));

    posicion = 34 * datosVenta.aux1[1];

    buscarArchivo(discoNetcom, TX_BIG_BANCOLOMBIA, buffer, posicion, 34);

    memcpy(producto, buffer + indice, 1);
    _convertBCDToASCII_(productoAux, producto, 2);
    memcpy(datosBig.indentificadorProducto, productoAux, 2);
    indice += (1);

    memcpy(datosBig.nombreConvenio, buffer + indice, 16);
    indice += (16);

    memcpy(binAsignadoAux2, buffer + indice, 3);
    _convertBCDToASCII_(binAsignadoAux, binAsignadoAux2, 6);
    memcpy(datosBig.binAsignado, binAsignadoAux, 6);
    indice += (3);

    memcpy(datosBig.fiid, buffer + indice, 2);
    indice += (2);

    _convertBCDToASCII_(bandera, buffer + indice, 1);
    memcpy(datosBig.bandera1, bandera, 2);
    indice += (1);

    memset(bandera, 0x00, sizeof(bandera));
    _convertBCDToASCII_(bandera, buffer + indice, 1);
    memcpy(datosBig.bandera2, bandera, 2);
    indice += (1);

    memset(bandera, 0x00, sizeof(bandera));
    _convertBCDToASCII_(bandera, buffer + indice, 1);
    memcpy(datosBig.bandera3, bandera, 2);
    indice += (1);

    memset(bandera, 0x00, sizeof(bandera));
    _convertBCDToASCII_(bandera, buffer + indice, 1);
    memcpy(datosBig.bandera4, bandera, 2);
    indice += (1);

    memset(bandera, 0x00, sizeof(bandera));
    _convertBCDToASCII_(bandera, buffer + indice, 1);
    memcpy(datosBig.bandera5, bandera, 2);
    indice += (1);

    memset(bandera, 0x00, sizeof(bandera));
    _convertBCDToASCII_(bandera, buffer + indice, 1);
    memcpy(datosBig.bandera6, bandera, 2);
    indice += (1);

    memset(bandera, 0x00, sizeof(bandera));
    _convertBCDToASCII_(bandera, buffer + indice, 1);
    memcpy(datosBig.bandera7, bandera, 2);
    indice += (1);

    memset(bandera, 0x00, sizeof(bandera));
    _convertBCDToASCII_(bandera, buffer + indice, 1);
    memcpy(datosBig.bandera8, bandera, 2);
    indice += (1);

    _convertBCDToASCII_(bandera2, buffer + indice, 3);
    memcpy(datosBig.banderasLibres, bandera2, 6);

    return datosBig;

}

int recibirOpcionMenu(int opcionLealtad) {

    int retorno = 0;
    int codigoEntidad = 0;

    do {
        memset(datosVenta.aux1, 0x00, sizeof(datosVenta.aux1));
        datosVenta.aux1[1] = codigoEntidad;
        codigoEntidad++;
        retorno = 1;
        LOGI("codigoEntidad %d", codigoEntidad);
        datosLealtadtx = capturarDatosBig();
        LOGI("identificar Producto %d", atoi(datosLealtadtx.indentificadorProducto));
        switch (opcionLealtad) {
            case ACUMULACION_LIFE:
                if (atoi(datosLealtadtx.indentificadorProducto) == 3) {
                    seleccionarItemsDocumento(ACUMULACION_LIFE);
                    //redencionAcumulacionLifeMiles(ACUMULACION_LIFE, datosVenta, datosLealtad);
                } else if (codigoEntidad > itemsLealtad) {
                    codigoEntidad = itemsLealtad + 1;
                }
                break;
            case REDENCION_LIFE:
                if (atoi(datosLealtadtx.indentificadorProducto) == 2) {
                    seleccionarItemsDocumento(REDENCION_LIFE);
                    //redencionAcumulacionLifeMiles(REDENCION_LIFE, datosVenta, datosLealtad);
                } else {
                    codigoEntidad = itemsLealtad + 1;
                }
                break;
            default:
                break;
        }
    } while (codigoEntidad <= itemsLealtad);
    return 0;
}

void seleccionarItemsDocumento(int opcionOtros) {

    char lineaMenu[100 + 1] = {0x00};
    strcpy(lineaMenu, ";CEDULA;");
    strcat(lineaMenu, "No.LIFEMILES;");
    strcat(lineaMenu, "TARJETA;");


    if (opcionOtros == ACUMULACION_LIFE || opcionOtros == ACUMULACION_LIFE_COMPRA) {

        showMenu("ACUMULACION LM;", 3, lineaMenu, 100);
    } else if (opcionOtros == REDENCION_LIFE) {
        showMenu("REDENCION LM;", 3, lineaMenu, 100);
    }
}

CombinacionesLifemiles
definicionBanderas(CombinacionesLifemiles combinacionesLifemiles, int opcionPago,
                   ConveniosBigBancolombia datosLealtad, int opcionOtros) {

    int idx = 0;
    int Banderas = 8;
    char bandera3[2 + 1];
    char bandera4[2 + 1];
    char bandera7[2 + 1];

    memset(bandera3, 0x00, sizeof(bandera3));
    memset(bandera4, 0x00, sizeof(bandera4));
    memset(bandera7, 0x00, sizeof(bandera7));
    memset(&combinacionesLifemiles, 0x00, sizeof(combinacionesLifemiles));

    for (idx = 1; idx <= Banderas; idx++) {
        if (idx == 1) {
            LOGI("atoi(datosLealtad.bandera1) %d", atoi(datosLealtad.bandera1));
            if ((atoi(datosLealtad.bandera1) == RED_TARJETA
                 || atoi(datosLealtad.bandera1) == RED_TAR_CED
                 || atoi(datosLealtad.bandera1) == RED_TAR_NoLIFE
                 || atoi(datosLealtad.bandera1) == RED_TAR_CED_NoLIFE
                 || atoi(datosLealtad.bandera1) == ACU_TARJETA
                 || atoi(datosLealtad.bandera1) == ACU_TAR_CED
                 || atoi(datosLealtad.bandera1) == ACU_TAR_NoLIFE
                 || atoi(datosLealtad.bandera1) == ACU_TAR_CED_NoLIFE) && opcionPago == TARJETA) {
                combinacionesLifemiles.tarjeta = 1;
            }
        } else if (idx == 2) {
            LOGI("atoi(datosLealtad.bandera2) %d", atoi(datosLealtad.bandera2));
            if ((atoi(datosLealtad.bandera2) == RED_CEDULA
                 || atoi(datosLealtad.bandera2) == RED_TAR_CED
                 || atoi(datosLealtad.bandera2) == RED_CED_NoLIFE
                 || atoi(datosLealtad.bandera2) == RED_TAR_CED_NoLIFE
                 || atoi(datosLealtad.bandera2) == ACU_CEDULA
                 || atoi(datosLealtad.bandera2) == ACU_TAR_CED
                 || atoi(datosLealtad.bandera2) == ACU_CED_NoLIFE
                 || atoi(datosLealtad.bandera2) == ACU_TAR_CED_NoLIFE) && opcionPago == CEDULA) {
                combinacionesLifemiles.cedula = 1;
            }
        } else if (idx == 3) {
            memcpy(bandera3, datosLealtad.bandera3, 2);
        } else if (idx == 4) {
            memcpy(bandera4, datosLealtad.bandera4, 2);
        } else if (idx == 5) {
            LOGI("atoi(datosLealtad.bandera5) %d", atoi(datosLealtad.bandera5));
            if ((atoi(datosLealtad.bandera5) == RED_TARJETA && opcionOtros == REDENCION_LIFE &&
                 opcionPago == TARJETA)
                || (atoi(datosLealtad.bandera5) == RED_CEDULA && opcionOtros == REDENCION_LIFE &&
                    opcionPago == CEDULA)
                ||
                (atoi(datosLealtad.bandera5) == RED_NoLIFEMILES && opcionOtros == REDENCION_LIFE &&
                 opcionPago == NoLIFEMILES)
                || (atoi(datosLealtad.bandera5) == RED_TAR_CED && opcionOtros == REDENCION_LIFE &&
                    (opcionPago == TARJETA || opcionPago == CEDULA))
                ||
                (atoi(datosLealtad.bandera5) == RED_TAR_NoLIFE && opcionOtros == REDENCION_LIFE &&
                 (opcionPago == TARJETA || opcionPago == NoLIFEMILES))
                ||
                (atoi(datosLealtad.bandera5) == RED_CED_NoLIFE && opcionOtros == REDENCION_LIFE &&
                 (opcionPago == CEDULA || opcionPago == NoLIFEMILES))
                ||
                (atoi(datosLealtad.bandera5) == RED_TAR_CED_NoLIFE && opcionOtros == REDENCION_LIFE)
                || (atoi(datosLealtad.bandera5) == ACU_TARJETA &&
                    (opcionOtros == ACUMULACION_LIFE || opcionOtros == ACUMULACION_LIFE_COMPRA) &&
                    opcionPago == TARJETA)
                || (atoi(datosLealtad.bandera5) == ACU_CEDULA &&
                    (opcionOtros == ACUMULACION_LIFE || opcionOtros == ACUMULACION_LIFE_COMPRA) &&
                    opcionPago == CEDULA)
                || (atoi(datosLealtad.bandera5) == ACU_NoLIFEMILES &&
                    (opcionOtros == ACUMULACION_LIFE || opcionOtros == ACUMULACION_LIFE_COMPRA) &&
                    opcionPago == NoLIFEMILES)
                || (atoi(datosLealtad.bandera5) == ACU_TAR_CED &&
                    (opcionOtros == ACUMULACION_LIFE || opcionOtros == ACUMULACION_LIFE_COMPRA) &&
                    (opcionPago == TARJETA || opcionPago == CEDULA))
                || (atoi(datosLealtad.bandera5) == ACU_TAR_NoLIFE &&
                    (opcionOtros == ACUMULACION_LIFE || opcionOtros == ACUMULACION_LIFE_COMPRA) &&
                    (opcionPago == TARJETA || opcionPago == NoLIFEMILES))
                || (atoi(datosLealtad.bandera5) == ACU_CED_NoLIFE &&
                    (opcionOtros == ACUMULACION_LIFE || opcionOtros == ACUMULACION_LIFE_COMPRA) &&
                    (opcionPago == CEDULA || opcionPago == NoLIFEMILES))
                || (atoi(datosLealtad.bandera5) == ACU_TAR_CED_NoLIFE &&
                    (opcionOtros == ACUMULACION_LIFE || opcionOtros == ACUMULACION_LIFE_COMPRA))
                    ) {
                combinacionesLifemiles.codigoCajero = 1;
            }
        } else if (idx == 6) {
            LOGI("atoi(datosLealtad.bandera6) %d", atoi(datosLealtad.bandera6));
            if ((atoi(datosLealtad.bandera6) == RED_TARJETA && opcionOtros == REDENCION_LIFE &&
                 opcionPago == TARJETA)
                || (atoi(datosLealtad.bandera6) == RED_CEDULA && opcionOtros == REDENCION_LIFE &&
                    opcionPago == CEDULA)
                ||
                (atoi(datosLealtad.bandera6) == RED_NoLIFEMILES && opcionOtros == REDENCION_LIFE &&
                 opcionPago == NoLIFEMILES)
                || (atoi(datosLealtad.bandera6) == RED_TAR_CED && opcionOtros == REDENCION_LIFE &&
                    (opcionPago == TARJETA || opcionPago == CEDULA))
                ||
                (atoi(datosLealtad.bandera6) == RED_TAR_NoLIFE && opcionOtros == REDENCION_LIFE &&
                 (opcionPago == TARJETA || opcionPago == NoLIFEMILES))
                ||
                (atoi(datosLealtad.bandera6) == RED_CED_NoLIFE && opcionOtros == REDENCION_LIFE &&
                 (opcionPago == CEDULA || opcionPago == NoLIFEMILES))
                ||
                (atoi(datosLealtad.bandera6) == RED_TAR_CED_NoLIFE && opcionOtros == REDENCION_LIFE)
                || (atoi(datosLealtad.bandera6) == ACU_TARJETA &&
                    (opcionOtros == ACUMULACION_LIFE || opcionOtros == ACUMULACION_LIFE_COMPRA) &&
                    opcionPago == TARJETA)
                || (atoi(datosLealtad.bandera6) == ACU_CEDULA &&
                    (opcionOtros == ACUMULACION_LIFE || opcionOtros == ACUMULACION_LIFE_COMPRA) &&
                    opcionPago == CEDULA)
                || (atoi(datosLealtad.bandera6) == ACU_NoLIFEMILES &&
                    (opcionOtros == ACUMULACION_LIFE || opcionOtros == ACUMULACION_LIFE_COMPRA) &&
                    opcionPago == NoLIFEMILES)
                || (atoi(datosLealtad.bandera6) == ACU_TAR_CED &&
                    (opcionOtros == ACUMULACION_LIFE || opcionOtros == ACUMULACION_LIFE_COMPRA) &&
                    (opcionPago == TARJETA || opcionPago == CEDULA))
                || (atoi(datosLealtad.bandera6) == ACU_TAR_NoLIFE &&
                    (opcionOtros == ACUMULACION_LIFE || opcionOtros == ACUMULACION_LIFE_COMPRA) &&
                    (opcionPago == TARJETA || opcionPago == NoLIFEMILES))
                || (atoi(datosLealtad.bandera6) == ACU_CED_NoLIFE &&
                    (opcionOtros == ACUMULACION_LIFE || opcionOtros == ACUMULACION_LIFE_COMPRA) &&
                    (opcionPago == CEDULA || opcionPago == NoLIFEMILES))
                || (atoi(datosLealtad.bandera6) == ACU_TAR_CED_NoLIFE &&
                    (opcionOtros == ACUMULACION_LIFE || opcionOtros == ACUMULACION_LIFE_COMPRA))
                    ) {
                combinacionesLifemiles.pin = 1;
            }
        } else if (idx == 7) {
            memcpy(bandera7, datosLealtad.bandera7, 2);
        } else if (idx == 8) {
            if (atoi(datosLealtad.bandera8) > 0) {
                combinacionesLifemiles.noLifemiles = 1;
            }
        }
    }
    return combinacionesLifemiles;
}

int combinacionesTipoDocumento(int opcionPago, int opcionOtros) {
    int iRet = 0;
    memset(&combinacionesLifemiles, 0x00, sizeof(combinacionesLifemiles));
    LOGI("opcion Pago  %d opcion lealtad %d", opcionPago, opcionOtros);
    combinacionesLifemiles = definicionBanderas(combinacionesLifemiles, opcionPago, datosLealtadtx,
                                                opcionOtros);
    LOGI("codigo cajero  %d ", combinacionesLifemiles.codigoCajero);
    LOGI("Pin  %d ", combinacionesLifemiles.pin);
    LOGI("binAsignado  %s ", datosLealtadtx.binAsignado);

    if (opcionPago == CEDULA && combinacionesLifemiles.cedula == 1) {
        iRet = 1;
    } else if (opcionPago == NoLIFEMILES && combinacionesLifemiles.noLifemiles == 1) {
        iRet = 1;
    } else if (opcionPago == TARJETA && combinacionesLifemiles.tarjeta == 1) {
        iRet = 1;
    }

    return iRet;
}

int procesoAcumulacionRedencionLifeMiles(int lectura, int acumularAjustar, int modoEntrada,
                                         DatosTarjetaAndroid leerTrack) {

    int retorno = 0;
    char bin[9 + 1];
    char vacio[1];
    char mensajeProducto[20];

    memset(vacio, 0x00, sizeof(vacio));
    memset(mensajeProducto, 0x00, sizeof(mensajeProducto));
    memset(datosVenta.cardName, 0x00, sizeof(datosVenta.cardName));
    LOGI("modo de entrada %d", modoEntrada);
    if (modoEntrada == CEDULA) {
        sprintf(datosVenta.isQPS, "%d", CEDULA_LIFE);
    } else if (modoEntrada == NoLIFEMILES) {
        sprintf(datosVenta.isQPS, "%d", No_LIFE);
    } else if (modoEntrada == TARJETA) {
        sprintf(datosVenta.isQPS, "%d", TARJETA_LIFE);
    }

    switch (acumularAjustar) {
        case ACUMULACION_LIFE:
        case ACUMULACION_LIFE_COMPRA:
            strcpy(mensajeProducto, "ACUMULACION");
            break;
        case REDENCION_LIFE:
            strcpy(mensajeProducto, "REDENCION");
            break;
        default:
            break;
    }
    if (strlen(leerTrack.track2) > 0) {
        memset(bin, 0x00, sizeof(bin));
        memcpy(bin, datosVenta.track2, 9);
        retorno = validarBinLealtad(bin);
        retorno = 1;
        if (retorno <= 0) {
            //screenMessage("MENSAJE", "TRANSACCION", "NO", "SOPORTADA", T_OUT(2));
        }
    }

    if (retorno > 0 || lectura == FALSE) {
        if (verificarHabilitacionCaja() == TRUE) {
            //retorno = lealtadCajaLifeMiles(TAM_JOURNAL_COMERCIO, (char*) &datosVenta, acumularAjustar, leerTrack,
            //                             combinacionesLifemiles, datosLealtad);
        } else {
            LOGI("procesarLifeMiles lectura  %d", lectura);
            retorno = procesarLifeMiles(acumularAjustar, FALSE, FALSE, leerTrack);
        }
    }

    if (retorno > 0 && verificarHabilitacionCaja() != TRUE) {
        aprobacionLealtad(acumularAjustar);
    }

    return retorno;
}

int validarBinLealtad(uChar *bin) {

    int iRet = 0;
    int idx = 0;
    int tamTabla = 0;
    uChar rangoBajo[5 + 1];
    uChar rangoAlto[5 + 1];
    uChar datoLealtad[1 + 1];
    uChar rangoBajoAscii[10 + 1];
    uChar rangoAltoAscii[10 + 1];

    tamTabla = tamArchivo(discoNetcom, TABLA_2_INICIALIZACION);

    if (tamTabla > 0) {

        do {
            memset(rangoBajo, 0x00, sizeof(rangoBajo));
            memset(rangoAlto, 0x00, sizeof(rangoAlto));
            memset(datoLealtad, 0x00, sizeof(datoLealtad));
            memset(rangoBajoAscii, 0x00, sizeof(rangoBajoAscii));
            memset(rangoAltoAscii, 0x00, sizeof(rangoAltoAscii));

            idx += 4;
            buscarArchivo(discoNetcom, TABLA_2_INICIALIZACION, rangoBajo, idx, 5);
            idx += 5;
            buscarArchivo(discoNetcom, TABLA_2_INICIALIZACION, rangoAlto, idx, 5);
            idx += 8;
            buscarArchivo(discoNetcom, TABLA_2_INICIALIZACION, datoLealtad, idx, 1);
            idx += 20;

            _convertBCDToASCII_(rangoBajoAscii, rangoBajo, 10);
            _convertBCDToASCII_(rangoAltoAscii, rangoAlto, 10);
            rangoBajoAscii[9] = 0x00;
            rangoAltoAscii[9] = 0x00;

            if (atoi(bin) >= atoi(rangoBajoAscii) && atoi(bin) <= atoi(rangoAltoAscii)) {
                if (datoLealtad[0] & 0x08) {
                    iRet = 1;
                }
                break;
            }
        } while (idx <= tamTabla);
    }

    return iRet;
}

int procesarLifeMiles(int acumularAjustar, int transaccionCaja, int cajaRiviera,
                      DatosTarjetaAndroid leerTrack) {

    int retorno = 1;
    int retornoAux = 0;
    DatosBasicosVenta datosBasicosVenta;
    uChar numeroRecibo[TAM_NUMERO_RECIBO + 1];
    char codigoCajero[5 + 1];
    char lealtadLifemiles[1 + 1];

    memset(&datosBasicosVenta, 0x00, sizeof(datosBasicosVenta));
    memset(numeroRecibo, 0x00, sizeof(numeroRecibo));
    memset(codigoCajero, 0x20, 5);
    memset(lealtadLifemiles, 0x00, sizeof(lealtadLifemiles));
    LOGI("procesarLifeMiles ya seteo  ");
    getParameter(ACUMULAR_LIFEMILES, lealtadLifemiles);

    cargarDatosTransaccionLifeMiles(acumularAjustar, transaccionCaja, leerTrack);

    if (atol(lealtadLifemiles) != 1) {
        concatenarCerosMontos();
    }


    if (retorno > 0) {
        if (retorno > 0) {
            LOGI("realizarTransaccionLifemiles   ");
            retorno = realizarTransaccionLifemiles(acumularAjustar, leerTrack);
        }
    }

    return retorno;
}

void
cargarDatosTransaccionLifeMiles(int acumularAjustar, int transaccionCaja, DatosTarjetaAndroid leerTrack) {

    uChar tipoTransaccion[TAM_TIPO_TRANSACCION + 1];
    uChar documento[10 + 1];
    LOGI("cargarDatosTransaccionLifeMiles  ");
    memset(tipoTransaccion, 0x00, sizeof(tipoTransaccion));
    memset(documento, 0x00, sizeof(documento));

    switch (acumularAjustar) {
        case ACUMULACION_LIFE:
        case ACUMULACION_LIFE_COMPRA:
            memcpy(datosVenta.processingCode, "001000", TAM_CODIGO_PROCESO);
            sprintf(tipoTransaccion, "%d", TRANSACCION_ACUMULAR_LEALTAD);
            break;
        case REDENCION_LIFE:
            memcpy(datosVenta.processingCode, "001000", TAM_CODIGO_PROCESO);
            sprintf(tipoTransaccion, "%d", TRANSACCION_REDENCION_LIFEMILES);
            break;
        default:
            break;
    }

    getTerminalId(datosVenta.terminalId);
    memcpy(datosVenta.posEntrymode, "022", TAM_ENTRY_MODE);

    if (atoi(datosVenta.isQPS) == CEDULA_LIFE || atoi(datosVenta.isQPS) == No_LIFE) {
        if (acumularAjustar == ACUMULACION_LIFE || acumularAjustar == ACUMULACION_LIFE_COMPRA) {
            LOGI("datosVenta.isQPS A%s  ",datosVenta.isQPS);
            memset(datosVenta.track2, 0x00, sizeof(datosVenta.track2));
            memset(datosVenta.fiel42, 0x00, sizeof(datosVenta.fiel42));
            strcpy(datosVenta.track2, "8600000000000000D00000");

            _convertASCIIToBCD_(datosVenta.fiel42, datosLealtadtx.binAsignado, 5);
            _convertASCIIToBCD_(datosVenta.fiel42 + 5, datosVenta.textoAdicionalInicializacion,
                                11);
        } else {
            LOGI("datosVenta.isQPS B %s  ",datosVenta.isQPS);
            memset(datosVenta.track2, 0x00, sizeof(datosVenta.track2));
            memset(datosVenta.tarjetaHabiente, 0x30, 50);
            memcpy(datosVenta.tarjetaHabiente, datosLealtadtx.binAsignado, 6);
            memcpy(datosVenta.track2, datosVenta.tarjetaHabiente, 6);
            strcpy(datosVenta.track2 + 6, "0000000000D00000");
            memcpy(datosVenta.fiel42, datosLealtadtx.binAsignado, 6);
            memcpy(datosVenta.fiel42 + 6, "A", 1);
            LOGI("datosVenta.track2 %s  ",datosVenta.track2);
            LOGI("datosVenta.fiel42 %s  ",datosVenta.fiel42);
        }
    } else {
        if (acumularAjustar == REDENCION_LIFE) {
            memcpy(datosVenta.fiel42, datosVenta.track2, 6);
            memcpy(datosVenta.fiel42 + 6, "A", 1);
        } else {
            LOGI("datosVenta.isQPS C%s  ",datosVenta.isQPS);
            memset(datosVenta.fiel42, 0x00, sizeof(datosVenta.fiel42));
            _convertASCIIToBCD_(datosVenta.fiel42, datosLealtadtx.binAsignado, 5);
            memcpy(datosVenta.tarjetaHabiente, datosVenta.track2, strlen(datosVenta.track2));
            memset(datosVenta.track2, 0x00, sizeof(datosVenta.track2));
            memcpy(datosVenta.track2, "860000", 6);
            memcpy(datosVenta.track2 + 6, datosVenta.tarjetaHabiente + 6,
                   strlen(datosVenta.tarjetaHabiente));
        }

    }

    memcpy(datosVenta.tipoTransaccion, tipoTransaccion, TAM_TIPO_TRANSACCION);

}

int
realizarTransaccionLifemiles(int acumularAjustar, DatosTarjetaAndroid leerTrack) {

    int retorno = 0;
    int intentos = 0;
    char pinblock[8 + 1] = {0x00};
    char trackAdicional[12 + 1];
    char working3DES[16 + 1];
    char variableAux[16 + 1];
    uChar bufferAux[76 + 1];
    TablaCuatroInicializacion tablaCuatro;

    memset(trackAdicional, 0x00, sizeof(trackAdicional));
    memset(variableAux, 0x00, sizeof(variableAux));
    memset(&tablaCuatro, 0x00, sizeof(tablaCuatro));
    memset(bufferAux, 0x00, sizeof(bufferAux));
    LOGI("realizarTransaccionLifemiles   B");
    if (combinacionesLifemiles.pin == 1) {
        if (strlen(leerTrack.track2) > 0) {

        } else {
            memcpy(trackAdicional, datosLealtadtx.binAsignado, 6);
            memcpy(trackAdicional + 6, "0000000000", 10);
        }

    } else {
        retorno = 1;
    }

    if (retorno > 0) {

        do {
            LOGI("armarTramaLifeMiles  ");
            armarTramaLifeMiles(intentos, acumularAjustar, pinblock);
            LOGI("procesarLealtad  ");
            retorno = procesarLealtad(acumularAjustar);

            if (retorno == -2) {
                intentos++;
            }

            if (intentos == MAX_INTENTOS_VENTA) {
                //screenMessage("COMUNICACION", "REINTENTE", "TRANSACCION", "", 2 * 1000);
                retorno = -1;
            }

        } while (retorno == -2 && intentos < MAX_INTENTOS_VENTA);
    }
    return retorno;
}

void
armarTramaLifeMiles(int intentos, int acumularAjustar, char *pinblock) {

    uChar nii[TAM_NII + 1];
    uChar tipoTransaccion[TAM_TIPO_TRANSACCION + 1];
    uChar campo48[18 + 1];
    uChar codigoCajero[TAM_CAJERO_LIFE_MILES + 1];
    char campo63[25 + 1];
    ISOHeader isoHeader;
    ResultTokenPack resultTokenPack;
    int indice = 0;
    char variableAux[11 + 1];
    char tipoCompra[2 + 1];

    memset(variableAux, 0x00, sizeof(variableAux));
    memset(tipoCompra, 0x00, sizeof(tipoCompra));
    memset(nii, 0x00, sizeof(nii));
    memset(tipoTransaccion, 0x00, sizeof(tipoTransaccion));
    memset(campo48, 0x00, sizeof(campo48));
    memset(&isoHeader, 0x00, sizeof(isoHeader));
    memset(&resultTokenPack, 0x00, sizeof(resultTokenPack));
    memset(codigoCajero, 0x00, sizeof(codigoCajero));
    memset(campo63, 0x00, sizeof(campo63));
    LOGI("armarTramaLifeMiles  B");
    getParameter(NII, nii);

    if (atoi(datosVenta.codigoCajero) != 0) {
        memcpy(campo63, datosVenta.codigoCajero, 5);
    } else {
        memcpy(campo63, "     ", 5);
    }

    indice += 5;

    if (acumularAjustar == ACUMULACION_LIFE || acumularAjustar == ACUMULACION_LIFE_COMPRA) {
        switch (atoi(datosVenta.tipoCuenta)) {
            case 10:
            case 20:
                memcpy(tipoCompra, "CD", 2);
                break;
            case 30:
                memcpy(tipoCompra, "CC", 2);
                break;
            default:
                memcpy(tipoCompra, "ND", 2);
                break;
        }

    } else {
        memcpy(tipoCompra, "  ", 2);
    }

    memcpy(campo63 + indice, tipoCompra, 2);
    indice += 2;
    memcpy(campo63 + indice, datosVenta.isQPS + 1, 1);
    indice += 1;

    if (atol(datosVenta.textoAdicionalInicializacion) != 0) {
        leftPad(variableAux, datosVenta.textoAdicionalInicializacion, 0x30, 11);
        memcpy(campo63 + indice, variableAux, 11);
    } else {
        memcpy(campo63 + indice, "           ", 11);
    }
    indice += 11;

    if (acumularAjustar == ACUMULACION_LIFE) {
        memcpy(campo63 + indice, datosLealtadtx.binAsignado, 6);
    } else if (acumularAjustar == ACUMULACION_LIFE_COMPRA) {
        memcpy(campo63 + indice, datosVenta.track2, 6);
    } else {
        memcpy(campo63 + indice, "      ", 6);
    }

    if (intentos == 0) {

        if (acumularAjustar == AJUSTAR_PUNTOS) {
            strcpy(datosVenta.numeroRecibo, datosVenta.numeroRecibo2);
        } else {
            generarInvoice(datosVenta.numeroRecibo);
        }
        generarSystemTraceNumber(datosVenta.systemTrace, (char *) "");
    }

    memcpy(datosVenta.msgType, "0200", TAM_MTI);
    memcpy(datosVenta.nii, nii + 1, TAM_NII);
    memcpy(datosVenta.posConditionCode, "00", TAM_POS_CONDITION);

    isoHeader.TPDU = 60;
    memcpy(isoHeader.destination, nii, TAM_NII + 1);
    memcpy(isoHeader.source, nii, TAM_NII + 1);
    setHeader(isoHeader);
    setMTI(datosVenta.msgType);

    setField(3, datosVenta.processingCode, TAM_CODIGO_PROCESO);
    setField(4, datosVenta.totalVenta, TAM_COMPRA_NETA);
    setField(11, datosVenta.systemTrace, TAM_SYSTEM_TRACE);
    setField(22, datosVenta.posEntrymode, TAM_ENTRY_MODE);
    setField(24, datosVenta.nii, TAM_NII);
    setField(25, datosVenta.posConditionCode, TAM_POS_CONDITION);
    setField(35, datosVenta.track2, strlen(datosVenta.track2));
    setField(41, datosVenta.terminalId, TAM_TERMINAL_ID);
    setField(42, datosVenta.fiel42, TAM_FIELD_42);
    setField(48, campo48, 18);

    if (strlen(pinblock) > 0) {
        setField(52, pinblock, LEN_PIN_DATA);
    }

    setField(62, datosVenta.numeroRecibo, TAM_NUMERO_RECIBO);

    memset(datosVenta.textoAdicional, 0x00, sizeof(datosVenta.textoAdicional));
    memcpy(datosVenta.textoAdicional, campo63, 25);
    setField(63, campo63, 25);
}

int procesarLealtad(int acumularAjustar) {

    int retorno = 0;
    ResultISOPack resultIsoPack;
    uChar dataRecibida[512];

    memset(dataRecibida, 0x00, sizeof(dataRecibida));
    memset(&resultIsoPack, 0x00, sizeof(resultIsoPack));

    resultIsoPack = packISOMessage();

    if (resultIsoPack.responseCode > 0) {
        if (acumularAjustar == ACUMULACION_LIFE) {
            retorno = realizarTransaccion(resultIsoPack.isoPackMessage, resultIsoPack.totalBytes,
                                          dataRecibida,
                                          TRANSACCION_ACUMULAR_LEALTAD, CANAL_DEMANDA,
                                          REVERSO_GENERADO);
        } else {
            retorno = realizarTransaccion(resultIsoPack.isoPackMessage, resultIsoPack.totalBytes,
                                          dataRecibida,
                                          TRANSACCION_REDENCION_LIFEMILES, CANAL_DEMANDA,
                                          REVERSO_GENERADO);
        }
    }

    if (retorno > 0) {
        retorno = desempaquetarLealtad(dataRecibida, retorno, acumularAjustar);
    }

    return retorno;
}

int desempaquetarLealtad(uChar *datosRecibidos, int tamRecibidos, int acumularAjustar) {

    int retorno = 0;
    uChar codigoRespuesta[TAM_CODIGO_RESPUESTA + 1];
    ResultISOUnpack resultIsoUnpack;
    ISOFieldMessage isoFieldMessage;

    char valorTotal[12 + 1];
    char mensajeError[50 + 1] = {0x00};

    memset(codigoRespuesta, 0x00, sizeof(codigoRespuesta));
    memset(&resultIsoUnpack, 0x00, sizeof(resultIsoUnpack));
    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(valorTotal, 0x00, sizeof(valorTotal));

    resultIsoUnpack = unpackISOMessage(datosRecibidos, tamRecibidos);

    if (resultIsoUnpack.responseCode > 0) {

        isoFieldMessage = getField(39);
        if (isoFieldMessage.totalBytes > 0) {
            memcpy(codigoRespuesta, isoFieldMessage.valueField, isoFieldMessage.totalBytes);
        }
    }

    if (strcmp(codigoRespuesta, "00") == 0) {

        retorno = verificacionTramaLealtad(acumularAjustar);

        if (retorno > 0) {
            borrarArchivo(discoNetcom, ARCHIVO_REVERSO);

            sprintf(datosVenta.estadoTransaccion, "%d", TRANSACCION_ACTIVA);
            strcpy(datosVenta.codigoRespuesta, "00");
            tomarDatosPuntos();

            if (atoi(datosVenta.tipoTransaccion) == TRANSACCION_ACUMULAR_LEALTAD
                || atoi(datosVenta.tipoTransaccion) == TRANSACCION_REDENCION_LIFEMILES) {

                memcpy(valorTotal, datosVenta.totalVenta, 12);
                memset(datosVenta.totalVenta, 0x00, sizeof(datosVenta.totalVenta));
                memset(datosVenta.tarjetaHabiente, 0x00, sizeof(datosVenta.tarjetaHabiente));
                leftTrim(datosVenta.totalVenta, valorTotal, 0x30);
                _guardarTransaccion_(datosVenta);
                _guardarDirectorioJournals_(TRANS_COMERCIO, datosVenta.numeroRecibo,
                                            datosVenta.estadoTransaccion);
            } else if (atoi(datosVenta.tipoTransaccion) == TRANSACCION_AJUSTAR_LEALTAD) {
                cambiarEstadoAcumulacionAjuste();
            }
        } else {
            setParameter(INDICADOR_REVERSO_ISMP, "1");
            retorno = generarReverso();

            if (retorno > 0) {
                retorno = -2;
            }
        }
    } else {
        borrarArchivo(discoNetcom, ARCHIVO_REVERSO);
        errorRespuestaTransaccion(codigoRespuesta, mensajeError);
    }

    return retorno;
}

int verificacionTramaLealtad(int acumularAjustar) {

    int resultado = 0;
    int validacion = 1;
    long variableAux = 0;
    uChar stringAux[20];
    ISOFieldMessage isoFieldMessage;
    ResultTokenUnpack resultTokenUnpack;
    TokenMessage tokenMessage;
    char informacion[11 + 1];

    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(stringAux, 0x00, sizeof(stringAux));
    memset(&resultTokenUnpack, 0x00, sizeof(resultTokenUnpack));
    memset(&tokenMessage, 0x00, sizeof(tokenMessage));
    memset(informacion, 0x00, sizeof(informacion));

    isoFieldMessage = getField(12);
    memcpy(datosVenta.txnTime, isoFieldMessage.valueField, 6);
    isoFieldMessage = getField(13);
    memcpy(datosVenta.txnDate, isoFieldMessage.valueField, 4);
    isoFieldMessage = getField(37);
    memcpy(datosVenta.rrn, isoFieldMessage.valueField + 6, 6);
    variableAux = atol(datosVenta.rrn);
    isoFieldMessage = getField(38);
    memcpy(datosVenta.codigoAprobacion, isoFieldMessage.valueField, 6);
    isoFieldMessage = getField(54);
    memcpy(datosVenta.appLabel, isoFieldMessage.valueField, 12);

    if (datosVenta.txnTime[0] != 0x00 && datosVenta.txnDate != 0x00 && variableAux != 0
        && datosVenta.codigoAprobacion[0] != 0x00) {

        /// Validar Codigo Proceso-Monto-System Trace-N. Terminal.
        isoFieldMessage = getField(3);
        validacion = strncmp(isoFieldMessage.valueField, datosVenta.processingCode,
                             isoFieldMessage.totalBytes);

        if (validacion == 0) {
            isoFieldMessage = getField(11);
            validacion = strncmp(isoFieldMessage.valueField, datosVenta.systemTrace,
                                 isoFieldMessage.totalBytes);
        }

        if (validacion == 0) {
            isoFieldMessage = getField(41);
            validacion = strncmp(isoFieldMessage.valueField, datosVenta.terminalId,
                                 isoFieldMessage.totalBytes);
        }

        isoFieldMessage = getField(60);
        resultTokenUnpack = unpackTokenMessage(isoFieldMessage.valueField,
                                               isoFieldMessage.totalBytes);

        tokenMessage = _getTokenMessage_("QI");

        if (tokenMessage.totalBytes > 0) {
            memcpy(datosVenta.textoInformacion, tokenMessage.valueToken, tokenMessage.totalBytes);
        }

    }

    if (validacion == 0) {
        resultado = 1;
    } else {
        resultado = -1;
    }

    return resultado;
}

void tomarDatosPuntos() {

    ISOFieldMessage isoFieldMessage;

    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    isoFieldMessage = getField(4);

    if (isoFieldMessage.totalBytes > 0) {
        memset(datosVenta.totalVenta, 0x00, sizeof(datosVenta.totalVenta));
        memcpy(datosVenta.totalVenta, isoFieldMessage.valueField, isoFieldMessage.totalBytes);
    }

    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    isoFieldMessage = getField(42);

    if (isoFieldMessage.totalBytes > 0) {
        memset(datosVenta.fiel42, 0x00, sizeof(datosVenta.fiel42));
        memcpy(datosVenta.fiel42, isoFieldMessage.valueField, isoFieldMessage.totalBytes);
    }

    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    isoFieldMessage = getField(54);

    if (isoFieldMessage.totalBytes > 0) {
        memset(datosVenta.tokenVivamos, 0x00, sizeof(datosVenta.tokenVivamos));
        memcpy(datosVenta.tokenVivamos, isoFieldMessage.valueField, isoFieldMessage.totalBytes);
    }
}

void cambiarEstadoAcumulacionAjuste() {

    DatosJournals datosJournals;
    DatosVenta datosVentaAux;

    memset(&datosJournals, 0x00, sizeof(datosJournals));

    strcpy(datosVenta.estadoTransaccion, "0");
    _guardarTransaccion_(datosVenta);
    _guardarDirectorioJournals_(TRANS_COMERCIO, datosVenta.numeroRecibo,
                                datosVenta.estadoTransaccion);

    datosJournals = buscarReciboMulticorresponsal(datosVenta.numeroRecibo);

    if (datosJournals.journal == TRANS_COMERCIO) {
        buscarArchivo(discoNetcom, JOURNAL, (char *) &datosVentaAux,
                      datosJournals.posicionTransaccion,
                      TAM_JOURNAL_COMERCIO);
        memcpy(datosVentaAux.codigoAprobacion, datosVenta.codigoAprobacion, TAM_COD_APROBACION);
        memset(datosVentaAux.compraNeta, 0x00, sizeof(datosVentaAux.compraNeta));
        strcpy(datosVentaAux.compraNeta, datosVentaAux.totalVenta);
        memset(datosVentaAux.totalVenta, 0x00, sizeof(datosVentaAux.totalVenta));
        strcpy(datosVentaAux.totalVenta, datosVenta.totalVenta);
        memset(datosVentaAux.tipoTransaccion, 0x00, sizeof(datosVentaAux.tipoTransaccion));
        sprintf(datosVentaAux.tipoTransaccion, "%d", TRANSACCION_AJUSTAR_LEALTAD);
        actualizarArchivo(discoNetcom, JOURNAL, (char *) &datosVentaAux,
                          datosJournals.posicionTransaccion,
                          TAM_JOURNAL_COMERCIO);
    }
}

void aprobacionLealtad(int acumularAjustar) {

    uChar puntosTransaccionAux[15 + 1];
    uChar puntosAcumulados[12 + 1];
    uChar puntosTransaccion[15 + 1];

    memset(puntosAcumulados, 0x00, sizeof(puntosAcumulados));
    memset(puntosTransaccionAux, 0x00, sizeof(puntosTransaccionAux));
    memset(puntosTransaccion, 0x00, sizeof(puntosTransaccion));

    memcpy(puntosTransaccion, datosVenta.fiel42 + 8, 7);
    _convertBCDToASCII_(puntosTransaccionAux, puntosTransaccion, 14);

    memset(puntosTransaccion, 0x00, sizeof(puntosTransaccion));
    leftTrim(puntosTransaccion, puntosTransaccionAux, '0');
    leftTrim(puntosAcumulados, datosVenta.tokenVivamos, '0');

    verificarCeroPuntos(puntosTransaccion, puntosAcumulados);



    //screenMessage("CODIGO APROBACION", datosVenta.codigoAprobacion, "NUMERO DE APROBACION", "", T_OUT(2));
    imprimirTiketLealtad(RECIBO_CLIENTE, FALSE);

}


void verificarCeroPuntos(uChar *puntosTransaccion, uChar *puntosAcumulados) {

    if (strlen(puntosTransaccion) == 0) {
        strcpy(puntosTransaccion, "0");
    }

    if (strlen(puntosAcumulados) == 0) {
        strcpy(puntosAcumulados, "0");
    }
}

void imprimirTiketLealtad(int tipoTicket, int duplicado) {

    int isColsubsidio = 0;
    int transaccionCaja = FALSE;
    int indice = 0;
    uChar parMensajePie[250 + 1];
    uChar parEspacio[50 + 1];
    uChar parPie[40 + 1];
    uChar valorFormato[20 + 1];
    uChar valorFormatoAux[20 + 1];
    uChar valorIva[20 + 1];
    uChar aprob[15 + 1];
    uChar rrnVenta[20 + 1];
    uChar stringAux[20 + 1];
    uChar parVersion[20 + 1];
    uChar fechaExpiracion[20 + 1];
    uChar cuotas[9 + 1];
    uChar parVenta[20 + 1];
    uChar cardName[15 + 1];
    uChar bin[9 + 1];
    uChar parTextoTotal[20 + 1];
    uChar parLogo[35 + 1];
    uChar parTextoCredito[300 + 1];
    uChar parIdComercio[TAM_ID_COMERCIO + 1];
    uChar parcodigoCajero[11 + 1];
    uChar parInformacion[TAM_TEXTO_INFORMACION + 1];
    uChar auxiliarCodigoCajero[2 + 1];
    uChar codigoCajeroVacio[TAM_CODIGO_CAJERO + 1];
    uChar cajaReciboVacio[TAM_CODIGO_CAJERO + 1];
    uChar auxiliar[20 + 1];
    uChar line1Puntos[30 + 1];
    uChar line2Puntos[30 + 1];
    uChar line3Puntos[30 + 1];
    uChar line4Puntos[30 + 1];
    uChar line5Puntos[30 + 1];
    uChar totalVenta[20 + 1];
    uChar puntosTransaccion[15 + 1];
    uChar puntosTransaccionAux[15 + 1];
    uChar puntosAcumulados[12 + 1];
    uChar totalAux[TAM_COMPRA_NETA + 1];
    uChar parDuplicado[30 + 1];
    uChar linea1[23 + 1];
    uChar linea2[23 + 1];
    uChar linea3[23 + 1];
    uChar linea4[23 + 1];
    uChar linea5[23 + 1];
    uChar totalAnulacion[20 + 1];
    uChar lineaDeclinada1[23 + 1];
    uChar lineaDeclinada2[23 + 1];
    uChar parTextoCompraNeta[20 + 1];
    uChar parCompraNeta[20 + 1];
    uChar parTextoIva[20 + 1];
    uChar parTextoInc[20 + 1];
    uChar parIva[20 + 1];
    uChar parInc[20 + 1];
    uChar idDoc[11 + 1];
    uChar parIdDoc[20 + 1];
    uChar parTextoPropina[20 + 1];
    uChar parPropina[20 + 1];
    uChar textoCredito[] =
            "PAGARE INCONDICIONALMENTE A LA VISTA Y A LA ORDEN DEL ACREEDOR EL VALOR TOTAL DE ESTE PAGARE JUNTO CON LOS INTERESES A LAS TASAS MAXIMAS PERMITIDAS POR LA LEY.";
    uChar parFirma[50 + 1];
    uChar parCedula[50 + 1];
    uChar parTel[50 + 1];
    EmisorAdicional emisorAdicional;
    TablaUnoInicializacion tablaUno;

    memset(codigoCajeroVacio, 0x00, sizeof(codigoCajeroVacio));
    memset(cajaReciboVacio, 0x00, sizeof(cajaReciboVacio));
    memset(parTextoTotal, 0x00, sizeof(parTextoTotal));
    memset(parLogo, 0x00, sizeof(parLogo));
    memset(parMensajePie, 0x00, sizeof(parMensajePie));
    memset(parEspacio, 0x00, sizeof(parEspacio));
    memset(parPie, 0x00, sizeof(parPie));
    memset(parTextoCredito, 0x00, sizeof(parTextoCredito));
    memset(aprob, 0x00, sizeof(aprob));
    memset(rrnVenta, 0x00, sizeof(rrnVenta));
    memset(valorFormato, 0x00, sizeof(valorFormato));
    memset(valorFormatoAux, 0x00, sizeof(valorFormatoAux));
    memset(valorIva, 0x00, sizeof(valorIva));
    memset(line1Puntos, 0x00, sizeof(line1Puntos));
    memset(line2Puntos, 0x00, sizeof(line2Puntos));
    memset(line3Puntos, 0x00, sizeof(line1Puntos));
    memset(line4Puntos, 0x00, sizeof(line2Puntos));
    memset(line5Puntos, 0x00, sizeof(line1Puntos));
    memset(parVersion, 0x00, sizeof(parVersion));
    memset(fechaExpiracion, 0x00, sizeof(fechaExpiracion));
    memset(cuotas, 0x00, sizeof(cuotas));
    memset(parVenta, 0x00, sizeof(parVenta));
    memset(cardName, 0x00, sizeof(cardName));
    memset(parIdComercio, 0x00, sizeof(parIdComercio));
    memset(parcodigoCajero, 0x00, sizeof(parcodigoCajero));
    memset(parIdDoc, 0x00, sizeof(parIdDoc));
    memset(auxiliarCodigoCajero, 0x00, sizeof(auxiliarCodigoCajero));
    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(puntosTransaccion, 0x00, sizeof(puntosTransaccion));
    memset(puntosTransaccionAux, 0x00, sizeof(puntosTransaccionAux));
    memset(puntosAcumulados, 0x00, sizeof(puntosAcumulados));
    memset(parDuplicado, 0x00, sizeof(parDuplicado));
    memset(parInformacion, 0x00, sizeof(parInformacion));
    memset(linea1, 0x00, sizeof(linea1));
    memset(linea2, 0x00, sizeof(linea2));
    memset(linea3, 0x00, sizeof(linea3));
    memset(linea4, 0x00, sizeof(linea4));
    memset(linea5, 0x00, sizeof(linea5));
    memset(lineaDeclinada1, 0x00, sizeof(lineaDeclinada1));
    memset(lineaDeclinada2, 0x00, sizeof(lineaDeclinada2));
    memset(totalAnulacion, 0x00, sizeof(totalAnulacion));
    memset(parTextoCompraNeta, 0x00, sizeof(parTextoCompraNeta));
    memset(parCompraNeta, 0x00, sizeof(parCompraNeta));
    memset(parTextoIva, 0x00, sizeof(parTextoIva));
    memset(parTextoInc, 0x00, sizeof(parTextoInc));
    memset(parIva, 0x00, sizeof(parIva));
    memset(parInc, 0x00, sizeof(parInc));
    memset(parTextoPropina, 0x00, sizeof(parTextoPropina));
    memset(parPropina, 0x00, sizeof(parPropina));
    memset(parFirma, 0x00, sizeof(parFirma));
    memset(parCedula, 0x00, sizeof(parCedula));
    memset(parTel, 0x00, sizeof(parTel));

    transaccionCaja = strcmp(datosVenta.track2, "8610000000000000000D0000") == 0 ? TRUE : FALSE;

    int existeEmisor = 0;
    memset(bin, 0x00, sizeof(bin));
    memcpy(bin, datosVenta.track2, 9);
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

    /////ES COLSUBSIDIO/////
    if (strncmp(bin, "8800", 4) == 0) {
        isColsubsidio = TRUE;
    }

    sprintf(fechaExpiracion, "%15s", " ");
    sprintf(cuotas, "%7s", " ");

    uChar tipoCuenta[12 + 1];
    uChar hora[10 + 1];
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
    strcpy(numeroRecibo, "RECIBO: ");
    strcat(numeroRecibo, datosVenta.numeroRecibo);

    /// TOTAL VENTA ///
    memset(totalVenta, 0x00, sizeof(totalVenta));
    memset(totalAux, 0x00, sizeof(totalAux));
    leftTrim(totalAux, datosVenta.totalVenta, '0');
    formatString(0x00, 0, totalAux, strlen(totalAux) - 2, valorFormato, 1);
    if (atoi(datosVenta.tipoTransaccion) == TRANSACCION_ACUMULAR_LEALTAD
        || atoi(datosVenta.tipoTransaccion) == TRANSACCION_BIG_BANCOLOMBIA
        || atoi(datosVenta.tipoTransaccion) == TRANSACCION_REDENCION_LIFEMILES) {

        if (atoi(datosVenta.tipoTransaccion) == TRANSACCION_BIG_BANCOLOMBIA && duplicado != 2) {
            sprintf(parTextoCompraNeta, "%-11s", "COMPRA NETA");
            memset(valorFormato, 0x00, sizeof(valorFormato));

            if (strncmp(datosVenta.modoDatafono, "06", 2)) {
                formatString(0x00, 0, datosVenta.compraNeta, strlen(datosVenta.compraNeta) - 2,
                             valorFormato, 1);
            } else {
                formatString(0x00, 0, datosVenta.compraNeta, strlen(datosVenta.compraNeta),
                             valorFormato, 1);
            }

            sprintf(parCompraNeta, "%12s", valorFormato);

            if (strlen(datosVenta.iva) >= 2) {
                memset(stringAux, 0x00, sizeof(stringAux));
                memcpy(stringAux, datosVenta.iva, strlen(datosVenta.iva) - 2);
                memset(valorFormato, 0x00, sizeof(valorFormato));
                formatString(0x00, 0, stringAux, strlen(stringAux), valorFormato, 1);
                sprintf(parTextoIva, "%-11s", "IVA");
                sprintf(parIva, "%12s", valorFormato);
            } else {
                sprintf(parTextoIva, "%s", NO_IMPRIMIR);
                strcpy(parIva, NO_IMPRIMIR);
            }

            if (strlen(datosVenta.inc) >= 2) {
                memset(stringAux, 0x00, sizeof(stringAux));
                memcpy(stringAux, datosVenta.inc, strlen(datosVenta.inc) - 2);
                memset(valorFormato, 0x00, sizeof(valorFormato));
                formatString(0x00, 0, stringAux, strlen(stringAux), valorFormato, 1);
                sprintf(parTextoInc, "%-11s", "INC");
                sprintf(parInc, "%12s", valorFormato);
            } else {
                sprintf(parTextoInc, "%s", NO_IMPRIMIR);
                strcpy(parInc, NO_IMPRIMIR);
            }

            if (tablaUno.options1.tipProcessing == 1 && atoi(datosVenta.propina) > 0) {

                sprintf(parTextoPropina, "%-11s", "PROPINA");

                memset(stringAux, 0x00, sizeof(stringAux));
                if (strlen(datosVenta.propina) >= 2) {
                    memcpy(stringAux, datosVenta.propina, strlen(datosVenta.propina) - 2);
                }
                memset(valorFormato, 0x00, sizeof(valorFormato));
                formatString(0x00, 0, stringAux, strlen(stringAux), valorFormato, 1);
                sprintf(parPropina, "%12s", valorFormato);
            } else {
                strcpy(parPropina, NO_IMPRIMIR);
                strcpy(parTextoPropina, NO_IMPRIMIR);
            }

            if ((atoi(datosVenta.estadoTarifa) == 0 && tipoTicket == RECIBO_COMERCIO)
                || (strncmp(datosVenta.cardName, "MAESTRO", 7) != 0 &&
                    tipoTicket == RECIBO_COMERCIO)) {

                if (atoi(datosVenta.tipoCuenta) == 30) {
                    strcpy(parTextoCredito, textoCredito);
                } else {
                    strcpy(parTextoCredito, NO_IMPRIMIR);
                }

                strcpy(parFirma, "FIRMA: _____________________________________");
                strcpy(parCedula, "C.C:   _____________________________________");
                strcpy(parTel, "TEL:   _____________________________________");
            } else {
                if (tipoTicket == RECIBO_COMERCIO) {
                    strcpy(parFirma, "\n        TRANSACCION AUTENTICADA CON PIN \n");
                } else {
                    strcpy(parFirma, NO_IMPRIMIR);
                }
                strcpy(parTextoCredito, NO_IMPRIMIR);
                strcpy(parCedula, NO_IMPRIMIR);
                strcpy(parTel, NO_IMPRIMIR);
            }
        } else {
            strcpy(parTextoCompraNeta, NO_IMPRIMIR);
            strcpy(parCompraNeta, NO_IMPRIMIR);
            sprintf(parTextoIva, "%s", NO_IMPRIMIR);
            strcpy(parIva, NO_IMPRIMIR);
            sprintf(parTextoInc, "%s", NO_IMPRIMIR);
            strcpy(parInc, NO_IMPRIMIR);
            strcpy(parPropina, NO_IMPRIMIR);
            strcpy(parTextoPropina, NO_IMPRIMIR);
            strcpy(parTextoCredito, NO_IMPRIMIR);
            strcpy(parFirma, NO_IMPRIMIR);
            strcpy(parCedula, NO_IMPRIMIR);
            strcpy(parTel, NO_IMPRIMIR);

            if ((atoi(datosVenta.estadoTarifa) == 0 && tipoTicket == RECIBO_COMERCIO
                 && atoi(datosVenta.tipoTransaccion) != TRANSACCION_REDENCION_LIFEMILES
                 && atoi(datosVenta.tipoTransaccion) != TRANSACCION_ACUMULAR_LEALTAD)
                || (strncmp(datosVenta.cardName, "MAESTRO", 7) != 0 && tipoTicket == RECIBO_COMERCIO
                    && atoi(datosVenta.tipoTransaccion) != TRANSACCION_REDENCION_LIFEMILES
                    && atoi(datosVenta.tipoTransaccion) != TRANSACCION_ACUMULAR_LEALTAD)) {

                if (atoi(datosVenta.tipoCuenta) == 30) {
                    strcpy(parTextoCredito, textoCredito);
                } else {
                    strcpy(parTextoCredito, NO_IMPRIMIR);
                }

                strcpy(parFirma, "FIRMA: _____________________________________");
                strcpy(parCedula, "C.C:   _____________________________________");
                strcpy(parTel, "TEL:   _____________________________________");
            } else {
                if (tipoTicket == RECIBO_COMERCIO &&
                    atoi(datosVenta.tipoTransaccion) != TRANSACCION_REDENCION_LIFEMILES
                    && atoi(datosVenta.tipoTransaccion) != TRANSACCION_ACUMULAR_LEALTAD) {
                    strcpy(parFirma, "\n        TRANSACCION AUTENTICADA CON PIN \n");
                } else {
                    strcpy(parFirma, NO_IMPRIMIR);
                }
                strcpy(parTextoCredito, NO_IMPRIMIR);
                strcpy(parCedula, NO_IMPRIMIR);
                strcpy(parTel, NO_IMPRIMIR);
            }
        }


        strcpy(lineaDeclinada1, NO_IMPRIMIR);
        strcpy(lineaDeclinada2, NO_IMPRIMIR);

        if (duplicado == 2) {
            strcpy(parTextoTotal, NO_IMPRIMIR);
            strcpy(totalVenta, NO_IMPRIMIR);

            memcpy(lineaDeclinada1, "TRANSACCION RECHAZADA", 23);
            memcpy(lineaDeclinada2, "FONDOS INSUFICIENTES", 23);

        } else {
            memset(valorFormato, 0x00, sizeof(valorFormato));
            memset(totalAux, 0x00, sizeof(totalAux));
            leftTrim(totalAux, datosVenta.totalVenta, '0');
            formatString(0x00, 0, totalAux, strlen(totalAux) - 2, valorFormato, 1);

            if (datosVenta.estadoTransaccion[0] == '0') {
                sprintf(totalAnulacion, "-%s", valorFormato);
                sprintf(totalVenta, "%13s", totalAnulacion);
                sprintf(parTextoTotal, "%-10s", "ANULACION");
            } else {
                sprintf(totalVenta, "%15s", valorFormato);
                sprintf(parTextoTotal, "%-8s", "TOTAL");
            }
        }

    } else if (atoi(datosVenta.tipoTransaccion) == TRANSACCION_AJUSTAR_LEALTAD) {
        strcpy(valorFormatoAux, "-");
        strcat(valorFormatoAux, valorFormato);
        sprintf(totalVenta, "%15s", valorFormatoAux);
    }
    strcpy(aprob, "\x0F  APRO: ");
    strcat(aprob, datosVenta.codigoAprobacion);

    sprintf(rrnVenta, "RRN: %s", datosVenta.rrn);

    /// ULTIMOS CUATRO ///
    char ultimosCuatro[6 + 1];
    memset(ultimosCuatro, 0x00, sizeof(ultimosCuatro));

    if (atoi(datosVenta.ultimosCuatro) != 0) {
        strcpy(ultimosCuatro, "**");
        if (transaccionCaja == TRUE) {
            strcat(ultimosCuatro, "****");
        } else {
            strcat(ultimosCuatro, datosVenta.ultimosCuatro);
        }
    } else {
        strcat(ultimosCuatro, "      ");
    }

    /// FECHA ///
    formatoFecha(datosVenta.txnDate);

    /// HORA ///
    sprintf(hora, "%.2s:%.2s:%.2s", datosVenta.txnTime, datosVenta.txnTime + 2,
            datosVenta.txnTime + 4);

    switch (atoi(datosVenta.tipoCuenta)) {
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

            if (strlen(datosVenta.cuotas) > 0 && atoi(datosVenta.cuotas) > 0) {
                sprintf(cuotas, "CUO: %2s", datosVenta.cuotas);
            } else {
                strcpy(cuotas, "       ");
            }
            sprintf(fechaExpiracion, "EXP: %-10s", datosVenta.fechaExpiracion);
            break;
        case 40:
            if (isColsubsidio == TRUE) {
                sprintf(tipoCuenta, "%7s", "Cr");
            } else if (atoi(datosVenta.creditoRotativo) == 8) {
                sprintf(tipoCuenta, "%7s", "SP");
            } else {
                sprintf(tipoCuenta, "%7s", "Ah");
            }
            memset(fechaExpiracion, 0x00, sizeof(fechaExpiracion));
            memset(cuotas, 0x00, sizeof(cuotas));

            if (strlen(datosVenta.cuotas) > 0 && atoi(datosVenta.cuotas) > 0) {
                sprintf(cuotas, "CUO: %2s", datosVenta.cuotas);
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
            if (strlen(datosVenta.cuotas) > 0 && atoi(datosVenta.cuotas) > 0) {
                sprintf(cuotas, "CUO: %2s", datosVenta.cuotas);
            } else {
                strcpy(cuotas, "       ");
            }

            sprintf(fechaExpiracion, "EXP: %-10s", datosVenta.fechaExpiracion);
            break;
        case 44:
            sprintf(tipoCuenta, "%7s", "BE");
            break;
        case 45:
            sprintf(tipoCuenta, "%7s", "RT");
            memset(fechaExpiracion, 0x00, sizeof(fechaExpiracion));
            memset(cuotas, 0x00, sizeof(cuotas));

            if (strlen(datosVenta.cuotas) > 0 && atoi(datosVenta.cuotas) > 0) {
                sprintf(cuotas, "CUO: %2s", datosVenta.cuotas);
            } else {
                strcpy(cuotas, "       ");
            }
            sprintf(fechaExpiracion, "EXP: %-10s", datosVenta.fechaExpiracion);
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
            if (strlen(datosVenta.cuotas) > 0 && atoi(datosVenta.cuotas) > 0) {
                sprintf(cuotas, "CUO: %2s", datosVenta.cuotas);
            } else {
                strcpy(cuotas, "       ");
            }

            sprintf(fechaExpiracion, "EXP: %-10s", datosVenta.fechaExpiracion);
            break;
        default:
            break;
    }

    if (isIssuerEspecial(datosVenta.cardName) == 1) {
        memset(cuotas, 0x00, sizeof(cuotas));
        sprintf(cuotas, "CUO: %2s", datosVenta.cuotas);
    }

    if (tipoTicket == RECIBO_CLIENTE) {
        strcpy(parTextoCredito, NO_IMPRIMIR);
        strcpy(parEspacio, NO_IMPRIMIR);
        strcpy(parPie, "*** CLIENTE ***");
    } else if (tipoTicket == RECIBO_QPS) {
        strcpy(parTextoCredito, NO_IMPRIMIR);
        strcpy(parPie, NO_IMPRIMIR);
    } else if (tipoTicket == RECIBO_COMERCIO) {
        if (atoi(datosVenta.tipoTransaccion) != TRANSACCION_BIG_BANCOLOMBIA) {
            strcpy(parTextoCredito, NO_IMPRIMIR);
            strcpy(parPie, "*** CLIENTE ***");
        } else {
            strcpy(parPie, "*** COMERCIO ***");
        }
    }

    if (transaccionCaja == TRUE) {
        sprintf(cardName, "%-15s", "CEDULA");
        sprintf(tipoCuenta, "%7s", "     Ah");
        sprintf(fechaExpiracion, "%15s", "               ");
        sprintf(cuotas, "%7s", "       ");
    } else {
        sprintf(cardName, "%-15s", datosVenta.cardName);
    }

    getParameter(VERSION_SW, parVersion);
    getParameter(REVISION_SW, parVersion + strlen(parVersion));

    //////////// SI ES MULTIPOS ///////////////
    if (strlen(datosVenta.codigoComercio) > 0) {
        strcpy(parIdComercio, datosVenta.codigoComercio);

        /////// SI ES RECIBO DE MULTIPOS ///////
        if (atoi(datosVenta.tipoTransaccion) == TRANSACCION_MULTIPOS) {
            memset(stringAux, 0x00, sizeof(stringAux));
            getParameter(TARIFA_ADMIN, stringAux);
        }
    } else {
        strcpy(parIdComercio, tablaUno.receiptLine3);
    }

    if (atoi(datosVenta.tipoTransaccion) != TRANSACCION_ACUMULAR_LEALTAD
        && atoi(datosVenta.tipoTransaccion) != TRANSACCION_REDENCION_LIFEMILES) {
        strcpy(parIdDoc, NO_IMPRIMIR);
        memset(stringAux, 0x00, sizeof(stringAux));
        if (strncmp(datosVenta.codigoCajero, codigoCajeroVacio, 10) == 0
            || strcmp(datosVenta.codigoCajero, NO_IMPRIMIR_CAJERO) == 0) {
            strcpy(parcodigoCajero, NO_IMPRIMIR);
        } else {
            memset(auxiliar, 0x00, sizeof(auxiliar));
            leftTrim(auxiliar, datosVenta.codigoCajero, 0x20);
            leftPad(stringAux, auxiliar, 0x30, TAM_CODIGO_CAJERO);
            memcpy(parcodigoCajero, stringAux, LEN_TEFF_CODIGO_CAJERO);
        }
    } else {

        memset(idDoc, 0x00, sizeof(idDoc));
        leftPad(idDoc, datosVenta.textoAdicionalInicializacion, 0x30, 11);
        strcpy(parcodigoCajero, NO_IMPRIMIR);

        if (atoi(datosVenta.isQPS) == CEDULA_LIFE &&
            atoi(datosVenta.tipoTransaccion) != TRANSACCION_AJUSTAR_LEALTAD) {
            strcpy(parIdDoc, "DOC. ID: *******");
            strcat(parIdDoc, idDoc + 7);
        } else if (atoi(datosVenta.isQPS) == No_LIFE &&
                   atoi(datosVenta.tipoTransaccion) != TRANSACCION_AJUSTAR_LEALTAD) {
            strcpy(parIdDoc, "DOC. LM: *******");
            strcat(parIdDoc, idDoc + 7);
        } else {
            strcpy(parIdDoc, NO_IMPRIMIR);
            strcpy(parcodigoCajero, NO_IMPRIMIR);
        }

    }

    memset(line1Puntos, 0x00, sizeof(line1Puntos));
    memset(line2Puntos, 0x00, sizeof(line2Puntos));
    memcpy(puntosTransaccion, datosVenta.fiel42 + 8, 7);
    _convertBCDToASCII_(puntosTransaccionAux, puntosTransaccion, 14);
    memset(puntosTransaccion, 0x00, sizeof(puntosTransaccion));
    leftTrim(puntosTransaccion, puntosTransaccionAux, '0');
    leftTrim(puntosAcumulados, datosVenta.tokenVivamos, '0');

    memset(parVenta, 0x00, sizeof(parVenta));
    memset(line1Puntos, 0x00, sizeof(line1Puntos));
    memset(line2Puntos, 0x00, sizeof(line2Puntos));
    sprintf(parVenta, "%-16s", "LEALTAD");

    strcpy(line1Puntos, NO_IMPRIMIR);
    strcpy(line2Puntos, NO_IMPRIMIR);
    strcpy(line3Puntos, NO_IMPRIMIR);
    strcpy(line4Puntos, NO_IMPRIMIR);
    strcpy(line5Puntos, NO_IMPRIMIR);

    if ((atoi(datosVenta.tipoTransaccion) == TRANSACCION_ACUMULAR_LEALTAD
         || atoi(datosVenta.tipoTransaccion) == TRANSACCION_REDENCION_LIFEMILES)
        && atoi(datosVenta.tipoTransaccion) != TRANSACCION_AJUSTAR_LEALTAD) {

        verificarCeroPuntos(puntosTransaccion, puntosAcumulados);

        if (atoi(datosVenta.tipoTransaccion) == TRANSACCION_ACUMULAR_LEALTAD
            && atoi(datosVenta.tipoTransaccion) != TRANSACCION_AJUSTAR_LEALTAD) {
            memset(puntosAcumulados, 0x00, sizeof(puntosAcumulados));
            leftTrim(puntosAcumulados, datosVenta.appLabel, '0');
            sprintf(line1Puntos, "%-17s%7s", "MILLAS ACUMULADAS", puntosAcumulados);
            strcpy(line2Puntos, NO_IMPRIMIR);
        } else {
//			Telium_STelium_Pprintf(line1Puntos, "%-10s%14s", "PUNTOS RED", puntosTransaccion);
//			Telium_STelium_Pprintf(line1Puntos, "%-16s%8s", "MILLAS REDIMIDAS", puntosTransaccion);

            strcpy(line1Puntos, NO_IMPRIMIR);
            strcpy(line2Puntos, NO_IMPRIMIR);
            strcpy(lineaDeclinada1, NO_IMPRIMIR);
            strcpy(lineaDeclinada2, NO_IMPRIMIR);
            strcpy(parTextoCompraNeta, NO_IMPRIMIR);
            strcpy(parCompraNeta, NO_IMPRIMIR);
            sprintf(parTextoIva, "%s", NO_IMPRIMIR);
            strcpy(parIva, NO_IMPRIMIR);
            sprintf(parTextoInc, "%s", NO_IMPRIMIR);
            strcpy(parInc, NO_IMPRIMIR);
            strcpy(parPropina, NO_IMPRIMIR);
            strcpy(parTextoPropina, NO_IMPRIMIR);
            strcpy(parTextoCredito, NO_IMPRIMIR);
//			strcpy(parFirma, NO_IMPRIMIR);
//			strcpy(parCedula, NO_IMPRIMIR);
//			strcpy(parTel, NO_IMPRIMIR);
        }

        sprintf(parTextoTotal, "%-8s", "TOTAL");

        if (strlen(datosVenta.textoInformacion) > 0 &&
            atoi(datosVenta.tipoTransaccion) != TRANSACCION_ACUMULAR_LEALTAD) {
            strcpy(parInformacion, datosVenta.textoInformacion);
        } else {
            strcpy(parInformacion, NO_IMPRIMIR);
        }
    } else if (atoi(datosVenta.tipoTransaccion) == TRANSACCION_BIG_BANCOLOMBIA) {

        memcpy(linea1, datosVenta.textoInformacion + indice, 23);
        indice += 23;
        memcpy(linea2, datosVenta.textoInformacion + indice, 23);
        indice += 23;
        memcpy(linea3, datosVenta.textoInformacion + indice, 23);
        indice += 23;
        memcpy(linea4, datosVenta.textoInformacion + indice, 23);
        indice += 23;
        memcpy(linea5, datosVenta.textoInformacion + indice, 23);

        strcpy(line1Puntos, linea1);
        strcpy(line2Puntos, linea2);
        strcpy(line3Puntos, linea3);
        strcpy(line4Puntos, linea4);
        strcpy(line5Puntos, linea5);

        strcpy(parTextoCredito, NO_IMPRIMIR);
        strcpy(parFirma, NO_IMPRIMIR);
        strcpy(parCedula, NO_IMPRIMIR);
        strcpy(parTel, NO_IMPRIMIR);

    } else {
        strcpy(line1Puntos, NO_IMPRIMIR);
        strcpy(line2Puntos, NO_IMPRIMIR);
        strcpy(lineaDeclinada1, NO_IMPRIMIR);
        strcpy(lineaDeclinada2, NO_IMPRIMIR);
        strcpy(parTextoCompraNeta, NO_IMPRIMIR);
        strcpy(parCompraNeta, NO_IMPRIMIR);
        sprintf(parTextoIva, "%s", NO_IMPRIMIR);
        strcpy(parIva, NO_IMPRIMIR);
        sprintf(parTextoInc, "%s", NO_IMPRIMIR);
        strcpy(parInc, NO_IMPRIMIR);
        strcpy(parPropina, NO_IMPRIMIR);
        strcpy(parTextoPropina, NO_IMPRIMIR);
        strcpy(parTextoCredito, NO_IMPRIMIR);
        strcpy(parFirma, NO_IMPRIMIR);
        strcpy(parCedula, NO_IMPRIMIR);
        strcpy(parTel, NO_IMPRIMIR);
        sprintf(parTextoTotal, "%-8s", "AJUSTE");
    }

    if (duplicado == TRUE) {
        strcpy(parDuplicado, "     ** DUPLICADO **");
    } else {
        strcpy(parDuplicado, NO_IMPRIMIR);
    }

    HasMap hash[] =
            {
                    {"_PAR_FECHA_",       datosVenta.txnDate},
                    {"_PAR_VERSION_",     parVersion},
                    {"_PAR_HORA_",        hora},
                    {"_PAR_NESTABLE_",    tablaUno.defaultMerchantName},
                    {"_PAR_DESTABLE_",    tablaUno.receiptLine2},
                    {"_PAR_CUNICO_",      parIdComercio},
                    {"_PAR_TERMINAL_",    datosVenta.terminalId},
                    {"_PAR_TARJETA_",     cardName},
                    {"_PAR_CUATROU_",     ultimosCuatro},
                    {"_PAR_RECIBO_",      numeroRecibo},
                    {"_PAR_RRN_",         rrnVenta},
                    {"_PAR_VENTA_",       parVenta},
                    {"_PAR_COMPRA_",      parTextoCompraNeta},
                    {"_PAR_VCOMPRA_",     parCompraNeta},
                    {"_PAR_IVA_",         parTextoIva},
                    {"_PAR_VIVA_",        parIva},
                    {"_PAR_INC_",         parTextoInc},
                    {"_PAR_VINC_",        parInc},
                    {"_PAR_PROPINA_",     parTextoPropina},
                    {"_PAR_VPROPINA_",    parPropina},
                    {"_PAR_LINEADECLI1_", lineaDeclinada1},
                    {"_PAR_LINEADECLI2_", lineaDeclinada2},
                    {"_PAR_TOTAL_",       parTextoTotal},
                    {"_PAR_VTOTAL_",      totalVenta},
                    {"_PAR_CREDITO_",     parTextoCredito},
                    {"_PAR_APRO_",        aprob},
                    {"_PAR_CUENTA_",      tipoCuenta},
                    {"_PAR_ESPACIO_",     parEspacio},
                    {"_PAR_DUPLICADO_",   parDuplicado},
                    {"_PAR_FIRMA_",       parFirma},
                    {"_PAR_CEDULA_",      parCedula},
                    {"_PAR_TELEFONO_",    parTel},
                    {"_PAR_PIE_",         parPie},
                    {"_PAR_INFORMACION_", parInformacion},
                    {"_PAR_EXP_",         fechaExpiracion},
                    {"_PAR_CUOTAS_",      cuotas},
                    {"_PAR_PUNTOS1_",     line1Puntos},
                    {"_PAR_PUNTOS2_",     line2Puntos},
                    {"_PAR_PUNTOS3_",     line3Puntos},
                    {"_PAR_PUNTOS4_",     line4Puntos},
                    {"_PAR_PUNTOS5_",     line5Puntos},
                    {"_PAR_CCAJERO_",     parcodigoCajero},
                    {"_PAR_IDDOC_",       parIdDoc},
                    {"FIN_HASH",          ""}
            };

    imprimirTicket(hash, parLogo, "/LEALTAD.TPL");
}

int ingresoDocumentoFromJava(char *documento, int modoEntrada) {
    int resultado = 1;
    int resultadoNoLifemiles = 0;

    long tLifemiles = 0;
    long noLifemiles = 0;
    long noLifemilesAux = 0;

    char varDocumento[10 + 1] = {0x00};
    char varDocumento2[10 + 1] = {0x00};
    char variableAux[11 + 1] = {0x00};
    char variableAux2[10 + 1] = {0x00};
    char digitoChequeo[1 + 1] = {0x00};


    switch (modoEntrada) {
        case CEDULA:
            memcpy(datosVenta.textoAdicionalInicializacion, documento, strlen(documento));
            break;
        case NoLIFEMILES:
            leftPad(variableAux2, documento, 0x30, 11);
            memcpy(digitoChequeo, variableAux2 + 10, 1);
            memcpy(varDocumento2, variableAux2, 10);
            tLifemiles = atol(varDocumento2);
            LOGI("tLifemiles %d", tLifemiles);
            if (tLifemiles <= 2000000000) {
                noLifemiles = atol(varDocumento2) / 7;
                noLifemilesAux = noLifemiles * 7;
                resultadoNoLifemiles = atol(varDocumento2) - noLifemilesAux;
                memset(varDocumento, 0x00, strlen(variableAux));
                LOGI("resultadoNoLifemiles %ld", tLifemiles);
                LOGI("digito de chequeo %d", atoi(digitoChequeo));
                if (resultadoNoLifemiles != atoi(digitoChequeo)) {
                    resultado = -1;
                } else {
                    memcpy(datosVenta.textoAdicionalInicializacion, variableAux, 12);
                    resultado = 1;
                }
            } else {
                memcpy(datosVenta.textoAdicionalInicializacion, variableAux, 12);
                resultado = 1;
            }
            break;
        default:
            break;
    }

    return resultado;
}

void concatenarCerosMontos() {
    strcat(datosVenta.compraNeta, "00");
    strcat(datosVenta.totalVenta, "00");
    strcat(datosVenta.iva, "00");
    strcat(datosVenta.inc, "00");
    strcat(datosVenta.propina, "00");
    strcat(datosVenta.baseDevolucion, "00");
}