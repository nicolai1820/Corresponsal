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

#define TAM_MARGEN 9
#define TAM_BUFFER 256
#define MAXIMOS_CAMPOS 23
#define TAM_MENSAJE_PURE 2

typedef struct {
    int ID;
    int longitud;
    int tipoDato;
} DataElementDescripcion;

DataElementDescripcion mensajePureCommerce[] =
        {
                {0 , 14, 0},
                {1 , 4 , 0},
                {2 , 37, 1}, //variable hasta 35
                {3 , 16, 0},
                {4 , 13, 1}, //variable hasta 11
                //{5 , 16, 0},
                {6 , 16, 0},
                //{7 , 22, 1}, //variable hasta 20
                {9 , 10, 0},
                {10, 101,1}, //variable hasta 99
                {15, 3 , 0},
                {16, 14, 0}, //mandatorio si disponible
                {17, 2 , 0},
                {18, 11, 0},
                {20, 37, 1}, //variable hasta 35
                {21, 16, 0},
                {22, 6 , 0},
                {24, 3 , 0},
                {25, 2 , 0},
                {26, 9 , 0},
                {27, 9 , 0}, //mandatorio si disponible
                {28, 16, 0}, //mandatorio si disponible
                {29, 3 , 0}
        };
int _verificarBinesDCC_(uChar * bin) {

    int iRet = 0;
    int salidaWhile = 0;
    int posicion = 0;
    int lengthDataBin = 20;
    int iRetSearch = 0;
    uChar dataBines[20 + 1];
    uChar rangoBajo[10 + 1];
    uChar rangoAlto[10 + 1];

    iRet = verificarArchivo(discoNetcom, BINES_DCC_BANDA);
    iRet = (iRet != FS_OK) ? FALSE : TRUE;

    if (iRet > 0) {
        do {
            memset(dataBines, 0x00, sizeof(dataBines));
            memset(rangoBajo, 0x00, sizeof(rangoBajo));
            memset(rangoAlto, 0x00, sizeof(rangoAlto));
            salidaWhile = buscarArchivo(discoNetcom, (char *)BINES_DCC_BANDA, dataBines, posicion, lengthDataBin);

            memcpy(rangoBajo, dataBines, 10);
            memcpy(rangoAlto, dataBines + 10, 10);

            iRetSearch = buscarBin(rangoBajo, rangoAlto, bin);

            if (iRetSearch == 1) {
                iRet = IS_TARJETA_DCC;
                break;
            }
            posicion += (lengthDataBin + 1);
        } while (salidaWhile == 0);
    }

    if (iRet != IS_TARJETA_DCC) {
        iRet = 1;
    }

    return iRet;
}

/****************************************************************************
 * @brief Verificar si el Bin esta entre los rangos presentes en la         *
 * 		  Tabla de bines Numero 2.											*
 * @param rangoBajo Rango bajo del bin.										*
 * @param rangoAlto Rango alto del bin.										*
 * @param bin Bin a verificar.											    *
 * @param return 1: si esta presente. 0 : si no esta presente.           	*
 * @author 														*
 ****************************************************************************/

int buscarBin(uChar *rangoBajo, uChar *rangoAlto, uChar * bin) {

    int resultado = -1;
    long binAux = 0;
    rangoBajo[9] = 0x00;
    rangoAlto[9] = 0x00;

    binAux = atol(bin);

    if (binAux >= atol(rangoBajo) && binAux <= atol(rangoAlto)) {
        resultado = 1;
    }

    return resultado;
}

/****************************************************************************
 * @brief Realizar la consulta DCC.											*
 * @param return 1: si esta presente. 0 : si no esta presente.           	*
 * @author 														*
 ****************************************************************************/
int consultaDCC(DatosVenta * tramaVenta, uChar * codigoMoneda) {

    int iRet = 0;
    int indiceReverso = 0;
    ResultISOPack resultISOPack;
    uChar inputData[512 + 1];
    uChar tokenDcc[50 + 1];

    memset(&resultISOPack, 0x00, sizeof(resultISOPack));
    memset(inputData, 0x00, sizeof(inputData));
    memset(tokenDcc, 0x00, sizeof(tokenDcc));

    resultISOPack = empaquetarConsultaDCC(*tramaVenta, codigoMoneda);

    if (resultISOPack.totalBytes > 0) {
        iRet = realizarTransaccion(resultISOPack.isoPackMessage, resultISOPack.totalBytes, inputData,
                                   TRANSACCION_DEFAULT, CANAL_DEMANDA, REVERSO_NO_GENERADO);
    }

    if ((iRet == -1) && (verificarArchivo(discoNetcom, ARCHIVO_REVERSO) == FS_OK)) {
        indiceReverso = 1;
    }

    if (iRet > 0) {
        iRet = procesarRespuestaConsultaDCC(inputData, iRet, tokenDcc, tramaVenta);
    }

    ///// CONTINUAR CON LA TRANSACCION SI NO HAY RESPUESTA DCC /////
    if (iRet != IS_TRANSACCION_DCC) {
        iRet = 1;
    } else {
        memcpy(tramaVenta->tokenVivamos, tokenDcc, 48);
        memcpy(tramaVenta->estadoTarifa, "DC", 2);
    }

    if (indiceReverso > 0) {
        iRet = -1;
    }

    return iRet;
}

/****************************************************************************
 * @brief Empaquetar la trama de consulta DCC.								*
 * @param return > 0: si esta OK. < 0 si hay un error.              	    *
 * @author 														*
 ****************************************************************************/

ResultISOPack empaquetarConsultaDCC(DatosVenta tramaVenta, uChar * codMoneda) {

    ResultISOPack resultISOPack;
    ISOHeader isoHeader8583;
    uChar niiDcc[TAM_NII + 1];
    uChar dateAndTime[10 + 1];
    uChar numeroTerminal[8 + 1];
    uChar codigoProceso[6 + 1];
    uChar codigoMoneda[3 + 1];
    uChar monto[12 + 1];
    uChar rrn[12 + 1];
    uChar systemTraceNumber[6 + 1];
    uChar field61[256 + 1];

    memset(&resultISOPack, 0x00, sizeof(resultISOPack));
    memset(&isoHeader8583, 0x00, sizeof(isoHeader8583));
    memset(dateAndTime, 0x00, sizeof(dateAndTime));
    memset(systemTraceNumber, 0x00, sizeof(systemTraceNumber));
    memset(field61, 0x00, sizeof(field61));
    memset(numeroTerminal, 0x00, sizeof(numeroTerminal));
    memset(rrn, 0x00, sizeof(rrn));
    memset(codigoProceso, 0x00, sizeof(codigoProceso));
    memset(codigoMoneda, 0x00, sizeof(codigoMoneda));

    getParameter(NII_DCC, niiDcc);
    getParameter(NUMERO_TERMINAL, numeroTerminal);
    generarSystemTraceNumber(systemTraceNumber,(char *)"");

    //// RRN ////
    memset(rrn, '0', 6);
    memcpy(rrn + 6, systemTraceNumber, 6);

    //// POS ENTRY MODE /////
    if (strlen(tramaVenta.posEntrymode) == 0) {
        strcpy(tramaVenta.posEntrymode, "051");
    }

    //// CODIGO MONEDA ////
    if ((strcmp(tramaVenta.posEntrymode, "051") != 0)) {
        memset(codigoMoneda, 0x00, sizeof(codigoMoneda));
        strcpy(codigoMoneda, "000");
    } else {
        memcpy(codigoMoneda, codMoneda, 3);
    }

    //// MONTO ////
    memcpy(monto, tramaVenta.totalVenta, 10);
    strcat(monto, "00");

    //// CODIGO DE PROCESO /////
    sprintf(codigoProceso, "00%.2s00", tramaVenta.tipoCuenta);

    /// HEADER ///
    isoHeader8583.TPDU = 60;
    memcpy(isoHeader8583.destination, niiDcc, TAM_NII + 1);
    memcpy(isoHeader8583.source, niiDcc, TAM_NII + 1);
    setHeader(isoHeader8583);
    /// MTI ///
    setMTI((uChar *)"0100");

    getDateAndTime(dateAndTime);

    /// LLENAR CAMPOS ///
    setField(3, codigoProceso, 6); // Codigo De Proceso de la Venta
    setField(4, monto, 12); // Monto
    setField(7, dateAndTime, 10);
    setField(11, systemTraceNumber, 6);
    setField(22, tramaVenta.posEntrymode, 3); // Pos Entry Mode
    setField(24, niiDcc + 1, TAM_NII);
    setField(25, (uChar *)"00", 2);
    setField(35, tramaVenta.track2, strlen(tramaVenta.track2));
    setField(37, rrn, 12); // Se envia el System Trace Number
    setField(41, numeroTerminal, 8);
    setField(49, codigoMoneda, 3); //Codigo de la Moneda del Pais.
    setField(60, (uChar *)"890", 3);

    resultISOPack = packISOMessage();

    return resultISOPack;

}

/****************************************************************************
 * @brief Obtener informacion de la fecha y la hora en formato MMDDhhmmss	*
 * @param dateAndTime Variable que guarda la informacion.					*
 * @author 														*
 ****************************************************************************/
void getDateAndTime(uChar * dateAndTime) {

   /* Telium_Date_t date;

    Telium_Read_date(&date);

    //MMDDhhmmss
    sprintf(dateAndTime, "%.2s%.2s%.2s%.2s%.2s", date.month, date.day, date.hour, date.minute, date.second);*/

}

int procesarRespuestaConsultaDCC(uChar * dataReceive, int nBytes, uChar * tokenDcc, DatosVenta * tramaVenta) {

    ResultISOUnpack resultadoUnpack;
    ISOFieldMessage isoFieldMessage;
    uChar actionCode[2 + 1];
    uChar actionCodeOk[] = {0x00 };
    uChar montoConvertido[12 + 1];
    uChar montoTransaccion[12 + 1];
    uChar informacionMoneda[50 + 1];

    uChar tasaCambio[8 + 1];
    int iRet = 0;
    int indicador = 0;

    memset(&resultadoUnpack, 0x00, sizeof(resultadoUnpack));
    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(actionCode, 0x00, sizeof(actionCode));
    memset(montoConvertido, 0x00, sizeof(montoConvertido));
    memset(tasaCambio, 0x00, sizeof(tasaCambio));
    memset(montoTransaccion, 0x00, sizeof(montoTransaccion));
    memset(informacionMoneda, 0x00, sizeof(informacionMoneda));

    ////// UNPACK DE LA TRAMA DE RESPUESTA //////
    resultadoUnpack = unpackISOMessage(dataReceive, nBytes);

    isoFieldMessage = getField(39);

    //// CONTINUAR SI EL CODIGO DE RESPUESTA ES APROBADO("00") //////
    if (isoFieldMessage.totalBytes > 0 && strcmp(isoFieldMessage.valueField, "00") == 0) {

        ////// OBTENER EL ACTION CODE ///////
        memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
        isoFieldMessage = getField(60);

        if (isoFieldMessage.totalBytes > 0) {
            indicador++;
            memcpy(actionCode, isoFieldMessage.valueField + 2, 2);
            memcpy(informacionMoneda, isoFieldMessage.valueField, isoFieldMessage.totalBytes);
        }

        ////// OBTENER EL MONTO CONVERTIDO ///////
        memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
        isoFieldMessage = getField(6);

        if (isoFieldMessage.totalBytes > 0) {
            indicador++;
            memcpy(montoConvertido, isoFieldMessage.valueField, 12);
        }

        ////// OBTENER LA TASA DE CAMBIO ///////
        memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
        isoFieldMessage = getField(10);

        if (isoFieldMessage.totalBytes > 0) {
            indicador++;
            memcpy(tasaCambio, isoFieldMessage.valueField, 8);
        }

        ////// OBTENER EL MONTO ///////
        memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
        isoFieldMessage = getField(4);

        if (isoFieldMessage.totalBytes > 0) {
            indicador++;
            memcpy(montoTransaccion, isoFieldMessage.valueField, 12);
        }

        //// PROCESAR VERIFICACION TRANSACCION DCC /////
        if (indicador == 4 && (memcmp(actionCode, actionCodeOk, 2) == 0)) {
            iRet = procesarVerificacionDCC(montoConvertido, tasaCambio, montoTransaccion, tramaVenta);
        }
    }

    ////// SI ES UNA TRANSACCION CON DCC - OBTENER CAMPOS /////
    if (iRet == IS_TRANSACCION_DCC) {
        armarTokenDcc(informacionMoneda, tokenDcc);
    }

    return iRet;

}

