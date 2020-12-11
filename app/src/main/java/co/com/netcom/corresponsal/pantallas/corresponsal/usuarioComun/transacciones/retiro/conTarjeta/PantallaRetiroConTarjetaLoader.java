package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.retiro.conTarjeta;

import androidx.appcompat.app.AppCompatActivity;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.Intent;
import android.graphics.PixelFormat;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.Spinner;
import android.widget.Toast;

import java.util.ArrayList;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.core.comunicacion.CardDTO;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.comunes.header.LogoComercio;
import co.com.netcom.corresponsal.pantallas.comunes.listView.ListViewDispositivos;
import co.com.netcom.corresponsal.pantallas.comunes.pantallaConfirmacion.pantallaConfirmacion;
import co.com.netcom.corresponsal.pantallas.comunes.popUp.PopUpDesconexion;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.inicio.pantallaInicialUsuarioComun;
import co.com.netcom.corresponsal.pantallas.funciones.MetodosSDKNewland;

public class PantallaRetiroConTarjetaLoader extends AppCompatActivity {


    private Header header = new Header("<b>Retiro Con Tarjeta</b>");
    private ArrayList<String> valores = new ArrayList<String>();
    private ArrayList<String> tipoDeCuenta = new ArrayList<String>();
    private int contador;
    private CardDTO tarjeta;
    private MetodosSDKNewland sdkNewland;
    public static Handler respuesta;
    private PopUpDesconexion popUp;


    public final static int PROCESO_EXISTOSO =1;
    public final static int DISPOSITIVO_DESCONECTADO =2;
    public final static int ERROR_DE_LECTURA =3;
    public final static int NUEVO_INTENTO =4;
    public final static int CANCELADO_POR_USUARIO=5;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_retiro_con_tarjeta_loader);


        //Se carga el header de la correspondiente actividad
        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderPantallaRetiroConTarjetaLoader,header).commit();

        //Se recuperan los valores de la vista anterior junto con el contador para recorrerlos
        Bundle extras = getIntent().getExtras();
        valores = extras.getStringArrayList("valores");
        contador = extras.getInt("contador");
        tipoDeCuenta = extras.getStringArrayList("tipoDeCuenta");

        //Se inicializa el objeto para usar el SDK de newland
        sdkNewland = new MetodosSDKNewland(getApplicationContext());

        //Se inicializa el objeto para crear los pop up
        popUp = new PopUpDesconexion(this);

        //Se inicializa el objeto para guardar los tipos de tarjeta
        tarjeta = new CardDTO();


        //Log.d("Valores",valores.toString());

        respuesta = new Handler() {

            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what) {
                    case PROCESO_EXISTOSO: {

                        tarjeta = sdkNewland.getDatosTarjeta();

                        Log.d("tarjeta",tarjeta.toString());


                        break;}
                    case DISPOSITIVO_DESCONECTADO:{
                        //Pop up de desconexión del dispositivo
                        popUp.crearPopUp();
                        break;}
                    case ERROR_DE_LECTURA:{
                        popUp.crearPopUpError("Transacción Cancelada",popUp.RETIRO_CON_TARJETA);
                        break;
                    }

                    case NUEVO_INTENTO:{

                        if (sdkNewland.isConnected()){
                            Thread nuevaLectura =new Thread(new Runnable() {

                                @Override
                                public void run() {
                                    sdkNewland.readCard("0",false);
                                }
                            });

                            try {
                                nuevaLectura.join();
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                            nuevaLectura.start();
                        }else{
                                    popUp.crearPopUp();
                        }

                        break;
                    }
                    case CANCELADO_POR_USUARIO:
                        popUp.crearPopUpCanceladaUsuario();
                        break;
                    default:
                        break;
                }
            }

        };

        //Se valida que el dispositivo este conectado, de no se así emite un pop up para notificar


        if (sdkNewland.isConnected()){
            new Thread(new Runnable() {
                @Override
                public void run() {
                    sdkNewland.readCard("0",false);
                }
            }).start();
        }else{
            popUp.crearPopUp();
        }


    }


    /** Se sobreescribe el metodo nativo onBackPressed para que el usuario no pueda retroceder dentro de la aplicación*/


    @Override
    public void onBackPressed() {

    }


}