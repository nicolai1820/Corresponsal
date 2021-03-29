//
// Created by NETCOM on 13/02/2020.
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

#define  LOG_TAG    "NETCOM_BANCOLOMBIA_UTILIDADES"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
uChar tokensP6[SIZE_TOKEN_P6 + 1];
uChar tokensQF[SIZE_TOKEN_QF + 1];

char textoBancolombia1[] = {
        "Bancolombia es responsable por los servicios prestados por el CB. El CB no puede prestar servicios financieros por su cuenta. Verifique que la informacion en este documento este correcta.Para reclamos comuniquese al 018000912345. Conserve esta tirilla como soporte."};
char textoBancolombia2[] = {
        "Para reclamos comuniquese al 018000912345 conserve esta tirilla como soporte."};
DatosVenta datosVentaBancolombia;

void formatearDatos(DatosTokens *datosTokens) {

    uChar buffer[SIZE_FACTURA + 1];

    memset(buffer, 0x00, sizeof(buffer));

    if (strlen(datosTokens->indicador1) == 0) {
        memset(datosTokens->indicador1, 0x30, sizeof(datosTokens->indicador1));
    }

    if (strlen(datosTokens->indicador2) == 0) {
        memset(datosTokens->indicador2, 0x30, sizeof(datosTokens->indicador2));
    }

    leftPad(buffer, datosTokens->referencia1, '0', SIZE_REFERENCIA);
    memset(datosTokens->referencia1, 0x00, sizeof(datosTokens->referencia1));
    memcpy(datosTokens->referencia1, buffer, SIZE_REFERENCIA);

    memset(buffer, 0x00, sizeof(buffer));
    leftPad(buffer, datosTokens->referencia2, '0', SIZE_REFERENCIA);
    memset(datosTokens->referencia2, 0x00, sizeof(datosTokens->referencia2));
    memcpy(datosTokens->referencia2, buffer, SIZE_REFERENCIA);

    if (strlen(datosTokens->factura) == 0) { ///// SE SETEA EN ESPACIOS PARA LA CONSULTA DE SALDO
        memset(datosTokens->factura, 0x20, sizeof(datosTokens->factura));
    } else {
        memset(buffer, 0x00, sizeof(buffer));
        _rightPad_(buffer, datosTokens->factura, '0', SIZE_FACTURA);
        memset(datosTokens->factura, 0x00, sizeof(datosTokens->factura));
        memcpy(datosTokens->factura, buffer, SIZE_FACTURA);
    }

}

DatosTarjetaSembrada cargarTarjetaSembrada(void) {
    int n = 0;
    long binIssuer = 0;
    char bin[20 + 1];
    int idx = 0;
    char data[69 + 1];
    TablaTresInicializacion dataIssuer;
    DatosTarjetaSembrada datosTarjetaSembrada;

    memset(bin, 0x00, sizeof(bin));
    memset(data, 0x00, sizeof(data));
    memset(&dataIssuer, 0x00, sizeof(dataIssuer));
    memset(&datosTarjetaSembrada, 0x00, sizeof(datosTarjetaSembrada));

    leerArchivo(discoNetcom, (char *) TARJETA_CNB, data);

    memset(datosTarjetaSembrada.track2, 0x00, sizeof(datosTarjetaSembrada.track2));
    memset(datosTarjetaSembrada.tarjetaHabiente, 0x00,
           sizeof(datosTarjetaSembrada.tarjetaHabiente));

    memcpy(datosTarjetaSembrada.track2, data, 37);
    memcpy(datosTarjetaSembrada.tarjetaHabiente, data + 38, 34);

    //sacar el pan de la tarjeta
    for (n = 0; n < strlen(datosTarjetaSembrada.track2); n++) {
        if (datosTarjetaSembrada.track2[n] == 'D' || datosTarjetaSembrada.track2[n] == 'd') {
            break;
        }
    }

    memset(datosTarjetaSembrada.pan, 0x00, sizeof(datosTarjetaSembrada.pan));
    memcpy(datosTarjetaSembrada.pan, datosTarjetaSembrada.track2, n);
    memcpy(bin, datosTarjetaSembrada.pan, 9);

    binIssuer = atol(bin);
    dataIssuer = _traerIssuer_(binIssuer);

    idx = strlen(datosTarjetaSembrada.pan) - 4;
    memset(datosTarjetaSembrada.ultimosCuatro, 0x00, sizeof(datosTarjetaSembrada.ultimosCuatro));
    memcpy(datosTarjetaSembrada.ultimosCuatro, datosTarjetaSembrada.pan + idx, 4);

    memset(datosTarjetaSembrada.cardName, 0x00, sizeof(datosTarjetaSembrada.cardName));
    strcpy(datosTarjetaSembrada.cardName, dataIssuer.cardName);

    memset(datosTarjetaSembrada.tipoCuenta, 0x00, sizeof(datosTarjetaSembrada.tipoCuenta));
    getParameter(CNB_TIPO_CUENTA, datosTarjetaSembrada.tipoCuenta);

    memset(datosTarjetaSembrada.terminalId, 0x00, sizeof(datosTarjetaSembrada.terminalId));
    getTerminalId(datosTarjetaSembrada.terminalId);

    return datosTarjetaSembrada;
}

void armarTrack(uChar *buffer, uChar *pan, int tipoTransaccion) {

    int indice = 0;
    int idx = 0;

    memset(buffer, 0x00, sizeof(buffer));

    if (tipoTransaccion == TRANSACCION_BCL_RETIRO_EFECTIVO) {
        memcpy(buffer, BIN_BANCOLOMBIA_RETIRO, 6);
    } else {
        memcpy(buffer, BIN_BANCOLOMBIA, 6);
    }
    indice += 6;

    idx = strlen((const char *) pan) - 10;
    LOGI("buffer idx %d ", idx);
    memcpy(buffer + indice, pan + idx, 10);
    indice += 10;
    LOGI("buffer copiado %s ", buffer);
    memcpy(buffer + indice, "D", 1);
    indice += 1;

    memcpy(buffer + indice, FECHA_VENCIMIENTO, 4);
    indice += 4;

    sprintf(buffer + indice, "%011d", 0);
    LOGI("buffer %s ", buffer);
}

void empaquetarP60(DatosTokens datosTokens, uChar *tipoTransaccion) {

    uChar buffer[160 + 1];
    int indice = 0;
    char iniciaTransaccion[2 + 1];
    uChar tipoCuenta[SIZE_TIPO_CUENTA + 1];
    uChar cuenta[SIZE_NUMERO_CUENTA + 1];

    memset(buffer, 0x00, sizeof(buffer));
    memset(tipoCuenta, 0x30, sizeof(tipoCuenta));
    memset(cuenta, 0x30, sizeof(cuenta));
    memset(iniciaTransaccion, 0x00, sizeof(iniciaTransaccion));

    getParameter(TIPO_INICIA_TRANSACCION, iniciaTransaccion);
    LOGI("tipoTransaccion  %s", tipoTransaccion);
    if (atoi(tipoTransaccion) == TRANSACCION_BCL_RECAUDO && ((verificarHabilitacionCaja() < 0)
                                                             /*|| verificarCajaIPCB() == 1*/
                                                             || (strcmp(iniciaTransaccion,
                                                                        RECAUDO_CAJA_NUEVO_SERVIDOR)
                                                                 == 0))) {
        _setTokenMessage_("X1", "0007", TAM_FIID);
        LOGI("set x1  %s", tipoTransaccion);
    }

    _setTokenMessage_((char *) "R3", datosTokens.valorKotenR3, SIZE_TOKEN_R3);
    LOGI("set R3  %s", datosTokens.valorKotenR3);
    if (atoi(tipoTransaccion) == TRANSACCION_BCL_ANULACION) {
        _setTokenMessage_((char *) "QF", datosTokens.tokensQF, SIZE_TOKEN_QF);

        // AGREGAR EL NUMERO DE RECIBO EN LA POSICION 3 DEL BUFFER ( SIN INCLUIR EL TIPO DE TAG NI LA LONGITUD )
        memcpy(datosTokens.tokensp6 + 2, datosTokens.referencia1, TAM_NUMERO_RECIBO);

        _setTokenMessage_((char *) "P6", datosTokens.tokensp6, SIZE_TOKEN_P6);

        return;
    }

    /////////// TOKEMS QF ////////////
    if (strlen(datosTokens.cuentaOrigen) > 0) {
        leftPad(cuenta, datosTokens.cuentaOrigen, '0', SIZE_NUMERO_CUENTA);
    }

    sprintf(buffer, "%s%s", datosTokens.tipoCuentaOrigen, cuenta);
    indice += (SIZE_TIPO_CUENTA + SIZE_NUMERO_CUENTA);

    memset(tipoCuenta, 0x30, sizeof(tipoCuenta));
    memset(cuenta, 0x30, sizeof(cuenta));

    if (strlen(datosTokens.tipoCuentaDestino) > 0) {
        memset(tipoCuenta, 0x00, sizeof(tipoCuenta));
        memcpy(tipoCuenta, datosTokens.tipoCuentaDestino, SIZE_TIPO_CUENTA);
    }

    if (strlen(datosTokens.cuentaDestino) > 0) {
        memset(cuenta, 0x00, sizeof(cuenta));
        leftPad(cuenta, datosTokens.cuentaDestino, '0', SIZE_NUMERO_CUENTA);
    }

    memcpy(buffer + indice, tipoCuenta, SIZE_TIPO_CUENTA);
    indice += SIZE_TIPO_CUENTA;

    memcpy(buffer + indice, cuenta, SIZE_NUMERO_CUENTA);
    indice += SIZE_NUMERO_CUENTA;

    memset(tokensQF, 0x00, sizeof(tokensQF));
    memcpy(tokensQF, buffer, SIZE_TOKEN_QF);

    _setTokenMessage_((char *) "QF", buffer, indice);
    LOGI("set QF  %s", buffer);
    armarTokensP6(datosTokens, atoi(tipoTransaccion));
}

