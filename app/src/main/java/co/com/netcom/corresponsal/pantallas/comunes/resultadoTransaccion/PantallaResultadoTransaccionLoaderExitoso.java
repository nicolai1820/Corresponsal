package co.com.netcom.corresponsal.pantallas.comunes.resultadoTransaccion;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.comunes.logIn.LogIn;

public class PantallaResultadoTransaccionLoaderExitoso extends AppCompatActivity {
    private Intent a;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_resultado_transaccion_loader);

        //Recuperar información transaccion : ID de la transacción
        Bundle i = getIntent().getExtras();
        String idTransaccion = i.getString("aprovalCode");


        //Se hace la transición a la siguiente pantalla luego de mostrar el loader
        a= new Intent(this, PantallaResultadoTransaccionExitosa.class);
        a.putExtra("aprovalCode",idTransaccion);

        new Handler().postDelayed(
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        startActivity(a);
                        overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);
                    }
                }),3500
        );
    }

    @Override
    public void onBackPressed() {

    }
}