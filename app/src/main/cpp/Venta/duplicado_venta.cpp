//
// Created by NETCOM on 04/03/2020.
//
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
#include "Utilidades.h"
#include "Mensajeria.h"
#include "Inicializacion.h"
#include "android/log.h"
#include "Archivos.h"
#include "configuracion.h"
#include "Tipo_datos.h"
#include "native-lib.h"
#include "comunicaciones.h"
#include "Definiciones.h"
#include "venta.h"

#include "android/log.h"

#define  LOG_TAG    "NETCOM_DUPLICADOS"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
void _duplicado_(void) {

    int iRet = 0;
    char claveSupervisor[4 + 1];
    char claveActual[4 + 1];
    uChar numeroRecibo[TAM_NUMERO_RECIBO + 1];
    uChar numeroReciboAux[TAM_NUMERO_RECIBO + 1];

    memset(claveSupervisor, 0x00, sizeof(claveSupervisor));
    memset(claveActual, 0x00, sizeof(claveActual));
    memset(numeroRecibo, 0x00, sizeof(numeroRecibo));
    memset(numeroReciboAux, 0x00, sizeof(numeroReciboAux));

    iRet = verificarArchivo(discoNetcom, DIRECTORIO_JOURNALS);

    if (iRet == FS_OK) {

        //iRet = getStringKeyboard("CLAVE SUPERVISOR","DUPLICADO","DIGITE CLAVE",4,claveSupervisor,DATO_PASSWORD);
        getParameter(VALOR_CLAVE_SUPERVISOR, claveActual);
        //iRet = capturarClave("CLAVE SUPERVISOR", "INGRESE", "CLAVE", claveActual, 4);
        if (iRet > 0) {
            //iRet = menuSeleccionRecibo();
            if (iRet > 0) {
                if (iRet == 2) { //otro recibo
                    do {
                        //iRet = getStringKeyboard("DUPLICADO", "INGRESE", "NUMERO RECIBO", 6, numeroReciboAux, DATO_NUMERICO);
                        if (iRet > 0 && strlen(numeroReciboAux) == 0) {
                           //screenMessage("DUPLICADO", "", "DIGITE UN RECIBO", "PARA DUPLICADO", 2 * 1000);
                        }
                    } while (iRet > 0 && strlen(numeroReciboAux) == 0);

                    leftPad(numeroRecibo, numeroReciboAux, 0x30, 6);
                }

            }

            if (iRet > 0) {
                duplicadoNumeroRecibo(numeroRecibo);
            }
        }
    } else {
       // screenMessage("DUPLICADO", "SIN TRANSACCIONES", "OPCION NO", "PERMITIDA", 2 * 1000);
    }
}

void otroReciboDuplicado(void) {

    int iRet = 0;
    char numeroRecibo[6 + 1];
    char numeroRecibo2[6 + 1];

    memset(numeroRecibo, 0x00, sizeof(numeroRecibo));
    memset(numeroRecibo2, 0x00, sizeof(numeroRecibo2));

    do {
        //iRet = getStringKeyboard("DUPLICADO", "INGRESE", "NUMERO RECIBO", 6, numeroRecibo, DATO_NUMERICO);
        if (iRet > 0 && strlen(numeroRecibo) == 0) {
            //screenMessage("DUPLICADO", "", "DIGITE UN RECIBO", "PARA DUPLICADO", 2 * 1000);
        }
    } while (iRet > 0 && strlen(numeroRecibo) == 0);
    //iRet= getStringKeyboard("DUPLICADO","INGRESE","NUMERO RECIBO",6,numeroRecibo,DATO_NUMERICO);
    leftPad(numeroRecibo2, numeroRecibo, 0x30, 6);

    if (iRet > 0 && strlen(numeroRecibo) > 0) {
        buscarRecibo(numeroRecibo2,1);
    }

}

int buscarRecibo(char *numeroRecibo, int tipoTicket) {

    int resultado = 0;
    int posicion = 0;
    char dataDuplicado[TAM_JOURNAL + 1];
    int controlRecibo = 0;
    DatosVenta datosVenta;

    memset(&datosVenta, 0x00, sizeof(datosVenta));
    memset(dataDuplicado, 0x00, sizeof(dataDuplicado));

    do {
        memset(dataDuplicado, 0x00, sizeof(dataDuplicado));
        resultado = buscarArchivo(discoNetcom, (char *)JOURNAL, dataDuplicado, posicion, sizeof(DatosVenta));
        //memcpy(recibo,dataDuplicado + 9,6);
        datosVenta = obtenerDatosVenta(dataDuplicado);
        LOGI("Recibos compare %s %s  ",datosVenta.numeroRecibo,numeroRecibo);
        if (strncmp(datosVenta.numeroRecibo, numeroRecibo, TAM_NUMERO_RECIBO) == 0) {
            controlRecibo = 1;
            resultado = 2;
        }
        posicion += sizeof(DatosVenta);
    } while (resultado == 0);

    if (controlRecibo == 1) {
        imprimirDuplicado(datosVenta, tipoTicket);
    } else{
        controlRecibo = -1;
    }
    return controlRecibo;
}

