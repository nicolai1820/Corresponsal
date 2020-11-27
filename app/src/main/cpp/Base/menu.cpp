//
// Created by ronald on 22/04/2020.
//
#include "Utilidades.h"
#include "Archivos.h"
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
#include "Inicializacion.h"
#include "native-lib.h"
#include "Mensajeria.h"
#include "comunicaciones.h"

#define  LOG_TAG    "NETCOM_MENUS_GUI_CONFIGURACIONES"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define SEPARADOR '.'

char niiFirma[60][4 + 1] = {0x00};
char ipOrigen[60][15 + 1] = {0x00};
char ipDestino[60][15 + 1] = {0x00};
char puertoFirma[60][5 + 1] = {0x00};
char nombreMenuAux[10][20 + 1] = {0x00};
char nombreValAux[10][20 + 1] = {0x00};
char metodoSolicitud[50][60 + 1] = {0x00};
char metodoRespuesta[50][60 + 1] = {0x00};
char host[50][30 + 1] = {0x00};
char auxiliar[60 + 1] = {0x00};

int solicitudMenuProducto(int producto) {

    int idx = 0;
    int idy = 0;
    int idz = 0;
    int tam = 0;
    int iRet = 0;
    int nitem = 0;
    int nItemIP = 0;
    int salida = 0;
    int indiceFile = 0;
    int contadorLineas = 0;
    int contadorItems = 0;

    char desconcatenar[80 + 1] = {0x00};
    char id[60][4 + 1] = {0x00};
    char titulo[30 + 1] = {0x00};
    char eftsec[60][6 + 1] = {0x00};
    char tcpIpActual[15 + 1] = {0x00};

    char itemsMenu[2048 + 1] = {0x00};
    unsigned char *archivoFirmas = NULL;
    memset(auxiliar, 0x00, sizeof(auxiliar));
    ITEM_MENU menuFirmas[99];
    LineaUrlFirma productoWeb;

    memset(&menuFirmas, 0x00, sizeof(menuFirmas));

    memset(&productoWeb, 0x00, sizeof(productoWeb));


    tam = tamArchivo(discoNetcom, (char *) ARCHIVO_URL_FIRMA);

    archivoFirmas = (unsigned char *) malloc(tam);
    memset(archivoFirmas, 0x00, sizeof(archivoFirmas));

    getParameter(IP_DEFAULT, tcpIpActual);
    strcpy(itemsMenu, ";");
    if (tam > 0) {
        leerArchivo(discoNetcom, (char *) ARCHIVO_URL_FIRMA, (char *) archivoFirmas);

        do {
            do {
                memset(desconcatenar, 0x00, sizeof(desconcatenar));
                do {
                    desconcatenar[idy] = archivoFirmas[idx];
                    idx++;
                    idy++;
                } while (archivoFirmas[idx] != ';');
                if (contadorItems == 0) {
                    memcpy(id[nitem], desconcatenar, 4);
                }
                if (atoi(id[nitem]) == 99) {
                    salida = 1;
                    contadorItems = 10;
                } else {
                    if (atoi(id[nitem]) == producto || atoi(id[nitem]) == 4) {
                        if (contadorItems == 1) {
                            memcpy(eftsec[nitem], desconcatenar, strlen(desconcatenar));
                        } else if (contadorItems == 2) {
                            memcpy(ipOrigen[nitem], desconcatenar, strlen(desconcatenar));
                        } else if (contadorItems == 3) {
                            memcpy(ipDestino[nitem], desconcatenar, strlen(desconcatenar));
                        } else if (contadorItems == 4) {
                            memcpy(puertoFirma[nitem], desconcatenar, strlen(desconcatenar));
                        } else if (contadorItems == 5) {
                            memcpy(niiFirma[nitem], desconcatenar, strlen(desconcatenar));
                        } else if (contadorItems == 6 && atoi(id[nitem]) != 4) {
                            memcpy(menuFirmas[nitem].textoPantalla, desconcatenar,
                                   strlen(desconcatenar));
                            sprintf(menuFirmas[nitem].valor, "%d", nitem);
                        } else if (contadorItems == 7) {
                            memcpy(metodoSolicitud[nitem], desconcatenar, strlen(desconcatenar));
                        } else if (contadorItems == 8) {
                            memcpy(metodoRespuesta[nitem], desconcatenar, strlen(desconcatenar));
                        } else if (contadorItems == 9) {
                            memcpy(host[nitem], desconcatenar, strlen(desconcatenar));
                            if (atoi(id[nitem]) == 04) {
                                indiceFile = 1;
                                if (strcmp(eftsec[nitem], "EFTSEC") == 0) {
                                    memcpy(productoWeb.encriptado, EFTSEC, indiceFile);
                                } else if (strcmp(eftsec[nitem], "TLS") == 0) {
                                    memcpy(productoWeb.encriptado, TLS, indiceFile);
                                } else {
                                    memcpy(productoWeb.encriptado, "0", indiceFile);
                                }
                                indiceFile++;

                                indiceFile = strlen(ipDestino[nitem]);
                                memcpy(productoWeb.ipDestino, ipDestino[nitem], indiceFile);

                                indiceFile = strlen(puertoFirma[nitem]);
                                memcpy(productoWeb.puerto, puertoFirma[nitem], indiceFile);

                                indiceFile = strlen(niiFirma[nitem]);
                                memcpy(productoWeb.nii, niiFirma[nitem], indiceFile);

                                indiceFile = strlen(metodoSolicitud[nitem]);
                                memcpy(productoWeb.metodo, metodoSolicitud[nitem], indiceFile);

                                indiceFile = strlen(host[nitem]);
                                memcpy(productoWeb.host, host[nitem], indiceFile);

                                borrarArchivo(discoNetcom, (char *) METODO_SOLICITUD_SERVICIO_WEB);
                                escribirArchivo(discoNetcom, (char *) METODO_SOLICITUD_SERVICIO_WEB,
                                                (char *) &productoWeb,
                                                sizeof(LineaUrlFirma));
                            }
                            if (atoi(id[nitem]) != 4) {
                                nitem++;
                            }
                        }

                    }
                    idy = 0;
                    idx++;
                    contadorItems++;
                }
            } while (contadorItems < 10);
            contadorItems = 0;
            contadorLineas++;
        } while (salida == 0);
        for (idz = 0; idz < nitem; idz++) {
            memcpy(nombreMenuAux[idz], menuFirmas[idz].textoPantalla,
                   strlen(menuFirmas[idz].textoPantalla));
            memcpy(nombreValAux[idz], menuFirmas[idz].valor, strlen(menuFirmas[idz].valor));
            if (strcmp(ipOrigen[idz], tcpIpActual) == 0) {
                memset(menuFirmas[nItemIP].textoPantalla, 0x00,
                       sizeof(menuFirmas[nItemIP].textoPantalla));
                memset(menuFirmas[nItemIP].valor, 0x00, sizeof(menuFirmas[nItemIP].valor));

                memcpy(menuFirmas[nItemIP].textoPantalla, nombreMenuAux[idz],
                       strlen(nombreMenuAux[idz]));
                memcpy(menuFirmas[nItemIP].valor, nombreValAux[idz], strlen(nombreValAux[idz]));
                strcat(itemsMenu, menuFirmas[nItemIP].textoPantalla);
                strcat(itemsMenu, ";");
                nItemIP++;
            }
        }

        if (nItemIP > 0) {
            showMenuEspecial(titulo, nItemIP, itemsMenu);
        } else {
            //screenMessage(titulo, "PRODUCTO", "NO", "DISPONIBLE", 800);
            iRet = -1;
        }
    }
    free(archivoFirmas);
    return iRet;
}