void armarTokensP6(DatosTokens datosTokens, int tipoTransaccion) {

    uChar buffer[160 + 1];
    int indice = 0;
    DatosTarjetaSembrada datosTarjetaSembrada;
    uChar referenciaAux1[TAM_P6_NUM_REFERENCIA + 1];
    uChar referenciaAux2[TAM_P6_NUM_REFERENCIA + 1];
    uChar iniciaTransaccion[2 + 1];

    memset(buffer, 0x30, sizeof(buffer));
    memset(&datosTarjetaSembrada, 0x30, sizeof(datosTarjetaSembrada));
    memset(tokensP6, 0x00, sizeof(tokensP6));
    memset(iniciaTransaccion, 0x00, sizeof(iniciaTransaccion));

    getParameter(TIPO_INICIA_TRANSACCION, iniciaTransaccion);

    ///// Indica que no hay nada para el tokens P6
    if (strlen(datosTokens.indicador1) == 0 && strlen(datosTokens.indicador2) == 0) {
        LOGI(" retorna");
        return;
    }

    if (tipoTransaccion != TRANSACCION_BCL_RECAUDO) {
        formatearDatos(&datosTokens);
    }

    //// SE EMPAQUETAN LOS INDICADORES
    memcpy(buffer + indice, datosTokens.indicador1, SIZE_INDICADOR);
    indice += SIZE_INDICADOR;

    memcpy(buffer + indice, datosTokens.indicador2, SIZE_INDICADOR);
    indice += SIZE_INDICADOR;
    LOGI(" tipoTxx %d", tipoTransaccion);
    switch (tipoTransaccion) {
        case TRANSACCION_BCL_SALDO:
            datosTarjetaSembrada = cargarTarjetaSembrada();

            /////////// TOKEMS P6 ////////////
            sprintf(buffer + indice, "%012d", 0);
            indice += 12;

            memcpy(buffer + indice, datosTarjetaSembrada.tipoCuenta, TAM_TIPO_CUENTA);
            indice += TAM_TIPO_CUENTA;

            memcpy(buffer + indice, datosTarjetaSembrada.pan, strlen(datosTarjetaSembrada.pan));
            indice += strlen(datosTarjetaSembrada.pan);

            memcpy(buffer + indice, datosTokens.factura, strlen(datosTokens.factura));
            indice += strlen(datosTokens.factura);
            break;

        case TRANSACCION_BCL_DEPOSITO:
            break;

        case TRANSACCION_BCL_TRANSFERENCIA:
            if (strcmp(datosTokens.indicador1, INDICADOR_MANUAL) ==
                0) { ///// PARA LECTURA MANUAL, SE ARMAR LA FACTURA CON EL COD DE SERVICIO Y LA REFERENCIA
                memcpy(datosTokens.factura, datosTokens.referencia2 + 25,
                       5); //// Se toman los 5 digitos del cod de servicio que estan justificados con 0 a la izquierda en el campo referencia

                leftPad(datosTokens.factura + 5, datosTokens.referencia1, '0', SIZE_FACTURA - 5);
            }

            memcpy(buffer + indice, datosTokens.referencia1, SIZE_REFERENCIA);
            indice += SIZE_REFERENCIA;
            memcpy(buffer + indice, datosTokens.factura, strlen(datosTokens.factura));
            indice += strlen(datosTokens.factura);
            break;
        case TRANSACCION_BCL_PAGO_TARJETA:
        case TRANSACCION_BCL_PAGO_CARTERA:
            LOGI(" entro case  %d", tipoTransaccion);
            memcpy(buffer + indice, datosTokens.referencia1, SIZE_REFERENCIA);//SI VA ORIGINAL
            indice += SIZE_REFERENCIA;
            memset(datosTokens.factura, 0x30, sizeof(datosTokens.factura));
            leftPad(datosTokens.factura, datosVentaBancolombia.tokenVivamos, '0', 128);
            memcpy(buffer + indice, datosTokens.factura, strlen(datosTokens.factura));
            indice += strlen(datosTokens.factura);
            LOGI(" sale de  case  %d", tipoTransaccion);
            break;
        case TRANSACCION_BCL_RECAUDO:

            if (verificarHabilitacionCaja() <= 0 /*|| verificarCajaIPCB() == 1*/
                || strcmp(iniciaTransaccion, RECAUDO_CAJA_NUEVO_SERVIDOR) == 0) {
                memset(buffer, 0x00, sizeof(buffer));
                memset(referenciaAux1, 0x00, sizeof(referenciaAux1));
                memset(referenciaAux2, 0x00, sizeof(referenciaAux2));
                leftTrim(referenciaAux1, datosTokens.referencia1, '0');
                _rightPad_(referenciaAux2, referenciaAux1, ' ', TAM_P6_NUM_REFERENCIA);
                memcpy(buffer, datosTokens.tokensp6, SIZE_TOKEN_P6);
                memcpy(buffer + 2, datosVentaBancolombia.numeroRecibo, TAM_NUMERO_RECIBO);
            } else {

                if (strcmp(datosTokens.indicador1, INDICADOR_MANUAL) ==
                    0) { ///// PARA LECTURA MANUAL, SE ARMAR LA FACTURA CON EL COD DE SERVICIO Y LA REFERENCIA
//				memcpy(datosTokens.factura, datosTokens.referencia2 + 25, 5); //// Se toman los 5 digitos del cod de servicio que estan justificados con 0 a la izquierda en el campo referencia
                    memset(referenciaAux1, 0x00, sizeof(referenciaAux1));
                    memset(referenciaAux2, 0x00, sizeof(referenciaAux2));
                    memcpy(referenciaAux1, datosTokens.referencia2, 5);
                    _rightTrim_(referenciaAux2, referenciaAux1, ' ');
                    memset(referenciaAux1, 0x00, sizeof(referenciaAux1));
                    leftPad(referenciaAux1, referenciaAux2, '0', 5);

                    memcpy(datosTokens.factura, referenciaAux1,
                           5); //// Se toman los 5 digitos del cod de servicio que estan justificados con 0 a la izquierda en el campo referencia

                    leftPad(datosTokens.factura + 5, datosTokens.referencia1, '0',
                            SIZE_FACTURA - 5);
                }

                leftPad(buffer + indice, datosTokens.referencia1, '0', SIZE_REFERENCIA);

                indice += SIZE_REFERENCIA;
                memcpy(buffer + indice, datosTokens.factura, strlen(datosTokens.factura));
                indice += strlen(datosTokens.factura);
                //}

                break;
                default:
                    break;
            }
    }
    memset(tokensP6, 0x00, sizeof(tokensP6));
    memcpy(tokensP6, buffer, SIZE_TOKEN_P6);
    LOGI("tokensP6    %s", tokensP6);
    _setTokenMessage_((char *) "P6", buffer, SIZE_TOKEN_P6);

}

int verificacionTramabancolombia(DatosCnbBancolombia *tramaVenta) {

    int resultado = 0;
    int validacion = 1;
    long variableAux = 0;
    uChar stringAux[20];
    ISOFieldMessage isoFieldMessage;
    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(stringAux, 0x00, sizeof(stringAux));

    isoFieldMessage = getField(37);
    memcpy(tramaVenta->rrn, isoFieldMessage.valueField + 6, 6);
    variableAux = atol(tramaVenta->rrn);
    isoFieldMessage = getField(38);
    memcpy(tramaVenta->codigoAprobacion, isoFieldMessage.valueField, 6);
    LOGI("tramaVenta->codigoAprobacion %s ", tramaVenta->codigoAprobacion);
    if (variableAux != 0 && tramaVenta->codigoAprobacion[0] != 0x00) {
        /// Validar Codigo Proceso-Monto-System Trace-N. Terminal.
        isoFieldMessage = getField(3);
        validacion = strncmp(isoFieldMessage.valueField, tramaVenta->processingCode,
                             isoFieldMessage.totalBytes);
        LOGI("Processing code  %s %s ", isoFieldMessage.valueField, tramaVenta->processingCode);
        if (validacion == 0) {
            if (strlen(tramaVenta->totalVenta) > 0) {
                isoFieldMessage = getField(4);
                variableAux = atol(isoFieldMessage.valueField);
                LOGI("total ventaa %s %s ", isoFieldMessage.valueField, tramaVenta->totalVenta);
                if (variableAux == atol(tramaVenta->totalVenta)) {
                    validacion = 0;
                } else {
                    validacion = -1;
                }
            } else {
                validacion = 0;
            }
        }

        if (validacion == 0) {
            isoFieldMessage = getField(11);
            LOGI("systemtrace %s %s ", isoFieldMessage.valueField, tramaVenta->systemTrace);
            validacion = strncmp(isoFieldMessage.valueField, tramaVenta->systemTrace,
                                 isoFieldMessage.totalBytes);
        }

        if (validacion == 0) {
            isoFieldMessage = getField(41);
            LOGI("terminal %s %s ", isoFieldMessage.valueField, tramaVenta->terminalId);
            validacion = strncmp(isoFieldMessage.valueField, tramaVenta->terminalId,
                                 isoFieldMessage.totalBytes);
        }

    }

    if (validacion == 0) {
        resultado = 1;
    } else {
        resultado = -1;
    }

    return resultado;
}

void guardarTransaccionBancolombia(char *nombreDisco, char *nombreArchivo,
                                   DatosCnbBancolombia datosDeposito) {

    int tamJournal = 0;
    char journalVenta[TAM_JOURNAL_BANCOLOMBIA + 1];

    tamJournal = sizeof(datosDeposito);

    memcpy(journalVenta, &datosDeposito, sizeof(datosDeposito));

    escribirArchivo(nombreDisco, nombreArchivo, journalVenta, tamJournal);
}

void obtenerTerminalModoCorresponsal(uChar *terminal) {

    int modo = 0;
    modo = verificarModoCNB();

    if (modo == MODO_PURO) {
        getParameter(NUMERO_TERMINAL, terminal);
    } else {
        getParameter(CNB_TERMINAL, terminal);
    }
}

void empaquetarEncabezado(DatosVenta *datosDeposito, uChar *pan, int intentosVentas) {

    ISOHeader isoHeader8583;
    uChar nii[TAM_NII + 1];
    DatosTokens datosToken;

    memset(&isoHeader8583, 0x00, sizeof(isoHeader8583));
    memset(nii, 0x00, sizeof(nii));
    memset(&datosToken, 0x00, sizeof(datosToken));

    getParameter(NII, nii);

    if (intentosVentas == 0) {
        obtenerTerminalModoCorresponsal(datosDeposito->terminalId);
        strcat(datosDeposito->totalVenta, "00");
        generarInvoice(datosDeposito->numeroRecibo);
        generarSystemTraceNumber(datosDeposito->systemTrace, (char *) " ");
    }


    memcpy(datosDeposito->nii, nii + 1, TAM_NII);
    memcpy(datosDeposito->posConditionCode, "00", TAM_POS_CONDITION);

    isoHeader8583.TPDU = 60;
    memcpy(isoHeader8583.destination, nii, TAM_NII + 1);
    memcpy(isoHeader8583.source, nii, TAM_NII + 1);
    setHeader(isoHeader8583);

    setMTI(datosDeposito->msgType);

    setField(3, datosDeposito->processingCode, TAM_CODIGO_PROCESO);
    setField(4, datosDeposito->totalVenta, 12);
    setField(11, datosDeposito->systemTrace, TAM_SYSTEM_TRACE);
    setField(22, datosDeposito->posEntrymode, TAM_ENTRY_MODE);
    setField(24, datosDeposito->nii, TAM_NII);
    setField(25, datosDeposito->posConditionCode, TAM_POS_CONDITION);

    if (strlen(datosDeposito->track2) == 0) {
        armarTrack(datosDeposito->track2, pan, atoi(datosDeposito->tipoTransaccion));
    }
    setField(35, datosDeposito->track2, strlen(datosDeposito->track2));

    setField(41, datosDeposito->terminalId, TAM_TERMINAL_ID);

    //// SI ES EMV /// por ahora no desarrollo datafono android 13022020
    if (globalsizep55 > 0) {
        setField(55, globalCampo55, globalsizep55);
        if (strlen(globalpinblock) < 0) {
            setField(53, SEGURIDAD_PIN_EMV, 16);
        }
    }
}

