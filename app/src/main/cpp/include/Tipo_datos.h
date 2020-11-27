//
// Created by NETCOM on 14/02/2020.
//

#ifndef POS_PIN_ANDROID_TIPO_DATOS_H
#define POS_PIN_ANDROID_TIPO_DATOS_H
/*
 * TIPO_DATOS.h
 *
 *  Created on: 06/12/2012
 *      Author: Mario Rueda
 */

#ifndef TIPO_DATOS_H_
#define TIPO_DATOS_H_
#include "Definiciones.h"

typedef  char uChar;
typedef unsigned char boolean;
typedef unsigned short int  	word;			/*!< signed or unsigned integer on 16 bits.*/

#define    	TRUE            1
#define    	FALSE           0

///////// PARAMETROS EXCEPCIONES DE EMV ////////

typedef struct{
    short obligaBanda;//bit 0
    short seleccionCuentaDefault;//bit 1
    short gasoPass;//uso GasoPass
    short seleccionCuentaMultiservicios;//uso Multibolsillos
    short bit4;//uso futuro
    short bit5;//uso futuro
    short bit6;//uso futuro
    short bit7;//uso futuro
}ConfiguracionExcepciones1;

typedef struct{
    short bit0;//uso futuro
    short bit1;//uso futuro
    short bit2;//uso futuro
    short bit3;//uso futuro
    short bit4;//uso futuro
    short bit5;//uso futuro
    short bit6;//uso futuro
    short bit7;//uso futuro
}ConfiguracionExcepciones2;

typedef struct{
    ConfiguracionExcepciones1  comportamientoExcepciones1;
    ConfiguracionExcepciones2  comportamientoExcepciones2;
}ConfiguracionExcepciones;

typedef struct{
    uChar totalVenta [12 + 1];
    short validoPin;
    uChar cardName[TAM_CARD_NAME + 1];
    uChar propina[TAM_COMPRA_NETA + 1];
    uChar tipoTransaccion[2 + 1];
}DatosValidacionPin;

//////// INFORMACION CONVENIOS ///////

typedef struct{
    uChar idCliente[22 + 1];
    uChar porcentajeDescuento[12 + 1];
    uChar codigoConvenio[4 + 1];
    uChar nombreConvenio[20 + 1];
    uChar descuento[12 + 1];
    int isConsultaVivamos;
    int isCompraVivamos;
    int isPagoOtraTarjeta;
}DatosDescuentosVivamos;

//////// EMISORES ADICIONALES ///////

typedef struct{
    uChar indiceEmisor;
    uChar indiceImagen;
    uChar nombreEmisor[15 + 1];
    int   existeIssuerEmisor;
}EmisorAdicional;

//////// EMISORES ADICIONALES ///////

typedef struct{
    uChar tipoTransaccion [TAM_TIPO_TRANSACCION + 1];
    uChar estadoTransaccion[TAM_ESTADO_TRANSACCION + 1]; //1 activo 0 anulada
    uChar tipoCuenta [TAM_TIPO_CUENTA + 1];//10 20 30 40
    uChar numeroRecibo[TAM_NUMERO_RECIBO + 1];
    uChar systemTrace[TAM_SYSTEM_TRACE + 1];
    uChar msgType[TAM_MTI + 1];
    uChar processingCode[TAM_CODIGO_PROCESO + 1];
    uChar compraNeta[TAM_COMPRA_NETA + 1];
    uChar iva[TAM_IVA + 1];
    uChar totalVenta[TAM_COMPRA_NETA + 1];
    uChar txnTime[TAM_TIME + 1];
    uChar txnDate[TAM_DATE + 1];
    uChar posEntrymode[TAM_ENTRY_MODE + 1];
    uChar nii[TAM_NII + 1];
    uChar posConditionCode[TAM_POS_CONDITION + 1];
    uChar track2[TAM_TRACK2 + 1];
    uChar terminalId[TAM_TERMINAL_ID + 1];
    uChar acquirerId[TAM_ACQIRER_ID + 1];
    uChar field57[TAM_FIELD_57 + 1];
    uChar field61[TAM_FIELD_61 + 1];
    uChar rrn[TAM_RRN + 1];
    uChar ultimosCuatro[TAM_CODIGO_PROCESO + 1];
    uChar cardName[TAM_CARD_NAME + 1];
    uChar codigoAprobacion[TAM_COD_APROBACION + 1];
    uChar codigoGrupo [TAM_GRUPO + 1];
    uChar codigoSubgrupo [TAM_SUBGRUPO + 1];
    uChar propina[TAM_COMPRA_NETA + 1];
    uChar baseDevolucion[TAM_COMPRA_NETA + 1];
    uChar fiel42[TAM_FIELD_42 + 1];
    uChar AID[TAM_AID +1];
    uChar ARQC[TAM_ARQC+1];
    uChar cuotas[TAM_CUOTAS+1];
    uChar textoAdicional[TAM_TEXTO_ADICIONAL+1];
    uChar isQPS[TAM_ACTIVO_QPS+1];
    uChar fechaExpiracion[TAM_FECHA_EXPIRACION+1];
    uChar inc[TAM_IVA + 1];
    uChar tarjetaHabiente[TAM_TARJETA_HABIENTE + 1];
    uChar textoAdicionalInicializacion[TAM_TEXTO_ADICIONAL+1];
    uChar modoDatafono[TAM_MODO_PRODUCTO + 1];
    uChar textoInformacion[TAM_TEXTO_INFORMACION + 1];
    uChar creditoRotativo[TAM_CREDITO_ROTATIVO + 1];
    uChar appLabel[TAM_CARD_NAME + 1];
    uChar codigoComercio[TAM_ID_COMERCIO + 1];
    uChar nombreComercio[TAM_COMERCIO + 1];
    uChar numeroRecibo2[TAM_NUMERO_RECIBO + 1];
    uChar estadoTarifa[TAM_ESTADO_TARIFA + 1];
    uChar codigoCajero[TAM_CODIGO_CAJERO + 1];
    uChar facturaCaja[TAM_FACTURA_CAJA + 1];
    uChar codigoRespuesta[TAM_CODIGO_RESPUESTA + 1];
    uChar tokenVivamos[TAM_TOKEN_VIVAMOS + 1];
    uChar aux1[TAM_AUX + 1];
}DatosVenta;

typedef struct{
    char textoPantalla[25 + 1];
    char valor [27 + 1];
}ITEM_MENU;

typedef struct {
    char totalVenta[TAM_COMPRA_NETA + 1];
    char compraNeta[TAM_COMPRA_NETA + 1];
    char iva[TAM_IVA + 1];
    char inc[TAM_IVA + 1];
    char baseDevolucion[TAM_COMPRA_NETA + 1];
    char track2[TAM_TRACK2 + 1];
    char tipoCuenta [TAM_TIPO_CUENTA + 1];
}DatosLifeMiles;

