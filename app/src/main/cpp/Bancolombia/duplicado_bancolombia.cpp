//
// Created by ronald on 23/03/2020.
//

#include <string>
#include <stdio.h>
#include <Utilidades.h>
#include <Mensajeria.h>
#include <configuracion.h>
#include <bancolombia.h>
#include <Inicializacion.h>
#include <Archivos.h>
#include <native-lib.h>
#include "android/log.h"

#define  LOG_TAG    "NETCOM_DUPLICADOS_CNB"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
int duplicadoCnbBancolombia(int opcionRecibo, int tipoRecibo, char *numeroRecibo) {

    int iRet = 0;
    uChar claveSupervisor[4 + 1];
    uChar claveActual[4 + 1];

    memset(claveSupervisor, 0x00, sizeof(claveSupervisor));
    memset(claveActual, 0x00, sizeof(claveActual));

    iRet = verificarArchivo(discoNetcom, JOURNAL_CNB_BANCOLOMBIA);
    LOGI("verifico si existen Transacciones  %d",iRet);
    if (iRet == FS_OK) {
        switch (opcionRecibo) {
            case 1: //Ultimo Recibo
                LOGI("es ultimo recibo  ");
                iRet = duplicadoUltimoRecibo(tipoRecibo);
                break;
            case 2: //Otro Recibo
                LOGI("es otro recibo  ");
                iRet =  duplicadoOtroRecibo(numeroRecibo, tipoRecibo);
                break;
        }


    } else {
        iRet = 0;
    }
    return iRet;
}

int duplicadoOtroRecibo(char * numeroRecibo, int tipoDuplicado) {

    int iRet = 0;
    uChar numeroReciboAux[6 + 1] = {0x00};
    DatosCnbBancolombia datosVenta;

    memset(&datosVenta, 0x00, sizeof(datosVenta));

    leftPad(numeroReciboAux, numeroRecibo, 0x30, 6);
    LOGI("numeroReciboAux %s",numeroReciboAux);
    iRet = buscarRecibo(numeroReciboAux, &datosVenta, ORIGEN_DUPLICADO);
    LOGI("iRet %d",iRet);

    if (iRet > 0) {
        imprimirDuplicadoBancolombia(datosVenta,tipoDuplicado);
        iRet = 1;
    } else{
        iRet = -1;
    }
    return iRet;
}

int duplicadoUltimoRecibo(int tipoDuplicado) {

    int iRet = 0;
    long lengthJournal = 0;

    uChar dataDuplicado[sizeof(DatosCnbBancolombia)];
    DatosCnbBancolombia datosVenta;

    memset(&datosVenta, 0x00, sizeof(datosVenta));
    memset(dataDuplicado, 0x00, sizeof(dataDuplicado));

    lengthJournal = tamArchivo(discoNetcom, JOURNAL_CNB_BANCOLOMBIA);

    lengthJournal -= sizeof(DatosCnbBancolombia);
       LOGI("leng journal %ld ",lengthJournal);
       if(lengthJournal >= 0){
           iRet = buscarArchivo(discoNetcom, JOURNAL_CNB_BANCOLOMBIA, dataDuplicado, lengthJournal,
                                sizeof(DatosCnbBancolombia));

           if (iRet >= 0) {
               memcpy(&datosVenta, dataDuplicado, sizeof(dataDuplicado));
               LOGI("este  es el ultimoRecibo %s ",datosVenta.numeroRecibo);
               ////imprimirRecibo
               imprimirDuplicadoBancolombia(datosVenta,tipoDuplicado);
               iRet = 1;
           }
       }

    return iRet;
}

int buscarRecibo(char *numeroRecibo, DatosCnbBancolombia *datosVenta, uChar *origenSolicitud) {

    int iRet = 0;
    int posicion = 0;
    int controlRecibo = 0;
    uChar dataDuplicado[TAM_JOURNAL_BANCOLOMBIA + 1];
    DatosCnbBancolombia datosVentaAux;

    do {
        memset(&datosVentaAux, 0x00, sizeof(datosVentaAux));
        memset(dataDuplicado, 0x00, sizeof(dataDuplicado));
        iRet = buscarArchivo(discoNetcom, JOURNAL_CNB_BANCOLOMBIA, dataDuplicado, posicion,
                             sizeof(DatosCnbBancolombia));

        memcpy(&datosVentaAux, dataDuplicado, sizeof(DatosCnbBancolombia));
        if (strncmp(datosVentaAux.numeroRecibo, numeroRecibo, TAM_NUMERO_RECIBO) == 0) {
            memcpy(datosVenta, &datosVentaAux, sizeof(DatosCnbBancolombia));
            controlRecibo = 1;
            iRet = 2;
        }

        posicion += sizeof(DatosCnbBancolombia);

    } while (iRet == 0);

    if (controlRecibo == FALSE) {
        //screenMessage(origenSolicitud, "NO EXISTE", "RECIBO", "", 2 * 1000);
    }

    return controlRecibo;
}

void imprimirDuplicadoBancolombia(DatosCnbBancolombia datosVenta, int tipoDuplicado) {

    int iRet = 0;
    int tipoTicket = 0;

    if (atoi(datosVenta.tipoTransaccion) == TRANSACCION_BCL_SALDO) {
        //CrearString de para mensajes declinados en JAva
        //screenMessage("MENSAJE", "NO SOPORTADA", "CANCELANDO", "LA TRANSACCION", 2 * 1000);
        return;
    }

    if (tipoDuplicado == 1) {
        tipoTicket = RECIBO_COMERCIO;
    } else if (tipoDuplicado == 2) {
        tipoTicket = RECIBO_CLIENTE;
    }
    imprimirTicketBancolombia(datosVenta, tipoTicket, RECIBO_DUPLICADO);


}

