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

#define  LOG_TAG    "NETCOM_MANEJO_IMPUESTOS"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

int solicitarImpuesto(int tipoImpuesto) {

    char impuesto[10 + 1];
    int resultado = 0;

    memset(impuesto, 0x00, sizeof(impuesto));
    getParameter(tipoImpuesto, impuesto);

    resultado = (atoi(impuesto) > 0) ? 1 : 0;

    return resultado;

}

unsigned long _calcularIva_(unsigned long valor, int porcentajeIva) {

    int factorIva = 0;
    int multiplicador = 100;
    unsigned long auxValor = 0;
    DatosOperacion resultadoDivision;

    factorIva = multiplicador + porcentajeIva;

    auxValor = valor * porcentajeIva;
    resultadoDivision = dividir(auxValor, factorIva, 0, 1);

    return atol(resultadoDivision.cociente);
}

/**
 * @brief Esta funcion se encarga de realizar la captura del impuesto del IVA
 * de una forma manual.
 * @param totalVenta Total de la venta digitada por el usuario.
 * @param ivaMaxPermitido porcentaje maximo de iva permitido.
 * @return Retorna el valor del iva capturado.
 */
long capturarIva(char *totalVenta, char *valorIva, int ivaMaxPermitido) {

    uChar maximoValorIngresado[12 + 1];
    long valorImpuesto = -1;
    int resultado = 0;
    DatosOperacion datosOperacion;

    memset(maximoValorIngresado, 0x00, sizeof(maximoValorIngresado));
    memset(&datosOperacion, 0x00, sizeof(datosOperacion));

    if (solicitarImpuesto(MAXIMO_IVA) == 1) {
        if (atol(totalVenta) < 0) {
            memset(totalVenta, 0x00, sizeof(totalVenta));
            strcpy(totalVenta, "0");
        }

        datosOperacion = dividir(atol(totalVenta) * ivaMaxPermitido, (100 + ivaMaxPermitido), 0, 1);
        sprintf(maximoValorIngresado, "%s", datosOperacion.cociente);
        LOGI("valor %s maximo %s  OK  ", valorIva, maximoValorIngresado);
        resultado = capturarMonto(valorIva, maximoValorIngresado, (char *) "0", 1);

        if (resultado > 0) {
            valorImpuesto = atol(valorIva);
        }
    } else {
        valorImpuesto = 0;
    }
    LOGI("valorImpuesto %ld ", valorImpuesto);
    return valorImpuesto;

}

long capturarIvaManual(char *totalVenta, int ivaMaxPermitido, char *mensajeAdicional) {

    uChar maximoValorIngresado[12 + 1];
    uChar valor[12 + 1];
    long valorImpuesto = -1;
    int resultado = 0;
    DatosOperacion datosOperacion;

    memset(maximoValorIngresado, 0x00, sizeof(maximoValorIngresado));
    memset(&datosOperacion, 0x00, sizeof(datosOperacion));
    memset(valor, 0x00, sizeof(valor));

    if (solicitarImpuesto(MAXIMO_IVA) == 1) {

        if (atol(totalVenta) < 0) {
            memset(totalVenta, 0x00, sizeof(totalVenta));
            strcpy(totalVenta, "0");
        }

        datosOperacion = dividir(atol(totalVenta) * ivaMaxPermitido, (100 + ivaMaxPermitido), 0, 1);
        sprintf(maximoValorIngresado, "%s", datosOperacion.cociente);

        resultado = capturarMonto(valor, maximoValorIngresado, (char *) "0", 1);

        if (resultado > 0) {
            valorImpuesto = atol(valor);
        }
    } else {
        valorImpuesto = 0;
    }

    return valorImpuesto;

}

long capturarInc(char *totalVenta, char *valorInc, int incMaxPermitido) {

    uChar maximoValorIngresado[12 + 1];
    long valorImpuesto = -1;
    int resultado = 0;
    DatosOperacion datosOperacion;

    memset(maximoValorIngresado, 0x00, sizeof(maximoValorIngresado));
    memset(&datosOperacion, 0x00, sizeof(datosOperacion));

    if (solicitarImpuesto(MAXIMO_INC) == 1) {

        if (atol(totalVenta) < 0) {
            memset(totalVenta, 0x00, sizeof(totalVenta));
            strcpy(totalVenta, "0");
        }

        datosOperacion = dividir(atol(totalVenta) * incMaxPermitido, (100 + incMaxPermitido), 0, 1);
        sprintf(maximoValorIngresado, "%s", datosOperacion.cociente);

        resultado = capturarMonto(valorInc, maximoValorIngresado, (char *) "0", 2);

        if (resultado > 0) {
            valorImpuesto = atol(valorInc);
        }

    } else {
        valorImpuesto = 0;
    }

    return valorImpuesto;

}

