//
// Created by NETCOM on 15/01/2020.
//

#include "Utilidades.h"
#include "Archivos.h"
#include <string>
#include "netinet/in.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "android/log.h"
#include "configuracion.h"
#include "Inicializacion.h"
#include "native-lib.h"
#include "Mensajeria.h"

#define  LOG_TAG    "NETCOM_UTILIDADES"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define SEPARADOR '.'

void _rightPad_(char *destination, char *source, char fill, int len) {

    int longitud = 0;

    memset(destination, 0x00, sizeof(destination));
    strcpy(destination, source);
    longitud = strlen(source);

    if (longitud < len) {
        memset(destination, fill, len);
        memcpy(destination, source, longitud);
    }
}

void leftPad(uChar *destination, uChar *source, char fill, int len) {

    int longitud = 0;
    int indice = 0;

    memset(destination, 0x00, sizeof(destination));
    strcpy(destination, source);
    longitud = strlen(source);

    if (longitud < len) {
        indice = len - longitud;
        memset(destination, fill, len);
        memcpy(destination + indice, source, longitud);
    }
}

void _convertASCIIToBCD_(uChar *destination, uChar *source, int length) {

    char byte = 0;
    int counter = 0;
    unsigned char pointer = 0;

    for (counter = 0; counter < length; counter += 2) {
        byte = source[counter];
        destination[pointer] = (unsigned char) (byte - ((byte <= '9') ? '0' : '7')) << 4;

        byte = source[counter + 1];
        destination[pointer++] |= (unsigned char) (byte - ((byte <= '9') ? '0' : '7'));
    }

}


void _convertBCDToASCII_(uChar *destination, uChar *source, int length) {

    char byte = 0;
    int counter = 0;
    unsigned char pointer = 0;

    for (counter = 0; counter < length; counter += 2) {
        byte = (source[pointer] & 0xF0) >> 4;
        destination[counter] = (unsigned char) (byte + ((byte < 10) ? '0' : '7'));
        byte = source[pointer++] & 0x0F;
        destination[counter + 1] = (unsigned char) (byte + ((byte < 10) ? '0' : '7'));
    }

}

void leftTrim(uChar *destino, uChar *origen, uChar caracter) {

    int indice = 0;
    int contador = 0;
    int tamSring = 0;
    memset(destino, 0x00, sizeof(destino));

    tamSring = strlen(origen);

    for (indice = 0; indice < tamSring; indice++) {
        if (origen[indice] == caracter) {
            contador++;
        } else {
            break;
        }
    }

    memcpy(destino, origen + contador, (tamSring - contador));
}

void _rightTrim_(uChar *destino, uChar *origen, uChar caracter) {

    int indice = 0;
    int tamString = 0;
    memset(destino, 0x00, sizeof(destino));

    tamString = strlen(origen);

    for (indice = (tamString - 1); indice >= 0; indice--) {
        if (origen[indice] == caracter) {
            origen[indice] = 0x00;
        } else {
            strcpy(destino, origen);
            break;
        }
    }
}

void generarInvoice(char *invoice) {

    int contador = 0;
    char dataStrace[6 + 1];
    char dataStraceN[6 + 1];

    memset(dataStrace, 0x00, sizeof(dataStrace));
    memset(dataStraceN, 0x00, sizeof(dataStraceN));
    memset(invoice, 0x00, sizeof(invoice));

    getParameter(INVOICE, dataStrace);
    contador = atoi(dataStrace);
    contador++;
    if (contador >= 999999) {
        contador = 1;
    }
    memset(dataStrace, 0x00, sizeof(dataStrace));
    sprintf(dataStrace, "%d", contador);
    leftPad(dataStraceN, dataStrace, 0x30, 6);
    setParameter(INVOICE, dataStraceN);

    memcpy(invoice, dataStraceN, 6);
}

void formatString(int caracter, long monto, uChar *cad_origen, int tam, uChar *cad_destino,
                  int formato) {

    static uChar temp[50];
    int i, j, k, tam2;

    memset(temp, 0x00, 50);
    if (caracter != 0x00) {
        j = 0;
        for (i = 0; i < tam; i++)
            if (cad_origen[i] != caracter)
                break;
        tam = tam - i;
        memcpy(temp, &cad_origen[i], tam);

    } else
        memcpy(temp, cad_origen, tam);

    if (formato == 8) // si e formato es alfanumérico lo dejo igual a "0= no aplica"
        formato = 0;

    switch (formato) {
        case 0:
            memcpy(cad_destino, temp, tam);
            break;
        case 1:
            if (monto >= 0 && tam == 0) {
                memset(temp, 0x00, sizeof(temp));
                sprintf(temp, "%ld", monto);
                tam = strlen(temp);
            }
            i = tam / 3; // Calcúlo la cantidad de puntos de mil que se van a agregar
            if (i * 3 == tam)
                i--;
            k = 0; // Es el tamaño+2 porque hay que agregar tambien el signo pesos y un espacio en blanco
            cad_destino[tam + 3] = 0x00;
            for (j = tam - 1; j >= 0; j--) {
                cad_destino[j + i + 2] = temp[j];
                k++;
                if ((k / 3) * 3 == k) {
                    i--;
                    cad_destino[j + i + 2] = '.';
                }
            }
            cad_destino[0] = '$';
            cad_destino[1] = ' ';
            break;

        case 2:
            if (monto >= 0 && tam == 0) {
                memset(temp, 0x00, sizeof(temp));
                sprintf(temp, "%ld", monto);
                tam = strlen(temp);
            }
            i = tam / 3; // Calcúlo la cantidad de puntos de mil que se van a agregar
            if (i * 3 == tam)
                i--;
            k = 0;
            cad_destino[tam + 1] = 0x00;
            for (j = tam - 1; j >= 0; j--) {
                cad_destino[j + i] = temp[j];
                k++;
                if ((k / 3) * 3 == k) {
                    i--;
                    cad_destino[j + i] = '.';
                }
            }
            break;
        case 3:
            sprintf((char *) cad_destino, "%.4s/%.2s/%.2s", &cad_origen[0], &cad_origen[4],
                    &cad_origen[6]);
            break;

        case 4:
            sprintf((char *) cad_destino, "%.4s/%.2s/%.2s %.2s:%.2s:%.2s", &cad_origen[0],
                    &cad_origen[4], &cad_origen[6],
                    &cad_origen[8], &cad_origen[10], &cad_origen[12]);
            break;

        case 5:
            memset(cad_destino, '*', strlen(cad_origen));
            break;

        case 6:
            if (monto >= 0 && tam == 0) {
                memset(temp, 0x00, sizeof(temp));
                sprintf(temp, "%ld", monto);
                tam = strlen(temp);
            }
            i = (tam - 2) / 3; // Calcúlo la cantidad de puntos de mil que se van a agregar
            if (i * 3 == (tam - 2))
                i--;
            k = 0; // Es el tamaño+2 porque hay que agregar tambien el signo pesos y un espacio en blanco

            if (tam > 2) {

                for (j = tam - 1 - 2; j >= 0; j--) {
                    cad_destino[j + i + 2] = temp[j];
                    k++;
                    if ((k / 3) * 3 == k) {
                        i--;
                        cad_destino[j + i + 2] = '.';
                    }
                }
                cad_destino[0] = '$';
                cad_destino[1] = ' ';
                tam2 = strlen(cad_destino);
                cad_destino[tam2] = ',';
                cad_destino[tam2 + 1] = temp[tam - 2];
                cad_destino[tam2 + 2] = temp[tam - 1];
                cad_destino[tam2 + 3] = 0x00;
            } else {
                cad_destino[2] = '0';
                cad_destino[3] = ',';
                switch (tam) {
                    case 0:
                        memset(&cad_destino[4], '0', 2);
                        break;
                    case 1:
                        sprintf(&cad_destino[4], "0%s", temp);
                        break;
                    case 2:
                        sprintf(&cad_destino[4], "%s", temp);
                        break;
                }
            }
            cad_destino[0] = '$';
            cad_destino[1] = ' ';
            break;

        case 7:
            if (monto >= 0 && tam == 0) {
                memset(temp, 0x00, sizeof(temp));
                sprintf(temp, "%ld", monto);
                tam = strlen(temp);
            }
            i = (tam - 2) / 3;
            if (i * 3 == (tam - 2))
                i--;
            k = 0;

            if (tam > 2) {

                for (j = tam - 1 - 2; j >= 0; j--) {
                    cad_destino[j + i] = temp[j];
                    k++;
                    if ((k / 3) * 3 == k) {
                        i--;
                        cad_destino[j + i] = '.';
                    }
                }
                tam2 = strlen(cad_destino);
                cad_destino[tam2] = ',';
                cad_destino[tam2 + 1] = temp[tam - 2];
                cad_destino[tam2 + 2] = temp[tam - 1];
                cad_destino[tam2 + 3] = 0x00;
            } else {
                cad_destino[0] = '0';
                cad_destino[1] = ',';
                switch (tam) {
                    case 0:
                        memset(&cad_destino[2], '0', 2);
                        break;
                    case 1:
                        sprintf(&cad_destino[2], "0%s", temp);
                        break;
                    case 2:
                        sprintf(&cad_destino[2], "%s", temp);
                        break;
                }
            }

            break;

        case 9:
            memcpy(cad_destino, temp, tam);
            if (strlen(cad_origen) > 1)
                memset(cad_destino, '*', strlen(cad_origen) - 1);
            break;
    }
}

void formatoFecha(char *fecha) {

    char nombreMes[3 + 1];
    char dia[2 + 1];
    char mes[2 + 1];
    char year[4 + 1];
    Date date;
    memset(nombreMes, 0x00, sizeof(nombreMes));
    memset(dia, 0x00, sizeof(dia));

    sprintf(mes, "%.2s", fecha);
    sprintf(dia, "%.2s", fecha + 2);
    memset(&date, 0x00, sizeof(date));
    memset(year, 0x00, sizeof(year));

    date = obtenerDate();

    memcpy(year, fecha + 4, 2);

    if (atoi(year) == 0) {
        memcpy(year, date.year, 2);
    }

    switch (atoi(mes)) {
        case 1:
            strcpy(nombreMes, "ENE");
            break;

        case 2:
            strcpy(nombreMes, "FEB");
            break;

        case 3:
            strcpy(nombreMes, "MAR");
            break;

        case 4:
            strcpy(nombreMes, "ABR");
            break;

        case 5:
            strcpy(nombreMes, "MAY");
            break;

        case 6:
            strcpy(nombreMes, "JUN");
            break;

        case 7:
            strcpy(nombreMes, "JUL");
            break;

        case 8:
            strcpy(nombreMes, "AGO");
            break;

        case 9:
            strcpy(nombreMes, "SEP");
            break;

        case 10:
            strcpy(nombreMes, "OCT");
            break;

        case 11:
            strcpy(nombreMes, "NOV");
            break;

        case 12:
            strcpy(nombreMes, "DIC");
            break;
        default:
            break;
    }

    memset(fecha, 0x00, sizeof(fecha));
    sprintf(fecha, "%s %s 20%.2s", nombreMes, dia, year);
}


