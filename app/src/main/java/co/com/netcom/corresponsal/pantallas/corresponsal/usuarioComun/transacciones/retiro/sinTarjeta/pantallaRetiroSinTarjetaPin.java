package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.retiro.sinTarjeta;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.WindowManager;
import android.widget.EditText;

import java.util.ArrayList;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.comunes.pantallaConfirmacion.pantallaConfirmacion;
import co.com.netcom.corresponsal.pantallas.comunes.popUp.PopUp;
import co.com.netcom.corresponsal.pantallas.funciones.CodigosTransacciones;
import co.com.netcom.corresponsal.pantallas.funciones.MetodosSDKNewland;

public class pantallaRetiroSinTarjetaPin extends AppCompatActivity {

    private EditText editText_RetiroSinTarjetaPin;
    private Header header = new Header("<b>Retiro sin tarjeta.</b>");
    private ArrayList<String> valores = new ArrayList<String>();
    private ArrayList<String> valoresRespaldo = new ArrayList<String>();
    private int contador;
    private int contadorRespaldo;
    private CodigosTransacciones codigo = new CodigosTransacciones();
    private MetodosSDKNewland sdkNewland;
    public static Handler respuesta;
    private PopUp popUp;
    private String pinBlock;

    public final static int PROCESO_EXISTOSO =1;
    public final static int DISPOSITIVO_DESCONECTADO =2;
    public final static int ERROR_DE_LECTURA =3;
    public final static int NUEVO_INTENTO =4;
    public final static int CANCELADO_POR_USUARIO=5;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_retiro_sin_tarjeta_pin);

        //Se recuperan los valores de la vista anterior junto con el contador para recorrerlos
        Bundle extras = getIntent().getExtras();
        valores = extras.getStringArrayList("valores");
        contador = extras.getInt("contador");

        //Se capturan los datos de respaldo
        for(int i =0; i< valores.size();i++){
            valoresRespaldo.add(valores.get(i));
        }

        contadorRespaldo = contador;

        //Se crea el respectivo header con el titulo de la activity
        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderRetiroSinTarjetaPin,header).commit();

        //Se inicializa los objetos que manejan el sdk del mpos y el que se encarga de generar los pop ups
        sdkNewland = new MetodosSDKNewland(getApplicationContext());
        popUp = new PopUp(this);


        //Se crea el correspondiente handler, para manejar el flujo de forma correcta en la aplicación

        respuesta = new Handler() {

            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what) {
                    case PROCESO_EXISTOSO: {

                        pinBlock = sdkNewland.getPinBlockFinal();

                        valores.add(pinBlock);

                        // pantalla loader transaccion pin

                        Intent i = new Intent(getApplicationContext(), PantallaRetiroSinTarjetaLoader.class);
                        i.putExtra("valores",valores);
                        startActivity(i);
                        overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);


                        break;}
                    case DISPOSITIVO_DESCONECTADO:{
                        //Pop up Desconexión
                        popUp.crearPopUp();
                        break;}
                    case ERROR_DE_LECTURA:{
                        //Pop up tarjeta invalida o transacción cancelada
                        popUp.crearPopUpError("Transacción Cancelada",popUp.RETIRO_SIN_TARJETA_PIN);
                        break;
                    }

                    case NUEVO_INTENTO:{
                        //Lectura del pin
                        if (sdkNewland.isConnected()){
                            Thread nuevaLectura =new Thread(new Runnable() {

                                @Override
                                public void run() {
                                    sdkNewland.pinBlock();
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
                    getWindow().setFlags(WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE,
                            WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE);
                    sdkNewland.pinBlock();
                }
            }).start();
        }else{
                    popUp.crearPopUp();
        }
    }


    /**Metodo nativo de android onBackPressed. Se sobreescribe para que el usuario no se pueda desplazar hacia atras
     * por medio del menu del celular.*/

    @Override
    public void onBackPressed() {

    }


    /**Metodo nativo de android onRestart. Se sobreescribe para que se eliminen los datos erroneos ingresados por
     * el usuario.*/

    @Override
    public void onRestart(){
        valores.clear();
        contador =0;

        for(int i =0; i< valoresRespaldo.size();i++){
            this.valores.add(valoresRespaldo.get(i));
        }

        this.contador =contadorRespaldo;
        super.onRestart();
    }
}