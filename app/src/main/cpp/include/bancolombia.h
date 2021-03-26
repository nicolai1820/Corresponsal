//
// Created by NETCOM on 10/02/2020.
//

#ifndef POS_PIN_ANDROID_BANCOLOMBIA_H
#define POS_PIN_ANDROID_BANCOLOMBIA_H
#include <jni.h>
typedef struct {
    uChar valorKotenR3[2 + 1];
    uChar tipoCuentaOrigen[2 + 1];
    uChar cuentaOrigen[19 + 1];
    uChar tipoCuentaDestino[2 + 1];
    uChar cuentaDestino[19 + 1];
    uChar indicador1[1 + 1];
    uChar indicador2[1 + 1];
    uChar referencia1[30 + 1];
    uChar referencia2[30 + 1];
    uChar factura[128 + 1];
    uChar tokensQF[42 + 1];
    uChar tokensp6[160 + 1];
    uChar tokenX1[2 + 1];
} DatosTokens;


typedef struct {
    uChar track2[TAM_TRACK2 + 1];
    uChar tarjetaHabiente[TAM_TARJETA_HABIENTE + 1];
    uChar ultimosCuatro[TAM_CODIGO_PROCESO + 1];
    uChar cardName[TAM_CARD_NAME + 1];
    uChar tipoCuenta[TAM_TIPO_CUENTA + 1];//10 20 30 40
    uChar terminalId[TAM_TERMINAL_ID + 1];
    uChar posEntrymode[TAM_ENTRY_MODE + 1];
    uChar processingCode[TAM_CODIGO_PROCESO + 1];
    uChar pan[19 + 1];
} DatosTarjetaSembrada;

typedef struct {

    int cantidadTransferencias;
    int cantidadRetiroTarjeta;
    int cantidadRetiroEfectivo;
    int cantidadDeposito;
    int cantidadRecaudo;
    int cantidadPagoTarjetaCredito;
    int cantidadPagoCartera;
    int cantidadConsultas;
    int cantidadEnvioGiros;
    int cantidadReclamacionGiros;
    int cantidadCancelacionGiros;
    int cantidadAnuladasDeposito;
    int cantidadAnuladasTransferencia;
    int cantidadAnuladasRecaudo;
    int cantidadAnuladasPagoTarjeta;
    int cantidadAnuladasPagoCartera;
    int cantidadCupos;

    long totalTransferencias;
    long totalRetiroTarjeta;
    long totalRetiroEfectivo;
    long totalDeposito;
    long totalRecaudo;
    long totalPagoTarjetaCredito;
    long totalPagoCartera;
    long totalEnvioGiros;
    long totalReclamacionGiros;
    long totalCancelacionGiros;
    long totalAnuladasDeposito;
    long totalAnuladasTransferencia;
    long totalAnuladasRecaudo;
    long totalAnuladasPagoTarjeta;
    long totalAnuladasPagoCartera;

} DatosCierreCnbBancolombia;

#define ORIGEN_ANULACION	"ANULACION"
#define ORIGEN_DUPLICADO	"DUPLICADO"
#define FALLBACK				"fb"

#define BIN_BANCOLOMBIA            "890007"
#define BIN_BANCOLOMBIA_RETIRO    "581682"
#define FECHA_VENCIMIENTO        "4912"
#define FIID_BANCOLOMBIA        "0007"

#define INDICADOR_TCREDITO        "1"
#define INDICADOR_CBANCO        "2"
#define INDICADOR_DOCUMENTO        "3"        //NUEVAS ESPECIFICACIONES
#define INDICADOR_CTA_CNB        "4"
#define INDICADOR_CTA_RECAUD    "5"    //NUEVAS ESPECIFICACIONES

#define TOKEN_SALDO                "00"
#define TOKEN_RETIRO            "09"
#define TOKEN_DEPOSITO            "15"
#define TOKEN_RETIRO_EFECTIVO    "16"
#define TOKEN_RECAUDO            "17"
#define TOKEN_TRANSFERENCIA        "19"
#define TOKEN_PAGO_TARJETA        "20"
#define TOKEN_PAGO_CARTERA        "21"
#define TOKEN_ENVIO_GIRO        "25"
#define TOKEN_RECLAMAR_GIRO        "26"
#define TOKEN_CONSULTA_GIRO        "27"
#define TOKEN_CANCELAR_GIRO        "28"
#define TOKEN_CONSULTA_CUPO        "31"

#define RETIRO_TARJETA            0
#define RETIRO_EFECTIVO            1
#define RETIRO_OTRA_CTA            2

#define IS_SUMAN_CIERRE(ID) ((ID == TRANSACCION_BCL_RETIRO || ID == TRANSACCION_BCL_DEPOSITO || ID == TRANSACCION_BCL_RECAUDO || ID == TRANSACCION_BCL_PAGO_TARJETA || ID == TRANSACCION_BCL_PAGO_CARTERA || ID == TRANSACCION_BCL_TRANSFERENCIA)?1:0 )
int enviarGiro(char *monto);

