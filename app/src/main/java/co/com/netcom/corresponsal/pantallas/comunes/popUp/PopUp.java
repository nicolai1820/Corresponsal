package co.com.netcom.corresponsal.pantallas.comunes.popUp;


import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.os.Handler;
import android.os.Message;
import android.text.InputFilter;
import android.text.Spanned;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import java.util.HashMap;
import java.util.Map;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.comunes.logIn.LogIn;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.consultaSaldo.pantallaConsultaSaldoLectura;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.inicio.pantallaInicialUsuarioComun;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.pagoFacturas.tarjetaEmpresarial.PantallaTarjetaEmpresarialLectura;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.retiro.conTarjeta.PantallaRetiroConTarjetaLoader;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.retiro.sinTarjeta.pantallaRetiroSinTarjetaPin;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.transferencia.pantallaTransferenciaLectura;
import co.com.netcom.corresponsal.pantallas.funciones.CodificarBase64;
import co.com.netcom.corresponsal.pantallas.funciones.ConstantesCorresponsal;
import co.com.netcom.corresponsal.pantallas.funciones.PreferencesUsuario;
import co.com.netcom.corresponsal.pantallas.funciones.Servicios;


public class PopUp extends AppCompatActivity {

    public final static int RETIRO_CON_TARJETA = 1;
    public final static int RETIRO_SIN_TARJETA_PIN=2;
    public final static int CONSULTA_SALDO_LEER_TARJETA = 3;
    public final static int TRANSFERENCIA_LEER_TARJETA=4;
    public final static int PAGO_FACTURA_EMPRESARIAL=5;


    private Context context;
    private Activity activity;
    private AlertDialog alertDialog;
    private AlertDialog alertDialogLoginFallido;
    private AlertDialog alertDialogConfirmarEnvioCorreo;
    private AlertDialog alertDialogRecuperarContrasena;
    private AlertDialog alertDialogCorreoComercio;
    private AlertDialog alertDialogCorreoCliente;
    private AlertDialog alertDialogGeneral;
    private AlertDialog alertDialogErrorServidor;
    private AlertDialog dialog;
    private Thread solicitarCierreSesion;
    private Thread solicitarEnvioCorreoComercio;
    private String respuestaCierreSesion;
    private static Map<String,String > respuestaCorreo= new HashMap<String,String>();


    /**Constructor de la clase PopUpDesconexion, recibo como parametro el contexto de la actividad donde se inicializa*/
    public PopUp(Context contexto){
        this.context = contexto;
        this.activity = (Activity) contexto;

    }