long capturarIncManual(char *totalVenta, int incMaxPermitido, char *mensajeAdicional) {

    uChar maximoValorIngresado[12 + 1];
    uChar valor[12 + 1];
    long valorImpuesto = -1;
    int resultado = 0;
    DatosOperacion datosOperacion;

    memset(maximoValorIngresado, 0x00, sizeof(maximoValorIngresado));
    memset(&datosOperacion, 0x00, sizeof(datosOperacion));
    memset(valor, 0x00, sizeof(valor));

    if (solicitarImpuesto(MAXIMO_INC) == 1) {

        if (atol(totalVenta) < 0) {
            memset(totalVenta, 0x00, sizeof(totalVenta));
            strcpy(totalVenta, "0");
        }

        datosOperacion = dividir(atol(totalVenta) * incMaxPermitido, (100 + incMaxPermitido), 0, 1);
        sprintf(maximoValorIngresado, "%s", datosOperacion.cociente);

        resultado = capturarMonto(valor, maximoValorIngresado, (char *) "0", 2);

        if (resultado > 0) {
            valorImpuesto = atol(valor);
        }

    } else {
        valorImpuesto = 0;
    }

    return valorImpuesto;

}

int capturarIvaMultiple(char *valorTotal, int maximoIva, char *neto, char *iva, char *base) {

    int porcentaje = 0;
    int resultado = -1;
    long nuevoValorTotal = 0;
    long capturaIva = 0;
    long iva16 = 0;
    long iva10 = 0;
    long ivaOtros = 0;
    long totalIva = 0;
    long compraNeta = 0;
    long baseDevolucion = 0;
    long compraTotal = 0;
    char total[12 + 1];
    char primerIva[2 + 1];
    char segundoIva[2 + 1];
    char mensaje[50 + 1];
    char simbolo[6 + 1];

    memset(primerIva, 0x00, sizeof(primerIva));
    memset(segundoIva, 0x00, sizeof(segundoIva));
    memset(mensaje, 0x00, sizeof(mensaje));
    memset(simbolo, 0x00, sizeof(simbolo));

    getParameter(PRIMER_IVA_MULTIPLE, primerIva);
    getParameter(SEGUNDO_IVA_MULTIPLE, segundoIva);

    strcpy(simbolo, " %:");
    sprintf(mensaje, "IVA %d %s ", atoi(primerIva), simbolo);
    if (solicitarImpuesto(MAXIMO_IVA) == 1) {
        //primer iva
        memset(total, 0x00, sizeof(total));
        strcpy(total, valorTotal);
        porcentaje = atoi(primerIva);
        capturaIva = capturarIvaManual(total, porcentaje, mensaje); //1379
        iva16 = capturaIva;

        if (capturaIva >= 0) {
            //calcula valores
            compraTotal = capturaIva * (100 + porcentaje) / porcentaje;
            nuevoValorTotal = atol(total) - compraTotal; //10002
            baseDevolucion += (compraTotal - capturaIva); //8619 *

            //segundo iva
            sprintf(mensaje, "IVA %d %s ", atoi(segundoIva), simbolo);
            memset(total, 0x00, sizeof(total));
            sprintf(total, "%ld", nuevoValorTotal);
            porcentaje = atoi(segundoIva);
            capturaIva = capturarIvaManual(total, porcentaje, mensaje); //545
            iva10 = capturaIva;
        }

        if (capturaIva >= 0) {
            //calcula valores
            compraTotal = capturaIva * (100 + porcentaje) / porcentaje; //5995
            nuevoValorTotal = atol(total) - compraTotal;
            baseDevolucion += (compraTotal - capturaIva); //5450 *

            //otro % ?
            memset(total, 0x00, sizeof(total));
            sprintf(total, "%ld", nuevoValorTotal); //4003
            capturaIva = capturarIvaManual(total, maximoIva, (char *) "Otros IVAS %:"); //551
            ivaOtros = capturaIva;
        }

        if (capturaIva >= 0) {
            totalIva = iva16 + iva10 + ivaOtros; //retorna
            compraNeta = atol(valorTotal) - totalIva;
            //retorna base devolucion.
            sprintf(neto, "%ld", compraNeta);
            sprintf(iva, "%ld", totalIva);
            sprintf(base, "%ld", baseDevolucion);
            resultado = 1;
        }

    } else {
        strcpy(neto, valorTotal);
        strcpy(iva, "0");
        strcpy(base, "0");
        resultado = 1;
    }

    return resultado;

}

