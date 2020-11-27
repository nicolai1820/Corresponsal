package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.pagoFacturas;

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
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.pagoFacturas.manual.pantallaPagoFacturasManual;

public class pantallaInicialPagoFacturas extends AppCompatActivity {

    private Header header = new Header("<b>Pago de facturas</b>");
    private BottomNavigationView menuFacturas;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_inicial_pago_facturas);

        //Se crea el header de la actividad con el titulo correspondiente.

        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderPantallaInicialFacturas,header).commit();

        //Se crea la conexion con la interfaz grafica

        menuFacturas = (BottomNavigationView) findViewById(R.id.menuFacturas);


        //Se crea el evento click de la barra de navegacion

        menuFacturas.setOnNavigationItemSelectedListener(new BottomNavigationView.OnNavigationItemSelectedListener() {
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

    /**Metodo pagoManualFacturas de tipo void, recibo como parametro un View, para poder ser implementado por
     * el button cartera. Hace el intent a la activity pantallaCarteraNumeroPagare*/

    public void pagoManualFacturas(View view){
            Intent i = new Intent(getApplicationContext(), pantallaPagoFacturasManual.class);
            startActivity(i);
            overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);
    }

    /**Metodo pagoFacturaEmpresarial de tipo void, recibo como parametro un View, para poder ser implementado por
     * el button cartera. Hace el intent a la activity pantallaCarteraNumeroPagare*/

    public void pagoFacturaEmpresarial(View view){

    }

    /**Metodo pagoFacturasCOdigoBarras de tipo void, recibo como parametro un View, para poder ser implementado por
     * el button cartera. Hace el intent a la activity pantallaCarteraNumeroPagare*/

    public void pagoFacturasCodigoBarras(View view){

    }

    /**Metodo nativo de android onBackPressed. Se sobreescribe para que el usuario no se pueda desplazar hacia atras
     * por medio del menu del celular.*/

    @Override
    public void onBackPressed() {

    }
}