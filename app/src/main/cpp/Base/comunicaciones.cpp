//
// Created by NETCOM on 26/02/2020.
//

#include "Utilidades.h"
#include "Archivos.h"
//#include <jni.h>
#include <string>
#include "netinet/in.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "android/log.h"
#include "configuracion.h"
#include "comunicaciones.h"
#include "Mensajeria.h"
#include "native-lib.h"
#include "lealtad.h"

#define  LOG_TAG    "NETCOM_COMUNICACIONES "
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define TCP_USER_TIMEOUT 10
int conectado;
int sockfd = 0;
SSL *cSSL;
int maximoIntentos;
struct sockaddr_in sock_add;
struct sockaddr_in server_addr;
char direccionIp[15 + 1];
int puerto = 0;
char puertoTls[4 + 1];
SSL_CTX *sslctx;
int newsockfd;
int  ssl_err;

int
realizarTransaccion(uChar *outputData, int tamanoInBuffer, uChar *inputData, int tipoTransaccion,
                    int cierraCanal, int generaReverso) {

    int n = 1;
    char canal[2 + 1] = {0x00};

    memset(direccionIp, 0x00, sizeof(direccionIp));
    getParameter(TIPO_CANAL, canal);

    n = verificarReversoExistente(outputData, tamanoInBuffer, tipoTransaccion);

    if (n > 0) {
        if (atoi(canal) == 2 && tipoTransaccion != TRANSACCION_PUNTOS_COLOMBIA &&
            tipoTransaccion != TRANSACCION_REVERSO_PUNTOS_COLOMBIA) {
            n = conectarTls(outputData, tamanoInBuffer, inputData, tipoTransaccion, cierraCanal,
                            generaReverso);
        } else {
            LOGI("tipoTransaccion %d", tipoTransaccion);
            n = realizarTransaccionTCP(outputData, tamanoInBuffer, inputData, tipoTransaccion,
                                       cierraCanal, generaReverso);
        }
    }
    LOGI("esto es n %d", n);
    if(n == -3){
        enviarStringToJava("TIME OUT GENERAL",(char *) "showScreenMessage");
    }
    return n;
}

void releaseSocket() {
    conectado = 0;
    close(sockfd);
}

void InitializeSSL() {
    SSL_load_error_strings();
    SSL_library_init();
    OpenSSL_add_all_algorithms();
}

void DestroySSL() {
    ERR_free_strings();
    EVP_cleanup();
}

void ShutdownSSL(SSL *s) {

    SSL_set_shutdown(s, SSL_SENT_SHUTDOWN | SSL_RECEIVED_SHUTDOWN);
    int r = SSL_shutdown(s);

    if(!r){
        shutdown(newsockfd, SHUT_RDWR);
        SSL_shutdown(s);
        LOGI("close ");
    }

    r = SSL_clear(s);
    LOGI("SSL_clear %d",r);
    SSL_free(s);
    LOGI("SSL_free %d",r);
}


int load_ca_der_or_pem(SSL_CTX *ctx, const char *ca_cert) {
    X509_LOOKUP *lookup = NULL;
    int ret = 0;
    lookup = X509_STORE_add_lookup(ctx->cert_store, X509_LOOKUP_file());
    if (lookup == NULL) {
        ERR_print_errors_fp(stderr);
        return -1;
    }

    {
        if (!X509_LOOKUP_ctrl(lookup, X509_L_FILE_LOAD, ca_cert, X509_FILETYPE_PEM, NULL)) {
            unsigned long err = ERR_peek_error();
            if (ERR_GET_LIB(err) == ERR_LIB_X509 &&
                ERR_GET_REASON(err) == X509_R_CERT_ALREADY_IN_HASH_TABLE) {

            } else
                ret = -1;
            ERR_print_errors_fp(stderr);
        }
    }
    return ret;
}