int capturarIncMultiple(char *valorTotal, char *inc, int maximoIva) {

    int porcentaje = 0;
    int resultado = -1;
    long nuevoValorTotal = 0;
    long capturaInc = 0;
    long inc8 = 0;
    long inc4 = 0;
    long incOtros = 0;
    long totalIva = 0;
    long compraTotal = 0;
    char total[12 + 1];
    char primerInc[2 + 1];
    char segundoInc[2 + 1];
    char mensaje[50 + 1];
    char simbolo[6 + 1];

    memset(primerInc, 0x00, sizeof(primerInc));
    memset(segundoInc, 0x00, sizeof(segundoInc));
    memset(mensaje, 0x00, sizeof(mensaje));
    memset(simbolo, 0x00, sizeof(simbolo));

    getParameter(PRIMER_INC_MULTIPLE, primerInc);
    getParameter(SEGUNDO_INC_MULTIPLE, segundoInc);

    strcpy(simbolo, " %:");
    sprintf(mensaje, "INC %d %s ", atoi(primerInc), simbolo);

    if (solicitarImpuesto(MAXIMO_INC) == 1) {
        //primer INC
        memset(total, 0x00, sizeof(total));
        strcpy(total, valorTotal);
        porcentaje = atoi(primerInc);
        capturaInc = capturarIncManual(total, porcentaje, mensaje); //1379
        inc8 = capturaInc;

        if (capturaInc >= 0) {
            //calcula valores
            compraTotal = capturaInc * (100 + porcentaje) / porcentaje;
            nuevoValorTotal = atol(total) - compraTotal; //10002

            //Segundo INC
            sprintf(mensaje, "INC %d %s ", atoi(segundoInc), simbolo);
            memset(total, 0x00, sizeof(total));
            sprintf(total, "%ld", nuevoValorTotal);
            porcentaje = atoi(segundoInc);
            capturaInc = capturarIncManual(total, porcentaje, mensaje); //545
            inc4 = capturaInc;
        }

        if (capturaInc >= 0) {
            //calcula valores
            compraTotal = capturaInc * (100 + porcentaje) / porcentaje; //5995
            nuevoValorTotal = atol(total) - compraTotal;

            //otro % ?
            if (nuevoValorTotal < 0) {
                nuevoValorTotal = 0;
            }
            memset(total, 0x00, sizeof(total));
            sprintf(total, "%ld", nuevoValorTotal); //4003
            capturaInc = capturarIncManual(total, maximoIva, (char *) "Otros INC %:"); //551
            incOtros = capturaInc;
        }

        if (capturaInc >= 0) {
            totalIva = inc8 + inc4 + incOtros; //retorna
            sprintf(inc, "%ld", totalIva);
            resultado = 1;
        }

    } else {
        strcpy(inc, "0");
        resultado = 1;
    }

    return resultado;

}

