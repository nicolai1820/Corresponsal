//
// Created by NETCOM on 25/02/2020.
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
#include <jni.h>
#include "Tipo_datos.h"
#include "comunicaciones.h"
#include "android/log.h"
#include "ctime"

#define ETH_IFO_ADDR     0x0103

#define ASUMIR_COMISION       1
#define NO_ASUMIR_COMISION    2

#define CLIENTE_GIRADOR       1
#define CLIENTE_BENEFICIARIO  2

#define  LOG_TAG    "NETCOM_GIROS"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

int confirmarGiro(uChar *totalVenta, uChar *dato, int asumirComision, uChar *totalGiro);

int
confirmarDatosGiros(uChar *tipoDocumento, uChar *identificacion, uChar *celular, int tipoCliente);


int empaquetarP58TokenQP(uChar *tokenQP);

int procesarGiros(DatosCnbBancolombia *datosGiros, int tipoTransaccion);


int numeroIdentificacion(DatosArchivoPar datosArchivo, int tipoCliente);

DatosComision calculoComisionGiro(uChar *totalVenta);

int unpackRespuestaGiros(DatosCnbBancolombia *datosGiros, char *dataRecibida, int cantidadBytes,
                         int tipoTransaccion);

void TransferenciaDatosGiros(DatosCnbBancolombia *tramaVenta);

int confirmarDatosGirosTeff(uChar *documentoG, uChar *celularG, uChar *documentoB, uChar *celularB,
                            uChar *monto);

int responderEnvioGiroCaja(DatosVenta *datosVentaBancolombia, char *monto);


int enviarGiro(char *monto) {

    int resultado = 0;
    int intentosVentas = 0;
    int flujoMedio = 0;
    int resultadoCaja = 0;
    long valor = 0;
    uChar tipoDocumento[2 + 1];
    uChar tipoDocumentoAux[2 + 1];
    uChar linea3[25 + 1];
    uChar valorFormato[12 + 1];
    char docAux[2 + 1];
    DatosCnbBancolombia datosEnviarGiro;
    DatosArchivoPar datosArchivo;
    DatosComision datosComision;

    memset(&datosEnviarGiro, 0x00, sizeof(datosEnviarGiro));
    memset(&datosVentaBancolombia, 0x00, sizeof(datosVentaBancolombia));
    memset(&datosArchivo, 0x00, sizeof(datosArchivo));
    memset(&datosComision, 0x00, sizeof(DatosComision));
    memset(linea3, 0x00, sizeof(linea3));
    memset(valorFormato, 0x00, sizeof(valorFormato));
    memset(docAux, 0x00, sizeof(docAux));

    resultadoCaja = verificarHabilitacionCaja();

    if (resultadoCaja == TRUE) {
        /*if (verificarCajaIPCB() == 1) {
            resultadoCaja = 0;
            flujoMedio = 1;
        }*/
    }

    if (resultadoCaja == TRUE) {
        memcpy(datosVentaBancolombia.totalVenta, monto, sizeof(datosVentaBancolombia.totalVenta));
        resultado = 1;
    }
    memcpy(datosVentaBancolombia.totalVenta, monto, strlen(monto));
    resultado = 1;
    if (resultado > 0) {

        datosComision = calculoComisionGiro(datosVentaBancolombia.totalVenta);

        if (resultadoCaja == 1) {
            // resultado = transaccionPeticionDatosCaja(&datosVentaBancolombia, TRANSACCION_TEFF_ENVIAR_GIRO);

            valor = atoi(datosComision.valorComision) - atoi(datosComision.valorIvaComision);
            sprintf(datosVentaBancolombia.propina, "%ld", valor);
        }

        if (atol(datosComision.valorComision) > 0) {

            memcpy(valorFormato, datosComision.valorComision, 12);
            valor = atoi(valorFormato);
            memset(valorFormato, 0x00, sizeof(valorFormato));
            formatString(0x00, valor, (char *) "", 0, valorFormato, 1);
            sprintf(linea3, "COMISION: %s", valorFormato);

            // resultado = getchAcceptCancel("CONFIRMACION", "COMISION QUE DEBE", "PAGAR EL GIRADOR?", linea3, 30 * 1000);

            if (resultado > 0) {
                resultado = confirmarGiro(datosVentaBancolombia.totalVenta,
                                          datosComision.valorComision,
                                          ASUMIR_COMISION, datosVentaBancolombia.compraNeta);
            }

            valor = atoi(datosComision.valorComision) - atoi(datosComision.valorIvaComision);
            sprintf(datosVentaBancolombia.propina, "%ld", valor);
            memset(datosComision.valorComision, 0x00, sizeof(datosComision.valorComision));
            sprintf(datosComision.valorComision, "%ld", valor);

            if (resultado > 0) {

                do {
                    if (resultadoCaja != 1) {
                        //datosArchivo = menuTiposDocumento();

                        resultado = numeroIdentificacion(datosArchivo, CLIENTE_GIRADOR);
                    }

                    if (resultado > 0) {
                        if (resultadoCaja != 1) {
                            //resultado = capturarDato("NUMERO CELULAR", "NUMERO CELULAR", "GIRADOR", 10, 10,  DATO_NUMERICO, 1, datosVentaBancolombia.facturaCaja);
                            memcpy(tipoDocumento, datosArchivo.tipoDocumento, 2);
                            if (flujoMedio == 1) {
                                memcpy(datosVentaBancolombia.numeroRecibo2, tipoDocumento, 2);
                            }
                        } else {
                            memcpy(docAux, datosVentaBancolombia.numeroRecibo2, 2);

                            if (strcmp(docAux, "00") == 0) {

                                memcpy(tipoDocumento, "CC", 2);
                            } else if (strcmp(docAux, "01") == 0) {
                                memcpy(tipoDocumento, "TI", 2);
                            } else if (strcmp(docAux, "02") == 0) {
                                memcpy(tipoDocumento, "CE", 2);
                            } else if (strcmp(docAux, "03") == 0) {
                                memcpy(tipoDocumento, "NT", 2);
                            } else if (strcmp(docAux, "04") == 0) {
                                memcpy(tipoDocumento, "PA", 2);
                            }

                        }
                        if (resultado > 0) {
                            if (resultadoCaja != 1) {
                                resultado = confirmarDatosGiros(tipoDocumento,
                                                                datosVentaBancolombia.aux1,
                                                                datosVentaBancolombia.facturaCaja,
                                                                GIRADOR);
                                if (resultado == 0) {
                                    //resultado = getchAcceptCancel("CANCELAR", "DESEA MODIFICAR", "LOS DATOS DEL GIRADOR?", "", 30 * 1000);
                                    if (resultado > 0) {
                                        resultado = 0;
                                    }
                                }
                            }

                            if (resultado > 0) {

                                do {
                                    if (resultadoCaja != 1) {
                                        //datosArchivo = menuTiposDocumento();
                                        resultado = numeroIdentificacion(datosArchivo,
                                                                         CLIENTE_BENEFICIARIO);
                                    }

                                    if (resultado > 0) {
                                        if (resultadoCaja != 1) {
                                            //resultado = capturarDato("NUMERO CELULAR", "NUMERO CELULAR", "BENEFICIARIO", 10, 10, DATO_NUMERICO, 1, datosVentaBancolombia.appLabel);
                                            memcpy(tipoDocumentoAux, datosArchivo.tipoDocumento, 2);
                                            if (flujoMedio == 1) {
                                                memcpy(datosVentaBancolombia.numeroRecibo2 + 2,
                                                       tipoDocumentoAux, 2);
                                            }
                                        } else {
                                            if (strcmp(datosVentaBancolombia.numeroRecibo2 + 2,
                                                       "00") == 0) {
                                                memcpy(tipoDocumentoAux, "CC", 2);
                                            } else if (
                                                    strcmp(datosVentaBancolombia.numeroRecibo2 + 2,
                                                           "01") == 0) {
                                                memcpy(tipoDocumentoAux, "TI", 2);
                                            } else if (
                                                    strcmp(datosVentaBancolombia.numeroRecibo2 + 2,
                                                           "02") == 0) {
                                                memcpy(tipoDocumentoAux, "CE", 2);
                                            } else if (
                                                    strcmp(datosVentaBancolombia.numeroRecibo2 + 2,
                                                           "03") == 0) {
                                                memcpy(tipoDocumentoAux, "NT", 2);
                                            } else if (
                                                    strcmp(datosVentaBancolombia.numeroRecibo2 + 2,
                                                           "04") == 0) {
                                                memcpy(tipoDocumentoAux, "PA", 2);
                                            }
                                        }
                                        if (resultado > 0) {
                                            if (resultadoCaja != 1) {
                                                resultado = confirmarDatosGiros(tipoDocumentoAux,
                                                                                datosVentaBancolombia.aux1 +
                                                                                15,
                                                                                datosVentaBancolombia.appLabel,
                                                                                BENEFICIARIO);
                                                if (resultado == 0) {
                                                    //resultado = getchAcceptCancel("CANCELAR", "DESEA MODIFICAR", "LOS DATOS DEL BENEFICIARIO?", "", 30 * 1000);
                                                    if (resultado > 0) {
                                                        resultado = 0;
                                                    }
                                                }
                                            }

                                            if (resultado > 0) {
                                                if (resultadoCaja == 1) {
                                                    resultado = confirmarDatosGirosTeff(
                                                            datosVentaBancolombia.aux1,
                                                            datosVentaBancolombia.facturaCaja,
                                                            datosVentaBancolombia.aux1 + 15,
                                                            datosVentaBancolombia.appLabel,
                                                            datosVentaBancolombia.totalVenta);
                                                    if (resultado == 0) {
                                                        resultado = -1;
                                                    }
                                                }
                                                if (resultado > 0) {
                                                    // resultado = getchAcceptCancel("TERMINOS Y CONDICIONES","HE LEIDO Y ACEPTADO", "EL REGLAMENTO DE","GIROS NACIONALES", 30 * 1000);
                                                }

                                                if (resultado > 0) {
                                                    do {
                                                        armarTramaGiro(&datosEnviarGiro,
                                                                       ENVIAR_GIROS, tipoDocumento,
                                                                       tipoDocumentoAux,
                                                                       datosComision);

                                                        obtenerTerminalModoCorresponsal(
                                                                datosVentaBancolombia.terminalId);
                                                        resultado = procesarGiros(&datosEnviarGiro,
                                                                                  ENVIAR_GIROS);

                                                        if (resultado == -2) {
                                                            intentosVentas++;
                                                        } else if (resultado == 0) {
                                                            resultado = -1;
                                                        }

                                                        if (intentosVentas == MAX_INTENTOS_VENTA) {
                                                            //screenMessage("COMUNICACION", "REINTENTE", "TRANSACCION","", 2 * 1000);
                                                            resultado = -1;
                                                        }

                                                    } while (resultado == -2 &&
                                                             intentosVentas < MAX_INTENTOS_VENTA);

                                                    if (resultado > 0) {
                                                        if (resultadoCaja == 1 || flujoMedio == 1) {
                                                            if (flujoMedio == 1) {
                                                                memcpy(monto,
                                                                       datosVentaBancolombia.totalVenta,
                                                                       12);
                                                            }

                                                            responderEnvioGiroCaja(
                                                                    &datosVentaBancolombia, monto);
                                                        }

                                                        mostrarAprobacionBancolombia(
                                                                &datosVentaBancolombia);
                                                    }
                                                }
                                            } else if (resultado != -2) {
                                                resultado = 0;
                                            }
                                        }
                                    }

                                } while (resultado == 0);
                            } else if (resultado != -2) {
                                resultado = 0;
                            }
                        }
                    }
                } while (resultado == 0);
            }
        } else {
            //screenMessage("MENSAJE", "NO EXISTE", "TABLA", "DE COMISIONES", 2 * 1000);
            resultado = -1;
        }
    }
    if (resultado <= 0) {
        if (resultadoCaja == 1) {
            setParameter(USO_CAJA_REGISTRADORA, (char *) "1");
            memcpy(datosVentaBancolombia.codigoRespuesta, "01", 2);
            //transaccionRespuestaDatosCaja(datosVentaBancolombia, TRANSACCION_TEFF_ENVIAR_GIRO);
        }
    }
    return resultado;
}

