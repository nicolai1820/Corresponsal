//
// Created by NETCOM on 04/03/2020.
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

#define APLICA_COSTO     1
#define NO_APLICA_COSTO  2

int _consultarInformacion_(DatosVenta * datosVenta) {

    ResultISOPack resultISOPack;
    TokenMessage tokenMessage;
    int resultado = 0;
    int enviar = 0;
    char inputData[512 + 1];
    char bin[10 + 1];

    memset(inputData, 0x00, sizeof(inputData));
    memset(bin, 0x00, sizeof(bin));
    memset(&resultISOPack, 0x00, sizeof(resultISOPack));
    memset(&tokenMessage, 0x00, sizeof(tokenMessage));

    //se valida si se debe enviar consulta
    memcpy(bin, datosVenta->track2, 10);
    enviar = activarTransaccionConsulta(bin, datosVenta->tipoTransaccion, datosVenta->modoDatafono);

    if (enviar == 1) {
        //empaqueta
        resultISOPack = empaquetarConsultaInformacion(*datosVenta);

        //enviar consulta de la informacion
        if (resultISOPack.totalBytes > 0) {

            setParameter(VERIFICAR_PAPEL_TERMINAL, (char *)"00");
            resultado = realizarTransaccion(resultISOPack.isoPackMessage, resultISOPack.totalBytes, inputData,
                                            TRANSACCION_DEFAULT, CANAL_DEMANDA, REVERSO_NO_GENERADO);
            setParameter(VERIFICAR_PAPEL_TERMINAL, (char *)"01");
        }

        //DESEMPAQUETA CONSULTA DE LA INFORMACION
        tokenMessage = desempaquetarConsultaInformacion(inputData, resultado);
        if (tokenMessage.totalBytes > 0) { //muestar el token
            mostrarInformacion(tokenMessage.valueToken);
            sprintf(datosVenta->textoInformacion, "%-120s", tokenMessage.valueToken);

            //resultado = getchAcceptCancel("C. INFORMACION", "", "DESEA CONTINUAR ?", "", 20 * 1000);

        } else {
            //resultado = getchAcceptCancel("C. INFORMACION", "INFORMACION NO", "DISPONIBLE", "DESEA CONTINUAR ?", 20 * 1000);
        }

    }

    if (enviar == 0) {
        resultado = 1;
    }

    //quitar
    if (enviar == -1) {
        resultado = -1;
    }

    return resultado;

}

void _verificarConsultaInformacion_(DatosVenta * datosVenta, uChar * inputData, int cantidadBytes) {

    TokenMessage tokenMessage;
    memset(&tokenMessage, 0x00, sizeof(tokenMessage));

    tokenMessage = desempaquetarConsultaInformacion(inputData, cantidadBytes);

    if (tokenMessage.totalBytes > 0) {
        sprintf(datosVenta->textoInformacion, "%-120s", tokenMessage.valueToken);
    }

}