void showMenuEspecial(char *titulo, int nitems, char *items) {

    char arreglo[100 + 1] = {0x00};

    sprintf(arreglo, "%s%d%s", titulo, nitems, items);
    enviarStringToJava((char *) arreglo, (char *) "mostrarMenu");
}

int recibirOpcionMenuProducto(int producto, int iRet) {


    int nii = 0;
    int pEftsec = 0;
    int puerto = 0;
    int valHost = 0;

    int direccioIpDestino = 0;

    char titulo[30 + 1] = {0x00};
    char eftsec[60][6 + 1] = {0x00};

    memset(auxiliar, 0x00, sizeof(auxiliar));

    LOGI("Netcom Linea reporte %d", iRet);

    if (producto == PRODUCTO_PCOLOMBIA) {
        direccioIpDestino = IP_PUNTOS_COLOMBIA;
        nii = NII_PUNTOS_COLOMBIA;
        puerto = PUERTO_PUNTOS_COLOMBIA;
        pEftsec = EFTSEC_PUNTOS_COLOMBIA;
        valHost = DOMINIO_PUNTOS_COLOMBIA;
        strcpy(titulo, "PUNTOS COLOMBIA;");
    } else if (producto == PRODUCTO_FIRMA_IP) {
        direccioIpDestino = IP_FIRMA;
        nii = NII_FIRMA;
        puerto = ID_FIRMA;
        pEftsec = EFTSEC_FIRMA;
        valHost = URL_FIRMA;
        strcpy(titulo, "SERVIDOR DE FIRMA;");
    } else if (producto == PRODUCTO_CAJA_IP) {
        direccioIpDestino = IP_CAJAS;
        nii = NII_TEFF;
        puerto = PUERTO_TEFF;
        pEftsec = EFTSEC_TEFF;
        valHost = URL_TEFF;
        strcpy(titulo, "CAJAS IP;");
    }
    setParameter(direccioIpDestino, ipDestino[iRet]);
    setParameter(nii, niiFirma[iRet]);
    setParameter(puerto, puertoFirma[iRet]);
    LOGI("IPDestino %s", ipDestino[iRet]);
    LOGI("NII %s", niiFirma[iRet]);
    LOGI("puerto %s", puertoFirma[iRet]);
    if (strcmp(eftsec[iRet], (char *) "EFTSEC") == 0) {
        setParameter(pEftsec, (char *) EFTSEC);
    } else if (strcmp(eftsec[iRet], (char *) "TLS") == 0) {
        setParameter(pEftsec, (char *) TLS);
    } else {
        setParameter(pEftsec, (char *) "0");
    }

    if (producto == PRODUCTO_PCOLOMBIA) {
        borrarArchivo(discoNetcom, (char *) METODO_SOLICITUD_PUNTOS_COLOMBIA);
        escribirArchivo(discoNetcom, (char *) METODO_SOLICITUD_PUNTOS_COLOMBIA,
                        metodoSolicitud[iRet],
                        strlen(metodoSolicitud[iRet]));
    } else if (producto == PRODUCTO_FIRMA_IP) {
        borrarArchivo(discoNetcom, (char *) METODO_CAJA_RESPUESTA);
        borrarArchivo(discoNetcom, (char *) METODO_ENVIO_FIRMA);
        escribirArchivo(discoNetcom, (char *) METODO_CAJA_RESPUESTA, metodoSolicitud[iRet],
                        strlen(metodoSolicitud[iRet]));
        escribirArchivo(discoNetcom, (char *) METODO_ENVIO_FIRMA, metodoRespuesta[iRet],
                        strlen(metodoRespuesta[iRet]));
    } else if (producto == PRODUCTO_CAJA_IP) {
        borrarArchivo(discoNetcom, (char *) METODO_SOLICITUD_CAJA);
        borrarArchivo(discoNetcom, (char *) METODO_CAJA_RESPUESTA);
        escribirArchivo(discoNetcom, (char *) METODO_SOLICITUD_CAJA, metodoSolicitud[iRet],
                        strlen(metodoSolicitud[iRet]));
        escribirArchivo(discoNetcom, (char *) METODO_CAJA_RESPUESTA, metodoRespuesta[iRet],
                        strlen(metodoRespuesta[iRet]));
    }
    setParameter(valHost, host[iRet]);
    if (producto == PRODUCTO_FIRMA_IP) {
        //activarEnvioSms();
    }
    iRet = 1;
    return iRet;
}

