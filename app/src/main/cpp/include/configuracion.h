//
// Created by NETCOM on 04/12/2018.
//

#ifndef POS_ANDROID_CONFIGURACION_H
#define POS_ANDROID_CONFIGURACION_H

//Parametros//
#define NII									1
#define NUMERO_TERMINAL 			        2
#define TIPO_COMUNICACION		            3
#define TIPO_CANAL							4
#define IP_DEFAULT							5
#define PUERTO_DEFAULT 				        6
#define IP_SALIDA							7
#define PUERTO_SALIDA 					    8
#define GPRS_APN							9
#define GPRS_USER							10
#define GPRS_PASS							11
#define TELEFONO_INICIAL				    12
#define CONSECUTIVO         				13
#define TERMINAL_INICIALIZADA 		        14
#define TIPO_TRANSACCION            		15
#define IVA_AUTOMATICO      				16
#define VALOR_CLAVE_ADMINISTRADOR 	        17
#define VALOR_CLAVE_SUPERVISOR 			    18
#define VALOR_CLAVE_BONOS 					19
#define NUMERO_PLACA     		        	20
#define DIVIDIDAS_ACTIVO    	        	21
#define IVA_MULTIPLE  			        	22
#define IMPRIMIR_QPS  				        23
#define NII_MULTIPOS  			         	24
#define VALOR_TRANSACCION_LIMITE		    25
#define VALOR_CVM_LIMITE					26
#define VALOR_FLOOR_LIMITE					27
#define TEXTO_COMERCIO              		28
#define ESTADO_TECLADO		        		29
#define INVOICE								30
#define VERSION_SW							31
#define REVISION_SW							32
#define BATCH								33
#define IP_VENTA_PRIMARIA					34
#define PUERTO_VENTA_PRIMARIA 			    35
#define TIMEOUT_VENTA_PRIMARIA		        36
#define INTENTOS_VENTA_PRIMARIA		        37
#define IP_VENTA_SECUNDARIA					38
#define PUERTO_VENTA_SECUNDARIA 	        39
#define TIMEOUT_VENTA_SECUNDARIA	        40
#define INTENTOS_VENTA_SECUNDARIA	        41
#define IP_CIERRE_PRIMARIA					42
#define PUERTO_CIERRE_PRIMARIA 		        43
#define TIMEOUT_CIERRE_PRIMARIA		        44
#define INTENTOS_CIERRE_PRIMARIA		    45
#define IP_CIERRE_SECUNDARIA				46
#define PUERTO_CIERRE_SECUNDARIA 	        47
#define TIMEOUT_CIERRE_SECUNDARIA	        48
#define INTENTOS_CIERRE_SECUNDARIA	        49
#define MAXIMO_MONTO						50
#define MAXIMO_IVA  						51
#define MAXIMO_PROPINA						52
#define MAXIMO_BASE							53
#define TIME_TELECARGA_AUTOMATICA		    54
#define TIME_INICIALIZACION_AUTOMATICA	    55
#define HABILITAR_CALL_WAIT					56
#define CODIGO_CALL_WAIT					57
#define VELOCIDAD_DIAL						58
#define HABILITAR_TONO_MARCACION			59
#define NUMERO_PBX							60
#define TIPO_MARCACION						61
#define TIME_ULTIMA_INICIALIZACION_EXITOSA	62
#define TIME_ULTIMA_TELECARGA_EXITOSA		63
#define TIPO_INICIALIZACION				    64
#define CONSECUTIVO_OFFLINE					65
#define  PPP_ACTIVO						    66
#define INC_AUTOMATICO						67
#define INC_MULTIPLE						68
#define MAXIMO_INC                  		69
#define IMPRIMIR_CIERRE						70
#define CALCULAR_BASE						71
#define MINIMO_MONTO						72
#define CODIGO_COMERCIO						73
#define NOMBRE_COMERCIO						74
#define DIRECCION_COMERCIO					75
#define NOMBRE_TRANSACCION					76
#define CREDITO_ROTATIVO					77// no se usa en su lugar TIPO_TRANSACCION_AUX
#define ID_BONOS							78
#define REVERSOS_ENVIADOS					79
#define LECTOR_BARRAS_ACTIVO   				80
#define MODO_DATAFONO						81
#define OTROS_PAGOS							82
#define LLAVE_EFTSEC						83
#define DIGITO_CHEQUEO_ETSEC				84
#define KIN_EFTSEC							85
#define ACTIVAR_RECARGA_TARJETA				86
#define ACTIVAR_RECARGA_PUNTO_RED			87
#define ACTIVAR_RECARGA_MOVIL_RED			88
#define WORKING_KEY_LOGON_3DES  			89
#define TIPO_CAMBIO_CLAVE           		90
#define TITULO_CAMBIO_CLAVE         		91
#define ASIGNACION_CLAVE_ACTIVO     	    92
#define TARIFA_ADMIN						93
#define COMERCIO_IATA						94
#define RECIBO_PENDIENTE					95
#define CAJA_REGISTRADORA           		96
#define CODIGO_CAJERO_CAJA          		97
#define RECARGA_EFECTIVO_CAJA       		98
#define NUMERO_ULTIMA_ANULADA      			99
#define CAJA_NO_RESPONDE           			100
#define HABILITACION_VIVAMOS	   			101
#define TRANSACCION_VIVAMOS		   			102
#define TIPO_TERMINAL			   			103
#define TIPO_INICIA_TRANSACCION	  			104
#define VERIFICAR_PAPEL_TERMINAL   			105
#define CNB_BANCO							106
#define CNB_TIPO_CUENTA						107
#define CNB_PEDIR_PIN						108
#define CNB_TERMINAL						109
#define HABILITACION_MODO_CNB				110
#define HABILITACION_MODO_CNB_AVAL			111
#define TIMEOUT_CONSULTA_DINERO_ELECTRONICO_PRIMARIA		112
#define TIMEOUT_CONSULTA_DINERO_ELECTRONICO_SECUNDARIA	    113
#define NII_DCC								114
#define USER_CNB_LOGON  		    		115
#define CNB_INICIA							116
#define HABILITACION_BONOS_KIOSCO			117
#define CNB_TIPO_TARJETA					118
#define CNB_INICIALIZADO					119
#define USER_CNB			        		120
#define ICIERRE_PARCIAL_CITIBANK			121
#define HABILITACION_MODO_CNB_CITIBANK		122
#define CONSULTA_COSTO_CNB	  				123
#define HABILITACION_MODO_CNB_BCL			124			/// Modo Bancolonbia
#define INDICADOR_OFFLINE					125
#define NUMERO_FACTURAS_AVAL       			126
#define AVANCE_EFECTIVO						127
#define TIPO_TRANSACCION_REVERSO    		128
#define TIPO_TRANSACCION_CITIBANK   		129
#define NII_CELUCOMPRA              		130
#define DESCUENTO_TARJETA_CAJA      		131
#define USO_CAJA_REGISTRADORA       		132
#define HABILITACION_IMPRIMIR_CIERRE_AVAL   133
#define DELAY_CAJAS                	 		134
#define TIEMPO_CAJAS						135
#define CNB_REALIZAR_CIERRE_PARCIAL  		136
#define ESTADO_CAJA                  		137
#define CAMBIAR_CLAVE_OPERADOR_EFECTIVO 	138
#define MODO_FALLBACK               		139
#define DEBUG_COMUNICACIONES        		140
#define CNB_CAMBIO_TERMINAL 				141
#define INICIALIZACION_CAMBIO_TERMINAL   	142
#define NUMERO_TERMINAL_AUXILIAR         	143
#define CAMBIO_TERMINAL_CIERRE      		144
#define VALIDAR_CERO_UNICO          		145
#define IMPRIMIR_INC						146
#define TERMINAL_INICIALIZADA_ANTERIOR   	147
#define TIPO_TRANSACCION_AUX        		148
#define MONTO_TRANSACCION_LIMITE    		149
#define MONTO_CVM_LIMITE            		150
#define IMPRESION_RECIBO_CAJA       		151
#define CNB_B_BANCOLOMBIA					152
#define CNB_T_C_BANCOLOMBIA		    		153
#define CNB_T_T_BANCOLOMBIA					154
#define CNB_B_AVAL							155
#define CNB_T_C_AVAL       		   	 		156
#define CNB_T_T_AVAL			    		157
#define NOMBRE_REVERSO						158
#define MODO_QR				        		159
#define NUMERO_TERMINAL_QR		   	 		160
#define MODO_PAYPASS		        		161
#define MODO_CONSULTA						162
#define FALLBACK_FALABELLA					163
#define INTENTOS_CONSULTA_QR				164
#define IMPRIMIR_PANEL_FIRMA				165
#define ESTADO_TELECARGA	    			166
#define TIPO_TELECARGA              		167
#define TIPO_CAJA							168
#define URL_TEFF							169
#define IP_CAJAS							170
#define ID_TEFF								171
#define PRODUCTO_PPRO_DCC		    		172
#define TIPO_TRANSACCION_CAJA               173
#define TIPO_TRANSACCION_OTP                174
#define TIMEOUT_TRANSACCION                 175
#define TIME_ULTIMO_CIERRE_EXITOSO          176
#define HORA_CIERRE                         177
#define CIERRE_EXITOSO                      178
#define PROCESO_CIERRE                 		179
#define FLUJO_UNIFICADO			    		180
#define FLUJO_UNIFICADO_EMV		    		181
#define MULTICORRESPONSAL_ACTIVO    		182
#define	VELOCIDAD_CAJAS						183
#define	BIG_BANCOLOMBIA						184
#define PRIMER_IVA_MULTIPLE					185
#define SEGUNDO_IVA_MULTIPLE				186
#define PRIMER_INC_MULTIPLE					187
#define SEGUNDO_INC_MULTIPLE				188
#define CAJAS_IP_CORRESPONSAL   			189
#define NUMERO_PBX_AUX						190
#define INICIALIZACION_ISMP                 191
#define INDICADOR_REVERSO_ISMP				192
#define ACUMULAR_LIFEMILES                  193
#define CONSECUTIVO_CNB						194
#define FLUJO_CAJA_CLESS                    195
#define BONO_TEFF_ACTIVO					196
#define HABILITACION_MODO_CNB_CORRESPONSAL	197
#define CNB_B_CORRESPONSAL					198
#define CNB_T_C_CORRESPONSAL  		  		199
#define CNB_T_T_CORRESPONSAL				200
#define RESPUESTA_CAJA_IP					201
#define URL_TEFF2							202
#define ID_TEFF2							203
#define TIPO_CAJA_USB						204
#define GALONES_ACTIVOS						205
#define HABILITA_FIRMA_IP        		    206
#define URL_FIRMA							207
#define IP_FIRMA							208
#define ID_FIRMA							209
#define NII_FIRMA							210
#define IP_FIRMA_ORIGEN						211
#define EFTSEC_FIRMA						212
#define CATALOGOS_DIGITALES					213
#define HABILITAR_CONSULTA_BONOS			214
#define PUERTO_TEFF							215
#define NII_TEFF							216
#define EFTSEC_TEFF							217
#define PRODUCTO_PUNTOS_COLOMBIA			218
#define	IP_PUNTOS_COLOMBIA					219
#define PUERTO_PUNTOS_COLOMBIA				220
#define DOMINIO_PUNTOS_COLOMBIA				221
#define NII_PUNTOS_COLOMBIA					222
#define EFTSEC_PUNTOS_COLOMBIA				223
#define COMPRA_CON_REFERENCIA	     		224
#define PUNTOS_COLOMBIA_ACTIVO				225
#define WORKING_KEY_APOS        			226