int buscarUltimoRecibo(int tipoTicket) {

    int resultado = 0;
    long tamanoJournal = 0;
    char dataDuplicado[TAM_JOURNAL + 1];
    //char recibo[6+1];
    DatosVenta datosVenta;

    memset(&datosVenta, 0x00, sizeof(datosVenta));
    memset(dataDuplicado, 0x00, sizeof(dataDuplicado));

    tamanoJournal = tamArchivo(discoNetcom, (char *)JOURNAL);
    if (tamanoJournal > 0) {
        tamanoJournal -= sizeof(DatosVenta);
        LOGI("tamanoJournal %ld  ",tamanoJournal);
        resultado = buscarArchivo(discoNetcom, (char *)JOURNAL, dataDuplicado, tamanoJournal, sizeof(DatosVenta));
        if (resultado >= 0) {
            LOGI("obtener datos venta   ");
            datosVenta = obtenerDatosVenta(dataDuplicado);
            imprimirDuplicado(datosVenta,tipoTicket);
            resultado = 1;
        }
    } else {
        resultado = 0;
        //screenMessage("DUPLICADO", "SIN TRANSACCIONES", "OPCION NO", "PERMITIDA", 2 * 1000);
    }
    return resultado;
}

DatosVenta buscarReciboByID(int id) {

    int posicion = 0;
    char dataDuplicado[TAM_JOURNAL + 1];
    //char recibo[6+1];
    DatosVenta datosVenta;
    //int iteracion = 0;

    memset(dataDuplicado, 0x00, sizeof(dataDuplicado));
    memset(&datosVenta, 0x00, sizeof(datosVenta));

    if (id > 0) {
        posicion = sizeof(DatosVenta) * id;
    }

    buscarArchivo(discoNetcom, (char *)JOURNAL, dataDuplicado, posicion, sizeof(DatosVenta));
    datosVenta = obtenerDatosVenta(dataDuplicado);

    return datosVenta;

}


