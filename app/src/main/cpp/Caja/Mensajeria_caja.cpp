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

#define FS  				0x1C
#define VERSION 			0x31
#define INDICADOR 			0x30
#define INDICADOR1 			0x30

#define ACK 				0x06
#define NACK 				0x15
#define HEADER_TEFF 		"6000000000"

#define LEN_MENSAJE                 2
#define LEN_BUFFER_CAJA             512

#define LEN_MENSAJE                 2
#define LEN_TEFF_PAN                12
#define LEN_SIZE                    2

int indiceActualCajas = 0;
int countFieldTEFF = 0;
int cantidadCampos = 1;
int dataTEFF = 0; //se coloca para saber si ya se inicializaron los campos de isoMessageTEFF para saber cuantos campos setearon
int terminaUnpack = 0;
ResultISOUnpack resultUnpackTEFF;
TEFFFieldMessage isoMessageTEFF[MAX_FIELD_TEFF + 1];
TEFFFieldMessage resulMessageTEFF[MAX_FIELD_TEFF + 1];
TEFFFieldMessage mesageHeader[3 + 1];
TEFFFieldMessage presentationHeaderMessage;

void setFieldTEFF(int field, uChar * valueField, int totalBytes) {

    TEFFFieldMessage isoFieldMsg;
    int id = 0;
    memset(isoFieldMsg.valueField, 0x00, sizeof(isoFieldMsg.valueField));

    if (dataTEFF == 0) {
        inicializarDataTEFF();
    }

    //obtiene el id/indice donde guardara la estructura de datos.
    id = getIdFieldCaja(field);
    isoFieldMsg.totalBytes = totalBytes;
    isoFieldMsg.field = field;
    memcpy(isoFieldMsg.valueField, valueField, isoFieldMsg.totalBytes);
    isoMessageTEFF[id] = isoFieldMsg;
}

TEFFFieldMessage getFiledTEFF(int field) {

    TEFFFieldMessage campo;

    campo = getUnPackFieldMessage(field);

    return campo;
}

int getIdFieldCaja(int field) {
    int id;
    int length;
    TEFFFieldMessage isoFieldMsg;

    memset(&isoFieldMsg, 0x00, sizeof(isoFieldMsg));

    //revisa si ya esta creado/seteado el campo especifico

    isoFieldMsg = getISOFieldMessageCaja(field);
    length = isoFieldMsg.totalBytes;
    if (length == 0) {
        id = countFieldTEFF++; // no estba seteado previamente
    } else {
        id = indiceActualCajas; //estaba seteado, entonces se devuelve el id donde fue seteado
    }
    return id;
}

TEFFFieldMessage getISOFieldMessageCaja(int field) {

    int idx = 0;
    TEFFFieldMessage fieldMessage;
    indiceActualCajas = -1;

    memset(&fieldMessage, 0x00, sizeof(fieldMessage));

    for (idx = 0; idx < MAX_FIELD_TEFF; idx++) {
        if (isoMessageTEFF[idx].field == field) {
            fieldMessage = isoMessageTEFF[idx];
            indiceActualCajas = idx; // indice donde se encontro.
            break;
        }
    }
    return fieldMessage;
}

TEFFFieldMessage getUnPackFieldMessage(int field) {

    int idx = 0;
    TEFFFieldMessage fieldMessage;
    indiceActualCajas = -1;

    memset(&fieldMessage, 0x00, sizeof(fieldMessage));

    for (idx = 0; idx < MAX_FIELD_TEFF; idx++) {
        if (resulMessageTEFF[idx].field == field) {
            fieldMessage = resulMessageTEFF[idx];
            indiceActualCajas = idx; // indice donde se encontro.
            break;
        }
    }
    return fieldMessage;
}

/**
 * @brief Funcion que realiza el empaquetamiento de los campos del mensaje TEFF.
 * @return Retorna una variable de la estructura ResultISOPack con la
 * informacion del proceso.
 * @author .
 */