#define SIZE_NII							4
#define SIZE_NUMERO_TERMINAL 	        	8
#define SIZE_TIPO_COMUNICACION 	         	2
#define SIZE_TIPO_CANAL 					2
#define SIZE_IP_DEFAULT 					15
#define SIZE_PUERTO_DEFAULT 				5
#define SIZE_GPRS_APN						30
#define SIZE_GPRS_USER						30
#define SIZE_GPRS_PASS						30
#define SIZE_TELEFONO_INICIAL   			15
#define SIZE_CLAVE_ADMINISTRADOR			6
#define SIZE_CLAVE_SUPERVIDOR				4
#define SIZE_ID_BONOS						2
#define SIZE_FIELD_63						98

#define SIZE_CODIGO_BANCO_CNB				4
#define SIZE_MAX_NUM_TARJETA 				19
#define SIZE_MIN_NUM_TARJETA				1

#define SIZE_ID_TEFF						15
#define SIZE_DOMINIO						30
#define SIZE_TIPO_CAJA						1

#define LEN_BUFFER_MAX             	1024
#define TOTAL_FIELD             	64
#define MAX_FIELD               	18

#define TAM_TIPO_TRANSACCION	    2
#define TAM_ESTADO_TRANSACCION	    1
#define TAM_NUMERO_RECIBO		   	6
#define TAM_SYSTEM_TRACE		   	6
#define TAM_MTI         		   	4
#define TAM_CODIGO_PROCESO		   	6
#define TAM_COMPRA_NETA 		    12
#define TAM_TIME        		    10
#define TAM_DATE        		   	8
#define TAM_ENTRY_MODE     		    3
#define TAM_NII          		    3
#define TAM_POS_CONDITION  		    2
#define TAM_TRACK2 					50
#define TAM_TERMINAL_ID				8
#define TAM_ACQIRER_ID				15
#define TAM_FIELD_57				50
#define TAM_FIELD_61				50
#define TAM_RRN     				12
#define TAM_CARD_NAME 				15
#define TAM_COD_APROBACION   		6
#define TAM_GRUPO        		    2
#define TAM_SUBGRUPO       		    1
#define TAM_FIELD_42        		15
#define TAM_TEXTO_ADICIONAL			25
//#define TAM_ACTIVO_QPS				1
#define TAM_FECHA_EXPIRACION		4
#define TAM_TARJETA_HABIENTE		50
#define TAM_MODO_PRODUCTO    		2
#define TAM_TEXTO_INFORMACION		120
#define TAM_CREDITO_ROTATIVO		1
#define TAM_ID_COMERCIO				10//se ajusta a diez solicitud RBM
#define TAM_COMERCIO				22
#define TAM_ESTADO_TARIFA		    2
#define TAM_CODIGO_CAJERO		    10
#define TAM_CAJERO_LIFE_MILES	    10
#define TAM_FACTURA_CAJA		    10
#define TAM_CODIGO_RESPUESTA		2
#define TAM_TOKEN_VIVAMOS           56
#define TAM_TOKEN_QR                28
#define TAM_ID_VALIDACION       	6
#define TAM_AUX       				50

#define FS_OK                           0	/*!< success */
#define FS_NOADR                        1   	/*!< bad address */
#define FS_KO                           -2	/*!< other errors */
#define FS_ERROR                        (-1)	/*!< other errors */


int _createParameter_(void);
int setParameter(int usKey, char *pcParam);
int getParameter(int usKey, char *pcParam);
int capturarImpuestoAutomatico(int tipoImpuesto);
int capturarImpuestoMultiple(int tipoImpuesto);
int validarcapturaImpuestoAutomatico(char * datoCapturado, int tipoImpuesto);
int solicitarImpuestosMultiples(char * datoPrimerImpuesto, char * datoSegundoImpuesto,int tipoImpuesto);
void crearDisco(char * path);
void getTerminalId(char * terminal);
int verificarModoCNB();
int getModoCNB(char *texto);
int validarMulticorresponsal(void);
int estadoTerminalConfigurado(void);
int verificarCambioTerminal(char * numeroTerminal);
void setearTransaccion(int tipoTransaccion);
void obtenerLlaveWorking(char * working);
extern char discoNetcom[60 +1 ];
#endif //DESAC_CONFIGURACION_H
