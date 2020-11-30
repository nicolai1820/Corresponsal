package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.recargas;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

import java.util.ArrayList;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.core.comunicacion.DatosComision;
import co.com.netcom.corresponsal.core.comunicacion.DatosRecaudo;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.comunes.pantallaConfirmacion.pantallaConfirmacion;
import co.com.netcom.corresponsal.pantallas.funciones.CodigosTransacciones;

public class pantallaRecargaCantidad extends AppCompatActivity {

    private Header header;
    private EditText cantidad;
    private String [] titulos={"Número de Celular","Cantidad"};
    private ArrayList<String> valores = new ArrayList<String>();
    private CodigosTransacciones codigo = new CodigosTransacciones();
    private DatosRecaudo datosRecaudo;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_recarga_cantidad);

        header= new Header("<b>Recargas</b>");

        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderPantallaRecargaCantidad,header).commit();

        cantidad = (EditText) findViewById(R.id.editText_RecargasCantidad);

        Bundle i = getIntent().getExtras();
        valores = i.getStringArrayList("valores");
        datosRecaudo = i.getParcelable("datosRecuado");


    }

    public void recargasCantidadVamos(View view){
        String cantidad_String = cantidad.getText().toString();

        if (cantidad_String.isEmpty()) {
            Toast.makeText(this,"Cantidad Invalida",Toast.LENGTH_SHORT).show();
        }else{

            valores.add(cantidad_String);
            datosRecaudo.setValorApagar(cantidad_String);

            //Se realiza el intent a la activity confirmar valores
            Intent i = new Intent(getApplicationContext(), pantallaConfirmacion.class);

            i.putExtra("titulo","<b>Recargas</b>");
            i.putExtra("descripcion","Por favor la cantidad.");
            i.putExtra("titulos",titulos);
            i.putExtra("valores",valores);
            i.putExtra("terminos",false);
            i.putExtra("clase","");
            i.putExtra("contador", 2);
            i.putExtra("datosRecaudo",datosRecaudo);
            i.putExtra("transaccion", codigo.CORRESPONSAL_PAGO_FACTURA_MANUAL);
            startActivity(i);
            overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);

        }

    }

    @Override
    public void onBackPressed() {

    }
}