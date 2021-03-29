//
// Created by NETCOM on 04/12/2018.
//
#include "configuracion.h"
#include "Tipo_datos.h"
#include "Utilidades.h"
#include "Archivos.h"
#include "Inicializacion.h"
#include <jni.h>
#include <string>
#include <sys/stat.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include "venta.h"
#include "android/log.h"
#include "comunicaciones.h"
#include "native-lib.h"
#define PARAMETRO_FINAL    999
#define MAX_PARAMETROS     999
#define TAM_KEY            3
#define TAM_LENGTH        2
#define TAM_FIN_REGISTRO    2

#define  LOG_TAG    "NETCOM_CONFIGURACION"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
typedef char uChar;

char pathNetcom[60 + 1];
const Parametros tablaParametros[] =
        {
                {NII,                       SIZE_NII,                    (char *) "0000"}, //
                {NUMERO_TERMINAL,           SIZE_NUMERO_TERMINAL,        (char *) ""},
                {TIPO_COMUNICACION,         SIZE_TIPO_COMUNICACION,      (char *) "0"},//1 2 3 4
                {TIPO_CANAL,                SIZE_TIPO_CANAL,             (char *) "1"},//1 2 PLANA / SSL
                {IP_DEFAULT,                SIZE_IP_DEFAULT,             (char *) ""},
                {PUERTO_DEFAULT,            SIZE_PUERTO_DEFAULT,         (char *) "0"},
                {IP_SALIDA,                 SIZE_IP_DEFAULT,             (char *) ""},
                {PUERTO_SALIDA,             SIZE_PUERTO_DEFAULT,         (char *) "0"},
                {GPRS_APN,                  SIZE_GPRS_APN,               (char *) "REDEBAN.COMCEL.COM.CO"},
                {GPRS_USER,                 SIZE_GPRS_USER,              (char *) ""},
                {GPRS_PASS,                 SIZE_GPRS_PASS,              (char *) ""},
                {TELEFONO_INICIAL,          SIZE_TELEFONO_INICIAL,       (char *) ""},
                {CONSECUTIVO,                                    6,      (char *) "000001"},
                {TERMINAL_INICIALIZADA,                          1,      (char *) "0"},
                {TIPO_TRANSACCION,                               2,      (char *) "02"},
                {IVA_AUTOMATICO,                                 2,      (char *) "00"},
                {VALOR_CLAVE_ADMINISTRADOR, SIZE_CLAVE_ADMINISTRADOR,    (char *) "028510"},
                {VALOR_CLAVE_SUPERVISOR,    SIZE_CLAVE_SUPERVIDOR,       (char *) "0000"},
                {VALOR_CLAVE_BONOS,                              4,      (char *) "0000"},
                {NUMERO_PLACA,                                   10,     (char *) "0000000000"},
                {DIVIDIDAS_ACTIVO,                               1,      (char *) "1"},
                {IVA_MULTIPLE,                                   1,      (char *) "0"},
                {IMPRIMIR_QPS,                                   1,      (char *) "0"},
                {NII_MULTIPOS,                                   4,      (char *) "0000"},
                {VALOR_TRANSACCION_LIMITE,                       6,      (char *) "000000"},
                {VALOR_CVM_LIMITE,                               6,      (char *) "000000"},
                {VALOR_FLOOR_LIMITE,                             6,      (char *) "000000"},
                {TEXTO_COMERCIO,                                 30,     (char *) "NO_IMPRIMIR"},
                {ESTADO_TECLADO,                                 1,      (char *) "1"},
                {INVOICE,                                        6,      (char *) "000001"},
                {VERSION_SW,                                     15,     (char *) ""},
                {REVISION_SW,                                    2,      (char *) ""},
                {BATCH,                                          6,      (char *) "000001"},
                {IP_VENTA_PRIMARIA,         SIZE_IP_DEFAULT,             (char *) ""},
                {PUERTO_VENTA_PRIMARIA,     SIZE_PUERTO_DEFAULT,         (char *) "0"},
                {TIMEOUT_VENTA_PRIMARIA,                         2,      (char *) "30"},
                {INTENTOS_VENTA_PRIMARIA,                        2,      (char *) "01"},
                {IP_VENTA_SECUNDARIA,       SIZE_IP_DEFAULT,             (char *) ""},
                {PUERTO_VENTA_SECUNDARIA,   SIZE_PUERTO_DEFAULT,         (char *) "0"},
                {TIMEOUT_VENTA_SECUNDARIA,                       2,      (char *) "30"},
                {INTENTOS_VENTA_SECUNDARIA,                      2,      (char *) "01"},
                {IP_CIERRE_PRIMARIA,        SIZE_IP_DEFAULT,             (char *) ""},
                {PUERTO_CIERRE_PRIMARIA,    SIZE_PUERTO_DEFAULT,         (char *) "0"},
                {TIMEOUT_CIERRE_PRIMARIA,                        2,      (char *) "30"},
                {INTENTOS_CIERRE_PRIMARIA,                       2,      (char *) "01"},
                {IP_CIERRE_SECUNDARIA,      SIZE_IP_DEFAULT,             (char *) ""},
                {PUERTO_CIERRE_SECUNDARIA,  SIZE_PUERTO_DEFAULT,         (char *) "0"},
                {TIMEOUT_CIERRE_SECUNDARIA,                      2,      (char *) "30"},
                {INTENTOS_CIERRE_SECUNDARIA,                     2,      (char *) "01"},
                {MAXIMO_MONTO,                                   9,      (char *) "999999999"},
                {MAXIMO_IVA,                                     6,      (char *) "999999"},
                {MAXIMO_PROPINA,                                 6,      (char *) "999999"},
                {MAXIMO_BASE,                                    8,      (char *) "99999999"},
                {TIME_TELECARGA_AUTOMATICA,                      10,     (char *) "5012312359"},//YYMMDD24HHmm
                {TIME_INICIALIZACION_AUTOMATICA,                 10,     (char *) "5012312359"},//YYMMDD24HHmm
                {HABILITAR_CALL_WAIT,                            2,      (char *) "0"},
                {CODIGO_CALL_WAIT,                               5,      (char *) ""},
                {VELOCIDAD_DIAL,                                 7,      (char *) "115200"},
                {HABILITAR_TONO_MARCACION,                       2,      (char *) "1"},
                {NUMERO_PBX,                                     5,      (char *) ""},
                {TIPO_MARCACION,                                 7,      (char *) "TONOS"},
                {TIME_ULTIMA_INICIALIZACION_EXITOSA,             12,     (char *) "0000000000"},
                {TIME_ULTIMA_TELECARGA_EXITOSA,                  12,     (char *) "00000000000"},
                {TIPO_INICIALIZACION,                            2,      (char *) "M"},
                {CONSECUTIVO_OFFLINE,                            3,      (char *) "001"},
                {PPP_ACTIVO,                                     1,      (char *) "1"},
                {INC_AUTOMATICO,                                 2,      (char *) "00"},
                {INC_MULTIPLE,                                   1,      (char *) "0"},
                {MAXIMO_INC,                                     6,      (char *) "999999"},
                {IMPRIMIR_CIERRE,                                1,      (char *) "1"},
                {CALCULAR_BASE,                                  1,      (char *) "0"},
                {MINIMO_MONTO,                                   9,      (char *) "000000001"},
                {CODIGO_COMERCIO,                                23,     (char *) ""},
                {NOMBRE_COMERCIO,                                23,     (char *) ""},
                {DIRECCION_COMERCIO,                             23,     (char *) ""},
                {NOMBRE_TRANSACCION,                             20,     (char *) "N/A"},
                {CREDITO_ROTATIVO,                               1,      (char *) "0"},  //en su lugar se usa TIPO_TRANSACCION_AUX
                {ID_BONOS,                  SIZE_ID_BONOS,               (char *) "9"},
                {REVERSOS_ENVIADOS,                              2,      (char *) "0"},
                {LECTOR_BARRAS_ACTIVO,                           2,      (char *) "0"},
                {MODO_DATAFONO,                                  2,      (char *) "0"},
                {OTROS_PAGOS,                                    2,      (char *) "0"},
                {LLAVE_EFTSEC,                                   16,     (char *) "0"},
                {DIGITO_CHEQUEO_ETSEC,                           2,      (char *) "0"},
                {KIN_EFTSEC,                                     4,      (char *) "0001"},
                {ACTIVAR_RECARGA_TARJETA,                        1,      (char *) "1"},
                {ACTIVAR_RECARGA_PUNTO_RED,                      1,      (char *) "1"},
                {ACTIVAR_RECARGA_MOVIL_RED,                      1,      (char *) "1"},
                {WORKING_KEY_LOGON_3DES,                         17,     (char *) "0"},
                {TIPO_CAMBIO_CLAVE,                              2,      (char *) "0"},
                {TITULO_CAMBIO_CLAVE,                            2,      (char *) "0"},
                {ASIGNACION_CLAVE_ACTIVO,                        2,      (char *) "0"},
                {TARIFA_ADMIN,                                   2,      (char *) "00"},
                {COMERCIO_IATA,             TAM_ID_COMERCIO,             (char *) "0000000000"},
                {RECIBO_PENDIENTE,                               1,      (char *) "0"},
                {CAJA_REGISTRADORA,                              2,      (char *) "0"},
                {CODIGO_CAJERO_CAJA,                             2,      (char *) "0"},
                {RECARGA_EFECTIVO_CAJA,                          2,      (char *) "0"},
                {NUMERO_ULTIMA_ANULADA,     TAM_NUMERO_RECIBO,           (char *) "000000"},
                {CAJA_NO_RESPONDE,                               2,      (char *) "0"},
                {HABILITACION_VIVAMOS,                           2,      (char *) "0"},
                {TRANSACCION_VIVAMOS,                            2,      (char *) "0"},
                {TIPO_TERMINAL,                                  10,     (char *) "0000000000"},
                {TIPO_INICIA_TRANSACCION,                        2,      (char *) "00"},            // Indica quien inicio alguna transaccion, ejemplo ( Menu Punto pago o cuales quiera que se necesite )
                {VERIFICAR_PAPEL_TERMINAL,                       2,      (char *) "01"},
                {CNB_BANCO,                 SIZE_CODIGO_BANCO_CNB,       (char *) "0000"},
                {CNB_TIPO_CUENTA,                                2,      (char *) "00"},
                {CNB_PEDIR_PIN,                                  2,      (char *) "00"},
                {CNB_TERMINAL,                                   9,      (char *) ""},
                {HABILITACION_MODO_CNB,                          2,      (char *) "00"},
                {HABILITACION_MODO_CNB_AVAL,                     2,      (char *) "00"},
                {TIMEOUT_CONSULTA_DINERO_ELECTRONICO_PRIMARIA,   3,      (char *) "300"},
                {TIMEOUT_CONSULTA_DINERO_ELECTRONICO_SECUNDARIA, 3,      (char *) "120"},
                {NII_DCC,                   SIZE_NII,                    (char *) "0000"},
                {USER_CNB_LOGON,                                 1 + 1,  (char *) "0"},
                {CNB_INICIA,                                     2,      (char *) "00"},                    // Indica que CNB inicio el proceso, difetente al parametro TIPO_INICIA_TRANSACCION que indica cualquiera que inicie.
                {HABILITACION_BONOS_KIOSCO,                      2,      (char *) "0"},
                {CNB_TIPO_TARJETA,                               2,      (char *) "0"},
                {CNB_INICIALIZADO,                               1,      (char *) "0"},
                {USER_CNB,                                       10 + 1, (char *) ""},
                {ICIERRE_PARCIAL_CITIBANK,                       1 + 1,  (char *) "0"},
                {HABILITACION_MODO_CNB_CITIBANK,                 1 + 1,  (char *) "0"},
                {CONSULTA_COSTO_CNB,                             1 + 1,  (char *) "0"},
                {HABILITACION_MODO_CNB_BCL,                      1 + 1,  (char *) "0"},
                {INDICADOR_OFFLINE,                              1 + 1,  (char *) "0"},
                {NUMERO_FACTURAS_AVAL,                           2 + 1,  (char *) "0"},
                {AVANCE_EFECTIVO,                                1,      (char *) "0"},
                {TIPO_TRANSACCION_REVERSO,                       2,      (char *) "00"},
                {TIPO_TRANSACCION_CITIBANK,                      1 + 1,  (char *) "0"},
                {NII_CELUCOMPRA,            SIZE_NII,                    (char *) "0000"},
                {DESCUENTO_TARJETA_CAJA,                         1 + 1,  (char *) "0"},
                {USO_CAJA_REGISTRADORA,                          1 + 1,  (char *) "0"},
                {HABILITACION_IMPRIMIR_CIERRE_AVAL,              1 + 1,  (char *) "0"},
                {DELAY_CAJAS,                                    3 + 1,  (char *) "150"},
                {TIEMPO_CAJAS,                                   3 + 1,  (char *) "10"},
                {CNB_REALIZAR_CIERRE_PARCIAL,                    1 + 1,  (char *) "0"},
                {ESTADO_CAJA,                                    1 + 1,  (char *) "0"},
                {CAMBIAR_CLAVE_OPERADOR_EFECTIVO,                1 + 1,  (char *) "0"},
                {MODO_FALLBACK,                                  1 + 1,  (char *) "0"},
                {DEBUG_COMUNICACIONES,                           1 + 1,  (char *) "0"},
                {CNB_CAMBIO_TERMINAL,                            1 + 1,  (char *) "0"},
                {INICIALIZACION_CAMBIO_TERMINAL,                 1 + 1,  (char *) "0"},
                {NUMERO_TERMINAL_AUXILIAR,  SIZE_NUMERO_TERMINAL,        (char *) ""},
                {CAMBIO_TERMINAL_CIERRE,                         1 + 1,  (char *) "0"},
                {VALIDAR_CERO_UNICO,                             1 + 1,  (char *) "0"},
                {IMPRIMIR_INC,                                   1 + 1,  (char *) "1"},
                {TERMINAL_INICIALIZADA_ANTERIOR,                 1 + 1,  (char *) "0"},
                {TIPO_TRANSACCION_AUX,                           2 + 1,  (char *) "0"},
                {MONTO_TRANSACCION_LIMITE,                       8 + 1,  (char *) "00000000"},
                {MONTO_CVM_LIMITE,                               8 + 1,  (char *) "00000000"},
                {IMPRESION_RECIBO_CAJA,                          1 + 1,  (char *) "1"},

                {CNB_B_BANCOLOMBIA,         SIZE_CODIGO_BANCO_CNB,       (char *) "0000"},
                {CNB_T_C_BANCOLOMBIA,                            2 + 1,  (char *) "00"},
                {CNB_T_T_BANCOLOMBIA,                            1 + 1,  (char *) "0"},

                {CNB_B_AVAL,                SIZE_CODIGO_BANCO_CNB,       (char *) "0000"},
                {CNB_T_C_AVAL,                                   2 + 1,  (char *) "00"},
                {CNB_T_T_AVAL,                                   1 + 1,  (char *) "0"},

                {NOMBRE_REVERSO,                                 20,     (char *) "N/A"},
                {MODO_QR,                                        1 + 1,  (char *) "0"},
                {NUMERO_TERMINAL_QR,        SIZE_NUMERO_TERMINAL,        (char *) ""},
                {MODO_PAYPASS,                                   1 + 1,  (char *) "0"},
                {MODO_CONSULTA,                                  1 + 1,  (char *) "0"},
                {FALLBACK_FALABELLA,                             1 + 1,  (char *) "0"},
                {INTENTOS_CONSULTA_QR,                           2 + 1,  (char *) "01"},
                {IMPRIMIR_PANEL_FIRMA,                           1 + 1,  (char *) "0"},
                {ESTADO_TELECARGA,                               1 + 1,  (char *) "0"},
                {TIPO_TELECARGA,                                 1 + 1,  (char *) "0"},

                {TIPO_CAJA,                 SIZE_TIPO_CAJA + 1,  (char *) "1"},
                {URL_TEFF,                  SIZE_DOMINIO + 1,    (char *) "SIP.NET.CO"},
                {IP_CAJAS,                  SIZE_IP_DEFAULT + 1, (char *) ""},
                {ID_TEFF,                   SIZE_ID_TEFF + 1,    (char *) ""},

                {PRODUCTO_PPRO_DCC,                              1 + 1,  (char *) "0"},
                {TIPO_TRANSACCION_CAJA,                          2 + 1,  (char *) "00"},
                {TIPO_TRANSACCION_OTP,                           2 + 1,  (char *) "00"},
                {TIMEOUT_TRANSACCION,                            2,      (char *) "30"},
                {TIME_ULTIMO_CIERRE_EXITOSO,                     12 + 1, (char *) "000000000000"},
                {HORA_CIERRE,                                    4,      (char *) "9999"},
                {CIERRE_EXITOSO,                                 1 + 1,  (char *) "1"},
                {PROCESO_CIERRE,                                 1 + 1,  (char *) "0"},
                {FLUJO_UNIFICADO,                                1 + 1,  (char *) "1"},
                {FLUJO_UNIFICADO_EMV,                            1 + 1,  (char *) "0"},
                {MULTICORRESPONSAL_ACTIVO,                       1 + 1,  (char *) "0"},
                {VELOCIDAD_CAJAS,                                6,      (char *) "9600"},
                {BIG_BANCOLOMBIA,                                1 + 1,  (char *) "0"},
                {PRIMER_IVA_MULTIPLE,                            2 + 1,  (char *) "00"},
                {SEGUNDO_IVA_MULTIPLE,                           2 + 1,  (char *) "00"},
                {PRIMER_INC_MULTIPLE,                            2 + 1,  (char *) "00"},
                {SEGUNDO_INC_MULTIPLE,                           2 + 1,  (char *) "00"},
                {CAJAS_IP_CORRESPONSAL,                          1 + 1,  (char *) "0"},
                {NUMERO_PBX_AUX,                                 15,     (char *) ""},
                {INICIALIZACION_ISMP,                            1 + 1,  (char *) "0"},
                {INDICADOR_REVERSO_ISMP,                         1 + 1,  (char *) "0"},
                {ACUMULAR_LIFEMILES,                             1 + 1,  (char *) "0"},
                {CONSECUTIVO_CNB,                                6,      (char *) "      "},
                {FLUJO_CAJA_CLESS,                               1 + 1,  (char *) "0"},
                {BONO_TEFF_ACTIVO,                               1 + 1,  (char *) "0"},
                {HABILITACION_MODO_CNB_CORRESPONSAL,             1 + 1,  (char *) "0"},
                {CNB_B_CORRESPONSAL,        SIZE_CODIGO_BANCO_CNB,       (char *) "0000"},
                {CNB_T_C_CORRESPONSAL,                           2 + 1,  (char *) "00"},
                {CNB_T_T_CORRESPONSAL,                           1 + 1,  (char *) "0"},
                {RESPUESTA_CAJA_IP,                              1 + 1,  (char *) "0"},
                {URL_TEFF2,                 SIZE_DOMINIO + 1,    (char *) "SIP.NET.CO"},
                {ID_TEFF2,                  SIZE_ID_TEFF + 1,    (char *) ""},
                {TIPO_CAJA_USB,                                  1 + 1,  (char *) ""},
                {GALONES_ACTIVOS,                                1 + 1,  (char *) ""},
                {HABILITA_FIRMA_IP,                              1 + 1,  (char *) "0"},
                {URL_FIRMA,                 SIZE_DOMINIO + 1,    (char *) "SIP1.NET.CO"},
                {IP_FIRMA,                  SIZE_IP_DEFAULT + 1, (char *) "205.144.171.73"},
                {ID_FIRMA,                  SIZE_ID_TEFF +
                                            1,                   (char *) "SIPPOS1"},//se utilizara como puerto
                {NII_FIRMA,                 SIZE_NII,                    (char *) "0038"},
                {IP_FIRMA_ORIGEN,           SIZE_IP_DEFAULT,             (char *) "190.24.137.204"},
                {EFTSEC_FIRMA,                                   1 + 1,  (char *) "0"},
                {CATALOGOS_DIGITALES,                            1 + 1,  (char *) "0"},
                {HABILITAR_CONSULTA_BONOS,                       1 + 1,  (char *) "0"},
                {PUERTO_TEFF,                                    4 + 1,  (char *) "0080"},
                {NII_TEFF,                  SIZE_NII,                    (char *) "0038"},
                {EFTSEC_TEFF,                                    1 + 1,  (char *) "0"},
                {PRODUCTO_PUNTOS_COLOMBIA,                       1 + 1,  (char *) "0"},
                {IP_PUNTOS_COLOMBIA,        SIZE_IP_DEFAULT + 1, (char *) "205.144.171.73"},
                {PUERTO_PUNTOS_COLOMBIA,                         4 + 1,  (char *) "0080"},
                {DOMINIO_PUNTOS_COLOMBIA,   SIZE_DOMINIO + 1,    (char *) "SIP1.NET.CO"},
                {NII_PUNTOS_COLOMBIA,       SIZE_NII,                    (char *) "0000"},
                {EFTSEC_PUNTOS_COLOMBIA,                         1 + 1,  (char *) "0"},
                {COMPRA_CON_REFERENCIA,                          1 + 1,  (char *) "0"},
                {PUNTOS_COLOMBIA_ACTIVO,                         1 + 1,  (char *) "0"},
                { WORKING_KEY_APOS,                         16 + 1,     (char *) "0"},
                { ENVIA_POS_ENTRY_MODE_TEFF, 1 + 1, (char *)"0"},
                { BONO_VIRTUAL, 1 + 1, (char *)"0"},
                { VERSION_QRCODE, 1 + 1, (char *)"0"},
                { TIEMPO_VISUALIZACION_QR_RBM, 3 + 1, (char *)"060"},
                { OMITIR_4_ULTIMOS, 1 + 1, (char *)"0"},
                { TIEMPO_PANTALLA_PAGO, 3 + 1, (char *)"060"},
                { TIEMPO_PANTALLA_CUOTAS, 3 + 1, (char *)"060"},
                { TIEMPO_PANTALLA_CUENTA, 3 + 1, (char *)"060"},
                { TIEMPO_RX_TEF_WS, 3 + 1, (char *)"060"},
                { NUMERO_RETRANS_WS, 2 + 1, (char *)"00"},
                { ALMACENAR_TX_WS, 1 + 1, (char *)"0"},
                { MODELO_CAJA_ESCUCHA, 1 + 1, (char *)"0"},

                ////////////////////////////
                {PARAMETRO_FINAL,                                0,      (char *) ""},
                ////////////////////////////
        };

