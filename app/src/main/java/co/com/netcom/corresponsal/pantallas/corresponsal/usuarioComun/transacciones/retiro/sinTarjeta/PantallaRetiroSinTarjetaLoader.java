package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.retiro.sinTarjeta;

import androidx.appcompat.app.AppCompatActivity;

import android.app.AlertDialog;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.WindowManager;

import java.util.ArrayList;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.comunes.pantallaConfirmacion.pantallaConfirmacion;
import co.com.netcom.corresponsal.pantallas.funciones.BaseActivity;
import co.com.netcom.corresponsal.pantallas.funciones.CodificarBase64;
import co.com.netcom.corresponsal.pantallas.funciones.CodigosTransacciones;
import co.com.netcom.corresponsal.pantallas.funciones.ConstantesCorresponsal;
import co.com.netcom.corresponsal.pantallas.funciones.EncripcionAES;
import co.com.netcom.corresponsal.pantallas.funciones.Hilos;
import co.com.netcom.corresponsal.pantallas.funciones.PreferencesUsuario;
import co.com.netcom.corresponsal.pantallas.funciones.Servicios;

public class PantallaRetiroSinTarjetaLoader extends BaseActivity {
    private ArrayList<String> valores = new ArrayList<String>();
    private Header header = new Header("<b>Retiro sin tarjeta.</b>");
    private Hilos hilos;
    private CodigosTransacciones codigosTransacciones;
    private PreferencesUsuario prefs_parametricasBanco;
    private PreferencesUsuario prefs_parametricasUser;
    private CodificarBase64 base64;
    private EncripcionAES aes;
    private Servicios service;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_retiro_sin_tarjeta_loader);

        hilos = new Hilos(getApplicationContext());
        codigosTransacciones = new CodigosTransacciones();


        //Se reeemplaza el respectio header de la actividad
        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderPantallaRetiroSinTarjetaLoader,header).commit();


        Bundle i = getIntent().getExtras();
        valores = i.getStringArrayList("valores");

        //Se crea el loader que se mostrara mientras se procesa la transaccion
        AlertDialog.Builder loader = new AlertDialog.Builder(PantallaRetiroSinTarjetaLoader.this);

        LayoutInflater inflater = this.getLayoutInflater();

        loader.setView(inflater.inflate(R.layout.loader_procesando_transaccion,null));
        loader.setCancelable(false);

        AlertDialog dialog = loader.create();
        dialog.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));
        dialog.show();
        //PREFERENCES FIID Y TIPODECUENTA
        prefs_parametricasBanco = new PreferencesUsuario(ConstantesCorresponsal.SHARED_PREFERENCES_PARAMETRICAS_BANCO,this);
        prefs_parametricasUser = new PreferencesUsuario(ConstantesCorresponsal.SHARED_PREFERENCES_INFO_USUARIO,this);

        //OBJETOS PARA DECODIFICAR EL PAN
        base64 = new CodificarBase64();
        aes = new EncripcionAES();

        //OBJETO PARA CONSUMIR SERVICIOS
        service = new Servicios(this);


        //Se agrega para que no puedan salir de la transaccion mientras se procesa
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE,
                WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE);

        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    hilos.transaccionesSinTarjeta(codigosTransacciones.CORRESPONSAL_RETIRO_SIN_TARJETA,valores,prefs_parametricasBanco.getFiidID(),prefs_parametricasBanco.getTipoCuenta(),aes.decrypt(prefs_parametricasUser.getEncryptionKey(),service.obtenerPanVirtual()));
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }).start();


    }


    /**Se sobre escribe el metodo nativo onBackPressed, para que el usuario no pueda interrumpir el flujo de la transacción.*/
    @Override
    public void onBackPressed() {
    }
}