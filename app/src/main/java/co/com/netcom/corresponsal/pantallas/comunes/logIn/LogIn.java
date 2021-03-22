package co.com.netcom.corresponsal.pantallas.comunes.logIn;

import androidx.annotation.UiThread;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.app.AlertDialog;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.InputFilter;
import android.text.Spanned;
import android.util.Base64;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

import javax.crypto.Cipher;
import javax.crypto.KeyGenerator;
import javax.crypto.SecretKey;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.SecretKeySpec;

import co.com.netcom.corresponsal.pantallas.comunes.pantallaConfirmacion.pantallaConfirmacion;
import co.com.netcom.corresponsal.pantallas.comunes.popUp.PopUp;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.inicio.pantallaInicialUsuarioComun;
import co.com.netcom.corresponsal.pantallas.funciones.CodificarBase64;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.funciones.CodificarSHA512;
import co.com.netcom.corresponsal.pantallas.funciones.ConstantesCorresponsal;
import co.com.netcom.corresponsal.pantallas.funciones.DeviceInformation;
import co.com.netcom.corresponsal.pantallas.funciones.EncripcionAES;
import co.com.netcom.corresponsal.pantallas.funciones.PreferencesUsuario;
import co.com.netcom.corresponsal.pantallas.funciones.Servicios;

public class LogIn extends AppCompatActivity {

