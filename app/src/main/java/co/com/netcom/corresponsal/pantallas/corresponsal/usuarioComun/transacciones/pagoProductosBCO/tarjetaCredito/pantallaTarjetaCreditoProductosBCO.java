package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.pagoProductosBCO.tarjetaCredito;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

import java.util.ArrayList;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.comunes.pantallaConfirmacion.pantallaConfirmacion;
import co.com.netcom.corresponsal.pantallas.funciones.CodigosTransacciones;

public class pantallaTarjetaCreditoProductosBCO extends AppCompatActivity {

    private Header header = new Header("<b>Pago productos banco.</b>");
    private EditText editText_TarjetaDeCreditoPagoProductosBCONumero, editText_TarjetaDeCreditoPagoProductosBCOCantidad;
    private String [] titulos={"Número de tarjeta","Cantidad a pagar"};
    private ArrayList<String> valores = new ArrayList<String>();
    private CodigosTransacciones codigo = new CodigosTransacciones();


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_tarjeta_credito_productos_b_c_o);

        //Se crea el header con el correspondiente titulo

        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderTarjetaCreditoProductosBCO,header).commit();

        //se realiz ala respectiva conexion con la interfaz grafica
        editText_TarjetaDeCreditoPagoProductosBCONumero = (EditText) findViewById(R.id.editText_TarjetaDeCreditoPagoProductosBCONumero);
        editText_TarjetaDeCreditoPagoProductosBCOCantidad = (EditText) findViewById(R.id.editText_TarjetaDeCreditoPagoProductosBCOCantidad);

    }

    /**Metodo TarjetaDeCreditoPagoProductosBCOContinuar de tipo void, recibo como parametro un View, para poder ser implementado por
     * el button button_TarjetaDeCreditoPagoProductosBCOContinuar. Hace el intent a la activity pantalla confirmacion y pasa como
     * extra todos los parametros ingresador por el usuario para ser confirmados*/

    public void TarjetaDeCreditoPagoProductosBCOContinuar(View view){

        //Se capturan los datos ingresador por el usuario

        String numeroTarjeta_string = editText_TarjetaDeCreditoPagoProductosBCONumero.getText().toString();
        String cantidad_string = editText_TarjetaDeCreditoPagoProductosBCOCantidad.getText().toString();

        if(numeroTarjeta_string.isEmpty()){
            Toast.makeText(getApplicationContext(),"Debe ingresar el número de cuenta",Toast.LENGTH_SHORT).show();
        } else if(cantidad_string.isEmpty()){
            Toast.makeText(getApplicationContext(),"Debe ingresar la cantidad",Toast.LENGTH_SHORT).show();
        } else {

            valores.add(numeroTarjeta_string);
            valores.add(cantidad_string);

            //Se realiza el intent a la activity confirmar valores
            Intent i = new Intent(getApplicationContext(), pantallaConfirmacion.class);

            i.putExtra("titulo","<b>Pago productos banco</b>");
            i.putExtra("descripcion","Por favor confirme los valores del depositante");
            i.putExtra("titulos",titulos);
            i.putExtra("valores",valores);
            i.putExtra("terminos",false);
            i.putExtra("clase","");
            i.putExtra("contador", 0);
            i.putExtra("transaccion", codigo.CORRESPONSAL_PAGO_PRODUCTOS);
            startActivity(i);
            overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);

        }


    }

    /**Metodo nativo de android onBackPressed. Se sobreescribe para que el usuario no se pueda desplazar hacia atras
     * por medio del menu del celular.*/

    @Override
    public void onBackPressed() {

    }

    /**Metodo nativo de android onRestart. Se sobreescribe para que se eliminen los datos erroneos ingresados por
     * el usuario.*/

    @Override
    public void onRestart(){

        valores.clear();
        super.onRestart();
    }
}