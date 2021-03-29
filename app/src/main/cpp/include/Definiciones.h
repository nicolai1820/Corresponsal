//
// Created by NETCOM on 14/02/2020.
//

#ifndef POS_PIN_ANDROID_DEFINICIONES_H
#define POS_PIN_ANDROID_DEFINICIONES_H
#define TAM_JOURNAL					sizeof(DatosVenta) + 1 //600
#define IS_BONO(ID) ((ID == TRANSACCION_HABILITAR_BONO || ID == TRANSACCION_REDENCION_BONO || ID == TRANSACCION_RECARGAR_BONO)?1:0)
#define IS_MULTIPOS(ID) ((ID == TRANSACCION_MULTIPOS || ID == TRANSACCION_TARIFA_ADMIN)?1:0)
#define IS_NO_CIERRE(ID) ((ID == TRANSACCION_CELUCOMPRA || ID == TRANSACCION_CODIGO_ESTATICO || ID == TRANSACCION_CONSULTA_QR || ID == TRANSACCION_PUNTOS_COLOMBIA)?1:0)
#define IS_AVAL(ID) ((ID == TRANSACCION_RETIRO_CNB_AVAL	|| ID == TRANSACCION_PAGO_FACTURAS_AVAL || ID == TRANSACCION_DEPOSITO_EFECTIVO_CNB_AVAL   || ID == TRANSACCION_TRANSFERENCIA_AVAL || ID == TRANSACCION_P_PRODUCTO_TARJETA_AVAL  || ID == TRANSACCION_P_PRODUCTO_NO_TARJETA_AVAL)?1:0)
#define IS_OTRAS_AUDITORIAS(ID) ((ID == TRANSACCION_GASO_PASS )?1:0)//Toda dll que tenga su  auditoria de debe colocar aqui para que se realice internamente
#define IS_SUMAN_RESUMEN(ID) ((ID == TRANSACCION_VENTA || ID == TRANSACCION_GASO_PASS || ID == TRANSACCION_PAGO_ELECTRONICO || ID == TRANSACCION_PAGO_ELECTRONICO_ESPECIAL)?1:0 )

#define T_BANCOLOMBIA(transaccion) ((transaccion == TRANSACCION_TEFF_RETIRO_SIN_TARJETA || transaccion == TRANSACCION_TEFF_RECAUDO_MANUAL || transaccion == TRANSACCION_TEFF_RECAUDO_LECTOR_BARRAS || transaccion == TRANSACCION_TEFF_RECAUDO_TEMPRESARIAL || transaccion == TRANSACCION_TEFF_PAGO_TCREDITO || transaccion == TRANSACCION_TEFF_PAGO_CARTERA || transaccion == TRANSACCION_TEFF_TRANSFERENCIA_BCL || transaccion == TRANSACCION_TEFF_DEPOSITO_BCL) ? 1 : 0)

#define CAMBIO_TERMINAL_CIERRE_ACTIVO             		"1"
#define CAMBIO_TERMINAL_CIERRE_INACTIVO          	    "0"

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
#define ENVIA_POS_ENTRY_MODE_TEFF			226
#define BONO_VIRTUAL                        227
#define VERSION_QRCODE						228
#define TIEMPO_VISUALIZACION_QR_RBM			229
#define OMITIR_4_ULTIMOS					230
#define TIEMPO_PANTALLA_PAGO				231
#define TIEMPO_PANTALLA_CUOTAS				232
#define TIEMPO_PANTALLA_CUENTA				233
#define TIEMPO_RX_TEF_WS					234
#define NUMERO_RETRANS_WS					235
#define ALMACENAR_TX_WS						236
#define MODELO_CAJA_ESCUCHA					237


#define RECARGA_TARJETA		1
#define RECARGA_EFECTIVO	2

#define PARAMETRO_FINAL    999
#define MAX_PARAMETROS     999

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

#define CANAL_PERMANENTE       				0
#define CANAL_DEMANDA          				1

#define TRANSACCION_DEFAULT         						0
#define TRANSACCION_INICIALIZACION  						1
#define TRANSACCION_VENTA           						2
#define TRANSACCION_ANULACION       						3
#define TRANSACCION_SALDO           						4
#define TRANSACCION_REVERSO         						5
#define TRANSACCION_ECHO             						6
#define TRANSACCION_HABILITAR_BONO							7
#define TRANSACCION_REDENCION_BONO							8
#define TRANSACCION_CONVENIO								9
#define TRANSACCION_CONSULTA_BONO							10
#define TRANSACCION_CIERRE									11
#define TRANSACCION_RECARGAR_BONO							12
#define TRANSACCION_CUENTAS_DIVIDIDAS 						13
#define TRANSACCION_PSP										14
#define TRANSACCION_PCR										15
#define TRANSACCION_CONSULTA_RECARGA						16
#define TRANSACCION_RECARGA									17
#define TRANSACCION_EFECTIVO								18
#define TRANSACCION_TRANSFERENCIA							19
#define TRANSACCION_PUNTOS_COLOMBIA							20
#define TRANSACCION_PAYPASS		        					21
#define TRANSACCIONES_OTRAS		        					22 // Se crea esta nueva definicion, para implementar futuros menus de transacciones de emv que esten por inicializacion
#define TRANSACCION_CAMBIO_CLAVE        					23
#define TRANSACCION_MULTIPOS								24
#define TRANSACCION_TARIFA_ADMIN							25
#define TRANSACCION_DESCUENTOS_VIVAMOS  					26
#define TRANSACCION_EFECTIVO_VIVAMOS    					27
#define TRANSACCION_CONSULTA_VIVAMOS    					28
#define TRANSACCION_PAGO_ELECTRONICO    					29
#define TRANSACCION_CONSULTA_PAGO_ELECTRONICO   			30
#define TRANSACCION_CONSULTAR_SALDO							31
#define TRANSACCION_CONSULTAR_VALOR_CONSIGNAR				32
#define TRANSACCION_GASO_PASS								33
#define TRANSACCION_SOLICITAR_PRODUCTO						34
#define TRANSACCION_EFECTIVO_RETIRO							35
#define TRANSACCION_EFECTIVO_DEPOSITO						36
#define TRANSACCION_PSP_CNB									37
#define TRANSACCION_CNB_CITIBANK							38
#define TRANSACCION_TARJETA_MI_COMPRA						39
#define TRANSACCION_HABILITAR_BONO_CHIP						40
#define TRANSACCION_RETIRO_CNB_AVAL	    					41
#define TRANSACCION_BCL_SALDO								42
#define TRANSACCION_CONSULTA_AVAL   						43
#define TRANSACCION_BCL_DEPOSITO							44
#define TRANSACCION_PAGO_ELECTRONICO_ESPECIAL    			45 //Se coloca nueva transaccion para pago electronico, y diferenciar comportamiento nuevo en reversos
#define TRANSACCION_PAPELERIA								46
#define TRANSACCION_BCL_RECAUDO								47
#define TRANSACCION_CELUCOMPRA								48
#define TRANSACCION_BCL_RETIRO								49
#define TRANSACCION_PAGO_FACTURAS_AVAL  					50
#define TRANSACCION_BCL_RETIRO_EFECTIVO						51
#define TRANSACCION_BCL_TRANSFERENCIA						52
#define TRANSACCION_PAGO_TARJETA_AVAL						53
#define TRANSACCION_BCL_PAGO_TARJETA						54
#define TRANSACCION_BCL_PAGO_CARTERA						55
#define TRANSACCION_BCL_ANULACION							56
#define TRANSACCION_REVERSO_PAGO_FACTURAS_AVAL  			57
#define TRANSACCION_TARJETA_PRIVADA     					58
#define TRANSACCION_DEPOSITO_EFECTIVO_CNB_AVAL    			59
#define TRANSACCION_LECTURA_DIRECTA     					60
#define TRANSACCION_DCC                						61
#define TRANSACCION_MCBC_SALDO								62
#define TRANSACCION_MCBC_DEPOSITO							63
#define TRANSACCION_MCBC_RECAUDO							64
#define TRANSACCION_MCBC_RETIRO								65
#define TRANSACCION_MCBC_RETIRO_EFECTIVO					66
#define TRANSACCION_MCBC_TRANSFERENCIA						67
#define TRANSACCION_MCBC_PAGO_TARJETA						68
#define TRANSACCION_MCBC_PAGO_CARTERA						69
#define TRANSACCION_MCBC_ANULACION							70
#define TRANSACCION_MCBA_RETIRO             				71
#define TRANSACCION_MCBA_DEPOSITO							72
#define TRANSACCION_MCBA_RECAUDO							73
#define TRANSACCION_MCBA_CONSULTA							74
#define TRANSACCION_MC_CONVENIO								75
#define TRANSACCION_ACUMULAR_LEALTAD						76
#define TRANSACCION_AJUSTAR_LEALTAD							77
#define TRANSACCION_SOLICITUD_QR							78
#define TRANSACCION_CONSULTA_QR								79
#define TRANSACCION_CODIGO_DINAMICO							80
#define TRANSACCION_CODIGO_ESTATICO							81
#define TRANSACCION_REDIMIR_FALABELLA						82
#define TRANSACCION_CAJAS_IP								83
#define TRANSACCION_RETIRO_OTP       						84
#define TRANSACCION_MULTICOMERCIO		    				85
#define TRANSACCION_LOGGIN_DCC								86
#define TRANSACCION_PAYWAVE									87
#define TRANSACCION_TRANSFERENCIA_AVAL						88
#define TRANSACCION_P_PRODUCTO_TARJETA_AVAL					89
#define TRANSACCION_P_PRODUCTO_NO_TARJETA_AVAL				90
#define TRANSACCION_BIG_BANCOLOMBIA							91
#define TRANSACCION_SALDO_CUPO_AVAL							92
#define TRANSACCION_PAGO_IMPUESTOS							93
#define TRANSACCION_REDENCION_LIFEMILES						94
#define TRANSACCION_FOCO_SODEXO								95
#define TRANSACCION_CORRESPONSALES_ANULACION			    96
#define TRANSACCION_BCL_GIROS								97
#define TRANSACCION_BCL_RECLAMAR_GIROS						98
#define TRANSACCION_BCL_CANCELAR_GIROS						99
#define TRANSACCION_PURE									100
#define TRANSACCION_AMEX									101
#define TRANSACCION_REVERSO_PUNTOS_COLOMBIA					102

