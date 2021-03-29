//
// Created by NETCOM on 04/03/2020.
//

#ifndef POS_INGENICO_VENTA_H
#define POS_INGENICO_VENTA_H
#include"Definiciones.h"
#include"Tipo_datos.h"

#define TARJETA_MOVISTAR			8002
#define TARJETA_COMCEL   			8003
#define TARJETA_TIGO    			8004
#define TARJETA_UFF     			8020
#define TARJETA_DIRECTV     		8006

#define IS_IMPAR(ID) ((ID == 1 || ID == 3 || ID == 5 || ID == 7 || ID == 9 || ID == 11)?1:0)
#define COMPRA_OTP 	             	   1
#define RETIRO_OTP			  	       2
#define BIN_PAGO_ELECTRONICO  	                "881017"
#define FECHA_VENCIMIENTO_PAGO_ELECTRONICO  	"6012"
#define ENVIO_PAGO_ELECTRONICO  	               1
#define ENVIO_SOLICITUD_PAGO_ELECTRONICO  	       2
#define ENVIO_ANULACION_PAGO_ELECTRONICO  	       3
#define ENVIO_CONSULTA_PAGO_ELECTRONICO_CLESS 	   4
#define MAXIMO_NUMERO_ENTIDADES                    10

#define PEDIR_NUEVAMENTE_PIN    -4
#define MAXIMO_INTENTOS_PIN     -3
#define COD_CREDITO 	             	   0
#define COD_AHORRO			  		       1
#define COD_CORRIENTE 	             	   2
#define COD_AHORRO_SIN_REF		  	       4
#define COD_CORRIENTE_SIN_REF 	       	   5
#define COD_TUYA						  80

typedef struct {
    int cantidadRecargaCelularEfectivo;
    int cantidadMovistar;
    int cantidadComcel;
    int cantidadTigo;
    int cantidadUff;
    int cantidadDirectv;
    long totalRecargaCelularEfectivo;
    long totalMovistar;
    long totalComcel;
    long totalTigo;
    long totalUff;
    long totalDirectv;
} TotalesRecargaCelular;

typedef struct {
    int cantidadAnuladas;
    int cantidadDebitos;
    int cantidadTransacciones;
    long totalDebitos;
    long totalAnuladas;
    long totalInc;
    long totalIva;
    long totalCompraNeta;
    long totalBase;
    long totalPropina;
} TotalesCelucompra;

typedef struct {
    int cantidadAnuladas;
    int cantidadDebitos;
    int cantidadTransacciones;
    long totalDebitos;
    long totalAnuladas;
    long totalInc;
    long totalIva;
    long totalCompraNeta;
    long totalBase;
    long totalPropina;
} TotalesQrcode;

typedef struct {
    int cantidadAnuladas;
    int cantidadDebitos;
    int cantidadTransacciones;
    long totalDebitos;
    long totalAnuladas;
    long totalInc;
    long totalIva;
    long totalCompraNeta;
    long totalBase;
    long totalPropina;
} TotalesConsultaEstado;

typedef struct {
    int cantidadAnuladas;
    long totalAnuladas;
} TotalesBigBancolombia;

