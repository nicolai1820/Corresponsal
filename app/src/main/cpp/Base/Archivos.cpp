//
// Created by NETCOM on 04/12/2018.
//

#include "Archivos.h"
//#include "include/Utilidades.h"
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
#include <fstream>
#include "android/log.h"
#define  LOG_TAG    "mysocket"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define FS_OK                           0	/*!< success */
#define FS_NOADR                        1   	/*!< bad address */
#define FS_KO                           -2	/*!< other errors */
#define FS_ERROR                        (-1)	/*!< other errors */
char discoNetcom[60 + 1];

int verificarArchivo( char * nombreDisco, const char * nombreArchivo) {

    long iRet = 0;
    int resultado = 0;
    char rutaFile[100 + 1];

    memset(rutaFile, 0x00, sizeof(rutaFile));

    strcpy(rutaFile,nombreDisco );
    strcat(rutaFile,nombreArchivo);

    FILE* pxFid = fopen(rutaFile, "r");

    if (pxFid == NULL) {
        iRet = FS_KO;
    } else {
        fclose(pxFid);
    }

    return iRet;
}

int borrarArchivo( char * nombreDisco,char * nombreArchivo) {

    int iRet = 0;
    char rutaFile[100 + 1];

    memset(rutaFile, 0x00, sizeof(rutaFile));

    strcpy(rutaFile,nombreDisco );
    strcat(rutaFile,nombreArchivo);
    iRet = unlink(rutaFile);
    return iRet;
}

int buscarArchivo(char * nombreDisco, char * nombreArchivo, char * dataLeida, int posicion, int tamLinea) {

    FILE* pxFid = NULL;
    unsigned int uiAccess = 0;
    long iRet = 0;
    long tamanoArchivo = 0;
    int resultado = 0;
    //char bufferRead[ + 1];
    memset(&pxFid, 0x00, sizeof(pxFid));
    memset(dataLeida, 0x00, sizeof(dataLeida));

    char rutaFile[100 + 1];

    memset(rutaFile, 0x00, sizeof(rutaFile));


    strcat(rutaFile,nombreArchivo);
    iRet = verificarArchivo(nombreDisco,rutaFile);
    if (iRet != FS_OK) {
        return -2;
    }

    // tamanoArchivo = sizeof(pxFid);

    memset(rutaFile, 0x00, sizeof(rutaFile));
    strcpy(rutaFile,nombreDisco );
    strcat(rutaFile,nombreArchivo );
    if (iRet == FS_OK) {
        pxFid = fopen(rutaFile, "r");
        fseek(pxFid, 0L, SEEK_END);
    }

    if (pxFid != NULL) {
        tamanoArchivo = ftell(pxFid);

        if (tamanoArchivo != 0) {
            fclose(pxFid);
            pxFid = fopen(rutaFile, "r");
            iRet = fseek(pxFid, posicion, FS_SEEKSET);
            if (iRet == FS_OK) {
                iRet = fread(dataLeida, 1, tamLinea, pxFid);

                if (iRet != tamLinea) {
                    memset(dataLeida, 0x00, sizeof(dataLeida));
                }
            } else {
                resultado = -1;
            }
        } else {
            resultado = -1;
        }
    }

    if (pxFid != NULL) {
        resultado = feof(pxFid);
        //iRet = FS_close(pxFid);
    } else {
        resultado = -1;
    }
    fclose(pxFid);
    return resultado;
}

int escribirArchivo(char * nombreDisco,  char * nombreArchivo, char * datosEscribir, int tamanoArchivo) {

    char rutaFile[100 + 1];

    memset(rutaFile, 0x00, sizeof(rutaFile));

    strcpy(rutaFile,nombreDisco );
    strcat(rutaFile,nombreArchivo);

    FILE* pxFid = fopen(rutaFile, "a"); // Identificador del Archivo
    long iRet = 0;


    if (pxFid == NULL) {
        iRet = FS_KO;
    }

    if (iRet != FS_ERROR && iRet != FS_KO) {

        iRet = fwrite(datosEscribir, 1, tamanoArchivo, pxFid);

    }

    fflush(pxFid);

    if (iRet != tamanoArchivo) {
        iRet = FS_KO;
    }

    fclose(pxFid);
    // _leerArchivo_(nombreArchivo,dataLeida);
    return iRet;
}