ResultISOPack empaquetarConsultaInformacion(DatosVenta datosVenta) {

    ResultISOPack resultISOPack;
    ResultISOUnpack resultadoUnpack;
    ISOHeader isoHeader8583;
    ISOFieldMessage isoFieldMessage;
    uChar consecutivo[6 + 1];
    uChar buffer[40 + 1];
    uChar terminalId[8 + 1];
    char modoInicia[2 + 1];
    char processingCode[TAM_CODIGO_PROCESO + 1];
    char tokenQC[166 + 1];
    int iRet = 0;

    memset(&resultISOPack, 0x00, sizeof(resultISOPack));
    memset(&resultadoUnpack, 0x00, sizeof(resultadoUnpack));
    memset(&isoHeader8583, 0x00, sizeof(isoHeader8583));
    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(consecutivo, 0x00, sizeof(consecutivo));
    memset(buffer, 0x00, sizeof(buffer));
    memset(terminalId, 0x00, sizeof(terminalId));
    memset(modoInicia, 0x00, sizeof(modoInicia));
    memset(processingCode, 0x00, sizeof(processingCode));
    memset(tokenQC, 0x00, sizeof(tokenQC));

    getParameter(CNB_INICIA, modoInicia);
    if (atoi(modoInicia) == 0) {
        getParameter(NUMERO_TERMINAL, terminalId);
    } else { // El llamado se hizo desde un CNB.
        getParameter(CNB_TERMINAL, terminalId);

        if (strlen(terminalId) == 0) { // Verifica si tiene dato, esto es verificar si esta en modo puro o modo mixto.
            memset(terminalId, 0x00, sizeof(terminalId));
            getParameter(NUMERO_TERMINAL, terminalId);
        }
    }

    uChar nii[TAM_NII + 1];
    memset(nii, 0x00, sizeof(nii));

    getParameter(NII, nii);

    isoHeader8583.TPDU = 60;
    memcpy(isoHeader8583.destination, nii, TAM_NII + 1);
    memcpy(isoHeader8583.source, nii, TAM_NII + 1);
    setHeader(isoHeader8583);

    generarSystemTraceNumber(consecutivo,(char *)"");

    if (atoi(datosVenta.tipoTransaccion) == TRANSACCION_TRANSFERENCIA_AVAL
        || atoi(datosVenta.tipoTransaccion) == TRANSACCION_P_PRODUCTO_TARJETA_AVAL
        || atoi(datosVenta.tipoTransaccion) == TRANSACCION_SALDO_CUPO_AVAL
        || atoi(datosVenta.tipoTransaccion) == TRANSACCION_RETIRO_CNB_AVAL
        || atoi(datosVenta.tipoTransaccion) == TRANSACCION_P_PRODUCTO_NO_TARJETA_AVAL) {
        sprintf(processingCode, "39%2s00", datosVenta.tipoCuenta);
    } else {
        sprintf(processingCode,"%s", "390000");
    }
    setMTI((char *)"0100");
    setField(3, (char *)"390000", 6);
    setField(11, consecutivo, 6);
    setField(22, datosVenta.posEntrymode, 3);
    setField(24, nii + 1, 3);
    setField(25, (char *)"00", 2);
    setField(35, datosVenta.track2, strlen(datosVenta.track2));
    setField(41, terminalId, 8);

    ////////empaqueta el token QT/////////
    ResultTokenPack resultTokenPack;
    memset(&resultTokenPack, 0x00, sizeof(resultTokenPack));
    // se completa con ceros

    iRet = leerArchivo(discoNetcom, (char *)TOKEN_QC, tokenQC);

    sprintf(buffer, "%6s%012d%012d%d%d", datosVenta.processingCode, 0, 0, 0, 0);
    if(strcmp(datosVenta.modoDatafono,"05") == 0){
        _setTokenMessage_((char *)"X1", (char *)TOKEN_X1_AVAL, TAM_TOKEN_X1_AVAL);
    }

    _setTokenMessage_((char *)"QT", buffer, strlen(buffer));

    if(iRet > 0 && atoi(datosVenta.tipoTransaccion) != TRANSACCION_SALDO_CUPO_AVAL){
        _setTokenMessage_((char *)"QC",tokenQC,154);
    }
    resultTokenPack = _packTokenMessage_();
    ////////empaqueta el token QT/////////

    setField(60, resultTokenPack.tokenPackMessage, resultTokenPack.totalBytes);

    resultISOPack = packISOMessage();

    return resultISOPack;
}

TokenMessage desempaquetarConsultaInformacion(char * inputData, int cantidadBytes) {

    ResultISOPack resultadoIsoPackMessage;
    ResultISOUnpack resultadoUnpack;
    ISOFieldMessage isoFieldMessage;
    ResultTokenUnpack resultTokenUnpack;
    TokenMessage tokenMessage;
    int idx = 0;

    memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
    memset(&resultadoUnpack, 0x00, sizeof(resultadoUnpack));
    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(&resultTokenUnpack, 0x00, sizeof(resultTokenUnpack));
    memset(&tokenMessage, 0x00, sizeof(tokenMessage));

    if (cantidadBytes > 0) {

        resultadoUnpack = unpackISOMessage(inputData, cantidadBytes);

        isoFieldMessage = getField(39);

        if (isoFieldMessage.totalBytes > 0 && strcmp(isoFieldMessage.valueField, "00") == 0) {
            for (idx = 0; idx < resultadoUnpack.totalField; idx++) {
                if (resultadoUnpack.listField[idx] == 60) {
                    isoFieldMessage = getField(60);
                    resultTokenUnpack = unpackTokenMessage(isoFieldMessage.valueField, isoFieldMessage.totalBytes);
                    tokenMessage = _getTokenMessage_((char *)"QI");
                    break;
                }
            }
        }

    }

    return tokenMessage;

}

