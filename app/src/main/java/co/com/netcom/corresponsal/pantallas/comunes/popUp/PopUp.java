package co.com.netcom.corresponsal.pantallas.comunes.popUp;


import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.os.Message;
import android.text.InputFilter;
import android.text.Spanned;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.consultaSaldo.pantallaConsultaSaldoLectura;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.inicio.pantallaInicialUsuarioComun;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.pagoFacturas.tarjetaEmpresarial.PantallaTarjetaEmpresarialLectura;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.retiro.conTarjeta.PantallaRetiroConTarjetaLoader;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.retiro.sinTarjeta.pantallaRetiroSinTarjetaPin;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.transferencia.pantallaTransferenciaLectura;


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
    private AlertDialog alertDialogRecuperarContrasena;

    /**Constructor de la clase PopUpDesconexion, recibo como parametro el contexto de la actividad donde se inicializa*/
    public PopUp(Context contexto){
        this.context = contexto;
        this.activity = (Activity) contexto;
    }


    /**Metodo de tipo void, el cual se encarga de crear un pop up para informar que el dispositivo MPOS se encuentra desconectado. */
    public void crearPopUp(){

        //Se hace la respectiva conexión con el frontEnd del pop up
        LayoutInflater li = LayoutInflater.from(context);
        View view = li.inflate(R.layout.activity_pop_up_error, null);
        final AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(context);
        alertDialogBuilder.setView(view);
        alertDialogBuilder.setCancelable(false);

        //Se genera la conexión con el boton del pop up
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
        alertDialogDesconexion.getWindow().setLayout(LinearLayout.LayoutParams.WRAP_CONTENT, LinearLayout.LayoutParams.WRAP_CONTENT);//

    }


    /**Metodo de tipo void que se encarga de crear un pop up con el correspondiente mensaje de error y permite intentar nuevamente la operación */

    public void crearPopUpError(String Error, int codigo){

        //Se hace la respectiva conexión con el frontEnd del pop up
        LayoutInflater li = LayoutInflater.from(context);
        View view = li.inflate(R.layout.activity_pop_up_error_mpos, null);
        final AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(context);
        alertDialogBuilder.setView(view);
        alertDialogBuilder.setCancelable(false);


        //Se genera la conexión con diversos elementos  del pop up
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


        //Se crea el evento click para el boton del pop up, el cual permite intentar nuevamente la operación, dependiendo de la pantalla.
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
        alertDialog.getWindow().setLayout(600, LinearLayout.LayoutParams.WRAP_CONTENT);//

    }


    /**Metodo de tipo void, el cual se encarga de crear un pop up para informar que la transaccion la cancelo el usuario */
    public void crearPopUpCanceladaUsuario(){

        //Se hace la respectiva conexión con el frontEnd del pop up
        LayoutInflater li = LayoutInflater.from(context);
        View view = li.inflate(R.layout.activity_pop_up_error, null);
        final AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(context);
        alertDialogBuilder.setView(view);
        alertDialogBuilder.setCancelable(false);

        //Se genera la conexión con el boton del pop up
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
        alertDialogUsuarioCancela.getWindow().setLayout(800, LinearLayout.LayoutParams.WRAP_CONTENT);//

    }





    /**Metodo de tipo void, el cual se encarga de crear un pop up para informar que el login fallo */
    public void crearPopUpLoginFallido(String mensaje){

        //Se hace la respectiva conexión con el frontEnd del pop up
        LayoutInflater li = LayoutInflater.from(context);
        View view = li.inflate(R.layout.activity_pop_up_error, null);
        final AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(context);
        alertDialogBuilder.setView(view);
        alertDialogBuilder.setCancelable(false);

        //Se genera la conexión con el boton del pop up
        TextView textViewErrorServidor = view.findViewById(R.id.textView_PopUp);
        Button btnAceptar= view.findViewById(R.id.button_PopUpSalir);
        btnAceptar.setText("Aceptar");
        textViewErrorServidor.setText(mensaje);



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
        alertDialogLoginFallido.getWindow().setLayout(LinearLayout.LayoutParams.WRAP_CONTENT, LinearLayout.LayoutParams.WRAP_CONTENT);//

    }

    /**Metodo de tipo void, el cual se encarga de crear un pop up para recuperar la contraseña */
    public void crearPopUpRecuperarContrasena(){

        //Se hace la respectiva conexión con el frontEnd del pop up
        LayoutInflater li = LayoutInflater.from(context);
        View view = li.inflate(R.layout.activity_pop_up_recuperar_contrasena, null);
        final AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(context);
        alertDialogBuilder.setView(view);
        alertDialogBuilder.setCancelable(false);


        //Se genera la conexión con el boton del pop up
        EditText usuario = view.findViewById(R.id.editText_PopUpRecuperarContrasenaUsuario);
        Button btnAceptar= view.findViewById(R.id.button_PopUpRecuperarContrasenaAceptar);
        Button btnCancelar= view.findViewById(R.id.button_PopUpRecuperarContrasenaCancelar);

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

                if (usuario_string.isEmpty()){
                    Toast.makeText(PopUp.this, "Debe ingresar un usuario", Toast.LENGTH_SHORT).show();
                }else{


                    alertDialogRecuperarContrasena.dismiss();

                }

            }
        });

        //Se crea el evento click para el boton del pop up, el cual redirige al inicio de la aplicacion
        btnCancelar.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                alertDialogRecuperarContrasena.dismiss();
            }
        });

        //Se crea el correspondiente Dialog que se mostrara al usuario
        alertDialogRecuperarContrasena = alertDialogBuilder.create();
        //Se agrega esta linea para que no tenga fondo por defecto el dialog
        alertDialogRecuperarContrasena.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));

        //Se muestra el Dialogo al usuario con su correspondiente ubicacion en la pantalla
        alertDialogRecuperarContrasena.show();
        alertDialogRecuperarContrasena.getWindow().setLayout(LinearLayout.LayoutParams.WRAP_CONTENT,LinearLayout.LayoutParams.WRAP_CONTENT);


    }

    /**Metodo de tipo void, el cual se encarga de crear un pop up para informar que el login fallo */
    public void crearPopUpContactless(String mensaje){

        //Se hace la respectiva conexión con el frontEnd del pop up
        LayoutInflater li = LayoutInflater.from(context);
        View view = li.inflate(R.layout.activity_pop_up_error, null);
        final AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(context);
        alertDialogBuilder.setView(view);
        alertDialogBuilder.setCancelable(false);

        //Se genera la conexión con el boton del pop up
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
        alertDialogLoginFallido.getWindow().setLayout(LinearLayout.LayoutParams.WRAP_CONTENT, LinearLayout.LayoutParams.WRAP_CONTENT);//

    }
}