void generarBatch(char *batch) {

    int contador = 0;
    char dataStrace[6 + 1];
    char dataStraceN[6 + 1];

    memset(dataStrace, 0x00, sizeof(dataStrace));
    memset(dataStraceN, 0x00, sizeof(dataStraceN));
    memset(batch, 0x00, sizeof(batch));

    getParameter(BATCH, dataStrace);
    contador = atoi(dataStrace);
    contador++;
    if (contador >= 999999) {
        contador = 1;
    }
    memset(dataStrace, 0x00, sizeof(dataStrace));
    sprintf(dataStrace, "%d", contador);
    _leftPad_(dataStraceN, dataStrace, 0x30, 6);
    setParameter(BATCH, dataStraceN);

    memcpy(batch, dataStraceN, 6);
}

void errorRespuestaTransaccion(char *codigoRespuesta, char *mensajeError) {

    ParametersGUI datoMensaje;
    int tipoCodigoRespuesta = 0;
    int rechazo = 0;
    char tipoTransaccion[2 + 1];

    memset(&datoMensaje, 0x00, sizeof(datoMensaje));
    memset(tipoTransaccion, 0x00, sizeof(tipoTransaccion));

    tipoCodigoRespuesta = atoi(codigoRespuesta);

    /** Titulo Default **/
    strcpy(datoMensaje.titulo, "MENSAJE");
    /** Time Out Default **/
    datoMensaje.timeOut = 3000;

    if (tipoCodigoRespuesta != 0) { // Codigo de respuesta numerico
        switch (tipoCodigoRespuesta) {
            case 1:
                strcpy(datoMensaje.message1, "LLAME");
                strcpy(datoMensaje.message2, "AL");
                strcpy(datoMensaje.message3, "EMISOR");
                break;
            case 2:
                strcpy(datoMensaje.message1, "LLAMAR");
                strcpy(datoMensaje.message2, "REF");
                strcpy(datoMensaje.message3, "");
                break;
            case 3:
                strcpy(datoMensaje.message1, "MARCA");
                strcpy(datoMensaje.message2, "NO");
                strcpy(datoMensaje.message3, "HABILITADA");
                break;
            case 5:
                strcpy(datoMensaje.message1, "RECHAZO");
                strcpy(datoMensaje.message2, "GENERAL");
                strcpy(datoMensaje.message3, "");
                break;
            case 8:
                strcpy(datoMensaje.message1, "APROBADO");
                strcpy(datoMensaje.message2, "");
                strcpy(datoMensaje.message3, "");
                break;
            case 11:
                strcpy(datoMensaje.message1, "CLAVE INVALIDA");
                strcpy(datoMensaje.message2, "GENERE UNA");
                strcpy(datoMensaje.message3, "NUEVA CLAVE");
                break;
            case 12:
                strcpy(datoMensaje.message1, "TRANSACCION");
                strcpy(datoMensaje.message2, "INVALIDA");
                strcpy(datoMensaje.message3, "");
                break;
            case 13:
                strcpy(datoMensaje.message1, "MONTO");
                strcpy(datoMensaje.message2, "INVALIDO");
                strcpy(datoMensaje.message3, "");
                break;
            case 14:
                strcpy(datoMensaje.message1, "TARJETA");
                strcpy(datoMensaje.message2, "INVALIDA");
                strcpy(datoMensaje.message3, "");
                break;
            case 15:
                strcpy(datoMensaje.message1, "TERMINAL");
                strcpy(datoMensaje.message2, "INVALIDO");
                strcpy(datoMensaje.message3, "");
                break;
            case 19:
                strcpy(datoMensaje.message1, "REINICIE");
                strcpy(datoMensaje.message2, "TRANSACCION");
                strcpy(datoMensaje.message3, "");
                break;
            case 25:
                strcpy(datoMensaje.message1, "NO");
                strcpy(datoMensaje.message2, "EXISTE");
                strcpy(datoMensaje.message3, "COMPROBANTE");
                break;
            case 30:
                strcpy(datoMensaje.message1, "FORMATO");
                strcpy(datoMensaje.message2, "INVALIDO");
                strcpy(datoMensaje.message3, "");
                break;
            case 31:
                strcpy(datoMensaje.message1, "TARJETA");
                strcpy(datoMensaje.message2, "NO SOPORTADA");
                strcpy(datoMensaje.message3, "");
                break;
            case 41: // SE PROPAGA
            case 43:
                strcpy(datoMensaje.message1, "RETENGA");
                strcpy(datoMensaje.message2, "Y");
                strcpy(datoMensaje.message3, "LLAME");
                break;
            case 51:
                strcpy(datoMensaje.message1, "FONDOS");
                strcpy(datoMensaje.message2, "INSUFICIENTES");
                strcpy(datoMensaje.message3, "");
                break;
            case 54:
                strcpy(datoMensaje.message1, "TARJETA");
                strcpy(datoMensaje.message2, "VENCIDA");
                strcpy(datoMensaje.message3, "");
                break;
            case 55:
                strcpy(datoMensaje.message1, "PIN");
                strcpy(datoMensaje.message2, "INVALIDO");
                strcpy(datoMensaje.message3, "");
                break;
            case 57:
                strcpy(datoMensaje.message1, "TRANSACCION");
                strcpy(datoMensaje.message2, "NO");
                strcpy(datoMensaje.message3, "PERMITIDA");
                break;
            case 58:
                strcpy(datoMensaje.message1, "TERMINAL");
                strcpy(datoMensaje.message2, "NO");
                strcpy(datoMensaje.message3, "HABILITADO");
                break;
            case 61:
                strcpy(datoMensaje.message1, "EXCEDE");
                strcpy(datoMensaje.message2, "MONTO");
                strcpy(datoMensaje.message3, "LIMITE");
                break;
            case 62:
                strcpy(datoMensaje.message1, "TARJETA");
                strcpy(datoMensaje.message2, "USO");
                strcpy(datoMensaje.message3, "RESTRINGIDO");
                break;
            case 65:
                strcpy(datoMensaje.message1, "EXCEDE");
                strcpy(datoMensaje.message2, "USO");
                strcpy(datoMensaje.message3, "DIA");
                break;
            case 66:
                strcpy(datoMensaje.message1, "BONO");
                strcpy(datoMensaje.message2, "NO");
                strcpy(datoMensaje.message3, "HABILITADO");
                break;
            case 67:
                strcpy(datoMensaje.message1, "BONO");
                strcpy(datoMensaje.message2, "BLOQUEADO");
                strcpy(datoMensaje.message3, "");
                break;
            case 68:
                strcpy(datoMensaje.message1, "BONO");
                strcpy(datoMensaje.message2, "VENCIDO");
                strcpy(datoMensaje.message3, "");
                break;
            case 69:
                strcpy(datoMensaje.message1, "BONO");
                strcpy(datoMensaje.message2, "YA");
                strcpy(datoMensaje.message3, "HABILITADO");
                break;
            case 73:
                strcpy(datoMensaje.message1, "REVERSO");
                strcpy(datoMensaje.message2, "PENDIENTE");
                strcpy(datoMensaje.message3, "");
                break;
            case 74:
                strcpy(datoMensaje.message1, "CEDULA");
                strcpy(datoMensaje.message2, "INVALIDA");
                strcpy(datoMensaje.message3, "");
                break;
            case 75:
                strcpy(datoMensaje.message1, "EXCEDE");
                strcpy(datoMensaje.message2, "INTENTOS");
                strcpy(datoMensaje.message3, "DE PIN");
                break;
            case 76:
                strcpy(datoMensaje.message1, "LLAMAR/ERROR");
                strcpy(datoMensaje.message2, "DC");
                strcpy(datoMensaje.message3, "");
                break;
            case 77:
                strcpy(datoMensaje.message1, "ERROR");
                strcpy(datoMensaje.message2, "EN");
                strcpy(datoMensaje.message3, "CIERRE");
                break;
            case 78:
                strcpy(datoMensaje.message1, "CARGO");
                strcpy(datoMensaje.message2, "NO");
                strcpy(datoMensaje.message3, "ENCONTRADO");
                break;
            case 79:
                strcpy(datoMensaje.message1, "LOTE");
                strcpy(datoMensaje.message2, "YA");
                strcpy(datoMensaje.message3, "ABIERTO");
                break;
            case 80:
                strcpy(datoMensaje.message1, "FACTURA");
                strcpy(datoMensaje.message2, "NO");
                strcpy(datoMensaje.message3, "EXISTE");
                break;
            case 81:
                strcpy(datoMensaje.message1, "FACTURA");
                strcpy(datoMensaje.message2, "YA");
                strcpy(datoMensaje.message3, "PAGADA");
                break;
            case 82:
                strcpy(datoMensaje.message1, "FACTURA");
                strcpy(datoMensaje.message2, "VENCIDA");
                strcpy(datoMensaje.message3, "");
                break;
            case 83:
                strcpy(datoMensaje.message1, "SERVICIO");
                strcpy(datoMensaje.message2, "NO");
                strcpy(datoMensaje.message3, "PERMITIDO");
                break;
            case 84:
                strcpy(datoMensaje.message1, "TARJETA");
                strcpy(datoMensaje.message2, "NO");
                strcpy(datoMensaje.message3, "AUTORIZADA");
                break;
            case 85:
                strcpy(datoMensaje.message1, "FACTURA");
                strcpy(datoMensaje.message2, "NO");
                strcpy(datoMensaje.message3, "PAGADA");
                break;
            case 86:
                strcpy(datoMensaje.message1, "CUENTA");
                strcpy(datoMensaje.message2, "INVALIDA");
                strcpy(datoMensaje.message3, "");
                break;
            case 87:
                strcpy(datoMensaje.message1, "EXCEDE");
                strcpy(datoMensaje.message2, "MONTO");
                strcpy(datoMensaje.message3, "DIARIO");
                break;
            case 89:
                strcpy(datoMensaje.message1, "NUMERO");
                strcpy(datoMensaje.message2, "TERMINAL");
                strcpy(datoMensaje.message3, "INVALIDO");
                break;
            case 91:
                strcpy(datoMensaje.message1, "IMPOSIBLE");
                strcpy(datoMensaje.message2, "AUTORIZAR");
                strcpy(datoMensaje.message3, "");
                break;
            case 92:
                strcpy(datoMensaje.message1, "CANCELADO");
                strcpy(datoMensaje.message2, "POR");
                strcpy(datoMensaje.message3, "USUARIO");
                break;
            case 94:
                strcpy(datoMensaje.message1, "LLAMAR/ERROR");
                strcpy(datoMensaje.message2, "SQ");
                strcpy(datoMensaje.message3, "");
                break;
            case 95:
                strcpy(datoMensaje.message1, "ESPERE");
                strcpy(datoMensaje.message2, "TRANSMITIENDO");
                strcpy(datoMensaje.message3, "");
                break;
            case 96:
                strcpy(datoMensaje.message1, "LLAME");
                strcpy(datoMensaje.message2, "96");
                strcpy(datoMensaje.message3, "");
                break;
            case 97:
                strcpy(datoMensaje.message1, "NO");
                strcpy(datoMensaje.message2, "EXISTE");
                strcpy(datoMensaje.message3, "PRE-AUTORIZACION");
                break;
            case 98:
                strcpy(datoMensaje.message1, "");
                strcpy(datoMensaje.message2, "PRE-AUTORIZACION");
                strcpy(datoMensaje.message3, "VENCIDA");
                break;
            default:
                rechazo = 1;
                break;
        }
    } else {
        getParameter(TIPO_TRANSACCION, tipoTransaccion);

        if (atoi(tipoTransaccion) == TRANSACCION_RECARGAR_BONO ||
            atoi(tipoTransaccion) == TRANSACCION_ANULACION) {
            if (strcmp(codigoRespuesta, "B1") == 0) {
                strcpy(datoMensaje.message1, "CONVENIO");
                strcpy(datoMensaje.message2, "NO");
                strcpy(datoMensaje.message3, "EXISTE");
            } else if (strcmp(codigoRespuesta, "B2") == 0) {
                strcpy(datoMensaje.message1, "CONVENIO");
                strcpy(datoMensaje.message2, "NO ASOCIADO");
                strcpy(datoMensaje.message3, "AL COMERCIO");
            } else if (strcmp(codigoRespuesta, "B3") == 0) {
                strcpy(datoMensaje.message1, "CONVENIO");
                strcpy(datoMensaje.message2, "NO PERMITE");
                strcpy(datoMensaje.message3, "RECARGAS DE BONOS");
            } else if (strcmp(codigoRespuesta, "B4") == 0) {
                strcpy(datoMensaje.message1, "TARJETA NO");
                strcpy(datoMensaje.message2, "ASOCIADA AL CONVENIO");
                strcpy(datoMensaje.message3, "SELECCIONADO");
            } else if (strcmp(codigoRespuesta, "B5") == 0) {
                strcpy(datoMensaje.message1, "PREFIJO DE TARJETA");
                strcpy(datoMensaje.message2, "DESTINO NO ASOCIADA");
                strcpy(datoMensaje.message3, "AL CONVENIO");
            } else if (strcmp(codigoRespuesta, "B6") == 0) {
                strcpy(datoMensaje.message1, "SUPERA NUMERO");
                strcpy(datoMensaje.message2, "MAXIMO DE");
                strcpy(datoMensaje.message3, "TRANSACCIONES DIARIAS");
            } else if (strcmp(codigoRespuesta, "B7") == 0) {
                strcpy(datoMensaje.message1, "SUPERA EL VALOR");
                strcpy(datoMensaje.message2, "MAXIMO DE LA");
                strcpy(datoMensaje.message3, "TRANSACCION");
            } else if (strcmp(codigoRespuesta, "B8") == 0) {
                strcpy(datoMensaje.message1, "TARJETA");
                strcpy(datoMensaje.message2, "MATRIZ");
                strcpy(datoMensaje.message3, "NO EXISTE");
            } else {
                rechazo = 1;
            }
        } else if (atoi(tipoTransaccion) == TRANSACCION_CODIGO_ESTATICO) {
            if (strcmp(codigoRespuesta, "B7") == 0) {
                strcpy(datoMensaje.message1, "TIEMPO DE");
                strcpy(datoMensaje.message2, "ESPERA");
                strcpy(datoMensaje.message3, "AGOTADO");
            } else if (strcmp(codigoRespuesta, "B9") == 0) {
                strcpy(datoMensaje.message1, "CODIGO");
                strcpy(datoMensaje.message2, "EXPIRADO");
                strcpy(datoMensaje.message3, "");
            } else if (strcmp(codigoRespuesta, "P2") == 0) {
                strcpy(datoMensaje.message1, "CODIGO");
                strcpy(datoMensaje.message2, "YA");
                strcpy(datoMensaje.message3, "CONSULTADO");
            } else {
                rechazo = 1;
            }
        } else if (atoi(tipoTransaccion) == TRANSACCION_CONSULTA_QR) {
            if (strcmp(codigoRespuesta, "B1") == 0) {
                strcpy(datoMensaje.message1, "TRANSACCION");
                strcpy(datoMensaje.message2, "PENDIENTE POR");
                strcpy(datoMensaje.message3, "APROBAR");
            } else if (strcmp(codigoRespuesta, "C0") == 0) {
                strcpy(datoMensaje.message1, "INTENTE");
                strcpy(datoMensaje.message2, "NUEVAMENTE");
                strcpy(datoMensaje.message3, "");
            } else {
                rechazo = 1;
            }
        } else {
            rechazo = 1;
        }
    }

    if (rechazo == 1) {
        strcpy(datoMensaje.message1, "RECHAZO");
        strcpy(datoMensaje.message2, "GENERAL");
        strcpy(datoMensaje.message3, "");
    }

    sprintf(mensajeError, "%s %s %s", datoMensaje.message1, datoMensaje.message2,
            datoMensaje.message3);
    enviarStringToJava(mensajeError,(char *) "showScreenMessage");
}