int procesarVerificacionDCC(uChar * montoConvertido, uChar * tasaCambio, uChar * montoTransaccion,
                            DatosVenta * tramaVenta) {

    int iRet = 0;
    int salida = 0;
    int indiceDecimal = 0;
    int confirmacion = 0;
    uChar stringAux[20 + 1];
    uChar montoTransaccionAux[30 + 1];
    uChar montoConvertidoAux[30 + 1];
    uChar tasaCambioAux[30 + 1];
    uChar linea1[30 + 1];
    uChar linea2[30 + 1];
    uChar linea3[30 + 1];

    memset(stringAux, 0x00, sizeof(stringAux));
    memset(montoTransaccionAux, 0x00, sizeof(montoTransaccionAux));
    memset(montoConvertidoAux, 0x00, sizeof(montoConvertidoAux));
    memset(tasaCambioAux, 0x00, sizeof(tasaCambioAux));
    memset(linea1, 0x00, sizeof(linea1));
    memset(linea2, 0x00, sizeof(linea2));
    memset(linea3, 0x00, sizeof(linea3));

    leftTrim(stringAux, montoTransaccion, '0');

    if (strlen(stringAux) == 0) {
        strcpy(montoTransaccionAux, "$ 0");
    } else {
        ///// MONTO DE LA TRANSACCION CON FORMATO ///////
        formatString(0x00, 0, stringAux, strlen(stringAux) - 2, montoTransaccionAux, 1);
    }

    memset(stringAux, 0x00, sizeof(stringAux));
    leftTrim(stringAux, montoConvertido, '0');

    if (strlen(stringAux) == 0) {
        strcpy(montoConvertidoAux, "$ 0");
    } else {
        ///// MONTO DE LA TRANSACCION CON FORMATO ///////
        formatString(0x00, 0, stringAux, strlen(stringAux), montoConvertidoAux, 6);
    }

    //// TRATAR TASA DE CAMBIO /////
    memset(stringAux, 0x00, sizeof(stringAux));
    indiceDecimal = tasaCambio[0] - '0';
    memcpy(tasaCambioAux, tasaCambio + 1, (7 - indiceDecimal));
    leftTrim(stringAux, tasaCambioAux, '0');
    memset(tasaCambioAux, 0x00, sizeof(tasaCambioAux));
    strcpy(tasaCambioAux, stringAux);
    memset(stringAux, 0x00, sizeof(stringAux));

    if (strlen(stringAux) == 0) {
        formatString(0x00, 0, tasaCambioAux, strlen(tasaCambioAux), stringAux, 1);
    } else {
        strcpy(stringAux, "0");
    }

    memset(tasaCambioAux, 0x00, sizeof(tasaCambioAux));
    strcpy(tasaCambioAux, stringAux + 2);

    if (indiceDecimal > 0 && indiceDecimal < 7) {
        strcat(tasaCambioAux, ",");
    }
    memcpy(tasaCambioAux + strlen(tasaCambioAux), tasaCambio + (8 - indiceDecimal), indiceDecimal);
    memset(stringAux, 0x00, sizeof(stringAux));
    leftTrim(stringAux, tasaCambioAux, '0');
    if (strlen(stringAux) == 3) {
        memset(tasaCambioAux, 0x00, sizeof(tasaCambioAux));
        strcpy(tasaCambioAux, stringAux);
        memset(stringAux, 0x00, sizeof(stringAux));
        sprintf(stringAux, "0%.3s", tasaCambioAux);
    }
    memset(tasaCambioAux, 0x00, sizeof(tasaCambioAux));
    strcpy(tasaCambioAux, "$ ");

    if (strlen(stringAux) == 0) {
        strcpy(stringAux, "0");
    }

    strcpy(tasaCambioAux + 2, stringAux);

    ////// REALIZAR LA VERIFICACION //////
    sprintf(linea1, "C. NETA: %s", montoTransaccionAux);
    sprintf(linea2, "T. CAMBIO: %s", tasaCambioAux);
    sprintf(linea3, "C. MON: %s", montoConvertidoAux);

    //screenMessageFiveLine("VERIFICACION DCC", linea1, linea2, linea3, "REALIZAR TRANS. DCC?", "Si(Verde)            No(Rojo)", 10);

    /*do {
        confirmacion = waitKey(30 * 100);

        switch (confirmacion) {
            case T_VAL:
                iRet = IS_TRANSACCION_DCC;
                strcpy(tramaVenta->acquirerId, tasaCambioAux);
                strcpy(tramaVenta->nombreComercio, montoConvertidoAux);
                salida = 1;
                break;
            case T_ANN:
            case TIMEOUT:
                iRet = -1;
                salida = 1;
                break;
        }
    } while (salida != 1);
*/
    return iRet;

}

void armarTokenDcc(uChar * informacionMoneda, uChar * tokenDcc) {

    uChar stringAux[20 + 1];
    uChar codigoMoneda[4 + 1];
    uChar montoConvertido[12 + 1];
    uChar niiDcc[4 + 1];
    uChar exchangeRate[10 + 1];
    uChar invertedRate[10 + 1];
    uChar encabezadoDcc[] = "01DC\x00\x42";
    int indice = 0;

    memset(stringAux, 0x00, sizeof(stringAux));
    memset(niiDcc, 0x00, sizeof(niiDcc));
    memset(codigoMoneda, 0x00, sizeof(codigoMoneda));
    memset(exchangeRate, 0x00, sizeof(exchangeRate));
    memset(invertedRate, 0x00, sizeof(invertedRate));

    getParameter(NII_DCC, niiDcc);

    //// ENCABEZADO TOKEN /////
    memcpy(tokenDcc + indice, encabezadoDcc, 6);
    indice += 6;
    //// SERVICIO DCC /////
    memcpy(tokenDcc + indice, niiDcc + 2, 2);
    indice += 2;
    ///// SEPARADOR /////
    memcpy(tokenDcc + indice, " ", 1);
    indice += 1;
    ///// MONTO CONVERTIDO //////
    memcpy(stringAux, informacionMoneda + 4, 6);
    _convertBCDToASCII_(montoConvertido, stringAux, 12);
    memcpy(tokenDcc + indice, montoConvertido, 12);
    indice += 12;
    ///// SEPARADOR /////
    memcpy(tokenDcc + indice, " ", 1);
    indice += 1;
    ///// CODIGO DE LA MONEDA //////
    memset(stringAux, 0x00, sizeof(stringAux));
    memcpy(stringAux, informacionMoneda + 10, 2);
    _convertBCDToASCII_(codigoMoneda, stringAux, 4);
    memcpy(tokenDcc + indice, codigoMoneda, 4);
    indice += 4;
    ///// SEPARADOR /////
    memcpy(tokenDcc + indice, " ", 1);
    indice += 1;
    ///// TASA DE CAMBIO /////
    memset(stringAux, 0x00, sizeof(stringAux));
    memcpy(stringAux, informacionMoneda + 12, 5);
    _convertBCDToASCII_(exchangeRate, stringAux, 10);
    memcpy(tokenDcc + indice, exchangeRate, 10);
    indice += 10;
    ///// SEPARADOR /////
    memcpy(tokenDcc + indice, " ", 1);
    indice += 1;
    ///// CAMBIO INVERTIDO //////
    memset(stringAux, 0x00, sizeof(stringAux));
    memcpy(stringAux, informacionMoneda + 17, 5);
    _convertBCDToASCII_(invertedRate, stringAux, 10);
    memcpy(tokenDcc + indice, invertedRate, 10);
    indice += 10;

}

int procesarDcc(DatosVenta *datosVenta, uChar *codigoPais, uChar *tokenDC, DatosCampo61Eco *campo61Eco, uChar *tokenDP) {

    int iRet = 1;
    uChar niiDcc[4 + 1];
    uChar bin[9 + 1];
    uChar cardName[TAM_CARD_NAME + 1];
    uChar campo61[256 + 1];
    uChar campo63[100 + 1];
    uChar variableAux[16 + 1];
    ResultTokenPack resultTokenPack;
    DatosInicializacionDcc datosInicializacionDcc;

    memset(niiDcc, 0x00, sizeof(niiDcc));
    memset(bin, 0x00, sizeof(bin));
    memset(cardName, 0x00, sizeof(cardName));
    memset(&resultTokenPack, 0x00, sizeof(resultTokenPack));
    memset(campo61, 0x00, sizeof(campo61));
    memset(campo63, 0x00, sizeof(campo63));
    memset(variableAux, 0x00, sizeof(variableAux));
    memset(&datosInicializacionDcc, 0x00, sizeof(&datosInicializacionDcc));

    datosInicializacionDcc = traerDatosInicialiazacionDcc();

    memcpy(cardName, datosVenta->cardName, TAM_CARD_NAME);

    getParameter(NII_DCC, niiDcc);

    ///// SI EXISTE NII_DCC VERIFICAR SI ESTE BIN
    ////  REALIZA CONSULTA DCC.
    if (atoi(niiDcc) > 0 && strncmp(codigoPais, NO_CODIGO, strlen(NO_CODIGO)) == 0) {
        memcpy(bin, datosVenta->track2, 9);
        iRet = verificarBinTarjeta(bin);
    } else if (atoi(niiDcc) > 0) {
        iRet = verificarTagTarjeta(codigoPais);
    }

    if (iRet == IS_TARJETA_DCC) {
        iRet = comprobarActivacionFranquicia(cardName, codigoPais);
    }

    if (iRet == IS_TARJETA_DCC) {//SE AGREGA TIPO CUENTA  "00" PARA PAYPASS
        iRet = realizarConsultaDcc(datosVenta, codigoPais, campo61, campo63);
    }

    if (strcmp(datosVenta->isQPS, "3") == 0 && strcmp(datosVenta->tipoCuenta, "00") != 0
        && strcmp(datosVenta->tipoCuenta, "10") != 0 && strcmp(datosVenta->tipoCuenta, "20") != 0) {
        /*do {
            iRet = getStringKeyboard("NUMERO DE CUOTAS", "INGRESE", "CUOTAS", 2, variableAux, DATO_NUMERICO);
            if (iRet > 0 && atoi(variableAux) == 0) {
                screenMessage("MENSAJE", "DATO INVALIDO", "INGRESE UN VALOR", "CUOTAS ENTRE 1-99", 2 * 1000);
            }
        } while (iRet > 0 && atoi(variableAux) == 0);*/
        leftPad(datosVenta->fiel42, variableAux, 0x30, 2);
        sprintf(datosVenta->cuotas, "%2s", datosVenta->fiel42);
        memset(datosVenta->fiel42 + 2, 0x30, 13);
    }

    if (iRet == IS_TRANSACCION_DCC) { // igual a IS_TARJETA_DCC
        if (strcmp(datosInicializacionDcc.perfilActivo, "1") == 0) {
            resultTokenPack = armarTokenDC(campo61, datosVenta->totalVenta, campo61Eco);
            memcpy(tokenDC, resultTokenPack.tokenPackMessage, 44);
            memcpy(datosVenta->estadoTarifa, "DC", 2);
        } else {
            resultTokenPack = armarTokenDP(datosVenta);
            memcpy(tokenDP, resultTokenPack.tokenPackMessage, 39);
            memcpy(datosVenta->estadoTarifa, "DP", 2);
        }
    }

    return iRet;
}

DatosInicializacionDcc traerDatosInicialiazacionDcc(void) {

    DatosInicializacionDcc datosDcc;
    DatosInicializacionDcc perfil1;
    DatosInicializacionDcc perfil2;

    uChar tablaPar[10 + 1];
    uChar tablaDcc[TAM_BUFFER + 1];
    int iRet = 0;

    memset(tablaDcc, 0x00, sizeof(tablaDcc));
    memset(tablaPar, 0x00, sizeof(tablaPar));
    memset(&datosDcc, 0x00, sizeof(datosDcc));
    memset(&perfil1, 0x00, sizeof(perfil1));
    memset(&perfil2, 0x00, sizeof(perfil2));
    datosDcc.codigoRespuesta = 0;

    iRet = leerArchivo(discoNetcom, (char *)TABLA_PARAMETROS, tablaPar);

    if (iRet > 0) {
        iRet = leerArchivo(discoNetcom, (char *)TABLA_DCC, tablaDcc);
    }

    if ((tablaPar[6] & 0x80) ^ ((tablaPar[6] << 1) & 0x80)) {

        datosDcc.codigoRespuesta = 1;
        memcpy(datosDcc.perfilActivo, "0", 1);

        memcpy(perfil1.IDDCC, tablaDcc + 4, 1);
        memcpy(perfil1.franquiciaActiva, tablaDcc + 5, 1);
        memcpy(perfil1.parametroDcc, tablaDcc + 6, 2);
        memcpy(perfil1.niiAsignado, tablaDcc + 8, 2);

        memcpy(perfil2.IDDCC, tablaDcc + 14, 1);
        memcpy(perfil2.franquiciaActiva, tablaDcc + 15, 1);
        memcpy(perfil2.parametroDcc, tablaDcc + 16, 2);
        memcpy(perfil2.niiAsignado, tablaDcc + 18, 2);

        if ((perfil1.IDDCC[0] & 0x01) && (tablaPar[6] & 0x80)) { //activo indice 17
            memcpy(datosDcc.IDDCC, perfil1.IDDCC, 1);
            memcpy(datosDcc.franquiciaActiva, perfil1.franquiciaActiva, 1);
            memcpy(datosDcc.parametroDcc, perfil1.parametroDcc, 2);
            memcpy(datosDcc.niiAsignado, perfil1.niiAsignado, 2);
            memcpy(datosDcc.perfilActivo, "1", 1);
        } else if ((perfil2.IDDCC[0] & 0x01) && (tablaPar[6] & 0x80)) { //activo indice 17
            memcpy(datosDcc.IDDCC, perfil2.IDDCC, 1);
            memcpy(datosDcc.franquiciaActiva, perfil2.franquiciaActiva, 1);
            memcpy(datosDcc.parametroDcc, perfil2.parametroDcc, 2);
            memcpy(datosDcc.niiAsignado, perfil2.niiAsignado, 2);
            memcpy(datosDcc.perfilActivo, "1", 1);
        } else if ((perfil1.IDDCC[0] & 0x02) && ((tablaPar[6] << 1) & 0x80)) { //activo indice 18
            memcpy(datosDcc.IDDCC, perfil1.IDDCC, 1);
            memcpy(datosDcc.franquiciaActiva, perfil1.franquiciaActiva, 1);
            memcpy(datosDcc.parametroDcc, perfil1.parametroDcc, 2);
            memcpy(datosDcc.niiAsignado, perfil1.niiAsignado, 2);
            memcpy(datosDcc.perfilActivo, "2", 1);
        } else if ((perfil2.IDDCC[0] & 0x02) && ((tablaPar[6] << 1) & 0x80)) { //activo indice 18
            memcpy(datosDcc.IDDCC, perfil2.IDDCC, 1);
            memcpy(datosDcc.franquiciaActiva, perfil2.franquiciaActiva, 1);
            memcpy(datosDcc.parametroDcc, perfil2.parametroDcc, 2);
            memcpy(datosDcc.niiAsignado, perfil2.niiAsignado, 2);
            memcpy(datosDcc.perfilActivo, "2", 1);
        } else {
            datosDcc.codigoRespuesta = 0;
        }
    }

    return datosDcc;
}