    /**Metodo de tipo void, el cual se encarga de crear un pop up para informar que el dispositivo MPOS se encuentra desconectado. */
    public void crearPopUp(){

        //Se hace la respectiva conexi??n con el frontEnd del pop up
        LayoutInflater li = LayoutInflater.from(context);
        View view = li.inflate(R.layout.activity_pop_up_error, null);
        final AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(context);
        alertDialogBuilder.setView(view);
        alertDialogBuilder.setCancelable(false);

        //Se genera la conexi??n con el boton del pop up
        Button btnSalir = view.findViewById(R.id.button_PopUpSalir);


        //Se crea el evento click para el boton del pop up, el cual redirige al inicio de la aplicacion
        btnSalir.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent i = new Intent(context, pantallaInicialUsuarioComun.class);
                i.putExtra("Fragmento",1);
                context.startActivity(i);
                activity.overridePendingTransition(R.anim.slide_in_left,R.anim.slide_out_right);


            }
        });

        //Se crea el correspondiente Dialog que se mostrara al usuario
        AlertDialog alertDialogDesconexion = alertDialogBuilder.create();
        //Se agrega esta linea para que no tenga fondo por defecto el dialog
        alertDialogDesconexion.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));

        //Se muestra el Dialogo al usuario con su correspondiente ubicacion en la pantalla
        alertDialogDesconexion.show();

        //Se obtienen las medidas de la pantalla del dispositivo
        DisplayMetrics displayMetrics = context.getResources().getDisplayMetrics();
        float dpHeight = displayMetrics.heightPixels;
        float dpWidth = displayMetrics.widthPixels ;

        alertDialogDesconexion.getWindow().setLayout((int)(dpWidth*0.7), LinearLayout.LayoutParams.WRAP_CONTENT);//

    }

    /**Metodo de tipo void que se encarga de crear un pop up con el correspondiente mensaje de error y permite intentar nuevamente la operaci??n */
    public void crearPopUpError(String Error, int codigo){

        //Se hace la respectiva conexi??n con el frontEnd del pop up
        LayoutInflater li = LayoutInflater.from(context);
        View view = li.inflate(R.layout.activity_pop_up_error_mpos, null);
        final AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(context);
        alertDialogBuilder.setView(view);
        alertDialogBuilder.setCancelable(false);


        //Se genera la conexi??n con diversos elementos  del pop up
        TextView descripcionErro = view.findViewById(R.id.textView_PopUp_Error);
        Button btnSalirError = view.findViewById(R.id.button_PopUpSalirError);
        Button btnIntentarNuevo = view.findViewById(R.id.button_PopUpVolverIntentar);

        //Se agrega el contenido del error
        descripcionErro.setText(Error);


        //Se crea el evento click para el boton del pop up, el cual redirige al inicio de la aplicacion

        btnSalirError.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent i = new Intent(context, pantallaInicialUsuarioComun.class);
                context.startActivity(i);
                activity.overridePendingTransition(R.anim.slide_in_left,R.anim.slide_out_right);
            }
        });


        //Se crea el evento click para el boton del pop up, el cual permite intentar nuevamente la operaci??n, dependiendo de la pantalla.
        btnIntentarNuevo.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                switch (codigo){
                    case RETIRO_CON_TARJETA:
                            alertDialog.dismiss();

                        try{
                            //Se envia un mensaje al handler de la clase retiro con tarjeta, para que se vuelva
                            //a habilitar la lectura de la tarjeta nuevamente
                            Message lecturaNueva = new Message();
                            lecturaNueva.what = PantallaRetiroConTarjetaLoader.NUEVO_INTENTO;
                            PantallaRetiroConTarjetaLoader.respuesta.sendMessage(lecturaNueva);

                        }catch (Exception e){ }


                        break;

                    case RETIRO_SIN_TARJETA_PIN:
                        alertDialog.dismiss();

                        try {
                            //Se envia un mensaje al handler de la clase retiro sin tarjeta, para que se vuelva
                            //a habilitar la lectura del pin nuevamente
                            Message lecturaNueva = new Message();
                            lecturaNueva.what = pantallaRetiroSinTarjetaPin.NUEVO_INTENTO;
                            pantallaRetiroSinTarjetaPin.respuesta.sendMessage(lecturaNueva);
                        }catch (Exception e){ }

                        break;

                    case CONSULTA_SALDO_LEER_TARJETA:
                        alertDialog.dismiss();
                        try{
                            //Se envia un mensaje al handler de la clase retiro con tarjeta, para que se vuelva
                            //a habilitar la lectura de la tarjeta nuevamente
                            Message lecturaNueva = new Message();
                            lecturaNueva.what = pantallaConsultaSaldoLectura.NUEVO_INTENTO;
                            pantallaConsultaSaldoLectura.respuesta.sendMessage(lecturaNueva);

                        }catch (Exception e){ }

                        break;

                    case TRANSFERENCIA_LEER_TARJETA:
                        alertDialog.dismiss();
                        try{
                            //Se envia un mensaje al handler de la clase retiro con tarjeta, para que se vuelva
                            //a habilitar la lectura de la tarjeta nuevamente
                            Message lecturaNueva = new Message();
                            lecturaNueva.what = pantallaTransferenciaLectura.NUEVO_INTENTO;
                            pantallaTransferenciaLectura.respuesta.sendMessage(lecturaNueva);

                        }catch (Exception e){ }

                        break;

                    case PAGO_FACTURA_EMPRESARIAL:
                        alertDialog.dismiss();
                        try{
                            //Se envia un mensaje al handler de la clase retiro con tarjeta, para que se vuelva
                            //a habilitar la lectura de la tarjeta nuevamente
                            Message lecturaNueva = new Message();
                            lecturaNueva.what = PantallaTarjetaEmpresarialLectura.NUEVO_INTENTO;
                            PantallaTarjetaEmpresarialLectura.respuesta.sendMessage(lecturaNueva);

                        }catch (Exception e){ }

                        break;

                    default:
                        break;
                }

            }
        });

        //Se crea el correspondiente Dialog que se mostrara al usuario
        alertDialog = alertDialogBuilder.create();
        //Se agrega esta linea para que no tenga fondo por defecto el dialog
        alertDialog.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));

        //Se muestra el Dialogo al usuario con su correspondiente ubicacion en la pantalla
        alertDialog.show();

        //Se obtienen las medidas de la pantalla
        DisplayMetrics displayMetrics = context.getResources().getDisplayMetrics();
        float dpHeight = displayMetrics.heightPixels;
        float dpWidth = displayMetrics.widthPixels ;

        alertDialog.getWindow().setLayout((int)(dpWidth*0.7), LinearLayout.LayoutParams.WRAP_CONTENT);//

    }

    /**Metodo de tipo void que se encarga de crear un pop up para notificar que no hay internet*/
    public void crearPopUpErrorInternet(){
        //Se hace la respectiva conexi??n con el frontEnd del pop up
        LayoutInflater li = LayoutInflater.from(context);
        View view = li.inflate(R.layout.activity_pop_up_error, null);
        final AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(context);
        alertDialogBuilder.setView(view);
        alertDialogBuilder.setCancelable(false);

        //Se genera la conexi??n con el boton del pop up
        TextView textViewErrorServidor = view.findViewById(R.id.textView_PopUp);
        Button btnAceptar= view.findViewById(R.id.button_PopUpSalir);
        btnAceptar.setText("Aceptar");
        textViewErrorServidor.setText("Verifica tu conexi??n a internet");

        //Se crea el evento click para el boton del pop up, el cual redirige al inicio de la aplicacion
        btnAceptar.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                alertDialogGeneral.dismiss();
            }
        });

        //Se crea el correspondiente Dialog que se mostrara al usuario
        alertDialogGeneral = alertDialogBuilder.create();
        //Se agrega esta linea para que no tenga fondo por defecto el dialog
        alertDialogGeneral.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));

        //Se muestra el Dialogo al usuario con su correspondiente ubicacion en la pantalla
        alertDialogGeneral.show();

        //Se obtiene las medidas de la pantalla
        DisplayMetrics displayMetrics = context.getResources().getDisplayMetrics();
        float dpHeight = displayMetrics.heightPixels;
        float dpWidth = displayMetrics.widthPixels ;

        alertDialogGeneral.getWindow().setLayout((int)(dpWidth*0.7), LinearLayout.LayoutParams.WRAP_CONTENT);//

    }

    /**Metodo de tipo void que se encarga de crear un pop up para notificar que existe un error con la comunicaci??n al servidor*/
    public void crearPopUpErrorServidor(){
        //Se hace la respectiva conexi??n con el frontEnd del pop up
        LayoutInflater li = LayoutInflater.from(context);
        View view = li.inflate(R.layout.activity_pop_up_error, null);
        final AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(context);
        alertDialogBuilder.setView(view);
        alertDialogBuilder.setCancelable(false);

        //Se genera la conexi??n con el boton del pop up
        TextView textViewErrorServidor = view.findViewById(R.id.textView_PopUp);
        Button btnAceptar= view.findViewById(R.id.button_PopUpSalir);
        btnAceptar.setText("Aceptar");
        textViewErrorServidor.setText("Ocurrio un error en la comunicaci??n con el servidor");

        //Se crea el evento click para el boton del pop up, el cual redirige al inicio de la aplicacion
        btnAceptar.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                alertDialogErrorServidor.dismiss();
            }
        });

        //Se crea el correspondiente Dialog que se mostrara al usuario
        alertDialogErrorServidor = alertDialogBuilder.create();
        //Se agrega esta linea para que no tenga fondo por defecto el dialog
        alertDialogErrorServidor.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));

        //Se muestra el Dialogo al usuario con su correspondiente ubicacion en la pantalla
        alertDialogErrorServidor.show();

        //Se obtienen la medidas de la pantalla del dispositivo
        DisplayMetrics displayMetrics = context.getResources().getDisplayMetrics();
        float dpHeight = displayMetrics.heightPixels;
        float dpWidth = displayMetrics.widthPixels ;

        alertDialogErrorServidor.getWindow().setLayout((int)(dpWidth*0.7), LinearLayout.LayoutParams.WRAP_CONTENT);//

    }

    /**Metodo de tipo void, el cual se encarga de crear un pop up para informar que la transaccion la cancelo el usuario */
    public void crearPopUpCanceladaUsuario(){

        //Se hace la respectiva conexi??n con el frontEnd del pop up
        LayoutInflater li = LayoutInflater.from(context);
        View view = li.inflate(R.layout.activity_pop_up_error, null);
        final AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(context);
        alertDialogBuilder.setView(view);
        alertDialogBuilder.setCancelable(false);

        //Se genera la conexi??n con el boton del pop up
        TextView textViewUsuarioCancela = view.findViewById(R.id.textView_PopUp);
        Button btnSalir = view.findViewById(R.id.button_PopUpSalir);
        btnSalir.setText("Aceptar");
        textViewUsuarioCancela.setText("Cancelado por el usuario");



        //Se crea el evento click para el boton del pop up, el cual redirige al inicio de la aplicacion
        btnSalir.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent i = new Intent(context, pantallaInicialUsuarioComun.class);
                context.startActivity(i);
                activity.overridePendingTransition(R.anim.slide_in_left,R.anim.slide_out_right);

            }
        });

        //Se crea el correspondiente Dialog que se mostrara al usuario
        AlertDialog alertDialogUsuarioCancela = alertDialogBuilder.create();
        //Se agrega esta linea para que no tenga fondo por defecto el dialog
        alertDialogUsuarioCancela.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));

        //Se muestra el Dialogo al usuario con su correspondiente ubicacion en la pantalla
        alertDialogUsuarioCancela.show();

        //Se crea para ajustar el pop up al tama??o de la pantalla
        DisplayMetrics displayMetrics = context.getResources().getDisplayMetrics();
        float dpHeight = displayMetrics.heightPixels;
        float dpWidth = displayMetrics.widthPixels ;

        alertDialogUsuarioCancela.getWindow().setLayout((int)(dpWidth*0.7), LinearLayout.LayoutParams.WRAP_CONTENT);//

    }

    /**Metodo de tipo void, el cual se encarga de crear un pop up para informar que el login fallo */
    public void crearPopUpLoginFallido(String mensaje){

        Handler resp = new Handler(){
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what) {
                    case 1:
                        if(respuestaCierreSesion.equals("MQ==")){
                            alertDialogLoginFallido.dismiss();
                            dialog.dismiss();
                            crearPopUpGeneral("Se cerro sesi??n correctamente");
                        }else{
                            crearPopUpLoginFallido("Ocurrio un error al cerrar sesi??n");
                        }

                }}};

        //Se verifica si el usuario tiene una sesi??n iniciada
        if(mensaje.equals("Ya tiene una sesion activa, debe cerrar sesion o esperar el cierre automatico")){
            //Se hace la respectiva conexi??n con el frontEnd del pop up
            LayoutInflater li = LayoutInflater.from(context);
            View view = li.inflate(R.layout.activity_pop_up_error_mpos, null);

            final AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(context);
            alertDialogBuilder.setView(view);
            alertDialogBuilder.setCancelable(false);

            //Se genera la conexi??n con el boton del pop up
            TextView textViewErrorServidor = view.findViewById(R.id.textView_PopUp_Error);
            Button btnAceptar= view.findViewById(R.id.button_PopUpSalirError);
            Button btnCerrar= view.findViewById(R.id.button_PopUpVolverIntentar);

            btnCerrar.setText("Cerrar");
            btnAceptar.setText("Aceptar");
            textViewErrorServidor.setText(mensaje);

            DisplayMetrics displayMetrics = context.getResources().getDisplayMetrics();
            float dpHeight = displayMetrics.heightPixels;
            float dpWidth = displayMetrics.widthPixels ;


            //Se crea el evento click para el boton del pop up, el cual redirige al inicio de la aplicacion
            btnAceptar.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    alertDialogLoginFallido.dismiss();
                }
            });

            //Se crea el evento click para el boton del pop up, el cual redirige al inicio de la aplicacion
            btnCerrar.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    //Se crea el loader que se mostrara mientras se procesa la transaccion
                    AlertDialog.Builder loader = new AlertDialog.Builder(context);
                    LayoutInflater inflater = activity.getLayoutInflater();
                    loader.setView(inflater.inflate(R.layout.loader_procesando_transaccion,null));
                    loader.setCancelable(false);

                    dialog = loader.create();
                    dialog.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));

                    Log.d("OPEN"," se abrio el loader");
                    dialog.show();

                    alertDialogLoginFallido.dismiss();
                    Servicios serv = new Servicios(context);
                    solicitarCierreSesion = new Thread(new Runnable() {
                        @Override
                        public void run() {
                            respuestaCierreSesion = serv.cerrarSesion();
                            try{
                                //Se envia un mensaje al handler de la clase consulta saldo, indicando que el usuario cancelo la transaccion
                                Message usuarioCancela = new Message();
                                usuarioCancela.what = 1;
                                resp.sendMessage(usuarioCancela);
                            }catch (Exception e){ }
                        }
                    });

                    solicitarCierreSesion.start();
                }
            });

            //Se crea el correspondiente Dialog que se mostrara al usuario
            alertDialogLoginFallido = alertDialogBuilder.create();
            //Se agrega esta linea para que no tenga fondo por defecto el dialog
            alertDialogLoginFallido.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));

            //Se muestra el Dialogo al usuario con su correspondiente ubicacion en la pantalla
            alertDialogLoginFallido.show();


            Log.d("heigh",String.valueOf(dpHeight));
            Log.d("width",String.valueOf(dpWidth*0.9));

            alertDialogLoginFallido.getWindow().setLayout((int)(dpWidth*0.7), LinearLayout.LayoutParams.WRAP_CONTENT);//
        }
        else{
            Log.d("e","else pop up");

            //Se hace la respectiva conexi??n con el frontEnd del pop up
            LayoutInflater li = LayoutInflater.from(context);
            View view = li.inflate(R.layout.activity_pop_up_error, null);

            final AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(context);
            alertDialogBuilder.setView(view);
            alertDialogBuilder.setCancelable(false);

            //Se genera la conexi??n con el boton del pop up
            TextView textViewErrorServidor = view.findViewById(R.id.textView_PopUp);
            Button btnAceptar= view.findViewById(R.id.button_PopUpSalir);
            btnAceptar.setText("Aceptar");
            textViewErrorServidor.setText(mensaje);

            DisplayMetrics displayMetrics = context.getResources().getDisplayMetrics();
            float dpHeight = displayMetrics.heightPixels;
            float dpWidth = displayMetrics.widthPixels ;


            //Se crea el evento click para el boton del pop up, el cual redirige al inicio de la aplicacion
            btnAceptar.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    alertDialogLoginFallido.dismiss();
                }
            });

            //Se crea el correspondiente Dialog que se mostrara al usuario
            alertDialogLoginFallido = alertDialogBuilder.create();
            //Se agrega esta linea para que no tenga fondo por defecto el dialog
            alertDialogLoginFallido.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));

            //Se muestra el Dialogo al usuario con su correspondiente ubicacion en la pantalla
            alertDialogLoginFallido.show();


            Log.d("heigh",String.valueOf(dpHeight));
            Log.d("width",String.valueOf(dpWidth*0.9));

            alertDialogLoginFallido.getWindow().setLayout((int)(dpWidth*0.7), LinearLayout.LayoutParams.WRAP_CONTENT);//
        }

    }

    /**Metodo de tipo void, el cual se encarga de crear un pop up para informar que el cierre de sesi??n fue fallido */
    public void crearPopUpLogOutFallido(){

        //Se hace la respectiva conexi??n con el frontEnd del pop up
        LayoutInflater li = LayoutInflater.from(context);
        View view = li.inflate(R.layout.activity_pop_up_error, null);
        final AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(context);
        alertDialogBuilder.setView(view);
        alertDialogBuilder.setCancelable(false);

        //Se genera la conexi??n con el boton del pop up
        TextView textViewErrorServidor = view.findViewById(R.id.textView_PopUp);
        Button btnAceptar= view.findViewById(R.id.button_PopUpSalir);
        btnAceptar.setText("Aceptar");
        textViewErrorServidor.setText("Hubo un error al cerrar sesi??n");

        DisplayMetrics displayMetrics = context.getResources().getDisplayMetrics();
        float dpHeight = displayMetrics.heightPixels;
        float dpWidth = displayMetrics.widthPixels ;

        //Se crea el evento click para el boton del pop up, el cual redirige al inicio de la aplicacion
        btnAceptar.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                alertDialogLoginFallido.dismiss();
            }
        });

        //Se crea el correspondiente Dialog que se mostrara al usuario
        alertDialogLoginFallido = alertDialogBuilder.create();
        //Se agrega esta linea para que no tenga fondo por defecto el dialog
        alertDialogLoginFallido.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));

        //Se muestra el Dialogo al usuario con su correspondiente ubicacion en la pantalla
        alertDialogLoginFallido.show();


        Log.d("heigh",String.valueOf(dpHeight));
        Log.d("width",String.valueOf(dpWidth*0.9));

        alertDialogLoginFallido.getWindow().setLayout((int)(dpWidth*0.7), LinearLayout.LayoutParams.WRAP_CONTENT);//


    }

    /**Metodo de tipo void, el cual se encarga de crear un pop up para recuperar la contrase??a */
    public void crearPopUpRecuperarContrasena(){

        //Se hace la respectiva conexi??n con el frontEnd del pop up
        LayoutInflater li = LayoutInflater.from(context);
        View view = li.inflate(R.layout.activity_pop_up_recuperar_contrasena, null);
        final AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(context);
        alertDialogBuilder.setView(view);
        alertDialogBuilder.setCancelable(true);


        //Se genera la conexi??n con el boton del pop up
        EditText usuario = view.findViewById(R.id.editText_PopUpRecuperarContrasenaUsuario);
        Button btnAceptar= view.findViewById(R.id.button_PopUpRecuperarContrasenaAceptar);
/*
        Button btnCancelar= view.findViewById(R.id.button_PopUpRecuperarContrasenaCancelar);
*/

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
        usuario.setFilters(new InputFilter[]{textFilter});


        //Se crea el evento click para el boton del pop up, el cual redirige al inicio de la aplicacion
        btnAceptar.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String usuario_string = usuario.getText().toString();
                CodificarBase64 base64 = new CodificarBase64();
                Servicios service = new Servicios(context);

                if (usuario_string.isEmpty()){
                    Toast.makeText(context, "Debe ingresar un usuario", Toast.LENGTH_SHORT).show();
                }else{
                    alertDialogRecuperarContrasena.dismiss();

                    Activity act = (Activity) context;

                    //Se crea el loader que se mostrara mientras se procesa la transaccion
                    AlertDialog.Builder loader = new AlertDialog.Builder(act);



                    LayoutInflater inflater = act.getLayoutInflater();

                    loader.setView(inflater.inflate(R.layout.loader_procesando_transaccion,null));
                    loader.setCancelable(false);

                    dialog = loader.create();
                    dialog.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));

                    Log.d("OPEN"," se abrio el loader");
                    dialog.show();



                    //Se crea hilo para hacer la petici??n al servidor del token
                   Thread solicitudToken =  new Thread(new Runnable() {
                        @Override
                        public void run() {
                            String token = service.solicitarToken();
                        }
                    });

                    solicitudToken.start();


                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            service.obtenerContrasenaTemporal(base64.convertirBase64(usuario_string));
                            dialog.dismiss();
                        }
                    }).start();
                }

            }
        });

   /*     //Se crea el evento click para el boton del pop up, el cual redirige al inicio de la aplicacion
        btnCancelar.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                alertDialogRecuperarContrasena.dismiss();
            }
        });*/
        //Se obitenen las medidas de la pantalla
        DisplayMetrics displayMetrics = context.getResources().getDisplayMetrics();
        float dpHeight = displayMetrics.heightPixels;
        float dpWidth = displayMetrics.widthPixels ;

        //Se crea el correspondiente Dialog que se mostrara al usuario
        alertDialogRecuperarContrasena = alertDialogBuilder.create();
        //Se agrega esta linea para que no tenga fondo por defecto el dialog
        alertDialogRecuperarContrasena.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));

        //Se muestra el Dialogo al usuario con su correspondiente ubicacion en la pantalla
        alertDialogRecuperarContrasena.show();
        alertDialogRecuperarContrasena.getWindow().setLayout((int)(dpWidth*0.7),LinearLayout.LayoutParams.WRAP_CONTENT);


    }

    /**Metodo de tipo void, el cual se encarga de crear un pop up para informar que la transacci??n no es soportada por Contactless */
    public void crearPopUpContactless(String mensaje){

        //Se hace la respectiva conexi??n con el frontEnd del pop up
        LayoutInflater li = LayoutInflater.from(context);
        View view = li.inflate(R.layout.activity_pop_up_error, null);
        final AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(context);
        alertDialogBuilder.setView(view);
        alertDialogBuilder.setCancelable(false);

        //Se genera la conexi??n con el boton del pop up
        TextView textViewErrorServidor = view.findViewById(R.id.textView_PopUp);
        Button btnAceptar= view.findViewById(R.id.button_PopUpSalir);
        btnAceptar.setText("Aceptar");
        textViewErrorServidor.setText(mensaje);



        //Se crea el evento click para el boton del pop up, el cual redirige al inicio de la aplicacion
        btnAceptar.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                Intent i = new Intent(context, pantallaInicialUsuarioComun.class);
                context.startActivity(i);
                activity.overridePendingTransition(R.anim.slide_in_left,R.anim.slide_out_right);
            }
        });

        //Se crea el correspondiente Dialog que se mostrara al usuario
        alertDialogLoginFallido = alertDialogBuilder.create();
        //Se agrega esta linea para que no tenga fondo por defecto el dialog
        alertDialogLoginFallido.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));

        //Se muestra el Dialogo al usuario con su correspondiente ubicacion en la pantalla
        alertDialogLoginFallido.show();
        //Se obitenen las medidas de la pantalla
        DisplayMetrics displayMetrics = context.getResources().getDisplayMetrics();
        float dpHeight = displayMetrics.heightPixels;
        float dpWidth = displayMetrics.widthPixels ;


        alertDialogLoginFallido.getWindow().setLayout((int)(dpWidth*0.7), LinearLayout.LayoutParams.WRAP_CONTENT);//

    }


    /**Metodo de tipo void, el cual se encarga de crear un pop up para informar que el login fallo */
    public void crearPopUpGeneral(String mensaje){


            //Se hace la respectiva conexi??n con el frontEnd del pop up
            LayoutInflater li = LayoutInflater.from(context);
            View view = li.inflate(R.layout.activity_pop_up_error, null);

            final AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(context);
            alertDialogBuilder.setView(view);
            alertDialogBuilder.setCancelable(false);

            //Se genera la conexi??n con el boton del pop up
            TextView textViewErrorServidor = view.findViewById(R.id.textView_PopUp);
            Button btnAceptar= view.findViewById(R.id.button_PopUpSalir);
            btnAceptar.setText("Aceptar");
            textViewErrorServidor.setText(mensaje);

            DisplayMetrics displayMetrics = context.getResources().getDisplayMetrics();
            float dpHeight = displayMetrics.heightPixels;
            float dpWidth = displayMetrics.widthPixels ;


            //Se crea el evento click para el boton del pop up, el cual redirige al inicio de la aplicacion
            btnAceptar.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    alertDialogLoginFallido.dismiss();
                }
            });

            //Se crea el correspondiente Dialog que se mostrara al usuario
            alertDialogLoginFallido = alertDialogBuilder.create();
            //Se agrega esta linea para que no tenga fondo por defecto el dialog
            alertDialogLoginFallido.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));

            //Se muestra el Dialogo al usuario con su correspondiente ubicacion en la pantalla
            alertDialogLoginFallido.show();


            Log.d("heigh",String.valueOf(dpHeight));
            Log.d("width",String.valueOf(dpWidth*0.9));

            alertDialogLoginFallido.getWindow().setLayout((int)(dpWidth*0.7), LinearLayout.LayoutParams.WRAP_CONTENT);//


    }

    /**Metodo de tipo void, el cual se encarga de crear un pop up para informar que se envio el correo exitosamente al usuario */
    public void crearPopUpEnvioContrasena(String mensaje){


        //Se hace la respectiva conexi??n con el frontEnd del pop up
        LayoutInflater li = LayoutInflater.from(context);
        View view = li.inflate(R.layout.activity_pop_up_olvide_contrasena_correo, null);

        final AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(context);
        alertDialogBuilder.setView(view);
        alertDialogBuilder.setCancelable(false);

        //Se genera la conexi??n con el boton del pop up
        TextView textViewErrorServidor = view.findViewById(R.id.textView_EnvioContrasenaPopUp);
        Button btnAceptar= view.findViewById(R.id.button_RncioContrasenaPopUp);
        btnAceptar.setText("Aceptar");
        textViewErrorServidor.setText(mensaje);

        DisplayMetrics displayMetrics = context.getResources().getDisplayMetrics();
        float dpHeight = displayMetrics.heightPixels;
        float dpWidth = displayMetrics.widthPixels ;


        //Se crea el evento click para el boton del pop up, el cual redirige al inicio de la aplicacion
        btnAceptar.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent i = new Intent(context, LogIn.class);
                context.startActivity(i);
                Log.d("Actividad", activity.toString());
                activity.overridePendingTransition(R.anim.slide_in_left, R.anim.slide_out_right);
            }
        });

        //Se crea el correspondiente Dialog que se mostrara al usuario
        alertDialogLoginFallido = alertDialogBuilder.create();
        //Se agrega esta linea para que no tenga fondo por defecto el dialog
        alertDialogLoginFallido.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));

        //Se muestra el Dialogo al usuario con su correspondiente ubicacion en la pantalla
        alertDialogLoginFallido.show();


        Log.d("heigh",String.valueOf(dpHeight));
        Log.d("width",String.valueOf(dpWidth*0.9));

        alertDialogLoginFallido.getWindow().setLayout((int)(dpWidth*0.7), LinearLayout.LayoutParams.WRAP_CONTENT);//


    }

    /**Metodo de tipo void, el cual se encarga de crear un pop up para informar que el login fallo */
    public void crearPopUpCambioContrasenaExitoso(String mensaje){


        //Se hace la respectiva conexi??n con el frontEnd del pop up
        LayoutInflater li = LayoutInflater.from(context);
        View view = li.inflate(R.layout.activity_pop_up_error, null);

        final AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(context);
        alertDialogBuilder.setView(view);
        alertDialogBuilder.setCancelable(false);

        //Se genera la conexi??n con el boton del pop up
        TextView textViewErrorServidor = view.findViewById(R.id.textView_PopUp);
        Button btnAceptar= view.findViewById(R.id.button_PopUpSalir);
        btnAceptar.setText("Aceptar");
        textViewErrorServidor.setText(mensaje);

        DisplayMetrics displayMetrics = context.getResources().getDisplayMetrics();
        float dpHeight = displayMetrics.heightPixels;
        float dpWidth = displayMetrics.widthPixels ;


        //Se crea el evento click para el boton del pop up, el cual redirige al inicio de la aplicacion
        btnAceptar.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent i = new Intent(context, LogIn.class);
                context.startActivity(i);
                Log.d("Actividad", activity.toString());
                activity.overridePendingTransition(R.anim.slide_in_left, R.anim.slide_out_right);            }
        });

        //Se crea el correspondiente Dialog que se mostrara al usuario
        alertDialogLoginFallido = alertDialogBuilder.create();
        //Se agrega esta linea para que no tenga fondo por defecto el dialog
        alertDialogLoginFallido.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));

        //Se muestra el Dialogo al usuario con su correspondiente ubicacion en la pantalla
        alertDialogLoginFallido.show();


        Log.d("heigh",String.valueOf(dpHeight));
        Log.d("width",String.valueOf(dpWidth*0.9));

        alertDialogLoginFallido.getWindow().setLayout((int)(dpWidth*0.7), LinearLayout.LayoutParams.WRAP_CONTENT);//


    }

    /**Metodo de tipo void, el cual se encarga de crear un pop up para confirmar que no quieren enviar correo al cliente*/
    public void crearPopUpConfirmarEnvioCorreo(int codigoTransaccion,String idTransaccion){

        Handler resp = new Handler(){
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what) {
                    case 1:

                        alertDialogConfirmarEnvioCorreo.dismiss();
                        //Poner pop up resultado correo
                        if(respuestaCorreo.get("responseCode").equals("MQ==")){
                            crearPopUpResultadoCorreoComercio("Correo enviado exitosamente al comercio");
                        }else{
                            crearPopUpResultadoCorreoComercio("Hubo un error en el envio del correo al comercio");
                        }

                }}};

        //Se hace la respectiva conexi??n con el frontEnd del pop up
        LayoutInflater li = LayoutInflater.from(context);
        View view = li.inflate(R.layout.activity_pop_up_error_mpos, null);

        final AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(context);
        alertDialogBuilder.setView(view);
        alertDialogBuilder.setCancelable(false);

        //Se genera la conexi??n con el boton del pop up
        TextView textViewErrorServidor = view.findViewById(R.id.textView_PopUp_Error);
        Button btnAceptar= view.findViewById(R.id.button_PopUpSalirError);
        Button btnCerrar= view.findViewById(R.id.button_PopUpVolverIntentar);

        btnCerrar.setText("Cancelar");
        btnAceptar.setText("Aceptar");
        textViewErrorServidor.setText("Esta seguro que no desea enviar copia al cliente?");

        DisplayMetrics displayMetrics = context.getResources().getDisplayMetrics();
        float dpHeight = displayMetrics.heightPixels;
        float dpWidth = displayMetrics.widthPixels ;


        //Se crea el evento click para el boton del pop up, el cual redirige al inicio de la aplicacion
        btnCerrar.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                //Envia copia al comercio
                alertDialogConfirmarEnvioCorreo.dismiss();
            }
        });

        //Se crea el evento click para el boton del pop up, el cual redirige al inicio de la aplicacion
        btnAceptar.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Servicios serv = new Servicios(context);
                CodificarBase64 base64 = new CodificarBase64();
                PreferencesUsuario prefsParam = new PreferencesUsuario(ConstantesCorresponsal.SHARED_PREFERENCES_PARAMETRICAS,context);
                solicitarEnvioCorreoComercio = new Thread(new Runnable() {
                    @Override
                    public void run() {
                        //solicitar envio correo

                                respuestaCorreo = serv.envioCorreo(idTransaccion,base64.decodificarBase64(prefsParam.getCommerceMail()),codigoTransaccion);
                        try{
                            //Se envia un mensaje al handler de la clase consulta saldo, indicando que el usuario cancelo la transaccion
                            Message usuarioCancela = new Message();
                            usuarioCancela.what = 1;
                            resp.sendMessage(usuarioCancela);
                        }catch (Exception e){ }
                    }
                });

                solicitarEnvioCorreoComercio.start();
            }
        });

        //Se crea el correspondiente Dialog que se mostrara al usuario
        alertDialogConfirmarEnvioCorreo = alertDialogBuilder.create();
        //Se agrega esta linea para que no tenga fondo por defecto el dialog
        alertDialogConfirmarEnvioCorreo.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));

        //Se muestra el Dialogo al usuario con su correspondiente ubicacion en la pantalla
        alertDialogConfirmarEnvioCorreo.show();


        Log.d("heigh",String.valueOf(dpHeight));
        Log.d("width",String.valueOf(dpWidth*0.9));

        alertDialogConfirmarEnvioCorreo.getWindow().setLayout((int)(dpWidth*0.7), LinearLayout.LayoutParams.WRAP_CONTENT);//
    }

    /**Metodo de tipo void, el cual se encarga de crear un pop up para informar el resultado del correo y muestra el resultado del correo del cliente*/
    public void crearPopUpResultadoCorreoComercio(String mensaje){


        //Se hace la respectiva conexi??n con el frontEnd del pop up
        LayoutInflater li = LayoutInflater.from(context);
        View view = li.inflate(R.layout.activity_pop_up_error, null);

        final AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(context);
        alertDialogBuilder.setView(view);
        alertDialogBuilder.setCancelable(false);

        //Se genera la conexi??n con el boton del pop up
        TextView textViewErrorServidor = view.findViewById(R.id.textView_PopUp);
        Button btnAceptar= view.findViewById(R.id.button_PopUpSalir);
        btnAceptar.setText("Aceptar");
        textViewErrorServidor.setText(mensaje);

        DisplayMetrics displayMetrics = context.getResources().getDisplayMetrics();
        float dpHeight = displayMetrics.heightPixels;
        float dpWidth = displayMetrics.widthPixels ;


        //Se crea el evento click para el boton del pop up, el cual redirige al inicio de la aplicacion
        btnAceptar.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                alertDialogCorreoComercio.dismiss();
                Intent i = new Intent(context, pantallaInicialUsuarioComun.class);
                context.startActivity(i);
                activity.overridePendingTransition(R.anim.slide_in_left,R.anim.slide_out_right);
            }
        });

        //Se crea el correspondiente Dialog que se mostrara al usuario
        alertDialogCorreoComercio = alertDialogBuilder.create();
        //Se agrega esta linea para que no tenga fondo por defecto el dialog
        alertDialogCorreoComercio.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));

        //Se muestra el Dialogo al usuario con su correspondiente ubicacion en la pantalla
        alertDialogCorreoComercio.show();


        Log.d("heigh",String.valueOf(dpHeight));
        Log.d("width",String.valueOf(dpWidth*0.9));

        alertDialogCorreoComercio.getWindow().setLayout((int)(dpWidth*0.7), LinearLayout.LayoutParams.WRAP_CONTENT);//


    }

    /**Metodo de tipo void, el cual se encarga de crear un pop up para informar el resultado del correo y muestra el resultado del correo del cliente*/
    public void crearPopUpResultadoCorreoCLiente(String mensaje,int codigoTransaccion,String idTransaccion){


        Handler resp = new Handler(){
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what) {
                    case 1:
                        dialog.dismiss();
                        //Poner pop up resultado correo
                        if(respuestaCorreo.get("responseCode").equals("MQ==")){
                            crearPopUpResultadoCorreoComercio("Correo enviado exitosamente al comercio");
                        }else{
                            crearPopUpResultadoCorreoComercio("Hubo un error en el envio del correo al comercio");
                        }

                }}};
        //Se hace la respectiva conexi??n con el frontEnd del pop up
        LayoutInflater li = LayoutInflater.from(context);
        View view = li.inflate(R.layout.activity_pop_up_error, null);

        final AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(context);
        alertDialogBuilder.setView(view);
        alertDialogBuilder.setCancelable(false);

        //Se genera la conexi??n con el boton del pop up
        TextView textViewErrorServidor = view.findViewById(R.id.textView_PopUp);
        Button btnAceptar= view.findViewById(R.id.button_PopUpSalir);
        btnAceptar.setText("Aceptar");
        textViewErrorServidor.setText(mensaje);

        DisplayMetrics displayMetrics = context.getResources().getDisplayMetrics();
        float dpHeight = displayMetrics.heightPixels;
        float dpWidth = displayMetrics.widthPixels ;


        //Se crea el evento click para el boton del pop up, el cual redirige al inicio de la aplicacion
        btnAceptar.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                //Se crea el loader que se mostrara mientras se procesa la transaccion
                AlertDialog.Builder loader = new AlertDialog.Builder(context);
                LayoutInflater inflater = activity.getLayoutInflater();
                loader.setView(inflater.inflate(R.layout.loader_procesando_transaccion,null));
                loader.setCancelable(false);

                dialog = loader.create();
                dialog.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));

                Log.d("OPEN"," se abrio el loader");
                dialog.show();

                Servicios serv = new Servicios(context);
                CodificarBase64 base64 = new CodificarBase64();
                PreferencesUsuario prefsParam = new PreferencesUsuario(ConstantesCorresponsal.SHARED_PREFERENCES_PARAMETRICAS,context);
                solicitarEnvioCorreoComercio = new Thread(new Runnable() {
                    @Override
                    public void run() {
                        //solicitar envio correo
                        alertDialogCorreoCliente.dismiss();

                        respuestaCorreo = serv.envioCorreo(idTransaccion,base64.decodificarBase64(prefsParam.getCommerceMail()),codigoTransaccion);
                        try{
                            //Se envia un mensaje al handler de la clase consulta saldo, indicando que el usuario cancelo la transaccion
                            Message usuarioCancela = new Message();
                            usuarioCancela.what = 1;
                            resp.sendMessage(usuarioCancela);
                        }catch (Exception e){ }
                    }
                });

                solicitarEnvioCorreoComercio.start();
            }
        });




        //Se crea el correspondiente Dialog que se mostrara al usuario
        alertDialogCorreoCliente = alertDialogBuilder.create();
        //Se agrega esta linea para que no tenga fondo por defecto el dialog
        alertDialogCorreoCliente.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));

        //Se muestra el Dialogo al usuario con su correspondiente ubicacion en la pantalla
        alertDialogCorreoCliente.show();


        Log.d("heigh",String.valueOf(dpHeight));
        Log.d("width",String.valueOf(dpWidth*0.9));

        alertDialogCorreoCliente.getWindow().setLayout((int)(dpWidth*0.7), LinearLayout.LayoutParams.WRAP_CONTENT);//


    }
}