int reclamarGiro(char *monto) {

    long valor = 0;
    int resultado = 0;
    int flujoMedio = 0;
    int resultadoCaja = 0;

    int intentosVentas = 0;
    uChar tipoDocumento[2 + 1];
    uChar lineaBeneficiario[50 + 1];
    uChar lineaMonto[50 + 1];
    uChar valorFormato[12 + 1];

    DatosCnbBancolombia datosReclamarGiro;
    DatosArchivoPar datosArchivo;
    DatosComision datosComision;

    memset(&datosArchivo, 0x00, sizeof(datosArchivo));
    memset(&datosReclamarGiro, 0x00, sizeof(datosReclamarGiro));
    memset(&datosVentaBancolombia, 0x00, sizeof(datosVentaBancolombia));
    memset(&datosComision, 0x00, sizeof(datosComision));
    memset(tipoDocumento, 0x30, sizeof(tipoDocumento));
    memset(lineaMonto, 0x30, sizeof(lineaMonto));
    memset(lineaBeneficiario, 0x30, sizeof(lineaBeneficiario));
    memset(valorFormato, 0x00, sizeof(valorFormato));

    resultadoCaja = verificarHabilitacionCaja();

    if (resultadoCaja == 1) {
        /* if (verificarCajaIPCB() == 1) {
             resultadoCaja = 0;
             flujoMedio = 1;
         }*/
    }

    if (resultadoCaja == 1) {
        //resultado = transaccionPeticionDatosCaja(&datosVentaBancolombia, TRANSACCION_TEFF_RECLAMAR_GIRO);
        if (resultado > 0) {
            valor = atoi(datosVentaBancolombia.totalVenta);
            formatString(0x00, valor, (char *) "", 0, valorFormato, 1);
            sprintf(lineaBeneficiario, "%s%s", "BENEFICIARIO: ", datosVentaBancolombia.aux1 + 15);
            sprintf(lineaMonto, "%s%s", "MONTO: ", valorFormato);
            //resultado = confirmarDatosTeffGiros("CONFIRMA RECLAMA GIRO", "", lineaBeneficiario,lineaMonto, "","");
            if (strcmp(datosVentaBancolombia.numeroRecibo2 + 2, "00") == 0) {
                memcpy(datosArchivo.tipoDocumento, "CC", 2);
            } else if (strcmp(datosVentaBancolombia.numeroRecibo2 + 2, "01") == 0) {
                memcpy(datosArchivo.tipoDocumento, "TI", 2);
            } else if (strcmp(datosVentaBancolombia.numeroRecibo2 + 2, "02") == 0) {
                memcpy(datosArchivo.tipoDocumento, "CE", 2);
            } else if (strcmp(datosVentaBancolombia.numeroRecibo2 + 2, "03") == 0) {
                memcpy(datosArchivo.tipoDocumento, "NT", 2);
            } else if (strcmp(datosVentaBancolombia.numeroRecibo2 + 2, "04") == 0) {
                memcpy(datosArchivo.tipoDocumento, "PA", 2);
            }
        }
    } else {
        //datosArchivo = menuTiposDocumento();
        resultado = numeroIdentificacion(datosArchivo, CLIENTE_BENEFICIARIO);
    }

    if (resultado > 0) {
        if (resultadoCaja != 1) {
            //resultado = capturarMonto("MONTO BASE", "", "INGRESE MONTO", 8, datosVentaBancolombia.totalVenta, "99999999", "1");
        }
        if (resultado > 0) {
            //resultado = getchAcceptCancel("TERMINOS Y CONDICIONES", "HE LEIDO Y ACEPTADO", "EL REGLAMENTO DE", "GIROS NACIONALES", 30 * 1000);
            if (resultado > 0) {
                //resultado = capturarDato("NUMERO REFERENCIA", "INGRESE NUMERO", "REFERENCIA GIRO", 10, 10, DATO_NUMERICO, 1, datosComision.referenciaReclamacion);
                if (resultado > 0) {
                    do {
                        armarTramaGiro(&datosReclamarGiro, RECLAMAR_GIROS, tipoDocumento,
                                       datosArchivo.tipoDocumento,
                                       datosComision);
                        resultado = procesarGiros(&datosReclamarGiro, RECLAMAR_GIROS);
                        if (resultado == -2) {
                            intentosVentas++;
                        }

                        if (intentosVentas == MAX_INTENTOS_VENTA) {
                            //screenMessage("COMUNICACION", "REINTENTE", "TRANSACCION", "", 2 * 1000);
                            resultado = -1;
                        }

                    } while (resultado == -2 && intentosVentas < MAX_INTENTOS_VENTA);

                    if (resultado > 0) {
                        if (resultadoCaja == 1 || flujoMedio == 1) {
                            setParameter(USO_CAJA_REGISTRADORA, (char *) "1");
                            memcpy(datosVentaBancolombia.codigoRespuesta, "00", 2);
                            //resultado = transaccionRespuestaDatosCaja(datosVentaBancolombia,TRANSACCION_TEFF_RECLAMAR_GIRO);
                            if (resultado < 0) {
                                memcpy(datosVentaBancolombia.codigoRespuesta, "WS", 2);
                                imprimirDeclinadas(datosVentaBancolombia, "ERROR CON EL WS");
                            }
                        }

                        mostrarAprobacionBancolombia(&datosVentaBancolombia);
                    }
                }
            }
        }
    }
    if (resultado <= 0) {
        if (resultadoCaja == 1) {
            setParameter(USO_CAJA_REGISTRADORA, (char *) "1");
            memcpy(datosVentaBancolombia.codigoRespuesta, "01", 2);
            //transaccionRespuestaDatosCaja(datosVentaBancolombia, TRANSACCION_TEFF_ENVIAR_GIRO);
        }
    }
    return resultado;
}

