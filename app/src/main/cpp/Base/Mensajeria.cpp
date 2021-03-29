//
// Created by NETCOM on 15/01/2020.
//

#include "Mensajeria.h"
#include "Utilidades.h"
//#include "include/Utilidades.h"
#include <string>
#include "android/log.h"

#define MAX_TOKEN  15
#define  LOG_TAG    "NETCOM MENSAJERIA"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
uChar isoHeader[5 + 1];
int countField = 0;
ISOFieldMessage isoMessage[18 + 1];
int indiceActual;
ResultISOUnpack globalResultadoUnpack;
ResultISOUnpack resultUnpack;
int lengthMessage;
int countToken = 0;
int indiceActualToken = 0;
int lengthMessageToken = 0;
TokenMessage tokenMessage[MAX_TOKEN + 1];
uChar messageInputToken[LEN_BUFFER + 1];
uChar messageInputISO[LEN_BUFFER_MAX * 2 + 1];

FieldDescription packageISO8583[] =
        {
                {0,  LEN_MSG_CODE, BCD_NUMERIC},
                {1,  LEN_BITMAP, BCD_BITMAP},
                {2,  LEN_PAN, BCD_LL_NUMERIC},
                {3,  LEN_PROC_CODE, BCD_NUMERIC},
                {4,  LEN_TRAN_AMT, BCD_NUMERIC},
                {5,  LEN_FRN_AMT, BCD_NUMERIC},
                {6,  12, BCD_NUMERIC},
                {7,  10, BCD_NUMERIC},
                {8,  8, BCD_NUMERIC},
                {9,  LEN_DCC_RATE, BCD_NUMERIC},
                {10, 8, BCD_NUMERIC},
                {11, LEN_STAN, BCD_NUMERIC},
                {12, LEN_LOCAL_TIME, BCD_NUMERIC},
                {13, LEN_LOCAL_DATE, BCD_NUMERIC},
                {14, LEN_EXP_DATE, BCD_NUMERIC},
                {15, 4, BCD_NUMERIC},
                {16, 4, BCD_NUMERIC},
                {17, 4, BCD_NUMERIC},
                {18, 4, BCD_NUMERIC},
                {19, 3, BCD_NUMERIC},
                {20, 3, BCD_NUMERIC},
                {21, 3, BCD_NUMERIC},
                {22, LEN_ENTRY_MODE, BCD_NUMERIC},
                {23, LEN_PAN_SEQ_NO, BCD_NUMERIC},
                {24, LEN_NII, BCD_NUMERIC},
                {25, LEN_COND_CODE, BCD_NUMERIC},
                {26, 2, BCD_NUMERIC},
                {27, 1, BCD_NUMERIC},
                {28, 9, BCD_AMOUNT},
                {29, 9, BCD_AMOUNT},
                {30, 9, BCD_AMOUNT},
                {31, 9, BCD_AMOUNT},
                {32, LEN_FIELD32, BCD_LL_NUMERIC},
                {33, 11, BCD_LL_NUMERIC},
                {34, 28, BCD_LL_CHAR},
                {35, LEN_TRACK2, BCD_LL_NUMERIC},
                {36, LEN_TRACK3, BCD_LLL_CHAR},
                {37, LEN_RRN, BCD_CHAR},
                {38, LEN_AUTH_CODE, BCD_CHAR},
                {39, LEN_RSP_CODE, BCD_CHAR},
                {40, LEN_FIELD40, BCD_CHAR},
                {41, LEN_TERM_ID, BCD_CHAR},
                {42, LEN_MERCHANT_ID, BCD_CHAR},
                {43, LEN_FIELD43, BCD_CHAR},
                {44, LEN_ADDL_RSP, BCD_LL_CHAR},
                {45, LEN_TRACK1, BCD_LL_CHAR},
                {46, 999, BCD_LLL_CHAR},
                {47, LEN_FIELD47, BCD_LLL_CHAR},
                {48, LEN_FIELD48, BCD_LLL_CHAR},
                {49, LEN_FRN_CURCY_CODE, BCD_CHAR},
                {50, LEN_CURCY_CODE, BCD_CHAR},
                {51, 2, BCD_CHAR},
                {52, LEN_PIN_DATA, BCD_BINARY},
                {53, 16, BCD_NUMERIC},
                {54, LEN_EXT_AMOUNT, BCD_LLL_CHAR},
                {55, LEN_ICC_DATA, BCD_LLL_CHAR},
                {56, LEN_FIELD56, BCD_LLL_CHAR},
                {57, LEN_FIELD57, BCD_LLL_CHAR},
                {58, LEN_FIELD58, BCD_LLL_CHAR},
                {59, 999, BCD_LLL_CHAR},
                {60, LEN_FIELD60, BCD_LLL_CHAR},
                {61, LEN_FIELD61, BCD_LLL_CHAR},
                {62, LEN_FIELD62, BCD_LLL_CHAR},
                {63, LEN_FIELD63, BCD_LLL_CHAR},
                {64, LEN_MAC, BCD_BINARY}
        };