void getTextoTransaccion(int tipoTransaccion, char *texto) {

    memset(texto, 0x00, sizeof(texto));

    switch (tipoTransaccion) {
        case TRANSACCION_BCL_SALDO:
            strcpy(texto, "SALDO");
            break;

        case TRANSACCION_BCL_DEPOSITO:
            strcpy(texto, "DEPOSITO");
            break;

        case TRANSACCION_BCL_RECAUDO:
            if (isRecaudoNequi == 1) {
                strcpy(texto, "RECARGA");
            } else {
                strcpy(texto, "RECAUDO");
            }
            break;

        case TRANSACCION_BCL_RETIRO:
        case TRANSACCION_BCL_RETIRO_EFECTIVO:
            strcpy(texto, "RETIRO");
            break;

        case TRANSACCION_BCL_TRANSFERENCIA:
            strcpy(texto, "TRANSFERENCIA");
            break;

        case TRANSACCION_BCL_PAGO_CARTERA:
        case TRANSACCION_BCL_PAGO_TARJETA:
            strcpy(texto, "PAGO");
            break;
        case TRANSACCION_BCL_GIROS:
            strcpy(texto, "ENVIO GIRO");
            break;
        case TRANSACCION_BCL_RECLAMAR_GIROS:
            strcpy(texto, "RECLAMAR GIRO");
            break;
        case TRANSACCION_BCL_CANCELAR_GIROS:
            strcpy(texto, "CANCELACION GIRO");
            break;
        default:
            strcpy(texto, "CIERRE");
            break;

    }

}

