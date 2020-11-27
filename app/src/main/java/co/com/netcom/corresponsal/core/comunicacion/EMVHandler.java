package co.com.netcom.corresponsal.core.comunicacion;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.RemoteException;
import android.util.Log;
import android.widget.LinearLayout;

import com.ingenico.lar.bc.apos.emv.UEMVWrapper;
import com.usdk.apiservice.aidl.constants.RFDeviceName;
import com.usdk.apiservice.aidl.emv.ACType;
import com.usdk.apiservice.aidl.emv.ActionFlag;
import com.usdk.apiservice.aidl.emv.CAPublicKey;
import com.usdk.apiservice.aidl.emv.CVMFlag;
import com.usdk.apiservice.aidl.emv.CVMMethod;
import com.usdk.apiservice.aidl.emv.CandidateAID;
import com.usdk.apiservice.aidl.emv.CardRecord;
import com.usdk.apiservice.aidl.emv.DOLType;
import com.usdk.apiservice.aidl.emv.ECCardLog;
import com.usdk.apiservice.aidl.emv.EMVError;
import com.usdk.apiservice.aidl.emv.EMVEventHandler;
import com.usdk.apiservice.aidl.emv.EMVTag;
import com.usdk.apiservice.aidl.emv.FinalData;
import com.usdk.apiservice.aidl.emv.ICCardLog;
import com.usdk.apiservice.aidl.emv.KernelID;
import com.usdk.apiservice.aidl.emv.OfflinePinVerifyResult;
import com.usdk.apiservice.aidl.emv.SearchCardListener;
import com.usdk.apiservice.aidl.emv.TransData;
import com.usdk.apiservice.aidl.emv.WaitCardFlag;
import com.usdk.apiservice.aidl.pinpad.OnPinEntryListener;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import androidx.recyclerview.widget.AsyncListUtil;
import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.emv.EmvData;
import co.com.netcom.corresponsal.emv.PinpadForMKSK;
import co.com.netcom.corresponsal.emv.TLV;
import co.com.netcom.corresponsal.emv.TLVList;
import co.com.netcom.corresponsal.emv.pinpad.KeyId;

public class EMVHandler {
    private final static String TAG = "EMVHandler";
    private boolean isEmvProcess;
    private boolean isShowDOL;
    private boolean isShowTLV;
    private boolean isShowDataAPDU;
    private boolean isShowBalance;
    private boolean isGetECCLogProcess;
    private boolean isGetICCLogProcess;
    private String resultMessage;
    private WeakReference<Context> context;
    private AsyncListUtil.ViewCallback callback;
    private WeakReference<EMVCallback> emvCallback;
    private CardDTO datosTarjeta;
    private UEMVWrapper uemvWrapper = new UEMVWrapper();
    private AlertDialog dialog;
    private String hMonto;
    private String hPropina;
    private Boolean soloTrack2;
    private static Boolean transaccionContinua = false;

    private static WaitThreat waitTipoCuentaInputThreat = new WaitThreat();

    public static class WaitThreat {
        Object syncObj = new Object();

        void waitForRslt() throws InterruptedException {
            synchronized (syncObj) {
                syncObj.wait();
            }
        }

        void notifyThread() {
            synchronized (syncObj) {
                syncObj.notify();
            }
        }
    }

