package co.com.netcom.corresponsal.pantallas.funciones;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothManager;
import android.content.Context;
import android.widget.Toast;


import java.util.ArrayList;
import java.util.List;

import com.middleware.manager.NLPosManager;
import com.newland.depend.PosManagerDelegate;
import com.newland.depend.entity.CardInfoEntity;
import com.newland.depend.entity.DeviceInformation;
import com.newland.depend.enums.CARD_STATE;
import com.newland.mtype.module.common.emv.AIDConfig;



public class MetodosSDKNewland {

    private Context context;
    private static final int TIMEOUT = 3000;
    private NLPosManager nlPosManager;
    private BluetoothAdapter bluetoothAdapter;
    private BluetoothManager bluetoothManager;
    private PosManagerDelegate swiperCardChangedListener = new PosManagerDelegate() {


        @Override
        public void onScannerResult(BluetoothDevice devInfo) {
            //appendString("onScannerResult, name = " + devInfo.getName() + ", address = " + devInfo.getAddress());
        }

        @Override
        public void discoverComplete(ArrayList<BluetoothDevice> devInfoList) {
            //appendString("discoverComplete, "+ devInfoList);
        }

        @Override
        public void onConnectSucc() {
            //appendString("onConnectSucc");
        }

        @Override
        public void onConnectTimeOut() {
            //appendString("onConnectTimeOut");
        }

        @Override
        public void onDeviceDisconnect() {
            //appendString("onDeviceDisconnect");
        }

        @Override
        public void onGetDeviceInfo(DeviceInformation deviceInfo) {
            //appendString("onGetDeviceInfo, sn = "+deviceInfo.getSn());
        }

        @Override
        public void onGetDateTimeSucc(String deviceTime) {
            //appendString("onGetDateTimeSucc, deviceTime = "+deviceTime);
        }

        @Override
        public void onSetDateTimeSucc() {
            //appendString("onSetDateTimeSucc");
        }

        @Override
        public void onSetParamSucc() {
            //appendString("onSetParamSucc");
        }

        @Override
        public void onGetParamSucc(byte[] value) {
            // appendString("onGetParamSucc value = "+ Dump.getHexDump(value));
        }

        @Override
        public void onLoadMainKeySucc() {
            //appendString("onLoadMainKeySucc");
        }

        @Override
        public void onLoadWorkingKeySucc() {

            // appendString("onLoadWorkingKeySucc");
        }

        @Override
        public void onLoadIPEKSucc() {

            //  appendString("onLoadIPEKSucc");
        }

        @Override
        public void onAddAIDSucc() {

            //  appendString("onAddAIDSucc");
        }

        @Override
        public void onAddCAPKSucc() {

            // appendString("onAddCAPKSucc");
        }

        @Override
        public void onGetAIDListSucc(List<AIDConfig> aidconfigLst) {
            //appendString("onGetAIDListSucc ");
            // for(AIDConfig aidInfo : aidconfigLst){
            // appendString("9f06 = "+ Dump.getHexDump(aidInfo.getAid()));
            //}
        }

        @Override
        public void onClearAIDsSucc() {

            //appendString("onClearAIDsSucc");
        }

        @Override
        public void onClearCAPKSucc()
        {
            //appendString("onClearCAPKSucc");
        }

        @Override
        public void onShowMessageSucc()
        {
            //appendString("onShowMessageSucc");
        }

        @Override
        public void onClearScreenSucc() {


            // appendString("onClearScreenSucc");
        }

        @Override
        public void onCallBuzzerSucc() {

            //  appendString("onCallBuzzerSucc");
        }

        @Override
        public void onOperateLightSucc() {

            // appendString("onOperateLightSucc");
        }

        @Override
        public void onCancelTradeSucc() {
            //appendString("onCancelTradeSucc");
        }

        @Override
        public void requestSelectApplication(byte[] aidList) {
            //appendString("requestSelectApplication = " + Dump.getHexDump(aidList));
            //addSelectApplication
        }

        @Override
        public void onReadCardSucc(CardInfoEntity cardInfoEntity) {
           /* appendString("onReadCardSucc = " + cardInfoEntity.getCardNo());
            appendString("ksn = " + Dump.getHexDump(cardInfoEntity.getKsn()));

            if (cardInfoEntity.getPinKsn() != null) {
                appendString("PinKsn = " + Dump.getHexDump(cardInfoEntity.getPinKsn()));
            }

            appendString("StrTrack1 = " + Dump.getHexDump(cardInfoEntity.getStrTrack1()));
            appendString("StrTrack2 = " + cardInfoEntity.getStrTrack2());
            appendString("StrTrack2 = " + Dump.getHexDump(cardInfoEntity.getStrTrack2()));
            appendString("StrTrack3 = " + Dump.getHexDump(cardInfoEntity.getStrTrack3()));
            appendString("Card No= " + cardInfoEntity.getCardNo());
            appendString("Pan Sequence = " + cardInfoEntity.getPanSeqNo());
            appendString("Card type = " + cardInfoEntity.getCardType());
            appendString("card Expiry Date = " + cardInfoEntity.getExpiryDate());
            appendString("iccData = " + Dump.getHexDump(cardInfoEntity.getIccDataPlain()));
            appendString("Encrypted = " + Dump.getHexDump(cardInfoEntity.getIccDataEncrypt()));
            appendString("pinblock = " + Dump.getHexDump(cardInfoEntity.getPinBlock()));

            nlPosManager.calculateMAC(PinManageType.DUKPT, DEFAULT_MAC_WK_INDEX, ISOUtils.hex2byte(DataforMac), MacAlgorithm.MAC_X919);
            */

        }

        @Override
        public void onInputPinCompleted(byte[] pinBlock, byte[] ksn) {
            //   appendString("onInputPinCompleted, pinBlock = " + Dump.getHexDump(pinBlock) +", ksn = "+ Dump.getHexDump(ksn));
        }

        @Override
        public void onGetMACSucc(byte[] macData, byte[] ksn) {
            //  appendString("onGetMACSucc = " + Dump.getHexDump(macData) + ", ksn = "+ Dump.getHexDump(ksn));
        }

        @Override
        public void onUpdateFirmwareProcess(float process) {
            // appendString("onUpdateFirmwareProcess = " + process);
        }

        @Override
        public void onUpdateFirmwareSucc() {

            // appendString("onUpdateFirmwareSucc ");
        }

        @Override
        public void onSetTerminalParamsSucc() {

            //  appendString("onSetTerminalParamsSucc ");
        }

        @Override
        public void onReceiveErrorCode(int errCode, String errInfo) {
            // appendString("onReceiveErrorCode, errCode = " + errCode +", errInfo =" +errInfo);
            //  if(errCode == ErrorCode.ERR_BTDISCONNECTED){
//				new Thread(new Runnable() {
//					@Override
//					public void run() {
//						nlPosManager.connectDeviceByBluetooth(60, testAddress);
//					}
//				}).start();
            //  }
        }

        @Override
        public void onIncreaseKsnSucc() {
            //   appendString("onIncreaseKsnSucc ");
        }

        @Override
        public void onCheckCardStateSucc(CARD_STATE card_state) {
            //  appendString("onCheckCardStateSucc = " + card_state.toString());
        }

        @Override
        public void onSetLanguageTypeSucc() {

        }

        @Override
        public void onLoadpublickeySucc() {

        }

        @Override
        public void onGetTransportSessionKey(String s, String s1) {

        }

        @Override
        public void onSecondIssuance(int excuteRslt, byte[] iccData) {
            // appendString("onSecondIssuance = "+ excuteRslt + ",iccData = " + Dump.getHexDump(iccData));
        }

        public void onGetTransportSessionKey(String paramString){

            // appendString("onGetTransportSessionKey = "+ paramString);
        }
    };


    public MetodosSDKNewland(Context context1){
        this.context = context1;

        bluetoothManager = (BluetoothManager) context.getSystemService(Context.BLUETOOTH_SERVICE);
        bluetoothAdapter = bluetoothManager.getAdapter();

        nlPosManager= (NLPosManager) new NLPosManager().sharedInstance(context, swiperCardChangedListener);
    }



    public void scanDevice() {
        if (bluetoothAdapter.isEnabled()) {

            nlPosManager.scanBlueDevice(TIMEOUT);
            Toast.makeText(context,"Buscando dispositivos",Toast.LENGTH_SHORT).show();
        } else {
            Toast.makeText(context,"Active Bluetooth",Toast.LENGTH_SHORT).show();

        }
    }

}