//definiciones Cierre//
int _cierreVenta_(void);
void limpiarDatos(void);
void cierreQrcode(void);
int cierreCorresponsales();
void cierreCeluCompra(void);
void limpiarDatosMultiPos(void);
void cierreConsultaEstado(void);
void ordenarGrupoFranquicia(void);
void imprimirTotalesPaypass(void);
void imprimirTotalesPayWave(void);
int getIdGrupo(char * codigoGrupo);
void imprimirReporteAuditoria(void);
int _reporteCierre_(int tipoReporte);
void imprimirReporteCierre(int rTotales);
extern DatosVenta _buscarReciboByID_(int id);
int buscarComercioMultiPos(char *idComercio);
int cierreDescuadradoVenta(int totalTransacciones);
void llenarDatosComercio(DatosVenta datosVenta);
void restarTamAjusteLealtadUltimoRecibo(long *tam);
int getIdSubgrupo(int idGrupo, char * codigoSubgrupo);
void invocarMenuBig(char * convenio, char * fiid, int idx);
int desempaquetarCierreVentas(char * inputData, int cantidadBytes);
int empaquetarTramaVenta(DatosVenta datosVenta, char * outputData);
void totalizarRecargaCelularTarjeta(uChar * operador, long totalVenta);
void totalizarRecargaCelularEfectivo(uChar * operador, long totalVenta);
void totalizarRecargaCelular(uChar * plataforma, uChar * operador, long totalVenta);
DatosCierreVenta calcularCierreVenta(int totalTransacciones, int incluyeAnuladasOffline);
ResultISOPack empaquetarCierreVentas(DatosCierreVenta datosCierreVenta, char * codigoProceso);


//DEFINICIONES DUPLICADOS////


void _duplicado_(void);
int buscarUltimoRecibo(int tipoTicket);
void otroReciboDuplicado(void);
void _ultimoReciboDuplicado_(void);
int buscarRecibo(char *numeroRecibo, int tipoTicket);
DatosVenta buscarReciboByID(int id);
DatosVenta obtenerDatosVenta(char *data);
void imprimirDuplicado(DatosVenta datosVenta, int tipoTikect);
void duplicadoNumeroRecibo(uChar *numeroRecibo);
void _imprimirDuplicado_(DatosVenta datosVenta);
void _duplicadoNumeroRecibo_(uChar *numeroRecibo);
void duplicadoComercio(DatosJournals datosJournals);
void imprimirReporteFocoSodexo(DatosVenta datosTransaccion);
DatosJournals buscarReciboMulticorresponsal(uChar *numeroRecibo);
void resumenComercio(int idx, DatosJournals datosJournal, ResumenTransacciones *resumenTransacciones,int tipoTransaccion);
void resumenMultBancolombia(int idx, DatosJournals datosJournal, ResumenTransacciones *resumenTransacciones,int tipoTransaccion);


//DEFINICIONES CONSULTA DE SALDO

int consultarSaldo(DatosTarjetaAndroid datosTarjeta, char *saldo);
extern int validarUltimosCuatroBanda(uChar * ultimosCuatro);

//DEFINICIONES ANULACION
int verificarModoJMR(void);
void _anulacion_(int tipoTransaccion);
DatosVenta ultimoReciboAnulacion(void);
DatosVenta _obtenerDatosVenta_(char * data);
void responderAnulacionCaja(DatosVenta datos);
void anularTransaccion(DatosVenta datosVenta);
void imprimirTicketDeclina(DatosVenta datosVenta);
void _anulacionNumeroRecibo_(uChar *numeroRecibo);
int _buscarTransaccionesTipo_(int tipoTransaccion);
void anularTransaccionOffLine(DatosVenta datosVenta);
int verificarUltimoRecibo(DatosVenta * ultimaVenta);
DatosVenta _buscarReciboAnulacion_(char *numeroRecibo);
int realizarConsultaReciboAnulacion(char *numeroRecibo);
int realizarConsultaReciboAnulacionBanbolombia(char *numeroRecibo);
int verificarAnulacionTarjeta(DatosVenta * datosVenta,char * estadotJ);
int mensajeAnulacionLaneSms(char * linea1, char *linea2, char * celular);
ResultISOPack empaquetarAnulacion(DatosVenta * datosVenta, int intentos);
extern DatosJournals buscarReciboMulticorresponsal(uChar *numeroRecibo);
int optenerPosicionTransaccion(char *numeroRecibo, char* datosVentaPosicion);
int enviarAnulacion(ResultISOPack resultadoIsoPackMessage, char * dataRecibida);
int realizarAnulacionTarjeta(int leerCard, DatosTarjeta leerTrack, char * track2);
int desempaquetarAnulacion(DatosVenta * datosVenta, char * inputData, int cantidadBytes);
void cambiarEstadoTransaccion(char *numeroRecibo, char * rrn, char * aprobacion, char* fecha, char* hora);
int validarRespuesta(char * p3Inicial, char * p4Inicial, char * p11Inicial, char * p41Inicial, char * inputData,int longitud);
int solicitarPinAnulacionBanda(char * posEntryMode, char* pidioPin, char * track2, char * cardName, char * totalVenta, char *pinBlock);