void imprimirDuplicado(DatosVenta datosVenta, int tipoTicket ) {

    int indice = 0;
    int resultado = 0;
    int tipoTransaccion = 0;
    int resultadoFoco = 0;
    uChar modo[2 + 1];
    uChar modoInicia[2 + 1];
    uChar reciboFoco[6 + 1];
    uChar bufferAux[1089 + 1];
    LOGI("imprimir duplicados ");
    memset(modo, 0x00, sizeof(modo));
    memset(bufferAux, 0x00, sizeof(bufferAux));
    memset(reciboFoco, 0x00, sizeof(reciboFoco));
    memset(modoInicia, 0x00, sizeof(modoInicia));

    getParameter(MODO_DATAFONO, modo);

    if (atoi(modo) == MODO_PAGO) {
        resultado = 2;
    }

        tipoTransaccion = atoi(datosVenta.tipoTransaccion);
        LOGI("imprimir tipoTransaccion   %d ",tipoTransaccion);
        switch (tipoTransaccion) {
            case TRANSACCION_REDENCION_BONO:
            case TRANSACCION_HABILITAR_BONO:
            case TRANSACCION_RECARGAR_BONO:
                //IMPRIMIR DUPLICADO BONO
                //imprimirBono(datosVenta, tipoTicket, RECIBO_DUPLICADO);
                //screenMessage("DUPLICADO","RECIBO",datosVenta.numeroRecibo,"EXITOSO",2*1000);
                break;
            case TRANSACCION_VENTA:
            case TRANSACCION_MULTIPOS:
            case TRANSACCION_TARIFA_ADMIN:
            case TRANSACCION_TARJETA_MI_COMPRA:
            case TRANSACCION_FOCO_SODEXO:
                if (datosVenta.estadoTarifa[0] == 'T') {
                    //imprimirTicketDescuentos(datosVenta, tipoTicket, RECIBO_DUPLICADO);
                } else {

                    if(tipoTransaccion == TRANSACCION_FOCO_SODEXO){
                        setParameter(TIPO_TRANSACCION, (char *)"95");
                    }
                    LOGI("imprimir duplicado");
                    imprimirTicketVenta(datosVenta, tipoTicket, RECIBO_DUPLICADO);
                    if (atoi(datosVenta.tipoTransaccion) == TRANSACCION_FOCO_SODEXO) {
                        do {
                            memset(reciboFoco, 0x00, sizeof(reciboFoco));
                            memset(bufferAux, 0x00, sizeof(bufferAux));
                            buscarArchivo(discoNetcom, (char *)JOURNAL_FOCO, bufferAux, indice, 1089);
                            memcpy(reciboFoco, bufferAux, sizeof(reciboFoco));
                            if (strncmp(datosVenta.numeroRecibo, reciboFoco, TAM_NUMERO_RECIBO) == 0) {
                                borrarArchivo(discoNetcom, (char *)QI_FOCO);
                                escribirArchivo(discoNetcom, (char *)QI_FOCO, bufferAux + 6, 1083);
                                resultadoFoco = 2;
                            }
                            indice += 1089;

                        } while (resultadoFoco == 0);

                        resultadoFoco = 0;
                        indice = 0;

                        //imprimirReporteFocoSodexo(datosVenta);
                        borrarArchivo(discoNetcom, (char *)QI_FOCO);
                        borrarArchivo(discoNetcom, (char *)SODEXO);
                    }

                }
                //screenMessage("DUPLICADO","RECIBO",datosVenta.numeroRecibo,"EXITOSO",2*1000);
                break;
            case TRANSACCION_PSP:
                //imprimirTicketPSP(datosVenta, tipoTicket, RECIBO_DUPLICADO);
                break;
            case TRANSACCION_PCR:
                //imprimirTicketPCR(datosVenta, tipoTicket, RECIBO_DUPLICADO);
                break;
            case TRANSACCION_RECARGA:
                //imprimirTicketRecargaCelular(datosVenta, tipoTicket, RECIBO_DUPLICADO);
                break;
            case TRANSACCION_EFECTIVO_VIVAMOS:
                //imprimirTicketDescuentos(datosVenta, tipoTicket, RECIBO_DUPLICADO);
                break;
            case TRANSACCION_PAGO_ELECTRONICO:
            case TRANSACCION_PAGO_ELECTRONICO_ESPECIAL:
               imprimirTicketDineroElectronico(datosVenta, tipoTicket, RECIBO_DUPLICADO);
                break;
            case TRANSACCION_EFECTIVO:
            case TRANSACCION_PAGO_IMPUESTOS:
                //imprimirTicketRetiro(datosVenta, tipoTicket, RECIBO_DUPLICADO);
                break;
            case TRANSACCION_TRANSFERENCIA_AVAL:
            case TRANSACCION_RETIRO_CNB_AVAL:
            case TRANSACCION_PAGO_FACTURAS_AVAL:
            case TRANSACCION_DEPOSITO_EFECTIVO_CNB_AVAL:
            case TRANSACCION_P_PRODUCTO_TARJETA_AVAL:
            case TRANSACCION_P_PRODUCTO_NO_TARJETA_AVAL:
                //serviciosImprimirDuplicado(APP_TYPE_CNB_AVAL, datosVenta, tipoTicket, RECIBO_DUPLICADO);
                break;
            case TRANSACCION_CELUCOMPRA:
                datosVenta.estadoTarifa[0] = 'D';
                datosVenta.estadoTarifa[1] = tipoTicket;
                //netcomServices(APP_TYPE_CELUCOMPRA, RECIBO_CELUCOMPRA, sizeof(datosVenta), (char *) &datosVenta);
                break;
            case TRANSACCION_TARJETA_PRIVADA:
                datosVenta.estadoTarifa[0] = 'D';
                datosVenta.estadoTarifa[1] = tipoTicket;
                //netcomServices(APP_TYPE_TARJETA_PRIVADA, RECIBO_TARJETA_PRIVADA, sizeof(datosVenta), (char *) &datosVenta);
                break;
            case TRANSACCION_GASO_PASS:
                //imprimirTicketGasopass(datosVenta, tipoTicket, RECIBO_DUPLICADO);
                break;
            case TRANSACCION_CONSULTA_QR:
                datosVenta.estadoTarifa[0] = 'D';
                datosVenta.estadoTarifa[1] = tipoTicket;
                //netcomServices(APP_TYPE_QR, RECIBO_DUPLICADO_QRCODE, sizeof(datosVenta), (char *) &datosVenta);
                break;
            case TRANSACCION_CODIGO_ESTATICO:
                datosVenta.estadoTarifa[0] = 'D';
                datosVenta.estadoTarifa[1] = tipoTicket;
                //netcomServices(APP_TYPE_CONSULTA_ESTADO, RECIBO_DUPLI_CONSULTA_ESTADO, sizeof(datosVenta),(char *) &datosVenta);
                break;
            case TRANSACCION_BIG_BANCOLOMBIA:

                memset(datosVenta.isQPS, 0x00, sizeof(datosVenta.isQPS));
                if (tipoTicket == RECIBO_COMERCIO) {
                    strcpy(datosVenta.isQPS, "0");
                } else {
                    strcpy(datosVenta.isQPS, "1");
                }
                //netcomServices(APP_TYPE_LEALTAD, DUPLICADO_LEALTAD, TAM_JOURNAL_COMERCIO, (char *) &datosVenta);
                break;
            default:
                break;
        }

}