void imprimirTicketBancolombia(DatosCnbBancolombia reciboVenta, int tipoTicket, int duplicado) {
    char textoCredito[] =
            "PAGARE INCONDICIONALMENTE A LA VISTA Y A LA ORDEN DEL ACREEDOR EL VALOR TOTAL DE ESTE PAGARE JUNTO CON LOS INTERESES A LAS TASAS MAXIMAS PERMITIDAS POR LA LEY.";
    uChar parPie[20 + 1];
    uChar valorFormato[20 + 1];
    uChar aprob[15 + 1];
    uChar rrnVenta[20 + 1];
    uChar stringAux[20 + 1];
    uChar parDuplicado[30 + 1];
    uChar parVersion[20 + 1];
    uChar parCardName[TAM_CARD_NAME + 1];
    uChar parVenta[20 + 1];
    uChar cardName[15 + 1];
    uChar textoComercio[30 + 1];
    uChar parSaludo[35 + 1];
    uChar bin[9 + 1];
    uChar parTextoCompraNeta[20 + 1];
    uChar parLogo[35 + 1];
    uChar cuentaAuxiliar[SIZE_MAX_NUM_TARJETA + 1];
    uChar numeroCuentaOrigen[32 + 1];
    uChar numeroCuentaDestino[32 + 1];
    EmisorAdicional emisorAdicional;
    TablaUnoInicializacion tablaUno;
    int isColsubsidio = 0;
    char modoInicia[2 + 1];
    int modoDatafono = 0;
    uChar parInformacion[TAM_TEXTO_INFORMACION + 1];
    uChar parFirma[50 + 1];
    uChar parCedula[50 + 1];
    uChar parTel[50 + 1];
    uChar parCosto[30 + 1];
    uChar parVCosto[20 + 1];
    uChar AID[TAM_AID + 1];
    uChar ARQC[TAM_ARQC + 1];
    uChar espacio[20 + 1];
    uChar cuenta[2 + 1];
    uChar textoVenta[15 + 1];
    uChar vConvenio[15 + 1];
    uChar nombre[24 + 1];
    uChar factura[20 + 1];
    uChar espacio0[20 + 1];
    uChar convenio[20 + 1];
    uChar vFactura[24 + 1];
    uChar parAuxiliar[30 + 1];
    uChar buffer[30 + 1];
    uChar fechaExpiracion[20 + 1];
    uChar parTextoCredito[160 + 1];
    uChar parTextoAuxiliar[20 + 1];
    uChar totalVentaAux[TAM_COMPRA_NETA + 1];
    uChar numCuenta[32 + 1];
    char celGirador[32 + 1];
    char celBeneficiario[32 + 1];
    char parGiro[20 + 1];
    char parTextoGiro[20 + 1];
    char parComision[20 + 1];
    char parTextoComision[20 + 1];
    char parIva[20 + 1];
    char parTextoIva[20 + 1];
    int indice = 0;

    memset(parTextoCompraNeta, 0x00, sizeof(parTextoCompraNeta));
    memset(parLogo, 0x00, sizeof(parLogo));
    memset(parSaludo, 0x00, sizeof(parSaludo));
    memset(parPie, 0x00, sizeof(parPie));
    memset(aprob, 0x00, sizeof(aprob));
    memset(rrnVenta, 0x00, sizeof(rrnVenta));
    memset(valorFormato, 0x00, sizeof(valorFormato));
    memset(parDuplicado, 0x00, sizeof(parDuplicado));
    memset(parVersion, 0x00, sizeof(parVersion));
    memset(parCardName, 0x00, sizeof(parCardName));
    memset(parVenta, 0x00, sizeof(parVenta));
    memset(cardName, 0x00, sizeof(cardName));
    memset(textoComercio, 0x00, sizeof(textoComercio));
    memset(cuentaAuxiliar, 0x00, sizeof(cuentaAuxiliar));
    memset(numeroCuentaOrigen, 0x00, sizeof(numeroCuentaOrigen));
    memset(numeroCuentaDestino, 0x00, sizeof(numeroCuentaDestino));
    memset(modoInicia, 0x00, sizeof(modoInicia));
    memset(parInformacion, 0x00, sizeof(parInformacion));
    memset(parFirma, 0x00, sizeof(parFirma));
    memset(parCedula, 0x00, sizeof(parCedula));
    memset(parTel, 0x00, sizeof(parTel));
    memset(parCosto, 0x00, sizeof(parCosto));
    memset(parVCosto, 0x00, sizeof(parVCosto));
    memset(AID, 0x00, sizeof(AID));
    memset(ARQC, 0x00, sizeof(ARQC));
    memset(espacio, 0x00, sizeof(espacio));
    memset(cuenta, 0x00, sizeof(cuenta));
    memset(textoVenta, 0x00, sizeof(textoVenta));
    memset(vConvenio, 0x00, sizeof(vConvenio));
    memset(nombre, 0x00, sizeof(nombre));
    memset(espacio0, 0x00, sizeof(espacio0));
    memset(convenio, 0x00, sizeof(convenio));
    memset(vFactura, 0x00, sizeof(vFactura));
    memset(parAuxiliar, 0x00, sizeof(parAuxiliar));
    memset(buffer, 0x00, sizeof(buffer));
    memset(fechaExpiracion, 0x00, sizeof(fechaExpiracion));
    memset(parTextoCredito, 0x00, sizeof(parTextoCredito));
    memset(parTextoAuxiliar, 0x00, sizeof(parTextoAuxiliar));
    memset(totalVentaAux, 0x00, sizeof(totalVentaAux));
    memset(numCuenta, 0x00, sizeof(numCuenta));
    memset(parGiro, 0x00, sizeof(parGiro));
    memset(parTextoGiro, 0x00, sizeof(parTextoGiro));
    memset(parComision, 0x00, sizeof(parComision));
    memset(parTextoComision, 0x00, sizeof(parTextoComision));
    memset(parIva, 0x00, sizeof(parIva));
    memset(parTextoIva, 0x00, sizeof(parTextoIva));
    memset(celGirador, 0x00, sizeof(celGirador));
    memset(celBeneficiario, 0x00, sizeof(celBeneficiario));

    strcpy(AID, NO_IMPRIMIR);
    strcpy(ARQC, NO_IMPRIMIR);
    strcpy(parCardName, NO_IMPRIMIR);
    strcpy(parSaludo, NO_IMPRIMIR);
    strcpy(parCosto, NO_IMPRIMIR);
    strcpy(parVCosto, NO_IMPRIMIR);
    strcpy(espacio, NO_IMPRIMIR);
    strcpy(textoVenta, NO_IMPRIMIR);
    strcpy(vConvenio, NO_IMPRIMIR);
    strcpy(nombre, NO_IMPRIMIR);
    strcpy(vFactura, NO_IMPRIMIR);
    strcpy(espacio0, NO_IMPRIMIR);
    strcpy(numeroCuentaOrigen, NO_IMPRIMIR);
    strcpy(parAuxiliar, NO_IMPRIMIR);
    strcpy(numeroCuentaDestino, NO_IMPRIMIR);
    strcpy(parTextoAuxiliar, NO_IMPRIMIR);
    strcpy(parGiro, NO_IMPRIMIR);
    strcpy(parTextoGiro, NO_IMPRIMIR);
    strcpy(parComision, NO_IMPRIMIR);
    strcpy(parTextoComision, NO_IMPRIMIR);
    strcpy(parIva, NO_IMPRIMIR);
    strcpy(parTextoIva, NO_IMPRIMIR);
    strcpy(celGirador, NO_IMPRIMIR);
    strcpy(celBeneficiario, NO_IMPRIMIR);

    int existeEmisor = 0;
    memset(bin, 0x00, sizeof(bin));
    memcpy(bin, reciboVenta.track2, 9);
    sprintf(parLogo, "%s%s", "0000", ".BMP");
    existeEmisor = buscarBinEmisor(bin);

    if (existeEmisor > 0) {
        memset(&emisorAdicional, 0x00, sizeof(emisorAdicional));
        emisorAdicional = traerEmisorAdicional(existeEmisor);
        if (emisorAdicional.existeIssuerEmisor == 1) {
            memset(parLogo, 0x00, sizeof(parLogo));
            memset(bin, 0x00, sizeof(bin));
            _convertBCDToASCII_(bin, &emisorAdicional.indiceImagen, 2);
            existeEmisor = atoi(bin);
            sprintf(parLogo, "%04d%s", existeEmisor, ".BMP");
        }
    }
    /////ES COLSUBSIDIO/////
    if (strncmp(bin, "8800", 4) == 0) {
        isColsubsidio = 1;
    }

    if (atoi(reciboVenta.tipoTransaccion) == TRANSACCION_BCL_GIROS
        || atoi(reciboVenta.tipoTransaccion) == TRANSACCION_BCL_RECLAMAR_GIROS) {
        sprintf(parTextoCompraNeta, "%-10s", "TOTAL");
    } else {
        sprintf(parTextoCompraNeta, "%-10s", "VALOR");
    }

    if (atoi(reciboVenta.tipoTransaccion) == TRANSACCION_BCL_SALDO) {
        memset(parTextoCompraNeta, 0x00, sizeof(parTextoCompraNeta));
        //sprintf(parTextoCompraNeta,"%-11s","VALOR");
        sprintf(parTextoCompraNeta, "%-11s", "DISPONIBLE");
    }

    sprintf(fechaExpiracion, "%15s", " ");

    uChar tipoCuenta[12 + 1];
    uChar hora[10 + 1];
    memset(tipoCuenta, 0x00, sizeof(tipoCuenta));
    memset(hora, 0x00, sizeof(hora));

    /// TRAE DATOS TABLA UNO INICIALIZACION ///

    memset(&tablaUno, 0x00, sizeof(tablaUno));
    modoDatafono = verificarModoCNB();

    if (modoDatafono == MODO_MIXTO) {
        tablaUno = desempaquetarCNB();
    } else {
        tablaUno = _desempaquetarTablaCeroUno_();
    }

    /// System Trace ///
    uChar numeroRecibo[15 + 1];
    memset(numeroRecibo, 0x00, sizeof(numeroRecibo));
    strcpy(numeroRecibo, "RECIBO: ");
    LOGI("numeroRecibo %s", reciboVenta.numeroRecibo);
    strcat(numeroRecibo, reciboVenta.numeroRecibo);

    /// TOTAL VENTA ///
    uChar totalVenta[20 + 1];
    uChar totalAnulacion[20 + 1];
    memset(totalVenta, 0x00, sizeof(totalVenta));

    memset(valorFormato, 0x00, sizeof(valorFormato));
    if (strcmp(reciboVenta.totalVenta, totalVentaAux) == 0) {
        memset(reciboVenta.totalVenta, 0x30, 3);
        formatString(0x30, 0, reciboVenta.totalVenta, strlen(reciboVenta.totalVenta) - 2,
                     valorFormato, 1);
    } else {
        formatString(0x00, 0, reciboVenta.totalVenta, strlen(reciboVenta.totalVenta) - 2,
                     valorFormato, 1);
    }

    sprintf(totalVenta, "%12s", valorFormato);

    strcpy(aprob, "APRO: ");
    strcat(aprob, reciboVenta.codigoAprobacion);
    LOGI("apro %s", aprob);
    sprintf(rrnVenta, "RRN: %s", reciboVenta.rrn);

    /// ULTIMOS CUATRO ///
    uChar ultimosCuatro[6 + 1];
    memset(ultimosCuatro, 0x00, sizeof(ultimosCuatro));

    if (atoi(reciboVenta.tipoTransaccion) == TRANSACCION_BCL_GIROS
        || atoi(reciboVenta.tipoTransaccion) == TRANSACCION_BCL_RECLAMAR_GIROS
        || atoi(reciboVenta.tipoTransaccion) == TRANSACCION_BCL_CANCELAR_GIROS) {
        strcpy(ultimosCuatro, "      ");
    } else {
        strcpy(ultimosCuatro, "**");
        strcat(ultimosCuatro, reciboVenta.ultimosCuatro);
    }

    /// FECHA ///
    formatoFecha(reciboVenta.txnDate);

    /// HORA ///
    sprintf(hora, "%.2s:%.2s:%.2s", reciboVenta.txnTime, reciboVenta.txnTime + 2,
            reciboVenta.txnTime + 4);
//    strcpy(fechaExpiracion,NO_IMPRIMIR);

    switch (atoi(reciboVenta.tipoCuenta)) {
        case 10:
            sprintf(cuenta, "%s", "Ah");
            memset(textoCredito, 0x00, sizeof(textoCredito));
            strcpy(textoCredito, NO_IMPRIMIR);
            break;
        case 20:
            sprintf(cuenta, "%s", "Cc");
            memset(textoCredito, 0x00, sizeof(textoCredito));
            strcpy(textoCredito, NO_IMPRIMIR);
            break;
        case 30:
            sprintf(cuenta, "%s", "Cr");
            memset(fechaExpiracion, 0x00, sizeof(fechaExpiracion));
            sprintf(fechaExpiracion, "EXP: %-10s", reciboVenta.fechaExpiracion);
            break;
        case 40:
            if (isColsubsidio == 1) {
                sprintf(cuenta, "%s", "Cr");
            } else {
                sprintf(cuenta, "%s", "Ah");
            }
            memset(textoCredito, 0x00, sizeof(textoCredito));
            strcpy(textoCredito, NO_IMPRIMIR);

            memset(fechaExpiracion, 0x00, sizeof(fechaExpiracion));
            sprintf(fechaExpiracion, "%s", "               ");
            break;
        case 41:
            sprintf(cuenta, "%s", "LT");
            memset(textoCredito, 0x00, sizeof(textoCredito));
            strcpy(textoCredito, NO_IMPRIMIR);
            break;
        case 42:
            sprintf(cuenta, "%s", "CM");
            memset(textoCredito, 0x00, sizeof(textoCredito));
            strcpy(textoCredito, NO_IMPRIMIR);
            break;
        case 43:
            sprintf(cuenta, "%s", "CU");
            memset(fechaExpiracion, 0x00, sizeof(fechaExpiracion));
            sprintf(fechaExpiracion, "EXP: %-10s", reciboVenta.fechaExpiracion);
            memset(textoCredito, 0x00, sizeof(textoCredito));
            strcpy(textoCredito, NO_IMPRIMIR);
            break;
        case 44:
            sprintf(cuenta, "%s", "BE");
            memset(textoCredito, 0x00, sizeof(textoCredito));
            strcpy(textoCredito, NO_IMPRIMIR);
            break;

        case 45:
            sprintf(cuenta, "%s", "RT");
            memset(fechaExpiracion, 0x00, sizeof(fechaExpiracion));
            sprintf(fechaExpiracion, "EXP: %-10s", reciboVenta.fechaExpiracion);
            memset(textoCredito, 0x00, sizeof(textoCredito));
            strcpy(textoCredito, NO_IMPRIMIR);
            break;
        case 46:
            sprintf(cuenta, "%s", "RG");
            memset(textoCredito, 0x00, sizeof(textoCredito));
            strcpy(textoCredito, NO_IMPRIMIR);
            break;
        case 47:
            sprintf(cuenta, "%s", "DS");
            memset(textoCredito, 0x00, sizeof(textoCredito));
            strcpy(textoCredito, NO_IMPRIMIR);
            break;

        case 48:
            sprintf(cuenta, "%s", "DB");
            memset(textoCredito, 0x00, sizeof(textoCredito));
            strcpy(textoCredito, NO_IMPRIMIR);
            break;

        case 49:
            sprintf(cuenta, "%s", "BC");
            memset(fechaExpiracion, 0x00, sizeof(fechaExpiracion));
            sprintf(fechaExpiracion, "EXP: %-10s", reciboVenta.fechaExpiracion);
            memset(textoCredito, 0x00, sizeof(textoCredito));
            strcpy(textoCredito, NO_IMPRIMIR);
            break;

        default:
            break;
    }
    if (strcmp(reciboVenta.tipoRetiro, "NQ") == 0) {
        strcpy(textoVenta, "RECARGA");
    } else {
        getTextoTransaccion(atoi(reciboVenta.tipoTransaccion), textoVenta);
    }

    if (tipoTicket == RECIBO_COMERCIO) {
        strcpy(parTextoCredito, textoCredito);
        strcpy(parPie, "*** COMERCIO ***");
        strcpy(parFirma, "FIRMA: _____________________________________");
        strcpy(parCedula, "C.C:   _____________________________________");
        strcpy(parTel, "TEL:   _____________________________________");
    } else if (tipoTicket == RECIBO_CLIENTE) {
        strcpy(parTextoCredito, NO_IMPRIMIR);
        strcpy(parPie, "*** CLIENTE ***");
        strcpy(parFirma, NO_IMPRIMIR);
        strcpy(parCedula, NO_IMPRIMIR);
        strcpy(parTel, NO_IMPRIMIR);
    }

    if (duplicado == RECIBO_DUPLICADO && reciboVenta.estadoTransaccion[0] == '1') {
        memset(parDuplicado, 0x00, sizeof(parDuplicado));
        strcpy(parDuplicado, "     ** DUPLICADO **");
        strcpy(espacio, "  ");

        sprintf(parVenta, "%-16s", textoVenta);

    } else if ((duplicado == RECIBO_DUPLICADO && reciboVenta.estadoTransaccion[0] == '0') ||
               duplicado == 6) { //6 anulacion
        memset(parDuplicado, 0x00, sizeof(parDuplicado));
        memset(parTextoAuxiliar, 0x00, sizeof(parTextoAuxiliar));

        if (duplicado == 6) {
            strcpy(parDuplicado, NO_IMPRIMIR);
            strcpy(parDuplicado, NO_IMPRIMIR);
        } else {
            strcpy(parDuplicado, "     ** DUPLICADO **");
        }

        //strcpy(parTextoAuxiliar,textoVenta);
        sprintf(parTextoAuxiliar, "%s", textoVenta);

        sprintf(parVenta, "%-16s", "ANULACION");

        memset(valorFormato, 0x00, sizeof(valorFormato));
        memset(totalVenta, 0x00, sizeof(totalVenta));
        formatString(0x00, 0, reciboVenta.totalVenta, strlen(reciboVenta.totalVenta) - 2,
                     valorFormato, 1);

        sprintf(totalAnulacion, "-%s", valorFormato);
        sprintf(totalVenta, "%12s", totalAnulacion);

        strcpy(parTextoCredito, NO_IMPRIMIR);
        strcpy(parFirma, NO_IMPRIMIR);
        strcpy(parCedula, NO_IMPRIMIR);
        strcpy(parTel, NO_IMPRIMIR);
//		memset(parTextoTotal,0x00,sizeof(parTextoTotal));
//		sprintf(parTextoTotal,"%-10s","ANULACION");
    } else {
        memset(parDuplicado, 0x00, sizeof(parDuplicado));
        strcpy(parDuplicado, NO_IMPRIMIR);
        sprintf(parVenta, "%-16s", textoVenta);
    }

    if (duplicado == 6) {
        strcpy(parTextoCredito, NO_IMPRIMIR);
        strcpy(parFirma, NO_IMPRIMIR);
        strcpy(parCedula, NO_IMPRIMIR);
        strcpy(parTel, NO_IMPRIMIR);
    }

    if (strlen(reciboVenta.ARQC) > 0) {
        memset(ARQC, 0x00, sizeof(ARQC));
        strcpy(ARQC, reciboVenta.ARQC);
    }

    if (strlen(reciboVenta.AID) > 0) {
        memset(AID, 0x00, sizeof(AID));
        strcpy(AID, reciboVenta.AID);
    }

    if (strlen(reciboVenta.ARQC) > 0 || strlen(reciboVenta.AID) >
                                        0) { // Segun version 12.3 toda transaccion con chip (Chip Banda, Chip PayPass) se debe mostrar ese mensaje

        memset(parSaludo, 0x00, sizeof(parSaludo));
        strcpy(parSaludo, "BIENVENIDO AL MUNDO CONTACTLESS");
        strcpy(parCardName, reciboVenta.appLabel);
    }

    ///// COSTO TRANSACCION ///////
    if (strlen(reciboVenta.field61) > 0 && atoi(reciboVenta.field61) > 0) {
        memset(parVCosto, 0x00, sizeof(parVCosto));
        memset(stringAux, 0x00, sizeof(stringAux));
        formatString(0x30, 0, reciboVenta.field61, strlen(reciboVenta.field61), stringAux, 1);
        sprintf(parVCosto, "%22s", stringAux);
        sprintf(parCosto, "%-24s", "Costo de la Transaccion:");
    }

    ////  CONSULTA DE INFORMACION
    if (strlen(reciboVenta.textoInformacion) > 0) {
        strcpy(parInformacion, reciboVenta.textoInformacion);
    } else {
        strcpy(parInformacion, NO_IMPRIMIR);
    }

    sprintf(cardName, "%-15s", reciboVenta.cardName);
    if (strlen(reciboVenta.cardName) == 0) {
        strcpy(cardName, NO_IMPRIMIR);
    } else {
        sprintf(tipoCuenta, "%6s", cuenta);
    }

    if (atoi(reciboVenta.tipoTransaccion) == TRANSACCION_BCL_RECAUDO) {
        ///// TOKENS P6 ///////
        memcpy(parAuxiliar, reciboVenta.field60, strlen(reciboVenta.field60));

        if (strcmp(reciboVenta.tipoRetiro, "NQ") == 0) {
            strcpy(factura, "Nro celular:");
            memset(vFactura, 0x00, sizeof(vFactura));
            memset(buffer, 0x00, sizeof(buffer));
            memcpy(vFactura, reciboVenta.numeroReferencia, strlen(reciboVenta.numeroReferencia));
        } else {
            strcpy(convenio, "CONVENIO:");
            memset(vConvenio, 0x00, sizeof(vConvenio));
            memset(buffer, 0x00, sizeof(buffer));

            memcpy(buffer, reciboVenta.convenio, SIZE_COD_SERVICO);
            leftPad(vConvenio, buffer, '0', SIZE_COD_SERVICO);

            strcpy(factura, "REF:");
            memset(vFactura, 0x00, sizeof(vFactura));
            memset(buffer, 0x00, sizeof(buffer));

            memcpy(buffer, reciboVenta.numeroReferencia, strlen(reciboVenta.numeroReferencia));
            leftPad(vFactura, buffer, '0', SIZE_REFERENCIA_MANUAL);
            strcpy(espacio0, "  ");

        }

        strcpy(cardName, NO_IMPRIMIR);
        sprintf(ultimosCuatro, "%s", "      ");

        if (atoi(reciboVenta.cuotas) == MANUAL_CUENTA_RECAUDADORA) {
            sprintf(numCuenta, "CUENTA: %s", reciboVenta.cuenta);
        } else {
            strcpy(numCuenta, NO_IMPRIMIR);
        }

    } else if (atoi(reciboVenta.tipoTransaccion) == TRANSACCION_BCL_DEPOSITO) {
        ///// SOLO PARA DEPOSITO
        sprintf(ultimosCuatro, "%s    ", cuenta);
        sprintf(numeroCuentaOrigen, "CTA: %s", reciboVenta.cuentaDestino);
    } else if (atoi(reciboVenta.tipoTransaccion) == TRANSACCION_BCL_TRANSFERENCIA) {
        sprintf(numeroCuentaOrigen, "CTA ORIGEN: %.6s", reciboVenta.cuenta);
        indice += strlen(numeroCuentaOrigen);
        sprintf(numeroCuentaOrigen + indice, "*********%s", reciboVenta.cuenta + 15);

        sprintf(numeroCuentaDestino, "CTA DESTIN: %s", reciboVenta.cuentaDestino);
    } else if (atoi(reciboVenta.tipoTransaccion) == TRANSACCION_BCL_PAGO_CARTERA) {
        strcpy(cardName, NO_IMPRIMIR);
        sprintf(ultimosCuatro, "%s", "      ");
        strcpy(convenio, "CREDITO BANCOLOMBIA");
        sprintf(vConvenio, "%s", "      ");
        sprintf(parAuxiliar, "%s", "      ");
        strcpy(factura, "PAGARE:");

        sprintf(vFactura, "%s", reciboVenta.numeroReferencia);
    } else if (atoi(reciboVenta.tipoTransaccion) == TRANSACCION_BCL_PAGO_TARJETA) {
        strcpy(cardName, NO_IMPRIMIR);
        sprintf(ultimosCuatro, "%s", "      ");

        strcpy(convenio, NO_IMPRIMIR);
        strcpy(factura, "TARJETA DE CREDITO:");

        sprintf(vFactura, "%s", reciboVenta.numeroReferencia);
    } else if (atoi(reciboVenta.tipoTransaccion) == TRANSACCION_BCL_GIROS
               || atoi(reciboVenta.tipoTransaccion) == TRANSACCION_BCL_CANCELAR_GIROS) {

        if (atoi(reciboVenta.tipoTransaccion) == TRANSACCION_BCL_GIROS) {
            sprintf(celGirador, "Cel. Girador: %.15s", reciboVenta.facturaCaja);
            sprintf(celBeneficiario, "Cel. Beneficiario: %.15s", reciboVenta.appLabel);
        }

        sprintf(numCuenta, "ID Girador: %.15s", reciboVenta.tokensQF);
        sprintf(parAuxiliar, "ID Beneficiario: %.15s", reciboVenta.tokensQF + 15);

        memset(valorFormato, 0x00, sizeof(valorFormato));
        if (atol(reciboVenta.tokensP6 + 12) != 0) {
            formatString(0x30, 0, reciboVenta.tokensP6 + 12, strlen(reciboVenta.tokensP6 + 12) - 2,
                         valorFormato, 1);
        }
        sprintf(parGiro, "%12s", valorFormato);
        sprintf(parTextoGiro, "%-11s", "V.GIRO");

        if (atoi(reciboVenta.tipoTransaccion) == TRANSACCION_BCL_GIROS
            || atoi(reciboVenta.tipoTransaccion) == TRANSACCION_BCL_CANCELAR_GIROS) {
            memset(valorFormato, 0x00, sizeof(valorFormato));
            if (atol(reciboVenta.tokensP6 + 24) != 0 &&
                atoi(reciboVenta.tipoTransaccion) != TRANSACCION_BCL_CANCELAR_GIROS) {
                formatString(0x30, 0, reciboVenta.tokensP6 + 24, strlen(reciboVenta.tokensP6 + 24),
                             valorFormato, 1);
            } else if (atol(reciboVenta.tokensP6 + 24) != 0 &&
                       atoi(reciboVenta.tipoTransaccion) == TRANSACCION_BCL_CANCELAR_GIROS) {
                formatString(0x30, 0, reciboVenta.tokensP6 + 24,
                             strlen(reciboVenta.tokensP6 + 24) - 2, valorFormato, 1);
            }

            if (atoi(reciboVenta.tokensP6 + 24) > 0) {
                sprintf(parComision, "%12s", valorFormato);
            } else {
                memset(valorFormato, 0x00, sizeof(valorFormato));
                formatString(0x30, 0, (char *) "0", 1, valorFormato, 1);
                sprintf(parComision, "%12s", valorFormato);
            }
            sprintf(parTextoComision, "%-11s", "V.COMISION");

            memset(valorFormato, 0x00, sizeof(valorFormato));
            if (atol(reciboVenta.tokensP6) != 0 &&
                atoi(reciboVenta.tipoTransaccion) != TRANSACCION_BCL_CANCELAR_GIROS) {
                formatString(0x30, 0, reciboVenta.tokensP6, strlen(reciboVenta.tokensP6),
                             valorFormato, 1);
            } else if (atol(reciboVenta.tokensP6) != 0 &&
                       atoi(reciboVenta.tipoTransaccion) == TRANSACCION_BCL_CANCELAR_GIROS) {
                formatString(0x30, 0, reciboVenta.tokensP6, strlen(reciboVenta.tokensP6) - 2,
                             valorFormato, 1);
            }

            if (atoi(reciboVenta.tokensP6) > 0) {
                sprintf(parIva, "%12s", valorFormato);
            } else {
                memset(valorFormato, 0x00, sizeof(valorFormato));
                formatString(0x30, 0, (char *) "0", 1, valorFormato, 1);
                sprintf(parIva, "%12s", valorFormato);
            }
            sprintf(parTextoIva, "%-11s", "IVA");
        }
        strcpy(parTextoCredito, NO_IMPRIMIR);
    } else if (atoi(reciboVenta.tipoTransaccion) == TRANSACCION_BCL_RECLAMAR_GIROS) {

        sprintf(parAuxiliar, "ID Beneficiario: %.15s", reciboVenta.tokensQF + 15);
        strcpy(parTextoCredito, NO_IMPRIMIR);
    }

    getParameter(VERSION_SW, parVersion);
    getParameter(REVISION_SW, parVersion + strlen(parVersion));
    getParameter(TEXTO_COMERCIO, textoComercio);

    HasMap hash[] =
            {
                    {"_PAR_FECHA_",         reciboVenta.txnDate},
                    {"_PAR_VERSION_",       parVersion},
                    {"_PAR_HORA_",          hora},
                    {"_PAR_NESTABLE_",      tablaUno.defaultMerchantName},
                    {"_PAR_DESTABLE_",      tablaUno.receiptLine2},
                    {"_PAR_TEXTO_ADM_",     textoComercio},
                    {"_PAR_CUNICO_",        tablaUno.receiptLine3},
                    {"_PAR_TERMINAL_",      reciboVenta.terminalId},
                    {"_PAR_TARJETA_",       cardName},
                    {"_PAR_CTA_ORIGEN_",    numeroCuentaOrigen},
                    {"_PAR_CTA_DEST_",      numeroCuentaDestino},
                    {"_PAR_CUATROU_",       ultimosCuatro},
                    {"_PAR_RECIBO_",        numeroRecibo},
                    {"_PAR_RRN_",           rrnVenta},
                    {"_PAR_VENTA_",         parVenta},
                    {"_PAR_GIRO_",          parTextoGiro},
                    {"_PAR_VGIRO_",         parGiro},
                    {"_PAR_COMISION_",      parTextoComision},
                    {"_PAR_VCOMISION_",     parComision},
                    {"_PAR_IVA_",           parTextoIva},
                    {"_PAR_VIVA_",          parIva},
                    {"_PAR_COMPRA_",        parTextoCompraNeta},
                    {"_PAR_VCOMPRA_",       totalVenta},
                    {"_PAR_DUPLICADO_",     parDuplicado},
                    {"_PAR_APRO_",          aprob},
                    {"_PAR_CUENTA_",        tipoCuenta},
                    {"_PAR_SALUDO_",        parSaludo},
                    {"_PAR_PIE_",           parPie},
                    {"_PAR_ARQC_",          ARQC}, // modificar
                    {"_PAR_AID_",           AID}, // modificar
                    {"_PAR_LABEL_",         parCardName},
                    {"_PAR_EXP_",           fechaExpiracion},
                    {"_PAR_TARJ_HABIENTE_", reciboVenta.tarjetaHabiente},
                    {"_PAR_CUOTAS_",        (char *) "       "},
                    {"_PAR_COSTO_",         parCosto},
                    {"_PAR_VCOSTO_",        parVCosto},
                    {"_PAR_INFORMACION_",   parInformacion},
                    {"_PAR_FIRMA_",         parFirma},
                    {"_PAR_CEDULA_",        parCedula},
                    {"_PAR_TELEFONO_",      parTel},
                    {"_PAR_TEXTO1_BCL_",    textoBancolombia1},
                    {"_PAR_TEXTO2_BCL_",    textoBancolombia2},
                    {"_PAR_ESPACIO_",       espacio},
                    {"_PAR_TITULO1_",       (char *) "CORRESPONSAL"},
                    {"_PAR_TITULO2_",       (char *) "BANCOLOMBIA"},
                    {"_PAR_CONVENIO_",      convenio},
                    {"_PAR_VCONVENIO_",     vConvenio},
                    {"_PAR_NOM_CONVENIO_",  parAuxiliar},
                    {"_PAR_CEL_GIRADOR_",   celGirador},
                    {"_PAR_NUM_CUENTA_",    numCuenta},
                    {"_PAR_CEL_BENEFI_",    celBeneficiario},
                    {"_PAR_FACTURA_",       factura},
                    {"_PAR_VFACTURA",       vFactura},
                    {"_PAR_ESPACIO0_",      espacio0},
                    {"_PAR_CREDITO_",       parTextoCredito},
                    {"_PAR_AUXILIAR_",      parTextoAuxiliar},
                    {"FIN_HASH",            (char *) ""}
            };

    imprimirTicket(hash, parLogo, (char *) "/TICKETBCL.TPL");
}

