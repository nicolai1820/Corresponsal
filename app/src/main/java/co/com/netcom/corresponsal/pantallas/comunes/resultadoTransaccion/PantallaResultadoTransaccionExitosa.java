package co.com.netcom.corresponsal.pantallas.comunes.resultadoTransaccion;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.comunes.correo.PantallaCopiaCorreo;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.comunes.popUp.PopUp;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.inicio.pantallaInicialUsuarioComun;

public class PantallaResultadoTransaccionExitosa extends AppCompatActivity {

    private Header header = new Header("<b>Resultado transacción</b>");
    private TextView textViewNumeroAprovacion;
    String idTransaccion;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_resultado_transaccion_exitosa);

        //Recuperar información transaccion : ID de la transacción
        Bundle i = getIntent().getExtras();
        idTransaccion = i.getString("aprovalCode");

        textViewNumeroAprovacion = findViewById(R.id.textView_referenciaTransaccion);

        textViewNumeroAprovacion.setText(idTransaccion);


        //Se carga el Header de la vista
        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderPantallaResultadoTransaccionExitosa,header).commit();
    }

    public void buttonSalirResultadoTransaccionExitosa(View view){
        PopUp pop = new PopUp(PantallaResultadoTransaccionExitosa.this);
        pop.crearPopUpConfirmarEnvioCorreo(idTransaccion);
    }

    public void buttonEnviarCopiaCorreo(View view){
        //Hacer el intent a la pantalla de enviar correo y agregar el id de la transacción
        Intent i = new Intent(getApplicationContext(), PantallaCopiaCorreo.class);
        i.putExtra("aprovalCode",idTransaccion);
        startActivity(i);
        overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);
    }

    @Override
    public void onBackPressed() {
    }
}