#define TRANSACCION_ACTIVA									1
#define TRANSACCION_ANULADA									0

#define MODO_COMERCIO      									1
#define MODO_PAGO											2
#define MODO_CNB_RBM										3
#define MODO_CNB_BANCOLOMBIA								4
#define MODO_CNB_AVAL										5
#define MODO_CNB_CITIBANK       							6
#define MODO_CNB_MULTICORRESPONSAL 							7
#define MODO_OFICINA										8
#define MODO_CNB_CORRESPONSAL								9

#define TIPO_PAGO_EFECTIVO_NCB								10

#define MODO_MIXTO											1
#define MODO_PURO											2
#define CNB_TIPO_TARJETA_VIRTUAL							1
#define CNB_TIPO_TARJETA_SEMBRADA							2
#define CNB_CIERRE_PARCIAL									1
#define CNB_CIERRE_TOTAL									2

#define TAM_TIPO_TRANSACCION	   			 				3
#define TAM_ESTADO_TRANSACCION	    						1
#define TAM_TIPO_CUENTA			    						2
#define TAM_NUMERO_RECIBO		  	  						6
#define TAM_SYSTEM_TRACE		    						6
#define TAM_MTI         		    						4
#define TAM_CODIGO_PROCESO		    						6
#define TAM_COMPRA_NETA 		    						12
#define TAM_IVA			 		    						12
#define TAM_TIME        		    						10
#define TAM_DATE        		    						8
#define TAM_ENTRY_MODE     		    						3
#define TAM_NII          		    						3
#define TAM_TRACK2 											50
#define TAM_TERMINAL_ID										8
#define TAM_ACQIRER_ID										15
#define TAM_FIELD_57										50
#define TAM_FIELD_61										50
#define TAM_RRN     										12
#define TAM_COD_APROBACION   								6
#define TAM_AID                     						22
#define TAM_ARQC                    						16
#define TAM_CUOTAS                   						2
#define	LEN_MSG_CODE										4
#define	LEN_BITMAP											8
#define	LEN_PAN												19
#define	LEN_PROC_CODE										6
#define	LEN_TRAN_AMT										12
#define	LEN_FRN_AMT											12
#define LEN_FIELD7											10
#define	LEN_DCC_RATE										8
#define	LEN_STAN											6
#define	LEN_LOCAL_TIME										6
#define	LEN_LOCAL_DATE										4
#define	LEN_EXP_DATE										4
#define	LEN_SETTLE_DATE										4
#define	LEN_ENTRY_MODE										3
#define	LEN_PAN_SEQ_NO										3
#define	LEN_NII												3
#define	LEN_COND_CODE										2
#define LEN_FIELD32             							20
#define	LEN_TRACK2											38
#define	LEN_TRACK3											104
#define	LEN_RRN												12
#define	LEN_AUTH_CODE										6
#define	LEN_RSP_CODE										2
#define	LEN_FIELD40											8
#define	LEN_TERM_ID											8
#define	LEN_MERCHANT_ID										15
#define	LEN_FIELD43											40
#define	LEN_ADDL_RSP										2
#define	LEN_TRACK1											76
#define	LEN_FIELD47											10
#define	LEN_FIELD48											100
#define	LEN_FRN_CURCY_CODE									3
#define	LEN_CURCY_CODE										3
#define	LEN_PIN_DATA										8
#define	LEN_EXT_AMOUNT										14
#define	LEN_ICC_DATA										520
#define	LEN_ICC_DATA2										110
#define	LEN_FIELD56											4
#define	LEN_FIELD57											256
#define	LEN_FIELD58											256
#define	LEN_FIELD60											512
#define	LEN_TMSFIELD60										600
#define	LEN_FIELD61											256
#define	LEN_INVOICE_NO										6
#define LEN_FIELD62             							20
#define	LEN_FIELD63											200
#define LEN_MAC												8

#define FIELD58						0x40
#define FIELD61						512

#define LEN_SIZEM                	2
#define LEN_HEADER             	 	5
#define LEN_SOURCE              	4
#define LEN_DESTINATION             4
#define LEN_BUFFER              	1300

#define LEN_BUFFER_MAX             	1024
#define TOTAL_FIELD             	64
#define MAX_FIELD               	18

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
#define TAM_ACTIVO_QPS				2
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

//CAJAS
#define TAM_CODIGO_OPERADOR			6
#define TAM_INF_ADICIONAL			128

#define SIZE_TOKEN_R3				2
#define SIZE_TIPO_CUENTA			2
#define SIZE_NUMERO_CUENTA			19
#define SIZE_CUENTA					11		//// Indica el numero de cuenta para deposito
#define SIZE_REFERENCIA				30
#define SIZE_FACTURA				128
#define SIZE_INDICADOR				1
#define SIZE_COD_SERVICO			5
#define SIZE_REFERENCIA_MANUAL		24
#define SIZE_TIPO_RETIRO			2
#define SIZE_CARTERA				11
#define SIZE_TARJETA_CREDITO		19
#define SIZE_TIPO_PRODUCTO			2

#define SIZE_TOKEN_QF				42
#define SIZE_TOKEN_P6				160
#define SIZE_TOKEN_X1				2    //NUEVO TOKEN PARA BANCOLOMBIA
#define _TRUE_			 			1
#define _FALSE_						0
#define _ERROR_					   -1
#define __BACK_KEY      		   -2
#define __EXIT_KEY      		   -3

#define INI_CNB_RBM  	1
#define INI_PUNTOS   	2
#define INI_BONOS    	3
#define INI_EMISORES 	4

#define RECIBO_CLIENTE      						1
#define RECIBO_COMERCIO     						2
#define RECIBO_QPS		     						3
#define RECIBO_DUPLICADO							4
#define RECIBO_NO_DUPLICADO							5
#define RECIBO_ANULACION							6

#define REVERSO_GENERADO							1
#define REVERSO_NO_GENERADO							0

#define TIPO_ENTRADA_MANUAL							1
#define TIPO_ENTRADA_LECTOR							2
#define TIPO_ENTRADA_CHIP							3

#define  TAG_TIPO_CUENTA_DIVIDIDA        			1
#define  TAG_CUENTAS_TOTAL_DIVIDIDA      			2
#define  TAG_CUENTAS_REALIZADAS_DIVIDIDA 			3
#define  TAG_VENTA_DIVIDIDA 						4
#define  TAG_PROPINA_DIVIDIDA 						5
#define  TAG_IVA_DIVIDIDA 							6
#define  TAG_BASE_DIVIDIDA 							7
#define  TAG_INC_DIVIDIDA 							8