DatosCnbBancolombia buscarReciboCNBByID(int id) {

    int resultado = 0;
    int posicion = 0;
    char dataDuplicado[TAM_JOURNAL_BANCOLOMBIA + 1];
    DatosCnbBancolombia datosVenta;
    int iteracion = 0;
    memset(dataDuplicado, 0x00, sizeof(dataDuplicado));
    memset(&datosVenta, 0x00, sizeof(datosVenta));

    do {
        memset(dataDuplicado, 0x00, sizeof(dataDuplicado));
        resultado = buscarArchivo(discoNetcom, (char *) JOURNAL_CNB_BANCOLOMBIA, dataDuplicado,
                                  posicion,
                                  sizeof(DatosCnbBancolombia));

        posicion += sizeof(DatosCnbBancolombia);
        if (iteracion == id) {
            memcpy(&datosVenta, dataDuplicado, sizeof(DatosCnbBancolombia));
            break;
        }
        iteracion++;

    } while (resultado == 0);

    return datosVenta;

}

void mostrarAprobacionBancolombia(DatosVenta *datosVenta) {

    int iRet = 0;
    int resultado = 0;
    int resultadoCaja = 0;

    uChar pan[20 + 1];
    uChar modo[2 + 1];
    uChar impresionRecibo[2 + 1];

    ISOFieldMessage isoFieldMessage;

    DatosCnbBancolombia datosBancolombia;


    memset(pan, 0x00, sizeof(pan));
    memset(modo, 0x00, sizeof(modo));
    memset(&datosBancolombia, 0x00, sizeof(datosBancolombia));
    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    //control de cambios login de operador
    if (atoi(datosVenta->tipoTransaccion) != TRANSACCION_BCL_SALDO) {
        setParameter(USER_CNB_LOGON, (char *) "1");
    }

    //verificarConsultaInformacion(datosVenta, globalDataRecibida, globalsizeData);

    ///// SE VALIDA EL CAMPO 61 PARA EL COSTO DE LA TRANSACCION ////
    verificarCosto(datosVenta->field61);

    /// BORRAR ARCHIVO REVERSO ///

    if (atoi(datosVenta->tipoTransaccion) != TRANSACCION_BCL_GIROS
        && atoi(datosVenta->tipoTransaccion) != TRANSACCION_BCL_RECLAMAR_GIROS
        && atoi(datosVenta->tipoTransaccion) != TRANSACCION_BCL_CANCELAR_GIROS) {

        isoFieldMessage = getField(12);
        memset(datosVenta->txnTime, 0x00, sizeof(datosVenta->txnTime));
        memcpy(datosVenta->txnTime, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

        isoFieldMessage = getField(13);
        memset(datosVenta->txnDate, 0x00, sizeof(datosVenta->txnDate));
        memcpy(datosVenta->txnDate, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

        isoFieldMessage = getField(37);
        memset(datosVenta->rrn, 0x00, sizeof(datosVenta->rrn));
        memcpy(datosVenta->rrn, isoFieldMessage.valueField + 6, 6);

        isoFieldMessage = getField(38);
        memset(datosVenta->codigoAprobacion, 0x00, sizeof(datosVenta->codigoAprobacion));
        memcpy(datosVenta->codigoAprobacion, isoFieldMessage.valueField,
               isoFieldMessage.totalBytes);
    }

    // CONVERTIR A ESTRUCTURA DE CNB BANCOLOMBIA
    sprintf(datosVenta->estadoTransaccion, "%d", TRANSACCION_ACTIVA);
    convertirDatosBancolombia(*datosVenta, &datosBancolombia);

    memcpy(datosBancolombia.cuenta, datosVenta->textoAdicionalInicializacion,
           sizeof(datosVenta->textoAdicionalInicializacion));
    memcpy(datosBancolombia.cuotas, datosVenta->isQPS, 2);
    memcpy(datosBancolombia.numeroRecibo, datosVenta->numeroRecibo,
           sizeof(datosVenta->numeroRecibo));

    borrarArchivo(discoNetcom, (char *) ARCHIVO_REVERSO);

    guardarTransaccionBancolombia(discoNetcom, (char *) JOURNAL_CNB_BANCOLOMBIA, datosBancolombia);

    _guardarDirectorioJournals_(TRANS_CNB_BANCOLOMBIA, datosBancolombia.numeroRecibo,
                                datosBancolombia.estadoTransaccion);

    //screenMessage("CODIGO APROBACION", datosVenta->codigoAprobacion, "NUMERO DE APROBACION", "", 0);

    if (atoi(datosBancolombia.tipoTransaccion) == TRANSACCION_BCL_SALDO) {
        imprimirTicketBancolombia(datosBancolombia, RECIBO_CLIENTE, RECIBO_NO_DUPLICADO);
    } else {
        resultadoCaja = verificarHabilitacionCaja();

        if (resultadoCaja == 1) {
            getParameter(IMPRESION_RECIBO_CAJA, impresionRecibo);
            iRet = strncmp(impresionRecibo, "01", 2);

        } else {
            iRet = 0;
        }

        if (iRet == 0) {
            imprimirTicketBancolombia(datosBancolombia, RECIBO_COMERCIO, RECIBO_NO_DUPLICADO);
            //resultado = getchAcceptCancel("RECIBO", "IMPRIMIR", "SEGUNDO", "RECIBO ?", 30 * 1000);
            if (resultado != __BACK_KEY) {
                //imprimirTicketBancolombia(datosBancolombia, RECIBO_CLIENTE, RECIBO_NO_DUPLICADO);
            }
        }
    }
    validarActualizacionTelecarga(globalresultadoUnpack);
}

/**
 * @brief Trapasa los datos de la estructura DatosVenta a la estructura DatosBancolombia,
 * Para luego guardar esta ultima en el JOURNAL de bancolombia.
 * @param datosVenta
 * @param datosBancolombia
 */
void convertirDatosBancolombia(DatosVenta datosVenta, DatosCnbBancolombia *datosBancolombia) {

    memcpy(datosBancolombia->tipoTransaccion, datosVenta.tipoTransaccion, TAM_TIPO_TRANSACCION);
    memcpy(datosBancolombia->estadoTransaccion, datosVenta.estadoTransaccion,
           TAM_ESTADO_TRANSACCION);
    memcpy(datosBancolombia->numeroRecibo, datosVenta.numeroRecibo, TAM_NUMERO_RECIBO);
    memcpy(datosBancolombia->tipoCuenta, datosVenta.tipoCuenta, TAM_TIPO_CUENTA);
    memcpy(datosBancolombia->cuenta, datosVenta.nombreComercio, SIZE_NUMERO_CUENTA);
    memcpy(datosBancolombia->systemTrace, datosVenta.systemTrace, TAM_SYSTEM_TRACE);
    memcpy(datosBancolombia->msgType, datosVenta.msgType, TAM_MTI);
    memcpy(datosBancolombia->processingCode, datosVenta.processingCode, TAM_CODIGO_PROCESO);
    memcpy(datosBancolombia->totalVenta, datosVenta.totalVenta, TAM_COMPRA_NETA);
    memcpy(datosBancolombia->txnTime, datosVenta.txnTime, TAM_TIME);
    memcpy(datosBancolombia->txnDate, datosVenta.txnDate, TAM_DATE);
    memcpy(datosBancolombia->posEntrymode, datosVenta.posEntrymode, TAM_ENTRY_MODE);
    memcpy(datosBancolombia->nii, datosVenta.nii, TAM_NII);
    memcpy(datosBancolombia->posConditionCode, datosVenta.posConditionCode, TAM_POS_CONDITION);
    memcpy(datosBancolombia->track2, datosVenta.track2, TAM_TRACK2);
    memcpy(datosBancolombia->terminalId, datosVenta.terminalId, TAM_TERMINAL_ID);
    memcpy(datosBancolombia->rrn, datosVenta.rrn, TAM_RRN);
    memcpy(datosBancolombia->ultimosCuatro, datosVenta.ultimosCuatro, TAM_CODIGO_PROCESO);
    memcpy(datosBancolombia->cardName, datosVenta.cardName, TAM_CARD_NAME);
    memcpy(datosBancolombia->codigoAprobacion, datosVenta.codigoAprobacion, TAM_COD_APROBACION);
    memcpy(datosBancolombia->AID, datosVenta.AID, TAM_AID);
    memcpy(datosBancolombia->ARQC, datosVenta.ARQC, TAM_ARQC);
    memcpy(datosBancolombia->appLabel, datosVenta.appLabel, TAM_CARD_NAME);
    memcpy(datosBancolombia->cuotas, datosVenta.cuotas, TAM_CUOTAS);
    memcpy(datosBancolombia->fechaExpiracion, datosVenta.fechaExpiracion, TAM_FECHA_EXPIRACION);
    memcpy(datosBancolombia->tarjetaHabiente, datosVenta.tarjetaHabiente, TAM_TARJETA_HABIENTE);
    memcpy(datosBancolombia->modoDatafono, datosVenta.modoDatafono, TAM_MODO_PRODUCTO);
    memcpy(datosBancolombia->textoInformacion, datosVenta.textoInformacion, TAM_TEXTO_INFORMACION);
    memcpy(datosBancolombia->field61, datosVenta.field61, TAM_FIELD_61);
    memcpy(datosBancolombia->convenio, datosVenta.codigoComercio, SIZE_COD_SERVICO);
    memcpy(datosBancolombia->field60, datosVenta.field57, TAM_FIELD_57);
    memcpy(datosBancolombia->numeroReferencia, datosVenta.tokenVivamos, SIZE_REFERENCIA_MANUAL);
    memcpy(datosBancolombia->tipoRetiro, datosVenta.numeroRecibo2, SIZE_TIPO_RETIRO);
    memcpy(datosBancolombia->tipoCuentaDestino, datosVenta.codigoGrupo, SIZE_TIPO_CUENTA);
    memcpy(datosBancolombia->cuentaDestino, datosVenta.textoAdicional, SIZE_NUMERO_CUENTA);
    memcpy(datosBancolombia->tipoProducto, datosVenta.iva, SIZE_TIPO_PRODUCTO);
    memcpy(datosBancolombia->codigoCajero, datosVenta.codigoCajero, TAM_CODIGO_CAJERO);
    memcpy(datosBancolombia->facturaCaja, datosVenta.facturaCaja, TAM_FACTURA_CAJA);
    memcpy(datosBancolombia->codigoRespuesta, datosVenta.codigoRespuesta, TAM_CODIGO_RESPUESTA);

    /// Esta en una variable global la cual se llena en la funcion "armarTokensP6", se usa para la anulacion, SE DEBE HACER DE UNA MEJOR FORMA.

    if (atoi(datosVenta.tipoTransaccion) == TRANSACCION_BCL_GIROS
        || atoi(datosVenta.tipoTransaccion) == TRANSACCION_BCL_RECLAMAR_GIROS
        || atoi(datosVenta.tipoTransaccion) == TRANSACCION_BCL_CANCELAR_GIROS) {
        memcpy(datosBancolombia->tokensQF, datosVenta.aux1, 15);
        memcpy(datosBancolombia->tokensQF + 15, datosVenta.aux1 + 15, 15);

        memcpy(datosBancolombia->tokensP6, datosVenta.iva, 12);
        memcpy(datosBancolombia->tokensP6 + 12, datosVenta.compraNeta, 12);
        memcpy(datosBancolombia->tokensP6 + 24, datosVenta.propina, 12);

    } else {
        memcpy(datosBancolombia->tokensQF, tokensQF, SIZE_TOKEN_QF);
        memcpy(datosBancolombia->tokensP6, tokensP6, SIZE_TOKEN_P6);
    }

}

void extraerTokens(char *token, uChar *buffer) {

    ISOFieldMessage isoFieldMessage;
    ResultTokenUnpack resultTokenUnpack;
    TokenMessage tokenMessage;
    int idx = 0;

    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(&resultTokenUnpack, 0x00, sizeof(resultTokenUnpack));
    memset(&tokenMessage, 0x00, sizeof(tokenMessage));

    /// se extrae el nombre del convenio que esta en el campo P63 tokens P6
    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));

    for (idx = 0; idx < globalresultadoUnpack.totalField; idx++) {
        if (globalresultadoUnpack.listField[idx] == 60) {
            isoFieldMessage = getField(60);

            resultTokenUnpack = unpackTokenMessage(isoFieldMessage.valueField,
                                                   isoFieldMessage.totalBytes);
            tokenMessage = _getTokenMessage_(token);

            if (tokenMessage.totalBytes > 0) {
                memcpy(buffer, tokenMessage.valueToken, tokenMessage.totalBytes);
            }

            break;
        }
    }
}