typedef struct {
    uChar panRangeLow[10 + 1];
    uChar panRangeHigh[10 + 1];
    uChar issuerTableId;
    uChar acquirerTableID;
}RangoBinesEncontrados;



////***** TABLA 1 INICIALIZACION *****/////
typedef struct{
    uChar nombreComercio[20 + 1];
    uChar idComercio[2+1];
    uChar panRangeLow[10 + 1];
    uChar panRangeHigh[10 + 1];
    uChar tipoBono;
    int resultado;
}IssuerBonos;

typedef struct{
    uChar pan[19+1];
    uChar fechaVencimiento[4+1];
    uChar cvc[3+1];
    uChar detalle[20+1];
}tarjetasBonos;

typedef struct
{
    short unusedCero;
    short unusedUno;
    short unusedDos;
    short autoAnswer;
    short referralDialing;
    short memoryDialing;
    short tollDialing;
    short keyboardDialing;
}TelephoneDialOptionsTablaUno;
typedef struct
{
    short amountDualEntry;
    short displayMagneticStripeReaderInput;
    short tipProcessing;
    short cashierProcessing;
    short lodgingProcessing;
    short printTimeOnReceipt;
    short businessDateFormat;
    short reconciliationMethod;
}Options1TablaUno;
typedef struct
{
    short reserved;
    short printerUsed;
    short printTotalOnReceipt;
    short tillProcessing;
    short refundTransactionsProcessedOffline;
    short voidTransactionsProcessedOffline;
    short enableReceiptPreprint;
    short CardVerifyKeyOrDebitKey;
}Options2TablaUno;
typedef struct
{
    short unusedCero;
    short electronicCardReaderInterface;
    short amountVerifyOnPinPad;
    short printCardVerifyReceipt;
    short takeImprintPromptManualEntry;
    short checkReaderInstalled;
    short cardReadFromPinPad;
    short unusedSiete;
}Options3TablaUno;
typedef struct
{
    short adjustByCashier;
    short promptServerNotCashier;
    short batchReviewByCashier;
    short tenderCashPrompt;
    short reserved4;
    short reserved5;
    short duplicateProcessing;
    short unused;
}Options4TablaUno;
typedef struct
{
    short hostSelectionType;
    short noHostSelectPromptAtSettlement;
    short notUsed2;
    short notUsed3;
    short notUsed4;
    short notUsed5;
    short notUsed6;
    short notUsed7;
}Options5TablaUno;
typedef struct
{
    short keyboardLocked;
    short voidPassword;
    short refundPassword;
    short adjustmentPassword;
    short reportPassword;
    short unused5;
    short unused6;
    short unused7;
}localTerminalOptionBits;
typedef struct
{
    short enterAddress;
    short driversLicenseNumber;
    short enterCheckNumber;
    short enterTransitNumber;
    short enterRouteNumber;
    short enterLastFourDigitsOfAccount;
    short enterCheckAccountNumber;
    short enterMicrNumber;
}CheckPrompts1Bits;
typedef struct
{
    short expirationDateMMYY;
    short idlePrompt;
    short enterTransCode;
    short enterIdNumber;
    short birthdateMMDDYY;
    short enterPhoneNumber;
    short enterZipCode;
    short stateCode;
}CheckPrompts2Bits;
typedef struct
{
    short notUsed0;
    short notUsed1;
    short notUsed2;
    short notUsed3;
    short notUsed4;
    short notUsed5;
    short notUsed6;
    short enterCheckNumberIfNotScanned;
}CheckPrompts3Bits;
typedef struct
{
    char dllTraking;
    char initializationControl;
    char printerBaudRate;
    char ecrBaudRate;
    char dateandTime[6 + 1];
    TelephoneDialOptionsTablaUno telephoneDialOptions;
    char password[2 + 1];
    char helpDeskPhone[12 + 1];
    Options1TablaUno options1;
    Options2TablaUno options2;
    Options3TablaUno options3;
    Options4TablaUno options4;
    char receiptLine2[23 + 1];
    char receiptLine3[23 + 1];
    char defaultMerchantName[23 + 1];
    char currencySymbol;
    char transactionCurrencyDigits;
    char currencyDecimalPointPosition;
    char languageIndicator;
    char settlementCurrencyDigits;
    char checkService;
    char checkGuaranteeIssuerTableId;
    char checkGuaranteeAcquirerTableId;
    localTerminalOptionBits localTerminalOptions;
    char defaultTransaction;
    char debitIssuerId;
    char debitAcquirerId;
    char phoneLineHold;
    char reservedForFutureUse[2 + 1];
    char pinPadType;
    char printerType;
    char cashBackLimit[2 + 1];
    char maintenancePassword[3 + 1];
    char nmsCallSchedule;
    CheckPrompts1Bits checkPrompts1;
    CheckPrompts2Bits checkPrompts2;
    CheckPrompts3Bits checkPrompts3;
    char merchantSurcharge[2 + 1];
    Options5TablaUno options5;
    char reservedForFutureUse2[8 + 1];
}TablaUnoInicializacion;
////***** TABLA 1 INICIALIZACION *****/////
////***** TABLA 2 INICIALIZACION *****/////
typedef struct
{
    short debitYesOrNoPrompt;
    short debitKeyOnly;
    short selectDebitFromPin;
    short notUsed3;
    short notUsed4;
    short notUsed5;
    short notUsed6;
    short notUsed7;
}Options1TablaDos;
typedef struct
{
    char cardRangeNumber;
    char panRangeLow[5 + 1];
    char panRangeHigh[5 + 1];
    char issuerTableId;
    char acquirerTableID;
    char panLength;
    Options1TablaDos options1;
    char options2;
    char options3;
    char options4;
    char issuerTableIdNumber2;
    char acquirerTableIdNumber2;
    char reservedForFutureUse[2 + 1];
    int encontrado;
}TablaDosInicializacion;
////***** TABLA 2 INICIALIZACION *****/////