#define CUENTAS_IGUALES             				1
#define CUENTAS_DIFERENTES          				2

#define PAGO_TARJETA                				1
#define TARJETA_DEBITO             	 				1
#define TARJETA_CREDITO             				2
#define PAGO_EFECTIVO              					2
#define PAGO_BONO                   				3

#define TIPO_COMUN_DIAL             				1
#define TIPO_COMUN_ETHERNET        					2
#define TIPO_COMUN_WIFI             				3
#define TIPO_COMUN_GPRS             				4

#define APN_CLARO_01          				"REDEBAN.COMCEL.COM.CO"
#define APN_CLARO_02          				"INTERNET.COMCEL.COM.CO"
#define APN_MOVISTAR         			    "VERTICALES.MOVISTAR.COM.CO"
#define APN_MOVISTAR_ALTERNO  	            "verticales.movistar.com.co"
#define APN_MOVISTAR_ALTERNO_2 	            "internet.movistar.com.co"
#define APN_MOVISTAR_ALTERNO_3 	            "INTERNET.MOVISTAR.COM.CO"
#define APN_TIGO              				"REDEBAN"
#define APN_AVANTEL_ALTERNO   	            "lte.avantel.com.co"
#define APN_AVANTEL			  				"LTE.AVANTEL.COM.CO"

#define MAX_FILE_DOWNLOAD  							70
#define DIAL		        						0
#define DIAL_EXTENDIDO								1
#define GPRS_CLARO  								2
#define GPRS_MOVISTAR  								3
#define GPRS_TIGO   								4
#define ETHERNET    								5
#define INTERNET    								6
#define GPRS_AVANTEL    							7
#define GPRS_MOVISTAR_INTERNET						8

#define IP_PUBLICA_01        "010000"
#define IP_PUBLICA_02        "010255"
#define IP_PUBLICA_03        "172016"
#define IP_PUBLICA_04        "172031"
#define IP_PUBLICA_05        "192168"

#define TAM_DIAL    		 				7
#define TAM_DIAL_EXTENDIDO  12

#define INICIALIZACION_MANUAL        	"930000"
#define INICIALIZACION_AUTOMATICA    	"930010"
#define INICIALIZACION_PROGRAMADA    	"930000"
#define SEGURIDAD_PIN_EMV           	"9801100100000000"

#define  TAG_TIPO_CUENTA_TRANSFERECIA        			1
#define  TAG_CUENTAS_TOTAL_TRANSFERECIA      			2
#define  TAG_CUENTAS_REALIZADAS_TRANSFERECIA 			3
#define  TAG_VENTA_NETA_TRANSFERECIA 					4
#define  TAG_VENTA_TOTAL_TRANSFERECIA 					5
#define  TAG_PROPINA_TRANSFERECIA 						6
#define  TAG_IVA_TRANSFERECIA 							7
#define  TAG_BASE_TRANSFERECIA 							8
#define  TAG_INC_TRANSFERECIA 							9

#define  CAPTURA_DATOS_MONTOS_E_IMPUESTOS        		1
#define  CAPTURA_COMPORTAMIENTO_TARJETA          	  	2
#define  INGRESO_PIN_EMV		                 		3
#define  EXTRAER_TAGS_ENVIO_EMV                  		4
#define  ENVIO_RECEPCION_EMV					 		5
#define  FIN_PROCESO_APROBADO_EMV               		6
#define  FIN_PROCESO_NO_APROBADO_EMV                    7
#define  INGRESO_PIN_EMV_OFFLINE		         		8

#define MAX_INTENTOS_VENTA								2
#define MAX_GRUPO										15
#define MAX_COMERCIOS_MULTIPOS							10

#define IMPUESTO_IVA			 						1
#define IMPUESTO_IVA_MULTIPLE	 						2
#define IMPUESTO_INC            	 					3
#define IMPUESTO_INC_MULTIPLE    						4
#define IMPUESTOS_PARAMETRICOS							5

#define IMPUESTO_MANUAL									0
#define IMPUESTO_IVA_AUTO								1
#define IMPUESTO_INC_AUTO								2

#define IS_CMR_EXTENDIDO   			"8"

#define LEN_BLOQUE_MULTIPOS  		32
#define LEN_BLOQUE_ENTIDADES  		24
#define LEN_TIPOS_DOCUMENTOS  		31
#define LEN_ARCHIVO_COMISION  		91
#define LEN_QR_DESCUENTO	  		12

#define ENCRYPT_TYPE_DES						0x80
#define ENCRYPT_TYPE_3DES						0x40

#define T_EXITO						"T.EXITO"
#define T_CMR						"C M R"
#define T_MASTERCRD					"MASTERCRD"
#define T_CMR_MASTER				"CMR MASTER"
#define T_TARJ_TCO					"TARJ. TCO"
#define T_TUYA						"TUYA"
#define T_TUYA_MC					"TUYA MC"
#define T_ALKOSTO					"T.ALKOSTO"
#define T_ALKOSTO_MC				"ALKOSTO MC"


#define FIELD_TEFF_CODIGO_RESPUESTA                 					0
#define FIELD_TEFF_CODIGO_AUTORIZACION              					1
#define FIELD_TEFF_TIPO_DE_TRANSACCION									2
#define FIELD_TEFF_TIPO_ENTRADA											3
#define FIELD_TEFF_ARQC													4
#define FIELD_TEFF_AID													5
#define FIELD_TEFF_CARDNAME												6
#define FIELD_TEFF_FECHA_EXPEDICION										7
#define FIELD_TEFF_VERSION_SW											8
#define FIELD_TEFF_PANEL_FIRMA											9
#define FIELD_TEFF_TEXTO_PAGARE											10
#define FIELD_TEFF_REFERENCIA_ADICIONAL									11
#define FIELD_TEFF_TASA_DE_CAMBIO										13
#define FIELD_TEFF_PESOS_DCC											14
#define FIELD_TEFF_MONEDA_EXTRANJERA									15
#define FIELD_TEFF_NOMBRE_COMERCIO										16
#define FIELD_TEFF_INDICADOR_FIRMA										17
#define FIELD_TEFF_NUMERO_DE_MILLAS										18
#define FIELD_TEFF_MODO_FLUJO                        					20
#define FIELD_TEFF_TIPO_RECAUDO											21
#define FIELD_TEFF_TIPO_DOCUMENTO_BENEFICIARIO							22
#define FIELD_TEFF_DOCUMENTO_BENEFICIARIO								23
#define FIELD_TEFF_TELEFONO_BENEFICIARIO								24
#define FIELD_TEFF_MONTO_GIRO											25
#define FIELD_TEFF_MONTO_COMISION										26
#define FIELD_TEFF_DATA_ADICIONAL										31//campoAra
#define FIELD_TEFF_ASTERISCOS_4_ULTIMOS_TARJETA    	 					30
#define FIELD_TEFF_CODIGO_CAJERO                   					 	33
#define FIELD_TEFF_TIPO_CUENTA                      					34
#define FIELD_TEFF_FRANQUICIA                       					35
#define FIELD_TEFF_MONTO                            					40
#define FIELD_TEFF_IVA                              					41
#define FIELD_TEFF_BASE_DEVOLUCION                  				    42
#define FIELD_TEFF_PROPINA                          					43
#define FIELD_TEFF_INC                              					44
#define FIELD_TEFF_CODIGO_CONVENIO                  				 	45
#define FIELD_TEFF_DESCRIPCION_CONVENIO						 			46
#define FIELD_TEFF_COD_RESPUESTA_BCL_JMR						 		48
#define FIELD_TEFF_REFERENCIA_COMPRA                	 				62
#define FIELD_TEFF_NUMERO_RECIBO_DATAFONO           	 				65
#define FIELD_TEFF_NUMERO_FACTURA_ENVIADO_CAJA    						66
#define FIELD_TEFF_CUOTAS                           					67
#define FIELD_TEFF_TIPO_AUTORIZADOR                 				 	68 //(Fenalcheque, Covinoc)
#define FIELD_TEFF_TIPO_DOCUMENTO                   					69
#define FIELD_TEFF_NUMERO_DOCUMENTO                 					70
#define FIELD_TEFF_NUMERO_TELEFONO                  					71
#define FIELD_TEFF_CODIGO_CORRESPONSAL              					72
#define FIELD_TEFF_CODIGO_BANCO                     					73
#define FIELD_TEFF_NUMERO_CUENTA                    					74
#define FIELD_TEFF_NUMERO_CHEQUE                    					75 //(Solo para Fenalcheque)
#define FIELD_TEFF_TIPO_PAGO	                    					76
#define FIELD_TEFF_RRN                              					77
#define FIELD_TEFF_NUMERO_TERMINAL                  					78
#define FIELD_TEFF_CODIGO_ESTABLECIMIENTO           					79
#define FIELD_TEFF_FECHA                            					80
#define FIELD_TEFF_HORA                             					81
#define FIELD_TEFF_CLAVE_SUPERVISOR                			    		84
#define FIELD_TEFF_VALOR_HABILITACION_BONO          					85 // Unicamente en transacción de habilitación de bonos
#define FIELD_TEFF_VALOR_REDENCION_BONO             					86 //Unicamente en transacción de redención de bonos
#define FIELD_TEFF_NUMERO_BONO                      					87
#define FIELD_TEFF_ESTADO_LEALTAD                   					88
#define FIELD_TEFF_CODIGO_OPERADOR                  					89 //o Empresa de Servicio
#define FIELD_TEFF_CLAVE_USUARIO                    					92
#define FIELD_TEFF_NUMERO_FACTURA                   					93
#define FIELD_TEFF_INFO_ADICIONAL_OPERACION         					94
#define FIELD_TEFF_DESCUENTO_TRANSACCION            					95
#define FIELD_TEFF_CLAVE_USUARIO_RECARGAS           					96 //Clave de usuario asignada por proveedor de recargas
#define FIELD_TEFF_CVM_RECIBO_DINAMICO	            					97//Recibo dinamico
#define FIELD_TEFF_RECIBO_DINAMICO		            					98//Recibo dinamico
#define FIELD_TEFF_VERSION_RECIBO_DINAMICO	           					99