int procesarTransaccion(void);
void solicitarSegundoRecibo(int tipoRecibo);
int unpackRespuestaSaldo(void);
int unpackDataTransferencia(void);
int capturarDatos(char tipoTarjeta);
int enviarRetiro(int tipoTransaccion);
int cierreCnbBancolombia(int tipoCierre);
int _cierreCnb_(void);
void armarTramaRetiro(int intentosVentas);
int unpackDataRetiro(int tipoTransaccion);
void setVariableGlobalesRetiro(void);
int procesarTransaccionTransferencia(void);
void verificarCosto(uChar *costoTransaccion);
void formatearDatos(DatosTokens *datosTokens);
void imprimirDetalleCierreCNB(int tipoCierre);
int duplicadoUltimoRecibo(int tipoDuplicado);

int retiroEfectivo(char *monto, char *cuenta, char * pinblock);

void extraerTokens(char *token, uChar *buffer);

void armarTramaTransferencia(int intentosVentas);

DatosTarjetaSembrada cargarTarjetaSembrada(void);

int verificarTokenQZ(uChar *data, int lengthData);

int procesarTransaccionRetiro(int tipoTransaccion);

int verificarTokenQi(uChar *data, int lengthData);

int enviarAnulacion(DatosCnbBancolombia *datosVenta);

void obtenerTerminalModoCorresponsal(uChar *terminal);

extern DatosCnbBancolombia buscarReciboCNBByID(int id);

void mostrarAprobacionBancolombia(DatosVenta *datosVenta);

void getTextoTransaccion(int tipoTransaccion, char *texto);

int duplicadoOtroRecibo(char* numeroRecibo, int tipoDuplicado);

void armarTrack(uChar *buffer, uChar *pan, int tipoTransaccion);

int procesarTransaccionAnulacion(DatosCnbBancolombia datosVenta);

void mostrarAprobacionAnulacion(DatosCnbBancolombia *datosVenta);

void armarTokensP6(DatosTokens datosTokens, int tipoTransaccion);

int verificacionTramabancolombia(DatosCnbBancolombia *tramaVenta);

TablaTresInicializacion traerIssuerCNB(uChar *pan, uChar *track2);

void mostrarAprobacionDeposito(DatosCnbBancolombia *datosDeposito);

int unpackDataRecaudo(uChar *tokenP6Recibido, int tipoRecaudoCaja);

void empaquetarP60(DatosTokens datosTokens, uChar *tipoTransaccion);

int enviarTransPago(char *referencia, char *monto, int tipoProducto);

int procesarTransaccionDeposito(DatosCnbBancolombia *datosDeposito);

void armarTrama(char *pinblock, int intentosVenta, int intentosPin);

void imprimirDeclinadas(DatosVenta datosDeposito, char *mensajeError);

void extraerCamposAprobacionAnulacion(DatosCnbBancolombia *datosVenta);

void cargarVariable(uChar *buffer, int *indice, uChar *datos, int tam);

int procesarTransaccionPagoProducto(DatosCnbBancolombia datosPagoProducto);

int procesarTransaccionRecaudo(uChar *tokenP6Enviado, int tipoRecaudoCaja);

int enviarRecaudo(DatosTokens datosToken, int tipoRecaudoCaja, char *total);

void armarTramaAnulacion(DatosCnbBancolombia *datosVenta, int intentosVentas);

void armarTramaDeposito(DatosCnbBancolombia *datosDeposito, int intentosVentas);

void imprimirDeclinadasCNB(DatosCnbBancolombia datosDeposito, char *mensajeError);

int unpackDataAnulacion(DatosCnbBancolombia datosVenta, uChar *numeroReciboTokenP6);

void armarTramaRecaudo(int intentosVentas, DatosTokens datosToken, int tipoMensaje);

void imprimirDuplicadoBancolombia(DatosCnbBancolombia datosVenta, int tipoDuplicado);

int duplicadoCnbBancolombia(int opcionRecibo, int tipoRecibo, char *numeroRecibo);

void empaquetarEncabezado(DatosVenta *datosDeposito, uChar *pan, int intentosVentas);

int enviarDeposito(char *cuenta, char *monto, int tipoCuenta, char* codigoAprobacion);
//int enviarDeposito();

void imprimirAnuladas(int cantidad, long total, int tipoTransaccion, int tipoCierre);

void armarTramaPagoProducto(DatosCnbBancolombia datosPagoProducto, int intentosVentas);

int desempaquetarConsultaRecaudo(uChar *datosRecibidos, int tam, uChar *tokenP6Recibido);

int unpackData(DatosCnbBancolombia *datosDeposito, char *dataRecibida, int cantidadBytes);

int enviarConsultaRecaudo(DatosTokens datosToken, DatosRecaudoAndroid *datosRecaudoAndroid);

void convertirDatosBancolombia(DatosVenta datosVenta, DatosCnbBancolombia *datosBancolombia);

int buscarRecibo(char *numeroRecibo, DatosCnbBancolombia *datosVenta, uChar *origenSolicitud);

void imprimirTicketBancolombia(DatosCnbBancolombia reciboVenta, int tipoTicket, int duplicado);

void imprimirTransaccionDeclinadaBancolombia(DatosTransaccionDeclinada datosTransaccionDeclinada);

