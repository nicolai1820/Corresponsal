package co.com.netcom.corresponsal.pantallas.comunes.pantallaConfirmacion;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.res.ResourcesCompat;


import co.com.netcom.corresponsal.core.comunicacion.CardDTO;
import co.com.netcom.corresponsal.core.comunicacion.DatosComision;
import co.com.netcom.corresponsal.core.comunicacion.DatosRecaudo;
import co.com.netcom.corresponsal.pantallas.funciones.CodigosTransacciones;

import android.app.AlertDialog;
import android.content.Intent;
import android.graphics.Color;
import android.graphics.Typeface;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.LinearLayout;
import android.widget.TextView;

import java.util.ArrayList;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.funciones.Hilos;

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



    }


   /**Metodo generarTextView de tipo void. Se encarga de generar dinamicamente los TextView de los valores a confirmar
    * tomando como referencia los arreglos que se rescatan del intent.*/
    public void generarTextView(){

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
        }
        else {

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

            //Se crea el loader que se mostrara mientras se procesa la transaccion
            AlertDialog.Builder loader = new AlertDialog.Builder(pantallaConfirmacion.this);

            LayoutInflater inflater = this.getLayoutInflater();

            loader.setView(inflater.inflate(R.layout.loader,null));
            loader.setCancelable(false);

            AlertDialog dialog = loader.create();
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

                            hiloTransacciones.transaccionesSinTarjeta(transaccion,datosTransaccion);
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

                            hiloTransacciones.transaccionesSinTarjeta(transaccion,datosTransaccion);
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

                            hiloTransacciones.transaccionesSinTarjeta(transaccion,datosTransaccion);
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

                            hiloTransacciones.transaccionesSinTarjeta(transaccion,datosTransaccion);
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
                    break;
                }
                //listo pero no lo estoy usando, revisar
                case CodigosTransacciones.CORRESPONSAL_CANCELACION_GIRO:{

                    new Thread() {
                        @Override
                        public void run() {

                            hiloTransacciones.cancelarGiro(valores,datosComision);
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

                            hiloTransacciones.transaccionesSinTarjeta(transaccion,datosTransaccion);
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


                    break;
                }
                //listo
                case CodigosTransacciones.CORRESPONSAL_PAGO_FACTURA_MANUAL:{
                    new Thread() {
                        @Override
                        public void run() {

                            hiloTransacciones.pagoFacturaManual(datosRecaudo);
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

                            hiloTransacciones.transaccionesSinTarjeta(transaccion,datosTransaccion);
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

                            hiloTransacciones.transaccionesSinTarjeta(transaccion,datosTransaccion);
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

                            hiloTransacciones.transaccionesSinTarjeta(transaccion,datosTransaccion);
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

                    break;
                }


                default: {
                        Log.d("DEFAULT","Default");
                    break;
                }

            }




        }
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