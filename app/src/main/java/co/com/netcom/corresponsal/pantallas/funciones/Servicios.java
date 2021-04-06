package co.com.netcom.corresponsal.pantallas.funciones;

import android.content.Context;
import android.content.SharedPreferences;
import android.os.Message;
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
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.TimeUnit;
import java.util.prefs.Preferences;

import javax.net.ssl.HostnameVerifier;
import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLSession;
import javax.net.ssl.TrustManagerFactory;

import co.com.netcom.corresponsal.pantallas.comunes.logIn.LogIn;
import co.com.netcom.corresponsal.pantallas.comunes.pantallaConfirmacion.pantallaConfirmacion;
import co.com.netcom.corresponsal.pantallas.comunes.popUp.PopUp;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.consultaSaldo.pantallaConsultaSaldoLectura;
import okhttp3.MediaType;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.RequestBody;
import okhttp3.Response;
import okio.Buffer;

public class Servicios {

    private String respuestaServidor;

    private static String urlToken = "https://192.168.215.12:9544/oauth2/token";
    private static String urlLogin = "https://192.168.215.12:8520/netcom/merchant/api/users/sessions";

    private static String urlBaseToken = "https://192.168.215.12:9544/";
    private static String urlBaseServicios ="https://192.168.215.12:8520/";
    private static String apiLogin ="netcom/merchant/api/users/sessions";
    private static String apiParametricas ="netcom/merchant/api/parametrics/new";
    private static String apiRefreshToken ="netcom/merchant/api/users/userID/sessions";
    private static String apiContrasenaTemporal ="netcom/merchant/api/users/forgot_password";
    private static String apiCambioContrasena ="netcom/merchant/api/users/userId/passwords";
    private static String apiVenta ="netcom/merchant/api/transactions/sales/v2";
    private static String apiParametricasBanco ="netcom/merchant/api/parametrics/terminal?userId=USERID&terminalCode=TERMINALCODE";
    private static String apiPanVirtual ="netcom/merchant/api/parametrics/terminal/pan?userId=USERID&terminalCode=TERMINALCODE";

    private Context context;
    private String token;

