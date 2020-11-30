package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.pagoFacturas.manual.manualConvenio;

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

public class pantallaPagoFacturasManualConvenio extends AppCompatActivity {

    private Header header = new Header("<b>Código de convenio</b>");
    private EditText editText_PagoFacturasManualCodigoConvenio, editText_PagoFacturasManualNumeroReferencia;
    private String [] titulos={"Código de convenio","Número de referencia "};
    private ArrayList<String> valores = new ArrayList<String>();
    private CodigosTransacciones codigo = new CodigosTransacciones();
    private ArrayList<String> tipoDeCuenta = new ArrayList<String>();
    private String tipoCuenta_String = "00";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_pago_facturas_manual_convenio);

        //Se crea el header de la actividad con el titulo correspondiente.

        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderPagoFacturasManualConvenio,header).commit();

        //Se cre la conexión con los elementos de la interfaz gráfica

        editText_PagoFacturasManualCodigoConvenio = (EditText) findViewById(R.id.editText_PagoFacturasManualCodigoConvenio);

        editText_PagoFacturasManualNumeroReferencia = (EditText) findViewById(R.id.editText_PagoFacturasManualNumeroReferencia);

    }


    /**Metodo pagoFacturasManualConvenioContinuar de tipo void, recibo como parametro un View, para poder ser implementado por
     * el button button_pagoFacturasManualConvenioContinuar. Hace el intent a la activity pantalla confirmacion y pasa como
     * extra todos los parametros ingresador por el usuario para ser confirmados*/


    public void pagoFacturasManualConvenioContinuar(View view){
        String codigoConvenio_string = editText_PagoFacturasManualCodigoConvenio.getText().toString();
        String numeroReferencia_string = editText_PagoFacturasManualNumeroReferencia.getText().toString();

        if(codigoConvenio_string.isEmpty()){
            Toast.makeText(getApplicationContext(),"Debe ingresar el código de convenio.",Toast.LENGTH_SHORT).show();
        }else if(numeroReferencia_string.isEmpty()){
            Toast.makeText(getApplicationContext(),"Debe ingresar el número de referencia",Toast.LENGTH_SHORT).show();
        } else {

                valores.add(codigoConvenio_string);
                valores.add(numeroReferencia_string);
                tipoDeCuenta.add(tipoCuenta_String);

            //Se realiza el intent a la activity confirmar valores
            Intent i = new Intent(getApplicationContext(), pantallaConfirmacion.class);

            i.putExtra("titulo","<b>Código de convenio</b>");
            i.putExtra("descripcion","Por favor confirme los valores.");
            i.putExtra("titulos",titulos);
            i.putExtra("valores",valores);
            i.putExtra("terminos",false);
            i.putExtra("clase","");
            i.putExtra("contador", 0);
            i.putExtra("tipoDeCuenta",tipoDeCuenta);
            i.putExtra("transaccion", codigo.CORRESPONSAL_CONSULTA_FACTURAS);
            startActivity(i);
            overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);

        }


    }

    /**Metodo nativo de android onBackPressed. Se sobreescribe para que el usuario no se pueda desplazar hacia atras
     * por medio del menu del celular.*/

    @Override
    public void onBackPressed() {

    }

    /**Metodo nativo de android onRestart(). Se sobreescribe este metodo para eliminar los datos que el usuario
     * digito de forma erronea.*/
    @Override
    public void onRestart(){

        valores.clear();
        super.onRestart();
    }
}