void mostrarInformacion(char * informacion) {

    int idx = 1;
    int indice = 0;
    int indice2 = 0;
    int resultado = 0;
    int numeroDatos = 6;
    char * dataLeida;
    char ** datos = NULL;

    //

    //screenMessage("CONSULTA INFORMACION", " ", " ", " ", 1);
    datos = (char **) malloc(numeroDatos * sizeof(char*));
    for (idx = 0; idx < numeroDatos; idx++) {
        datos[idx] = (char*) malloc(30 * sizeof(char));
    }

    idx = 1;

    do {
        dataLeida = (char *) malloc(30);
        memset(dataLeida, 0x00, sizeof(dataLeida));
        resultado = justificarPalabra(informacion, dataLeida, idx, 20);
        if (resultado > 0) {
            datos[indice++] = dataLeida;
            indice2 = indice;
        }
        if (indice == 5 || resultado == -1) {
            if(resultado == -1){
                datos[indice2] = (char *)"";
                datos[indice2++] = (char *)"";
                datos[indice2++] = (char *)"";
                datos[indice2++] = (char *)"";
            }
            indice = 0;
        }
        idx++;
    } while (resultado > 0);
}

int activarTransaccionConsulta(char * bin, char * tipoTransaccion, char * modoDatafono) {

    int resultado = 0;
    int pregunta = 0;
    TablaConsultaInformacion consultaInformacion;

    memset(&consultaInformacion, 0x00, sizeof(consultaInformacion));

    consultaInformacion = traerConsultaInformacion(bin);

    //Evalua actividad para el modo.
    switch (atoi(modoDatafono)) {
        case MODO_COMERCIO:
            resultado = consultaInformacion.optionsProducto.modoComercio;
            break;
        case MODO_PAGO:
            resultado = consultaInformacion.optionsProducto.modoPuntoPago;
            break;
        case MODO_CNB_RBM:
            resultado = consultaInformacion.optionsProducto.modoCNBRBM;
            break;
        case MODO_CNB_BANCOLOMBIA:
            resultado = consultaInformacion.optionsProducto.modoCNBBancolombia;
            break;
        case MODO_CNB_AVAL:
            resultado = consultaInformacion.optionsProducto.modoCNBAval;
            break;
        case MODO_CNB_CORRESPONSAL:
            resultado = consultaInformacion.optionsProducto.modoCNBBancolombia;
            break;
        default:
            break;
    }

    switch (atoi(tipoTransaccion)) {

        case TRANSACCION_VENTA:
            resultado = (resultado == 1 && consultaInformacion.optionsTransaccion.compra == 1);
            break;
        case TRANSACCION_SALDO:
            resultado = (resultado == 1 && consultaInformacion.optionsTransaccion.saldo == 1);
            break;
        case TRANSACCION_PSP:
            resultado = (resultado == 1 && consultaInformacion.optionsTransaccion.psp == 1);
            break;
        case TRANSACCION_PCR:
            resultado = (resultado == 1 && consultaInformacion.optionsTransaccion.pcr == 1);
            break;
        case TRANSACCION_CONSULTA_RECARGA:
            resultado = (resultado == 1 && consultaInformacion.optionsTransaccion.consultaRecarga == 1);
            break;
        case TRANSACCION_RECARGA:
            resultado = (resultado == 1 && consultaInformacion.optionsTransaccion.recarga == 1);
            break;
        case TRANSACCION_EFECTIVO:
        case TRANSACCION_EFECTIVO_DEPOSITO:
            resultado = (resultado == 1 && consultaInformacion.optionsTransaccion.efectivo == 1);
            break;
        case TRANSACCION_TRANSFERENCIA:
            resultado = (resultado == 1 && consultaInformacion.optionsTransaccion.transferencia == 1);
            break;
        case TRANSACCION_RETIRO_CNB_AVAL:
            resultado = (resultado == 1 && consultaInformacion.optionsTransaccion.retiroSinTarjeta == 1);
            break;
        case TRANSACCION_TRANSFERENCIA_AVAL:
            resultado = (resultado == 1 && consultaInformacion.optionsTransaccion.transferenciaSimple == 1);
            break;
        case TRANSACCION_P_PRODUCTO_TARJETA_AVAL:
        case TRANSACCION_P_PRODUCTO_NO_TARJETA_AVAL:
            resultado = (resultado == 1 && consultaInformacion.optionsTransaccion.pagoProducto == 1);
            break;
        default:
            break;
    }

    if (resultado == 1) {
        pregunta = preguntarConsultaInformacion(consultaInformacion.tipoMensaje);
        resultado = (pregunta > 0) ? 1 : 0;
    }

    return resultado;

}