int verificarHabilitacionCaja(void) {

    int respuesta = 0;
    char auxiliarRespuesta[3 + 1];

    memset(auxiliarRespuesta, 0x00, sizeof(auxiliarRespuesta));
    getParameter(CAJA_REGISTRADORA, auxiliarRespuesta);
    respuesta = ((atoi(auxiliarRespuesta) == TRUE) ? 1 : -1);

    return respuesta;
}

int isIssuerEspecial(char *issuer) {
    int resultado = 0;

    if (strncmp(issuer, T_EXITO, strlen(T_EXITO)) == 0) {
        resultado = 1;
    } else if (strncmp(issuer, T_CMR, strlen(T_CMR)) == 0) {
        resultado = 1;
    } else if (strncmp(issuer, T_MASTERCRD, strlen(T_MASTERCRD)) == 0) {
        resultado = 1;
    } else if (strncmp(issuer, T_CMR_MASTER, strlen(T_CMR_MASTER)) == 0) {
        resultado = 1;
    } else if (strncmp(issuer, T_TARJ_TCO, strlen(T_TARJ_TCO)) == 0) {
        resultado = 1;
    } else if (strncmp(issuer, T_TUYA, strlen(T_TUYA)) == 0) {
        resultado = 1;
    } else if (strncmp(issuer, T_TUYA_MC, strlen(T_TUYA_MC)) == 0) {
        resultado = 1;
    } else if (strncmp(issuer, T_ALKOSTO, strlen(T_ALKOSTO)) == 0) {
        resultado = 1;
    } else if (strncmp(issuer, T_ALKOSTO_MC, strlen(T_ALKOSTO_MC)) == 0) {
        resultado = 1;
    }

    return resultado;
}

DatosOperacion dividir(unsigned long dividendo, unsigned long divisor, int cifras, int redondear) {

    DatosOperacion resultadoDivision;
    unsigned long parteEntera;
    unsigned long residuo = 0;
    unsigned long auxDividendo = 0;
    unsigned long auxParteEntera = 0;

    int i = 0;
    char salida[50];
    char decimales[50];

    parteEntera = dividendo / divisor;
    residuo = dividendo % divisor;
    auxDividendo = residuo * 10;

    memset(salida, 0x00, sizeof(salida));
    memset(decimales, 0x00, sizeof(decimales));
    memset(&resultadoDivision, 0x00, sizeof(resultadoDivision));

    for (i = 0; i < cifras + 1; i++) {
        auxParteEntera = auxDividendo / divisor;
        residuo = auxDividendo % divisor;
        if (auxParteEntera == 0) {
            auxDividendo *= 10;
        }
        auxDividendo = residuo * 10;

        sprintf(decimales + i, "%ld", auxParteEntera);

    }

    if (redondear == TRUE) {
        redondearNumero(&parteEntera, decimales, cifras);
    }

    if (cifras > 0) {
        sprintf(salida, "%ld%c%s", parteEntera, SEPARADOR, decimales);
    } else {
        sprintf(salida, "%ld", parteEntera);
    }

    strcpy(resultadoDivision.cociente, salida);
    strcpy(resultadoDivision.parteDecimal, decimales);
    resultadoDivision.divideno = dividendo;
    resultadoDivision.divisor = divisor;
    resultadoDivision.cifras = cifras;

    return resultadoDivision;

}

void redondearNumero(unsigned long *parteEntera, char decimales[], int cifras) {

    char buffer[50];
    char suma[2];
    int digito;
    int i;

    memset(suma, 0x00, sizeof(suma));
    memset(buffer, 0x00, sizeof(buffer));
    for (i = strlen(decimales); i < strlen(decimales) - cifras; i++) {
        decimales[i] = '0';
    }
    digito = decimales[cifras] - 0x30;

    if (digito >= 5 && cifras == 0) {
        *parteEntera = *parteEntera + 1;
    }
    if (digito >= 5 && cifras > 0) {
        digito = decimales[cifras - 1] - 0x30;
        digito++;
        sprintf(suma, "%02d", digito);
        buffer[cifras - 1] = suma[1];
        for (i = cifras - 2; i >= 0; i--) {
            digito = decimales[i] - 0x30;
            digito = digito + suma[0] - 0x30;
            sprintf(suma, "%02d", digito);
            buffer[i] = suma[1];
        }
        *parteEntera = *parteEntera + suma[0] - 0x30;
    } else {
        memcpy(buffer, decimales, cifras);
    }

    memset(decimales, 0x00, sizeof(decimales));
    strcpy(decimales, buffer);

}

int verificarHabilitacionObjeto(int Objeto) {

    int respuesta = 0;
    char auxiliarRespuesta[3 + 1];
    memset(auxiliarRespuesta, 0x00, sizeof(auxiliarRespuesta));
    getParameter(Objeto, auxiliarRespuesta);
    respuesta = ((atoi(auxiliarRespuesta) == TRUE) ? 1 : -1);

    return respuesta;
}

int justificarPalabra(char *palabraIn, char *palabraOut, int numeroPalabra, int maximoCaracteres) {

    char textoDividir[512 + 1];
    char token[] = " ";
    char *ptr;
    char lineaImprimir[maximoCaracteres + 1];
    int longitud = 0;
    int indice = 0;
    int resultado = -1;

    memset(lineaImprimir, 0x00, sizeof(lineaImprimir));
    memset(textoDividir, 0x00, sizeof(textoDividir));

    strcpy(textoDividir, palabraIn);
    ptr = stringtok(textoDividir, token);
    memcpy(lineaImprimir, ptr, strlen(ptr));

    while ((ptr = stringtok(NULL, token)) != NULL) {
        longitud = strlen(lineaImprimir) + strlen(ptr) + 1;
        if (longitud <= maximoCaracteres) {
            strcat(lineaImprimir, " ");
            strcat(lineaImprimir, ptr);
        } else {
            indice++;
            if (numeroPalabra == indice)
                break;

            memset(lineaImprimir, 0x00, sizeof(lineaImprimir));
            strcat(lineaImprimir, ptr);
        }

    }

    indice++;

    if (numeroPalabra <= indice) {
        strcpy(palabraOut, lineaImprimir);
        resultado = 1;
    }

    return resultado;

}