////***** TABLA 3 INICIALIZACION *****/////
typedef struct
{
    short selectAccountType;
    short pinEntry;
    short manualPanAllowed;
    short expirationDateRequired;
    short offLineEntryAllowed;
    short voiceReferalAllowed;
    short descriptorsRequired;
    short adjustAllowed;
}Options1TablaTres;
typedef struct
{
    short performMod10CheckOnPAN;
    short invoiceNumberRequired;
    short printReceipt;
    short captureTransactions;
    short checkExpirationDate;
    short beMyGuest;
    short blockRefunds;
    short blockCardVerify;
}Options2TablaTres;
typedef struct
{
    short blockVoids;
    short cashBack;
    short printSignatureNotRequired;
    short printNoRefundsAllowed;
    short refundProcessingOffline;
    short voidProcessingOffline;
    short allowCashOnlyTransaction;
    short generateSixDigitAuthCode;
}Options3TablaTres;
typedef struct
{
    short debitTransaction;
    short verifyLastFourPANDigits;
    short authorizeOverFloorLimit;
    short autoReprintFailedReceipt;
    short ignoreTrack1Error;
    short promptForTax;
    short promptForFiveDigitCreditPlan;
    short promptForProductCodes;
}Options4TablaTres;
typedef struct
{
    short pinDisabledForDebit;
    short pinDisabledForReturn;
    short pinDisabledForVoid;
    short pinDisabledForAdjust;
    short pinDisabledForCashback;
    short pinDisabledForBalanceInquiry;
    short pinDisabledForPreauthorization;
    short unused;
}PinEntryTransactionsBits;
typedef struct
{
    short doNotCaptureDebitTransactions;
    short etbProcessing;
    short generationNumberPrompt;
    short additionalRefundAmountPrompts;
    short purchaseOrderNumberPrompt;
    short barAmountPrompt;
    short unused;
    short disablePrintingRrn;
}Options5TablaTres;
typedef struct
{
    short AvsEnabledOnSale;
    short cardholderAddressRequired;
    short cardholderZipCodeRequired;
    short unused3;
    short unused4;
    short unused5;
    short unused6;
    short unused7;
}Options6TablaTres;
typedef struct
{
    short additionalPrompt1;
    short additionalPrompt2;
    short additionalPrompt3;
    short additionalPrompt4;
    short additionalPrompt5;
    short additionalPrompt6;
    short additionalPrompt7;
    short additionalPrompt8;
}AdditionalPromptsSelectBits;
typedef struct
{
    char issuerNumber;//5
    char issuerTableId;//6
    char cardName[TAM_CARD_NAME + 1];//16
    char referralPhoneNumber[12 + 1];//28
    Options1TablaTres options1;//29
    Options2TablaTres options2;//30
    Options3TablaTres options3;//31,printNoRefundsAllowed => cuotas
    Options4TablaTres options4;//32
    char defaultAccount;//33
    char reservedForFutureUse[2 + 1];//35
    char floorLimit[2 + 1];//37
    char reauthorizationMarginPercentage;//38
    PinEntryTransactionsBits pinEntryTransactions;//39
    AdditionalPromptsSelectBits additionalPromptsSelect;//40
    char cardType;
    char downPaymentIndicator;
    Options5TablaTres options5;
    Options6TablaTres options6;
    char reservedForFutureUse2[4 + 1];
    int respuesta;

}TablaTresInicializacion;
////***** TABLA 3 INICIALIZACION *****/////

////***** TABLA 4 INICIALIZACION *****/////
typedef struct
{
    short reservedByHypercom0;
    short reservedByHypercom1;
    short reservedByHypercom2;
    short ignoreHostTime;
    short reservedByHypercom4;
    short reservedByHypercom5;
    short reservedByHypercom6;
    short reservedByHypercom7;
}Options1TablaCuatro;
typedef struct
{
    short reservedByHypercom0;
    short reservedByHypercom1;
    short printVisaIIMerchantNumberInsteadOfAcquirerId;
    short sendCurrentBatchNumberInMessages;
    short macKeyExchangeMethod;
    short PinKeyExchangeMethod;
    short disableStatisticsFollowingSettlement;
    short disableNbdHeader;
}Options2TablaCuatro;
typedef struct
{
    short disable0220Messages;
    short achTotalsReportOnSettlement;
    short nonSettlementAcquirer;
    short hdcMode;
    short unused4;
    short unused5;
    short unused6;
    short unused7;
}Options3TablaCuatro;
typedef struct
{
    char acquirerNumber;
    char acquirerId;
    char acquirerProgramName[10 + 1];
    char acquirerName[10 + 1];
    char primaryTransactionTelephoneNumber[12 + 1];
    char primaryTransactionConnectionTime;
    char primaryTransactionDialAttempts;
    char secondaryTransactionTelephoneNumber[12 + 1];
    char secondaryTransactionConnectionTime;
    char secondaryTransactionDialAttempts;
    char primarySettlementTelephoneNumber[12 + 1];
    char primarySettlementConnectionTime;
    char primarySettlementDialAttempts;
    char secondarySettlementTelephoneNumber[12 + 1];
    char secondarySettlementConnectionTime;
    char secondarySettlementDialAttempts;
    char modemModeTransactions;
    Options1TablaCuatro options1;
    Options2TablaCuatro options2;
    Options3TablaCuatro options3;
    char options4;
    char NetworkInternationalId[2 + 1];
    char cardAcceptorTerminalId[8 + 1];
    char cardAcceptorIdentificationCode[15 + 1];
    char timeOut;
    char currentBatchNumber[3 + 1];
    char nextBatchNumber[3 + 1];
    char settleTime[4 + 1];
    char settleDay[2 + 1];
    char encryptedPinKey[8 + 1];
    char masterPinKeyIndex;
    char encryptedMacKey[8 + 1];
    char encryptedMacKeyIndex;
    char visaITerminalId[23 + 1];
    char modemModeSettlement;
    char maximumAutosettlementAttempts;
    char settlementLogonId;
    char settlementProtocolName[10 + 1];
    char AcquiringInstitutionIdCode[6 + 1];
    char encryptedPinKey2[8 + 1];
}TablaCuatroInicializacion;
////***** TABLA 4 INICIALIZACION *****/////

////***** TABLA 5 INICIALIZACION *****/////
/*typedef struct{
  unsigned char posicionLectura;
  unsigned char cantidadLectura;
  unsigned char codigoEan[10 + 1];
  unsigned char codigoConvenio[4 + 1];
  unsigned char nombreConvenio[6 + 1];
}TablaCincoInicializacion;*/

typedef struct{
    int posicionLectura;
    int cantidadLectura;
    uChar codigoEan[10 + 1];
    uChar codigoConvenio[4 + 1];
    uChar nombreConvenio[6 + 1];
}TablaCincoInicializacion;

////***** TABLA 5 INICIALIZACION *****/////

////***** TABLA 6 INICIALIZACION *****/////

typedef struct{
    uChar consecutivoRegistro;
    uChar banderaAgrupacion;
    uChar texto[20 + 1];
}TablaSeisInicializacion;

////***** TABLA 6 INICIALIZACION *****/////

////***** TABLA 7 INICIALIZACION *****/////