int capturarDatosBasicosVenta(DatosBasicosVenta *datosBasicosVenta) {

    int resultadoVenta = 1;
    int porcentajeIva = 0;
    int porcentajeInc = 0;
    int salidaWhile = FALSE;
    int ivaMultiple = 0;
    int incMultiple = 0;
    int resultadoImpuestos = 0;
    long montoTotal = 0;
    long valorIva = 0;
    long valorInc = 0;
    long valorNeto = 0;
    long motoMinimo = 0;
    long motoMaximo = 0;
    uChar ivaMaxPermitido[2 + 1];
    uChar propinaMaxPermitido[2 + 1];
    uChar maximoValorIngresado[12 + 1];
    uChar minimoValorIngresado[12 + 1];
    DatosVenta tramaVenta;
    uChar datoImpuesto[2 + 1];
    TablaUnoInicializacion tablaUno;
    uChar totalVentaIva[12 + 1];
    uChar totalVentaInc[12 + 1];
    uChar variableAux[16 + 1];
    uChar montoAux[16 + 1];
    uChar valorAcumulado[12 + 1];
    char otroPorcentaje[2 + 1];
    char lineaResumen[100 + 1] = {0x00};
    memset(&tramaVenta, 0x00, sizeof(tramaVenta));
    memset(datoImpuesto, 0x00, sizeof(datoImpuesto));
    memset(montoAux, 0x00, sizeof(montoAux));
    memset(ivaMaxPermitido, 0x00, sizeof(ivaMaxPermitido));
    memset(otroPorcentaje, 0x00, sizeof(otroPorcentaje));

    getParameter(IVA_AUTOMATICO, datoImpuesto);
    if (strcmp(datoImpuesto, "IP") != 0) {
        porcentajeIva = atoi(datoImpuesto);
    }

    memset(datoImpuesto, 0x00, sizeof(datoImpuesto));
    getParameter(INC_AUTOMATICO, datoImpuesto);
    if (strcmp(datoImpuesto, "IP") != 0) {
        porcentajeInc = atoi(datoImpuesto);
    }

    memset(variableAux, 0x00, sizeof(variableAux));
    getParameter(IVA_MULTIPLE, variableAux);
    ivaMultiple = atoi(variableAux);

    memset(variableAux, 0x00, sizeof(variableAux));
    getParameter(INC_MULTIPLE, variableAux);
    incMultiple = atoi(variableAux);

    tablaUno = _desempaquetarTablaCeroUno_();
    sprintf(ivaMaxPermitido, "%02x", tablaUno.ecrBaudRate);
    sprintf(propinaMaxPermitido, "%02x", tablaUno.printerBaudRate);

    memset(totalVentaIva, 0x00, sizeof(totalVentaIva));
    memset(totalVentaInc, 0x00, sizeof(totalVentaInc));

    if ((porcentajeIva == 0 || porcentajeInc == 0)) {
        memset(minimoValorIngresado, 0x00, sizeof(minimoValorIngresado));
        memset(maximoValorIngresado, 0x00, sizeof(maximoValorIngresado));
        memset(variableAux, 0x00, sizeof(variableAux));

        getParameter(MAXIMO_MONTO, maximoValorIngresado);
        getParameter(MINIMO_MONTO, minimoValorIngresado);

        strcpy(totalVentaIva, datosBasicosVenta->totalVenta);
    }

    //////////////  PEDIR IVA ///////////////
    LOGI("porcentajeIva %d ", porcentajeIva);
    if ((resultadoVenta > 0 && porcentajeIva == 0 && porcentajeInc == 0 &&
         ivaMultiple == 0)) { //manual
        LOGI("Evaluar iva manual ");
        valorIva = capturarIva(datosBasicosVenta->totalVenta, datosBasicosVenta->iva,
                               atoi(ivaMaxPermitido));
        resultadoVenta = valorIva;
        LOGI("valorIva %ld", valorIva);
        if (resultadoVenta >= 0) {
            resultadoVenta = 1;
        }
        // se debe calcular compra neta y capturar base de devolucion
        if (valorIva > 0) {
            /////////////////// BASE DE DEVOLUCION ///////////////////////////
            memset(maximoValorIngresado, 0x00, sizeof(maximoValorIngresado));
            sprintf(maximoValorIngresado, "%ld", atol(datosBasicosVenta->totalVenta) - valorIva);
            LOGI("capturar base devolucion %s ", maximoValorIngresado);
            resultadoVenta = capturarMonto(datosBasicosVenta->baseDevolucion,
                                           maximoValorIngresado, datosBasicosVenta->iva, 3);
            LOGI("resultado base   %d", resultadoVenta);

        }
        ///////////////////////////// INC /////////////////////////////////
        if (resultadoVenta > 0 && incMultiple == 0) {
            valorInc = capturarInc(datosBasicosVenta->totalVenta, datosBasicosVenta->inc,
                                   atoi(ivaMaxPermitido));
            resultadoVenta = valorInc;
            if (resultadoVenta >= 0) {
                resultadoVenta = 1;
            }
        } else if (resultadoVenta > 0 && incMultiple == 1) { //&& porcentajeInc == 0
            //cambiar iva por inc
            resultadoVenta = capturarIncMultiple(tramaVenta.totalVenta, tramaVenta.inc,
                                                 atoi(ivaMaxPermitido));
        }

        valorNeto = atol(datosBasicosVenta->totalVenta) - atol(datosBasicosVenta->iva) -
                    atol(datosBasicosVenta->inc);
        sprintf(datosBasicosVenta->compraNeta, "%ld", valorNeto);
        LOGI("Impuetos compra neta %s ", datosBasicosVenta->compraNeta);
    } else if (resultadoVenta > 0 && porcentajeIva == 0 &&
               porcentajeInc > 0) {//IMPUESTOS AUTOMATICOS
        LOGI("es inc automatico %d ", porcentajeInc);
        ///////////////////////////// INC /////////////////////////////////
        if (resultadoVenta > 0 && incMultiple == 0) {

            valorInc = _calcularIva_(atol(datosBasicosVenta->totalVenta), porcentajeInc);
            resultadoVenta = valorInc;

            memset(datosBasicosVenta->inc, 0x00, sizeof(datosBasicosVenta->inc));
            sprintf(datosBasicosVenta->inc, "%ld", valorInc);
            resultadoVenta = 1;

            memset(datosBasicosVenta->compraNeta, 0x00, sizeof(datosBasicosVenta->compraNeta));
            valorNeto = atol(datosBasicosVenta->totalVenta) - atol(datosBasicosVenta->inc);
            sprintf(datosBasicosVenta->compraNeta, "%ld", valorNeto);
            strcpy(datosBasicosVenta->baseDevolucion, "0");
        }

    } else if (resultadoVenta > 0 && porcentajeIva > 0) { //automatico
        memcpy(datosBasicosVenta->totalVentaIva, datosBasicosVenta->totalVenta,
               strlen(datosBasicosVenta->totalVenta));
        LOGI("monto para  A inc %s ", datosBasicosVenta->totalVentaInc);
        LOGI("monto para iva  %s ", datosBasicosVenta->totalVentaIva);
        if (porcentajeIva != 0 && porcentajeInc != 0) {

            LOGI("son los dos automaticos inc %d ", porcentajeInc);
            memset(maximoValorIngresado, 0x00, sizeof(maximoValorIngresado));
            memset(minimoValorIngresado, 0x00, sizeof(minimoValorIngresado));
            memset(totalVentaIva, 0x00, sizeof(totalVentaIva));
            getParameter(MAXIMO_MONTO, maximoValorIngresado);
            getParameter(MINIMO_MONTO, minimoValorIngresado);

            memset(valorAcumulado, 0x00, sizeof(valorAcumulado));
            memcpy(valorAcumulado, maximoValorIngresado, sizeof(maximoValorIngresado));

            memset(maximoValorIngresado, 0x00, sizeof(maximoValorIngresado));
            memset(totalVentaInc, 0x00, sizeof(totalVentaInc));
            getParameter(MAXIMO_MONTO, maximoValorIngresado);
            getParameter(MINIMO_MONTO, minimoValorIngresado);
            motoMinimo = 1;
            if (atol(datosBasicosVenta->totalVentaIva) >= 0) { // > 0
                motoMinimo = atol(minimoValorIngresado) - atol(datosBasicosVenta->totalVentaIva);
                if (motoMinimo < 0) {
                    motoMinimo = 1;
                }
            }

            motoMaximo = atol(maximoValorIngresado) - atol(datosBasicosVenta->totalVentaIva);
            memset(minimoValorIngresado, 0x00, sizeof(minimoValorIngresado));
            memset(maximoValorIngresado, 0x00, sizeof(maximoValorIngresado));
            if (motoMaximo < motoMinimo) {
                motoMinimo = motoMaximo;
            }
            sprintf(minimoValorIngresado, "%ld", motoMinimo);
            sprintf(maximoValorIngresado, "%ld", motoMaximo);

            if (atol(minimoValorIngresado) == 1) {
                memset(minimoValorIngresado, 0x00, sizeof(minimoValorIngresado));
                strcpy(minimoValorIngresado, "0");
            }


            if (resultadoVenta > 0 && ((atol(datosBasicosVenta->totalVentaIva) +
                                        atol(datosBasicosVenta->totalVentaInc)) == 0)) {
                screenMessageFiveLine((char *) "MENSAJE", (char *) "DATOS INVALIDOS",
                                      (char *) "POR FAVOR",
                                      (char *) "INGRESAR ", (char *) "NUEVAMENTE ",
                                      lineaResumen, (char *) "");
            }

        }


        LOGI("calcular iva  %s ", datosBasicosVenta->totalVentaIva);

        ////// REALIZAR CALCULOS COMPRA NETA, IVA Y BASE DEVOLUCION ////////
        valorIva = _calcularIva_(atol(datosBasicosVenta->totalVentaIva), porcentajeIva);
        // se debe calcular compra neta y base de devolucion
        memset(datosBasicosVenta->iva, 0x00, sizeof(datosBasicosVenta->iva));

        ///////////////////////////// INC /////////////////////////////////
        if (resultadoVenta > 0 && incMultiple == 0 && porcentajeInc > 0) {
            valorInc = _calcularIva_(atol(datosBasicosVenta->totalVentaInc), porcentajeInc);
            resultadoVenta = valorInc;
            if (resultadoVenta >= 0) {
                memset(datosBasicosVenta->inc, 0x00, sizeof(datosBasicosVenta->inc));
                sprintf(datosBasicosVenta->inc, "%ld", valorInc);
                resultadoVenta = 1;
            }
        }
        LOGI("valorIva   %ld ", valorIva);
        ///////////////////
        memset(datosBasicosVenta->compraNeta, 0x00, sizeof(datosBasicosVenta->compraNeta));
        sprintf(datosBasicosVenta->iva, "%ld", valorIva);
        LOGI("datosBasicosVenta->iva   %s ", datosBasicosVenta->iva);
        valorNeto =
                atol(datosBasicosVenta->totalVentaIva) + atol(datosBasicosVenta->totalVentaInc) -
                valorIva - valorInc;
        sprintf(datosBasicosVenta->compraNeta, "%ld", valorNeto);
        //ajustar recuperando el parametro de calcular base.
        memset(variableAux, 0x00, sizeof(variableAux));
        getParameter(CALCULAR_BASE, variableAux);
        if (atoi(variableAux) == 1) {
            valorNeto = atol(datosBasicosVenta->totalVentaIva) - valorIva;
            sprintf(datosBasicosVenta->baseDevolucion, "%ld", valorNeto);

        } else {
            strcpy(datosBasicosVenta->baseDevolucion, "0");
        }
        /////////////////////////////////////////////

        montoTotal =
                atol(datosBasicosVenta->totalVentaIva) + atol(datosBasicosVenta->totalVentaInc);
        sprintf(datosBasicosVenta->totalVenta, "%ld", montoTotal);
        LOGI("total impuestos    %s ", datosBasicosVenta->totalVenta);
        LOGI("resultadoVenta    %d ", resultadoVenta);
    } else if (resultadoVenta > 0 &&
               ivaMultiple == 1 /*&& producto != CUENTAS_IGUALES*/) { //multiple
        resultadoVenta = capturarIvaMultiple(tramaVenta.totalVenta, atoi(ivaMaxPermitido),
                                             tramaVenta.compraNeta,
                                             tramaVenta.iva, tramaVenta.baseDevolucion);
        ///////////////////////////// INC /////////////////////////////////
        if (resultadoVenta > 0 && incMultiple == 0 && porcentajeInc == 0) { //inc manual
            valorInc = capturarInc(datosBasicosVenta->totalVenta, datosBasicosVenta->inc,
                                   atoi(ivaMaxPermitido));
            resultadoVenta = valorInc;
            if (resultadoVenta >= 0) {
                memset(tramaVenta.inc, 0x00, sizeof(tramaVenta.inc));
                sprintf(tramaVenta.inc, "%ld", valorInc);
                valorNeto = atol(tramaVenta.compraNeta) - valorInc;
                memset(tramaVenta.compraNeta, 0x00, sizeof(tramaVenta.compraNeta));
                sprintf(tramaVenta.compraNeta, "%ld", valorNeto);
                resultadoVenta = 1;
            }
        } else if (resultadoVenta > 0 && incMultiple == 1 && porcentajeInc == 0) { // inc multiple
            //cambiar iva por inc
            resultadoVenta = capturarIncMultiple(tramaVenta.totalVenta, tramaVenta.inc,
                                                 atoi(ivaMaxPermitido));
            valorNeto = atol(tramaVenta.compraNeta) - atol(tramaVenta.inc);
            memset(tramaVenta.compraNeta, 0x00, sizeof(tramaVenta.compraNeta));
            sprintf(tramaVenta.compraNeta, "%ld", valorNeto);
        }
    }

    /////////////// SI PERMITE PROPINA ///////////////
    //tablaUno.options1.tipProcessing = 1;
    if (resultadoVenta > 0 && tablaUno.options1.tipProcessing == 1) {

        /////////////// SI PERMITE TASA AEROPORTUARIA ///////////////
        if (tablaUno.telephoneDialOptions.keyboardDialing == 1) {
            //resultadoVenta = _capturarTasaAeroportuaria_(&tramaVenta, propinaMaxPermitido);
            if (resultadoVenta >= 0) {
                resultadoVenta = 1;
            }
        } else {
            //resultadoVenta = _validarPropina_(&tramaVenta, propinaMaxPermitido);
            if (resultadoVenta >= 0) {
                resultadoVenta = 1;
            }
        }
    }

    /*if (resultadoVenta > 0) {
        strcpy(datosBasicosVenta->totalVenta, tramaVenta.totalVenta);
        strcpy(datosBasicosVenta->compraNeta, tramaVenta.compraNeta);
        strcpy(datosBasicosVenta->propina, tramaVenta.propina);
        strcpy(datosBasicosVenta->baseDevolucion, tramaVenta.baseDevolucion);
        strcpy(datosBasicosVenta->iva, tramaVenta.iva);
        strcpy(datosBasicosVenta->inc, tramaVenta.inc);
        ////// VALIDAR VENTA DESCUENTOS VIVAMOS //////

    }*/

    return resultadoVenta;

}