int cancelarGiro(char *monto) {

    int resultado = 0;
    int resultadoCaja = 0;
    int flujoMedio = 0;

    int intentosVentas = 0;
    uChar tipoDocumento[2 + 1];
    uChar tipoDocumentoAux[2 + 1];
    uChar valorFormato[12 + 1];
    uChar linea3[25 + 1];
    uChar lineaGirador[50 + 1];
    uChar lineaBeneficiario[50 + 1];
    uChar auxMonto[12 + 1];
    uChar auxIva[12 + 1];
    uChar auxCompra[12 + 1];
    char docAux[2 + 1];

    DatosArchivoPar datosArchivo;
    DatosCnbBancolombia datosReclamarGiro;
    DatosComision datosComision;

    memset(&datosArchivo, 0x00, sizeof(datosArchivo));
    memset(&datosComision, 0x00, sizeof(datosComision));
    memset(&datosReclamarGiro, 0x00, sizeof(datosReclamarGiro));
    memset(&datosVentaBancolombia, 0x00, sizeof(datosVentaBancolombia));
    memset(valorFormato, 0x00, sizeof(valorFormato));
    memset(linea3, 0x00, sizeof(linea3));
    memset(lineaGirador, 0x00, sizeof(lineaGirador));
    memset(lineaBeneficiario, 0x00, sizeof(lineaBeneficiario));
    memset(tipoDocumento, 0x00, sizeof(tipoDocumento));
    memset(tipoDocumentoAux, 0x00, sizeof(tipoDocumentoAux));
    memset(auxMonto, 0x00, sizeof(auxMonto));
    memset(auxIva, 0x00, sizeof(auxIva));
    memset(auxCompra, 0x00, sizeof(auxCompra));
    memset(docAux, 0x00, sizeof(docAux));

    resultadoCaja = verificarHabilitacionCaja();

    if (resultadoCaja == 1) {
        /*if (verificarCajaIPCB() == 1) {
            resultadoCaja = 0;
            flujoMedio = 1;
        }*/
    }

    if (resultadoCaja == 1) {
        // resultado = transaccionPeticionDatosCaja(&datosVentaBancolombia, TRANSACCION_TEFF_CANCELAR_GIRO);
        if (resultado > 0) {
            sprintf(lineaGirador, "%s%s", "GIRADOR: ", datosVentaBancolombia.aux1);
            sprintf(lineaBeneficiario, "%s%s", "BENEFICIARIO: ", datosVentaBancolombia.aux1 + 15);
            //resultado = confirmarDatosTeffGiros("CONFIRMA CANCELAR GIRO", "", lineaGirador, lineaBeneficiario, "", "");
            memcpy(docAux, datosVentaBancolombia.numeroRecibo2, 2);
            if (strcmp(docAux, "00") == 0) {
                memcpy(tipoDocumento, "CC", 2);
            } else if (strcmp(docAux, "01") == 0) {
                memcpy(tipoDocumento, "TI", 2);
            } else if (strcmp(docAux, "02") == 0) {
                memcpy(tipoDocumento, "CE", 2);
            } else if (strcmp(docAux, "03") == 0) {
                memcpy(tipoDocumento, "NT", 2);
            } else if (strcmp(docAux, "04") == 0) {
                memcpy(tipoDocumento, "PA", 2);
            }
            if (strcmp(datosVentaBancolombia.numeroRecibo2 + 2, "00") == 0) {
                memcpy(tipoDocumentoAux, "CC", 2);
            } else if (strcmp(datosVentaBancolombia.numeroRecibo2 + 2, "01") == 0) {
                memcpy(tipoDocumentoAux, "TI", 2);
            } else if (strcmp(datosVentaBancolombia.numeroRecibo2 + 2, "02") == 0) {
                memcpy(tipoDocumentoAux, "CE", 2);
            } else if (strcmp(datosVentaBancolombia.numeroRecibo2 + 2, "03") == 0) {
                memcpy(tipoDocumentoAux, "NT", 2);
            } else if (strcmp(datosVentaBancolombia.numeroRecibo2 + 2, "04") == 0) {
                memcpy(tipoDocumentoAux, "PA", 2);
            }

        }
    } else {
        //datosArchivo = menuTiposDocumento();
        memcpy(tipoDocumento, datosArchivo.tipoDocumento, 2);
        resultado = numeroIdentificacion(datosArchivo, CLIENTE_GIRADOR);
    }

    if (resultado > 0) {
        if (resultadoCaja != 1) {
            //datosArchivo = menuTiposDocumento();
            memcpy(tipoDocumentoAux, datosArchivo.tipoDocumento, 2);
            resultado = numeroIdentificacion(datosArchivo, CLIENTE_BENEFICIARIO);
        }

        if (resultado > 0) {

            //resultado = capturarDato("MENSAJE", "INGRESE NUMERO", "DE PIN", 6, 8, DATO_PASSWORD, 1, datosComision.referenciaCancelacion);

            if (resultado > 0) {
                armarConsultaCancelacion(&datosReclamarGiro, CONSULTA_CANCELAR_GIROS, tipoDocumento,
                                         tipoDocumentoAux,
                                         datosComision);
                resultado = procesarGiros(&datosReclamarGiro, CONSULTA_CANCELAR_GIROS);

                memcpy(datosVentaBancolombia.inc, datosVentaBancolombia.textoInformacion, 12);
                sprintf(datosVentaBancolombia.compraNeta, "%ld", atol(datosVentaBancolombia.inc));
                memcpy(datosComision.valorComision, datosVentaBancolombia.textoInformacion + 12,
                       12);
                sprintf(datosVentaBancolombia.propina, "%ld", atol(datosComision.valorComision));
                memcpy(datosComision.valorIvaComision, datosVentaBancolombia.textoInformacion + 24,
                       12);
                sprintf(datosVentaBancolombia.iva, "%ld", atol(datosComision.valorIvaComision));

                if (resultado > 0) {

                    memset(valorFormato, 0x00, sizeof(valorFormato));

                    if (atoi(datosVentaBancolombia.totalVenta) > 0) {
                        formatString(0x00, 0, datosVentaBancolombia.totalVenta,
                                     strlen(datosVentaBancolombia.totalVenta) - 2, valorFormato, 1);
                    } else {
                        formatString(0x00, 0, datosVentaBancolombia.totalVenta,
                                     strlen(datosVentaBancolombia.totalVenta), valorFormato, 1);
                    }

                    sprintf(linea3, "%s", valorFormato);

                    //resultado = getchAcceptCancel("MONTO", "VALOR A DEVOLVER", "AL CLIENTE", linea3, 30 * 1000);
                    if (resultado > 0) {
                        do {
                            armarTramaGiro(&datosReclamarGiro, CANCELAR_GIROS, tipoDocumento,
                                           tipoDocumentoAux,
                                           datosComision);
                            resultado = procesarGiros(&datosReclamarGiro, CANCELAR_GIROS);
                            if (resultado == -2) {
                                intentosVentas++;
                            }
                            if (intentosVentas == MAX_INTENTOS_VENTA) {
                                //screenMessage("COMUNICACION", "REINTENTE", "TRANSACCION", "", 2 * 1000);
                                resultado = -1;
                            }
                        } while (resultado == -2 && intentosVentas < MAX_INTENTOS_VENTA);
                    }
                }
                if (resultado > 0) {
                    if (resultadoCaja == 1 || flujoMedio == 1) {
                        setParameter(USO_CAJA_REGISTRADORA, (char *) "1");
                        memcpy(datosVentaBancolombia.codigoRespuesta, "00", 2);

                        memcpy(auxMonto, datosVentaBancolombia.propina,
                               strlen(datosVentaBancolombia.propina) - 2);
                        memcpy(auxIva, datosVentaBancolombia.iva,
                               strlen(datosVentaBancolombia.iva) - 2);
                        memcpy(auxCompra, datosVentaBancolombia.compraNeta,
                               strlen(datosVentaBancolombia.compraNeta) - 2);

                        memset(datosVentaBancolombia.iva, 0x00, 12);
                        memset(datosVentaBancolombia.propina, 0x00, 12);
                        memset(datosVentaBancolombia.compraNeta, 0x00, 12);

                        memcpy(datosVentaBancolombia.propina, auxMonto,
                               sizeof(datosVentaBancolombia.propina));
                        memcpy(datosVentaBancolombia.iva, auxIva,
                               sizeof(datosVentaBancolombia.iva));
                        memcpy(datosVentaBancolombia.compraNeta, auxCompra,
                               sizeof(datosVentaBancolombia.compraNeta));

                        //resultado = transaccionRespuestaDatosCaja(datosVentaBancolombia, TRANSACCION_TEFF_CANCELAR_GIRO);

                        if (resultado < 0 && flujoMedio == 1) {
                            memcpy(datosVentaBancolombia.codigoRespuesta, "WS", 2);
                            imprimirDeclinadas(datosVentaBancolombia, "ERROR CON EL WS");
                        }
                        strcat(datosVentaBancolombia.propina, "00");
                        strcat(datosVentaBancolombia.iva, "00");
                        strcat(datosVentaBancolombia.compraNeta, "00");
                    }

                    mostrarAprobacionBancolombia(&datosVentaBancolombia);
                }
            }
        }
    }

    /*if (resultado <= 0) {
        if (resultadoCaja == TRUE) {
            setParameter(USO_CAJA_REGISTRADORA, "1");
            memcpy(datosVentaBancolombia.codigoRespuesta, "01", 2);
            transaccionRespuestaDatosCaja(datosVentaBancolombia, TRANSACCION_TEFF_ENVIAR_GIRO);
        }
    }*/
    return resultado;
}

int confirmarGiro(uChar *totalVenta, uChar *dato, int asumirComision, uChar *totalGiro) {

    int resultado = 0;
    uChar valorFormato[12 + 1];
    uChar valorFormatoAux[12 + 1];
    uChar valorFormatoTotal[12 + 1];
    uChar linea1[25 + 1];
    uChar linea2[25 + 1];
    uChar linea3[25 + 1];
    long valor = 0;
    long valor1 = 0;
    long valor2 = 0;

    memset(valorFormato, 0x00, sizeof(valorFormato));
    memset(valorFormatoAux, 0x00, sizeof(valorFormatoAux));
    memset(valorFormatoTotal, 0x00, sizeof(valorFormatoTotal));
    memset(linea1, 0x00, sizeof(linea1));
    memset(linea2, 0x00, sizeof(linea2));
    memset(linea3, 0x00, sizeof(linea3));

    memcpy(valorFormato, totalVenta, 12);
    memcpy(totalGiro, totalVenta, 12);
    valor = atoi(valorFormato);
    memset(valorFormato, 0x00, sizeof(valorFormato));
    formatString(0x00, valor, (char *) "", 0, valorFormato, 1);

    memcpy(valorFormatoAux, dato, 12);
    valor1 = atoi(valorFormatoAux);
    memset(valorFormatoAux, 0x00, sizeof(valorFormatoAux));
    formatString(0x00, valor1, (char *) "", 0, valorFormatoAux, 1);

    sprintf(linea1, "GIRO: %s", valorFormato);
    sprintf(linea2, "COMIS: %s", valorFormatoAux);

    if (asumirComision == ASUMIR_COMISION) {
        valor2 = valor + valor1;
        formatString(0x00, valor2, (char *) "", 0, valorFormatoTotal, 1);
    } else {
        valor2 = valor - valor1;
        formatString(0x00, valor2, (char *) "", 0, valorFormatoTotal, 1);
    }

    sprintf(linea3, "TOTAL: %s", valorFormatoTotal);
    sprintf(totalVenta, "%ld", valor2);

    enviarStringToJava("CONFIRMACION", "guardarDatosGiro");
    enviarStringToJava(linea1, "guardarDatosGiro");
    enviarStringToJava(linea2, "guardarDatosGiro");
    enviarStringToJava(linea3, "guardarDatosGiro");

    return 1;//provicional android
}

int
confirmarDatosGiros(uChar *tipoDocumento, uChar *identificacion, uChar *celular, int tipoCliente) {

    int resultado = 1;//provicional Android
    uChar linea1[24 + 1];
    uChar linea2[24 + 1];
    uChar linea3[24 + 1];
    uChar linea4[24 + 1];

    memset(linea1, 0x00, sizeof(linea1));
    memset(linea2, 0x00, sizeof(linea2));
    memset(linea3, 0x00, sizeof(linea3));
    memset(linea4, 0x00, sizeof(linea4));

    if (tipoCliente == GIRADOR) {
        memcpy(linea1, "      DATOS GIRADOR      ", 24);
    } else {
        memcpy(linea1, "    DATOS BENEFICIARIO   ", 24);
    }

    sprintf(linea2, "TIPO DOC: %.2s", tipoDocumento);
    sprintf(linea3, "NUMERO ID:%s", identificacion);
    sprintf(linea4, "CELULAR :%s", celular);

    //resultado = confirmarDatos("CONFIRMACION", linea1, linea2, linea3, linea4);

    return resultado;
}