//Configuracion////
int _createParameter_() {
    int iRet = 0;
    uChar linea[256];
    //unsigned char lineaComplemento [256];
    uChar key[TAM_KEY + 1];
    uChar length[TAM_KEY + 1];
    uChar value[256 + 1];
    uChar lineaaux[256];
    char pathHost[60 + 1];
    // char discoNetcom[60 +1 ];

    int indice = 0;
    int idx = 0;
    int idy = 0;
    int len = 0;
    int resultado = 0;
    int posicion = 0;

    memset(pathHost, 0x00, sizeof(pathHost));
    memset(pathNetcom, 0x00, sizeof(pathNetcom));
    memset(linea, 0x00, sizeof(linea));
    memset(lineaaux, 0x00, sizeof(linea));
    indice = 0;

    //strcpy(pathHost,internalStoragePath );
    strcpy(pathNetcom, internalStoragePath);

    //strcat(pathHost, "param.txt");
    //strcpy(pathNetcom, "/param.txt");
    iRet = verificarArchivo(pathNetcom, "/param.txt");
    //iRet = 0;
    if (iRet != FS_OK) { //no existe carga la version por primera vez
        for (idx = 0; idx < MAX_PARAMETROS; idx++) {
            memset(linea, 0x00, sizeof(linea));
            memset(key, 0x00, sizeof(key));
            memset(length, 0x00, sizeof(length));
            memset(value, 0x00, sizeof(value));
            indice = 0;

            sprintf(key, "%03d", tablaParametros[idx].key);
            sprintf(length, "%02d", tablaParametros[idx].length);
            _rightPad_(value, tablaParametros[idx].value, 0x03,
                       tablaParametros[idx].length);

            memcpy(linea + indice, key, TAM_KEY);
            indice += TAM_KEY;
            memcpy(linea + indice, length, TAM_LENGTH);
            indice += TAM_LENGTH;
            memcpy(linea + indice, value, tablaParametros[idx].length);
            indice += tablaParametros[idx].length;
            linea[indice] = 0x0D;
            linea[indice + 1] = 0x0A;
            indice += TAM_FIN_REGISTRO;

            iRet = escribirArchivo(pathNetcom, (char *) "/param.txt", linea,
                                   strlen(linea));
            if (tablaParametros[idx + 1].key == PARAMETRO_FINAL) {
                break;
            }
        }

    } else {
        // SI EXISTE ARCHIVO DE PARAMETROS Y HAY QUE REESCRIBIRLO
        do {
            memset(linea, 0x00, sizeof(linea));
            memset(key, 0x00, sizeof(key)); // 2
            memset(length, 0x00, sizeof(length)); // 2
            memset(value, 0x00, sizeof(value)); // len + 2
            len = TAM_KEY + TAM_LENGTH + tablaParametros[idx].length
                  + TAM_FIN_REGISTRO + 3;

            resultado = buscarArchivo(pathNetcom, (char *) "/param.txt", linea,
                                      posicion, len);
            idx++;
            posicion += len;

        } while (resultado == 0);

        if (tablaParametros[idx - 1].key != PARAMETRO_FINAL) {
            //escribir nuevos parametros.
            for (idy = idx - 1; idy < MAX_PARAMETROS; idy++) {
                memset(linea, 0x00, sizeof(linea));
                memset(key, 0x00, sizeof(key));
                memset(length, 0x00, sizeof(length));
                memset(value, 0x00, sizeof(value));
                indice = 0;

                sprintf(key, "%03d", tablaParametros[idy].key);
                sprintf(length, "%02d", tablaParametros[idy].length);
                _rightPad_(value, tablaParametros[idy].value, 0x03,
                           tablaParametros[idy].length);

                memcpy(linea + indice, key, TAM_KEY);
                indice += TAM_KEY;
                memcpy(linea + indice, length, TAM_LENGTH);
                indice += TAM_LENGTH;
                memcpy(linea + indice, value, tablaParametros[idy].length);
                indice += tablaParametros[idy].length;
                linea[indice] = 0x0D;
                linea[indice + 1] = 0x0A;
                indice += TAM_FIN_REGISTRO;
                iRet = escribirArchivo(pathNetcom, (char *) "/param.txt", linea,
                                       strlen(linea));

                if (tablaParametros[idy + 1].key == PARAMETRO_FINAL) {
                    break;
                }

            }
        }
    }

    return iRet;
}