int capturarMonto(char *montoCapturado, char *montoMaximo, char *montoMinimo, int tipoImpuesto) {

    char linea[20 + 1];
    char valor[12 + 1];
    char lineaImpuesto[22 + 1] = {0x00};
    char lineaResumen[100 + 1] = {0x00};
    int iRet = 1;

    if (tipoImpuesto == 1) {
        strcpy(lineaImpuesto, "IVA");
    } else if (tipoImpuesto == 2) {
        strcpy(lineaImpuesto, "INC");
    } else if (tipoImpuesto == 3) {
        strcpy(lineaImpuesto, "BASE DE DEVOLUCION");
    } else if(tipoImpuesto == 4){
        strcpy(lineaImpuesto, "PROPINA");
    }
    LOGI("linea impuesto  %s ", lineaImpuesto);
    if (atol(montoCapturado) > atol(montoMaximo)) {
        LOGI("condicion 1   ");
        memset(linea, 0x00, sizeof(linea));
        memset(valor, 0x00, sizeof(valor));
        formatString(0x30, 0, montoMaximo, strlen(montoMaximo), valor, 1);
        sprintf(linea, "( %s )", valor);

        screenMessageFiveLine((char *) "MENSAJE ", (char *) " EL VALOR SUPERA ",
                              (char *) " MONTO PERMITIDO ", linea,
                              (char *) " DIGITE UN NUEVO VALOR ", lineaImpuesto, lineaResumen);
        LOGI("lineaResumen %s ", lineaResumen);
        enviarStringToJava(lineaResumen, (char *) "showScreenMessage");
        iRet = 0;
    }

    if (atol(montoCapturado) < atol(montoMinimo)) {
        memset(linea, 0x00, sizeof(linea));
        memset(valor, 0x00, sizeof(valor));
        formatString(0x30, 0, montoMinimo, strlen(montoMinimo), valor, 1);
        sprintf(linea, "( %s )", valor);
        screenMessageFiveLine((char *) "MENSAJE ", (char *) " EL VALOR DEL ",
                              (char *) "MONTO MINIMO ",
                              linea,
                              (char *) " DIGITE UN NUEVO VALOR", lineaImpuesto,
                              lineaResumen);
        LOGI("lineaResumen %s ", lineaResumen);
        enviarStringToJava(lineaResumen, (char *) "showScreenMessage");
        iRet = 0;
    }
    return iRet;
}