ResultISOPack _packDataMessageTEFF_(void) {

    int i = 0;
    int sizeTotal = 0;

    char sizeAuxiliar[2 + 1];
    char campoAuxiliar[2 + 1];

    ResultISOPack resultadoIsoPackMessage;

    memset(sizeAuxiliar, 0x00, sizeof(sizeAuxiliar));
    memset(campoAuxiliar, 0x00, sizeof(campoAuxiliar));
    memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
    cantidadCampos = cantidadCamposSeteados();
    if (cantidadCampos != MAX_FIELD_TEFF) {
        ordenarCampos(cantidadCampos);
        resultadoIsoPackMessage.isoPackMessage[sizeTotal++] = FS;
        for (i = 0; i < cantidadCampos; i++) {
            memset(sizeAuxiliar, 0x00, sizeof(sizeAuxiliar));
            memset(campoAuxiliar, 0x00, sizeof(campoAuxiliar));
            sprintf(campoAuxiliar, "%02d", isoMessageTEFF[i].field);
            memcpy(resultadoIsoPackMessage.isoPackMessage + sizeTotal, campoAuxiliar, 2);
            convertIntegerToHex(isoMessageTEFF[i].totalBytes, sizeAuxiliar);
            sizeTotal += 2;
            memcpy(resultadoIsoPackMessage.isoPackMessage + sizeTotal, sizeAuxiliar, 2);
            sizeTotal += 2;
            if (isoMessageTEFF[i].totalBytes > 0) {
                memcpy(resultadoIsoPackMessage.isoPackMessage + sizeTotal, isoMessageTEFF[i].valueField,
                       isoMessageTEFF[i].totalBytes);
                sizeTotal += isoMessageTEFF[i].totalBytes;
            }
            if (i < cantidadCampos - 1) {
                resultadoIsoPackMessage.isoPackMessage[sizeTotal] = FS;
                sizeTotal++;
            }/*else{
			 resultadoIsoPackMessage.isoPackMessage[sizeTotal] = ETX;
			 sizeTotal ++;
			 }*/
        }

    }

    resultadoIsoPackMessage.totalBytes = sizeTotal;

    return resultadoIsoPackMessage;
}

ResultISOPack packMessageTEFF(void) {

    ResultISOPack resultadoPackMessageTEFF;
    ResultISOPack packAuxiliar;
    TEFFFieldMessage headerMessage;
    uChar LRC;
    int sizeTotal = 0;
    char sizeBCD[LEN_MENSAJE + 1];
    char sizeAuxiliar[5 + 1];
    char bufferAuxiliar[512 + 1];
    char bufferAuxiliar2[512 + 1];

    memset(bufferAuxiliar, 0x00, sizeof(bufferAuxiliar));
    memset(bufferAuxiliar2, 0x00, sizeof(bufferAuxiliar2));
    memset(&resultadoPackMessageTEFF, 0x00, sizeof(resultadoPackMessageTEFF));
    memset(&packAuxiliar, 0x00, sizeof(packAuxiliar));
    memset(&headerMessage, 0x00, sizeof(headerMessage));
    memset(sizeBCD, 0x00, sizeof(sizeBCD));
    memset(sizeAuxiliar, 0x00, sizeof(sizeAuxiliar));

    headerMessage = packDataHeaderTEFF();
    packAuxiliar = _packDataMessageTEFF_();

    sprintf(sizeAuxiliar, "%04d", (packAuxiliar.totalBytes + headerMessage.totalBytes)); //-1
    _convertASCIIToBCD_(sizeBCD, sizeAuxiliar, strlen(sizeAuxiliar));

    memcpy(bufferAuxiliar, sizeBCD, 2);
    sizeTotal += 2;

    memcpy(bufferAuxiliar + sizeTotal, headerMessage.valueField, headerMessage.totalBytes);
    sizeTotal += headerMessage.totalBytes;

    if (packAuxiliar.totalBytes > 0) {
        memcpy(bufferAuxiliar + sizeTotal, packAuxiliar.isoPackMessage, packAuxiliar.totalBytes);
        sizeTotal += packAuxiliar.totalBytes;
    }

    bufferAuxiliar[sizeTotal] = ETX;
    sizeTotal++;

    LRC = calcularLRC(bufferAuxiliar, sizeTotal);

    resultadoPackMessageTEFF.isoPackMessage[0] = STX;
    memcpy(resultadoPackMessageTEFF.isoPackMessage + 1, bufferAuxiliar, sizeTotal);
    sizeTotal++;

    resultadoPackMessageTEFF.isoPackMessage[sizeTotal] = LRC;
    sizeTotal++;

    resultadoPackMessageTEFF.totalBytes = sizeTotal;

    inicializarCamposTEFF();
    return resultadoPackMessageTEFF;

}

