//
// Created by NETCOM on 14/02/2020.
//

#include "Definiciones.h"
#include "Tipo_datos.h"
#include <string>
#include <stdio.h>
#include <Utilidades.h>
#include <Archivos.h>
#include "configuracion.h"
#include "android/log.h"
#include "native-lib.h"

#define  LOG_TAG    "Reportes"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

void imprimirTicket(HasMap *hash, char * logo, char * templateImpresion) {

    char dataTiket[500];
    int posicion = 0;
    int resultado = 0;
    memset(dataTiket, 0x00, sizeof(dataTiket));
    LineaImprimir linea;
    int indice = 0;
    char nombreTemplate[60 + 1];
    char transaccion[1 + 1];
    int papelCargado = 0;
    unsigned char terminal[64 + 1];

    memset(transaccion, 0x00, sizeof(transaccion));
    memset(terminal, 0x00, sizeof(terminal));

    getParameter(TIPO_TRANSACCION, transaccion);

    indice = 0;
    memset(nombreTemplate, 0x00, sizeof(nombreTemplate));
    sprintf(nombreTemplate,"%s", templateImpresion);

    borrarArchivo(discoNetcom, (char*)RECIBO_DIGITAL);
    LOGI("Netcom  template %s",nombreTemplate);
    do {
        memset(dataTiket, 0x00, sizeof(dataTiket));
        memset(&linea, 0x00, sizeof(linea));

        resultado = buscarArchivo(discoNetcom, nombreTemplate, dataTiket, posicion, 70);

        if (resultado < 0) {
            LOGI("Netcom no se encontro template %d",resultado);
            break;
        }

        linea = _obtenerLineaTemplate_(dataTiket, hash);
        if (linea.presencia == 1) {
         imprimirLineaDigital(linea);
        }
        posicion += 70;
        indice++;
    } while (resultado == 0);
    if(resultado > 0){
        LOGI("Netcom imprimirReciboTicket ");
        imprimirReciboTicket();
    }
    borrarArchivo(discoNetcom, (char*)RECIBO_DIGITAL);
}

LineaImprimir _obtenerLineaTemplate_(char * mystr, HasMap * valorHash) {

    //char mystr[] = "<[1,6,1,0,0,0,0,0]>Fecha : _PAR_FECHA_ Hora : _PAR_HORA_";
    //char mystr[] = "<[1,3,0,1]>_PAR_NESTABLE_@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@";
    char caracteristicasLinea[512 + 1];
    char contenidoLinea[512 + 1];
    CaracteristicaLinea carLinea;
    char newstr[512 + 1];
    char token[] = ",";
    char *ptr;
    int idx = 0;
    int idy = 0;
    char tokenFinal[] = "]>";

    LineaImprimir lineaTemplate;

    memset(&carLinea, 0x00, sizeof(carLinea));
    memset(caracteristicasLinea, 0x00, sizeof(caracteristicasLinea));
    memset(contenidoLinea, 0x00, sizeof(contenidoLinea));
    memset(&lineaTemplate, 0x00, sizeof(lineaTemplate));
    memset(newstr, 0x00, sizeof(newstr));

    lineaTemplate.presencia = 1;
    //strcpy(lineaTemplate.contenidoLinea, mystr);
    if (strlen(mystr) >= 2) {
        //busca el fin de caracteristicas linea ]>
        for (idx = 0; idx < strlen(mystr); idx++) {
            if (memcmp(mystr + idx, tokenFinal, strlen(tokenFinal)) == 0) {
                //printf("\nOK %d ",idx);
                idx += strlen(tokenFinal);
                break;
            }
        }

        if (idx >= 11 && mystr[0] == '<' && mystr[1] == '[' && mystr[idx - 2] == ']' && mystr[idx - 1] == '>') {

            memcpy(caracteristicasLinea, mystr + 2, idx - 4);
            //////
            ptr = strtok(caracteristicasLinea, token);
            //////
            carLinea.alineacion = atoi(ptr);
            idy = 0;

            while ((ptr = strtok(NULL, token)) != NULL) {
                if (idy == 0)
                    carLinea.fuente = atoi(ptr);
                if (idy == 1)
                    carLinea.negrilla = atoi(ptr);
                if (idy == 2)
                    carLinea.inverso = atoi(ptr);

                idy++;
            }
            memcpy(contenidoLinea, mystr + idx, strlen(mystr) - idx);

            idx = 0;
            while (strcmp((valorHash + idx)->clave, "FIN_HASH") != 0) {
                replaceString(contenidoLinea, newstr, (valorHash + idx)->clave, (valorHash + idx)->valor);
                memset(contenidoLinea, 0x00, sizeof(contenidoLinea));
                strcpy(contenidoLinea, newstr);
                memset(newstr, 0x00, sizeof(newstr));

                idx++;
            }

            int tamLinea = 0;
            while (contenidoLinea[tamLinea] != '~') {
                tamLinea++;
                if (tamLinea == strlen(contenidoLinea)) {
                    break;
                }
            }

            if (searchString(contenidoLinea, NO_IMPRIMIR) == 1) {
                lineaTemplate.presencia = 0;
            }

            lineaTemplate.carateristica = carLinea;
            memcpy(lineaTemplate.contenidoLinea, contenidoLinea, tamLinea);
        }
    }

    return lineaTemplate;
}