typedef struct{
    uChar codigoFiid[2 + 1];
    uChar datoCheque;
    uChar nombreBanco[14 + 1];
    uChar binAsignado[6 + 1];
    int tipoTransaccion;
    int banderaValidacion;
    int codigoEntidad;
}TablaSieteInicializacion;

////***** TABLA 7 INICIALIZACION *****/////


////***** TABLA 8 INICIALIZACION *****/////
typedef struct{
    TablaCincoInicializacion  Pcr ;
    int posicionLecturaMonto;
    int cantidadLecturaMonto;
}TablaOchoInicializacion;


////***** TABLA 0C INICIALIZACION *****/////

typedef struct{
    uChar targetPorcentage[2 + 1];
    uChar maximunPorcentage[2 + 1];
    uChar thresholdValueAmount[4 + 1];
    uChar tacDenial[10 + 1];
    uChar tacOnline[10 + 1];
    uChar tacDefault[10 + 1];
    uChar transactionCurrencyCode[2 + 1];
}TablaCeroCInicializacion;

////***** TABLA 0C INICIALIZACION *****/////

////***** TABLA 0D INICIALIZACION *****/////

typedef struct{
    uChar indice0D[2 + 1];
    uChar longitudAid[2 + 1];
    uChar aid[32 + 1];
    uChar option1[2 + 1];
    uChar floor[4 + 1];
    uChar tAidVersion1[4 + 1];
    uChar tAidVersion2[4 + 1];
    uChar tAidVersion3[4 + 1];
    uChar defaultTdol[40 + 1];
    uChar defaultDdol[40 + 1];
    uChar offlineApprovedResponseCode[4 + 1];
    uChar offlineDeclinedResponseCode[4 + 1];
    uChar unableToGoOnLineOffApproved[4 + 1];
    uChar unableToGoOnLineOffDeclined[4 + 1];
}TablaCeroDInicializacion;

////***** TABLA 0D INICIALIZACION *****/////

////***** TABLA 0E INICIALIZACION *****/////

typedef struct{

    uChar tag9F06[16 + 1];
    uChar tag9F22[7 + 1];
    uChar tag9F8121[48 + 1];
    uChar tag9F8122[10 + 1];
    uChar tag9F8123[512 + 1];

} TablaCeroEInicializacion;

////***** TABLA 0E INICIALIZACION *****/////

////***** TABLA 0F INICIALIZACION *****/////

typedef struct{
    uChar tag9F06[9 + 1];
    uChar tag9F22[5 + 1];
    uChar tag9F8424[8 + 1];
}TablaCeroFInicializacion;

/*
typedef struct {
   uChar codigoComercioMultiPos[TAM_ID_COMERCIO + 1];
   uChar nombreComercioMultiPos[TAM_COMERCIO + 1];
   int respuesta;
}InformacionMultiPos;*/
////***** TABLA 0F INICIALIZACION *****/////



typedef struct{
    int TPDU;
    uChar source [LEN_SOURCE + 1];
    uChar destination [LEN_DESTINATION + 1];
}ISOHeader;

typedef struct{
    int totalBytes;
    int field;
    uChar valueField [LEN_BUFFER + 1];
}ISOFieldMessage;


typedef struct{
    int responseCode;
    int totalBytes;
    uChar isoPackMessage[LEN_BUFFER_MAX + 1];
}ResultISOPack;

typedef struct{
    int responseCode;
    int totalField;
    int listField [MAX_FIELD + 1]; //TOTAL_FIELD
}ResultISOUnpack;

typedef struct{
    int responseCode;
    int totalBytes;
    uChar isoPackMessage[2048 + 1];
}ResultISOPackRecibo;

typedef struct
{
    char track1[80 + 1];
    char track2[80 + 1];
    char pan[20 + 1];
    TablaTresInicializacion dataIssuer;
    int respuesta;
    int isChip; // 1 si, 0 no
    int isFallBack;
    int isBonos;
    int isDivididas;
    int isPayPass;
    int isEmisorAdicional;
    char fechaExpiracion[4 + 1];
    char nombreTarjetaHabiente[TAM_TARJETA_HABIENTE + 1];
    ConfiguracionExcepciones configuracionExcepciones;
}DatosTarjeta;


typedef struct{
    char cociente[50];
    unsigned long divideno;
    unsigned long divisor;
    unsigned long parteEntera;
    char parteDecimal[50];
    int cifras;
}DatosOperacion ;

typedef struct{
    int tipoDisplay;
    int tipoComunicacion;
}TipoTerminal;



typedef struct{
    int fieldId;
    int fieldLength;
    int dataType;
}FieldDescription;



typedef struct{

    int cantidadVentaCredito;
    int cantidadVentaDebito;
    int cantidadAnuladasCredito;
    int cantidadAnuladasDebito;
    long totalVentaCredito;
    long totalVentaDebito;
    long totalAnuladasCredito;
    long totalAnuladasDebito;

}DatosCierreVenta;


typedef struct{

    uChar codigoSubgrupo[TAM_SUBGRUPO + 1];
    uChar nombreSubgrupo[TAM_CARD_NAME + 1];
    int cantidadVenta;
    long totalVenta;
    long totalIva;
    long totalInc;
    long totalPropina;
    int cantidadAnulacion;
    long totalAnulacion;
    long totalBase;
    int cantidadQPS;
    long totalQPS;
}DatosSubgrupo;

typedef struct{
    int totalIdSubgrupo;
    uChar codigoGrupo[TAM_GRUPO + 1];
    uChar nombreGrupo[20 + 1];
    DatosSubgrupo datosSubgrupo [10 + 1];
    //int indiceComercio;
}DatosGrupo;


typedef struct{
    uChar codigoComercio[TAM_ID_COMERCIO + 1];
    uChar nombreComercio[TAM_COMERCIO + 1];
    int respuesta;
}DatosComercioMultiPos;



typedef struct{
    int cantidadRedencion;
    int cantidadHabilitacion;
    int cantidadRecarga;
    int cantidadAnulacion;
    long totalRedencion;
    long totalHabilitacion;
    long totalRecarga;
    long totalAnulacion;
}DatosBonos;


typedef struct{
    int cantidadVentaCredito;
    int cantidadVentaDebito;
    long totalVentaCredito;
    long totalVentaDebito;
}DatosOtrosCierre;

typedef struct {
    int cantidadLealtad;
    int cantidadAcumuladas;
    int cantidadRedimidas;
    int cantidadPendientes;
    int cantidadRedimidasBig;
    long totalAcumuladas;
    long totalRedimidas;
    long totalPendientes;
    long totalRedimidasBig;
    int bigBancolombia;
}DatosLealtad;


/**
 * @brief Este estructura define la configuracion de iva y propina.
 */