int capturarImpuestoIVaManual(char *ivaManual, char *totalVenta) {

    int resultadoVenta = 0;
    long valorIva = 0;

    uChar ivaMaxPermitido[2 + 1] = {0x00};
    TablaUnoInicializacion tablaUno;
    uChar totalVentaIva[12 + 1];

    tablaUno = _desempaquetarTablaCeroUno_();
    sprintf(ivaMaxPermitido, "%02x", tablaUno.ecrBaudRate);

    strcpy(totalVentaIva, totalVenta);

    LOGI("Evaluar iva manual ");
    valorIva = capturarIva(totalVenta, ivaManual, atoi(ivaMaxPermitido));
    resultadoVenta = valorIva;
    LOGI("valorIva %ld", valorIva);
    if (resultadoVenta >= 0) {
        resultadoVenta = 1;
    }

    return resultadoVenta;
}

int capturarBaseDevolucionManual(char *baseDevolucion, char *totalVenta, char *iva) {
    int resultadoVenta = 0;
    char maximoValorIngresado[12 + 1] = {0x00};
    /////////////////// BASE DE DEVOLUCION ///////////////////////////
    sprintf(maximoValorIngresado, "%ld", atol(totalVenta) - atol(iva));
    LOGI("capturar base devolucion %s ", maximoValorIngresado);
    resultadoVenta = capturarMonto(baseDevolucion,
                                   maximoValorIngresado, iva, 3);
    LOGI("resultado base   %d", resultadoVenta);
    return resultadoVenta;
}