#define LEN_TEFF_CODIGO_RESPUESTA                   				  	2
#define LEN_TEFF_TIPO_DE_TRANSACCION									3
#define LEN_TEFF_TIPO_ENTRADA_TRANSACCION								2
#define LEN_TEFF_ARQC													16
#define LEN_TEFF_AID													16
#define LEN_TEFF_CARDNAME												32
#define LEN_TEFF_VERSION_SW												20
#define LEN_TEFF_FECHA_EXPEDICION										4
#define LEN_TEFF_PANEL_DE_FIRMA											2
#define LENT_TEFF_TEXTO_PAGARE											255
#define LEN_TEFF_CODIGO_AUTORIZACION                			  		6
#define LEN_TEFF_ASTERISCOS_4_ULTIMOS_TARJETA       					12
#define LEN_TEFF_CODIGO_CAJERO                      					10
#define LEN_TEFF_TIPO_CUENTA                        					2
#define LEN_TEFF_FRANQUICIA                         					10
#define LEN_TEFF_MONTO                              					12
#define LEN_TEFF_IVA                                					12
#define LEN_TEFF_BASE_DEVOLUCION                    					12
#define LEN_TEFF_PROPINA                            					12
#define LEN_TEFF_INC                                					12
#define LEN_TEFF_NUMERO_RECIBO_DATAFONO             					6
#define LEN_TEFF_NUMERO_FACTURA_ENVIADO_CAJA       						10
#define LEN_TEFF_NUMERO_FACTURA_MULTIPOS            					10
#define LEN_TEFF_CUOTAS                             					2
#define LEN_TEFF_TIPO_AUTORIZADOR                   					1 //(Fenalcheque, Covinoc)
#define LEN_TEFF_TIPO_DOCUMENTO                     					2
#define LEN_TEFF_NUMERO_DOCUMENTO                   					11
#define LEN_TEFF_NUMERO_TELEFONO                    					7
#define LEN_TEFF_CODIGO_BANCO                       					2
#define LEN_TEFF_NUMERO_CUENTA                      					13
#define LEN_TEFF_NUMERO_CHEQUE                      					6 //(Solo para Fenalcheque)
#define LEN_TEFF_RRN                                					6
#define LEN_TEFF_NUMERO_TERMINAL                    					8
#define LEN_TEFF_CODIGO_ESTABLECIMIENTO             					23
#define LEN_TEFF_FECHA                              					6
#define LEN_TEFF_HORA                               					4
#define LEN_TEFF_CLAVE_SUPERVISOR                   					4
#define LEN_TEFF_VALOR_HABILITACION_BONO            					12 // Unicamente en transacción de habilitación de bonos
#define LEN_TEFF_VALOR_REDENCION_BONO               					12 //Unicamente en transacción de redención de bonos
#define LEN_TEFF_NUMERO_BONO                        					25
#define LEN_TEFF_ESTADO_LEALTAD                     					1
#define LEN_TEFF_CODIGO_OPERADOR                    					4 //o Empresa de Servicio
#define LEN_TEFF_NUMERO_FACTURA                     					30
#define LEN_BUFFER_TEFF                             					1000
#define LEN_TEFF_INFO_ADICIONAL_OPERACION           					118  //30
#define LEN_TEFF_DESCUENTO_TRANSACCION              					12
#define LEN_TEFF_CLAVE_USUARIO_RECARGAS             					6
#define LEN_BUFFER_INTERCAMBIO_EMV                  					512
#define LEN_TEFF_MODO_FLUJO                         					2
#define LEN_TEFF_TIPO_RECAUDO											2
#define LEN_TEFF_BIN_TARJETA											6

#define TRANSACCION_TEFF_COMPRAS                        				0
#define TRANSACCION_TEFF_ANULACIONES                    				2
#define TRANSACCION_TEFF_CHEQUES                        				1
#define TRANSACCION_TEFF_HABILITACIONES                 				3
#define TRANSACCION_TEFF_REDENCION                      				4
#define TRANSACCION_TEFF_RECARGA_BONO                   				6
#define TRANSACCION_TEFF_SINCRONIZACION              					7
#define TRANSACCION_TEFF_REDIMIR_FALABELLA								8
#define TRANSACCION_TEFF_LEALTAD										9
#define TRANSACCION_TEFF_RETIRO_ARA										10
#define TRANSACCION_TEFF_RECARGA_EFECTIVO           					11
#define TRANSACCION_TEFF_ACUMULACION_PUNTOS_COL        					12
#define TRANSACCION_TEFF_TARJETA_PRIVADA       							14
#define TRANSACCION_TEFF_SOLICITUD_RECAUDO_AVAL     					15
#define TRANSACCION_TEFF_REPORTE_RECAUDO_AVAL       					16
#define TRANSACCION_TEFF_SOLICITUD_FACTURAS_RECAUDO_AVAL     			17
#define TRANSACCION_TEFF_REPORTE_FACTURAS_PAGADAS_AVAL   				18
#define TRANSACCION_TEFF_RETIRO_EFECTIVO            					18
#define TRANSACCION_TEFF_RETIRO_SIN_TARJETA         					19
#define TRANSACCION_TEFF_RETIRO_TARJETA_PRINCIPAL 						19
#define TRANSACCION_TEFF_DEPOSITO_BCL 									20 // OJO ES 20 PRUEBAS NETCOM
#define TRANSACCION_TEFF_RECAUDO_MANUAL 								21
#define TRANSACCION_TEFF_RECAUDO_LECTOR_BARRAS 							22
#define TRANSACCION_TEFF_PAGO_TCREDITO 									24
#define TRANSACCION_TEFF_RECAUDO_TEMPRESARIAL 							23
#define TRANSACCION_TEFF_PAGO_CARTERA 									25
#define TRANSACCION_TEFF_TRANSFERENCIA_BCL 								26
#define TRANSACCION_TEFF_COMPRAS_CLESS                        			27
#define TRANSACCION_TEFF_ANULACION	 									27 // OJO ES 20 PRUEBAS NETCOM
#define TRANSACCION_TEFF_COMPRA_REFERENCIA							    31
#define TRANSACCION_TEFF_CONSULTA_CUPO_BCL								32
#define TRANSACCION_TEFF_CONSULTA_SALDO_BCL								33
#define TRASACCION__TEFF_NOTIFICACION_SMS								34
#define TRANSACCION_TEFF_ENVIAR_GIRO									35
#define TRANSACCION_TEFF_RECLAMAR_GIRO									36
#define TRANSACCION_TEFF_CANCELAR_GIRO									37
#define TRANSACCION_TEFF_BONO_VIRTUAL									43
#define TRANSACCION_TEFF_TARJETA_MI_COMPRA          					62
#define TRANSACCION_TEFF_PAGO_TARJETA_PRIVADA 							64
#define TRANSACCION_TEFF_QR_DESCUENTO_81	        					81
#define TRANSACCION_TEFF_QR_DESCUENTO_82	        					82
#define TRANSACCION_TEFF_QR_DESCUENTO_83	        					83
#define TRANSACCION_TEFF_QR_DESCUENTO_84	        					84
#define TRANSACCION_TEFF_QR_DESCUENTO_85	        					85
#define TRANSACCION_TEFF_RETIRO_AVAL		        					98
#define TRANSACCION_TEFF_COMPRA_EXITO		        					99
#define TRANSACCION_TEFF_EFECTIVO			        					99
#define TRANSACCION_TEFF_LEALTAD_LIFEMILES							   100
#define TRANSACCION_TEFF_ACUMULACION_LIFEMILLES	     				   101
#define TRANSACCION_TEFF_ACUMULACION_PUNTOSCOL	     				   102

