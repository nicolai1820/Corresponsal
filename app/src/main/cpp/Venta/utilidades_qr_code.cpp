//
// Created by ronald on 10/04/2020.
//


#include "DEFINICIONES.h"
#include "Utilidades.h"
#include "configuracion.h"
#include "Archivos.h"

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
#include "Mensajeria.h"
#include "Inicializacion.h"
#include "android/log.h"
#include "Tipo_datos.h"
#include "native-lib.h"
#include "comunicaciones.h"
#include "Definiciones.h"
#include "venta.h"
#include "android/log.h"

#define  LOG_TAG    "NETCOM_QRCODE_UTILITIES"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  PROCESINGCODE				"008800"
#define  PROCESINGCODE_RBM			"320000"
#define IMAGEN_PANTALLA		2
int b64invs[]={ 62, -1, -1, -1, 63, 52, 53, 54, 55, 56, 57, 58,
                59, 60, 61, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5,
                6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                21, 22, 23, 24, 25, -1, -1, -1, -1, -1, -1, 26, 27, 28,
                29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
                43, 44, 45, 46, 47, 48, 49, 50, 51 };

const char b64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/**
 * @brief	muestra en pantalla o en ticket la imagen QR
 * @param 	dataQr, datos con la información de la imagen QR
 * @author
 */
int mostrarQR(uChar *dataQr) {

    int tamModulos = 0;
    int tamImagen = 0;
    int iRet = 0;
    int resultadoQR = 0;
    uChar tamChar[2 + 1];
    uChar imagen[25000];

    memset(tamChar, 0x00, sizeof(tamChar));
    memcpy(tamChar, dataQr, 2);
    LOGI("mostrarQR %s ", tamChar);
    tamModulos = atoi(tamChar);

    memset(imagen, 0x00, sizeof(imagen));

    tamImagen = crearImagenBMP(dataQr, imagen, tamModulos, IMAGEN_PANTALLA);

     iRet = pintarImagenQr(imagen, tamImagen);

    return iRet;
}


int pintarImagenQr(uChar *imagen, int tamImagen) {
    LOGI("pintar  QR");
    int retorno = 0;
    uChar data[20000];
    memset(data, 0x00, sizeof(data));
    sprintf(data, "%05d", tamImagen);
    memcpy(data + 5, imagen, tamImagen);

    LOGI("escribir qrcode %d",tamImagen);
    retorno = escribirArchivo(discoNetcom,(char *)"/qrcode.bmp",imagen ,tamImagen);
    //ojo = 1//
    return retorno;
}

int crearImagenBMP(uChar *cadBase64, uChar *imagen, int tamModulos, int tipoSalida) {

    int idxCadenaSalida = 0;
    int pixelsModulo = 0;

    switch (tipoSalida) {

        case IMAGEN_PANTALLA:
            if (tamModulos > 53) {
                pixelsModulo = 2;
            } else {
                pixelsModulo = 4;
            }
            break;
        default:

            break;
    }
    int sizeRow = ((tamModulos * pixelsModulo) + 31) / 32;
    sizeRow = sizeRow * 4;
    LOGI("borrar file qr");
    borrarArchivo(discoNetcom,(char *)"/qrcode.bmp");
    idxCadenaSalida = crearEncabezadoBMP(imagen, tamModulos, tipoSalida);

    idxCadenaSalida = generarQrcodeEmvCo(tamModulos * pixelsModulo , sizeRow, pixelsModulo, imagen,idxCadenaSalida, cadBase64 + 2);

    return idxCadenaSalida;
}