int setParameter(int usKey, char *pcParam) {

    int resultado = 0;
    int len = 0;
    int posicion = 0;
    int iRet = -1;
    int idx = 0;
    uChar linea[256 + 1];
    uChar length[TAM_LENGTH + 1];
    uChar value[256 + 1];
    char pathNetcom[60 + 1];

    memset(pathNetcom, 0x00, sizeof(pathNetcom));

    strcpy(pathNetcom, "/param.txt");
    //strcat(pathNetcom,"/param.txt");
    for (;;) {

        len = TAM_KEY + TAM_LENGTH + tablaParametros[idx].length + TAM_FIN_REGISTRO;

        if (idx == (usKey - 1)) {

            memset(linea, 0x00, sizeof(linea));
            memset(length, 0x00, sizeof(length));

            resultado = buscarArchivo(discoNetcom, pathNetcom, linea, posicion, len);

            memset(value, 0x00, sizeof(value));
            _rightPad_(value, pcParam, 0x03, tablaParametros[idx].length);
            posicion += (TAM_KEY + TAM_LENGTH);

            iRet = actualizarArchivo(discoNetcom, pathNetcom, value, posicion,
                                     tablaParametros[idx].length);
            iRet = 1;
            break;
        }

        idx++;
        posicion += len;

        if (resultado != 0) {
            break;
        }
    }

    return iRet;
}