void duplicadoNumeroRecibo(uChar *numeroRecibo) {

    long tamDirJournal = 0;
    uChar numeroReciboAux[TAM_NUMERO_RECIBO + 1];
    DatosJournals datosJournals;

    memset(numeroReciboAux, 0x00, sizeof(numeroReciboAux));
    memset(&datosJournals, 0x00, sizeof(datosJournals));

    if (numeroRecibo[0] == 0x00) {
        tamDirJournal = tamArchivo(discoNetcom, (char *)DIRECTORIO_JOURNALS);
        buscarArchivo(discoNetcom, (char *)DIRECTORIO_JOURNALS, (char *) &datosJournals, tamDirJournal - TAM_DIR_JOURNALS,
                      TAM_DIR_JOURNALS);
        memcpy(numeroRecibo, datosJournals.numeroRecibo, TAM_NUMERO_RECIBO);
    }

    leftPad(numeroReciboAux, numeroRecibo, '0', 6);

    datosJournals = buscarReciboMulticorresponsal(numeroReciboAux);

    if (strlen(datosJournals.numeroRecibo) <= 0) {
        //screenMessage("DUPLICADO", "NO EXISTE", "RECIBO", "", 2 * 1000);
    }

    switch (datosJournals.journal) {
        case TRANS_MULT_BANCOLOMBIA:
        case TRANS_MULT_AVAL:
             //netcomServices(APP_TYPE_MULTICORRESPONSAL, DUPLICADO_MULTICORRESPONSAL, TAM_DIR_JOURNALS,(char *) &datosJournals);

            break;
        case TRANS_COMERCIO:
            duplicadoComercio(datosJournals);
            break;
        case TRANS_CNB_BANCOLOMBIA:
            //netcomServices(APP_TYPE_CNB_BANCOLOMBIA, DUPLICADO_RECIBO_BANCOLOMBIA, TAM_NUMERO_RECIBO, numeroRecibo);
            break;
        case TRANS_CNB_CORRESPONSALES:
            //netcomServices(APP_TYPE_CNB_CORRESPONSALES, DUPLICADO_RECIBO_CORRESPONSAL, TAM_NUMERO_RECIBO, numeroRecibo);
            break;
        default:
            break;
    }
}

void duplicadoComercio(DatosJournals datosJournals) {

    DatosVenta datosVenta;
    long iRet = FS_ERROR;
    int resultado = 0;
    char fechaExpiracion[TAM_FECHA_EXPIRACION + 1];

    memset(&datosVenta, 0x00, sizeof(datosVenta));
    memset(fechaExpiracion, 0x00, sizeof(fechaExpiracion));

    iRet = buscarArchivo(discoNetcom, (char *)JOURNAL, (char *) &datosVenta, datosJournals.posicionTransaccion,TAM_JOURNAL_COMERCIO);

    if (iRet >= FS_KO && strncmp(datosVenta.numeroRecibo, datosJournals.numeroRecibo, TAM_NUMERO_RECIBO) == 0) {
        if (atoi(datosVenta.tipoTransaccion) == TRANSACCION_AJUSTAR_LEALTAD
            || atoi(datosVenta.tipoTransaccion) == TRANSACCION_ACUMULAR_LEALTAD
            || atoi(datosVenta.tipoTransaccion) == TRANSACCION_BIG_BANCOLOMBIA
            || atoi(datosVenta.tipoTransaccion) == TRANSACCION_REDENCION_LIFEMILES
            || atoi(datosVenta.tipoTransaccion) == TRANSACCION_PUNTOS_COLOMBIA) {

            memcpy(fechaExpiracion, datosVenta.fechaExpiracion, TAM_FECHA_EXPIRACION);

            if (atoi(datosVenta.tipoTransaccion) == TRANSACCION_BIG_BANCOLOMBIA) {
                //resultado = showMenu("TIPO DUPLICADO", 0, 0, 2, menuTipoDuplicado, 30 * 1000);

                if (resultado > 0) {
                    if (resultado == 1) {
                        strcpy(datosVenta.aux1, "00");
                    }
                    if (resultado == 2) {
                        strcpy(datosVenta.aux1, "01");
                    }
                    memcpy(datosVenta.fechaExpiracion, fechaExpiracion, TAM_FECHA_EXPIRACION);
                    //netcomServices(APP_TYPE_LEALTAD, DUPLICADO_LEALTAD, TAM_JOURNAL_COMERCIO, (char *) &datosVenta);
                }
            } else {
                memcpy(datosVenta.fechaExpiracion, fechaExpiracion, TAM_FECHA_EXPIRACION);
               // netcomServices(APP_TYPE_LEALTAD, DUPLICADO_LEALTAD, TAM_JOURNAL_COMERCIO, (char *) &datosVenta);
            }
        } else {
            imprimirDuplicado(datosVenta,1);
        }
    } else {
        //screenMessage("DUPLICADO", "NO EXISTE", "RECIBO", "", 2 * 1000);
    }
}