int conectarTls(uChar *message, int len, uChar *dataRecibida, int tipoTransaccion,
                int cierraCanal, int generaReverso) {

    char address[15 + 1];
    char puertoTls[4 + 1];
    char auxiliar[512 + 1];
    int n = 0;
    int error = 0;
    int contador = 0;
    struct timeval tv;
    tv.tv_sec = 30;
    tv.tv_usec = 0;

    memset(address, 0x00, sizeof(address));
    memset(puertoTls, 0x00, sizeof(puertoTls));
    memset(auxiliar, 0x00, sizeof(auxiliar));

    char buf[1024];

    memset(buf, 0, sizeof(buf));

    if (tipoTransaccion != TRANSACCION_REVERSO && generaReverso == REVERSO_GENERADO &&
        ssl_err >= 0) {
        if (len > 0) {
            escribirArchivo(discoNetcom, (char *) ARCHIVO_REVERSO, message, len);
            /*if (transaccion == TRANSACCION_MCBA_RECAUDO || transaccion == TRANSACCION_MCBC_RECAUDO) {
                crearReversosRecaudosMulticorresponsal((unsigned char*)dataEnviarPlana, lengthDataEnviarPlana,
                                                       //REVERSOS_RECAUDOS_MULT);
            } else if (tTransaccion == TRANSACCION_BCL_RECAUDO
                       && strcmp(iniciaTransaccion, RECAUDO_CAJA_NUEVO_SERVIDOR) == 0) {
                crearReversosRecaudosMulticorresponsal((unsigned char *)dataEnviarPlana, lengthDataEnviarPlana,
                                                       REVERSOS_RECAUDOS_NUEVO_SERVIDOR);
            }*/
        }
    }

    LOGI("tipoTransaccion %d ", tipoTransaccion);
    //interpretarTrama(message, len * 2);
    if (tipoTransaccion != TRANSACCION_INICIALIZACION && tipoTransaccion != TRANSACCION_CIERRE && tipoTransaccion != TRANSACCION_DEFAULT) {

        LOGI("escribiendo y a java");
        enviarStringToJava("PROCESANDO", "mostrarInicializacion");
    }

    n = SSL_write(cSSL, message, len);


    if(setsockopt(newsockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv) < 0){
    }


    do {
        n = SSL_read(cSSL, dataRecibida, 2048);

        LOGI("leyendo %d ",n);

        if (n > 0) {
            //interpretarTrama(dataRecibida, n * 2);
        }
        usleep(200);
        if (n <= 0) {
            error = SSL_get_error(cSSL, n);
            switch (error) {
                case SSL_ERROR_WANT_READ:
                    usleep(10);
                    break;
                case SSL_ERROR_ZERO_RETURN://TIMEOUT
                    n = -3;
                    break;
                default:
                    ERR_print_errors_fp(stderr);
            }
        }
        contador++;
        if (contador == 10) {
            n = -3;
        }
    } while (n == 0);
    if(n == -1){
        n = -3;
    }
    //}
    return n;
}