TEFFFieldMessage packDataHeaderTEFF(void) {

    char auxiliar[2 + 1];
    int sizeTotal = 0;
    int resultado = 0;
    int codigoRespuesta = 0;
    int codigoTransaccion = 0;
    int tipoMensaje = 0;
    ISOFieldMessage camposHeader;

    memset(&camposHeader, 0x00, sizeof(camposHeader));
    memset(&presentationHeaderMessage, 0x00, sizeof(presentationHeaderMessage));
    memset(auxiliar, 0x00, sizeof(auxiliar));

    memcpy(presentationHeaderMessage.valueField, HEADER_TEFF, strlen(HEADER_TEFF));
    sizeTotal += strlen(HEADER_TEFF);

    presentationHeaderMessage.valueField[sizeTotal] = VERSION;
    sizeTotal++;

    tipoMensaje = getTipoMensajeTEFF();
    memset(auxiliar, 0x00, sizeof(auxiliar));
    sprintf(auxiliar, "%d", tipoMensaje);
    memcpy(presentationHeaderMessage.valueField + sizeTotal, auxiliar, 1);
    sizeTotal++;

    codigoTransaccion = getCodigoTransaccionTEFF();
    memset(auxiliar, 0x00, sizeof(auxiliar));
    sprintf(auxiliar, "%02d", codigoTransaccion);
    memcpy(presentationHeaderMessage.valueField + sizeTotal, auxiliar, 2);
    sizeTotal += 2;

    codigoRespuesta = getCodigoRespuestaTEFF();
//	printf("\x1B""CR: %d\n",codigoRespuesta.valueField);
//    ttestall(0,200);

    resultado = ((codigoRespuesta == NO_HAY_CODIGO_DE_RESPUESTA) ? 1 : -1);

    if (resultado > 0) { //NO_HAY_CODIGO_DE_RESPUESTA
        memset(auxiliar, 0x20, sizeof(auxiliar));
        memcpy(presentationHeaderMessage.valueField + sizeTotal, auxiliar, 2);
    } else {

        //memcpy(presentationHeaderMessage.valueField  + sizeTotal,camposHeader.valueField ,camposHeader.totalBytes);
        memcpy(presentationHeaderMessage.valueField + sizeTotal, "00", 2);

    }
    sizeTotal += 2;

    presentationHeaderMessage.valueField[sizeTotal] = INDICADOR;
    sizeTotal++;

    /*presentationHeaderMessage.valueField[sizeTotal] = FS ;
     sizeTotal ++;*/

    presentationHeaderMessage.totalBytes = sizeTotal;

    return presentationHeaderMessage;
}

TEFFFieldMessage _getHeaderTEFF_(void) {

    return presentationHeaderMessage;
}
void inicializarDataTEFF(void) {
    int idx = 0;
    dataTEFF = TRUE;

    for (idx = 0; idx < MAX_FIELD_TEFF; idx++) {
        memset(&isoMessageTEFF[idx], 0x00, sizeof(isoMessageTEFF[idx]));
        isoMessageTEFF[idx].field = -1;
    }
}

int cantidadCamposSeteados(void) {

    char firmaHabilitada[2 + 1];
    int cantidadCampos = MAX_FIELD_TEFF;
    memset(firmaHabilitada, 0x00, sizeof(firmaHabilitada));

    getParameter(HABILITA_FIRMA_IP, firmaHabilitada);

    int i = 0;
    for (i = 0; i < cantidadCampos; i++) {

        if (isoMessageTEFF[i].field < 0) {
            break;
        }
    }
    return i;
}

