//
// Created by NETCOM on 15/01/2020.
//
#include "Tipo_datos.h"
#ifndef POS_ANDROID_MENSAJERIA_H
#define POS_ANDROID_MENSAJERIA_H
typedef  char uChar;

#define	LEN_MSG_CODE										4
#define	LEN_BITMAP											8
#define	LEN_PAN												19
#define	LEN_PROC_CODE										6
#define	LEN_TRAN_AMT										12
#define	LEN_FRN_AMT											12
#define LEN_FIELD7											10
#define	LEN_DCC_RATE										8
#define	LEN_STAN											6
#define	LEN_LOCAL_TIME										6
#define	LEN_LOCAL_DATE										4
#define	LEN_EXP_DATE										4
#define	LEN_SETTLE_DATE										4
#define	LEN_ENTRY_MODE										3
#define	LEN_PAN_SEQ_NO										3
#define	LEN_NII												3
#define	LEN_COND_CODE										2
#define LEN_FIELD32             							20
#define	LEN_TRACK2											38
#define	LEN_TRACK3											104
#define	LEN_RRN												12
#define	LEN_AUTH_CODE										6
#define	LEN_RSP_CODE										2
#define	LEN_FIELD40											8
#define	LEN_TERM_ID											8
#define	LEN_MERCHANT_ID										15
#define	LEN_FIELD43											40
#define	LEN_ADDL_RSP										2
#define	LEN_TRACK1											76
#define	LEN_FIELD47											10
#define	LEN_FIELD48											100
#define	LEN_FRN_CURCY_CODE									3
#define	LEN_CURCY_CODE										3
#define	LEN_PIN_DATA										8
#define	LEN_EXT_AMOUNT										14
#define	LEN_ICC_DATA										520
#define	LEN_ICC_DATA2										110
#define	LEN_FIELD56											4
#define	LEN_FIELD57											256
#define	LEN_FIELD58											256
#define	LEN_FIELD60											512
#define	LEN_TMSFIELD60										600
#define	LEN_FIELD61											256
#define	LEN_INVOICE_NO										6
#define LEN_FIELD62             							20
#define	LEN_FIELD63											200
#define LEN_MAC												8
#define TOTAL_FIELD                                     	64

#define BCD_BITMAP          1
#define BCD_NUMERIC         2
#define BCD_LL_NUMERIC      3
#define BCD_AMOUNT          4
#define BCD_CHAR            5
#define BCD_LL_CHAR         6
#define BCD_LLL_CHAR        7
#define BCD_BINARY          8





int getIdFieldISO(int field);
ISOFieldMessage getISOFieldMessageISO(int field);
void packDataElement(FieldDescription packageISO8583,uChar *outputElement, uChar * inputElement, int *inLength);
int validateDataElement();
void convertIntegerToHex(int integer,uChar * buffer);
void inicializarCampos(void);
int unpackDataElement(FieldDescription packageISO8583, uChar *outputElement, uChar * inputElement,int * inLength);
ISOFieldMessage getField(int field);
ResultISOUnpack unpackISOMessage(uChar * messageISO, int inLength);
void setHeader(ISOHeader header);
void setField(int field, uChar * valueField, int totalBytes);
void setMTI(uChar* valueMTI);
ResultISOPack packISOMessage(void);
void inicializarMensajeriaTokens(void);
ResultTokenPack _packTokenMessage_(void);
void _setTokenMessage_(char * token, uChar * valueToken, int totalBytes);
int getIdToken(char * token);
TokenMessage _getTokenMessage_(char * token);
ResultTokenUnpack unpackTokenMessage(uChar * messageToken, int inLength);
ISOFieldMessage getMTI();
void limpiarTokenMessage();
#endif //POS_ANDROID_MENSAJERIA_H