int solicitarImpuestosMultiples(char * datoPrimerImpuesto, char * datoSegundoImpuesto, int tipoImpuesto) {

    char impuestoMaximo[2 + 1];
    int diferencia = 0;
    int resultado = 1;

    memset(impuestoMaximo, 0x00, sizeof(impuestoMaximo));

    getParameter(MAXIMO_IVA,impuestoMaximo);


    if (atol(datoPrimerImpuesto) < 1) {
        resultado = -1;
    }

    if (atoi(datoPrimerImpuesto) > atoi(impuestoMaximo)) {
        resultado = -2;
    }


    if (resultado > 0) {
        diferencia = atoi(impuestoMaximo) - atoi(datoPrimerImpuesto);
        memset(impuestoMaximo, 0x00, sizeof(impuestoMaximo));
        sprintf(impuestoMaximo, "%d", diferencia);

        if (atol(datoSegundoImpuesto) < 1) {
            resultado = -1;
        }

        if (atoi(datoSegundoImpuesto) >= atoi(datoPrimerImpuesto)) {
            resultado = -3;
        }
    }

    if (resultado > 0) {
        if(tipoImpuesto == IMPUESTO_IVA_MULTIPLE) {
            if (atoi(datoPrimerImpuesto) > atoi(datoSegundoImpuesto)) {
                setParameter(PRIMER_IVA_MULTIPLE, datoPrimerImpuesto);
                setParameter(SEGUNDO_IVA_MULTIPLE, datoSegundoImpuesto);
            } else if (atoi(datoSegundoImpuesto) > atoi(datoPrimerImpuesto)) {
                setParameter(PRIMER_IVA_MULTIPLE, datoSegundoImpuesto);
                setParameter(SEGUNDO_IVA_MULTIPLE, datoPrimerImpuesto);
            }
        } else if (tipoImpuesto == IMPUESTO_INC_MULTIPLE) {
            if (atoi(datoPrimerImpuesto) > atoi(datoSegundoImpuesto)) {
                setParameter(PRIMER_INC_MULTIPLE, datoPrimerImpuesto);
                setParameter(SEGUNDO_INC_MULTIPLE, datoSegundoImpuesto);
            } else if (atoi(datoSegundoImpuesto) > atoi(datoPrimerImpuesto)) {
                setParameter(PRIMER_INC_MULTIPLE, datoSegundoImpuesto);
                setParameter(SEGUNDO_INC_MULTIPLE, datoPrimerImpuesto);
            }
        }
    } else {
        if (tipoImpuesto == IMPUESTO_IVA_MULTIPLE) {
            setParameter(PRIMER_IVA_MULTIPLE, (char *)"00");
            setParameter(SEGUNDO_IVA_MULTIPLE, (char *)"00");
        } else if(tipoImpuesto == IMPUESTO_INC_MULTIPLE) {
            setParameter(PRIMER_INC_MULTIPLE, (char *)"00");
            setParameter(SEGUNDO_INC_MULTIPLE, (char *)"00");
        }
    }

    return resultado;
}

