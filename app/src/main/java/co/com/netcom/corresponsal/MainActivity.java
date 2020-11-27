/*package co.com.netcom.apos;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.RemoteException;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.widget.Toolbar;

import com.ingenico.lar.apos.DeviceHelper;
import com.ingenico.lar.bc.Pinpad;
import com.ingenico.lar.bc.PinpadCallbacks;
import com.ingenico.lar.bc.PinpadOutputHandler;
import com.ingenico.lar.larlib.BytesUtil;
import com.usdk.apiservice.aidl.data.IntValue;
import com.usdk.apiservice.aidl.pinpad.KeySystem;
import com.usdk.apiservice.aidl.pinpad.UPinpad;
import com.usdk.apiservice.aidl.printer.PrinterError;
import com.usdk.apiservice.aidl.printer.UPrinter;

import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

import cn.pedant.SweetAlert.SweetAlertDialog;
import co.com.netcom.apos.core.comunicacion.IntegradorC;
import co.com.netcom.apos.core.comunicacion.ParametrosC;
import co.com.netcom.apos.core.comunicacion.Procesos;
import co.com.netcom.apos.emv.DeviceManager;
import co.com.netcom.apos.emv.PinpadForMKSK;
import co.com.netcom.apos.pantallas.corresponsal.AdministradorAccesoActivity;
import co.com.netcom.apos.pantallas.comunes.BaseActivity;
import co.com.netcom.apos.pantallas.comunes.ProcesarActivity;
import co.com.netcom.apos.pantallas.comunes.ProcesarTarjeta;
import co.com.netcom.apos.pantallas.comunes.ResultadoActivity;
import co.com.netcom.apos.pantallas.comunes.Transaction;
import co.com.netcom.apos.pantallas.configuracion.ConfiguracionAccesoActivity;
import co.com.netcom.apos.pantallas.corresponsal.CorresponsalInicialActivity;
import co.com.netcom.apos.pantallas.corresponsal.CorresponsalLoginActivity;
import co.com.netcom.apos.pantallas.corresponsal.CorresponsalnicialAdministradorActivity;
import co.com.netcom.apos.pantallas.flujo.VenderActivity;
import co.com.netcom.apos.pantallas.transacciones.TransaccionesMenuActivity;

public class MainActivity extends BaseActivity implements PinpadCallbacks{
    private final static String TAG = "MainActivity";
    private IntegradorC integradorC;
    private String terminalInicializado;
    private String tipoTelecarga;
    private Pinpad pinpad = null;
    private String tipoComunicacion;
    WifiManager adminWifi;

    @SuppressLint("WifiManagerLeak")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        PinpadManager.me().setCallbacks(this);

        Toolbar barraNav = (Toolbar) findViewById(R.id.toolbarMain);
        setSupportActionBar(barraNav);

        integradorC = new IntegradorC(MainActivity.this);
        adminWifi= (WifiManager) this.getApplicationContext().getSystemService(Context.WIFI_SERVICE);
        tipoComunicacion = integradorC.obtenerParametrosC(ParametrosC.TIPO_COMUNICACION);
        if(tipoComunicacion.equals("4")){
            adminWifi.setWifiEnabled(false);
        }
        String path = this.getFilesDir().getAbsolutePath();
        integradorC.crearRutaArchivo(path);
        IntegradorC.inicializarJava();
        IntegradorC.copyFiletoExternalStorage(R.raw.caroot, "caroot.pem", path + "/", MainActivity.this);
        integradorC.copyFiletoExternalStorage(R.raw.ticketbcl, "TICKETBCL.TPL", path + "/", MainActivity.this);
        integradorC.copyFiletoExternalStorage(R.raw.tbcldecli, "TBCLDECLI.TPL", path + "/", MainActivity.this);
        integradorC.copyFiletoExternalStorage(R.raw.comsgiro, "COMSGIRO.TXT", path + "/", MainActivity.this);
        integradorC.copyFiletoExternalStorage(R.raw.tdoc, "TDOC.TXT", path + "/", MainActivity.this);
        integradorC.copyFiletoExternalStorage(R.raw.ticket, "TICKET.TPL", path + "/", MainActivity.this);
        integradorC.copyFiletoExternalStorage(R.raw.tdelect, "TDELECT.TPL", path + "/", MainActivity.this);
        integradorC.copyFiletoExternalStorage(R.raw.lealtad, "LEALTAD.TPL", path + "/", MainActivity.this);
        integradorC.copyFiletoExternalStorage(R.raw.urlf, "URLF.TXT", path + "/", MainActivity.this);
        integradorC.copyFiletoExternalStorage(R.raw.reverso, "REVERSO.TPL", path + "/", MainActivity.this);

        terminalInicializado = integradorC.obtenerParametrosC(ParametrosC.TERMINAL_INICIALIZADA);
        Log.d("Case 2", "terminalInicializado " + terminalInicializado);
        if (terminalInicializado.equals("2")) {
            Log.d("Case 2", "Error en la inicializacion ");
            integradorC.restaurarInicializacion();
        }

        // Migrado de BC

        // inject key
        new Timer().schedule(new TimerTask() {
            @Override
            public void run() {
                try {
                    Log.d(TAG, "Inicia Try getPinpad (0, 0, KeySystem.KS_MKSK)");
                    final UPinpad pinpad = DeviceHelper.me().getPinpad(0, 0, KeySystem.KS_MKSK);
                    pinpad.open();

                    Log.d(TAG, "Pinpad abierto OK");


                    try {
                        if (pinpad.isKeyExist(0)){
                            Log.d(TAG, "run: Existe la llave en la 0 perro"); // Es la 4
                            Log.d(TAG, "Llave pinpad cargada correctamente");
                            byte[] kvc = pinpad.calcKCV(0);
                            Log.d(TAG, "ALI Key KVC = " + BytesUtil.bytes2HexString(kvc));
                        }
                        if (pinpad.isKeyExist(1)){
                            Log.d(TAG, "run: Existe la llave en la 1 perro"); // Es la 3
                            Log.d(TAG, "Llave pinpad cargada correctamente");
                            byte[] kvc = pinpad.calcKCV(1);
                            Log.d(TAG, "ALI Key KVC = " + BytesUtil.bytes2HexString(kvc));

                            String WORKINGKEY_DATA_PIN = IntegradorC.obtenerLlaveWorkingFromC();
                            Log.d(TAG, "WORKINGKEY_DATA_PIN: "+WORKINGKEY_DATA_PIN);
                        }
                        if (pinpad.isKeyExist(2)){
                            Log.d(TAG, "run: Existe la llave en la 2 perro"); // Es la 10
                            Log.d(TAG, "Llave pinpad cargada correctamente");
                            byte[] kvc = pinpad.calcKCV(2);
                            Log.d(TAG, "ALI Key KVC = " + BytesUtil.bytes2HexString(kvc));
                        }
                        if (pinpad.isKeyExist(3)){
                            Log.d(TAG, "run: Existe la llave en la 3 perro");
                            Log.d(TAG, "Llave pinpad cargada correctamente");
                            byte[] kvc = pinpad.calcKCV(3);
                            Log.d(TAG, "ALI Key KVC = " + BytesUtil.bytes2HexString(kvc));
                        }
                        if (pinpad.isKeyExist(4)){
                            Log.d(TAG, "run: Existe la llave en la 4 perro"); // Esta es la 2
                            Log.d(TAG, "Llave pinpad cargada correctamente");
                            byte[] kvc = pinpad.calcKCV(4);
                            Log.d(TAG, "ALI Key KVC = " + BytesUtil.bytes2HexString(kvc));
                        }
                        if (pinpad.isKeyExist(5)){
                            Log.d(TAG, "run: Existe la llave en la 5 perro");
                            Log.d(TAG, "Llave pinpad cargada correctamente");
                            byte[] kvc = pinpad.calcKCV(5);
                            Log.d(TAG, "ALI Key KVC = " + BytesUtil.bytes2HexString(kvc));
                        }
                        if (pinpad.isKeyExist(6)){
                            Log.d(TAG, "run: Existe la llave en la 6 perro");
                            Log.d(TAG, "Llave pinpad cargada correctamente");
                            byte[] kvc = pinpad.calcKCV(6);
                            Log.d(TAG, "ALI Key KVC = " + BytesUtil.bytes2HexString(kvc));
                        }
                        if (pinpad.isKeyExist(7)){
                            Log.d(TAG, "run: Existe la llave en la 7 perro");
                            Log.d(TAG, "Llave pinpad cargada correctamente");
                            byte[] kvc = pinpad.calcKCV(7);
                            Log.d(TAG, "ALI Key KVC = " + BytesUtil.bytes2HexString(kvc));
                        }
                        if (pinpad.isKeyExist(8)){
                            Log.d(TAG, "run: Existe la llave en la 8 perro");
                            Log.d(TAG, "Llave pinpad cargada correctamente");
                            byte[] kvc = pinpad.calcKCV(8);
                            Log.d(TAG, "ALI Key KVC = " + BytesUtil.bytes2HexString(kvc));
                        }
                        if (pinpad.isKeyExist(9)){
                            Log.d(TAG, "run: Existe la llave en la 9 perro");
                            Log.d(TAG, "Llave pinpad cargada correctamente");
                            byte[] kvc = pinpad.calcKCV(9);
                            Log.d(TAG, "ALI Key KVC = " + BytesUtil.bytes2HexString(kvc));
                        }
                        if (pinpad.isKeyExist(10)){
                            Log.d(TAG, "run: Existe la llave en la 10 perro");
                            Log.d(TAG, "Llave pinpad cargada correctamente");
                            byte[] kvc = pinpad.calcKCV(10);
                            Log.d(TAG, "ALI Key KVC = " + BytesUtil.bytes2HexString(kvc));
                        }
                        if (pinpad.isKeyExist(11)){
                            Log.d(TAG, "run: Existe la llave en la 11 perro");
                            Log.d(TAG, "Llave pinpad cargada correctamente");
                            byte[] kvc = pinpad.calcKCV(11);
                            Log.d(TAG, "ALI Key KVC = " + BytesUtil.bytes2HexString(kvc));
                        }
                        if (pinpad.isKeyExist(12)){
                            Log.d(TAG, "run: Existe la llave en la 12 perro");
                            Log.d(TAG, "Llave pinpad cargada correctamente");
                            byte[] kvc = pinpad.calcKCV(12);
                            Log.d(TAG, "ALI Key KVC = " + BytesUtil.bytes2HexString(kvc));
                        }
                        if (pinpad.isKeyExist(13)){
                            Log.d(TAG, "run: Existe la llave en la 13 perro");
                            Log.d(TAG, "Llave pinpad cargada correctamente");
                            byte[] kvc = pinpad.calcKCV(13);
                            Log.d(TAG, "ALI Key KVC = " + BytesUtil.bytes2HexString(kvc));
                        }
                        // pinpad.loadPlainTextKey(KeyType.MAIN_KEY, 4, new byte[]{0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22});


                    } finally {
                        pinpad.close();
                    }
                } catch (RemoteException e) {
                    Log.e(TAG, "pinpad", e);
                }
            }
        }, 1000);

        final PinpadOutputHandler checkEventHandler = pinpadOutput -> {
            if (pinpadOutput.getResultCode() == 0) {
                final String output = pinpadOutput.getOutput();
                final char whichEvent = output.charAt(0);
                Log.d(TAG, "checkEvent event " + whichEvent);
                if (whichEvent == '1') {
                    Log.d(TAG, "checkEvent SWIPE: '" + output + "'");
                    PinpadManager.TrackData track = PinpadManager.extractTrack(output, 1);

                    // bundle and forward transaction
                    Bundle t = new Bundle();
                    t.putInt(Transaction.TYPE, Transaction.TYPE_SALE);
                    t.putInt(Transaction.MODE, Transaction.MODE_MAG);
                    t.putString(Transaction.TRACK1, track.track1);
                    t.putString(Transaction.TRACK2, track.track2);
                    t.putString(Transaction.TRACK3, track.track3);
                    /*Intent intent = new Intent(MainActivity.this, AskAmountActivity.class);
                    intent.putExtra("transaction", t);
                    startActivity(intent);
                    finish();*/