void capturarEliminarReverso(void) {

    int resultado = 0;
    int n = 0;
    int idx = 0;
    char dataReverso[TAM_JOURNAL];
    long valorVenta = 0;
    char linea1[50 + 1];
    char terminal[8 + 1];
    char recibo[6 + 1] = {0x00};
    char cuatroUltimos[4 + 1];
    char valorFormato[15 + 1];
    char lineaAuxiliar[10 + 1] = {0x00};
    char nombreTransaccion[20 + 1];
    DatosVenta datosReverso;
    uChar mensajeReverso[30 + 1];
    ResultISOUnpack resultadoISO;
    ISOFieldMessage isoField;

    memset(&resultadoISO, 0x00, sizeof(resultadoISO));
    memset(nombreTransaccion, 0x00, sizeof(nombreTransaccion));
    memset(mensajeReverso, 0x00, sizeof(mensajeReverso));
    memset(terminal, 0x00, sizeof(terminal));
    memset(recibo, 0x00, sizeof(recibo));
    memset(cuatroUltimos, 0x00, sizeof(cuatroUltimos));
    memset(&datosReverso, 0x00, sizeof(datosReverso));

    getParameter(NOMBRE_REVERSO, nombreTransaccion);
    resultado = leerArchivo(discoNetcom, ARCHIVO_REVERSO, dataReverso);
    LOGI("tx %s", nombreTransaccion);
    if (resultado > 0) {
        resultadoISO = unpackISOMessage(dataReverso, resultado);
        memset(&isoField, 0x00, sizeof(isoField));
        isoField = getField(4);
        memset(linea1, 0x00, sizeof(linea1));
        memcpy(linea1, isoField.valueField, 10);
        memcpy(lineaAuxiliar, linea1, 10);
        valorVenta = atol(lineaAuxiliar);
        LOGI("valorVenta %d", valorVenta);
        memset(linea1, 0x00, sizeof(linea1));
        memset(valorFormato, 0x00, sizeof(valorFormato));
        formatString(0x00, valorVenta, "", 0, valorFormato, 1);

        isoField = getField(35);

        for (n = 0; n < strlen(isoField.valueField); n++) {
            if (isoField.valueField[n] == 'D'
                || isoField.valueField[n] == 'd') {
                break;
            }
        }
        memcpy(linea1, isoField.valueField, n);
        LOGI("linea1 %s", isoField.valueField);
        idx = strlen(linea1);
        idx -= 4;
        memcpy(cuatroUltimos, linea1 + idx, 4);
        LOGI("cuatroUltimos %s", cuatroUltimos);
        isoField = getField(41);
        memcpy(terminal, isoField.valueField, 8);
        LOGI("terminal %s", terminal);
        //isoField = getField(62);
       // memcpy(recibo, isoField.valueField, 6);
        getParameter(INVOICE, recibo);
        LOGI("recibo B %s", recibo);
    }

    if (resultado <= 0) {
        resultado = leerArchivo(discoNetcom, REVERSO_PUNTOS_COLOMBIA, (char *) &datosReverso);
        if (resultado > 0) {
            memcpy(cuatroUltimos, "0000", 4);
            memcpy(terminal, datosReverso.terminalId, 8);
            memcpy(recibo, datosReverso.numeroRecibo, 6);
            memset(nombreTransaccion, 0x00, sizeof(nombreTransaccion));
            memcpy(linea1, datosReverso.totalVenta, 10);
            valorVenta = atol(linea1);
            memset(linea1, 0x00, sizeof(linea1));
            memset(valorFormato, 0x00, sizeof(valorFormato));
            formatString(0x00, valorVenta, "", 0, valorFormato, 1);
            strcpy(nombreTransaccion, "PUNTOS COLOMBIA");
        }

    }
    if (resultado > 0) {
        borrarArchivo(discoNetcom, ARCHIVO_REVERSO);
        borrarArchivo(discoNetcom, REVERSOS_RECAUDOS_MULT);
        borrarArchivo(discoNetcom, ARCHIVO_DCC_LOGGIN_REVERSO);
        borrarArchivo(discoNetcom, REVERSO_PUNTOS_COLOMBIA);
        imprimirReverso(nombreTransaccion, valorFormato, REVERSO_ELIMINADO, terminal, recibo,
                        cuatroUltimos);
    }

}