int validarcapturaImpuestoAutomatico(char * datoCapturado, int tipoImpuesto) {

    char valorImpuestoValidacion[2 + 1];
    char mensajeImpuesto[3 + 1];
    int clave1 = 0;
    uChar impuestoMaximo[2 + 1];
    uChar impuestoAutomatico[1 + 1];
    TablaUnoInicializacion tablaUno;
    int continuar = 1;

    memset(&tablaUno, 0x00, sizeof(tablaUno));
    memset(valorImpuestoValidacion, 0x00, sizeof(valorImpuestoValidacion));
    memset(mensajeImpuesto, 0x00, sizeof(mensajeImpuesto));
    memset(impuestoMaximo, 0x00, sizeof(impuestoMaximo));
    memset(impuestoAutomatico, 0x00, sizeof(impuestoAutomatico));

    if (tipoImpuesto == IMPUESTO_IVA) {
        clave1 = IVA_AUTOMATICO;
    } else if (tipoImpuesto == IMPUESTO_INC) {
        clave1 = INC_AUTOMATICO;
    }

    if (verificarArchivo(discoNetcom, (char *)TABLA_1_INICIALIZACION) == FS_OK) {
        tablaUno = _desempaquetarTablaCeroUno_();
        sprintf(impuestoMaximo, "%02x", tablaUno.ecrBaudRate);
    } else {
        strcpy(impuestoMaximo, "00");
    }

    if (atoi(impuestoMaximo) > 0) {

        if (atol(datoCapturado) < 1 ) {
            continuar = -1;
        }

        if (atoi(datoCapturado) > atoi(impuestoMaximo)) {
            continuar = -2;
        }

        /////// DESHABILITAR LA PREGUNTA DEL CALCULO DE LA BASE DE DEVOLUCION SI EL PORCENTAJE
        /////// DE IVA ES DEL 5 U 8 %.
        if ( tipoImpuesto == IMPUESTO_IVA) {
            memset(impuestoAutomatico, 0x00, sizeof(impuestoAutomatico));
            getParameter(INC_AUTOMATICO, impuestoAutomatico);

            if (strcmp(impuestoAutomatico, "00") == 0) {
                setParameter(IMPRIMIR_INC, (char *)"0");
            } else {
                setParameter(IMPRIMIR_INC, (char *)"1");
            }

            if ((atoi(datoCapturado) == 5) || (atoi(datoCapturado) == atoi(impuestoMaximo))) {
                setParameter(CALCULAR_BASE, (char *)"1");
            }
        }

        if (tipoImpuesto == IMPUESTO_INC) {
            setParameter(IMPRIMIR_INC, (char *)"1");
        }

        if (continuar > 0 && tipoImpuesto == IMPUESTO_IVA) {
            continuar = 2;
        } else {
            continuar = 1;
        }
    }
    if (continuar < 0) {
        memset(datoCapturado, 0x00, sizeof(datoCapturado));
        strcpy(datoCapturado, "00");
    }

    setParameter(clave1, datoCapturado);
    //imprimir recibo comprobante de iva e inc.
    if (continuar == 1) {
        if (tipoImpuesto == IMPUESTO_IVA) {
//            screenMessage("ADM IVA", "", "RECIBO", "IVA AUTOMATICO",
//                          2 * 1000);
        } else {
//            screenMessage("ADM INC", "", "RECIBO", "INC AUTOMATICO",
//                          2 * 1000);
        }
//        imprimirComprobanteImpuesto(atoi(datoCapturado), tipoImpuesto);
    }
    return continuar;
}

int capturarImpuestoMultiple(int tipoImpuesto) {

    char valorImpuesto[2 + 1];
    int clave = 0;
    int continuar = 1;


    if (tipoImpuesto == IMPUESTO_IVA_MULTIPLE) {
        clave = IVA_MULTIPLE;

    } else if (tipoImpuesto == IMPUESTO_INC_MULTIPLE) {
        clave = INC_MULTIPLE;
        getParameter(INC_AUTOMATICO, valorImpuesto);
    }

    memset(valorImpuesto, 0x00, sizeof(valorImpuesto));
    getParameter(IVA_AUTOMATICO, valorImpuesto);
    if (strncmp(valorImpuesto, "00", 2) != 0) {
        continuar = -1;
    }
    memset(valorImpuesto, 0x00, sizeof(valorImpuesto));
    getParameter(INC_AUTOMATICO, valorImpuesto);
    if (strncmp(valorImpuesto, "00", 2) != 0) {
        continuar = -2;
    }

    if (continuar == 1) {
        memset(valorImpuesto, 0x00, sizeof(valorImpuesto));
        getParameter(clave, valorImpuesto);

        if (strncmp(valorImpuesto, "0", 1) == 0) {
            continuar = 1;
        } else {
            continuar = 0;
        }

        setParameter(clave, valorImpuesto);

    }
    return continuar;
}

int capturarImpuestoAutomatico(int tipoImpuesto) {

    char valorImpuesto[2 + 1];
    char valorImpuestoValidacion[2 + 1];
    int clave1 = 0;
    uChar impuestoMaximo[2 + 1];
    TablaUnoInicializacion tablaUno;
    int continuar = 1;

    memset(&tablaUno, 0x00, sizeof(tablaUno));
    memset(valorImpuesto, 0x00, sizeof(valorImpuesto));
    memset(valorImpuestoValidacion, 0x00, sizeof(valorImpuestoValidacion));
    memset(impuestoMaximo, 0x00, sizeof(impuestoMaximo));

    if (tipoImpuesto == IMPUESTO_IVA) {
        clave1 = IVA_AUTOMATICO;
    } else if (tipoImpuesto == IMPUESTO_INC) {
        clave1 = INC_AUTOMATICO;
    }

    getParameter(clave1, valorImpuesto);

    if (strncmp(valorImpuesto, "00", 2) == 0) {
        //automatico
        memset(valorImpuestoValidacion, 0x00, sizeof(valorImpuestoValidacion));
        getParameter(IVA_MULTIPLE, valorImpuestoValidacion);
        if (strncmp(valorImpuestoValidacion, "0", 2) != 0) {
            continuar = -1;
        }
        memset(valorImpuestoValidacion, 0x00, sizeof(valorImpuestoValidacion));
        getParameter(INC_MULTIPLE, valorImpuestoValidacion);
        if (strncmp(valorImpuestoValidacion, "0", 2) != 0) {
            continuar = -2;
        }

        if (continuar == 1) {
            if (verificarArchivo(discoNetcom, (char *)TABLA_1_INICIALIZACION) == FS_OK) {
                tablaUno = _desempaquetarTablaCeroUno_();
                sprintf(impuestoMaximo, "%02x", tablaUno.ecrBaudRate);
            } else {
                strcpy(impuestoMaximo, "00");
            }

            if (atoi(impuestoMaximo) > 0) {
                continuar = 1;

            } else {
                continuar = -3;
            }
        }

    } else {
        continuar = 0;
    }

    return continuar;
}

