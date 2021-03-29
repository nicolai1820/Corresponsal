//
// Created by ronald on 18/04/2020.
//

#ifndef APOS_LEALTAD_H
#define APOS_LEALTAD_H
#include"Definiciones.h"
#include"Tipo_datos.h"

#define ACTIVACION				1
#define REDENCION				2
#define CONSULTA				3
//#define OTROS					4

#define ACUMULAR				4
#define AJUSTAR_PUNTOS			5

#define CEDULA					1
#define NoLIFEMILES             2
#define TARJETA					3

#define TARJETA_LIFE			1
#define CEDULA_LIFE             2
#define No_LIFE					3

#define BANCOLOMBIA				1
#define DAVIVIENDA				2

#define REDENCION_BIG			1

#define ACUMULACION_LIFE		1
#define REDENCION_LIFE			2
#define ACUMULACION_LIFE_COMPRA 3

#define RED_TARJETA				1
#define RED_CEDULA				2
#define RED_NoLIFEMILES			3
#define RED_TAR_CED	     		4
#define RED_TAR_NoLIFE			5
#define RED_CED_NoLIFE			6
#define RED_TAR_CED_NoLIFE		7
#define ACU_TARJETA				8
#define ACU_CEDULA				9
#define ACU_NoLIFEMILES        10
#define ACU_TAR_CED  		   11
#define ACU_TAR_NoLIFE		   12
#define ACU_CED_NoLIFE		   13
#define ACU_TAR_CED_NoLIFE	   14

#define TAM_BUFFER            256
#define TIMEOUT_TX            252
#define CANCEL_TX             253

#define ANULACION				1
#define AJUSTAR					2

typedef struct{
    int tarjeta;
    int cedula;
    int ultimosCuatro;
    int enmascarado;
    int codigoCajero;
    int pin;
    int operacionesMixtas;
    int noLifemiles;
}CombinacionesLifemiles;
int invocarMenuLifeMiles();
int procesarLealtad(int acumularAjustar);
int validarBinLealtad(uChar *bin);
void seleccionarItemsDocumento(int opcionOtros);
int recibirOpcionMenu(int opcionLealtad);
void iniciarLifeMiles(DatosBasicosVenta datosBasicosVenta);
ConveniosBigBancolombia capturarDatosBig();
int combinacionesTipoDocumento(int opcionPago, int opcionOtros );
void cargarDatosTransaccionLifeMiles(int acumularAjustar, int transaccionCaja,DatosTarjetaAndroid leerTrack);
int realizarTransaccionLifemiles( int acumularAjustar, DatosTarjetaAndroid leerTrack);
int procesarLifeMiles( int acumularAjustar, int transaccionCaja, int cajaRiviera,DatosTarjetaAndroid leerTrack);
void armarTramaLifeMiles(int intentos, int acumularAjustar, char * pinblock);
int desempaquetarLealtad(uChar *datosRecibidos, int tamRecibidos,  int acumularAjustar);
int verificacionTramaLealtad(int acumularAjustar);
void tomarDatosPuntos();
void cambiarEstadoAcumulacionAjuste();
void aprobacionLealtad(int acumularAjustar);
void verificarCeroPuntos(uChar *puntosTransaccion, uChar *puntosAcumulados);
void imprimirTiketLealtad(int tipoTicket, int duplicado);
int procesoAcumulacionRedencionLifeMiles(int lectura, int opcionLealtad, int modoEntrada, DatosTarjetaAndroid leerTrack);
int ingresoDocumentoFromJava(char *documento, int modoEntrada);
void concatenarCerosMontos();


//DEFINICIONES PUNTOS COLOMBIA
int otroMedioPago(int opcionPago);
int pagoParcial(char * monto);
int pagoTotal(DatosVenta *datosVenta, int cajaActiva);
int puntosColombia(DatosBasicosVenta datosBasicosVenta);
int redimirPuntosColombia(int tipoDocumento, char *documento, char *codigoCajero, char * galones, char * monto);
int acumularPuntosColombia(int tipoDocumento, char *documento, char *codigoCajero, char * galones);
int recibirOpcionMenuPuntosColombia(int opcionLealtad, int tipoDocumento, char *documento, char *codigoCajero, char *galones, char * monto);
void cambiarEstadoTxPuntosColombia(char *numeroRecibo, char * rrn, char * aprobacion, char* fecha, char* hora, char* textoAdicional);
int anulacionPuntosColombia(int tipoAnulacionReverso);
extern CombinacionesLifemiles combinacionesLifemiles;

#endif //APOS_LEALTAD_H