void imprimirReverso(char *nombreTransaccion, char *valorFormato, int tipoRecibo, char *terminal,
                     char *recibo, char *cuatroUltimos) {

    char parVersion[20 + 1];
    char hora[5 + 1];
    char fecha[8 + 1];
    char textoComercio[30 + 1];
    char parComercio[TAM_COMERCIO + 1];
    char parLogo[35 + 1];
    char parPie[52 + 1];
    char parPie1[100 + 1];
    char parPie2[200 + 1];
    char valorFormatoAux[50 + 1];
    char numeroRecibo[18 + 1];
    char ultimosCuatro[6 + 1];
    TablaUnoInicializacion tablaUno;
    DatosVenta reciboVenta;

    memset(parVersion, 0x00, sizeof(parVersion));
    memset(hora, 0x00, sizeof(hora));
    memset(fecha, 0x00, sizeof(fecha));
    memset(textoComercio, 0x00, sizeof(textoComercio));
    memset(parComercio, 0x00, sizeof(parComercio));
    memset(parLogo, 0x00, sizeof(parLogo));
    memset(parPie, 0x00, sizeof(parPie));
    memset(parPie1, 0x00, sizeof(parPie1));
    memset(parPie2, 0x00, sizeof(parPie2));
    memset(valorFormatoAux, 0x00, sizeof(valorFormatoAux));
    memset(numeroRecibo, 0x00, sizeof(numeroRecibo));
    memset(ultimosCuatro, 0x00, sizeof(ultimosCuatro));
    memset(&tablaUno, 0x00, sizeof(tablaUno));
    memset(&reciboVenta, 0x00, sizeof(reciboVenta));

    getParameter(VERSION_SW, parVersion);
    getParameter(REVISION_SW, parVersion + strlen(parVersion));
    getParameter(TEXTO_COMERCIO, textoComercio);

    tablaUno = _desempaquetarTablaCeroUno_();
    sprintf(parLogo, "%s%s", "0000", ".BMP");

    horaFechaTerminal(reciboVenta.txnTime, reciboVenta.txnDate);
    memcpy(fecha, reciboVenta.txnDate + 2, 4);
    formatoFecha(fecha);
    LOGI("415 ");
    sprintf(hora, "%.2s:%.2s", reciboVenta.txnTime, reciboVenta.txnTime + 2);

    strcpy(parComercio, reciboVenta.nombreComercio);

    strcpy(ultimosCuatro, "**");
    strcat(ultimosCuatro, cuatroUltimos);

    strcpy(numeroRecibo, "RECIBO: ");
    strcat(numeroRecibo, recibo);
    sprintf(valorFormatoAux, "%s%s", valorFormato, " pesos. ");

    if (tipoRecibo == REVERSO_ENVIADO) {
        strcpy(parPie, "Se ha completado un reverso de ");
        LOGI("429 ");
        strcat(parPie, nombreTransaccion);
        strcpy(parPie1, "por un valor de ");
        LOGI("432 ");
        strcat(parPie1, valorFormatoAux);
        strcpy(parPie2,
               "Para confirmar el estado de la transaccion comuniquese con la entidad bancaria");
    } else {
        strcpy(parPie, "Se ha eliminado un reverso de ");
        strcat(parPie, nombreTransaccion);
        strcpy(parPie1, "por un valor de ");
        strcat(parPie1, valorFormatoAux);
        strcpy(parPie2,
               "Para confirmar el estado de la transaccion comuniquese con la entidad bancaria. Esta operacion se hace por autorizacion del administrador del sistema");
    }
    LOGI("salio ");
    HasMap hash[] = {

            {"_PAR_FECHA_",     fecha},
            {"_PAR_VERSION_",   parVersion},
            {"_PAR_HORA_",      hora},
            {"_PAR_NESTABLE_",  tablaUno.defaultMerchantName},
            {"_PAR_DESTABLE_",  tablaUno.receiptLine2},
            {"_PAR_TEXTO_ADM_", textoComercio},
            {"_PAR_COMERCIO_",  parComercio},
            {"_PAR_CUATROU_",   ultimosCuatro},
            {"_PAR_RECIBO_",    numeroRecibo},
            {"_PAR_TERMINAL_",  terminal},
            {"_PAR_PIE_",       parPie},
            {"_PAR_PIE1_",      parPie1},
            {"_PAR_PIE2_",      parPie2},

            {"FIN_HASH",        ""}};

    imprimirTicket(hash, parLogo, "/REVERSO.TPL");
}