char *stringtok(char *str, const char *delim) {
    static char *p = 0;
    if (str)
        p = str;
    else if (!p)
        return 0;
    str = p + strspn(p, delim);
    p = str + strcspn(str, delim);
    if (p == str)
        return p = 0;
    p = *p ? *p = 0, p + 1 : 0;
    return str;
}

void validarFallBack(DatosVenta *datosVenta, DatosTarjeta *leerTrack) {

    int idx = 0;
    int isObligadoBanda = 0;

    //isObligadoBanda = leerTrack->configuracionExcepciones.comportamientoExcepciones1.obligaBanda;// ORIGINAL

    /////////////// ES FALL BACK ? ///////////////
    if (((leerTrack->isFallBack == 1) && (leerTrack->isChip == 1))
        || ((leerTrack->isFallBack == 1) && (isObligadoBanda != FALSE))) {
        strcpy(datosVenta->posEntrymode, "801");
    } else {
        strcpy(datosVenta->posEntrymode, "021");
    }

    memcpy(datosVenta->fechaExpiracion, leerTrack->fechaExpiracion, 4);
    idx = strlen(leerTrack->pan);
    LOGI("idx  A  %d ", idx);
    idx -= 4;
    LOGI("idx  B  %d ", idx);
    memcpy(datosVenta->ultimosCuatro, leerTrack->pan + idx, 4);
    LOGI("ultimos 4   %s ", datosVenta->ultimosCuatro);
    strcpy(datosVenta->cardName, leerTrack->dataIssuer.cardName);
//    validarTipotarjeta(leerTrack->dataIssuer.issuerNumber, datosVenta);
    memcpy(datosVenta->track2, leerTrack->track2, strlen(leerTrack->track2));

    strcpy(datosVenta->tarjetaHabiente, leerTrack->nombreTarjetaHabiente);

    /////////////// CODIGO DEL PROCESO POR DEFAULT ///////////////
    leerTrack->dataIssuer.defaultAccount *= 10;
    sprintf(datosVenta->tipoCuenta, "%02d", leerTrack->dataIssuer.defaultAccount);
    sprintf(datosVenta->processingCode, "00%02d00", leerTrack->dataIssuer.defaultAccount);
}

DatosTarjeta detectarTrack2(char *track2) {

    int n = 0;
    int existeEmisor = 0;
    int longitudTrack = 0;

    char tipoTarjeta = 0x00;
    char bin[20 + 1] = {0x00};
    uChar parametroFallBack[1 + 1] = {0x00};
    uChar fallBackFalabella[1 + 1] = {0x00};
    uChar idBono[SIZE_ID_BONOS + 1] = {0x00};

    DatosTarjeta datosTarjeta;
    EmisorAdicional emisorAdicional;
    TablaTresInicializacion dataIssuer;

    memset(&dataIssuer, 0, sizeof(dataIssuer));
    memset(&datosTarjeta, 0, sizeof(datosTarjeta));
    memset(&emisorAdicional, 0, sizeof(emisorAdicional));

    if ((strlen(track2) - 3) > 38) {
        longitudTrack = 38;
    } else {
        longitudTrack = (strlen(track2) - 3);
    }
    memcpy(datosTarjeta.track2, track2 + 1, longitudTrack);

    //sacar el pan de la tarjeta
    for (n = 0; n < strlen(datosTarjeta.track2); n++) {
        if (datosTarjeta.track2[n] == 'D' || datosTarjeta.track2[n] == 'd') {
            break;
        }
    }
    memcpy(datosTarjeta.fechaExpiracion, datosTarjeta.track2 + n + 1, 4);

    ///revisar si la tarjeta es chip
    tipoTarjeta = datosTarjeta.track2[n + 5];
    LOGI("tipoTarjeta  %d ", tipoTarjeta);
    if (tipoTarjeta == '2' || tipoTarjeta == '6') {
        datosTarjeta.isChip = 1;
    } else {
        datosTarjeta.isChip = 0;
    }
    LOGI("n  %d ", n);
    LOGI("datosTarjeta.track2  %s ", datosTarjeta.track2);
    memcpy(datosTarjeta.pan, datosTarjeta.track2, n);
    memset(track2, 0x00, sizeof(track2));
    memcpy(bin, datosTarjeta.pan, 9);

    long binIssuer = atol(bin);

    ////// VERIFICAR SI LA TARJETA PERTENECE A BONOS /////
    dataIssuer = _traerIssuer_(binIssuer);
    if (strncmp(dataIssuer.cardName, "BONOS", 5) == 0) {

        datosTarjeta.isBonos = 2;

        getParameter(ID_BONOS, idBono);

        memset(track2, 0x00, sizeof(track2));
        _convertBCDToASCII_(track2, &dataIssuer.issuerTableId,
                            SIZE_ID_BONOS);

        if (atoi(idBono) == atoi(track2)) {
            datosTarjeta.isBonos = 1;
        } else {
            datosTarjeta.dataIssuer = dataIssuer;
        }
    }

    if (datosTarjeta.isBonos == 0) {
        if (dataIssuer.respuesta == 1) {
            existeEmisor = buscarBinEmisor(bin);
            if (existeEmisor > 0) {
                emisorAdicional = traerEmisorAdicional(existeEmisor);
                if (emisorAdicional.existeIssuerEmisor == 1) {
                    memset(dataIssuer.cardName, 0x00, sizeof(dataIssuer.cardName));
                    memcpy(dataIssuer.cardName, emisorAdicional.nombreEmisor, 15);
                    datosTarjeta.isEmisorAdicional = 1;
                }
            }
            datosTarjeta.respuesta = _TRUE_;
            datosTarjeta.dataIssuer = dataIssuer;

        }
    }

    /////// se ajusta para solo coloque datos tarjeta respuesta en true
    if (dataIssuer.respuesta > 0) {

        datosTarjeta.respuesta = _TRUE_;
        /////// VERIFICAR EXCEPCIONES DE EMV ////////
        if (datosTarjeta.isChip == 1) {
            datosTarjeta.configuracionExcepciones = excepcionesEmv(binIssuer);

            /**
             * se ajusta para que se tomen en cuenta las excepciones solo si no es fallBack
             */
            getParameter(MODO_FALLBACK, parametroFallBack);
            getParameter(FALLBACK_FALABELLA, fallBackFalabella);

            if (strcmp(parametroFallBack, "0") == 0) {
                if (datosTarjeta.configuracionExcepciones.comportamientoExcepciones1.obligaBanda
                    || strcmp(fallBackFalabella, "1") == 0) {
                    datosTarjeta.isChip = 0;
                } else {
                    datosTarjeta.respuesta = FALSE;
                    datosTarjeta.isChip = 1;
                }
            }
        } else {

            datosTarjeta.configuracionExcepciones = excepcionesEmv(binIssuer);
        }
    }
    return datosTarjeta;
}

int comportamientoTarjetaLabelEMV(DatosVenta *datosVenta, TablaTresInicializacion dataIssuer) {

    char buffer[TAM_CARD_NAME + 1];
    int existeEmisor = 0;
    EmisorAdicional emisorAdicional;
    char bin[20 + 1];

    memset(&emisorAdicional, 0x00, sizeof(emisorAdicional));
    memset(bin, 0x00, sizeof(bin));
    memset(buffer, 0x00, sizeof(buffer));
    memset(datosVenta->cardName, 0x00, sizeof(datosVenta->cardName));
    if (strncmp(datosVenta->appLabel, "VISA ELEC", 9) == 0) {
        memset(datosVenta->appLabel, 0x00, sizeof(datosVenta->appLabel));
        strcpy(datosVenta->appLabel, "V.ELECTRON");

    }
    if (strlen(datosVenta->appLabel) > 10) {
        memcpy(buffer, datosVenta->appLabel, 10);
        memset(datosVenta->appLabel, 0x00, sizeof(datosVenta->appLabel));
        memcpy(datosVenta->appLabel, buffer, 10);
    }

    strcpy(datosVenta->cardName, dataIssuer.cardName);
    strcpy(datosVenta->cardName, dataIssuer.cardName);
    memcpy(bin, datosVenta->track2, 9);
    existeEmisor = buscarBinEmisor(bin);

    if (existeEmisor > 0) {
        emisorAdicional = traerEmisorAdicional(existeEmisor);
        if (emisorAdicional.existeIssuerEmisor == 1) {
            memset(datosVenta->cardName, 0x00, sizeof(datosVenta->cardName));
            memcpy(datosVenta->cardName, emisorAdicional.nombreEmisor, 15);
        }
    }
    return 1;
}

int verificarUsuario(uChar *usuario, uChar *datosUsuario) {

    uChar informacionUsuario[30 + 1];
    uChar nombreUsuario[12 + 1];
    long lengthArchivo = 0;
    int existeArchivo = 0;
    int resultado = NO_EXISTE_USUARIO;
    int numberIssuer = 0;
    int indice = 0;
    int posicion = 0;

    ////// VERIFICAR SI EXISTE EL ARCHIVO ///////
    existeArchivo = verificarArchivo(discoNetcom, ISSUER_CNB);

    if (existeArchivo == FS_OK) {
        lengthArchivo = tamArchivo(discoNetcom, ISSUER_CNB);
        numberIssuer = lengthArchivo / LENGTH_USUARIOS_CNB;
        for (indice = 0; indice < numberIssuer; indice++) {
            memset(informacionUsuario, 0x00, sizeof(informacionUsuario));
            memset(nombreUsuario, 0x00, sizeof(nombreUsuario));
            buscarArchivo(discoNetcom, ISSUER_CNB, informacionUsuario, posicion,
                          LENGTH_USUARIOS_CNB);
            memcpy(nombreUsuario, informacionUsuario, 10);
            if (strcmp(nombreUsuario, usuario) == 0) {
                memcpy(datosUsuario, informacionUsuario, LENGTH_USUARIOS_CNB);
                resultado = EXISTE_USUARIO;
                break;
            }
            posicion += LENGTH_USUARIOS_CNB;
        }
    }
    return resultado;
}

int crearUsuarioCnb(char *usuario, char *clave, char *tipoUsuario) {

    int resultado = 1;
    int existeUsuario = 0;
    int escribeArchivo = 0;
    uChar dataUsuario[30 + 1];
    uChar fecha[8 + 1];


    memset(dataUsuario, 0x00, sizeof(dataUsuario));
    memset(fecha, 0x00, sizeof(fecha));
    LOGI("Verificar usuario ");
    obtenerFecha(fecha);
    existeUsuario = verificarUsuario(usuario, dataUsuario);
    if (existeUsuario == EXISTE_USUARIO) {
        resultado = -1;
    }

    ////// GUARDAR DATOS DEL USUARIO //////
    if (resultado > 0) {
        memset(dataUsuario, 0x00, sizeof(dataUsuario));
        memcpy(dataUsuario, usuario, 10);
        memcpy(dataUsuario + 10, clave, 6);
        memcpy(dataUsuario + 16, fecha, 6);
        memcpy(dataUsuario + 22, tipoUsuario, 2);

        escribeArchivo = escribirArchivo(discoNetcom, ISSUER_CNB, dataUsuario,
                                         LENGTH_USUARIOS_CNB);
        if (escribeArchivo != LENGTH_USUARIOS_CNB) {
            resultado = -2;
        } else {
            resultado = 1;
        }
    }
    return resultado;
}