ResultISOUnpack cantidadCamposSeteadosUnPack(void) {

    int i = 0;
    ResultISOUnpack campos;

    memset(&campos, 0x00, sizeof(campos));
    for (i = 0; i < MAX_FIELD_TEFF; i++) {

        if (resulMessageTEFF[i].field >= 0) {
            campos.listField[i] = resulMessageTEFF[i].field;
            campos.totalField++;

        } else {
            i = MAX_FIELD_TEFF;
        }
    }
    return campos;
}

void inicializarCamposTEFF(void) {

    int idx = 0;

    cantidadCampos = 1;
    dataTEFF = 0;
    indiceActualCajas = 0;
    countFieldTEFF = 0;

    memset(&presentationHeaderMessage, 0x00, sizeof(presentationHeaderMessage));

    for (idx = 0; idx < MAX_FIELD_TEFF; idx++) {
        memset(&isoMessageTEFF[idx], 0x00, sizeof(isoMessageTEFF[idx]));
    }

}

ResultISOUnpack unpackMessageTEFF(uChar * messageTEFF, int inLength) {

    char bufferAuxiliar[512 + 1];
    char bufferMessage[512 + 1];
    char lenghMessageBCD[4 + 1];
    char header[20 + 1];
    int lenghDataMessage = 0;
    int idex = 0;
    int i = 0;
    int idx = 0;
    int size = 0;
    char longitudBCD[2 + 1];
    char longitudaAuxiliar[2 + 1];
    int longitudMensaje = 0;
    ResultISOUnpack cantidaCampos;

    memset(bufferMessage, 0x00, sizeof(bufferMessage));
    memset(bufferAuxiliar, 0x00, sizeof(bufferAuxiliar));
    memset(lenghMessageBCD, 0x00, sizeof(lenghMessageBCD));
    memset(&cantidaCampos, 0x00, sizeof(cantidaCampos));

    memcpy(bufferAuxiliar, messageTEFF, inLength);
    inicializarCamposTEFF();
    size += 2;
    memcpy(longitudBCD, messageTEFF + size, 2);
    size += 2;
    _convertBCDToASCII_(longitudaAuxiliar, longitudBCD, 4);
    longitudMensaje = atoi(longitudaAuxiliar);

    idex += 4;
    for (i = idex; i < longitudMensaje; i++) {
        if (bufferAuxiliar[i] == FS) {
            break;
        }
    }

    memcpy(header, bufferAuxiliar + idex, i - idex);
    idex += (i - idex);
    unpackDataHeaderMessageTEFF(header, i - 4);

    lenghDataMessage = (longitudMensaje - idex + 4);
    memcpy(bufferMessage, bufferAuxiliar + idex + 1, lenghDataMessage);

    unpackDataMessageTEFF(bufferMessage, lenghDataMessage);

    for (idx = 0; idx < MAX_FIELD_TEFF; idx++) {
        memset(&resulMessageTEFF[idx], 0x00, sizeof(resulMessageTEFF[idx]));
    }
    for (idx = 0; idx < MAX_FIELD_TEFF; idx++) {
        resulMessageTEFF[idx] = isoMessageTEFF[idx];
    }
   inicializarCamposTEFF();

    cantidaCampos = cantidadCamposSeteadosUnPack();

    return cantidaCampos;

}

void unpackDataMessageTEFF(char *message, int inLength) {

    TEFFFieldMessage fiel;
    char dataMessage[512 + 1];
    char bufferAuxiliar[30 + 1];
    int idex = 0;
    int idex2 = 0;
    int i = 0;
    int ultimoCampo = 0;
    int hayCampo = 0;

    memset(dataMessage, 0x00, sizeof(dataMessage));
    memset(bufferAuxiliar, 0x00, sizeof(bufferAuxiliar));

    memcpy(dataMessage, message, inLength);
    for (i = 0; i < inLength; i++) {

        if (dataMessage[i] == ETX && (i == inLength - 1)) {

            idex = i - idex2;
            ultimoCampo = TRUE;
            hayCampo = TRUE;

        } else if (dataMessage[i] == FS) {

            idex = i - idex2;
            hayCampo = TRUE;
        }

        if (hayCampo == TRUE) {

            memset(bufferAuxiliar, 0x00, sizeof(bufferAuxiliar));
            memcpy(bufferAuxiliar, dataMessage + idex2, idex);
            idex2 += idex + 1;
            fiel = unpackDataFieldTEFF(bufferAuxiliar, idex);
            setFieldTEFF(fiel.field, fiel.valueField, fiel.totalBytes);

            hayCampo = 0;
        }
        if (ultimoCampo == TRUE) {
            break;
        }

    }

}