int verificarTokenQi(uChar *data, int lengthData) {

    ResultTokenUnpack resultTokenUnpack;
    TokenMessage tokenMessage;
    int iRet = 0;

    memset(&tokenMessage, 0x00, sizeof(tokenMessage));
    memset(&resultTokenUnpack, 0x00, sizeof(resultTokenUnpack));

    resultTokenUnpack = unpackTokenMessage(data, lengthData);
    tokenMessage = _getTokenMessage_((char *) "QI");

    if (tokenMessage.totalBytes > 0) {
        memset(datosVentaBancolombia.compraNeta, 0x00, 12);
        memcpy(datosVentaBancolombia.compraNeta, tokenMessage.valueToken + 3, 10);
        memset(datosVentaBancolombia.baseDevolucion, 0x00, 12);
        memcpy(datosVentaBancolombia.baseDevolucion, tokenMessage.valueToken + 18, 10);
        iRet = 1;
    }

    return iRet;
}

void imprimirDeclinadasCNB(DatosCnbBancolombia datosDeposito, char *mensajeError) {

    DatosTransaccionDeclinada datosTransaccionDeclinada;

    memset(&datosTransaccionDeclinada, 0x00, sizeof(datosTransaccionDeclinada));

    getTextoTransaccion(atoi(datosDeposito.tipoTransaccion),
                        datosTransaccionDeclinada.textoTransaccion);

    if (strlen(datosDeposito.cardName) > 0) {
        strcpy(datosTransaccionDeclinada.cardName, datosDeposito.cardName);
    }

    strcpy(datosTransaccionDeclinada.titulo1, "CORRESPONSAL");
    strcpy(datosTransaccionDeclinada.titulo2, "BANCOLOMBIA");
    strcpy(datosTransaccionDeclinada.tipoCuenta, datosDeposito.tipoCuenta);
    strcpy(datosTransaccionDeclinada.cuotas, datosDeposito.cuotas);
    strcpy(datosTransaccionDeclinada.tarjetaHabiente, datosDeposito.tarjetaHabiente);
    strcpy(datosTransaccionDeclinada.fechaExpiracion, datosDeposito.fechaExpiracion);
    strcpy(datosTransaccionDeclinada.mensajeError, mensajeError);

    imprimirTransaccionDeclinadaBancolombia(datosTransaccionDeclinada);
}