//DEFINICIONES CONSULTAS DE INFORMACION Y COSTOS
void mostrarInformacion(char * informacion);
int _consultarCosto_(DatosVenta * tramaVenta);
int transaccionCosto(DatosVenta * tramaCosto);
int preguntarConsultaInformacion(char TipoMensaje);
int _consultarInformacion_(DatosVenta * datosVenta);
int validarCosto(int permiteCosto, uChar * campoCosto);
int validarCosto(int permiteCosto, uChar * campoCosto);
int desempaquetarConsulta(uChar * dataRecibida, int totalBytes);
ResultISOPack empaquetarConsultaInformacion(DatosVenta datosVenta);
TokenMessage desempaquetarConsultaInformacion(char * inputData, int cantidadBytes);
int activarTransaccionConsulta(char * bin, char * tipoTransaccion, char * modoDatafono);
void _verificarConsultaInformacion_(DatosVenta * datosVenta, uChar * inputData, int cantidadBytes);

//DEFINICIONES CORE
int verificacionLlegadaTrama(DatosVenta * tramaVenta);
void estadoTarifa(DatosVenta reciboVenta, char * mensaje);
int ventaQps(DatosVenta * datosVenta, ResultISOUnpack resultadoUnpack);
int verificarTokenQi(DatosVenta * datosVenta, uChar * data, int lengthData);
void imprimirTicketVenta(DatosVenta reciboVenta, int tipoTicket, int duplicado);
void verificarPuntosEmisor(DatosVenta * datosVenta, uChar * data, int lengthData);
void getPuntosEmisor(uChar * line1, uChar * line2, uChar * line3, uChar * data1, uChar * data2, uChar * isPuntos,int tipoTicket);
int enviarTransaccion(DatosVenta *tramaVenta, int transaccionQPS, char * pinblock, uChar *tokenDC,DatosCampo61Eco campo61Eco, int intentosPin, uChar *tokenDP, int isDivididas, char * campo11TEff);