void horaFechaTerminal(char *hora, char *fecha) {

    Date date;
    char dia[2 + 1];
    char mes[2 + 1];
    char ano[2 + 1];
    char horaAuxiliar[LEN_TEFF_HORA + 1];
    char fechaAuxiliar[LEN_TEFF_FECHA + 2];

    memset(horaAuxiliar, 0x00, sizeof(0x00));
    memset(fechaAuxiliar, 0x00, sizeof(0x00));
    memset(&date, 0x00, sizeof(date));
    memset(dia, 0x00, sizeof(dia));
    memset(mes, 0x00, sizeof(mes));
    memset(ano, 0x00, sizeof(ano));

    date = obtenerDate();
    memcpy(horaAuxiliar, date.hour, 2);
    memcpy(horaAuxiliar + 2, date.minutes, 2);

    memcpy(ano, date.year, 2);
    strcat(fechaAuxiliar, ano);

    memcpy(mes, date.month, 2);
    strcat(fechaAuxiliar, mes);

    memcpy(dia, date.day, 2);
    strcat(fechaAuxiliar, dia);

    memcpy(hora, horaAuxiliar, LEN_TEFF_HORA);
    memcpy(fecha, fechaAuxiliar, LEN_TEFF_FECHA);

}

int verificarReversoExistente() {
    int resultado = 0;
    resultado = verificarArchivo(discoNetcom, ARCHIVO_REVERSO);

    if (resultado < 0) {
        resultado = verificarArchivo(discoNetcom, REVERSO_PUNTOS_COLOMBIA);
    }
    LOGI("resultado %d", resultado);
    return resultado;
}

