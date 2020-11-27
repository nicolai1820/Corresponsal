//
// Created by ronald on 27/03/2020.
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

#define CUENTA_AHORROS          1
#define CUENTA_CORRIENTE        2
#define CUENTA_CRED_ROTATIVO    3
#define AHORROS                 10
#define CORRIENTE               20
#define CREDITO                 30
#define MULTI_BOLSILLOS            50
#define  LOG_TAG    "NETCOM_VENTA_EMV"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

int globalsizeP55;
int globalTipoCuenta = 0;
int globalPinOffLine = 0;

uChar globalCVC2[3 + 1];
uChar globalDocumento[10 + 1];
uChar globalPinBlokP52[17 + 1];
uChar globalpaqueteP55[1024 + 1];
uChar codigoRespuestaGlobal[3 + 1];

ResultISOUnpack globalResultadoUnpackEmV;


int realizarVentaEMV(DatosTarjetaAndroid datosTarjeta, DatosBasicosVenta datosBasicosVenta) {


    int nBytes = 0;
    int tamano = 0;
    int codigo = 0;
    int reverso = 0;
    int intentos = 0;
    int acivarReverso = 0;
    int transaccionOK = 0;
    int reversoEnviado = 0;

    uChar pan[20 + 1] = {0x00};
    char codigoRespuesta[2] = {0x00};
    char transaccion[2 + 1] = {0x00};
    char mensajeError[21 + 1] = {0x00};
    char dataRecibida[2048 + 1] = {0x00};
    char dataRespaldo[2048 + 1] = {0x00};

    ResultISOPack salidaIsoPackMessage;
    ISOFieldMessage isoFieldMessage;

    TablaTresInicializacion dataIssuerEMV;

    globalsizeP55 = datosTarjeta.sizeP55;
    globalPinOffLine = datosTarjeta.isPin;
    memset(&globalTramaVenta, 0x00, sizeof(globalTramaVenta));
    memset(globalDocumento, 0x00, sizeof(globalDocumento));
    comportamientoTarjeta(datosTarjeta);
    memcpy(globalTramaVenta.tipoCuenta, datosTarjeta.tipoCuenta, strlen(datosTarjeta.tipoCuenta));
    leftPad(globalTramaVenta.cuotas, datosTarjeta.cuotas, 0x30,2);
    memcpy(globalTramaVenta.ultimosCuatro, datosTarjeta.pan + strlen(datosTarjeta.pan) - 4, 4);
    memcpy(globalTramaVenta.fechaExpiracion, datosTarjeta.fechaExpiracion,
           strlen(datosTarjeta.fechaExpiracion));
    memcpy(globalTramaVenta.ARQC, datosTarjeta.arqc, strlen(datosTarjeta.arqc));

    memcpy(globalpaqueteP55, datosTarjeta.data55, globalsizeP55);
    if (globalPinOffLine == 1) {
        LOGI("pinBlock %s ", datosTarjeta.pinblock);

        memcpy(globalPinBlokP52, datosTarjeta.pinblock, strlen(datosTarjeta.pinblock));
    }
    memcpy(globalTramaVenta.totalVenta, datosBasicosVenta.totalVenta,
           strlen(datosBasicosVenta.totalVenta));
    memcpy(globalTramaVenta.iva, datosBasicosVenta.iva, strlen(datosBasicosVenta.iva));
    memcpy(globalTramaVenta.inc, datosBasicosVenta.inc, strlen(datosBasicosVenta.inc));
    memcpy(globalTramaVenta.baseDevolucion, datosBasicosVenta.baseDevolucion, strlen(datosBasicosVenta.baseDevolucion));
    LOGI("baseDevolucion %s",globalTramaVenta.baseDevolucion);
    memcpy(globalTramaVenta.propina, datosBasicosVenta.propina, strlen(datosBasicosVenta.propina));
    memcpy(globalTramaVenta.compraNeta, datosBasicosVenta.compraNeta,
           strlen(datosBasicosVenta.compraNeta));
    memcpy(globalDocumento,datosTarjeta.documento, strlen(datosTarjeta.documento));
    memcpy(globalCVC2,datosTarjeta.cvc2, strlen(datosTarjeta.cvc2));
    getParameter(TIPO_TRANSACCION, transaccion);
    if (atoi(transaccion) != TRANSACCION_SALDO) {
        acivarReverso = REVERSO_GENERADO;
    } else {
        acivarReverso = REVERSO_NO_GENERADO;
    }

    memset(&salidaIsoPackMessage, 0x00, sizeof(salidaIsoPackMessage));

    salidaIsoPackMessage = ensambleTramaTransacion();
    borrarArchivo(discoNetcom, (char *)RESPALDO); //hay que quitar
    escribirArchivo(discoNetcom, (char *)RESPALDO, salidaIsoPackMessage.isoPackMessage,
                    salidaIsoPackMessage.totalBytes);
    do {

        if (atoi(transaccion) == TRANSACCION_BIG_BANCOLOMBIA) {
            tamano = realizarTransaccion(salidaIsoPackMessage.isoPackMessage,
                                         salidaIsoPackMessage.totalBytes,
                                         dataRecibida, TRANSACCION_BIG_BANCOLOMBIA,
                                         CANAL_PERMANENTE, acivarReverso);
        } else {
            if (intentos == 0) {
                if (atoi(globalTramaVenta.totalVenta) != 0 ||
                    atoi(transaccion) == TRANSACCION_SALDO ||
                    atoi(transaccion) == TRANSACCION_FOCO_SODEXO) {

                    tamano = realizarTransaccion(salidaIsoPackMessage.isoPackMessage,
                                                 salidaIsoPackMessage.totalBytes, dataRecibida,
                                                 TRANSACCION_VENTA, CANAL_PERMANENTE,
                                                 acivarReverso);
                    LOGI("tamano recibida %d", tamano);

                }

            } else {
                memset(dataRecibida, 0x00, sizeof(dataRecibida));
                nBytes = 0;
                nBytes = leerArchivo(discoNetcom, (char *)RESPALDO, dataRespaldo);
                tamano = realizarTransaccion(dataRespaldo, nBytes, dataRecibida, TRANSACCION_VENTA,
                                             CANAL_PERMANENTE,
                                             acivarReverso);
            }
        }

        if (tamano > 0) {
            LOGI("tamano umpack %d", tamano);
            globalResultadoUnpackEmV = unpackISOMessage(dataRecibida, tamano);
            LOGI("salio de  umpack %d", tamano);
            isoFieldMessage = getField(39);
            memcpy(codigoRespuesta, isoFieldMessage.valueField, isoFieldMessage.totalBytes);
            memset(globalTramaVenta.codigoRespuesta, 0x00,
                   sizeof(globalTramaVenta.codigoRespuesta));
            memcpy(globalTramaVenta.codigoRespuesta, isoFieldMessage.valueField,
                   isoFieldMessage.totalBytes);
            memset(codigoRespuestaGlobal, 0x00, sizeof(codigoRespuestaGlobal));

            codigo = strncmp(codigoRespuesta, "00", 2);
            LOGI("codigo %d", codigo);
            if (codigo == 0) {

                memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
                isoFieldMessage = getField(60);
                //// Verificar Puntos Del Emisor ////
                if (isoFieldMessage.totalBytes > 0) {
                    verificarPuntosEmisor(&globalTramaVenta, isoFieldMessage.valueField,
                                          isoFieldMessage.totalBytes);
                }

                if (intentos == 0 && atoi(transaccion) != TRANSACCION_BIG_BANCOLOMBIA
                    && atoi(transaccion) != TRANSACCION_FOCO_SODEXO) {
                    _verificarConsultaInformacion_(&globalTramaVenta, dataRecibida, tamano);
                }

                if (atoi(transaccion) != TRANSACCION_SALDO) {
                    reverso = verificacionLlegadaTrama(&globalTramaVenta);
                    if (reverso == 1) { reverso = 0; } else { reverso = 1; }
                    LOGI("reverso %d", reverso);

                    //reverso = validacionDatosRespuesta(dataRecibida, tamano);
                } else {
                    //verificarConsultaSaldo(dataRecibida, tamano);
                    reverso = FALSE;
                }
                if (reverso == TRUE) { ///cambiar logica

                    //se coloca codigo de respuesta en 01 para mandar rechado a la caja cuando el reverso es errado
                    memcpy(globalTramaVenta.codigoRespuesta, "01", 2);
                    reversoEnviado = generarReverso();
                    if (reversoEnviado > 0) {
                        intentos++;
                    } else {
                        intentos = MAX_INTENTOS_VENTA + 1;
                    }

                    if (intentos == MAX_INTENTOS_VENTA) {
                        //screenMessage("COMUNICACION", "REINTENTE", "TRANSACCION", "", 2 * 1000);
                    }
                    transaccionOK = FALSE;

                } else {
                    transaccionOK = TRUE;
                    intentos = MAX_INTENTOS_VENTA + 1;
                }
            } else {

                memcpy(globalTramaVenta.codigoRespuesta, "01", 2);
                transaccionOK = FALSE;
                borrarArchivo(discoNetcom, (char *)ARCHIVO_REVERSO);
                borrarArchivo(discoNetcom, (char *)ARCHIVO_DCC_LOGGIN_REVERSO);
                errorRespuestaTransaccion(codigoRespuesta, mensajeError);
                intentos = MAX_INTENTOS_VENTA + 1;
            }

        } else {
            transaccionOK = FALSE;
        }
    } while (tamano > 0 && intentos < MAX_INTENTOS_VENTA);

    if (transaccionOK == TRUE) {

        // hInputTLVTree = transaccionExitosaTLVTREE(&hOutputTree, dataRecibida, tamano);
        // para validar si debe enviar reversos al terminar proceso

        memset(codigoRespuestaGlobal, 0x00, sizeof(codigoRespuestaGlobal));
        LOGI("codigoRespuestaGlobal %s", codigoRespuestaGlobal);
        strcpy(codigoRespuestaGlobal, codigoRespuesta);
        borrarArchivo(discoNetcom, (char *)ARCHIVO_REVERSO);
        borrarArchivo(discoNetcom,(char *)RESPALDO);//EMV DEBE COMPLETAR EL FLUJO EN EL STOP, PROVISIONAL CUARENTENA
        sprintf(globalTramaVenta.tipoTransaccion, "%02d", TRANSACCION_VENTA);
        sprintf(globalTramaVenta.estadoTransaccion, "%d", TRANSACCION_ACTIVA);
        LOGI("fecha ex %s", globalTramaVenta.fechaExpiracion);
        _guardarTransaccion_(globalTramaVenta);
        LOGI("imprimir %s ", globalTramaVenta.cardName);
        imprimirTicketVenta(globalTramaVenta, RECIBO_COMERCIO, RECIBO_COMERCIO);
        return 1;
    }

    //TlvTree_ReleaseEx(&hOutputTree);
    return 0;
}

