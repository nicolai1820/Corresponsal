package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.retiro.sinTarjeta;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.WindowManager;

import java.util.ArrayList;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.funciones.CodigosTransacciones;
import co.com.netcom.corresponsal.pantallas.funciones.Hilos;

public class PantallaRetiroSinTarjetaLoader extends AppCompatActivity {
    private ArrayList<String> valores = new ArrayList<String>();
    private Header header = new Header("<b>Retiro sin tarjeta.</b>");
    private Hilos hilos;
    private CodigosTransacciones codigosTransacciones;

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

        //Se agrega para que no puedan salir de la transaccion mientras se procesa
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE,
                WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE);

        new Thread(new Runnable() {
            @Override
            public void run() {
                hilos.transaccionesSinTarjeta(codigosTransacciones.CORRESPONSAL_RETIRO_SIN_TARJETA,valores);
            }
        }).start();


    }


    /**Se sobre escribe el metodo nativo onBackPressed, para que el usuario no pueda interrumpir el flujo de la transacción.*/
    @Override
    public void onBackPressed() {
    }
}