typedef struct
{
    int  ivaAutomatico;
    int  PorcentajeIva;
    int  habilitarPropina;

}ConfIvaPropina;
/**
 * @brief Este estructura se utiliza para la escritura de mensajes.
 */
typedef struct{
    char mensajes [30];
}lineasMensaje;

//Declaración de Estructuras
typedef struct
{
    int year;
    int month;
    int day;
} DateTo;

typedef struct
{
    int hora;
    int minu;
    int segu;
} HoraTo;

typedef struct {

    uChar *ListaMenu;
} ItemMenu;

typedef struct
{
    char venta[20 + 1];
    char iva[20 + 1];
    char totalVenta[20 + 1];
}Ventas;

typedef struct {
    int alineacion;
    int fuente;
    int negrilla;
    int ancho;
    int alto;
    int reducida;
    int inverso;
    int subrayado;
}CaracteristicaLinea;

typedef struct {
    int presencia;
    char contenidoLinea [512 + 1];
    CaracteristicaLinea carateristica;
}LineaImprimir;

typedef struct {
    char clave [20+1];
    char  * valor;
}HasMap;


typedef struct
{
    word key;                // Parameter key
    word length;             // Parameter length
    char *value;             // Parameter default
} Parametros;

/**
 * @brief Este estructura define la configuracion de cuentas divididas.
 */
typedef struct
{
    long ventaDivididas;
    long ivaDivididas;
    long propinaDivididas;
    long baseDevolucion;
    long IncDivididas;

}DatosDivididas;

typedef struct{
    uChar totalVentaIva[12 + 1];
    uChar totalVentaInc[12 + 1];
    uChar totalVenta [12 + 1];
    uChar compraNeta [12 + 1];
    uChar baseDevolucion [12 + 1];
    uChar iva [12 + 1];
    uChar inc [12 + 1];
    uChar propina [12 + 1];
    short indicePagos;
    short indicePagados;
    short tipoPagoDivididas;
}DatosBasicosVenta;

typedef struct
{
    int tipoCuenta;
    int cuentasTotal ;
    int codigoRespuesta ;
    DatosBasicosVenta datosBasicosVenta;
}TransferenciaDatos;

typedef struct
{
    unsigned long	tag ;							/*!< tag of the element. */
    unsigned int	length ;					/*!< length of the element. */
    unsigned long   intValue ;				/*!< pointer to the data of the element. */
    uChar   charValue [100 + 1];				/*!< pointer to the data of the element. */
} DataElementI;

typedef struct{
    int totalBytes;
    uChar token [2 + 1];
    uChar valueToken [LEN_BUFFER + 1];
}TokenMessage;

typedef struct{
    int responseCode;
    int totalBytes;
    uChar tokenPackMessage[LEN_BUFFER + 1];
}ResultTokenPack;

typedef struct{
    int responseCode;
    int totalToken;
}ResultTokenUnpack;

typedef struct{
    short compra;
    short psp;
    short pcr;
    short consultaRecarga;
    short recarga;
    short efectivo;
    short saldo;
    short transferencia;
    short retiroSinTarjeta;
    short deposito;
    short transferenciaSimple;
    short pagoProducto;
    short unused1;
    short unused2;
    short unused3;
    short unused4;
}OptionsTransaccion;

typedef struct
{
    short modoComercio;
    short modoPuntoPago;
    short modoCNBRBM;
    short modoCNBBancolombia;
    short modoCNBAval;
    short modoUnused1;
    short modoUnused2;
    short modoUnused3;
}OptionsProducto;

typedef struct
{
    OptionsProducto    optionsProducto;
    OptionsTransaccion optionsTransaccion;
    char tipoMensaje;
}TablaConsultaInformacion;

typedef struct
{
    uChar bin [5 + 1];
    uChar tipoMensaje [1 + 1];
    uChar bitmapTipoTx [2 + 1];
    uChar bitmapTipoProducto [1 + 1];
}DatosConsulta;

typedef struct{
    char mensaje1 [21 + 1];
    char mensaje2 [21 + 1];
    char mensaje3 [21 + 1];
}DatoMessage;


typedef struct{
    int estadoTransaccion [500 + 1];
    long cantidadActivas;
    long cantidadAnuladas;
    long totalActivas;
    long totalAnuladas;
    long totalTransacciones;
}ResumenTransacciones;


/******************************TIPO DATOS BANCOLOMBIA**************************/
typedef struct{
    uChar tipoTransaccion [TAM_TIPO_TRANSACCION + 1];
    uChar estadoTransaccion[TAM_ESTADO_TRANSACCION + 1]; //1 activo 0 anulada
    uChar tipoCuenta [TAM_TIPO_CUENTA + 1];//10 20 30 40
    uChar cuenta[ SIZE_NUMERO_CUENTA + 1 ];
    uChar numeroRecibo[TAM_NUMERO_RECIBO + 1];
    uChar systemTrace[TAM_SYSTEM_TRACE + 1];
    uChar msgType[TAM_MTI + 1];
    uChar processingCode[TAM_CODIGO_PROCESO + 1];
    uChar totalVenta[TAM_COMPRA_NETA + 1];
    uChar txnTime[TAM_TIME + 1];
    uChar txnDate[TAM_DATE + 1];
    uChar posEntrymode[TAM_ENTRY_MODE + 1];
    uChar nii[TAM_NII + 1];
    uChar posConditionCode[TAM_POS_CONDITION + 1];
    uChar track2[TAM_TRACK2 + 1];
    uChar terminalId[TAM_TERMINAL_ID + 1];
    uChar rrn[TAM_RRN + 1];
    uChar ultimosCuatro[TAM_CODIGO_PROCESO + 1];
    uChar cardName[TAM_CARD_NAME + 1];
    uChar codigoAprobacion[TAM_COD_APROBACION + 1];
    uChar AID[TAM_AID +1];
    uChar ARQC[TAM_ARQC+1];
    uChar appLabel[TAM_CARD_NAME + 1];
    uChar cuotas[TAM_CUOTAS+1];
    uChar fechaExpiracion[TAM_FECHA_EXPIRACION+1];
    uChar tarjetaHabiente[TAM_TARJETA_HABIENTE + 1];
    uChar modoDatafono[TAM_MODO_PRODUCTO + 1];
    uChar textoInformacion[TAM_TEXTO_INFORMACION + 1];
    uChar tipoCuentaDestino[ SIZE_TIPO_CUENTA + 1 ];
    uChar cuentaDestino[ SIZE_NUMERO_CUENTA + 1 ];
    uChar field61[TAM_FIELD_61 + 1];
    uChar convenio[ SIZE_COD_SERVICO + 1 ];
    uChar numeroReferencia[SIZE_REFERENCIA_MANUAL + 1];
    uChar field60[TAM_FIELD_61 + 1];
    uChar tipoRetiro[ SIZE_TIPO_PRODUCTO + 1 ];
    uChar tipoProducto[ SIZE_TIPO_PRODUCTO + 1 ];
    uChar estadoCierre[ SIZE_TIPO_PRODUCTO + 1 ];
    uChar codigoCajero[TAM_CODIGO_CAJERO + 1];
    uChar facturaCaja[TAM_FACTURA_CAJA + 1];
    uChar codigoRespuesta[TAM_CODIGO_RESPUESTA + 1];
    uChar tokensQF[ SIZE_TOKEN_QF + 1 ];
    uChar tokensP6[ SIZE_TOKEN_P6 + 1 ];
}DatosCnbBancolombia;