int realizarTransaccionTCP(uChar *message, int len, uChar *dataRecibida, int tipoTransaccion,
                           int cierraCanal, int generaReverso) {
    char auxiliar[512 + 1];
    int n = 0;
    int contador = 0;
    int connfd = 0;
    struct timeval tv;
    tv.tv_sec = 30;
    tv.tv_usec = 0;
    memset(auxiliar, 0x00, sizeof(auxiliar));

    leerParametros(tipoTransaccion);
    if (conectado == 0) {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            return -3;
        }

    }

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(puerto);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    if (inet_pton(AF_INET, direccionIp, &server_addr.sin_addr) < 0) {
        //return "address error";
    }
    if (conectado == 0) {
        socklen_t server_addr_length = sizeof(server_addr);
        connfd = connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
        if (connfd < 0) {
            //return "connect error";
        } else {
            conectado = 1;
        }
    }

    if (tipoTransaccion != TRANSACCION_REVERSO_PUNTOS_COLOMBIA && tipoTransaccion != TRANSACCION_REVERSO && generaReverso == REVERSO_GENERADO &&
        connfd >= 0) {
        if (len > 0) {
            if (tipoTransaccion == TRANSACCION_PUNTOS_COLOMBIA) {
                crearReversoPuntosColombia();
            } else {
                LOGI("Escribir reverso ppor TCPyP ");
                escribirArchivo(discoNetcom, (char *) ARCHIVO_REVERSO, message, len);
                /*if (transaccion == TRANSACCION_MCBA_RECAUDO || transaccion == TRANSACCION_MCBC_RECAUDO) {
                    crearReversosRecaudosMulticorresponsal((unsigned char*)dataEnviarPlana, lengthDataEnviarPlana,
                                                           //REVERSOS_RECAUDOS_MULT);
                } else if (tTransaccion == TRANSACCION_BCL_RECAUDO
                           && strcmp(iniciaTransaccion, RECAUDO_CAJA_NUEVO_SERVIDOR) == 0) {
                    crearReversosRecaudosMulticorresponsal((unsigned char *)dataEnviarPlana, lengthDataEnviarPlana,
                                                           REVERSOS_RECAUDOS_NUEVO_SERVIDOR);
                }*/
            }

        }
    }


    //char *msg = "i send a data to server. \n";
    int sdf = send(sockfd, message, len, 0);
    if(setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv) < 0){
        LOGI("nada recibido  %d ", contador);
    }
    if (tipoTransaccion != TRANSACCION_INICIALIZACION && tipoTransaccion != TRANSACCION_CIERRE) {
        enviarStringToJava("PROCESANDO", "mostrarInicializacion");
    }
    //interpretarTrama(message, len * 2);
    //char buff[4096];
    do {
        LOGI("contador antes %d ", contador);
        n = recv(sockfd, dataRecibida, 1024, 0);
        LOGI("n saliendo de socket  %d ", n);
        if(n < 0){
            if (errno != EWOULDBLOCK)
            {
                LOGI("pasa por error  despues  %d ", contador);
                perror("  recv() failed");
                n = -3;
            }
            break;
        }
        LOGI("contador despues  %d ", contador);
        contador++;
        if (contador == 100) {
            n = -3;
        }
    } while (n == 0);
    if (n > 0) {
        //interpretarTrama(dataRecibida, n * 2);
    }
    if(n == -1){
        n= -3;
    }
    //_convertBCDToASCII_(auxiliar,dataRecibida,(n * 2));
    dataRecibida[n] = 0;
    if (cierraCanal == CANAL_DEMANDA) {
        releaseSocket();
    }
    return n;
}


/**
 * Envia y recibe el reverso de alguna transaccion.
 * @return 1 OK.
 */
