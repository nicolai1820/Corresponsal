#include <jni.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <sys/time.h>
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/socket.h>
#include "netinet/in.h"
#include <arpa/inet.h>
#include "Archivos.h"
#include "native-lib.h"

#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <memory.h>
#include <pthread.h>
#include <dlfcn.h>
#include <lealtad.h>
#include "Utilidades.h"
#include "Mensajeria.h"
#include "bancolombia.h"
#include "Definiciones.h"
#include "comunicaciones.h"
#include "venta.h"
#include "Inicializacion.h"
#include "configuracion.h"
#include "android/log.h"

#define  LOG_TAG    "NETCOM_NATIVE_LIB"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

typedef unsigned long uLong;
typedef unsigned int uInt;

#define  VERSION_SOFTWARE  "1.0"
#define  REVISION_SOFTWARE "RC"

int entrandoAotraFuncion(int num1, int num2);

int getIdFieldISO(int field);

void inicializarCampos(void);

void setMTI(uChar *valueMTI);

void setField(int field, uChar *valueField, int totalBytes);

void leftPad(uChar *destination, uChar *source, char fill, int len);

void convertASCIIToBCD(uChar *destination, uChar *source, int length);

void rightPad(char *destination, char *source, char fill, int len);

void _convertASCIIToBCD_(uChar *destination, uChar *source, int length);

int realizarTransaccion(uChar *message, int len, uChar *dataRecibida);

void releaseSocket();

//void _guardarTransaccion_(DatosVenta tramaVenta);
void crearDisco(char *path);

int _createParameter_();

int setParameter(int usKey, char *pcParam);

int getParameter(int usKey, char *pcParam);

int invocarInicializacionGeneral(char *codigoProcesoInicial);

void configurarDatosComunicacion(TablaCuatroInicializacion tablaCuatro);

void _inicializacionSerial_(void);

void obtenerStringDeclinada(char *cadena, JNIEnv *env, jobject thiz);

DatosComision obtenerDatosComision(JNIEnv *env, jobject datos_comision);

DatosBasicosVenta obtenerDatosBasicos(JNIEnv *env, jobject datos_basicos);

DatosTarjetaAndroid obtenerDatosTarjeta(JNIEnv *env, jobject datos_tarjeta);

void setearDatosBasicosJava(DatosBasicosVenta datosBasicos, JNIEnv *env, jobject datos_basicos);

DatosVenta globalTramaVenta;
DatosCnbBancolombia globalDatosBancolombia;
void generarStringRespuesta(char * respuesta);
void
setearParametrosObjetoComision(JNIEnv *env, DatosComision datosComision, jobject datos_comision);

void setearDatosTarjta(DatosTarjetaAndroid datosTarjetaAndroid, JNIEnv *env, jobject datos_tarjeta);

void resultadoTransaccionBancolombia(int resultado, char *respuesta, JNIEnv *env, jobject thiz);

JNIEnv *env2;
jobject instance2;

int enviarStringToJava(char *mensaje, char *metodo) {
    env2->ExceptionClear();
    jclass jClassInstance = env2->GetObjectClass(instance2);

    jmethodID callBackJava = env2->GetMethodID(jClassInstance, metodo,
                                               "(Ljava/lang/String;)V");

    if (NULL == callBackJava) return 1;
    int result = 0;

    // Call back Java method with parameters
    env2->CallVoidMethod(instance2, callBackJava, env2->NewStringUTF(mensaje));
    //result = env2->CallCharMethod(instance2, callBackJava, contador);

    return result;
}

