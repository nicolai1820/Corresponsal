package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.giros.cancelar;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.Toast;

import java.util.ArrayList;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.comunes.pantallaConfirmacion.pantallaConfirmacion;
import co.com.netcom.corresponsal.pantallas.funciones.CodigosTransacciones;

public class pantallaCancelarGiroPinReferencia extends AppCompatActivity {

    private Header header = new Header("<b>Cancelar Giro</b>");
    private EditText editText_CancelarGiroPinReferencia;
    private String [] titulos={"Pin de Referencia"};
    private ArrayList<String> valores = new ArrayList<String>();
    private ArrayList<String> valoresRespaldo = new ArrayList<String>();
    private ArrayList<String> tipoDocumento = new ArrayList<String>();
    private int contador;
    private int contadorRespaldo;
    private CodigosTransacciones codigo = new CodigosTransacciones();


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_cancelar_giro_pin_referencia);

        //Se recuperan los valores de la vista anterior junto con el contador para recorrerlos
        Bundle extras = getIntent().getExtras();
        valores = extras.getStringArrayList("valores");
        contador = extras.getInt("contador");
        tipoDocumento = extras.getStringArrayList("tipoDocumento");


        //Se capturan los datos de respaldo
        for(int i =0; i< valores.size();i++){
            valoresRespaldo.add(valores.get(i));
        }
        contadorRespaldo = contador;

        //Se crea el header de la actividad con el correspondiente titulo
        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderCancelarGiroPinReferencia,header).commit();

        //Se crea la conexion con los elementos de la interfaz grafica
        editText_CancelarGiroPinReferencia = (EditText) findViewById(R.id.editText_CancelarGiroPinReferencia);

    }

    /**Metodo continuarPinReferenciaCancelarGiro de tipo void, recibo como parametro un View, para poder ser implementado por
     * el button ContinuarPinReferenciaCancelarGiro. Hace el intent a la activity pantalla confirmacion y pasa como
     * extra todos los parametros ingresador por el usuario*/

    public void continuarPinReferenciaCancelarGiro(View view){

        //Se obtiene los valores digitados por el usuario

        String numeroPinReferencia_string = editText_CancelarGiroPinReferencia.getText().toString();

        if (numeroPinReferencia_string.isEmpty()){
            Toast.makeText(getApplicationContext(),"Debe ingresar el número de documento",Toast.LENGTH_LONG).show();
        } else {

            valores.add(numeroPinReferencia_string);

            //Se realiza el intent a la activity confirmar valores
            Intent i = new Intent(getApplicationContext(), pantallaConfirmacion.class);

            i.putExtra("titulo","<b>Cancelar Giro</b>");
            i.putExtra("descripcion","Por favor confirme el pin de referencia");
            i.putExtra("titulos",titulos);
            i.putExtra("valores",valores);
            i.putExtra("terminos",false);
            i.putExtra("clase","");
            i.putExtra("contador", contador);
            i.putExtra("tipoDocumento", tipoDocumento);
            i.putExtra("transaccion", codigo.CORRESPONSAL_CANCELACION_GIRO_CONSULTA);
            startActivity(i);
            overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);

        }

    }

    /**Metodo nativo de android onBackPressed. Se sobreescribe para que el usuario no se pueda desplazar hacia atras
     * por medio del menu del celular.*/

    @Override
    public void onBackPressed() {

    }

    /**Metodo nativo de android onRestart. Se sobreescribe para que se eliminen los datos erroneos ingresados por el
     * usuario.*/

    @Override
    public void onRestart(){

        valores.clear();
        contador =0;

        for(int i =0; i< valoresRespaldo.size();i++){
            this.valores.add(valoresRespaldo.get(i));
        }

        this.contador =contadorRespaldo;

        super.onRestart();
    }


}