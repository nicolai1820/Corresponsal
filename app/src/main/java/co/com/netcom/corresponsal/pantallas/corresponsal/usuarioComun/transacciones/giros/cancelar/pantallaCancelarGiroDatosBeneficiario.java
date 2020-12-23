package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.giros.cancelar;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.Arrays;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.comunes.pantallaConfirmacion.pantallaConfirmacion;

public class pantallaCancelarGiroDatosBeneficiario extends AppCompatActivity {

    private Header header = new Header("<b>Cancelar Giro</b>");
    private Spinner spinner_tipoDocumentoBeneficiarioCancelarGiro;
    private EditText editText_tipoDocumentoBeneficiarioCancelarGiro;
    private String [] titulos={"Tipo de documento","Número de documento"};
    private ArrayList<String> valores = new ArrayList<String>();
    private ArrayList<String> valoresRespaldo = new ArrayList<String>();
    private ArrayList<Integer> iconos = new ArrayList<Integer>();
    private ArrayList<String> tipoDocumento = new ArrayList<String>();
    private ArrayList<String> tipoDocumentoRespaldo = new ArrayList<String>();
    private int contador;
    private int contadorRespaldo;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_cancelar_giro_datos_beneficiario);

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

        //Se crea el header de la actividad con el correspondiente titulo
        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderCancelarGiroDatosBeneficiario,header).commit();

        //Se crea la conexion con los elementos de la interfaz grafica
        spinner_tipoDocumentoBeneficiarioCancelarGiro = (Spinner) findViewById(R.id.spinner_tipoDocumentoBeneficiarioCancelarGiro);
        editText_tipoDocumentoBeneficiarioCancelarGiro = (EditText) findViewById(R.id.editText_DocumentoBeneficiarioCancelarGiro);


        //se crea un arreglo con los valores del spinner
        String [] opciones = {"Tipo de documento", "Tarjeta de identidad","Cedula de ciudadania", "Pasaporte"};
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
        spinner_tipoDocumentoBeneficiarioCancelarGiro.setAdapter(adapter);
    }


    /**Metodo continuarDatosBeneficiarioCancelarGiro de tipo void, recibo como parametro un View, para poder ser implementado por
     * el button ContinuarDatosBeneficiarioCancelarGiro. Hace el intent a la activity pantalla confirmacion y pasa como
     * extra todos los parametros ingresador por el usuario*/

    public void continuarDatosBeneficiarioCancelarGiro(View view){

        //Se obtiene los valores digitados por el usuario

        String numeroDocumentoGirador_string = editText_tipoDocumentoBeneficiarioCancelarGiro.getText().toString();
        String tipoDocumento_String = spinner_tipoDocumentoBeneficiarioCancelarGiro.getSelectedItem().toString();

        //Se verifica que los campos no esten vacios

        if (numeroDocumentoGirador_string.isEmpty()){
            Toast.makeText(getApplicationContext(),"Debe ingresar el número de documento",Toast.LENGTH_LONG).show();
        }else if (tipoDocumento_String.equals("Tipo de Documento")){
            Toast.makeText(getApplicationContext(),"Debe seleccionar un tipo de documento",Toast.LENGTH_LONG).show();

        } else {

            valores.add(tipoDocumento_String);
            valores.add(numeroDocumentoGirador_string);
            tipoDocumento.add(tipoDocumento_String);

            iconos.add(3);
            iconos.add(3);
            //Se realiza el intent a la activity confirmar valores
            Intent i = new Intent(getApplicationContext(), pantallaConfirmacion.class);

            i.putExtra("titulo","<b>Cancelar Giro</b>");
            i.putExtra("descripcion","Por favor confirme los datos del beneficiario.");
            i.putExtra("titulos",titulos);
            i.putExtra("valores",valores);
            i.putExtra("terminos",false);
            i.putExtra("clase","co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.giros.cancelar.pantallaCancelarGiroPinReferencia");
            i.putExtra("contador", contador);
            i.putExtra("iconos",iconos);
            i.putExtra("tipoDocumento", tipoDocumento);
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