/*************************transacciones multicorresponsal***********************/
#define TRANSACCION_TEFF_RECAUDO_CORRESPONSAL							15
#define TRANSACCION_TEFF_RTA_CONSULTA_RECAUDO_CORRESPONSAL 				16
#define TRANSACCION_TEFF_SOL_PAGO_RECAUDO_CORRESPONSAL		   			17


#define TRANSACCION_TEFF_RTA_PAGO_RECAUDO_CORRRESPONSAL					18
#define TRANSACCION_TEFF_DEPOSITO_CORRESPONSAL 							20
#define TRANSACCION_TEFF_PAGO_TARJETA_CREDITO_CORRESPONSAL 				24
#define TRANSACCION_TEFF_PAGO_CARTERA_CORRESPONSAL 						25
#define TRANSACCION_TEFF_TRANSFERENCIA_CORRESPONSAL             		26
#define TRANSACCION_TEFF_ABONOS_CORRESPONSAL			           		98
#define TRANSACCION_TEFF_IDENTIFICAR_TRANSACCION_REALIZAR       		99
/*************************transacciones multicorresponsal***********************/


/*************************DEFINICIONES PARA CAJA ESCUCHA*************************/
#define TRANSACCION_TEFF_ESCUCHA_PAGO		 				       		21
#define TRANSACCION_TEFF_ESCUCHA_CONSULTA	 				       		27
#define TRANSACCION_TEFF_ESCUCHA_CONSULTA_CONVENIO			       		28


/*************************DEFINICIONES PARA CAJA ESCUCHA*************************/



/**LA TRANSACCIÓN SELECCIONADA EN LA CAJA PARA MULTICORRESPONSAL USADAS EN
 *  CAJAS Y MULTICORRESPONSAL*/
#define RETIRO_MULTICORRESPONSAL          								1
#define CONSULTA_RECAUDO_MULTICORRESPONSAL   							2
#define PAGO_RECAUDO_MULTICORRESPONSAL									3
#define DEPOSITO_MULTICORRESPONSAL      								4
#define ABONOS_MULTICORRESPONSAL        								5
#define TRANSFERENCIA_MULTICORRESPONSAL									6

#define CONSULTA_MULTICORRESPONSAL 										1
#define CONSULTA_BANCOLOMBIA											2

#define CAJA_MULTICORRESPONSAL                      	 				1
#define SOLICITUD_CAJA                             					 	1
#define RESPUESTA_CAJA                              				 	2

/**TIPO DE TRANSACCIONES OBTENIDAS DE LA CAJA PARA CNB BANCOLOMBIA*/
#define NO_APLICA													0
#define RETIRO_CUENTA_PRINCIPAL										1
#define RETIRO_CUENTA_SECUNDARIA									2
#define RETIRO_SIN_TARJETA											3
#define DEPOSITO_BANCOLOMBIA										4
#define	RECAUDO_MANUAL												5
#define RECAUDO_CODIGO_BARRAS										6
#define	RECAUDO_TARJETA_EMPRESARIAL									7
#define PAGO_TARJETA_CREDITO										8
#define PAGO_CARTERA												9
#define	TRANSFERENCIA_CTA_PRICIPAL									10
#define TRANSFERENCIA_CTA_SECUDARIA									11
#define ANULACION_BANCOLOMBIA										12
#define CONSULTA_RECAUDO_BANCOLOMBIA								13
#define PAGO_RECAUDO_BANCOLOMBIA									14
#define CONSULTA_SALDO_BANCOLOMBIA									15
#define RECARGAS_NEQUI												16
#define RETIRO_CON_TARJETA											17
#define	ENVIO_GIRO													18
#define RECIBIR_GIRO												19
#define CANCELAR_GIRO												20
#define CONSULTA_CUPO_BANCOLOMBIA									21

#define MENSAJE_TEFF_REQUERIMIENTO  								0
#define MENSAJE_TEFF_RESPUESTA  				    				1
#define MENSAJE_TEFF_NO_REQUIERE_RESPUESTA          				2
#define CLAVE_INVALIDAD_TEFF                        				3

#define NO_HAY_CODIGO_DE_RESPUESTA                 					-1

#define TEFF_APROBADA  				                				0
#define TEFF_DECLINADA  				           					1
#define TEFF_ENTIDAD_NO_RESPONDE  	   								4

#define ASTERISCOS                    								0
#define ASTERISCOS_4_ULTIMOS          								1
#define MAX_FIELD_TEFF                             					33

#define ENVIAR_GIROS		          								1
#define RECLAMAR_GIROS                             					2
#define CANCELAR_GIROS                             					3
#define CONSULTA_CANCELAR_GIROS                             		4

#define GIRADOR		          										1
#define BENEFICIARIO                             					2

#define CEDULA_CIUDADANIA	          								1
#define PASAPORTE_GIROS	                           					2
#define NIT			                             					3

#define SIZE_CODIGO_BANCO_CNB										4
#define SIZE_MAX_NUM_TARJETA 										19
#define SIZE_MIN_NUM_TARJETA_SEMBRADA								13


/////////////////////////////////// ARCHIVOS ///////////////////////////////////
#define DISCO_NETCOM                        					"/N"
#define ARCHIVO_REVERSO											"/REVERSO.TXT"
#define JOURNAL													"/JOURNAL.TXT"
#define JOURNAL_CNB_RBM											"/JCNBRBM.TXT"
#define JOURNAL_CNB_BANCOLOMBIA									"/JCNBBCL.TXT"
#define JOURNAL_CNB_CITIBANK									"/JCNBCTBK.TXT"
#define JOURNAL_CNB_CITIBANKP									"/JCNBCTBP.TXT"
#define JOURNAL_CNB_CORRESPONSALES								"/JCORRES.TXT"
#define DIRECTORIO_JOURNALS										"/JOURDIR.TXT"
#define JOURNAL_MULT_BCL										"/JMULTBCL.TXT"
#define JOURNAL_MCNB_AVAL										"/JCNBAVAL.TXT"
#define JOURNAL_FOCO											"/JFOCO.TXT"
#define ARCHIVO_DCC_LOGGIN										"/DCLOGGIN.TXT"
#define ARCHIVO_DCC_LOGGIN_REVERSO								"/REVERDCC.TXT"
#define JOURNAL_DCC												"/J_DCC.TXT"
#define TABLA_INICIALIZACION_QRCODE	        					"/QR.TXT"
#define DATOS_QRCODE	       				      				"/QRCODE.TXT"
#define RUTA_TABLA_MULTICORRESPONSAL        					"/MULTICO.TXT"
#define REVERSO_MULTICORRESPONSAL                               "/REVERAUX.TXT"
#define TARJ_SEMB_BANCOLOMBIA              						"/T_BANCOL.TXT"
#define TARJ_SEMB_AVAL                      					"/T_C_AVAL.TXT"
#define TARJ_SEMB_CITIBANK                  					"/T_CITIBK.TXT"
#define TARJ_SEMB_RBM                       					"/T_C_RBM.TXT"
#define TARJ_SEMB_CORRESPONSALES       				            "/T_C_CORR.TXT"
#define CAJA_RECAUDOS_CONSULTA									"/RE_CONSU.TXT"
#define CAJA_RECAUDOS_PAGO_SOL									"/RE_P_SOL.TXT"
#define CAJA_RECAUDOS_PAGO_RTA									"/RE_P_RTA.TXT"
#define JOURNAL_RECAUDOS_MULT									"/J_RECAUD.TXT"
#define REVERSOS_RECAUDOS_MULT									"/REVERREC.TXT"
#define TX_BIG_BANCOLOMBIA										"/Big_Ban.txt"
#define FILE_CELUCOMPRA  										"/CELUCOMP.TXT"
#define FILE_TCELUCOMPRA 									    "/TCELUCOM.TXT"
#define TOKEN_QC        									    "/TOKENQC.TXT"
#define CODIGO_UNICO_QR											"/UNICO-QR.TXT"
#define CONSULTA_QR	                                            "/CONSU-QR.TXT"
#define DATOS_QRCODE	       				                    "/QRCODE.TXT"
#define DATOS_TEQRCODE	       				                    "/TEQRCODE.TXT"
#define DATOS_TX_BANDA										    "/DTBANDA.TXT"
#define TX_BONOS											    "/BONOS.TXT"
#define RESPALDO_CAJA                                           "/RESPCAJA.TXT"
#define ARCHIVO_SINCRONIZACION_CAJA                             "/SINCROCA.TXT"
#define TARJETA_CNB                                             "/TARJ_CNB.TXT"
#define REVERSO_CNB_AVAL                                        "/RCNBAVAL.TXT"
#define JOURNAL_CNB_AVAL_PLUS	                                "/J-AVAL.TXT"
#define ARCHIVO_PAGO_CARTERA                                    "/CARTERA.TXT"
#define TEMPORAL                                                "/TEMP.TXT"
#define CONSULTA_ESTADO                                         "/COESTADO.TXT"
#define TABLA_PARAMETROS										"/TABPAR.TXT"
#define TABLA_DCC                                               "/TABLDCC.TXT"
#define PPRO18_DCC                                              "/DCC-P18.TXT"
#define RELACION_DIVIDIDAS                                      "/RELACDIV.TXT"
#define KOFFLINE                                                "/KOFFLINE.TXT"
#define RESPALDO                                                "/RESPALDO.TXT"
#define ARCHIVO_RECARGA_TARJETA                                 "/RTARJE.TXT"
#define ARCHIVO_RECARGA_TARJETA_2                               "/JTARJE.TXT"
#define ARCHINO_MES_ANO                                         "/MESFINAL.TXT"