int preguntarConsultaInformacion(char TipoMensaje) {

    int ret = 0;

    switch (TipoMensaje) {
        case 0x00:
            //ret = getchAcceptCancel("C. INFORMACION", "Consulta de Costo", "e informacion adicional", "Desea Consultar?", 20 * 1000);
            break;
        case 0x01:
            //ret = getchAcceptCancel("C. INFORMACION", "Consulta de Costo", "de la transaccion", "Desea Consultar?", 20 * 1000);
            break;
        case 0x02:
            //ret = getchAcceptCancel("C. INFORMACION", "Consulta de informacion", "adicional", "Desea Consultar?",20 * 1000);
            break;
        default:
            break;
    }
    return ret;
}

int _consultarCosto_(DatosVenta * tramaVenta) {

    int resultado = 0;

    //////// PREGUNTAR SI DESEA CONSULTAR COSTO TRANSACCION ///////

    //resultado = getchAcceptCancel("CONSULTA", "DESEA CONSULTAR EL", "COSTO DE TRANSACCION", "CONFIRMAR?", 120 * 1000);

    if (resultado > 0) {
        resultado = transaccionCosto(tramaVenta);
    }

    else if (resultado == __EXIT_KEY) {
        resultado = -1;
    } else {
        resultado = 1;
    }

    return resultado;
}

int transaccionCosto(DatosVenta * tramaCosto) {

    ResultISOPack resultadoIsoPackMessage;
    ISOHeader isoHeader8583;
    uChar dataRecibida[512];
    uChar codigoProceso[6 + 1];
    uChar systemTrace[6 + 1];
    uChar field61[13 + 1];
    int respuesta = 0;
    int resultadoTransaccion = 0;
    char tipoTransaccion[2 + 1];
    //int costoTransaccion	 = 0;

    memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
    memset(&isoHeader8583, 0x00, sizeof(isoHeader8583));
    memset(dataRecibida, 0x00, sizeof(dataRecibida));
    memset(codigoProceso, 0x00, sizeof(codigoProceso));
    memset(systemTrace, 0x00, sizeof(systemTrace));
    memset(field61, 0x00, sizeof(field61));
    memset(tipoTransaccion, 0x00, sizeof(tipoTransaccion));

    _convertASCIIToBCD_(field61, tramaCosto->processingCode, 6);

    sprintf(codigoProceso, "89%.2s00", tramaCosto->tipoCuenta);
    generarSystemTraceNumber(systemTrace,(char *)"");

    uChar nii[TAM_NII + 1];
    memset(nii, 0x00, sizeof(nii));

    getParameter(NII, nii);

    isoHeader8583.TPDU = 60;
    memcpy(isoHeader8583.destination, nii, TAM_NII + 1);
    memcpy(isoHeader8583.source, nii, TAM_NII + 1);
    setHeader(isoHeader8583);

    /*isoHeader8583.TPDU=60;
     strcpy(isoHeader8583.destination,"0001");
     strcpy(isoHeader8583.source,"0001");
     setHeader(isoHeader8583);*/

    setMTI((char *)"0100");
    setField(3, codigoProceso, 6);
    setField(11, systemTrace, 6);
    setField(22, tramaCosto->posEntrymode, 3); //de acuerdo al tipo de transaccion, banda,chip o fall back.
    setField(24, nii + 1, 3);
    setField(25, (char *)"00", 2);
    setField(35, tramaCosto->track2, strlen(tramaCosto->track2));
    setField(41, tramaCosto->terminalId, 8);
    setField(61, field61, 13);

    resultadoIsoPackMessage = packISOMessage();

    if (resultadoIsoPackMessage.responseCode > 0) {
        getParameter(TIPO_TRANSACCION, tipoTransaccion);
        setParameter(VERIFICAR_PAPEL_TERMINAL, (char *)"00");
        resultadoTransaccion = realizarTransaccion(resultadoIsoPackMessage.isoPackMessage,
                                                   resultadoIsoPackMessage.totalBytes, dataRecibida, TRANSACCION_VENTA, CANAL_PERMANENTE,
                                                   REVERSO_NO_GENERADO); //////aqui modificar
        setParameter(VERIFICAR_PAPEL_TERMINAL, (char *)"01");
        setParameter(TIPO_TRANSACCION, tipoTransaccion);
    }

    if (resultadoTransaccion > 0) {
        respuesta = desempaquetarConsulta(dataRecibida, resultadoTransaccion);
        if (respuesta > 0) {
            memset(tramaCosto->field61, 0x00, sizeof(tramaCosto->field61));
            strcpy(tramaCosto->field61, dataRecibida);
        }
    }
    return respuesta;
}