int cerrarSesion(void) {

    int resultado = 1;

    setParameter(USER_CNB_LOGON,(char*)"0");

    return resultado;
}

int usuarioAmodificarCnb(char *usuario) {

    int resultado = 1;
    int existeUsuario = 0;
    uChar dataUsuario[30 + 1];

    memset(dataUsuario, 0x00, sizeof(dataUsuario));

    if ((strcmp("REDEBAN", usuario) == 0) || (strcmp("COMERCIO", usuario) == 0)) {
        resultado = -2;
    } else {
        existeUsuario = verificarUsuario(usuario, dataUsuario);
        if (existeUsuario == NO_EXISTE_USUARIO) {
            resultado = -1;
        }
    }

    return resultado;
}

int eliminarUsuario(char * usuarioActual) {

    int resultado = 0;
    int numberIssuer = 0;
    int indice = 0;
    int posicion = 0;
    long lengthArchivo = 0;
    char informacionUsuario[24 + 1];
    char nombreUsuario[10 + 1];

    /// YA SE VERIFICO ANTERIORMENTE LA EXISTENCIA DEL ARCHIVO USUARIOS CNB
    lengthArchivo = tamArchivo(discoNetcom, ISSUER_CNB);
    numberIssuer = lengthArchivo / LENGTH_USUARIOS_CNB;

    resultado = renombrarArchivo(discoNetcom, ISSUER_CNB, RESPALDO);

    if (resultado == FS_OK) {
        borrarArchivo(discoNetcom, ISSUER_CNB);
        for (indice = 0; indice < numberIssuer; indice++) {
            memset(informacionUsuario, 0x00, sizeof(informacionUsuario));
            memset(nombreUsuario, 0x00, sizeof(nombreUsuario));
            buscarArchivo(discoNetcom, RESPALDO, informacionUsuario, posicion, LENGTH_USUARIOS_CNB);
            memcpy(nombreUsuario, informacionUsuario, 10);

            if (strcmp(nombreUsuario, usuarioActual) != 0) {
                resultado = escribirArchivo(discoNetcom, ISSUER_CNB, informacionUsuario,
                                            LENGTH_USUARIOS_CNB);
            }
            if (numberIssuer == 1) {
                resultado = LENGTH_USUARIOS_CNB;
            }
            posicion += LENGTH_USUARIOS_CNB;
        }
        borrarArchivo(discoNetcom, RESPALDO);
    } else {
        resultado = 0;

    }

    return resultado;
}

int eliminarUsuarioCnb(char *usuario) {

    int resultado = 0;
    int existeArchivo = 0;
    int existeUsuario = 0;
    int resultadoEliminar = 0;
    char usuarioActual[12 + 1];
    char clave[8 + 1];
    char datosUsuario[30 + 1];

    memset(clave, 0x00, sizeof(clave));

    ////// VERIFICAR LA EXISTENCIA DEL ARCHIVO /////////
    existeArchivo = verificarArchivo(discoNetcom, ISSUER_CNB);

    if (existeArchivo == FS_OK) {
        resultado = 1;
    }

    ////// INGRESAR USUARIO A MODIFICAR /////////
    if (resultado > 0) {
        memset(datosUsuario, 0x00, sizeof(datosUsuario));
        if (strcmp(usuario, "REDEBAN") != 0 && strcmp(usuario, "COMERCIO") != 0) {
            existeUsuario = verificarUsuario(usuario, datosUsuario);
            if (existeUsuario == NO_EXISTE_USUARIO) {
                resultado = -1;
            } else {
                resultado = 1;
            }
        } else {
            resultado = -2;
        }
    }

    ////// ELIMINAR USUARIO ///////
    if (resultado > 0) {
        resultadoEliminar = eliminarUsuario(usuario);
    }

    if (resultado > 0) {
        if (resultadoEliminar != LENGTH_USUARIOS_CNB) {
            resultado = -3;
        } else {
            resultado = 1;
        }
    }
    ////// SI NO EXISTE USUARIO MUESTRE MENSAJE ////////
    if (existeArchivo == FS_KO && resultado == 0) {
        resultado = -4;
    }

    return resultado;
}

int listadoUsuarios(void) {

    int resultado = 0;
    int existeArchivo = 0;
    int numberIssuer = 0;
    int posicion = 0;
    int indice = 0;
    int textoRecibo = 0;
    long lengthArchivo = 0;
    char informacionUsuario[40 + 1];

    ////// VERIFICAR SI EXISTE EL ARCHIVO ///////
    existeArchivo = verificarArchivo(discoNetcom, ISSUER_CNB);

    if (existeArchivo == FS_OK) {
        resultado = TRUE;
    }

    if (resultado > 0) {
        lengthArchivo = tamArchivo(discoNetcom, ISSUER_CNB);
        numberIssuer = lengthArchivo / LENGTH_USUARIOS_CNB;
//        imprimirEncabezado();

        imprimirEstablecimiento();
        textoRecibo = 1;
        for (indice = 0; indice < numberIssuer; indice++) {
            memset(informacionUsuario, 0x00, sizeof(informacionUsuario));
            buscarArchivo(discoNetcom, ISSUER_CNB, informacionUsuario, posicion, LENGTH_USUARIOS_CNB);
            textoRecibo++;
            if (indice == (numberIssuer - 1)) {
                textoRecibo = 1;
            }
            imprimirListaUsuario(informacionUsuario, textoRecibo);
            pprintf((char *) "final");
            posicion += LENGTH_USUARIOS_CNB;
        }
    }
    return resultado;
}

void imprimirListaUsuario(uChar * informacionUsuario, int textoRecibo) {

    uChar nombreUsuario[24 + 1];
    uChar fechaRegistro[8 + 1];
    uChar fechaImprimir[12 + 1];
    uChar lineaImprimir[50 + 1];
    int tipoUsuario = 0;
    int lengthNombre = 0;
    char lineaSalida[60 + 1];

    memset(lineaImprimir, 0x00, sizeof(lineaImprimir));
    memset(nombreUsuario, 0x00, sizeof(nombreUsuario));

    tipoUsuario = atoi(&informacionUsuario[22]);
    memset(lineaImprimir, ' ', 47);
    ///// NOMBRE DE USUARIO ////
    memcpy(nombreUsuario, informacionUsuario, 10);
    lengthNombre = strlen(nombreUsuario);
    memcpy(lineaImprimir, nombreUsuario, lengthNombre);
    ///// FECHA DE REGISTRO ///////
    memcpy(fechaRegistro, informacionUsuario + 16, 6);
    sprintf(fechaImprimir, "%.2s/%.2s/20%.2s", fechaRegistro + 4, fechaRegistro + 2, fechaRegistro);
    memcpy(lineaImprimir + 19, fechaImprimir, 10);
    ///// TIPO DE USUARIO ////////
    if (tipoUsuario == ADMINISTRADOR_CNB) {
        memcpy(lineaImprimir + 34, "ADMINISTRADOR", 13);
    } else {
        memcpy(lineaImprimir + 39, "OPERADOR", 8);
    }

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "%s\n", lineaImprimir);
    pprintf(lineaSalida);

    if (textoRecibo == 1) {

        memset(lineaSalida, 0x00, sizeof(lineaSalida));
        sprintf(lineaSalida, "%s\n", "-------------------------------------------------");
        pprintf(lineaSalida);

        memset(lineaSalida, 0x00, sizeof(lineaSalida));
        sprintf(lineaSalida, "%s\n", "              http://www.rbm.com.co\n");
        pprintf(lineaSalida);

        memset(lineaSalida, 0x00, sizeof(lineaSalida));
        sprintf(lineaSalida, "%s\n", "-------------------------------------------------");
        pprintf(lineaSalida);
    }
}

void imprimirticketPrueba(void) {

    int indicador = 0;
    char lineaSalida[60 + 1];

/*    while (_verificarPapel_(TRANSACCION_VENTA) != TRUE) {
        if (indicador == 0) {
            indicador = 1;
            screenMessage("MENSAJE", "NO HAY PAPEL", "COLOCAR PAPEL", "EN IMPRESORA", 10);
        }
    }*/

//    screenMessage("MENSAJE", "", "IMPRIMIENDO", "", 10);
//    imprimirLogo("0000.BMP", printer);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "%s\n", "REDEBAN MULTICOLOR");
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "%s\n", "REDEBAN MULTICOLOR");
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "%s\n", "@");
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "%s\n", "E" "REDEBAN MULTICOLOR");
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "%s\n", "@");
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "%s\n", "F" "REDEBAN MULTICOLOR");
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida,"%s\n", "@");
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "%s\n", "B1" "REDEBAN MULTICOLOR");
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "%s\n", "@");
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "%s\n\n\n\n\n", "B0" "REDEBAN MULTICOLOR");
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "%s\n", "@");
    pprintf(lineaSalida);

}

void imprimirEstablecimiento(void) {

    TablaUnoInicializacion tablaUno;
    uChar stringAux[24 + 1];
    int corrimiento = 0;
    char lineaSalida[60 + 1];

    memset(&tablaUno, 0x00, sizeof(tablaUno));
    memset(stringAux, 0x00, sizeof(stringAux));

    tablaUno = _desempaquetarTablaCeroUno_();
    memset(stringAux, 0x20, 24);

    corrimiento = (24 - strlen(tablaUno.defaultMerchantName)) / 2;
    strcpy(stringAux + corrimiento, tablaUno.defaultMerchantName);
    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "%s\n", stringAux);
    pprintf(lineaSalida);

    memset(stringAux, 0x20, 24);
    corrimiento = (24 - strlen(tablaUno.receiptLine2)) / 2;
    strcpy(stringAux + corrimiento, tablaUno.receiptLine2);
    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "%s\n", stringAux);
    pprintf(lineaSalida);

    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "%s", "                    LISTA DE USUARIOS\n\n");
    pprintf(lineaSalida);
    memset(lineaSalida, 0x00, sizeof(lineaSalida));
    sprintf(lineaSalida, "%s", " USUARIO   FECHA     TIPO\n\n");
    pprintf(lineaSalida);
}