extern "C"
JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_echoTestJava(JNIEnv *env, jobject instance) {
    // TODO: implement echoTestJava()
    env2 = env;
    instance2 = instance;
    int iRet = 0;

    if (realizarConexionTlS() > 0) {
        iRet = echoTest();
        cerrarConexion();
    }
    jthrowable exc = env->ExceptionOccurred();

    if (exc) {
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
    return iRet;
}

extern "C"
JNIEXPORT int JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_logonJava(JNIEnv *env,jobject instance) {

    env2 = env;
    instance2 = instance;

    int respuesta = 0;

    if (realizarConexionTlS() > 0) {
        respuesta = logon3DES();
        cerrarConexion();
    }

    return respuesta;
}

extern "C"
JNIEXPORT void JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_inicializar(JNIEnv *env, jclass clazz) {
    // TODO: implement inicializar()
    strcpy(discoNetcom, internalStoragePath);
    crearDisco(discoNetcom);
    _createParameter_();
}

extern "C"
JNIEXPORT void JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_initNDK(JNIEnv *env, jclass clazz,
                                                              jstring mng) {
    // TODO: implement initNDK()
    internalStoragePath = env->GetStringUTFChars(mng, 0);
    struct stat sb;
    int32_t res = stat(internalStoragePath, &sb);
    if (0 == res && sb.st_mode && S_IFDIR) {
    } else if (ENOENT == errno) {
        res = mkdir(internalStoragePath, 0770);
    }
}

extern "C"
JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_consultarInfoParametros(JNIEnv *env,
                                                                              jclass clazz,
                                                                              jstring user,
                                                                              jstring pass) {
    const char *parametroRecibido;
    const char *parametroRecibido2;
    uChar tipoUsuario[12 + 1] = {0x00};
    uChar datosUsuario[24 + 1] = {0x00};
    uChar clave[6 + 1] = {0x00};
    int continuar = 0;
    int tipo = 0;

    char usuario[20 + 1] = {0x00};
    char password[20 + 1] = {0x00};

    parametroRecibido = (env)->GetStringUTFChars(user, JNI_FALSE);
    parametroRecibido2 = (env)->GetStringUTFChars(pass, JNI_FALSE);

    memcpy(usuario, parametroRecibido, strlen(parametroRecibido));
    memcpy(password, parametroRecibido2, strlen(parametroRecibido2));

    if (strcmp(usuario, "REDEBAN") == 0 || strcmp(usuario, "COMERCIO") == 0) {
        continuar = 1;
        strcpy(tipoUsuario, (char *) "1");
        if (continuar == 1 && strcmp(password, "123456") == 0) {
            continuar = 1;
        } else {
            continuar = 0;
        }
    } else {
        continuar = verificarUsuario(usuario, datosUsuario);
        if (continuar == EXISTE_USUARIO) {
            memcpy(clave, datosUsuario + 10, 6);
        }

        if (continuar == EXISTE_USUARIO && strcmp(password, clave) == 0) {
            memcpy(tipoUsuario, datosUsuario + 22, 1);
            tipo = atoi(tipoUsuario);
            if (tipo == 1) {
                continuar = EXISTE_USUARIO_ADMINISTRADOR;
                setParameter(USER_CNB_LOGON, (char *) "3");
            } else {
                continuar = EXISTE_USUARIO_OPERADOR;
                setParameter(USER_CNB_LOGON, (char *) "4");
            }

        } else {
            continuar = -1;
        }
    }
    return continuar;

}
extern "C"
JNIEXPORT jstring JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_obtenerLlaveWorkingFromC(JNIEnv *env,
                                                                            jclass clazz) {
    char workingKey[32 + 1] = {0x00};
    obtenerLlaveWorking(workingKey);
    LOGI("workingKey  %s",workingKey);
    return env->NewStringUTF(workingKey);
}extern "C"
JNIEXPORT jstring JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_enviarDeposito(JNIEnv *env, jobject thiz,
                                                                     jstring numero_cuenta,
                                                                     jstring valor,
                                                                     jint tipo_cuenta) {
    env2 = env;
    instance2 = thiz;

    const char *parametroRecibido;
    const char *parametroRecibido2;


    char cuenta[20 + 1] = {0x00};
    char monto[12 + 1] = {0x00};
    char linea[100 + 1] = {0x00};
    char codigoAprobacion[20 + 1] = {0x00};
    char respuesta[1024 + 1] = {0x00};
    char dataAux1[512 + 1] = {0x00};

    parametroRecibido = (env)->GetStringUTFChars(numero_cuenta, JNI_FALSE);
    parametroRecibido2 = (env)->GetStringUTFChars(valor, JNI_FALSE);

    setParameter(IP_DEFAULT,"190.24.137.204");
    setParameter(PUERTO_DEFAULT,"3007");
    setParameter(NII,"0160");
    setParameter(TIPO_CANAL,"02");

    memset(&datosVentaBancolombia, 0x00, sizeof(DatosVenta));
    memcpy(cuenta, parametroRecibido, strlen(parametroRecibido));
    memcpy(monto, parametroRecibido2, strlen(parametroRecibido2));
    int resultado = 0;

    LOGI("Entre al c y agregue los datos ");

    setParameter(HABILITACION_MODO_CNB, (char *) "4");//Temporal de android

    LOGI("Despues de set parameter habilitacion modo CNB");
    LOGI("Verificando la conexion");
    resultado = enviarDeposito(cuenta, monto, tipo_cuenta, codigoAprobacion);
    generarStringRespuesta(respuesta);
    return env->NewStringUTF(respuesta);
}


extern "C"
JNIEXPORT jstring JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_enviarPagoPoducto(JNIEnv *env, jobject thiz,
                                                                        jstring referencia_producto,
                                                                        jstring valor,
                                                                        jint tipo_producto) {
    env2 = env;
    instance2 = thiz;

    const char *parametroRecibido;
    const char *parametroRecibido2;
    int nRet = 0;

    char referencia[20 + 1] = {0x00};
    char monto[12 + 1] = {0x00};
    char respuesta[1024 + 1] = {0x00};

    parametroRecibido = (env)->GetStringUTFChars(referencia_producto, JNI_FALSE);
    parametroRecibido2 = (env)->GetStringUTFChars(valor, JNI_FALSE);

    memcpy(referencia, parametroRecibido, strlen(parametroRecibido));
    memcpy(monto, parametroRecibido2, strlen(parametroRecibido2));
    memset(&datosVentaBancolombia, 0x00, sizeof(datosVentaBancolombia));
    int resultado = 0;
    setParameter(HABILITACION_MODO_CNB, (char *) "4");//Temporal de android
    resultado = enviarTransPago(referencia, monto, tipo_producto);
    strcpy(respuesta, globalTramaVenta.codigoAprobacion);
    resultadoTransaccionBancolombia(resultado, respuesta, env, thiz);

    generarStringRespuesta(respuesta);

    return env->NewStringUTF(respuesta);
}extern "C"
JNIEXPORT jstring JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_enviarConsultaSaldoBclObj(JNIEnv *env,
                                                                                jobject thiz,
                                                                                jobject datos_tarjeta) {
    env2 = env;
    instance2 = thiz;
    int resultado = 0;

    char codigoAprobacion[20 + 1] = {0x00};
    char respuesta[1024 + 1] = {0x00};

    DatosTarjetaAndroid datosExternos;
    memset(&datosExternos, 0x00, sizeof(datosExternos));

    datosExternos = obtenerDatosTarjeta(env, datos_tarjeta);

    setParameter(HABILITACION_MODO_CNB, (char *) "4");//Temporal de android
    resultado = enviarTransaccionSaldoBCL(datosExternos, 3, codigoAprobacion);
    generarStringRespuesta(respuesta);

    limpiarTokenMessage();
    return env->NewStringUTF(respuesta);
}
extern "C"
JNIEXPORT jstring JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_enviarRetiroTarjetaBCLObj(JNIEnv *env,
                                                                                jobject thiz,
                                                                                jobject datos_tarjeta,
                                                                                jstring monto,
                                                                                jint otra_cuenta,
                                                                                jstring numero_de_cuenta) {
    env2 = env;
    instance2 = thiz;
    int resultado = 0;

    char codigoAprobacion[20 + 1] = {0x00};
    char respuesta[1024 + 1] = {0x00};

    const char *parametroMonto;
    const char *paramOtraCuenta;

    parametroMonto = (env)->GetStringUTFChars(monto, JNI_FALSE);
    paramOtraCuenta = (env)->GetStringUTFChars(numero_de_cuenta, JNI_FALSE);
    DatosTarjetaAndroid datosExternos;
    memset(&datosExternos, 0x00, sizeof(datosExternos));

    datosExternos = obtenerDatosTarjeta(env, datos_tarjeta);
    memset(&datosVentaBancolombia, 0x00, sizeof(datosVentaBancolombia));
    setParameter(HABILITACION_MODO_CNB, (char *) "4");//Temporal de android
    resultado = enviarTransaccionRetiroTarjetaBCL(datosExternos, 3, codigoAprobacion,
                                                      (char *) "10",
                                                      (char *) paramOtraCuenta,
                                                      (char *) parametroMonto,
                                                      otra_cuenta);
    generarStringRespuesta(respuesta);

    return env->NewStringUTF(respuesta);
}extern "C"
JNIEXPORT jstring JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_enviarRetiroSinTarjeta(JNIEnv *env,
                                                                             jobject thiz,
                                                                             jstring monto,
                                                                             jstring numero_cuenta,
                                                                             jstring pinBlock) {
    env2 = env;
    instance2 = thiz;
    int resultado = 0;
    const char *paramMonto;
    const char *paramCuenta;
    const char *paramPinBlock;
    char respuesta[100 + 1] = {0x00};

    paramMonto = (env)->GetStringUTFChars(monto, JNI_FALSE);
    paramCuenta = (env)->GetStringUTFChars(numero_cuenta, JNI_FALSE);
    paramPinBlock = (env)->GetStringUTFChars(pinBlock, JNI_FALSE);
    memset(&datosVentaBancolombia, 0x00, sizeof(datosVentaBancolombia));
    LOGI("ON Retiro sin tarjeta ");
    resultado = retiroEfectivo((char *) paramMonto, (char *) paramCuenta,
                                   (char *) paramPinBlock);
    generarStringRespuesta(respuesta);
    limpiarTokenMessage();
    return env->NewStringUTF(respuesta);
}extern "C"
JNIEXPORT jstring JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_enviarTransferencia(JNIEnv *env, jobject thiz,
                                                                          jobject datos_tarjeta,
                                                                          jstring tipo_cuenta_o,
                                                                          jstring tipo_cuenta_d,
                                                                          jstring cuenta_destino,
                                                                          jstring otra_cuenta,
                                                                          jstring monto) {
    env2 = env;
    instance2 = thiz;
    int resultado = 0;
    char codigoAprobacion[20 + 1] = {0x00};
    char respuesta[100 + 1] = {0x00};
    const char *paramTipo_cuenta_o;
    const char *paramtipo_cuenta_d;
    const char *paramCuentaDestino;
    const char *paramOtraCuenta;
    const char *paramMonto;

    DatosTarjetaAndroid datosExternos;
    memset(&datosExternos, 0x00, sizeof(datosExternos));

    datosExternos = obtenerDatosTarjeta(env, datos_tarjeta);

    paramMonto = (env)->GetStringUTFChars(monto, JNI_FALSE);
    paramTipo_cuenta_o = (env)->GetStringUTFChars(tipo_cuenta_o, JNI_FALSE);
    paramtipo_cuenta_d = (env)->GetStringUTFChars(tipo_cuenta_d, JNI_FALSE);
    paramCuentaDestino = (env)->GetStringUTFChars(cuenta_destino, JNI_FALSE);
    paramOtraCuenta = (env)->GetStringUTFChars(otra_cuenta, JNI_FALSE);

    setParameter(HABILITACION_MODO_CNB, (char *) "4");//Temporal de android
    memset(&datosVentaBancolombia, 0x00, sizeof(datosVentaBancolombia));
    resultado = enviarTransferencia(datosExternos, (char *) paramTipo_cuenta_o,
                                        (char *) paramtipo_cuenta_d,
                                        (char *) paramCuentaDestino, (char *) paramOtraCuenta,
                                        (char *) paramMonto);
    strcpy(codigoAprobacion, datosVentaBancolombia.codigoAprobacion);
    generarStringRespuesta(respuesta);
    limpiarTokenMessage();
    return env->NewStringUTF(respuesta);
}

DatosTarjetaAndroid obtenerDatosTarjeta(JNIEnv *env, jobject datos_tarjeta) {

    const char *parametroRecibido1;
    const char *parametroRecibido2;
    const char *parametroRecibido3;
    const char *parametroRecibido4;
    const char *parametroRecibido5;
    const char *parametroRecibido6;
    const char *parameFechaExpiracion;
    const char *paramePan;
    const char *parameCuotas;
    const char *parameTipoCuenta;
    const char *paramDocumento;
    const char *paramCvc2;

    DatosTarjetaAndroid datosLeidos;
    memset(&datosLeidos, 0x00, sizeof(datosLeidos));
    jclass datosTarjetaClass = (env)->GetObjectClass(datos_tarjeta);

    jfieldID track2 = (env)->GetFieldID(datosTarjetaClass, "track2", "Ljava/lang/String;");
    jstring track2_js = static_cast<jstring>((env)->GetObjectField(datos_tarjeta, track2));
    if (track2_js != NULL) {
        parametroRecibido1 = (env)->GetStringUTFChars(track2_js, JNI_FALSE);
        memcpy(datosLeidos.track2, (char *) parametroRecibido1, strlen(parametroRecibido1));
    }
    jfieldID aid = (env)->GetFieldID(datosTarjetaClass, "aid", "Ljava/lang/String;");
    jstring aid_js = static_cast<jstring>((env)->GetObjectField(datos_tarjeta, aid));
    if (aid_js != NULL) {
        parametroRecibido4 = (env)->GetStringUTFChars(aid_js, JNI_FALSE);
        memcpy(datosLeidos.aid, (char *) parametroRecibido4, strlen(parametroRecibido4));
    }
    jfieldID arqc = (env)->GetFieldID(datosTarjetaClass, "arqc", "Ljava/lang/String;");
    jstring arqc_js = static_cast<jstring>((env)->GetObjectField(datos_tarjeta, arqc));
    if (arqc_js != NULL) {
        parametroRecibido5 = (env)->GetStringUTFChars(arqc_js, JNI_FALSE);
        memcpy(datosLeidos.arqc, (char *) parametroRecibido5, strlen(parametroRecibido5));
    }
    jfieldID aplabel = (env)->GetFieldID(datosTarjetaClass, "aplabel", "Ljava/lang/String;");
    jstring aplabel_js = static_cast<jstring>((env)->GetObjectField(datos_tarjeta, aplabel));
    if (aplabel_js != NULL) {
        parametroRecibido6 = (env)->GetStringUTFChars(aplabel_js, JNI_FALSE);
        memcpy(datosLeidos.apLabel, (char *) parametroRecibido6, strlen(parametroRecibido6));
    }
    jfieldID tagVenta = (env)->GetFieldID(datosTarjetaClass, "tagVenta", "Ljava/lang/String;");
    jstring tagVenta_js = static_cast<jstring>((env)->GetObjectField(datos_tarjeta, tagVenta));
    if (tagVenta_js != NULL) {
        parametroRecibido2 = (env)->GetStringUTFChars(tagVenta_js, JNI_FALSE);
        memcpy(datosLeidos.data55, (char *) parametroRecibido2, strlen(parametroRecibido2));
    }
    jfieldID fechaExpiracion = (env)->GetFieldID(datosTarjetaClass, "fechaExpiracion",
                                                 "Ljava/lang/String;");
    jstring jfechaExpiracion = static_cast<jstring>((env)->GetObjectField(datos_tarjeta,
                                                                          fechaExpiracion));
    if (jfechaExpiracion != NULL) {
        parameFechaExpiracion = (env)->GetStringUTFChars(jfechaExpiracion, JNI_FALSE);
        memcpy(datosLeidos.fechaExpiracion, (char *) parameFechaExpiracion, 4);
    }
    jfieldID cuotas = (env)->GetFieldID(datosTarjetaClass, "cuotas", "Ljava/lang/String;");
    jstring jCuotas = static_cast<jstring>((env)->GetObjectField(datos_tarjeta, cuotas));
    if (jCuotas != NULL) {
        parameCuotas = (env)->GetStringUTFChars(jCuotas, JNI_FALSE);
        memcpy(datosLeidos.cuotas, (char *) parameCuotas, strlen(parameCuotas));
    }

    jfieldID pan = (env)->GetFieldID(datosTarjetaClass, "pan", "Ljava/lang/String;");
    jstring jPan = static_cast<jstring>((env)->GetObjectField(datos_tarjeta, pan));
    if (jPan != NULL) {
        paramePan = (env)->GetStringUTFChars(jPan, JNI_FALSE);
        memcpy(datosLeidos.pan, (char *) paramePan, strlen(paramePan));
    }
    jfieldID tipoCuentaId = (env)->GetFieldID(datosTarjetaClass, "tipoCuentaId",
                                              "Ljava/lang/String;");
    jstring jTipoCuentaId = static_cast<jstring>((env)->GetObjectField(datos_tarjeta,
                                                                       tipoCuentaId));
    if (jTipoCuentaId != NULL) {
        parameTipoCuenta = (env)->GetStringUTFChars(jTipoCuentaId, JNI_FALSE);
        memcpy(datosLeidos.tipoCuenta, (char *) parameTipoCuenta, strlen(parameTipoCuenta));
    }
    jfieldID documento = (env)->GetFieldID(datosTarjetaClass, "documento",
                                           "Ljava/lang/String;");
    jstring jdocumento = static_cast<jstring>((env)->GetObjectField(datos_tarjeta,
                                                                    documento));
    if (jdocumento != NULL) {
        paramDocumento = (env)->GetStringUTFChars(jdocumento, JNI_FALSE);
        memcpy(datosLeidos.documento, (char *) paramDocumento, strlen(paramDocumento));
    }
    jfieldID cvc2 = (env)->GetFieldID(datosTarjetaClass, "cvc2",
                                      "Ljava/lang/String;");
    jstring jCvc2 = static_cast<jstring>((env)->GetObjectField(datos_tarjeta,
                                                               cvc2));
    if (jCvc2 != NULL) {
        paramCvc2 = (env)->GetStringUTFChars(jCvc2, JNI_FALSE);
        memcpy(datosLeidos.cvc2, (char *) paramCvc2, strlen(paramCvc2));
    }
    jfieldID isPinAsked = (env)->GetFieldID(datosTarjetaClass, "isPinAsked", "I");
    jfieldID tamTagVenta = (env)->GetFieldID(datosTarjetaClass, "tamTagVenta", "I");


    jint isPinAsked_js = (env)->GetIntField(datos_tarjeta, isPinAsked);
    jint jtamTagVenta = (env)->GetIntField(datos_tarjeta, tamTagVenta);
    if (isPinAsked_js == 1) {

        jfieldID pinBlock = (env)->GetFieldID(datosTarjetaClass, "pinBlock", "Ljava/lang/String;");
        jstring pinBlock_js = static_cast<jstring>((env)->GetObjectField(datos_tarjeta, pinBlock));
        if (pinBlock_js != NULL) {
            parametroRecibido3 = (env)->GetStringUTFChars(pinBlock_js, JNI_FALSE);
            memcpy(datosLeidos.pinblock, (char *) parametroRecibido3, strlen(parametroRecibido3));
            setParameter(IMPRIMIR_PANEL_FIRMA, (char *) "1");
        } else {
            setParameter(IMPRIMIR_PANEL_FIRMA, (char *) "0");
        }
    }

    datosLeidos.isPin = isPinAsked_js;
    datosLeidos.sizeP55 = jtamTagVenta;
    return datosLeidos;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_enviarConsultaRecaudoLector(JNIEnv *env,
                                                                                  jobject thiz,
                                                                                  jstring codigo_leido,
                                                                                  jobject datos_recaudo) {
    env2 = env;
    instance2 = thiz;
    int resultado = 0;
    jint jIndicadorBaseDatos;
    jlong JvalorFactura;
    jint jindicadorValor;
    jstring jtipoRecaudo;
    jstring jtokenP6;

    char codigoAprobacion[100 + 1] = {0x00};
    char respuesta[1024 + 1] = {0x00};
    char cTipoRecaudo[20 + 1] = "1";
    const char *paramcodigoLeido;

    DatosRecaudoAndroid datosExternos;

    paramcodigoLeido = (env)->GetStringUTFChars(codigo_leido, JNI_FALSE);
    memset(&datosVentaBancolombia, 0x00, sizeof(datosVentaBancolombia));
    setParameter(HABILITACION_MODO_CNB, (char *) "4");//Temporal de android
        resultado = enviarConsultaRecaudoLector(&datosExternos, (char *) paramcodigoLeido,
                                                codigoAprobacion);

        if (strcmp(datosVentaBancolombia.codigoRespuesta, "00") != 0) {
            resultadoTransaccionBancolombia(resultado, codigoAprobacion, env, thiz);
        }



    jclass datosRecaudoClass = (env)->GetObjectClass(datos_recaudo);

    // Obtiene el valor de la propiedad de un objeto java desde C-JNI
    jfieldID indicadorBaseDatos = (env)->GetFieldID(datosRecaudoClass, "indicadorBaseDatos", "I");
    jfieldID valorFactura = (env)->GetFieldID(datosRecaudoClass, "valorFactura", "J");
    jfieldID indicadorValor = (env)->GetFieldID(datosRecaudoClass, "indicadorValor", "I");
    jfieldID tipoRecaudo = (env)->GetFieldID(datosRecaudoClass, "tipoRecaudo",
                                             "Ljava/lang/String;");
    jfieldID tokenP6 = (env)->GetFieldID(datosRecaudoClass, "tokenP6",
                                         "Ljava/lang/String;");
    //modificar la propieda de un objeto desde C-jni

    jIndicadorBaseDatos = datosExternos.indicadorBaseDatos;
    JvalorFactura = datosExternos.valorFactura;
    jindicadorValor = datosExternos.indicadorValor;
    jtipoRecaudo = (env)->NewStringUTF(cTipoRecaudo);
    jtokenP6 = (env)->NewStringUTF(datosExternos.tokenP6);

    (env)->SetIntField(datos_recaudo, indicadorBaseDatos, jIndicadorBaseDatos);
    (env)->SetLongField(datos_recaudo, valorFactura, JvalorFactura);
    (env)->SetObjectField(datos_recaudo, tipoRecaudo, jtipoRecaudo);
    (env)->SetIntField(datos_recaudo, indicadorValor, jindicadorValor);
    (env)->SetObjectField(datos_recaudo, tokenP6, jtokenP6);
    generarStringRespuesta(respuesta);
    std::string nameCPP(respuesta);

    return env->NewStringUTF(respuesta);
}extern "C"
JNIEXPORT jstring JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_enviarPagoRecaudoLector(JNIEnv *env,
                                                                              jobject thiz,
                                                                              jstring codigo_leido,
                                                                              jobject datos_recaudo) {
    env2 = env;
    instance2 = thiz;
    int resultado = 0;
    char codigoAprobacion[6 + 1] = {0x00};
    char respuesta[100 + 1] = {0x00};
    const char *paramTotal_venta;
    const char *paramtokenP6;
    jclass datosRecaudoClass = (env)->GetObjectClass(datos_recaudo);
    jfieldID valorApagar = (env)->GetFieldID(datosRecaudoClass, "valorApagar",
                                             "Ljava/lang/String;");
    jfieldID tokenP6 = (env)->GetFieldID(datosRecaudoClass, "tokenP6",
                                         "Ljava/lang/String;");
    jstring jValorApagar = static_cast<jstring>((env)->GetObjectField(datos_recaudo, valorApagar));
    jstring jtokenP6 = static_cast<jstring>((env)->GetObjectField(datos_recaudo, tokenP6));
    if(jValorApagar != NULL){
        paramTotal_venta = (env)->GetStringUTFChars(jValorApagar, JNI_FALSE);
    }

    if(jtokenP6 != NULL){
        paramtokenP6 = (env)->GetStringUTFChars(jtokenP6, JNI_FALSE);
    }
    DatosTokens datosToken;
    memset(&datosToken, 0x00, sizeof(datosToken));

    if(jtokenP6 != NULL){
        memcpy(datosToken.tokensp6, paramtokenP6, 160);
    }

    resultado = enviarRecaudo(datosToken, TRANSACCION_TEFF_RECAUDO_MANUAL,
                              (char *) paramTotal_venta);
    generarStringRespuesta(respuesta);
    return env->NewStringUTF(respuesta);
}extern "C"
JNIEXPORT jstring JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_enviarConsultaRecaudoManual(JNIEnv *env,
                                                                                  jobject thiz,
                                                                                  jobject datos_recaudo) {
    env2 = env;
    instance2 = thiz;

    int resultado = 0;
    char codigoAprobacion[100 + 1] = {0x00};
    char respuesta[1024 + 1] = {0x00};
    const char *paramNumeroFactura;
    const char *paramCodigoConvenio;
    const char *paramTipoRecaudo;
    const char *paramCuentaRecaudadora;

    jint jIndicadorBaseDatos;
    jlong JvalorFactura;
    jint jindicadorValor;
    jstring jtipoRecaudo;
    jstring jtokenP6;
    jstring jNombreConvenio;

    char cTipoRecaudo[20 + 1] = "2";

    DatosRecaudoAndroid datosExternos;

    memset(&datosExternos, 0x00, sizeof(datosExternos));

    jclass datosRecaudoClass = (env)->GetObjectClass(datos_recaudo);

    jfieldID numeroFactura = (env)->GetFieldID(datosRecaudoClass, "numeroFactura",
                                               "Ljava/lang/String;");
    jstring jnumeroFactura = static_cast<jstring>((env)->GetObjectField(datos_recaudo,
                                                                        numeroFactura));
    if (jnumeroFactura != NULL) {
        paramNumeroFactura = (env)->GetStringUTFChars(jnumeroFactura, JNI_FALSE);
        memcpy(datosExternos.numeroFactura, (char *) paramNumeroFactura,
               strlen(paramNumeroFactura));
    }

    jfieldID codigoConvenio = (env)->GetFieldID(datosRecaudoClass, "codigoConvenio",
                                                "Ljava/lang/String;");
    jstring jcodigoConvenio = static_cast<jstring>((env)->GetObjectField(datos_recaudo,
                                                                         codigoConvenio));
    if (jcodigoConvenio != NULL) {
        paramCodigoConvenio = (env)->GetStringUTFChars(jcodigoConvenio, JNI_FALSE);
        memcpy(datosExternos.codigoConvenio, (char *) paramCodigoConvenio,
               strlen(paramCodigoConvenio));
    }
    jfieldID tipoRecaudo = (env)->GetFieldID(datosRecaudoClass, "tipoRecaudo",
                                             "Ljava/lang/String;");
    jstring jTipoRecaudo = static_cast<jstring>((env)->GetObjectField(datos_recaudo,
                                                                      tipoRecaudo));
    if (jTipoRecaudo != NULL) {
        paramTipoRecaudo = (env)->GetStringUTFChars(jTipoRecaudo, JNI_FALSE);
        memcpy(datosExternos.tipoRecaudo, (char *) paramTipoRecaudo, strlen(paramTipoRecaudo));
    }
    jfieldID cuentaRecaudadora = (env)->GetFieldID(datosRecaudoClass, "cuentaRecaudadora",
                                                   "Ljava/lang/String;");
    jstring jCuentaRecaudadora = static_cast<jstring>((env)->GetObjectField(datos_recaudo,
                                                                            cuentaRecaudadora));
    if (jCuentaRecaudadora != NULL) {
        paramCuentaRecaudadora = (env)->GetStringUTFChars(jCuentaRecaudadora, JNI_FALSE);
        memcpy(datosExternos.cuentaRecaudadora, (char *) paramCuentaRecaudadora,
               strlen(paramCuentaRecaudadora));
    }
    memset(&datosVentaBancolombia, 0x00, sizeof(datosVentaBancolombia));

        resultado = enviarConsultaRecaudoManual(&datosExternos, codigoAprobacion);
        if (resultado < 0) {
            resultadoTransaccionBancolombia(resultado, codigoAprobacion, env, thiz);
            return env->NewStringUTF(codigoAprobacion);
        }
        if (strcmp(datosVentaBancolombia.codigoRespuesta, "00") != 0 && resultado >= 0) {
            resultadoTransaccionBancolombia(resultado, codigoAprobacion, env, thiz);
            cerrarConexion();
        }

    // Obtiene el valor de la propiedad de un objeto java desde C-JNI
    jfieldID indicadorBaseDatos = (env)->GetFieldID(datosRecaudoClass, "indicadorBaseDatos", "I");
    jfieldID valorFactura = (env)->GetFieldID(datosRecaudoClass, "valorFactura", "J");
    jfieldID indicadorValor = (env)->GetFieldID(datosRecaudoClass, "indicadorValor", "I");

    jfieldID tokenP6 = (env)->GetFieldID(datosRecaudoClass, "tokenP6",
                                         "Ljava/lang/String;");
    jfieldID nombreConvenio = (env)->GetFieldID(datosRecaudoClass, "nombreConvenio",
                                                "Ljava/lang/String;");
    //modificar la propieda de un objeto desde C-jni

    jIndicadorBaseDatos = datosExternos.indicadorBaseDatos;
    JvalorFactura = datosExternos.valorFactura;
    jindicadorValor = datosExternos.indicadorValor;
    jtipoRecaudo = (env)->NewStringUTF(cTipoRecaudo);
    jtokenP6 = (env)->NewStringUTF(datosExternos.tokenP6);
    jNombreConvenio = (env)->NewStringUTF(datosExternos.nombreConvenio);
    (env)->SetIntField(datos_recaudo, indicadorBaseDatos, jIndicadorBaseDatos);
    (env)->SetLongField(datos_recaudo, valorFactura, JvalorFactura);
    (env)->SetObjectField(datos_recaudo, tipoRecaudo, jtipoRecaudo);
    (env)->SetObjectField(datos_recaudo, tokenP6, jtokenP6);
    (env)->SetObjectField(datos_recaudo, nombreConvenio, jNombreConvenio);
    (env)->SetIntField(datos_recaudo, indicadorValor, jindicadorValor);
    generarStringRespuesta(respuesta);
    std::string nameCPP(codigoAprobacion);

    return env->NewStringUTF(respuesta);
}

void _guardarDirectorioJournals_(short tipoJournal, uChar *numeroRecibo, uChar *estadoTransaccion) {

    DatosJournals datosJournals;
    int tamDatosJournals = 0;
    long posicionTransaccion = 0;
    long posicionUltima = 0;
    long iRet = 0;
    tamDatosJournals = sizeof(DatosJournals);
    boolean anteriorActiva = 1;
    memset(&datosJournals, 0x00, sizeof(datosJournals));
    posicionUltima = tamArchivo(discoNetcom, (char *) DIRECTORIO_JOURNALS) - tamDatosJournals;

    iRet = buscarArchivo(discoNetcom, (char *) DIRECTORIO_JOURNALS, (char *) &datosJournals,
                         posicionUltima, tamDatosJournals);
    if (iRet > 0) {
        anteriorActiva = atoi(datosJournals.estadoTransaccion);
    }

    memset(&datosJournals, 0x00, sizeof(datosJournals));

    switch (tipoJournal) {
        case TRANS_COMERCIO:
            posicionTransaccion = tamArchivo(discoNetcom, (char *) JOURNAL) - TAM_JOURNAL_COMERCIO;
            break;
        default:
            break;
    }

    datosJournals.journal = tipoJournal;
    datosJournals.posicionTransaccion = posicionTransaccion;
    memcpy(datosJournals.numeroRecibo, numeroRecibo, TAM_NUMERO_RECIBO);
    memcpy(datosJournals.estadoTransaccion, estadoTransaccion, 1);

    if (anteriorActiva == 1) {
        escribirArchivo(discoNetcom, (char *) DIRECTORIO_JOURNALS, (char *) &datosJournals,
                        tamDatosJournals);
    } else {
        actualizarArchivo(discoNetcom, (char *) DIRECTORIO_JOURNALS, (char *) &datosJournals,
                          posicionUltima, tamDatosJournals);
    }
}

void imprimirReciboTicket(void) {
    jclass jClassInstance = env2->GetObjectClass(instance2);

    jmethodID callBackJava = env2->GetMethodID(jClassInstance, "imprimir",
                                               "()V");

    if (NULL == callBackJava) return;
    // Call back Java method with parameters
    env2->CallVoidMethod(instance2, callBackJava);
}

void pprintf(char *linea) {
    jclass jClassInstance = env2->GetObjectClass(instance2);

    jmethodID callBackJava = env2->GetMethodID(jClassInstance, "imprimirLinea",
                                               "(Ljava/lang/String;)V");

    if (NULL == callBackJava) return;
    // Call back Java method with parameters
    env2->CallVoidMethod(instance2, callBackJava, env2->NewStringUTF(linea));
}

extern "C"
JNIEXPORT jstring JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_inicializarTerminal(JNIEnv *env,
                                                                          jobject thiz, jstring tipo_inicializacion) {
    env2 = env;
    instance2 = thiz;

    int resultado = 0;
    char respuesta[50 + 1];
    char dataAux[20 + 1];

    const char *paramTipoInizializacion;
    paramTipoInizializacion = (env)->GetStringUTFChars(tipo_inicializacion, JNI_FALSE);

    strcpy(discoNetcom, internalStoragePath);
    crearDisco(discoNetcom);
    _createParameter_();

    setParameter(CONSECUTIVO, (char *) "000003");
    if (realizarConexionTlS() > 0) {
        setParameter(TIPO_TELECARGA, "1");
        resultado = invocarInicializacionGeneral((char *) paramTipoInizializacion);
        cerrarConexion();
        if (resultado > 0) {
            memset(dataAux, 0x00, sizeof(dataAux));
            getParameter(IP_VENTA_PRIMARIA, dataAux);
            setParameter(IP_DEFAULT, dataAux);

            memset(dataAux, 0x00, sizeof(dataAux));
            getParameter(PUERTO_VENTA_PRIMARIA, dataAux);
            setParameter(PUERTO_DEFAULT, dataAux);
            strcpy(respuesta, "INICIALIZACION EXITOSA");
        } else {
            strcpy(respuesta, "ERROR EN INICIALIZACION");
        }
    } else {
        strcpy(respuesta, "ERROR SIN CONEXION");
    }
    setParameter(TIPO_TELECARGA, (char *)"0");
    return env->NewStringUTF(respuesta);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_inicializarParametros(JNIEnv *env,
                                                                            jobject thiz) {
    env2 = env;
    instance2 = thiz;
    int resultado = 0;
    char respuesta[100 + 1] = {0x00};
    if (realizarConexionTlS() > 0) {
        resultado = inicializacionParametros();
        if (resultado > 0) {
            strcpy(respuesta, "INICIALIZACION OK");
        } else {
            strcpy(respuesta, "ERROR INICIALIZACION");
        }
    } else {
        cerrarConexion();
        strcpy(respuesta, "ERROR INICIALIZACION");
    }
    return env->NewStringUTF(respuesta);
}
extern "C"

JNIEXPORT jstring JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_inicializarSerial(JNIEnv *env,
                                                                        jobject thiz) {
    env2 = env;
    instance2 = thiz;
    char codigoRespuesta[20 + 1] = {0x00};
    if (realizarConexionTlS() > 0) {
        setParameter(TIPO_TELECARGA, "1");
        _inicializacionSerial_();
        setParameter(TIPO_TELECARGA, "0");
        cerrarConexion();
        strcpy(codigoRespuesta, "SerialOK");
    } else {
        strcpy(codigoRespuesta, "ERROR SIN CONEXION");
    }


    return env->NewStringUTF(codigoRespuesta);
}extern "C"

JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_cerrarSesion(JNIEnv *env,
                                                                   jobject thiz) {
    env2 = env;
    instance2 = thiz;
    int iRet = 0;

    iRet = cerrarSesion();

    return iRet;
}extern "C"

JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_crearUsuarioCnb(JNIEnv *env, jobject thiz,
                                                                      jstring usuario,
                                                                      jstring clave,
                                                                      jstring tipo_user) {
    const char *paramUsuario;
    const char *paramClave;
    const char *paramTipoUser;
    int iRet = 0;

    paramUsuario = (env)->GetStringUTFChars(usuario, JNI_FALSE);
    paramClave = (env)->GetStringUTFChars(clave, JNI_FALSE);
    paramTipoUser = (env)->GetStringUTFChars(tipo_user, JNI_FALSE);

    iRet = crearUsuarioCnb((char *) paramUsuario, (char *) paramClave, (char *) paramTipoUser);

    return iRet;

}extern "C"
JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_capturarClavesInicializacion(JNIEnv *env,
                                                                                   jclass thiz,
                                                                                   jstring usuario,
                                                                                   jstring clave) {
    const char *paramUsuario;
    const char *paramClave;
    char claveNativa[12 + 1] = {0x00};
    int codigoRespuesta = 0;

    paramUsuario = (env)->GetStringUTFChars(usuario, JNI_FALSE);
    paramClave = (env)->GetStringUTFChars(clave, JNI_FALSE);

    if (strcmp(paramUsuario, "admin") == 0) {
        getParameter(VALOR_CLAVE_ADMINISTRADOR, claveNativa);
    } else if (strcmp(paramUsuario, "supervisor") == 0) {
        getParameter(VALOR_CLAVE_SUPERVISOR, claveNativa);
    }

    codigoRespuesta = capturarClave(claveNativa, (char *) paramClave);

    return codigoRespuesta;
}

extern "C"
JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_enviarCierreCNB(JNIEnv *env, jobject thiz,

                                                                      jint tipo_cierre) {
    env2 = env;
    instance2 = thiz;
    int iRet = 1;
    if (tipo_cierre == CNB_CIERRE_TOTAL) {
        iRet = realizarConexionTlS();
    }
    if (iRet > 0) {
        iRet = cierreCnbBancolombia(tipo_cierre);
    }
    if (tipo_cierre == CNB_CIERRE_TOTAL) {
        cerrarConexion();
    }
    return iRet;
}

extern "C"
JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_solicitarDuplicadoCNBBCL(JNIEnv *env,
                                                                               jobject thiz,
                                                                               jint opcion_recibo,
                                                                               jint tipo_recibo,
                                                                               jstring numero_recibo) {

    env2 = env;
    instance2 = thiz;
    int iRet = 0;
    const char *paramNumeroRecibo;
    paramNumeroRecibo = (env)->GetStringUTFChars(numero_recibo, JNI_FALSE);

    iRet = duplicadoCnbBancolombia(opcion_recibo, tipo_recibo, (char *) paramNumeroRecibo);
    return iRet;

}extern "C"
JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_codigoTerminalCnb(JNIEnv *env,
                                                                        jobject thiz,
                                                                        jstring numero_terminal) {

    env2 = env;
    instance2 = thiz;
    int iRet = 0;
    const char *paramNumeroTerminal;

    paramNumeroTerminal = (env)->GetStringUTFChars(numero_terminal, JNI_FALSE);

    iRet = codigoTerminalCnb((char *) paramNumeroTerminal);

    return iRet;

}extern "C"
JNIEXPORT jboolean JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_guardarParametrosTerminal(JNIEnv *env,
                                                                                jobject thiz,
                                                                                jstring valor_nii,
                                                                                jstring valor_nii_multipos,
                                                                                jstring valor_nii_celucompra,
                                                                                jstring valor_nii_dcc,
                                                                                jstring valor_numero_terminal,
                                                                                jstring valor_id_bonos,
                                                                                jstring valor_numero_placa) {
    env2 = env;
    instance2 = thiz;


    const char *paramNii;
    const char *paramNiiMultipos;
    const char *paramNiiCelucompra;
    const char *paramNiiDCC;
    const char *paramNumeroTerminal;
    const char *paramiDBonos;
    const char *paramNumeroPlaca;

    int respuesta = 1;

    paramNii = (env)->GetStringUTFChars(valor_nii, JNI_FALSE);
    paramNiiMultipos = (env)->GetStringUTFChars(valor_nii_multipos, JNI_FALSE);
    paramNiiCelucompra = (env)->GetStringUTFChars(valor_nii_celucompra, JNI_FALSE);
    paramNiiDCC = (env)->GetStringUTFChars(valor_nii_dcc, JNI_FALSE);
    paramNumeroTerminal = (env)->GetStringUTFChars(valor_numero_terminal, JNI_FALSE);
    paramiDBonos = (env)->GetStringUTFChars(valor_id_bonos, JNI_FALSE);
    paramNumeroPlaca = (env)->GetStringUTFChars(valor_numero_placa, JNI_FALSE);

    setParameter(NII, (char *) paramNii);
    setParameter(NII_MULTIPOS, (char *) paramNiiMultipos);
    setParameter(NII_CELUCOMPRA, (char *) paramNiiCelucompra);
    setParameter(NII_DCC, (char *) paramNiiDCC);
    respuesta = verificarCambioTerminal((char *) paramNumeroTerminal);
    setParameter(ID_BONOS, (char *) paramiDBonos);
    setParameter(NUMERO_PLACA, (char *) paramNumeroPlaca);

    if (respuesta <= 0) {
        return (jboolean) false;
    }
    return (jboolean) true;

}extern "C"

JNIEXPORT jboolean JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_guardarParametrosComunicacionIpPuerto(
        JNIEnv *env,
        jobject thiz,
        jstring valor_ip,
        jstring valor_puerto,
        jstring tipo_canal) {


    const char *paramIp;
    const char *paramPuerto;
    const char *tipoCanal;
    paramIp = (env)->GetStringUTFChars(valor_ip, JNI_FALSE);
    paramPuerto = (env)->GetStringUTFChars(valor_puerto, JNI_FALSE);
    tipoCanal = (env)->GetStringUTFChars(tipo_canal, JNI_FALSE);

    setParameter(IP_DEFAULT, (char *) paramIp);
    setParameter(PUERTO_DEFAULT, (char *) paramPuerto);
    setParameter(TIPO_CANAL, (char *) tipoCanal);
    return (jboolean) true;
}extern "C"

JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_guardarParametrosPanVirtual(JNIEnv *env,
                                                                                  jobject thiz,
                                                                                  jstring valor_codigoBanco,
                                                                                  jstring valor_numeroTarjeta,
                                                                                  jstring tipo_cuenta) {


    const char *paramCodigoBanco;
    const char *paramNumeroTarjeta;
    const char *tipoCuenta;
    int iRet = 0;

    paramCodigoBanco = (env)->GetStringUTFChars(valor_codigoBanco, JNI_FALSE);
    paramNumeroTarjeta = (env)->GetStringUTFChars(valor_numeroTarjeta, JNI_FALSE);
    tipoCuenta = (env)->GetStringUTFChars(tipo_cuenta, JNI_FALSE);

    iRet = habilitarCNBconPAN((char *) paramCodigoBanco, (char *) paramNumeroTarjeta,
                              (char *) tipoCuenta);

    return iRet;
}extern "C"

JNIEXPORT jstring JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_getParameterFromC(JNIEnv *env, jobject thiz,
                                                                        jint clave) {

    uChar valor[80 + 1] = {0x00};

    getParameter(clave, valor);

    return env->NewStringUTF(valor);
}extern "C"

JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_validarcapturaImpuestoAutomatico(JNIEnv *env,
                                                                                       jobject thiz,
                                                                                       jstring valor_capturado,
                                                                                       jint tipoImpuesto) {

    int iRet = 0;
    const char *paramCapturado;

    paramCapturado = (env)->GetStringUTFChars(valor_capturado, JNI_FALSE);

    iRet = validarcapturaImpuestoAutomatico((char *) paramCapturado, tipoImpuesto);

    return iRet;
}extern "C"

JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_validarcapturaImpuestoMultiple(JNIEnv *env,
                                                                                     jobject thiz,
                                                                                     jstring valor_primer_impuesto,
                                                                                     jstring valor_segundo_impuesto,
                                                                                     jint tipoImpuesto) {

    int iRet = 0;
    const char *paramPrimerImpuesto;
    const char *paramSegundoImpuesto;

    paramPrimerImpuesto = (env)->GetStringUTFChars(valor_primer_impuesto, JNI_FALSE);
    paramSegundoImpuesto = (env)->GetStringUTFChars(valor_segundo_impuesto, JNI_FALSE);

    iRet = solicitarImpuestosMultiples((char *) paramPrimerImpuesto, (char *) paramSegundoImpuesto,
                                       tipoImpuesto);

    return iRet;
}extern "C"

JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_capturarImpuestoAutomatico(JNIEnv *env,
                                                                                 jobject thiz,
                                                                                 jint tipoImpuesto) {

    int iRet = 0;

    iRet = capturarImpuestoAutomatico(tipoImpuesto);

    return iRet;
}extern "C"

JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_capturarImpuestoMultiple(JNIEnv *env,
                                                                               jobject thiz,
                                                                               jint tipoImpuesto) {

    int iRet = 0;

    iRet = capturarImpuestoMultiple(tipoImpuesto);

    return iRet;
}extern "C"

JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_setearIvaAutomatico(JNIEnv *env, jobject thiz,
                                                                          jstring valor) {

    int iRet = 0;
    const char *paramValor;

    paramValor = (env)->GetStringUTFChars(valor, JNI_FALSE);
    setParameter(IVA_AUTOMATICO, (char *) paramValor);

    return iRet;
}extern "C"

JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_setearIvaMultiple(JNIEnv *env, jobject thiz,
                                                                        jstring valor) {

    int iRet = 0;
    const char *paramValor;

    paramValor = (env)->GetStringUTFChars(valor, JNI_FALSE);
    setParameter(IVA_MULTIPLE, (char *) paramValor);

    return iRet;
}extern "C"

JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_setearIncAutomatico(JNIEnv *env, jobject thiz,
                                                                          jstring valor) {

    int iRet = 0;
    const char *paramValor;

    paramValor = (env)->GetStringUTFChars(valor, JNI_FALSE);
    setParameter(INC_AUTOMATICO, (char *) paramValor);

    return iRet;
}extern "C"

JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_setearIncMultiple(JNIEnv *env, jobject thiz,
                                                                        jstring valor) {

    int iRet = 0;
    const char *paramValor;

    paramValor = (env)->GetStringUTFChars(valor, JNI_FALSE);
    setParameter(INC_MULTIPLE, (char *) paramValor);

    return iRet;
}extern "C"

JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_setearCalculoBaseDevolucion(JNIEnv *env,
                                                                                  jobject thiz,
                                                                                  jstring valor) {

    int iRet = 0;
    const char *paramValor;

    paramValor = (env)->GetStringUTFChars(valor, JNI_FALSE);
    setParameter(CALCULAR_BASE, (char *) paramValor);

    return iRet;
}extern "C"

JNIEXPORT jstring JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_getParameterFromEfectivo(JNIEnv *env,
                                                                               jobject thiz,
                                                                               jint clave) {

    uChar valor[1 + 1] = {0x00};
    getParameter(clave, valor);

    return env->NewStringUTF(valor);
}extern "C"
JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_borrarTransacciones(JNIEnv *env,
                                                                          jobject thiz) {
    int respuesta = 0;

    respuesta = borrartransacciones();

    return respuesta;
}extern "C"
JNIEXPORT jstring JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_borrarParametros(JNIEnv *env, jobject thiz) {
    char codigoRespuesta[50 + 1] = {0x00};
    borrarParametros(codigoRespuesta);
    return env->NewStringUTF(codigoRespuesta);
}
extern "C"
JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_deshabilitarCNB(JNIEnv *env, jobject thiz) {
    int codigoRespuesta = 0;
    codigoRespuesta = deshabilitarCNB();
    return codigoRespuesta;
}
extern "C"
JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_habilitarCNBConCtaSembrada(JNIEnv *env,
                                                                                 jobject thiz,
                                                                                 jstring codigo_Banco,
                                                                                 jstring tipo_Cuenta,
                                                                                 jstring track2,
                                                                                 jstring nombre_tarjeta_habiente) {

    int codigoRespuesta = 0;
    const char *paramCodigoBanco;
    const char *paramTipoCuenta;
    const char *paramTrack2;
    const char *paramNombreTarjetaHabiente;

    paramCodigoBanco = (env)->GetStringUTFChars(codigo_Banco, JNI_FALSE);
    paramTipoCuenta = (env)->GetStringUTFChars(tipo_Cuenta, JNI_FALSE);
    paramTrack2 = (env)->GetStringUTFChars(track2, JNI_FALSE);
    paramNombreTarjetaHabiente = (env)->GetStringUTFChars(nombre_tarjeta_habiente, JNI_FALSE);

    codigoRespuesta = habilitarCNBConCtaSembrada((char *) paramCodigoBanco,
                                                 (char *) paramTipoCuenta, (char *) paramTrack2,
                                                 (char *) paramNombreTarjetaHabiente);

    return codigoRespuesta;
}
extern "C"
JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_verificarComisionEnvioGiro(JNIEnv *env,
                                                                                 jobject thiz,
                                                                                 jstring monto,
                                                                                 jobject datos_comision) {
    env2 = env;
    instance2 = thiz;
    int numeroItems = 0;
    const char *paramMonto;
    DatosComision datosComision;


    paramMonto = (env)->GetStringUTFChars(monto, JNI_FALSE);
    memset(&datosComision, 0x00, sizeof(datosComision));

    LOGI("monto  %s",paramMonto);
    datosComision = verificarComision((char *) paramMonto);
    LOGI("Despues datos comision");

    numeroItems = verificarNumeroItemsDocumentos();
    LOGI("Despues numeroItems ");

    setearParametrosObjetoComision(env, datosComision, datos_comision);
    LOGI("cARGANDO linea 1413 ");
    return numeroItems;
}
extern "C"
JNIEXPORT jstring JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_obtenerMenuYparametrosDocumentosGiros(
        JNIEnv *env, jobject thiz, jobject datos_archivo_par, jint items, jint indice) {

    DatosArchivoPar datosArchivoPar;
    memset(&datosArchivoPar, 0x00, sizeof(datosArchivoPar));
    datosArchivoPar = menuTiposDocumento(items, indice);

    jclass datosArchivoClass = (env)->GetObjectClass(datos_archivo_par);

    jfieldID tipoDocumento = (env)->GetFieldID(datosArchivoClass, "tipoDocumento",
                                               "Ljava/lang/String;");
    jfieldID descripcion = (env)->GetFieldID(datosArchivoClass, "descripcion",
                                             "Ljava/lang/String;");
    jfieldID longitudMinima = (env)->GetFieldID(datosArchivoClass, "longitudMinima",
                                                "Ljava/lang/String;");
    jfieldID longitudMaxima = (env)->GetFieldID(datosArchivoClass, "longitudMaxima",
                                                "Ljava/lang/String;");
    jfieldID tipoIngreso = (env)->GetFieldID(datosArchivoClass, "tipoIngreso",
                                             "Ljava/lang/String;");

    (env)->SetObjectField(datos_archivo_par, tipoDocumento,
                          env->NewStringUTF(datosArchivoPar.tipoDocumento));
    (env)->SetObjectField(datos_archivo_par, descripcion,
                          env->NewStringUTF(datosArchivoPar.descripcion));
    (env)->SetObjectField(datos_archivo_par, longitudMinima,
                          env->NewStringUTF(datosArchivoPar.longitudMinima));
    (env)->SetObjectField(datos_archivo_par, longitudMaxima,
                          env->NewStringUTF(datosArchivoPar.longitudMaxima));
    (env)->SetObjectField(datos_archivo_par, tipoIngreso,
                          env->NewStringUTF(datosArchivoPar.tipoIngreso));


    return env->NewStringUTF("OK");
}extern "C"
JNIEXPORT jstring JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_realizarEnvioGiroCnb(JNIEnv *env,
                                                                           jobject thiz,
                                                                           jobject comision,
                                                                           jstring tpd_girador,
                                                                           jstring tpd_beneficiario,
                                                                           jstring dgirador,
                                                                           jstring d_beneficiario,
                                                                           jstring monto,
                                                                           jstring cel_girador,
                                                                           jstring cel_beneficiario) {
    env2 = env;
    instance2 = thiz;
    int resultado = 0;
    long valor = 0;
    const char *paramTpd_girador;
    const char *paramTpd_beneficiario;
    const char *paramDGirador;
    const char *paramDbeneficiario;
    const char *paramMonto;
    const char *paramCelularGirador;
    const char *paramCelularBeneficiario;

    char codigoAprobacion[20 + 1] = {0x00};
    char respuesta[1024 + 1] = {0x00};
    DatosComision datosComision;
    memset(&datosComision, 0x00, sizeof(datosComision));
    datosComision = obtenerDatosComision(env, comision);

    paramMonto = (env)->GetStringUTFChars(monto, JNI_FALSE);
    paramTpd_girador = (env)->GetStringUTFChars(tpd_girador, JNI_FALSE);
    paramTpd_beneficiario = (env)->GetStringUTFChars(tpd_beneficiario, JNI_FALSE);
    paramDGirador = (env)->GetStringUTFChars(dgirador, JNI_FALSE);
    paramDbeneficiario = (env)->GetStringUTFChars(d_beneficiario, JNI_FALSE);
    paramCelularGirador = (env)->GetStringUTFChars(cel_girador, JNI_FALSE);
    paramCelularBeneficiario = (env)->GetStringUTFChars(cel_beneficiario, JNI_FALSE);
    //memset(&datosVentaBancolombia, 0x00, sizeof(datosVentaBancolombia));
    memcpy(datosVentaBancolombia.aux1, (char *) paramDGirador, strlen((char *) paramDGirador));
    memcpy(datosVentaBancolombia.aux1 + 15, (char *) paramDbeneficiario,
           strlen((char *) paramDbeneficiario));
    memcpy(datosVentaBancolombia.totalVenta, (char *) paramMonto, strlen((char *) paramMonto));
    memcpy(datosVentaBancolombia.facturaCaja, (char *) paramCelularGirador,
           strlen((char *) paramCelularGirador));
    memcpy(datosVentaBancolombia.appLabel, (char *) paramCelularBeneficiario,
           strlen((char *) paramCelularBeneficiario));
    valor = atoi(datosComision.valorComision) /*- atoi(datosComision.valorIvaComision)*/;
    sprintf(datosVentaBancolombia.propina, "%ld", valor);
    //memset(datosComision.valorComision, 0x00, sizeof(datosComision.valorComision));
    //sprintf(datosComision.valorComision, "%ld", valor);
        resultado = realizarEnvioGiro((char *) paramTpd_girador, (char *) paramTpd_beneficiario,
                                      datosComision);
        strcpy(codigoAprobacion, datosVentaBancolombia.codigoAprobacion);
    generarStringRespuesta(respuesta);

    limpiarTokenMessage();
    return env->NewStringUTF(respuesta);
}extern "C"
JNIEXPORT jstring JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_realizarReclamacionGiro(JNIEnv *env,
                                                                              jobject thiz,
                                                                              jstring referenciaReclamacion,
                                                                              jstring tpd_beneficiario,
                                                                              jstring d_beneficiario,
                                                                              jstring monto) {
    env2 = env;
    instance2 = thiz;
    int resultado = 0;
    const char *paramTpd_beneficiario;
    const char *paramDbeneficiario;
    const char *paramMonto;
    const char *paramReferenciaReclamacion;
    char codigoAprobacion[20 + 1] = {0x00};
    char respuesta[1024 + 1] = {0x00};
    uChar tipoDocumento[2 + 1] = {0x00};

    DatosComision datosComision;
    memset(&datosComision, 0x00, sizeof(datosComision));

    paramMonto = (env)->GetStringUTFChars(monto, JNI_FALSE);
    paramTpd_beneficiario = (env)->GetStringUTFChars(tpd_beneficiario, JNI_FALSE);
    paramDbeneficiario = (env)->GetStringUTFChars(d_beneficiario, JNI_FALSE);

    paramReferenciaReclamacion = (env)->GetStringUTFChars(referenciaReclamacion, JNI_FALSE);

    memcpy(datosComision.referenciaReclamacion, (char *) paramReferenciaReclamacion,
           strlen((char *) paramReferenciaReclamacion));
    memset(&datosVentaBancolombia, 0x00, sizeof(datosVentaBancolombia));
    memcpy(datosVentaBancolombia.aux1 + 15, (char *) paramDbeneficiario,
           strlen((char *) paramDbeneficiario));
    memcpy(datosVentaBancolombia.totalVenta, (char *) paramMonto, strlen((char *) paramMonto));

        memcpy(tipoDocumento, "00", 2);
        resultado = reclamacionGiro(tipoDocumento, (char *) paramTpd_beneficiario, datosComision);
        strcpy(codigoAprobacion, datosVentaBancolombia.codigoAprobacion);
    generarStringRespuesta(respuesta);

    limpiarTokenMessage();
    return env->NewStringUTF(respuesta);
}extern "C"
JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_verificarItemsDocumento(JNIEnv *env,
                                                                              jobject thiz) {
    jint resultado = verificarNumeroItemsDocumentos();
    return resultado;
}extern "C"
JNIEXPORT jstring JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_realizaConsultaCancelacionGiroCnb(JNIEnv *env,
                                                                                        jobject thiz,
                                                                                        jobject datos_comision,
                                                                                        jstring tpd_girador,
                                                                                        jstring tpd_beneficiario,
                                                                                        jstring dgirador,
                                                                                        jstring d_beneficiario,
                                                                                        jstring referencia_cancelacion) {
    env2 = env;
    instance2 = thiz;
    int resultado = 0;
    const char *paramTpd_girador;
    const char *paramTpd_beneficiario;
    const char *paramDbeneficiario;
    const char *paramDgirador;
    const char *paramRerencia_cancelacion;

    uChar tipoDocumento[2 + 1] = {0x00};
    uChar valorAPAgar[12 + 1] = {0x00};
    uChar respuesta[1024 + 1] = {0x00};
    char valorDevolucion[50 + 1] = {0x00};
    DatosComision datosComision;
    memset(&datosComision, 0x00, sizeof(datosComision));
    memset(&datosVentaBancolombia, 0x00, sizeof(datosVentaBancolombia));

    paramTpd_girador = (env)->GetStringUTFChars(tpd_girador, JNI_FALSE);
    paramTpd_beneficiario = (env)->GetStringUTFChars(tpd_beneficiario, JNI_FALSE);
    paramDbeneficiario = (env)->GetStringUTFChars(d_beneficiario, JNI_FALSE);
    paramDgirador = (env)->GetStringUTFChars(dgirador, JNI_FALSE);
    paramRerencia_cancelacion = (env)->GetStringUTFChars(referencia_cancelacion, JNI_FALSE);

    memcpy(datosComision.referenciaCancelacion, (char *) paramRerencia_cancelacion,
           strlen((char *) paramRerencia_cancelacion));

    memcpy(datosVentaBancolombia.aux1, (char *) paramDgirador, strlen((char *) paramDgirador));
    memcpy(datosVentaBancolombia.aux1 + 15, (char *) paramDbeneficiario,
           strlen((char *) paramDbeneficiario));

        resultado = enviarConsultaCancelacionGiro(&datosComision, (char *) paramTpd_girador,
                                                  (char *) paramTpd_beneficiario, valorDevolucion);



    generarStringRespuesta(respuesta);
    setearParametrosObjetoComision(env, datosComision, datos_comision);
    if (resultado > 0) {
        strcpy(respuesta, "00;");
        strcat(respuesta, valorDevolucion);
        return env->NewStringUTF(respuesta);
    }
    return env->NewStringUTF(valorAPAgar);
}extern "C"
JNIEXPORT jstring JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_realizarCancelacionGiro(JNIEnv *env,
                                                                              jobject thiz,
                                                                              jobject comision,
                                                                              jstring tpd_girador,
                                                                              jstring tpd_beneficiario,
                                                                              jstring dgirador,
                                                                              jstring d_beneficiario
) {
    env2 = env;
    instance2 = thiz;
    int resultado = 0;
    const char *paramTpd_girador;
    const char *paramTpd_beneficiario;
    const char *paramDbeneficiario;
    const char *paramDgirador;
    const char *paramMonto;
    char codigoAprobacion[20 + 1] = {0x00};
    char auxiliarAprobacion[6 + 1] = {0x00};
    char respuesta[1024 + 1] = {0x00};
    paramTpd_girador = (env)->GetStringUTFChars(tpd_girador, JNI_FALSE);
    paramTpd_beneficiario = (env)->GetStringUTFChars(tpd_beneficiario, JNI_FALSE);
    paramDbeneficiario = (env)->GetStringUTFChars(d_beneficiario, JNI_FALSE);
    paramDgirador = (env)->GetStringUTFChars(dgirador, JNI_FALSE);

    DatosComision datosComision;
    memset(&datosComision, 0x00, sizeof(datosComision));
    datosComision = obtenerDatosComision(env, comision);

    memcpy(datosVentaBancolombia.aux1, (char *) paramDgirador, strlen((char *) paramDgirador));
    memcpy(datosVentaBancolombia.aux1 + 15, (char *) paramDbeneficiario,
           strlen((char *) paramDbeneficiario));
    resultado = enviarCancelacionGiro((char *) paramTpd_girador, (char *) paramTpd_beneficiario,
                                      datosComision);
    generarStringRespuesta(respuesta);
    limpiarTokenMessage();

    return env->NewStringUTF(respuesta);
}

DatosComision obtenerDatosComision(JNIEnv *env, jobject datos_comision) {

    const char *paramMontoMinimo;
    const char *paramMontoMaximo;
    const char *paramComisionFija;
    const char *paramPorcentajeComision;
    const char *paramPorcentajeIva;
    const char *paramFechaInicio;
    const char *paramValorComision;
    const char *paramValorIvaComision;
    const char *paramReferenciaCancelacion;
    const char *paramReferenciaReclamacion;

    DatosComision datosComision;

    memset(&datosComision, 0x00, sizeof(datosComision));
    jclass datosComisionClass = (env)->GetObjectClass(datos_comision);

    jfieldID montoMinimo = (env)->GetFieldID(datosComisionClass, "montoMinimo",
                                             "Ljava/lang/String;");
    jfieldID montoMaximo = (env)->GetFieldID(datosComisionClass, "montoMaximo",
                                             "Ljava/lang/String;");
    jfieldID comisionFija = (env)->GetFieldID(datosComisionClass, "comisionFija",
                                              "Ljava/lang/String;");
    jfieldID porcentajeComision = (env)->GetFieldID(datosComisionClass, "porcentajeComision",
                                                    "Ljava/lang/String;");
    jfieldID porcentajeIva = (env)->GetFieldID(datosComisionClass, "porcentajeIva",
                                               "Ljava/lang/String;");
    jfieldID fechaInicio = (env)->GetFieldID(datosComisionClass, "fechaInicio",
                                             "Ljava/lang/String;");
    jfieldID valorComision = (env)->GetFieldID(datosComisionClass, "valorComision",
                                               "Ljava/lang/String;");
    jfieldID valorIvaComision = (env)->GetFieldID(datosComisionClass, "valorIvaComision",
                                                  "Ljava/lang/String;");
    jfieldID referenciaCancelacion = (env)->GetFieldID(datosComisionClass, "referenciaCancelacion",
                                                       "Ljava/lang/String;");
    jfieldID referenciaReclamacion = (env)->GetFieldID(datosComisionClass, "referenciaReclamacion",
                                                       "Ljava/lang/String;");

    jstring jmontoMinimo = static_cast<jstring>((env)->GetObjectField(datos_comision, montoMinimo));
    if (jmontoMinimo != NULL) {
        paramMontoMinimo = (env)->GetStringUTFChars(jmontoMinimo, JNI_FALSE);
        memcpy(datosComision.montoMinimo, (char *) paramMontoMinimo,
               strlen((char *) paramMontoMinimo));
    }

    jstring jmontoMaximo = static_cast<jstring>((env)->GetObjectField(datos_comision, montoMaximo));
    if (jmontoMaximo != NULL) {
        paramMontoMaximo = (env)->GetStringUTFChars(jmontoMaximo, JNI_FALSE);
        memcpy(datosComision.montoMaximo, (char *) paramMontoMaximo,
               strlen((char *) paramMontoMaximo));
    }
    jstring jcomisionFija = static_cast<jstring>((env)->GetObjectField(datos_comision,
                                                                       comisionFija));
    if (jcomisionFija != NULL) {
        paramComisionFija = (env)->GetStringUTFChars(jcomisionFija, JNI_FALSE);
        memcpy(datosComision.comisionFija, (char *) paramComisionFija,
               strlen((char *) paramComisionFija));
    }
    jstring jporcentajeComision = static_cast<jstring>((env)->GetObjectField(datos_comision,
                                                                             porcentajeComision));
    if (jporcentajeComision != NULL) {
        paramPorcentajeComision = (env)->GetStringUTFChars(jporcentajeComision, JNI_FALSE);
        memcpy(datosComision.porcentajeComision, (char *) paramPorcentajeComision,
               strlen((char *) paramPorcentajeComision));
    }
    jstring jporcentajeIva = static_cast<jstring>((env)->GetObjectField(datos_comision,
                                                                        porcentajeIva));
    if (jporcentajeIva != NULL) {
        paramPorcentajeIva = (env)->GetStringUTFChars(jporcentajeIva, JNI_FALSE);
        memcpy(datosComision.porcentajeIva, (char *) paramPorcentajeIva,
               strlen((char *) paramPorcentajeIva));
    }
    jstring jfechaInicio = static_cast<jstring>((env)->GetObjectField(datos_comision, fechaInicio));
    if (jfechaInicio != NULL) {
        paramFechaInicio = (env)->GetStringUTFChars(jfechaInicio, JNI_FALSE);
        memcpy(datosComision.fechaInicio, (char *) paramFechaInicio,
               strlen((char *) paramFechaInicio));
    }
    jstring jvalorComision = static_cast<jstring>((env)->GetObjectField(datos_comision,
                                                                        valorComision));
    if (jvalorComision != NULL) {
        paramValorComision = (env)->GetStringUTFChars(jvalorComision, JNI_FALSE);
        memcpy(datosComision.valorComision, (char *) paramValorComision,
               strlen((char *) paramValorComision));
    }
    jstring jvalorIvaComision = static_cast<jstring>((env)->GetObjectField(datos_comision,
                                                                           valorIvaComision));
    if (jvalorIvaComision != NULL) {
        paramValorIvaComision = (env)->GetStringUTFChars(jvalorIvaComision, JNI_FALSE);
        memcpy(datosComision.valorIvaComision, (char *) paramValorIvaComision,
               strlen((char *) paramValorIvaComision));
    }
    jstring jreferenciaCancelacion = static_cast<jstring>((env)->GetObjectField(datos_comision,
                                                                                referenciaCancelacion));
    if (jreferenciaCancelacion != NULL) {
        paramReferenciaCancelacion = (env)->GetStringUTFChars(jreferenciaCancelacion, JNI_FALSE);
        memcpy(datosComision.referenciaCancelacion, (char *) paramReferenciaCancelacion,
               strlen((char *) paramReferenciaCancelacion));
    }
    jstring jreferenciaReclamacion = static_cast<jstring>((env)->GetObjectField(datos_comision,
                                                                                referenciaReclamacion));
    if (jreferenciaReclamacion != NULL) {
        paramReferenciaReclamacion = (env)->GetStringUTFChars(jreferenciaReclamacion, JNI_FALSE);
        memcpy(datosComision.referenciaReclamacion, (char *) paramReferenciaReclamacion,
               strlen((char *) paramReferenciaReclamacion));
    }
    return datosComision;
}

void
setearParametrosObjetoComision(JNIEnv *env, DatosComision datosComision, jobject datos_comision) {

    env->ExceptionClear();

    jclass datosComisionClass = (env)->GetObjectClass(datos_comision);

    jfieldID montoMinimo = (env)->GetFieldID(datosComisionClass, "montoMinimo",
                                             "Ljava/lang/String;");
    jfieldID montoMaximo = (env)->GetFieldID(datosComisionClass, "montoMaximo",
                                             "Ljava/lang/String;");
    jfieldID comisionFija = (env)->GetFieldID(datosComisionClass, "comisionFija",
                                              "Ljava/lang/String;");
    jfieldID porcentajeComision = (env)->GetFieldID(datosComisionClass, "porcentajeComision",
                                                    "Ljava/lang/String;");
    jfieldID porcentajeIva = (env)->GetFieldID(datosComisionClass, "porcentajeIva",
                                               "Ljava/lang/String;");
    jfieldID fechaInicio = (env)->GetFieldID(datosComisionClass, "fechaInicio",
                                             "Ljava/lang/String;");
    jfieldID valorComision = (env)->GetFieldID(datosComisionClass, "valorComision",
                                               "Ljava/lang/String;");
    jfieldID valorIvaComision = (env)->GetFieldID(datosComisionClass, "valorIvaComision",
                                                  "Ljava/lang/String;");
    jfieldID referenciaCancelacion = (env)->GetFieldID(datosComisionClass, "referenciaCancelacion",
                                                       "Ljava/lang/String;");
    jfieldID referenciaReclamacion = (env)->GetFieldID(datosComisionClass, "referenciaReclamacion",
                                                       "Ljava/lang/String;");

    //modificar la propieda de un objeto desde C-jni
    (env)->SetObjectField(datos_comision, montoMinimo,
                          env->NewStringUTF(datosComision.montoMaximo));
    (env)->SetObjectField(datos_comision, montoMaximo,
                          env->NewStringUTF(datosComision.montoMaximo));
    (env)->SetObjectField(datos_comision, comisionFija,
                          env->NewStringUTF(datosComision.comisionFija));
    (env)->SetObjectField(datos_comision, porcentajeComision,
                          env->NewStringUTF(datosComision.porcentajeComision));
    (env)->SetObjectField(datos_comision, porcentajeIva,
                          env->NewStringUTF(datosComision.porcentajeIva));
    (env)->SetObjectField(datos_comision, fechaInicio,
                          env->NewStringUTF(datosComision.fechaInicio));
    LOGI("cARGANDO linea 1822 paso de la mitad ");

    (env)->SetObjectField(datos_comision, valorComision,
                          env->NewStringUTF(datosComision.valorComision));
    LOGI("cARGANDO linea 1826 paso de la mitad ");

    (env)->SetObjectField(datos_comision, valorIvaComision,
                          env->NewStringUTF(datosComision.valorIvaComision));
    LOGI("cARGANDO linea 1830 datosComision.referenciaCancelacion  %s",datosComision.referenciaCancelacion);
    (env)->SetObjectField(datos_comision, referenciaCancelacion,
                          env->NewStringUTF(datosComision.referenciaCancelacion));
    LOGI("cARGANDO linea 1833 paso de la mitad ");
    (env)->SetObjectField(datos_comision, referenciaReclamacion,
                          env->NewStringUTF(datosComision.referenciaReclamacion));

    LOGI("finalizo funcion ");

}

extern "C"
JNIEXPORT jstring JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_realizarConsultaCupoBCL(JNIEnv *env,
                                                                              jobject thiz) {
    env2 = env;
    instance2 = thiz;
    int resultado = 0;
    char respuesta[100 + 1] = {0x00};
    char codigoRechazo[100 + 1] = {0x00};
    if (realizarConexionTlS() > 0) {
        resultado = gestionarConsultaCupoCB(respuesta);
        cerrarConexion();
        if (resultado > 0) {
            strcpy(codigoRechazo, "00;");
            strcat(codigoRechazo, respuesta);
            return env->NewStringUTF(codigoRechazo);
        } else {
            obtenerStringDeclinada(codigoRechazo, env, thiz);
            strcpy(respuesta, "05");
            strcat(respuesta, ";");
            strcat(respuesta, codigoRechazo);
        }
    } else {
        strcpy(respuesta, "05;ERROR SIN CONEXION");
    }

    limpiarTokenMessage();

    return env->NewStringUTF(respuesta);
}extern "C"
JNIEXPORT jstring JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_realizarVentaEmv(JNIEnv *env, jobject thiz,
                                                                       jobject datos_tarjta,
                                                                       jobject datos_basicos_venta) {

    env2 = env;
    instance2 = thiz;

    int resultado = 0;
    char codigoRechazo[20 + 1] = {0x00};
    char respuesta[100 + 1] = {0x00};

    DatosBasicosVenta datosBasicos;
    DatosTarjetaAndroid datosTarjeta;

    memset(&datosBasicos, 0x00, sizeof(datosBasicos));
    memset(&datosTarjeta, 0x00, sizeof(datosTarjeta));

    datosTarjeta = obtenerDatosTarjeta(env, datos_tarjta);
    datosBasicos = obtenerDatosBasicos(env, datos_basicos_venta);
    if (realizarConexionTlS() > 0) {
        resultado = realizarVentaEMV(datosTarjeta, datosBasicos);
        cerrarConexion();
    } else {
        strcpy(respuesta, "ERROR SIN CONEXION");
    }

    if (resultado > 0) {

        strcpy(respuesta, globalTramaVenta.codigoRespuesta);
        strcat(respuesta, ";");
        strcat(respuesta, "NUMERO DE APROBACION ");
        strcat(respuesta, globalTramaVenta.codigoAprobacion);
    } else {
        obtenerStringDeclinada(codigoRechazo, env, thiz);
        strcpy(respuesta, "05");
        strcat(respuesta, ";");
        strcat(respuesta, codigoRechazo);
    }


    return env->NewStringUTF(respuesta);
}

DatosBasicosVenta obtenerDatosBasicos(JNIEnv *env, jobject datos_basicos) {

    const char *paramTotalVenta;
    const char *paramBaseDevolucion;
    const char *paramIva;
    const char *paramInc;
    const char *paramPropina;
    const char *paramCompraNeta;
    const char *paramTotalVentaIva;
    const char *paramTotalVentaInc;

    DatosBasicosVenta datosBasicos;
    memset(&datosBasicos, 0x00, sizeof(datosBasicos));
    jclass datosBasicosClass = (env)->GetObjectClass(datos_basicos);

    jfieldID totalVenta = (env)->GetFieldID(datosBasicosClass, "totalVenta", "Ljava/lang/String;");
    jstring jTotalVenta = static_cast<jstring>((env)->GetObjectField(datos_basicos,
                                                                     totalVenta));
    if (jTotalVenta != NULL) {
        paramTotalVenta = (env)->GetStringUTFChars(jTotalVenta, JNI_FALSE);
        memcpy(datosBasicos.totalVenta, (char *) paramTotalVenta, strlen(paramTotalVenta));
    }

    jfieldID baseDevolucion = (env)->GetFieldID(datosBasicosClass, "baseDevolucion",
                                                "Ljava/lang/String;");
    jstring jBaseDevolucion = static_cast<jstring>((env)->GetObjectField(datos_basicos,
                                                                         baseDevolucion));
    if (jBaseDevolucion != NULL) {
        paramBaseDevolucion = (env)->GetStringUTFChars(jBaseDevolucion, JNI_FALSE);
        memcpy(datosBasicos.baseDevolucion, (char *) paramBaseDevolucion,
               strlen(paramBaseDevolucion));
    }

    jfieldID iva = (env)->GetFieldID(datosBasicosClass, "iva", "Ljava/lang/String;");
    jstring jIva = static_cast<jstring>((env)->GetObjectField(datos_basicos, iva));

    if (jIva != NULL) {
        paramIva = (env)->GetStringUTFChars(jIva, JNI_FALSE);
        memcpy(datosBasicos.iva, (char *) paramIva, strlen(paramIva));
    }

    jfieldID inc = (env)->GetFieldID(datosBasicosClass, "inc", "Ljava/lang/String;");
    jstring jInc = static_cast<jstring>((env)->GetObjectField(datos_basicos, inc));

    if (jInc != NULL) {
        paramInc = (env)->GetStringUTFChars(jInc, JNI_FALSE);
        memcpy(datosBasicos.inc, (char *) paramInc, strlen(paramInc));
    }

    jfieldID propina = (env)->GetFieldID(datosBasicosClass, "propina", "Ljava/lang/String;");
    jstring jPropina = static_cast<jstring>((env)->GetObjectField(datos_basicos, propina));
    if (NULL != jPropina) {
        paramPropina = (env)->GetStringUTFChars(jPropina, JNI_FALSE);
        memcpy(datosBasicos.propina, (char *) paramPropina, strlen(paramPropina));
    }

    jfieldID compraNeta = (env)->GetFieldID(datosBasicosClass, "compraNeta", "Ljava/lang/String;");
    jstring jCompraneta = static_cast<jstring>((env)->GetObjectField(datos_basicos, compraNeta));
    if (NULL != jCompraneta) {
        paramCompraNeta = (env)->GetStringUTFChars(jCompraneta, JNI_FALSE);
        memcpy(datosBasicos.compraNeta, (char *) paramCompraNeta, strlen(paramCompraNeta));
    }
    jfieldID totalVentaIva = (env)->GetFieldID(datosBasicosClass, "totalVentaIva",
                                               "Ljava/lang/String;");
    jstring jTotalVentaIva = static_cast<jstring>((env)->GetObjectField(datos_basicos,
                                                                        totalVentaIva));
    if (NULL != jTotalVentaIva) {
        paramTotalVentaIva = (env)->GetStringUTFChars(jTotalVentaIva, JNI_FALSE);
        memcpy(datosBasicos.totalVentaIva, (char *) paramTotalVentaIva, strlen(paramTotalVentaIva));
    }
    jfieldID totalVentaInc = (env)->GetFieldID(datosBasicosClass, "totalVentaInc",
                                               "Ljava/lang/String;");
    jstring jTotalVentaInc = static_cast<jstring>((env)->GetObjectField(datos_basicos,
                                                                        totalVentaInc));
    if (NULL != jTotalVentaInc) {
        paramTotalVentaInc = (env)->GetStringUTFChars(jTotalVentaInc, JNI_FALSE);
        memcpy(datosBasicos.totalVentaInc, (char *) paramTotalVentaInc, strlen(paramTotalVentaInc));
    }
    return datosBasicos;
}

void _guardarTransaccion_(DatosVenta tramaVenta) {

    int iRet = 0;
    iRet = escribirArchivo(discoNetcom, (char *) JOURNAL, (char *) &tramaVenta, sizeof(DatosVenta));
    if (iRet > 0) {
        _guardarDirectorioJournals_(TRANS_COMERCIO, tramaVenta.numeroRecibo,
                                    tramaVenta.estadoTransaccion);
    }
}

void obtenerStringDeclinada(char *cadena, JNIEnv *env, jobject thiz) {
    const char *paramString;
    env->ExceptionClear();
    jclass jClassInstance = env->GetObjectClass(thiz);
    jmethodID callBackJava = env->GetMethodID(jClassInstance, "obtenerStringDeclinada",
                                              "()Ljava/lang/String;");
    if (NULL == callBackJava);
    int result = 0;

    // Call back Java method with parameters
    jstring jstr = (jstring) env->CallObjectMethod(thiz, callBackJava);

    paramString = (env)->GetStringUTFChars(jstr, JNI_FALSE);

    strcpy(cadena, (char *) paramString);

}

extern "C"
JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_solicitarDuplicadoVenta(JNIEnv *env,
                                                                              jobject thiz,
                                                                              jint opcion_recibo,
                                                                              jint tipo_recibo,

                                                                              jstring numero_recibo) {
    env2 = env;
    instance2 = thiz;
    char numeroRecibo[6 + 1] = {0x00};
    const char *paramNumeroRecibo;
    int resultado = 0;
    paramNumeroRecibo = (env)->GetStringUTFChars(numero_recibo, JNI_FALSE);

    if (opcion_recibo == 1) {
        resultado = buscarUltimoRecibo(tipo_recibo);
    } else {
        leftPad(numeroRecibo, (char *) paramNumeroRecibo, 0x20, 6);
        resultado = buscarRecibo(numeroRecibo, tipo_recibo);
    }
    return resultado;

}extern "C"
JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_reporteCierreVentas(JNIEnv *env, jobject thiz,
                                                                          jint tipo_reporte) {

    env2 = env;
    instance2 = thiz;
    int iRet = 0;
    iRet = _reporteCierre_(tipo_reporte);
    return iRet;
}extern "C"
JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_realizarCierreVenta(JNIEnv *env,
                                                                          jobject thiz) {

    env2 = env;
    instance2 = thiz;
    int iRet = 0;
    if (realizarConexionTlS() > 0) {
        iRet = _cierreVenta_();
        cerrarConexion();
    }
    return iRet;
}
extern "C"
JNIEXPORT jstring JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_realizarConsultaReciboAnular(JNIEnv *env,
                                                                                   jobject thiz,
                                                                                   jint tipo_recibo,
                                                                                   jstring numero_recibo) {

    env2 = env;
    instance2 = thiz;
    char numeroRecibo[6 + 1] = {0x00};
    char respuesta[100 + 1] = {0x00};
    const char *paramNumeroRecibo;
    if (tipo_recibo == 2) {
        paramNumeroRecibo = (env)->GetStringUTFChars(numero_recibo, JNI_FALSE);
        leftPad(numeroRecibo, (char *) paramNumeroRecibo, 0x20, 6);
    }

    realizarConsultaReciboAnulacion(numeroRecibo);
    obtenerStringDeclinada(respuesta, env, thiz);

    return env->NewStringUTF(respuesta);
}extern "C"
JNIEXPORT jstring JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_realizarTransaccionAnulacion(JNIEnv *env,
                                                                                   jobject thiz,
                                                                                   jobject datos_tarjeta,
                                                                                   jstring numero_recibo) {
    env2 = env;
    instance2 = thiz;
    char respuesta[100 + 1] = {0x00};
    const char *paramNumeroRecibo;
    DatosTarjetaAndroid datosTarjetaAndroid;

    memset(&datosTarjetaAndroid, 0x00, sizeof(datosTarjetaAndroid));

    datosTarjetaAndroid = obtenerDatosTarjeta(env, datos_tarjeta);
    paramNumeroRecibo = (env)->GetStringUTFChars(numero_recibo, JNI_FALSE);

    if (strcmp(datosTarjetaAndroid.track2, globalTramaVenta.track2) == 0) {
        if (realizarConexionTlS() > 0) {
            anularTransaccion(globalTramaVenta);
            cerrarConexion();
            strcpy(respuesta, globalTramaVenta.codigoAprobacion);
        } else {
            strcpy(respuesta, "ERROR SIN CONEXION");
        }
    }

    return env->NewStringUTF(respuesta);
}extern "C"
JNIEXPORT jstring JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_realizarTransaccionAnulacionBancolombia(
        JNIEnv *env,
        jobject thiz,
        jstring numero_recibo) {
    env2 = env;
    instance2 = thiz;
    int resultado = 0;
    char respuesta[100 + 1] = {0x00};
    const char *paramNumeroRecibo;

    paramNumeroRecibo = (env)->GetStringUTFChars(numero_recibo, JNI_FALSE);

    if (realizarConexionTlS() > 0) {
        limpiarTokenMessage();
        resultado = enviarAnulacion(&globalDatosBancolombia);
        cerrarConexion();
        strcpy(respuesta, globalDatosBancolombia.codigoAprobacion);
        resultadoTransaccionBancolombia(resultado, respuesta, env, thiz);
    } else {
        strcpy(respuesta, "05;ERROR SIN CONEXION");
    }


    return env->NewStringUTF(respuesta);
}extern "C"
JNIEXPORT jstring JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_realizarConsultaReciboAnularBancolombia(
        JNIEnv *env,
        jobject thiz,
        jint tipo_recibo,
        jstring numero_recibo) {

    env2 = env;
    instance2 = thiz;
    char numeroRecibo[6 + 1] = {0x00};
    char respuesta[100 + 1] = {0x00};
    const char *paramNumeroRecibo;
    if (tipo_recibo == 2) {
        paramNumeroRecibo = (env)->GetStringUTFChars(numero_recibo, JNI_FALSE);
        leftPad(numeroRecibo, (char *) paramNumeroRecibo, 0x20, 6);
    }
    realizarConsultaReciboAnulacionBanbolombia(numeroRecibo);
    obtenerStringDeclinada(respuesta, env, thiz);

    return env->NewStringUTF(respuesta);
}extern "C"
JNIEXPORT void JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_setParameterToC(JNIEnv *env, jobject thiz,
                                                                      jint clave, jstring valor) {
    const char *paramValor;
    paramValor = (env)->GetStringUTFChars(valor, JNI_FALSE);
    setParameter(clave, (char *) paramValor);

}extern "C"
JNIEXPORT void JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_realizarImpresionSegundoRecibo(JNIEnv *env,
                                                                                     jobject thiz,
                                                                                     jint corresponsal,
                                                                                     jint anulacion) {
    env2 = env;
    instance2 = thiz;
    int tipoRecibo = RECIBO_NO_DUPLICADO;

    if(anulacion == 1){
        tipoRecibo = RECIBO_ANULACION;
    }
    if (corresponsal == 1) {
        solicitarSegundoRecibo(tipoRecibo);
    } else {
        imprimirTicketVenta(globalTramaVenta, RECIBO_CLIENTE, tipoRecibo);
    }
}extern "C"
JNIEXPORT jstring JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_validarMontosIngresadosVenta(JNIEnv *env,
                                                                                   jobject thiz,
                                                                                   jobject datos_basicos_venta) {
    env2 = env;
    instance2 = thiz;

    int resultado = 0;
    char respuesta[100 + 1] = {0x00};
    DatosBasicosVenta datosBasicos;

    memset(&datosBasicos, 0x00, sizeof(datosBasicos));
    datosBasicos = obtenerDatosBasicos(env, datos_basicos_venta);

    resultado = capturarDatosBasicosVenta(&datosBasicos);

    if (resultado > 0) {
        setearDatosBasicosJava(datosBasicos, env, datos_basicos_venta);
        strcpy(respuesta, "OK");

    } else {
        obtenerStringDeclinada(respuesta, env, thiz);
    }

    return env->NewStringUTF(respuesta);
}

void setearDatosBasicosJava(DatosBasicosVenta datosBasicos, JNIEnv *env, jobject datos_basicos) {

    env->ExceptionClear();
    jclass datosBasicosClass = (env)->GetObjectClass(datos_basicos);

    jfieldID totalVenta = (env)->GetFieldID(datosBasicosClass, "totalVenta", "Ljava/lang/String;");
    jfieldID baseDevolucion = (env)->GetFieldID(datosBasicosClass, "baseDevolucion",
                                                "Ljava/lang/String;");
    jfieldID iva = (env)->GetFieldID(datosBasicosClass, "iva", "Ljava/lang/String;");
    jfieldID inc = (env)->GetFieldID(datosBasicosClass, "inc", "Ljava/lang/String;");
    jfieldID propina = (env)->GetFieldID(datosBasicosClass, "propina", "Ljava/lang/String;");
    jfieldID compraNeta = (env)->GetFieldID(datosBasicosClass, "compraNeta", "Ljava/lang/String;");

    (env)->SetObjectField(datos_basicos, totalVenta, env->NewStringUTF(datosBasicos.totalVenta));
    (env)->SetObjectField(datos_basicos, iva, env->NewStringUTF(datosBasicos.iva));
    (env)->SetObjectField(datos_basicos, inc, env->NewStringUTF(datosBasicos.inc));
    (env)->SetObjectField(datos_basicos, baseDevolucion,
                          env->NewStringUTF(datosBasicos.baseDevolucion));
    (env)->SetObjectField(datos_basicos, propina, env->NewStringUTF(datosBasicos.propina));
    (env)->SetObjectField(datos_basicos, compraNeta, env->NewStringUTF(datosBasicos.compraNeta));
}

extern "C"
JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_comportamientoTiposCuentaEmv(JNIEnv *env,
                                                                                   jobject thiz,
                                                                                   jobject datos_tarjta) {
    env2 = env;
    instance2 = thiz;
    int resultado = 0;
    DatosTarjetaAndroid datosTarjetaAndroid;

    memset(&datosTarjetaAndroid, 0x00, sizeof(datosTarjetaAndroid));

    datosTarjetaAndroid = obtenerDatosTarjeta(env, datos_tarjta);

    solicitudTipoCuenta(&datosTarjetaAndroid);

    setearDatosTarjta(datosTarjetaAndroid, env, datos_tarjta);

    return 1;
}

void
setearDatosTarjta(DatosTarjetaAndroid datosTarjetaAndroid, JNIEnv *env, jobject datos_tarjeta) {

    env->ExceptionClear();
    jclass datosTarjetaClass = (env)->GetObjectClass(datos_tarjeta);
    jint jAskLastFour = datosTarjetaAndroid.asklastfour;
    jint jAskDocument = datosTarjetaAndroid.askDocument;
    jint jAskCuotes = datosTarjetaAndroid.askCuotes;
    jint jAskCvc2 = datosTarjetaAndroid.askCvc2;
    jint jAskAdicionalText = datosTarjetaAndroid.askAdicionalText;


    jfieldID asklastfour = (env)->GetFieldID(datosTarjetaClass, "asklastfour", "I");
    jfieldID askDocument = (env)->GetFieldID(datosTarjetaClass, "askDocument", "I");
    jfieldID askCuotes = (env)->GetFieldID(datosTarjetaClass, "askCuotes", "I");
    jfieldID askCvc2 = (env)->GetFieldID(datosTarjetaClass, "askCvc2", "I");
    jfieldID askAdicionalText = (env)->GetFieldID(datosTarjetaClass, "askAdicionalText", "I");
    jfieldID grupo = (env)->GetFieldID(datosTarjetaClass, "grupo", "Ljava/lang/String;");
    jfieldID subGrupo = (env)->GetFieldID(datosTarjetaClass, "subGrupo", "Ljava/lang/String;");

    (env)->SetIntField(datos_tarjeta, asklastfour, jAskLastFour);
    (env)->SetIntField(datos_tarjeta, askDocument, jAskDocument);
    (env)->SetIntField(datos_tarjeta, askCuotes, jAskCuotes);
    (env)->SetIntField(datos_tarjeta, askCvc2, jAskCvc2);
    (env)->SetIntField(datos_tarjeta, askAdicionalText, jAskAdicionalText);
    (env)->SetObjectField(datos_tarjeta, grupo, env->NewStringUTF(datosTarjetaAndroid.grupo));
    (env)->SetObjectField(datos_tarjeta, subGrupo, env->NewStringUTF(datosTarjetaAndroid.subGrupo));
}

extern "C"
JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_usuarioAmodificarCnb(JNIEnv *env,
                                                                           jobject thiz,
                                                                           jstring usuario) {
    const char *paramUsuario;

    int iRet = 0;

    paramUsuario = (env)->GetStringUTFChars(usuario, JNI_FALSE);

    iRet = usuarioAmodificarCnb((char *) paramUsuario);

    return iRet;

}
extern "C"
JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_eliminarUsuarioCnb(JNIEnv *env, jobject thiz,
                                                                         jstring usuario) {
    const char *paramUsuario;

    int iRet = 0;

    paramUsuario = (env)->GetStringUTFChars(usuario, JNI_FALSE);

    iRet = eliminarUsuarioCnb((char *) paramUsuario);

    return iRet;

}
extern "C"
JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_listaUsuarios(JNIEnv *env, jobject thiz) {

    env2 = env;
    instance2 = thiz;
    int iRet = 0;

    iRet = listadoUsuarios();

    return iRet;
}
extern "C"
JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_modificarUsuarioCnb(JNIEnv *env, jobject thiz,
                                                                          jstring usuario,
                                                                          jstring usuario2,
                                                                          jstring clave,
                                                                          jstring tipo_user) {
    const char *paramUsuario;
    const char *paramUsuario2;
    const char *paramClave;
    const char *paramTipoUser;
    int iRet = 0;

    paramUsuario = (env)->GetStringUTFChars(usuario, JNI_FALSE);
    paramUsuario2 = (env)->GetStringUTFChars(usuario2, JNI_FALSE);
    paramClave = (env)->GetStringUTFChars(clave, JNI_FALSE);
    paramTipoUser = (env)->GetStringUTFChars(tipo_user, JNI_FALSE);

    iRet = modificarUsuarioCnb((char *) paramUsuario, (char *) paramUsuario2, (char *) paramClave,
                               (char *) paramTipoUser);

    return iRet;

}extern "C"
JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_realizarSolicitudQrcode(JNIEnv *env,
                                                                              jobject thiz,
                                                                              jobject datos_basicos_venta) {
    env2 = env;
    instance2 = thiz;
    int resultado = 0;

    DatosBasicosVenta datosBasicosVenta;
    memset(&datosBasicosVenta, 0x00, sizeof(datosBasicosVenta));

    datosBasicosVenta = obtenerDatosBasicos(env, datos_basicos_venta);
    if (realizarConexionTlS() > 0) {
        resultado = armarTramaSolicitudPaypass(datosBasicosVenta);
    }

    return resultado;
}extern "C"
JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_realizarConsultaQrcode(JNIEnv *env,
                                                                             jobject thiz) {
    env2 = env;
    instance2 = thiz;
    int resultado = 0;
    resultado = consultaAutoQR(sizeof(globalTramaVenta), (char *) &globalTramaVenta);

    return resultado;
}extern "C"
JNIEXPORT void JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_solicitarMenuEntidadesDineroElectronico(
        JNIEnv *env, jobject thiz) {
    env2 = env;
    instance2 = thiz;
    solicitarMenuEntidades();
}

void setearDatosDineroElectronico(ParametrosDineroElectronico datosLeidos, JNIEnv *env,
                                  jobject parametros_dinero) {

    env->ExceptionClear();

    jclass datosDineroClass = (env)->GetObjectClass(parametros_dinero);

    jint jPermiteCelular = datosLeidos.permiteCelular;
    jint jPermiteDocumento = datosLeidos.permiteDocumento;
    jint jPermiteCuotas = datosLeidos.permiteCuotas;
    jint jPermiteSeleccionCuenta = datosLeidos.permiteSeleccionCuenta;
    jint jTipoPinSolicitado = datosLeidos.tipoPinSolicitado;

    jfieldID itemsTipoDocumento = (env)->GetFieldID(datosDineroClass, "itemsTipoDocumento",
                                                    "Ljava/lang/String;");
    jfieldID tipoDocumento = (env)->GetFieldID(datosDineroClass, "tipoDocumento",
                                               "Ljava/lang/String;");
    jfieldID nombreBanco = (env)->GetFieldID(datosDineroClass, "nombreBanco", "Ljava/lang/String;");
    jfieldID fiddOtp = (env)->GetFieldID(datosDineroClass, "fiddOtp", "Ljava/lang/String;");
    jfieldID binAsignado = (env)->GetFieldID(datosDineroClass, "binAsignado", "Ljava/lang/String;");
    jfieldID permiteCelular = (env)->GetFieldID(datosDineroClass, "permiteCelular", "I");
    jfieldID permiteDocumento = (env)->GetFieldID(datosDineroClass, "permiteDocumento", "I");
    jfieldID permiteCuotas = (env)->GetFieldID(datosDineroClass, "permiteCuotas", "I");
    jfieldID permiteSeleccionCuenta = (env)->GetFieldID(datosDineroClass, "permiteSeleccionCuenta",
                                                        "I");
    jfieldID tipoPinSolicitado = (env)->GetFieldID(datosDineroClass, "tipoPinSolicitado", "I");

    (env)->SetIntField(parametros_dinero, permiteCelular, jPermiteCelular);
    (env)->SetIntField(parametros_dinero, permiteDocumento, jPermiteDocumento);
    (env)->SetIntField(parametros_dinero, permiteCuotas, jPermiteCuotas);
    (env)->SetIntField(parametros_dinero, tipoPinSolicitado, jTipoPinSolicitado);
    (env)->SetIntField(parametros_dinero, permiteSeleccionCuenta, jPermiteSeleccionCuenta);
    (env)->SetObjectField(parametros_dinero, tipoDocumento,
                          env->NewStringUTF(datosLeidos.tipoDocumento));
    (env)->SetObjectField(parametros_dinero, itemsTipoDocumento,
                          env->NewStringUTF(datosLeidos.itemsTipoDocumento));
    (env)->SetObjectField(parametros_dinero, nombreBanco,
                          env->NewStringUTF(datosLeidos.nombreBanco));
    (env)->SetObjectField(parametros_dinero, fiddOtp, env->NewStringUTF(datosLeidos.fiddOtp));

}

extern "C"
JNIEXPORT void JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_solicitarParametrosDineroElectronico(
        JNIEnv *env, jobject thiz, jobject parametros_dinero_electronico, jint tipo_tx, jint opcion,
        jstring valor) {
    env2 = env;
    instance2 = thiz;

    const char *paramvalor;

    paramvalor = (env)->GetStringUTFChars(valor, JNI_FALSE);
    ParametrosDineroElectronico parametrosDineroElectronico;

    memset(&parametrosDineroElectronico, 0x00, sizeof(parametrosDineroElectronico));

    parametrosDineroElectronico = capturarDatosTransaccion(tipo_tx, opcion, (char *) paramvalor);
    setearDatosDineroElectronico(parametrosDineroElectronico, env, parametros_dinero_electronico);
}
extern "C"
JNIEXPORT jstring JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_consultarSaldo(JNIEnv *env,
                                                                     jobject thiz,
                                                                     jobject datos_tarjeta) {
    env2 = env;
    instance2 = thiz;
    int resultado = 0;

    char saldo[20 + 1] = {0x00};
    char respuesta[100 + 1] = {0x00};
    char codigoRechazo[100 + 1] = {0x00};

    DatosTarjetaAndroid datosExternos;
    memset(&datosExternos, 0x00, sizeof(datosExternos));

    datosExternos = obtenerDatosTarjeta(env, datos_tarjeta);

    if (realizarConexionTlS() > 0) {
        resultado = consultarSaldo(datosExternos, saldo);
        cerrarConexion();
    }

    if (resultado > 0) {
        strcpy(respuesta, "00");
        strcat(respuesta, ";");
        strcat(respuesta, saldo);
    } else {
        obtenerStringDeclinada(codigoRechazo, env, thiz);
        strcpy(respuesta, "05");
        strcat(respuesta, ";");
        strcat(respuesta, codigoRechazo);
    }
    limpiarTokenMessage();

    return env->NewStringUTF(respuesta);
}
extern "C"
JNIEXPORT void JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_armarTrack2DineroElectronico(JNIEnv *env,
                                                                                   jobject thiz,
                                                                                   jobject parmetroscompra,
                                                                                   jint tipo_documento,
                                                                                   jstring documento,
                                                                                   jstring celular) {
    env2 = env;
    instance2 = thiz;
    const char *paramDocumento;
    const char *paramCelular;

    paramDocumento = (env)->GetStringUTFChars(documento, JNI_FALSE);
    paramCelular = (env)->GetStringUTFChars(celular, JNI_FALSE);

    ParametrosDineroElectronico parametrosDineroElectronico;

    memset(&parametrosDineroElectronico, 0x00, sizeof(parametrosDineroElectronico));
    parametrosDineroElectronico = armarTrack2DineroElectronico(tipo_documento,
                                                               (char *) paramDocumento,
                                                               (char *) paramCelular);

    setearDatosDineroElectronico(parametrosDineroElectronico, env, parmetroscompra);
}extern "C"
JNIEXPORT jstring JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_realizarCompraDineroElectronico(JNIEnv *env,
                                                                                      jobject thiz,
                                                                                      jstring pinblock,
                                                                                      jint intentos_pin,
                                                                                      jstring tipo_cuenta,
                                                                                      jstring cuotas) {
    env2 = env;
    instance2 = thiz;
    int resultado = 0;
    const char *paramPinblock;
    const char *paramTipoCuenta;
    const char *paramCuotas;
    char codigoRechazo[20 + 1] = {0x00};
    char respuesta[100 + 1] = {0x00};
    paramPinblock = (env)->GetStringUTFChars(pinblock, JNI_FALSE);
    paramTipoCuenta = (env)->GetStringUTFChars(tipo_cuenta, JNI_FALSE);
    paramCuotas = (env)->GetStringUTFChars(cuotas, JNI_FALSE);

    if (realizarConexionTlS() > 0) {
        resultado = realizarProcesoTransaccionDineroElectronico((char *) paramPinblock,
                                                                intentos_pin,
                                                                (char *) paramTipoCuenta,
                                                                (char *) paramCuotas);
        cerrarConexion();
    } else {
        resultado = -1;
    }
    if (resultado > 0) {

        strcpy(respuesta, globalTramaVenta.codigoRespuesta);
        strcat(respuesta, ";");
        strcat(respuesta, "NUMERO DE APROBACION ");
        strcat(respuesta, globalTramaVenta.codigoAprobacion);
    } else {
        obtenerStringDeclinada(codigoRechazo, env, thiz);
        strcpy(respuesta, "05");
        strcat(respuesta, ";");
        strcat(respuesta, codigoRechazo);
    }
    return env->NewStringUTF(respuesta);
}extern "C"
JNIEXPORT void JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_enviarDatosBasicoCompraDineroElectronico(
        JNIEnv *env, jobject thiz, jobject datos_basicos_venta) {
    env2 = env;
    instance2 = thiz;
    DatosBasicosVenta datosBasicosVenta;
    memset(&datosBasicosVenta, 0x00, sizeof(datosBasicosVenta));
    datosBasicosVenta = obtenerDatosBasicos(env, datos_basicos_venta);
    obtencionDatosBasicosVenta(datosBasicosVenta);
}extern "C"
JNIEXPORT jstring JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_obtenerDatosParaPinDineroElectronico(
        JNIEnv *env, jobject thiz) {
    char cadenaSalida[50 + 1] = {0x00};
    char montoAuxlilar[12 + 1] = {0x00};
    char montoAuxiliar2[12 + 1] = {0x00};
    strcpy(cadenaSalida, globalTramaVenta.cardName);
    strcat(cadenaSalida, ";");
    leftTrim(montoAuxlilar, globalTramaVenta.totalVenta, '0');
    memcpy(montoAuxiliar2, montoAuxlilar, strlen(montoAuxlilar) - 2);
    strcat(cadenaSalida, montoAuxiliar2);

    return env->NewStringUTF(cadenaSalida);
}extern "C"
JNIEXPORT void JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_obtenerMenuEntidadesLifeMiles(JNIEnv *env,
                                                                                    jobject thiz,
                                                                                    jobject datos_basicos_venta) {
    env2 = env;
    instance2 = thiz;
    DatosBasicosVenta datosBasicosVenta;
    memset(&datosBasicosVenta, 0x00, sizeof(datosBasicosVenta));
    datosBasicosVenta = obtenerDatosBasicos(env, datos_basicos_venta);
    iniciarLifeMiles(datosBasicosVenta);
}extern "C"
JNIEXPORT void JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_seleccionarItemsDocumentoLealtadLifemiles(
        JNIEnv *env, jobject thiz, jint opcion) {
    env2 = env;
    instance2 = thiz;

    recibirOpcionMenu(opcion);

}extern "C"
JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_combinacionesTipoDocumento(JNIEnv *env,
                                                                                 jobject thiz,
                                                                                 jint opcion_lealtad,
                                                                                 jint opcion_documento) {

    env2 = env;
    instance2 = thiz;
    int resultado = 0;
    resultado = combinacionesTipoDocumento(opcion_documento, opcion_lealtad);

    return resultado;
}

extern "C"
JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_procesoAcumulacionRendencion(JNIEnv *env,
                                                                                   jobject thiz,
                                                                                   jint lectura,
                                                                                   jint acumular_ajustar,
                                                                                   jint modo_entrada,
                                                                                   jobject leer_track) {
    env2 = env;
    instance2 = thiz;
    int resultado = 0;

    DatosTarjetaAndroid datosTarjeta;
    memset(&datosTarjeta, 0x00, sizeof(datosTarjeta));
    if (lectura == 1) {
        datosTarjeta = obtenerDatosTarjeta(env, leer_track);
    }
    if (realizarConexionTlS() > 0) {
        resultado = procesoAcumulacionRedencionLifeMiles(lectura, acumular_ajustar, modo_entrada,
                                                         datosTarjeta);
        cerrarConexion();
    } else {
        resultado = -1;
    }


    return resultado;
}extern "C"
JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_ingresoDocumentoLifeMiles(JNIEnv *env,
                                                                                jobject thiz,
                                                                                jstring documento,
                                                                                jint modo_entrada) {
    env2 = env;
    instance2 = thiz;
    int resultado = 0;
    const char *paramDocumento;

    paramDocumento = (env)->GetStringUTFChars(documento, JNI_FALSE);

    resultado = ingresoDocumentoFromJava((char *) paramDocumento, modo_entrada);
    return resultado;
}extern "C"
JNIEXPORT void JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_iniciarPuntosColombia(JNIEnv *env,
                                                                            jobject thiz,
                                                                            jobject datos_basicos_venta) {
    env2 = env;
    instance2 = thiz;
    DatosBasicosVenta datosBasicosVenta;
    memset(&datosBasicosVenta, 0x00, sizeof(datosBasicosVenta));
    datosBasicosVenta = obtenerDatosBasicos(env, datos_basicos_venta);
    puntosColombia(datosBasicosVenta);
}extern "C"
JNIEXPORT void JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_redencionAcumulacionPuntoscolombia(
        JNIEnv *env, jobject thiz, jint opcion_lealtad, jint tipo_documento, jstring documento,
        jstring codigo_cajero, jstring galones, jstring monto) {
    env2 = env;
    instance2 = thiz;
    const char *paramDocumento;
    const char *paramCodigoCajero;
    const char *paramGalones;
    const char *paramMonto;

    paramDocumento = (env)->GetStringUTFChars(documento, JNI_FALSE);
    paramCodigoCajero = (env)->GetStringUTFChars(codigo_cajero, JNI_FALSE);
    paramGalones = (env)->GetStringUTFChars(galones, JNI_FALSE);
    paramMonto = (env)->GetStringUTFChars(monto, JNI_FALSE);

    recibirOpcionMenuPuntosColombia(opcion_lealtad, tipo_documento, (char *) paramDocumento,
                                    (char *) paramCodigoCajero, (char *) paramGalones,
                                    (char *) monto);
}extern "C"
JNIEXPORT void JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_solicitudConfigurarPuntosColombia(JNIEnv *env,
                                                                                        jobject thiz,
                                                                                        jint producto) {
    env2 = env;
    instance2 = thiz;
    solicitudMenuProducto(producto);
}extern "C"
JNIEXPORT void JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_configurarProductoIPSeleccionado(JNIEnv *env,
                                                                                       jobject thiz,
                                                                                       jint producto,
                                                                                       jint opcion) {
    env2 = env;
    instance2 = thiz;
    recibirOpcionMenuProducto(producto, opcion);
}extern "C"
JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_capturarIvaManual(JNIEnv *env, jobject thiz,
                                                                        jstring iva,
                                                                        jstring total_venta) {
    env2 = env;
    instance2 = thiz;
    int resultado = 0;

    const char *paramIva;
    const char *paramTotal;

    paramIva = (env)->GetStringUTFChars(iva, JNI_FALSE);
    paramTotal = (env)->GetStringUTFChars(total_venta, JNI_FALSE);
    resultado = capturarImpuestoIVaManual((char *) paramIva, (char *) paramTotal);

    return resultado;
}extern "C"
JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_capturarBaseDevolucionManual(JNIEnv *env,
                                                                                   jobject thiz,
                                                                                   jstring base_devolucion,
                                                                                   jstring total_venta,
                                                                                   jstring iva) {
    env2 = env;
    instance2 = thiz;
    int resultado = 0;

    const char *paramIva;
    const char *paramTotal;
    const char *paramBaseDev;

    paramIva = (env)->GetStringUTFChars(iva, JNI_FALSE);
    paramTotal = (env)->GetStringUTFChars(total_venta, JNI_FALSE);
    paramBaseDev = (env)->GetStringUTFChars(base_devolucion, JNI_FALSE);
    resultado = capturarBaseDevolucionManual((char *) paramBaseDev, (char *) paramTotal,
                                             (char *) paramIva);

    return resultado;
}extern "C"
JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_capturarIncManual(JNIEnv *env, jobject thiz,
                                                                        jobject datos_basicos_venta) {


    env2 = env;
    instance2 = thiz;

    int resultado = 0;
    DatosBasicosVenta datosBasicos;

    memset(&datosBasicos, 0x00, sizeof(datosBasicos));
    datosBasicos = obtenerDatosBasicos(env, datos_basicos_venta);

    resultado = capturarImpuestoIncManual(&datosBasicos);

    if (resultado > 0) {
        setearDatosBasicosJava(datosBasicos, env, datos_basicos_venta);
    }

    return resultado;

}extern "C"
JNIEXPORT void JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_restaurarInicializacion(JNIEnv *env,
                                                                              jobject thiz) {
    _restaurarInicializacion_();
}extern "C"
JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_validarPropina(JNIEnv *env, jobject thiz,
                                                                     jint opcion_propina,
                                                                     jobject datos_basicos_venta,
                                                                     jint porcentaje_propina) {
    env2 = env;
    instance2 = thiz;

    int resultado = 0;
    DatosBasicosVenta datosBasicos;

    memset(&datosBasicos, 0x00, sizeof(datosBasicos));
    datosBasicos = obtenerDatosBasicos(env, datos_basicos_venta);

    resultado = validarPropinaImpuesto(opcion_propina, &datosBasicos, porcentaje_propina);

    if (resultado > 0) {
        setearDatosBasicosJava(datosBasicos, env, datos_basicos_venta);
    }

    return resultado;
}extern "C"
JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_tasaAeroportuariaPropinaHabilitada(
        JNIEnv *env, jobject thiz) {
    return verificarhabilitacionTasaPropina();
}extern "C"
JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_verificarReverso(JNIEnv *env, jobject thiz) {
    return verificarReversoExistente();
}extern "C"
JNIEXPORT void JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_eliminarReversoExistente(JNIEnv *env,
                                                                               jobject thiz) {
    env2 = env;
    instance2 = thiz;
    capturarEliminarReverso();
}extern "C"
JNIEXPORT void JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_imprimirticketPrueba(JNIEnv *env,
                                                                       jobject thiz) {

    env2 = env;
    instance2 = thiz;

    imprimirticketPrueba();
    pprintf((char *) "final");

}extern "C"
JNIEXPORT void JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_ipVersionamiento(JNIEnv *env,
                                                                               jobject thiz) {
    unsigned char version[20 + 1];
    char dataAux[20 + 1];

    memset(version, 0x00, sizeof(version));
    memset(dataAux, 0x00, sizeof(dataAux));

    strcpy((char *)version, "RBM-A8 ");
    strcat((char *)version, VERSION_SOFTWARE);

    setParameter(VERSION_SW, (char *)version);
    setParameter(REVISION_SW, REVISION_SOFTWARE);

    getParameter(IP_VENTA_PRIMARIA, dataAux);
    setParameter(IP_DEFAULT, dataAux);

    memset(dataAux, 0x00, sizeof(dataAux));
    getParameter(PUERTO_VENTA_PRIMARIA, dataAux);
    setParameter(PUERTO_DEFAULT, dataAux);
}extern "C"
JNIEXPORT jstring JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_mostrarValorReverso(JNIEnv *env,
                                                                          jobject thiz) {
    char valorReverso[21 + 1] = {0x00};
    env2 = env;
    instance2 = thiz;
    int resultado = 0;

    resultado = verificarValorReverso(valorReverso);
    if (resultado <= 0) {
        strcpy(valorReverso, "NOREVERSO");
    }
    return env->NewStringUTF(valorReverso);
}extern "C"
JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_enviarReverso(JNIEnv *env, jobject thiz) {
    env2 = env;
    instance2 = thiz;
    int resultado = 0;
    if (realizarConexionTlS() > 0) {
        resultado = mostrarReverso();
        cerrarConexion();
    } else {
        resultado = -1;
    }
    return resultado;

}

void resultadoTransaccionBancolombia(int resultado, char *respuesta, JNIEnv *env, jobject thiz) {
    char codigoRechazo[50 + 1] = {0x00};

    if (resultado > 0) {

        strcpy(respuesta, datosVentaBancolombia.codigoRespuesta);
        strcat(respuesta, ";");
        strcat(respuesta, "NUMERO DE APROBACION ");
        strcat(respuesta, datosVentaBancolombia.codigoAprobacion);
    } else {
        obtenerStringDeclinada(codigoRechazo, env, thiz);
        strcpy(respuesta, "05");
        strcat(respuesta, ";");
        strcat(respuesta, codigoRechazo);
    }
}

extern "C"
JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_escribirFile(JNIEnv *env, jobject thiz,
                                                                   jstring data, jint tamano) {
    const char *paramData;

    paramData = (env)->GetStringUTFChars(data, JNI_FALSE);
    LOGI("nombreDisco al intentar borrar %s  ",discoNetcom);

    borrarArchivo(discoNetcom, "/PARAMTERMINAL.txt");
    LOGI("nombreDisco escribir file %s  ",discoNetcom);

    escribirArchivo(discoNetcom, "/PARAMTERMINAL.txt", (char *) paramData, tamano);
    return 0;
}extern "C"
JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_cerrarSocket(JNIEnv *env, jobject thiz) {

    return cerrarConexion();
}extern "C"
JNIEXPORT jstring JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_enviarRecaudoTarjetaEmpresarial(JNIEnv *env,
                                                                                      jobject thiz,
                                                                                      jstring track2,
                                                                                      jstring monto) {
    env2 = env;
    instance2 = thiz;
    const char *paramTrack2;
    const char *paramMonto;
    int resultado = 0;
    char respuesta [100 + 1] = {0x00};
    paramTrack2 = (env)->GetStringUTFChars(track2, JNI_FALSE);
    paramMonto = (env)->GetStringUTFChars(monto, JNI_FALSE);

    memset(&datosVentaBancolombia, 0x00, sizeof(datosVentaBancolombia));
    setParameter(HABILITACION_MODO_CNB, (char *) "4");//Temporal de android
    if (realizarConexionTlS() > 0) {
        resultado = gestionarRecaudoTarjetaEmpresarial((char *)paramTrack2, (char *)paramMonto);
        cerrarConexion();
        resultadoTransaccionBancolombia(resultado, respuesta, env, thiz);

    } else {
        strcpy(respuesta, "05;ERROR SIN CONEXION");
    }

    return env->NewStringUTF(respuesta);
}extern "C"
JNIEXPORT jstring JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_obtenerConsecutivoDeTransaccionEMV(
        JNIEnv *env, jclass clazz) {
    char respuesta[10 + 1] = {0x00};
    getParameter(CONSECUTIVO,respuesta);
    return env->NewStringUTF(respuesta);
}extern "C"
JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_verificarTransaccionesBcl(JNIEnv *env,
                                                                                jclass clazz) {
    return  verificarArchivo(discoNetcom, JOURNAL_CNB_BANCOLOMBIA);
}extern "C"
JNIEXPORT void JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_imprimirTransaccionBancolombiaDeclinada(
        JNIEnv *env, jobject thiz, jstring mensaje_cancelacion, jint tipo_transaccion) {
    env2 = env;
    instance2 = thiz;

    const char *paramMensajeCancelacion;
    paramMensajeCancelacion = (env)->GetStringUTFChars(mensaje_cancelacion, JNI_FALSE);
    sprintf(datosVentaBancolombia.tipoTransaccion,"%d",tipo_transaccion);
    imprimirDeclinadas(datosVentaBancolombia, (char *)paramMensajeCancelacion);
}extern "C"
JNIEXPORT jstring JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_obtenerPanRetiroEfectivo(JNIEnv *env, jobject thiz, jstring numero_cuenta) {
    env2 = env;
    instance2 = thiz;
    int resultado = 0;
    char cardName[20 + 1] = {0x00};
    char track2[50 + 1] = {0x00};
    char respuesta[100 + 1] = {0x00};
    char numeroCuenta[20 + 1]= {0x00};

    const char *paramNumeroCuenta;
    paramNumeroCuenta = (env)->GetStringUTFChars(numero_cuenta, JNI_FALSE);

    memcpy(numeroCuenta, (char*)paramNumeroCuenta, strlen((char*)paramNumeroCuenta));
    resultado = armarPanRetiroEfectivobancolombia(track2, numeroCuenta, cardName);
    memcpy(respuesta, track2, 16);
    strcat(respuesta,";");
    strcat(respuesta,cardName);
    return env->NewStringUTF(respuesta);
}extern "C"
JNIEXPORT void JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_obtenerModoCnb(JNIEnv *env, jclass clazz) {

    uChar modo[2 + 1] = {0x00};
    uChar modoCnbCitiBank[1 + 1] = {0x00};
    uChar modoCnbBancolombia[1 + 1]  = {0x00};
    uChar modoCnbCorresponsales[1 + 1] = {0x00};
    uChar modoCnbAval[2 + 1] = {0x00};

    memset(modo, 0x00, sizeof(modo));
    if (atoi(modoCnbCitiBank) == TRUE) {
        sprintf(modo, "%02x", MODO_CNB_CITIBANK);
    } else if (atoi(modoCnbBancolombia) == TRUE || strcmp(modoCnbBancolombia, BCL_JMR) == 0) {
        sprintf(modo, "%02x", MODO_CNB_BANCOLOMBIA);
    } else if (atoi(modoCnbCorresponsales) == TRUE ) {
        sprintf(modo, "%02x", MODO_CNB_CORRESPONSAL);
    }  else if (atoi(modoCnbAval) == MODO_CNB_AVAL) {
        sprintf(modo, "%02x", MODO_CNB_AVAL);
        borrarArchivo(discoNetcom, TOKEN_QC);
    } else {
        sprintf(modo, "%02x", MODO_CNB_RBM);
    }

    setParameter(HABILITACION_MODO_CNB, modo);
}extern "C"
JNIEXPORT void JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_limpiarGlobalesC(JNIEnv *env, jclass clazz) {
    setVariableGlobalesRetiro();
}extern "C"
JNIEXPORT jstring JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_procesoAprobadoEmv(JNIEnv *env, jobject thiz, jint tipo_transaccion) {
    env2 = env;
    instance2 = thiz;
    char respuesta [50 + 1] = {0x00};
    int resultado = 0;
    if(tipo_transaccion == 1){
        mostrarAprobacionBancolombia(&datosVentaBancolombia);
        strcpy(respuesta,"00;");
        strcat(respuesta,datosVentaBancolombia.codigoAprobacion);
    } else {
        //resultado = aprobacionTransaccionVenta("00");
    }
    if(resultado > 0){
        strcpy(respuesta,"00;");
        strcat(respuesta,globalTramaVenta.codigoAprobacion);
    }
    return env->NewStringUTF(respuesta);
}extern "C"
JNIEXPORT jint JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_validarInicializacionProgramada(JNIEnv *env, jobject thiz) {
    return validarInicializacionProgramada();
}
extern "C"
JNIEXPORT void JNICALL
Java_co_com_netcom_corresponsal_core_comunicacion_IntegradorC_cargarInformacionPanVirtual(JNIEnv *env, jobject thiz, jstring fiid,
                                                                                          jstring tipo_cuenta, jstring pan_virtual) {
    const char *paramFidd;
    const char *paramTipoCuenta;
    const char *paramPanVirtual;

    paramFidd = (env)->GetStringUTFChars(fiid, JNI_FALSE);
    paramTipoCuenta = (env)->GetStringUTFChars(tipo_cuenta, JNI_FALSE);
    paramPanVirtual = (env)->GetStringUTFChars(pan_virtual, JNI_FALSE);
    cargarInformacionPanVirtualCorresposal((char *)paramFidd, (char *) paramTipoCuenta,  (char *)paramPanVirtual);
}

void generarStringRespuesta(char * respuesta){
    char dataAux1[1024 + 1] = {0x00};
    char dataAux2[512 + 1] = {0x00};
    _convertBCDToASCII_(dataAux1,globalresultadoIsoPack.isoPackMessage,globalresultadoIsoPack.totalBytes);
    _convertBCDToASCII_(dataAux2,globalresultadoIsoPack.isoPackMessage + (globalresultadoIsoPack.totalBytes + 2)/2,globalresultadoIsoPack.totalBytes -2);
    LOGI("ESTO ES RESULTADO AL dataAux1 %s",dataAux1);
    LOGI("ESTO ES RESULTADO AL respuesta %s",dataAux2);
    LOGI(" TAMNO %d",globalresultadoIsoPack.totalBytes);
    strcpy(respuesta, dataAux1);
    strcat(respuesta, dataAux2);
    borrarArchivo(discoNetcom,(char *) TARJETA_CNB);
}