void armarTramaGiro(DatosCnbBancolombia *datosEnviarGiro, int tipoTransaccion, uChar *tipoDocumento,
                    uChar *tipoDocumentoAux, DatosComision datosComision) {

    int indice = 0;
    int longitudTotal = 0;
    ISOHeader isoHeader8583;
    uChar nii[TAM_NII + 1];
    uChar p59[16];
    DatosTokens datosToken;
    uChar tokenQP[134 + 1];
    uChar buffer[130 + 1];
    uChar bufferAux[165 + 1];
    uChar variableAux[15 + 1];
    uChar variableAuxIva[36 + 1];
    uChar field48[36 + 1];
    ResultTokenPack resultTokenPack;
    DatosTarjetaSembrada datosTarjetaSembrada;

    memset(&isoHeader8583, 0x00, sizeof(isoHeader8583));
    memset(nii, 0x00, sizeof(nii));
    memset(p59, 0x30, sizeof(p59));
    memset(&datosToken, 0x00, sizeof(datosToken));
    memset(&resultTokenPack, 0x00, sizeof(resultTokenPack));
    memset(&datosTarjetaSembrada, 0x30, sizeof(datosTarjetaSembrada));
    memset(tokenQP, 0x00, sizeof(tokenQP));
    memset(buffer, 0x00, sizeof(buffer));
    memset(bufferAux, 0x00, sizeof(bufferAux));
    memset(variableAuxIva, 0x30, sizeof(variableAuxIva));
    memset(field48, 0x00, sizeof(field48));

    getParameter(NII, nii);
    generarInvoice(datosVentaBancolombia.numeroRecibo);
    generarSystemTraceNumber(datosVentaBancolombia.systemTrace, (char *) "");

    obtenerTerminalModoCorresponsal(datosVentaBancolombia.terminalId);

    sprintf(variableAuxIva, "%010ld00", atol(datosVentaBancolombia.iva));
    _convertASCIIToBCD_(field48, variableAuxIva, 36);

    if (tipoTransaccion == ENVIAR_GIROS) {
        sprintf(datosVentaBancolombia.tipoTransaccion, "%02d", TRANSACCION_BCL_GIROS);
    } else if (tipoTransaccion == RECLAMAR_GIROS) {
        sprintf(datosVentaBancolombia.tipoTransaccion, "%02d", TRANSACCION_BCL_RECLAMAR_GIROS);
    } else {
        sprintf(datosVentaBancolombia.tipoTransaccion, "%02d", TRANSACCION_BCL_CANCELAR_GIROS);
    }

    memcpy(datosVentaBancolombia.msgType, "0200", TAM_MTI);
    memcpy(datosVentaBancolombia.nii, nii + 1, TAM_NII);
    memcpy(datosVentaBancolombia.posConditionCode, "00", TAM_POS_CONDITION);

    if (tipoTransaccion != CANCELAR_GIROS && tipoTransaccion != CONSULTA_CANCELAR_GIROS) {
        strcat(datosVentaBancolombia.totalVenta, "00");
        strcat(datosVentaBancolombia.compraNeta, "00");
        strcat(datosComision.valorComision, "00");
        strcat(datosComision.valorIvaComision, "00");
    }

    isoHeader8583.TPDU = 60;
    memcpy(isoHeader8583.destination, nii, TAM_NII + 1);
    memcpy(isoHeader8583.source, nii, TAM_NII + 1);
    setHeader(isoHeader8583);

    setMTI(datosVentaBancolombia.msgType);

    if (tipoTransaccion == ENVIAR_GIROS) {
        memcpy(datosVentaBancolombia.processingCode, "711000", TAM_CODIGO_PROCESO);
    } else if (tipoTransaccion == RECLAMAR_GIROS) {
        memcpy(datosVentaBancolombia.processingCode, "721000", TAM_CODIGO_PROCESO);
    } else {
        memcpy(datosVentaBancolombia.processingCode, "741000", TAM_CODIGO_PROCESO);
    }

    setField(3, datosVentaBancolombia.processingCode, TAM_CODIGO_PROCESO);
    setField(4, datosVentaBancolombia.totalVenta, TAM_COMPRA_NETA);
    setField(11, datosVentaBancolombia.systemTrace, TAM_SYSTEM_TRACE);
    setField(22, (char *) "021", 3);
    setField(24, datosVentaBancolombia.nii, TAM_NII);
    setField(25, datosVentaBancolombia.posConditionCode, TAM_POS_CONDITION);

    datosTarjetaSembrada = cargarTarjetaSembrada();
    memcpy(datosVentaBancolombia.tipoCuenta, datosTarjetaSembrada.tipoCuenta, 2);

    armarTrack(datosVentaBancolombia.track2, datosTarjetaSembrada.pan,
               atoi(datosVentaBancolombia.tipoTransaccion));
    setField(35, datosVentaBancolombia.track2, strlen(datosVentaBancolombia.track2));

    setField(41, datosVentaBancolombia.terminalId, TAM_TERMINAL_ID);

    if (tipoTransaccion == ENVIAR_GIROS) {
        setField(48, field48, 18);
    }

    //// ARMADO DE TOKENS
    longitudTotal = empaquetarP58TokenQP(tokenQP);
    setField(58, tokenQP, longitudTotal);
    memcpy(buffer, tipoDocumento, 2);
    indice += 2;

    memset(variableAux, 0x00, sizeof(variableAux));
    _rightPad_(variableAux, datosVentaBancolombia.aux1, 0x20, 15);
    memcpy(buffer + indice, variableAux, 15);
    indice += 15;

    memset(variableAux, 0x00, sizeof(variableAux));
    leftPad(variableAux, datosVentaBancolombia.facturaCaja, 0x30, 10);
    memcpy(buffer + indice, variableAux, 10);
    indice += 10;

    memcpy(buffer + indice, tipoDocumentoAux, 2);
    indice += 2;

    memset(variableAux, 0x00, sizeof(variableAux));
    _rightPad_(variableAux, datosVentaBancolombia.aux1 + 15, 0x20, 15);
    memcpy(buffer + indice, variableAux, 15);
    indice += 15;

    memset(variableAux, 0x00, sizeof(variableAux));
    leftPad(variableAux, datosVentaBancolombia.appLabel, 0x30, 10);
    memcpy(buffer + indice, variableAux, 10);
    indice += 10;

    memset(variableAux, 0x00, sizeof(variableAux));
    if (tipoTransaccion == ENVIAR_GIROS || tipoTransaccion == CANCELAR_GIROS) {
        leftPad(variableAux, datosVentaBancolombia.compraNeta, 0x30, 12);
    } else {
        leftPad(variableAux, datosVentaBancolombia.totalVenta, 0x30, 12);
    }
    memcpy(buffer + indice, variableAux, 12);
    indice += 12;

    memset(variableAux, 0x00, sizeof(variableAux));
    leftPad(variableAux, datosComision.valorComision, 0x30, 12);
    memcpy(buffer + indice, variableAux, 12);
    indice += 12;

    memset(variableAux, 0x00, sizeof(variableAux));
    leftPad(variableAux, datosComision.valorIvaComision, 0x30, 12);
    memcpy(buffer + indice, variableAux, 12);
    indice += 12;

    memset(variableAux, 0x00, sizeof(variableAux));
    leftPad(variableAux, datosVentaBancolombia.totalVenta, 0x30, 12);
    memcpy(buffer + indice, variableAux, 12);
    indice += 12;

    memset(variableAux, 0x00, sizeof(variableAux));
    leftPad(variableAux, datosComision.referenciaCancelacion, 0x20, 8);
    memcpy(buffer + indice, variableAux, 8);
    indice += 8;

    memset(variableAux, 0x00, sizeof(variableAux));
    _rightPad_(variableAux, datosComision.referenciaReclamacion, 0x30, 10);
    memcpy(buffer + indice, variableAux, 10);
    indice += 10;

    if (tipoTransaccion == RECLAMAR_GIROS) {
        _setTokenMessage_((char *) "R3", (char *) TOKEN_RECLAMAR_GIRO, SIZE_TOKEN_R3);
    } else if (tipoTransaccion == CANCELAR_GIROS) {
        _setTokenMessage_((char *) "R3", (char *) TOKEN_CANCELAR_GIRO, SIZE_TOKEN_R3);
    } else {
        _setTokenMessage_((char *) "R3", (char *) TOKEN_ENVIO_GIRO, SIZE_TOKEN_R3);
    }

    _setTokenMessage_((char *) "QI", buffer, indice);

    //// SE RECUPERA EL CAMPO 60
    resultTokenPack = _packTokenMessage_();
    setField(60, resultTokenPack.tokenPackMessage, resultTokenPack.totalBytes);

}