int modificarUsuarioCnb(char *usuario, char *usuario2, char *clave, char *tipoUsuario) {

    int iRet = 0;
    int existeArchivo = 0;
    int existeUsuario = 0;
    int resultadoActualizar = TRUE;
    uChar claveActual[8 + 1];
    char datosUsuario[30 + 1];
    char fecha[8 + 1];
    char usuarioActual[12 + 1];

    memset(tipoUsuario, 0x00, sizeof(tipoUsuario));
    memset(fecha, 0x00, sizeof(fecha));
    memset(claveActual, 0x00, sizeof(claveActual));
    memset(usuarioActual, 0x00, sizeof(usuarioActual));

    obtenerFecha(fecha);
    ////// VERIFICAR LA EXISTENCIA DEL ARCHIVO /////////
    existeArchivo = verificarArchivo(discoNetcom,ISSUER_CNB);

    if (existeArchivo == FS_OK) {
        iRet = 1;
    }

    ////// INGRESAR USUARIO A MODIFICAR /////////
    if (iRet > 0) {
        existeUsuario = verificarUsuario(usuario, datosUsuario);
        if (existeUsuario == EXISTE_USUARIO) {
            iRet = -1;
        }
    }

    ////// INGRESAR CLAVE DE USUARIO /////////
    if (iRet > 0) {

        memcpy(tipoUsuario, datosUsuario + 22, 2);
        memcpy(claveActual, datosUsuario + 10, 6);

        if (strcmp(clave, claveActual) == 0) {
            iRet = -2;
        }
    }
    LOGI("tipoUsuario %s",tipoUsuario);
    ////// ACTUALIZAR ARCHIVO ////////
    if (iRet > 0) {
        memset(datosUsuario, 0x00, sizeof(datosUsuario));
        memcpy(datosUsuario, usuario, 10);
        memcpy(datosUsuario + 10, clave, 6);
        memcpy(datosUsuario + 16, fecha, 6);
        memcpy(datosUsuario + 22, tipoUsuario, 2);
        memcpy(usuarioActual, usuario2, 12);
        resultadoActualizar = actualizarDatosUsuario(usuarioActual, datosUsuario);
    }

    if (iRet > 0 && resultadoActualizar != LENGTH_USUARIOS_CNB) {
        iRet = -3;
    }

    ////// SI NO EXISTE USUARIO MUESTRE MENSAJE ////////
    if (existeArchivo == FS_KO && iRet == 0) {
        iRet = -4;
    }

    if (iRet > 0 && resultadoActualizar == LENGTH_USUARIOS_CNB) {
        iRet = 1;
    }
    return iRet;
}

int actualizarDatosUsuario(char * usuarioActual, char * datosActualizar) {

    int resultado = 0;
    int numberIssuer = 0;
    int indice = 0;
    int posicion = 0;
    long lengthArchivo = 0;
    char informacionUsuario[30 + 1];
    char nombreUsuario[12 + 1];

    /// YA SE VERIFICO ANTERIORMENTE LA EXISTENCIA DEL ARCHIVO USUARIOS CNB

    lengthArchivo = tamArchivo(discoNetcom, ISSUER_CNB);
    numberIssuer = lengthArchivo / LENGTH_USUARIOS_CNB;

    for (indice = 0; indice < numberIssuer; indice++) {
        memset(informacionUsuario, 0x00, sizeof(informacionUsuario));
        memset(nombreUsuario, 0x00, sizeof(nombreUsuario));
        buscarArchivo(discoNetcom, ISSUER_CNB, informacionUsuario, posicion, LENGTH_USUARIOS_CNB);
        memcpy(nombreUsuario, informacionUsuario, 10);
        if (strcmp(nombreUsuario, usuarioActual) == 0) {
            resultado = actualizarArchivo(discoNetcom, ISSUER_CNB, datosActualizar, posicion,
                                          LENGTH_USUARIOS_CNB);
            break;
        }
        posicion += LENGTH_USUARIOS_CNB;
    }

    if (resultado != 0) {
        resultado = -1;
    } else {
        resultado = LENGTH_USUARIOS_CNB;
    }

    return resultado;
}

void generarUsuarioEstandar(uChar *usuario) {

    //Telium_Date_t date;
    int resultado = 1;
    int existeUsuario = 0;
    int escribeArchivo = 0;
    uChar fecha[8 + 1];
    uChar datosUsuario[30 + 1];

    memset(fecha, 0x00, sizeof(fecha));
    memset(datosUsuario, 0x00, sizeof(datosUsuario));

    obtenerFecha(fecha);

    existeUsuario = verificarUsuario(usuario, datosUsuario);
    if (existeUsuario == EXISTE_USUARIO) {
        resultado = -1;
    }

    if (resultado > 0) {
        LOGI("guardando datos de usuario");
        memset(datosUsuario, 0x00, sizeof(datosUsuario));
        memcpy(datosUsuario, usuario, 10);
        memcpy(datosUsuario + 10, "123456", 6);
        memcpy(datosUsuario + 16, fecha, 6);
        strcpy(datosUsuario + 22, "1");
        escribeArchivo = escribirArchivo(discoNetcom, ISSUER_CNB, datosUsuario,
                                         LENGTH_USUARIOS_CNB);
        if (escribeArchivo != LENGTH_USUARIOS_CNB) {
            //screenMessageFiveLine("MENSAJE", "", "ERROR AL", "CREAR USUARIO", "", "", 1);
            LOGI("Error al crear usuario");
        }
    }

}

int capturarClave(char *clave, char *claveCapturada) {

    int iRet = 1;
    LOGI("clave nativa %s clave capturada %s", clave, claveCapturada);
    if (strcmp(clave, claveCapturada) != 0) {
        iRet = 0;
    }

    return iRet;
}

int habilitarCNBconPAN(char *codigoBanco, char *numeroTarjeta, char *tipoCuenta) {

    int iRet = 1;

    setParameter(CNB_BANCO, codigoBanco);
    setParameter(CNB_TIPO_CUENTA, tipoCuenta);

    memset(tipoCuenta, 0x00, sizeof(tipoCuenta));
    sprintf(tipoCuenta, "%02d", CNB_TIPO_TARJETA_VIRTUAL);
    setParameter(CNB_TIPO_TARJETA, tipoCuenta);
    memcpy(numeroTarjeta + strlen(numeroTarjeta), "D101210100000", 13);
    escribirArchivo(discoNetcom, TARJETA_CNB, numeroTarjeta, strlen(numeroTarjeta));

    return iRet;
}

int borrartransacciones() {

    int respuesta = 0;

    setParameter(CNB_REALIZAR_CIERRE_PARCIAL, "0");
    respuesta = borrarArchivo(discoNetcom, JOURNAL);
//    respuesta = borrarArchivo(discoNetcom, JOURNAL_CNB_RBM);
    respuesta = borrarArchivo(discoNetcom, JOURNAL_CNB_BANCOLOMBIA);
//    respuesta = borrarArchivo(discoNetcom, FILE_CELUCOMPRA);
//    respuesta = borrarArchivo(discoNetcom, DATOS_QRCODE);
//    respuesta = borrarArchivo(discoNetcom, JOURNAL_CNB_CORRESPONSALES);
//    respuesta = borrarArchivo(discoNetcom, SINCRONIZACION_SERVICIO_IP);

    /** por multicorresponsal */
//    respuesta = borrarArchivo(discoNetcom, DIRECTORIO_JOURNALS);
//    respuesta = borrarArchivo(discoNetcom, JOURNAL_MCNB_AVAL);
//    respuesta = borrarArchivo(discoNetcom, JOURNAL_MULT_BCL);
//    respuesta = borrarArchivo(discoNetcom, JOURNAL_DCC);

    return respuesta;
}

int habilitarCNBConCtaSembrada(char *codigoBanco, char *tipoCuenta, char *track2, char *nombreTarjetaHabiente){

    int resultado = 1;
    char data[69 + 1];

    memset(data, 0x00, sizeof(data));

//    if (fiidSeleccionado[0] == 0x00) {
    setParameter(CNB_BANCO, codigoBanco);
    setParameter(CNB_TIPO_CUENTA, tipoCuenta);

    memset(tipoCuenta, 0x00, sizeof(tipoCuenta));
    sprintf(tipoCuenta, "%02d", CNB_TIPO_TARJETA_SEMBRADA);
    setParameter(CNB_TIPO_TARJETA, tipoCuenta);
    //memcpy(leerTrack.track2,"6013670020505791=49121207870000000000",37);

    memcpy(data, track2, 37);
    memcpy(data + 38, nombreTarjetaHabiente, 34);

    escribirArchivo(discoNetcom, (char *)TARJETA_CNB, data, 69);

    /*   } else {
           int CNB_B = 0;
           int CNB_T_C = 0;
           int CNB_T_T = 0;
           uChar tarjetaSembrada[69 + 1];

           memset(tarjetaSembrada, 0x00, sizeof(tarjetaSembrada));

           definirDatosCorresonsal(&CNB_B, &CNB_T_C, &CNB_T_T, tarjetaSembrada, fiidSeleccionado);

           setParameter(CNB_B, codigoBanco);
           setParameter(CNB_T_C, tipoCuenta);

           memset(tipoCuenta, 0x00, sizeof(tipoCuenta));
           Telium_Sprintf(tipoCuenta, "%02d", CNB_TIPO_TARJETA_SEMBRADA);
           setParameter(CNB_T_T, tipoCuenta);

           //memcpy(leerTrack.track2,"6013670020505791=49121207870000000000",37);

           memcpy(data, leerTrack.track2, 37);
           memcpy(data + 38, leerTrack.nombreTarjetaHabiente, 34);

           escribirArchivo(DISCO_NETCOM, tarjetaSembrada, data, 69);
       }*/


    return resultado;
}

int deshabilitarCNB(void) {
    int resultado = 0;
    char codigoBanco[SIZE_CODIGO_BANCO_CNB + 1];
    char numeroTarjeta[SIZE_MAX_NUM_TARJETA + 1];
    char tipoCuenta[TAM_TIPO_CUENTA + 1];

/*    resultado = getchAcceptCancel("CORRESPONSAL", "DESHABILITAR", "CORRESPONSAL?", "", 30 * 1000);
    if (resultado > 0) {
        if (fiidSeleccionado[0] == 0x00) {*/
    memset(codigoBanco, 0x30, SIZE_CODIGO_BANCO_CNB); // Se restablece el valor a 0.
    memset(numeroTarjeta, 0x00, sizeof(numeroTarjeta));
    memset(tipoCuenta, 0x00, sizeof(tipoCuenta));
    setParameter(CNB_BANCO, codigoBanco);
    setParameter(CNB_TIPO_CUENTA, tipoCuenta);
    setParameter(CNB_PEDIR_PIN, (char *)"00");
    borrarArchivo(discoNetcom, TARJETA_CNB);
    borrarArchivo(discoNetcom, RUTA_TABLA_MULTICORRESPONSAL);
/*        } else {
            int CNB_B = 0;
            int CNB_T_C = 0;
            int CNB_T_T = 0;
            uChar tarjetaSembrada[69 + 1];

            memset(tarjetaSembrada, 0x00, sizeof(tarjetaSembrada));

            definirDatosCorresonsal(&CNB_B, &CNB_T_C, &CNB_T_T, tarjetaSembrada, fiidSeleccionado);

            memset(codigoBanco, 0x30, SIZE_CODIGO_BANCO_CNB); // Se restablece el valor a 0.
            memset(numeroTarjeta, 0x00, sizeof(numeroTarjeta));
            memset(tipoCuenta, 0x00, sizeof(tipoCuenta));

            setParameter(CNB_B, codigoBanco);
            setParameter(CNB_T_C, tipoCuenta);
            setParameter(CNB_PEDIR_PIN, "00");
            borrarArchivo(DISCO_NETCOM, tarjetaSembrada);
        }*/

//        screenMessage("MENSAJE", "CUENTA", "CORRESPONSAL", "DESHABILITADA", 2 * 1000);

    ///// Se deja el logue de CNB en 0 //////
    setParameter(USER_CNB_LOGON, "0");
//    }

    return resultado;
}
void borrarParametros(char *codigoRespuesta) {

    int resultado = 0;
    long tam = 0;
    int tamJournal = 0;
    int maximoId = 0;

    tam = tamArchivo(discoNetcom, JOURNAL);
    tamJournal = sizeof(DatosVenta);

    maximoId = tam / tamJournal;
    if (maximoId <= 0) {
        setParameter(GPRS_APN, "");
        setParameter(GPRS_USER, "");
        setParameter(GPRS_PASS, "");
        setParameter(IP_DEFAULT, "");
        setParameter(PUERTO_DEFAULT, "");
        setParameter(NUMERO_TERMINAL, "");
        setParameter(NUMERO_TERMINAL_QR, "");
        setParameter(NII, "");
        setParameter(NII_MULTIPOS, "");
        setParameter(NII_DCC, "");
        setParameter(TERMINAL_INICIALIZADA, "0");
        setParameter(VALOR_CLAVE_ADMINISTRADOR, "028510");
        setParameter(VALOR_CLAVE_SUPERVISOR, "0000");
        strcpy(codigoRespuesta, "BORRADO EXITOSO");
    } else {
        strcpy(codigoRespuesta, "EJECUTAR CIERRE ");
    }

}