int verificarBinTarjeta(uChar *bin) {

    int iRet = 0;
    int salidaWhile = 0;
    int posicion = 0;
    int lengthDataBin = 20;
    int iRetSearch = 0;
    uChar dataBines[20 + 1];
    uChar rangoBajo[10 + 1];
    uChar rangoAlto[10 + 1];

    iRet = verificarArchivo(discoNetcom, ARCHIVO_BINLOCAL);
    iRet = (iRet != FS_OK) ? FALSE : TRUE;

    if (iRet > 0) {

        do {
            memset(dataBines, 0x00, sizeof(dataBines));
            memset(rangoBajo, 0x00, sizeof(rangoBajo));
            memset(rangoAlto, 0x00, sizeof(rangoAlto));
            salidaWhile = buscarArchivo(discoNetcom, (char *)ARCHIVO_BINLOCAL, dataBines, posicion, lengthDataBin);

            memcpy(rangoBajo, dataBines, 10);
            memcpy(rangoAlto, dataBines + 10, 10);

            rangoBajo[9] = 0x00;
            rangoAlto[9] = 0x00;
            if (atol(bin) >= atol(rangoBajo) && atol(bin) <= atol(rangoAlto)) {
                iRetSearch = 1;
            }

            if (iRetSearch == 1) {
                iRet = 0;
                break;
            }

            posicion += (lengthDataBin + 2);

        } while (salidaWhile == 0);

        if (iRet > 0) {
            iRet = IS_TARJETA_DCC;
        }
    }

    if (iRet != IS_TARJETA_DCC) {
        iRet = 1;
    }

    return iRet;
}
int verificarTagTarjeta(uChar * codigoPais) {

    int iRet = 1;

    if (strncmp(codigoPais, "0170", 4) != 0) {
        iRet = IS_TARJETA_DCC;
    }

    return iRet;
}

int comprobarActivacionFranquicia(uChar *cardName, uChar *codigoPais) {

    int iRet = 1;
    int tipoTarjeta = 0;
    uChar banderaActivacion[2 + 1];
    uChar banderaAuxiliar[2 + 1];
    uChar tablaDcc[TAM_BUFFER + 1];
    uChar tablaPar[10 + 1];
    DatosInicializacionDcc datosInicializacionDcc;
    uChar nombreFranquicia[TAM_CARD_NAME + 1];

    memset(banderaActivacion, 0x00, sizeof(banderaActivacion));
    memset(banderaAuxiliar, 0x00, sizeof(banderaAuxiliar));
    memset(tablaDcc, 0x00, sizeof(tablaDcc));
    memset(tablaPar, 0x00, sizeof(tablaPar));
    memset(&datosInicializacionDcc, 0x00, sizeof(datosInicializacionDcc));
    memset(nombreFranquicia, 0x00, sizeof(nombreFranquicia));

    strcpy(nombreFranquicia, cardName);
    iRet = leerArchivo(discoNetcom, (char *)TABLA_PARAMETROS, tablaPar);

    if (iRet > 0) {
        iRet = leerArchivo(discoNetcom, (char *)TABLA_DCC, tablaDcc);
    }

    datosInicializacionDcc = traerDatosInicialiazacionDcc();
    memcpy(banderaAuxiliar, datosInicializacionDcc.franquiciaActiva, 1);

    convertirMayusculas((char*)cardName);

    //!revisar nombres correctos de las tarjetas
    if (strncmp(cardName, "MASTERCARD", 10) == 0 || strncmp(cardName, "MasterCard", 8) == 0
        || strncmp(cardName, "MAESTRO", 7) == 0) { //!verificado
        tipoTarjeta = 1;
    } else if (strncmp(cardName, "DEBIT MAST", 10) == 0) { //!por verificar
        tipoTarjeta = 2;
    } else if (strncmp(cardName, "VISA", 4) == 0) { //!verificado
        tipoTarjeta = 3;
    }

    if (iRet > 0) {

        iRet = 1;
        _convertBCDToASCII_(banderaActivacion, banderaAuxiliar, 2);

        switch (atoi(banderaActivacion)) {
            case 1:
                if (tipoTarjeta == 1 || tipoTarjeta == 2) {
                    iRet = IS_TARJETA_DCC;
                }
                break;
            case 2:
                if (tipoTarjeta == 3) {
                    iRet = IS_TARJETA_DCC;
                }
                break;
            case 3:
                if (tipoTarjeta == 1 || tipoTarjeta == 2 || tipoTarjeta == 3) {
                    iRet = IS_TARJETA_DCC;
                }
                break;
            default:
                iRet = 1;
                break;
        }
    }

    if (strncmp(codigoPais, "0170", 4) != 0 && strncmp(codigoPais, NO_CODIGO, strlen(NO_CODIGO)) != 0) {
        iRet = IS_TARJETA_DCC;
    }

    return iRet;
}

void convertirMayusculas(uChar *ptrS) {

    int contador = 0;

    while (*ptrS != '\0' || contador < 10) {

        if (islower(*ptrS)) {
            *ptrS = toupper(*ptrS);
        }

        ++ptrS; /* mueve ptrS al siguiente carácter */
        contador++;
    }
}

int realizarConsultaDcc(DatosVenta *datosVenta, uChar *codigoPais, uChar *campo61, uChar *campo63) {

    int iRet = 0;
    int indicadorReverso = 0;
    ResultISOPack resultISOPack;
    uChar datosRecibidos[512 + 1];
    DatosInicializacionDcc datosInicializacionDcc;

    memset(&resultISOPack, 0x00, sizeof(resultISOPack));
    memset(datosRecibidos, 0x00, sizeof(datosRecibidos));
    memset(&datosInicializacionDcc, 0x00, sizeof(&datosInicializacionDcc));

    datosInicializacionDcc = traerDatosInicialiazacionDcc();

    if (strcmp(datosInicializacionDcc.perfilActivo, "1") == 0) {
        resultISOPack = empaquetarMensajeDcc(datosVenta, codigoPais); //!
        setParameter(PRODUCTO_PPRO_DCC, (char *)"0");
    } else {
        resultISOPack = empaquetarMensajePPRO18(datosVenta, codigoPais);
        setParameter(PRODUCTO_PPRO_DCC, (char *)"1");
    }

    setParameter(TIPO_TRANSACCION, (char *)"61");

    iRet = realizarTransaccion(resultISOPack.isoPackMessage, resultISOPack.totalBytes, datosRecibidos, TRANSACCION_DCC,
                               CANAL_PERMANENTE, REVERSO_NO_GENERADO);

    if (iRet == -1 && (verificarArchivo(discoNetcom, ARCHIVO_REVERSO) == FS_OK)) {
        indicadorReverso = 1;
    }

    releaseSocket();

    if (iRet > 0 && strcmp(datosInicializacionDcc.perfilActivo, "1") == 0) {
        iRet = desempaquetarRespuesta(datosRecibidos, iRet, campo61, datosVenta);
    } else if (iRet > 0) {
        iRet = desempaquetarConsultaPPRO18(datosRecibidos, iRet, campo63, datosVenta);
    }

    if (iRet < 1 && indicadorReverso != 1 && iRet != -2) {
        iRet = 1;
    } else if (indicadorReverso == 1) {
        iRet = -1;
    } else if (iRet == -2) {
        iRet = -1;
    }

    return iRet;
}

ResultTokenPack armarTokenDC(uChar *campo61, uChar * monto, DatosCampo61Eco *campo61Eco) { //!tener en cuenta estructura como parametro

    int indice = 0;
    int cantidadDec = 0;
    uChar auxiliar[16 + 1];
    uChar campo[64 + 1];
    uChar amount[16 + 1];
    uChar amountAux[16 + 1];
    uChar margen[12 + 1];
    uChar montoAux[12 + 1];
    uChar rateInt[16 + 1];
    uChar rateIntAux[16 + 1];
    uChar rateDec[16 + 1];
    uChar rateDecAux[16 + 1];
    uChar rateDisplay[16 + 1];
    uChar rateDisplayAux[16 + 1];
    DatosEmpaquetados datosTokenDC;
    DatosInicializacionDcc datosDcc;
    ResultTokenPack resultTokenPack;

    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(campo, 0x00, sizeof(campo));
    memset(amount, 0x00, sizeof(amount));
    memset(amountAux, 0x00, sizeof(amountAux));
    memset(margen, 0x00, sizeof(margen));
    memset(montoAux, 0x00, sizeof(montoAux));
    memset(rateInt, 0x00, sizeof(rateInt));
    memset(rateDec, 0x00, sizeof(rateDec));
    memset(rateIntAux, 0x00, sizeof(rateIntAux));
    memset(rateDecAux, 0x00, sizeof(rateDecAux));
    memset(rateDisplay, 0x00, sizeof(rateDisplay));
    memset(rateDisplayAux, 0x00, sizeof(rateDisplayAux));
    memset(&datosTokenDC, 0x00, sizeof(datosTokenDC));
    memset(&datosDcc, 0x00, sizeof(datosDcc));
    memset(&resultTokenPack, 0x00, sizeof(resultTokenPack));

    ////////////////////////////////////////////
    ////parametroDcc
    datosDcc = traerDatosInicialiazacionDcc();

    memcpy(datosTokenDC.buffer, datosDcc.parametroDcc, 2); //!revisar asignacion sin tabla.
    indice += 2;
    ////////////////////////////////////////////

    ////////////////////////////////////////----
    ////Foreign  Amount - DCC transaction amount
    //// campo 6  (Foreign Amount)
    extraerCampo(6, campo61, campo);
    strcpy(campo61Eco->fielId6, campo);
    extraerCampo(9, campo61, campo61Eco->fielId9);
    extraerCampo(10, campo61, campo61Eco->fielId10);
    extraerCampo(20, campo61, campo61Eco->fielId20);
    extraerCampo(21, campo61, campo61Eco->fielId21);

    /** 6.2 – Foreign Currency Amount 12 bytes*/
    memcpy(amountAux, campo + 3, 12);
    leftPad(amount, amountAux, '0', 16);
    memset(amountAux, 0x00, sizeof(amountAux));
    _convertASCIIToBCD_(amountAux, amount, 16);

    memcpy(datosTokenDC.buffer + indice, amountAux, 8);
    indice += 8;

    /** 6.1 – Foreign Currency Number */
    memcpy(datosTokenDC.buffer + indice, campo, 3);
    indice += 3;

    ///-----////////////////////////////////////

    ///////////////////////////////////////-----
    /// campo 26 Mark-Up Percentage
    memset(campo, 0x00, sizeof(campo));
    extraerCampo(26, campo61, campo);

    //!amount usada para calcular el monto mas el margen
    memset(amount, 0x00, sizeof(amount));
    memset(amountAux, 0x00, sizeof(amountAux));

    leftPad(montoAux, monto, '0', 10);
    calcularMontoMargen(montoAux, campo, amount, margen);

    _convertASCIIToBCD_(amountAux, amount, 16);
    memcpy(datosTokenDC.buffer + indice, amountAux, 8);
    indice += 8;
    ///-----////////////////////////////////////

    ///////////////////////////////////////-----
    /// campo 18 Inverted Rate
    memset(amount, 0x00, sizeof(amount));
    memset(amountAux, 0x00, sizeof(amountAux));
    memset(campo, 0x00, sizeof(campo));

    extraerCampo(18, campo61, campo);

    memcpy(auxiliar, campo, 1);

    cantidadDec = atoi(auxiliar);
    memcpy(rateInt, campo + 1, 9 - cantidadDec);
    memcpy(rateDec, campo + 1 + (9 - cantidadDec), cantidadDec);

    _rightPad_(rateDecAux, rateDec, '0', 9);
    leftPad(rateIntAux, rateInt, '0', 7);

    memcpy(rateDisplay, rateIntAux, 7);
    memcpy(rateDisplay + 7, rateDecAux, 9);

    _convertASCIIToBCD_(rateDisplayAux, rateDisplay, 16);

    memcpy(datosTokenDC.buffer + indice, rateDisplayAux, 8);
    indice += 8;
    ///-----////////////////////////////////////

    ///////////////////////////////////////-----
    memcpy(datosTokenDC.buffer + indice, "D", 1);
    indice += 1;

    /// margen del monto entregado en el formato requerido por "calcularMontoMargen"
    memcpy(datosTokenDC.buffer + indice, margen, 8); //! revisar llenar de ceros a la izquierda el margen
    indice += 8;

    datosTokenDC.totalBytes = indice;

    _setTokenMessage_((char *)"DC", datosTokenDC.buffer, datosTokenDC.totalBytes);
    resultTokenPack = _packTokenMessage_();

    return resultTokenPack;
}

