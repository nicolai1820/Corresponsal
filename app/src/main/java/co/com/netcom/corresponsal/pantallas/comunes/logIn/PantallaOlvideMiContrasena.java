package co.com.netcom.corresponsal.pantallas.comunes.logIn;

import androidx.appcompat.app.AppCompatActivity;

import android.app.AlertDialog;
import android.content.Intent;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.InputFilter;
import android.text.Spanned;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.comunes.popUp.PopUp;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.inicio.pantallaInicialUsuarioComun;
import co.com.netcom.corresponsal.pantallas.funciones.CodificarBase64;
import co.com.netcom.corresponsal.pantallas.funciones.DeviceInformation;
import co.com.netcom.corresponsal.pantallas.funciones.Servicios;

public class PantallaOlvideMiContrasena extends AppCompatActivity {

    private Header header = new Header("<b>Reestablecer la contraseña</b>");
    private EditText editTextUsuario;
    AlertDialog dialog;
    public static Handler respuesta;
    private Thread solicitudToken;
    private String token;
    private Servicios service;
    private Thread solicitudClaveTemporal;
    private String usuarioEncriptado;
    private String respuestaServicio;
    private CodificarBase64 base64 = new CodificarBase64();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_olvide_mi_contrasena);

        service = new Servicios(getApplicationContext());


        //Se crea el header de la vista
        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderPantallaOlvideMiContrasena,header).commit();

        editTextUsuario = findViewById(R.id.editText_PantallaOlvideMiContrasena);

        //Se crea este filtro para que el usuario no digite espacios en los editText
        InputFilter textFilter = new InputFilter() {

            @Override

            public CharSequence filter (CharSequence source, int start, int end, Spanned dest, int dstart, int dend){

                StringBuilder sbText = new StringBuilder(source);
                String text = sbText.toString();

                if (text.contains(" ")) {

                    return text.substring(start,end-1).toString();
                }
                return source;
            }
        };

        editTextUsuario.setFilters(new InputFilter[]{textFilter,new InputFilter.LengthFilter(10)});


        //Se crea el Handler para verificar si mostrar o no el loader.

        respuesta = new Handler(){
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what) {
                    case 1:

                        //Este error deberia ocurrir unicamente cuando el servidor no responde
                        if(token.isEmpty()){

                            dialog.dismiss();
                            PopUp popUp = new PopUp(PantallaOlvideMiContrasena.this);
                            popUp.crearPopUpErrorServidor();
                        }else{

                            //Se cre hilo para hacer la petición del inicio de sesión.
                            solicitudClaveTemporal = new Thread(new Runnable() {
                                @Override
                                public void run() {
                                    respuestaServicio= service.obtenerContrasenaTemporal(usuarioEncriptado);

                                    if(respuestaServicio.equals("1")){
                                        //Se envia un mensaje al handler de la clase consulta saldo, indicando que el usuario cancelo la transaccion
                                        Message usuarioCancela = new Message();
                                        usuarioCancela.what = 2;
                                        PantallaOlvideMiContrasena.respuesta.sendMessage(usuarioCancela);
                                    }else{
                                        //Se envia un mensaje al handler de la clase consulta saldo, indicando que el usuario cancelo la transaccion
                                        Message usuarioCancela = new Message();
                                        usuarioCancela.what = 3;
                                        PantallaOlvideMiContrasena.respuesta.sendMessage(usuarioCancela);
                                    }

                                }
                            });

                            solicitudClaveTemporal.start();
                        }


                        break;
                    case 2:
                        dialog.dismiss();

                        PopUp popUp = new PopUp(PantallaOlvideMiContrasena.this);
                        popUp.crearPopUpLoginFallido("Ha ocurrido un error en el envio del correo.");
                        break;

                    case 3:
                        dialog.dismiss();
                        PopUp popUp1 = new PopUp(PantallaOlvideMiContrasena.this);
                        popUp1.crearPopUpEnvioContrasena("Se ha enviado la contraseña al correo");
                }
            }

        };
    }




    public void solicitarClaveTemporal(View v){
        String usuario = editTextUsuario.getText().toString();

        if(usuario.isEmpty()){
            Toast.makeText(getApplicationContext(),"Debe ingresar un usuario",Toast.LENGTH_SHORT).show();
        }else{

            DeviceInformation deviceInfo = new DeviceInformation(getApplicationContext());

            if(deviceInfo.getInternetStatus()){
                usuarioEncriptado = base64.convertirBase64(usuario);

                //Se crea el loader que se mostrara mientras se procesa la transaccion
                AlertDialog.Builder loader = new AlertDialog.Builder(PantallaOlvideMiContrasena.this);

                LayoutInflater inflater = this.getLayoutInflater();

                loader.setView(inflater.inflate(R.layout.loader_procesando_transaccion,null));
                loader.setCancelable(false);

                dialog = loader.create();
                dialog.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));

                Log.d("OPEN"," se abrio el loader");
                dialog.show();

                //Se crea hilo para hacer la petición al servidor del token
                solicitudToken =  new Thread(new Runnable() {
                    @Override
                    public void run() {
                        token = service.solicitarToken();
                        Log.d("TOKEN",token.toString());
                        try{
                            //Se envia un mensaje al handler de la clase consulta saldo, indicando que el usuario cancelo la transaccion
                            Message usuarioCancela = new Message();
                            usuarioCancela.what = 1;
                            PantallaOlvideMiContrasena.respuesta.sendMessage(usuarioCancela);
                        }catch (Exception e){ }

                    }
                });

                solicitudToken.start();

            }else{
                PopUp pop = new PopUp(getApplicationContext());
                pop.crearPopUpErrorInternet();
            }

        }


    }

    @Override
    public void onBackPressed() {
        overridePendingTransition(R.anim.slide_in_left, R.anim.slide_out_right);
        super.onBackPressed();
    }
}