typedef struct{
    char montoMinimo [12 + 1];
    char montoMaximo [12 + 1];
    char comisionFija [12 + 1];
    char porcentajeComision [4 + 1];
    char porcentajeIva [4 + 1];
    char fechaInicio [8 + 1];
    char valorComision [12 + 1];
    char valorIvaComision [12 + 1];
    char referenciaCancelacion [8 + 1];
    char referenciaReclamacion [10 + 1];
} DatosComision;

typedef struct{
    uChar tipoDocumento [2 + 1];
    uChar descripcion [20 + 1];
    uChar longitudMinima [2 + 1];
    uChar longitudMaxima [2 + 1];
    uChar tipoIngreso [2 + 1];
} DatosArchivoPar;

typedef struct{
    uChar tipo [2 + 1];
    uChar bin [6 + 1];
} DatosQrDescuentos;
/******************************TIPO DATOS CORRESPONSALES**************************/
typedef struct{
    uChar tipoTransaccion [TAM_TIPO_TRANSACCION + 1];
    uChar estadoTransaccion[TAM_ESTADO_TRANSACCION + 1]; //1 activo 0 anulada
    uChar tipoCuenta [TAM_TIPO_CUENTA + 1];//10 20 30 40
    uChar cuenta[ SIZE_NUMERO_CUENTA + 1 ];
    uChar numeroRecibo[TAM_NUMERO_RECIBO + 1];
    uChar systemTrace[TAM_SYSTEM_TRACE + 1];
    uChar msgType[TAM_MTI + 1];
    uChar processingCode[TAM_CODIGO_PROCESO + 1];
    uChar totalVenta[TAM_COMPRA_NETA + 1];
    uChar txnTime[TAM_TIME + 1];
    uChar txnDate[TAM_DATE + 1];
    uChar posEntrymode[TAM_ENTRY_MODE + 1];
    uChar nii[TAM_NII + 1];
    uChar posConditionCode[TAM_POS_CONDITION + 1];
    uChar track2[TAM_TRACK2 + 1];
    uChar terminalId[TAM_TERMINAL_ID + 1];
    uChar rrn[TAM_RRN + 1];
    uChar ultimosCuatro[TAM_CODIGO_PROCESO + 1];
    uChar cardName[TAM_CARD_NAME + 1];
    uChar codigoAprobacion[TAM_COD_APROBACION + 1];
    uChar AID[TAM_AID +1];
    uChar ARQC[TAM_ARQC+1];
    uChar appLabel[TAM_CARD_NAME + 1];
    uChar cuotas[TAM_CUOTAS+1];
    uChar fechaExpiracion[TAM_FECHA_EXPIRACION+1];
    uChar tarjetaHabiente[TAM_TARJETA_HABIENTE + 1];
    uChar modoDatafono[TAM_MODO_PRODUCTO + 1];
    uChar textoInformacion[TAM_TEXTO_INFORMACION + 1];
    uChar tipoCuentaDestino[ SIZE_TIPO_CUENTA + 1 ];
    uChar cuentaDestino[ SIZE_NUMERO_CUENTA + 1 ];
    uChar field61[TAM_FIELD_61 + 1];
    uChar convenio[ SIZE_COD_SERVICO + 1 ];
    uChar numeroReferencia[SIZE_REFERENCIA_MANUAL + 1];
    uChar field60[TAM_FIELD_61 + 1];
    uChar tipoRetiro[ SIZE_TIPO_PRODUCTO + 1 ];
    uChar tipoProducto[ SIZE_TIPO_PRODUCTO + 1 ];
    uChar estadoCierre[ SIZE_TIPO_PRODUCTO + 1 ];
    uChar codigoCajero[TAM_CODIGO_CAJERO + 1];
    uChar facturaCaja[TAM_FACTURA_CAJA + 1];
    uChar codigoRespuesta[TAM_CODIGO_RESPUESTA + 1];
    uChar tokensQF[ SIZE_TOKEN_QF + 1 ];
    uChar tokensP6[ SIZE_TOKEN_P6 + 1 ];
}DatosCORRESPONSALES;

typedef struct{
    int respuesta;
    uChar numeroFactura[24 + 1];
    uChar monto[12 + 1];
    uChar codigoEan[10 + 1];
    uChar codigoConvenio[10 + 1];
    uChar nombreConvenio[6 + 1];
}DatosLectorBarras;

typedef struct {
    short permiteSeleccionCuenta;
    short permiteTextoAdicional;
    uChar texto[20 + 1];
    char ivaMaximoPermitido[2 + 1];
    char propinaMaximaPermitida[2 + 1];
    short permiteIngresoCoutas;
    short permiteIngresoDocumento;
    short permiteIngresoPin;
    short permiteConsultaCostoTransaccion;
    short isSuperCupo;
    short permiteIngresoUltmosCuatro;
    char encryptedPinKey[8 + 1];
    char encryptedPinKey2[8 + 1];
    short permitePropina;
    short permiteCelular;
    char itemsTipoDocumento[50 + 1];
    char tipoDocumento[20 + 1];
}ParametrosVenta;

typedef struct{
    int totalBytes;
    int field;
    uChar valueField [LEN_BUFFER_TEFF + 1];
}TEFFFieldMessage;

typedef struct{
    uChar buffer[20 + 1];
    uChar metodo [1 +1 ];
}DatosVersion;

typedef struct{
    DatosBasicosVenta datos;
    uChar totalAnulacion [12 + 1];
    int cantidadVenta ;
    int cantidadAnulaciones ;
}DatosBasicosCierre;

typedef struct {
    uChar textoTransaccion[20 + 1];
    uChar cardName[TAM_CARD_NAME + 1];
    uChar tipoCuenta[TAM_TIPO_CUENTA + 1]; //10 20 30 40
    uChar cuotas[TAM_CUOTAS + 1];
    uChar tarjetaHabiente[TAM_TARJETA_HABIENTE + 1];
    uChar fechaExpiracion[TAM_FECHA_EXPIRACION + 1];
    uChar titulo1[24 + 1];
    uChar titulo2[24 + 1];
    uChar mensajeError[48 + 1];
    uChar fecha[TAM_DATE + 1];
    uChar hora[TAM_TIME + 1];
    uChar codigoCajero[TAM_CODIGO_CAJERO + 1];
    uChar costoTransaccion[13 + 1];
    uChar montoTransaccion[12 + 1];
    uChar numeroRecibo[6 + 1];
}DatosTransaccionDeclinada;

