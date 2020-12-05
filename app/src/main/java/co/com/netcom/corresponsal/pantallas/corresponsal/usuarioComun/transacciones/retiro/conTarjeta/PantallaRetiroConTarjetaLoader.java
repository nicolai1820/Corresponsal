package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.retiro.conTarjeta;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.Toast;

import java.util.ArrayList;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.core.comunicacion.CardDTO;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.comunes.header.LogoComercio;
import co.com.netcom.corresponsal.pantallas.comunes.listView.ListViewDispositivos;
import co.com.netcom.corresponsal.pantallas.funciones.MetodosSDKNewland;

public class PantallaRetiroConTarjetaLoader extends AppCompatActivity {


    private Header header = new Header("<b>Retiro Con Tarjeta</b>");
    private ArrayList<String> valores = new ArrayList<String>();
    private ArrayList<String> tipoDeCuenta = new ArrayList<String>();
    private int contador;
    private CardDTO tarjeta;
    private MetodosSDKNewland sdkNewland;
    public static Handler respuesta;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_retiro_con_tarjeta_loader);

        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderPantallaRetiroConTarjetaLoader,header).commit();

        //Se recuperan los valores de la vista anterior junto con el contador para recorrerlos
        Bundle extras = getIntent().getExtras();
        valores = extras.getStringArrayList("valores");
        contador = extras.getInt("contador");
        tipoDeCuenta = extras.getStringArrayList("tipoDeCuenta");
        sdkNewland = new MetodosSDKNewland(getApplicationContext());

        tarjeta = new CardDTO();

        //Log.d("Valores",valores.toString());

        respuesta = new Handler() {

            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what) {
                    case 1: {

                        //tarjeta = sdkNewland.getDatosTarjeta();

                        /*Log.d("Track 1",tarjeta.getTrack1());
                        Log.d("Track 2",tarjeta.getTrack2());
                        Log.d("Track 3", tarjeta.getTrack3());
                        Log.d("DOCUMENTO",tarjeta.getDocumento());
                        Log.d("PAN",tarjeta.getPan());
                        Log.d("FECHA",tarjeta.getFechaExpiracion());*/


                        break;}
                    case 2:{
                        break;}

                    default:
                        break;
                }
            }

        };

        new Thread(new Runnable() {
            @Override
            public void run() {
                sdkNewland.readCard();

            }
        }).start();





    }



    @Override
    public void onBackPressed() {

    }
}