int crearEncabezadoBMP(uChar *imagen, int cantidadModulos, int tipoSalida) {

    static int tamHeader = 62;
    int factor = 0;

    switch (cantidadModulos) {
        case 49:
        case 53:
            factor = 4;
            break;
        case 81:
        case 77:
            factor = 2;
            break;
        default:
            factor = 2;
            break;
    }

    int sizeRow = ((cantidadModulos * factor) + 31) / 32;
    sizeRow = sizeRow * 4;
    unsigned char img[sizeRow * (cantidadModulos * factor)];
    /////////////////////////////
    memset(img, 0x00, sizeof(img));

    unsigned char header[] = { 0x42, 0x4d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x28, 0x00,
                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00 };

    if ((cantidadModulos * factor) > 0xff) {
        header[18] = (cantidadModulos * factor) - 256;
        header[19] = (cantidadModulos * factor) / 256;
        header[22] = (cantidadModulos * factor) - 256;
        header[23] = (cantidadModulos * factor) / 256;
    } else {
        header[18] = cantidadModulos * factor;
        header[22] = cantidadModulos * factor;
    }

    header[2] = (sizeof(img) + 62) - 256;
    header[3] = (sizeof(img) + 62) / 256;
    header[34] = (sizeof(img)) - 256;
    header[35] = (sizeof(img)) / 256;

    memcpy(imagen, header, tamHeader);
    return tamHeader;
}

int conversionCaracterBitAscii(uChar caracter, uChar *bitAscii) {

    int ret = 0;
    int numero = 0;

    ret = decodificarDato(caracter, &numero);

    if (ret > 0) {
        ret = convertirDecimalBitAscii(numero, bitAscii);
    }

    return ret;
}

int decodificarDato(uChar caracter, int *num) {

    int ret = 1;

    if (caracter >= 'A' && caracter <= 'Z') {
        *num = (unsigned int) caracter - 0x41;
    } else if (caracter >= 'a' && caracter <= 'z') {
        *num = (unsigned int) caracter - 0x47;
    } else if (caracter >= '0' && caracter <= '9') {
        *num = (unsigned int) caracter + 0x04;
    } else if (caracter == '+') {
        *num = (unsigned int) caracter + 0x13;
    } else if (caracter == '/') {
        *num = (unsigned int) caracter + 0x10;
    } else {
        ret = 0;
    }

    return ret;
}

int convertirDecimalBitAscii(int num, uChar *bitAscii) {

    int retorno = 0;
    uChar bit[6 + 1];
    int num2 = 64;
    int i = 0;

    memset(bit, 0x00, sizeof(bit));

    if (num >= 0 && num <= 63) {
        retorno = 1;

        for (i = 0; i < 6; i++) {
            num2 = i != 5 ? num2 / 2 : 1;
            bit[i] = num / num2 > 0 ? '1' : '0';
            num = num % num2;
        }

        memcpy(bitAscii, bit, 6);
    }
    return retorno;
}

int b64_decoded_size(uChar *in, int intLen) {
    size_t len;
    size_t ret;
    size_t i;

    if (in == NULL)
        return 0;

    len = intLen;
    ret = len / 4 * 3;

    for (i = len; i-- > 0;) {
        if (in[i] == '=') {
            ret--;
        } else {
            break;
        }
    }

    return ret;
}


void b64_generate_decode_table() {
    int inv[80];
    size_t i;

    memset(inv, -1, sizeof(inv));
    for (i = 0; i < sizeof(b64chars) - 1; i++) {
        inv[b64chars[i] - 43] = i;
    }
}

int b64_isvalidchar(char c) {
    if (c >= '0' && c <= '9')
        return 1;
    if (c >= 'A' && c <= 'Z')
        return 1;
    if (c >= 'a' && c <= 'z')
        return 1;
    if (c == '+' || c == '/' || c == '=')
        return 1;
    return 0;
}

int b64_decode(uChar *in, int lenInt, unsigned char *out, size_t outlen) {
    size_t len;
    size_t i;
    size_t j;
    int v;

    if (in == NULL || out == NULL) {
        return 0;
    }

    len = lenInt;
    if (outlen < b64_decoded_size(in, lenInt) || len % 4 != 0) {

        return 0;
    }

    for (i = 0; i < len; i++) {
        if (!b64_isvalidchar(in[i])) {
            return 0;
        }
    }

    //b64_generate_decode_table();

    for (i = 0, j = 0; i < len; i += 4, j += 3) {
        v = b64invs[in[i] - 43];
        v = (v << 6) | b64invs[in[i + 1] - 43];
        v = in[i + 2] == '=' ? v << 6 : (v << 6) | b64invs[in[i + 2] - 43];
        v = in[i + 3] == '=' ? v << 6 : (v << 6) | b64invs[in[i + 3] - 43];

        out[j] = (v >> 16) & 0xFF;
        if (in[i + 2] != '=')
            out[j + 1] = (v >> 8) & 0xFF;
        if (in[i + 3] != '=')
            out[j + 2] = v & 0xFF;
    }

    return 1;
}

