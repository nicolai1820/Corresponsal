package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.pagoFacturas.tarjetaEmpresarial;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.core.comunicacion.CardDTO;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.comunes.popUp.PopUp;
import co.com.netcom.corresponsal.pantallas.funciones.BaseActivity;
import co.com.netcom.corresponsal.pantallas.funciones.MetodosSDKNewland;

public class PantallaTarjetaEmpresarialLectura extends BaseActivity {

    private Header header;
    private CardDTO tarjeta;
    private MetodosSDKNewland sdkNewland;
    public static Handler respuesta;
    private PopUp popUp;

    public final static int PROCESO_EXISTOSO =1;
    public final static int DISPOSITIVO_DESCONECTADO =2;
    public final static int ERROR_DE_LECTURA =3;
    public final static int NUEVO_INTENTO =4;
    public final static int CANCELADO_POR_USUARIO=5;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_tarjeta_empresarial_lectura);

        //Se crea el objeto header con el correspondiente titulo
        header= new Header("<b>Pago de facturas empresarial.</b>");

        //se carga el fragmento del titulo
        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderTarjetaEmpresarialLectura,header).commit();

        //Se inicializa el CardDTO
        tarjeta = new CardDTO();

        //Se inicializa el objeto para crear los pop up
        popUp = new PopUp(PantallaTarjetaEmpresarialLectura.this);

        //Se inicializa el objeto para usar los métodos del sdk de newland

        sdkNewland = new MetodosSDKNewland(PantallaTarjetaEmpresarialLectura.this);

        respuesta = new Handler() {

            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what) {

                    //Caso en el cual la tarjeta fue leida correctamente por el MPOS
                    case PROCESO_EXISTOSO: {

                        tarjeta = sdkNewland.getDatosTarjeta();

                        Intent i = new Intent(getApplicationContext(), PantallaTarjetaEmpresarialCantidad.class);
                        i.putExtra("tarjeta",tarjeta);
                        startActivity(i);
                        overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);

                        break;}

                    //Caso en el cual el dispositivo MPOS se desconecto en medio del proceso

                    case DISPOSITIVO_DESCONECTADO:{
                        //Pop up de desconexión del dispositivo
                        popUp.crearPopUp();
                        break;}

                    //Caso en el cual el dispositivo MPOS no pudo leer la tarjeta o el usuario cancelara la transacción
                    case ERROR_DE_LECTURA:{
                        Log.d("TRANSFERENCIA","ERROR TRANSFERENCIA");
                        popUp.crearPopUpError("Transacción Cancelada",popUp.PAGO_FACTURA_EMPRESARIAL);
                        break;
                    }

                    //Caso en el cual el usuario decidio intentar nuevamente la operación
                    case NUEVO_INTENTO:{

                        if (sdkNewland.isConnected()){
                            Thread nuevaLectura =new Thread(new Runnable() {

                                @Override
                                public void run() {
                                    sdkNewland.readCard("0",false, sdkNewland.TARJETA_EMPRESARIAL);
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

                    sdkNewland.readCard("0",false, sdkNewland.TARJETA_EMPRESARIAL);

                }
            }).start();
        }else {
            popUp.crearPopUp();
        }
    }




    /** Se sobreescribe el metodo nativo onBackPressed para que el usuario no pueda retroceder dentro de la aplicación*/
    @Override
    public void onBackPressed() {

    }
}