package co.com.netcom.corresponsal.pantallas.comunes.pantallaConfirmacion;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.res.ResourcesCompat;


import co.com.netcom.corresponsal.core.comunicacion.DatosComision;
import co.com.netcom.corresponsal.core.comunicacion.IntegradorC;
import co.com.netcom.corresponsal.pantallas.funciones.CodigosTransacciones;

import android.app.AlertDialog;
import android.content.Intent;
import android.graphics.Typeface;
import android.os.Bundle;
import android.util.Log;
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
    ArrayList<String> valores = new ArrayList<String>();
    private Typeface quicksandBold;
    private Typeface quicksandRegular;
    private int contador;
    private int transaccion;
    private ArrayList<String> tipoDocumento = new ArrayList<String>();
    private ArrayList<String> tipoDeCuenta = new ArrayList<String>();
    private ArrayList<String> datosTransaccion = new ArrayList<String>();
    private Hilos hiloTransacciones;

    private IntegradorC integradorC;
    private String valorGiro;
    private DatosComision datosComision;

    private String[] respuestaTransacion = new String[4];


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_confirmacion);

        integradorC = new IntegradorC(this);

        //hiloTransacciones = new Hilos(getApplicationContext());

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


        valorGiro = i.getString("valorGiro");
        datosComision = new DatosComision();
        datosComision = i.getParcelable("datosComision");

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

            //Se agrega el icono
            TextView icono = new TextView(getBaseContext());
            icono.setBackgroundResource(R.drawable.boton_inicio);
            icono.setText("#");
            icono.setTextColor(getResources().getColor(R.color.blanco));
            icono.setTextAlignment(TextView.TEXT_ALIGNMENT_CENTER);
            icono.setTextSize(21);


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
        }else {

            for (int j = 0; j < titulos.length; j++) {

                LinearLayout linearLayout = new LinearLayout(getBaseContext());
                linearLayout.setOrientation(LinearLayout.HORIZONTAL);

                //Se agrega el icono
                TextView icono = new TextView(getBaseContext());
                icono.setBackgroundResource(R.drawable.boton_inicio);
                icono.setText("#");
                icono.setTextColor(getResources().getColor(R.color.blanco));
                icono.setTextAlignment(TextView.TEXT_ALIGNMENT_CENTER);
                icono.setTextSize(21);


                TextView referencia = new TextView(getBaseContext());
                referencia.setTextSize(21);
                referencia.setTypeface(quicksandBold);
                referencia.setTextColor(getResources().getColor(R.color.azul));
                referencia.setText(titulos[j]);


                //Se crean los TextView de los valores.
                TextView contenido = new TextView(getBaseContext());
                referencia.setTextColor(getResources().getColor(R.color.azul));
                contenido.setTextSize(29);
                referencia.setTypeface(quicksandRegular);
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
            startActivity(intento);
            overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);

        }else {

            AlertDialog.Builder loader = new AlertDialog.Builder(pantallaConfirmacion.this);

            LayoutInflater inflater = this.getLayoutInflater();

            loader.setView(inflater.inflate(R.layout.loader,null));
            loader.setCancelable(false);

            AlertDialog dialog = loader.create();
            Log.d("OPEN"," se abrio el loader");


            dialog.show();
            datosTransaccion.clear();

            switch (transaccion){

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

                            new Thread(() -> {
                                String resultado = null;

                                resultado = integradorC.enviarDeposito(datosTransaccion.get(0),datosTransaccion.get(1),Integer.parseInt(datosTransaccion.get(2)));

                                Log.d("RESULTADO DEPOSITO",resultado);
                            }).start();

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

                                new Thread(() -> {
                                    String resultado = null;

                                    resultado = integradorC.realizarReclamacionGiro(datosTransaccion.get(2),datosTransaccion.get(3),datosTransaccion.get(0),datosTransaccion.get(1));

                                    Log.d("RESULTADO RECLAMAR GIRO",resultado);
                                }).start();

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

                            new Thread(() -> {
                                String resultado = null;
                                //Se crea un objeto de tipo datos recaudo.

                                DatosComision datosComision = new DatosComision();
                                resultado = integradorC.realizaConsultaCancelacionGiroCnb(datosComision,datosTransaccion.get(3),datosTransaccion.get(4),datosTransaccion.get(0),datosTransaccion.get(1),datosTransaccion.get(2));

                                if (resultado == null) {
                                    //PANTALLA RESULTADO TRANSACCION
                                } else {
                                    respuestaTransacion = resultado.split(";");
                                    if (respuestaTransacion[0].equals("00")) {


                                        // PANTALLA CONFIRMACIÓN CANCELAR GIRO
                                        //Agregar titulos
                                        //Agregar Descripción
                                        //Agregar tipo transaccion
                                        //agregar valor respuesta
                                        //agregar array valores
                                        //Agregar datosComision

                                        Intent intento = new Intent(getApplicationContext(),pantallaConfirmacion.class);
                                        intento.putExtra("valorGiro", respuestaTransacion[1]);
                                        intento.putExtra("valores", datosTransaccion);
                                        intento.putExtra("datosComision", datosComision);
                                        intento.putExtra("titulo","<b>CancelarGiro</b>");
                                        intento.putExtra("descripcion","Verifique el valor del giro");
                                        intento.putExtra("transaccion",CodigosTransacciones.CORRESPONSAL_CANCELACION_GIRO);

                                       /* Intent intento = new Intent(getApplicationContext(), pantallaConfirmacion.class);
                                        intento.putExtra("valorGiro", respuestaTransacion[1]);
                                        intento.putExtra("valores", datos);
                                        intento.putExtra("datosComision", datosComision);
                                        intento.putExtra("titulo","<b>CancelarGiro</b>");
                                        intento.putExtra("descripcion","Verifique el valor del giro");
                                        intento.putExtra("transaccion",CodigosTransacciones.CORRESPONSAL_CANCELACION_GIRO);

                                        startActivity(intento);*/

                                    } else {
                                        //PANTALLA RESULTADO TRANSACCION

                                   }
                                }
                            }).start();

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

                //Arriba Codigo nuevo


                // CODIGO FUNCIONAL CON CLASE HILO
                /*case CodigosTransacciones.CORRESPONSAL_DEPOSITO:{
                    for (int i=0;i < valores.size();i++){
                        datosTransaccion.add(valores.get(i));
                    }
                    for (int i =0; i<tipoDeCuenta.size();i++) {
                        datosTransaccion.add(tipoDeCuenta.get(i));
                    }



                    new Thread() {
                        @Override
                        public void run() {

                            hiloTransacciones.transacciones(transaccion,datosTransaccion);
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

                            hiloTransacciones.transacciones(transaccion,datosTransaccion);
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
                case CodigosTransacciones.CORRESPONSAL_ENVIO_GIRO:{

                    break;
                }
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

                            hiloTransacciones.transacciones(transaccion,datosTransaccion);
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
                case CodigosTransacciones.CORRESPONSAL_RETIRO_SIN_TARJETA:{
                    break;
                }
                case CodigosTransacciones.CORRESPONSAL_RETIRO_CON_TARJETA:{
                    break;
                }
                case CodigosTransacciones.CORRESPONSAL_CONSULTA_SALDO:{
                    break;
                }
                case CodigosTransacciones.CORRESPONSAL_PAGO_FACTURA_MANUAL:{
                    break;
                }
                case CodigosTransacciones.CORRESPONSAL_PAGO_FACTURA_TARJETA_EMPRESARIAL:{
                    break;
                }
                case CodigosTransacciones.CORRESPONSAL_PAGO_PRODUCTOS:{
                    break;
                }
                case CodigosTransacciones.CORRESPONSAL_TRANSFERENCIA:{
                    break;
                }
                case CodigosTransacciones.CORRESPONSAL_RECARGAS:{
                    break;
                }*/


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