TEFFFieldMessage unpackDataFieldTEFF(char *message, int inLength) {

    TEFFFieldMessage fieldMessage;
    char lengthField[2 + 1];
    unsigned long lend = 0;
    char Field[2 + 1];
    char lengthFieldASCII[4 + 1];

    memset(&fieldMessage, 0x00, sizeof(fieldMessage));
    memset(lengthField, 0x00, sizeof(lengthField));
    memset(lengthFieldASCII, 0x00, sizeof(lengthFieldASCII));
    memset(Field, 0x00, sizeof(Field));

    memcpy(Field, message, 2);
    memcpy(lengthField, message + 2, 2);

    sprintf(lengthFieldASCII, "%02X%02X", lengthField[0], lengthField[1]);

    lend = hexToint(lengthFieldASCII, 4);
    fieldMessage.field = atoi(Field);

    fieldMessage.totalBytes = lend;
    memcpy(fieldMessage.valueField, message + 4, lend);

    return fieldMessage;
}

/**
 * @brief Funcion que realiza el desempaquetamiento los campos
 *        de presentacion del mensaje de caja  TEFF.
 * @param header : mensaje de presentacion.
 * @param inLength : tamaño del mensaje de presentacion.
 * @author .
 */
void unpackDataHeaderMessageTEFF(char *header, int inLength) {

    int idex = 0;
    char bufferAuxiliar[20 + 1];
    char codigoTransaccion[2 + 1];
    char codigoRespuesta[2 + 1];
    char tipoMensaje[1 + 1];

    memset(codigoTransaccion, 0x00, sizeof(codigoTransaccion));
    memset(bufferAuxiliar, 0x00, sizeof(bufferAuxiliar));
    memset(codigoRespuesta, 0x00, sizeof(codigoRespuesta));
    memset(tipoMensaje, 0x00, sizeof(tipoMensaje));

    memcpy(bufferAuxiliar, header, inLength);
    idex += 11; //esta es la longitud del header y version que siempre esta fija

    memcpy(tipoMensaje, bufferAuxiliar + idex, 1);
    setTipoMensajeTEFF(atoi(tipoMensaje));
    idex += 1;

    memcpy(codigoTransaccion, bufferAuxiliar + idex, 2);
    setCodigoTransaccionTEFF(atoi(codigoTransaccion));
    idex += 2;

    memcpy(codigoRespuesta, bufferAuxiliar + idex, 2);
    setCodigoRespuestaTEFF(atoi(codigoRespuesta));

}

int getTipoMensajeTEFF(void) {

    TEFFFieldMessage tipoMensajeTEFF;
    char tipoMensaje[2 + 1];

    memset(tipoMensaje, 0x00, sizeof(tipoMensaje));
    memset(&tipoMensajeTEFF, 0x00, sizeof(tipoMensajeTEFF));

    tipoMensajeTEFF = mesageHeader[0];

    memcpy(tipoMensaje, tipoMensajeTEFF.valueField, tipoMensajeTEFF.totalBytes);

    return atoi(tipoMensaje);
}

int getCodigoTransaccionTEFF(void) {

    TEFFFieldMessage codigoTransaccionTEFF;
    char codigoTransaccion[2 + 1];
    memset(codigoTransaccion, 0x00, sizeof(codigoTransaccion));
    memset(&codigoTransaccionTEFF, 0x00, sizeof(codigoTransaccionTEFF));

    codigoTransaccionTEFF = mesageHeader[1];
    memcpy(codigoTransaccion, codigoTransaccionTEFF.valueField, codigoTransaccionTEFF.totalBytes);

    return atoi(codigoTransaccion);
}

