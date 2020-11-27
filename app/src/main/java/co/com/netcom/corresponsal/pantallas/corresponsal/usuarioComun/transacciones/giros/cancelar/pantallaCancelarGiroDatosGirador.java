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

public class pantallaCancelarGiroDatosGirador extends AppCompatActivity {

    private Header header = new Header("<b>Cancelar Giro</b>");
    private Spinner spinner_tipoDocumentoGiradorCancelarGiro;
    private EditText editText_tipoDocumentoGiradorCancelarGiro;
    private String [] titulos={"Número de documento"};
    private ArrayList<String> valores = new ArrayList<String>();
    private ArrayList<String> tipoDocumento = new ArrayList<String>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_cancelar_giro_datos_girador);

        //Se crea el header de la actividad con el correspondiente titulo
        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderCancelarGiroDatosGirador,header).commit();


        //Se crea la conexion con los elementos de la interfaz grafica
        spinner_tipoDocumentoGiradorCancelarGiro = (Spinner) findViewById(R.id.spinner_tipoDocumentoGiradorCancelarGiro);
        editText_tipoDocumentoGiradorCancelarGiro = (EditText) findViewById(R.id.editText_DocumentoGiradorCancelarGiro);

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
        spinner_tipoDocumentoGiradorCancelarGiro.setAdapter(adapter);

    }

    /**Metodo continuarDatosGiradorCancelarGiro de tipo void, recibo como parametro un View, para poder ser implementado por
     * el button ContinuarDatosGiradorCancelarGiro. Hace el intent a la activity pantalla confirmacion y pasa como
     * extra todos los parametros ingresador por el usuario*/

    public void continuarDatosGiradorCancelarGiro(View view){

        //Se obtiene los valores digitados por el usuario

        String numeroDocumentoGirador_string = editText_tipoDocumentoGiradorCancelarGiro.getText().toString();
        String tipoDocumento_String = spinner_tipoDocumentoGiradorCancelarGiro.getSelectedItem().toString();

        //Se verifica que los campos no esten vacios

        if (numeroDocumentoGirador_string.isEmpty()){
            Toast.makeText(getApplicationContext(),"Debe ingresar el número de documento",Toast.LENGTH_LONG).show();
        }else if (tipoDocumento_String.equals("Tipo de Documento")){
            Toast.makeText(getApplicationContext(),"Debe seleccionar un tipo de documento",Toast.LENGTH_LONG).show();

        } else {

            valores.add(numeroDocumentoGirador_string);
            tipoDocumento.add(tipoDocumento_String);

            //Se realiza el intent a la activity confirmar valores
            Intent i = new Intent(getApplicationContext(), pantallaConfirmacion.class);

            i.putExtra("titulo","<b>Cancelar Giro</b>");
            i.putExtra("descripcion","Por favor confirme los datos del depositante.");
            i.putExtra("titulos",titulos);
            i.putExtra("valores",valores);
            i.putExtra("terminos",false);
            i.putExtra("clase","co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.giros.cancelar.pantallaCancelarGiroDatosBeneficiario");
            i.putExtra("contador", 0);
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

    /**Metodo nativo de android onRestart. Se sobreescribe para que se eliminen los datos erroneos ingresados por
     * el usuario.*/

    @Override
    public void onRestart(){

        this.valores.clear();
        this.tipoDocumento.clear();
        super.onRestart();
    }
}