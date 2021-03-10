package co.com.netcom.corresponsal.pantallas.comunes.logIn;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.text.InputFilter;
import android.text.Spanned;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import co.com.netcom.corresponsal.pantallas.comunes.popUp.PopUp;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.inicio.pantallaInicialUsuarioComun;
import co.com.netcom.corresponsal.pantallas.funciones.CodificarBase64;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.funciones.CodificarSHA512;
import co.com.netcom.corresponsal.pantallas.funciones.Servicios;

public class LogIn extends AppCompatActivity {

    private EditText editText_User,editText_Password;
    private TextView textView_Password, textView_Dudas;
    private CodificarBase64 base64;
    private CodificarSHA512 sha512;
    private Thread solicitudToken;
    private Thread solicitudInicioSesion;
    private String token;
    private String usuarioEncriptado;
    private String contrasenaEncriptada;
    private String estadoConexion= null;
    private Servicios servicioLogin=null;
    private PopUp popUp;
    SharedPreferences sharedPreferences;
    SharedPreferences.Editor sharedPreferencesEditor;
    private int sesion;


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
        servicioLogin = new Servicios(getApplicationContext());

        //Se crea la conexion con la interfaz grafica
        editText_User = (EditText) findViewById(R.id.editText_User);
        editText_Password = (EditText) findViewById(R.id.editText_Password);
        textView_Password = (TextView) findViewById(R.id.textView_PasswordLogin);

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
                    Manifest.permission.ACCESS_FINE_LOCATION},0);
        } else{
                Log.d("PERMISOS","Si hay permisos");
        }

        /** Se crea el evento click para el textview de recuperar contraseña, el cual crea un pop up donde se recibe el usuario
         * para posteriormente consumir el servicio de recuperar contraseña.*/
        textView_Password.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                popUp.crearPopUpRecuperarContrasena();
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
        editText_User.setFilters(new InputFilter[]{textFilter});
        editText_Password.setFilters(new InputFilter[]{textFilter});



    }


    /** Metodo inicioSesion de tipo void, el cual recibe como parametro un View view para poder ser implementado
     * por el button_inicioSesion, el cual consume el servicio de inicio de sesion*/

    public void inicioSesion(View v){

        //Se capturan los datos ingresados por el usuario
      /*  String user = editText_User.getText().toString();
        String password = editText_Password.getText().toString();

        //Se verifica que los campos no esten vacios
        if (user.isEmpty()){
            Toast.makeText(getApplicationContext(),"Debe Ingresar un usuario",Toast.LENGTH_SHORT).show();

        } else if(password.isEmpty()){
            Toast.makeText(getApplicationContext(),"Debe Ingresar una contraseña",Toast.LENGTH_SHORT).show();
        } else{

            usuarioEncriptado=base64.convertirBase64(user);
            contrasenaEncriptada=base64.convertirBase64(sha512.codificarSHA512(password));

           Log.d("USUARIODECO",base64.decodificarBase64(usuarioEncriptado));

            //Se crea hilo para hacer la petición al servidor del token
            solicitudToken =  new Thread(new Runnable() {
                @Override
                public void run() {
                    token = servicioLogin.solicitarToken();
                    Log.d("TOKEN",token.toString());
                }
            });

            solicitudToken.start();
            try {
                solicitudToken.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            //Se cre hilo para hacer la petición del inicio de sesión.
            solicitudInicioSesion = new Thread(new Runnable() {
                @Override
                public void run() {
                    Log.d("HILO2","Segundo Hilo");
                    estadoConexion= base64.decodificarBase64(servicioLogin.Login(usuarioEncriptado,contrasenaEncriptada,token));

                    Log.d("LOGIN",estadoConexion);

                }
           });

            solicitudInicioSesion.start();
            try {
                solicitudInicioSesion.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }


            //Log.d("ESTADODECODIFICADO",base64.decodificarBase64(estadoConexion));


            //Log.d("ESTADO",base64.decodificarBase64(estadoConexion));

            if (Integer.parseInt(estadoConexion)==1){
                sharedPreferencesEditor.putString("Usuario",servicioLogin.getUserId());
                sharedPreferencesEditor.commit();

                Intent i = new Intent(this, pantallaInicialUsuarioComun.class);
                startActivity(i);
                overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);
            }else{
                popUp.crearPopUpLoginFallido(base64.decodificarBase64(servicioLogin.getRespuestaServidor()));
            }*/

            Intent i = new Intent(this, pantallaInicialUsuarioComun.class);
            startActivity(i);
            overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);

       // }
    }



}