ResumenTransacciones _obtenerResumenTransacciones_(int tipoTransaccion) {

    int idx = 0;
    long tam = 0;
    ResumenTransacciones resumenTransacciones;
    DatosJournals datosJournals;

    int cantidadTransacciones = 0;
    int iRet = FS_ERROR;

    memset(&resumenTransacciones, 0x00, sizeof(resumenTransacciones));
    memset(&datosJournals, 0x00, sizeof(datosJournals));

    tam = verificarArchivo(discoNetcom, DIRECTORIO_JOURNALS);

    if (tam > 0) {
        tam = tamArchivo(discoNetcom, (char *)DIRECTORIO_JOURNALS);
        cantidadTransacciones = tam / TAM_DIR_JOURNALS;
    }

    if (tam > 0) {

        iRet = buscarArchivo(discoNetcom, (char *)DIRECTORIO_JOURNALS, (char *) &datosJournals, tam - TAM_DIR_JOURNALS,
                             TAM_DIR_JOURNALS);
        if (iRet >= 0 && atoi(datosJournals.estadoTransaccion) == 0) {
            cantidadTransacciones--;
        }
    }

    resumenTransacciones.totalTransacciones = cantidadTransacciones;

    for (idx = 0; idx < cantidadTransacciones; idx++) {
        iRet = buscarArchivo(discoNetcom, (char *)DIRECTORIO_JOURNALS, (char *) &datosJournals, idx * TAM_DIR_JOURNALS,
                             TAM_DIR_JOURNALS);

        switch (datosJournals.journal) {
            case TRANS_MULT_BANCOLOMBIA:
                resumenMultBancolombia(idx, datosJournals, &resumenTransacciones, tipoTransaccion);
                break;
            case TRANS_COMERCIO:
                resumenComercio(idx, datosJournals, &resumenTransacciones, tipoTransaccion);
                break;
            default:
                break;
        }

    }

    return resumenTransacciones;

}

DatosJournals buscarReciboMulticorresponsal(uChar *numeroRecibo) {

    DatosJournals datosJournals;
    long iRet = FS_ERROR;
    long tamDirectorioJournals = 0;
    int cantidadTransacciones = 0;
    int indice = 0;

    memset(&datosJournals, 0x00, sizeof(datosJournals));

    tamDirectorioJournals = tamArchivo(discoNetcom, (char *)DIRECTORIO_JOURNALS);

    cantidadTransacciones = tamDirectorioJournals / TAM_DIR_JOURNALS;

    if (cantidadTransacciones > 0) {
        for (indice = 0; indice < cantidadTransacciones; indice++) {
            iRet = buscarArchivo(discoNetcom, (char *)DIRECTORIO_JOURNALS, (char *) &datosJournals, indice * TAM_DIR_JOURNALS,
                                 TAM_DIR_JOURNALS);

            if (iRet >= FS_KO && strncmp(datosJournals.numeroRecibo, numeroRecibo, TAM_NUMERO_RECIBO) == 0) {
                break;
            } else if (indice == cantidadTransacciones - 1) {
                memset(&datosJournals, 0x00, sizeof(datosJournals));
            }
        }
    }
    return datosJournals;
}

void resumenMultBancolombia(int idx, DatosJournals datosJournal, ResumenTransacciones *resumenTransacciones,
                            int tipoTransaccion) {

    long iRet = FS_ERROR;
    DatosCnbBancolombia datosBancolombia;
    uChar valor[TAM_COMPRA_NETA + 1];

    memset(&datosBancolombia, 0x00, sizeof(datosBancolombia));
    memset(valor, 0x00, sizeof(valor));

    iRet = buscarArchivo(discoNetcom, (char *)JOURNAL_MULT_BCL, (char *) &datosBancolombia, datosJournal.posicionTransaccion,
                         TAM_JOURNAL_BANCOLOMBIA);

    if (iRet >= 0) {
        if (tipoTransaccion == atoi(datosBancolombia.tipoTransaccion) || tipoTransaccion == TRANSACCION_DEFAULT
            || (IS_SUMAN_RESUMEN(atoi(datosBancolombia.tipoTransaccion)))) {

            if (strncmp(datosBancolombia.estadoTransaccion, "1", 1) == 0) {
                memset(valor, 0x00, sizeof(valor));
                resumenTransacciones->estadoTransaccion[idx] = 1;
                memcpy(valor, datosBancolombia.totalVenta, strlen(datosBancolombia.totalVenta) - 2); //-2 por la parte decimal
                resumenTransacciones->totalActivas += atol(valor);
                resumenTransacciones->cantidadActivas++;
            } else {
                memset(valor, 0x00, sizeof(valor));
                resumenTransacciones->estadoTransaccion[idx] = 0;
                memcpy(valor, datosBancolombia.totalVenta, strlen(datosBancolombia.totalVenta) - 2); //-2 por la parte decimal
                resumenTransacciones->totalAnuladas += atol(valor);
                resumenTransacciones->cantidadAnuladas++;
            }

        }
    }
}