int generarReverso(void) {

    ResultISOPack resultadoIsoPackMessage;
    ResultISOUnpack resultadoUnpack;
    ISOFieldMessage isoFieldMessage;
    ISOHeader isoHeader8583;
    uChar niiApdu[4 + 1];
    uChar dataReversoRecibido[512 + 1];
    uChar dataReversoEnviado[1024 + 1];
    uChar codigoRespuesta[2 + 1];
    uChar codigoAutorizacion[10 + 1];
    uChar transaccion[2 + 1];
    char mensajeError[100 + 1] = {0x00};
    int tamReversoEnviado = 0;
    int resultado = 0;
    int numIntentos = 0;
    int idx = 0;

    memset(codigoAutorizacion, 0x00, sizeof(codigoAutorizacion));
    memset(codigoRespuesta, 0x00, sizeof(codigoRespuesta));
    memset(dataReversoRecibido, 0x00, sizeof(dataReversoRecibido));
    memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
    memset(&resultadoUnpack, 0x00, sizeof(resultadoUnpack));
    memset(&isoHeader8583, 0x00, sizeof(isoHeader8583));
    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(dataReversoEnviado, 0x00, sizeof(dataReversoEnviado));
    memset(niiApdu, 0x00, sizeof(niiApdu));
    memset(transaccion, 0x00, sizeof(transaccion));
    LOGI("generar reverso ");
    getParameter(TIPO_TRANSACCION_REVERSO, transaccion);
    tamReversoEnviado = leerArchivo(discoNetcom, (char *) ARCHIVO_REVERSO, dataReversoEnviado);

    if (tamReversoEnviado > 0) {

        memcpy(codigoRespuesta, dataReversoEnviado + 3, 2);
        sprintf(niiApdu, "%02x%02x", codigoRespuesta[0], codigoRespuesta[1]);
        resultadoUnpack = unpackISOMessage(dataReversoEnviado, tamReversoEnviado);
        isoHeader8583.TPDU = 60;
        memcpy(isoHeader8583.destination, niiApdu, TAM_NII + 1);
        memcpy(isoHeader8583.source, niiApdu, TAM_NII + 1);
        setHeader(isoHeader8583);

        for (idx = 0; idx < resultadoUnpack.totalField; idx++) {
            memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
            isoFieldMessage = getField(resultadoUnpack.listField[idx]);
            //Si no envia el pin, no debe enviar el campo 53 tampoco
            if (resultadoUnpack.listField[idx] != 52 && resultadoUnpack.listField[idx] != 53) {
                setField(resultadoUnpack.listField[idx], isoFieldMessage.valueField,
                         isoFieldMessage.totalBytes);
            } else if (atoi(transaccion) == TRANSACCION_PAGO_ELECTRONICO) {
                setField(resultadoUnpack.listField[idx], isoFieldMessage.valueField,
                         isoFieldMessage.totalBytes);
            }
        }

        setMTI((char *) "0400");
        resultadoIsoPackMessage = packISOMessage();

        //do{

        memset(dataReversoRecibido, 0x00, sizeof(dataReversoRecibido));
        resultado = realizarTransaccion(resultadoIsoPackMessage.isoPackMessage,
                                        resultadoIsoPackMessage.totalBytes,
                                        dataReversoRecibido, TRANSACCION_REVERSO, CANAL_DEMANDA, 0);

        if (resultado > 0) {

            memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));

            resultadoUnpack = unpackISOMessage(dataReversoRecibido, resultado);

            isoFieldMessage = getField(39);
            memset(codigoRespuesta, 0x00, sizeof(codigoRespuesta));
            memcpy(codigoRespuesta, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

            // verificar trama de reverso recibida
            resultado = verificacionTramaReverso(dataReversoEnviado, tamReversoEnviado);
            LOGI("Reverso resultado %d  ",resultado);
            if (resultado > 0) {

                if (strcmp(codigoRespuesta, "00") == 0) {
                    borrarArchivo(discoNetcom, (char *) ARCHIVO_REVERSO);
                    setParameter(REVERSOS_ENVIADOS, (char *) "0");
                    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
                } else {
                    setParameter(REVERSOS_ENVIADOS, (char *) "0");
                    resultado = 1;

                    borrarArchivo(discoNetcom, (char *) ARCHIVO_REVERSO);
                }
                if (atoi(transaccion) == TRANSACCION_PAGO_ELECTRONICO) {
                    setParameter(TIPO_TRANSACCION_REVERSO, (char *) "00");
                }
            } else {
                numIntentos = maximoIntentos;
                LOGI("Reverso Pendiente  ");
                memset(codigoRespuesta, 0x00, sizeof(codigoRespuesta));
                strcpy(codigoRespuesta,"73");
                enviarStringToJava("REVERSO PENDIENTE", "mostrarInicializacion");
                errorRespuestaTransaccion(codigoRespuesta, mensajeError);
                // printMessage(strlen(MESSAGE_REVERSO_PENDIENTE), MESSAGE_REVERSO_PENDIENTE);
            }
        }
        numIntentos++;

    }

    return resultado;
}

