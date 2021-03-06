package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.consultaSaldo;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.MenuItem;
import android.view.WindowManager;

import com.google.android.material.bottomnavigation.BottomNavigationView;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.core.comunicacion.CardDTO;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.comunes.popUp.PopUp;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.inicio.pantallaInicialUsuarioComun;
import co.com.netcom.corresponsal.pantallas.funciones.BaseActivity;
import co.com.netcom.corresponsal.pantallas.funciones.MetodosSDKNewland;

public class pantallaConsultaSaldoLectura extends BaseActivity {

    public final static int PROCESO_EXISTOSO =1;
    public final static int DISPOSITIVO_DESCONECTADO =2;
    public final static int ERROR_DE_LECTURA =3;
    public final static int NUEVO_INTENTO =4;
    public final static int CANCELADO_POR_USUARIO=5;
    public final static int CONTACTLESS=6;





    private Header header = new Header("<b>Consulta de saldo.</b>");
    private CardDTO tarjeta;
    private MetodosSDKNewland sdkNewland;
    public static Handler respuesta;
    private PopUp popUp;
    private BottomNavigationView menuConsultaSaldos;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_consulta_saldo_lectura);


        //Se carga el contenido del header
        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderPantallaConsultaDeSaldoLectura,header).commit();

        //Se inicializa el CardDTO
        tarjeta = new CardDTO();

        //Se inicializa el objeto para crear los pop up
        popUp = new PopUp(pantallaConsultaSaldoLectura.this);

        //Se inicializa el objeto para usar los m??todos del sdk de newland

        sdkNewland = new MetodosSDKNewland(pantallaConsultaSaldoLectura.this);

        menuConsultaSaldos = (BottomNavigationView) findViewById(R.id.menuConsultaSaldo);

        //Se crea el evento click de la barra de navegacion

        menuConsultaSaldos.setOnNavigationItemSelectedListener(new BottomNavigationView.OnNavigationItemSelectedListener() {
            @Override
            public boolean onNavigationItemSelected(@NonNull MenuItem menuItem) {

                if(menuItem.getItemId() == R.id.home_general){
                    Intent i = new Intent(getApplicationContext(), pantallaInicialUsuarioComun.class);
                    startActivity(i);
                    overridePendingTransition(R.anim.slide_in_left,R.anim.slide_out_right);

                }

                return true;
            }
        });


        respuesta = new Handler() {

            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what) {

                    //Caso en el cual la tarjeta fue leida correctamente por el MPOS
                    case PROCESO_EXISTOSO: {

                        tarjeta = sdkNewland.getDatosTarjeta();

                        Log.d("TARJETA SALDO",tarjeta.toString());

                        Intent i = new Intent(getApplicationContext(), pantallaConsultaSaldoTipoCuenta.class);
                        i.putExtra("tarjeta",tarjeta);
                        startActivity(i);
                        overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);


                        break;}

                    //Caso en el cual el dispositivo MPOS se desconecto en medio del proceso

                    case DISPOSITIVO_DESCONECTADO:{
                        //Pop up de desconexi??n del dispositivo
                        popUp.crearPopUp();
                        break;}

                    //Caso en el cual el dispositivo MPOS no pudo leer la tarjeta o el usuario cancelara la transacci??n
                    case ERROR_DE_LECTURA:{
                        Log.d("CONSULTA SALDO","ERROR CONSULTA");
                        popUp.crearPopUpError("Transacci??n Cancelada",popUp.CONSULTA_SALDO_LEER_TARJETA);
                        break;
                    }

                    //Caso en el cual el usuario decidio intentar nuevamente la operaci??n
                    case NUEVO_INTENTO:{

                        if (sdkNewland.isConnected()){
                            Thread nuevaLectura =new Thread(new Runnable() {

                                @Override
                                public void run() {
                                    sdkNewland.readCard("0",false,sdkNewland.CONSULTA_SALDO);
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
                    case CONTACTLESS:
                        popUp.crearPopUpContactless("Transacci??n no soportada por Contactless");
                        break;
                    default:
                        break;
                }
            }

        };


        //Se valida que el dispositivo este conectado, de no se as?? emite un pop up para notificar

        if (sdkNewland.isConnected()){
            new Thread(new Runnable() {
                @Override
                public void run() {
                    getWindow().setFlags(WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE,
                            WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE);
                    sdkNewland.readCard("0",false,sdkNewland.CONSULTA_SALDO);


                }
            }).start();
        }else {
            popUp.crearPopUp();
        }

    }


    /** Se sobreescribe el metodo nativo onBackPressed para que el usuario no pueda retroceder dentro de la aplicaci??n*/
    @Override
    public void onBackPressed() {

    }



}