package co.com.netcom.corresponsal.core.comunicacion;

import android.content.Context;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.RemoteException;
import android.util.Log;

import com.ingenico.lar.apos.DeviceHelper;
import com.usdk.apiservice.aidl.printer.ASCScale;
import com.usdk.apiservice.aidl.printer.ASCSize;
import com.usdk.apiservice.aidl.printer.AlignMode;
import com.usdk.apiservice.aidl.printer.HZScale;
import com.usdk.apiservice.aidl.printer.HZSize;
import com.usdk.apiservice.aidl.printer.OnPrintListener;
import com.usdk.apiservice.aidl.printer.UPrinter;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.util.Calendar;
import java.util.Collections;
import java.util.GregorianCalendar;
import java.util.List;

import cn.pedant.SweetAlert.SweetAlertDialog;
import co.com.netcom.corresponsal.R;

/*import co.com.netcom.aposa8.api.ingenico.ScannerForBack;
import co.com.netcom.aposa8.api.ingenico.util.printer.Printer;*/

/**
 * Created by alvaroriosch on 10/12/18.
 */

public class IntegradorC {
    private static final int FONT_SIZE_SMALL = 0;
    private static final int FONT_SIZE_NORMAL = 1;
    private static final int FONT_SIZE_LARGE = 2;
    public static String datosInicializacion = "CONECTANDO";
    public static String[] datosScreenMessageFive = new String[6];
    public static String[] datosGiro = new String[10];
    public String screenMessageFiveLine = "Rechazo";
    public String showMenu = "NOMENU";
    public static int counter;
    private final static String TAG = "INTEGRADOR";
    private boolean reciboImpreso = false;
    private static WaitThreat esperarThreat = new WaitThreat();

    // thread wait 、awake
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