int verificacionTramaReverso(uChar *dataEnviada, int inLength) {
    int resultado = -1;
    uChar msgType[TAM_MTI + 1];
    uChar processingCode[TAM_CODIGO_PROCESO + 1];
    uChar systemTrace[TAM_SYSTEM_TRACE + 1];
    uChar terminalId[TAM_TERMINAL_ID + 1];
    uChar stringAux[20];
    ResultISOUnpack resultadoUnpack;
    ISOFieldMessage isoFieldMessage;

    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(msgType, 0x00, sizeof(msgType));
    memset(processingCode, 0x00, sizeof(processingCode));
    memset(systemTrace, 0x00, sizeof(systemTrace));
    memset(terminalId, 0x00, sizeof(terminalId));
    memset(stringAux, 0x00, sizeof(stringAux));
    memset(&resultadoUnpack, 0x00, sizeof(resultadoUnpack));

    isoFieldMessage = getMTI();
    memcpy(msgType, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

    if ((strncmp("0410", msgType, TAM_MTI) == 0)) {

        isoFieldMessage = getField(3);
        memcpy(processingCode, isoFieldMessage.valueField, 6);

        isoFieldMessage = getField(11);
        memcpy(systemTrace, isoFieldMessage.valueField, 6);

        isoFieldMessage = getField(41);
        memcpy(terminalId, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

        resultadoUnpack = unpackISOMessage(dataEnviada, inLength);

        resultado = 1;
    } else {
        resultado = -1;
    }

    return resultado;
}

int verificarReversoExistente(uChar *outputData, int tamanoInBuffer, int tipoTransaccion) {

    int reverso = 0;
    int resultado = 1;
    int inputDataAuxiliar = 0;

    if (tipoTransaccion != TRANSACCION_REVERSO &&
        tipoTransaccion != TRANSACCION_REVERSO_PUNTOS_COLOMBIA) {
        setearTransaccion(tipoTransaccion);
        LOGI("seteo transaccion ");
        reverso = verificarArchivo(discoNetcom, (char *) ARCHIVO_REVERSO);
        if (reverso == FS_OK) {
            LOGI("cantidadBytes existe reverso");
            borrarArchivo(discoNetcom, (char *) TEMPORAL);
            escribirArchivo(discoNetcom, (char *) TEMPORAL, outputData, tamanoInBuffer);
            resultado = generarReverso();
        }

        reverso = verificarArchivo(discoNetcom, REVERSO_PUNTOS_COLOMBIA);
        if (reverso == FS_OK) {
            LOGI(" existe reverso puntos colombia");
            resultado = anulacionPuntosColombia(2);
        }
    }
    LOGI(" salio %d",resultado);
    return resultado;
}

int echoTest() {
    int resultadoTransaccion = 0;

    uChar codigoRespuesta[2 + 1];
    uChar dataRecibida[512];
    uChar terminal[8 + 1];
    uChar nii[4 + 1];
    ISOHeader isoHeader8583;
    ResultISOPack resultadoIsoPackMessage;
    ResultISOUnpack resultadoUnpack;
    ISOFieldMessage isoFieldMessage;

    memset(nii, 0x00, sizeof(nii));
    memset(terminal, 0x00, sizeof(terminal));
    memset(dataRecibida, 0x00, sizeof(dataRecibida));
    memset(codigoRespuesta, 0x00, sizeof(codigoRespuesta));
    memset(&resultadoUnpack, 0x00, sizeof(resultadoUnpack));
    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(&isoHeader8583, 0x00, sizeof(isoHeader8583));
    memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));


    getParameter(NII, nii);
    getParameter(NUMERO_TERMINAL, terminal);

    isoHeader8583.TPDU = 60;
    memcpy(isoHeader8583.destination, nii, TAM_NII + 1);
    memcpy(isoHeader8583.source, nii, TAM_NII + 1);
    setHeader(isoHeader8583);

    setMTI("0800");
    setField(3, "990000", 6);
    setField(24, nii + 1, 3); // INGRESAR NII DEL TERMINAL
    setField(41, terminal, 8); // INGRESAR NUMERO DE TERMINAL

    resultadoIsoPackMessage = packISOMessage();

    if (resultadoIsoPackMessage.responseCode > 0) {

        setParameter(VERIFICAR_PAPEL_TERMINAL, "00");
        resultadoTransaccion = realizarTransaccion(resultadoIsoPackMessage.isoPackMessage,
                                                   resultadoIsoPackMessage.totalBytes, dataRecibida,
                                                   TRANSACCION_DEFAULT, CANAL_DEMANDA,
                                                   REVERSO_NO_GENERADO);
        setParameter(VERIFICAR_PAPEL_TERMINAL, "01");
    }

    if (resultadoTransaccion > 0) {
        resultadoTransaccion = 1;
    }

    return resultadoTransaccion;
}