ResultISOPack ensambleTramaTransacion() {

    uChar nii[TAM_NII + 1] = {0x00};

    ISOHeader isoHeader8583;

    ResultISOPack resultadoIsoPackMessage;

    memset(&isoHeader8583, 0x00, sizeof(isoHeader8583));
    memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));

    getParameter(NII, nii);

    isoHeader8583.TPDU = 60;
    memcpy(isoHeader8583.destination, nii, TAM_NII + 1);
    memcpy(isoHeader8583.source, nii, TAM_NII + 1);
    setHeader(isoHeader8583);

    transaccionVentaEMV();

    resultadoIsoPackMessage = packISOMessage();
    return resultadoIsoPackMessage;
}

void transaccionVentaEMV() {

    int lengthTextoAdicional = 0;

    char bin[9 + 1] = {0x00};
    char productoPPRO[1 + 1] = {0x00};
    char bufferAuxiliar[15 + 1] = {0x00};
    char bufferCorrrimiento[15 + 1] = {0x30};

    uChar recibo[6 + 1] = {0x00};
    uChar tokenDC[64 + 1] = {0x00};
    uChar tokenDP[64 + 1] = {0x00};
    uChar field61[26 + 1] = {0x00};
    uChar tipoTransaccion[2 + 1] = {0x00};

    DatosVenta auxiliar;

    TablaTresInicializacion tablaTres;

    memset(&auxiliar, 0x00, sizeof(auxiliar));
    memset(&tablaTres, 0x00, sizeof(tablaTres));

    setMTI((char *)"0200");

    cargarCampos_4_48_57(auxiliar.totalVenta);
    sprintf(globalTramaVenta.processingCode, "00%s00", globalTramaVenta.tipoCuenta);
    memcpy(bin, globalTramaVenta.track2, 9);
    tablaTres = _traerIssuer_(atol(bin));

    setField(3, globalTramaVenta.processingCode, 6);

    if (strlen(globalDocumento) == 0) {
        strcpy(bufferAuxiliar, "0000000000");
    }

    cargarCampos_11_22_35_24_25_41_52_53_55();

    ///////////// BANDERA INDICANDO SI LA TARJETA TIENE HABILITADO QPS ///////////////
    memcpy(bin, globalTramaVenta.track2, 9);

    if (tablaTres.additionalPromptsSelect.additionalPrompt6 == 1 &&
        atoi(tipoTransaccion) != TRANSACCION_BIG_BANCOLOMBIA
        && atoi(tipoTransaccion) != TRANSACCION_DCC) {
        globalTramaVenta.isQPS[0] = '1';
        LOGI("is qps :( %s",  globalTramaVenta.isQPS);
        _convertASCIIToBCD_(field61, (char *)"10000000000000000000000000", 13);
        setField(61, field61, 13);
    }

    generarInvoice(recibo);
    memcpy(globalTramaVenta.numeroRecibo, recibo, strlen(recibo));
    setField(62, recibo, 6); //numero recibo



    ////// ENVIAR TEXTO ADICIONAL CAMPO 63 ////////
    lengthTextoAdicional = strlen(globalTramaVenta.textoAdicional);

    if (lengthTextoAdicional > 0 && atoi(tipoTransaccion) != TRANSACCION_BIG_BANCOLOMBIA) {
        setField(63, globalTramaVenta.textoAdicional, lengthTextoAdicional);
    }

    if (strncmp(globalTramaVenta.isQPS, "2", 1) == 0 || strncmp(globalTramaVenta.isQPS, "3", 1) == 0
        || strncmp(globalTramaVenta.aux1 + 49, "2", 1) == 0 ||
        strncmp(globalTramaVenta.aux1 + 49, "3", 1) == 0) {
        if (strcmp(productoPPRO, "0") == 0) {
            //getBufferDcc(tokenDC);
        } else {
            //getBufferDcc(tokenDP);
        }
    }

    if (strncmp(tokenDC + 2, "DC", 2) == 0) {
        memset(globalTramaVenta.estadoTarifa, 0x00, TAM_ESTADO_TARIFA + 1);
        memcpy(globalTramaVenta.estadoTarifa, "DC", TAM_ESTADO_TARIFA);
        memcpy(globalTramaVenta.tokenVivamos, tokenDC, 44);
        setField(47, tokenDC, 44);
        setField(60, tokenDC, 44);
    } else if (strncmp(tokenDP + 2, "DP", 2) == 0) {
        memset(globalTramaVenta.estadoTarifa, 0x00, TAM_ESTADO_TARIFA + 1);
        memcpy(globalTramaVenta.estadoTarifa, "DP", TAM_ESTADO_TARIFA);
        memcpy(globalTramaVenta.tokenVivamos, tokenDP, 39);
        setField(47, tokenDP, 39);
        setField(60, tokenDP, 39);
    } else if (strcmp(globalTramaVenta.estadoTarifa, "DT") == 0) {
        setField(60, globalTramaVenta.tokenVivamos, 19);
    } else {
        LOGI("bufferCorrrimiento antes %s ", bufferCorrrimiento);
        memcpy(bufferCorrrimiento, globalTramaVenta.cuotas, 2);
        memcpy(bufferCorrrimiento + 2, globalCVC2, 3);
        memcpy(bufferCorrrimiento + 5, globalDocumento, strlen(globalDocumento));
        LOGI("globalDocumento %s ", globalDocumento);
        LOGI("bufferCorrrimiento %s ", bufferCorrrimiento);
        setField(42, bufferCorrrimiento, 15);
        memcpy(globalTramaVenta.fiel42, bufferCorrrimiento, strlen(bufferCorrrimiento));
    }
}