int verificarValorReverso(char *lineaMonto) {
    int resultado = 0;
    char recibo[6 + 1] = {0x00};
    char cuatroUltimos[4 + 1] = {0x00};
    char terminal[8 + 1] = {0x00};
    char linea1[21 + 1] = {0x00};
    LOGI("data reverso normal ");
    resultado = datosReversoNormal(recibo, cuatroUltimos, terminal, linea1);
    memcpy(lineaMonto, linea1, strlen(linea1));
    return resultado;
}

int mostrarReverso(void) {

    int resultado = 0;
    boolean reversosRecaudosMulticorresponsal = FALSE;
    char dataReverso[TAM_JOURNAL];

    resultado = tamArchivo(discoNetcom, REVERSOS_RECAUDOS_MULT);
    if (resultado > 0) {
        borrarArchivo(discoNetcom, ARCHIVO_REVERSO);
        reversosRecaudosMulticorresponsal = TRUE;
    } else {
        resultado = leerArchivo(discoNetcom, ARCHIVO_REVERSO, dataReverso);
    }

    if (resultado > 0) {
        if (reversosRecaudosMulticorresponsal) {
            //mostrarReversosRecaudoMulticorresponsal();
        } else {
            resultado = mostrarDetalleReverso(dataReverso, resultado, 1, 1);
        }
    } else {
        resultado = verificarArchivo(discoNetcom, REVERSO_PUNTOS_COLOMBIA);
        if (resultado == FS_OK) {
            leerArchivo(DISCO_NETCOM, REVERSO_PUNTOS_COLOMBIA, dataReverso);
            mostrarReversoPuntosColombia(dataReverso, resultado, 1, 1);
        }
    }

    return resultado;
}

int
mostrarDetalleReverso(uChar *dataReverso, int tamReverso, int cantidadReversos, int numeroReverso) {

    long valorVenta = 0;
    char linea1[21 + 1];
    char valorFormato[15 + 1];
    char nombreTransaccion[20 + 1];
    int iRet = 0;
    int reversoEnviado = 0;
    int n = 0;
    int idx = 0;
    char recibo[6 + 1];
    char cuatroUltimos[4 + 1];
    char terminal[15 + 1];
    uChar mensajeReverso[30 + 1];
    ResultISOUnpack resultadoISO;
    ISOFieldMessage isoField;

    memset(&resultadoISO, 0x00, sizeof(resultadoISO));
    memset(nombreTransaccion, 0x00, sizeof(nombreTransaccion));
    memset(mensajeReverso, 0x00, sizeof(mensajeReverso));
    memset(terminal, 0x00, sizeof(terminal));
    memset(recibo, 0x00, sizeof(recibo));
    memset(cuatroUltimos, 0x00, sizeof(cuatroUltimos));

    datosReversoNormal(recibo, cuatroUltimos, terminal, linea1);
    reversoEnviado = generarReverso();
    getParameter(NOMBRE_REVERSO,nombreTransaccion);
    if (reversoEnviado > 0) {
        LOGI("imprimir reverso ");
        imprimirReverso(nombreTransaccion, linea1, REVERSO_ENVIADO, terminal, recibo,
                        cuatroUltimos);
    }

    return  reversoEnviado;
}