int getCodigoRespuestaTEFF(void) {

    TEFFFieldMessage codigoRespuestaTEFF;
    char codigoDeRespuesta[2 + 1];
    memset(codigoDeRespuesta, 0x00, sizeof(codigoDeRespuesta));
    memset(&codigoRespuestaTEFF, 0x00, sizeof(codigoRespuestaTEFF));

    codigoRespuestaTEFF = mesageHeader[2];

    memcpy(codigoDeRespuesta, codigoRespuestaTEFF.valueField, codigoRespuestaTEFF.totalBytes);

    return atoi(codigoDeRespuesta);
}

void setTipoMensajeTEFF(int tipoMensaje) {

    char tipoDeMensaje[1 + 1];

    memset(tipoDeMensaje, 0x00, sizeof(tipoDeMensaje));
    memset(&mesageHeader[0], 0x00, sizeof(mesageHeader[0]));
    sprintf(tipoDeMensaje, "%d", tipoMensaje);

    mesageHeader[0].totalBytes = 1;
    memcpy(mesageHeader[0].valueField, tipoDeMensaje, 1);
}

void setCodigoTransaccionTEFF(int codigoTransaccion) {

    char numeroCodigoTransaccion[2 + 1];

    memset(numeroCodigoTransaccion, 0x00, sizeof(numeroCodigoTransaccion));
    memset(&mesageHeader[1], 0x00, sizeof(mesageHeader[1]));
    sprintf(numeroCodigoTransaccion, "%02d", codigoTransaccion);

    mesageHeader[1].totalBytes = 2;
    memcpy(mesageHeader[1].valueField, numeroCodigoTransaccion, 2);
}

void setCodigoRespuestaTEFF(int codigoRespuesta) {

    char codigoDeRespuesta[2 + 1];

    memset(codigoDeRespuesta, 0x00, sizeof(codigoDeRespuesta));
    memset(&mesageHeader[2], 0x00, sizeof(mesageHeader[2]));
    sprintf(codigoDeRespuesta, "%02d", codigoRespuesta);

    mesageHeader[2].totalBytes = 2;
    memcpy(mesageHeader[2].valueField, codigoDeRespuesta, 2);
}
unsigned long hexToint(char * stringHexa, unsigned int lengthString) {

    int idx = 0;
    unsigned long datoLong = 0;

    for (idx = 0; idx < lengthString; idx++) {
        if (stringHexa[idx] <= 57) {
            datoLong += (stringHexa[idx] - 48) * (1 << (4 * (lengthString - 1 - idx)));
        } else {
            datoLong += (stringHexa[idx] - 55) * (1 << (4 * (lengthString - 1 - idx)));
        }
    }
    return datoLong;
}

