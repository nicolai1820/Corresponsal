package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.giros.cancelar;

import androidx.appcompat.app.AppCompatActivity;

import android.app.AlertDialog;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.TextView;

import java.util.ArrayList;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.core.comunicacion.DatosComision;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.comunes.pantallaConfirmacion.pantallaConfirmacion;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.inicio.pantallaInicialUsuarioComun;
import co.com.netcom.corresponsal.pantallas.funciones.BaseActivity;
import co.com.netcom.corresponsal.pantallas.funciones.CodificarBase64;
import co.com.netcom.corresponsal.pantallas.funciones.ConstantesCorresponsal;
import co.com.netcom.corresponsal.pantallas.funciones.EncripcionAES;
import co.com.netcom.corresponsal.pantallas.funciones.Hilos;
import co.com.netcom.corresponsal.pantallas.funciones.PreferencesUsuario;
import co.com.netcom.corresponsal.pantallas.funciones.Servicios;

public class pantallaCancelarGiroCantidad extends BaseActivity {

    ArrayList<String> valores = new ArrayList<String>();
    private String valorGiro;
    private DatosComision datosComision;
    private Header header;
    private TextView textViewCantidad;
    private Hilos hiloTransacciones;
    private PreferencesUsuario prefs_parametricasBanco;
    private PreferencesUsuario prefs_parametricasUser;
    private CodificarBase64 base64;
    private EncripcionAES aes;
    private Servicios service;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_cancelar_giro_cantidad);
        overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);


        hiloTransacciones = new Hilos(getApplicationContext());

        //Se crea el header con el titulo que se rescato del intent
        header = new Header("<b>Cancelar Giro</b>");
        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderCancelarGiroCantidad,header).commit();

        //Se obtienen los datos del intent
        Bundle i = getIntent().getExtras();
        valores = i.getStringArrayList("valores");
        valorGiro = i.getString("valorGiro");
        datosComision = new DatosComision();
        datosComision = i.getParcelable("datosComision");


        //Se hace la conexion con la interfaz grafica
        textViewCantidad = (TextView) findViewById(R.id.textView_CantidadGiro);

        textViewCantidad.setText(valorGiro);

        //PREFERENCES FIID Y TIPODECUENTA
        prefs_parametricasBanco = new PreferencesUsuario(ConstantesCorresponsal.SHARED_PREFERENCES_PARAMETRICAS_BANCO,this);
        prefs_parametricasUser = new PreferencesUsuario(ConstantesCorresponsal.SHARED_PREFERENCES_INFO_USUARIO,this);

        //OBJETOS PARA DECODIFICAR EL PAN
        base64 = new CodificarBase64();
        aes = new EncripcionAES();

        //OBJETO PARA CONSUMIR SERVICIOS
        service = new Servicios(this);

    }


    public void continuarCancelarGiroCantidad(View view){

        AlertDialog.Builder loader = new AlertDialog.Builder(pantallaCancelarGiroCantidad.this);

        LayoutInflater inflater = this.getLayoutInflater();

        loader.setView(inflater.inflate(R.layout.loader,null));
        loader.setCancelable(false);

        AlertDialog dialog = loader.create();
        Log.d("OPEN"," se abrio el loader cancelacion");


        dialog.show();


        new Thread() {
            @Override
            public void run() {

                try {
                    hiloTransacciones.cancelarGiro(valores,datosComision,prefs_parametricasBanco.getFiidID(),prefs_parametricasBanco.getTipoCuenta(),aes.decrypt(prefs_parametricasUser.getEncryptionKey(),base64.decodificarBase64(service.obtenerPanVirtual())));
                } catch (Exception e) {
                    e.printStackTrace();
                }
                try {
                    // code runs in a thread
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            Log.d("CLOSE"," se cerro el loader cancelacion");
                            dialog.dismiss();
                        }
                    });
                } catch (final Exception ex) {

                }
            }
        }.start();

        //Se hace el intent a el resultado de la cancelacion


    }

    public void cancelarCancelarGiroCantidad(View view){
        Intent i = new Intent(getApplicationContext(), pantallaInicialUsuarioComun.class);
        startActivity(i);
        overridePendingTransition(R.anim.slide_in_left,R.anim.slide_out_right);
    }

    public void onBackPressed(){

    }

}