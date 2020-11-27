//
// Created by NETCOM on 02/03/2020.
//

#ifndef POS_INGENICO_CAJAS_H
#define POS_INGENICO_CAJAS_H
#include "Tipo_datos.h"

typedef  char uChar;

#define SYN '\x16'
#define DLE '\x10'
#define ETX '\x03'
#define STX '\x02'
#define CR  '\x0d'
#define LF  '\x0a'

#define LEN_TEFF_PAN                12
#define FS  				0x1C
#define VERSION 			0x31
#define INDICADOR 			0x30
#define REQUERIMIENTO 		0x30
#define RESPUESTA 			0x31
#define MENSAJE 			0x32
#define ACK 				0x06
#define NACK 				0x15
#define TEFF_HEADER 		"6000000000"
#define DATOS_ENTRADA               1
#define DATOS_SALIDA                2
#define DATOS_ENTRADA_MONTOS        3
#define LEN_MENSAJE                 2
#define LEN_SIZE                    2

#define SIZE_CUENTA				    11		//// Indica el numero de cuenta para deposito

/*Mensajeria caja*/

int getTipoMensajeTEFF(void);
int getIdFieldCaja(int field);
void inicializarDataTEFF(void);
void inicializarCamposTEFF(void);
int cantidadCamposSeteados(void);
int getCodigoRespuestaTEFF(void);
int getCodigoTransaccionTEFF(void);
ResultISOPack packMessageTEFF(void);
void ordenarCampos(int cantidadCampos);
void setTipoMensajeTEFF(int tipoMensaje);
TEFFFieldMessage getFiledTEFF(int field);
TEFFFieldMessage packDataHeaderTEFF(void);
void setCodigoRespuestaTEFF(int codigoRespuesta);
TEFFFieldMessage getUnPackFieldMessage(int field);
TEFFFieldMessage getISOFieldMessageCaja(int field);
ResultISOPackRecibo packDataMessageTEFFRecibo(void);
void setCodigoTransaccionTEFF(int codigoTransaccion);
void unpackDataMessageTEFF(char *message, int inLength);
uChar calcularLRC(const uChar *buffer, unsigned int length);
void unpackDataHeaderMessageTEFF(char *header, int inLength);
void setFieldTEFF(int field, uChar * valueField, int totalBytes);
TEFFFieldMessage unpackDataFieldTEFF(char *message, int inLength);
ResultISOUnpack unpackMessageTEFF(uChar * messageTEFF, int inLength);
unsigned long hexToint(char * stringHexa, unsigned int lengthString);

