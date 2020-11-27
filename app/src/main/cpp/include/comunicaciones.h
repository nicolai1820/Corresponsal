//
// Created by NETCOM on 26/02/2020.
//

#ifndef POS_PIN_ANDROID_COMUNICACIONES_H
#define POS_PIN_ANDROID_COMUNICACIONES_H
typedef  char uChar;

void releaseSocket();
int generarReverso(void);
int echoTest();
int logon3DES();
int verificacionTramaReverso(uChar * dataEnviada, int inLength);
int verificarReversoExistente(uChar * outputData, int tamanoInBuffer, int tipoTransaccion);
int  conectarTls(uChar * message, int len, uChar * dataRecibida, int tipoTransaccion,int cierraCanal, int generaReverso);
int realizarTransaccionTCP(uChar * message, int len, uChar * dataRecibida,int tipoTransaccion,int cierraCanal, int generaReverso);
int realizarTransaccion(uChar * outputData, int tamanoInBuffer, uChar * inputData, int tipoTransaccion, int cierraCanal, int generaReverso);
void definirParametrosPuntosColombia();
void leerParametros(int tipoTransaccion);
void definirParametrosDefault();
void crearReversoPuntosColombia();
int realizarConexionTlS();
int cerrarConexion();
void DestroySSL();
int conexionSSL();
int realizarConexionTCP();
#endif //POS_PIN_ANDROID_COMUNICACIONES_H