typedef struct{
    char inbufferCustomEMV[1023+1];
    int longitudCustomEMV;
    char inbufferCoreEMV [512 + 1];
    int longitudCoreEMV;
    int tipoFlujo;
    int respuesta;
}DatosServicioEMV;

typedef struct {
    long limiteEmv;//AidFloorLimit - 0D
    long limitaCvmContacless;//AidCvmLimit - 0D
    long limiteContacless;//AidClessLimit - 0D
    short indicarOperacionesOfflines;// Habilita o deshabilita Ope. Offline.
    short tipoDescargaOffLine;// Tipo de descarga transacciones offLine.
    int timeDescargaOffline;//Parametro que indica el tiempo en min. para liberar operaciones offline.
    int cantidadOperacionesOffLine;//Indica la cantidad maxima de operaciones offline permitidas antes de liberar ope. offline.
}LimitesOffLine;

typedef struct{
    DatosVenta reciboVenta;
    int tipoCliente;
    int duplicado;
}DatosServicioDuplicado;

typedef struct {
    uChar titulo[25 + 1];
    uChar linea1[25 + 1];
    uChar linea2[25 + 1];
    uChar datoCapturado[50 + 1];
    int digMinimos;
    int digMaximos;
    int tipoDato;
    int validarCeros;
}PropiedadesCapturarDatos;


typedef struct{
    uChar consecutivoFactura [2 + 1];
    uChar totalVenta[TAM_COMPRA_NETA + 1];
    uChar codigoConvenio[10 + 1];
    uChar codigoRespuesta[TAM_CODIGO_RESPUESTA + 1];
    uChar comportamientoCaja[TAM_CODIGO_RESPUESTA + 1];
    uChar numeroReferencia[30 + 1];
    uChar codigoCajero[TAM_CODIGO_CAJERO + 1];
    uChar tipoTransaccion [TAM_TIPO_TRANSACCION + 1];
    uChar estadoTransaccion[TAM_ESTADO_TRANSACCION + 1];
    uChar numeroRecibo[TAM_NUMERO_RECIBO + 1];
    uChar rrn[TAM_RRN + 1];
    uChar field60[120 + 1];
    uChar codigoAprobacion[TAM_COD_APROBACION + 1];
    int lectorBarras ;
}DatosCajaAval;


typedef struct {
    char parameterIn[125 + 1];
    char parameterOut[125 + 1];
}MessageTransfer;

typedef struct{
    uChar tipoDocumento [2 + 1];
    uChar documento[10 + 1];
    uChar track[30 + 1];
    uChar fiid[3 + 1];
    int tipoLectura;
}DatosStckerCless;


typedef struct{
    uChar codigoCorresponsal [3 + 1];
    uChar fiidEntidad [TAM_FIID + 1];
    uChar tipoOperacion [4 + 1];
    uChar codigoEstablecimiento[TAM_CODIGO_UNICO + 1];
}DatosCajaMulticorresponsal;


typedef struct {
    short journal;
    long  posicionTransaccion;
    uChar  numeroRecibo [TAM_NUMERO_RECIBO + 1];
    uChar  estadoTransaccion [TAM_ESTADO_TRANSACCION + 1];
    uChar  tipoTransaccion [TAM_TIPO_TRANSACCION + 1];
} DatosJournals;

/**Estructura de datos para QR CODE * **/
typedef struct{
    uChar terminal [8 + 1];
    uChar tiempoIntentos [3 + 1];
    uChar numeroIntentos [1 + 1];
    uChar niiQr [4 + 1];
    int intentosRealizados;
}DatosIniQrCode;
typedef struct{
    uChar tiempoConsulta [2 + 1];
    uChar numeroIntentos [1 + 1];
    uChar tiempoVisualizacion [2 + 1];
    uChar niiGeneracionQr [4 + 1];
    uChar niiTransaccional [4 + 1];
}DatosQrCodeEmvco;

typedef struct {
    int  tipoTransaccion;
    int  corresponsal;  // indica el corresponsal al que pertenece la transaccion
    uChar codigoRespuesta[TAM_CODIGO_RESPUESTA + 1];//00
    uChar codigoAprobacion[TAM_COD_APROBACION + 1];//01
    uChar codigoCajero[TAM_CODIGO_CAJERO + 1];//33
    uChar totalVenta[TAM_COMPRA_NETA + 1];//40
    uChar numeroRecibo[TAM_NUMERO_RECIBO + 1];//65
    uChar cuotas[TAM_CUOTAS + 1];//67
    uChar codigoCorresponsal [3 + 1]; //72
    uChar fiidEntidad [TAM_FIID + 1]; //73
    uChar rrn[TAM_RRN + 1];//77
    uChar terminalId[TAM_TERMINAL_ID + 1];//78
    uChar codigoEstablecimiento[TAM_CODIGO_UNICO + 1];//79
    uChar txnDate[TAM_DATE + 1];//80
    uChar txnTime[TAM_TIME + 1];//81
    uChar codigoConvenio[TAM_CODIGO_OPERADOR + 1];//89
    uChar facturaPago[LEN_TEFF_NUMERO_FACTURA + 1];//!93	(Número Factura Pago)
    uChar informacionAdicional[TAM_INF_ADICIONAL + 1];//94
} RecaudoMulticorresponsal;

typedef struct {
    int tipoComunicacion;
    char ipInicializacion[20 + 1];
    char puertoInicializacion[20 + 1];
    char telefonoInicializacion[20 + 1];
}dataComunicacion;

/**Estructura de datos para Flujo Unificado * **/
typedef struct {
    uChar monto[TAM_COMPRA_NETA + 1];
    uChar codigoCajero[TAM_CODIGO_CAJERO + 1];
    uChar facturasRecaudo[1 + 1];
    uChar dataAuxiliar[30 + 1];
}datosFlujoUnificado;

/////***ESTRUCTURAS PARA DCC PURE COMMERCE***//////
typedef struct {
    uChar numeroRecibo[TAM_NUMERO_RECIBO + 1];
    uChar fielId6[TAM_61_6 + 1];
    uChar fielId9[TAM_61_9 + 1];
    uChar fielId10[TAM_61_10 + 1];
    uChar fielId14[TAM_61_14 + 1];
    uChar fielId20[TAM_61_20 + 1];
    uChar fielId21[TAM_61_21 + 1];
    uChar issuerCountryCode[4 + 1];
} DatosCampo61Eco;