static void cargarCampos_4_48_57(char *ventaT) {

    unsigned long venta = 0;
    unsigned long iva = 0;
    unsigned long propina = 0;
    unsigned long base = 0;
    unsigned long ventaTotal = 0;

    uChar auxVentaTotal[12 + 1] = {0x00};
    uChar auxPropina[12 + 1] = {0x00};
    uChar variableAux[36 + 1] = {0x00};
    uChar bufferAuxiliar[36 + 1] = {0x00};
    uChar tipoTransaccion[2 + 1] = {0x00};

    TablaUnoInicializacion tablaUno;

    memset(&tablaUno, 0x00, sizeof(tablaUno));

    venta = atol(globalTramaVenta.compraNeta);
    iva = atol(globalTramaVenta.iva);

    propina = atol(globalTramaVenta.propina);
    base = atol(globalTramaVenta.baseDevolucion);

    ventaTotal = atol(globalTramaVenta.totalVenta) + propina; //venta+iva+propina;
    LOGI("B.suma o no suma  %s ", globalTramaVenta.totalVenta);
    tablaUno = _desempaquetarTablaCeroUno_();

    sprintf(globalTramaVenta.baseDevolucion, "%ld", base);
    sprintf(globalTramaVenta.compraNeta, "%ld", venta);

    strcat(globalTramaVenta.compraNeta, "00");

    if (tablaUno.options1.tipProcessing == 1) {

        if (strlen(globalTramaVenta.propina) > 0) {

            strcat(globalTramaVenta.propina, "00");
            sprintf(auxPropina, "%010ld", propina);

            strcat(auxPropina, "00");
            LOGI("B.propina %s ", globalTramaVenta.propina);
        } else {
            memset(auxPropina, 0x30, sizeof(auxPropina));
        }

        if (atoi(tipoTransaccion) != TRANSACCION_BIG_BANCOLOMBIA) {
            setField(57, auxPropina, 12);
        } else {
            setField(54, auxPropina, 12);
        }

    }
    LOGI("X.totalVenta %s ", globalTramaVenta.totalVenta);

    sprintf(globalTramaVenta.totalVenta, "%ld", ventaTotal);
    strcat(globalTramaVenta.totalVenta, "00");
    ///cambio para cajas
    memset(globalTramaVenta.iva, 0x00, sizeof(globalTramaVenta.iva));
    sprintf(globalTramaVenta.iva, "%ld", iva);
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

    _convertASCIIToBCD_(bufferAuxiliar, variableAux, 36);

    memset(auxVentaTotal, 0x00, sizeof(auxVentaTotal));
    sprintf(auxVentaTotal, "%010ld", ventaTotal);
    LOGI("crash B%s ", auxVentaTotal);

    strcat(auxVentaTotal, "00");
    LOGI("crash C%s ", auxVentaTotal);

    setField(4, auxVentaTotal, 12);
    setField(48, bufferAuxiliar, 18);

    strcpy(globalTramaVenta.field57, auxPropina);
}

