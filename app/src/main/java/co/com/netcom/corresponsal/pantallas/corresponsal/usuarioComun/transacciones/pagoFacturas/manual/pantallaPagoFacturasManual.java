package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.pagoFacturas.manual;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.pagoFacturas.manual.manualConvenio.pantallaPagoFacturasManualConvenio;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.pagoFacturas.manual.manualCuentaRecaudadora.pantallaPagoFacturasManualCuentaRecaudadora;

public class pantallaPagoFacturasManual extends AppCompatActivity {

    private Header header = new Header("<b>Pago de facturas</b>");

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_pago_facturas_manual);

        //Se crea el header de la actividad con el titulo correspondiente.

        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderPagoFacturasManual,header).commit();

    }

    /**Metodo pagoFactuasManualConvenio de tipo void, recibo como parametro un View, para poder ser implementado por
     * el button button_pagoFactuasManualConvenio. Hace el intent a la activity pantallaPagoFactuasManualConvenio*/

    public void pagoFactuasManualConvenio(View view){
        Intent i = new Intent(getApplicationContext(), pantallaPagoFacturasManualConvenio.class);
        startActivity(i);
        overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);
    }

    /**Metodo pagoFactuasManualRecaudadora de tipo void, recibo como parametro un View, para poder ser implementado por
     * el button button_pagoFactuasManualRecaudadora. Hace el intent a la activity pantallaPagoFactuasManualRecaudadora*/

    public void pagoFactuasManualRecaudadora(View view){
        Intent i = new Intent(getApplicationContext(), pantallaPagoFacturasManualCuentaRecaudadora.class);
        startActivity(i);
        overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);
    }

    /**Metodo nativo de android onBackPressed. Se sobreescribe para que el usuario no se pueda desplazar hacia atras
     * por medio del menu del celular.*/

    @Override
    public void onBackPressed() {

    }
}