void setField(int field, uChar * valueField, int totalBytes) {

    ISOFieldMessage isoFieldMsg;
    int id = 0;

    if (totalBytes > 0) {
        memset(isoFieldMsg.valueField, 0x00, sizeof(isoFieldMsg.valueField));
        //obtiene el id/indice donde guardara la estructura de datos.
        id = getIdFieldISO(field);
        isoFieldMsg.totalBytes = totalBytes;
        isoFieldMsg.field = field;
        memcpy(isoFieldMsg.valueField, valueField, isoFieldMsg.totalBytes);
        isoMessage[id] = isoFieldMsg;
    }

}

/**
 * @brief Funcion que setea el header del mensaje ISO.
 * @param header Estructura de tipo ISOHeader con las propiedades del header.
 * @author .
 */
void setHeader(ISOHeader header) {

    uChar iso[12 + 1];
    memset(iso, 0x00, sizeof(iso));
    memset(isoHeader, 0x00, sizeof(isoHeader));

    sprintf(iso, "%d%s%s", header.TPDU, header.destination, header.source);
    _convertASCIIToBCD_(isoHeader, iso, strlen(iso));
}
int getIdFieldISO(int field) {
    int id;
    int length;
    ISOFieldMessage isoFieldMsg;

    memset(&isoFieldMsg, 0x00, sizeof(isoFieldMsg));
    //revisa si ya esta creado/seteado el campo especifico
    isoFieldMsg = getISOFieldMessageISO(field);
    length = isoFieldMsg.totalBytes;
    if (length == 0) {
        id = countField++; // no estba seteado previamente
    } else {
        id = indiceActual; //estaba seteado, entonces se devuelve el id donde fue seteado
    }

    return id;
}

ISOFieldMessage getISOFieldMessageISO(int field) {

    int idx = 0;
    ISOFieldMessage fieldMessage;
    indiceActual = -1;

    memset(&fieldMessage, 0x00, sizeof(fieldMessage));

    for (idx = 0; idx < 18; idx++) {
        if (isoMessage[idx].field == field) {
            fieldMessage = isoMessage[idx];
            indiceActual = idx; // indice donde se encontro.
            break;
        }
    }

    return fieldMessage;
}

