package co.com.netcom.corresponsal.pantallas.comunes.resultadoTransaccion;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.comunes.logIn.LogIn;

public class PantallaResultadoTransaccionLoaderFallida extends AppCompatActivity {
    private Intent a;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_resultado_transaccion_loader_fallida);


        //Recuperar información transaccion : ID de la transacción
        Bundle i = getIntent().getExtras();
        String responseMessage = i.getString("responseMessage");


        //Se hace la transición a la siguiente pantalla luego de mostrar el loader
        a= new Intent(this, PantallaResultadoTransaccionFallida.class);
        a.putExtra("responseMessage",responseMessage);
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