static void imprimirLineaDividida(LineaImprimir justificarLineas, int maximoCaracteres,char *saltoLinea,char* alineacion,char *tipoLetra,char *formato) {

    char textoDividir[500]; // = "PAGARE INCONDICIONALMENTE A LA VISTA Y A LA ORDEN DEL ACREEDOR EL VALOR TOTAL DE ESTE PAGARE JUNTO CON LOS INTERESES A LAS TASAS MAXIMAS PERMITIDAS POR LA LEY.";
    char token[] = " ";
    char *ptr;
    char lineaImprimir[250 + 1];
    char lineaImprimirAux[250 + 1] = {0x00};

    int longitud = 0;
    int indice = 0;

    memset(lineaImprimir, 0x00, sizeof(lineaImprimir));
    memset(textoDividir, 0x00, sizeof(textoDividir));

    memcpy(textoDividir, justificarLineas.contenidoLinea, strlen(justificarLineas.contenidoLinea));
    ptr = strtok(textoDividir, token);
    memcpy(lineaImprimir, ptr, strlen(ptr));
    indice += strlen(ptr);
    LOGI("A lineaImprimir %s ",lineaImprimir);
    while ((ptr = strtok(NULL, token)) != NULL) {
        longitud = strlen(lineaImprimir) + strlen(ptr) + 1;
        LOGI("longitud %d ",longitud);
        LOGI("maximoCaracteres %d ",maximoCaracteres);
        if (longitud <= maximoCaracteres) {

            strcat(lineaImprimir, " ");
            strcat(lineaImprimir, ptr);
        } else {
            memset(justificarLineas.contenidoLinea, 0x00, sizeof(justificarLineas.contenidoLinea));
            strcpy(justificarLineas.contenidoLinea, lineaImprimir);
            memset(lineaImprimir, 0x00, sizeof(lineaImprimir));
            justificarLineasImpresion(lineaImprimir, &justificarLineas, maximoCaracteres);
            sprintf(lineaImprimirAux, "%s%s%s%s%s\n", saltoLinea, alineacion, tipoLetra, formato,
                    lineaImprimir);
            //if (tamFile < 1100) {
            LOGI("Netcom primera  partida %s",lineaImprimirAux);
            escribirArchivo(discoNetcom, (char*)RECIBO_DIGITAL, lineaImprimirAux, strlen(lineaImprimirAux));
            //ttestall ( PRINTER , 0 );
            memset(lineaImprimir, 0x00, sizeof(lineaImprimir));
            strcat(lineaImprimir, ptr);
        }
    }

    if (strlen(lineaImprimir) != 0) {
        memset(justificarLineas.contenidoLinea, 0x00, sizeof(justificarLineas.contenidoLinea));
        strcpy(justificarLineas.contenidoLinea, lineaImprimir);
        memset(lineaImprimir, 0x00, sizeof(lineaImprimir));
        justificarLineasImpresion(lineaImprimir, &justificarLineas, maximoCaracteres);
        justificarLineasImpresion(lineaImprimir, &justificarLineas, maximoCaracteres);
        sprintf(lineaImprimirAux, "%s%s%s%s%s\n", saltoLinea, alineacion, tipoLetra, formato,
                lineaImprimir);
        //if (tamFile < 1100) {
        LOGI("Netcom Linea partida %s",lineaImprimirAux);
        escribirArchivo(discoNetcom, (char*)RECIBO_DIGITAL, lineaImprimirAux, strlen(lineaImprimirAux));
    }

}