static void
cargarCampos_11_22_35_24_25_41_52_53_55() {

    char bufferNII[6 + 1] = {0x00};
    char systemTrace[6 + 1] = {0x00};
    char numeroTerminal[8 + 1] = {0x00};

    generarSystemTraceNumber(systemTrace, (char *)"");
    getParameter(NUMERO_TERMINAL, numeroTerminal);
    getParameter(NII, bufferNII);
    memset(globalTramaVenta.systemTrace, 0x00, sizeof(globalTramaVenta.systemTrace));

    strcpy(globalTramaVenta.systemTrace, systemTrace);
    setField(11, systemTrace, 6);

    cargarCampos_22_35();

    memcpy(globalTramaVenta.nii, bufferNII + 1, 3);
    setField(24, bufferNII + 1, 3);

    strcpy(globalTramaVenta.posConditionCode, "00");
    setField(25, (char *)"00", 2);

    strcpy(globalTramaVenta.terminalId, numeroTerminal);
    setField(41, numeroTerminal, 8);

    if (globalPinOffLine == TRUE) {
        setField(52, globalPinBlokP52, LEN_PIN_DATA);
        setField(53, (char *) "9801100100000000", 16);
    }

    setField(55, globalpaqueteP55, globalsizeP55);
}

static void cargarCampos_22_35() {

    setField(22, (char *)"051", 3);
    setField(35, globalTramaVenta.track2, strlen(globalTramaVenta.track2));
    strncpy(globalTramaVenta.posEntrymode, "051", 3);
}