    private EditText editText_User,editText_Password;
    private TextView textView_Password, textView_CambiarContraseña;
    private CodificarBase64 base64;
    private CodificarSHA512 sha512;
    private Thread solicitudToken;
    private Thread solicitudInicioSesion;
    private Thread solicitarParametricas;
    private UiThread loader;
    private String token;
    private String usuarioEncriptado;
    private String contrasenaEncriptada;
    private String estadoConexion= null;
    private Servicios objetoServicios =null;
    private PopUp popUp;
    SharedPreferences sharedPreferences;
    SharedPreferences.Editor sharedPreferencesEditor;
    private int sesion;
    AlertDialog dialog;
    public static Handler respuesta;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_log_in);

        //Se cre el objeto para crear los pop up
        popUp = new PopUp(LogIn.this);

        //Se crea el objeto para convertir a base 64 y sha 512
        base64 = new CodificarBase64();
        sha512 = new CodificarSHA512();

        //Se cre el objeto para hacer las peticiones http
        objetoServicios = new Servicios(getApplicationContext());

        //Se crea la conexion con la interfaz grafica
        editText_User = (EditText) findViewById(R.id.editText_User);
        editText_Password = (EditText) findViewById(R.id.editText_Password);
        textView_Password = (TextView) findViewById(R.id.textView_PasswordLogin);
        textView_CambiarContraseña = findViewById(R.id.textView_CambiarContrasena);

        sharedPreferences = getSharedPreferences("User",MODE_PRIVATE);
        sharedPreferencesEditor = sharedPreferences.edit();

        //se trata de rescartar el intent en caso de que se cierre por timeout
        Bundle i = getIntent().getExtras();
        try {
            sesion = i.getInt("sesion",0);
        }catch (Exception e){}

        //se crea popUp sesion cerrada por timeout
        if (sesion==1){
            popUp.crearPopUpLoginFallido("Sesión cerrada por time out.");
        }

        //Se crea un elemento para verificar que los permisos esten activados
        int permisoUbicacionCoarse = ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_COARSE_LOCATION);
        int permisoUbicacionFine = ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION);
        int permisoAlmacenamiento = ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE);

        if (permisoAlmacenamiento != PackageManager.PERMISSION_GRANTED || permisoUbicacionCoarse!=PackageManager.PERMISSION_GRANTED ||permisoUbicacionFine!=PackageManager.PERMISSION_GRANTED){
            ActivityCompat.requestPermissions(this,new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE,
                    Manifest.permission.ACCESS_COARSE_LOCATION,
                    Manifest.permission.ACCESS_FINE_LOCATION,Manifest.permission.READ_PHONE_STATE},0);
        } else{
                Log.d("PERMISOS","Si hay permisos");
        }

        /** Se crea el evento click para el textview de recuperar contraseña, el cual crea un pop up donde se recibe el usuario
         * para posteriormente consumir el servicio de recuperar contraseña.*/
        textView_CambiarContraseña.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                //popUp.crearPopUpRecuperarContrasena();
                //Preguntar porque no se como se obtiene el userid, cuando no hay un inicio de sesión previo
                Intent i = new Intent(getApplicationContext() , PantallaCambioContrasena.class);
                startActivity(i);
                overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);
            }
        });

        /**Se cre el evento click para el text view olvide mi contraseña, el cual crea un pop up donde se recibo el usuario para
         * posteriormente */

        textView_Password.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                popUp.crearPopUpRecuperarContrasena();
             /*   Intent i = new Intent(getApplicationContext() , PantallaCambioContrasena.class);
                startActivity(i);
                overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);*/
            }
        });

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

        //Se agrega el filtro para que no se acepten espacios en el editext
        editText_User.setFilters(new InputFilter[]{textFilter,new InputFilter.LengthFilter(10)});
        editText_Password.setFilters(new InputFilter[]{textFilter,new InputFilter.LengthFilter(15)});

        //Se crea el Handler para verificar si mostrar o no el loader.

        respuesta = new Handler(){
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what) {
                    case 1:

                        //Este error deberia ocurrir unicamente cuando el servidor no responde
                        if(token.isEmpty()){

                      /*      PopUp popUp = new PopUp(LogIn.this);
                            popUp.crearPopUpErrorServidor();*/
                            Intent i = new Intent(getApplicationContext(), pantallaInicialUsuarioComun.class);
                            startActivity(i);
                            overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);
                            dialog.dismiss();
                        }else{

                  //Se cre hilo para hacer la petición del inicio de sesión.
                            solicitudInicioSesion = new Thread(new Runnable() {
                                @Override
                                public void run() {
                                    Log.d("HILO2","Segundo Hilo");
                                    estadoConexion= base64.decodificarBase64(objetoServicios.Login(usuarioEncriptado,contrasenaEncriptada,token));
                                    //Se envia un mensaje al handler de la clase consulta saldo, indicando que el usuario cancelo la transaccion
                                    Message usuarioCancela = new Message();
                                    usuarioCancela.what = 4;
                                    LogIn.respuesta.sendMessage(usuarioCancela);
                                }
                            });

                            solicitudInicioSesion.start();

                          /*  estadoConexion="1";
                            Message usuarioCancela = new Message();
                            usuarioCancela.what = 4;
                            LogIn.respuesta.sendMessage(usuarioCancela);*/

                        }


                        break;

                    case 2:
                        dialog.dismiss();

                        break;
                    case 3:
                        dialog.dismiss();
                        PopUp op = new PopUp(LogIn.this);
                        op.crearPopUpLoginFallido("No se descargaron las parametricas");

                        break;

                        case 4:
                            if (Integer.parseInt(estadoConexion)==1){
                /*      EncripcionAES en = new EncripcionAES();

                      try {
                          PreferencesUsuario prefs_enc = new PreferencesUsuario(ConstantesCorresponsal.SHARED_PREFERENCES_INFO_USUARIO,this);
                          CodificarBase64 b = new CodificarBase64();
                          Log.d("AES", en.encrypt(b.decodificarBase64(prefs_enc.getEncryptionKey()),"Holis"));

                          Log.d("AES LIMPIO",en.decrypt(b.decodificarBase64(prefs_enc.getEncryptionKey()), en.encrypt(b.decodificarBase64(prefs_enc.getEncryptionKey()),"Holis")));




                      }catch (Exception e){
                          Log.d("EXC",e.toString());
                      }*/


                                solicitarParametricas = new  Thread(new Runnable() {
                                    @Override
                                    public void run() {
                                        //objetoServicios.obtenerParametricas(objetoServicios.getUserId(),getApplicationContext());
                                        objetoServicios.obtenerParametricas();

                                    }
                                });

                                solicitarParametricas.start();
               /*            try {
                               solicitarParametricas.join();
                           } catch (InterruptedException e) {
                               e.printStackTrace();
                           }*/

                            }
                            else{
                                dialog.dismiss();
                                //popUp.crearPopUpLoginFallido(base64.decodificarBase64(objetoServicios.getRespuestaServidor()));
                                Intent i = new Intent(getApplicationContext(), pantallaInicialUsuarioComun.class);
                                startActivity(i);
                                overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);

                            }


                            break;
                    case 5:
                            Intent i = new Intent(getApplicationContext(), pantallaInicialUsuarioComun.class);
                                startActivity(i);
                                overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);
                        dialog.dismiss();
                           break;
                }
            }

        };

    }


    /** Metodo inicioSesion de tipo void, el cual recibe como parametro un View view para poder ser implementado
     * por el button_inicioSesion, el cual consume el servicio de inicio de sesion*/

    public void inicioSesion(View v)  {
        DeviceInformation deviceInformation = new DeviceInformation(this);
        boolean internet = deviceInformation.getInternetStatus();
        boolean ubicacion = deviceInformation.getLocationStatus();
           //Se capturan los datos ingresados por el usuario
           String user = editText_User.getText().toString();
           String password = editText_Password.getText().toString();

           //Se verifica que los campos no esten vacios
           if (user.isEmpty()){
               Toast.makeText(getApplicationContext(),"Debe Ingresar un usuario",Toast.LENGTH_SHORT).show();

           }
           else if(password.isEmpty()){
               Toast.makeText(getApplicationContext(),"Debe Ingresar una contraseña",Toast.LENGTH_SHORT).show();
           }
           else{
          if(internet){

               //Se crea el loader que se mostrara mientras se procesa la transaccion
               AlertDialog.Builder loader = new AlertDialog.Builder(LogIn.this);

               LayoutInflater inflater = this.getLayoutInflater();

               loader.setView(inflater.inflate(R.layout.loader_procesando_transaccion,null));
               loader.setCancelable(false);

               dialog = loader.create();
               dialog.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));

               Log.d("OPEN"," se abrio el loader");
               dialog.show();

              usuarioEncriptado=base64.convertirBase64(user);
              contrasenaEncriptada=base64.convertirBase64(sha512.codificarSHA512(password));

              Log.d("USUARIODECO",base64.decodificarBase64(usuarioEncriptado));

              //Se crea hilo para hacer la petición al servidor del token
                  solicitudToken =  new Thread(new Runnable() {
                      @Override
                      public void run() {
                         token = objetoServicios.solicitarToken();
                          Log.d("TOKEN",token.toString());
                          try{
                              //Se envia un mensaje al handler de la clase consulta saldo, indicando que el usuario cancelo la transaccion
                              Message usuarioCancela = new Message();
                              usuarioCancela.what = 1;
                              LogIn.respuesta.sendMessage(usuarioCancela);
                          }catch (Exception e){ }

                      }
                  });

                   solicitudToken.start();

               }
          else{
                   Log.d("Internet Desactivado","No internet");
                   PopUp popUp = new PopUp(this);
                   popUp.crearPopUpErrorInternet();
               }
           }

    }

    @Override
    public void onBackPressed() {

    }
}