void justificarLineasImpresion(char * cadenaDestino, LineaImprimir *justificarLineas, int longLinea) {

    char lineaImprimir[100];
    char lineaImprimirAux[80];
    char lineaImprimirAux2[80];
    int tipoAlineacion = 0;
    int tamLinea = 0;
    int tamLineaAux = 0;
    int idx = 0;
    memset(lineaImprimir, 0x00, sizeof(lineaImprimir));
    memset(lineaImprimirAux, 0x00, sizeof(lineaImprimirAux));
    memset(lineaImprimirAux2, 0x00, sizeof(lineaImprimirAux2));
    memset(cadenaDestino, 0x00, sizeof(cadenaDestino));

    strcpy(lineaImprimir, justificarLineas->contenidoLinea);
    tamLinea = strlen(lineaImprimir);
    tipoAlineacion = justificarLineas->carateristica.alineacion;

    switch (tipoAlineacion) {

        case 1:
        case 3:
            strcpy(cadenaDestino, lineaImprimir);
            break;

        case 2:
            alineacionCentrada(longLinea, lineaImprimir);
            strcpy(cadenaDestino, lineaImprimir);
            break;

        case 4: //justificado montos
            for (idx = 0; idx < tamLinea; idx++) {
                if (lineaImprimir[longLinea - idx] == ' ') {
                    break;
                }
            }

            tamLineaAux = longLinea - tamLinea; //LineaSpace

            memcpy(lineaImprimirAux, lineaImprimir, idx);
            memcpy(lineaImprimirAux2, lineaImprimir + idx + 1, ((tamLinea - idx) - 1));
            memset(lineaImprimirAux + idx, ' ', tamLineaAux + 1);
            lineaImprimirAux[idx + tamLineaAux - 1] = '$';
            memcpy(lineaImprimirAux + idx + tamLineaAux + 1, lineaImprimirAux2, ((tamLinea - idx) - 1));
            memcpy(cadenaDestino, lineaImprimirAux, longLinea);
            break;
        default:
            strcpy(cadenaDestino, lineaImprimir);
            break;
    }

}

void alineacionCentrada(int cantidadCaracteresLinea, char *dataLinea) {

    int tamLinea = 0;
    int tamLineaAl = 0;
    char cadenaDestino[80 + 1];
    memset(cadenaDestino, 0x00, sizeof(cadenaDestino));

    tamLineaAl = strlen(dataLinea);
    tamLinea = cantidadCaracteresLinea - tamLineaAl;
    tamLinea /= 2;

    _leftPad_(cadenaDestino, dataLinea, 0x20, tamLinea + tamLineaAl);

    memset(dataLinea, 0x00, sizeof(dataLinea));
    strcpy(dataLinea, cadenaDestino);
}

