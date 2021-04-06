package co.com.netcom.corresponsal.pantallas.comunes.correo;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;

public class PantallaCopiaCorreo extends AppCompatActivity {

    private Header header = new Header("<b>Resultado transacción</b>");


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_copia_correo);


        //Se carga el Header de la vista
        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderPantallaCopiaCorreo,header).commit();


    }
}