void ordenarCampos(int cantidadCampos) {

    int izq = 0;
    int der = 0;
    int medio = 0;
    int i = 0;
    int j = 0;
    TEFFFieldMessage temp;

    for (i = 1; i < cantidadCampos; i++) {

        memset(&temp, 0x00, sizeof(temp));

        temp = isoMessageTEFF[i];
        izq = 0;
        der = i - 1;

        while (izq <= der) {

            medio = (izq + der) / 2;
            if (temp.field < isoMessageTEFF[medio].field) {
                der = medio - 1;
            } else {
                izq = medio + 1;
            }
        }

        for (j = i - 1; j >= izq; j--) {

            isoMessageTEFF[j + 1] = isoMessageTEFF[j];
        }

        isoMessageTEFF[izq] = temp;
    }
}
ResultISOPackRecibo packMessageTEFFReciboDinamico(void) {

    ResultISOPackRecibo resultadoPackMessageTEFF;
    ResultISOPackRecibo packAuxiliar;
    TEFFFieldMessage headerMessage;
    uChar LRC;
    int sizeTotal = 0;
    char sizeBCD[LEN_MENSAJE + 1];
    char sizeAuxiliar[5 + 1];
    char bufferAuxiliar[1024 + 1];
    char bufferAuxiliar2[1024 + 1];

    memset(bufferAuxiliar, 0x00, sizeof(bufferAuxiliar));
    memset(bufferAuxiliar2, 0x00, sizeof(bufferAuxiliar2));
    memset(&resultadoPackMessageTEFF, 0x00, sizeof(resultadoPackMessageTEFF));
    memset(&packAuxiliar, 0x00, sizeof(packAuxiliar));
    memset(&headerMessage, 0x00, sizeof(headerMessage));
    memset(sizeBCD, 0x00, sizeof(sizeBCD));
    memset(sizeAuxiliar, 0x00, sizeof(sizeAuxiliar));

    headerMessage = packDataHeaderTEFF();
    packAuxiliar = packDataMessageTEFFRecibo();

    sprintf(sizeAuxiliar, "%04d", (packAuxiliar.totalBytes + headerMessage.totalBytes)); //-1
    _convertASCIIToBCD_(sizeBCD, sizeAuxiliar, strlen(sizeAuxiliar));

    memcpy(bufferAuxiliar, sizeBCD, 2);
    sizeTotal += 2;

    memcpy(bufferAuxiliar + sizeTotal, headerMessage.valueField, headerMessage.totalBytes);
    sizeTotal += headerMessage.totalBytes;

    if (packAuxiliar.totalBytes > 0) {
        memcpy(bufferAuxiliar + sizeTotal, packAuxiliar.isoPackMessage, packAuxiliar.totalBytes);
        sizeTotal += packAuxiliar.totalBytes;
    }

    bufferAuxiliar[sizeTotal] = ETX;
    sizeTotal++;

    LRC = calcularLRC(bufferAuxiliar, sizeTotal);

    resultadoPackMessageTEFF.isoPackMessage[0] = STX;
    memcpy(resultadoPackMessageTEFF.isoPackMessage + 1, bufferAuxiliar, sizeTotal);
    sizeTotal++;

    resultadoPackMessageTEFF.isoPackMessage[sizeTotal] = LRC;
    sizeTotal++;

    resultadoPackMessageTEFF.totalBytes = sizeTotal;

    inicializarCamposTEFF();
    return resultadoPackMessageTEFF;

}
ResultISOPackRecibo packDataMessageTEFFRecibo(void) {

    int i = 0;
    int sizeTotal = 0;

    char sizeAuxiliar[2 + 1];
    char campoAuxiliar[2 + 1];

    ResultISOPackRecibo resultadoIsoPackMessage;

    memset(sizeAuxiliar, 0x00, sizeof(sizeAuxiliar));
    memset(campoAuxiliar, 0x00, sizeof(campoAuxiliar));
    memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
    cantidadCampos = cantidadCamposSeteados();
    if (cantidadCampos != MAX_FIELD_TEFF) {
        ordenarCampos(cantidadCampos);
        resultadoIsoPackMessage.isoPackMessage[sizeTotal++] = FS;
        for (i = 0; i < cantidadCampos; i++) {
            memset(sizeAuxiliar, 0x00, sizeof(sizeAuxiliar));
            memset(campoAuxiliar, 0x00, sizeof(campoAuxiliar));
            sprintf(campoAuxiliar, "%02d", isoMessageTEFF[i].field);
            memcpy(resultadoIsoPackMessage.isoPackMessage + sizeTotal, campoAuxiliar, 2);
            convertIntegerToHex(isoMessageTEFF[i].totalBytes, sizeAuxiliar);
            sizeTotal += 2;
            memcpy(resultadoIsoPackMessage.isoPackMessage + sizeTotal, sizeAuxiliar, 2);
            sizeTotal += 2;
            if (isoMessageTEFF[i].totalBytes > 0) {
                memcpy(resultadoIsoPackMessage.isoPackMessage + sizeTotal, isoMessageTEFF[i].valueField,
                       isoMessageTEFF[i].totalBytes);
                sizeTotal += isoMessageTEFF[i].totalBytes;
            }
            if (i < cantidadCampos - 1) {
                resultadoIsoPackMessage.isoPackMessage[sizeTotal] = FS;
                sizeTotal++;
            }
        }
    }

    resultadoIsoPackMessage.totalBytes = sizeTotal;

    return resultadoIsoPackMessage;
}

uChar calcularLRC(const uChar *buffer, unsigned int length) {

    uChar checksum = 0;

    int idx = 0;

    for (idx = 0; idx < length; idx++) {
        checksum ^= buffer[idx];
    }

    return checksum;
}