ResultTokenPack armarTokenDP(DatosVenta *datosVenta) {

    int indice = 0;
    DatosEmpaquetados datosTokenDP;
    DatosInicializacionDcc datosDcc;
    ResultTokenPack resultTokenPack;
    DatosCampo63 campo63;
    uChar campoAux[3 + 1];

    memset(&datosTokenDP, 0x00, sizeof(datosTokenDP));
    memset(&datosDcc, 0x00, sizeof(datosDcc));
    memset(&resultTokenPack, 0x00, sizeof(resultTokenPack));
    memset(&campo63, 0x00, sizeof(campo63));
    memset(campoAux, 0x00, sizeof(campoAux));

    datosDcc = traerDatosInicialiazacionDcc();
    campo63 = capturarDatoCampo63();

    memcpy(datosTokenDP.buffer, datosDcc.parametroDcc, 2);
    indice += 2;

    memcpy(campoAux, campo63.Tag32 + 28, 3);
    memcpy(datosTokenDP.buffer + indice, campoAux, 3);
    indice += 3;

    memcpy(datosTokenDP.buffer + indice, campo63.Tag34, 3);
    indice += 3;

    memcpy(datosTokenDP.buffer + indice, campo63.Tag67, 1);
    indice += 1;

    memcpy(datosTokenDP.buffer + indice, datosVenta->textoInformacion + 6, 12);
    indice += 12;

    memcpy(datosTokenDP.buffer + indice, datosVenta->field61, 8);
    indice += 8;

    memcpy(datosTokenDP.buffer + indice, datosVenta->textoInformacion, 2);
    indice += 2;

    memcpy(datosTokenDP.buffer + indice, datosVenta->textoInformacion + 2, 2);
    indice += 2;

    datosTokenDP.totalBytes = indice;

    _setTokenMessage_((char *)"DP", datosTokenDP.buffer, datosTokenDP.totalBytes);
    resultTokenPack = _packTokenMessage_();

    return resultTokenPack;
}
ResultISOPack empaquetarMensajeDcc(DatosVenta *datosVenta, uChar *codigoPais) {

    ResultISOPack resultISOPack;
    ISOHeader isoHeader8583;
    uChar niiDcc[TAM_NII + 1];
    uChar dateAndTime[10 + 1];
    uChar codigoProceso[6 + 1];
    uChar codigoMoneda[3 + 1];
    uChar monto[12 + 1];
    uChar rrn[12 + 1];
    uChar systemTraceNumber[6 + 1];
    uChar field61[256 + 1];
    uChar campo60[256 + 1];
    uChar campo61[256 + 1];
    uChar cardName[TAM_CARD_NAME + 1];
    uChar montoAuxiliar[12 + 1];

    DatosEmpaquetados datos;

    memset(&resultISOPack, 0x00, sizeof(resultISOPack));
    memset(&isoHeader8583, 0x00, sizeof(isoHeader8583));
    memset(dateAndTime, 0x00, sizeof(dateAndTime));
    memset(systemTraceNumber, 0x00, sizeof(systemTraceNumber));
    memset(field61, 0x00, sizeof(field61));
    memset(rrn, 0x00, sizeof(rrn));
    memset(codigoProceso, 0x00, sizeof(codigoProceso));
    memset(codigoMoneda, 0x00, sizeof(codigoMoneda));
    memset(campo60, 0x00, sizeof(campo60));
    memset(campo61, 0x00, sizeof(campo61));
    memset(niiDcc, 0x00, sizeof(niiDcc));
    memset(&datos, 0x00, sizeof(datos));
    memset(cardName, 0x00, sizeof(cardName));
    memset(montoAuxiliar, 0x00, sizeof(montoAuxiliar));

    getParameter(NII_DCC, niiDcc);
    generarSystemTraceNumber(systemTraceNumber,(char *)"");

    //// RRN ////
    memset(rrn, '0', 6);
    memcpy(rrn + 6, systemTraceNumber, 6);

    memcpy(datosVenta->systemTrace, systemTraceNumber, 6);

    //// POS ENTRY MODE /////
    if (strlen(datosVenta->posEntrymode) == 0) {
        strcpy(datosVenta->posEntrymode, "051");
    }

    //// CODIGO MONEDA ////
    if ((strcmp(datosVenta->posEntrymode, "051") != 0)) {
        memset(codigoMoneda, 0x00, sizeof(codigoMoneda));
        strcpy(codigoMoneda, "000");
    } else {
        //!memcpy(codigoMoneda,codMoneda,3);
    }

    //// MONTO ////
    memcpy(monto, datosVenta->totalVenta, 10);
    strcat(monto, "00");

    //// CODIGO DE PROCESO /////
    sprintf(codigoProceso, "00%.2s00", datosVenta->tipoCuenta);

    /// HEADER ///
    isoHeader8583.TPDU = 60;
    memcpy(isoHeader8583.destination, niiDcc, TAM_NII + 1);
    memcpy(isoHeader8583.source, niiDcc, TAM_NII + 1);
    setHeader(isoHeader8583);
    /// MTI ///
    setMTI((uChar *)"0800");

    obtenerFechaHora(dateAndTime);
    uChar hora[6 + 1];
    uChar mes[4 + 1];

    memset(hora, 0x00, sizeof(hora));
    memset(mes, 0x00, sizeof(mes));

    memcpy(hora, dateAndTime + 4, 6);
    memcpy(mes, dateAndTime, 4);

    /// LLENAR CAMPOS ///
    setField(7, dateAndTime, 10);
    setField(11, systemTraceNumber, 6);
    setField(12, hora, 6);
    setField(13, mes, 4);

    datos = armarCampo60();
    setField(60, datos.buffer, datos.totalBytes);

    memcpy(cardName, datosVenta->cardName, TAM_CARD_NAME);

    memset(&datos, 0x00, sizeof(datos));

    sprintf(montoAuxiliar, "%d", atoi(datosVenta->totalVenta) + atoi(datosVenta->propina));
    datos = armarCampo61(montoAuxiliar, cardName, datosVenta->track2, codigoPais); //!revisar
    setField(61, datos.buffer, datos.totalBytes);

    resultISOPack = packISOMessage();

    return resultISOPack;
}

ResultISOPack empaquetarMensajePPRO18(DatosVenta *datosVenta, uChar *codigoPais) {

    ResultISOPack resultISOPack;
    ISOHeader isoHeader8583;
    DatosEmpaquetados datos;
    uChar niiDcc[TAM_NII + 1];
    uChar dateAndTime[10 + 1];
    uChar numeroTerminal[8 + 1];
    uChar codigoProceso[6 + 1];
    uChar codigoMoneda[3 + 1];
    uChar monto[12 + 1];
    uChar systemTraceNumber[6 + 1];
    uChar campo63[256 + 1];
    uChar variableAux[36 + 1];

    memset(&resultISOPack, 0x00, sizeof(resultISOPack));
    memset(&isoHeader8583, 0x00, sizeof(isoHeader8583));
    memset(&datos, 0x00, sizeof(datos));
    memset(niiDcc, 0x00, sizeof(niiDcc));
    memset(dateAndTime, 0x00, sizeof(dateAndTime));
    memset(numeroTerminal, 0x00, sizeof(numeroTerminal));
    memset(codigoProceso, 0x00, sizeof(codigoProceso));
    memset(codigoMoneda, 0x00, sizeof(codigoMoneda));
    memset(monto, 0x00, sizeof(monto));
    memset(systemTraceNumber, 0x00, sizeof(systemTraceNumber));
    memset(campo63, 0x00, sizeof(campo63));
    memset(variableAux, 0x00, sizeof(variableAux));

    getParameter(NII_DCC, niiDcc);
    getParameter(NUMERO_TERMINAL, numeroTerminal);
    generarSystemTraceNumber(systemTraceNumber,(char *)"");

    //// SYSTEM TRACE ////
    memcpy(datosVenta->systemTrace, systemTraceNumber, 6);

    //// POS ENTRY MODE /////
    if (strlen(datosVenta->posEntrymode) == 0) {
        strcpy(datosVenta->posEntrymode, "051");
    }

    //// CODIGO MONEDA ////
    strcpy(codigoMoneda, "170");

    //// MONTO ////
    if (strcmp(datosVenta->posEntrymode, "051") == 0) {
        sprintf(variableAux, "%d", atoi(datosVenta->totalVenta) + atoi(datosVenta->propina));
        memcpy(monto, variableAux, 10);
    } else {
        memcpy(monto, datosVenta->totalVenta, 10);
    }
    strcat(monto, "00");

    //// CODIGO DE PROCESO /////
    sprintf(codigoProceso, "00%.2s00", datosVenta->tipoCuenta);

    //// CODIGO COMERCIO /////
    uChar variableAux2[TAM_FIELD_42 + 1];
    memset(variableAux2, 0x20, sizeof(variableAux2));
    getParameter(CODIGO_COMERCIO, variableAux2);
    //	memcpy(variableAux2, datosVenta->codigoComercio, TAM_ID_COMERCIO);

    /// HEADER ///
    isoHeader8583.TPDU = 60;
    memcpy(isoHeader8583.destination, niiDcc, TAM_NII + 1);
    memcpy(isoHeader8583.source, niiDcc, TAM_NII + 1);
    setHeader(isoHeader8583);

    /// MTI ///
    setMTI((uChar *)"0100");

    /// FECHA - HORA ///
    obtenerFechaHora(dateAndTime);
    uChar hora[6 + 1];
    uChar mes[4 + 1];
    memset(hora, 0x00, sizeof(hora));
    memset(mes, 0x00, sizeof(mes));
    memcpy(hora, dateAndTime + 4, 6);
    memcpy(mes, dateAndTime, 4);

    /// LLENAR CAMPOS ///
    setField(2, datosVenta->track2, 16);
    setField(3, codigoProceso, TAM_CODIGO_PROCESO);
    setField(4, monto, TAM_COMPRA_NETA);
    setField(7, dateAndTime, TAM_TIME);
    setField(11, systemTraceNumber, TAM_SYSTEM_TRACE);
    setField(22, datosVenta->posEntrymode, TAM_ENTRY_MODE);
    setField(24, niiDcc + 1, TAM_NII);
    setField(25, (uChar *)"00", TAM_POS_CONDITION);
    setField(41, numeroTerminal, TAM_TERMINAL_ID);
    setField(42, variableAux2, TAM_FIELD_42);


    datos = armarCampo63();
    setField(63, datos.buffer, datos.totalBytes);

    resultISOPack = packISOMessage();

    return resultISOPack;
}