void armarConsultaCancelacion(DatosCnbBancolombia *datosEnviarGiro, int tipoTransaccion,
                              uChar *tipoDocumento,
                              uChar *tipoDocumentoAux,
                              DatosComision datosComision) {

    int indice = 0;
    int longitudTotal = 0;
    ISOHeader isoHeader8583;
    uChar nii[TAM_NII + 1];
    uChar p59[16];
    DatosTokens datosToken;
    uChar tokenQP[134 + 1];
    uChar buffer[130 + 1];
    uChar variableAux[15 + 1];
    uChar variableAuxIva[36 + 1];
    uChar field48[36 + 1];
    ResultTokenPack resultTokenPack;
    DatosTarjetaSembrada datosTarjetaSembrada;

    memset(&isoHeader8583, 0x00, sizeof(isoHeader8583));
    memset(nii, 0x00, sizeof(nii));
    memset(p59, 0x30, sizeof(p59));
    memset(&datosToken, 0x00, sizeof(datosToken));
    memset(&resultTokenPack, 0x00, sizeof(resultTokenPack));
    memset(&datosTarjetaSembrada, 0x30, sizeof(datosTarjetaSembrada));
    memset(tokenQP, 0x00, sizeof(tokenQP));
    memset(buffer, 0x00, sizeof(buffer));
    memset(variableAuxIva, 0x30, sizeof(variableAuxIva));
    memset(field48, 0x00, sizeof(field48));

    getParameter(NII, nii);
    generarInvoice(datosVentaBancolombia.numeroRecibo);
    generarSystemTraceNumber(datosVentaBancolombia.systemTrace, (char *) "");

    obtenerTerminalModoCorresponsal(datosVentaBancolombia.terminalId);

    sprintf(variableAuxIva + 12, "%010ld00", atol(datosVentaBancolombia.iva));
    _convertASCIIToBCD_(field48, variableAuxIva, 36);

    sprintf(datosVentaBancolombia.tipoTransaccion, "%02d", TRANSACCION_BCL_CANCELAR_GIROS);

    memcpy(datosVentaBancolombia.msgType, "0100", TAM_MTI);
    memcpy(datosVentaBancolombia.nii, nii + 1, TAM_NII);
    memcpy(datosVentaBancolombia.posConditionCode, "00", TAM_POS_CONDITION);

    strcat(datosVentaBancolombia.totalVenta, "00");
    strcat(datosComision.valorComision, "00");
    strcat(datosComision.valorIvaComision, "00");

    isoHeader8583.TPDU = 60;
    memcpy(isoHeader8583.destination, nii, TAM_NII + 1);
    memcpy(isoHeader8583.source, nii, TAM_NII + 1);
    setHeader(isoHeader8583);

    setMTI(datosVentaBancolombia.msgType);

    memcpy(datosVentaBancolombia.processingCode, "731000", TAM_CODIGO_PROCESO);

    setField(3, datosVentaBancolombia.processingCode, TAM_CODIGO_PROCESO);
    setField(4, datosVentaBancolombia.totalVenta, TAM_COMPRA_NETA);
    setField(11, datosVentaBancolombia.systemTrace, TAM_SYSTEM_TRACE);
    setField(22, (char *) "021", 3);
    setField(24, datosVentaBancolombia.nii, TAM_NII);
    setField(25, datosVentaBancolombia.posConditionCode, TAM_POS_CONDITION);

    datosTarjetaSembrada = cargarTarjetaSembrada();

    armarTrack(datosVentaBancolombia.track2, datosTarjetaSembrada.pan,
               atoi(datosVentaBancolombia.tipoTransaccion));
    setField(35, datosVentaBancolombia.track2, strlen(datosVentaBancolombia.track2));

    setField(41, datosVentaBancolombia.terminalId, TAM_TERMINAL_ID);

    //// ARMADO DE TOKENS

    longitudTotal = empaquetarP58TokenQP(tokenQP);
    setField(58, tokenQP, longitudTotal);

    memcpy(buffer, tipoDocumento, 2);
    indice += 2;

    memset(variableAux, 0x00, sizeof(variableAux));
    _rightPad_(variableAux, datosVentaBancolombia.aux1, 0x20, 15);
    memcpy(buffer + indice, variableAux, 15);
    indice += 15;

    memset(variableAux, 0x00, sizeof(variableAux));
    leftPad(variableAux, datosVentaBancolombia.facturaCaja, 0x30, 10);
    memcpy(buffer + indice, variableAux, 10);
    indice += 10;

    memcpy(buffer + indice, tipoDocumentoAux, 2);
    indice += 2;

    memset(variableAux, 0x00, sizeof(variableAux));
    _rightPad_(variableAux, datosVentaBancolombia.aux1 + 15, 0x20, 15);
    memcpy(buffer + indice, variableAux, 15);
    indice += 15;

    memset(variableAux, 0x00, sizeof(variableAux));
    leftPad(variableAux, datosVentaBancolombia.appLabel, 0x30, 10);
    memcpy(buffer + indice, variableAux, 10);
    indice += 10;

    memset(variableAux, 0x00, sizeof(variableAux));
    if (verificarHabilitacionCaja() == TRUE) {
        memset(variableAux, 0x30, 12);
    } else {
        leftPad(variableAux, datosVentaBancolombia.totalVenta, 0x30, 12);
    }
    memcpy(buffer + indice, variableAux, 12);
    indice += 12;

    memset(variableAux, 0x00, sizeof(variableAux));
    leftPad(variableAux, datosComision.valorComision, 0x30, 12);
    memcpy(buffer + indice, variableAux, 12);
    indice += 12;

    memset(variableAux, 0x00, sizeof(variableAux));
    leftPad(variableAux, datosComision.valorIvaComision, 0x30, 12);
    memcpy(buffer + indice, variableAux, 12);
    indice += 12;

    memset(variableAux, 0x00, sizeof(variableAux));
    if (verificarHabilitacionCaja() == TRUE) {
        memset(variableAux, 0x30, 12);
    } else {
        leftPad(variableAux, datosVentaBancolombia.totalVenta, 0x30, 12);
    }
    memcpy(buffer + indice, variableAux, 12);
    indice += 12;

    memset(variableAux, 0x00, sizeof(variableAux));
    leftPad(variableAux, datosComision.referenciaCancelacion, 0x20, 8);
    memcpy(buffer + indice, variableAux, 8);
    indice += 8;

    memset(variableAux, 0x00, sizeof(variableAux));
    _rightPad_(variableAux, datosComision.referenciaReclamacion, 0x30, 10);
    memcpy(buffer + indice, variableAux, 10);
    indice += 10;

    _setTokenMessage_((char *) "R3", (char *) TOKEN_CONSULTA_GIRO, SIZE_TOKEN_R3);
    _setTokenMessage_((char *) "QI", buffer, indice);

    //// SE RECUPERA EL CAMPO 60
    resultTokenPack = _packTokenMessage_();
    setField(60, resultTokenPack.tokenPackMessage, resultTokenPack.totalBytes);
}

int empaquetarP58TokenQP(uChar *tokenQP) {

    int idx = 0;
    int longitudAux = 0;
    int longitudTotal = 0;
    unsigned char *p;
    unsigned int local_addr = 0;
    //T_DGPRS_GET_INFORMATION infoGPRS;

    uChar szTemp[32];
    uChar filler[10 + 1];
    uChar serial[18 + 1];
    uChar usuario[10 + 1];
    unsigned char dataAux[20 + 1];
    uChar mac_addr[12 + 1];
    uChar tokenQZ[80 + 1];
    uChar auxiliar[25 + 1];
    uChar tipoComunicacion[2 + 1];
    uChar consecutivo[6 + 1];
    char estructuraToken[56 + 1];
    char buffer[134 + 1];
    char dataLeida[100 + 1] = {0x00};
    char parametrosFile[10][20 + 1] = {0x00};

    char *token;
    ResultTokenPack resultTokenPackQZ;

    memset(filler, 0x00, sizeof(filler));
    memset(serial, 0x00, sizeof(serial));
    memset(usuario, 0x00, sizeof(usuario));
    memset(dataAux, 0x00, sizeof(dataAux));
    memset(tokenQZ, 0x00, sizeof(tokenQZ));
    memset(mac_addr, 0x00, sizeof(mac_addr));
    memset(auxiliar, 0x00, sizeof(auxiliar));
    memset(tipoComunicacion, 0x00, sizeof(tipoComunicacion));
    memset(consecutivo, 0x00, sizeof(consecutivo));
    memset(estructuraToken, 0x20, 56);
    memset(buffer, 0x20, 134);

    getParameter(USER_CNB, usuario);
    getParameter(CONSECUTIVO_CNB, consecutivo);

    getParameter(TIPO_COMUNICACION, tipoComunicacion);

    leerArchivo(discoNetcom,"/PARAMTERMINAL.txt",dataLeida);

    token = strtok(dataLeida, ";");
    memcpy(parametrosFile[idx],token, strlen(token));

    while( token != NULL ) {
        idx++;
        token = strtok(NULL, ";");
        if(token != NULL){
            memcpy(parametrosFile[idx],token, strlen(token));
        }
    }
    LOGI("parametrosFile[0] %s ", parametrosFile[0]);
    LOGI("parametrosFile[1] %s ", parametrosFile[1]);
    LOGI("parametrosFile[2] %s ", parametrosFile[2]);
    //Hexasc(dataAux,(unsigned char *)Tmp,8);
    memcpy(serial,  parametrosFile[1], 8);
    leftPad(auxiliar, serial, 0x20, 18);
    strcpy(tokenQZ, auxiliar);

    memset(auxiliar, 0x00, sizeof(auxiliar));

    leftPad(mac_addr,parametrosFile[2],0x30,12);

    strcat(tokenQZ, mac_addr);

    strcpy(szTemp,  parametrosFile[0]);
    strcat(tokenQZ, szTemp);

    memset(auxiliar, 0x00, sizeof(auxiliar));
    leftPad(auxiliar, usuario, 0x20, 10);
    strcat(tokenQZ, auxiliar);
    strcat(tokenQZ, consecutivo);

    memset(auxiliar, 0x00, sizeof(auxiliar));
    leftPad(auxiliar, filler, 0x20, 10);
    strcat(tokenQZ, auxiliar);

    _setTokenMessage_((char *) "QZ", tokenQZ, 68);

    memcpy(estructuraToken, "ES", 2);
    memcpy(estructuraToken + 2, "0007", 4);
    memcpy(estructuraToken + 6, datosVentaBancolombia.processingCode, 6);

    _setTokenMessage_((char *) "QP", estructuraToken, 56);

    resultTokenPackQZ = _packTokenMessage_();

    longitudAux = resultTokenPackQZ.totalBytes;

    memcpy(buffer, resultTokenPackQZ.tokenPackMessage, resultTokenPackQZ.totalBytes);
    memcpy(tokenQP, buffer, longitudAux);

    longitudTotal = longitudAux;

    return longitudTotal;
}

