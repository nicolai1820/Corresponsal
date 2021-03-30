package co.com.netcom.corresponsal.pantallas.comunes.pantallaConfirmacion;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.res.ResourcesCompat;


import co.com.netcom.corresponsal.core.comunicacion.CardDTO;
import co.com.netcom.corresponsal.core.comunicacion.DatosComision;
import co.com.netcom.corresponsal.core.comunicacion.DatosRecaudo;
import co.com.netcom.corresponsal.pantallas.comunes.logIn.LogIn;
import co.com.netcom.corresponsal.pantallas.comunes.popUp.PopUp;
import co.com.netcom.corresponsal.pantallas.comunes.resultadoTransaccion.PantallaResultadoTransaccionExitosa;
import co.com.netcom.corresponsal.pantallas.comunes.resultadoTransaccion.PantallaResultadoTransaccionLoaderExitoso;
import co.com.netcom.corresponsal.pantallas.comunes.resultadoTransaccion.PantallaResultadoTransaccionLoaderFallida;
import co.com.netcom.corresponsal.pantallas.funciones.CodificarBase64;
import co.com.netcom.corresponsal.pantallas.funciones.CodigosTransacciones;

import android.app.AlertDialog;
import android.content.Intent;
import android.graphics.Color;
import android.graphics.Typeface;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.LinearLayout;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.Map;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.funciones.ConstantesCorresponsal;
import co.com.netcom.corresponsal.pantallas.funciones.DeviceInformation;
import co.com.netcom.corresponsal.pantallas.funciones.EncripcionAES;
import co.com.netcom.corresponsal.pantallas.funciones.Hilos;
import co.com.netcom.corresponsal.pantallas.funciones.InfoVenta;
import co.com.netcom.corresponsal.pantallas.funciones.PreferencesUsuario;
import co.com.netcom.corresponsal.pantallas.funciones.Servicios;

public class pantallaConfirmacion extends AppCompatActivity {

    private String tituloActivity;
    private Header header;
    private String descripcion_string;
    private TextView descripcion;
    private LinearLayout linearLayoutConfirmacion;
    private Class<?>  actividad;
    private String [] titulos;
    private ArrayList<String> valores = new ArrayList<String>();
    private Typeface quicksandBold;
    private Typeface quicksandRegular;
    private int contador;
    private int transaccion;
    private ArrayList<String> tipoDocumento = new ArrayList<String>();
    private ArrayList<String> tipoDeCuenta = new ArrayList<String>();
    private ArrayList<String> datosTransaccion = new ArrayList<String>();
    private ArrayList<Integer> iconos = new ArrayList<Integer>();
    private Hilos hiloTransacciones;

    private String valorGiro;
    private DatosComision datosComision;
    private DatosRecaudo datosRecaudo;
    private CardDTO tarjeta;

    private final static int ICONO_DINERO =1;
    private final static int ICONO_CUENTA =2;
    private final static int ICONO_NUMERO =3;
    private final static int ICONO_TIPO_DOCUMENTO=4;

    private AlertDialog dialog;
    private PreferencesUsuario prefs_parametricasBanco;
    private PreferencesUsuario prefs_parametricasUser;
    private CodificarBase64 base64;
    private EncripcionAES aes;
    private Servicios service;
    public static Handler respuesta;