void imprimirDeclinadas(DatosVenta datosDeposito, char *mensajeError) {

    DatosTransaccionDeclinada datosTransaccionDeclinada;

    memset(&datosTransaccionDeclinada, 0x00, sizeof(datosTransaccionDeclinada));

    getTextoTransaccion(atoi(datosDeposito.tipoTransaccion),
                        datosTransaccionDeclinada.textoTransaccion);

    if (strlen(datosDeposito.cardName) > 0) {
        strcpy(datosTransaccionDeclinada.cardName, datosDeposito.cardName);
    }

    if (atoi(datosDeposito.tipoTransaccion) == TRANSACCION_BCL_DEPOSITO) {
        strcat(datosDeposito.totalVenta, "00");
    }

    strcpy(datosTransaccionDeclinada.titulo1, "CORRESPONSAL");
    strcpy(datosTransaccionDeclinada.titulo2, "BANCOLOMBIA");
    strcpy(datosTransaccionDeclinada.tipoCuenta, datosDeposito.tipoCuenta);
    strcpy(datosTransaccionDeclinada.cuotas, datosDeposito.cuotas);
    strcpy(datosTransaccionDeclinada.tarjetaHabiente, datosDeposito.tarjetaHabiente);
    strcpy(datosTransaccionDeclinada.fechaExpiracion, datosDeposito.fechaExpiracion);
    strcpy(datosTransaccionDeclinada.mensajeError, mensajeError);
    strcpy(datosTransaccionDeclinada.montoTransaccion, datosDeposito.totalVenta);
    strcpy(datosTransaccionDeclinada.numeroRecibo, datosDeposito.numeroRecibo);

    imprimirTransaccionDeclinadaBancolombia(datosTransaccionDeclinada);
}

TablaTresInicializacion traerIssuerCNB(uChar *pan, uChar *track2) {

    char bin[9 + 1];
    int idx = 0;

    memset(bin, 0x00, sizeof(bin));

    //sacar el pan de la tarjeta
    for (idx = 0; idx < strlen(track2); idx++) {
        if (track2[idx] == 'D' || track2[idx] == 'd') {
            break;
        }
    }

    memcpy(pan, track2, idx);
    memcpy(bin, pan, 9);

    return _traerIssuer_(atol(bin));
}

