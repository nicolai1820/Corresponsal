package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.retiro;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.MenuItem;
import android.view.View;

import com.google.android.material.bottomnavigation.BottomNavigationView;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.inicio.pantallaInicialUsuarioComun;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.retiro.conTarjeta.PantallaRetiroConTarjetaCantidad;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.retiro.sinTarjeta.pantallaRetiroSinTarjetaCantidad;
import co.com.netcom.corresponsal.pantallas.funciones.BaseActivity;

public class PantallaInicialRetiro extends BaseActivity {

    private Header header = new Header("<b>Retiro</b>");
    private BottomNavigationView menuRetiro;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_inicial_retiro);

        //Se crea el respectivo header de la actividad con el titulo correspondiente

        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderPantallaInicialRetiro,header).commit();

        //Se crea la conexion con la interfaz grafica

        menuRetiro = (BottomNavigationView) findViewById(R.id.menuRetiro);


        //Se crea el evento click de la barra de navegacion

        menuRetiro.setOnNavigationItemSelectedListener(new BottomNavigationView.OnNavigationItemSelectedListener() {
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

    /**Metodo pantallaInicialRetiroSinTarjeta de tipo void, recibo como parametro un View, para poder ser implementado por
     * el button_pantallaInicialRetiroSinTarjeta. Hace el intent a la activity pantallaRetiroSinTarjeta*/

    public void pantallaInicialRetiroSinTarjeta(View view){
        Intent i = new Intent(getApplicationContext(), pantallaRetiroSinTarjetaCantidad.class);
        startActivity(i);
        overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);

    }

    /**Metodo pantallaInicialRetiroConTarjeta de tipo void, recibo como parametro un View, para poder ser implementado por
     * el button_pantallaInicialRetiroConTarjeta. Hace el intent a la activity pantallaRetiroConTarjeta*/

    public void pantallaInicialRetiroConTarjeta(View view){
        Intent i = new Intent(getApplicationContext(), PantallaRetiroConTarjetaCantidad.class);
        startActivity(i);
        overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);

    }


    /**Metodo nativo de android onBackPressed. Se sobreescribe para que el usuario no se pueda desplazar hacia atras
     * por medio del menu del celular.*/

    @Override
    public void onBackPressed() {

    }
}