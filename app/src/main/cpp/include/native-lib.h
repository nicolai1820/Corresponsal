//
// Created by NETCOM on 26/11/2018.
//

#ifndef POS_ANDROID__NATIVE_LIB_H
#define POS_ANDROID__NATIVE_LIB_H
#include"Tipo_datos.h"
void _guardarTransaccion_(DatosVenta tramaVenta);
int enviarStringToJava(char *mensaje, char * metodo);
void pprintf(char* linea);
void mostrarPantallaNativa(char* linea);
void imprimirReciboTicket(void);
void _guardarDirectorioJournals_(short tipoJournal, uChar *numeroRecibo, uChar *estadoTransaccion);
#endif //POS_ANDROID__LIB_H