void setMTI(uChar* valueMTI) {
    setField(0, valueMTI, 4);
}
ResultISOPack packISOMessage(void) {
    int i = 0;
    int index = 0;
    int length = 0;
    int pos = 0;

    uChar outputElement[1024 + 1];
    uChar bitMap[8 + 1];
    uChar tam[4 + 1];
    ResultISOPack resultadoIsoPackMessage;
    ISOFieldMessage fieldMessage;
    memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
    memset(&fieldMessage, 0x00, sizeof(fieldMessage));
    memset(outputElement, 0x00, sizeof(outputElement));
    memset(tam, 0x00, sizeof(tam));
    memset(bitMap, 0x00, sizeof(bitMap));

    resultadoIsoPackMessage.responseCode = validateDataElement();

    index += 5;
    index += 2;

    if (resultadoIsoPackMessage.responseCode == 1) {
        //MTI
        fieldMessage = getISOFieldMessageISO(0);
        length = fieldMessage.totalBytes;
        if (length != 0) {
            packDataElement(packageISO8583[0], outputElement, fieldMessage.valueField, &length);
            memcpy(resultadoIsoPackMessage.isoPackMessage + index, outputElement, length);
            index += length;
        }

        index += 8; // + 8 para el bitmap

        for (i = 1; i < 64; i++) {
            memset(&fieldMessage, 0x00, sizeof(fieldMessage));
            fieldMessage = getISOFieldMessageISO(i);
            length = fieldMessage.totalBytes;
            if (length != 0) {
                //length = strlen(isoMessage[i].valueField);
                packDataElement(packageISO8583[i], outputElement, fieldMessage.valueField, &length);
                memcpy(resultadoIsoPackMessage.isoPackMessage + index, outputElement, length);
                index += length;
                pos = i - 1;
                *(bitMap + (pos / 8)) |= (0x80 >> (pos % 8));
            }

        }
        resultadoIsoPackMessage.totalBytes = index;
        convertIntegerToHex((resultadoIsoPackMessage.totalBytes - 2), resultadoIsoPackMessage.isoPackMessage);
        memcpy(resultadoIsoPackMessage.isoPackMessage + 2, isoHeader, 5);
        memcpy(resultadoIsoPackMessage.isoPackMessage + LEN_MSG_CODE / 2 + 2 + 5, bitMap, 8);
    }

    inicializarCampos();

    return resultadoIsoPackMessage;
}

int validateDataElement() {
    int errorCode = 1;
    int i = 0;
    ISOFieldMessage fieldMessage;
    memset(&fieldMessage, 0x00, sizeof(fieldMessage));

    for (i = 0; i < 18; i++) {
        memset(&fieldMessage, 0x00, sizeof(fieldMessage));
        fieldMessage = getISOFieldMessageISO(i);
        if (fieldMessage.totalBytes > packageISO8583[i].fieldLength) {
            errorCode = -1 * i;
            break;
        }
        //PIN
        if (fieldMessage.field == 52 && fieldMessage.totalBytes > 0
            && fieldMessage.totalBytes < packageISO8583[52].fieldLength) {
            errorCode = -52;
            break;
        }
    }

    return errorCode;
}

void packDataElement(FieldDescription packageISO8583,uChar *outputElement, uChar * inputElement,
                     int *inLength) {

    int length = 0;
    char tam[4 + 1];
    uChar bufferAux[1300 + 1];

    length = *inLength;

    memset(bufferAux, 0x00, sizeof(bufferAux));
    memset(outputElement, 0x00, sizeof(outputElement));
    memcpy(bufferAux, inputElement, length);

    switch (packageISO8583.dataType) {
        case BCD_NUMERIC:
            if (packageISO8583.fieldLength % 2 == 0) {
                memset(bufferAux, 0x00, sizeof(bufferAux));
                _leftPad_(bufferAux, inputElement, '0', packageISO8583.fieldLength);
                //printf("\n %s - %d",bufferAux,packageISO8583.fieldLength);
            } else {
                memset(bufferAux, 0x00, sizeof(bufferAux));
                _leftPad_(bufferAux, inputElement, '0', packageISO8583.fieldLength + 1);
            }
            length = strlen(bufferAux);
            _convertASCIIToBCD_(outputElement, bufferAux, length);
            *inLength = (packageISO8583.fieldLength + 1) / 2;
            break;
        case BCD_LL_NUMERIC:
            if (length % 2 != 0) {
                memset(bufferAux, 0x00, sizeof(bufferAux));
                _rightPad_(bufferAux, inputElement, 'F', length + 1);
            }
            _convertASCIIToBCD_(outputElement + 1, bufferAux, strlen(inputElement));
            memset(tam, 0x00, sizeof(tam));
            if (packageISO8583.fieldId == 35 && length > packageISO8583.fieldLength) { //track2
                length = packageISO8583.fieldLength;
            }
            sprintf(tam, "%02d", length);
            /*convertASCIIToBCD(outputElement + 1,bufferAux,
             strlen(inputElement));*/
            _convertASCIIToBCD_(outputElement, tam, strlen(tam));
            *inLength = (length + 1) / 2 + 1; // + 1 para el byte de tamaño
            break;
        case BCD_AMOUNT:

            break;
        case BCD_CHAR:
            // Si el campo 42 viene completo de 15 se omite el rigpad por inconvenientes de
            // valores de bcd.
            if (packageISO8583.fieldId == 42 && packageISO8583.fieldLength == 15) {
                memcpy(outputElement, inputElement, 15);
            } else {
                _rightPad_(outputElement, inputElement, ' ', packageISO8583.fieldLength);
            }

            *inLength = packageISO8583.fieldLength;
            break;
        case BCD_LL_CHAR:
            //numericToChar(length, 1, outputElement);
            memset(tam, 0x00, sizeof(tam));
            sprintf(tam, "%02d", length);
            _convertASCIIToBCD_(outputElement, tam, 2); // revisar
            memcpy(outputElement + 1, inputElement, length);
            *inLength = length + 1; // + 1 para el byte de tamaño
            break;
        case BCD_LLL_CHAR:
            //numericToChar(length, 2, outputElement);
            memset(tam, 0x00, sizeof(tam));
            sprintf(tam, "%04d", length);
            _convertASCIIToBCD_(outputElement, tam, 4); //REVISAR
            memcpy(outputElement + 2, inputElement, length);
            *inLength = length + 2; // + 2 para el byte de tamaño
            break;

        case BCD_BINARY:
            memcpy(outputElement, inputElement, length);
            *inLength = packageISO8583.fieldLength;
            break;

    }

}