void screenMessageFiveLine(char *titulo, char *linea1, char *linea2, char *linea3, char *linea4,
                           char *linea5, char *lineaResumen) {

    strcpy(lineaResumen, titulo);
    strcat(lineaResumen, ";");
    strcat(lineaResumen, linea1);
    strcat(lineaResumen, ";");
    strcat(lineaResumen, linea2);
    strcat(lineaResumen, ";");
    strcat(lineaResumen, linea3);
    strcat(lineaResumen, ";");
    strcat(lineaResumen, linea4);
    strcat(lineaResumen, ";");
    strcat(lineaResumen, linea5);

}

void showMenu(char * titulo, int nitems, char* items, int longitud ){

    char  arreglo[1024 + 1] = {0x00};
    sprintf(arreglo,"%s%d%s", titulo,nitems,items);
    enviarStringToJava(arreglo,"mostrarMenu");

}
void obtenerFecha(char *fecha) {
    time_t ttime = time(0);
    char year[4 + 1] = {0x00};
    char month[2 + 1] = {0x00};
    char auxiliarMes[2 + 1] = {0x00};
    char day[2 + 1] = {0x00};
    char *dt = ctime(&ttime);
    tm *local_time = localtime(&ttime);
    sprintf(year, "%d",1900 + local_time->tm_year );
    sprintf(month, "%d",1 + local_time->tm_mon );
    leftPad(auxiliarMes,month,0x30,2);
    sprintf(day, "%d",local_time->tm_mday);
    sprintf(fecha, "%.2s%.2s%.2s", year, auxiliarMes, day);
    LOGI("Fecha obtenida %s", fecha);

}

Date obtenerDate() {
    time_t ttime = time(0);
    Date date;
    memset(&date, 0x00, sizeof(date));
    char auxiliar[2 + 1] = {0x00};
    char *dt = ctime(&ttime);
    tm *local_time = localtime(&ttime);
    sprintf(date.year, "%d",1900 + local_time->tm_year );
    sprintf(auxiliar, "%d",1 + local_time->tm_mon );
    leftPad(date.month,auxiliar,0x30,2);
    memset(auxiliar, 0x00, sizeof(auxiliar));
    sprintf(auxiliar, "%d",local_time->tm_mday);
    leftPad(date.day,auxiliar,0x30,2);
    memset(auxiliar, 0x00, sizeof(auxiliar));
    sprintf(auxiliar, "%d",local_time->tm_hour);
    leftPad(date.hour,auxiliar,0x30,2);
    memset(auxiliar, 0x00, sizeof(auxiliar));
    sprintf(auxiliar, "%d",local_time->tm_min);
    leftPad(date.minutes,auxiliar,0x30,2);
    memset(auxiliar, 0x00, sizeof(auxiliar));
    sprintf(auxiliar, "%d",local_time->tm_sec);
    leftPad(date.secondes,auxiliar,0x30,2);
    return date;

}
void interpretarTrama(char * dataOrigen, int tamLeido){
    char dataAuxiliar[2048 + 1] = {0x00};
    _convertBCDToASCII_(dataAuxiliar,dataOrigen,tamLeido);
    LOGI("TLS leyo %s", dataAuxiliar);
}

ParametrosVenta traerParametrosVenta(DatosTarjeta datosTarjeta) {

    ParametrosVenta parametrosVenta;
    TablaUnoInicializacion tablaUno;
    TablaCuatroInicializacion tablaCuatro;
    TablaSeisInicializacion textoAdicional;

    memset(&parametrosVenta, 0x00, sizeof(parametrosVenta));
    memset(&textoAdicional, 0x00, sizeof(textoAdicional));
    memset(&tablaCuatro, 0x00, sizeof(tablaCuatro));
    memset(&tablaUno, 0x00, sizeof(tablaUno));

    ////////  PARAMETROS DE INICIALIZACION /////////
    tablaUno = _desempaquetarTablaCeroUno_();
    tablaCuatro = _desempaquetarTablaCuatro_();

    ////////  PROPINA E IVA MAX. PERMITIDO  ////////
    sprintf(parametrosVenta.ivaMaximoPermitido, "%02x", tablaUno.ecrBaudRate);
    sprintf(parametrosVenta.propinaMaximaPermitida, "%02x", tablaUno.printerBaudRate);

    ///////// PERMITE TEXTO ADICIONAL ? ///////////
    textoAdicional = desempaquetarTablaSeis(0x00);
    if (textoAdicional.consecutivoRegistro == '1') {
        parametrosVenta.permiteTextoAdicional = 1;
        strcpy(parametrosVenta.texto, textoAdicional.texto);
    }

    //////// ES SUPER CUPO ? ////////
    if (datosTarjeta.dataIssuer.additionalPromptsSelect.additionalPrompt8 == 1) {
        parametrosVenta.isSuperCupo = 1;
    }

    /////// PERMITE SELECCION DE CUENTA ////////
    if (datosTarjeta.dataIssuer.options1.selectAccountType == 1) {
        parametrosVenta.permiteSeleccionCuenta = 1;
    }

    ////// PERMITE INGRESO CUOTAS ? ////////
    if (datosTarjeta.dataIssuer.options3.printNoRefundsAllowed == 1) {
        parametrosVenta.permiteIngresoCoutas = 1;
    }

    ////// PERMITE INGRESO DOCUMENTO ? ///////
    if (datosTarjeta.dataIssuer.additionalPromptsSelect.additionalPrompt1 == 1) {
        parametrosVenta.permiteIngresoDocumento = 1;
    }

    ////// PERMITE INGRESO DE PIN ///////
    if (datosTarjeta.dataIssuer.options1.pinEntry == 1) {
        parametrosVenta.permiteIngresoPin = 1;
    }

    ////// PERMITE COSTO TRANSACCION ? ////////
    if (datosTarjeta.dataIssuer.additionalPromptsSelect.additionalPrompt4 == 1) {
        parametrosVenta.permiteConsultaCostoTransaccion = 1;
    }

    ////// PERMITE INGRESO DE ULTIMOS CUATRO ///////
    if (datosTarjeta.dataIssuer.options4.verifyLastFourPANDigits == 1) {
        parametrosVenta.permiteIngresoUltmosCuatro = 1;
    }

    ////// PERMITE INGRESO DE PROPINA ///////
    if (tablaUno.options1.tipProcessing == 1) {
        parametrosVenta.permitePropina = 1;
    }

    return parametrosVenta;
}

int validarSeleccionCuenta(int *pideCuotas, DatosVenta *datosVenta, DatosTarjeta *leerTrack,
                           int validarCreditoRotativo) {

    int resultado = 1;
    int isSeleccionCuentaDefault = 0;

    isSeleccionCuentaDefault = leerTrack->configuracionExcepciones.comportamientoExcepciones1.seleccionCuentaDefault;

    if (isSeleccionCuentaDefault != FALSE) {
        validarCreditoRotativo = 0;
    }

    if (leerTrack->configuracionExcepciones.comportamientoExcepciones1.seleccionCuentaMultiservicios) {
        validarCreditoRotativo = 0;
        isSeleccionCuentaDefault = 0;
    }

    /////////////// SI PIDE SELECCION DE CUENTA O TOMA DEFAULT ///////////////
    if (leerTrack->dataIssuer.options1.selectAccountType == TRUE || isSeleccionCuentaDefault == TRUE) {
        menuTipoDeCuenta(validarCreditoRotativo);
        if (resultado > 0) {
            if (resultado == 40) {
                strcpy(datosVenta->creditoRotativo, "1");
                (*pideCuotas) = 1;
            }

            memset(datosVenta->processingCode, 0x00, sizeof(datosVenta->processingCode));
            sprintf(datosVenta->tipoCuenta, "%02d", resultado);
            sprintf(datosVenta->processingCode, "00%d00", resultado);

            if (strncmp(datosVenta->tipoCuenta, "30", TAM_TIPO_CUENTA) == 0) {
                (*pideCuotas) = 1;
                leerTrack->dataIssuer.options3.printNoRefundsAllowed = 1;
            }
        }
    }

    ///////////////// VALIDA SI DEBE PEDIR PIN /////////////////
    if ((strncmp(datosVenta->tipoCuenta, "10", TAM_TIPO_TRANSACCION) == 0)
        || strncmp(datosVenta->tipoCuenta, "20", TAM_TIPO_TRANSACCION) == 0) {
        leerTrack->dataIssuer.options1.pinEntry = 1;
        leerTrack->dataIssuer.options3.printNoRefundsAllowed = 0;
        (*pideCuotas) = 0;
    }

    validarTipotarjeta(leerTrack->dataIssuer.issuerNumber, datosVenta);

    return resultado;
}

void validarTipotarjeta(char tipoTarjeta, DatosVenta * datosVenta) {

    int tipoCuenta = 0;

    tipoCuenta = atoi(datosVenta->tipoCuenta);
    TablaTresInicializacion tablaTres;

    if (tipoTarjeta == 0x03 && (tipoCuenta == 10 || tipoCuenta == 20)) {
        memset(datosVenta->codigoSubgrupo, 0x00, sizeof(datosVenta->codigoSubgrupo));
        memset(datosVenta->cardName, 0x00, sizeof(datosVenta->cardName));
        strcpy(datosVenta->cardName, "V.ELECTRON");
        strcpy(datosVenta->codigoSubgrupo, "E");
    }
    if (tipoTarjeta == 0x05 && (tipoCuenta == 10 || tipoCuenta == 20)) {
        memset(datosVenta->cardName, 0x00, sizeof(datosVenta->cardName));
        strcpy(datosVenta->cardName, "MAESTRO");
        tablaTres = _traerIssuer_(601660700); //bin de una tarjeta maestro
        memcpy(datosVenta->codigoSubgrupo, tablaTres.reservedForFutureUse2, TAM_SUBGRUPO);
    }
}