int actualizarArchivo(char * nombreDisco, char * nombreArchivo, char * datosEscribir, int posicion, int cantidadBytes) {

    FILE *pxFid;
    unsigned int uiAccess = 0;
    long iRet = 0;
    long tamanoArchivo = 0;
    int resultado = 0;
    int finalArchivo = 0;

    char rutaFile[100 + 1];

    memset(rutaFile, 0x00, sizeof(rutaFile));

    iRet = verificarArchivo(nombreDisco,nombreArchivo);
    if (iRet != FS_OK) {
        return -2;
    }
    strcpy(rutaFile,nombreDisco);
    strcat(rutaFile,nombreArchivo);
    if (iRet == FS_OK) {
        pxFid = fopen(rutaFile, "r+");
        fseek(pxFid, 0L, SEEK_END);
    }

    if (pxFid != NULL) {
        tamanoArchivo = ftell(pxFid);
        if (tamanoArchivo != 0) {
            iRet = fseek(pxFid, posicion, FS_SEEKSET);
            if (iRet == FS_OK) {
                iRet = fwrite(datosEscribir, 1, cantidadBytes, pxFid);
            } else {
                resultado = -1;
            }
        } else {
            resultado = -1;
        }
    }

    if (pxFid != NULL) {
        resultado = finalArchivo = feof(pxFid);
        //iRet = FS_close(pxFid);
    } else {
        resultado = -1;
    }
    fclose(pxFid);

    return resultado;
}

int actualizarArchivoCnb(char * nombreDisco, char * nombreArchivo, char * datosEscribir, int posicion,
                         int cantidadBytes) {

//    S_FS_PARAM_CREATE xCfg;
    FILE *pxFid;
    unsigned int uiAccess = 0;
    long iRet = 0;
    long tamanoArchivo = 0;
    int resultado = 0;
    //int finalArchivo = 0;
//    memset(&xCfg, 0x00, sizeof(xCfg));
//    memset(&pxFid, 0x00, sizeof(pxFid));
//    xCfg.AccessMode = FS_WRTMOD;
//    uiAccess = xCfg.AccessMode;

    iRet = verificarArchivo(nombreDisco, nombreArchivo);
    if (iRet != FS_OK) {
        return -2;
    }

    if (iRet == FS_OK) {
        pxFid = fopen(nombreArchivo, "r+");
    }
    LOGI("pxFid %s", pxFid);
    if (pxFid != NULL) {
        tamanoArchivo = ftell(pxFid);
        LOGI("tamanoArchivo %d", tamanoArchivo);
        if (tamanoArchivo != 0) {
            iRet = fseek(pxFid, posicion, FS_SEEKSET);
            LOGI("iRet %d", iRet);
            if (iRet == FS_OK) {
                resultado = fwrite(datosEscribir, 1, cantidadBytes, pxFid);
            } else {
                resultado = -1;
            }
        } else {
            resultado = -1;
        }
    }
    LOGI("resultado %d", resultado);
    LOGI("cantidadBytes %d", cantidadBytes);
    if (resultado != cantidadBytes) {
        resultado = -1;
    }

    fclose(pxFid);

    return resultado;
}

int leerArchivo(char * nombreDisco, char * nombreArchivo, char * dataLeida) {

    unsigned int uiAccess;
    long posicion = 0;
    FILE *pxFid = NULL;
    long iRet = 0;
    int tamanoArchivo = 6000;
    memset(dataLeida, 0x00, sizeof(dataLeida));

    char rutaFile[100 + 1];

    memset(rutaFile, 0x00, sizeof(rutaFile));

    strcpy(rutaFile,nombreDisco );
    strcat(rutaFile,nombreArchivo);
    pxFid = fopen(rutaFile, "r");
    fseek(pxFid, 0L, SEEK_END);
    if (pxFid == NULL) {
        iRet = FS_KO;
    }

    if (pxFid != NULL) {
        tamanoArchivo = ftell(pxFid); //Longitud del archivo en bytes
        iRet = tamanoArchivo;
    }

    char bufferRead[8000 + 1];
    memset(bufferRead, 0x00, sizeof(bufferRead));
    if (iRet != FS_ERROR && iRet != FS_KO) {
        iRet = fseek(pxFid, posicion, FS_SEEKSET); // Buscar posición dada
    }

    if (iRet == FS_OK && tamanoArchivo != 0) {
        iRet = fread(bufferRead, 1, tamanoArchivo, pxFid);
    }

    if (iRet != tamanoArchivo) {
        iRet = FS_KO;
    }
    fclose(pxFid);

    if (iRet == tamanoArchivo && iRet != FS_KO) {
        memcpy(dataLeida, bufferRead, iRet);
        //iRet = FS_close(pxFid);
        return tamanoArchivo;
    } else {
        return iRet;
    }
}