typedef struct {
    uChar issuerCountryCode[4 + 1];
} DatosPais;

typedef struct {
    DatosVenta datosVenta;
    uChar issuerCountryCode[TAM_ISSUER_COUNTRY_CODE + 1];
    DatosCampo61Eco campo61Eco;
} DatosTransaccionLoggin;
/////***ESTRUCTURAS PARA DCC PURE COMMERCE***//////

/////***ESTRUCTURA PARA BIG-BANCOLOMBIA***//////
typedef struct {
    char identificadorLinea[1 + 1];
    char indentificadorProducto[1 + 1];
    char nombreConvenio[16 + 1];
    char binAsignado[6 + 1];
    char fiid[2 + 1];
    char bandera1[2 + 1];
    char bandera2[2 + 1];
    char bandera3[2 + 1];
    char bandera4[2 + 1];
    char bandera5[2 + 1];
    char bandera6[2 + 1];
    char bandera7[2 + 1];
    char bandera8[2 + 1];
    char banderasLibres[6 + 1];
} ConveniosBigBancolombia;

typedef struct{
    int codigoRespuesta;
    int totalBytes;
    char buffer[256];
}DatosEmpaquetadosQI;

typedef struct{
    uChar codigoComercio[TAM_ID_COMERCIO + 1];
    uChar nombreComercio[TAM_COMERCIO + 1];
    int respuesta;
}DatosComercioBig;
/////***ESTRUCTURA PARA BIG-BANCOLOMBIA***//////

/////***ESTRUCTURAS PARA DCC PURE COMMERCE***//////
/** Estructura para definir el tipo de datafono**/

typedef struct {
    int tecnologia;                      //Me define si el datafono es TELIUM I - TELIUM II
    int clessTerminal;                   //Me indica si la terminal tiene modulo cless o no
    int pinpadActivate;                  //Me indica si la terminal tiene pinpad o no conectado
    int pinpadWithCless;	             //Me indica si el pinpad tiene cless o no
} DatosTipoDatafono;

////********Estructuras para cifrar y descifrar datos **************//////////

typedef struct {
    unsigned char dataEncriptar[8];
    unsigned char dataEncriptada[8];
} DataEncriptar;

typedef struct {
    char encriptado[1 + 1];
    char ipDestino[15 + 1];
    char puerto[5 + 1];
    char nii[4 + 1];
    char metodo[50 + 1];
    char host[30 + 1];
} LineaUrlFirma;

typedef struct {
    char titulo[30 + 1];
    char message1[30 + 1];
    char message2[30 + 1];
    char message3[30 + 1];
    char message4[30 + 1];
    char message5[30 + 1];
    int timeOut;
}ParametersGUI;

typedef struct{
    uChar IDDCC[1 + 1];
    uChar franquiciaActiva[1 + 1];
    uChar parametroDcc[2 + 1];
    uChar niiAsignado[4 + 1];
    uChar perfilActivo[1 + 1];
    int codigoRespuesta;
}DatosInicializacionDcc;

typedef struct{
    int codigoRespuesta;
    int totalBytes;
    uChar buffer[266 + 1];
}DatosEmpaquetados;

typedef struct{
    uChar Tag32      [59 + 1];
    uChar Tag34       [3 + 1];
    uChar Tag60      [20 + 1];
    uChar Tag67      [20 + 1];
//	uChar simbolo    [32 + 1];
    uChar code        [3 + 1];
    int  totalBytes;
}DatosCampo63;

typedef struct
{
    int isPin;
    int asklastfour;
    int askDocument;
    int askCuotes;
    int askCvc2;
    int askAdicionalText;
    int sizeP55;
    char arqc[30 + 1];
    char aid[30 + 1];
    char apLabel[30 + 1];
    char track2[80 + 1];
    char tipoCuenta[2 + 1];
    char pinblock[16 + 1];
    char data55[512 + 1];
    char pan[20 + 1];
    char cuotas[2 + 1];
    char fechaExpiracion[4 + 1];
    char tarjetaHabiente[50 + 1];
    char grupo[2 + 1];
    char subGrupo[2 + 1];
    char documento[10 + 1];
    char cvc2[3 + 1];
}DatosTarjetaAndroid;

typedef struct
{
    int indicadorBaseDatos;
    long valorFactura;
    int indicadorValor;
    char tipoRecaudo[25 + 1];
    char valorApagar[12 + 1];
    char numeroFactura[25 + 1];
    char codigoConvenio[6 + 1];
    char cuentaRecaudadora[25 + 1];
    char tokenP6[161 + 1];
    char nombreConvenio[50 + 1];
}DatosRecaudoAndroid;

typedef struct{
    uChar codigoComercio[TAM_ID_COMERCIO + 1];
    uChar nombreComercio[TAM_COMERCIO + 1];
}DatosTerminalQr;
#define TAM_BUFFER_QR                    256
#define TAM_NUMERO_TARJETA			      19

typedef struct{
    uChar cardName[TAM_CARD_NAME + 1];
    uChar bin [6 + 1];
    uChar ultimosCuatro[TAM_CODIGO_PROCESO + 1];
    uChar cuotas [TAM_CUOTAS + 1];
    uChar tipoCuenta [TAM_TIPO_CUENTA + 1];
    uChar numeroTarjeta [TAM_NUMERO_TARJETA + 1];
    uChar buffer [TAM_BUFFER_QR + 1];
    int  totalBytes;
}DatosEmpaquetadosQrCode;

typedef struct{
    char nombreBanco [15 + 1];
    long totalVenta ;
    long totalVentaNeta;
    long totalIva ;
    long totalInc ;
    long totalPropina;
    long totalBase;
    int cantidadVenta ;
    int cantidadAnulaciones ;
    long totalAnulaciones;
}DatosCierreDineroElectronico;

typedef struct {
    int tipoPinSolicitado;
    int permiteCuotas;
    int permiteSeleccionCuenta;
    int permiteCelular;
    int permiteDocumento;
    char itemsTipoDocumento[50 + 1];
    char tipoDocumento[20 + 1];
    char codigoEntidad[4 + 1];
    char nombreBanco[20 + 1];
    char fiddOtp[4 + 1];
    char binAsignado[6 + 1];
}ParametrosDineroElectronico;

typedef struct {
  char year[4 + 1];
  char month[2 + 1];
  char day[2 + 1];
  char hour[2 + 1];
  char minutes[2 + 1];
  char secondes[2 + 1];

}Date;
#endif /* TIPO_DATOS_H_ */

#endif //POS_PIN_ANDROID_TIPO_DATOS_H
