//
// Created by NETCOM on 04/12/2018.
//

#ifndef POS_ANDROID_INICIALIZACION_H
#define POS_ANDROID_INICIALIZACION_H

#include "Utilidades.h"
#include "Tipo_datos.h"
void respaldoTablas(void);
void inicializarDCC(void);
int inicializarPaypass(void);
void inicializarQRCODE(void);
int traerInicializacionBcl();
int inicializacionBonos(void);
int inicializarMultiPos(void);
int validarDCC(uChar byteDCC);
int inicializacionPuntos(void);
void inicializarBigBancolombia();
void borrarTablas(int tipoTabla);
int inicializacionEmisores(void);
void _inicializacionSerial_(void);
void inicializarMulticorresponsal();
void _restaurarInicializacion_(void);
int buscarBinEmisor(uChar * binEmisor);
int desempaquetarTablaParametros(void);
void calcularBitsEncendidos(char * byte);
DatosQrCodeEmvco capturarDatosQrCode(void);
int _armarTokenAutorizacion_(char* buffer);
DatosIniQrCode capturarDatosQrCodeRbm(void);
void habilitarMultiplesCorresponsales(void);
void inicializacionConsultaInformacion(void);
TablaUnoInicializacion desempaquetarCNB(void);
DatosConsulta obtenerDatosConsulta(char * data);
TablaTresInicializacion _traerIssuer_(long bin);
void leftTrim_(uChar * dataLeft, uChar caracter);
ConfiguracionExcepciones excepcionesEmv(long bin);
int inicializarExcepciones(int tipoInicializacion);
void rightTrim_(uChar * dataRight, uChar caracter);
TablaSieteInicializacion invocarMenuEntidades(void);
EmisorAdicional traerEmisorAdicional(int indiceEmisor);
TablaTresInicializacion dataTablaTres(uChar * tablaTres);
TablaUnoInicializacion _desempaquetarTablaCeroUno_(void);
TablaTresInicializacion buscarIssuer(uChar issuerTableId);
TablaCuatroInicializacion _desempaquetarTablaCuatro_(void);
int buscarBin(uChar *rangoBajo, uChar *rangoAlto, long bin);
int invocarInicializacionGeneral(char * codigoProcesoInicial);
void configurarTime(void);
TablaConsultaInformacion traerConsultaInformacion(uChar * bin);
TablaSeisInicializacion desempaquetarTablaSeis(uChar indiceGrupo);
void logonMultipos(uChar * codigoProceso, uChar * codigoRespuesta);
void leftPad(uChar * destination, uChar * source, char fill, int len);
void configurarDatosComunicacion(TablaCuatroInicializacion tablaCuatro);
int buscarBinOptimo(RangoBinesEncontrados * rangoBin, int cantidadBines);
int transaccionInicializacion(uChar * codigoProcesoInicial, uChar * codigoVerificacion, uChar * nombreArchivo, uChar * nii);
int codigoTerminalCnb(char *numeroTerminal);
int _inicializacionCNB_(char * codigoProcesoInicial, char *texto, int tipoAutomatico);
int inicializacionParametros(void);
#endif //DESAC_INICIALIZACION_H