void imprimirTransaccionDeclinadaBancolombia(DatosTransaccionDeclinada datosTransaccionDeclinada) {

    uChar terminal[8 + 1];
    TablaUnoInicializacion tablaUno;
    Date date;

    uChar hora[10 + 1];
    uChar fecha[10 + 1];
    uChar parVersion[20 + 1];
    uChar parLogo[35 + 1];
    uChar tipoCuenta[12 + 1];
    uChar data[69 + 1];
    int modoDatafono = 0;
    uChar fechaExpiracion[20 + 1];
    uChar cuotas[9 + 1];
    uChar cardName[15 + 1];
    uChar titulo1[21 + 1];
    uChar titulo2[21 + 1];
    uChar espacio[20 + 1];
    uChar textoComercio[30 + 1];
    uChar totalVenta[12 + 1] = {0x00};
    uChar valorFormato[20 + 1] = {0x00};
    uChar stringValor[20 + 1] = {0x00};
    uChar stringError[50 + 1] = {0x00};
    char textoCompleto[250 + 1] = {0x00};
    uChar totalAnulacion[20 + 1] = {0x00};
    uChar textoAuxiliar[30 + 1] = {0x00};
    char textoLeyenda[] = {
            " NO PUEDE SER NOTIFICADA AL WEB SERVICE, TENER EN CUENTA ESTE COMPROBANTE PARA LOS PROCESOS DE CONCILIACION DEL COMERCIO"};


    memset(terminal, 0x00, sizeof(terminal));
    memset(&tablaUno, 0x00, sizeof(tablaUno));
    memset(hora, 0x00, sizeof(hora));
    memset(parVersion, 0x00, sizeof(parVersion));
    memset(parLogo, 0x00, sizeof(parLogo));
    memset(fecha, 0x00, sizeof(fecha));
    memset(tipoCuenta, 0x00, sizeof(tipoCuenta));
    memset(data, 0x00, sizeof(data));
    memset(fechaExpiracion, 0x00, sizeof(fechaExpiracion));
    memset(cuotas, 0x00, sizeof(cuotas));
    memset(cardName, 0x00, sizeof(cardName));
    memset(titulo1, 0x00, sizeof(titulo1));
    memset(titulo2, 0x00, sizeof(titulo2));
    memset(textoComercio, 0x00, sizeof(textoComercio));

    obtenerTerminalModoCorresponsal(terminal);

    getParameter(VERSION_SW, parVersion);
    getParameter(REVISION_SW, parVersion + strlen(parVersion));
    getParameter(TEXTO_COMERCIO, textoComercio);

    sprintf(parLogo, "%s%s", "0000", ".BMP");

    modoDatafono = verificarModoCNB();
    if ((modoDatafono == MODO_MIXTO)) {
        tablaUno = desempaquetarCNB();
    } else {
        tablaUno = _desempaquetarTablaCeroUno_();
    }

    sprintf(fechaExpiracion, "%15s", " ");
    sprintf(cuotas, "%7s", " ");

    switch (atoi(datosTransaccionDeclinada.tipoCuenta)) {
        case 10:
            sprintf(tipoCuenta, "%7s", "Ah");
            break;
        case 20:
            sprintf(tipoCuenta, "%7s", "Cc");
            break;
        case 30:
            sprintf(tipoCuenta, "%7s", "Cr");
            memset(fechaExpiracion, 0x00, sizeof(fechaExpiracion));
            memset(cuotas, 0x00, sizeof(cuotas));

            if (strlen(datosTransaccionDeclinada.cuotas) > 0 &&
                atoi(datosTransaccionDeclinada.cuotas) > 0) {
                sprintf(cuotas, "CUO: %2s", datosTransaccionDeclinada.cuotas);
            } else {
                strcpy(cuotas, "       ");
            }
            sprintf(fechaExpiracion, "EXP: %-10s", datosTransaccionDeclinada.fechaExpiracion);

            break;
        case 40:
            sprintf(tipoCuenta, "%7s", "Ah");
            memset(fechaExpiracion, 0x00, sizeof(fechaExpiracion));
            memset(cuotas, 0x00, sizeof(cuotas));
            if (strlen(datosTransaccionDeclinada.cuotas) > 0 &&
                atoi(datosTransaccionDeclinada.cuotas) > 0) {
                sprintf(cuotas, "CUO: %2s", datosTransaccionDeclinada.cuotas);
            } else {
                strcpy(cuotas, "       ");
            }

            sprintf(fechaExpiracion, "%s", "               ");
            break;
        case 41:
            sprintf(tipoCuenta, "%7s", "LT");
            break;
        case 42:
            sprintf(tipoCuenta, "%7s", "CM");
            break;
        case 43:
            sprintf(tipoCuenta, "%7s", "CU");
            memset(fechaExpiracion, 0x00, sizeof(fechaExpiracion));
            memset(cuotas, 0x00, sizeof(cuotas));
            if (strlen(datosTransaccionDeclinada.cuotas) > 0 &&
                atoi(datosTransaccionDeclinada.cuotas) > 0) {
                sprintf(cuotas, "CUO: %2s", datosTransaccionDeclinada.cuotas);
            } else {
                strcpy(cuotas, "       ");
            }
            sprintf(fechaExpiracion, "EXP: %-10s", datosTransaccionDeclinada.fechaExpiracion);
            break;
        case 44:
            sprintf(tipoCuenta, "%7s", "BE");
            break;

        case 45:
            sprintf(tipoCuenta, "%7s", "RT");
            memset(fechaExpiracion, 0x00, sizeof(fechaExpiracion));
            memset(cuotas, 0x00, sizeof(cuotas));
            if (strlen(datosTransaccionDeclinada.cuotas) > 0 &&
                atoi(datosTransaccionDeclinada.cuotas) > 0) {
                sprintf(cuotas, "CUO: %2s", datosTransaccionDeclinada.cuotas);
            } else {
                strcpy(cuotas, "       ");
            }

            sprintf(fechaExpiracion, "EXP: %-10s", datosTransaccionDeclinada.fechaExpiracion);
            break;
        case 46:
            sprintf(tipoCuenta, "%7s", "RG");
            break;
        case 47:
            sprintf(tipoCuenta, "%7s", "DS");
            break;

        case 48:
            sprintf(tipoCuenta, "%7s", "DB");
            break;

        case 49:
            sprintf(tipoCuenta, "%7s", "BC");
            memset(fechaExpiracion, 0x00, sizeof(fechaExpiracion));
            memset(cuotas, 0x00, sizeof(cuotas));
            if (strlen(datosTransaccionDeclinada.cuotas) > 0 &&
                atoi(datosTransaccionDeclinada.cuotas) > 0) {
                sprintf(cuotas, "CUO: %2s", datosTransaccionDeclinada.cuotas);
            } else {
                strcpy(cuotas, "       ");
            }
            sprintf(fechaExpiracion, "EXP: %-10s", datosTransaccionDeclinada.fechaExpiracion);
            break;

        default:
            sprintf(tipoCuenta, "%7s", "  ");
            break;
    }

    sprintf(cardName, "%-15s", datosTransaccionDeclinada.cardName);

    if (strlen(datosTransaccionDeclinada.cardName) == 0) {
        strcpy(cardName, NO_IMPRIMIR);
    }

    strcpy(titulo1, NO_IMPRIMIR);
    strcpy(titulo2, NO_IMPRIMIR);
    strcpy(espacio, NO_IMPRIMIR);

    if (strlen(datosTransaccionDeclinada.titulo1) > 0) {
        strcpy(titulo1, datosTransaccionDeclinada.titulo1);
    }

    if (strlen(datosTransaccionDeclinada.titulo2) > 0) {
        strcpy(titulo2, datosTransaccionDeclinada.titulo2);

        strcpy(espacio, "  ");
    }

    if (strcmp(datosTransaccionDeclinada.mensajeError, "ERROR CON EL WS") == 0) {
        formatString(0x00, 0, datosTransaccionDeclinada.montoTransaccion,
                     strlen(datosTransaccionDeclinada.montoTransaccion) - 2, valorFormato, 1);
        sprintf(totalVenta, "%12s", valorFormato);
        sprintf(stringValor, "%-10s", "VALOR");
        sprintf(textoCompleto, "%s%s%s%s", "LA TRANSACCION ",
                datosTransaccionDeclinada.numeroRecibo, " POR VALOR DE ", totalVenta);
        strcat(textoCompleto, textoLeyenda);
        strcpy(stringError, NO_IMPRIMIR);
    } else if (strcmp(datosTransaccionDeclinada.mensajeError, "ERROR CON EL WS_AN") == 0) {
        memset(valorFormato, 0x00, sizeof(valorFormato));
        memset(totalVenta, 0x00, sizeof(totalVenta));
        formatString(0x00, 0, datosTransaccionDeclinada.montoTransaccion,
                     strlen(datosTransaccionDeclinada.montoTransaccion) - 2, valorFormato, 1);
        sprintf(stringValor, "%-10s", "VALOR");
        sprintf(totalAnulacion, "-%s", valorFormato);
        sprintf(totalVenta, "%12s", totalAnulacion);
        sprintf(textoCompleto, "%s%s%s%s", "LA TRANSACCION ",
                datosTransaccionDeclinada.numeroRecibo, " POR VALOR DE ", totalVenta);
        strcat(textoCompleto, textoLeyenda);
        memcpy(textoAuxiliar, datosTransaccionDeclinada.textoTransaccion,
               strlen(datosTransaccionDeclinada.textoTransaccion));
        memset(datosTransaccionDeclinada.textoTransaccion, 0x00,
               sizeof(datosTransaccionDeclinada.textoTransaccion));
        strcpy(datosTransaccionDeclinada.textoTransaccion, "ANULACION ");
        strcat(datosTransaccionDeclinada.textoTransaccion, textoAuxiliar);
    } else {
        strcpy(stringError, datosTransaccionDeclinada.mensajeError);
        strcpy(totalVenta, NO_IMPRIMIR);
        strcpy(stringValor, NO_IMPRIMIR);
        strcpy(textoCompleto, NO_IMPRIMIR);
    }

    date = obtenerDate();


    /// HORA ///
    sprintf(hora, "%s:%s:%s", date.hour, date.minutes, date.secondes);
    char horaAuxiliar[10 + 1] = {0x00};
    memcpy(horaAuxiliar, hora, strlen(hora));
    sprintf(fecha, "%s%s%s", date.month, date.day,date.year);
    formatoFecha(fecha);
    LOGI("fecha  A %s", fecha);
    LOGI("hora  A %s", hora);
    LOGI("fecha  B %s", fecha);
    HasMap hash[] =
            {
                    {"_PAR_FECHA_",         fecha},
                    {"_PAR_VERSION_",       parVersion},
                    {"_PAR_HORA_",          horaAuxiliar},
                    {"_PAR_NESTABLE_",      tablaUno.defaultMerchantName},
                    {"_PAR_DESTABLE_",      tablaUno.receiptLine2},
                    {"_PAR_CUNICO_",        tablaUno.receiptLine3},
                    {"_PAR_TERMINAL_",      terminal},
                    {"_PAR_TARJETA_",       cardName},
                    {"_PAR_VENTA_",         datosTransaccionDeclinada.textoTransaccion},
                    {"_PAR_CUENTA_",        tipoCuenta},
                    {"_PAR_COMPRA_",        stringValor},
                    {"_PAR_VCOMPRA_",       totalVenta},
                    {"_PAR_TCANCELADA_",    stringError},
                    {"_PAR_MCANCELADA_",    NO_IMPRIMIR},
                    {"_PAR_LEYENDA_",       textoCompleto},
                    {"_PAR_PIE_",           "*** COMERCIO ***"},
                    {"_PAR_TARJ_HABIENTE_", datosTransaccionDeclinada.tarjetaHabiente},
                    {"_PAR_EXP_",           fechaExpiracion},
                    {"_PAR_CUOTAS_",        cuotas},
                    {"_PAR_TITULO1_",       titulo1},
                    {"_PAR_TITULO2_",       titulo2},
                    {"_PAR_ESPACIO_",       espacio},
                    {"_PAR_TEXTO_ADM_",     textoComercio},

                    {"FIN_HASH",            ""}
            };

    imprimirTicket(hash, parLogo, (char *) "/TBCLDECLI.TPL");
}

void solicitarSegundoRecibo(int tipoRecibo) {
    DatosCnbBancolombia datosBancolombia;
    memset(&datosBancolombia, 0x00, sizeof(datosBancolombia));
    if (tipoRecibo == RECIBO_ANULACION) {
        imprimirTicketBancolombia(globalDatosBancolombia, RECIBO_CLIENTE, tipoRecibo);
    } else {
        convertirDatosBancolombia(datosVentaBancolombia, &datosBancolombia);
        imprimirTicketBancolombia(datosBancolombia, RECIBO_CLIENTE, tipoRecibo);
    }
}

void preAprobacionEmv(){
    ISOFieldMessage isoFieldMessage;

    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));

    isoFieldMessage = getField(38);
    memset(datosVentaBancolombia.codigoAprobacion, 0x00, sizeof(datosVentaBancolombia.codigoAprobacion));
    memcpy(datosVentaBancolombia.codigoAprobacion, isoFieldMessage.valueField,
           isoFieldMessage.totalBytes);
    LOGI("preAprobacion EMV  %s ", datosVentaBancolombia.codigoAprobacion);
    validarActualizacionTelecarga(globalresultadoUnpack);
}