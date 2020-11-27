package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.giros.reclamar;

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

public class pantallaReclamarGiroNumeroReferencia extends AppCompatActivity {

    private Header header = new Header("<b>Reclamar Giro</b>");
    private EditText editText_NumeroReferenciaReclamarGiro;
    private ArrayList<String> valores = new ArrayList<String>();
    private ArrayList<String> valoresRespaldo = new ArrayList<String>();
    private ArrayList<String> tipoDocumento = new ArrayList<String>();
    private ArrayList<String> tipoDocumentoRespaldo = new ArrayList<String>();
    private String [] titulos={"Número de referencia"};
    private int contador;
    private int contadorRespaldo;
    private CodigosTransacciones codigo = new CodigosTransacciones();


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_reclamar_giro_numero_referencia);

        //Se recuperan los valores de la vista anterior junto con el contador para recorrerlos
        Bundle extras = getIntent().getExtras();
        valores = extras.getStringArrayList("valores");
        contador = extras.getInt("contador");
        tipoDocumento = extras.getStringArrayList("tipoDocumento");

        //Se capturan los datos de respaldo
        for(int i =0; i< valores.size();i++){
            valoresRespaldo.add(valores.get(i));
        }
        for(int i =0; i< tipoDocumento.size();i++){
            tipoDocumentoRespaldo.add(tipoDocumento.get(i));
        }
        contadorRespaldo = contador;


        //Se genera la conexión con la interfaz gráfica
        editText_NumeroReferenciaReclamarGiro = findViewById(R.id.editText_NumeroReferenciaReclamarGiro);

       //Se crea el respectivo header con el titulo de la activity
        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderReclamarGiroNumeroReferencia,header).commit();
    }

    public void button_VamosNumeroReferenciaReclamarGiro(View view){

        String numeroReferencia = editText_NumeroReferenciaReclamarGiro.getText().toString();

        if(numeroReferencia.isEmpty()){
            Toast.makeText(getApplicationContext(),"Debe ingresar el número de referencia",Toast.LENGTH_LONG).show();
        } else{

            valores.add(numeroReferencia);
            //Se realiza el intent a la activity confirmar valores
            Intent i = new Intent(getApplicationContext(), pantallaConfirmacion.class);
            i.putExtra("titulo","<b>Reclamar Giro</b>");
            i.putExtra("descripcion","Por favor confirme el número de referencia");
            i.putExtra("titulos",titulos);
            i.putExtra("valores",valores);
            i.putExtra("terminos",false);
            i.putExtra("clase","");
            i.putExtra("contador", contador);
            i.putExtra("tipoDocumento", tipoDocumento);
            i.putExtra("transaccion", codigo.CORRESPONSAL_RECLAMACION_GIRO);
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
        contador =0;


        for(int i =0; i< valoresRespaldo.size();i++){
            this.valores.add(valoresRespaldo.get(i));
        }
        this.contador =contadorRespaldo;

        super.onRestart();
    }


}