int procesarGiros(DatosCnbBancolombia *datosGiros, int tipoTransaccion) {

    ResultISOPack resultadoIsoPackMessage;
    int resultadoTransaccion = -1;
//	int globalsizeData = 0;
    char globalDataRecibida[512];

    memset(&resultadoIsoPackMessage, 0x00, sizeof(resultadoIsoPackMessage));
    memset(globalDataRecibida, 0x00, sizeof(globalDataRecibida));

    resultadoIsoPackMessage = packISOMessage();

    if (resultadoIsoPackMessage.responseCode > 0) {

        if (tipoTransaccion != CONSULTA_CANCELAR_GIROS) {

            resultadoTransaccion = realizarTransaccion(resultadoIsoPackMessage.isoPackMessage,
                                                       resultadoIsoPackMessage.totalBytes,
                                                       globalDataRecibida, TRANSACCION_BCL_GIROS,
                                                       CANAL_DEMANDA,
                                                       REVERSO_GENERADO);

        } else {
            resultadoTransaccion = realizarTransaccion(resultadoIsoPackMessage.isoPackMessage,
                                                       resultadoIsoPackMessage.totalBytes,
                                                       globalDataRecibida, TRANSACCION_BCL_GIROS,
                                                       CANAL_DEMANDA,
                                                       REVERSO_NO_GENERADO);
        }

        if (resultadoTransaccion > 0) {
//			globalsizeData = resultadoTransaccion;
            // desarmar la trama
            resultadoTransaccion = unpackRespuestaGiros(datosGiros, globalDataRecibida,
                                                        resultadoTransaccion, tipoTransaccion);
        } else {
            if (resultadoTransaccion == -3) {
                imprimirDeclinadas(datosVentaBancolombia, "TIME OUT GENERAL");
            }
        }
    }

    return resultadoTransaccion;
}

DatosArchivoPar menuTiposDocumento(int items, int indiceExt) {

    int indice = 0;
    int posicion = indiceExt * LEN_TIPOS_DOCUMENTOS;
    int opcion = indiceExt * LEN_TIPOS_DOCUMENTOS;
    long lengthTiposDocumento = 0;
    char buffer[LEN_TIPOS_DOCUMENTOS + 1];
    DatosArchivoPar datosArchivo;

    memset(buffer, 0x00, sizeof(buffer));
    memset(&datosArchivo, 0x00, sizeof(datosArchivo));

    //DatosArchivoPar datosArchivoAux[itemsTipoDocumento + 1];

    //memset(&menuTiposDocumento, 0x00, sizeof(menuTiposDocumento));
    //memset(&datosArchivoAux, 0x00, sizeof(datosArchivoAux));

    if (items == 1) {
        memset(buffer, 0x00, sizeof(buffer));

        buscarArchivo(discoNetcom, (char *) "/TDOC.TXT", buffer, posicion, LEN_TIPOS_DOCUMENTOS);

        sprintf(datosArchivo.tipoDocumento, "%.2s", buffer);
        sprintf(datosArchivo.descripcion, "%.20s", buffer + 2);
        sprintf(datosArchivo.longitudMinima, "%.2s", buffer + 22);
        sprintf(datosArchivo.longitudMaxima, "%.2s", buffer + 24);
        sprintf(datosArchivo.tipoIngreso, "%.2s", buffer + 26);
    } else {
        memset(buffer, 0x00, sizeof(buffer));

        buscarArchivo(discoNetcom, (char *) "/TDOC.TXT", buffer, posicion,
                      LEN_TIPOS_DOCUMENTOS);

        sprintf(datosArchivo.tipoDocumento, "%.2s", buffer);
        sprintf(datosArchivo.descripcion, "%.20s", buffer + 2);
        sprintf(datosArchivo.longitudMinima, "%.2s", buffer + 22);
        sprintf(datosArchivo.longitudMaxima, "%.2s", buffer + 24);
        sprintf(datosArchivo.tipoIngreso, "%.2s", buffer + 26);

        /*posicion += LEN_TIPOS_DOCUMENTOS;
        sprintf(menuTiposDocumento[indice].valor, "%.10s",
                datosArchivoAux[indice].tipoDocumento);
        sprintf(menuTiposDocumento[indice].textoPantalla, "%.20s",
                datosArchivoAux[indice].descripcion);*/


        //opcion = showMenu("TIPO DOCUMENTO", 0, 0, itemsTipoDocumento, menuTiposDocumento, 30 * 1000);
        /*opcion = 1;//temporal Android
        if (opcion > 0) {
            sprintf(datosArchivo.tipoDocumento, "%.2s", datosArchivoAux[opcion - 1].tipoDocumento);
            sprintf(datosArchivo.descripcion, "%.20s", datosArchivoAux[opcion - 1].descripcion);
            sprintf(datosArchivo.longitudMinima, "%.2s",
                    datosArchivoAux[opcion - 1].longitudMinima);
            sprintf(datosArchivo.longitudMaxima, "%.2s",
                    datosArchivoAux[opcion - 1].longitudMaxima);
            sprintf(datosArchivo.tipoIngreso, "%.2s", datosArchivoAux[opcion - 1].tipoIngreso);
        } else {
            indice = itemsTipoDocumento;
            memcpy(datosArchivo.tipoIngreso, "XX", 2);
        }*/
    }

    return datosArchivo;
}

int numeroIdentificacion(DatosArchivoPar datosArchivo, int tipoCliente) {

    int iRet = 1;//provicionalandroid
    int longitudMinima = 0;
    int longitudMaxima = 0;
    char tipoIngreso[2 + 1];
    uChar linea2[12 + 1];

    memset(tipoIngreso, 0x00, sizeof(tipoIngreso));
    memset(linea2, 0x00, sizeof(linea2));

    longitudMinima = atoi(datosArchivo.longitudMinima);
    longitudMaxima = atoi(datosArchivo.longitudMaxima);

    switch (tipoCliente) {
        case CLIENTE_GIRADOR:
            memcpy(linea2, "GIRADOR", 7);
            break;
        case CLIENTE_BENEFICIARIO:
            memcpy(linea2, "BENEFICIARIO", 12);
            break;
        default:
            break;
    }

    memcpy(tipoIngreso, datosArchivo.tipoIngreso, 2);

    if (tipoCliente == CLIENTE_GIRADOR) {
        if (strncmp(tipoIngreso, "NU", 2) == 0) {
            //iRet = capturarDato("IDENTIFICACION", "NUMERO IDENTIFICACION", linea2, longitudMinima, longitudMaxima,DATO_NUMERICO, 1, datosVentaBancolombia.aux1);
            iRet = 1;
            memcpy(datosVentaBancolombia.aux1, "79233034", 8);
        } else if (strncmp(tipoIngreso, "AN", 2) == 0) {
            //iRet = getStringKeyboard("IDENTIFICACION", "NUMERO IDENTIFICACION", linea2, 15, datosVentaBancolombia.aux1, DATO_TERMINAL);
            memcpy(datosVentaBancolombia.aux1, "79233034", 8);
        } else if (strncmp(tipoIngreso, "AL", 2) == 0) {
            //iRet = capturarDato("IDENTIFICACION", "NUMERO IDENTIFICACION", linea2, longitudMinima, longitudMaxima, DATO_NUMERICO, 1, datosVentaBancolombia.aux1);
            memcpy(datosVentaBancolombia.aux1, "79233034", 8);
        } else if (strncmp(tipoIngreso, "XX", 2) == 0) {
            //iRet = -1;
            iRet = 1;//provicional android
        }
    } else {
        if (strncmp(tipoIngreso, "NU", 2) == 0) {
            //iRet = capturarDato("IDENTIFICACION", "NUMERO IDENTIFICACION", linea2, longitudMinima, longitudMaxima, DATO_NUMERICO, 1, datosVentaBancolombia.aux1 + 15);
            memcpy(datosVentaBancolombia.aux1, "79233034", 8);
        } else if (strncmp(tipoIngreso, "AN", 2) == 0) {
            //iRet = getStringKeyboard("IDENTIFICACION", "NUMERO IDENTIFICACION", linea2, 15, datosVentaBancolombia.aux1 + 15, DATO_TERMINAL);
            memcpy(datosVentaBancolombia.aux1, "79233034", 8);
        } else if (strncmp(tipoIngreso, "AL", 2) == 0) {
            // iRet = capturarDato("IDENTIFICACION", "NUMERO IDENTIFICACION", linea2, longitudMinima, longitudMaxima,DATO_NUMERICO, 1, datosVentaBancolombia.aux1 + 15);
            memcpy(datosVentaBancolombia.aux1, "79233034", 8);
        } else if (strncmp(tipoIngreso, "XX", 2) == 0) {
            //iRet = -1;
            iRet = 1;//provicional android

        }
    }

    return iRet;
}