void convertIntegerToHex(int integer,uChar * buffer) {
    buffer[0] = integer / 256;
    buffer[1] = integer - (buffer[0] * 256);
}

void inicializarCampos(void) {

    int idx = 0;

    indiceActual = 0;
    countField = 0;

    //memset(&resultUnpack,0x00,sizeof(resultUnpack));
    //memset(isoHeader,0x00,sizeof(isoHeader));
    //memset(messageInputISO,0x00,sizeof(messageInputISO));

    for (idx = 0; idx < 18; idx++) {
        memset(&isoMessage[idx], 0x00, sizeof(isoMessage[idx]));
    }

}

ResultISOUnpack unpackISOMessage(uChar * messageISO, int inLength) {

    int i = 0;
    int index = 0;
    int length = 0;
    int uiBitmap = 8;
    int pos = 0;
    int idx = 0;
    int totalBytes = 0;
    uChar outputElement[LEN_BUFFER_MAX * 2 + 1];
    uChar bitMap[8 + 1];
    //ResultISOUnpack resultUnpack;
    ISOFieldMessage isoFieldMessage;

    memset(&resultUnpack, 0x00, sizeof(resultUnpack));
    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(outputElement, 0x00, sizeof(outputElement));
    memset(bitMap, 0x00, sizeof(bitMap));
    memset(messageInputISO, 0x00, sizeof(messageInputISO));

    memcpy(messageInputISO, messageISO, inLength);
    lengthMessage = inLength;

    index += LEN_HEADER;
    index += LEN_SIZEM;

    //MTI
    length = packageISO8583[0].fieldLength;
    totalBytes = unpackDataElement(packageISO8583[0], outputElement, messageISO + index, &length);

    index += length;

    memcpy(bitMap, messageISO + index, uiBitmap);
    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    index += uiBitmap;
    resultUnpack.totalField = 0;
    for (i = 1; i < TOTAL_FIELD; i++) {
        if (*(bitMap + i / 8) & (0x80 >> i % 8)) {
            pos = i + 1;
            resultUnpack.listField[idx++] = pos;
            resultUnpack.totalField++;
        }
    }
    resultUnpack.responseCode = 1;

    inicializarCampos();

    return resultUnpack;
}

