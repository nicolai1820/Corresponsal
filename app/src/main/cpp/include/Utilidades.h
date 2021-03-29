//
// Created by NETCOM on 15/01/2020.
//

#ifndef POS_ANDROID_UTILIDADES_H
#define POS_ANDROID_UTILIDADES_H

typedef  char uChar;
typedef unsigned char boolean;

#include "Definiciones.h"
#include "Tipo_datos.h"
typedef unsigned short int  	word;




#define BCD_BITMAP          1
#define BCD_NUMERIC         2
#define BCD_LL_NUMERIC      3
#define BCD_AMOUNT          4
#define BCD_CHAR            5
#define BCD_LL_CHAR         6
#define BCD_LLL_CHAR        7
#define BCD_BINARY          8
#define LEN_BUFFER_MAX             	1024
#define LEN_SIZEM                	2
#define LEN_HEADER             	 	5
#define FS_SEEKSET                      0	/*!< seek from start of file */

#define FS_OK                           0	/*!< success */
#define FS_NOADR                        1   	/*!< bad address */
#define FS_KO                           -2	/*!< other errors */
#define FS_ERROR                        (-1)	/*!< other errors */


#define PARAMETRO_FINAL    999
#define MAX_PARAMETROS     999
#define TAM_KEY		    3
#define TAM_LENGTH		2
#define TAM_FIN_REGISTRO	2

#define GPRS_TCP_IP              1
#define GPRS_SSL                 2
#define ETH_IFO_ADDR   		0x0103

#define FS_OK                           0	/*!< success */
#define FS_NOADR                        1   	/*!< bad address */
#define FS_KO                           -2	/*!< other errors */
#define FS_ERROR                        (-1)	/*!< other errors */
#define NO_IMPRIMIR "NO_IMPRIMIR"

#define EXISTE_USUARIO                 1
#define EXISTE_USUARIO_ADMINISTRADOR   3
#define EXISTE_USUARIO_OPERADOR        4
#define NO_EXISTE_USUARIO    -1
#define LENGTH_USUARIOS_CNB  24

#define ADMINISTRADOR_CNB     1
#define OPERADOR_CNB          2

