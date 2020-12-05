package co.com.netcom.corresponsal.pantallas.comunes.popUp;

import androidx.appcompat.app.AppCompatActivity;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.Intent;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.inicio.pantallaInicialUsuarioComun;
import co.com.netcom.corresponsal.pantallas.funciones.CodigosTransacciones;
import co.com.netcom.corresponsal.pantallas.funciones.Hilos;

public class PopUp extends Activity {


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pop_up);

    }




    public void salirPopUp(View view){

        Intent i = new Intent(getApplicationContext(), pantallaInicialUsuarioComun.class);
        startActivity(i);
        overridePendingTransition(R.anim.slide_in_left,R.anim.slide_out_right);

    }

    public void continuarPopUp(View view){

        finish();
    }

    public void pop(){
        AlertDialog.Builder builder = new AlertDialog.Builder(getApplicationContext());

        View view = LayoutInflater.from(getApplicationContext()).inflate( R.layout.pop_up,null);

        builder.setView(view);


        final AlertDialog alertDialog = builder.create();

        alertDialog.show();
    }



}