ISOFieldMessage getField(int field) {

    int i = 0;
    int index = 0;
    int length = 0;
    int uiBitmap = 8;
    int pos = 0;
    int totalBytes = 0;
    uChar outputElement[LEN_BUFFER_MAX * 2 + 1];
    unsigned char bitMap[8 + 1];

    ISOFieldMessage isoFieldMessage;

    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(outputElement, 0x00, sizeof(outputElement));
    memset(bitMap, 0x00, sizeof(bitMap));

    index += LEN_HEADER;
    index += LEN_SIZEM;

    //MTI
    length = packageISO8583[0].fieldLength;

    totalBytes = unpackDataElement(packageISO8583[0], outputElement, messageInputISO + index, &length);

    index += length;
    memcpy(bitMap, messageInputISO + index, uiBitmap);
    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    index += uiBitmap;

    for (i = 1; i < TOTAL_FIELD; i++) {
        //memset(&isoFieldMessage,0x00,sizeof(isoFieldMessage));
        //totalBytes = 0;
        if (*(bitMap + i / 8) & (0x80 >> i % 8)) {
            pos = i + 1;
            length = packageISO8583[pos].fieldLength;
            memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));

            totalBytes = unpackDataElement(packageISO8583[pos], outputElement, messageInputISO + index, &length);
            isoFieldMessage.totalBytes = totalBytes;
            memcpy(isoFieldMessage.valueField, outputElement, isoFieldMessage.totalBytes);
            index += length;
            if (pos == field){
                break;

            }
        }
    }
    return isoFieldMessage;
}

int unpackDataElement(FieldDescription packageISO8583, uChar *outputElement, uChar * inputElement,
                      int * inLength) {

    int length = 0;
    int totalBytes = 0;
    uChar bufferAux[(LEN_BUFFER_MAX * 2) + 1];
    memset(bufferAux, 0x00, sizeof(bufferAux));
    memset(outputElement, 0x00, sizeof(outputElement));
    memcpy(bufferAux, inputElement, sizeof(inputElement));

    length = *inLength;
    totalBytes = length;

    switch (packageISO8583.dataType) {
        case BCD_NUMERIC:
            memset(bufferAux, 0x00, sizeof(bufferAux));
            if (packageISO8583.fieldLength % 2 == 0) {
                _convertBCDToASCII_(outputElement, inputElement, packageISO8583.fieldLength);
            } else {
                _convertBCDToASCII_(bufferAux, inputElement, packageISO8583.fieldLength + 1);
                memcpy(outputElement, bufferAux + 1, packageISO8583.fieldLength);
            }
            outputElement[packageISO8583.fieldLength] = 0x00;
            *inLength = (packageISO8583.fieldLength + 1) / 2;
            totalBytes = packageISO8583.fieldLength;
            break;
        case BCD_LL_NUMERIC:
            memset(bufferAux, 0x00, sizeof(bufferAux));
            _convertBCDToASCII_(bufferAux, inputElement, 2);
            length = atoi(bufferAux);
            memset(bufferAux, 0x00, sizeof(bufferAux));
            _convertBCDToASCII_(bufferAux, inputElement + 1, length);
            memcpy(outputElement, bufferAux, length);
            outputElement[length] = 0x00;
            *inLength = (length + 1) / 2 + 1; // + 1 para el byte de tamaño
            totalBytes = length;
            break;
        case BCD_AMOUNT:
            ;
            break;
        case BCD_CHAR:
            memcpy(outputElement, inputElement, packageISO8583.fieldLength);
            outputElement[packageISO8583.fieldLength] = 0x00;
            *inLength = packageISO8583.fieldLength;
            totalBytes = packageISO8583.fieldLength;
            break;
        case BCD_LL_CHAR:
            memset(bufferAux, 0x00, sizeof(bufferAux));
            _convertBCDToASCII_(bufferAux, inputElement, 2);
            length = atoi(bufferAux);
            memset(bufferAux, 0x00, sizeof(bufferAux));
            memcpy(outputElement, inputElement + 1, length);
            outputElement[length] = 0x00;
            *inLength = length + 1; // + 1 para el byte de tamaño
            totalBytes = length;
            break;
        case BCD_LLL_CHAR:
            memset(bufferAux, 0x00, sizeof(bufferAux));
            _convertBCDToASCII_(bufferAux, inputElement, 4);
            length = atoi(bufferAux);
            memset(bufferAux, 0x00, sizeof(bufferAux));
            memcpy(outputElement, inputElement + 2, length);
            outputElement[length] = 0x00;
            *inLength = length + 2; // + 2 para el byte de tamaño
            totalBytes = length;
            break;

        case BCD_BINARY:
            memcpy(outputElement, inputElement, length);
            outputElement[length] = 0x00;
            //*inLength = packageISO8583.fieldLength;
            totalBytes = length;
            break;

    }

    return totalBytes;

}

