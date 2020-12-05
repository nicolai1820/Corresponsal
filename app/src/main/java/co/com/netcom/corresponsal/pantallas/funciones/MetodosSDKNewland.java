package co.com.netcom.corresponsal.pantallas.funciones;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothManager;
import android.content.Context;
import android.graphics.drawable.ColorDrawable;
import android.os.Message;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.Toast;


import androidx.core.app.ActivityCompat;
import androidx.fragment.app.Fragment;

import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.List;

import com.middleware.manager.NLPosManager;
import com.newland.depend.PosManagerDelegate;
import com.newland.depend.entity.CardInfoEntity;
import com.newland.depend.entity.DeviceInformation;
import com.newland.depend.entity.ReadCardInfo;
import com.newland.depend.enums.CARD_STATE;
import com.newland.depend.enums.DataEncryptTypeEnum;
import com.newland.mtype.module.common.cardreader.OpenCardType;
import com.newland.mtype.module.common.emv.AIDConfig;
import com.newland.mtype.module.common.pin.MacAlgorithm;
import com.newland.mtype.module.common.pin.PinManageType;
import com.newland.mtype.util.Dump;
import com.newland.mtype.util.ISOUtils;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.core.comunicacion.CardDTO;
import co.com.netcom.corresponsal.pantallas.comunes.popUp.PopUp;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.inicio.pantallaAjustesUsuarioComun;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.retiro.conTarjeta.PantallaRetiroConTarjetaLoader;


public class MetodosSDKNewland {

    private Context context;
    private static final int TIMEOUT = 5000;
    private NLPosManager nlPosManager;
    private BluetoothAdapter bluetoothAdapter;
    private BluetoothManager bluetoothManager;
    private ArrayList<String> devices = new ArrayList<>();;
    private CardDTO datosTarjeta;
    private static List<Integer> cipherTag;
    private static List<Integer> plainTag;


    public static final int DEFAULT_MK_INDEX = 1;
    public static final int DEFAULT_TLK_INDEX = 10;
    public static final int DEFAULT_PIN_WK_INDEX = 2;
    public static final int DEFAULT_MAC_WK_INDEX = 2;
    public static final int DEFAULT_DATA_WK_INDEX = 2;
    public static final int DEFAULT_DATA_KSN_INDEX = 2;

    String DataforMac = "313330343832333030313031303043303038303431303037383631393131303731323337323818FFFF18159753E000013630398000000000";


    private static void sdkInit() {
        plainTag = new ArrayList<Integer>();
        plainTag.add(0x82);
        plainTag.add(0x9F02);
        plainTag.add(0x9F03);
        plainTag.add(0x50);
        plainTag.add(0x9F12);
        plainTag.add(0x9F36);
        plainTag.add(0x9F26);
        plainTag.add(0x9F27);//check 9F27 bite8-7 : 00 - AAC ; 01 - TC ; 10 - ARQC
        plainTag.add(0x9F34);
        plainTag.add(0x84);
        plainTag.add(0x9F10);
        plainTag.add(0x9F1E);
        plainTag.add(0x9F35);
        plainTag.add(0x95);
        plainTag.add(0x9F1A);
        plainTag.add(0x5F2A);
        plainTag.add(0x9A);
        plainTag.add(0x9F37);
        plainTag.add(0x9C);
        plainTag.add(0x8A);
        plainTag.add(0x9F19);
        plainTag.add(0x9F6C);
        plainTag.add(0x9F66);
        plainTag.add(0x57);  //This is for Track2 data
        plainTag.add(0x4F);
		/*plainTag.add(0x5A);
		plainTag.add(0x9F24);
		plainTag.add(0x9F33);
		plainTag.add(0x9F6E);
		plainTag.add(0x5F20);
		plainTag.add(0x5F24);
		plainTag.add(0x9F16);
		plainTag.add(0x9F21);
		plainTag.add(0x9F06);
		plainTag.add(0x5F30);
		plainTag.add(0x9F4E);
		plainTag.add(0x9B);
		plainTag.add(0x9F15);*/
        //DF75 to know the final result aftr second gen AC





        cipherTag = new ArrayList<Integer>();
        cipherTag.add(0x5F20);
        cipherTag.add(0x4f);
        cipherTag.add(0x5F24);
        cipherTag.add(0x9F16);
        cipherTag.add(0x9F21);
        cipherTag.add(0x9A);
        cipherTag.add(0x9F02);
        cipherTag.add(0x9F03);
        cipherTag.add(0x9F34);
        cipherTag.add(0x9F12);
        cipherTag.add(0x9F06);
        cipherTag.add(0x5F30);
        cipherTag.add(0x9F4E);
        cipherTag.add(0x5A);
        cipherTag.add(0x57);
        cipherTag.add(0x9F10);
        cipherTag.add(0x82);
        cipherTag.add(0x8E);
        cipherTag.add(0x5F25);
        cipherTag.add(0x9F07);
        cipherTag.add(0x9F0D);
        cipherTag.add(0x9F0E);
        cipherTag.add(0x9F0F);
        cipherTag.add(0x9F26);
        cipherTag.add(0x9F27);
        cipherTag.add(0x9F39);
        cipherTag.add(0x9C);
        cipherTag.add(0x9F33);
        cipherTag.add(0x9F37);
        cipherTag.add(0x9F36);
        cipherTag.add(0x9F40);
        cipherTag.add(0x95);
        cipherTag.add(0x9B);
        cipherTag.add(0x84);
        cipherTag.add(0x5F2A);
        cipherTag.add(0x5F34);
        cipherTag.add(0x9F09);
        cipherTag.add(0x9F1A);
        cipherTag.add(0x9F1E);
        cipherTag.add(0x9F35);
        cipherTag.add(0x5F28);
        cipherTag.add(0x50);
        cipherTag.add(0x9F08);
        cipherTag.add(0x8A);
        cipherTag.add(0x9F4C);
    }