int comportamientoTarjeta(DatosTarjetaAndroid datosTarjetaAndroid) {

    unsigned short encontro = 0;
    uChar label[20];
    uChar buffer[50];
    uChar AID[22 + 1];
    uChar auxiarAID[22 + 1];
    int tipoTarjeta = TRANSACCION_CHIP_POR_DEFECTO;
//	int comparando = 0;
    int n = 0;
    unsigned short longitudAuxiliar = 0;

    char dMasterMayuscula[] = {"DEBIT MAST"};
    char dMasterMinuscula[] = {"Debit Mast"};

    char dMasterMinuscula1[] = {"MasterDebito"};
    char dMasterMayuscula2[] = {"MASTERDEBITO"};
    char dMasterMayuscula3[] = {"Masterdebito"};
    char dMasterMinuscula4[] = {"MasterDebit"};
    char dMasterMayuscula5[] = {"MASTERDEBIT"};
    char dMasterMinuscula6[] = {"masterdebit"};
    char dMasterMayuscula7[] = {"DEBIT MASTER"};
    char dMasterMayuscula8[] = {"DEBIT MASTERCARD"};
    char dMasterMayuscula9[] = {"DEBITO MASTER"};
    char dMasterMayuscula10[] = {"DEBITO MASTERCARD"};

    char visaDebitoMinuscula[] = {"Visa Debit"};
    char visaDebitoMayuscula[] = {"VISA DEBIT"};
    char visaDebitMinuscula[] = {"Visa Debito"};
    char visaDebitMayuscula[] = {"VISA DEBITO"};
    char visaMinuscula[] = {"visa debito"};
    char visaMayuscula[] = {"Visa Debit"};

    char visaCreditoMinuscula[] = {"Visa Credito"};
    char visaCreditoMayuscula[] = {"VISA CREDITO"};
    char visaCreditMinuscula[] = {"visa credito"};
    char visaCreditMayuscula[] = {"Visa Credit"};
    char visaCredMinuscula[] = {"visa credit"};
    char visaCredMayuscula[] = {"VISA CREDIT"};

    char visaPrepagoMinuscula[] = {"Visa Prepago"};
    char visaPrepagoMayuscula[] = {"VISA PREPAGO"};
    char visaPrepagMinuscula[] = {"visa prepago"};
    char visaPrepagMayuscula[] = {"Visa Prepaid"};
    char visaPrepMinuscula[] = {"VISA PREPAID"};
    char visaPrepMayuscula[] = {"visa prepaid"};

	unsigned char idDiners[] = { 0xA0, 0x00, 0x00, 0x01, 0x52, 0x30, 0x10 };
//	unsigned char idMaestro[] = { 0x00, 0x00, 0x00, 0x00, 0x04, 0x30, 0x60 };
    TablaTresInicializacion tablaTres;
    char bin[9 + 1];
    int tamTrack2 = 0;
    int existeEmisor = 0;
    EmisorAdicional emisorAdicional;

    memset(&tablaTres, 0x00, sizeof(tablaTres));
    memset(bin, 0x00, sizeof(bin));
    memset(AID, 0x00, sizeof(AID));
    memset(buffer, 0x00, sizeof(buffer));
    memset(auxiarAID, 0x00, sizeof(auxiarAID));
    memset(label, 0x00, sizeof(label));

    tablaTres = _traerIssuer_(atol(bin));

    LOGI("AID idDiners %s", idDiners);
    LOGI("Arreglo AIDs %X%X%X",idDiners[0],idDiners[1],idDiners[2]);
    if (strlen(datosTarjetaAndroid.tarjetaHabiente) > 1) {
        memcpy(globalTramaVenta.tarjetaHabiente, datosTarjetaAndroid.tarjetaHabiente,
               strlen(datosTarjetaAndroid.tarjetaHabiente));
    }

    // Extraccion del Track 2

    memcpy(globalTramaVenta.track2, datosTarjetaAndroid.track2, strlen(datosTarjetaAndroid.track2));


    //memcpy(AID, datosTarjetaAndroid.aid, strlen(datosTarjetaAndroid.aid));
    _convertASCIIToBCD_(AID + 1,datosTarjetaAndroid.aid,strlen(datosTarjetaAndroid.aid));
    LOGI("AID convertido %s", AID);
    LOGI("Arreglo AIDs %X",AID[5]);
    memcpy(globalTramaVenta.AID, datosTarjetaAndroid.aid, strlen(datosTarjetaAndroid.aid));


    //memcpy(label, datosTarjetaAndroid.apLabel, strlen(datosTarjetaAndroid.apLabel));
    _convertASCIIToBCD_(label, datosTarjetaAndroid.apLabel, strlen(datosTarjetaAndroid.apLabel));
    LOGI("label convertido %s", label);
    if ((AID[1] == 0xA0) && (AID[2] == 0x00) && (AID[3] == 0x00) && (AID[4] == 0x01) &&
        (AID[5] == 0x52)
        && (AID[6] == 0x30) && (AID[7] == 0x10)) {
        tipoTarjeta = 2; //Dinners credito
    }

    if ((AID[1] == 0xA0) && (AID[2] == 0x00) && (AID[3] == 0x00) && (AID[4] == 0x04) &&
        (AID[5] == 0x66)
        && (AID[6] == 0x10) && (AID[7] == 0x10)) {
        tipoTarjeta = 2; //La polar portarse como credito
    }
    if ((AID[1] == 0xA0) && (AID[2] == 0x00) && (AID[3] == 0x00) && (AID[4] == 0x00) &&
        (AID[5] == 0x25)
        && (AID[6] == 0x01) && (AID[7] == 0x04)) {
        tipoTarjeta = 2; //AMEX portarse como credito
    }

//	comparando = memcmp(AID + 4, idMaestro + 4, 3);

    if ((AID[5] == 0x04) && (AID[6] == 0x30) && (AID[7] == 0x60)) { //maestro
        LOGI("Es Maestro");
        tipoTarjeta = 1;
    } else {
        if ((AID[5] == 0x04) && (AID[6] == 0x10) && (AID[7] == 0x10)) { //Mastercard
            tipoTarjeta = 2;
            if (strncmp(dMasterMayuscula, label, 10) == 0 ||
                strncmp(dMasterMinuscula, label, 10) == 0) {
                tipoTarjeta = 1;
            }
        }
    }

    if ((AID[5] == 0x03) && (AID[6] == 0x20) && (AID[7] == 0x10)) { //Visa Electron
        tipoTarjeta = 1;
        LOGI("Es visa electron ");
    } else if ((AID[5] == 0x03) && (AID[6] == 0x10) && (AID[7] == 0x10)) {
        if (strncmp(visaDebitoMayuscula, label, 10) == 0 ||
            strncmp(visaDebitoMinuscula, label, 10) == 0
            || strncmp(visaDebitMinuscula, label, 10) == 0 ||
            strncmp(visaDebitMayuscula, label, 10) == 0
            || strncmp(visaMinuscula, label, 10) == 0 || strncmp(visaMayuscula, label, 10) == 0
            || strncmp(visaPrepagoMinuscula, label, 10) == 0 ||
            strncmp(visaPrepagoMayuscula, label, 10) == 0
            || strncmp(visaPrepagMinuscula, label, 12) == 0 ||
            strncmp(visaPrepagMayuscula, label, 12) == 0
            || strncmp(visaPrepMinuscula, label, 12) == 0 ||
            strncmp(visaPrepMayuscula, label, 12) == 0) {
            tipoTarjeta = 1;
            LOGI("Es visa credito ");
        } else if (strncmp(visaCreditoMinuscula, label, 12) == 0 ||
                   strncmp(visaCreditoMayuscula, label, 12) == 0
                   || strncmp(visaCreditMinuscula, label, 12) == 0 ||
                   strncmp(visaCreditMayuscula, label, 11) == 0
                   || strncmp(visaCredMinuscula, label, 11) == 0 ||
                   strncmp(visaCredMayuscula, label, 11) == 0) {
            tipoTarjeta = 2;
        } else {
            tipoTarjeta = 5;
        }
    }

    if ((AID[1] == 0xAA) && (AID[2] == 0xA0) && (AID[3] == 0x00) && (AID[4] == 0x00) &&
        (AID[5] == 0x01)
        && (AID[6] == 0x00) && (AID[7] == 0x01)) { // Db //Privada Debito
        tipoTarjeta = 1;
    }

    if ((AID[1] == 0xAA) && (AID[2] == 0xA0) && (AID[3] == 0x00) && (AID[4] == 0x00) &&
        (AID[5] == 0x01)
        && (AID[6] == 0x00) && (AID[7] == 0x02)) { // Cr //Privada Credito
        tipoTarjeta = 2;
    }

    if ((AID[1] == 0xA0) && (AID[2] == 0x00) && (AID[3] == 0x00) && (AID[4] == 0x00) &&
        (AID[5] == 0x03)) { //Visa
        //tarjeta_visa = 1;
    }

    memset(globalTramaVenta.appLabel, 0x00, sizeof(globalTramaVenta.appLabel));

    if (strlen(label) > 15) {
        memcpy(globalTramaVenta.appLabel, label, TAM_CARD_NAME);
    } else {
        strcpy(globalTramaVenta.appLabel, label);
        LOGI("globalTramaVenta.appLabel %s", globalTramaVenta.appLabel);
    }

    if (strncmp(visaDebitoMayuscula, label, 10) == 0 || strncmp(visaDebitoMinuscula, label, 10) == 0
        || strncmp(visaDebitMinuscula, label, 10) == 0 ||
        strncmp(visaDebitMayuscula, label, 10) == 0
        || strncmp(visaMinuscula, label, 10) == 0 || strncmp(visaMayuscula, label, 10) == 0
        || strncmp(visaPrepagoMinuscula, label, 10) == 0 ||
        strncmp(visaPrepagoMayuscula, label, 10) == 0
        || strncmp(visaPrepagMinuscula, label, 12) == 0 ||
        strncmp(visaPrepagMayuscula, label, 12) == 0
        || strncmp(visaPrepMinuscula, label, 12) == 0 ||
        strncmp(visaPrepMayuscula, label, 12) == 0) {
        strcpy(label, "VISA DEBIT");
    } else if (strncmp(visaCreditoMinuscula, label, 12) == 0 ||
               strncmp(visaCreditoMayuscula, label, 12) == 0
               || strncmp(visaCreditMinuscula, label, 12) == 0 ||
               strncmp(visaCreditMayuscula, label, 11) == 0
               || strncmp(visaCredMinuscula, label, 11) == 0 ||
               strncmp(visaCredMayuscula, label, 11) == 0) {
        strcpy(label, "VISA");
    } else if (strncmp(label, "VISA ELEC", 9) == 0) {
        memset(label, 0x00, sizeof(label));
        strcpy(label, "V.ELECTRON");
    } else if (strncmp(dMasterMinuscula1, label, 12) == 0 ||
               strncmp(dMasterMayuscula2, label, 12) == 0
               || strncmp(dMasterMayuscula3, label, 12) == 0 ||
               strncmp(dMasterMinuscula4, label, 11) == 0
               || strncmp(dMasterMayuscula5, label, 11) == 0 ||
               strncmp(dMasterMinuscula6, label, 11) == 0
               || strncmp(dMasterMayuscula7, label, 12) == 0 ||
               strncmp(dMasterMayuscula8, label, 16) == 0
               || strncmp(dMasterMayuscula9, label, 13) == 0 ||
               strncmp(dMasterMayuscula10, label, 17) == 0) {
        strcpy(label, "MASTERDEBIT");
    } else {
        memcpy(bin, globalTramaVenta.track2, 9);
        memset(label, 0x00, sizeof(label));
        strcpy(label, tablaTres.cardName);
    }

    strncpy(globalTramaVenta.cardName, label, 11);
    LOGI("globalTramaVenta.cardName %s", globalTramaVenta.cardName);
    memset(bin, 0x00, sizeof(bin));
    memcpy(bin, globalTramaVenta.track2, 9);
    existeEmisor = buscarBinEmisor(bin);

    if (existeEmisor > 0) {
        emisorAdicional = traerEmisorAdicional(existeEmisor);
        if (emisorAdicional.existeIssuerEmisor == 1) {

            memset(globalTramaVenta.cardName, 0x00, sizeof(globalTramaVenta.cardName));
            memcpy(globalTramaVenta.cardName, emisorAdicional.nombreEmisor,
                   strlen(emisorAdicional.nombreEmisor));
            LOGI("existe emisos adicional %s", globalTramaVenta.cardName);
        }
    }

    if (strncmp(globalTramaVenta.cardName, "VISA DEBIT", 10) != 0) {
        memcpy(globalTramaVenta.codigoGrupo, tablaTres.reservedForFutureUse, TAM_GRUPO);
        LOGI("globalTramaVenta.codigoGrupo %s", globalTramaVenta.codigoGrupo);
    } else {
        memcpy(globalTramaVenta.codigoGrupo, "06", TAM_GRUPO);
    }

    memcpy(globalTramaVenta.codigoSubgrupo, tablaTres.reservedForFutureUse2, TAM_SUBGRUPO);

    return tipoTarjeta;
}

