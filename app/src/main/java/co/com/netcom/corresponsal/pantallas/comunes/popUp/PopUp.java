package co.com.netcom.corresponsal.pantallas.comunes.popUp;

import androidx.appcompat.app.AppCompatActivity;

import android.app.AlertDialog;
import android.content.Context;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.funciones.CodigosTransacciones;
import co.com.netcom.corresponsal.pantallas.funciones.Hilos;

public class PopUp extends AppCompatActivity {

    private Hilos hilo;
    private CodigosTransacciones codigo;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pop_up);

       // hilo = new Hilos();
        codigo = new CodigosTransacciones();
    }


        //El contexto se debe pasar como Clase.this, para que no genere errores.

   /* public void PopUp(Context contexto, String tituloContenido,String descripcion, String[] titulos, String[]datos,int transaccion){

        if(transaccion == codigo.CORRESPONSAL_RETIRO_CON_TARJETA){
            //se recorren los datos especificos que digito el usuario, exceptuando el número de tarjeta, para presentarlos en el popup
            //Agregar a text view los elementos a confirmar
            try {
                Log.d("entradaContexto", contexto.toString());

                AlertDialog.Builder builder = new AlertDialog.Builder(contexto);

                View view = LayoutInflater.from(contexto).inflate(R.layout.pop_up, null);

                builder.setView(view);
                ((TextView) view.findViewById(R.id.TituloPopUp)).setText(tituloContenido);
                ((Button) view.findViewById(R.id.ButtonAceptar)).setText("Aceptar");
                ((Button) view.findViewById(R.id.ButtonCancelar)).setText("Cancelar");

                final AlertDialog alertDialog = builder.create();

                view.findViewById(R.id.ButtonAceptar).setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        hilo.transacciones(transaccion, datos);
                    }
                });

                view.findViewById(R.id.ButtonCancelar).setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        alertDialog.dismiss();
                    }
                });

                if (alertDialog.getWindow() != null) {

                    alertDialog.getWindow().setBackgroundDrawable(new ColorDrawable(0));
                }
                alertDialog.show();


            } catch (Exception e) {
                Log.d("Errorpop", e.toString());
            }
        }

        else if(transaccion == codigo.CORRESPONSAL_CONSULTA_SALDO){
            //se recorren los datos especificos que digito el usuario, exceptuando el número de tarjeta, para presentarlos en el popup
            //Agregar a text view los elementos a confirmar
            try {
                Log.d("entradaContexto", contexto.toString());

                AlertDialog.Builder builder = new AlertDialog.Builder(contexto);

                View view = LayoutInflater.from(contexto).inflate(R.layout.pop_up, null);

                builder.setView(view);
                ((TextView) view.findViewById(R.id.TituloPopUp)).setText(tituloContenido);
                ((Button) view.findViewById(R.id.ButtonAceptar)).setText("Aceptar");
                ((Button) view.findViewById(R.id.ButtonCancelar)).setText("Cancelar");

                final AlertDialog alertDialog = builder.create();

                view.findViewById(R.id.ButtonAceptar).setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        hilo.transacciones(transaccion, datos);
                    }
                });

                view.findViewById(R.id.ButtonCancelar).setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        alertDialog.dismiss();
                    }
                });

                if (alertDialog.getWindow() != null) {

                    alertDialog.getWindow().setBackgroundDrawable(new ColorDrawable(0));
                }
                alertDialog.show();


            } catch (Exception e) {
                Log.d("Errorpop", e.toString());
            }
        }

        else if(transaccion == codigo.CORRESPONSAL_TRANSFERENCIA){
            //se recorren los datos especificos que digito el usuario, exceptuando el número de tarjeta, para presentarlos en el popup
            //Agregar a text view los elementos a confirmar
            try {
                Log.d("entradaContexto", contexto.toString());

                AlertDialog.Builder builder = new AlertDialog.Builder(contexto);

                View view = LayoutInflater.from(contexto).inflate(R.layout.pop_up, null);

                builder.setView(view);
                ((TextView) view.findViewById(R.id.TituloPopUp)).setText(tituloContenido);
                ((Button) view.findViewById(R.id.ButtonAceptar)).setText("Aceptar");
                ((Button) view.findViewById(R.id.ButtonCancelar)).setText("Cancelar");

                final AlertDialog alertDialog = builder.create();

                view.findViewById(R.id.ButtonAceptar).setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        hilo.transacciones(transaccion, datos);
                    }
                });

                view.findViewById(R.id.ButtonCancelar).setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        alertDialog.dismiss();
                    }
                });

                if (alertDialog.getWindow() != null) {

                    alertDialog.getWindow().setBackgroundDrawable(new ColorDrawable(0));
                }
                alertDialog.show();


            } catch (Exception e) {
                Log.d("Errorpop", e.toString());
            }
        }

        else if(transaccion == codigo.CORRESPONSAL_CONSULTA_SALDO){
            //se recorren los datos especificos que digito el usuario, exceptuando el número de tarjeta, para presentarlos en el popup
            //Agregar a text view los elementos a confirmar
            try {
                Log.d("entradaContexto", contexto.toString());

                AlertDialog.Builder builder = new AlertDialog.Builder(contexto);

                View view = LayoutInflater.from(contexto).inflate(R.layout.pop_up, null);

                builder.setView(view);
                ((TextView) view.findViewById(R.id.TituloPopUp)).setText(tituloContenido);
                ((Button) view.findViewById(R.id.ButtonAceptar)).setText("Aceptar");
                ((Button) view.findViewById(R.id.ButtonCancelar)).setText("Cancelar");

                final AlertDialog alertDialog = builder.create();

                view.findViewById(R.id.ButtonAceptar).setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        hilo.transacciones(transaccion, datos);
                    }
                });

                view.findViewById(R.id.ButtonCancelar).setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        alertDialog.dismiss();
                    }
                });

                if (alertDialog.getWindow() != null) {

                    alertDialog.getWindow().setBackgroundDrawable(new ColorDrawable(0));
                }
                alertDialog.show();


            } catch (Exception e) {
                Log.d("Errorpop", e.toString());
            }
        }

        else {
            try {
                Log.d("entradaContexto", contexto.toString());

                AlertDialog.Builder builder = new AlertDialog.Builder(contexto);

                View view = LayoutInflater.from(contexto).inflate(R.layout.pop_up, null);

                builder.setView(view);
                ((TextView) view.findViewById(R.id.TituloPopUp)).setText(tituloContenido);
                ((Button) view.findViewById(R.id.ButtonAceptar)).setText("Aceptar");
                ((Button) view.findViewById(R.id.ButtonCancelar)).setText("Cancelar");

                final AlertDialog alertDialog = builder.create();

                view.findViewById(R.id.ButtonAceptar).setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {

                        //hilo.transacciones(transaccion, datos);
                    }
                });

                view.findViewById(R.id.ButtonCancelar).setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {

                        alertDialog.dismiss();
                    }
                });

                if (alertDialog.getWindow() != null) {

                    alertDialog.getWindow().setBackgroundDrawable(new ColorDrawable(0));
                }
                alertDialog.show();


            } catch (Exception e) {
                Log.d("Errorpop", e.toString());
            }
        }
    }*/
}