int procesarRespuesta(char *dataRecibida, int lengtData) {

    ResultISOUnpack resultadoUnpack;
    ISOFieldMessage isoFieldMessage;
    char codigoRespuesta[2 + 1];
    char working3DES[17 + 1];
    int idx = 0;
    int iRet = 0;
    int existeP63 = 0;
    int resultado = 0;

    memset(&resultadoUnpack, 0x00, sizeof(resultadoUnpack));
    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(codigoRespuesta, 0x00, sizeof(codigoRespuesta));
    memset(working3DES, 0x00, sizeof(working3DES));

    resultadoUnpack = unpackISOMessage(dataRecibida, lengtData);
    isoFieldMessage = getField(39);
    memcpy(codigoRespuesta, isoFieldMessage.valueField, 2);

    /////// VALIDAR CODIGO DE RESPUESTA ////////
    if (strncmp(codigoRespuesta, "00", 2) == 0) {
        resultado = 1;
    }

    //////// VERIFICAR CAMPO 63 /////////
    if (resultado > 0) {
        for (idx = 0; idx < resultadoUnpack.totalField; idx++) {
            if (resultadoUnpack.listField[idx] == 63) {
                existeP63 = 1;
                break;
            }
        }
    }

    ///////// EXTRAER WORKING KEY /////////
    if (resultado > 0) {
        working3DES[0] = '1';
        if (existeP63 > 0) {
            isoFieldMessage = getField(63);
            memcpy(working3DES + 1, isoFieldMessage.valueField + 4, 16);
        } else {
            isoFieldMessage = getField(62);
            memcpy(working3DES + 1, isoFieldMessage.valueField, 8);
            memcpy(working3DES + 9, isoFieldMessage.valueField, 8);
        }

        setParameter(WORKING_KEY_LOGON_3DES, working3DES);

        if (strlen(working3DES) == 17) {
            iRet = 1;
            //           printMessage(strlen(COMUNICACION_EXITOSA), COMUNICACION_EXITOSA);
        } else {
            iRet = 0;
            //           screenMessage("COMUNICACION", "ERROR EN COMUNICACION", "REINTENTANDO", "TRANSACCION LOGON", 3 * 1000);
        }

    }
    return iRet;
}

int logon3DES() {
    int resultado = 0;
    ResultISOPack resultadoIsoPackMessage;
    ISOHeader isoHeader8583;
    uChar terminal[8 + 1];
    uChar dataRecibida[512 + 1];
    uChar nii[4 + 1];

    memset(terminal, 0x00, sizeof(terminal));
    memset(nii, 0x00, sizeof(nii));
    memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
    memset(&isoHeader8583, 0x00, sizeof(isoHeader8583));
    memset(dataRecibida, 0x00, sizeof(dataRecibida));

    getParameter(NUMERO_TERMINAL, terminal);
    getParameter(NII, nii);

    /////// ARMAR TRAMA LOGON ////////
    isoHeader8583.TPDU = 60;
    memcpy(isoHeader8583.destination, nii, TAM_NII + 1);
    memcpy(isoHeader8583.source, nii, TAM_NII + 1);
    setHeader(isoHeader8583);

    setMTI((char *)"0800");
    setField(3, (char *)"920000", 6);
    setField(24, nii + 1, 3);
    setField(41, terminal, 8);

    ////// EMPAQUETAR TRAMA ////////
    resultadoIsoPackMessage = packISOMessage();

    ////// ENVIAR TRAMA ///////
    if (resultadoIsoPackMessage.responseCode > 0) {
        setParameter(VERIFICAR_PAPEL_TERMINAL, (char *)"00");
        resultado = realizarTransaccion(resultadoIsoPackMessage.isoPackMessage,
                                        resultadoIsoPackMessage.totalBytes,
                                        dataRecibida, TRANSACCION_DEFAULT, CANAL_DEMANDA,
                                        REVERSO_NO_GENERADO);
    }

    /////// PROCESAR RESPUESTA ///////
    if (resultado > 0) {
        resultado = procesarRespuesta(dataRecibida, resultado);
    } else {
        resultado = 0;
    }
    return resultado;
}

void leerParametros(int tipoTransaccion) {

    if (tipoTransaccion == TRANSACCION_PUNTOS_COLOMBIA || tipoTransaccion == TRANSACCION_REVERSO_PUNTOS_COLOMBIA) {
        definirParametrosPuntosColombia();
    } else {
        definirParametrosDefault();
    }
}

void definirParametrosPuntosColombia() {
    char puertoPC[5 + 1] = {0x00};
    char dominio[20 + 1] = {0x00};
    getParameter(PUERTO_PUNTOS_COLOMBIA, puertoPC);
    getParameter(IP_PUNTOS_COLOMBIA, dominio);

    memset(direccionIp, 0x00, sizeof(direccionIp));
    memcpy(direccionIp, dominio, strlen(dominio));

    puerto = atoi(puertoPC);
}