//DEFINICIONES DCC
int definirEntero(uChar hexa);
int verificarBinesDCC(uChar * bin);
int verificarBinTarjeta(uChar *bin);
DatosEmpaquetados armarCampo63(void);
DatosEmpaquetados armarCampo60(void);
void convertirMayusculas(uChar *ptrS);
DatosCampo63 capturarDatoCampo63(void);
void getDateAndTime(uChar * dateAndTime);
void obtenerFechaHora(uChar * fechaHora);
void asignarHexa(int entero, uChar *hexa);
void obtenerCodigoUnico(uChar *codigoUnico);
int verificarTagTarjeta(uChar * codigoPais);
int preguntarDccPPRO(DatosVenta *datosVenta);
void convertLongToHex(long numero, uChar * hexa);
int verificacionTramaRecibida(uChar *systemTrace);
ResultTokenPack armarTokenDP(DatosVenta *datosVenta);
int definirExistenciaCampo(uChar *bitMap, int idCampo);
int definirSimboloDecimales(int codigo, uChar *simbolo);
void seleccionarTarjeta(uChar * tipoTarjeta, uChar *type);
DatosInicializacionDcc traerDatosInicialiazacionDcc(void);
int extraerCampo(int IDcampo, uChar *buffer, uChar *campo);
int preguntarUsoDcc(uChar *campo61, DatosVenta *datosVenta);
int buscarBin(uChar *rangoBajo, uChar *rangoAlto, uChar * bin);
int consultaDCC(DatosVenta * tramaVenta, uChar * codigoMoneda);
void armarTokenDcc(uChar * informacionMoneda, uChar * tokenDcc);
int comprobarActivacionFranquicia(uChar *cardName, uChar *codigoPais);
ResultISOPack empaquetarMensajeDcc(DatosVenta *datosVenta, uChar *codigoPais);
ResultISOPack empaquetarConsultaDCC(DatosVenta tramaVenta, uChar * codMoneda);
ResultISOPack empaquetarMensajePPRO18(DatosVenta *datosVenta, uChar *codigoPais);
ResultTokenPack armarTokenDC(uChar *campo61, uChar * monto, DatosCampo61Eco *campo61Eco);
void calcularMontoMargen(uChar *monto, uChar *porcentaje, uChar *resultado, uChar *margen);
int realizarConsultaDcc(DatosVenta *datosVenta, uChar *codigoPais, uChar *campo61, uChar *campo63);
DatosEmpaquetados armarCampo61(uChar *totalVenta, uChar *nombreTarjeta, uChar *track2, uChar *codigoPais);
int desempaquetarRespuesta(uChar *datosRecibidos, int longitudDatos, uChar *campo61, DatosVenta *datosVenta);
int procesarRespuestaConsultaDCC(uChar * dataReceive, int nBytes, uChar * tokenDcc, DatosVenta * tramaVenta);
int desempaquetarConsultaPPRO18(uChar *datosRecibidos, int longitudDatos, uChar *campo63, DatosVenta *datosVenta);
int procesarVerificacionDCC(uChar * montoConvertido, uChar * tasaCambio, uChar * montoTransaccion,DatosVenta * tramaVenta);

//Definiciones Venta EMV
int menuTipoDeCuenta(void);
void transaccionVentaEMV(void);
static void cargarCampos_22_35();
ResultISOPack ensambleTramaTransacion();
static void cargarCampos_4_48_57(char* ventaT);
static void cargarCampos_11_22_35_24_25_41_52_53_55();
int solicitudTipoCuenta(DatosTarjetaAndroid *datosTarjetaAndroid);
int solicitudTipoCuentaNuevosAid(int *outCuenta, int solicitaCRotativo);
int realizarVentaEMV(DatosTarjetaAndroid datosTarjeta, DatosBasicosVenta datosBasicosVenta);
int comportamientoTarjeta(DatosTarjetaAndroid datosTarjetaAndroid);
int solicitudCuenta(DatosTarjetaAndroid *datosTarjetaAndroid, int tipoTarjetaAid);
int comportamientotiposdeCuenta(int cuenta, int *codigoTipoCuenta, int solicitaCRotativo, char * binCafam);
int solicitudTipoCuentaDebitoEMV(int *outCuenta, int solicitaCRotativo);
int solicitudTipoCuentaCreditoEMV(int *outCuenta, int solicitaCRotativo);
//Definiciones manejo de impuestos//
int capturarDatosBasicosVenta(DatosBasicosVenta *datosBasicosVenta);
int capturarMonto(char *montoCapturado,char *montoMaximo, char *montoMinimo, int impuesto);
int capturarImpuestoIVaManual(char *ivaManual, char *totalVenta);
int capturarBaseDevolucionManual(char * baseDevolucion, char * totalVenta, char * iva);
int capturarImpuestoIncManual(DatosBasicosVenta *datosBasicosVenta);
int validarPropinaImpuesto(int OpcionPropina,DatosBasicosVenta *datosBasicosVenta, int porcentajePropina);
int validarPropina(int OpcionPropina, DatosBasicosVenta *datosBasicosVenta, uChar * maximoPropina, int porcentajePropina);
int verificarhabilitacionTasaPropina();
//Definiciones Cierre y reportes
void imprimirReporteCierreVentas(int rTotales);