int solicitudTipoCuenta(DatosTarjetaAndroid *datosTarjetaAndroid) {
    int acepta = 0;
    int tipoTarjeta = 0;

    tipoTarjeta = comportamientoTarjeta(*datosTarjetaAndroid);
    LOGI("tipoTarjeta %d", tipoTarjeta);
    acepta = solicitudCuenta(datosTarjetaAndroid,tipoTarjeta);
    return acepta;
}

int solicitudCuenta(DatosTarjetaAndroid *datosTarjetaAndroid, int tipoTarjetaAID) {

    int decision = 0;
    long busquedaBin = 0;
    int cProcesoTipoCuenta = 0;
    char tipoTransaccionAux[2 + 1] = {0x00};
    char bin[20 + 1] = {0x00};
    char binCafam[6 + 1] = {0x00};
    TablaTresInicializacion dataIssuer;

    TablaSeisInicializacion textoAdicional;
    ConfiguracionExcepciones configuracionExcepciones;
    ConveniosBigBancolombia datosBig;

    memset(&dataIssuer, 0x00, sizeof(dataIssuer));
    memset(&datosBig, 0x00, sizeof(datosBig));
    memset(&configuracionExcepciones, 0x00, sizeof(configuracionExcepciones));

    memcpy(bin, datosTarjetaAndroid->pan, 9);

    getParameter(TIPO_TRANSACCION, tipoTransaccionAux);

    busquedaBin = atol(bin);
    dataIssuer = _traerIssuer_(busquedaBin);

    if (strncmp(globalTramaVenta.cardName, "VISA DEBIT", 10) != 0) {
        memcpy(datosTarjetaAndroid->grupo, dataIssuer.reservedForFutureUse, TAM_GRUPO);
    } else {
        memcpy(datosTarjetaAndroid->grupo, "06", TAM_GRUPO);
    }

    memcpy(datosTarjetaAndroid->subGrupo, dataIssuer.reservedForFutureUse2, TAM_SUBGRUPO);
    LOGI(" subGrupo %s ", datosTarjetaAndroid->subGrupo);
    //////////// VERIFICAR LA CONFIGURACION DE LAS EXCEPCIONES DE EMV //////////////////
    configuracionExcepciones = excepcionesEmv(busquedaBin);

    if (configuracionExcepciones.comportamientoExcepciones1.seleccionCuentaDefault) {
        tipoTarjetaAID = 5; // Tipo De Cuenta Default. Ahorros,Corriente,Credito.
        LOGI("tipoTarjetaAID es 5 por excepciones  ");
    }

    memcpy(binCafam, datosTarjetaAndroid->pan, 6);

    if (configuracionExcepciones.comportamientoExcepciones1.seleccionCuentaMultiservicios
        || strncmp(binCafam, "606125", 6) == 0) {
        tipoTarjetaAID = MULTI_BOLSILLOS; // MultiBolsillos.
        LOGI("tipoTarjetaAID es multibolsillo  ");
    }
    //////////// VERIFICAR LA CONFIGURACION DE LAS EXCEPCIONES DE EMV //////////////////

    if (configuracionExcepciones.comportamientoExcepciones1.bit4) {
        tipoTarjetaAID = TRANSACCION_CHIP_POR_DEFECTO;
        LOGI("tipoTarjetaAID es 6 por excepciones");
    }

    if (configuracionExcepciones.comportamientoExcepciones1.bit7 && tipoTarjetaAID != 5) { //foco
        tipoTarjetaAID = 0;
        LOGI("tipoTarjetaAID es Focosodexo ");
        memcpy(globalTramaVenta.tipoCuenta, "30", sizeof(globalTramaVenta.tipoCuenta));
    }

    decision = comportamientotiposdeCuenta(tipoTarjetaAID, &cProcesoTipoCuenta,
                                           configuracionExcepciones.comportamientoExcepciones1.bit4,
                                           binCafam);

    datosTarjetaAndroid->asklastfour = dataIssuer.options4.verifyLastFourPANDigits;
    datosTarjetaAndroid->askDocument = dataIssuer.additionalPromptsSelect.additionalPrompt1;
    datosTarjetaAndroid->askCuotes = dataIssuer.options3.printNoRefundsAllowed;
    datosTarjetaAndroid->askCvc2 = dataIssuer.additionalPromptsSelect.additionalPrompt2;

    ///////// PERMITE TEXTO ADICIONAL ? ///////////
    textoAdicional = desempaquetarTablaSeis(0x00);
    if (textoAdicional.consecutivoRegistro == '1') {
        datosTarjetaAndroid->askAdicionalText = 1;
    }
    return decision;
}