/*CAJAS*/
void ajusteFecha(char *fecha);
int verificarCodigoCajero(void);
int definirLongitudCampo(int campo);
int validarCampo69(DatosVenta *datos);
int labelEspecialesDescuento(char *track);
int transaccionPeticionRetiro(DatosVenta *datos);
void cargarCamposLealtad(DatosVenta *datosVenta);
int confirmacionLRC(char* inBuffer, int longitud);
int transaccionPeticionDeposito(DatosVenta *datos);
int transaccionPeticionEfectivo(DatosVenta *datos);
int validarDescuentosCaja(char *token, char *tipo);
int verificarMontosInconsistentes(DatosVenta * datos);
int transaccionPeticionPagoTarjeta(DatosVenta *datos);
int transaccionPeticionDatosCompra(DatosVenta *datos);
int transaccionAnulacionBancolombia(DatosVenta *datos);
int cargarDatosCajaAnulada(DatosVenta* datosVentaCaja);
DatosQrDescuentos binQrDescuentos(char * TransaccionOTP);
int transaccionPeticionTarjetaPrivada(DatosVenta *datos);
void armarCamposGiros(int transaccion, DatosVenta datos);
int transaccionPeticionDatosAnulacion(DatosVenta *datos);
int transaccionPeticionRetiroEfectivo(DatosVenta *datos);
void validacionCodigoRespuestaCaja(char *codigoRespuesta);
int transaccionRespuestaRedimirFalabella(DatosVenta datos);
int transaccionPeticionDatosRecargaBono(DatosVenta *datos);
ResultISOPack tramaRespuestaClaveInvalida(int transaccion);
int transaccionPeticionDatosRedimirBono(DatosVenta *datos);
int tiposDeCuentaCaja(char* cuenta, int validacionEspecial);
void guardarTransaccionCajaAnulacion(DatosVenta tramaVenta);
int validarPosEntryMode(int tipoOperacion, char * posEntry);
int transaccionPeticionDatosHabilitarBono(DatosVenta *datos);
int ajustarDatosTransaccion(char *buffer, int len, int inOut);
int transaccionRespuestaDatos(DatosVenta datos, int transaccion);
int validarClaveSupervisor(char *claveSupervisor, int tipoClave);
int validarClaveSupervisorHabilitacionBono(char *claveSupervisor);
int transaccionPeticionEnvioGiro(DatosVenta *datos, int transaccion);
void guardarTransaccionCajasSincronizacion(char *buffer, int longitud);
int transaccionRespuestaDatosPrivada(DatosVenta datos, int transaccion);
int transaccionPeticionTransferencia(DatosVenta *datos, int transaccion);
int transaccionPeticionRecaudoManual(DatosVenta *datos, int transaccion);
int transaccionPeticionConsultaSaldo(DatosVenta *datos, int transaccion);
int transaccionRespuestaDatosBono(DatosVenta datos, int transaccionBono);
int transaccionRespuestaDatosLealtad(DatosVenta datos, int validacionClave);
int transaccionRespuestaDatosCaja(DatosVenta datos, int tipoTransaccionCaja);
int transaccionPeticionDatosLealtad(DatosVenta *datosVenta, int tipoTransaccion);
int verificarAnulacionBclPago(int tipoTransaccion, uChar *tipoTransaccionOrigen);
int verificarAnulacionBclRecaudo(int tipoTransaccion, uChar *tipoTransaccionOrigen);
int verificarAnulacionBclDeposito(int tipoTransaccion, uChar *tipoTransaccionOrigen);
int transaccionRespuestaCupo(uChar *bufferSalida, DatosVenta datos, int transaccionCupo, int tipoCaja);
int realizarTransaccionMedioComunicacion(uChar * packMessage, int totalBytes, uChar * recibida, int timeOut);
int realizarTransaccionAnulacionMedioComunicacion(uChar * packMessage, int totalBytes, uChar * recibida, int timeOut);