#define TABLA_1_INICIALIZACION                                  "/T-001.TXT"
#define TABLA_2_INICIALIZACION                                  "/T-002.TXT"
#define TABLA_3_INICIALIZACION                                  "/T-003.TXT"
#define TABLA_4_INICIALIZACION                                  "/T-004.TXT"
#define TABLA_5_INICIALIZACION                                  "/T-005.TXT"
#define TABLA_6_INICIALIZACION                                  "/T-006.TXT"
#define TABLA_7_INICIALIZACION                                  "/T-007.TXT"
#define TABLA_8_INICIALIZACION                                  "/T-008.TXT"
#define TABLA_12_INICIALIZACION                                 "/T-012.TXT"
#define TABLA_13_INICIALIZACION                                 "/T-013.TXT"
#define TABLA_14_INICIALIZACION                                 "/T-014.TXT"
#define TABLA_03_D                                              "/T-%03d.TXT"
#define TABLA_CI                                                "/TAB-CI.TXT"
#define TABLA_CNB                                               "/TA-CNB.TXT"
#define FILE_FIID                                               "/ADFIID.TXT"
#define FILE_TFIID                                              "/TEFIID.TXT"
#define TABLA_EXCEPCIONES                                       "/EXCEPC.TXT"
#define CAKEYS_TXT                                              "/CAKEYS.TXT"
#define INICIALIZACION_MULTIPOS                                 "/MULTIP.TXT"
#define WORKING_MULTIPOS                                        "/WORKIN.TXT"
#define INICIALIZACION_PUNTOS                                   "/PUNTOS.TXT"
#define INICIALIZACION_EMISORES                                 "/EMISOR.TXT"
#define INICIALIZACION_BI_EMISORES                              "/BIEMIS.TXT"
#define INICIALIZACION_TABLA_QRCODE                             "/T-QR.TXT"
#define EMISORES_ADICIONALES                                    "/EMISOR.TXT"
#define BI_EMISORES_ADICIONALES                                 "/BIEMIS.TXT"
#define ISSUER_CNB                                              "/ISUCNB.TXT"
#define TRANSFERENCIA_DATOS                                     "/TRANFER.TXT"
#define ARCHIVO_CONVENIO_DESCUENTOS                             "/CONVDES.TXT"
#define TEMPORAL_DESCUENTOS                                     "/TEMPDES.TXT"
#define ARCHIVO_URL_FIRMA                                       "/URLF.TXT"
#define TARJ_SEMB_BBVA              				            "/T_BBVA.TXT"
#define REVERSOS_RECAUDOS_NUEVO_SERVIDOR		                "/REVERNS.TXT"
#define PIN_PINBLOCK                                            "/Pb.TXT"
#define SODEXO                                                  "/SDXO.TXT"
#define QI_FOCO                                                 "/QIFC.TXT"
#define REVERSOAUXILIARRECAUDO									"/RAUX.txt"
//////////////////////////// ARCHIVOS RESPALDO //////////////////////////////////
#define TABLA_PARAMETROS_R                                      "/TABLAPA.RES"
#define CAKEYS_R                                                "/CAKEYS.RES"
#define TABLA_EXCEPCIONES_R                                     "/EXCEPC.RES"
#define INICIALIZACION_MULTIPOS_R                               "/MULTIP.RES"
#define WORKING_MULTIPOS_R                                      "/WORKIN.RES"
#define TX_BONOS_R											    "/BONOS.RES"
#define INICIALIZACION_PUNTOS_R                                 "/PUNTOS.RES"
#define INICIALIZACION_EMISORES_R                               "/EMISOR.RES"
#define INICIALIZACION_BI_EMISORES_R                            "/BIEMIS.RES"
#define TABLA_CI_R                                              "/TAB-CI.RES"
#define TABLA_CNB_R                                             "/TA-CNB.RES"
#define TABLA_DCC_R                                             "/TABLDCC.RES"
#define RUTA_TABLA_MULTICORRESPONSAL_R        					"/MULTICO.RES"
#define TABLA_03_D_R                                            "/T-%03d.TXTR"
#define TABLA_1_INICIALIZACION_R                                "/T-001.TXTR"
#define CELULAR_SMS				    							"/SMS.TXT"
#define SINCRONIZACION_SERVICIO_IP								"/SCCJ.TXT"
#define RECIBO_DIGITAL											"/TCKT.TXT"
#define METODO_SOLICITUD_PUNTOS_COLOMBIA						"/PCSOL.txt"
#define METODO_CAJA_RESPUESTA									"/FCJR.txt"
#define METODO_ENVIO_FIRMA										"/FFIRM.txt"
#define METODO_SOLICITUD_CAJA									"/CJSOL.txt"
#define METODO_SOLICITUD_SERVICIO_WEB							"/WSOL.txt"
#define ANULACION_PENDIENTE										"/DTANUL.txt"
////////////////////////////// ARCHIVOS HOST ///////////////////////////////////
#define ARCHIVO_BINLOCAL                                        "/BLOC.PAR"
#define ARCHIVO_HABILITAR                                       "/HABIL.PAR"
#define ARCHIVO_COSTOS                                          "/COS.PAR"
#define ARCHIVO_CONSEC                                          "/KSEC.PAR"
#define ARCHIVO_DOWNLOAD                                        "/DLOAD.PAR"
#define BINES_DCC_BANDA                                         "/BINESDCC.PAR"
#define CAKEYS                                                  "/CAKEYS.PAR"
#define REVERSO_PUNTOS_COLOMBIA									"/REVPC.txt"
#define RELACION_PUNTOS_COLOMBIA								"/HOST/RPC.txt"
#define MONTO_PENDIENTE_PUNTOS_COLOMBIA							"/HOST/NMPC.txt"
#define REVERSO_FIRMA_IP										"/REVF.txt"
#define LECTURA_CATALOGOS_REFERENCIADA							"/LEC.txt"
#define TABLA_INICIALIZACION_QRCODE_EMVCO     					"/QrEmvc.txt"
#define REVERSO_AUXILIAR_PUNTOS_COLOMBIA    					"/REVAUXPC.txt"