void imprimirLineaDigital(LineaImprimir lineaRecibo) {

    char lineaImprimir_[512 + 1];
    char formato[10 + 1];
    char impresion[512 + 1];
    char alineacion[10 + 1];

    int tipoAlineacion = 0;
    int tamFile = 0;
    char saltoLinea[] = "[0X0A]";
    char alineadoIzquierda[] = "[0X1A]";
    char alineadoCentro[] = "[0X1C]";

    char letraPequena[] = "[0X2A]";
    char letraMediana[] = "[0X2B]";

    char tipoLetra[6 + 1];

    memset(formato, 0x00, sizeof(formato));
    memset(tipoLetra, 0x00, sizeof(tipoLetra));
    memset(impresion, 0x00, sizeof(impresion));
    memset(alineacion, 0x00, sizeof(alineacion));

    tipoAlineacion = lineaRecibo.carateristica.alineacion;

    switch (tipoAlineacion) {

        case 1:
        case 3:
        case 4:
            strcpy(alineacion, alineadoIzquierda);
            break;
        case 2:
            strcpy(alineacion, alineadoCentro);
            break;
        default:
            break;
    }

    switch (lineaRecibo.carateristica.fuente) {
        case 1: //formato normal
            memcpy(tipoLetra, letraPequena, 6);
            if (lineaRecibo.carateristica.negrilla == 1) {
                memset(formato, 0x00, sizeof(formato));
                strcat(formato, "[0X3B]");
            } else if (lineaRecibo.carateristica.inverso == 1) {
                memset(formato, 0x00, sizeof(formato));
                strcat(formato, "[0X3C]");
            } else {
                strcat(formato, "[0X3A]");
            }

            break;

        case 2: //mediana
            memcpy(tipoLetra, letraMediana, 6);
            if (lineaRecibo.carateristica.negrilla == 1) {
                memset(formato, 0x00, sizeof(formato));
                strcat(formato, "[0X3B]");
            } else 	if (lineaRecibo.carateristica.inverso == 1) {
                memset(formato, 0x00, sizeof(formato));
                strcat(formato, "[0X3C]");
            } else {
                strcat(formato, "[0X3A]");
            }

            break;

        case 3:
            memcpy(tipoLetra, letraPequena, 6);
            if (lineaRecibo.carateristica.negrilla == 1) {
                memset(formato, 0x00, sizeof(formato));
                strcat(formato, "[0X3B]");
            } else 	if (lineaRecibo.carateristica.inverso == 1) {
                memset(formato, 0x00, sizeof(formato));
                strcat(formato, "[0X3C]");
            } else {
                strcat(formato, "[0X3A]");
            }

            break;

        default:
            memcpy(tipoLetra, letraPequena, 6);
            if (lineaRecibo.carateristica.negrilla == 1) {
                memset(formato, 0x00, sizeof(formato));
                strcat(formato, "[0X3B]");
            } else if (lineaRecibo.carateristica.inverso == 1) {
                memset(formato, 0x00, sizeof(formato));
                strcat(formato, "[0X3C]");
            } else {
                strcat(formato, "[0X3A]");
            }
            break;
    }
    memset(lineaImprimir_, 0x00, sizeof(lineaImprimir_));
    LOGI("tamano linea %ld",strlen(lineaRecibo.contenidoLinea));
    if (strlen(lineaRecibo.contenidoLinea) < 50) {
        if (strlen(lineaRecibo.contenidoLinea) > 0) {
            sprintf(impresion, "%s%s%s%s%s\n", saltoLinea, alineacion, tipoLetra, formato,
                           lineaRecibo.contenidoLinea);
            tamFile = tamArchivo(discoNetcom, (char*)RECIBO_DIGITAL);
            //if (tamFile < 1100) {
            LOGI("Netcom Linea reporte %s",impresion);
            escribirArchivo(discoNetcom, (char*)RECIBO_DIGITAL, impresion, strlen(impresion));
            //}
        }
    } else {
            imprimirLineaDividida(lineaRecibo, 48,saltoLinea,alineacion,tipoLetra,formato);
    }
}


void replaceString(const char *source, char *destination, const char *oldChar, const char *newChar) {
    int i, count = 0;
    size_t newlen = strlen(newChar);
    size_t oldlen = strlen(oldChar);

    for (i = 0; source[i] != '\0'; i++) {
        if (strstr(&source[i], oldChar) == &source[i]) {
            count++;
            i += oldlen - 1;
        }
    }

    i = 0;

    while (*source) {
        if (strstr(source, oldChar) == source) {
            strcpy(&destination[i], newChar);
            i += newlen;
            source += oldlen;
        } else {
            destination[i++] = *source++;
        }
    }

    destination[i] = '\0';
}

static int searchString(const char *source, const char *oldChar) {
    int resultado = 0;
    size_t oldlen = strlen(oldChar);

    while (*source) {
        if (strstr(source, oldChar) == source) {
            source += oldlen;
            resultado = 1;
        } else {
            source++;
        }
    }

    return resultado;

}