/*CAJAS IP*/
int verificarFirma(char * firmoOK);
int validarACKCajasIp(uChar *buffer);
int armarTpdu(char* data, int producto);
int reversarTransaccionFirma(int error);
int validarCampoCorresponsal(int campo);
int verificarDCC(DatosVenta *reciboVenta);
int identificarRespuestaHttp(uChar *buffer);
int cargarDatosVentaCajasIP(DatosVenta *datos);
void redefinirTamLlegada(uChar *bufferLlegada);
int cargarDatosLealtadCajasIP(DatosVenta *datos);
int agregarLongitud(int  indice, char* dataSend);
int cargarDatosEfectivoCajasIP(DatosVenta *datos);
int redefinirLongitudLRC(uChar *dataHex, int tam);
int respuestaClaveInvalidaIP(int tipoTransaccion);
int cargarDatosAnulacionCajasIP(DatosVenta *datos);
int verificarCodigoRespuestaCajasIP(uChar *buffer);
int cargarDatosRecargaBonoCajasIP(DatosVenta *datos);
int extraerDatosCaja(uChar *buffer, uChar *bufferCaja);
void verificarLLegadaCodigoCajero(uChar *codigoCajero);
int cargarDatosRedencionBonoCajasIP(DatosVenta *datos);
void generarDatosRespuestaAdicionales(DatosVenta datos);
int traerDatosConsumoRest(uChar *terminal, uChar *host);
void renombrarFileASincronizar(int tipoTransaccionCaja);
ResultISOPackRecibo packMessageTEFFReciboDinamico(void);
int validarCodigoTransaccionTEFF(int codigoTransaccion);
int cargarDatosHabilitacionBonoCajasIP(DatosVenta *datos);
int verificarCodigoRespuestaPuntosColombia(uChar *buffer);
int extraerDatosCajaWso2(uChar *buffer, uChar *bufferCaja);
int convertirCadenaHex(uChar *datosAscii, uChar *datosHex);
int traerDatosConsumoRestFirma(uChar *terminal, uChar *host);
int armarTramaEstandarSolicitudCaja(uChar *datoshex, int tam);
void definirCaracterAscii(uChar *caracterAscii, uChar caracterhex);
int cargarCamposEstructuraDatosVenta(char * datosVenta, int Campo);
int extraerDatosCajaPuntosColombia(uChar *buffer, uChar *bufferCaja);
int armarPeticionDatos(char *data, char* dataTransaccion, int indice);
int traerDatosConsumoRestRespuestaFinal(uChar *terminal, uChar *host);
int generarTramaPeticionIPCompra(uChar *bufferSalida, DatosVenta datos);
int generarTramaPeticionIPLealtad(uChar *bufferSalida, DatosVenta datos);
int generarTramaPeticionIPEfectivo(uChar *bufferSalida, DatosVenta datos);
int generarDatosRespuestaIPLealtad(uChar *bufferSalida, DatosVenta datos);
int armarRespuestaDatos(char *dataEnviada, char* bufferSalida, int indice);
int generarTramaPeticionIPAnulacion(uChar *bufferSalida, DatosVenta datos);
int cargarDatosPeticionTransaccion(DatosVenta *datos, int tipoTransaccion);
int generarDatosRespuestaIPAnulacion(uChar *bufferSalida, DatosVenta datos);
int generarTramaPeticionIPRecargaBono(uChar *bufferSalida, DatosVenta datos);
int generarTramaPeticionRedencionBono(uChar *bufferSalida, DatosVenta datos);
int unpackRespuestaCaja(char *bufferOut, int longitud, int tipoTransaccionCaja);
int generarTramaPeticionIPHabilitacionBono(uChar *bufferSalida, DatosVenta datos);
int armarReciboDinamico(uChar *bufferSalida, DatosVenta datos, int tipoTransaccion);
int cargarDatosRespuestaIP(uChar *bufferSalida, DatosVenta datos, int tipoTransaccion);
void extraerPanAsteriscos(char *track2, int lentrack2, char* outPan, int tipoAsteriaco);
int armarTramaReversoSip(DatosVenta datos, int tipoTransaccionCaja, uChar* dataEnviada);
int generarDatosRespuestaIPCompra(uChar *bufferSalida, DatosVenta datos, int transaccion);
int generarTramaPeticionIP(uChar *bufferSalida, DatosVenta datos, int tipoTransaccionCaja);
int generarDatosRespuestaIPBonos(uChar *bufferSalida, DatosVenta datos, int transaccionBono);
int respuestaFirmaIP(char * bufferSalida, char * terminal, char * tcpIpActual, int crearReverso);
int solicitarEnviarFirma(char* terminal2, char * recibo, int tipoTransaccionCaja, int partes, char * posEntrymode);
int adecuarRenglon(char * lineaSource, char * compare, int indice1, int longitud1, int indice2, int longitud2, char * campo98);
void armarLineaRecibo(char *lineaImprimir, int salto, char *alineacion, int tipoLetra, int tipoFormato, char *valscr, char *campo98);

//PUNTOS COLOMBIA//
int solicitudPuntosColombia(DatosVenta *datos, int tipotransaccion);
#endif //POS_INGENICO_CAJAS_H