int comportamientotiposdeCuenta(int cuenta, int *codigoTipoCuenta, int solicitaCRotativo,
                                char *binCafam) {
    int cProcesoTipoCuenta = 0;
    int respuesta = 0;
    char tipoTransaccionAuxiliar[2 + 1];

    memset(tipoTransaccionAuxiliar, 0x00, sizeof(tipoTransaccionAuxiliar));
    getParameter(TIPO_TRANSACCION_AUX, tipoTransaccionAuxiliar);
    LOGI("solicitaCRotativo %d", solicitaCRotativo);
    if (atoi(tipoTransaccionAuxiliar) == TRANSACCION_SALDO) {
        solicitaCRotativo = 0;
    }
    LOGI("cuenta %d", cuenta);
    switch (cuenta) {
        case 1: //debito EMV
            respuesta = solicitudTipoCuentaDebitoEMV(&cProcesoTipoCuenta, solicitaCRotativo);
            break;
        case 2: //credito
            respuesta = solicitudTipoCuentaCreditoEMV(&cProcesoTipoCuenta, solicitaCRotativo);
            break;
        case 3: //
            respuesta = 1;
            cProcesoTipoCuenta = 0;
            break;
        case MULTI_BOLSILLOS:
            respuesta = menuMultiservicios(binCafam);
            if (respuesta > 0) {
                cProcesoTipoCuenta = respuesta;
            }
            break;
        case 5:
            respuesta = menuTipoDeCuenta();
            cProcesoTipoCuenta = (respuesta > 0) ? respuesta : FALSE;
            break;
        case TRANSACCION_CHIP_POR_DEFECTO:
            LOGI("cProcesoTipoCuenta %d", cProcesoTipoCuenta);
            LOGI("solicitaCRotativo %d", solicitaCRotativo);
            respuesta = solicitudTipoCuentaNuevosAid(&cProcesoTipoCuenta, solicitaCRotativo);

            break;

        default:
            cProcesoTipoCuenta = 0;
            respuesta = 1;
            break;
    }
    *codigoTipoCuenta = cProcesoTipoCuenta;
    return respuesta;
}