ResultTokenPack _packTokenMessage_(void) {

    int idx = 0;
    int index = 0;
    uChar outputToken[LEN_BUFFER + 1];
    char buffer[4 + 1];
    char bufferAux[4 + 1];
    ResultTokenPack resultTokenPack;

    memset(outputToken, 0x00, sizeof(outputToken));
    memset(&resultTokenPack, 0x00, sizeof(resultTokenPack));

    //cantidad de tokens 2 bytes ascci
    sprintf(outputToken + index, "%02d", countToken);
    index += 2; // longitud de cantidad de tokens

    for (idx = 0; idx < countToken; idx++) {
        memset(buffer, 0x00, sizeof(buffer));
        memset(bufferAux, 0x00, sizeof(bufferAux));

        //token n 2 bytes ascci "QI"
        sprintf(outputToken + index, "%s", tokenMessage[idx].token);
        index += 2;
        //longitud token n 2 bytes BCD
        sprintf(bufferAux, "%04d", tokenMessage[idx].totalBytes);
        _convertASCIIToBCD_(buffer, bufferAux, 4);
        memcpy(outputToken + index, buffer, 2);
        index += 2;
        //Data token n n bytes
        memcpy(outputToken + index, tokenMessage[idx].valueToken, tokenMessage[idx].totalBytes);
        index += tokenMessage[idx].totalBytes;

    }

    if (countToken > 0) {
        resultTokenPack.responseCode = 1;
        resultTokenPack.totalBytes = index;
        memcpy(resultTokenPack.tokenPackMessage, outputToken, resultTokenPack.totalBytes + 2 + 2);
    }

    inicializarMensajeriaTokens();

    return resultTokenPack;

}

void inicializarMensajeriaTokens(void) {

    int idx = 0;

    countToken = 0;
    indiceActualToken = 0;

    for (idx = 0; idx < MAX_TOKEN; idx++) {
        memset(&tokenMessage[idx], 0x00, sizeof(tokenMessage[idx]));
    }
}

void _setTokenMessage_(char * token, uChar * valueToken, int totalBytes) {

    TokenMessage tokenMsg;
    int id = 0;

    if (totalBytes > 0) {
        memset(tokenMsg.valueToken, 0x00, sizeof(tokenMsg.valueToken));
        //obtiene el id/indice donde guardara la estructura de datos.
        id = getIdToken(token);
        tokenMsg.totalBytes = totalBytes;
        memcpy(tokenMsg.token, token, strlen(token));
        memcpy(tokenMsg.valueToken, valueToken, tokenMsg.totalBytes);
        tokenMessage[id] = tokenMsg;
    }

}

int getIdToken(char * token) {

    int id = 0;
    int length = 0;
    TokenMessage tokenMsg;

    memset(&tokenMsg, 0x00, sizeof(tokenMsg));
    //revisa si ya esta creado/seteado el campo especifico
    tokenMsg = _getTokenMessage_(token);
    length = tokenMsg.totalBytes;
    if (length == 0) {
        id = countToken++; // no estba seteado previamente
    } else {
        id = indiceActualToken; //estaba seteado, entonces se devuelve el id donde fue seteado
    }

    return id;
}

