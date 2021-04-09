package co.com.netcom.corresponsal.pantallas.comunes.resultadoTransaccion;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.inicio.pantallaInicialUsuarioComun;
import co.com.netcom.corresponsal.pantallas.funciones.CodificarBase64;

public class PantallaResultadoTransaccionFallida extends AppCompatActivity {

    private Header header = new Header("<b>Resultado transacción</b>");
    private TextView textViewTxFallida;
    private CodificarBase64 base64 = new CodificarBase64();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_resultado_transaccion_fallida);

        //Crear conexión con la parte visual de la aplicación
        textViewTxFallida = findViewById(R.id.textView_PantallaTransaccionFallida);

        //Recuperar información transaccion : ID de la transacción
        Bundle i = getIntent().getExtras();
        String responseMessage = i.getString("responseMessage");

        if(!responseMessage.isEmpty()){
            textViewTxFallida.setText(base64.decodificarBase64(responseMessage));
        }
        //Se carga el Header de la vista
        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderPantallaTransaccionFallida,header).commit();
    }


    public void salirTransaccionFallida(View v){

        Intent i = new Intent(getApplicationContext(), pantallaInicialUsuarioComun.class);
        i.putExtra("Fragmento",1);
        startActivity(i);
        overridePendingTransition(R.anim.slide_in_left,R.anim.slide_out_right);
    }

    @Override
    public void onBackPressed() {
    }
}