int getParameter(int usKey, char *pcParam) {

    int resultado = 0;
    int len = 0;
    int posicion = 0;
    int iRet = -1;
    int idx = 0;
    int i = 0;
    int lengthValue = 0;
    int lengthParam = 0;
    uChar linea[256 + 1];
    uChar length[TAM_LENGTH + 1];
    uChar value[256 + 1];
    char pathNetcom[60 + 1];

    memset(pathNetcom, 0x00, sizeof(pathNetcom));

    strcpy(pathNetcom, "/param.txt");
    for (;;) {

        len = TAM_KEY + TAM_LENGTH + tablaParametros[idx].length + TAM_FIN_REGISTRO;

        if (idx == (usKey - 1)) {

            memset(linea, 0x00, sizeof(linea));
            memset(length, 0x00, sizeof(length));
            memset(value, 0x00, sizeof(value));

            resultado = buscarArchivo(discoNetcom, pathNetcom, linea, posicion, len);

            memcpy(length, linea + TAM_KEY, TAM_LENGTH);
            lengthParam = atoi(length);
            memcpy(value, linea + TAM_KEY + TAM_LENGTH, lengthParam);
            iRet = 1;
            break;
        }

        idx++;
        posicion += len;

        if (resultado != 0) {
            break;
        }
    }

    /** Obtener longitud exacta del valor **/
    for (i = 0; i < lengthParam; i++) {
        if (value[i] == 0x03) {
            break;
        } else {
            lengthValue++;
        }
    }

    /** Copiar valor de retorno **/
    if (lengthValue > 0) {
        memcpy(pcParam, value, lengthValue);
    }

    return iRet;
}

/**
 * @brief Obtiene el terminal ID configurado en el datafono, se tiene en cuenta si es
 * Modo Comercio, Modo CNB ( Modo Mixto o Puro ).
 * @param terminal Buffer en donde quedara almacenado el codigo de terminal.
 */
void getTerminalId(char *terminal) {
    char modoInicia[2 + 1];

    memset(modoInicia, 0x00, sizeof(modoInicia));
    memset(terminal, 0x00, sizeof(terminal));

    getParameter(CNB_INICIA, modoInicia);

    if (atoi(modoInicia) == 0) {
        getParameter(NUMERO_TERMINAL, terminal);
    } else { // El llamado se hizo desde un CNB.
        getParameter(CNB_TERMINAL, terminal);

        if (strlen(terminal) ==
            0) { // Verifica si tiene dato, esto es verificar si esta en modo puro o modo mixto.
            memset(terminal, 0x00, sizeof(terminal));
            getParameter(NUMERO_TERMINAL, terminal);
        }
    }

}

int verificarModoCNB() {
    int resultado = 0;
    uChar cnb[2 + 1];
    char numeroTerminal[SIZE_NUMERO_TERMINAL + 1];
    char codigoBanco[SIZE_CODIGO_BANCO_CNB + 1];
    int modo = 0;

    memset(cnb, 0x00, sizeof(cnb));

    if (validarMulticorresponsal() != 2) {

        getParameter(HABILITACION_MODO_CNB, cnb);
        if (atoi(cnb) == MODO_CNB_RBM || atoi(cnb) == MODO_CNB_CITIBANK ||
            atoi(cnb) == MODO_CNB_BANCOLOMBIA
            || atoi(cnb) == MODO_CNB_CORRESPONSAL) {
            resultado = 1;
        }

        if (resultado > 0) {
            memset(numeroTerminal, 0x00, sizeof(numeroTerminal));
            memset(codigoBanco, 0x00, sizeof(codigoBanco));

            getParameter(CNB_TERMINAL, numeroTerminal);
            getParameter(CNB_BANCO, codigoBanco);
            if ((strlen(numeroTerminal) > 0) &&
                (strncmp(codigoBanco, "0000", SIZE_CODIGO_BANCO_CNB) != 0)) {
                modo = MODO_MIXTO;
            } else if ((strncmp(codigoBanco, "0000", SIZE_CODIGO_BANCO_CNB) != 0)) {
                modo = MODO_PURO;
            }
        }
    }

    return modo;
}

int validarMulticorresponsal(void) {

    int iRet = 0;
    int tam = 0;
    int iteraciones = 0;
    int i = 0;
    int idx = 3;
    int cantidadMulticorresponsales = 0;

    tam = tamArchivo(discoNetcom, (char *) RUTA_TABLA_MULTICORRESPONSAL);

    if (tam > 0) {

        char datosMulticorresponsal[tam + 1];
        memset(datosMulticorresponsal, 0x00, sizeof(datosMulticorresponsal));

        iRet = leerArchivo(discoNetcom, (char *) RUTA_TABLA_MULTICORRESPONSAL,
                           datosMulticorresponsal);

        if (iRet > 0) {
            iteraciones = tam / 46; //!! 46 es el tamaño de datos por cada multicorresponsal

            for (i = 0; i < iteraciones; i++) {

                if (strncmp(datosMulticorresponsal + idx, "0007", 4) == 0
                    || strncmp(datosMulticorresponsal + idx, "0903", 4) == 0) {

                    cantidadMulticorresponsales++;
                }
                idx += 46;
            }
        }
    }
    return cantidadMulticorresponsales;
}

int estadoTerminalConfigurado(void) {

    char data[20 + 1] = {0x00};
    int resultado = 1;
    int verificar = 0;

    getParameter(TERMINAL_INICIALIZADA, data);
    LOGI("Terminal configurado %s ", data);
    if (atoi(data) == 0) {
        resultado = -1;
        //screenMessage("MENSAJE", "DEBE INICIALIZAR", "ESPECIFIQUE NII,TEL", "NUMERO TERMINAL", 5 * 1000);
    }

    //////// VERIFICAR ARCHIVO EFT_SET ///////////
    verificar = verificarArchivo(discoNetcom, "/caroot.pem");
    LOGI("caroot.pem %d ", verificar);
    /// SE COMENTA PARA PRUEBAS SIN ENCRIPCION
    if (verificar != FS_OK) {
        resultado = -1;
        //screenMessage("MENSAJE", "NO EXISTE ARCHIVO", "CAROOT.PEM", "PARA TLS", 2 * 1000);
    }

    return resultado;
}

int getModoCNB(char *texto) {
    uChar cnb[2 + 1];
    int resultado = -1;

    memset(cnb, 0x00, sizeof(cnb));
    memset(texto, 0x00, sizeof(texto));

    getParameter(HABILITACION_MODO_CNB, cnb);

    if (atoi(cnb) == MODO_CNB_RBM) {
        memcpy(texto, "CNB", 7);
    } else if (atoi(cnb) == MODO_CNB_BANCOLOMBIA) {
        memcpy(texto, "CNB BANCOLOMBIA", 15);
    } else if (atoi(cnb) == MODO_CNB_CORRESPONSAL) {
        memcpy(texto, "CB CORRESPONSAL", 15);
    } else if (atoi(cnb) == MODO_CNB_CITIBANK) {
        memcpy(texto, "CNB CITI", 8);
    }
    resultado = atoi(cnb);

    return resultado;
}