    private PosManagerDelegate swiperCardChangedListener = new PosManagerDelegate() {


        @Override
        public void onScannerResult(BluetoothDevice devInfo) {

            String sSubCadena = devInfo.getAddress().substring(0,5);

            if (sSubCadena.equals("08:00")){
                devices.add(devInfo.getName() + "\n" + devInfo.getAddress());
                Log.d("AGREGUE",devInfo.getAddress());

            }

        }

        @Override
        public void discoverComplete(ArrayList<BluetoothDevice> devInfoList) {
            //appendString("discoverComplete, "+ devInfoList);

            Log.d("LISTA", devices.toString());
            setDevices(devices);
            Message apagarLeds = new Message();
            apagarLeds.what = 3;
            pantallaAjustesUsuarioComun.procesoTransacion.sendMessage(apagarLeds);
        }

        @Override
        public void onConnectSucc() {
            Message conexionExitosa= new Message();
            conexionExitosa.what = 1;
            pantallaAjustesUsuarioComun.procesoTransacion.sendMessage(conexionExitosa);

            /*Message estadoActivo = new Message();
            estadoActivo.what = 2;
            LogoComercio.estado.sendMessage(estadoActivo);*/
        }

        @Override
        public void onConnectTimeOut() {
            Message conexionFallida = new Message();
            conexionFallida.what = 2;
            pantallaAjustesUsuarioComun.procesoTransacion.sendMessage(conexionFallida);        }

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



           Log.d("TRACK 1",Dump.getHexDump(cardInfoEntity.getStrTrack1()));
           Log.d("TRACK 2",Dump.getHexDump(cardInfoEntity.getStrTrack2()));
           Log.d("TRACK 3",Dump.getHexDump(cardInfoEntity.getStrTrack3()));
           Log.d("NUMERO TARJETA",cardInfoEntity.getCardNo());
           Log.d("PAN",cardInfoEntity.getPanSeqNo());
           Log.d("FECHA",cardInfoEntity.getExpiryDate());




            datosTarjeta.setTrack1(Dump.getHexDump(cardInfoEntity.getStrTrack1()));
            datosTarjeta.setTrack2(Dump.getHexDump(cardInfoEntity.getStrTrack2()));
            datosTarjeta.setTrack3(Dump.getHexDump(cardInfoEntity.getStrTrack3()));
            datosTarjeta.setDocumento(cardInfoEntity.getCardNo());
            datosTarjeta.setPan(cardInfoEntity.getPanSeqNo());
            datosTarjeta.setFechaExpiracion(cardInfoEntity.getExpiryDate());

            Message respuestaLectura = new Message();
            respuestaLectura.what = 1;
            PantallaRetiroConTarjetaLoader.respuesta.sendMessage(respuestaLectura);

            nlPosManager.calculateMAC(PinManageType.DUKPT, DEFAULT_MAC_WK_INDEX, ISOUtils.hex2byte(DataforMac), MacAlgorithm.MAC_X919);


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

        sdkInit();

        datosTarjeta = new CardDTO();

        bluetoothManager = (BluetoothManager) context.getSystemService(Context.BLUETOOTH_SERVICE);
        bluetoothAdapter = bluetoothManager.getAdapter();

        nlPosManager= (NLPosManager) new NLPosManager().sharedInstance(context, swiperCardChangedListener);
    }