DatosComision calculoComisionGiro(uChar *totalVenta) {

    int lineasArchivo = 0;
    int indice = 0;
    int posicion = 0;
    int comision1 = 0;
    int comision2 = 0;
    int posicionAux = 0;
    long lengthArchivoComision = 0;
    long valor = 0;
    long valorAux = 0;
    char buffer[LEN_ARCHIVO_COMISION + 1];
    char fecha[6 + 1];
    uChar valorFormato[4 + 1];
    DatosComision datosComisionAux;

    //llamar metodo Java que devuelva la fecha//
    memset(&datosComisionAux, 0x00, sizeof(datosComisionAux));

    memset(valorFormato, 0x00, sizeof(valorFormato));
    memset(buffer, 0x00, sizeof(buffer));
    memset(fecha, 0x00, sizeof(fecha));

    lengthArchivoComision = tamArchivo(discoNetcom, (char *) "/COMSGIRO.TXT");
    lineasArchivo = lengthArchivoComision / LEN_ARCHIVO_COMISION;
    obtenerFecha(fecha);

    posicionAux = (lineasArchivo - 1) * LEN_ARCHIVO_COMISION;
    buscarArchivo(discoNetcom, (char *) "/COMSGIRO.TXT", buffer, posicionAux, LEN_ARCHIVO_COMISION);

    sprintf(datosComisionAux.fechaInicio, "%.6s", buffer + 2);

    if (atoi(fecha) <= atoi(datosComisionAux.fechaInicio)) {
        comision1 = 1;
    } else {
        comision2 = 1;
    }

    if (comision1 == 1) {
        for (indice = 0; indice < lineasArchivo; indice++) {

            memset(buffer, 0x00, sizeof(buffer));
            memset(&datosComisionAux, 0x00, sizeof(DatosComision));

            buscarArchivo(discoNetcom, (char *) "/COMSGIRO.TXT", buffer, posicion,
                          LEN_ARCHIVO_COMISION);

            sprintf(datosComisionAux.montoMinimo, "%.10s", buffer);
            sprintf(datosComisionAux.montoMaximo, "%.10s", buffer + 12);
            sprintf(datosComisionAux.comisionFija, "%.10s", buffer + 24);
            sprintf(datosComisionAux.porcentajeComision, "%.4s", buffer + 36);
            sprintf(datosComisionAux.porcentajeIva, "%.4s", buffer + 40);

            posicion += LEN_ARCHIVO_COMISION;

            if (atol(totalVenta) >= atol(datosComisionAux.montoMinimo)
                && atol(totalVenta) <= atol(datosComisionAux.montoMaximo)) {
                indice = lineasArchivo;
            }

            valor = atol(totalVenta) * atoi(datosComisionAux.porcentajeComision);
            valorAux = valor / 10000;
            valor = atol(datosComisionAux.comisionFija) + valorAux;
            sprintf(datosComisionAux.valorComision, "%ld", valor);
            valorAux = (valor * 10000) / (atoi(datosComisionAux.porcentajeIva) + 10000);
            valor = valor - valorAux;
            sprintf(datosVentaBancolombia.iva, "%ld", valor);
            sprintf(datosComisionAux.valorIvaComision, "%ld", valor);
        }
    } else if (comision2 == 1) {

        for (indice = 0; indice < lineasArchivo; indice++) {

            memset(buffer, 0x00, sizeof(buffer));

            buscarArchivo(discoNetcom, (char *) "/COMSGIRO.TXT", buffer, posicion,
                          LEN_ARCHIVO_COMISION);

            sprintf(datosComisionAux.montoMinimo, "%.10s", buffer + 44);
            sprintf(datosComisionAux.montoMaximo, "%.10s", buffer + 56);
            sprintf(datosComisionAux.comisionFija, "%.10s", buffer + 68);
            sprintf(datosComisionAux.porcentajeComision, "%.4s", buffer + 80);
            sprintf(datosComisionAux.porcentajeIva, "%.4s", buffer + 84);

            posicion += LEN_ARCHIVO_COMISION;

            if (atol(totalVenta) >= atol(datosComisionAux.montoMinimo)
                && atol(totalVenta) <= atol(datosComisionAux.montoMaximo)) {
                indice = lineasArchivo;
            }

            valor = atol(totalVenta) * atoi(datosComisionAux.porcentajeComision);
            valorAux = valor / 10000;
            valor = atol(datosComisionAux.comisionFija) + valorAux;
            sprintf(datosComisionAux.valorComision, "%ld", valor);
            valorAux = (valor * 10000) / (atoi(datosComisionAux.porcentajeIva) + 10000);
            valor = valor - valorAux;
            sprintf(datosVentaBancolombia.iva, "%ld", valor);
            sprintf(datosComisionAux.valorIvaComision, "%ld", valor);
        }
    }

    return datosComisionAux;
}

/**
 * @brief Procesa la respuesta de cada transaccion
 * @param dataRecibida
 * @param tipoTransaccion
 * @param cantidadBytes
 * @param datoVenta
 * @return 1: OK, 0: Fallo, -4: Volver a pedir pin, -2: Volver a enviar la trama.
 * -5: Declinada, -3: Cancela la solicitud de PIN
 */