    private static Handler esperarEventHandler = new Handler(Looper.getMainLooper()) {

        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what) {
                case 1:
                    esperarThreat.notifyThread();
                    break;
                default:
                    break;
            }
        }

    };

    public static WaitThreat getEsperarThreat() {
        return esperarThreat;
    }

    public static void setEsperarThreat(WaitThreat esperarThreat) {
        IntegradorC.esperarThreat = esperarThreat;
    }

    public static Handler getEsperarEventHandler() {
        return esperarEventHandler;
    }

    public static void setEsperarEventHandler(Handler esperarEventHandler) {
        IntegradorC.esperarEventHandler = esperarEventHandler;
    }

    public static String getIPAddressIPv4(String id) {
        try {

            List<NetworkInterface> interfaces = Collections.list(NetworkInterface.getNetworkInterfaces());
            for (NetworkInterface intf : interfaces) {
                if (!intf.getName().contains(id)) {
                    List<InetAddress> addrs = Collections.list(intf.getInetAddresses());
                    for (InetAddress addr : addrs) {

                        if (!addr.isLoopbackAddress()) {
                            String sAddr = addr.getHostAddress();
                            if (addr instanceof Inet4Address) {
                                return sAddr;
                            }
                        }

                    }
                }
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        return null;
    }

    public static String getMacAddress() {
        try {
            List<NetworkInterface> all = Collections.list(NetworkInterface.getNetworkInterfaces());
            for (NetworkInterface nif : all) {
                if (!nif.getName().equalsIgnoreCase("wlan0")) continue;

                byte[] macBytes = nif.getHardwareAddress();
                if (macBytes == null) {
                    return "";
                }

                StringBuilder res1 = new StringBuilder();
                for (byte b : macBytes) {
                    res1.append(Integer.toHexString(b & 0xFF) + ":");
                }

                if (res1.length() > 0) {
                    res1.deleteCharAt(res1.length() - 1);
                }
                return res1.toString();
            }
        } catch (Exception ex) {
        }
        return "";
    }

    public String getShowMenu() {
        return showMenu;
    }

    public void setShowMenu(String showMenu) {
        this.showMenu = showMenu;
    }

    public static void setCounter(int counter) {
        IntegradorC.counter = counter;
    }

    public static int getCounter() {
        return counter;
    }

    public static String getDatosInicializacion() {
        return datosInicializacion;
    }

    public static void setDatosInicializacion(String datosInicializacion) {
        IntegradorC.datosInicializacion = datosInicializacion;
    }

    public String getScreenMessageFiveLine() {
        return screenMessageFiveLine;
    }

    public void setScreenMessageFiveLine(String screenMessageFiveLine) {
        this.screenMessageFiveLine = screenMessageFiveLine;
    }

    static {
        System.loadLibrary("native-lib");
    }

    private static Context context;


    public IntegradorC(Context context) {
        this.context = context;
    }

    public int registrarUsuario(String usuario, String clave, String tipoUser) {
        return crearUsuarioCnb(usuario, clave, tipoUser);
    }

    public static void inicializarJava() {
        inicializar();
    }

    public static void crearRutaArchivo(String path) {
        initNDK(path);
    }

    public static void copyFiletoExternalStorage(int resourceId, String resourceName, String path, Context context) {
        String pathSDCard = path + resourceName;
        try {
            InputStream in = context.getResources().openRawResource(resourceId);
            FileOutputStream out = null;
            out = new FileOutputStream(pathSDCard);
            byte[] buff = new byte[1024];
            int read = 0;
            try {
                while ((read = in.read(buff)) > 0) {
                    out.write(buff, 0, read);
                }
            } finally {
                in.close();
                out.close();
            }
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    public static int consultarUsuario(String user, String pass) {
        return consultarInfoParametros(user, pass);
    }

    public static int capturarClavesInicializacionNativa(String usuario, String clave) {
        return capturarClavesInicializacion(usuario, clave);
    }

    public String obtenerParametrosC(int clave) {
        return getParameterFromC(clave);
    }

    public void setearParametrosC(int clave, String valor) {
        setParameterToC(clave, valor);
    }

 //   public String enviarRetiroTarjetaBCL(CardDTO datosTarjeta, String monto, int otraCuenta,  String numeroCuenta) {
  //      public String enviarRetiroTarjetaBCLObj(CardDTO datosTarjeta, String monto, int otraCuenta,  String numeroCuenta);
  //  }

//    public String enviarRetiroSinTarjetaBCL(String monto, String cuenta, String pinBlock) {
 //       return enviarRetiroSinTarjeta(monto, cuenta, pinBlock);
//    }

    private void imprimirLinea(String linea) throws RemoteException {
        imprimirLineaNativa(linea);
    }

    public static String obtenerFechaJava() {

        String strfecha = "";
        //Instanciamos el objeto Calendar
        //en fecha obtenemos la fecha y hora del sistema
        Calendar fecha = new GregorianCalendar();

        //Obtenemos el valor del año, mes, día,
        //hora, minuto y segundo del sistema
        //usando el método get y el parámetro correspondiente
        int año = fecha.get(Calendar.YEAR);
        int mes = fecha.get(Calendar.MONTH);
        int dia = fecha.get(Calendar.DAY_OF_MONTH);
        int hora = fecha.get(Calendar.HOUR_OF_DAY);
        int minuto = fecha.get(Calendar.MINUTE);
        int segundo = fecha.get(Calendar.SECOND);
        mes++;
        strfecha = Integer.toString(año) + String.format("%02d", mes) + String.format("%02d", dia) + String.format("%02d", hora) + String.format("%02d", minuto) + String.format("%02d", segundo);

        return strfecha;
    }

    private String obtenerStringDeclinada() {
        return this.screenMessageFiveLine;
    }


    public void imprimirLineaNativa(String linea) throws RemoteException {

        DeviceHelper.me().bindService();
        UPrinter printer = DeviceHelper.me().getPrinter();

        printer.setPrnGray(2);
        setFontSpec(FONT_SIZE_SMALL, printer);
        if (linea.equals("final")) {

            printer.feedLine(3);
            // Start printing
            printer.startPrint(new OnPrintListener.Stub() {

                @Override
                public void onFinish() throws RemoteException {

                }

                @Override
                public void onError(int error) throws RemoteException {


                }
            });
        } else {
            printer.addText(AlignMode.LEFT, linea.trim());
        }

    }

    public void imprimir() throws RemoteException, IOException {

        DeviceHelper.me().bindService();
        UPrinter printer = DeviceHelper.me().getPrinter();

        // Get statue
        printer.getStatus();
        // Show dialog
        //showDialog(R.string.waiting_for_printing, false);

        // Set gray
        printer.setPrnGray(7);
        // Print image
        printer.addImage(AlignMode.CENTER, readAssetsFile(R.raw.l0000));

        String all = "";
        String alineacion = "l";
        String tipoLetra = "";
        String formato = "";

        String path = context.getFilesDir().getAbsolutePath();
        BufferedReader br = new BufferedReader(new FileReader(path + "/TCKT.TXT"));
        String strLine;
        while ((strLine = br.readLine()) != null) {

            all = "";
            tipoLetra = strLine.substring(12, 18);
            setFontSpec(FONT_SIZE_SMALL, printer);
            if (tipoLetra.equals("[0X2B]")) {
            } else {
                setFontSpec(FONT_SIZE_SMALL, printer);
            }

            alineacion = strLine.substring(6, 12);
            if (alineacion.equals("[0X1C]")) {
                printer.addText(AlignMode.CENTER, strLine.substring(24).trim());
                Log.d("ADD printer ", "linea " + strLine.substring(24));
            } else {
                printer.addText(AlignMode.LEFT, strLine.substring(24).trim());
                Log.d("ADD printer ", "linea " + strLine.substring(24));
            }
        }

        // Feed lines
        printer.feedLine(3);

        // Start printing
        printer.startPrint(new OnPrintListener.Stub() {

            @Override
            public void onFinish() throws RemoteException {
                printer.cutPaper();
            }

            @Override
            public void onError(int error) throws RemoteException {


            }
        });
    }

    /**
     * Set font spec.
     */
    private void setFontSpec(int fontSpec, UPrinter printer) throws RemoteException {
        switch (fontSpec) {
            case FONT_SIZE_SMALL:
                printer.setHzSize(HZSize.DOT24x16);
                printer.setHzScale(HZScale.SC1x1);
                printer.setAscSize(ASCSize.DOT16x8);
                printer.setAscScale(ASCScale.SC1x2);
                //printer.setYSpace(0);
                break;

            case FONT_SIZE_NORMAL:
                printer.setHzSize(HZSize.DOT24x24);
                printer.setHzScale(HZScale.SC1x1);
                printer.setAscSize(ASCSize.DOT24x12);
                printer.setAscScale(ASCScale.SC1x1);
                break;

            case FONT_SIZE_LARGE:
                printer.setHzSize(HZSize.DOT24x24);
                printer.setHzScale(HZScale.SC1x2);
                printer.setAscSize(ASCSize.DOT24x12);
                printer.setAscScale(ASCScale.SC1x2);
                break;
        }
    }

    private byte[] readAssetsFile(int  resourceId) throws RemoteException {
        InputStream input = null;
        try {
            input =context.getResources().openRawResource(resourceId);
            byte[] buffer = new byte[input.available()];
            int size = input.read(buffer);
            if (size == -1) {
                //throw new RemoteException(getContext().getString(R.string.read_fail));
            }
            return buffer;
        } catch (IOException e) {
            throw new RemoteException(e.getMessage());
        } finally {
            if (input != null) {
                try {
                    input.close();
                } catch (IOException e) {
                    //showToast(e.getLocalizedMessage());
                }
            }
        }
    }
    /*private void mostrarInicializacion(String mensaje) {
        try {
            LED.getInstance().operateGreenLight();
            datosInicializacion = mensaje;
            Message msg = ProcesarActivity.getEsperarEventHandler().obtainMessage(0);
            msg.sendToTarget();
            esperarThreat.waitForRslt();

        } catch (RemoteException e) {
            e.printStackTrace();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }*/

    private void mostrarMenu(String mensaje) {
        showMenu = mensaje;
    }

    public void guardarDatosGiro(String cadenaGiro) {
        datosGiro[counter] = cadenaGiro;
        counter++;

    }

    public void showScreenMessage(String str) {
        screenMessageFiveLine = str;
    }

    public Boolean solicitarSegundoRecibo(String texto, int corresponsal, int anulacion) {
        String mensaje = texto + " IMPRIMIR SEGUNDO RECIBO?";

        Log.d("SolictarRecibo", "onItemClick: solicitar");

        SweetAlertDialog sw = new SweetAlertDialog(context, SweetAlertDialog.WARNING_TYPE);
        sw.setTitleText("RECIBO");
        sw.setContentText(mensaje);
        sw.setConfirmText("SI");
        sw.setCancelText("NO");
        sw.showCancelButton(true);
        sw.setConfirmClickListener(new SweetAlertDialog.OnSweetClickListener() {
            @Override
            public void onClick(SweetAlertDialog sDialog) {
                if (reciboImpreso == false) {
                    reciboImpreso = true;
                    realizarImpresionSegundoRecibo(corresponsal,anulacion);
                    sDialog.dismissWithAnimation();
                }
            }
        });
        sw.setCancelClickListener(new SweetAlertDialog.OnSweetClickListener() {
            @Override
            public void onClick(SweetAlertDialog sDialog) {
                reciboImpreso = true;
                sDialog.cancel();

            }
        })
                .show();
        new Thread() {
            @Override
            public void run() {
                try {
                    sleep(4000);
                    if (reciboImpreso == false) {
                        reciboImpreso = true;
                        realizarImpresionSegundoRecibo(corresponsal,anulacion);
                    }
                    sw.dismiss();

                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }.start();
        Log.d(TAG, "reciboImpreso integrador "+ reciboImpreso);
        return reciboImpreso;
    }

    public void alertScreenMessage(String texto, String titulo) {

        new SweetAlertDialog(context, SweetAlertDialog.WARNING_TYPE)
                .setTitleText(titulo)
                .setContentText(texto)
                .setConfirmText("SI")
                .setCancelText("NO")
                .showCancelButton(true)
                .setConfirmClickListener(new SweetAlertDialog.OnSweetClickListener() {
                    @Override
                    public void onClick(SweetAlertDialog sDialog) {
                        sDialog.dismissWithAnimation();
                    }
                })
                .setCancelClickListener(new SweetAlertDialog.OnSweetClickListener() {
                    @Override
                    public void onClick(SweetAlertDialog sDialog) {
                        sDialog.cancel();
                    }
                })
                .show();
    }

    private static native void initNDK(String mng);

    public native int echoTestJava();

    public  native int logonJava();

    private static native void inicializar();

    // CORRESPONSAL CNB BANCOLONBIA
    public static native int consultarInfoParametros(String user, String pass);

   public native String enviarDeposito(String numeroCuenta, String valor, int tipoCuenta);


    public native String enviarPagoPoducto(String referenciaProducto, String valor, int tipoProducto);

    public native static String obtenerLlaveWorkingFromC();

    public native String enviarConsultaSaldoBclObj(CardDTO datosTarjeta);

    public native String enviarRetiroTarjetaBCLObj(CardDTO datosTarjeta, String monto, int otraCuenta, String numeroDeCuenta);

    public native String enviarRetiroSinTarjeta(String monto, String numeroCuenta, String pinBlock);

    public native String enviarTransferencia(CardDTO datosTarjeta, String tipoCuentaO, String TipoCuentaD, String cuentaDestino, String otraCuenta, String monto);

    public native String enviarConsultaRecaudoLector(String codigoLeido, DatosRecaudo datosRecaudo);

    public native String enviarPagoRecaudoLector(String codigoLeido, DatosRecaudo datosRecaudo);

    public native String enviarConsultaRecaudoManual(DatosRecaudo datosRecaudo);

    public native int enviarCierreCNB(int tipoCierre);

    public native int solicitarDuplicadoCNBBCL(int opcionRecibo, int tipoRecibo, String numeroRecibo);

    public native String realizarEnvioGiroCnb(DatosComision comision, String tpdGirador, String tpdBeneficiario, String dgirador, String dBeneficiario, String monto, String celGirador, String celBeneficiario);

    public native int verificarItemsDocumento();

    public native String realizarReclamacionGiro(String referenciaReclamacion, String tpdBeneficiario, String dBeneficiario, String monto);

    public native String realizaConsultaCancelacionGiroCnb(DatosComision datosComision, String tpdGirador, String tpdBeneficiario, String dgirador, String dBeneficiario, String referenciaCancelacion);

    public native String realizarCancelacionGiro(DatosComision datosComision, String tpdGirador, String tpdBeneficiario, String dgirador, String dBeneficiario);

    public native String realizarConsultaCupoBCL();

    // CONFIGURACION
    public native String inicializarTerminal(String tipoInicializacion);

    public native String inicializarParametros();

    public native String inicializarSerial();

    public native int cerrarSesion();

    public native int crearUsuarioCnb(String usuario, String clave, String tipoUser);

    private static native int capturarClavesInicializacion(String usuario, String clave);

    public native boolean guardarParametrosTerminal(String valorNii, String valorNiiMultipos, String valorNiiCelucompra, String valorNiiDCC, String valorNumeroTerminal, String valorIdBonos, String valorNumeroPlaca);

    public native int codigoTerminalCnb(String valorNumeroTerminal);

    public native boolean guardarParametrosComunicacionIpPuerto(String valorIp, String valorPuerto, String tipoCanal);

    public native int guardarParametrosPanVirtual(String codigoBanco, String numeroTarjeta, String tipoCuenta);

    public native int deshabilitarCNB();

    private native String getParameterFromC(int clave);

    public native int capturarImpuestoAutomatico(int tipoImpuesto);

    public native int capturarImpuestoMultiple(int tipoImpuesto);

    public native int setearIvaAutomatico(String valor);

    public native int setearIvaMultiple(String valor);

    public native int setearIncAutomatico(String valor);

    public native int setearIncMultiple(String valor);

    public native int setearCalculoBaseDevolucion(String valor);

    public native int validarcapturaImpuestoAutomatico(String valorCapturado, int tipoImpuesto);

    public native int validarcapturaImpuestoMultiple(String valorPrimerImpuesto, String valorSegundoImpuesto, int tipoImpuesto);

    public native void setParameterToC(int clave, String valor);

    public native int borrarTransacciones();

    public native String borrarParametros();

    public native int habilitarCNBConCtaSembrada(String codigo_Banco, String tipo_Cuenta, String track2, String nombre_tarjeta_habiente);

    public native int verificarComisionEnvioGiro(String monto, DatosComision datosComision);

    public native String obtenerMenuYparametrosDocumentosGiros(DatosArchivoPar datosArchivoPar, int items, int indice);

    public native void realizarImpresionSegundoRecibo(int corresponsal, int anulacion);

    public native int usuarioAmodificarCnb(String usuario);

    public native int eliminarUsuarioCnb(String usuario);

    public native int listaUsuarios();

    public native int modificarUsuarioCnb(String usuario, String usuario2, String clave, String tipoUser);

    /*VENTA FLUJO UNIFICADO*/
    public native String validarMontosIngresadosVenta(DatosBasicosVenta datosBasicosVenta);

    public native String realizarVentaEmv(CardDTO datosTarjta, DatosBasicosVenta datosBasicosVenta);

    public native int solicitarDuplicadoVenta(int opcionRecibo, int tipoRecibo, String numeroRecibo);

    public native int reporteCierreVentas(int tipoReporte);//1 totales 2 auditoria

    public native int realizarCierreVenta();

    public native String realizarConsultaReciboAnular(int tipoRecibo, String numeroRecibo);

    public native String realizarTransaccionAnulacion(CardDTO datosTarjeta, String numeroRecibo);

    public native String realizarConsultaReciboAnularBancolombia(int tipoRecibo, String numeroRecibo);

    public native String realizarTransaccionAnulacionBancolombia(String numeroRecibo);

    public native int comportamientoTiposCuentaEmv(CardDTO datosTarjta);

    public native int realizarSolicitudQrcode(DatosBasicosVenta datosBasicosVenta);

    public native int realizarConsultaQrcode();

    public native void solicitarMenuEntidadesDineroElectronico();

    public native void solicitarParametrosDineroElectronico(ParametrosDineroElectronico parametrosDineroElectronico, int tipoTx, int opcion, String valor);

    public native String consultarSaldo(CardDTO datosTarjeta);

    public native void armarTrack2DineroElectronico(ParametrosDineroElectronico parmetroscompra, int tipoDocumento, String documento, String celular);

    public native void enviarDatosBasicoCompraDineroElectronico(DatosBasicosVenta datosBasicosVenta);

    public native String realizarCompraDineroElectronico(String pinblock, int intentosPin, String tipoCuenta, String cuotas);

    public native String obtenerDatosParaPinDineroElectronico();

    public native void obtenerMenuEntidadesLifeMiles(DatosBasicosVenta datosBasicosVenta);

    public native void seleccionarItemsDocumentoLealtadLifemiles(int opcion);

    public native int combinacionesTipoDocumento(int opcionLealtad, int opcionDocumento);

    public native int ingresoDocumentoLifeMiles(String documento, int modoEntrada);

    public native int procesoAcumulacionRendencion(int lectura, int acumularAjustar, int modoEntrada, CardDTO leerTrack);

    public native void iniciarPuntosColombia(DatosBasicosVenta datosBasicosVenta);

    public native void redencionAcumulacionPuntoscolombia(int opcionLealtad, int tipoDocumento, String documento, String codigoCajero, String galones, String monto);

    public native void solicitudConfigurarPuntosColombia(int producto);

    public native void configurarProductoIPSeleccionado(int producto, int opcion);

    public native int capturarIvaManual(String iva, String totalVenta);

    public native int capturarBaseDevolucionManual(String baseDevolucion, String totalVenta, String iva);

    public native int capturarIncManual(DatosBasicosVenta datosBasicosVenta);

    public native void restaurarInicializacion();

    public native int validarPropina(int OpcionPropina, DatosBasicosVenta datosBasicosVenta, int porcentajePropina);

    public native int tasaAeroportuariaPropinaHabilitada();

    public native int verificarReverso();

    public native void eliminarReversoExistente();

    public native void imprimirticketPrueba();

    public native void ipVersionamiento();

    public native String mostrarValorReverso();

    public native int enviarReverso();

    public native int escribirFile(String data, int tamano);

    public native int cerrarSocket();

    public native String enviarRecaudoTarjetaEmpresarial(String track2, String monto);

    public native static String obtenerConsecutivoDeTransaccionEMV();
    public native static int verificarTransaccionesBcl();

    public native  void imprimirTransaccionBancolombiaDeclinada(String mensajeCancelacion,int tipoTransaccion);

    public native String obtenerPanRetiroEfectivo(String numeroCuenta);
    public native static void obtenerModoCnb();
    public native static void limpiarGlobalesC();
    public native String procesoAprobadoEmv(int tipoTransaccion);
    public native int validarInicializacionProgramada();
}