    private static Handler tipoCuentaEventHandler = new Handler(Looper.getMainLooper()) {

        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what) {
                case Const.DATOS_ADICIONALES_FINISH:
                    transaccionContinua = true;
                    waitTipoCuentaInputThreat.notifyThread();
                    break;
                case Const.TRANSACCION_CANCELADA:
                    transaccionContinua = false;
                    waitTipoCuentaInputThreat.notifyThread();
                default:
                    break;
            }
        }

    };

    public static Handler getTipoCuentaEventHandler() {
        return tipoCuentaEventHandler;
    }

    public static void setTipoCuentaEventHandler(Handler tipoCuentaEventHandler) {
        EMVHandler.tipoCuentaEventHandler = tipoCuentaEventHandler;
    }

    public EMVHandler() {
        this.datosTarjeta = new CardDTO();
    }


    public EMVHandler(WeakReference<Context> context, AsyncListUtil.ViewCallback callback) {
        this.context = context;
        this.callback = callback;
        this.datosTarjeta = new CardDTO();
    }

    public EMVHandler(WeakReference<Context> context, WeakReference<EMVCallback> callback) {
        this.context = context;
        this.emvCallback = callback;
        this.datosTarjeta = new CardDTO();
    }

    /**
     * Procesos puntuales y divididos para las transacciones
     */
    public void procesarTarjeta(String monto, String propina, Boolean soloTrack2Val) throws RemoteException {
        hMonto = String.format("%010d", Integer.parseInt(monto) + Integer.parseInt(propina));
        hMonto = hMonto + "00";
        hPropina = String.format("%012d", Integer.parseInt(propina));
        soloTrack2 = soloTrack2Val;

        startProcess();
    }

    public void finalizar(String s) {

        try {
            uemvWrapper.emv.respondEvent(s);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void execute(String value) throws RemoteException {
        Log.d(TAG, "string value " + value);
        if (value.equals(context.get().getString(R.string.start_process))) {
            startProcess();

        } else if (value.equals(context.get().getString(R.string.get_balance))) {
            getBalance();

        } else if (value.equals(context.get().getString(R.string.get_dol))) {
            getDOL();

        } else if (value.equals(context.get().getString(R.string.get_tlv))) {
            getTLV();

        } else if (value.equals(context.get().getString(R.string.get_data_apdu))) {
            getDataAPDU();

        } else if (value.equals(context.get().getString(R.string.get_icc_log))) {
            getICCLog();

        } else if (value.equals(context.get().getString(R.string.get_ecc_log))) {
            getECCLog();
        }
    }

    private void startProcess() throws RemoteException {
        isEmvProcess = true;
        resultMessage = "";

        // Show dialoges
        if (!(isShowDOL || isShowTLV)) {
            //showDialog(onDismissListener, R.string.waiting_for_card);
            Log.d(TAG, context.get().getString(R.string.waiting_for_card));
        }

        // Start EMV process
        Bundle bundle = new Bundle();
        //bundle.putByte("flagPSE", uemvWrapper.emv.PSE_AID_LIST);
        //bundle.putByte("flagCtlAsCb", ENABLE_CONTACTLESS_CARD_SELECT_APP);
        bundle.putBoolean("flagICCLog", isGetECCLogProcess || isGetICCLogProcess);
        uemvWrapper.emv.startProcess(bundle, onEmvEventHandler);
    }

    private void showDialog(DialogInterface.OnDismissListener onDismissListener, int resCode) {
        dialog = new AlertDialog.Builder(context.get(), R.style.Theme_MaterialComponents_Light_Dialog_Alert)
                .setNegativeButton(context.get().getString(R.string.cancel), null)
                .setCancelable(false)
                .create();
        dialog.setOnDismissListener(onDismissListener);
        dialog.setMessage(context.get().getString(resCode));
        dialog.show();
        dialog.getWindow().setLayout(600, LinearLayout.LayoutParams.WRAP_CONTENT);
    }

    void hideDialog() {
        Log.d(TAG, "hideDialog: ");
        dialog.cancel();
    }

    //todo no quemar desde aplicacion
    private EMVEventHandler onEmvEventHandler = new EMVEventHandler.Stub() {
        @Override
        public void onInitEMV() throws RemoteException {
            Log.d(TAG, "----- onInitEMV -----");
            datosTarjeta.setIsChip(true);
            // Clear all the aids in EMV kernel.
            uemvWrapper.emv.manageAID(ActionFlag.CLEAR, null, true);
            // Add all aids from EMV data to EMV kernel.
            for (Map.Entry<String, Boolean> entry : EmvData.aids.entrySet()) {
                uemvWrapper.emv.manageAID(ActionFlag.ADD, entry.getKey(), entry.getValue());
            }

            // Set DOL
            uemvWrapper.emv.setDOL(DOLType.DDOL, "9F3704");

            // Set TLV
            uemvWrapper.emv.setTLV(KernelID.PBOC, com.usdk.apiservice.aidl.emv.EMVTag.EMV_TAG_TM_TERMTYPE, "22");

            // Set TLV list
            TLVList tlvList = new TLVList();
            tlvList.addTLV(TLV.fromData(com.usdk.apiservice.aidl.emv.EMVTag.EMV_TAG_TM_CAP, BytesUtil2.hexString2Bytes("60F8C8")));
            tlvList.addTLV(TLV.fromData(com.usdk.apiservice.aidl.emv.EMVTag.EMV_TAG_TM_CAP_AD, BytesUtil2.hexString2Bytes("6F00F0F001")));
            tlvList.addTLV(TLV.fromData(com.usdk.apiservice.aidl.emv.EMVTag.EMV_TAG_TM_CNTRYCODE, BytesUtil2.hexString2Bytes("0170")));
            tlvList.addTLV(TLV.fromData(com.usdk.apiservice.aidl.emv.EMVTag.EMV_TAG_TM_FLOORLMT, BytesUtil2.hexString2Bytes("00000000")));
            tlvList.addTLV(TLV.fromData(com.usdk.apiservice.aidl.emv.EMVTag.DEF_TAG_TAC_DECLINE, BytesUtil2.hexString2Bytes("0000000000")));
            tlvList.addTLV(TLV.fromData(com.usdk.apiservice.aidl.emv.EMVTag.DEF_TAG_TAC_ONLINE, BytesUtil2.hexString2Bytes("FFFFFFFFFF")));
            tlvList.addTLV(TLV.fromData(com.usdk.apiservice.aidl.emv.EMVTag.DEF_TAG_TAC_DEFAULT, BytesUtil2.hexString2Bytes("FFFFFFFFFF")));
            tlvList.addTLV(TLV.fromData(com.usdk.apiservice.aidl.emv.EMVTag.DEF_TAG_RAND_SLT_THRESHOLD, BytesUtil2.hexString2Bytes("000000000000")));
            tlvList.addTLV(TLV.fromData(com.usdk.apiservice.aidl.emv.EMVTag.DEF_TAG_RAND_SLT_PER, BytesUtil2.hexString2Bytes("00")));
            tlvList.addTLV(TLV.fromData(com.usdk.apiservice.aidl.emv.EMVTag.DEF_TAG_RAND_SLT_MAXPER, BytesUtil2.hexString2Bytes("99")));
            tlvList.addTLV(TLV.fromData(com.usdk.apiservice.aidl.emv.EMVTag.C_TAG_TM_9F66, BytesUtil2.hexString2Bytes("26000080")));
            tlvList.addTLV(TLV.fromData(com.usdk.apiservice.aidl.emv.EMVTag.C_TAG_TM_9F7A, BytesUtil2.hexString2Bytes("01")));
            tlvList.addTLV(TLV.fromData(com.usdk.apiservice.aidl.emv.EMVTag.C_TAG_TM_9F7B, BytesUtil2.hexString2Bytes("999999999999")));
            tlvList.addTLV(TLV.fromData(com.usdk.apiservice.aidl.emv.EMVTag.C_TAG_TM_DF69, BytesUtil2.hexString2Bytes("01")));
            tlvList.addTLV(TLV.fromData(com.usdk.apiservice.aidl.emv.EMVTag.C_TAG_TM_TRANS_LIMIT, BytesUtil2.hexString2Bytes("999999999999")));
            tlvList.addTLV(TLV.fromData(com.usdk.apiservice.aidl.emv.EMVTag.C_TAG_TM_CVM_LIMIT, BytesUtil2.hexString2Bytes("999999999999")));
            tlvList.addTLV(TLV.fromData(com.usdk.apiservice.aidl.emv.EMVTag.C_TAG_TM_FLOOR_LIMIT, BytesUtil2.hexString2Bytes("999999999999")));
            tlvList.addTLV(TLV.fromData(com.usdk.apiservice.aidl.emv.EMVTag.EMV_TAG_TM_TVR, BytesUtil2.hexString2Bytes("0000048000")));
            tlvList.addTLV(TLV.fromData(com.usdk.apiservice.aidl.emv.EMVTag.EMV_TAG_TM_CURCODE, BytesUtil2.hexString2Bytes("0170")));
            tlvList.addTLV(TLV.fromData(com.usdk.apiservice.aidl.emv.EMVTag.EMV_TAG_TM_TRANSTYPE, BytesUtil2.hexString2Bytes("00")));
            uemvWrapper.emv.setTLVList(KernelID.PBOC, tlvList.toString());
        }

        @Override
        public void onWaitCard(int flag) throws RemoteException {
            Log.d(TAG, "----- onWaitCard -----");
            Log.d(TAG, "flag : " + flag);
        //    turnOnBlueAndRedLights();
 //           Beeper.getInstance().startBeep(500);
            // Get DOL and stop EMV
            if (isShowDOL) {
                // Get DOL
                isShowDOL = false;
                resultMessage = uemvWrapper.emv.getDOL(DOLType.DDOL);

                // Stop EMV process
                stopProcess();
                return;
            }

            // Get TLV and stop EMV
            if (isShowTLV) {
                // Get TLV
                isShowTLV = false;
                resultMessage = uemvWrapper.emv.getTLV(com.usdk.apiservice.aidl.emv.EMVTag.EMV_TAG_TM_TERMTYPE);

                // Stop EMV process
                stopProcess();
                return;
            }

            switch (flag) {
                case WaitCardFlag.NORMAL:
                    // This case would never happen, if you have already searchCard before startEMV.
                    // Otherwise it would happen for searching card.
                    searchCard();
                    break;

                case WaitCardFlag.ISS_SCRIPT_UPDATE:
                case WaitCardFlag.SHOW_CARD_AGAIN:
                    researchRFCard();
                    break;

                case WaitCardFlag.EXECUTE_CDCVM:
                    // Halt RF card reader.
                    uemvWrapper.emv.halt();

                    // Delay and research.
                    new Thread() {
                        @Override
                        public void run() {
                            try {
                                sleep(1200);
                                researchRFCard();
                            } catch (RemoteException | InterruptedException e) {
                                e.printStackTrace();
                            }
                        }
                    }.start();
                    break;

                default:
                    // Stop EMV process
                    resultMessage = context.get().getString(R.string.unknown_error);
                    stopProcess();
                    break;
            }
        }

        @Override
        @Deprecated
        public void onCardChecked(int cardType) throws RemoteException {
            Log.d(TAG, "----- onCardChecked -----");
            Log.d(TAG, "cardType : " + cardType);

            // Only happen when use startProcess() which is deprecated
        }

        @Override
        public void onAppSelect(boolean reselect, List<CandidateAID> aids) throws RemoteException {
            Log.d(TAG, "----- onAppSelect -----");
            Log.d(TAG, "reselect : " + reselect);
            Log.d(TAG, "aidsSize : " + aids.size());

            // Get ECC log and stop EMV
            if (isGetECCLogProcess) {
                // Get ECC log
                isGetECCLogProcess = false;
                List<ECCardLog> ecLogs = new ArrayList<>();
                uemvWrapper.emv.getECCLog(aids.get(0).getAID(), ecLogs);
                if (ecLogs.size() > 0) {
                    for (ECCardLog ecCardLog : ecLogs) {
                        String date = "20" + BytesUtil2.byteArray2HexString(ecCardLog.getDate());
                        String time = BytesUtil2.byteArray2HexString(ecCardLog.getTime());
                        String beforeBalance = StringUtil.getReadableAmount(String.valueOf(Long.parseLong(BytesUtil2.byteArray2HexString(ecCardLog.getPreValue()))));
                        String afterBalance = StringUtil.getReadableAmount(String.valueOf(Long.parseLong(BytesUtil2.byteArray2HexString(ecCardLog.getAftValue()))));
                        resultMessage += date + "," + time + "," + beforeBalance + "," + afterBalance + "\r\n";
                    }
                } else {
                    resultMessage = context.get().getString(R.string.no_data);
                }

                // Stop EMV process
                stopProcess();
                return;
            }

            // Get ICC log and stop EMV
            if (isGetICCLogProcess) {
                // Get ICC log
                isGetICCLogProcess = false;
                List<ICCardLog> icLogs = new ArrayList<>();
                uemvWrapper.emv.getICCLog(aids.get(0).getAID(), icLogs);
                if (icLogs.size() > 0) {
                    for (ICCardLog icCardLog : icLogs) {
                        String date = "20" + BytesUtil2.byteArray2HexString(icCardLog.getDate());
                        String time = BytesUtil2.byteArray2HexString(icCardLog.getTime());
                        String amount = StringUtil.getReadableAmount(String.valueOf(Long.parseLong(BytesUtil2.byteArray2HexString(icCardLog.getAmount()))));
                        resultMessage += date + "," + time + "," + amount + "\r\n";
                    }
                } else {
                    resultMessage = context.get().getString(R.string.no_data);
                }

                // Stop EMV process
                stopProcess();
                return;
            }

            // Response
            TLVList tlvList = new TLVList();
            tlvList.addTLV(TLV.fromData(com.usdk.apiservice.aidl.emv.EMVTag.EMV_TAG_TM_AID, aids.get(0).getAID()));

            uemvWrapper.emv.respondEvent(tlvList.toString());
        }

        @Override
        public void onFinalSelect(FinalData finalData) throws RemoteException {
            String fechatx;
            String date;
            String time;
            Log.d(TAG, "----- onFinalSelect -----");
            Log.d(TAG, "KernelID:" + finalData.getKernelID());
            Log.d(TAG, "AID:" + BytesUtil2.bytes2HexString(finalData.getAID()));
            datosTarjeta.setAid(BytesUtil2.bytes2HexString(finalData.getAID()));
            if (isShowDataAPDU) {
                // Get data APDU
                isShowDataAPDU = false;
                resultMessage = uemvWrapper.emv.getDataAPDU("9F51");

                // Stop EMV process
                stopProcess();
                return;
            }

            if (isShowBalance) {
                // Get balance
                isShowBalance = false;
                resultMessage = String.valueOf(uemvWrapper.emv.getBalance());

                // Stop EMV process
                stopProcess();
                return;
            }
            fechatx = IntegradorC.obtenerFechaJava();
            date = fechatx.substring(2, 8);
            time = fechatx.substring(8);
            String systemTraceAuditoryNumber = IntegradorC.obtenerConsecutivoDeTransaccionEMV();
            // Response
            TLVList tlvList = new TLVList();
            tlvList.addTLV(TLV.fromData(com.usdk.apiservice.aidl.emv.EMVTag.EMV_TAG_TM_AUTHAMNTN, BytesUtil2.hexString2Bytes(hMonto))); //Monto
            tlvList.addTLV(TLV.fromData(com.usdk.apiservice.aidl.emv.EMVTag.EMV_TAG_TM_OTHERAMNTN, BytesUtil2.hexString2Bytes(hPropina))); //propina
            tlvList.addTLV(TLV.fromData(com.usdk.apiservice.aidl.emv.EMVTag.EMV_TAG_TM_TRANSDATE, BytesUtil2.hexString2Bytes(date)));// Fecha
            tlvList.addTLV(TLV.fromData(com.usdk.apiservice.aidl.emv.EMVTag.EMV_TAG_TM_TRANSTIME, BytesUtil2.hexString2Bytes("000000")));//hora
            tlvList.addTLV(TLV.fromData(com.usdk.apiservice.aidl.emv.EMVTag.EMV_TAG_TM_TRSEQCNTR, BytesUtil2.hexString2Bytes(systemTraceAuditoryNumber)));
            tlvList.addTLV(TLV.fromData(com.usdk.apiservice.aidl.emv.EMVTag.DEF_TAG_SERVICE_TYPE, BytesUtil2.hexString2Bytes("00")));

//hideDialog();
            uemvWrapper.emv.responseEvent(tlvList.toString());
        }

        @Override
        public void onReadRecord(CardRecord cardRecord) throws RemoteException {
            Log.d(TAG, "----- onReadRecord -----");

            // CA Public Key
            CAPublicKey pubKey = new CAPublicKey();
            pubKey.setIndex((byte) 0x01);
            pubKey.setRid(BytesUtil2.hexString2Bytes("A000000333"));
            pubKey.setMod(BytesUtil2.hexString2Bytes("1234567890"));
            pubKey.setExp(BytesUtil2.hexString2Bytes("03"));
            pubKey.setExpDate(BytesUtil2.hexString2Bytes("20171212"));
            pubKey.setHashFlag((byte) 0x00);
            pubKey.setHash(BytesUtil2.hexString2Bytes("1111111111111111111111111111111111111111"));
            uemvWrapper.emv.setCAPubKey(pubKey);

            // Response

            //Armar campo55
            /*TLVList listaCampo55 = new TLVList();
            listaCampo55.addTLV(EmvTags.EMV_TAG_TM_AUTHAMNTN,BytesUtil2.hexString2Bytes(EMV.getInstance().getTLV(EmvTags.EMV_TAG_TM_AUTHAMNTN)));
            listaCampo55.addTLV(EmvTags.EMV_TAG_TM_OTHERAMNTN,BytesUtil2.hexString2Bytes(EMV.getInstance().getTLV(EmvTags.EMV_TAG_TM_OTHERAMNTN)));
            listaCampo55.addTLV(EmvTags.EMV_TAG_TM_CNTRYCODE,BytesUtil2.hexString2Bytes(EMV.getInstance().getTLV(EmvTags.EMV_TAG_TM_CNTRYCODE)));
            listaCampo55.addTLV(EmvTags.EMV_TAG_TM_TVR,BytesUtil2.hexString2Bytes(EMV.getInstance().getTLV(EmvTags.EMV_TAG_TM_TVR)));
            listaCampo55.addTLV(EmvTags.EMV_TAG_TM_CURCODE,BytesUtil2.hexString2Bytes(EMV.getInstance().getTLV(EmvTags.EMV_TAG_TM_CURCODE)));
            listaCampo55.addTLV(EmvTags.EMV_TAG_TM_TRANSDATE,BytesUtil2.hexString2Bytes(EMV.getInstance().getTLV(EmvTags.EMV_TAG_TM_TRANSDATE)));
            listaCampo55.addTLV(EmvTags.EMV_TAG_TM_TRANSTYPE,BytesUtil2.hexString2Bytes(EMV.getInstance().getTLV(EmvTags.EMV_TAG_TM_TRANSTYPE)));
            listaCampo55.addTLV(EmvTags.EMV_TAG_TM_UNPNUM,BytesUtil2.hexString2Bytes(EMV.getInstance().getTLV(EmvTags.EMV_TAG_TM_UNPNUM)));
            listaCampo55.addTLV(EmvTags.EMV_TAG_TM_TERMTYPE,BytesUtil2.hexString2Bytes(EMV.getInstance().getTLV(EmvTags.EMV_TAG_TM_TERMTYPE)));
            listaCampo55.addTLV(EmvTags.EMV_TAG_IC_CID,BytesUtil2.hexString2Bytes(EMV.getInstance().getTLV(EmvTags.EMV_TAG_IC_CID)));
            listaCampo55.addTLV(EmvTags.EMV_TAG_IC_AC,BytesUtil2.hexString2Bytes(EMV.getInstance().getTLV(EmvTags.EMV_TAG_IC_AC)));
            listaCampo55.addTLV(EmvTags.EMV_TAG_IC_AIP,BytesUtil2.hexString2Bytes(EMV.getInstance().getTLV(EmvTags.EMV_TAG_IC_AIP)));
            listaCampo55.addTLV(EmvTags.EMV_TAG_IC_ATC,BytesUtil2.hexString2Bytes(EMV.getInstance().getTLV(EmvTags.EMV_TAG_IC_ATC)));
            listaCampo55.addTLV(EmvTags.EMV_TAG_IC_ISSAPPDATA,BytesUtil2.hexString2Bytes(EMV.getInstance().getTLV(EmvTags.EMV_TAG_IC_ISSAPPDATA)));
            listaCampo55.addTLV(EmvTags.EMV_TAG_TM_CVMRESULT,BytesUtil2.hexString2Bytes(EMV.getInstance().getTLV(EmvTags.EMV_TAG_TM_CVMRESULT)));
            listaCampo55.addTLV(EmvTags.EMV_TAG_IC_PANSN,BytesUtil2.hexString2Bytes(EMV.getInstance().getTLV(EmvTags.EMV_TAG_IC_PANSN)));
            listaCampo55.addTLV(EmvTags.EMV_TAG_TM_CAP,BytesUtil2.hexString2Bytes(EMV.getInstance().getTLV(EmvTags.EMV_TAG_TM_CAP)));
            listaCampo55.addTLV(EmvTags.EMV_TAG_TM_IFDSN,BytesUtil2.hexString2Bytes(EMV.getInstance().getTLV(EmvTags.EMV_TAG_TM_IFDSN)));
            listaCampo55.addTLV(EmvTags.EMV_TAG_TM_APPVERNO,BytesUtil2.hexString2Bytes(EMV.getInstance().getTLV(EmvTags.EMV_TAG_TM_APPVERNO)));
            listaCampo55.addTLV(EmvTags.EMV_TAG_TM_TRSEQCNTR,BytesUtil2.hexString2Bytes(EMV.getInstance().getTLV(EmvTags.EMV_TAG_TM_TRSEQCNTR)));*/
            //TODO llenar campos
            /*datosTarjeta.setIsChip(true);
            datosTarjeta.setTagVenta(listaCampo55.toString());
            String rawtrack2 = EMV.getInstance().getTLV(EmvTags.EMV_TAG_IC_TRACK2DATA);
            String [] rawSeparator = rawtrack2.split("[=D]");
            String track2 = rawSeparator[0]+rawtrack2.charAt(rawSeparator[0].length())+"2"+rawSeparator[1];
            datosTarjeta.setTrack2(track2);//57

            //Aqui va informacion adicional///
            que tenga comportamiento parecido al del PIN en onconfirm
            se debe manetener en este proceso si sin ir ni a pantalla de pin , ni al end process
            guarda los datos en datostarjeta.
            los cuatro ultimos se valida contra los ultimos 4 del pan de la  tarjeta. en datafono actualmente
            hace intentos.
             */
            EMVTag emvTag = new EMVTag() {
                @Override
                public int hashCode() {
                    return super.hashCode();
                }
            };
            datosTarjeta.setAplabel(uemvWrapper.emv.getTLV(emvTag.EMV_TAG_IC_APPLABEL));
            datosTarjeta.setPan(BytesUtil2.bytes2HexString(cardRecord.getPan()));
            Log.d(TAG, "PAN:" + datosTarjeta.getPan());
            Log.d(TAG, "Aid:" + datosTarjeta.getAid());
            Log.d(TAG, "Label:" + datosTarjeta.getAplabel());
            if (!soloTrack2) {
                emvCallback.get().procesarDatosAdicionalesCallback(datosTarjeta);
                try {
                    waitTipoCuentaInputThreat.waitForRslt();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            //hasta que no se halla realizado el proceso el no debe continuar///
            uemvWrapper.emv.responseEvent(null);
        }

        @Override
        public void onCardHolderVerify(CVMMethod cvmMethod) throws RemoteException {
            int pinOK = 0;
            Log.d(TAG, "----- onCardHolderVerify -----");
            Log.d(TAG, "CVM:" + cvmMethod.getCVM());
            Log.d(TAG, "PINTimes:" + cvmMethod.getPINTimes());
            EMVTag emvTag = new EMVTag() {
                @Override
                public int hashCode() {
                    return super.hashCode();
                }
            };
            switch (cvmMethod.getCVM()) {
                case CVMFlag.EMV_CVMFLAG_OFFLINEPIN:
                    //inputOfflinePin(cvmMethod.getPINTimes());
                    break;

                case CVMFlag.EMV_CVMFLAG_ONLINEPIN:
                    Log.d(TAG, "inputOnlinePin");
                    //initPinpad();
                    if(transaccionContinua){
                        inputOnlinePin(uemvWrapper.emv.getTLV(emvTag.EMV_TAG_IC_PAN));
                    } else {
                        stopProcess();
                    }

                    //}

                    break;
                case CVMFlag.EMV_CVMFLAG_SIGNATURE:
                    datosTarjeta.setIsPinAsked(0);
                    break;
            }
            // Response
            //TLVList tlvList = new TLVList();
            //tlvList.addTLV(TLV.fromData(com.usdk.apiservice.aidl.emv.EMVTag.DEF_TAG_CHV_STATUS, BytesUtil2.hexString2Bytes("01")));
            //if (pinOK == 1) {
            //uemvWrapper.emv.responseEvent(tlvList.toString());
            //}
        }

        @Override
        public void onOnlineProcess(TransData transData) throws RemoteException {
            Log.d(TAG, "----- onOnlineProcess -----");

            // Response
            TLVList tlvList = new TLVList();
            tlvList.addTLV(TLV.fromData(com.usdk.apiservice.aidl.emv.EMVTag.DEF_TAG_ONLINE_STATUS, BytesUtil2.hexString2Bytes("00")));
            tlvList.addTLV(TLV.fromData(com.usdk.apiservice.aidl.emv.EMVTag.DEF_TAG_AUTHORIZE_FLAG, BytesUtil2.hexString2Bytes("01")));
            tlvList.addTLV(TLV.fromData(com.usdk.apiservice.aidl.emv.EMVTag.DEF_TAG_HOST_TLVDATA, transData.getTLVData()));
            tlvList.addTLV(TLV.fromData(com.usdk.apiservice.aidl.emv.EMVTag.EMV_TAG_TM_ARC, BytesUtil2.hexString2Bytes("3030")));
            tlvList.addTLV(TLV.fromData(EMVTag.EMV_TAG_TM_AUTHCODE, BytesUtil2.hexString2Bytes("000000000001")));
            tlvList.addTLV(TLV.fromData(com.usdk.apiservice.aidl.emv.EMVTag.EMV_TAG_TM_IFDSN, BytesUtil2.hexString2Bytes("12285688")));//serial del equipo

            isEmvProcess = false;
            Log.d(TAG, "Netcom: info adicional 1");
            //emvCallback.get().infoAdicional(datosTarjeta, tlvList.toString());
            EMVTag emvTag = new EMVTag() {
                @Override
                public int hashCode() {
                    return super.hashCode();
                }
            };
            TLVList listaCampo55 = new TLVList();
            listaCampo55.addTLV(emvTag.EMV_TAG_TM_AUTHAMNTN, BytesUtil2.hexString2Bytes(uemvWrapper.emv.getTLV(emvTag.EMV_TAG_TM_AUTHAMNTN)));
            listaCampo55.addTLV(emvTag.EMV_TAG_TM_OTHERAMNTN, BytesUtil2.hexString2Bytes(uemvWrapper.emv.getTLV(emvTag.EMV_TAG_TM_OTHERAMNTN)));
            listaCampo55.addTLV(emvTag.EMV_TAG_TM_CNTRYCODE, BytesUtil2.hexString2Bytes(uemvWrapper.emv.getTLV(emvTag.EMV_TAG_TM_CNTRYCODE)));
            listaCampo55.addTLV(emvTag.EMV_TAG_TM_TVR, BytesUtil2.hexString2Bytes(uemvWrapper.emv.getTLV(emvTag.EMV_TAG_TM_TVR)));
            listaCampo55.addTLV(emvTag.EMV_TAG_TM_CURCODE, BytesUtil2.hexString2Bytes(uemvWrapper.emv.getTLV(emvTag.EMV_TAG_TM_CURCODE)));
            listaCampo55.addTLV(emvTag.EMV_TAG_TM_TRANSDATE, BytesUtil2.hexString2Bytes(uemvWrapper.emv.getTLV(emvTag.EMV_TAG_TM_TRANSDATE)));
            listaCampo55.addTLV(emvTag.EMV_TAG_TM_TRANSTYPE, BytesUtil2.hexString2Bytes(uemvWrapper.emv.getTLV(emvTag.EMV_TAG_TM_TRANSTYPE)));
            listaCampo55.addTLV(emvTag.EMV_TAG_TM_UNPNUM, BytesUtil2.hexString2Bytes(uemvWrapper.emv.getTLV(emvTag.EMV_TAG_TM_UNPNUM)));
            listaCampo55.addTLV(emvTag.EMV_TAG_TM_TERMTYPE, BytesUtil2.hexString2Bytes(uemvWrapper.emv.getTLV(emvTag.EMV_TAG_TM_TERMTYPE)));
            listaCampo55.addTLV(emvTag.EMV_TAG_IC_CID, BytesUtil2.hexString2Bytes(uemvWrapper.emv.getTLV(emvTag.EMV_TAG_IC_CID)));
            listaCampo55.addTLV(emvTag.EMV_TAG_IC_AC, BytesUtil2.hexString2Bytes(uemvWrapper.emv.getTLV(emvTag.EMV_TAG_IC_AC)));

            listaCampo55.addTLV(emvTag.EMV_TAG_IC_AIP, BytesUtil2.hexString2Bytes(uemvWrapper.emv.getTLV(emvTag.EMV_TAG_IC_AIP)));
            listaCampo55.addTLV(emvTag.EMV_TAG_IC_ATC, BytesUtil2.hexString2Bytes(uemvWrapper.emv.getTLV(emvTag.EMV_TAG_IC_ATC)));
            listaCampo55.addTLV(emvTag.EMV_TAG_IC_ISSAPPDATA, BytesUtil2.hexString2Bytes(uemvWrapper.emv.getTLV(emvTag.EMV_TAG_IC_ISSAPPDATA)));
            listaCampo55.addTLV(emvTag.EMV_TAG_TM_CVMRESULT, BytesUtil2.hexString2Bytes(uemvWrapper.emv.getTLV(emvTag.EMV_TAG_TM_CVMRESULT)));
            listaCampo55.addTLV(emvTag.EMV_TAG_IC_PANSN, BytesUtil2.hexString2Bytes(uemvWrapper.emv.getTLV(emvTag.EMV_TAG_IC_PANSN)));
            listaCampo55.addTLV(emvTag.EMV_TAG_TM_CAP, BytesUtil2.hexString2Bytes(uemvWrapper.emv.getTLV(emvTag.EMV_TAG_TM_CAP)));
            listaCampo55.addTLV(emvTag.EMV_TAG_TM_IFDSN, BytesUtil2.hexString2Bytes(uemvWrapper.emv.getTLV(emvTag.EMV_TAG_TM_IFDSN)));
            listaCampo55.addTLV(emvTag.EMV_TAG_TM_APPVERNO, BytesUtil2.hexString2Bytes(uemvWrapper.emv.getTLV(emvTag.EMV_TAG_TM_APPVERNO)));
            listaCampo55.addTLV(emvTag.EMV_TAG_TM_TRSEQCNTR, BytesUtil2.hexString2Bytes(uemvWrapper.emv.getTLV(emvTag.EMV_TAG_TM_TRSEQCNTR)));
            datosTarjeta.setIsChip(true);
            datosTarjeta.setTagVenta(listaCampo55.toString());
            String rawtrack2 = uemvWrapper.emv.getTLV(emvTag.EMV_TAG_IC_TRACK2DATA);
            String[] rawSeparator = rawtrack2.split("[=D]");
            String track2 = rawSeparator[0] + rawtrack2.charAt(rawSeparator[0].length()) + rawSeparator[1];
            datosTarjeta.setTrack2(track2.substring(0,track2.length() - 1));//57

            //emvCallback.get().infoAdicional();
            datosTarjeta.setArqc(uemvWrapper.emv.getTLV(emvTag.EMV_TAG_IC_AC));
            datosTarjeta.setPan(uemvWrapper.emv.getTLV(emvTag.EMV_TAG_IC_PAN));
            datosTarjeta.setFechaExpiracion(uemvWrapper.emv.getTLV(emvTag.EMV_TAG_IC_APPEXPIREDATE));
            datosTarjeta.setTamTagVenta(datosTarjeta.getTagVenta().length());
            Log.i(TAG, "Netcom EMV ");

            //uemvWrapper.emv.responseEvent(tlvList.toString());
            emvCallback.get().procesarTarjetaCallback(datosTarjeta);
            uemvWrapper.emv.responseEvent(tlvList.toString());
        }

        @Override
        public void onEndProcess(int resultCode, TransData transData) throws RemoteException {
            Log.d(TAG, "----- onEndProcess -----");
            Log.d(TAG, "resultCode:" + resultCode);

            // Hide dialoges
            isEmvProcess = false;

            // Show result message
            if (resultMessage != null && !resultMessage.isEmpty()) {
                if (emvCallback.get() != null) {
                    // emvCallback.get().showToast(resultMessage);
                } else {
                    // callback.modifyView(EMVActivity.TEXT_VIEW_CODE, resultMessage);
                }
                return;
            }

            // Check process result
            if (resultCode != EMVError.SUCCESS) {
                //Toast.makeText(context, context.getString(EMV.getErrorId(resultCode)), Toast.LENGTH_SHORT).show();
                if (emvCallback.get() != null) {
                    //emvCallback.get().showToast(context.get().getString(EMV.getErrorId(resultCode)));
                } else {
                    //callback.showToast(context.get().getString(EMV.getErrorId(resultCode)));
                }
                return;
            }


            // Show transaction message
            switch (transData.getACType()) {
                case ACType.EMV_ACTION_AAC:
                    //Toast.makeText(context, context.getString(R.string.transaction_declined), Toast.LENGTH_SHORT).show();
                    if (emvCallback.get() != null) {
                        //emvCallback.get().showToast(context.get().getString(R.string.transaction_declined));
                    } else {
                        //callback.showToast(context.get().getString(R.string.transaction_declined));
                    }
                    break;
                case ACType.EMV_ACTION_TC:
                    //Toast.makeText(context, context.getString(R.string.transaction_approved), Toast.LENGTH_SHORT).show();
                    if (emvCallback.get() != null) {
                        //emvCallback.get().showToast(context.get().getString(R.string.transaction_approved));
                    } else {
                        //callback.showToast(context.get().getString(R.string.transaction_approved));
                    }
                    break;
                case ACType.EMV_ACTION_ARQC:
                    //Toast.makeText(context, context.getString(R.string.request_online_authorization), Toast.LENGTH_SHORT).show();
                    if (emvCallback.get() != null) {
                        //emvCallback.get().showToast(context.get().getString(R.string.request_online_authorization));
                    } else {
                        //callback.showToast(context.get().getString(R.string.request_online_authorization));
                    }
                    break;
            }
            try {
                Log.d(TAG, "onEndProcess: Llamar inputOnlinePin");
                //inputOnlinePin();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        @Override
        public void onVerifyOfflinePin(int flag, byte[] random, CAPublicKey caPublicKey, OfflinePinVerifyResult offlinePinVerifyResult) throws RemoteException {
            Log.d(TAG, "----- onVerifyOfflinePin -----");
            Log.d(TAG, "flag : " + flag);

            // Response
            offlinePinVerifyResult.setSW(0x90, 0x00);
            //offlinePinVerifyResult.setResult(uemvWrapper.emv.VERIFY_OFFLINE_PIN_SUCCESS);
        }

        @Override
        public void onObtainData(int command, byte[] data) throws RemoteException {
            Log.d(TAG, "----- onObtainData -----");
            Log.d(TAG, command + " : " + BytesUtil2.bytes2HexString(data));
        }

        @Override
        public void onSendOut(int command, byte[] data) throws RemoteException {
            Log.d(TAG, "----- onSendOut -----");
            Log.d(TAG, command + " : " + BytesUtil2.bytes2HexString(data));
        }
    };

    public void infoCompleta() throws RemoteException {
        uemvWrapper.emv.responseEvent(null);
    }

    public void stopProcess() throws RemoteException {
        if (!(isShowDOL || isShowTLV)) {
            uemvWrapper.emv.stopSearch();
            uemvWrapper.emv.halt();
        }

        if (isEmvProcess) {
            uemvWrapper.emv.stopProcess();
        }
    }

    private void searchCard() throws RemoteException {
        // Start searching card
        Bundle bundle = new Bundle();
        bundle.putBoolean("supportICCard", true);
        bundle.putBoolean("supportRFCard", true);
        bundle.putBoolean("supportMagCard", true);


        bundle.putString("rfDeviceName", RFDeviceName.INNER);

        uemvWrapper.emv.searchCard(bundle, 60, searchCardListener);
    }

    private SearchCardListener searchCardListener = new SearchCardListener.Stub() {
        @Override
        public void onCardSwiped(Bundle bundle) throws RemoteException {
            Log.d(TAG, "----- onCardSwiped -----");
            Log.d(TAG, "PAN : " + bundle.getString("PAN"));
            Log.d(TAG, "TRACK1 : " + bundle.getString("TRACK1"));
            Log.d(TAG, "TRACK2 : " + bundle.getString("TRACK2"));
            Log.d(TAG, "TRACK3 : " + bundle.getString("TRACK3"));
            Log.d(TAG, "SERVICE_CODE : " + bundle.getString("SERVICE_CODE"));
            Log.d(TAG, "EXPIRED_DATE : " + bundle.getString("EXPIRED_DATE"));

            // Stop EMV process
            resultMessage = "PAN : " + bundle.getString("PAN") + "\n";
            resultMessage += "TRACK1 : " + bundle.getString("TRACK1") + "\n";
            resultMessage += "TRACK2 : " + bundle.getString("TRACK2") + "\n";
            resultMessage += "TRACK3 : " + bundle.getString("TRACK3") + "\n";
            resultMessage += "SERVICE_CODE : " + bundle.getString("SERVICE_CODE") + "\n";
            resultMessage += "EXPIRED_DATE : " + bundle.getString("EXPIRED_DATE") + "\n";

            //generar DTO
            datosTarjeta.setTipoEntrada(1);
            datosTarjeta.setBanda(true);
            datosTarjeta.setTrack1(bundle.getString("TRACK1"));
            datosTarjeta.setTrack2(bundle.getString("TRACK2"));
            datosTarjeta.setTrack2(datosTarjeta.getTrack2().replace("=","D"));
            datosTarjeta.setTrack3(bundle.getString("TRACK3"));
            datosTarjeta.setFechaExpiracion(bundle.getString("EXPIRED_DATE"));
            datosTarjeta.setChip(false);
            datosTarjeta.setIsBanda(true);
            datosTarjeta.setIsChip(false);
            datosTarjeta.setPan(bundle.getString("PAN"));
            stopProcess();
            if (!soloTrack2 && !bundle.getString("SERVICE_CODE").startsWith("201")) {
                isEmvProcess = true;
                emvCallback.get().procesarDatosAdicionalesCallback(datosTarjeta);
                try {
                    waitTipoCuentaInputThreat.waitForRslt();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                if (transaccionContinua) {
                    inputOnlinePin(datosTarjeta.getPan());
                    try {
                        waitTipoCuentaInputThreat.waitForRslt();
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
                if(transaccionContinua){
                    emvCallback.get().procesarTarjetaCallback(datosTarjeta);
                }

            } else {
                emvCallback.get().onErrorMagneticCard(1);
            }
        }

        @Override
        public void onCardInsert() throws RemoteException {
            Log.d(TAG, "----- onCardInsert -----");

            uemvWrapper.emv.respondCard();
        }

        @Override
        public void onCardPass(int cardType) throws RemoteException {
            Log.d(TAG, "----- onCardPass -----");
            Log.d(TAG, "cardType: " + cardType);

            uemvWrapper.emv.respondCard();
        }

        @Override
        public void onTimeout() throws RemoteException {
            Log.d(TAG, "----- onTimeout -----");

            // Stop EMV process
            resultMessage = context.get().getString(R.string.wait_card_timeout);
            stopProcess();
        }

        @Override
        public void onError(int error, String message) throws RemoteException {
            Log.d(TAG, "----- onError -----");
            Log.d(TAG, error + " : " + message);
            // Stop EMV process
            //resultMessage = context.get().getString(uemvWrapper.emv.getErrorId(error));

            uemvWrapper.emv.stopProcess();
            stopProcess();
            emvCallback.get().onErrorMagneticCard(0);

        }
    };

    private void researchRFCard() throws RemoteException {
        // Start searching RF card
        Bundle bundle = new Bundle();
        bundle.putBoolean("supportICCard", false);
        bundle.putBoolean("supportRFCard", true);
        bundle.putBoolean("supportMagCard", false);
        //if (DeviceManager.MODEL_AECR_C10.equals(DeviceManager.getInstance().getModel())) {
        //  bundle.putString("rfDeviceName", RFDeviceName.EXTERNAL);
        //} else {
        bundle.putString("rfDeviceName", RFDeviceName.INNER);
        //}
        uemvWrapper.emv.searchCard(bundle, 60, searchCardListener);
    }

    private void getBalance() throws RemoteException {
        isShowBalance = true;
        startProcess();
    }

    private void getDOL() throws RemoteException {
        isShowDOL = true;
        startProcess();
    }

    private void getTLV() throws RemoteException {
        isShowTLV = true;
        startProcess();
    }

    private void getDataAPDU() throws RemoteException {
        isShowDataAPDU = true;
        startProcess();
    }

    private void getICCLog() throws RemoteException {
        isGetICCLogProcess = true;
        startProcess();
    }

    private void getECCLog() throws RemoteException {
        isGetECCLogProcess = true;
        startProcess();
    }


    void solicitarDatos() {
        Log.d(TAG, "Netcom solicitarDatos ");
        //emvCallback.get().infoAdicional(datosTarjeta, null);
    }

    private DialogInterface.OnDismissListener onDismissListener = dialog -> {
        try {
            stopProcess();
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    };


    public void inputOnlinePin(String pan) throws RemoteException {
//        Beeper.getInstance().startBeep(500);
        onInputPin("POR FAVOR INGRESE  PIN");

        Bundle param = new Bundle();
        param.putByteArray("panBlock", BytesUtil2.hexString2Bytes(pan));
        param.putInt("timeout", 300);
        param.putByteArray("pinLimit", new byte[]{0, 4, 5, 6, 7, 8, 9, 10, 11, 12});
//        param.putByte("pinBlockFormat", ...);
//        param.putInt("betweenPinKeyTimeout", ...);

        PinpadForMKSK.getInstance().open();
        PinpadForMKSK.getInstance().startPinEntry(KeyId.mainKey, param, new OnPinEntryListener.Stub() {

            @Override
            public void onInput(int len, int key) throws RemoteException {
                Log.d(TAG, "----- onInput -----");
                Log.d(TAG, "len:" + len + ", key:" + key);

//                Beeper.getInstance().startBeep(100);
            }

            @Override
            public void onError(int error) throws RemoteException {
                Log.d(TAG, "----- onError -----");
                Log.d(TAG, "error:" + error);
                stopProcess();
                emvCallback.get().finalizarProcesoTimeOutStop();
            }

            @Override
            public void onConfirm(byte[] data, boolean isNonePin) throws RemoteException {
                Log.d(TAG, "----- onConfirm -----");
                Log.d(TAG, "isNonePin:" + isNonePin + "\r\n");
                /*try {
                    datosTarjeta.setIsPinAsked(1);
                    datosTarjeta.setPinBlock(BytesUtil2.fromBytes(data,"GBK") );
                } catch (UnsupportedEncodingException e) {
                    e.printStackTrace();
                }
                if (datosTarjeta.getIsChip() == false) {
                    Log.d(TAG, "Finalizó por banda  ");
                    Message tipoCuentaFinalizo = new Message();
                    tipoCuentaFinalizo.what = Const.DATOS_ADICIONALES_FINISH;
                    EMVHandler.getTipoCuentaEventHandler().sendMessage(tipoCuentaFinalizo);
                } else {
                    responderEMV();
                }*/
            }

            @Override
            public void onCancel() throws RemoteException {
                Log.d(TAG, "----- onCancel -----");
                stopProcess();
                emvCallback.get().finalizarProcesoTimeOutStop();
            }
        });

    }

    private void responderEMV() {
        TLVList tlvList = new TLVList();
        tlvList.addTLV(TLV.fromData(com.usdk.apiservice.aidl.emv.EMVTag.DEF_TAG_CHV_STATUS, BytesUtil2.hexString2Bytes("01")));
        //if (pinOK == 1) {
        try {
            uemvWrapper.emv.responseEvent(tlvList.toString());
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }

    private void onInputPin(String s) {

    }

    /**
     * Turn on blue and red lights.
     */
 /**   private void turnOnBlueAndRedLights() throws RemoteException {
        LED.getInstance().turnOn(Light.BLUE, Light.RED);
    }

    /**
     * Operate blue light.
     */
/**    private void operateBlueLight() throws RemoteException {
        LED.getInstance().operateBlueLight();
    }

    /**
     * Operate yellow light.
     */
 /**   private void operateYellowLight() throws RemoteException {
        LED.getInstance().operateYellowLight();
    }

    /**
     * Operate green light.
     */
  /**  private void operateGreenLight() throws RemoteException {
        LED.getInstance().operateGreenLight();
    }

    /**
     * Operate red light.
     */
  /*  private void operateRedLight() throws RemoteException {
        LED.getInstance().operateRedLight();
    }

    /**
     * Turn off all.
     */
 /*   private void turnOffAll() throws RemoteException {
        LED.getInstance().turnOffAll();
    }

    /**
     * Turn on all.
     */
   /* private void turnOnAll() throws RemoteException {
        LED.getInstance().turnOnAll();
    }
*/

}
