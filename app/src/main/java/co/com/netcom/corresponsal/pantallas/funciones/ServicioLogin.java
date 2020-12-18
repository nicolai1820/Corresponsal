package co.com.netcom.corresponsal.pantallas.funciones;

import android.content.Context;
import android.util.Log;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;
import java.io.InputStream;
import java.security.KeyManagementException;
import java.security.KeyStore;
import java.security.KeyStoreException;
import java.security.NoSuchAlgorithmException;
import java.security.SecureRandom;
import java.security.cert.CertificateException;
import java.security.cert.CertificateFactory;

import javax.net.ssl.HostnameVerifier;
import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLSession;
import javax.net.ssl.TrustManagerFactory;

import okhttp3.MediaType;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.RequestBody;
import okhttp3.Response;

public class ServicioLogin {

    private String respuestaServidor;

    private static String urlToken = "https://192.168.215.12:9544/oauth2/token";
    private static String urlLogin = "https://192.168.215.12:8520/netcom/merchant/api/users/sessions";
    private Context context;

    public ServicioLogin(Context contexto){
        this.context = contexto;
    }

    /**Metodo cerrarSesion de tipo void que consume el servicio rest para solicitar el token*/

    public String solicitarToken(){

        //Metodo para funcionar con cualquier ssl
        OkHttpClient.Builder builder = new OkHttpClient.Builder();
        builder.hostnameVerifier(new HostnameVerifier() {
            @Override
            public boolean verify(String hostname, SSLSession session) {
                return true;
            }
        });


        OkHttpClient client = builder.sslSocketFactory(getSLLContext().getSocketFactory()).build();



        MediaType mediaType = MediaType.parse("application/x-www-form-urlencoded");
        RequestBody body = RequestBody.create(mediaType, "client_id=CFGkFIU4JKhy9cjG6HnoctOg8aQa&client_secret=Tek1bEVvPXLNb7AI5fkVVbhToEMa&grant_type=password&username=admin&password=admin");
        Request request = new Request.Builder()
                .url(urlToken)
                .method("POST", body)
                .addHeader("Access-Control-Allow-Origin", "*")
                .addHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS, PUT")
                .addHeader("Content-Type", "application/x-www-form-urlencoded")
                .build();

        try {
            Response response = client.newCall(request).execute();
            String jsonData = response.body().string();
            JSONObject Jobject = new JSONObject(jsonData);


            return Jobject.getString("access_token");

        } catch (Exception e){
            throw new RuntimeException(e);
        }
    }

    /**Metodo cerrarSesion, retorna un String, que determina si la respuesta del servidor fue fallida o exitosa, recibe como parametros un String
     * el cual es el token que requieren todos los servicios, un String el cual es el usuario y otro String el cual es la contraseña, este metodo,
     * se encarga de consumir el servicio de Login*/

    public String Login(String usuario,String contrasena, String token){

        //Se debe sobreescribir este metodo para que acepte cualquier certificado seguro.
        OkHttpClient.Builder builder = new OkHttpClient.Builder();
        builder.hostnameVerifier(new HostnameVerifier() {
            @Override
            public boolean verify(String hostname, SSLSession session) {
                return true;
            }
        });

        Log.d("USUARIO SERVICIO",String.valueOf(usuario.length()));
        Log.d("CONTRASEÑA",contrasena);

        OkHttpClient client2 = builder.sslSocketFactory(getSLLContext().getSocketFactory()).build();

        MediaType mediaType = MediaType.parse("application/json");
        String body_string = "{\"user\": \""+usuario+"\",\"channel\": \"MQ==\",\"password\": \""+contrasena+"\",\"uuid\": \"NWEyODIxZTBiZTE4MDQ2NA==\",\"ipAddress\": \"MTcyLjE3LjEuMTI0\",\"version\":\"MjIx\"}";
        RequestBody body = RequestBody.create(mediaType,body_string);
        //RequestBody body = RequestBody.create(mediaType, "{\"user\":\"${usuario}\",\"channel\": \"MQ==\",\"password\": \"NDQ1YzcyM2U3MmY5NjE1OWIyODI2MDNhZmMyNDAwYWRhNTNiMTQyNDQ3MGI0ZmExZGJjOGU2ZTEzNDRhZjMzNjU1M2RiNjBhM2EwYzY3NDM4YTdiYTE5ZmZlYjJhMWM0OTZmMjkyZWNlN2RhY2U0ZmQxODJkMGQ0N2E4NWU5NjA=\",\"uuid\": \"NWEyODIxZTBiZTE4MDQ2NA==\",\"ipAddress\": \"MTcyLjE3LjEuMTI0\",\"version\":\"MjIx\"}");

            Request request = new Request.Builder()

                .url(urlLogin)
                .method("POST", body)
                .addHeader("Content-Type", "application/json")
                .addHeader("Accept", "text/*;q=0.3, text/html;q=0.7, text/html;level=1,text/html;level=2;q=0.4, */*;q=0.5")
                .addHeader("Authorization", "Bearer "+token)
                .build();
        try {
            Response response = client2.newCall(request).execute();

            String jsonData = response.body().string();
            JSONObject Jobject = new JSONObject(jsonData);
            Log.d("RESPUESTA",Jobject.toString());
            respuestaServidor = Jobject.getString("responseCode");
            return Jobject.getString("loginState");

        } catch (IOException | JSONException e) {
            throw new RuntimeException(e);        }

    }

