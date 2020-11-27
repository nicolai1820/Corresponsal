package co.com.netcom.corresponsal.pantallas.comunes.logIn;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

//import com.android.volley.Request;
//import com.android.volley.Response;
//import com.android.volley.VolleyError;
//import com.android.volley.toolbox.StringRequest;
//import com.android.volley.toolbox.Volley;

/*
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.regex.Pattern;
*/


import java.security.SecureRandom;
import java.security.cert.X509Certificate;

import javax.net.ssl.HostnameVerifier;
import javax.net.ssl.HttpsURLConnection;
import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLSession;
import javax.net.ssl.TrustManager;
import javax.net.ssl.X509TrustManager;

import co.com.netcom.corresponsal.pantallas.funciones.ConvertirBase64;

import co.com.netcom.corresponsal.R;
import okhttp3.MediaType;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.RequestBody;
import okhttp3.Response;

public class LogIn extends AppCompatActivity {

    private EditText editText_User,editText_Password;
    private TextView textView_Password, textView_Dudas;
    private ConvertirBase64 base64;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_log_in);

        //Se crea el objeto para convertir a base 64
        base64 = new ConvertirBase64();

        //Se crea la conexion con la interfaz grafica
        editText_User = (EditText) findViewById(R.id.editText_User);
        editText_Password = (EditText) findViewById(R.id.editText_Password);
        textView_Dudas = (TextView) findViewById(R.id.textView_Dudas);
        textView_Password = (TextView) findViewById(R.id.editText_Password);

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


        /** Se crea el evento click para el textview de recuperar contraseña, el cual lo lleva a la pantalla de recuperacion*/
        textView_Password.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Toast.makeText(getApplicationContext(),"Redirige a recuperar password",Toast.LENGTH_LONG);
            }
        });


        /** Se crea el evento click para el textview de tiene dudas?, el cual redirige a la pantalla de dudas.*/

        textView_Dudas.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Toast.makeText(getApplicationContext(),"Redirige a pantalla dudas",Toast.LENGTH_LONG);

            }
        });
    }


    /** Metodo inicioSesion de tipo void, el cual recibe como parametro un View view para poder ser implementado
     * por el button_inicioSesion, el cual consume el servicio de inicio de sesion*/

    public void inicioSesion(View v){

        //Se capturan los datos ingresados por el usuario
        String user = editText_User.getText().toString();
        String password = editText_Password.getText().toString();

        //Se verifica que los campos no esten vacios
        if (user.isEmpty()){
            Toast.makeText(getApplicationContext(),"Debe Ingresar un usuario",Toast.LENGTH_SHORT).show();

        } else if(password.isEmpty()){
            Toast.makeText(getApplicationContext(),"Debe Ingresar una contraseña",Toast.LENGTH_SHORT).show();
        } else{
            Log.d("USER64",base64.convertirBase64(user));
            Log.d("PASSWORD64",base64.convertirBase64(password));

            //String url = "https://192.168.215.12:9544/oauth2/token";

            /*StringRequest postRequest = new StringRequest(Request.Method.POST, url,
                    new Response.Listener<String>() {
                        @Override
                        public void onResponse(String response) {
                            Log.d("RESPUESTA",response);
                        }
                    },
                    new Response.ErrorListener() {
                        @Override
                        public void onErrorResponse(VolleyError error) {
                            Log.d("ERROR",error.toString());
                            error.printStackTrace();

                        }
                    }
            ) {
                @Override
                protected Map<String, String> getParams()
                {
                    Map<String, String>  params = new HashMap<>();
                    // the POST parameters:
                    params.put("client_id", "CFGkFIU4JKhy9cjG6HnoctOg8aQa");
                    params.put("client_secret", "Tek1bEVvPXLNb7AI5fkVVbhToEMa");
                    params.put("grant_type", "password");
                    params.put("username", "admin");
                    params.put("password", "admin");
                    return params;
                }
            };
            Volley.newRequestQueue(this).add(postRequest);*/

            Thread thread = new Thread(new Runnable() {

                @Override
                public void run() {
                    OkHttpClient client = new OkHttpClient().newBuilder()
                            .build();
                    MediaType mediaType = MediaType.parse("application/x-www-form-urlencoded");
                    RequestBody body = RequestBody.create(mediaType, "client_id=CFGkFIU4JKhy9cjG6HnoctOg8aQa&client_secret=Tek1bEVvPXLNb7AI5fkVVbhToEMa&grant_type=password&username=admin&password=admin");
                    Request request = new Request.Builder()
                            .url("https://192.168.215.12:9544/oauth2/token")
                            .method("POST", body)
                            .addHeader("Access-Control-Allow-Origin", "*")
                            .addHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS, PUT")
                            .addHeader("Content-Type", "application/x-www-form-urlencoded")
                            .build();

                    try {
                        Response response = client.newCall(request).execute();

                        Log.d("RESPUESTA",response.toString());
                    } catch (Exception e){
                        Log.d("ERROR",e.toString());
                    }
                }
            });

            thread.start();

        }
    }
}