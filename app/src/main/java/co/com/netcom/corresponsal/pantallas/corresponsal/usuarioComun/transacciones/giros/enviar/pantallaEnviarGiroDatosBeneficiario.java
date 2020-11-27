package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.giros.enviar;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.Arrays;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.core.comunicacion.Procesos;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.comunes.pantallaConfirmacion.pantallaConfirmacion;
import co.com.netcom.corresponsal.pantallas.funciones.CodigosTransacciones;

public class pantallaEnviarGiroDatosBeneficiario extends AppCompatActivity {

    private Header header = new Header("<b>Enviar Giro</b>");
    private Spinner spinner_tipoDocumentoBeneficiarioEnviarGiro;
    private EditText editText_DocumentoBeneficiarioEnviarGiro,editText_NumeroCelularBeneficiarioEnviarGiro;
    private ArrayList<String> valores = new ArrayList<String>();
    private ArrayList<String> valoresRespaldo = new ArrayList<String>();
    private ArrayList<String> tipoDocumento = new ArrayList<String>();
    private ArrayList<String> tipoDocumentoRespaldo = new ArrayList<String>();
    private String [] titulos={"Número de documento","Número de celular."};
    private int contador;
    private int contadorRespaldo;
    private CodigosTransacciones codigo = new CodigosTransacciones();


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_enviar_giro_datos_beneficiario);

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


        //Se genera la conexion con la interfaz grafica
        spinner_tipoDocumentoBeneficiarioEnviarGiro = (Spinner) findViewById(R.id.spinner_tipoDocumentoBeneficiarioEnviarGiro);
        editText_DocumentoBeneficiarioEnviarGiro = (EditText) findViewById(R.id.editText_DocumentoBeneficiarioEnviarGiro);
        editText_NumeroCelularBeneficiarioEnviarGiro = (EditText) findViewById(R.id.editText_NumeroCelularBeneficiarioEnviarGiro);


        //Se crea el respectivo header con el titulo de la activity
        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderGirosDatosBeneficiario,header).commit();

        //se crea un arreglo con los valores del spinner
        String [] opciones = {"Tipo de documento"," Tarjeta de identidad","Cedula de ciudadania", "Pasaporte"};
        ArrayList<String> arrayList = new ArrayList<>(Arrays.asList(opciones));

        //Se agregan las opciones al spinner
        ArrayAdapter<String> adapter = new ArrayAdapter<String>(getApplication(), R.layout.spinner_elements_style, arrayList){

            //Se deshabilita la primera opcion del spinner
            @Override
            public boolean isEnabled(int position){
                if(position == 0) {
                    return false;
                }
                else {
                    return true;
                }
            }
        };
        spinner_tipoDocumentoBeneficiarioEnviarGiro.setAdapter(adapter);
    }

    /**Metodo continuarDatosGirador de tipo void, recibo como parametro un View, para poder ser implementado por
     * el button ContinuarDatosGiradorEnviarGiro. Hace el intent a la activity pantalla confirmacion y pasa como
     * extra todos los parametros ingresador por el usuario*/

    public void continuarDatosBeneficiario(View view){

        String numeroDocumento = editText_DocumentoBeneficiarioEnviarGiro.getText().toString();
        String numeroCelular = editText_NumeroCelularBeneficiarioEnviarGiro.getText().toString();
        String documento_string = spinner_tipoDocumentoBeneficiarioEnviarGiro.getSelectedItem().toString();

        if (numeroDocumento.isEmpty()){
            Toast.makeText(getApplicationContext(),"Debe ingresar el número de documento",Toast.LENGTH_LONG).show();
        } else if(numeroCelular.isEmpty()){
            Toast.makeText(getApplicationContext(),"Debe ingresa el número de celular",Toast.LENGTH_LONG).show();
        } else if (documento_string.equals("Tipo de documento")){
            Toast.makeText(getApplicationContext(),"Debe seleccionar el tipo de documento",Toast.LENGTH_LONG).show();
        }
        else {

            valores.add(numeroDocumento);
            valores.add(numeroCelular);
            tipoDocumento.add(documento_string);

            //Se realiza el intent a la activity confirmar valores
            Intent i = new Intent(getApplicationContext(), pantallaConfirmacion.class);
            i.putExtra("titulo","<b>Enviar Giro</b>");
            i.putExtra("descripcion","Por favor confirme los datos del Beneficiario.");
            i.putExtra("titulos",titulos);
            i.putExtra("valores",valores);
            i.putExtra("terminos",false);
            i.putExtra("clase","");
            i.putExtra("contador", contador);
            i.putExtra("tipoDocumento", tipoDocumento);
            i.putExtra("transaccion", codigo.CORRESPONSAL_ENVIO_GIRO);
            startActivity(i);
            overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);

        }

    }


    /**Metodo nativo de android onBackPressed. Se sobreescribe para que el usuario no se pueda desplazar hacia atras
     * por medio del menu del celular.*/

    @Override
    public void onBackPressed() {

    }

    /**Metodo nativo de android onRestart. Se sobreescribe para que se eliminen los datos erroneos digitados
     * por el usuario*/

    @Override
    protected void onRestart() {

        valores.clear();
        tipoDocumento.clear();
        contador =0;

        for(int i =0; i< valoresRespaldo.size();i++){
            this.valores.add(valoresRespaldo.get(i));
        }

        for(int i =0; i< tipoDocumentoRespaldo.size();i++){
            this.tipoDocumento.add(tipoDocumentoRespaldo.get(i));
        }

        this.contador =contadorRespaldo;

        super.onRestart();
    }


}