/*
                } else if (whichEvent == '2') {
                    Log.d(TAG, "checkEvent CHIP: '" + output + "'");

                    // start a transaction without card info, GetCardActivity will deal with this later
                    Bundle t = new Bundle();
                    t.putInt(Transaction.TYPE, Transaction.TYPE_SALE);
                    /*Intent intent = new Intent(MainActivity.this, AskAmountActivity.class);
                    intent.putExtra("transaction", t);
                    startActivity(intent);
                    finish();*/
      /*          } else {
                    // should never happen...
                    Log.e(TAG, "checkEvent unexpected event");
                }
            }
            else {
                Log.e(TAG, "checkEvent output error " + pinpadOutput.getResultCode());
            }
        };

        new Thread(() -> {
            // Pinpad.open *must not* be called on UI thread
            PinpadManager.me().open();

            // at main screen just check for swipe and card insertion
            // note that actual EMV processing will *not* start -- only detect the *insertion*
            final int ret = PinpadManager.me().checkEvent("1120", MainActivity.this, checkEventHandler);
            if (ret != Pinpad.PP_OK) {
                Log.e(TAG, "checkEvent call error " + ret);
            }
        }).start();

        tipoTelecarga = integradorC.obtenerParametrosC(ParametrosC.TIPO_TELECARGA);
        Log.e(TAG, "tipoTelecarga " + tipoTelecarga);
        if(tipoTelecarga.equals("1")){
            Intent intento = new Intent(MainActivity.this, ProcesarActivity.class);
            intento.putExtra("proceso", Procesos.TRANSACCIONES_INICIALIZACION);
            intento.putExtra("tipoInicializacion", "930010");
            startActivity(intento);
        }
        // Fin migrado BC

    }

    private void getExistentKeyIdsInKeySystem() throws RemoteException {
        // Get key IDs
        byte keyUsage = 0x11; // mac key
        String message = "";
        List<IntValue> keyIds;
        keyIds = PinpadForMKSK.getInstance().getExistentKeyIdsInKeySystem(keyUsage);
        message += keyIds.size() + "\r\n";
        for (IntValue keyId : keyIds) {
            message += keyId.getData() + "\r\n";
        }

        // Show message
        Log.d(TAG, "getExistentKeyIdsInKeySystem: " + message);
    }



    @Override
    public int onShowMessage(int i, String s) {
        final String formatted = PinpadManager.formatMessage(i, s);
        if (formatted != null) {
            runOnUiThread(() -> Log.d(TAG,formatted));
        }
        return Pinpad.PP_OK;
    }

    @Override
    public int onShowPinEntry(String s, long l, int i) {
        // never called here
        return 0;
    }

    @Override
    public void onAbort() {
        // never called here
    }

    @Override
    public void onShowMenu(int i, String s, String[] strings, MenuResult menuResult) {
        // never called here
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.config_menu, menu);
        return true;
    }


    @Override
    public boolean onOptionsItemSelected(@NonNull MenuItem item) {
        Log.d("ClickMenu", "Menú click: " + item.getItemId());
        switch (item.getItemId()){
            case R.id.itemMenu1:
                Log.d("MenuItem", "Item menú: 1");
                irAAdministrador(item.getActionView());
                return true;
            case R.id.itemMenu2:
                Log.d("MenuItem", "Item menú: 2");
                irAConfiguracion(item.getActionView());
                return true;
            default:
                Log.d("MenuItem", "onOptionsItemSelected: default");
                return super.onOptionsItemSelected(item);
        }

    }

    public void vender(View el) {

        String numeroTerminalActual = integradorC.obtenerParametrosC(ParametrosC.CNB_TERMINAL);

        if (terminalInicializado.equals("1")) {
/*            if(numeroTerminalActual.equals("")){
                mostrarResultado(false, "MENSAJE", "DATAFONO EN MODO PURO CNB ", false);
            } else {*/
         /*   Intent intento = new Intent(MainActivity.this, VenderActivity.class);
            startActivity(intento);
//            }
        } else {
            mostrarResultado(false, "MENSAJE", "DEBE INICIALIZAR", false);
        }
    }

    public void irALogin(View el) throws RemoteException {
        int printerOK;
        PinpadManager.me().abort();
        DeviceHelper.me().bindService();
        UPrinter printer = DeviceHelper.me().getPrinter();

        // Get statue
        printerOK = printer.getStatus();
        if(printerOK != PrinterError.SUCCESS){
            Toast.makeText(MainActivity.this, "NO HAY PAPEL COLOCAR PAPEL EN LA IMPRESORA   ", Toast.LENGTH_LONG).show();
            return;
        }
        IntegradorC integradorC = new IntegradorC(getApplicationContext());
        IntegradorC.obtenerModoCnb();
        IntegradorC.limpiarGlobalesC();
        String login = integradorC.obtenerParametrosC(ParametrosC.USER_CNB_LOGON);
        String terminalInicializado = integradorC.obtenerParametrosC(ParametrosC.TERMINAL_INICIALIZADA);
        String modoCnbHabilitado = integradorC.obtenerParametrosC(ParametrosC.HABILITACION_MODO_CNB_BCL);

        try {
            Log.d("Case 2", "onItemClick: entra corresponsal a" );
            hardwareTerminalParaNativo();
        } catch (RemoteException e) {
            e.printStackTrace();
        }

        Log.d("Case 2", "onItemClick: entra corresponsal " );

        if (terminalInicializado.equals("1") && modoCnbHabilitado.equals("1")) {
            integradorC.setearParametrosC(ParametrosC.USER_CNB_LOGON,"0");
            Log.d("Case 2", "onItemClick: login " + login);
            if (login.equals("1") || login.equals("0")) {
                if (login.equals("1")) {
                    Intent intento = new Intent(MainActivity.this, CorresponsalInicialActivity.class);
                    startActivity(intento);
                } else {
                    Intent intento = new Intent(MainActivity.this, CorresponsalLoginActivity.class);
                    startActivity(intento);
                }
            } else if (login.equals("3")) {
                Intent intento = new Intent(MainActivity.this, CorresponsalnicialAdministradorActivity.class);
                startActivity(intento);
            } else if (login.equals("4")) {
                Intent intento = new Intent(MainActivity.this, CorresponsalInicialActivity.class);
                startActivity(intento);
            } else if (login.equals("0")) {
                Intent intento = new Intent(MainActivity.this, CorresponsalLoginActivity.class);
            }
        } else {
            if (!terminalInicializado.equals("1")) {
                mostrarResultado(false, "MENSAJE", "DEBE INICIALIZAR", false);
            } else if (!modoCnbHabilitado.equals("1")) {
                mostrarResultado(false, "MENSAJE", "INICIALIZE CNB", false);
            }
        }

    }

    public void irATransacciones(View el) {
        try {
            Log.d("Case 2", "onItemClick: entra corresponsal a" );
            hardwareTerminalParaNativo();
        } catch (RemoteException e) {
            e.printStackTrace();
        }

        Intent intento = new Intent(MainActivity.this, TransaccionesMenuActivity.class);
        startActivity(intento);
    }

    public void irAConfiguracion(View el) {
        Intent intento = new Intent(MainActivity.this, ConfiguracionAccesoActivity.class);
        startActivity(intento);
    }

    public void irAAdministrador(View el) {
        Intent intento = new Intent(MainActivity.this, AdministradorAccesoActivity.class);
        startActivity(intento);
    }

    public void irAOtrasTransacciones(View el) {
//        Intent intento = new Intent(MainActivity.this, OtrasMenuActivity.class);
//        startActivity(intento);
        mostrarResultado(false, "MENSAJE", "SEGUNDA FASE DESARROLLO", false);
    }

    public static void mostrarMensaje(String titulo, String mensaje, Context contexto, Class destino) {
        new SweetAlertDialog(contexto, SweetAlertDialog.SUCCESS_TYPE)
                .setTitleText(titulo)
                .setContentText(mensaje)
                .setConfirmClickListener(new SweetAlertDialog.OnSweetClickListener() {
                    @Override
                    public void onClick(SweetAlertDialog sDialog) {
                        if (destino != null) {
                            Class baseNue = destino;
                            if (baseNue.getClassLoader() != null) {
                                Log.d("Ctx", "Con baseNue");
                            } else {
                                Log.d("Ctx", "Sin baseNue");
                            }
                            Intent intento = new Intent(contexto, baseNue);
                            contexto.startActivity(intento);
                        }
                        sDialog.dismissWithAnimation();
                    }
                })
                .show();
    }

    public void mostrarResultado(Boolean res, String cod, String msg, Boolean print) {
        Intent intento = new Intent(MainActivity.this, ResultadoActivity.class);
        intento.putExtra("resultado", res);
        intento.putExtra("codigo", cod);
        intento.putExtra("mensaje", msg);
        intento.putExtra("imprimir", print);
        startActivity(intento);
    }

    public void hardwareTerminalParaNativo() throws RemoteException {

        try {
            ConnectivityManager cm =
                    (ConnectivityManager)MainActivity.this.getSystemService(Context.CONNECTIVITY_SERVICE);

            NetworkInfo activeNetwork = cm.getActiveNetworkInfo();
            boolean isConnected = activeNetwork != null &&
                    activeNetwork.isConnectedOrConnecting();

            boolean isWiFi = activeNetwork.getType() == ConnectivityManager.TYPE_WIFI;
            boolean isMobile = activeNetwork.getType() == ConnectivityManager.TYPE_MOBILE;
            Log.d("Case 2", "onItemClick: isWiFi " + isWiFi);
            Log.d("Case 2", "onItemClick: isMobile " + isMobile);
            IntegradorC integradorC = new IntegradorC(getApplicationContext());
            if(isWiFi == true && isMobile == false){
                integradorC.setearParametrosC(ParametrosC.TIPO_COMUNICACION,"3");
            } else if (isWiFi == false && isMobile == true){
                integradorC.setearParametrosC(ParametrosC.TIPO_COMUNICACION,"4");
            }

            String direccionIP = IntegradorC.getIPAddressIPv4("wlan");
            Log.d("Ctx", "direccionIP " + direccionIP);
            direccionIP = direccionIP.replace(".", ";");
            String[] ipPartida = direccionIP.split(";");
            ipPartida[0] = String.format("%03d", Integer.parseInt(ipPartida[0]));
            ipPartida[1] = String.format("%03d", Integer.parseInt(ipPartida[1]));
            ipPartida[2] = String.format("%03d", Integer.parseInt(ipPartida[2]));
            ipPartida[3] = String.format("%03d", Integer.parseInt(ipPartida[3]));
            direccionIP = ipPartida[0] + ipPartida[1] + ipPartida[2] + ipPartida[3];

            String dataTerminal = DeviceManager.getInstance().getSerialNo();
            dataTerminal = dataTerminal.substring(dataTerminal.length() - 8);
            String macAdress = IntegradorC.getMacAddress().toUpperCase();
            macAdress = macAdress.replace(":", "");
            String dataEscribir = direccionIP + ";" + dataTerminal + ";" + macAdress;
            Log.d("DataFile", "data File " + dataEscribir);
            integradorC.escribirFile(dataEscribir, dataEscribir.length());
        } catch (Exception e){
            Log.d("Ctx", "Paila excepcion por no conexion " );
            String dataTerminal = DeviceManager.getInstance().getSerialNo();
            dataTerminal = dataTerminal.substring(dataTerminal.length() - 8);
            String dataEscribir = "089054220178" + ";" + dataTerminal + ";" + "B486AFD857A7";
            Log.d("DataFile", "data File " + dataEscribir);
            integradorC.escribirFile(dataEscribir, dataEscribir.length());
        }

    }

}*/