int desempaquetarRespuesta(uChar *datosRecibidos, int longitudDatos, uChar *campo61, DatosVenta *datosVenta) {

    ISOFieldMessage isoFieldMessage;
    ResultISOUnpack resultadoUnpack;
    uChar codigoRespuesta[2 + 1];
    DatosEmpaquetados campo60;
    uChar actionCode[4 + 1];
    int iRet = 0;

    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(&resultadoUnpack, 0x00, sizeof(resultadoUnpack));
    memset(codigoRespuesta, 0x00, sizeof(codigoRespuesta));
    memset(&campo60, 0x00, sizeof(campo60));
    memset(actionCode, 0x00, sizeof(actionCode));
    resultadoUnpack = unpackISOMessage(datosRecibidos, longitudDatos);

    isoFieldMessage = getField(39);
    memcpy(codigoRespuesta, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    isoFieldMessage = getField(60);

    memcpy(campo60.buffer, isoFieldMessage.valueField, isoFieldMessage.totalBytes);
    campo60.totalBytes = isoFieldMessage.totalBytes;

    if (isoFieldMessage.totalBytes > 0) {
        memcpy(actionCode, isoFieldMessage.valueField + 32, 4);
    }

    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    isoFieldMessage = getField(61);

    if (isoFieldMessage.totalBytes > 0) {
        memcpy(campo61, isoFieldMessage.valueField, isoFieldMessage.totalBytes);
    } else {
        iRet = 0;
    }

    if (strcmp(codigoRespuesta, "00") == 0 && strncmp(actionCode, "8800", 4) == 0) {
        iRet = verificacionTramaRecibida(datosVenta->systemTrace);
        if (iRet > 0) {
            iRet = preguntarUsoDcc(campo61, datosVenta);
        }
    }

    return iRet;
}

int desempaquetarConsultaPPRO18(uChar *datosRecibidos, int longitudDatos, uChar *campo63, DatosVenta *datosVenta) {

    ISOFieldMessage isoFieldMessage;
    ResultISOUnpack resultadoUnpack;
    uChar codigoRespuesta[2 + 1];
    uChar auxTotalVenta[12 + 1];
    int iRet = 0;
    int validacion = 1;
    long montoTotal = 0;

    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(&resultadoUnpack, 0x00, sizeof(resultadoUnpack));
    memset(codigoRespuesta, 0x00, sizeof(codigoRespuesta));
    memset(auxTotalVenta, 0x00, sizeof(auxTotalVenta));

    resultadoUnpack = unpackISOMessage(datosRecibidos, longitudDatos);

    isoFieldMessage = getField(4);
    memcpy(auxTotalVenta, isoFieldMessage.valueField, isoFieldMessage.totalBytes);
    montoTotal = atol(datosVenta->totalVenta);
    sprintf(datosVenta->totalVenta, "%ld", montoTotal);

    isoFieldMessage = getField(6);
    memcpy(datosVenta->textoInformacion + 6, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

    isoFieldMessage = getField(10);
    memcpy(datosVenta->field61, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

    isoFieldMessage = getField(39);
    memcpy(codigoRespuesta, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

    isoFieldMessage = getField(49);
    memcpy(datosVenta->textoInformacion, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

    isoFieldMessage = getField(51);
    memcpy(datosVenta->textoInformacion + 3, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    isoFieldMessage = getField(63);

    borrarArchivo(discoNetcom, (char *)PPRO18_DCC);
    escribirArchivo(discoNetcom, (char *)PPRO18_DCC, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

    if (strcmp(codigoRespuesta, "00") == 0) {

        isoFieldMessage = getField(11);
        validacion = strncmp(isoFieldMessage.valueField, datosVenta->systemTrace, isoFieldMessage.totalBytes);

        if (validacion == 0) {
            iRet = preguntarDccPPRO(datosVenta);
        } else {
            iRet = -1;
        }
    }
    return iRet;
}

int extraerCampo(int IDcampo, uChar *buffer, uChar *campo) {

    int iRet = 0;
    int i = 0;
    int tamCampo = 0;
    int indice = 0;
    int indiceAux = 0;
    uChar longitud[TAM_MENSAJE_PURE + 1];
    uChar campoPrueba[TAM_BUFFER + 1];
    uChar bitMap[8 + 1];

    memset(longitud, 0x00, sizeof(longitud));
    memset(campoPrueba, 0x00, sizeof(campoPrueba));
    memset(bitMap, 0x00, sizeof(bitMap));

    memcpy(bitMap, buffer + 6, 8);

    for (i = 0; i < MAXIMOS_CAMPOS; i++) {

        indiceAux = indice;

        iRet = definirExistenciaCampo(bitMap, mensajePureCommerce[i].ID);

        if (iRet > 0) {

            if (mensajePureCommerce[i].tipoDato == 1) {
                memcpy(longitud, buffer + indice, 2);
                tamCampo = atoi(longitud);
                indice += tamCampo + 2;
                memset(campoPrueba, 0x00, sizeof(campoPrueba));
                memcpy(campoPrueba, buffer + indiceAux + 2, tamCampo);
            } else {
                tamCampo = mensajePureCommerce[i].longitud;
                indice += tamCampo;
                memset(campoPrueba, 0x00, sizeof(campoPrueba));
                memcpy(campoPrueba, buffer + indiceAux, tamCampo);
            }

            if (mensajePureCommerce[i].ID == IDcampo) {
                memcpy(campo, campoPrueba, tamCampo);
                iRet = 1;
                break;
            }
        }
    }

    return iRet;
}
void calcularMontoMargen(uChar *monto, uChar *porcentaje, uChar *resultado, uChar *margen) {

    int i = 0;
    int j = 0;
    int punto = 0;
    long producto = 0;
    long resultado1 = 0;
    long acarreo1 = 0;
    long auxTamProducto = 0;
    long tamaMonto = 0;
    long decimales = 0;
    uChar montoAux[10 + 1];
    uChar unidadMargen[1 + 1];
    uChar porcentajeAux[9 + 1];
    uChar productosSuma[10][20 + 1];
    uChar sumaParcial[1 + 1];
    uChar tamProducto[20 + 1];
    uChar resultadoEntero[10 + 1];
    uChar resultadoDecimal[10 + 1];
    uChar resultadoJustificado[16 + 1];
    uChar redondeo[1 + 1];

    memset(montoAux, 0x00, sizeof(montoAux));
    memset(unidadMargen, 0x00, sizeof(unidadMargen));
    memset(porcentajeAux, 0x00, sizeof(porcentajeAux));
    memset(productosSuma, 0x00, sizeof(productosSuma));
    memset(sumaParcial, 0x00, sizeof(sumaParcial));
    memset(tamProducto, 0x00, sizeof(tamProducto));
    memset(resultadoEntero, 0x00, sizeof(resultadoEntero));
    memset(resultadoDecimal, 0x00, sizeof(resultadoDecimal));
    memset(resultadoJustificado, 0x00, sizeof(resultadoJustificado));
    memset(redondeo, 0x00, sizeof(redondeo));

    memcpy(montoAux, monto, 10);

    for (i = 0; i < TAM_MARGEN; i++) {
        if (porcentaje[i] == '.') {
            punto = i;
        } else if (punto == 0) {
            porcentajeAux[i] = porcentaje[i];
        } else if (isdigit((int)porcentaje[i])) {
            porcentajeAux[i - 1] = porcentaje[i];
        } else {
            porcentajeAux[i - 1] = '0';
        }
    }

    tamaMonto = strlen(montoAux);

    for (i = 0; i < 8; i++) { //!maximo del rango
        memset(unidadMargen, 0x00, sizeof(unidadMargen));
        memcpy(unidadMargen, porcentajeAux + i, 1);
        producto = atol(montoAux) * atol(unidadMargen);
        sprintf(tamProducto, "%ld", producto);
        auxTamProducto = strlen(tamProducto);
        memset(productosSuma[i], 0x30, sizeof(productosSuma[i]));
        productosSuma[i][20] = '\0';
        memcpy(productosSuma[i] + i + 1 - (auxTamProducto - tamaMonto), tamProducto, auxTamProducto);
    }

    productosSuma[i][20] = '\0';

    resultado1 = 0;

    for (i = 19; i >= 0; i--) {

        resultado1 = acarreo1;

        for (j = 0; j < 8; j++) {
            memset(sumaParcial, 0x00, sizeof(sumaParcial));
            memcpy(sumaParcial, productosSuma[j] + i, 1);
            resultado1 += atol(sumaParcial);
        }

        acarreo1 = resultado1 / 10;
        resultado1 = resultado1 % 10;

        memset(sumaParcial, 0x00, sizeof(sumaParcial));
        sprintf(sumaParcial, "%ld", resultado1);
        memcpy(productosSuma[8] + i, sumaParcial, 1);
    }

    memset(productosSuma[9], 0x00, sizeof(productosSuma[9]));

    memcpy(productosSuma[9], productosSuma[8], sizeof(productosSuma[9]));
    memcpy(resultadoEntero, productosSuma[9], 9);
    memcpy(resultadoDecimal, productosSuma[9] + 9, 9);

    if (atol(resultadoEntero) == 0) {
        memset(resultadoEntero, 0x00, sizeof(resultadoEntero));
        strcpy(resultadoEntero, "0");
    } else {
        uChar resultadoAux[12 + 1];
        memset(resultadoAux, 0x00, sizeof(resultadoAux)), leftTrim(resultadoAux, resultadoEntero, '0');
        memset(resultadoEntero, 0x00, sizeof(resultadoEntero)), memcpy(resultadoEntero, resultadoAux,
                                                                       strlen(resultadoAux));
    }

    resultadoDecimal[2] = '\0';

    resultado1 = atol(montoAux) + atol(resultadoEntero);

    memcpy(redondeo, resultadoDecimal, 1);

    decimales = atol(resultadoDecimal);
    if (redondeo[0] >= '5') { //!"5"
        decimales++;
    } else if (redondeo[0] > '0') { //!"0"
        decimales--;
    }

    memset(resultadoDecimal, 0x00, sizeof(resultadoDecimal));
    sprintf(resultadoDecimal, "%.2ld", decimales);

    /////// llenar el margen a enviar en el token DC
    uChar margenAuxiliar[9 + 1];
    memset(margenAuxiliar, 0x00, sizeof(margenAuxiliar));
    memcpy(margenAuxiliar, resultadoEntero, strlen(resultadoEntero));
    memcpy(margenAuxiliar + strlen(resultadoEntero), resultadoDecimal, 2);

    leftPad(margen, margenAuxiliar, '0', 8);

    memset(resultadoEntero, 0x00, sizeof(resultadoEntero));
   sprintf(resultadoEntero, "%ld", resultado1);

    leftPad(resultadoJustificado, resultadoEntero, '0', 14);

    memcpy(resultadoJustificado + 14, resultadoDecimal, 2);
    memcpy(resultado, resultadoJustificado, 16);
}

DatosCampo63 capturarDatoCampo63(void) {

    int indice = 0;
    char buffer[108 + 1];
    uChar tasaCambioAux[2 + 1];
    DatosCampo63 campo63;

    memset(&buffer, 0x00, sizeof(&buffer));
    memset(tasaCambioAux, 0x00, sizeof(tasaCambioAux));
    memset(&campo63, 0x00, sizeof(&campo63));

    buscarArchivo(discoNetcom, (char *)PPRO18_DCC, buffer, 0, 108);

    indice = 3;
    memcpy(campo63.Tag34, buffer + indice, 3);
    indice += 3;

    memcpy(campo63.Tag32, buffer + indice, 61);
    indice += 61;

    memcpy(campo63.Tag60, buffer + indice, 20);
    indice += 20;

    memcpy(campo63.Tag67, buffer + indice + 7, 1);
    memcpy(campo63.code, campo63.Tag32 + 28, 3);

    return campo63;
}

void obtenerFechaHora(uChar * fechaHora) {
    //Telium_Read_date(&date);
    ///sprintf(fechaHora, "%.2s%.2s%.2s%.2s%.2s", date.month, date.day, date.hour, date.minute, date.second);
}

DatosEmpaquetados armarCampo60(void) {

    int indice = 0;

    DatosEmpaquetados datos;

    memset(&datos, 0x00, sizeof(datos));

    //Telium_Read_date(&date);

    memcpy(datos.buffer, "20", 2);
    indice += 2;
    //memcpy(datos.buffer + indice, date.year, 2);
    indice += 2;
    memcpy(datos.buffer + indice, "                 ", 17);
    indice += 17;
    memcpy(datos.buffer + indice, "890", 3);
    indice += 3;
    memcpy(datos.buffer + indice, "20", 2);
    indice += 2;
    //memcpy(datos.buffer + indice, date.year, 2);
    indice += 2;
    //memcpy(datos.buffer + indice, date.month, 2);
    indice += 2;
    //memcpy(datos.buffer + indice, date.day, 2);
    indice += 2;
    sprintf(datos.buffer + indice, "%.4d", 0);
    indice += 4;

    datos.totalBytes = indice;

    return datos;
}
DatosEmpaquetados armarCampo63(void) {

    char bufferAux[14 + 1];
    int indice = 0;

    DatosEmpaquetados datos;

    memset(&bufferAux, 0x00, sizeof(&bufferAux));
    memset(&datos, 0x00, sizeof(datos));

    memcpy(bufferAux, "03", 2);
    _convertASCIIToBCD_(datos.buffer + 1, bufferAux, 2);
    indice += 2;

    memcpy(datos.buffer + indice, "12", 2);
    indice += 2;

    memcpy(datos.buffer + indice, "D", 1);
    indice += 1;

    memset(&bufferAux, 0x00, sizeof(&bufferAux));
    memcpy(bufferAux, "03", 2);
    _convertASCIIToBCD_(datos.buffer + indice + 1, bufferAux, 2);
    indice += 2;

    memcpy(datos.buffer + indice, "27", 2);
    indice += 2;

    memcpy(datos.buffer + indice, "R", 1);
    indice += 1;

    memset(&bufferAux, 0x00, sizeof(&bufferAux));
    memcpy(bufferAux, "04", 2);
    _convertASCIIToBCD_(datos.buffer + indice + 1, bufferAux, 2);
    indice += 2;

    memcpy(datos.buffer + indice, "33", 2);
    indice += 2;

    memset(&bufferAux, 0x00, sizeof(&bufferAux));
    memcpy(bufferAux, "E101", 4);
    _convertASCIIToBCD_(datos.buffer + indice, bufferAux, 4);
    indice += 2;

    datos.totalBytes = indice;

    return datos;
}

int verificacionTramaRecibida(uChar *systemTrace) {

    int resultado = 0;
    int validacion = 1;
    uChar stringAux[20];
    ISOFieldMessage isoFieldMessage;

    uChar txnTime[TAM_TIME + 1];
    uChar txnDate[TAM_DATE + 1];

    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(stringAux, 0x00, sizeof(stringAux));

    memset(txnTime, 0x00, sizeof(txnTime));
    memset(txnDate, 0x00, sizeof(txnDate));

    isoFieldMessage = getField(12);
    memcpy(txnTime, isoFieldMessage.valueField, 6);
    isoFieldMessage = getField(13);
    memcpy(txnDate, isoFieldMessage.valueField, 4);

    if (txnTime[0] != 0x00) {

        isoFieldMessage = getField(11);
        validacion = strncmp(isoFieldMessage.valueField, systemTrace, isoFieldMessage.totalBytes);

        if (validacion == 0) {
            resultado = 1;
        } else {
            resultado = -1;
        }


    }


    return resultado;
}

int preguntarUsoDcc(uChar *campo61, DatosVenta *datosVenta) {

    int iRet = 0;
    int confirmacion = 0;
    int salida = 0;
    int decimalesMoneda = 0;
    int montoAux = 0;

    uChar campo6[124 + 1];
    uChar campo6Aux[124 + 1];
    uChar campo18[124 + 1];
    uChar campo29[3 + 1];
    uChar cantidadDecimales[1 + 1];
    uChar linea1[30 + 1];
    uChar linea2[30 + 1];
    uChar linea3[30 + 1];
    uChar stringAux[20 + 1];
    uChar montoTransaccionAux[30 + 1];
    uChar tasaCambio[30 + 1];
    uChar tasaCambioAux[30 + 1];
    uChar monedaConvertida[30 + 1];
    uChar monedaConvertidaAux[30 + 1];
    uChar decimalesMonedaConvertida[30 + 1];
    uChar simbolo[30 + 1];
    uChar stringAux2[20 + 1];

    memset(campo6, 0x00, sizeof(campo6));
    memset(campo18, 0x00, sizeof(campo18));
    memset(cantidadDecimales, 0x00, sizeof(cantidadDecimales));
    memset(linea1, 0x00, sizeof(linea1));
    memset(linea2, 0x00, sizeof(linea2));
    memset(linea3, 0x00, sizeof(linea3));
    memset(stringAux, 0x00, sizeof(stringAux));
    memset(montoTransaccionAux, 0x00, sizeof(montoTransaccionAux));
    memset(tasaCambio, 0x00, sizeof(tasaCambio));
    memset(tasaCambioAux, 0x00, sizeof(tasaCambioAux));
    memset(monedaConvertida, 0x00, sizeof(monedaConvertida));
    memset(monedaConvertidaAux, 0x00, sizeof(monedaConvertidaAux));
    memset(decimalesMonedaConvertida, 0x00, sizeof(decimalesMonedaConvertida));
    memset(simbolo, 0x00, sizeof(simbolo));
    memset(campo29, 0x00, sizeof(campo29));
    memset(stringAux2, 0x00, sizeof(stringAux2));

    extraerCampo(6, campo61, campo6);
    memcpy(campo6Aux, campo6 + 3, 12);

    extraerCampo(18, campo61, campo18); //tasa de cambio 5 1994 045780
    memcpy(cantidadDecimales, campo18, 1);
    memcpy(tasaCambio, campo18 + 1, 9 - atoi(cantidadDecimales));
    leftTrim(tasaCambioAux, tasaCambio, '0');
    extraerCampo(29, campo61, campo29);

    decimalesMoneda = definirSimboloDecimales(atoi(campo29), simbolo);

    memset(tasaCambio, 0x00, sizeof(tasaCambio));
    formatString(0x00, 0, tasaCambioAux, strlen(tasaCambioAux), tasaCambio, 1);

    if ((9 - atoi(cantidadDecimales)) < 9) {
        memset(tasaCambioAux, 0x00, sizeof(tasaCambioAux));
        memcpy(tasaCambioAux, campo18 + 10 - atoi(cantidadDecimales), atoi(cantidadDecimales));
        strcat(tasaCambio, ",");
        if (atoi(cantidadDecimales) > 2) {
            strncat(tasaCambio, tasaCambioAux, 2);
        } else {
            strcat(tasaCambio, tasaCambioAux);
        }
    }

    memset(cantidadDecimales, 0x00, sizeof(cantidadDecimales));
    memcpy(cantidadDecimales, campo6 + 15, 1);

    memcpy(monedaConvertida, campo6 + 3, 12 - atoi(cantidadDecimales));
    memcpy(decimalesMonedaConvertida, campo6 + 3 + 12 - atoi(cantidadDecimales), atoi(cantidadDecimales));
    leftTrim(monedaConvertidaAux, monedaConvertida, '0');

    if (strlen(monedaConvertidaAux) == 0) {
        strcpy(monedaConvertida, "$ 0");
    } else {
        memset(monedaConvertida, 0x00, sizeof(monedaConvertida));
        formatString(0x00, 0, monedaConvertidaAux, strlen(monedaConvertidaAux), monedaConvertida, 1);
    }

    if (atoi(decimalesMonedaConvertida) > 0) {
        strcat(monedaConvertida, ",");
    }

    if (strlen(decimalesMonedaConvertida) > decimalesMoneda) {
        strncat(monedaConvertida, decimalesMonedaConvertida, decimalesMoneda);
    } else {
        strcat(monedaConvertida, decimalesMonedaConvertida);
    }

    if (strcmp(datosVenta->posEntrymode, "051") == 0) {
        montoAux = atoi(datosVenta->totalVenta) + atoi(datosVenta->propina);
    } else {
        montoAux = atoi(datosVenta->totalVenta);
    }
    sprintf(stringAux2, "%d", montoAux);
    leftTrim(stringAux, stringAux2, '0');

    if (strlen(stringAux) == 0) {
        strcpy(montoTransaccionAux, "$ 0");
    } else {
        formatString(0x00, 0, stringAux, strlen(stringAux), montoTransaccionAux, 1);
    }

    sprintf(linea1, "C. NETA: %s", montoTransaccionAux);
    sprintf(linea2, "T. CAMBIO: %s", tasaCambio);
    sprintf(linea3, "C. MON:%s ", " ");
    strcat(simbolo, " ");
    strcat(simbolo, monedaConvertida + 2);
    strcat(linea3, simbolo);

    memcpy(datosVenta->acquirerId, tasaCambio, strlen(tasaCambio));
    memcpy(datosVenta->nombreComercio, simbolo, strlen(simbolo));

    //screenMessageFiveLine("VERIFICACION DCC", linea1, linea2, linea3, "REALIZAR TRANS. DCC?", "Si(Verde)            No(Rojo)", 1 * 1000);

    /*
    do {
        confirmacion = waitKey(30 * 100);

        switch (confirmacion) {
            case T_VAL:
                iRet = IS_TRANSACCION_DCC;
                memcpy(datosVenta->aux1 + 48, "2", 1);
                salida = 1;
                break;
            case T_ANN:
            case TIMEOUT:
                iRet = -1;
                salida = 1;
                memcpy(datosVenta->aux1 + 48, "0", 1);
                break;
        }
    } while (salida != 1);*/

    return iRet;

}

DatosEmpaquetados armarCampo61(uChar *totalVenta, uChar *nombreTarjeta, uChar *track2, uChar *codigoPais) {

    int indice = 0;
    long cantidadCeros = 0;
    uChar bitmap[8 + 1];
    uChar longitud[3 + 1];
    uChar temporal[24 + 1];
    uChar datoHexa[10 + 1];
    uChar datoHexaAuxiliar[10 + 1];
    uChar ceros[25 + 1];
    uChar tamPan[2 + 1];
    DatosEmpaquetados datos;

    memset(bitmap, 0x00, sizeof(bitmap));
    memset(longitud, 0x00, sizeof(longitud));
    memset(temporal, 0x00, sizeof(temporal));
    memset(datoHexa, 0x00, sizeof(datoHexa));
    memset(datoHexaAuxiliar, 0x00, sizeof(datoHexaAuxiliar));
    memset(ceros, 0x00, sizeof(ceros));
    memset(tamPan, 0x00, sizeof(tamPan));
    memset(&datos, 0x00, sizeof(datos));

    cantidadCeros = strcspn(track2, "D");
    sprintf(tamPan, "%ld", cantidadCeros);
    cantidadCeros -= 10;
    memcpy(datos.buffer, "71", 2);
    indice += 2;

    if (strncmp(codigoPais, NO_CODIGO, strlen(NO_CODIGO)) == 0) {
        memcpy(bitmap, "fa028400", 8);
    } else {
        memcpy(bitmap, "fa028408", 8);
    }

    indice += 4; //espacio para el tamaño del buffer
    memcpy(datos.buffer + indice, bitmap, 8);
    indice += 8;

    /**SUBDATOS.BUFFER*/
    memcpy(datos.buffer + indice, bitmap, 4);
    indice += 4;
    memcpy(datos.buffer + indice, "10", 2);
    indice += 2;

    memset(temporal, 0x00, sizeof(temporal));
    obtenerCodigoUnico(temporal);
//	memset(temporal, 0x00, sizeof(temporal));
//	strcpy(temporal, "0012552378");
    memcpy(datos.buffer + indice, temporal, 10);/**Merchan ID(codigo unico)*/
    indice += 10;

    memset(temporal, 0x00, sizeof(temporal));
    getParameter(NUMERO_TERMINAL, temporal);
    sprintf(datos.buffer + indice, "%.8s        ", temporal); //! corregido terminal a la izquierda no ceros sino con espacios a la derecha
    indice += 16;
    memcpy(datos.buffer + indice, "11", 2);
    indice += 2;
    sprintf(datos.buffer + indice, "000000%.5s", "14899");/** adquirer ID RBM */
    indice += 11;
    memcpy(datos.buffer + indice, "170", 3);
    indice += 3;
    leftPad(temporal, totalVenta, '0', 10);
    strcat(temporal, "00");
    memcpy(datos.buffer + indice, temporal, 12); //monto total
    indice += 12;
    memcpy(datos.buffer + indice, "2", 1); //cantidad de decimales
    indice += 1;
    memcpy(datos.buffer + indice, tamPan, 2); //longitud
    indice += 2;
    memcpy(datos.buffer + indice, track2, 10); //diez primeros del pan
    indice += 10;
    leftPad(ceros, (char *)"0", '0', cantidadCeros);
    memcpy(datos.buffer + indice, ceros, cantidadCeros); //rellenar ceros a la derecha hasta el tamaño del pan
    indice += cantidadCeros;

    seleccionarTarjeta(nombreTarjeta, temporal); //nombre de la tarjeta
    memcpy(datos.buffer + indice, temporal, 3);
    indice += 3;

    /**SUBDATOS.BUFFER*/
    memcpy(datos.buffer + indice, bitmap + 4, 2);
    indice += 2;
    sprintf(datos.buffer + indice, "%.6d", 0);
    indice += 6;

    /**SUBDATOS.BUFFER*/
    if (strncmp(codigoPais, NO_CODIGO, strlen(NO_CODIGO)) != 0) {
        memcpy(datos.buffer + indice, bitmap + 6, 2);
        indice += 2;
        memcpy(datos.buffer + indice, codigoPais + 1, 3); //! el tag 9F42
        indice += 3;
        datos.totalBytes = indice;
    } else {
        memcpy(datos.buffer + indice, bitmap + 6, 2);
        indice += 2;
        datos.totalBytes = indice;
    }

    convertLongToHex(datos.totalBytes - 14, datoHexa);
    leftPad(datoHexaAuxiliar, datoHexa, '0', 4);

    ////////////

    memcpy(datos.buffer + 2, datoHexaAuxiliar, 4);

    return datos;
}

int preguntarDccPPRO(DatosVenta *datosVenta) {

    int iRet = 0;
    int confirmacion = 0;
    int salida = 0;
    int decimalesMoneda = 0;
    int montoAux = 0;
    int indice = 0;
    DatosCampo63 campo63;
    uChar stringAux[20 + 1];
    uChar stringAux2[20 + 1];
    uChar linea1[30 + 1];
    uChar linea2[30 + 1];
    uChar linea3[30 + 1];
    uChar linea4[30 + 1];
    uChar linea5[30 + 1];
    uChar montoTransaccionAux[30 + 1];
    uChar tasaCambio[30 + 1];
    uChar monedaConvertida[12 + 1];
    uChar monedaConvertidaAux[12 + 1];
    uChar monedaDecimales[11 + 1];
    uChar monedaDecimalesAuxiliar[15 + 1];
    uChar simbolo[12 + 1];
    uChar tasaCambioAux[30 + 1];
    uChar tasaCambioAux2[30 + 1];
    uChar cantidadDecimales[1 + 1];
    uChar decimalesAuxiliar[1 + 1];
    uChar comision_A[3 + 1];
    uChar comision_B[3 + 1];
    uChar comision[1 + 1];
    uChar auxiliar[3 + 1];

    memset(&campo63, 0x00, sizeof(&campo63));
    memset(stringAux, 0x00, sizeof(stringAux));
    memset(linea1, 0x00, sizeof(linea1));
    memset(linea2, 0x00, sizeof(linea2));
    memset(linea3, 0x00, sizeof(linea3));
    memset(linea4, 0x00, sizeof(linea4));
    memset(montoTransaccionAux, 0x00, sizeof(montoTransaccionAux));
    memset(tasaCambio, 0x00, sizeof(tasaCambio));
    memset(simbolo, 0x00, sizeof(simbolo));
    memset(tasaCambioAux, 0x00, sizeof(tasaCambioAux));
    memset(tasaCambioAux2, 0x00, sizeof(tasaCambioAux2));
    memset(monedaConvertida, 0x00, sizeof(monedaConvertida));
    memset(monedaConvertidaAux, 0x00, sizeof(monedaConvertidaAux));
    memset(cantidadDecimales, 0x00, sizeof(cantidadDecimales));
    memset(stringAux2, 0x00, sizeof(stringAux2));
    memset(decimalesAuxiliar, 0x00, sizeof(decimalesAuxiliar));
    memset(monedaDecimales, 0x00, sizeof(monedaDecimales));
    memset(monedaDecimalesAuxiliar, 0x00, sizeof(monedaDecimalesAuxiliar));
    memset(comision_A, 0x00, sizeof(comision_A));
    memset(comision_B, 0x00, sizeof(comision_B));
    memset(comision, 0x00, sizeof(comision));
    memset(auxiliar, 0x00, sizeof(auxiliar));


    campo63 = capturarDatoCampo63();

    _convertBCDToASCII_(decimalesAuxiliar, campo63.Tag32 + 32, 2);


    decimalesMoneda = atoi(decimalesAuxiliar);

    _convertBCDToASCII_(comision_A, campo63.Tag34 + 1, 2);
    _convertBCDToASCII_(comision_B, campo63.Tag34 + 2, 2);

    leftTrim(auxiliar,comision_A,'0');
    memset(comision_A, 0x00, sizeof(comision_A));
    memcpy(comision_A, auxiliar, 4);

    strcat(comision_A,",");
    strcat(comision_A,comision_B);
    strcat(comision, comision_A);
    strcat(comision, "%");

    memcpy(datosVenta->aux1 + 40, comision, 5);
    memcpy(simbolo, campo63.Tag32 + 28, 3);
    memcpy(campo63.code, simbolo, 4);

    memcpy(cantidadDecimales, datosVenta->field61, 1);
    memcpy(tasaCambio, datosVenta->field61 + 1, 8);

    if (atoi(cantidadDecimales) <= 7) {
        memcpy(tasaCambioAux, tasaCambio, 7 - atoi(cantidadDecimales));
        memcpy(tasaCambioAux2, datosVenta->field61 + 8 - atoi(cantidadDecimales), atoi(cantidadDecimales));
        if (atoi(cantidadDecimales) == 7) {
            strcat(tasaCambioAux, "0");
        }
        strcat(tasaCambioAux, ",");
        strcat(tasaCambioAux, tasaCambioAux2);
    }

    if (strcmp(datosVenta->posEntrymode, "051") == 0) {
        montoAux = atoi(datosVenta->totalVenta) + atoi(datosVenta->propina);
    } else {
        montoAux = atoi(datosVenta->totalVenta);
    }
    sprintf(stringAux2, "%d", montoAux);
    leftTrim(stringAux, stringAux2, '0');
    if (strlen(stringAux) == 0) {
        strcpy(montoTransaccionAux, "0");
    } else {
        formatString(0x00, 0, stringAux, strlen(stringAux), montoTransaccionAux, 2);
    }

    _convertBCDToASCII_(monedaDecimales, campo63.Tag60 + 10, sizeof(monedaDecimales));
    leftTrim(monedaDecimalesAuxiliar, monedaDecimales, '0');
    if (decimalesMoneda > 0) {
        indice = strlen(monedaDecimalesAuxiliar) - decimalesMoneda;
        memcpy(monedaConvertida, datosVenta->textoInformacion + 6, TAM_COMPRA_NETA - decimalesMoneda);
        if(atoi(monedaConvertida) <= 0){
            memcpy(monedaDecimalesAuxiliar, monedaDecimales + 10, decimalesMoneda);
            indice = 0;
            strcat(monedaConvertida, "$ 0");
        }
        strcat(monedaConvertida, ",");
        strcat(monedaConvertida, monedaDecimalesAuxiliar + indice);
    } else {
        memcpy(monedaConvertida, datosVenta->textoInformacion + 6, TAM_COMPRA_NETA);
    }
    leftTrim(monedaConvertidaAux, monedaConvertida, '0');
    memset(stringAux, 0x00, sizeof(stringAux));
    if(indice <= 0){
        leftTrim(stringAux, monedaConvertidaAux, '$');
    } else {
        memcpy(stringAux, monedaConvertidaAux, sizeof(monedaConvertidaAux));
    }
    memset(monedaConvertida, 0x00, sizeof(monedaConvertida));
    formatString(0x00, 0, stringAux, strlen(monedaConvertidaAux), monedaConvertida, 0);
    uChar simboloAux[12 + 1];
    memset(simboloAux, 0x00, sizeof(simboloAux));
    memcpy(simboloAux, simbolo, sizeof(simbolo));

    sprintf(linea2, "1 COP/%s", " ");
    strcat(tasaCambioAux, " ");
    strcat(tasaCambioAux, simboloAux);
    strcat(linea2, tasaCambioAux);

    sprintf(linea3, "1. %s ", " ");
    strcat(simbolo, " ");
    strcat(simbolo, monedaConvertida);
    strcat(linea3, simbolo);
    sprintf(linea4, "2. COP %s", montoTransaccionAux);

    if (strncmp(datosVenta->cardName, "VISA", 4) == 0) {
        sprintf(linea5, "INCLUYE COMISION:%s", comision);
    } else {
        strcat(linea5, "   ");
    }
    memcpy(datosVenta->acquirerId, tasaCambioAux, strlen(tasaCambioAux));
    memcpy(datosVenta->nombreComercio, simbolo, strlen(simbolo));

   // screenMessage("MENSAJE", "PASE EL", "TERMINAL", "AL CLIENTE", 5 * 1000);

    //screenMessageFiveLine("SELECCIONA TU MONEDA", "Tasa De Cambio:", linea2, linea3, linea4, linea5, 1 * 1000);

   /* do {
        confirmacion = waitKey(30 * 100);

        switch (confirmacion) {
            case 23:
                iRet = -2;
                salida = 1;
                memcpy(datosVenta->isQPS, "4", 1);
                break;
            case 22:
            case 49:
                iRet = IS_TRANSACCION_DCC;
                memcpy(datosVenta->isQPS, "2", 1);
                salida = 1;
                break;
            case 50:
            case 0:
                iRet = -1;
                salida = 1;
                memcpy(datosVenta->isQPS, "3", 1);
                break;
        }
    } while (salida != 1);*/

    return iRet;

}
int definirExistenciaCampo(uChar *bitMap, int idCampo) {

    uChar bitmapInterno[32 + 1];
    uChar bitmapAux[32 + 1];
    int tam = 0;
    int entAux = 0;
    int i = 0;
    int j = 0;
    int iRet = 0;

    memset(bitmapInterno, 0x00, sizeof(bitmapInterno));
    memset(bitmapAux, 0x00, sizeof(bitmapAux));

    tam = strlen(bitMap);

    for (i = 0; i < tam; i++) {

        entAux = definirEntero(bitMap[i]);

        for (j = 0; j <= 3; j++) {
            bitmapAux[3 - j] = entAux % 2;
            entAux /= 2;
            // 4 ascci para representar un hexa (inicialmente en ascii)
        }

        memcpy(bitmapInterno + (4 * i) + 1, bitmapAux, 4);
    }

    if (bitmapInterno[idCampo] == 1 || idCampo == 0 || idCampo == 25) {
        iRet = 1;
    }

    return iRet;
}
int definirSimboloDecimales(int codigo, uChar *simbolo) {
    int iRet = 0;
    switch(codigo){
        case 8:     strcpy(simbolo, "ALL"); iRet = 2; break;
        case 12:    strcpy(simbolo, "DZD"); iRet = 2; break;
        case 32:    strcpy(simbolo, "ARS"); iRet = 2; break;
        case 36:    strcpy(simbolo, "AUD"); iRet = 2; break;
        case 44:    strcpy(simbolo, "BSD"); iRet = 2; break;
        case 48:    strcpy(simbolo, "BHD"); iRet = 3; break;
        case 50:    strcpy(simbolo, "BDT"); iRet = 2; break;
        case 51:    strcpy(simbolo, "AMD"); iRet = 2; break;
        case 52:    strcpy(simbolo, "BBD"); iRet = 2; break;
        case 60:    strcpy(simbolo, "BMD"); iRet = 2; break;
        case 64:    strcpy(simbolo, "BTN"); iRet = 2; break;
        case 68:    strcpy(simbolo, "BOB"); iRet = 2; break;
        case 72:    strcpy(simbolo, "BWP"); iRet = 2; break;
        case 84:    strcpy(simbolo, "BZD"); iRet = 2; break;
        case 90:    strcpy(simbolo, "SBD"); iRet = 2; break;
        case 96:    strcpy(simbolo, "BND"); iRet = 2; break;
        case 104:   strcpy(simbolo, "MMK"); iRet = 2; break;
        case 108:   strcpy(simbolo, "BIF"); iRet = 0; break;
        case 116:   strcpy(simbolo, "KHR"); iRet = 2; break;
        case 124:   strcpy(simbolo, "CAD"); iRet = 2; break;
        case 132:   strcpy(simbolo, "CVE"); iRet = 2; break;
        case 136:   strcpy(simbolo, "KYD"); iRet = 2; break;
        case 144:   strcpy(simbolo, "LKR"); iRet = 2; break;
        case 152:   strcpy(simbolo, "CLP"); iRet = 2; break;
        case 156:   strcpy(simbolo, "CNY"); iRet = 2; break;
        case 170:   strcpy(simbolo, "COP"); iRet = 2; break;
        case 174:   strcpy(simbolo, "KMF"); iRet = 0; break;
        case 188:   strcpy(simbolo, "CRC"); iRet = 2; break;
        case 191:   strcpy(simbolo, "HRK"); iRet = 2; break;
        case 192:   strcpy(simbolo, "CUP"); iRet = 2; break;
        case 203:   strcpy(simbolo, "CZK"); iRet = 2; break;
        case 208:   strcpy(simbolo, "DKK"); iRet = 2; break;
        case 214:   strcpy(simbolo, "DOP"); iRet = 2; break;
        case 230:   strcpy(simbolo, "ETB"); iRet = 2; break;
        case 232:   strcpy(simbolo, "ERN"); iRet = 2; break;
        case 238:   strcpy(simbolo, "FKP"); iRet = 2; break;
        case 242:   strcpy(simbolo, "FJD"); iRet = 2; break;
        case 262:   strcpy(simbolo, "DJF"); iRet = 0; break;
        case 270:   strcpy(simbolo, "GMD"); iRet = 2; break;
        case 292:   strcpy(simbolo, "GIP"); iRet = 2; break;
        case 320:   strcpy(simbolo, "GTQ"); iRet = 2; break;
        case 324:   strcpy(simbolo, "GNF"); iRet = 0; break;
        case 328:   strcpy(simbolo, "GYD"); iRet = 2; break;
        case 332:   strcpy(simbolo, "HTG"); iRet = 2; break;
        case 340:   strcpy(simbolo, "HNL"); iRet = 2; break;
        case 344:   strcpy(simbolo, "HKD"); iRet = 2; break;
        case 348:   strcpy(simbolo, "HUF"); iRet = 2; break;
        case 352:   strcpy(simbolo, "ISK"); iRet = 2; break;
        case 356:   strcpy(simbolo, "INR"); iRet = 2; break;
        case 360:   strcpy(simbolo, "IDR"); iRet = 2; break;
        case 364:   strcpy(simbolo, "IRR"); iRet = 2; break;
        case 368:   strcpy(simbolo, "IQD"); iRet = 3; break;
        case 376:   strcpy(simbolo, "ILS"); iRet = 2; break;
        case 388:   strcpy(simbolo, "JMD"); iRet = 2; break;
        case 392:   strcpy(simbolo, "JPY"); iRet = 0; break;
        case 398:   strcpy(simbolo, "KZT"); iRet = 2; break;
        case 400:   strcpy(simbolo, "JOD"); iRet = 3; break;
        case 404:   strcpy(simbolo, "KES"); iRet = 2; break;
        case 408:   strcpy(simbolo, "KPW"); iRet = 2; break;
        case 410:   strcpy(simbolo, "KRW"); iRet = 0; break;
        case 414:   strcpy(simbolo, "KWD"); iRet = 3; break;
        case 417:   strcpy(simbolo, "KGS"); iRet = 2; break;
        case 418:   strcpy(simbolo, "LAK"); iRet = 2; break;
        case 422:   strcpy(simbolo, "LBP"); iRet = 2; break;
        case 430:   strcpy(simbolo, "LRD"); iRet = 2; break;
        case 434:   strcpy(simbolo, "LYD"); iRet = 3; break;
        case 440:   strcpy(simbolo, "LTL"); iRet = 2; break;
        case 446:   strcpy(simbolo, "MOP"); iRet = 2; break;
        case 454:   strcpy(simbolo, "MWK"); iRet = 2; break;
        case 458:   strcpy(simbolo, "MYR"); iRet = 2; break;
        case 462:   strcpy(simbolo, "MVR"); iRet = 2; break;
        case 478:   strcpy(simbolo, "MRO"); iRet = 2; break;
        case 480:   strcpy(simbolo, "MUR"); iRet = 2; break;
        case 484:   strcpy(simbolo, "MXN"); iRet = 2; break;
        case 496:   strcpy(simbolo, "MNT"); iRet = 2; break;
        case 498:   strcpy(simbolo, "MDL"); iRet = 2; break;
        case 504:   strcpy(simbolo, "MAD"); iRet = 2; break;
        case 512:   strcpy(simbolo, "OMR"); iRet = 3; break;
        case 516:   strcpy(simbolo, "NAD"); iRet = 2; break;
        case 524:   strcpy(simbolo, "NPR"); iRet = 2; break;
        case 532:   strcpy(simbolo, "ANG"); iRet = 2; break;
        case 533:   strcpy(simbolo, "AWG"); iRet = 2; break;
        case 548:   strcpy(simbolo, "VUV"); iRet = 0; break;
        case 554:   strcpy(simbolo, "NZD"); iRet = 2; break;
        case 558:   strcpy(simbolo, "NIO"); iRet = 2; break;
        case 566:   strcpy(simbolo, "NGN"); iRet = 2; break;
        case 578:   strcpy(simbolo, "NOK"); iRet = 2; break;
        case 586:   strcpy(simbolo, "PKR"); iRet = 2; break;
        case 590:   strcpy(simbolo, "PAB"); iRet = 2; break;
        case 598:   strcpy(simbolo, "PGK"); iRet = 2; break;
        case 600:   strcpy(simbolo, "PYG"); iRet = 0; break;
        case 604:   strcpy(simbolo, "PEN"); iRet = 2; break;
        case 608:   strcpy(simbolo, "PHP"); iRet = 2; break;
        case 634:   strcpy(simbolo, "QAR"); iRet = 2; break;
        case 643:   strcpy(simbolo, "RUB"); iRet = 2; break;
        case 646:   strcpy(simbolo, "RWF"); iRet = 0; break;
        case 654:   strcpy(simbolo, "SHP"); iRet = 2; break;
        case 678:   strcpy(simbolo, "STD"); iRet = 2; break;
        case 682:   strcpy(simbolo, "SAR"); iRet = 2; break;
        case 690:   strcpy(simbolo, "SCR"); iRet = 2; break;
        case 694:   strcpy(simbolo, "SLL"); iRet = 2; break;
        case 702:   strcpy(simbolo, "SGD"); iRet = 2; break;
        case 704:   strcpy(simbolo, "VND"); iRet = 0; break;
        case 706:   strcpy(simbolo, "SOS"); iRet = 2; break;
        case 710:   strcpy(simbolo, "ZAR"); iRet = 2; break;
        case 728:   strcpy(simbolo, "SSP"); iRet = 2; break;
        case 748:   strcpy(simbolo, "SZL"); iRet = 2; break;
        case 756:   strcpy(simbolo, "CHF"); iRet = 2; break;
        case 760:   strcpy(simbolo, "SYP"); iRet = 2; break;
        case 764:   strcpy(simbolo, "THB"); iRet = 2; break;
        case 776:   strcpy(simbolo, "TOP"); iRet = 2; break;
        case 780:   strcpy(simbolo, "TTD"); iRet = 2; break;
        case 784:   strcpy(simbolo, "AED"); iRet = 2; break;
        case 788:   strcpy(simbolo, "TND"); iRet = 3; break;
        case 800:   strcpy(simbolo, "UGX"); iRet = 2; break;
        case 807:   strcpy(simbolo, "MKD"); iRet = 2; break;
        case 818:   strcpy(simbolo, "EGP"); iRet = 2; break;
        case 826:   strcpy(simbolo, "GBP"); iRet = 2; break;
        case 834:   strcpy(simbolo, "TZS"); iRet = 2; break;
        case 840:   strcpy(simbolo, "USD"); iRet = 2; break;
        case 858:   strcpy(simbolo, "UYU"); iRet = 2; break;
        case 860:   strcpy(simbolo, "UZS"); iRet = 2; break;
        case 882:   strcpy(simbolo, "WST"); iRet = 2; break;
        case 886:   strcpy(simbolo, "YER"); iRet = 2; break;
        case 901:   strcpy(simbolo, "TWD"); iRet = 2; break;
        case 934:   strcpy(simbolo, "TMT"); iRet = 2; break;
        case 936:   strcpy(simbolo, "GHS"); iRet = 2; break;
        case 937:   strcpy(simbolo, "VEF"); iRet = 2; break;
        case 938:   strcpy(simbolo, "SDG"); iRet = 2; break;
        case 941:   strcpy(simbolo, "RSD"); iRet = 2; break;
        case 943:   strcpy(simbolo, "MZN"); iRet = 2; break;
        case 944:   strcpy(simbolo, "AZM"); iRet = 2; break;
        case 946:   strcpy(simbolo, "RON"); iRet = 2; break;
        case 949:   strcpy(simbolo, "TRY"); iRet = 2; break;
        case 950:   strcpy(simbolo, "XAF"); iRet = 0; break;
        case 951:   strcpy(simbolo, "XCD"); iRet = 2; break;
        case 952:   strcpy(simbolo, "XOF"); iRet = 0; break;
        case 953:   strcpy(simbolo, "XPF"); iRet = 0; break;
        case 955:   strcpy(simbolo, "XBA"); iRet = 0; break;
        case 956:   strcpy(simbolo, "XBB"); iRet = 0; break;
        case 957:   strcpy(simbolo, "XBC"); iRet = 0; break;
        case 958:   strcpy(simbolo, "XBD"); iRet = 0; break;
        case 959:   strcpy(simbolo, "XAU"); iRet = 0; break;
        case 960:   strcpy(simbolo, "XDR"); iRet = 0; break;
        case 961:   strcpy(simbolo, "XAG"); iRet = 0; break;
        case 962:   strcpy(simbolo, "XPT"); iRet = 0; break;
        case 963:   strcpy(simbolo, "XTS"); iRet = 0; break;
        case 964:   strcpy(simbolo, "XPD"); iRet = 0; break;
        case 967:   strcpy(simbolo, "ZMW"); iRet = 2; break;
        case 968:   strcpy(simbolo, "SRD"); iRet = 2; break;
        case 969:   strcpy(simbolo, "MGA"); iRet = 2; break;
        case 971:   strcpy(simbolo, "AFA"); iRet = 2; break;
        case 972:   strcpy(simbolo, "TJS"); iRet = 2; break;
        case 973:   strcpy(simbolo, "AOA"); iRet = 2; break;
        case 974:   strcpy(simbolo, "BYR"); iRet = 0; break;
        case 975:   strcpy(simbolo, "BGN"); iRet = 2; break;
        case 976:   strcpy(simbolo, "CDF"); iRet = 2; break;
        case 977:   strcpy(simbolo, "BAM"); iRet = 2; break;
        case 978:   strcpy(simbolo, "EUR"); iRet = 2; break;
        case 980:   strcpy(simbolo, "UAH"); iRet = 2; break;
        case 981:   strcpy(simbolo, "GEL"); iRet = 2; break;
        case 985:   strcpy(simbolo, "PLN"); iRet = 2; break;
        case 986:   strcpy(simbolo, "BRL"); iRet = 2; break;
        case 990:   strcpy(simbolo, "CLF"); iRet = 0; break;
        case 999:   strcpy(simbolo, "XXX"); iRet = 0; break;
    }
    return iRet;
}

void obtenerCodigoUnico(uChar *codigoUnico) {

    TablaUnoInicializacion tablaUno;

    memset(&tablaUno, 0x00, sizeof(tablaUno));

    tablaUno = _desempaquetarTablaCeroUno_();

    memcpy(codigoUnico, tablaUno.receiptLine3, 10);
}

void seleccionarTarjeta(uChar * tipoTarjeta, uChar *type) {

    uChar tipoTarjetaAux[20 + 1];

    strcpy(tipoTarjetaAux, tipoTarjeta);
    convertirMayusculas((uChar*)tipoTarjetaAux);

    if (strncmp(tipoTarjetaAux, "VISA", 4) == 0) {
        strcpy(type, "VSA");
    } else if (strncmp(tipoTarjetaAux, "MASTERCARD", 10) == 0) {
        strcpy(type, "MCD");
    } else if (strncmp(tipoTarjetaAux, "DINERS", 6) == 0) {
        strcpy(type, "DIN");
    } else if (strncmp(tipoTarjetaAux, "AMEX", 4) == 0) {
        strcpy(type, "AMX");
    } else if (strncmp(tipoTarjetaAux, "JCB", 3) == 0) {
        strcpy(type, "JCB");
    } else {
        strcpy(type, "OTH");
    }
}

void convertLongToHex(long numero, uChar * hexa) {

    int resultadoHexa[10 + 1];
    uChar hexaAuxiliar[10 + 1];
    uChar caracterHexa[10 + 1];
    int i = 0;
    int resultado = 0;

    memset(hexaAuxiliar, 0x00, sizeof(hexaAuxiliar));
    memset(caracterHexa, 0x00, sizeof(caracterHexa));
    memset(resultadoHexa, 0x00, sizeof(resultadoHexa));
    memset(hexa, 0x00, 10 + 1);

    resultado = numero;

    sprintf(hexaAuxiliar, "%ld", numero);
    memset(hexaAuxiliar, 0x00, sizeof(hexaAuxiliar));

    do {
        resultadoHexa[i] = resultado % 16;
        resultado /= 16;
        asignarHexa(resultadoHexa[i], caracterHexa);
        strcat(hexaAuxiliar, caracterHexa);
    } while (resultado != 0);

    memset(caracterHexa, 0x00, sizeof(caracterHexa));

    for (i = 0; i < strlen(hexaAuxiliar); i++) {
        caracterHexa[i] = hexaAuxiliar[strlen(hexaAuxiliar) - 1 - i];
    }

    memcpy(hexa, caracterHexa, strlen(hexaAuxiliar));
    hexa[strlen(hexaAuxiliar)] = 0x00;
}

int definirEntero(uChar hexa) {

    int decimal = 0;

    switch (hexa) {

        case '0':
            decimal = 0;
            break;
        case '1':
            decimal = 1;
            break;
        case '2':
            decimal = 2;
            break;
        case '3':
            decimal = 3;
            break;
        case '4':
            decimal = 4;
            break;
        case '5':
            decimal = 5;
            break;
        case '6':
            decimal = 6;
            break;
        case '7':
            decimal = 7;
            break;
        case '8':
            decimal = 8;
            break;
        case '9':
            decimal = 9;
            break;
        case 'A':
        case 'a':
            decimal = 10;
            break;
        case 'B':
        case 'b':
            decimal = 11;
            break;
        case 'C':
        case 'c':
            decimal = 12;
            break;
        case 'D':
        case 'd':
            decimal = 13;
            break;
        case 'E':
        case 'e':
            decimal = 14;
            break;
        case 'F':
        case 'f':
            decimal = 15;
            break;
        default:
            break;
    }

    return decimal;
}

void asignarHexa(int entero, uChar *hexa) {

    switch (entero) {
        case 0:
            memcpy(hexa, "0", 1);
            break;
        case 1:
            memcpy(hexa, "1", 1);
            break;
        case 2:
            memcpy(hexa, "2", 1);
            break;
        case 3:
            memcpy(hexa, "3", 1);
            break;
        case 4:
            memcpy(hexa, "4", 1);
            break;
        case 5:
            memcpy(hexa, "5", 1);
            break;
        case 6:
            memcpy(hexa, "6", 1);
            break;
        case 7:
            memcpy(hexa, "7", 1);
            break;
        case 8:
            memcpy(hexa, "8", 1);
            break;
        case 9:
            memcpy(hexa, "9", 1);
            break;
        case 10:
            memcpy(hexa, "a", 1);
            break;
        case 11:
            memcpy(hexa, "b", 1);
            break;
        case 12:
            memcpy(hexa, "c", 1);
            break;
        case 13:
            memcpy(hexa, "d", 1);
            break;
        case 14:
            memcpy(hexa, "e", 1);
            break;
        case 15:
            memcpy(hexa, "f", 1);
            break;
    }
}