int capturarImpuestoIncManual(DatosBasicosVenta *datosBasicosVenta) {
    int resultadoVenta = 0;
    long valorNeto = 0;
    long valorInc = 0;

    TablaUnoInicializacion tablaUno;
    uChar ivaMaxPermitido[2 + 1] = {0x00};

    tablaUno = _desempaquetarTablaCeroUno_();
    sprintf(ivaMaxPermitido, "%02x", tablaUno.ecrBaudRate);

    ///////////////////////////// INC /////////////////////////////////
    valorInc = capturarInc(datosBasicosVenta->totalVenta, datosBasicosVenta->inc,
                           atoi(ivaMaxPermitido));
    resultadoVenta = valorInc;
    if (resultadoVenta >= 0) {
        resultadoVenta = 1;
    }


    valorNeto = atol(datosBasicosVenta->totalVenta) - atol(datosBasicosVenta->iva) -
                atol(datosBasicosVenta->inc);
    sprintf(datosBasicosVenta->compraNeta, "%ld", valorNeto);
    return resultadoVenta;
}

int validarPropinaImpuesto(int OpcionPropina,DatosBasicosVenta *datosBasicosVenta, int porcentajePropina) {
    int resultadoVenta = 0;
    uChar propinaMaxPermitido[2 + 1] = {0x00};
    TablaUnoInicializacion tablaUno;

    memset(&tablaUno, 0x00, sizeof(tablaUno));
    tablaUno = _desempaquetarTablaCeroUno_();
    sprintf(propinaMaxPermitido, "%02x", tablaUno.printerBaudRate);

    resultadoVenta = validarPropina(OpcionPropina, datosBasicosVenta, propinaMaxPermitido, porcentajePropina);
    if (resultadoVenta >= 0) {
        resultadoVenta = 1;
    }
    return resultadoVenta;
}

