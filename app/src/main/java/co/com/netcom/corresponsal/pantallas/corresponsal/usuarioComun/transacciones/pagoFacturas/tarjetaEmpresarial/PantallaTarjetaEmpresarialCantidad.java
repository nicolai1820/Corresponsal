package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.pagoFacturas.tarjetaEmpresarial;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

import java.util.ArrayList;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.core.comunicacion.CardDTO;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.comunes.pantallaConfirmacion.pantallaConfirmacion;
import co.com.netcom.corresponsal.pantallas.funciones.BaseActivity;
import co.com.netcom.corresponsal.pantallas.funciones.CodigosTransacciones;

public class PantallaTarjetaEmpresarialCantidad extends BaseActivity {

    private Header header;
    private CardDTO tarjeta;
    private ArrayList<String> valores = new ArrayList<String>();
    private int contador;
    private String [] titulos={"Cantidad"};
    private ArrayList<Integer> iconos = new ArrayList<Integer>();
    private EditText editText_TarjetaEmpresarialCantidad;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_tarjeta_empresarial_cantidad);

        //Se hace la conexión con la interfaz gráfica
        editText_TarjetaEmpresarialCantidad = findViewById(R.id.editText_TarjetaEmpresarialCantidad);


        //Se inicializa el fragmento del titulo
        header = new Header("<b>Pago de facturas empresarial.</b>");

        //Se reemplaza el fragmento del titulo en la vista
        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderPantallaTarjetaEmpresarialCantidad,header).commit();

        //Se rescatan los extras que vienen de la pantalla anterior
        Bundle extras = getIntent().getExtras();

        tarjeta = new CardDTO();
        tarjeta = extras.getParcelable("tarjeta");

    }


    /**Metodo de tipo void que recibe como parametro un View para poder ser implementado por la interfaz grafica,
     * se encarga de redirigir a la pantalla de confirmación con los respectivos valores ingresados por el usuario*/

    public void pagoFacturaEmpresarialContinuar(View view){

        String cantidad = editText_TarjetaEmpresarialCantidad.getText().toString();

        if(cantidad.isEmpty()){
            Toast.makeText(this,"Debe ingresar la cantidad a pagar",Toast.LENGTH_SHORT).show();
        } else {

            valores.add(cantidad);

            iconos.add(1);
            Log.d("VALORES",valores.toString());
            Log.d("TARJETA",tarjeta.toString());

            //Se realiza el intent a la activity confirmar valores
            Intent i = new Intent(getApplicationContext(), pantallaConfirmacion.class);

            i.putExtra("titulo","<b>Pago de facturas empresarial.</b>");
            i.putExtra("descripcion","Por favor confirme los valores");
            i.putExtra("titulos",titulos);
            i.putExtra("valores",valores);
            i.putExtra("terminos",false);
            i.putExtra("clase","");
            i.putExtra("contador", contador);
            i.putExtra("tarjeta",tarjeta);
            i.putExtra("iconos",iconos);
            i.putExtra("transaccion", CodigosTransacciones.CORRESPONSAL_PAGO_FACTURA_TARJETA_EMPRESARIAL);
            startActivity(i);
            overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);

        }

    }

    /**Metodo nativo de android onBackPressed. Se sobreescribe para que el usuario no se pueda desplazar hacia atras
     * por medio del menu del celular.*/

    @Override
    public void onBackPressed() {

    }

    /**Metodo nativo de android onRestart.Se sobreescribe para que se elimen los datos erroneos digitados por el usuario*/

    @Override
    protected void onRestart() {

        valores.clear();

        super.onRestart();
    }
}