void mostrarReversoPuntosColombia(uChar *dataReverso, int tamReverso, int cantidadReversos,
                                  int numeroReverso) {

    long valorVenta = 0;
    char linea1[50 + 1];
    char valorFormato[15 + 1];
    char nombreTransaccion[20 + 1];
    uChar mensajeReverso[30 + 1];
    DatosVenta datosVenta;

    memset(nombreTransaccion, 0x00, sizeof(nombreTransaccion));
    memset(mensajeReverso, 0x00, sizeof(mensajeReverso));
    memset(&datosVenta, 0x00, sizeof(datosVenta));

    memcpy(&datosVenta, dataReverso, sizeof(datosVenta));

    memset(linea1, 0x00, sizeof(linea1));
    memcpy(linea1, datosVenta.totalVenta, 12);

    getParameter(NOMBRE_REVERSO, nombreTransaccion);
    valorVenta = atol(linea1);
    memset(linea1, 0x00, sizeof(linea1));
    memset(valorFormato, 0x00, sizeof(valorFormato));
    formatString(0x00, valorVenta, "", 0, valorFormato, 1);
    sprintf(linea1, "Valor : %13s", valorFormato);

}

int datosReversoNormal(char *recibo, char *cuatroUltimos, char *terminal, char *linea1) {

    long valorVenta = 0;
    char valorFormato[15 + 1];
    int iRet = 0;
    int n = 0;
    int idx = 0;
    uChar mensajeReverso[30 + 1];
    uChar lineaAuxiliar[10 + 1] = {0x00};
    uChar dataReverso[1024 + 1] = {0x00};
    ResultISOUnpack resultadoISO;
    ISOFieldMessage isoField;
    DatosVenta datosVenta;

    memset(&datosVenta, 0x00, sizeof(datosVenta));

    memset(&resultadoISO, 0x00, sizeof(resultadoISO));
    memset(mensajeReverso, 0x00, sizeof(mensajeReverso));
    LOGI("data reverso normal IN ");
    iRet = verificarArchivo(discoNetcom, ARCHIVO_REVERSO);
    if(iRet == FS_OK){
        iRet = leerArchivo(discoNetcom, ARCHIVO_REVERSO, dataReverso);
    }
    LOGI("leerArchivo %d ",iRet);
    if (iRet > 0) {
        resultadoISO = unpackISOMessage(dataReverso, iRet);

        memset(&isoField, 0x00, sizeof(isoField));
        isoField = getField(35);

        for (n = 0; n < strlen(isoField.valueField); n++) {
            if (isoField.valueField[n] == 'D'
                || isoField.valueField[n] == 'd') {
                break;
            }
        }

        idx = n;
        idx -= 4;
        memcpy(linea1, isoField.valueField, n);
        memcpy(cuatroUltimos, linea1 + idx, 4);

        memset(&isoField, 0x00, sizeof(isoField));
        isoField = getField(4);
        memset(linea1, 0x00, sizeof(linea1));
        memcpy(linea1, isoField.valueField, 10);
        memcpy(lineaAuxiliar, linea1, 10);

        LOGI("Monto encontrado  %s",lineaAuxiliar);
        valorVenta = atol(lineaAuxiliar);
        LOGI("valorVenta  %ld",valorVenta);
        memset(linea1, 0x00, sizeof(linea1));
        memset(valorFormato, 0x00, sizeof(valorFormato));
        formatString(0x00, valorVenta, "", 0, valorFormato, 1);
        LOGI("valorFormato  %s",valorFormato);
        sprintf(linea1, "Valor : %s", valorFormato);
        LOGI("linea1 %s", linea1);
        isoField = getField(41);
        memcpy(terminal, isoField.valueField, 10);
        memset(&isoField, 0x00, sizeof(isoField));
        //isoField = getField(62);
        //memcpy(recibo, isoField.valueField, 6);
        getParameter(INVOICE, recibo);

        LOGI("Recibo encontrado A %s",recibo);
        LOGI("Recibo encontrado  B %s",isoField.valueField);
    } else {
        iRet = verificarArchivo(discoNetcom, REVERSO_PUNTOS_COLOMBIA);
        if(iRet == FS_OK){
            iRet = leerArchivo(discoNetcom, REVERSO_PUNTOS_COLOMBIA, dataReverso);
        }
        if (iRet > 0){
            memset(linea1, 0x00, sizeof(linea1));
            memcpy(linea1, datosVenta.totalVenta, 12);
            valorVenta = atol(linea1);
            memset(linea1, 0x00, sizeof(linea1));
            memset(valorFormato, 0x00, sizeof(valorFormato));
            formatString(0x00, valorVenta, "", 0, valorFormato, 1);
            sprintf(linea1, "Valor : %13s", valorFormato);
        }
    }
    return iRet;
}