int renombrarArchivo(char * nombreDisco, char * nombreActual, char * nombreNuevo) {

    long iRet;
    char rutaFile[100 + 1];
    char rutaNuevo[100 + 1];

    memset(rutaFile, 0x00, sizeof(rutaFile));

    iRet = verificarArchivo(nombreDisco,nombreActual);
    if (iRet == FS_OK) {
        strcpy(rutaFile,nombreDisco );
        strcat(rutaFile,nombreActual);

        strcpy(rutaNuevo,nombreDisco );
        strcat(rutaNuevo,nombreNuevo);
        iRet = rename(rutaFile, rutaNuevo);
    }


    return iRet;
}
void cerrarArchivo(FILE *pxFid) {

    if (pxFid != NULL) {
        fclose(pxFid);
        pxFid = NULL;
    }

}

FILE *abrirArchivo(char * nombreDisco, char * nombreArchivo, char * modo) {

    unsigned int uiAccess;
    FILE *pxFid = NULL; // Identificador del Archivo
    long iRet = 0;
    char rutaFile[100 + 1];
    memset(rutaFile, 0x00, sizeof(rutaFile));

    strcpy(rutaFile,nombreDisco );
    strcat(rutaFile,nombreArchivo);

    if (iRet != FS_OK) {
        iRet = FS_KO;
    }

    if (iRet == FS_OK) {
        pxFid = fopen(rutaFile, modo);
    }

    return pxFid;
}


int operacionArchivo(FILE *pxFid, int tipoOperacion, char * data, int posicion, int cantidadBytes) {

    int tamanoArchivo = 0;
    int iRet = 0;
    int resultado = 0;

    if (pxFid != NULL) {
        //leer
        if (tipoOperacion == 1) {
            pxFid = fopen(data, "r");
            fseek(pxFid, 0L, SEEK_END);
            tamanoArchivo = ftell(pxFid);
            fclose(pxFid);
            if (tamanoArchivo != 0) {
                iRet = fseek(pxFid, posicion, FS_SEEKSET);
                if (iRet == FS_OK) {
                    iRet = fread(data, 1, cantidadBytes, pxFid);
                    if (iRet == cantidadBytes) {
                        resultado = iRet;
                    }
                } else {
                    resultado = -1;
                }
            } else {
                resultado = -1;
            }

            resultado = feof(pxFid);
        }

        //escribir archivo
        if (tipoOperacion == 2) {
            resultado = fwrite(data, 1, cantidadBytes, pxFid);
            fflush(pxFid);
            if (resultado != cantidadBytes) {
                resultado = FS_KO;
            }

        }

    }

    return resultado;
}


long tamArchivo(char * nombreDisco, char * nombreArchivo) {

    FILE *pxFid;
    unsigned int uiAccess = 0;
    long iRet = 0;
    long tamanoArchivo = 0;

    char rutaFile[100 + 1];

    memset(rutaFile, 0x00, sizeof(rutaFile));

    strcpy(rutaFile,nombreDisco );
    strcat(rutaFile,nombreArchivo);

    memset(&pxFid, 0x00, sizeof(pxFid));

    if (iRet == FS_OK) {
        pxFid = fopen(rutaFile, "a");
        if (pxFid != NULL) {
            fseek(pxFid, 0L, SEEK_END);
        }
    }

    if (pxFid != NULL) {
        tamanoArchivo = ftell(pxFid);
        LOGI("Netcom no esxp %s",rutaFile);
        LOGI("Netcom tamano %ld",tamanoArchivo);
    }

    if (pxFid != NULL) {
        //iRet = FS_close(pxFid);
    } else {
        tamanoArchivo = -1;
    }
    fclose(pxFid);
    return tamanoArchivo;
}

void crearDisco(char * path){
    strcpy(discoNetcom, path);
    LOGI("Netcom crear disco %s",discoNetcom);
}