/**
 * base64_decode - Base64 decode
 * @src: Data to be decoded
 * @len: Length of the data to be decoded
 * @out_len: Pointer to output length variable
 * Returns: Allocated buffer of out_len bytes of decoded data,
 * or %NULL on failure
 *
 * Caller is responsible for freeing the returned buffer.
 */

static const unsigned char base64_table[65] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


unsigned int base64_decode(unsigned char *src, int len, uChar *out) {
    unsigned char dtable[256];
    uChar *pos, block[4], dataAscii[3], tmp;
    int i, count;
    int pad = 0;
    int res = -1;

    memset(dtable, 0x80, 256);
    for (i = 0; i < sizeof(base64_table) - 1; i++)
        dtable[base64_table[i]] = (unsigned char) i;
    dtable['='] = 0;

    count = 0;
    for (i = 0; i < len; i++) {
        if (dtable[src[i]] != 0x80)
            count++;
    }

    if (count == 0 || count % 4) {

        return res;
    }

    pos = out;

    count = 0;
    int index = 0;
    for (i = 0; i < len; i++) {

        tmp = dtable[src[i]];
        if (tmp == 0x80) {
            continue;
        }

        if (src[i] == '=')
            pad++;
        block[count] = tmp;

        count++;
        if (count == 4) {
            dataAscii[0] = (block[0] << 2) | (block[1] >> 4);
            dataAscii[1] = (block[1] << 4) | (block[2] >> 2);
            dataAscii[2] = (block[2] << 6) | block[3];
            count = 0;

            memcpy(out + index, dataAscii, 3);
            index += 3;
            if (pad) {
                if (pad == 1) {
                    pos--;
                    res = 1;
                } else if (pad == 2) {
                    pos -= 2;
                    res = 2;
                } else {
                    return res;
                }
                break;
            }
        }
    }
    res = 0;
    return res;
}
bool getBit(int ID, int position) {
    return ((ID >> position) & 1);
}

int setBit(int position) {
    int ret = 0;
    switch (position) {
        case 7:
            ret = 0x80;
            break;
        case 6:
            ret = 0x40;
            break;
        case 5:
            ret = 0x20;
            break;
        case 4:
            ret = 0x10;
            break;
        case 3:
            ret = 0x08;
            break;
        case 2:
            ret = 0x04;
            break;
        case 1:
            ret = 0x02;
            break;
        case 0:
            ret = 0x01;
            break;
        default:
            ret = 0xff;
            break;
    }
    return (ret);
}

int map(int sizeX, int sizeRow, uChar *buff, int inputLen, int factor,
        uChar *outData) {

    uChar res[(sizeRow * sizeX)];

    memset(res, 0x00, sizeof(res));
    int tamImagen = (sizeRow * sizeX) + 62;

    int pos = 0;
    int k = 0;
    int r = 0;
    int u = 0;
    int j = 0;

    int posOut = 0;
    int bitOut = 7;
    int counter = 0;
    int countrow = 0;

    for ( pos = 0; pos < inputLen; pos++) {
        for ( k = 7; k >= 0; k--) {
            if (posOut >= sizeof(res)) {
                break;
            }
            char val = getBit(buff[pos], k);

            counter++;
            if (val > 0) {
                for ( r = 0; r < factor; r++) {
                    res[posOut] += (char) setBit(bitOut);
                    bitOut--;
                    if (bitOut < 0) {
                        bitOut = 7;
                        posOut++;
                        countrow++;
                    }
                }

            } else {
                for ( r = 0; r < factor; r++) {
                    res[posOut] += 0x00;
                    bitOut--;
                    if (bitOut < 0) {
                        bitOut = 7;
                        posOut++;
                        countrow++;
                    }
                }
            }

            if ((counter) >= (sizeX / factor)) {
                posOut++;
                for (u = countrow; u < (sizeRow - 1); u++) {
                    res[posOut++] = 0;
                }
                countrow = 0;
                counter = 0;
                bitOut = 7;
                //bucle de lineas eje Y
                for ( r = 0; r < factor - 1; r++) {
                    memcpy(&res[posOut], &res[posOut - sizeRow], sizeRow);
                    posOut += sizeRow;
                }
            }
        }

    }

    memset(outData, 0x00, sizeof(res));
    //se debe intercambiar las lineas del buffer
    int l = sizeof(res) - sizeRow;
    for ( j = 0; j < sizeof(res); j += sizeRow) {
        memcpy(outData + j, &res[l], sizeRow);
        l -= sizeRow;
    }

    return tamImagen;
}