int unpackRespuestaGiros(DatosCnbBancolombia *datosGiros, char *dataRecibida, int cantidadBytes,
                         int tipoTransaccion) {

    ISOFieldMessage isoFieldMessage;
    uChar codigoRespuesta[2 + 1];
    uChar saldoDisponible[15 + 1];
    int resultado = 2;
    uChar mensajeError[48 + 1];

    memset(&isoFieldMessage, 0x00, sizeof(isoFieldMessage));
    memset(&codigoRespuesta, 0x00, sizeof(codigoRespuesta));
    memset(saldoDisponible, 0x00, sizeof(saldoDisponible));
    memset(mensajeError, 0x00, sizeof(mensajeError));

    /// VALIDACIONES TRANSACCION ///
    globalresultadoUnpack = unpackISOMessage(dataRecibida, cantidadBytes);

    isoFieldMessage = getField(12);
    memset(datosVentaBancolombia.txnTime, 0x00, sizeof(datosVentaBancolombia.txnTime));
    memcpy(datosVentaBancolombia.txnTime, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

    isoFieldMessage = getField(13);
    memset(datosVentaBancolombia.txnDate, 0x00, sizeof(datosVentaBancolombia.txnDate));
    memcpy(datosVentaBancolombia.txnDate, isoFieldMessage.valueField, isoFieldMessage.totalBytes);

    isoFieldMessage = getField(39);
    memcpy(codigoRespuesta, isoFieldMessage.valueField, isoFieldMessage.totalBytes);
    //se coloca codigo de respuesta para cajas
    memcpy(datosVentaBancolombia.codigoRespuesta, isoFieldMessage.valueField,
           isoFieldMessage.totalBytes);

    if (strcmp(codigoRespuesta, "00") == 0) {

        TransferenciaDatosGiros(datosGiros);

        if (tipoTransaccion != CONSULTA_CANCELAR_GIROS) {
            resultado = verificacionTramabancolombia(datosGiros);
            if (resultado > 0) {
                memcpy(datosVentaBancolombia.codigoAprobacion, datosGiros->codigoAprobacion,
                      6);
                memcpy(datosVentaBancolombia.rrn, datosGiros->rrn, strlen(datosGiros->rrn));
            }

        } else {
            resultado = 1;
            isoFieldMessage = getField(60);
            memset(datosVentaBancolombia.textoInformacion, 0x00,
                   sizeof(datosVentaBancolombia.textoInformacion));
            memcpy(datosVentaBancolombia.textoInformacion, isoFieldMessage.valueField + 66, 54);
        }

        if (resultado > 0) {
            resultado = 1;

            isoFieldMessage = getField(4);

            memset(datosVentaBancolombia.totalVenta, 0x00,
                   sizeof(datosVentaBancolombia.totalVenta));
            leftTrim(datosVentaBancolombia.totalVenta, isoFieldMessage.valueField, '0');
        } else if (resultado < 1) {
            resultado = generarReverso();

            if (resultado == -3) {
                imprimirDeclinadasCNB(*datosGiros, "TIME OUT GENERAL");
            } else if (resultado == -1) {
                imprimirDeclinadasCNB(*datosGiros, "REVERSO PENDIENTE");
            }

            if (resultado > 0) {
                resultado = -2;
            }
        }
    } else {

        if (tipoTransaccion == ENVIAR_GIROS || tipoTransaccion == RECLAMAR_GIROS ||
            tipoTransaccion == CANCELAR_GIROS
            || tipoTransaccion == CONSULTA_CANCELAR_GIROS) {
            isoFieldMessage = getField(60);
            borrarArchivo(discoNetcom, (char *) "/RGIROS.TXT");
            escribirArchivo(discoNetcom, (char *) "/RGIROS.TXT", isoFieldMessage.valueField,
                            isoFieldMessage.totalBytes);
            memcpy(codigoRespuesta, "00", 2);
        }

        borrarArchivo(discoNetcom, (char *) ARCHIVO_REVERSO);
        errorRespuestaTransaccion(codigoRespuesta, mensajeError);
        resultado = 0;

        if (strcmp(codigoRespuesta, "55") == 0) {
            resultado = -4;
        } else if (strcmp(codigoRespuesta, "75") == 0) {
            resultado = -3;
        }

        //// SI LA RESPUESTA ES 55 O 75 Y ES DEFERENTE DE CHIP, ENTONCES NO DEBE IMPRIMIR
        if ((resultado == -4) && (strlen(datosVentaBancolombia.AID) == 0)) {
            return resultado;
        }

        //getMensajeError(codigoRespuesta, mensajeError);
        imprimirDeclinadas(datosVentaBancolombia, mensajeError);
    }

    /*isoFieldMessage = getField(58);
    if (isoFieldMessage.totalBytes > 0) {
        verificarTokenQZ(isoFieldMessage.valueField, isoFieldMessage.totalBytes);
    }*/

    return resultado;
}

void TransferenciaDatosGiros(DatosCnbBancolombia *tramaVenta) {

    memcpy(tramaVenta->processingCode, datosVentaBancolombia.processingCode, TAM_CODIGO_PROCESO);
    memcpy(tramaVenta->totalVenta, datosVentaBancolombia.totalVenta, TAM_COMPRA_NETA);
    memcpy(tramaVenta->systemTrace, datosVentaBancolombia.systemTrace, TAM_SYSTEM_TRACE);
    memcpy(tramaVenta->terminalId, datosVentaBancolombia.terminalId, TAM_TERMINAL_ID);

}

int confirmarDatosGirosTeff(uChar *documentoG, uChar *celularG, uChar *documentoB, uChar *celularB,
                            uChar *monto) {

    int resultado = 0;
    long valor = 0;

    uChar linea1[24 + 1];
    uChar linea2[24 + 1];
    uChar linea3[24 + 1];
    uChar linea4[24 + 1];
    uChar linea5[24 + 1];
    uChar valorFormato[12 + 1];

    memset(linea1, 0x00, sizeof(linea1));
    memset(linea2, 0x00, sizeof(linea2));
    memset(linea3, 0x00, sizeof(linea3));
    memset(linea4, 0x00, sizeof(linea4));
    memset(linea5, 0x00, sizeof(linea5));
    memset(valorFormato, 0x00, sizeof(valorFormato));

    valor = atoi(monto);
    formatString(0x00, valor, (char *) "", 0, valorFormato, 1);
    sprintf(linea1, (char *) "GIRADOR: %.10s", documentoG);
    sprintf(linea2, (char *) "CEL.GIRAD: %s", celularG);
    sprintf(linea3, (char *) "BENEFIC:%s", documentoB);
    sprintf(linea4, (char *) "CEL.BENEF :%s", celularB);
    sprintf(linea5, (char *) "MONTO :%s", valorFormato);

    //resultado = confirmarDatosTeffGiros("CONFIRMAR ENVIO GIRO", linea1, linea2, linea3, linea4,linea5);

    return resultado;
}

int responderEnvioGiroCaja(DatosVenta *datosVentaBancolombia, char *monto) {

    int iRet = 0;
    char auxMonto[12 + 1];
    char auxNeta[12 + 1];

    memset(auxMonto, 0x00, sizeof(auxMonto));
    memset(auxNeta, 0x00, sizeof(auxNeta));

    setParameter(USO_CAJA_REGISTRADORA, (char *) "1");
    memcpy(datosVentaBancolombia->codigoRespuesta, (char *) "00", 2);
    memcpy(auxMonto, datosVentaBancolombia->totalVenta, sizeof(datosVentaBancolombia->totalVenta));
    memcpy(auxNeta, datosVentaBancolombia->compraNeta, sizeof(datosVentaBancolombia->compraNeta));

    memset(datosVentaBancolombia->compraNeta, 0x00, sizeof(datosVentaBancolombia->compraNeta));
    memcpy(datosVentaBancolombia->compraNeta, datosVentaBancolombia->totalVenta,
           strlen(datosVentaBancolombia->totalVenta) - 2);

    memset(datosVentaBancolombia->totalVenta, 0x00, sizeof(datosVentaBancolombia->totalVenta));
    memcpy(datosVentaBancolombia->totalVenta, monto, sizeof(datosVentaBancolombia->totalVenta));
    strcat(datosVentaBancolombia->totalVenta, "00");

    // iRet = transaccionRespuestaDatosCaja(*datosVentaBancolombia, TRANSACCION_TEFF_ENVIAR_GIRO);

    /* if (iRet < 0 && verificarCajaIPCB() == 1) {
         memcpy(datosVentaBancolombia->codigoRespuesta, "WS", 2);
         memset(datosVentaBancolombia->totalVenta, 0x00, sizeof(datosVentaBancolombia->totalVenta));
         memcpy(datosVentaBancolombia->totalVenta, auxMonto, sizeof(datosVentaBancolombia->totalVenta));
         imprimirDeclinadas(*datosVentaBancolombia, "ERROR CON EL WS");
     }*/

    memset(datosVentaBancolombia->totalVenta, 0x00, sizeof(datosVentaBancolombia->totalVenta));
    memset(datosVentaBancolombia->compraNeta, 0x00, sizeof(datosVentaBancolombia->totalVenta));
    memcpy(datosVentaBancolombia->totalVenta, auxMonto, sizeof(datosVentaBancolombia->totalVenta));
    memcpy(datosVentaBancolombia->compraNeta, auxNeta, sizeof(datosVentaBancolombia->compraNeta));

    return iRet;
}

DatosComision verificarComision(char *monto) {

    int resultado = 0;
    int intentosVentas = 0;
    int flujoMedio = 0;
    int resultadoCaja = 0;
    long valor = 0;
    uChar linea3[25 + 1] = {0x00};
    uChar valorFormato[12 + 1] = {0x00};
    DatosComision datosComision;

    memset(&datosVentaBancolombia, 0x00, sizeof(datosVentaBancolombia));
    memset(&datosComision, 0x00, sizeof(DatosComision));
    memcpy(datosVentaBancolombia.totalVenta, monto, strlen(monto));

    datosComision = calculoComisionGiro(datosVentaBancolombia.totalVenta);

    if (atol(datosComision.valorComision) > 0) {

        memcpy(valorFormato, datosComision.valorComision, 12);
        valor = atoi(valorFormato);
        memset(valorFormato, 0x00, sizeof(valorFormato));
        formatString(0x00, valor, (char *) "", 0, valorFormato, 1);
        sprintf(linea3, "COMISION: %s", valorFormato);

        enviarStringToJava("COMISION QUE DEBE PAGAR EL GIRADOR", "guardarDatosGiro");
        enviarStringToJava(linea3, "guardarDatosGiro");

        resultado = confirmarGiro(datosVentaBancolombia.totalVenta,
                                  datosComision.valorComision,
                                  ASUMIR_COMISION, datosVentaBancolombia.compraNeta);

    }

    return datosComision;
}

int verificarNumeroItemsDocumentos() {
    int itemsTipoDocumento = 0;
    long lengthTiposDocumento = 0;

    lengthTiposDocumento = tamArchivo(discoNetcom, (char *) "/TDOC.TXT");
    itemsTipoDocumento = lengthTiposDocumento / LEN_TIPOS_DOCUMENTOS;

    return itemsTipoDocumento;
}

int realizarEnvioGiro(char *tipoDocumento, char *tipoDocumentoAux, DatosComision datosComision) {

    int resultado = 0;
    int intentosVentas = 0;
    long valor = 0;
    DatosCnbBancolombia datosEnviarGiro;// no hace nada quitarlo
    memset(&datosEnviarGiro, 0x00, sizeof(datosEnviarGiro));

    valor = atoi(datosComision.valorComision) - atoi(datosComision.valorIvaComision);
    sprintf(datosVentaBancolombia.propina, "%ld", valor);
    memset(datosComision.valorComision, 0x00, sizeof(datosComision.valorComision));
    sprintf(datosComision.valorComision, "%ld", valor);
    do {
        armarTramaGiro(&datosEnviarGiro, ENVIAR_GIROS, tipoDocumento, tipoDocumentoAux,
                       datosComision);

        obtenerTerminalModoCorresponsal(
                datosVentaBancolombia.terminalId);
        resultado = procesarGiros(&datosEnviarGiro,
                                  ENVIAR_GIROS);
        if (resultado == -2) {
            intentosVentas++;
        } else if (resultado == 0) {
            resultado = -1;
        }

        if (intentosVentas == MAX_INTENTOS_VENTA) {
            //screenMessage("COMUNICACION", "REINTENTE", "TRANSACCION","", 2 * 1000);
            resultado = -1;
        }

    } while (resultado == -2 && intentosVentas < MAX_INTENTOS_VENTA);

    if (resultado > 0) {
        mostrarAprobacionBancolombia(&datosVentaBancolombia);
    }
    return resultado;
}

int reclamacionGiro(char *tipoDocumento, char *tipoDocumentoAux, DatosComision datosComision) {
    int resultado = 0;
    int intentosVentas = 0;
    int resultadoCaja = 0;
    int flujoMedio = 0;

    DatosCnbBancolombia datosReclamarGiro;// no hace nada quitarlo
    memset(&datosReclamarGiro, 0x00, sizeof(datosReclamarGiro));
    do {
        armarTramaGiro(&datosReclamarGiro, RECLAMAR_GIROS, tipoDocumento,
                       tipoDocumentoAux,
                       datosComision);
        resultado = procesarGiros(&datosReclamarGiro, RECLAMAR_GIROS);
        if (resultado == -2) {
            intentosVentas++;
        }

        if (intentosVentas == MAX_INTENTOS_VENTA) {
            //screenMessage("COMUNICACION", "REINTENTE", "TRANSACCION", "", 2 * 1000);
            resultado = -1;
        }

    } while (resultado == -2 && intentosVentas < MAX_INTENTOS_VENTA);

    if (resultado > 0) {
        if (resultadoCaja == 1 || flujoMedio == 1) {
            setParameter(USO_CAJA_REGISTRADORA, (char *) "1");
            memcpy(datosVentaBancolombia.codigoRespuesta, "00", 2);
            //resultado = transaccionRespuestaDatosCaja(datosVentaBancolombia,TRANSACCION_TEFF_RECLAMAR_GIRO);
            if (resultado < 0) {
                memcpy(datosVentaBancolombia.codigoRespuesta, "WS", 2);
                imprimirDeclinadas(datosVentaBancolombia, "ERROR CON EL WS");
            }
        }

        mostrarAprobacionBancolombia(&datosVentaBancolombia);
    }

    return resultado;
}

int enviarConsultaCancelacionGiro(DatosComision *datosComision, char *tipoDocumento,
                                  char *tipoDocumentoAux, char * valorDevolucion) {

    int resultado = 0;


    uChar valorFormato[12 + 1] = {0x00};
    uChar linea3[25 + 1] = {0x00};

    DatosArchivoPar datosArchivo;
    DatosCnbBancolombia datosReclamarGiro;
    armarConsultaCancelacion(&datosReclamarGiro, CONSULTA_CANCELAR_GIROS, tipoDocumento,
                             tipoDocumentoAux,
                             *datosComision);
    resultado = procesarGiros(&datosReclamarGiro, CONSULTA_CANCELAR_GIROS);

    memcpy(datosVentaBancolombia.inc, datosVentaBancolombia.textoInformacion, 12);
    sprintf(datosVentaBancolombia.compraNeta, "%ld", atol(datosVentaBancolombia.inc));
    memcpy(datosComision->valorComision, datosVentaBancolombia.textoInformacion + 12,
           12);
    sprintf(datosVentaBancolombia.propina, "%ld", atol(datosComision->valorComision));
    memcpy(datosComision->valorIvaComision, datosVentaBancolombia.textoInformacion + 24,
           12);
    sprintf(datosVentaBancolombia.iva, "%ld", atol(datosComision->valorIvaComision));

    if (resultado > 0) {

        memset(valorFormato, 0x00, sizeof(valorFormato));

        if (atoi(datosVentaBancolombia.totalVenta) > 0) {
            formatString(0x00, 0, datosVentaBancolombia.totalVenta,
                         strlen(datosVentaBancolombia.totalVenta) - 2, valorFormato, 1);
        } else {
            formatString(0x00, 0, datosVentaBancolombia.totalVenta,
                         strlen(datosVentaBancolombia.totalVenta), valorFormato, 1);
        }

        sprintf(valorDevolucion, "%s", valorFormato);
        //resultado = getchAcceptCancel("MONTO", "VALOR A DEVOLVER", "AL CLIENTE", linea3, 30 * 1000);
    }

    return atoi(datosVentaBancolombia.totalVenta);
}

int
enviarCancelacionGiro(char *tipoDocumento, char *tipoDocumentoAux, DatosComision datosComision) {

    int resultado = 0;
    int intentosVentas = 0;
    DatosCnbBancolombia datosReclamarGiro;
    do {
        armarTramaGiro(&datosReclamarGiro, CANCELAR_GIROS, tipoDocumento,
                       tipoDocumentoAux,
                       datosComision);
        resultado = procesarGiros(&datosReclamarGiro, CANCELAR_GIROS);
        if (resultado == -2) {
            intentosVentas++;
        }
        if (intentosVentas == MAX_INTENTOS_VENTA) {
            //screenMessage("COMUNICACION", "REINTENTE", "TRANSACCION", "", 2 * 1000);
            resultado = -1;
        }
    } while (resultado == -2 && intentosVentas < MAX_INTENTOS_VENTA);

    if (resultado > 0) {
        mostrarAprobacionBancolombia(&datosVentaBancolombia);
    }
    return resultado;
}