int validarPropina(int OpcionPropina, DatosBasicosVenta *datosBasicosVenta, uChar * maximoPropina, int porcentajePropina) {

    int resultadoPropina = 0;
    int otraPropina = 0;
    int montoPropina = 0;
    char maximoValor[12 + 1];
    DatosOperacion datosOperacion;


    ///// Si Escoje Ingresar un Valor Diferente /////
    if (OpcionPropina == 0) {
        memset(&datosOperacion, 0x00, sizeof(datosOperacion));
        porcentajePropina = atoi(maximoPropina);
        datosOperacion = dividir(atol(datosBasicosVenta->compraNeta) * porcentajePropina, 100, 0, 1);
        montoPropina = atoi(datosOperacion.cociente);

        sprintf(maximoValor, "%d", montoPropina);
        LOGI("la propina es :   %s", datosBasicosVenta->propina);
        LOGI("el valor maximo es :   %s",maximoValor);
        otraPropina = capturarMonto(datosBasicosVenta->propina, maximoValor, "0",4);
        if (strlen(datosBasicosVenta->propina) == 0) {
            strcpy(datosBasicosVenta->propina, "0");
        }

        if (otraPropina <= 0) {
            resultadoPropina = -1;
        }
    } else if (OpcionPropina > 0) {
        LOGI("camino else  :   %s", datosBasicosVenta->propina);
        memset(&datosOperacion, 0x00, sizeof(datosOperacion));
        datosOperacion = dividir(atol(datosBasicosVenta->compraNeta) * porcentajePropina, 100, 0, 1);
        montoPropina = atoi(datosOperacion.cociente);
        porcentajePropina = montoPropina;
        LOGI("montoPropina   %d", montoPropina);
        memset(maximoValor, 0x00, sizeof(maximoValor));
        sprintf(datosBasicosVenta->propina, "%d", montoPropina);
        resultadoPropina = montoPropina;

    }

    return resultadoPropina;
}

int verificarhabilitacionTasaPropina(){
    TablaUnoInicializacion tablaUno;

    memset(&tablaUno, 0x00, sizeof(tablaUno));
    tablaUno = _desempaquetarTablaCeroUno_();

    int resultadoVenta = 0;
    if (tablaUno.options1.tipProcessing == 1) {

        /////////////// SI PERMITE TASA AEROPORTUARIA ///////////////
        if (tablaUno.telephoneDialOptions.keyboardDialing == 1) {
                resultadoVenta = 2;
        } else {
                resultadoVenta = 1;
        }
    }
    return resultadoVenta;
}