    //Metodo provicional para que se pueda hacer login al servidor local deibo al problema del certificado autofirmado
    private SSLContext getSLLContext() {
        SSLContext sslContext = null;
        try {
            CertificateFactory certificateFactory = CertificateFactory.getInstance("X.509");
            InputStream certificate = context.getAssets().open("localhost.pem");
            KeyStore keyStore = KeyStore.getInstance(KeyStore.getDefaultType());
            keyStore.load(null);
            String certificateAlias = Integer.toString(0);
            keyStore.setCertificateEntry(certificateAlias, certificateFactory.generateCertificate(certificate));
            sslContext = SSLContext.getInstance("TLS");
            final TrustManagerFactory trustManagerFactory = TrustManagerFactory.getInstance(TrustManagerFactory.getDefaultAlgorithm());
            trustManagerFactory.init(keyStore);
            sslContext.init(null, trustManagerFactory.getTrustManagers(), new SecureRandom());
        } catch (CertificateException e) {
            e.printStackTrace();
        } catch (KeyStoreException e) {
            e.printStackTrace();
        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        } catch (KeyManagementException e) {
            e.printStackTrace();
        }
        return sslContext;
    }

    /**Metodo cerrarSesion, retorna un entero, que determina si la respuesta del servidor fue fallida o exitosa, recibe como parametro un String
     * el cual es el token que requieren todos los servicios. Este metodo se encarga de consumir el servicio rest de cerrarSesion*/

    public int cerrarSesion(String token){

        //Se debe sobreescribir este metodo para que acepte cualquier certificado seguro.
        OkHttpClient.Builder builder = new OkHttpClient.Builder();
        builder.hostnameVerifier(new HostnameVerifier() {
            @Override
            public boolean verify(String hostname, SSLSession session) {
                return true;
            }
        });

        OkHttpClient client = builder.sslSocketFactory(getSLLContext().getSocketFactory()).build();

        MediaType mediaType = MediaType.parse("application/json");
        RequestBody body = RequestBody.create(mediaType, "");
        Request request = new Request.Builder()
                .url("https://192.168.215.12:8520/netcom/merchant/api/users/MzQy/sessions")
                .method("DELETE", body)
                .addHeader("Content-Type", "application/json")
                .addHeader("Authorization", "Bearer e47ce13e-a0f3-3214-bb60-237a80a87cd7")
                .build();

       try {
           Response response = client.newCall(request).execute();

           String jsonData = response.body().string();
           JSONObject Jobject = new JSONObject(jsonData);
           Log.d("RESPUESTA",Jobject.toString());
           return Integer.parseInt(Jobject.getString("responseCode"));

       }catch (IOException | JSONException e) {
            throw new RuntimeException(e);        }
    }


    /**Metodo getRespuestaServidor que retorna un String, se encarga de retornar la variable que captura la descripción del proceso del servicio
     * de login.*/
    public String getRespuestaServidor(){
        return this.respuestaServidor;
    }



}