TokenMessage _getTokenMessage_(char *token) {

    int idx = 0;
    int acumulado = 2;
    char buffer[4 + 1];
    char tokenLocal[2 + 1];
    int totalBytes = 0;
    uChar valueToken[LEN_BUFFER + 1];
    char bufferAux[4 + 1];
    TokenMessage tokenMessage;

    memset(&tokenMessage, 0x00, sizeof(tokenMessage));
    LOGI("Entrada a bucle for %d", lengthMessageToken);
    for (idx = 0; idx < lengthMessageToken; idx += acumulado) {

        memset(buffer, 0x00, sizeof(buffer));
        memset(bufferAux, 0x00, sizeof(bufferAux));
        memset(valueToken, 0x00, sizeof(valueToken));
        memset(tokenLocal, 0x00, sizeof(tokenLocal));
        totalBytes = 0;
        memcpy(tokenLocal, messageInputToken + acumulado, 2); // tag
        LOGI("tokenLocal %s", tokenLocal);
        acumulado += 2;
        memcpy(bufferAux, messageInputToken + acumulado, 2); // tamaño
        acumulado += 2;
        LOGI("bufferAux %s", bufferAux);
        //convertir a ascci y a entero
        _convertBCDToASCII_(buffer, bufferAux, 4);
        LOGI("bufferAschi %s", buffer);

        if (strcmp(buffer, "3030") != 0 && atoi(buffer) < 2500 ){
            totalBytes = atoi(buffer);
            if (totalBytes > 0) {
                LOGI("totalBytes %d", totalBytes);
                memcpy(valueToken, messageInputToken + acumulado, totalBytes);
                acumulado += totalBytes;
            }
        }


        if (strcmp(tokenLocal, token) == 0) {
            LOGI("  Encontro token");
            memcpy(tokenMessage.token, tokenLocal, 2); // tag
            tokenMessage.totalBytes = totalBytes;
            memcpy(tokenMessage.valueToken, valueToken, tokenMessage.totalBytes);
            break;
        }

    }

    return tokenMessage;

}

ResultTokenUnpack unpackTokenMessage(uChar * messageToken, int inLength) {

    int idx = 0;
    int acumulado = 2;
    int length = 0;
    char buffer[4 + 1];
    char bufferAux[4 + 1];
    ResultTokenUnpack resultTokenUnpack;

    memset(&resultTokenUnpack, 0x00, sizeof(resultTokenUnpack));
    memset(buffer, 0x00, sizeof(buffer));
    memset(messageInputToken, 0x00, sizeof(messageInputToken));

    memcpy(buffer, messageToken, 2);
    memcpy(messageInputToken, messageToken, inLength);
    countToken = atoi(buffer);
    lengthMessageToken = inLength;

    for (idx = 0; idx < lengthMessageToken; idx += acumulado) {

        memset(buffer, 0x00, sizeof(buffer));
        memset(bufferAux, 0x00, sizeof(bufferAux));
        acumulado += 2; //token
        memcpy(bufferAux, messageInputToken + acumulado, 2);
        acumulado += 2; // tamaño
        //convertir a ascci y a entero
        _convertBCDToASCII_(buffer, bufferAux, 4);
        length = atoi(buffer);
        acumulado += length;

    }

    if (acumulado == inLength) {
        resultTokenUnpack.responseCode = 1;
        resultTokenUnpack.totalToken = countToken;
    }

    inicializarMensajeriaTokens();

    return resultTokenUnpack;

}

ISOFieldMessage getMTI() {

    int index = 0;
    int length = 0;
    int totalBytes = 0;
    uChar outputElement[LEN_BUFFER_MAX + 1];
    uChar bitMap[8 + 1];
    ISOFieldMessage isoFieldMessage;

    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(outputElement, 0x00, sizeof(outputElement));
    memset(bitMap, 0x00, sizeof(bitMap));

    index += LEN_HEADER;
    index += LEN_SIZEM;

    //MTI
    length = packageISO8583[0].fieldLength;
    totalBytes = unpackDataElement(packageISO8583[0], outputElement, messageInputISO + index, &length);

    isoFieldMessage.totalBytes = totalBytes;
    memcpy(isoFieldMessage.valueField, outputElement, isoFieldMessage.totalBytes);

    return isoFieldMessage;
}

TokenMessage getTokenMessage(char * token) {

    int idx = 0;
    TokenMessage tokenMsg;
    indiceActualToken = -1;

    memset(&tokenMsg, 0x00, sizeof(tokenMsg));

    for (idx = 0; idx < MAX_TOKEN; idx++) {
        if (strcmp(tokenMessage[idx].token, token) == 0) {
            tokenMsg = tokenMessage[idx];
            indiceActualToken = idx; // indice donde se encontro.
            break;
        }
    }

    return tokenMsg;
}

void limpiarTokenMessage(){
    lengthMessageToken = 0;
}