void formatoFecha(char *fecha);
void obtenerFecha(char *fecha);
void generarBatch(char *batch);
void generarInvoice(char *invoice);
int isIssuerEspecial(char *issuer);
int verificarHabilitacionCaja(void);
DatosTarjeta detectarTrack2(char* track2);
int verificarHabilitacionObjeto(int Objeto);
void generarUsuarioEstandar(uChar * usuario);
void borrarParametros(char * codigoRespuesta);
int borrartransacciones();
int deshabilitarCNB(void);
int habilitarCNBConCtaSembrada(char *codigoBanco, char *tipoCuenta, char *track2, char *nombreTarjetaHabiente);
char * stringtok(char * str, const char * delim);
void imprimirLineaDigital(LineaImprimir lineaRecibo);
int capturarClave(char *clave, char *claveCapturada);
int habilitarCNBconPAN(char *codigoBanco, char *numeroTarjeta, char *tipoCuenta);
void interpretarTrama(char * dataOrigen, int tamLeido);
int verificarUsuario(uChar * usuario, uChar * datosUsuario);
void leftTrim(uChar * destino, uChar * origen, uChar caracter);
static int searchString(const char *source, const char *oldChar);
void _rightTrim_(uChar * destino, uChar * origen, uChar caracter);
void generarSystemTraceNumber(char *numeroRecibo, char *pathNetcom);
void validarFallBack(DatosVenta *datosVenta, DatosTarjeta *leerTrack);
void alineacionCentrada(int cantidadCaracteresLinea, char *dataLinea);
LineaImprimir _obtenerLineaTemplate_(char * mystr, HasMap * valorHash);
void _rightPad_(char * destination, char * source, char fill, int len);
void _leftPad_(uChar * destination, uChar * source, char fill, int len);
void _convertBCDToASCII_(uChar *destination, uChar *source, int length);
void _convertASCIIToBCD_(uChar *destination, uChar *source, int length);
void imprimirTicket(HasMap *hash, char * logo, char * templateImpresion);
void errorRespuestaTransaccion(char * codigoRespuesta, char * mensajeError);
void redondearNumero(unsigned long *parteEntera, char decimales[], int cifras);
int crearUsuarioCnb(char *usuario, char *clave, char *tipoUsuario);
int cerrarSesion(void);
int comportamientoTarjetaLabelEMV(DatosVenta *datosVenta, TablaTresInicializacion dataIssuer);
DatosOperacion dividir(unsigned long dividendo, unsigned long divisor, int cifras, int redondear);
int justificarPalabra(char * palabraIn, char * palabraOut, int numeroPalabra, int maximoCaracteres);
void replaceString(const char *source, char *destination, const char *oldChar, const char *newChar);
void justificarLineasImpresion(char * cadenaDestino, LineaImprimir *justificarLineas, int longLinea);
void formatString(int caracter, long monto, uChar *cad_origen, int tam, uChar *cad_destino,int formato);
static void imprimirLineaDividida(LineaImprimir justificarLineas, int ,char *saltoLinea,char* alineacion,char *tipoLetra,char *formato);
int usuarioAmodificarCnb(char *usuario);
int eliminarUsuarioCnb(char *usuario);
int listadoUsuarios(void);
void imprimirEstablecimiento(void);
void imprimirListaUsuario(uChar * informacionUsuario, int textoRecibo);
int modificarUsuarioCnb(char *usuario, char *usuario2,char *clave , char *tipoUsuario);
int actualizarDatosUsuario(char * usuarioActual, char * datosActualizar);
int eliminarUsuario(char * usuarioActual);
ParametrosVenta traerParametrosVenta(DatosTarjeta datosTarjeta);
int validarSeleccionCuenta(int *pideCuotas, DatosVenta *datosVenta, DatosTarjeta *leerTrack,
                           int validarCreditoRotativo);
void validarTipotarjeta(char tipoTarjeta, DatosVenta * datosVenta);
void menuTipoDeCuenta(int validarSeleccionCuenta);




//DEFINICIONES GUI//
void screenMessageFiveLine(char *titulo, char *linea1, char *linea2, char *linea3, char *linea4,
                           char *linea5, char *lineaResumen);
void showMenu(char * titulo, int nitems, char* items, int longitud);
int validarMultiServicios(int *pideCuotas, DatosVenta *datosVenta, DatosTarjeta *leerTrack);
int menuMultiservicios(char * binCafam);
void showMenuEspecial(char * titulo, int nitems, char* items);
int solicitudMenuProducto(int producto);
int recibirOpcionMenuProducto(int producto, int iRet);
void horaFechaTerminal(char *hora, char *fecha);
void capturarEliminarReverso(void);
void imprimirReverso(char * nombreTransaccion, char * valorFormato, int tipoRecibo, char * terminal,
                     char * recibo, char * cuatroUltimos);
int verificarReversoExistente();
int
mostrarDetalleReverso(uChar *dataReverso, int tamReverso, int cantidadReversos, int numeroReverso);
void mostrarReversoPuntosColombia(uChar *dataReverso, int tamReverso, int cantidadReversos, int numeroReverso);
int datosReversoNormal(char* recibo,char* cuatroUltimos,  char* terminal, char* linea1);
int verificarValorReverso(char *linea1);
void imprimirticketPrueba(void);
int mostrarReverso(void);
int fechaActualizacion(ResultISOUnpack resultadoUnpack);
void validarActualizacionTelecarga(ResultISOUnpack resultadoUnpack);
int  validarInicializacionProgramada();
void cargarInformacionPanVirtualCorresposal(char * fiid, char* tipoCuenta, char * panVirtual);
Date obtenerDate();
extern char pathNetcom[60 + 1];
extern char discoNetcom[60 + 1];
extern char tipoCanal[2 + 1];
extern const char * internalStoragePath;
extern int isCnb;
extern int conectado;
#endif //POS_ANDROID_UTILIDADES_H