void resumenComercio(int idx, DatosJournals datosJournal, ResumenTransacciones *resumenTransacciones,
                     int tipoTransaccion) {

    long iRet = FS_ERROR;
    DatosVenta datosVenta;
    uChar valor[TAM_COMPRA_NETA + 1];

    memset(&datosVenta, 0x00, sizeof(datosVenta));
    memset(valor, 0x00, sizeof(valor));

    iRet = buscarArchivo(discoNetcom, (char *)JOURNAL, (char *) &datosVenta, datosJournal.posicionTransaccion,
                         TAM_JOURNAL_COMERCIO);
    if (iRet >= 0) {
        if (tipoTransaccion == atoi(datosVenta.tipoTransaccion) || tipoTransaccion == TRANSACCION_DEFAULT
            || (IS_SUMAN_RESUMEN(atoi(datosVenta.tipoTransaccion)))) {

            if (strncmp(datosVenta.estadoTransaccion, "1", 1) == 0) {
                memset(valor, 0x00, sizeof(valor));
                resumenTransacciones->estadoTransaccion[idx] = 1;
                memcpy(valor, datosVenta.totalVenta, strlen(datosVenta.totalVenta) - 2); //-2 por la parte decimal
                resumenTransacciones->totalActivas += atol(valor);
                resumenTransacciones->cantidadActivas++;
            } else {
                memset(valor, 0x00, sizeof(valor));
                resumenTransacciones->estadoTransaccion[idx] = 0;
                memcpy(valor, datosVenta.totalVenta, strlen(datosVenta.totalVenta) - 2); //-2 por la parte decimal
                resumenTransacciones->totalAnuladas += atol(valor);
                resumenTransacciones->cantidadAnuladas++;
            }
        }
    }
}
void imprimirReporteFocoSodexo(DatosVenta datosTransaccion) {

    int idx = 0;
    int idy = 0;
    int idz = 0;

    uChar lineaImprimir[48 + 1];

    char aux2[20 + 1];
    char buffer[200 + 1];
    char auxiliar[28 + 1];
    char total[5][12 + 1];
    char idGrupo[5][4 + 1];
    char cantidad[5][8 + 1];
    char unitario[5][12 + 1];
    char auxiliar2[28 + 1];
    char dataSodexo[242 + 1];
    char idProducto[5][6 + 1];
    char archivoFoco[512 + 1];
    char dataImprimir[1083 + 1];
    char idProductoAux[5][6 + 1];
    char nombreProducto[5][20 + 1];
    char idProductoFile[25][6 + 1];
    char productoCombustible[30 + 1];
    char lineaAux[50 + 1];

    TablaUnoInicializacion tablaUno;

    memset(aux2, 0x00, sizeof(aux2));
    memset(total, 0x00, sizeof(total));
    memset(buffer, 0x00, sizeof(buffer));
    memset(idGrupo, 0x00, sizeof(idGrupo));
    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(cantidad, 0x00, sizeof(cantidad));
    memset(unitario, 0x00, sizeof(unitario));
    memset(auxiliar2, 0x00, sizeof(auxiliar2));
    memset(&tablaUno, 0x00, sizeof(tablaUno));
    memset(dataSodexo, 0x00, sizeof(dataSodexo));
    memset(idProducto, 0x00, sizeof(idProducto));
    memset(archivoFoco, 0x00, sizeof(archivoFoco));
    memset(idProductoAux, 0x00, sizeof(idProducto));
    memset(dataImprimir, 0x00, sizeof(dataImprimir));
    memset(lineaImprimir, 0x00, sizeof(lineaImprimir));
    memset(idProductoFile, 0x00, sizeof(idProductoFile));
    memset(nombreProducto, 0x00, sizeof(nombreProducto));
    memset(productoCombustible, 0x00, sizeof(productoCombustible));

    tablaUno = _desempaquetarTablaCeroUno_();
    leerArchivo(discoNetcom, (char *)SODEXO, dataSodexo);
    leerArchivo(discoNetcom, (char *)QI_FOCO, dataImprimir);

    memcpy(buffer, dataSodexo + 31, sizeof(buffer));

    idx = 0;
    idz = 0;

    for (idx = 0; idx < strlen(dataImprimir); idx++) {

        memset(lineaImprimir, 0x00, sizeof(lineaImprimir));

        if(dataImprimir[idx] != '@'){
            lineaAux[idy] = dataImprimir[idx];
            idy++;
            idz = 0;
        } else 	if (dataImprimir[idx] == '@') {
            if (idz < 4) {
               sprintf(lineaImprimir, "%s%s\n", "        ", lineaAux);
               pprintf(lineaImprimir);
            }
            idy = 0;
            idz++;
            memset(lineaAux, 0x00, sizeof(lineaAux));
        }

    }

    memset(lineaImprimir, 0x00, sizeof(lineaImprimir));
    sprintf(lineaImprimir,"%s", "\n\n\n\n\n");
    pprintf(lineaImprimir);
}
char * _stringtok_(char * str, const char * delim) {
    static char* p = 0;
    if (str)
        p = str;
    else if (!p)
        return 0;
    str = p + strspn(p, delim);
    p = str + strcspn(str, delim);
    if (p == str)
        return p = 0;
    p = *p ? *p = 0, p + 1 : 0;
    return str;
}

