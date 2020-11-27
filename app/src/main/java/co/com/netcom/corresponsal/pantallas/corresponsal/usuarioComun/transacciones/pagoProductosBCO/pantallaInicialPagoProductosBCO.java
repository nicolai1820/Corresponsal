package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.pagoProductosBCO;

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
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.pagoProductosBCO.cartera.pantallaCarteraNumeroPagare;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.pagoProductosBCO.tarjetaCredito.pantallaTarjetaCreditoProductosBCO;

public class pantallaInicialPagoProductosBCO extends AppCompatActivity {

    private Header header = new Header("<b>Pago productos banco.</b>");
    private BottomNavigationView menuPagosBCO;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_inicial_pago_productos_b_c_o);

        //Se crea el header de la actividad con el titulo correspondiente.

        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderPagosBCOInicial,header).commit();

        //Se crea la conexion con la interfaz grafica

        menuPagosBCO = (BottomNavigationView) findViewById(R.id.menuPagosBCO);


        //Se crea el evento click de la barra de navegacion

        menuPagosBCO.setOnNavigationItemSelectedListener(new BottomNavigationView.OnNavigationItemSelectedListener() {
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



    /**Metodo carteraPagoProductosBCO de tipo void, recibo como parametro un View, para poder ser implementado por
     * el button cartera. Hace el intent a la activity pantallaCarteraNumeroPagare*/

    public void CarteraPagoProductosBCO(View view){

        Intent i = new Intent(getApplicationContext(), pantallaCarteraNumeroPagare.class);
        startActivity(i);
        overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);

    }

    /**Metodo PagoProductosBCOTarjetaDeCredito de tipo void, recibo como parametro un View, para poder ser implementado por
     * el button cartera. Hace el intent a la activity pantallaTarjetaCreditoProductosBCO*/

    public void PagoProductosBCOTarjetaDeCredito(View view){

        Intent i = new Intent(getApplicationContext(), pantallaTarjetaCreditoProductosBCO.class);
        startActivity(i);
        overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);

    }

    /**Metodo nativo de android onBackPressed. Se sobreescribe para que el usuario no se pueda desplazar hacia atras
     * por medio del menu del celular.*/

    @Override
    public void onBackPressed() {

    }
}