void menuTipoDeCuenta(int validarSeleccionCuenta) {

    int tipoCuenta = 0;
    int creditoRotativo = 0;
    int nItem = 3;
    int mostrarOpcion = 0;
    char transaccion[2 + 1];
    uChar transaccionAux[2 + 1];
    char itemsMenu[100 + 1] = {0x00};

    memset(transaccion, 0x00, sizeof(transaccion));
    memset(transaccionAux, 0x00, sizeof(transaccionAux));

    getParameter(TIPO_TRANSACCION, transaccion);
    getParameter(TIPO_TRANSACCION_AUX, transaccionAux);

    switch (atoi(transaccion)) {
        case TRANSACCION_MULTIPOS:
        case TRANSACCION_GASO_PASS:
        case TRANSACCION_DESCUENTOS_VIVAMOS:
            mostrarOpcion = 1;
            break;
    }

    //viene de avance (credito rotativo por excepciones)
    if (atoi(transaccionAux) == TRANSACCION_EFECTIVO) {
        mostrarOpcion = 1;
        creditoRotativo = validarSeleccionCuenta;
    }

    if (creditoRotativo == 1 && validarSeleccionCuenta == 1 && mostrarOpcion == 1) {
        nItem = 4;
    }

    //// VERIFICA SI ES CONSULTA DE SALDO DE BANCOLOMBIA PARA SOLO MOSTRAR LOS TIPOS DE CUENTA AHORRO Y CORRIENTE

    if (atoi(transaccion) == TRANSACCION_BCL_SALDO) {
        nItem = 2;
    }
    strcpy(itemsMenu,";Ahorro;10;");
    strcat(itemsMenu,"Corriente;20;");
    strcat(itemsMenu,"Credito;30;");
    strcat(itemsMenu,"Cr.Rotativo;40;");
    showMenu("TIPO DE CUENTA", nItem, itemsMenu,100);

}

int validarMultiServicios(int *pideCuotas, DatosVenta *datosVenta, DatosTarjeta *leerTrack) {

    int resultado = 1;
    int isSeleccionCuentaMultiBolsillos = 0;
    char binCafam[6 + 1];

    memset(binCafam, 0x00, sizeof(binCafam));

    isSeleccionCuentaMultiBolsillos =
            leerTrack->configuracionExcepciones.comportamientoExcepciones1.seleccionCuentaMultiservicios;
    LOGI("isSeleccionCuentaMultiBolsillos  %d", isSeleccionCuentaMultiBolsillos);
    ///////////////  SI ES TRANSACCION MULTISERVICIOS ///////////////
    memcpy(binCafam, leerTrack->track2, 6);
    LOGI("binCafam  %s", binCafam);
    if (isSeleccionCuentaMultiBolsillos == TRUE || strncmp(binCafam, "606125", 6) == 0) {

        resultado = menuMultiservicios(binCafam);
        if (resultado > 0) {
            memset(datosVenta->processingCode, 0x00, sizeof(datosVenta->processingCode));
            sprintf(datosVenta->processingCode, "00%02d00", resultado);
            sprintf(datosVenta->tipoCuenta, "%02d", resultado);
            leerTrack->dataIssuer.options1.selectAccountType = 0;
            leerTrack->dataIssuer.options1.pinEntry = 1;
            leerTrack->dataIssuer.options3.printNoRefundsAllowed = 1;
            leerTrack->configuracionExcepciones.comportamientoExcepciones1.seleccionCuentaDefault = 0;
            if (resultado == 40 || resultado == 43 || resultado == 45 || resultado == 49) {
                (*pideCuotas) = 1;
            }
        }
    }

    validarTipotarjeta(leerTrack->dataIssuer.issuerNumber, datosVenta);

    return resultado;
}

int menuMultiservicios(char * binCafam) {

    int iRet = 1;
    char items[1024 + 1] = {0x00};
    if (strncmp(binCafam, "606125", 6) != 0) {
        strcpy(items, ";LEALTAD;41;");
        strcat(items, "CUOTA MONETARIA;42;");
        strcat(items, "CUPO CREDITO;43;");
        strcat(items, "BONO EFECTIVO;44;");
        strcat(items, "CREDITO ROTATIVO;45;");
        strcat(items, "BONO REGALO;46;");
        strcat(items, "BONO DESCUENTO;47;");
        strcat(items, "BOLSILLO DEBITO;48;");
        strcat(items, "BOLSILLO CREDITO;49;");
        showMenu("TIPO DE CUENTA;", 9, items, 300);
    } else {
        strcpy(items, ";SUBSIDIO FAMILIAR;42;");
        strcat(items, "CREDITO AFILIADO;43;");
        strcat(items, "MONEDERO;44;");
        showMenu("TIPO DE CUENTA;", 3, items, 100);
    }
    return iRet;
}

int fechaActualizacion(ResultISOUnpack resultadoUnpack) {

    uChar fechaActivacion[12 + 1];
    int idx = 0;
    int resultado = 0;
    ISOFieldMessage isoFieldMessage;
    char fechaProceso[10 + 1];

    LOGI("fechaActualizacion  Inicializacion automatica %d",resultadoUnpack.totalField);
    for (idx = 0; idx < resultadoUnpack.totalField; idx++) {
        if (resultadoUnpack.listField[idx] == 63 || resultadoUnpack.listField[idx] == 59) {
            memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
            isoFieldMessage = getField(63);
            if (isoFieldMessage.totalBytes < 16){
                isoFieldMessage = getField(59);
            }

            if (isoFieldMessage.totalBytes >= 16 ) {

                memset(fechaActivacion, 0x00, sizeof(fechaActivacion));
                if (strncmp(isoFieldMessage.valueField + 2, "4202", 4) == 0
                    || strncmp(isoFieldMessage.valueField + 2, "4205", 4) == 0) { //inicialicion
                    memcpy(fechaActivacion, isoFieldMessage.valueField + 6 + 2, 10);
                    setParameter(TIME_INICIALIZACION_AUTOMATICA, fechaActivacion);
                    LOGI("fechaActivacion  %s ", fechaActivacion);
                    if (strncmp(fechaActivacion, "9999999999", 10) == 0) {
                        memset(fechaProceso, 0x00, sizeof(fechaProceso));
                        strcpy(fechaProceso, "2112312359");
                        setParameter(TIME_INICIALIZACION_AUTOMATICA, fechaProceso);
                        resultado = 1;
                        LOGI("parece ser automatica   %s ", fechaProceso);
                    }
                } else if (strncmp(isoFieldMessage.valueField + 2, "4204", 4) == 0) { //telecarga
                    memcpy(fechaActivacion, isoFieldMessage.valueField + 6 + 2, 10);
                    setParameter(TIME_TELECARGA_AUTOMATICA, fechaActivacion);
                    if (strncmp(fechaActivacion, "9999999999", 10) == 0) {
                        memset(fechaProceso, 0x00, sizeof(fechaProceso));
                        strcpy(fechaProceso, "2112312359");
                        setParameter(TIME_TELECARGA_AUTOMATICA, fechaProceso);
                        resultado = 2;
                        setParameter(TERMINAL_INICIALIZADA, "4");
                    }
                }
            }
            break;
        }
    }
    LOGI("resultado    %d ", resultado);
    return resultado;

}

void validarActualizacionTelecarga(ResultISOUnpack resultadoUnpack) {

    int actualizacion = 0;
    int resultado = 0;
    ///////////////////

    actualizacion = fechaActualizacion(resultadoUnpack);
    if (actualizacion == 1) { // inicializacion automatica
        LOGI("debe mandarse a inicializar de una vez    %d ", actualizacion);
        setParameter(TIPO_TELECARGA, "1");
        //Solo se setea el parametro para que despues de imprimir segundo recibo en C se inicialice
       // inicializacionGeneral(INICIALIZACION_AUTOMATICA);930010
    } else if (actualizacion == 2) { // telecarga automatica
//		setParameter(PROCESO_CIERRE, "1");
//		resultado = cierreVenta();
//        resultado = invocarCierreAutomatico();
        if (resultado > 0) {
            setParameter(TIPO_TELECARGA, "2");
            //telecargaTMS();
        }
    }
    ///////////////////
}

int  validarInicializacionProgramada(){

    int resultado = 0;
    char fechaProceso[10 + 1] = {0x00};
    char fechaTerminalAux[6 + 1] = {0x00};
    char fechaProcesoAux[6 + 1] = {0x00};
    char horaTerminal[4 + 1] = {0x00};
    char horaProceso[4 + 1] = {0x00};
    char fechaTerminal[10 + 1]= {0x00};

    Date date;
    memset(&date, 0x00, sizeof(date));

    date  = obtenerDate();

    sprintf(fechaTerminal, "%.2s%.2s%.2s%.2s%.2s", date.year, date.month, date.day, date.hour, date.minutes);
    getParameter(TIME_INICIALIZACION_AUTOMATICA, fechaProceso);
    LOGI("fecha de proceso  %s ", fechaProceso);
    LOGI("fechaTerminal  %s ", fechaTerminal);

    //realiza inicializacion programada
    memcpy(fechaTerminalAux, fechaTerminal, 6);
    memcpy(fechaProcesoAux, fechaProceso, 6);
    memcpy(horaTerminal, fechaTerminal + 6, 4);
    memcpy(horaProceso, fechaProceso + 6, 4);

    LOGI("fechaTerminalAux  %s ", fechaTerminalAux);
    LOGI("fechaProcesoAux  %s ", fechaProcesoAux);
    LOGI("horaTerminal  %s ", horaTerminal);
    LOGI("horaProceso  %s ", horaProceso);
    if (atol(fechaTerminalAux) >= atol(fechaProcesoAux)) {
        if (atol(horaTerminal) >= atol(horaProceso)) {
            memset(fechaProceso, 0x00, sizeof(fechaProceso));
            memcpy(fechaProceso, "4012312359", sizeof(fechaProceso));
            setParameter(TIPO_TELECARGA, "1");
            setParameter(TIME_INICIALIZACION_AUTOMATICA, fechaProceso);
            resultado = 1;
            //invocarInicializacionGeneral(INICIALIZACION_AUTOMATICA);
        }

    }

    return resultado;
}

void cargarInformacionPanVirtualCorresposal(char * fiid, char* tipoCuenta, char * panVirtual){

    LOGI("fidd %s", fiid);
    LOGI("tipoCuenta %s", tipoCuenta);
    LOGI("panVirtual %s", panVirtual);
    setParameter(CNB_BANCO, fiid);
    setParameter(CNB_TIPO_CUENTA, tipoCuenta);
    memcpy(panVirtual + strlen(panVirtual), "D101210100000", 13);

    memset(tipoCuenta, 0x00, sizeof(tipoCuenta));
    sprintf(tipoCuenta, "%02d", CNB_TIPO_TARJETA_VIRTUAL);
    LOGI("tipoCuenta %s",tipoCuenta);
    setParameter(CNB_TIPO_TARJETA, tipoCuenta);
    borrarArchivo(discoNetcom,(char *) TARJETA_CNB);
    escribirArchivo(discoNetcom, (char *) TARJETA_CNB, panVirtual, strlen(panVirtual));
}