    public void scanDevice()  {
        if (bluetoothAdapter.isEnabled()) {
            //Devices.clear();
            nlPosManager.scanBlueDevice(TIMEOUT);
        } else {
            Toast.makeText(context,"Active Bluetooth",Toast.LENGTH_SHORT).show();

        }
    }


    public void connectDevice(int tiempo, String direccion){

        nlPosManager.connectDeviceByBluetooth(tiempo,direccion);

    }

    public void disconnectDevice(){

        nlPosManager.disconnectDevice();
    }

    public boolean isConnected(){

        return  nlPosManager.isConnected();
    }




    public ArrayList<String> getDevices(){

        return this.devices;
    }

    public void setDevices(ArrayList<String> lista ){
        this.devices = lista;
    }

    public CardDTO getDatosTarjeta(){

        return this.datosTarjeta;
    }

    public void readCard(){


       // if (cantidadActiva){
            ReadCardInfo paramsInfo = new ReadCardInfo();
            paramsInfo.setOpenCardType(new OpenCardType[] { OpenCardType.SWIPER, OpenCardType.ICCARD, OpenCardType.NCCARD });
            paramsInfo.setAmout(new BigDecimal("100.00"));
            paramsInfo.setCashback(new BigDecimal("0.00"));
            BigDecimal totalAmount = new BigDecimal("0.00");
            totalAmount =paramsInfo.getAmout().add(paramsInfo.getCashback());
            String strAmountDisplay = String.format("Total : %s\n  INSERT/SWIPE/WAVE",totalAmount);
            paramsInfo.setText(strAmountDisplay);
            paramsInfo.setCipherTag(cipherTag);
            paramsInfo.setPlainTag(plainTag);
            paramsInfo.setEFSupportAppsel(false);
            paramsInfo.setEncrypt(true);
            paramsInfo.setForceOnline(true);
            paramsInfo.setWkIndex(DEFAULT_DATA_WK_INDEX);
            paramsInfo.setPkIndex(DEFAULT_PIN_WK_INDEX);
            paramsInfo.setType(DataEncryptTypeEnum.DUKPT);
            paramsInfo.setTimeout(TIMEOUT);
            String formatDisplayContent = String.format("Total : %s\n      ENTER PIN", totalAmount);
            paramsInfo.setDisplayContent(formatDisplayContent);
            nlPosManager.startReadCard(paramsInfo);
      /*  }else{

            ReadCardInfo paramsInfo = new ReadCardInfo();
            paramsInfo.setOpenCardType(new OpenCardType[] { OpenCardType.SWIPER, OpenCardType.ICCARD, OpenCardType.NCCARD });
            String strAmountDisplay = String.format("Total : %s\n  INSERT/SWIPE/WAVE");
            paramsInfo.setText(strAmountDisplay);
            paramsInfo.setCipherTag(cipherTag);
            paramsInfo.setPlainTag(plainTag);
            paramsInfo.setEFSupportAppsel(false);
            paramsInfo.setEncrypt(true);
            paramsInfo.setForceOnline(true);
            paramsInfo.setWkIndex(DEFAULT_DATA_WK_INDEX);
            paramsInfo.setPkIndex(DEFAULT_PIN_WK_INDEX);
            paramsInfo.setType(DataEncryptTypeEnum.DUKPT);
            paramsInfo.setTimeout(TIMEOUT);
            String formatDisplayContent = String.format("Total : %s\n      ENTER PIN");
            paramsInfo.setDisplayContent(formatDisplayContent);
            nlPosManager.startReadCard(paramsInfo);

        }*/

    }


}
