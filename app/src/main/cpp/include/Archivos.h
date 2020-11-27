//
// Created by NETCOM on 04/12/2018.
//

#ifndef POS_ANDROID_ARCHIVOS_H
#define POS_ANDROID_ARCHIVOS_H
#define FS_SEEKSET                      0	/*!< seek from start of file */
#define FS_SEEKCUR                      1	/*!< seek from current position */
#define FS_SEEKEND                      2	/*!< seek from end of file */
#include <stdio.h>
extern int verificarArchivo(char * discoNetcom ,const char * nombreArchivo);
extern int escribirArchivo(char * nombreDisco,  char * nombreArchivo, char * datosEscribir, int tamanoArchivo);
FILE *abrirArchivo(char * nombreDisco, char * nombreArchivo, char * modo);
extern int buscarArchivo(char * nombreDisco, char * nombreArchivo, char * dataLeida, int posicion, int tamLinea);
extern int actualizarArchivo(char * nombreDisco, char * nombreArchivo, char * datosEscribir, int posicion, int cantidadBytes);
extern int actualizarArchivoCnb(char * nombreDisco, char * nombreArchivo, char * datosEscribir, int posicion,
                                int cantidadBytes);
int borrarArchivo( char * nombreDisco,char * nombreArchivo);
int renombrarArchivo(char * nombreDisco,char * nombreActual, char * nombreNuevo);
void crearDisco(char * path);
int leerArchivo(char * nombreDisco,char * nombreArchivo, char * dataLeida);
void cerrarArchivo(FILE *pxFid);
int operacionArchivo(FILE *pxFid, int tipoOperacion, char * data, int posicion, int cantidadBytes);
long tamArchivo(char * nombreDisco, char * nombreArchivo);
#endif //DESAC_ARCHIVOS_H