int verificarCambioTerminal(char *terminal) {
    int respuesta = 0;
    int modoCnb = 0;
    int tarjetaSembrada = 0;
    int verificarFile = 0;
    int cantidadCorresponsales = 0;

    char numeroTerminalActual[8 + 1] = {0x00};
    uChar numeroTerminalAux[8 + 1] = {0x00};
    char numeroTerminalVacio[8 + 1] = {0x00};
    char texto[15 + 1] = {0x00};
    char lineaResumen[100 + 1];
    char dataAux[20 + 1];

    getParameter(NUMERO_TERMINAL, numeroTerminalActual);
    LOGI("Terminal actual %s ", numeroTerminalActual);
    memcpy(numeroTerminalAux, numeroTerminalActual, 8);
    if ((strncmp(numeroTerminalActual, terminal, 8) == 0)
        || (strncmp(numeroTerminalActual, numeroTerminalVacio, 8) == 0)) {
        respuesta = setParameter(NUMERO_TERMINAL, terminal);
        LOGI("Terminal seteado por ser el mismo o estar vacio  %s ", terminal);
    } else {
        tarjetaSembrada = verificarArchivo(discoNetcom, TARJETA_CNB);
        modoCnb = verificarModoCNB();
        getModoCNB(texto);
        LOGI("Terminal diferente sembrada  %d modoCnb %d ", tarjetaSembrada, modoCnb);
        if ((tarjetaSembrada != 1 && modoCnb == 0) ||
            modoCnb == MODO_MIXTO) { //&& modoCnb != MODO_PURO
            setParameter(CAMBIO_TERMINAL_CIERRE, CAMBIO_TERMINAL_CIERRE_ACTIVO);
            enviarStringToJava("REALIZANDO CIERRE COMERCIO ...", (char *)"mostrarInicializacion");
            verificarFile = verificarArchivo(discoNetcom, DATOS_QRCODE);
            respuesta = (verificarFile == FS_OK) ? 1 : 0;

            if (respuesta > 0) {
                borrarArchivo(discoNetcom, DATOS_QRCODE);
            }

            if(realizarConexionTlS() > 0){
                respuesta = _cierreVenta_();
                cerrarConexion();
            }
            setParameter(CAMBIO_TERMINAL_CIERRE, CAMBIO_TERMINAL_CIERRE_INACTIVO);
        } else {
            respuesta = 1;
        }
        LOGI("respuesta %d ", respuesta);
        if (respuesta > 0) {
            if(realizarConexionTlS() > 0){
                respuesta = cierreCorresponsales();
                cerrarConexion();
            }

        }
        LOGI("respuesta1 %d ", respuesta);
        if (respuesta > 0) {

            cantidadCorresponsales = validarMulticorresponsal();

            if (cantidadCorresponsales == 2) {
                //respuesta = netcomServices(APP_TYPE_MULTICORRESPONSAL, CIERRE_MULTICORRESPONSAL, 1,&pucBuffer);
                respuesta = 1;
                borrarArchivo(discoNetcom, DIRECTORIO_JOURNALS);
            }
        }
        LOGI("respuesta2 %d ", respuesta);
        if (respuesta > 0) {
            respuesta = setParameter(NUMERO_TERMINAL, terminal);

            if (respuesta < 0) {
                enviarStringToJava("ERROR AL ESCRIBIR NTERMINAL", (char *)"mostrarInicializacion");
            } else {
                setParameter(NUMERO_TERMINAL_AUXILIAR, numeroTerminalAux);
                setParameter(INICIALIZACION_CAMBIO_TERMINAL, (char *) "1");
                setParameter(TIPO_TELECARGA, "1");
                if(realizarConexionTlS() > 0) {
                    enviarStringToJava((char *)"REALIZANDO INICIALIZACION CONECTANDO...", (char *)"mostrarInicializacion");
                    respuesta = invocarInicializacionGeneral(INICIALIZACION_MANUAL);
                    if (respuesta > 0){
                        memset(dataAux, 0x00, sizeof(dataAux));
                        getParameter(IP_VENTA_PRIMARIA, dataAux);
                        setParameter(IP_DEFAULT, dataAux);

                        memset(dataAux, 0x00, sizeof(dataAux));
                        getParameter(PUERTO_VENTA_PRIMARIA, dataAux);
                        setParameter(PUERTO_DEFAULT, dataAux);
                    }
                    cerrarConexion();
                }
            }
        }
    }
    LOGI("respuesta al salir de cambio terminal %d",respuesta);
    setParameter(TIPO_TELECARGA, "0");
    return respuesta;
}