#define TRANS_COMERCIO								3//transaccion del comercio
#define TRANS_MULT_BANCOLOMBIA						1//transaccion del multicorresponsal bancolombia
#define TRANS_MULT_AVAL								2//transaccion del multicorresponsal aval
#define TRANS_CNB_BANCOLOMBIA						4//transaccion del cnb bancolombia
#define TRANS_CNB_AVAL								5//transaccion del cnb aval
#define TRANS_CNB_CORRESPONSALES					6//transaccion del cnb corresponsales
#define TAM_JOURNAL_COMERCIO						sizeof(DatosVenta) //tamaño de la estructura DatosVenta
#define TAM_JOURNAL_BANCOLOMBIA        				sizeof(DatosCnbBancolombia) //tamaño de la estructura propia de bancolombia
#define TAM_JOURNAL_CORRESPONSALES	        		sizeof(DatosCORRESPONSALES) //tamaño de la estructura propia de corresponsales
#define TAM_DIR_JOURNALS							sizeof(DatosJournals)
#define TAM_CAJA_RECAUDO_MULT						sizeof(RecaudoMulticorresponsal)

/****DEFINICIONES MULTICORRESPONSAL***/
#define TAM_DATOS_MULTICORRESPONSAL         			46
#define TAM_FIID					        			4
#define TAM_CODIGO_UNICO			        			19
#define TAM_PAN_VIRTUAL				        			19
#define TAM_TIPO_CUENTA_MULTICORRESPONSAL				2 //modificado de 1

#define DIRECCIONAMIENTO_QR		    				"/CFGRBM.TXT"
#define DIRECCIONAMIENTO_QR_INTERNO 				"/CFGRBM.TXT"
#define RECIBO_DIGITAL								"/TCKT.TXT"

////// EMISORES ADICIONALES //////
#define LENGTH_BINES_EMISORES  14
#define LENGTH_BLOQUE_EMISORES 20
////// EMISORES ADICIONALES //////

#define  IS_QRCODE_EMVCO			0
#define  IS_QRCODE_RBM				1
////// DEFINICIONES CLUB VIVAMOS //////    455
#define CONSULTA_DESCUENTO          			1
#define COMPRA_VIVAMOS 		        			2
#define PAGO_EFECTIVO_DESCUENTOS    			3
#define LENGTH_CONVENIO_VIVAMOS    				24
#define EXISTE_CONVENIO             			1
#define NO_EXISTE_CONVENIO          			2
#define TARJETA_DESCUENTOS          			1
#define EFECTIVO_OTROS_DESCUENTOS   			2
#define CAPTURAR_DESCUENTO		  			 	'1'
#define VALIDAR_VENTA_VIVAMOS	   			 	'2'
#define IMPRESION_VENTA_VIVAMOS	   		 		'3'

////// DEFINICIONES VERSIONAMIENTO //////
#define SERVICE_VERSIONAMIENTO   				112
#define REPORTE_VERSIONAMIENTO   				1
#define SET_VERSIONAMIENTO   					2

////// DEFINICIONES DINERO ELECTRONICO //////
////// DEFINICIONES NUEVOS AID EMV //////
#define TRANSACCION_CHIP_POR_DEFECTO  			6

///// DEFINICIONES DCC ///////
#define IS_TARJETA_DCC        					20
#define IS_TRANSACCION_DCC    					50
#define NO_CODIGO			 					"NCOD" // se asigna a las banda dado que las chip si tienen el código.

//// DEFINICION DE SERVICIOS /////
#define LOGEO_USERS              				110
#define CAMBIAR_CLAVE_USER		 				111
#define MENU_CNB_RBM			 				112
#define SERVICIO_MENU_ENTRADA	 				113
#define SET_USER_CNB			 				114
#define SERVICIO_CORE_EMV        				118
#define VERIFICAR_PUNTOS_EMISOR  				119
#define SERVICIO_PRINCIPAL		 				120
#define SERVICIO_IMPRIMIR_DUPLICADO 			121
#define SERVICIO_CELUCOMPRA      				122
#define RECIBO_CELUCOMPRA	     				124
#define ANULACION_CELUCOMPRA	 				125
#define SERVICIO_PAGO_TARJETA	 				126
#define SERVICIO_PAGO_TARJETA_EMV	 			127
#define SERVICIO_REVERSOS        				128
#define SERVICIO_IMPRIMIR_REPORTE       		129
#define SERVICIO_MENU_USER						130
#define SERVICIO_INTERCAMBIO_DATOS  			132
#define SERVICIO_CIERRE                 		134
#define SERVICIO_LECTURA_DIRECTA        		135

#define SERVICIO_TARJETA_PRIVADA 				140
#define RECIBO_TARJETA_PRIVADA					141
#define CIERRE_CNB_RBM							142
#define CIERRE_CNB_BANCOLOMBIA					143

#define MENU_MULTICORRESPONSAL        			144
#define INICIAR_PROCESO_MULTICORRESPONSAL 		145
#define CIERRE_MULTICORRESPONSAL				146
#define DUPLICADO_MULTICORRESPONSAL     		147
#define REPORTE_MULTICORRESPONSAL				148
#define DUPLICADO_RECIBO_BANCOLOMBIA			149
#define ANULACION_RECIBO_BANCOLOMBIA			150
#define IMPRE_RECAUDOS_MULTICORRESPONSAL  		151
#define REVERSOS_RECAUDOS_MULTICORRESPONSAL 	152
#define INICIAR_LEALTAD							153
#define DUPLICADO_LEALTAD						154
#define LEALTAD_CAJA							155
#define PROCESO_QR 			     				156
#define RECIBO_DUPLICADO_QRCODE    				157
#define CIERRE_QR 					    		158
#define PROCESO_CONSULTA_ESTADO		    		159
#define RECIBO_DUPLI_CONSULTA_ESTADO    		160
#define ARMAR_SOLICITUD_QR              		161
#define REALIZAR_SOLICITUD_QR              		162
#define CONSULTA_AUTO_QR 						163
#define ANULACION_QR     						164
#define PITAR_IMAGEN_QR 						165
#define SCREEN_MESSAGE_OPTIMIZADO				166
#define GETCH_ACCEPT_CANCEL_MULTILINEA			167
#define INICIAR_PAPELERIA           			168
#define VENTA_MI_COMPRA              			169
#define CIERRE_MI_COMPRA              			170
#define INICIAR_FALABELLA             			171
#define FALBACK_FALABELLA                   	172
#define PROCESO_BANCOLOMBIA_CAJAS				173
#define OPCIONES_CIERRE_BANCOLOMBIA		    	174
#define RETIRO_AVAL_FLUJO_UNIFICADO         	175
#define INICIAR_BIG_BANCOLOMBIA					176
#define IMPRIMIR_BIG_BANCOLOMBIA				177
#define REVERSOS_NUEVO_SERVIDOR              	178
#define IMPRE_RECAUDOS_NUEVO_SERVIDOR			179
#define INICIAR_LIFEMILES                       180
#define LEALTAD_CAJA_LIFEMILES                  181
#define ANULACION_PUNTOS_COLOMBIA               182
#define MODO_ESCUCHA_MULTICORRESPOSAL			183
#define INVOCAR_SERVICIO_FIRMA					184

//////////////// DEFINICIONES CNB CORRESPONSAL /////////////
#define SERVICIO_PRINCIPAL_CORRESPONSAL			   182
#define SERVICIO_LECTURA_DIRECTA_CORRESPONSALES	   183
#define CIERRE_CNB_CORRESPONSAL				 	   184
#define DUPLICADO_RECIBO_CORRESPONSAL			   185
#define ANULACION_RECIBO_CORRESPONSAL			   186
#define PROCESO_CORRESPONSAL_CAJAS				   187
#define OPCIONES_CIERRE_CORRESPONSAL			   188
#define REVERSOS_NUEVO_SERVIDOR_CORRESPONSAL	   189
#define IMPRE_RECAUDOS_NUEVO_SERVIDOR_CORRESPONSAL 190
//////////////// DEFINICIONES CNB CORRESPONSAL /////////////

/** DEFINCIONES CONFIGURACIÓN CAJAS*/

#define	CAJAS_SERIAL						1
#define	CAJAS_IP							2
#define	CAJAS_IP_CB							3
#define CAJAS_USB							4
#define CAJAS_LAN							5
#define	CAJAS_IP_GPRS						1
#define PARAMETRO_URL						1
#define PARAMETRO_ID						2
#define NOMBRE_DOMINIO						1
#define DIRECCION_IP						2

#define NO_DEFINIDO							"0"
#define CAJA_SERIAL							"1"
#define CAJA_DOMINIO						"2"
#define CAJA_IP								"3"
#define CAJA_USB							"4"
#define CAJA_LAN							"5"

#define HTTP_EXITOSO						"200 OK"