int desempaquetarConsulta(uChar * dataRecibida, int totalBytes) {

    ResultISOUnpack resultadoUnpack;
    ISOFieldMessage isoFieldMessage;
    uChar campo61[12 + 1];
    uChar montoCosto[10 + 1];
    uChar codigoRespuesta[2 + 1];
    int resultado = 0;
    int costoTransaccion = 0;
    int idx = 0;
    int existeCampoCosto = 0;

    memset(&resultadoUnpack, 0x00, sizeof(resultadoUnpack));
    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(campo61, 0x00, sizeof(campo61));
    memset(codigoRespuesta, 0x00, sizeof(codigoRespuesta));
    memset(montoCosto, 0x00, sizeof(montoCosto));

    resultadoUnpack = unpackISOMessage(dataRecibida, totalBytes);
    isoFieldMessage = getField(39);
    memcpy(codigoRespuesta, isoFieldMessage.valueField, 2);

    /////// VERIFICAR SI LLEGO EL CAMPO 61 //////
    for (idx = 0; idx < resultadoUnpack.totalField; idx++) {
        if (resultadoUnpack.listField[idx] == 61) {
            existeCampoCosto = 1;
        }
    }

    if (strncmp(codigoRespuesta, "00", 2) == 0) {
        if (existeCampoCosto == 1) {
            isoFieldMessage = getField(61);
            memcpy(campo61, isoFieldMessage.valueField, 6);
            if (campo61[0] == 0x00) {
                memcpy(montoCosto, campo61 + 1, 5);
                costoTransaccion = APLICA_COSTO;
            } else if (campo61[0] == 0x01) {
                costoTransaccion = NO_APLICA_COSTO;
            }
        } else {
            costoTransaccion = NO_APLICA_COSTO;
        }
        resultado = validarCosto(costoTransaccion, montoCosto);
    } else {
        //resultado = getchAcceptCancel("CONSULTA", "CONSULTA", "DECLINADA", "DESEA CONTINUAR ?", 20 * 1000);
    }

    if (resultado > 0) {
        if (costoTransaccion == APLICA_COSTO) {
            memset(dataRecibida, 0x00, sizeof(dataRecibida));
            strcpy(dataRecibida, montoCosto);
            // costoTransaccion = atoi(campo61);
        }/*
		 else{
		 costoTransaccion = 1;
		 }*/
    }/*else{
	 costoTransaccion = __BACK_KEY;
	 }*/

    return resultado;
}

int validarCosto(int permiteCosto, uChar * campoCosto) {

    int resultado = 0;
    uChar variableAux[25 + 1];
    uChar costoPantalla[25 + 1];
    uChar monto[25 + 1];
    memset(variableAux, 0x00, sizeof(variableAux));
    memset(costoPantalla, 0x00, sizeof(costoPantalla));
    memset(monto, 0x00, sizeof(monto));

    if (permiteCosto == APLICA_COSTO) {

        _convertBCDToASCII_(variableAux, campoCosto, 10);
        memset(campoCosto, 0x00, sizeof(campoCosto));
        strcpy(campoCosto, variableAux);
        formatString(0x30, 0, variableAux, strlen(variableAux), monto, 2);
        sprintf(costoPantalla, "COSTO DE:$%s", monto);
        //resultado = getchAcceptCancel("CONSULTA DE COSTO", "LA OPERACION TIENE UN", costoPantalla, "DESEA CONTINUAR?", 20 * 1000);
    } else {
        //resultado = getchAcceptCancel("CONSULTA DE COSTO", "TARIFA NO", "DISPONIBLE", "DESEA CONTINUAR?", 20 * 1000);
    }

    return resultado;
}