int enviarConsultaRecaudoManual(DatosRecaudoAndroid *datosRecaudoAndroid, char * codigoAprobacion);

int unpackDataPagoProducto(DatosCnbBancolombia *datosPagoProducto, char *dataRecibida, int cantidadBytes);

int capturarMontoRecaudo(long valorFactura, int indicadorBaseDatos, int indicadorValor, uChar *tipoRecaudo);

int enviarTransaccionSaldoBCL(DatosTarjetaAndroid datosTarjetaAndroid, int intentosPin, char *codigoRespuesta);

void guardarTransaccionBancolombia(char *nombreDisco, char *nombreArchivo, DatosCnbBancolombia datosDeposito);

void cargarIndicadoresDB(uChar *tokenRecibido, long *valorFactura, int *indicadorBaseDatos, int *indicadorValor);

void cambiarEstadoTransaccionBancolombia(char *numeroRecibo, char *rrn, char *aprobacion, char *fecha, char *hora);

int enviarConsultaRecaudoLector(DatosRecaudoAndroid *datosRecaudoAndroid, char *codigoLeido, char *codigoAprobacion);

int confirmarDatosTeffGiros(char *titulo, char *mensaje1, char *mensaje2, char *mensaje3, char *mensaje4, char *mensaje5);

void armarP6(DatosTokens *datosTokens, uChar *cuentaRecaudadora, uChar *TCRecaudadora, int tipoRecaudoManual, uChar *tarjetaEmpresarial);

int compararTokens(uChar *tokenRecibido, uChar *tokenEnviado, int tipoComparacion, uChar *nombreConvenio, RecaudoMulticorresponsal *datosRecaudo);

void detallesTransaccion(int cantidad, long total, int tipoTransaccion, uChar *descripcion, int tipoCierre, int cantidadAnuladas, long totalAnuladas);

int enviarTransferencia(DatosTarjetaAndroid datosTarjetaAndroid, char *tpCuentaOrg, char *TipoCuentaD, char *cuentaDestino, char *otraCuenta,
                        char *monto);

int procesarConsulta(uChar *tokenP6, long *valorFactura, int *indicadorBaseDatos, int *indicadorValor, uChar *nombreConvenio,
                     RecaudoMulticorresponsal *datosRecaudo);

int consultaRecaudo(DatosTokens *datosTokens, long *valorFactura, int *indicadorBaseDatos, int *indicadorValor, uChar *nombreConvenio,
                    RecaudoMulticorresponsal *datosRecaudo);

int enviarTransaccionRetiroTarjetaBCL(DatosTarjetaAndroid datosTarjetaAndroid, int intentosPin, char *codigoRespuesta, char *tipoCuenta,
                                      char *cuentaDestino, char *monto, int otraCuenta);

void preAprobacionEmv();
//Definiciones Giros ///
int verificarNumeroItemsDocumentos();
DatosComision verificarComision(char *monto);
DatosArchivoPar menuTiposDocumento(int itemsTipoDocumento, int indice);
int reclamacionGiro(char * tipoDocumento, char * tipoDocumentoAux, DatosComision datosComision);
int realizarEnvioGiro(char * tipoDocumento, char * tipoDocumentoAux, DatosComision datosComision);
int enviarCancelacionGiro(char * tipoDocumento, char * tipoDocumentoAux, DatosComision datosComision);
int enviarConsultaCancelacionGiro(DatosComision *datosComision, char* tipoDocumento, char*tipoDocumentoAux, char*valorDevolucion);
void armarTramaGiro(DatosCnbBancolombia *datosEnviarGiro, int tipoTransaccion, uChar *tipoDocumento,uChar *tipoDocumentoAux,
                    DatosComision datosComision);
void armarConsultaCancelacion(DatosCnbBancolombia *datosEnviarGiro, int tipoTransaccion,
                              uChar *tipoDocumento,
                              uChar *tipoDocumentoAux,
                              DatosComision datosComision);

//Definiciones saldocupo
void mostrarCupoCb(char * respuesta);
int  gestionarConsultaCupoCB(char * respuesta);
int enviarConsultarSaldo(char * pinblock, int intentosPin);

int gestionarRecaudoTarjetaEmpresarial(char *track2Empresarial, char *monto);
int cierreCnbTerminalBancolombia(void);
int armarPanRetiroEfectivobancolombia(char * track2, char *pan, char * cardName);
extern int globalsizeData;
extern int globalsizep55;
extern int  isRecaudoNequi;
extern char globalCampo55[1024 + 1];
extern char globalpinblock[16 + 1];
extern char  mensajeDeclinada[30 + 1];
extern int globalTransaccionAprobada;
extern char globalDataRecibida[512 + 1];
extern DatosVenta datosVentaBancolombia;
extern uChar tokensP6[SIZE_TOKEN_P6 + 1];
extern uChar tokensQF[SIZE_TOKEN_QF + 1];
extern ResultISOUnpack globalresultadoUnpack;
extern ResultISOPack globalresultadoIsoPack;
extern DatosCnbBancolombia globalDatosBancolombia;

#endif //POS_PIN_ANDROID_BANCOLOMBIA_H