void definirParametrosDefault() {
    char ipDefault[15 + 1] = {0x00};
    char puertoDefault[5 + 1] = {0x00};
    getParameter(IP_DEFAULT, ipDefault);
    getParameter(PUERTO_DEFAULT, puertoDefault);
    LOGI("ipDefault, %s ",ipDefault);
    memset(direccionIp, 0x00, sizeof(direccionIp));
    memcpy(direccionIp, ipDefault, strlen(ipDefault));

    puerto = atoi(puertoDefault);
}

void crearReversoPuntosColombia() {
    char dataLeida[1024 + 1] = {0x00};
    leerArchivo(discoNetcom, (char *) REVERSO_AUXILIAR_PUNTOS_COLOMBIA, dataLeida);
    escribirArchivo(discoNetcom, (char *) REVERSO_PUNTOS_COLOMBIA, dataLeida, sizeof(DatosVenta));
    borrarArchivo(discoNetcom, (char *) REVERSO_AUXILIAR_PUNTOS_COLOMBIA);
}

int realizarConexionTlS(){
    int n = 1;
    char canal[2 + 1] = {0x00};

    memset(direccionIp, 0x00, sizeof(direccionIp));
    getParameter(TIPO_CANAL, canal);
    if (atoi(canal) == 2 ) {
        n =  conexionSSL();
    } else {
        n = realizarConexionTCP();
    }
    return n;
}

int cerrarConexion(){

    char canal[2 + 1] = {0x00};
    //int iRet = 0;
    memset(direccionIp, 0x00, sizeof(direccionIp));
    getParameter(TIPO_CANAL, canal);

    if (atoi(canal) == 2 ) {
        LOGI("ShutdownSSL ");
        ShutdownSSL(cSSL);
        LOGI("Shutdown ");
        close(newsockfd);
        LOGI("DestroySSL ");
        DestroySSL();
    } else {
        releaseSocket();
    }
    LOGI("Cerro conexion ");

    return 1;
}

int conexionSSL(){

    char pathCertificado[50 + 1] = {0x00};

    getParameter(IP_DEFAULT, direccionIp);
    getParameter(PUERTO_DEFAULT, puertoTls);

    InitializeSSL();
    verificarArchivo(discoNetcom, "/caroot.pem");

    strcpy(pathCertificado, discoNetcom);
    strcat(pathCertificado, "/caroot.pem");
    sslctx = SSL_CTX_new(TLSv1_2_client_method());

    SSL_CTX_set_verify_depth(sslctx, 4);
    SSL_CTX_ctrl(sslctx, SSL_CTRL_OPTIONS, SSL_OP_ALL, NULL);

    SSL_load_error_strings();

    load_ca_der_or_pem(sslctx, pathCertificado);
    cSSL = SSL_new(sslctx);

    newsockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (newsockfd < 0) {
        return -1;
        //Log and Error
        // return;
    }
    SSL_set_fd(cSSL, newsockfd);
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(puertoTls));
    server_addr.sin_addr.s_addr = INADDR_ANY;
    if (inet_pton(AF_INET, direccionIp, &server_addr.sin_addr) < 0) {
        //return "address error";
    } else {
        LOGI("CONECTANDO ");
        connect(newsockfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
        //Here is the SSL Accept portion.  Now all reads and writes must use SSL
        ssl_err = SSL_connect(cSSL);

    }

    LOGI("ssl_err %d ", ssl_err);
    if (ssl_err <= 0) {
        int ret = SSL_get_error(cSSL, ssl_err);
        char error_buffer[1024] = "";
        ERR_error_string(ret, error_buffer);
        int no = errno;
        //Error occurred, log and close down ssl
        cerrarConexion();
        return -1;
    }

    return 1;
}
int realizarConexionTCP(){
    int connfd = 0;

    if (conectado == 0) {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            return -1;
        }
        leerParametros(0);
    }

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(puerto);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    if (inet_pton(AF_INET, direccionIp, &server_addr.sin_addr) < 0) {
        //return "address error";
    }
    if (conectado == 0) {
        socklen_t server_addr_length = sizeof(server_addr);
        connfd = connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
        if (connfd < 0) {
            //return "connect error";
        } else {
            conectado = 1;
        }
    }
    return conectado;
}