int generarQrcodeEmvCo(int sizeX, int sizeRow, int factor,uChar * img, int tamimagen, uChar * source) {

    int tamscr = strlen(source);

    int sizeImg = b64_decoded_size(source, tamscr);
    uChar bytesImg[sizeImg];
    int res = base64_decode((unsigned char*)source, tamscr, bytesImg);

    tamimagen = map(sizeX, sizeRow, bytesImg, sizeImg-res, factor, img + tamimagen);
    tamimagen += 28;

    return tamimagen;
}


int direccionamientoQRcode(char * terminal, char * nii, char * processingCode, char *posEntrymode , char * codigoComercio ) {
    LOGI("entro");
    int resultado = 0;
    int tipoQrCode = 0;
    DatosQrCodeEmvco datosQr;
    DatosIniQrCode datosQrRbm;
    DatosTerminalQr datoTerminal;

    memset(&datosQr, 0x00, sizeof(datosQr));
    memset(&datosQrRbm, 0x00, sizeof(datosQrRbm));
    memset(&datoTerminal, 0x00, sizeof(datoTerminal));
    LOGI("tipoQrCode %d ",tipoQrCode);

    tipoQrCode = verificarTipoQrcode();
    if (tipoQrCode == IS_QRCODE_EMVCO) {
        LOGI("is EMVCO");
        datosQr = capturarDatosQrCode();
        getParameter(NUMERO_TERMINAL, terminal);
        if(strcmp(processingCode, "008900")  == 0){
            strcpy(nii, datosQr.niiTransaccional);
        } else {
            strcpy(nii, datosQr.niiGeneracionQr);
        }
        if (strcmp(processingCode, "008900") != 0  && strcmp(processingCode, "028900") != 0) {
            memcpy(processingCode, PROCESINGCODE, TAM_CODIGO_PROCESO);
        }
        memcpy(posEntrymode, "030", TAM_ENTRY_MODE);
        getParameter(CODIGO_COMERCIO, codigoComercio);
    } else {
        LOGI("is RBM");
        datosQrRbm = capturarDatosQrCodeRbm();
        datoTerminal = capturarDatoTerminal();
        strcpy(nii, datosQrRbm.niiQr);
        strcpy(terminal, datosQrRbm.terminal);
        if (strcmp(processingCode, "008900") != 0  && strcmp(processingCode, "028900") != 0) {
            memcpy(processingCode, PROCESINGCODE_RBM, TAM_CODIGO_PROCESO);
        } else {
            memcpy(processingCode, "330000", TAM_CODIGO_PROCESO);
        }
        memcpy(posEntrymode, "021", TAM_ENTRY_MODE);
        memcpy(codigoComercio, datoTerminal.codigoComercio, TAM_ID_COMERCIO);
    }

    return resultado;
}

int verificarTipoQrcode() {

    int tipoQr = IS_QRCODE_RBM;
    char valQr[2 + 1];

    memset(valQr, 0x00, sizeof(valQr));

    getParameter(VERSION_QRCODE, valQr);
    LOGI("valQr %s ",valQr);
    if (atoi(valQr) == 1) {
        tipoQr = IS_QRCODE_EMVCO;
    }
    return tipoQr;
}