DatosVenta obtenerDatosVenta(char *data) {

    DatosVenta datosVenta;
    int indice = 0;
    memset(&datosVenta, 0x00, sizeof(datosVenta));

    memcpy(datosVenta.tipoTransaccion, data + indice, TAM_TIPO_TRANSACCION);
    LOGI("tipoTransaccion %s", datosVenta.tipoTransaccion);
    indice += (TAM_TIPO_TRANSACCION + 1);
    memcpy(datosVenta.estadoTransaccion, data + indice, TAM_ESTADO_TRANSACCION);
    LOGI("estadoTransaccion %s", datosVenta.estadoTransaccion);
    indice += (TAM_ESTADO_TRANSACCION + 1);
    memcpy(datosVenta.tipoCuenta, data + indice, TAM_TIPO_CUENTA);
    LOGI("tipoCuenta %s", datosVenta.tipoCuenta);
    indice += (TAM_TIPO_CUENTA + 1);
    memcpy(datosVenta.numeroRecibo, data + indice, TAM_NUMERO_RECIBO);
    LOGI("numeroRecibo %s", datosVenta.numeroRecibo);
    indice += (TAM_NUMERO_RECIBO + 1);
    memcpy(datosVenta.systemTrace, data + indice, TAM_SYSTEM_TRACE);
    LOGI("systemTrace %s", datosVenta.systemTrace);
    indice += (TAM_SYSTEM_TRACE + 1);
    memcpy(datosVenta.msgType, data + indice, TAM_MTI);
    LOGI("msgType %s", datosVenta.msgType);
    indice += (TAM_MTI + 1);
    memcpy(datosVenta.processingCode, data + indice, TAM_CODIGO_PROCESO);
    LOGI("processingCode %s", datosVenta.processingCode);
    indice += (TAM_CODIGO_PROCESO + 1);
    memcpy(datosVenta.compraNeta, data + indice, TAM_COMPRA_NETA);
    LOGI("compraNeta %s", datosVenta.compraNeta);
    indice += (TAM_COMPRA_NETA + 1);
    memcpy(datosVenta.iva, data + indice, TAM_IVA);
    LOGI("iva %s", datosVenta.iva);
    indice += (TAM_IVA + 1);
    memcpy(datosVenta.totalVenta, data + indice, TAM_COMPRA_NETA);
    LOGI("totalVenta %s", datosVenta.totalVenta);
    indice += (TAM_COMPRA_NETA + 1);
    memcpy(datosVenta.txnTime, data + indice, TAM_TIME);
    LOGI("txnTime %s", datosVenta.txnTime);
    indice += (TAM_TIME + 1);
    memcpy(datosVenta.txnDate, data + indice, TAM_DATE);
    LOGI("txnDate %s", datosVenta.txnDate);
    indice += (TAM_DATE + 1);
    memcpy(datosVenta.posEntrymode, data + indice, TAM_ENTRY_MODE);
    LOGI("posEntrymode %s", datosVenta.posEntrymode);
    indice += (TAM_ENTRY_MODE + 1);
    memcpy(datosVenta.nii, data + indice, TAM_NII);
    LOGI("nii %s", datosVenta.nii);
    indice += (TAM_NII + 1);
    memcpy(datosVenta.posConditionCode, data + indice, TAM_POS_CONDITION);
    LOGI("posConditionCode %s", datosVenta.posConditionCode);
    indice += (TAM_POS_CONDITION + 1);
    memcpy(datosVenta.track2, data + indice, TAM_TRACK2);
    LOGI("track2 %s", datosVenta.track2);
    indice += (TAM_TRACK2 + 1);
    memcpy(datosVenta.terminalId, data + indice, TAM_TERMINAL_ID);
    LOGI("terminalId %s", datosVenta.terminalId);
    indice += (TAM_TERMINAL_ID + 1);
    memcpy(datosVenta.acquirerId, data + indice, TAM_ACQIRER_ID);
    LOGI("acquirerId %s", datosVenta.acquirerId);
    indice += (TAM_ACQIRER_ID + 1);
    memcpy(datosVenta.field57, data + indice, TAM_FIELD_57);
    LOGI("field57 %s", datosVenta.field57);
    indice += (TAM_FIELD_57 + 1);
    memcpy(datosVenta.field61, data + indice, TAM_FIELD_61);
    LOGI("field61 %s", datosVenta.field61);
    indice += (TAM_FIELD_61 + 1);
    memcpy(datosVenta.rrn, data + indice, TAM_RRN);
    LOGI("rrn %s", datosVenta.rrn);
    indice += (TAM_RRN + 1);
    memcpy(datosVenta.ultimosCuatro, data + indice, TAM_CODIGO_PROCESO);
    LOGI("ultimosCuatro %s", datosVenta.ultimosCuatro);
    indice += (TAM_CODIGO_PROCESO + 1);
    memcpy(datosVenta.cardName, data + indice, TAM_CARD_NAME);
    LOGI("cardName %s", datosVenta.cardName);
    indice += (TAM_CARD_NAME + 1);
    memcpy(datosVenta.codigoAprobacion, data + indice, TAM_COD_APROBACION);
    LOGI("codigoAprobacion %s", datosVenta.codigoAprobacion);
    indice += (TAM_COD_APROBACION + 1);
    memcpy(datosVenta.codigoGrupo, data + indice, TAM_GRUPO);
    LOGI("codigoGrupo %s", datosVenta.codigoGrupo);
    indice += (TAM_GRUPO + 1);
    memcpy(datosVenta.codigoSubgrupo, data + indice, TAM_SUBGRUPO);
    LOGI("codigoSubgrupo %s", datosVenta.codigoSubgrupo);
    indice += (TAM_SUBGRUPO + 1);
    memcpy(datosVenta.propina, data + indice, TAM_COMPRA_NETA);
    LOGI("propina %s", datosVenta.propina);
    indice += (TAM_COMPRA_NETA + 1);
    memcpy(datosVenta.baseDevolucion, data + indice, TAM_COMPRA_NETA);
    LOGI("baseDevolucion %s", datosVenta.baseDevolucion);
    indice += (TAM_COMPRA_NETA + 1);
    memcpy(datosVenta.fiel42, data + indice, TAM_FIELD_42);
    LOGI("fiel42 %s", datosVenta.fiel42);
    indice += (TAM_FIELD_42 + 1);
    memcpy(datosVenta.AID, data + indice, TAM_AID);
    LOGI("AID %s", datosVenta.AID);
    indice += (TAM_AID + 1);
    memcpy(datosVenta.ARQC, data + indice, TAM_ARQC);
    LOGI("ARQC %s", datosVenta.ARQC);
    indice += (TAM_ARQC + 1);
    memcpy(datosVenta.cuotas, data + indice, TAM_CUOTAS);
    LOGI("cuotas %s", datosVenta.cuotas);
    indice += (TAM_CUOTAS + 1);
    memcpy(datosVenta.textoAdicional, data + indice, TAM_TEXTO_ADICIONAL);
    LOGI("textoAdicional %s", datosVenta.textoAdicional);
    indice += (TAM_TEXTO_ADICIONAL + 1);
    memcpy(datosVenta.isQPS, data + indice, TAM_ACTIVO_QPS);
    indice += (TAM_ACTIVO_QPS + 2);
    LOGI("isQPS %s", datosVenta.isQPS);
    LOGI("indice %d",indice);
    memcpy(datosVenta.fechaExpiracion, data + indice, TAM_FECHA_EXPIRACION);
    LOGI("fechaExpiracion %s", datosVenta.fechaExpiracion);
    indice += (TAM_FECHA_EXPIRACION + 1);
    memcpy(datosVenta.inc, data + indice, TAM_IVA);
    LOGI("inc %s", datosVenta.inc);
    indice += (TAM_IVA + 1);
    memcpy(datosVenta.tarjetaHabiente, data + indice, TAM_TARJETA_HABIENTE);
    indice += (TAM_TARJETA_HABIENTE + 1);
    LOGI("tarjetaHabiente %s", datosVenta.tarjetaHabiente);
    memcpy(datosVenta.textoAdicionalInicializacion, data + indice, TAM_TEXTO_ADICIONAL);
    indice += (TAM_TEXTO_ADICIONAL + 1);
    LOGI("textoAdicionalInicializacion %s", datosVenta.textoAdicionalInicializacion);
    memcpy(datosVenta.modoDatafono, data + indice, TAM_MODO_PRODUCTO);
    indice += (TAM_MODO_PRODUCTO + 1);
    LOGI("modoDatafono %s", datosVenta.modoDatafono);
    memcpy(datosVenta.textoInformacion, data + indice, TAM_TEXTO_INFORMACION);
    indice += (TAM_TEXTO_INFORMACION + 1);
    LOGI("textoInformacion %s", datosVenta.textoInformacion);
    memcpy(datosVenta.creditoRotativo, data + indice, TAM_CREDITO_ROTATIVO);
    indice += (TAM_CREDITO_ROTATIVO + 1);
    LOGI("creditoRotativo %s", datosVenta.creditoRotativo);
    memcpy(datosVenta.appLabel, data + indice, TAM_CARD_NAME);
    indice += (TAM_CARD_NAME + 1);
    LOGI("appLabel %s", datosVenta.appLabel);
    memcpy(datosVenta.codigoComercio, data + indice, TAM_ID_COMERCIO);
    indice += (TAM_ID_COMERCIO + 1);
    memcpy(datosVenta.nombreComercio, data + indice, TAM_COMERCIO);
    indice += (TAM_COMERCIO + 1);
    memcpy(datosVenta.numeroRecibo2, data + indice, TAM_NUMERO_RECIBO);
    indice += (TAM_NUMERO_RECIBO + 1);
    memcpy(datosVenta.estadoTarifa, data + indice, TAM_ESTADO_TARIFA);
    indice += (TAM_ESTADO_TARIFA + 1);
    memcpy(datosVenta.codigoCajero, data + indice, TAM_CODIGO_CAJERO);
    indice += (TAM_CODIGO_CAJERO + 1);
    memcpy(datosVenta.facturaCaja, data + indice, TAM_FACTURA_CAJA);
    indice += (TAM_FACTURA_CAJA + 1);
    memcpy(datosVenta.codigoRespuesta, data + indice, TAM_CODIGO_RESPUESTA);
    indice += (TAM_CODIGO_RESPUESTA + 1);
    memcpy(datosVenta.tokenVivamos, data + indice, TAM_TOKEN_VIVAMOS);
    indice += (TAM_TOKEN_VIVAMOS + 1);
    LOGI("Netcom total %s", datosVenta.totalVenta);
    LOGI("INC %s", datosVenta.inc);
    LOGI("Netcom respuesta %s", datosVenta.codigoRespuesta);
    LOGI("Netcom systemtrace %s", datosVenta.systemTrace);
    LOGI("Netcom numeros de recibo %s", datosVenta.numeroRecibo);
    return datosVenta;

}