    public Servicios(Context contexto){
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


        OkHttpClient client = builder.sslSocketFactory(getSLLContext().getSocketFactory()).callTimeout(30,TimeUnit.SECONDS).build();
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
            Log.d("RESPUESTA",jsonData);
            JSONObject Jobject = new JSONObject(jsonData);

            token = Jobject.getString("access_token");
            //tokenRefresh = Jobject.getString("refresh_token");
            if (!token.isEmpty()){
                PreferencesUsuario preferences = new PreferencesUsuario(ConstantesCorresponsal.SHARED_PREFERENCES_TOKEN,context);
                preferences.setToken(token);
                //setTokenRefresh(tokenRefresh);
            }

            return Jobject.getString("access_token");


        } catch (Exception e){

            return "";
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

        OkHttpClient client2 = builder.sslSocketFactory(getSLLContext().getSocketFactory()).callTimeout(30,TimeUnit.SECONDS).build();

        MediaType mediaType = MediaType.parse("application/json");
        String body_string = "{\"user\": \""+usuario+"\",\"channel\": \"MQ==\",\"password\": \""+contrasena+"\",\"uuid\": \"NWEyODIxZTBiZTE4MDQ2NA==\",\"ipAddress\": \"MTcyLjE3LjEuMTI0\",\"version\":\"MjIx\"}";
        RequestBody body = RequestBody.create(mediaType,body_string);
        //RequestBody body = RequestBody.create(mediaType, "{\"user\":\"${usuario}\",\"channel\": \"MQ==\",\"password\": \"NDQ1YzcyM2U3MmY5NjE1OWIyODI2MDNhZmMyNDAwYWRhNTNiMTQyNDQ3MGI0ZmExZGJjOGU2ZTEzNDRhZjMzNjU1M2RiNjBhM2EwYzY3NDM4YTdiYTE5ZmZlYjJhMWM0OTZmMjkyZWNlN2RhY2U0ZmQxODJkMGQ0N2E4NWU5NjA=\",\"uuid\": \"NWEyODIxZTBiZTE4MDQ2NA==\",\"ipAddress\": \"MTcyLjE3LjEuMTI0\",\"version\":\"MjIx\"}");

            Request request = new Request.Builder()

                .url(urlBaseServicios+apiLogin)
                .method("POST", body)
                .addHeader("Content-Type", "application/json")
                .addHeader("Accept", "text/*;q=0.3, text/html;q=0.7, text/html;level=1,text/html;level=2;q=0.4, */*;q=0.5")
                .addHeader("Authorization", "Bearer "+token)
                .build();
        try {
            Response response = client2.newCall(request).execute();

            String jsonData = response.body().string();
            Log.d("RESP",jsonData);
            JSONObject Jobject = new JSONObject(jsonData);
            Log.d("RESPUESTA",Jobject.toString());
            String estado = Jobject.getString("loginState");

            Log.d("ESTADO",estado);

           if(estado.equals("MQ==")){
                //Se guarda la información que llega en el login
                PreferencesUsuario prefs = new PreferencesUsuario(ConstantesCorresponsal.SHARED_PREFERENCES_INFO_USUARIO,context);
                prefs.setUserId(Jobject.getString("userId"));
                prefs.setEncryptionKey(Jobject.getString("encryptionKey"));
                prefs.setTerminalCode(usuario);

            }else{
                respuestaServidor = Jobject.getString("descriptionState");
            }
           /* respuestaServidor = Jobject.getString("descriptionState");
            try {
                //UserId = Jobject.getString("userId");
                //sharedPreferencesEditor.putString("userId",UserId);
            }catch (Exception e){ }*/

            return Jobject.getString("loginState");

        } catch (IOException | JSONException e) {
            CodificarBase64 code = new CodificarBase64();
            respuestaServidor=code.convertirBase64("Ambiente no disponible");
          return code.convertirBase64("-2");}

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

    public String cerrarSesion(){

        //Se debe sobreescribir este metodo para que acepte cualquier certificado seguro.
        OkHttpClient.Builder builder = new OkHttpClient.Builder();
        builder.hostnameVerifier(new HostnameVerifier() {
            @Override
            public boolean verify(String hostname, SSLSession session) {
                return true;
            }
        });

        PreferencesUsuario prefs_usuario = new PreferencesUsuario(ConstantesCorresponsal.SHARED_PREFERENCES_INFO_USUARIO,context);
        String userId = prefs_usuario.getUserId();

        PreferencesUsuario prefs_token = new PreferencesUsuario(ConstantesCorresponsal.SHARED_PREFERENCES_TOKEN,context);
        String token = prefs_token.getToken();

        Log.d("USERID",userId);
        Log.d("token",token);

        String [] direccion = urlLogin.split("users");
        String direccionFinal = direccion[0]+"users/"+userId+direccion[1];

        Log.d("DIRECCION",direccionFinal);

        OkHttpClient client = builder.sslSocketFactory(getSLLContext().getSocketFactory()).build();

        MediaType mediaType = MediaType.parse("application/json");
        RequestBody body = RequestBody.create(mediaType, "");
        Request request = new Request.Builder()
                .url(direccionFinal)
                .method("DELETE", body)
                .addHeader("Content-Type", "application/json")
                .addHeader("Authorization", "Bearer "+token)
                .addHeader("Cookie", "JSESSIONID=0B06CF9EE115DB565CF5FFC8EACB0E36; requestedURI=../../ooauth2/token")
                .build();
        Log.d("SOLICITE_SERVICIO","solicite el servicio");
       try {

           Response response = client.newCall(request).execute();
           Log.d("RESPUESTA","obtuve respuesta"+response.toString());
           String jsonData = response.body().string();
           Log.d("RESPUESTA",jsonData);
           JSONObject Jobject = new JSONObject(jsonData);
           Log.d("RESPUESTA",Jobject.toString());

           return Jobject.getString("responseCode");

       }catch (IOException | JSONException e) { return "";}
    }


    /**Metodo refrescarToken, se encarga de refrescar el token, para que la sesión no expire*/

    public void refrescarToken(){
        //Se debe sobreescribir este metodo para que acepte cualquier certificado seguro.
        OkHttpClient.Builder builder = new OkHttpClient.Builder();
        builder.hostnameVerifier(new HostnameVerifier() {
            @Override
            public boolean verify(String hostname, SSLSession session) {
                return true;
            }
        });
        PreferencesUsuario prefs = new PreferencesUsuario(ConstantesCorresponsal.SHARED_PREFERENCES_TOKEN,context);
        Log.d("Token Antiguo",prefs.getToken());
        PreferencesUsuario prefs_userID = new PreferencesUsuario(ConstantesCorresponsal.SHARED_PREFERENCES_INFO_USUARIO,context);

        OkHttpClient client = builder.sslSocketFactory(getSLLContext().getSocketFactory()).build();

        MediaType mediaType = MediaType.parse("application/x-www-form-urlencoded");
        RequestBody body = RequestBody.create(mediaType, "");
        Request request = new Request.Builder()
                .url(urlBaseToken+apiRefreshToken.replaceFirst("userID",prefs_userID.getUserId()))
                .method("PUT", body)
                .addHeader("Access-Control-Allow-Origin", "*")
                .addHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS, PUT")
                .addHeader("Content-Type", "application/x-www-form-urlencoded")
                .addHeader("Authorization", prefs.getToken())
                .build();

        try {
            Response response = client.newCall(request).execute();

            String jsonData = response.body().string();
            JSONObject Jobject = new JSONObject(jsonData);
            Log.d("RESPUESTA",Jobject.toString());
            CodificarBase64 base64 = new CodificarBase64();
            if (base64.decodificarBase64(Jobject.getString("responseCode"))=="MQ=="){
                Log.d("Refresh token","exitoso");
            }else{
                Log.d("Error","error");

            }

        }catch (IOException | JSONException e) {
                   }
    }

    /**Metodo obtenerParametricas, se encarga de solicitar las parametricas del comercio, luego de que se inicio sesión*/

    public void obtenerParametricas(){
        //Se debe sobreescribir este metodo para que acepte cualquier certificado seguro.
        OkHttpClient.Builder builder = new OkHttpClient.Builder();
        builder.hostnameVerifier(new HostnameVerifier() {
            @Override
            public boolean verify(String hostname, SSLSession session) {
                return true;
            }
        });

        //Objeto SharedPreferences
        PreferencesUsuario prefs = new PreferencesUsuario(ConstantesCorresponsal.SHARED_PREFERENCES_TOKEN,context);
        PreferencesUsuario prefs_userid = new PreferencesUsuario(ConstantesCorresponsal.SHARED_PREFERENCES_INFO_USUARIO,context);

        String userId = prefs_userid.getUserId();

        Log.d("UserId",userId);
        Log.d("Token",prefs.getToken());
        OkHttpClient client = builder.sslSocketFactory(getSLLContext().getSocketFactory()).callTimeout(30, TimeUnit.SECONDS).build();

        MediaType mediaType = MediaType.parse("application/json");
        //RequestBody body = RequestBody.create(mediaType, "{\n\t\"userId\": \""+userId+"\",\n\t\"configAppVersion\": \"MA\",\n\t\"taxsVersion\": \"MA\",\n\t\"prefixesVersion\": \"MA\",\n\t\"binPrivadoVersion\": \"MA\",\n\t\"authorizersVersion\": \"MA\",\n\t\"pspInfoVersion\": \"MA\",\n\t\"rechargeInfoVersion\": \"MA\",\n\t\"configureMposVersion\": \"MA\",\n\t\"genericDataVersion\": \"MA\"\n}");
        RequestBody body = RequestBody.create(mediaType, "{\n\t\"userId\": \""+prefs_userid.getUserId()+"\",\n\t\"configAppVersion\": \"MA\",\n\t\"taxsVersion\": \"MA\",\n\t\"prefixesVersion\": \"MA\",\n\t\"binPrivadoVersion\": \"MA\",\n\t\"authorizersVersion\": \"MA\",\n\t\"pspInfoVersion\": \"MA\",\n\t\"rechargeInfoVersion\": \"MA\",\n\t\"configureMposVersion\": \"MA\",\n\t\"genericDataVersion\": \"MA\",\n\t\"appVersion\": \"MA\",\n    \"terminalCode\": \""+prefs_userid.getTerminalCode()+"\"\n\n}");
        Request request = new Request.Builder()
                .url(urlBaseServicios+apiParametricas)
                .method("POST", body)
                .addHeader("Content-Type", "application/json")
                .addHeader("Accept", "text/*;q=0.3, text/html;q=0.7, text/html;level=1,text/html;level=2;q=0.4, */*;q=0.5")
                .addHeader("Authorization", "Bearer "+prefs.getToken())
                .build();

        String a="";
        try {
            final Request copy = request.newBuilder().build();
            final Buffer buffer = new Buffer();
            copy.body().writeTo(buffer);
          a = buffer.readUtf8();
        } catch (final IOException e) {
             a = "did not work";
        }

        Log.d("Body",a);
        Log.d("Service",request.toString());
        Log.d("Headers",request.headers().toString());

        try {
            Response response = client.newCall(request).execute();
            Log.d("RESPONSE",response.toString());

            String jsonData = response.body().string();
            JSONObject Jobject = new JSONObject(jsonData);
            Log.d("RESPUESTA",Jobject.toString());

            try{
                PreferencesUsuario prefs_parametricas = new PreferencesUsuario(ConstantesCorresponsal.SHARED_PREFERENCES_PARAMETRICAS,context);
                prefs_parametricas.setParametricas(Jobject);
                //Se envia un mensaje al handler de la clase consulta saldo, indicando que el usuario cancelo la transaccion
                Message usuarioCancela = new Message();
                usuarioCancela.what = 6;
                LogIn.respuesta.sendMessage(usuarioCancela);
            }catch(Exception e){
                Log.d("ERROR","Hubo un error en las parametricas");
            }



        } catch (IOException | JSONException e) {
            //Se envia un mensaje al handler de la clase consulta saldo, indicando que el usuario cancelo la transaccion
            Message usuarioCancela = new Message();
            usuarioCancela.what = 3;
            //Se pone en 5 para poder hacer Login
            //usuarioCancela.what = 5;
            LogIn.respuesta.sendMessage(usuarioCancela);

        }

    }

    /**Metodo getRespuestaServidor que retorna un String, se encarga de retornar la variable que captura la descripción del proceso del servicio
     * de login.*/
    public String getRespuestaServidor(){
        return this.respuestaServidor;
    }


    /**Metodo para Obtener contraseña temporal*/
    public String obtenerContrasenaTemporal(String usuario){

        //Metodo para funcionar con cualquier ssl
        OkHttpClient.Builder builder = new OkHttpClient.Builder();
        builder.hostnameVerifier(new HostnameVerifier() {
            @Override
            public boolean verify(String hostname, SSLSession session) {
                return true;
            }
        });

        PreferencesUsuario prefs = new PreferencesUsuario(ConstantesCorresponsal.SHARED_PREFERENCES_TOKEN,context);
        Log.d("TOKENCON",prefs.getToken());
        Log.d("USER",usuario);
        OkHttpClient client = builder.sslSocketFactory(getSLLContext().getSocketFactory()).callTimeout(30,TimeUnit.SECONDS).build();

        MediaType mediaType = MediaType.parse("application/json");
        RequestBody body = RequestBody.create(mediaType, "{\n    \"userName\": \""+usuario+"\"\n}");
        Request request = new Request.Builder()
                .url(urlBaseServicios+apiContrasenaTemporal)
                .method("POST", body)
                .addHeader("Authorization", "Bearer "+prefs.getToken())
                .addHeader("Content-Type", "application/json")
                .build();
        String a="";
        try {
            final Request copy = request.newBuilder().build();
            final Buffer buffer = new Buffer();
            copy.body().writeTo(buffer);
            a = buffer.readUtf8();
        } catch (final IOException e) {
            a = "did not work";
        }


        try {
            Response response = client.newCall(request).execute();

            String jsonData = response.body().string();
            Log.d("RESP",jsonData);
            JSONObject Jobject = new JSONObject(jsonData);

            if(Jobject.getString("responseCode")=="Mq=="){
                return "1";
            }else{
                return "0";
            }

        } catch (IOException | JSONException e) {
            return "0";
        }

    }

    /**Metodo para realizar el cambio de contraseña*/

    public String cambiarContraseña(String contrasenaAntigua,String contrasenaNueva, String confirmacionContrasena){

        PreferencesUsuario prefs = new PreferencesUsuario(ConstantesCorresponsal.SHARED_PREFERENCES_INFO_USUARIO,context);
        PreferencesUsuario prefsToken = new PreferencesUsuario(ConstantesCorresponsal.SHARED_PREFERENCES_TOKEN,context);

        //Metodo para funcionar con cualquier ssl
        OkHttpClient.Builder builder = new OkHttpClient.Builder();
        builder.hostnameVerifier(new HostnameVerifier() {
            @Override
            public boolean verify(String hostname, SSLSession session) {
                return true;
            }
        });

        OkHttpClient client = builder.sslSocketFactory(getSLLContext().getSocketFactory()).callTimeout(30,TimeUnit.SECONDS).build();

        MediaType mediaType = MediaType.parse("application/json");
        RequestBody body = RequestBody.create(mediaType, "{\n\t\"currentPassword\": \""+contrasenaAntigua+"\",\n\t\"newPassword\": \""+contrasenaNueva+"\",\n\t\"confirmPassword\": \""+confirmacionContrasena+"\"\n}");
        Request request = new Request.Builder()
                .url(urlBaseServicios+apiCambioContrasena.replaceFirst("userId",prefs.getUserId()))
                .method("POST", body)
                .addHeader("Content-Type", "application/json")
                .addHeader("Accept", "text/*;q=0.3, text/html;q=0.7, text/html;level=1,text/html;level=2;q=0.4, */*;q=0.5")
                .addHeader("Authorization", "Bearer "+ prefsToken.getToken())
                .build();

        String a="";
        try {
            final Request copy = request.newBuilder().build();
            final Buffer buffer = new Buffer();
            copy.body().writeTo(buffer);
            a = buffer.readUtf8();
        } catch (final IOException e) {
            a = "did not work";
        }

        Log.d("Body",a);
        Log.d("Service",request.toString());
        Log.d("Headers",request.headers().toString());

        try {
            Response response = client.newCall(request).execute();

            String jsonData = response.body().string();
            JSONObject Jobject = new JSONObject(jsonData);

            Log.d("RESP",jsonData);
            this.respuestaServidor = Jobject.getString("responseCode");

            return Jobject.getString("responseCode");

        } catch (IOException | JSONException e) {
            return "MA==";
        }
    }


    /**Metodo para consumir el servicio de venta.*/
    public Map<String,String> generarVenta(InfoVenta objetoVenta){

        //Metodo para funcionar con cualquier ssl
        OkHttpClient.Builder builder = new OkHttpClient.Builder();
        builder.hostnameVerifier(new HostnameVerifier() {
            @Override
            public boolean verify(String hostname, SSLSession session) {
                return true;
            }
        });

        PreferencesUsuario prefs = new PreferencesUsuario(ConstantesCorresponsal.SHARED_PREFERENCES_TOKEN,context);
        PreferencesUsuario prefs_userId = new PreferencesUsuario(ConstantesCorresponsal.SHARED_PREFERENCES_INFO_USUARIO,context);

        OkHttpClient client = builder.sslSocketFactory(getSLLContext().getSocketFactory()).callTimeout(30,TimeUnit.SECONDS).build();
        MediaType mediaType = MediaType.parse("application/json");
        CodificarBase64 base641 = new CodificarBase64();
        String encriptionkey = base641.decodificarBase64(prefs_userId.getEncryptionKey());
        EncripcionAES aes = new EncripcionAES();
        RequestBody body = RequestBody.create(mediaType,"");

        Log.d("OBJETO VENTA",objetoVenta.ConvertToString());
        try {
            //body = RequestBody.create(mediaType, "{\t\n    \"userId\": \"" + prefs_userId.getUserId() + "\",\t\n    \"commerceId\": \""+aes.encrypt(encriptionkey, objetoVenta.getCommerceId())+"\",\t\n    \"transactionId\": \""+aes.encrypt(encriptionkey, objetoVenta.getTransactionId())+"\",\t\n    \"detailSale\": {\r\n        \"commerceCode\": \""+aes.encrypt(encriptionkey, objetoVenta.getCommerceCode())+"\",\r\n        \"commerceId\": \""+aes.encrypt(encriptionkey, objetoVenta.getDetailSalecommerceId())+"\",\r\n        \"terminalCode\": \""+aes.encrypt(encriptionkey, objetoVenta.getTerminalCode())+"\",\r\n        \"transactionType\": \""+aes.encrypt(encriptionkey, objetoVenta.getTransactionType())+"\",\r\n        \"mposType\": \""+aes.encrypt(encriptionkey, objetoVenta.getMposType())+"\",\r\n        \"numberQuotas\": \""+aes.encrypt(encriptionkey, objetoVenta.getNumberQuotas())+"\",\r\n        \"accountType\": \""+aes.encrypt(encriptionkey, objetoVenta.getAccountType())+"\",\r\n        \"partialAmount\": \""+aes.encrypt(encriptionkey, objetoVenta.getPartialAmount())+"\",\r\n        \"iva\": \""+aes.encrypt(encriptionkey, objetoVenta.getIva())+"\",\r\n        \"base\": \""+aes.encrypt(encriptionkey, objetoVenta.getBase())+"\",\r\n        \"inc\": \""+aes.encrypt(encriptionkey, objetoVenta.getInc())+"\",\r\n        \"tip\": \""+aes.encrypt(encriptionkey, objetoVenta.getTip())+"\",\r\n        \"pan\": \""+aes.encrypt(encriptionkey, objetoVenta.getPan())+"\",\r\n        \"totalAmount\": \""+aes.encrypt(encriptionkey, objetoVenta.getTotalAmount())+"\",\r\n        \"tagSale\": \""+aes.encrypt(encriptionkey, objetoVenta.getTagSale())+"\",\r\n        \"track2\": \""+aes.encrypt(encriptionkey, objetoVenta.getTrack2())+"\",\r\n        \"encryptType\": \""+aes.encrypt(encriptionkey, objetoVenta.getEncryptType())+"\",\r\n        \"ksn\": \""+aes.encrypt(encriptionkey, objetoVenta.getKsn())+"\",\r\n        \"pinBlock\": \""+aes.encrypt(encriptionkey, objetoVenta.getPinBlock())+"\",\r\n        \"ksnPinBlock\": \""+aes.encrypt(encriptionkey, objetoVenta.getKsnPinBlock())+"\",\r\n        \"postEntryMode\": \""+aes.encrypt(encriptionkey, objetoVenta.getPostEntryMode())+"\",\r\n        \"country\": \""+aes.encrypt(encriptionkey, objetoVenta.getCountry())+"\",\r\n        \"department\": \""+aes.encrypt(encriptionkey, objetoVenta.getDepartment())+"\",\r\n        \"city\": \""+aes.encrypt(encriptionkey, objetoVenta.getCity())+"\",\r\n        \"address\": \""+aes.encrypt(encriptionkey, objetoVenta.getAddress())+"\",\r\n        \"latitude\": \""+aes.encrypt(encriptionkey, objetoVenta.getLatitude())+"\",\r\n        \"length\": \""+aes.encrypt(encriptionkey, objetoVenta.getLength())+"\",\r\n        \"consecutive\": \""+aes.encrypt(encriptionkey, objetoVenta.getConsecutive())+"\",\r\n        \"billNumber\": \""+aes.encrypt(encriptionkey, objetoVenta.getBillNumber())+"\",\r\n        \"entityCode\": \""+aes.encrypt(encriptionkey, objetoVenta.getEntityCode())+"\",\r\n        \"arqc\": \""+aes.encrypt(encriptionkey, objetoVenta.getArqc())+"\",\r\n        \"cardName\": \""+aes.encrypt(encriptionkey, objetoVenta.getCardName())+"\",\r\n        \"apLabel\": \""+aes.encrypt(encriptionkey, objetoVenta.getApLabel())+"\",\r\n        \"channel\": \""+aes.encrypt(encriptionkey, objetoVenta.getChannel())+"\",\r\n        \"expirationDate\": \""+aes.encrypt(encriptionkey, objetoVenta.getExpirationDate())+"\",\r\n        \"aid\": \""+aes.encrypt(encriptionkey, objetoVenta.getAid())+"\",\r\n        \"documentNumber\": \""+aes.encrypt(encriptionkey, objetoVenta.getDocumentNumber())+"\",\r\n        \"numeroCuenta\": \""+aes.encrypt(encriptionkey, objetoVenta.getNumeroCuenta())+"\",\r\n        \"numeroConvenio\": \""+aes.encrypt(encriptionkey, objetoVenta.getNumeroConvenio())+"\",\r\n        \"capturaLectorBarras\": \""+aes.encrypt(encriptionkey, objetoVenta.getCapturaLectorBarras())+"\",\r\n        \"numeroCelular\": \""+aes.encrypt(encriptionkey, objetoVenta.getNumeroCelular())+"\",\r\n        \"numeroTarjeta\": \""+aes.encrypt(encriptionkey, objetoVenta.getNumeroTarjeta())+"\",\r\n        \"numeroPagare\": \""+aes.encrypt(encriptionkey, objetoVenta.getNumeroPagare())+"\",\r\n        \"tipoCuentaDestino\": \""+aes.encrypt(encriptionkey, objetoVenta.getTipoCuentaDestino())+"\",\r\n        \"cuentaDestino\": \""+aes.encrypt(encriptionkey, objetoVenta.getCuentaDestino())+"\",\r\n        \"tipoDocumentoGirador\": \""+aes.encrypt(encriptionkey, objetoVenta.getTipoDocumentoGirador())+"\",\r\n        \"numeroDocumentoGirador\": \""+aes.encrypt(encriptionkey, objetoVenta.getNumeroDocumentoGirador())+"\",\r\n        \"numeroCelularGirador\": \""+aes.encrypt(encriptionkey, objetoVenta.getNumeroCelularGirador())+"\",\r\n        \"tipoDocumentoBeneficiario\": \""+aes.encrypt(encriptionkey, objetoVenta.getTipoDocumentoBeneficiario())+"\",\r\n        \"numeroDocumentoBeneficiario\": \""+aes.encrypt(encriptionkey, objetoVenta.getNumeroDocumentoBeneficiario())+"\",\r\n        \"numeroCelularBeneficiario\": \""+aes.encrypt(encriptionkey, objetoVenta.getNumeroCelularBeneficiario())+"\",\r\n        \"tramaISO\":\""+aes.encrypt(encriptionkey,objetoVenta.getTramaISO())+"\"\r\n    }\r\n}");
            body = RequestBody.create(mediaType, "{\t\n    \"userId\": \"" + prefs_userId.getUserId() + "\",\t\n    \"commerceId\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getCommerceId()))+"\",\t\n    \"transactionId\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getTransactionId()))+"\",\t\n    \"detailSale\": {\r\n        \"commerceCode\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getCommerceCode()))+"\",\r\n        \"commerceId\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getDetailSalecommerceId()))+"\",\r\n        \"terminalCode\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getTerminalCode()))+"\",\r\n        \"transactionType\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getTransactionType()))+"\",\r\n        \"mposType\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getMposType()))+"\",\r\n        \"numberQuotas\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getNumberQuotas()))+"\",\r\n        \"accountType\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getAccountType()))+"\",\r\n        \"partialAmount\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getPartialAmount()))+"\",\r\n        \"iva\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getIva()))+"\",\r\n        \"base\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getBase()))+"\",\r\n        \"inc\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getInc()))+"\",\r\n        \"tip\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getTip()))+"\",\r\n        \"pan\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getPan()))+"\",\r\n        \"totalAmount\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getTotalAmount()))+"\",\r\n        \"tagSale\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getTagSale()))+"\",\r\n        \"track2\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getTrack2()))+"\",\r\n        \"encryptType\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getEncryptType()))+"\",\r\n        \"ksn\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getKsn()))+"\",\r\n        \"pinBlock\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getPinBlock()))+"\",\r\n        \"ksnPinBlock\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getKsnPinBlock()))+"\",\r\n        \"postEntryMode\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getPostEntryMode()))+"\",\r\n        \"country\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getCountry()))+"\",\r\n        \"department\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getDepartment()))+"\",\r\n        \"city\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getCity()))+"\",\r\n        \"address\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getAddress()))+"\",\r\n        \"latitude\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getLatitude()))+"\",\r\n        \"length\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getLength()))+"\",\r\n        \"consecutive\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getConsecutive()))+"\",\r\n        \"billNumber\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getBillNumber()))+"\",\r\n        \"entityCode\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getEntityCode()))+"\",\r\n        \"arqc\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getArqc()))+"\",\r\n        \"cardName\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getCardName()))+"\",\r\n        \"apLabel\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getApLabel()))+"\",\r\n        \"channel\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getChannel()))+"\",\r\n        \"expirationDate\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getExpirationDate()))+"\",\r\n        \"aid\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getAid()))+"\",\r\n        \"documentNumber\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getDocumentNumber()))+"\",\r\n        \"numeroCuenta\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getNumeroCuenta()))+"\",\r\n        \"numeroConvenio\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getNumeroConvenio()))+"\",\r\n        \"capturaLectorBarras\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getCapturaLectorBarras()))+"\",\r\n        \"numeroCelular\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getNumeroCelular()))+"\",\r\n        \"numeroTarjeta\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getNumeroTarjeta()))+"\",\r\n        \"numeroPagare\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getNumeroPagare()))+"\",\r\n        \"tipoCuentaDestino\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getTipoCuentaDestino()))+"\",\r\n        \"cuentaDestino\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getCuentaDestino()))+"\",\r\n        \"tipoDocumentoGirador\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getTipoDocumentoGirador()))+"\",\r\n        \"numeroDocumentoGirador\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getNumeroDocumentoGirador()))+"\",\r\n        \"numeroCelularGirador\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getNumeroCelularGirador()))+"\",\r\n        \"tipoDocumentoBeneficiario\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getTipoDocumentoBeneficiario()))+"\",\r\n        \"numeroDocumentoBeneficiario\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getNumeroDocumentoBeneficiario()))+"\",\r\n        \"numeroCelularBeneficiario\": \""+base641.convertirBase64(aes.encrypt(encriptionkey, objetoVenta.getNumeroCelularBeneficiario()))+"\",\r\n        \"tramaISO\":\""+base641.convertirBase64(aes.encrypt(encriptionkey,objetoVenta.getTramaISO()))+"\",\r\n        \"numeroCuentaRecaudo\":\""+base641.convertirBase64(aes.encrypt(encriptionkey,objetoVenta.getNumeroCuentaRecaudo()))+"\"\r\n    }\r\n}");


        }
        catch (Exception e){
        }
        Request request = new Request.Builder()
                .url(urlBaseServicios+apiVenta)
                .method("POST", body)
                .addHeader("Authorization", "Bearer "+prefs.getToken())
                .addHeader("Content-Type", "application/json")
                .build();

        String a="";
        try {
            final Request copy = request.newBuilder().build();
            final Buffer buffer = new Buffer();
            copy.body().writeTo(buffer);
            a = buffer.readUtf8();
        } catch (final IOException e) {
            a = "did not work";
        }

        Log.d("Body",a);
        Log.d("Service",request.toString());
        Log.d("Headers",request.headers().toString());

        try {
            Response response = client.newCall(request).execute();
            Log.d("RESPONSE",response.toString());

            String jsonData = response.body().string();
            JSONObject Jobject = new JSONObject(jsonData);
            Log.d("RESPUESTA",Jobject.toString());

            Message usuarioCancela = new Message();
            usuarioCancela.what = 1;
            pantallaConfirmacion.respuesta.sendMessage(usuarioCancela);

            Map <String,String > resp = new HashMap<String,String>();
            resp.put("responseCode",Jobject.getString("reponseCode"));
            resp.put("responseMessage",Jobject.getString("responseMessage"));
            resp.put("aprovalCode",Jobject.getString("aprovalCodel"));
            return resp;


        }
        catch (IOException | JSONException e) {
            Message usuarioCancela = new Message();
            usuarioCancela.what = 2;
            pantallaConfirmacion.respuesta.sendMessage(usuarioCancela);
            CodificarBase64 base64 = new CodificarBase64();
            Map <String,String > resp = new HashMap<String,String>();
            resp.put("responseCode",base64.convertirBase64("code"));
            resp.put("responseMessage",base64.convertirBase64("responseMessage"));
            resp.put("aprovalCode",base64.convertirBase64("aprivalCode"));
            return resp;

        }

    }

    /**Metodo para obtener las parametricas del Fiid y el tipo de cuenta.*/

    public void obtenerParametricasBanco(){

        //Se debe sobreescribir este metodo para que acepte cualquier certificado seguro.
        OkHttpClient.Builder builder = new OkHttpClient.Builder();
        builder.hostnameVerifier(new HostnameVerifier() {
            @Override
            public boolean verify(String hostname, SSLSession session) {
                return true;
            }
        });

        //Objeto SharedPreferences
        PreferencesUsuario prefs_token = new PreferencesUsuario(ConstantesCorresponsal.SHARED_PREFERENCES_TOKEN,context);
        PreferencesUsuario prefs_user = new PreferencesUsuario(ConstantesCorresponsal.SHARED_PREFERENCES_INFO_USUARIO,context);
        PreferencesUsuario prefs_parametricas = new PreferencesUsuario(ConstantesCorresponsal.SHARED_PREFERENCES_PARAMETRICAS,context);

        OkHttpClient client = builder.sslSocketFactory(getSLLContext().getSocketFactory()).callTimeout(30, TimeUnit.SECONDS).build();

        String url_userid= apiParametricasBanco.replaceFirst("USERID",prefs_user.getUserId());
        String url_final = url_userid.replaceFirst("TERMINALCODE",prefs_parametricas.getTerminalCode());

        Log.d("url_userid",url_userid);
        Log.d("url_final",url_final);

        Request request = new Request.Builder()
                .url(urlBaseServicios+url_final)
                .method("GET", null)
                .addHeader("Authorization", "Bearer "+prefs_token.getToken())
                .build();


        try {
            Response response = client.newCall(request).execute();
            Log.d("RESPONSE",response.toString());

            String jsonData = response.body().string();
            JSONObject Jobject = new JSONObject(jsonData);
            Log.d("RESPUESTA",Jobject.toString());

            try{
                PreferencesUsuario prefs_parametricas_banco = new PreferencesUsuario(ConstantesCorresponsal.SHARED_PREFERENCES_PARAMETRICAS_BANCO,context);
                prefs_parametricas_banco.setParametricasBanco(Jobject);
                //Se envia un mensaje al handler de la clase consulta saldo, indicando que el usuario cancelo la transaccion
                Message usuarioCancela = new Message();
                usuarioCancela.what = 5;
                LogIn.respuesta.sendMessage(usuarioCancela);

            }catch(Exception e){
                Log.d("ERROR","Hubo un error en las parametricas");
            }



        } catch (IOException | JSONException e) {
            //Se envia un mensaje al handler de la clase consulta saldo, indicando que el usuario cancelo la transaccion
            Message usuarioCancela = new Message();
            usuarioCancela.what = 3;
            LogIn.respuesta.sendMessage(usuarioCancela);

        }

    }

    /**Metodo para obtener el pan virtual del servidor*/
    public String obtenerPanVirtual(){

        //Se debe sobreescribir este metodo para que acepte cualquier certificado seguro.
        OkHttpClient.Builder builder = new OkHttpClient.Builder();
        builder.hostnameVerifier(new HostnameVerifier() {
            @Override
            public boolean verify(String hostname, SSLSession session) {
                return true;
            }
        });

        //Objeto SharedPreferences
        PreferencesUsuario prefs_token = new PreferencesUsuario(ConstantesCorresponsal.SHARED_PREFERENCES_TOKEN,context);
        PreferencesUsuario prefs_user = new PreferencesUsuario(ConstantesCorresponsal.SHARED_PREFERENCES_INFO_USUARIO,context);
        PreferencesUsuario prefs_parametricas = new PreferencesUsuario(ConstantesCorresponsal.SHARED_PREFERENCES_PARAMETRICAS,context);

        OkHttpClient client = builder.sslSocketFactory(getSLLContext().getSocketFactory()).callTimeout(30, TimeUnit.SECONDS).build();


        Request request = new Request.Builder()
                .url(urlBaseServicios+apiPanVirtual.replaceFirst("USERID",prefs_user.getUserId()).replaceFirst("TERMINALCODE",prefs_parametricas.getTerminalCodeParametricas()))
                .method("GET", null)
                .addHeader("Authorization", "Bearer "+prefs_token.getToken())
                .build();

        try {
            Response response = client.newCall(request).execute();
            Log.d("RESPONSE",response.toString());

            String jsonData = response.body().string();
            JSONObject Jobject = new JSONObject(jsonData);
            Log.d("RESPUESTA",Jobject.toString());

            return Jobject.getString("pan");


        } catch (IOException | JSONException e) {
            return "";

        }
    }
}