    private Map<String,String > resp;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_confirmacion);


        hiloTransacciones = new Hilos(pantallaConfirmacion.this);

        /*Se obtienen los datos que vienen en el intent: titulo activity, descripcion,titulo de contenidos, valores
         y la clase a la cual se debe redireccionar*/

        Bundle i = getIntent().getExtras();
        tituloActivity = i.getString("titulo");
        descripcion_string = i.getString("descripcion");
        String className = i.getString("clase");


        try {
            actividad = Class.forName(className);
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        }

        tipoDeCuenta = i.getStringArrayList("tipoDeCuenta");
        titulos = i.getStringArray("titulos");
        valores = i.getStringArrayList("valores");
        contador = i.getInt("contador");

        tipoDocumento = i.getStringArrayList("tipoDocumento");
        transaccion = i.getInt("transaccion");
        iconos = i.getIntegerArrayList("iconos");



        //Se obtienen los valores si es un giro
        valorGiro = i.getString("valorGiro");
        datosComision = new DatosComision();
        datosComision = i.getParcelable("datosComision");

        datosRecaudo = new DatosRecaudo();
        datosRecaudo = i.getParcelable("datosRecaudo");

        //Se obtiene los valores ed la tarjeta
        tarjeta = new CardDTO();
        tarjeta = i.getParcelable("tarjeta");


        //Se crea el header con el titulo que se rescato del intent
        header = new Header(tituloActivity);
        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderConfirmarValores,header).commit();



        //Se realiza la conexion con los elementos de la interfaz grafica.
        linearLayoutConfirmacion = (LinearLayout) findViewById(R.id.linearLayoutConfirmacion);
        descripcion = (TextView) findViewById(R.id.textViewDescripcionValores);
        descripcion.setText(descripcion_string);

        //Se crea la un objeto tipo Typeface, para acceder a los recursos y utilizar las fuentes.

        quicksandBold = ResourcesCompat.getFont(getApplicationContext(), R.font.quicksand_bold);
        quicksandRegular = ResourcesCompat.getFont(getApplicationContext(), R.font.quicksand_regular);

        //Se llama al metodo para crear  los textView
        generarTextView();

        //PREFERENCES FIID Y TIPODECUENTA
        prefs_parametricasBanco = new PreferencesUsuario(ConstantesCorresponsal.SHARED_PREFERENCES_PARAMETRICAS_BANCO,this);
        prefs_parametricasUser = new PreferencesUsuario(ConstantesCorresponsal.SHARED_PREFERENCES_INFO_USUARIO,this);

        //OBJETOS PARA DECODIFICAR EL PAN
        base64 = new CodificarBase64();
        aes = new EncripcionAES();

        //OBJETO PARA CONSUMIR SERVICIOS
        service = new Servicios(this);

        //Handler para manejar flujos
        respuesta = new android.os.Handler(){
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what) {
                    case 1:
                        dialog.dismiss();
                        Log.d("VENTA",resp.toString());
                        if(resp.get("responseCode").equals("MQ==")){
                            Intent i = new Intent(pantallaConfirmacion.this, PantallaResultadoTransaccionLoaderExitoso
                                    .class);
                            i.putExtra("aprovalCode",resp.get("aprovalCode"));
                            startActivity(i);
                        }else {
                            Intent i = new Intent(pantallaConfirmacion.this, PantallaResultadoTransaccionLoaderFallida.class);
                            startActivity(i);
                    }
                        break;
                    case 2:
                        dialog.dismiss();
                        Log.d("VENTA",resp.toString());
                        break;
                }}};
    }


   /**Metodo generarTextView de tipo void. Se encarga de generar dinamicamente los TextView de los valores a confirmar
    * tomando como referencia los arreglos que se rescatan del intent.*/
    public void generarTextView(){

        //Caso especifico cancelacion giro
        if(transaccion == CodigosTransacciones.CORRESPONSAL_CANCELACION_GIRO){
            LinearLayout linearLayout = new LinearLayout(getBaseContext());
            linearLayout.setOrientation(LinearLayout.HORIZONTAL);

            LinearLayout.LayoutParams paramImage = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.WRAP_CONTENT, LinearLayout.LayoutParams.WRAP_CONTENT);
            paramImage.setMargins(0,20,0,20);
            TextView icono = new TextView(getBaseContext());

            if (iconos.get(1) == ICONO_DINERO){
                try {
                    //Se agrega el icono

                    icono.setBackgroundResource(R.drawable.icon_money);
                    icono.setTextColor(getResources().getColor(R.color.blanco));
                    icono.setTextAlignment(TextView.TEXT_ALIGNMENT_CENTER);
                    icono.setTextSize(21);
                    icono.setLayoutParams(paramImage);
                }catch (Exception e){}
            }else if (iconos.get(1) == ICONO_CUENTA){
                try {
                    //Se agrega el icono
                    icono.setBackgroundResource(R.drawable.icon_cuenta);
                    icono.setTextColor(getResources().getColor(R.color.blanco));
                    icono.setTextAlignment(TextView.TEXT_ALIGNMENT_CENTER);
                    icono.setTextSize(21);
                    icono.setLayoutParams(paramImage);
                }catch (Exception e){}
            }else if (iconos.get(1) == ICONO_TIPO_DOCUMENTO){
                try {
                    //Se agrega el icono
                    icono.setBackgroundResource(R.drawable.identificacion);
                    icono.setTextColor(getResources().getColor(R.color.blanco));
                    icono.setTextAlignment(TextView.TEXT_ALIGNMENT_CENTER);
                    icono.setTextSize(21);
                    icono.setLayoutParams(paramImage);
                }catch (Exception e){}
            }else {
                try {
                    //Se agrega el icono
                    icono.setBackgroundResource(R.drawable.icon_number);
                    icono.setTextColor(getResources().getColor(R.color.blanco));
                    icono.setTextAlignment(TextView.TEXT_ALIGNMENT_CENTER);
                    icono.setTextSize(21);
                    icono.setLayoutParams(paramImage);
                }catch (Exception e){}
            }



            TextView referencia = new TextView(getBaseContext());
            referencia.setTextSize(21);
            referencia.setTypeface(quicksandBold);
            referencia.setTextColor(getResources().getColor(R.color.azul));
            referencia.setText(titulos[0]);


            //Se crean los TextView de los valores.
            TextView contenido = new TextView(getBaseContext());
            referencia.setTextColor(getResources().getColor(R.color.azul));
            contenido.setTextSize(29);
            referencia.setTypeface(quicksandRegular);
            contenido.setText(valorGiro);


            linearLayout.addView(icono);
            linearLayout.addView(contenido);
            //Se agregan los TextViews al LinearLayout
            linearLayoutConfirmacion.addView(referencia);
            linearLayoutConfirmacion.addView(linearLayout);
        } else {
            //Todas las transacciones excepto cancelar giro

            for (int j = 0; j < titulos.length; j++) {


                LinearLayout.LayoutParams paramImage = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.WRAP_CONTENT, LinearLayout.LayoutParams.WRAP_CONTENT);
                paramImage.setMargins(0,20,0,20);


                //Se crea el linear layout
                LinearLayout linearLayout = new LinearLayout(getBaseContext());
                linearLayout.setOrientation(LinearLayout.HORIZONTAL);
                linearLayout.setGravity( Gravity.CENTER_VERTICAL);


                //Se agrega el icono
                TextView icono = new TextView(getBaseContext());

                Log.d("REFENCIA ICONO",String.valueOf(iconos.get(j).intValue()));

                try {
                    if (iconos.get(j) == ICONO_DINERO){
                        //Se agrega el icono
                        icono.setBackgroundResource(R.drawable.icon_money);
                        icono.setTextColor(getResources().getColor(R.color.blanco));
                        icono.setTextAlignment(TextView.TEXT_ALIGNMENT_CENTER);
                        icono.setTextSize(21);
                        icono.setLayoutParams(paramImage);
                    }else if (iconos.get(j) == ICONO_CUENTA){
                        //Se agrega el icono
                        icono.setBackgroundResource(R.drawable.icon_cuenta);
                        icono.setTextColor(getResources().getColor(R.color.blanco));
                        icono.setTextAlignment(TextView.TEXT_ALIGNMENT_CENTER);
                        icono.setTextSize(21);
                        icono.setLayoutParams(paramImage);
                    }else if (iconos.get(j) == ICONO_NUMERO){
                        //Se agrega el icono
                        icono.setBackgroundResource(R.drawable.icon_number);
                        icono.setTextColor(getResources().getColor(R.color.blanco));
                        icono.setTextAlignment(TextView.TEXT_ALIGNMENT_CENTER);
                        icono.setTextSize(21);
                        icono.setLayoutParams(paramImage);
                    }else if (iconos.get(j) == ICONO_TIPO_DOCUMENTO){
                            //Se agrega el icono
                            icono.setBackgroundResource(R.drawable.identificacion);
                            icono.setTextColor(getResources().getColor(R.color.blanco));
                            icono.setTextAlignment(TextView.TEXT_ALIGNMENT_CENTER);
                            icono.setTextSize(21);
                            icono.setLayoutParams(paramImage);

                    }
                }catch (Exception e){}



                //Se agrega el titulo
                TextView referencia = new TextView(getBaseContext());
                referencia.setTextSize(21);
                referencia.setTypeface(quicksandBold);
                referencia.setTextColor(getResources().getColor(R.color.azul));
                referencia.setText(titulos[j]);


                //se agrega las margenes del contenido
                LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.WRAP_CONTENT, LinearLayout.LayoutParams.WRAP_CONTENT);
                params.setMargins(25,0,0,0);

                //Se crean los TextView de los valores.
                TextView contenido = new TextView(getBaseContext());
                contenido.setTextSize(29);
                contenido.setTypeface(quicksandRegular);
                contenido.setTextColor(getResources().getColor(R.color.azul));
                contenido.setGravity(Gravity.CENTER_VERTICAL);
                contenido.setLayoutParams(params);


                contenido.setText(valores.get(contador).toString());

                contador++;

                linearLayout.addView(icono);
                linearLayout.addView(contenido);
                //Se agregan los TextViews al LinearLayout
                linearLayoutConfirmacion.addView(referencia);
                linearLayoutConfirmacion.addView(linearLayout);


            }

        }
    }

    /**Metodo confirmarValores de tipo void, recibe como parametro un View, para poder ser implementado por el boton Confirmar.
     * Se encarga de hacer el intent a la clase a la cual se debe redireccionar adicionando el arreglo de los datos y el contador,
     * en caso de que ser la ultima pantalla se ejecuta la correspondiente transaccion*/


    public void confirmarValores(View view){

        /*Se determina si la el codigo de la transaccion es vacio, si lo es sigue con el flujo normal, de lo contrario, ejecuta
         el hilo correspondiente a esa transacción*/

        if(transaccion == 0){
            Intent intento = new Intent(getApplicationContext(), actividad);
            intento.putExtra("contador", contador);
            intento.putExtra("valores", valores);
            intento.putExtra("tipoDocumento",tipoDocumento);
            intento.putExtra("tipoDeCuenta",tipoDeCuenta);
            try {
                intento.putExtra("tarjeta",tarjeta);
            }catch (Exception e){}

            startActivity(intento);
            overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);

        }else {

            DeviceInformation deviceInformation = new DeviceInformation(this);
         if(deviceInformation.getInternetStatus()){

             if(deviceInformation.getLocationStatus()){
                 //Se crea el loader que se mostrara mientras se procesa la transaccion
                 AlertDialog.Builder loader = new AlertDialog.Builder(pantallaConfirmacion.this);

                 LayoutInflater inflater = this.getLayoutInflater();

                 loader.setView(inflater.inflate(R.layout.loader_procesando_transaccion,null));
                 loader.setCancelable(false);

                 dialog = loader.create();
                 dialog.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));

                 Log.d("OPEN"," se abrio el loader");


                 dialog.show();
                 datosTransaccion.clear();

                 switch (transaccion){

                     //listo
                     case CodigosTransacciones.CORRESPONSAL_DEPOSITO:{
                         for (int i=0;i < valores.size();i++){
                             datosTransaccion.add(valores.get(i));
                         }
                         for (int i =0; i<tipoDeCuenta.size();i++) {
                             datosTransaccion.add(tipoDeCuenta.get(i));
                         }

                         new Thread() {
                             @Override
                             public void run() {

                                 try {
                                     //Log.d("SERVICIO",service.obtenerPanVirtual());
                                     Log.d("EN",aes.encrypt(base64.decodificarBase64(prefs_parametricasUser.getEncryptionKey()),"24832432950423023423423"));

                                     Log.d("DE",aes.decrypt(base64.decodificarBase64(prefs_parametricasUser.getEncryptionKey()),aes.encrypt(base64.decodificarBase64(prefs_parametricasUser.getEncryptionKey()),"24832432950423023423423")));
                                     Log.d("DE-SERVIDOR",aes.decrypt(base64.decodificarBase64(prefs_parametricasUser.getEncryptionKey()),base64.decodificarBase64(service.obtenerPanVirtual())));
                                     Log.d("encryption",prefs_parametricasUser.getEncryptionKey());

                                 } catch (Exception e) {
                                     e.printStackTrace();
                                 }
                                 try {
                                     hiloTransacciones.transaccionesSinTarjeta(transaccion,datosTransaccion,prefs_parametricasBanco.getFiidID(),prefs_parametricasBanco.getTipoCuenta(),aes.decrypt(base64.decodificarBase64(prefs_parametricasUser.getEncryptionKey()),base64.decodificarBase64(service.obtenerPanVirtual())));
                                 } catch (Exception e) {
                                     e.printStackTrace();
                                 }
                                 try {
                                     // code runs in a thread
                                     runOnUiThread(new Runnable() {
                                         @Override
                                         public void run() {
                                             //Log.d("CLOSE"," se cerro el loader");
                                             //dialog.dismiss();
                                             enviarTransaccion();
                                         }
                                     });
                                 } catch (final Exception ex) {

                                 }
                             }
                         }.start();
                         //datosTransaccion.clear();
                         break;
                     }
                     //listo
                     case CodigosTransacciones.CORRESPONSAL_RECLAMACION_GIRO:{

                         for (int i=0;i < valores.size();i++){
                             datosTransaccion.add(valores.get(i));
                         }

                         for (int i =0; i<tipoDocumento.size();i++) {
                             datosTransaccion.add(tipoDocumento.get(i));
                         }

                         Log.d("DATOS",datosTransaccion.toString());
                         new Thread() {
                             @Override
                             public void run() {

                                 try {
                                     hiloTransacciones.transaccionesSinTarjeta(transaccion,datosTransaccion,prefs_parametricasBanco.getFiidID(),prefs_parametricasBanco.getTipoCuenta(),aes.decrypt(base64.decodificarBase64(prefs_parametricasUser.getEncryptionKey()),base64.decodificarBase64(service.obtenerPanVirtual())));
                                 } catch (Exception e) {
                                     e.printStackTrace();
                                 }
                                 try {
                                     // code runs in a thread
                                     runOnUiThread(new Runnable() {
                                         @Override
                                         public void run() {
                                             Log.d("CLOSE"," se cerro el loader");
                                             dialog.dismiss();
                                         }
                                     });
                                 } catch (final Exception ex) {

                                 }
                             }
                         }.start();
                         enviarTransaccion();
                         //datosTransaccion.clear();

                         break;
                     }
                     //listo
                     case CodigosTransacciones.CORRESPONSAL_ENVIO_GIRO:{

                         for (int i=0; i<valores.size();i++){
                             datosTransaccion.add(valores.get(i));
                         }

                         for(int i =0; i<tipoDocumento.size();i++){
                             datosTransaccion.add(tipoDocumento.get(i));
                         }

                         new Thread() {
                             @Override
                             public void run() {

                                 try {
                                     hiloTransacciones.transaccionesSinTarjeta(transaccion,datosTransaccion,prefs_parametricasBanco.getFiidID(),prefs_parametricasBanco.getTipoCuenta(),aes.decrypt(base64.decodificarBase64(prefs_parametricasUser.getEncryptionKey()),base64.decodificarBase64(service.obtenerPanVirtual())));
                                 } catch (Exception e) {
                                     e.printStackTrace();
                                 }
                                 try {
                                     // code runs in a thread
                                     runOnUiThread(new Runnable() {
                                         @Override
                                         public void run() {
                                             Log.d("CLOSE"," se cerro el loader");
                                             dialog.dismiss();
                                         }
                                     });
                                 } catch (final Exception ex) {

                                 }
                             }
                         }.start();
                         enviarTransaccion();
                         break;
                     }
                     //listo
                     case CodigosTransacciones.CORRESPONSAL_CANCELACION_GIRO_CONSULTA:{

                         for (int i=0; i<valores.size();i++){
                             datosTransaccion.add(valores.get(i));
                         }

                         for(int i =0; i<tipoDocumento.size();i++){
                             datosTransaccion.add(tipoDocumento.get(i));
                         }

                         new Thread() {
                             @Override
                             public void run() {

                                 try {
                                     hiloTransacciones.transaccionesSinTarjeta(transaccion,datosTransaccion,prefs_parametricasBanco.getFiidID(),prefs_parametricasBanco.getTipoCuenta(),aes.decrypt(base64.decodificarBase64(prefs_parametricasUser.getEncryptionKey()),base64.decodificarBase64(service.obtenerPanVirtual())));
                                 } catch (Exception e) {
                                     e.printStackTrace();
                                 }
                                 try {
                                     // code runs in a thread
                                     runOnUiThread(new Runnable() {
                                         @Override
                                         public void run() {
                                             Log.d("CLOSE"," se cerro el loader");
                                             dialog.dismiss();
                                         }
                                     });
                                 } catch (final Exception ex) {

                                 }
                             }
                         }.start();
                         enviarTransaccion();
                         break;
                     }
                     //listo pero no lo estoy usando, revisar
                     case CodigosTransacciones.CORRESPONSAL_CANCELACION_GIRO:{

                         new Thread() {
                             @Override
                             public void run() {

                                 try {
                                     hiloTransacciones.cancelarGiro(valores,datosComision,prefs_parametricasBanco.getFiidID(),prefs_parametricasBanco.getTipoCuenta(),aes.decrypt(base64.decodificarBase64(prefs_parametricasUser.getEncryptionKey()),base64.decodificarBase64(service.obtenerPanVirtual())));
                                 } catch (Exception e) {
                                     e.printStackTrace();
                                 }
                                 try {
                                     // code runs in a thread
                                     runOnUiThread(new Runnable() {
                                         @Override
                                         public void run() {
                                             Log.d("CLOSE"," se cerro el loader");
                                             dialog.dismiss();
                                         }
                                     });
                                 } catch (final Exception ex) {

                                 }
                             }
                         }.start();
                         enviarTransaccion();
                         break;
                     }

                     //listo ------- veriricar porque no se confirman valores
                     case CodigosTransacciones.CORRESPONSAL_RETIRO_SIN_TARJETA:{

                         for (int i=0;i < valores.size();i++){
                             datosTransaccion.add(valores.get(i));
                         }

                         datosTransaccion.add(tarjeta.getPinBlock());


                         new Thread() {
                             @Override
                             public void run() {

                                 try {
                                     hiloTransacciones.transaccionesSinTarjeta(transaccion,datosTransaccion,prefs_parametricasBanco.getFiidID(),prefs_parametricasBanco.getTipoCuenta(),aes.decrypt(base64.decodificarBase64(prefs_parametricasUser.getEncryptionKey()),base64.decodificarBase64(service.obtenerPanVirtual())));
                                 } catch (Exception e) {
                                     e.printStackTrace();
                                 }
                                 try {
                                     // code runs in a thread
                                     runOnUiThread(new Runnable() {
                                         @Override
                                         public void run() {
                                             Log.d("CLOSE"," se cerro el loader");
                                             dialog.dismiss();
                                         }
                                     });
                                 } catch (final Exception ex) {

                                 }
                             }
                         }.start();

                         enviarTransaccion();

                         break;
                     }
                     //listo
                     case CodigosTransacciones.CORRESPONSAL_RETIRO_CON_TARJETA:{
                         for (int i=0;i < valores.size();i++){
                             datosTransaccion.add(valores.get(i));
                         }
                         for (int i =0;i < tipoDeCuenta.size();i++){
                             datosTransaccion.add(tipoDeCuenta.get(i));
                         }

                         new Thread() {
                             @Override
                             public void run() {

                                 hiloTransacciones.transaccionesConTarjeta(transaccion,datosTransaccion,tarjeta);
                                 try {
                                     // code runs in a thread
                                     runOnUiThread(new Runnable() {
                                         @Override
                                         public void run() {
                                             Log.d("CLOSE"," se cerro el loader");
                                             dialog.dismiss();
                                         }
                                     });
                                 } catch (final Exception ex) {

                                 }
                             }
                         }.start();
                         enviarTransaccion();
                         break;
                     }
                     //Listo
                     case CodigosTransacciones.CORRESPONSAL_CONSULTA_SALDO:{

                         new Thread() {
                             @Override
                             public void run() {

                                 hiloTransacciones.transaccionesConTarjeta(CodigosTransacciones.CORRESPONSAL_CONSULTA_SALDO,null,tarjeta);
                                 try {
                                     // code runs in a thread
                                     runOnUiThread(new Runnable() {
                                         @Override
                                         public void run() {
                                             dialog.dismiss();
                                         }
                                     });
                                 } catch (final Exception ex) {

                                 }
                             }
                         }.start();

                         enviarTransaccion();
                         break;
                     }
                     //listo
                     case CodigosTransacciones.CORRESPONSAL_PAGO_FACTURA_MANUAL:{
                         new Thread() {
                             @Override
                             public void run() {

                                 try {
                                     hiloTransacciones.pagoFacturaManual(datosRecaudo,prefs_parametricasBanco.getFiidID(),prefs_parametricasBanco.getTipoCuenta(),aes.decrypt(base64.decodificarBase64(prefs_parametricasUser.getEncryptionKey()),base64.decodificarBase64(service.obtenerPanVirtual())));
                                 } catch (Exception e) {
                                     e.printStackTrace();
                                 }
                                 try {
                                     // code runs in a thread
                                     runOnUiThread(new Runnable() {
                                         @Override
                                         public void run() {
                                             Log.d("CLOSE"," se cerro el loader");
                                             dialog.dismiss();
                                         }
                                     });
                                 } catch (final Exception ex) {

                                 }
                             }
                         }.start();
                         enviarTransaccion();
                         break;
                     }
                     //Listo
                     case CodigosTransacciones.CORRESPONSAL_PAGO_FACTURA_TARJETA_EMPRESARIAL:{

                         for (int i=0;i < valores.size();i++){
                             datosTransaccion.add(valores.get(i));
                         }

                         datosTransaccion.add(tarjeta.getTrack2());

                         new Thread() {
                             @Override
                             public void run() {

                                 try {
                                     hiloTransacciones.transaccionesSinTarjeta(transaccion,datosTransaccion,prefs_parametricasBanco.getFiidID(),prefs_parametricasBanco.getTipoCuenta(),aes.decrypt(base64.decodificarBase64(prefs_parametricasUser.getEncryptionKey()),base64.decodificarBase64(service.obtenerPanVirtual())));
                                 } catch (Exception e) {
                                     e.printStackTrace();
                                 }
                                 try {
                                     // code runs in a thread
                                     runOnUiThread(new Runnable() {
                                         @Override
                                         public void run() {
                                             Log.d("CLOSE"," se cerro el loader");
                                             dialog.dismiss();
                                         }
                                     });
                                 } catch (final Exception ex) {

                                 }
                             }
                         }.start();

                         enviarTransaccion();
                         break;
                     }
                     //listo
                     case CodigosTransacciones.CORRESPONSAL_PAGO_PRODUCTOS:{
                         for (int i=0;i < valores.size();i++){
                             datosTransaccion.add(valores.get(i));
                         }


                         new Thread() {
                             @Override
                             public void run() {

                                 try {
                                     hiloTransacciones.transaccionesSinTarjeta(transaccion,datosTransaccion,prefs_parametricasBanco.getFiidID(),prefs_parametricasBanco.getTipoCuenta(),aes.decrypt(base64.decodificarBase64(prefs_parametricasUser.getEncryptionKey()),base64.decodificarBase64(service.obtenerPanVirtual())));
                                 } catch (Exception e) {
                                     e.printStackTrace();
                                 }
                                 try {
                                     // code runs in a thread
                                     runOnUiThread(new Runnable() {
                                         @Override
                                         public void run() {
                                             Log.d("CLOSE"," se cerro el loader");
                                             dialog.dismiss();
                                         }
                                     });
                                 } catch (final Exception ex) {

                                 }
                             }
                         }.start();

                         enviarTransaccion();
                         break;
                     }
                     //Listo
                     case CodigosTransacciones.CORRESPONSAL_TRANSFERENCIA:{
                         for (int i=0;i < valores.size();i++){
                             datosTransaccion.add(valores.get(i));
                         }
                         for (int i =0;i < tipoDeCuenta.size();i++){
                             datosTransaccion.add(tipoDeCuenta.get(i));
                         }

                         new Thread() {
                             @Override
                             public void run() {

                                 hiloTransacciones.transaccionesConTarjeta(transaccion,datosTransaccion,tarjeta);
                                 try {
                                     // code runs in a thread
                                     runOnUiThread(new Runnable() {
                                         @Override
                                         public void run() {
                                             Log.d("CLOSE"," se cerro el loader");
                                             dialog.dismiss();
                                         }
                                     });
                                 } catch (final Exception ex) {

                                 }
                             }
                         }.start();

                         enviarTransaccion();
                         break;
                     }
                     //listo para recargas,  creo que funciona para facturas
                     case CodigosTransacciones.CORRESPONSAL_CONSULTA_FACTURAS:{
                         for (int i=0;i < valores.size();i++){
                             datosTransaccion.add(valores.get(i));
                         }

                         try {
                             for (int i=0;i < tipoDeCuenta.size();i++){
                                 datosTransaccion.add(tipoDeCuenta.get(i));
                             }
                         }catch (Exception e){}

                         new Thread() {
                             @Override
                             public void run() {

                                 try {
                                     hiloTransacciones.transaccionesSinTarjeta(transaccion,datosTransaccion,prefs_parametricasBanco.getFiidID(),prefs_parametricasBanco.getTipoCuenta(),aes.decrypt(base64.decodificarBase64(prefs_parametricasUser.getEncryptionKey()),base64.decodificarBase64(service.obtenerPanVirtual())));
                                 } catch (Exception e) {
                                     e.printStackTrace();
                                 }
                                 try {
                                     // code runs in a thread
                                     runOnUiThread(new Runnable() {
                                         @Override
                                         public void run() {
                                             Log.d("CLOSE"," se cerro el loader");
                                             dialog.dismiss();
                                         }
                                     });
                                 } catch (final Exception ex) {

                                 }
                             }
                         }.start();
                         enviarTransaccion();
                         break;
                     }

                     default: {
                         Log.d("DEFAULT","Default");
                         break;
                     }
                 }

             }else{
                 PopUp pop = new PopUp(pantallaConfirmacion.this);
                 pop.crearPopUpLoginFallido("Debe activar la ubicación");
             }
         }else{
             PopUp pop = new PopUp(pantallaConfirmacion.this);
             pop.crearPopUpErrorInternet();
         }
        }
    }

    /**Metodo enviarTransacción, de tipo void, que se encarga de enviar el servicio de venta*/
    public void enviarTransaccion(){
        InfoVenta inforVenta = new InfoVenta();
        DeviceInformation devideInfo = new DeviceInformation(this);
        PreferencesUsuario prefs_Parametricas = new PreferencesUsuario(ConstantesCorresponsal.SHARED_PREFERENCES_PARAMETRICAS,this);
        PreferencesUsuario prefs_InfoUser = new PreferencesUsuario(ConstantesCorresponsal.SHARED_PREFERENCES_INFO_USUARIO,this);

        inforVenta.setUserId(prefs_InfoUser.getUserId());
        inforVenta.setCommerceId(base64.decodificarBase64(prefs_Parametricas.getCommerceId()));
        inforVenta.setTransactionId("12345");
            //Detail Sale
            inforVenta.setCommerceCode(base64.decodificarBase64(prefs_Parametricas.getCommerceCode()));
            inforVenta.setDetailSalecommerceId(base64.decodificarBase64(prefs_Parametricas.getCommerceCode()));
            inforVenta.setTerminalCode(base64.decodificarBase64(prefs_Parametricas.getTerminalCode()));
            inforVenta.setEncryptType("DUKPT");
            inforVenta.setCountry("");
            inforVenta.setDepartment("ee");
            inforVenta.setCity("");
            inforVenta.setAddress(base64.decodificarBase64(prefs_Parametricas.getAddress()));
            inforVenta.setLatitude(devideInfo.getLatitude());
            inforVenta.setLength(devideInfo.getLongitude());
            inforVenta.setConsecutive(devideInfo.getNewConsecutive());
            inforVenta.setBillNumber("");
            inforVenta.setEntityCode("");
            inforVenta.setChannel("1");


        switch (transaccion){
            case CodigosTransacciones.CORRESPONSAL_DEPOSITO: {
                inforVenta.setTransactionType("");
                inforVenta.setMposType("");
                inforVenta.setNumberQuotas("");
                inforVenta.setAccountType("");
                inforVenta.setPartialAmount("");
                inforVenta.setIva("");
                inforVenta.setBase("");
                inforVenta.setInc("");
                inforVenta.setTip("");
                inforVenta.setPan("");
                inforVenta.setTotalAmount(datosTransaccion.get(2));
                inforVenta.setTagSale("");
                inforVenta.setTrack2("");
                inforVenta.setKsn("");
                inforVenta.setPinBlock("");
                inforVenta.setKsnPinBlock("");
                inforVenta.setPostEntryMode("");
                inforVenta.setArqc("");
                inforVenta.setCardName("");
                inforVenta.setApLabel("");
                inforVenta.setExpirationDate("");
                inforVenta.setAid("");
                inforVenta.setDocumentNumber("");
                inforVenta.setNumeroCuenta("");
                inforVenta.setNumeroConvenio("");
                inforVenta.setCapturaLectorBarras("");
                inforVenta.setNumeroCelular("");
                inforVenta.setNumeroTarjeta("");
                inforVenta.setNumeroPagare("");
                inforVenta.setTipoCuentaDestino(datosTransaccion.get(3));
                inforVenta.setCuentaDestino(datosTransaccion.get(1));
                inforVenta.setTipoDocumentoGirador("");
                inforVenta.setNumeroDocumentoGirador("");
                inforVenta.setNumeroCelularGirador("");
                inforVenta.setTipoDocumentoBeneficiario("");
                inforVenta.setNumeroDocumentoBeneficiario("");
                inforVenta.setNumeroCelularBeneficiario("");
            }
                break;

            case CodigosTransacciones.CORRESPONSAL_RECLAMACION_GIRO:{
                //referencia del giro en datosTransaccion.get(3)
                inforVenta.setTransactionType("");
                inforVenta.setMposType("");
                inforVenta.setNumberQuotas("");
                inforVenta.setAccountType("");
                inforVenta.setPartialAmount("");
                inforVenta.setIva("");
                inforVenta.setBase("");
                inforVenta.setInc("");
                inforVenta.setTip("");
                inforVenta.setPan("");
                inforVenta.setTotalAmount(datosTransaccion.get(2));
                inforVenta.setTagSale("");
                inforVenta.setTrack2("");
                inforVenta.setKsn("");
                inforVenta.setPinBlock("");
                inforVenta.setKsnPinBlock("");
                inforVenta.setPostEntryMode("");
                inforVenta.setArqc("");
                inforVenta.setCardName("");
                inforVenta.setApLabel("");
                inforVenta.setExpirationDate("");
                inforVenta.setAid("");
                inforVenta.setDocumentNumber("");
                inforVenta.setNumeroCuenta("");
                inforVenta.setNumeroConvenio("");
                inforVenta.setCapturaLectorBarras("");
                inforVenta.setNumeroCelular("");
                inforVenta.setNumeroTarjeta("");
                inforVenta.setNumeroPagare("");
                inforVenta.setTipoCuentaDestino("");
                inforVenta.setCuentaDestino("");
                inforVenta.setTipoDocumentoGirador("");
                inforVenta.setNumeroDocumentoGirador("");
                inforVenta.setNumeroCelularGirador("");
                inforVenta.setTipoDocumentoBeneficiario(datosTransaccion.get(4));
                inforVenta.setNumeroDocumentoBeneficiario(datosTransaccion.get(1));
                inforVenta.setNumeroCelularBeneficiario("");
            }
                break;

            case CodigosTransacciones.CORRESPONSAL_ENVIO_GIRO: {
                inforVenta.setTransactionType("");
                inforVenta.setMposType("");
                inforVenta.setNumberQuotas("");
                inforVenta.setAccountType("");
                inforVenta.setPartialAmount(datosTransaccion.get(2));
                inforVenta.setIva(datosTransaccion.get(3));
                inforVenta.setBase("");
                inforVenta.setInc("");
                inforVenta.setTip("");
                inforVenta.setPan("");
                inforVenta.setTotalAmount(String.valueOf((Integer.parseInt(datosTransaccion.get(2))+Integer.parseInt(datosTransaccion.get(1)))));
                inforVenta.setTagSale("");
                inforVenta.setTrack2("");
                inforVenta.setKsn("");
                inforVenta.setPinBlock("");
                inforVenta.setKsnPinBlock("");
                inforVenta.setPostEntryMode("");
                inforVenta.setArqc("");
                inforVenta.setCardName("");
                inforVenta.setApLabel("");
                inforVenta.setExpirationDate("");
                inforVenta.setAid("");
                inforVenta.setDocumentNumber("");
                inforVenta.setNumeroCuenta("");
                inforVenta.setNumeroConvenio("");
                inforVenta.setCapturaLectorBarras("");
                inforVenta.setNumeroCelular("");
                inforVenta.setNumeroTarjeta("");
                inforVenta.setNumeroPagare("");
                inforVenta.setTipoCuentaDestino("");
                inforVenta.setCuentaDestino("");
                inforVenta.setTipoDocumentoGirador(datosTransaccion.get(10));
                inforVenta.setNumeroDocumentoGirador(datosTransaccion.get(5));
                inforVenta.setNumeroCelularGirador(datosTransaccion.get(6));
                inforVenta.setTipoDocumentoBeneficiario(datosTransaccion.get(11));
                inforVenta.setNumeroDocumentoBeneficiario(datosTransaccion.get(8));
                inforVenta.setNumeroCelularBeneficiario(datosTransaccion.get(7));
            }
                break;

            case CodigosTransacciones.CORRESPONSAL_CANCELACION_GIRO_CONSULTA: {
                //pin de referencia datosTransaccion.get(4)
                inforVenta.setTransactionType("");
                inforVenta.setMposType("");
                inforVenta.setNumberQuotas("");
                inforVenta.setAccountType("");
                inforVenta.setPartialAmount("");
                inforVenta.setIva("");
                inforVenta.setBase("");
                inforVenta.setInc("");
                inforVenta.setTip("");
                inforVenta.setPan("");
                inforVenta.setTotalAmount("");
                inforVenta.setTagSale("");
                inforVenta.setTrack2("");
                inforVenta.setKsn("");
                inforVenta.setPinBlock("");
                inforVenta.setKsnPinBlock("");
                inforVenta.setPostEntryMode("");
                inforVenta.setArqc("");
                inforVenta.setCardName("");
                inforVenta.setApLabel("");
                inforVenta.setExpirationDate("");
                inforVenta.setAid("");
                inforVenta.setDocumentNumber("");
                inforVenta.setNumeroCuenta("");
                inforVenta.setNumeroConvenio("");
                inforVenta.setCapturaLectorBarras("");
                inforVenta.setNumeroCelular("");
                inforVenta.setNumeroTarjeta("");
                inforVenta.setNumeroPagare("");
                inforVenta.setTipoCuentaDestino("");
                inforVenta.setCuentaDestino("");
                inforVenta.setTipoDocumentoGirador(datosTransaccion.get(5));
                inforVenta.setNumeroDocumentoGirador(datosTransaccion.get(1));
                inforVenta.setNumeroCelularGirador("");
                inforVenta.setTipoDocumentoBeneficiario(datosTransaccion.get(6));
                inforVenta.setNumeroDocumentoBeneficiario(datosTransaccion.get(3));
                inforVenta.setNumeroCelularBeneficiario("");
            }
                break;

            case CodigosTransacciones.CORRESPONSAL_CANCELACION_GIRO: {
                inforVenta.setTransactionType("");
                inforVenta.setMposType("");
                inforVenta.setNumberQuotas("");
                inforVenta.setAccountType("");
                inforVenta.setPartialAmount("");
                inforVenta.setIva("");
                inforVenta.setBase("");
                inforVenta.setInc("");
                inforVenta.setTip("");
                inforVenta.setPan("");
                inforVenta.setTotalAmount("");
                inforVenta.setTagSale("");
                inforVenta.setTrack2("");
                inforVenta.setKsn("");
                inforVenta.setPinBlock("");
                inforVenta.setKsnPinBlock("");
                inforVenta.setPostEntryMode("");
                inforVenta.setArqc("");
                inforVenta.setCardName("");
                inforVenta.setApLabel("");
                inforVenta.setExpirationDate("");
                inforVenta.setAid("");
                inforVenta.setDocumentNumber("");
                inforVenta.setNumeroCuenta("");
                inforVenta.setNumeroConvenio("");
                inforVenta.setCapturaLectorBarras("");
                inforVenta.setNumeroCelular("");
                inforVenta.setNumeroTarjeta("");
                inforVenta.setNumeroPagare("");
                inforVenta.setTipoCuentaDestino("");
                inforVenta.setCuentaDestino("");
                inforVenta.setTipoDocumentoGirador(datosTransaccion.get(5));
                inforVenta.setNumeroDocumentoGirador(datosTransaccion.get(1));
                inforVenta.setNumeroCelularGirador("");
                inforVenta.setTipoDocumentoBeneficiario(datosTransaccion.get(6));
                inforVenta.setNumeroDocumentoBeneficiario(datosTransaccion.get(3));
                inforVenta.setNumeroCelularBeneficiario("");
            }
                break;

            case CodigosTransacciones.CORRESPONSAL_RETIRO_SIN_TARJETA: {
                inforVenta.setTransactionType("");
                inforVenta.setMposType("");
                inforVenta.setNumberQuotas("");
                inforVenta.setAccountType("");
                inforVenta.setPartialAmount("");
                inforVenta.setIva("");
                inforVenta.setBase("");
                inforVenta.setInc("");
                inforVenta.setTip("");
                inforVenta.setPan("");
                inforVenta.setTotalAmount(datosTransaccion.get(1));
                inforVenta.setTagSale("");
                inforVenta.setTrack2("");
                inforVenta.setKsn("");
                inforVenta.setPinBlock(datosTransaccion.get(2));
                inforVenta.setKsnPinBlock("");
                inforVenta.setPostEntryMode("");
                inforVenta.setArqc("");
                inforVenta.setCardName("");
                inforVenta.setApLabel("");
                inforVenta.setExpirationDate("");
                inforVenta.setAid("");
                inforVenta.setDocumentNumber("");
                inforVenta.setNumeroCuenta(datosTransaccion.get(0));
                inforVenta.setNumeroConvenio("");
                inforVenta.setCapturaLectorBarras("");
                inforVenta.setNumeroCelular("");
                inforVenta.setNumeroTarjeta("");
                inforVenta.setNumeroPagare("");
                inforVenta.setTipoCuentaDestino("");
                inforVenta.setCuentaDestino("");
                inforVenta.setTipoDocumentoGirador("");
                inforVenta.setNumeroDocumentoGirador("");
                inforVenta.setNumeroCelularGirador("");
                inforVenta.setTipoDocumentoBeneficiario("");
                inforVenta.setNumeroDocumentoBeneficiario("");
                inforVenta.setNumeroCelularBeneficiario("");
            }
                break;

            case CodigosTransacciones.CORRESPONSAL_RETIRO_CON_TARJETA: {
                inforVenta.setTransactionType("");
                inforVenta.setMposType("");
                inforVenta.setNumberQuotas("");
                inforVenta.setAccountType("");
                inforVenta.setPartialAmount("");
                inforVenta.setIva("");
                inforVenta.setBase("");
                inforVenta.setInc("");
                inforVenta.setTip("");
                inforVenta.setPan("");
                inforVenta.setTotalAmount(datosTransaccion.get(0));
                inforVenta.setTagSale("");
                inforVenta.setTrack2("");
                inforVenta.setKsn("");
                inforVenta.setPinBlock("");
                inforVenta.setKsnPinBlock("");
                inforVenta.setPostEntryMode("");
                inforVenta.setArqc("");
                inforVenta.setCardName("");
                inforVenta.setApLabel("");
                inforVenta.setExpirationDate("");
                inforVenta.setAid("");
                inforVenta.setDocumentNumber("");
                if (datosTransaccion.size()>3){
                    inforVenta.setNumeroCuenta(datosTransaccion.get(2));
                }else{
                    inforVenta.setNumeroCuenta("");
                }
                inforVenta.setNumeroConvenio("");
                inforVenta.setCapturaLectorBarras("");
                inforVenta.setNumeroCelular("");
                inforVenta.setNumeroTarjeta("");
                inforVenta.setNumeroPagare("");
                inforVenta.setTipoCuentaDestino("");
                inforVenta.setCuentaDestino("");
                inforVenta.setTipoDocumentoGirador("");
                inforVenta.setNumeroDocumentoGirador("");
                inforVenta.setNumeroCelularGirador("");
                inforVenta.setTipoDocumentoBeneficiario("");
                inforVenta.setNumeroDocumentoBeneficiario("");
                inforVenta.setNumeroCelularBeneficiario("");
            }
                break;

            case CodigosTransacciones.CORRESPONSAL_CONSULTA_SALDO: {
                inforVenta.setTransactionType("");
                inforVenta.setMposType("");
                inforVenta.setNumberQuotas("");
                inforVenta.setAccountType("");
                inforVenta.setPartialAmount("");
                inforVenta.setIva("");
                inforVenta.setBase("");
                inforVenta.setInc("");
                inforVenta.setTip("");
                inforVenta.setPan("");
                inforVenta.setTotalAmount("");
                inforVenta.setTagSale("");
                inforVenta.setTrack2(tarjeta.getTrack2());
                inforVenta.setKsn(tarjeta.getKsn());
                inforVenta.setPinBlock(tarjeta.getPinBlock());
                inforVenta.setKsnPinBlock(tarjeta.getKsnPinBlock());
                inforVenta.setPostEntryMode(tarjeta.getPosEntryMode());
                inforVenta.setArqc("");
                inforVenta.setCardName("");
                inforVenta.setApLabel("");
                inforVenta.setExpirationDate("");
                inforVenta.setAid("");
                inforVenta.setDocumentNumber("");
                inforVenta.setNumeroCuenta("");
                inforVenta.setNumeroConvenio("");
                inforVenta.setCapturaLectorBarras("");
                inforVenta.setNumeroCelular("");
                inforVenta.setNumeroTarjeta("");
                inforVenta.setNumeroPagare("");
                inforVenta.setTipoCuentaDestino("");
                inforVenta.setCuentaDestino("");
                inforVenta.setTipoDocumentoGirador("");
                inforVenta.setNumeroDocumentoGirador("");
                inforVenta.setNumeroCelularGirador("");
                inforVenta.setTipoDocumentoBeneficiario("");
                inforVenta.setNumeroDocumentoBeneficiario("");
                inforVenta.setNumeroCelularBeneficiario("");
            }
                break;

            case CodigosTransacciones.CORRESPONSAL_PAGO_FACTURA_MANUAL: {
                inforVenta.setTransactionType("");
                inforVenta.setMposType("");
                inforVenta.setNumberQuotas("");
                inforVenta.setAccountType("");
                inforVenta.setPartialAmount("");
                inforVenta.setIva("");
                inforVenta.setBase("");
                inforVenta.setInc("");
                inforVenta.setTip("");
                inforVenta.setPan("");
                inforVenta.setTotalAmount("");
                inforVenta.setTagSale("");
                inforVenta.setTrack2("");
                inforVenta.setKsn("");
                inforVenta.setPinBlock("");
                inforVenta.setKsnPinBlock("");
                inforVenta.setPostEntryMode("");
                inforVenta.setArqc("");
                inforVenta.setCardName("");
                inforVenta.setApLabel("");
                inforVenta.setExpirationDate("");
                inforVenta.setAid("");
                inforVenta.setDocumentNumber("");
                inforVenta.setNumeroCuenta("");
                inforVenta.setNumeroConvenio(datosRecaudo.getNumeroFactura());
                inforVenta.setCapturaLectorBarras("");
                inforVenta.setNumeroCelular("");
                inforVenta.setNumeroTarjeta("");
                inforVenta.setNumeroPagare("");
                inforVenta.setTipoCuentaDestino("");
                inforVenta.setCuentaDestino("");
                inforVenta.setTipoDocumentoGirador("");
                inforVenta.setNumeroDocumentoGirador("");
                inforVenta.setNumeroCelularGirador("");
                inforVenta.setTipoDocumentoBeneficiario("");
                inforVenta.setNumeroDocumentoBeneficiario("");
                inforVenta.setNumeroCelularBeneficiario("");
            }
                break;

            case CodigosTransacciones.CORRESPONSAL_PAGO_FACTURA_TARJETA_EMPRESARIAL: {
                inforVenta.setTransactionType("");
                inforVenta.setMposType("");
                inforVenta.setNumberQuotas("");
                inforVenta.setAccountType("");
                inforVenta.setPartialAmount("");
                inforVenta.setIva("");
                inforVenta.setBase("");
                inforVenta.setInc("");
                inforVenta.setTip("");
                inforVenta.setPan("");
                inforVenta.setTotalAmount(datosTransaccion.get(0));
                inforVenta.setTagSale("");
                inforVenta.setTrack2(datosTransaccion.get(1));
                inforVenta.setKsn(tarjeta.getKsn());
                inforVenta.setPinBlock(tarjeta.getPinBlock());
                inforVenta.setKsnPinBlock(tarjeta.getKsnPinBlock());
                inforVenta.setPostEntryMode(tarjeta.getPosEntryMode());
                inforVenta.setArqc("");
                inforVenta.setCardName("");
                inforVenta.setApLabel("");
                inforVenta.setExpirationDate("");
                inforVenta.setAid("");
                inforVenta.setDocumentNumber("");
                inforVenta.setNumeroCuenta("");
                inforVenta.setNumeroConvenio("");
                inforVenta.setCapturaLectorBarras("");
                inforVenta.setNumeroCelular("");
                inforVenta.setNumeroTarjeta("");
                inforVenta.setNumeroPagare("");
                inforVenta.setTipoCuentaDestino("");
                inforVenta.setCuentaDestino("");
                inforVenta.setTipoDocumentoGirador("");
                inforVenta.setNumeroDocumentoGirador("");
                inforVenta.setNumeroCelularGirador("");
                inforVenta.setTipoDocumentoBeneficiario("");
                inforVenta.setNumeroDocumentoBeneficiario("");
                inforVenta.setNumeroCelularBeneficiario("");
            }
                break;

            case CodigosTransacciones.CORRESPONSAL_PAGO_PRODUCTOS: {
                //En donde se envia esta información
                //Cartera numero pagare//numero pagare, cantidad, origen
                //Tarjeta crédito// numero tarjeta, cantidad, orign
                inforVenta.setTransactionType("");
                inforVenta.setMposType("");
                inforVenta.setNumberQuotas("");
                inforVenta.setAccountType("");
                inforVenta.setPartialAmount("");
                inforVenta.setIva("");
                inforVenta.setBase("");
                inforVenta.setInc("");
                inforVenta.setTip("");
                inforVenta.setPan("");
                inforVenta.setTotalAmount(datosTransaccion.get(0));
                inforVenta.setTagSale("");
                inforVenta.setTrack2("");
                inforVenta.setKsn("");
                inforVenta.setPinBlock("");
                inforVenta.setKsnPinBlock("");
                inforVenta.setPostEntryMode("");
                inforVenta.setArqc("");
                inforVenta.setCardName("");
                inforVenta.setApLabel("");
                inforVenta.setExpirationDate("");
                inforVenta.setAid("");
                inforVenta.setDocumentNumber("");
                inforVenta.setNumeroCuenta("");
                inforVenta.setNumeroConvenio("");
                inforVenta.setCapturaLectorBarras("");
                inforVenta.setNumeroCelular("");
                inforVenta.setNumeroTarjeta(datosTransaccion.get(2));
                inforVenta.setNumeroPagare(datosTransaccion.get(2));
                inforVenta.setTipoCuentaDestino("");
                inforVenta.setCuentaDestino("");
                inforVenta.setTipoDocumentoGirador("");
                inforVenta.setNumeroDocumentoGirador("");
                inforVenta.setNumeroCelularGirador("");
                inforVenta.setTipoDocumentoBeneficiario("");
                inforVenta.setNumeroDocumentoBeneficiario("");
                inforVenta.setNumeroCelularBeneficiario("");
            }
                break;

            case CodigosTransacciones.CORRESPONSAL_TRANSFERENCIA: {
                inforVenta.setTransactionType("");
                inforVenta.setMposType("");
                inforVenta.setNumberQuotas("");
                inforVenta.setAccountType("");
                inforVenta.setPartialAmount("");
                inforVenta.setIva("");
                inforVenta.setBase("");
                inforVenta.setInc("");
                inforVenta.setTip("");
                inforVenta.setPan("");
                inforVenta.setTotalAmount(datosTransaccion.get(2));
                inforVenta.setTagSale("");
                inforVenta.setTrack2("");
                inforVenta.setKsn("");
                inforVenta.setPinBlock("");
                inforVenta.setKsnPinBlock("");
                inforVenta.setPostEntryMode("");
                inforVenta.setArqc("");
                inforVenta.setCardName("");
                inforVenta.setApLabel("");
                inforVenta.setExpirationDate("");
                inforVenta.setAid("");
                inforVenta.setDocumentNumber("");
                inforVenta.setNumeroConvenio("");
                inforVenta.setCapturaLectorBarras("");
                inforVenta.setNumeroCelular("");
                inforVenta.setNumeroTarjeta("");
                inforVenta.setNumeroPagare("");
                inforVenta.setTipoCuentaDestino("");
                if (datosTransaccion.size() > 6) {
                    //Numer0 cuenta origen
                    inforVenta.setCuentaDestino(datosTransaccion.get(5));
                    inforVenta.setNumeroCuenta(datosTransaccion.get(1));
                } else {
                    inforVenta.setCuentaDestino(datosTransaccion.get(4));
                }
                inforVenta.setTipoDocumentoGirador("");
                inforVenta.setNumeroDocumentoGirador("");
                inforVenta.setNumeroCelularGirador("");
                inforVenta.setTipoDocumentoBeneficiario("");
                inforVenta.setNumeroDocumentoBeneficiario("");
                inforVenta.setNumeroCelularBeneficiario("");
            }
                break;

            case CodigosTransacciones.CORRESPONSAL_CONSULTA_FACTURAS: {
                //Numero de faturas en donde y tipo recaudo
                inforVenta.setTransactionType("");
                inforVenta.setMposType("");
                inforVenta.setNumberQuotas("");
                inforVenta.setAccountType("");
                inforVenta.setPartialAmount("");
                inforVenta.setIva("");
                inforVenta.setBase("");
                inforVenta.setInc("");
                inforVenta.setTip("");
                inforVenta.setPan("");
                inforVenta.setTotalAmount("");
                inforVenta.setTagSale("");
                inforVenta.setTrack2("");
                inforVenta.setKsn("");
                inforVenta.setPinBlock("");
                inforVenta.setKsnPinBlock("");
                inforVenta.setPostEntryMode("");
                inforVenta.setArqc("");
                inforVenta.setCardName("");
                inforVenta.setApLabel("");
                inforVenta.setExpirationDate("");
                inforVenta.setAid("");
                inforVenta.setDocumentNumber("");
                inforVenta.setNumeroCuenta(datosTransaccion.get(1));
                inforVenta.setNumeroConvenio(datosTransaccion.get(2));
                inforVenta.setCapturaLectorBarras("");
                inforVenta.setNumeroCelular("");
                inforVenta.setNumeroTarjeta("");
                inforVenta.setNumeroPagare("");
                inforVenta.setTipoCuentaDestino("");
                inforVenta.setCuentaDestino("");
                inforVenta.setTipoDocumentoGirador("");
                inforVenta.setNumeroDocumentoGirador("");
                inforVenta.setNumeroCelularGirador("");
                inforVenta.setTipoDocumentoBeneficiario("");
                inforVenta.setNumeroDocumentoBeneficiario("");
                inforVenta.setNumeroCelularBeneficiario("");
            }
                break;
        }


        new Thread(new Runnable() {
            @Override
            public void run() {
                resp = service.generarVenta(inforVenta);
            }
        }).start();
    }


    /**Metodo cancelarValores de tipo void, recibo como parametro un View, para poder ser implementado por el boton Cancelar.
     * Se encarga de cerrar la actividad y volver a la actividad donde se hizo el respectivo llamado.*/

    public void cancelarValores(View view){
        if(transaccion == CodigosTransacciones.CORRESPONSAL_CANCELACION_GIRO){
            //Redirige al inicio de la aplicacion
        }else {
            finish();
        }
    }

    /**Metodo onBackPressed es sobreescrito para que el usuario no se pueda desplazar hacia atras con los botones del celular.*/

    @Override
    public void onBackPressed() {

    }

    /**Metodo finish es sobreescrito para que se pueda agregar la animacion a la transicion de pantallas*/

    @Override
    public void finish() {
        super.finish();
        overridePendingTransition(R.anim.slide_in_left, R.anim.slide_out_right);
    }



}