void setearTransaccion(int tipoTransaccion) {

    char nombreTransaccion[20 + 1];
    char nombreReverso[20 + 1];
    char typeTransation[2 + 1];
    char transaccion[2 + 1];

    memset(nombreTransaccion, 0x00, sizeof(nombreTransaccion));
    memset(transaccion, 0x00, sizeof(transaccion));
    memset(typeTransation, 0x00, sizeof(typeTransation));
    LOGI("tipo Transaccion para reverso %d",tipoTransaccion);
    switch (tipoTransaccion) {
        case TRANSACCION_VENTA:
            strcpy(nombreTransaccion, "VENTA");
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            break;
        case TRANSACCION_CUENTAS_DIVIDIDAS:
            strcpy(nombreTransaccion, "VENTA");
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            break;
        case TRANSACCION_SALDO:
        case TRANSACCION_CONSULTAR_SALDO:
            strcpy(nombreTransaccion, "SALDO");
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            break;
        case TRANSACCION_ANULACION:
            strcpy(nombreTransaccion, "ANULACION");
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            break;
        case TRANSACCION_PUNTOS_COLOMBIA:
            break;
        case TRANSACCION_REDENCION_BONO:
            strcpy(nombreTransaccion, "REDIMIR BONO");
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            break;
        case TRANSACCION_HABILITAR_BONO:
            strcpy(nombreTransaccion, "HABILITACION BONO");
            sprintf(transaccion, "%02d", tipoTransaccion);
           setParameter(TIPO_TRANSACCION, transaccion);
            break;
        case TRANSACCION_RECARGAR_BONO:
            strcpy(nombreTransaccion, "RECARGA BONO");
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            break;
        case TRANSACCION_PSP:
            strcpy(nombreTransaccion, "PSP");
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            break;
        case TRANSACCION_PCR:
            strcpy(nombreTransaccion, "PCR");
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            break;
        case TRANSACCION_RECARGA:
            strcpy(nombreTransaccion, "RECARGA");
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            break;
        case TRANSACCION_TRANSFERENCIA:
            strcpy(nombreTransaccion, "TRANSFERENCIA");
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            break;

        case TRANSACCION_MULTIPOS:
            strcpy(nombreTransaccion, "MULTIPOS");
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            break;

        case TRANSACCION_TARIFA_ADMIN:
            strcpy(nombreTransaccion, "TARIFA ADMIN");
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            break;
        case TRANSACCION_DESCUENTOS_VIVAMOS:
            strcpy(nombreTransaccion, "DESCUENTOS");
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            break;

        case TRANSACCION_CONSULTA_VIVAMOS:
            strcpy(nombreTransaccion, "CONSULTA");
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            break;

        case TRANSACCION_CONSULTAR_VALOR_CONSIGNAR:
            strcpy(nombreTransaccion, "V.CONSIGNAR");
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            break;

        case TRANSACCION_SOLICITAR_PRODUCTO:
            strcpy(nombreTransaccion, "SOLICITUD PRODUCTO");
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            break;

        case TRANSACCION_EFECTIVO:
            strcpy(nombreTransaccion, "EFECTIVO");
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            break;

        case TRANSACCION_EFECTIVO_RETIRO:
        case TRANSACCION_BCL_RETIRO:
        case TRANSACCION_BCL_RETIRO_EFECTIVO:
        case TRANSACCION_MCBC_RETIRO:
        case TRANSACCION_MCBC_RETIRO_EFECTIVO:
        case TRANSACCION_MCBA_RETIRO:
            strcpy(nombreTransaccion, "RETIRO");
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            break;

        case TRANSACCION_EFECTIVO_DEPOSITO:
        case TRANSACCION_BCL_DEPOSITO:
        case TRANSACCION_MCBC_DEPOSITO:
        case TRANSACCION_MCBA_DEPOSITO:
            strcpy(nombreTransaccion, "DEPOSITO");
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            break;

        case TRANSACCION_PSP_CNB:
        case TRANSACCION_BCL_RECAUDO:
        case TRANSACCION_MCBA_RECAUDO:
        case TRANSACCION_MCBC_RECAUDO:
            strcpy(nombreTransaccion, "RECAUDO");
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            break;

        case TRANSACCION_PAGO_ELECTRONICO:
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION_REVERSO, transaccion);
        case TRANSACCION_PAGO_ELECTRONICO_ESPECIAL:
            strcpy(nombreTransaccion, "PAGO ELECTRONICO");
            memset(transaccion, 0x00, sizeof(transaccion));
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            if (tipoTransaccion == TRANSACCION_PAGO_ELECTRONICO_ESPECIAL) {
               setParameter(TIPO_TRANSACCION_REVERSO, transaccion);
            }

            break;
        case TRANSACCION_RETIRO_OTP:
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION_REVERSO, transaccion);
            strcpy(nombreTransaccion, "EFECTIVO ELECTRONICO");
            memset(transaccion, 0x00, sizeof(transaccion));
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            if (tipoTransaccion == TRANSACCION_PAGO_ELECTRONICO_ESPECIAL) {
                setParameter(TIPO_TRANSACCION_REVERSO, transaccion);
            }

            break;
        case TRANSACCION_CNB_CITIBANK:
            getParameter(TIPO_TRANSACCION_CITIBANK, typeTransation);
            switch (atoi(typeTransation)) {
                case 1: //RETIRO_AVANCE_CITI
                    strcpy(nombreTransaccion, "RETIRO/AVANCE CITI");
                    break;

                case 2: //DEPOSITO_CITI
                    strcpy(nombreTransaccion, "DEPOSITO CITI");
                    break;

                case 3: //RECAUDO_CITI
                    strcpy(nombreTransaccion, "RECAUDO CITI");
                    break;

                case 4: //PAGO_PRODUCTOS_BANCO_CITI:
                    strcpy(nombreTransaccion, "PAGO PRODUCTOS CITI");
                    break;

                case 5: //PAGO_FACTURAS_CITI:
                    strcpy(nombreTransaccion, "PAGO FACTURAS CITI");
                    break;
            }

            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);

            break;
        case TRANSACCION_BCL_TRANSFERENCIA:
        case TRANSACCION_MCBC_TRANSFERENCIA:
            strcpy(nombreTransaccion, "TRANSFERENCIA");
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            break;

        case TRANSACCION_TARJETA_PRIVADA:
            strcpy(nombreTransaccion, "PAGO T. PRIVADA");
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            break;
        case TRANSACCION_RETIRO_CNB_AVAL:
            strcpy(nombreTransaccion, "RETIRO AVAL");
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            break;
        case TRANSACCION_PAGO_FACTURAS_AVAL:
            strcpy(nombreTransaccion, "RECAUDO AVAL");
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            break;
        case TRANSACCION_TRANSFERENCIA_AVAL:
            strcpy(nombreTransaccion, "TRANSFERENCIA  AVAL");
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            break;
        case TRANSACCION_P_PRODUCTO_TARJETA_AVAL:
        case TRANSACCION_P_PRODUCTO_NO_TARJETA_AVAL:
            strcpy(nombreTransaccion, "PAGO PRODUCTO");
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            break;
        case TRANSACCION_PAGO_IMPUESTOS:
            strcpy(nombreTransaccion, "PAGO IMPUESTOS");
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            break;
        case TRANSACCION_DEPOSITO_EFECTIVO_CNB_AVAL:
            strcpy(nombreTransaccion, "DEPOSITO AVAL");
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            break;
        case TRANSACCION_BCL_PAGO_CARTERA:
        case TRANSACCION_BCL_PAGO_TARJETA:
        case TRANSACCION_MCBC_PAGO_TARJETA:
        case TRANSACCION_MCBC_PAGO_CARTERA:
            strcpy(nombreTransaccion, "PAGO");
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            break;

        case TRANSACCION_CELUCOMPRA:
            strcpy(nombreTransaccion, "CELUCOMPRA");
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            break;
        case TRANSACCION_GASO_PASS:
            strcpy(nombreTransaccion, "GASO PASS");
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            break;
        case TRANSACCION_ACUMULAR_LEALTAD:
            strcpy(nombreTransaccion, "LEALTAD ACUMULACION");
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            break;
        case TRANSACCION_AJUSTAR_LEALTAD:
            strcpy(nombreTransaccion, "LEALTAD AJUSTE");
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            break;
        case TRANSACCION_CONSULTA_QR:
            strcpy(nombreTransaccion, "CODIGO QR");
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            break;
        case TRANSACCION_CODIGO_ESTATICO:
            strcpy(nombreTransaccion, "CONSULTA  ESTADO");
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            break;
        case TRANSACCION_REDIMIR_FALABELLA:
            strcpy(nombreTransaccion, "REDENCION FALABELLA");
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            break;
        case TRANSACCION_BIG_BANCOLOMBIA:
            strcpy(nombreTransaccion, "LEALTAD PLUS");
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            break;
        case TRANSACCION_REDENCION_LIFEMILES:
            strcpy(nombreTransaccion, "LEALTAD REDENCION");
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            break;
        case TRANSACCION_FOCO_SODEXO:
            strcpy(nombreTransaccion, "SODEXO WIZEO");
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            break;
        case TRANSACCION_BCL_GIROS:
        case TRANSACCION_BCL_RECLAMAR_GIROS:
        case TRANSACCION_BCL_CANCELAR_GIROS:
            strcpy(nombreTransaccion, "GIROS BANCOLOMBIA");
            sprintf(transaccion, "%02d", tipoTransaccion);
            setParameter(TIPO_TRANSACCION, transaccion);
            break;
        default:
            strcpy(nombreTransaccion, "VENTA");
            break;

    }

    if (verificarArchivo(discoNetcom, ARCHIVO_REVERSO) == FS_OK ) {
        getParameter(NOMBRE_TRANSACCION, nombreReverso);
        LOGI("si existe alguna monda  ");

    } else {
        LOGI("este es el nombre de la tx  %s",nombreTransaccion);
        setParameter(NOMBRE_REVERSO, nombreTransaccion);
    }
    setParameter(NOMBRE_TRANSACCION, nombreTransaccion);

}
void obtenerLlaveWorking(char * working){
    char auxiliarWorking[32 + 1] = {0x00};
    getParameter(WORKING_KEY_APOS, auxiliarWorking);
    _convertBCDToASCII_(working,auxiliarWorking, 32 );
    LOGI("working %s ", working);
}