#define ERROR_NO_RESPONDE					-20
#define ERROR_RESPONDE_INCORRECTO			-21
#define	ERROR_CONFIGURACION					-22
#define ERROR_REVERSO						-23
#define ERROR_CLAVE_INVALIDA				-24
#define ERROR_NO_FIRMO						-25
#define RETIRO_CHIP							-26
#define ERROR_CON_CAJA						-27
#define ERROR_REVERSO_PENDIENTE				-28
#define SINCRONIZANDO_PENDIENTE				-29

#define EFTSEC								 "1"
#define TLS									 "2"
#define PRODUCTO_FIRMA_IP					  1
#define PRODUCTO_PCOLOMBIA					  2
#define PRODUCTO_CAJA_IP					  3
#define PRODUCTO_TEST_WEB					  4
#define PRODUCTO_TEST_WEB_EFTSEC			  5

#define NO_IMPRIMIR_CAJERO					"N_IMPRIMIR"// no imprimir para ip
/** DEFINCIONES CONFIGURACIÓN CAJAS*/

/**DEFINICIONES CAJAS IP*/
#define METODO_GET_CAJA						"GET /SIPServiceRest/api/SolicitaDatos?Terminal="
#define METODO_PUT_CAJA						"PUT /SIPServiceRest/api/Respuesta?Terminal="
//#define METODO_PUT_FIRMA					"PUT /SIPServiceRest/api/Grabarrecibo?Terminal="
#define	METODO_GET_KIOSKO					"GET /services/CajaIPNetcom/consultarCaja?TERMINAL="
#define METODO_RESPONSE_KIOSKO				"GET /services/CajaIPNetcom/ResponderCaja?TERMINAL="
#define METODO_GET_CAJA_ISC250				"GET /SIPRESTV6/api/SolicitaDatos?Terminal="
#define METODO_PUT_CAJA_ISC250				"PUT /SIPRESTV6/api/Respuesta?Terminal="
#define METODO_PUT_FIRMA					"PUT /SIPRESTV6/api/Grabarrecibo?Terminal="
/**DEFINICIONES CAJAS IP*/

/**DEFINICIONES PARA RECAUDO BANCOLOMBIA / MULTICORRESPONSAL*/
#define MANUAL_CODIGO_CONVENIO				1
#define MANUAL_CUENTA_RECAUDADORA 			2

#define MENSAJE_CONSULTA					1
#define MENSAJE_PAGO						2

#define NO_BASE_DATOS						0
#define BASE_DATOS							1

#define PAGO_VALOR_EXACTO					0
#define PAGO_CERO							1
#define PAGO_MAYOR							2
#define	PAGO_MAYOR_O_CERO					3
#define PAGO_MENOR_NO_CERO					4
#define PAGO_MENOR_O_CERO					5
#define PAGO_CUALQUIER_NO_CERO				6
#define PAGO_CUALQUIER_O_CERO				7

#define INDICADOR_NO_APLICA 				"0"		// mismo no hay factura
#define INDICADOR_LECTOR					"1"
#define INDICADOR_MANUAL					"2"
#define INDICADOR_TEMPRESARIAL				"3"

#define TAM_P6_IND_1			 			1
#define TAM_P6_IND_2						1
#define TAM_P6_NUM_RECIBO					6
#define TAM_P6_NUM_REFERENCIA				24
#define TAM_P6_COD_CONVENIO					5
#define TAM_P6_COD_CONVENIO_CORRESPONSALES	7
#define TAM_P6_NOMBRE_CONVENIO				30
#define TAM_P6_IND_BASE_DATOS				1
#define TAM_P6_IND_VALOR					2
#define TAM_P6_VALOR_FACTURA				12
#define TAM_P6_ESPACIO_44					44 //REEMPLAZA EL TAMAÑO DE 30 + 1 + 1 + 12 //PAGO
#define TAM_P6_ESPACIO_45					45 // SE USA PARA LA CONSULTA EL PAGO TIENE UN TAM DE 44
#define TAM_P6_TC_RECAUDADORA				2
#define TAM_P6_CTA_RECAUDADORA				16
#define TAM_P6_NUM_TRANZABILIDAD			12
#define TAM_P6_NUM_FACTURA					48	// ESTAN A REDUCIR 1 A 49 PORQUE SE AUMENTO 1 A TAM_P6_IND_VALOR
#define TAM_P6_NUM_PADRE					5  // ESTAN ASOCIADOS A 5 + 43 == 48
#define TAM_P6_NUM_FACTURA_INDEP			43 // ESTAN ASOCIADOS A 5 + 43 == 48
#define TAM_P6_ESPACIO_61					60//REEMPLAZA EL TAMAÑO DE 12 + 49
#define COMPARACION_CONSULTA				1
#define COMPARACION_PAGO				    2

#define OPERACION_RETIRO_PRINCIPAL		"95"
#define OPERACION_RETIRO_SECUNDARIA		"94"
#define OPERACION_ANULACION_BANCOL		"AB"//no es del campo 69

/**DEFINICIONES PARA RECAUDO BANCOLOMBIA / MULTICORRESPONSAL*/

/** DEFINICIONES DCC */
#define TAM_61_6				16
#define TAM_61_9				10
#define TAM_61_10			 	99
#define TAM_61_13			    1
#define TAM_61_14			    1
#define TAM_61_19			    6
#define TAM_61_20			  	35
#define TAM_61_21			  	16
#define LOGIN_INTENTOS    		4
#define TAM_J_DCC				195

#define TAM_ISSUER_COUNTRY_CODE 	4

#define ONLINE_AUTHORIZED_TRANSACTION	"0"
#define OFFLINE_AUTHORIZED_TRANSACTION	"1"
#define REVERSAL						"2"
#define VOID_TRANSACTION				"3"
#define REFUND_TRANSACTION				"4"
#define SALE_ADJUSTMENT_TRANSACTION		"5"
#define TIP_TRANSACTION					"6"
/** DEFINICIONES DCC */

/** DEFINICIONES AVAL_PLUS*/
#define FIID_BANCOLOMBIA    				"0007"
#define FIID_CORRESPONSALES    				"0013"
#define FIID_BANCO_AVVILLAS    				"0052"
#define FIID_BANCO_OCCIDENTE    			"0023"
#define FIID_BANCO_POPULAR    				"0002"
#define FIID_BANCO_BOGOTA    				"0001"
#define TOKEN_X1_AVAL						"0903"
#define TAM_TOKEN_X1_AVAL					4

/**DEFINICIONES BANCOLOMBIA JMR*/
#define BCL_JMR 							"2"// modo para comercion ARA Jeronimo Martins
#define CUENTA_PRINCIPAL					"1"
#define CUENTA_SECUNDARIA					"2"
#define RESPUESTA_DECLINADA					"12"
#define RESPUESTA_ERROR_CONEXION			"99"
#define TAM_RESPUESTA_JMR					2
#define CAJA_JMR							"JM"
#define	RECAUDO_CAJA_NUEVO_SERVIDOR			"RN"

/**DEFINICIONES INICIO TRANSACCION */
#define INICIA_GASOPASS						"GS"
#define INICIA_FLUJO_UNIFICADO				"FU"

/**DEFINICIONES BIG - BANCOLOMBIA*/
#define C_CIUDADANIA			1
#define C_EXTRANJERIA			2
#define T_IDENTIDAD				3
#define NIT_BIG					4
#define PASAPORTE				5

/**DEFINICIONES PUNTOS COLOMBIA*/
#define CONSULTA_PC			27
#define REDENCION_PC		28
#define ACUMULACION_PC		29
#define ANULACION_PC		30
#define REVERSO_PC			40
#define PAGO_TOTAL			 1
#define PAGO_PARCIAL		 2
#define OTRO_MEDIO			 3
#define TARJETA_PC			 1
#define EFECTIVO			 2
#define PAGO_EN_EFECTIVO	"P"

#define LEALTAD_VENTA						1
#define LEALTAD_RIVIERA						2

/**DEFINICIONES RECIBO DIGITAL**/

#define CON_SALTO				1
#define SIN_SALTO				0

#define ULTIMA_LETRA			0
#define LETRA_PEQUENA			1
#define LETRA_MEDIANA			2
#define LETRA_GRANDE			3

#define ULTIMO_FORMATO			0
#define FORMATO_ESTANDAR		1
#define FORMATO_NEGRITA			2
#define FORMATO_FONDO_NEGRO		3
/*****************************/

#define REVERSO_ENVIADO 	  1
#define REVERSO_ELIMINADO     2
#endif //POS_PIN_ANDROID_DEFINICIONES_H
