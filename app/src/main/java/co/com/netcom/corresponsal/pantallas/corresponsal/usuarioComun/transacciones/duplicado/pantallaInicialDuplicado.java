package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.duplicado;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.MenuItem;

import com.google.android.material.bottomnavigation.BottomNavigationView;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.inicio.pantallaInicialUsuarioComun;

public class pantallaInicialDuplicado extends AppCompatActivity {

    private Header header = new Header("<b>Duplicado<b/>");
    private BottomNavigationView menuDuplicado;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_inicial_duplicado);

        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderDuplicado,header).commit();

        //Se crea la conexion con la interfaz grafica

        menuDuplicado = (BottomNavigationView) findViewById(R.id.menuDuplicado);


        //Se crea el evento click de la barra de navegacion

        menuDuplicado.setOnNavigationItemSelectedListener(new BottomNavigationView.OnNavigationItemSelectedListener() {
            @Override
            public boolean onNavigationItemSelected(@NonNull MenuItem menuItem) {

                if(menuItem.getItemId() == R.id.home_general){
                    Intent i = new Intent(getApplicationContext(), pantallaInicialUsuarioComun.class);
                    startActivity(i);
                    overridePendingTransition(R.anim.slide_in_left,R.anim.slide_out_right);

                }

                return true;
            }
        });
    }

}