int solicitudTipoCuentaDebitoEMV(int *outCuenta, int solicitaCRotativo) {

    int decision = 0;
    int nItem = 2;
    int creditoRotativo = 0;
    char transaccion[2 + 1];
    char items[100 + 1] = {0x00};

    memset(transaccion, 0x00, sizeof(transaccion));

    creditoRotativo = solicitaCRotativo;

    getParameter(TIPO_TRANSACCION, transaccion);

    if (atoi(transaccion) == TRANSACCION_SALDO) {
        creditoRotativo = 0;
    }

    if (creditoRotativo == 1) {
        nItem = 3;
    }

    strcpy(items, ";Ahorro;10;");
    strcat(items, "Corriente;20;");
    strcat(items, "Cr.Rotativo;40;");
    showMenu((char *)"TIPO DE CUENTA;", nItem, items, 100);


    return decision;
}

int solicitudTipoCuentaCreditoEMV(int *outCuenta, int solicitaCRotativo) {

    int superCupo = 0;
    int creditoRotativo = 0;
    int tipoTransaccion = 0;
    int decision = 0;
    long binIssuer = 0;
    char transaccionActual[2 + 1];
    char items[100 + 1] = {0x00};
    uChar variableAux[10 + 1];
    TablaTresInicializacion dataIssuer;

    memset(transaccionActual, 0x00, sizeof(transaccionActual));
    memset(variableAux, 0x00, sizeof(variableAux));
    memset(&dataIssuer, 0x00, sizeof(dataIssuer));

    getParameter(TIPO_TRANSACCION, transaccionActual);

    tipoTransaccion = atoi(transaccionActual);

    creditoRotativo = solicitaCRotativo;

    if (tipoTransaccion == TRANSACCION_SALDO) {
        creditoRotativo = 0;
    }

    memcpy(variableAux, globalTramaVenta.track2, 9);
    binIssuer = atol(variableAux);
    dataIssuer = _traerIssuer_(binIssuer);

    if (dataIssuer.additionalPromptsSelect.additionalPrompt8 == 1) {
        superCupo = 1;
    }

    if (superCupo == 1
        && (tipoTransaccion == TRANSACCION_VENTA || tipoTransaccion == TRANSACCION_CUENTAS_DIVIDIDAS
            || tipoTransaccion == TRANSACCION_PAYPASS)) {
        strcpy(items, ";Credito;30;");
        strcat(items, "CMR Extendido;40 ");
        showMenu((char *)"TIPO DE CUENTA;", 2, items, 100);

    } else if (creditoRotativo == 1) {
        strcpy(items, ";Credito;30;");
        strcat(items, "Cr. Rotativo;40;");
        showMenu((char *)"TIPO DE CUENTA;", 2, items, 100);
    }

    return decision;
}

int menuTipoDeCuenta(void) {

    int tipoCuenta = 0;
    char items[100 + 1] = {0x00};

    strcpy(items, ";Ahorro;10;");
    strcat(items, "Corriente;20;");
    strcat(items, "Credito;30;");
    showMenu((char *)"TIPO DE CUENTA;", 3, items,100);

    return tipoCuenta;
}
int solicitudTipoCuentaNuevosAid(int *outCuenta, int solicitaCRotativo) {

    int decision = 0;
    int item = 0;
    int creditoRotativo = 0;

    creditoRotativo = solicitaCRotativo;

    if (creditoRotativo == 1) {
        item = 4;

    } else {
        item = 3;
    }
    char items[100 + 1] = {0x00};

    strcpy(items, ";Ahorro;10;");
    strcat(items, "Corriente;20;");
    strcat(items, "Credito;30;");
    strcat(items, "Cr. Rotativo;40;");
    showMenu((char *)"TIPO DE CUENTA;",item, items, 100);

    return decision;
}