//Definiciones QRCODE
int verificarTipoQrcode();
int mostrarQR(uChar *dataQr);
int realizarSolicitudQR(int anulacion);
DatosTerminalQr capturarDatoTerminal(void);
DatosQrCodeEmvco capturarDatosQrCode(void);
void imprimirReciboQR(DatosVenta datosVenta);
int decodificarDato(uChar caracter, int *num);
int consultaAutoQR(int sizeData, char * buffer);
int armarTramaConsultaQR(DatosVenta datosVenta);
int pintarImagenQr(uChar *imagen, int tamImagen);
DatosEmpaquetadosQrCode capturarConsultaQR(void);
DatosVenta valorCompraNeta(DatosVenta datosVenta);
int convertirDecimalBitAscii(int num, uChar *bitAscii);
void mostrarEstado(char * codigoRespuesta, char *datosAuxiliar);
int realizarConsultaQR(DatosVenta datosVenta, int modoConsulta);
int armarTramaSolicitudPaypass(DatosBasicosVenta datosBasicosVenta);
int crearEncabezadoBMP(uChar *imagen, int cantidadModulos, int tipoSalida);
int crearImagenBMP(uChar *cadBase64, uChar *imagen, int tamModulos, int tipoSalida);
int desempaquetarSolicitud(char * dataRecibida, int tipoTransaccion, int cantidadBytes);
int eliminarDatosQrCode(DatosVenta informacionQrCode, int numTransacciones, int identificador);
int armarCamposQrCode(DatosVenta * datosVenta, char * nii, char * codigoComercio, int inicioPaypass);
int generarQrcodeEmvCo(int sizeX, int sizeRow, int factor,uChar * img, int tamimagen, uChar * source);
int desempaquetarConsultaQRcode(char * dataRecibida, int tipoTransaccion, int cantidadBytes, DatosVenta datosVenta);
int direccionamientoQRcode(char * terminal, char * nii, char * processingCode, char *posEntrymode , char * codigoComercio );

//DEFINICIONES DINERO ELECTRONICO//
TablaSieteInicializacion obtenerOpcionMenu(int opcion, char * valor);
ParametrosDineroElectronico capturarDatosTransaccion(int tipoTxOtp, int opcion, char *valor);
void armarTramaDineroElectronico(char *pinblock, int transaccionDineroElectronico, int intentos,
                                 int intentosPin);
int procesarCompraDineroElectronico(char *pinblock, int intentosPin);
int procesarTransaccion(int tipoTransaccion);
int procesarDatosTarjeta(DatosTarjeta *leerTrack);
int capturarPINEmisores(char *track);
int consultaCompraDineroElectronico(char *pinblock, int intentosPin);
DatosEmpaquetados armarCampo42(DatosVenta datosVenta);
DatosIniQrCode datoInteroperabilidad(void);
void imprimirTicketDineroElectronico(DatosVenta reciboVenta, int tipoTicket, int duplicado);
int crearEntidad(char *entidad);
void imprimirCierreDineroElectronico(DatosCierreDineroElectronico datosCierre, int cantidad);
int unpackRespuestaDineroElectronico(int tipoTransaccion, char *dataRecibida, int sizeData);
int verificarAnulacion(char *estadoTransaccion, char *totalVenta, char *numeroRecibo);
void realizarImpresion(int duplicado);
int getNumeroEntidades(void);
void seteaEstructuraEntidades(void);
void imprimirCierreRetiro(DatosCierreDineroElectronico datosCierre, int cantidad);
void solicitarMenuEntidades();
ParametrosDineroElectronico
armarTrack2DineroElectronico(int  tipoDocumento, char *documento, char *celular);
int verificarSolicitudPin();
int realizarProcesoTransaccionDineroElectronico(char* pinblock, int intentosPin,char* tipoCuenta,char* cuotas);
void obtencionDatosBasicosVenta(DatosBasicosVenta datosBasicos);
void cierreDineroElectronicoRetiro(DatosBasicosCierre *basicos);
void cierreDineroElectronico(DatosBasicosCierre *basicos);

extern DatosVenta globalTramaVenta;

#endif //POS_INGENICO_VENTA_H
