package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.giros.reclamar;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.text.InputFilter;
import android.util.Log;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.Switch;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.Arrays;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.comunes.pantallaConfirmacion.pantallaConfirmacion;
import co.com.netcom.corresponsal.pantallas.funciones.BaseActivity;

public class pantallaReclamarGiroDatosBeneficiario extends BaseActivity {

    private boolean terminos_boolean;
    private Switch terminos_switch;
    private Header header = new Header("<b>Reclamar Giro</b>");
    private ArrayList<String> valores = new ArrayList<>();
    private ArrayList<String> tipoDocumento = new ArrayList<>();
    String titulos [] = {"Tipo de documento","Número de documento","Valor del giro"};
    private ArrayList<Integer> iconos = new ArrayList<Integer>();
    private EditText editText_DocumentoBeneficiarioReclamarGiro,editText_MontoReclamarGiro;
    private Spinner spinner_tipoDocumentoBeneficiarioReclamarGiro;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_reclamar_giro_datos_beneficiario);
        terminos_boolean =false;

        //Se crea la conexion con la interfaz grafica
        terminos_switch = (Switch) findViewById(R.id.switch_ReclamarGiro);
        editText_DocumentoBeneficiarioReclamarGiro = (EditText) findViewById(R.id.editText_DocumentoBeneficiarioReclamarGiro);
        editText_MontoReclamarGiro =(EditText) findViewById(R.id.editText_MontoReclamarGiro);
        spinner_tipoDocumentoBeneficiarioReclamarGiro = (Spinner) findViewById(R.id.spinner_tipoDocumentoBeneficiarioReclamarGiro);

        //metodo del switch para asignar el estado de los terminos y condiciones
        terminos_switch.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(terminos_switch.isChecked()){
                    terminos_boolean = true;
                } else {
                    terminos_boolean =false;
                }
            }});

        //se crea un arreglo con los valores del spinner
        String [] opciones = {"Tipo de documento","Tarjeta de identidad","Cedula de ciudadania", "Pasaporte"};
        ArrayList<String> arrayList = new ArrayList<>(Arrays.asList(opciones));

        //Se agregan las opciones al spinner
        ArrayAdapter<String> adapter = new ArrayAdapter<String>(getApplication(), R.layout.spinner_elements_style, arrayList){

            //Se deshabilita la primera opcion del spinner
            @Override
            public boolean isEnabled(int position){
                if(position == 0) {
                    // Disable the first item from Spinner
                    // First item will be use for hint
                    return false;
                }
                else {
                    return true;
                }
            }
        };
        spinner_tipoDocumentoBeneficiarioReclamarGiro.setAdapter(adapter);

        //Se inicializa el header con el respectivo titulo de la vista.
        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderReclamarGiroDatosBeneficiario,header).commit();
        editText_DocumentoBeneficiarioReclamarGiro.setFilters(new InputFilter[]{new InputFilter.LengthFilter(15)});
        editText_MontoReclamarGiro.setFilters(new InputFilter[]{new InputFilter.LengthFilter(15)});

    }




    /**Metodo continuarDatosBeneficiarioReclamarGiro de tipo void, recibe como parametro un View para poder ser implementado por el boton continuar,
     * este hace un intent a la activity pantalla confirmacion, para que el usuario verifique los valores digitados*/


    public void continuarDatosBeneficiarioReclamarGiro(View view){
        String documentoBeneficiarioReclamarGiro = editText_DocumentoBeneficiarioReclamarGiro.getText().toString();
        String montoReclamarGiro = editText_MontoReclamarGiro.getText().toString();
        String documento_string = spinner_tipoDocumentoBeneficiarioReclamarGiro.getSelectedItem().toString();

        if(!terminos_boolean){
            Log.d("prueba",documentoBeneficiarioReclamarGiro);
            Toast.makeText(getApplicationContext(),"Debe aceptar los terminos y condiciones",Toast.LENGTH_LONG).show();

        }else if(documentoBeneficiarioReclamarGiro.isEmpty()){
                Toast.makeText(getApplicationContext(),"Debe ingresar el documento del beneficiario",Toast.LENGTH_LONG).show();
        } else if(montoReclamarGiro.isEmpty()){
                Toast.makeText(getApplicationContext(),"Debe ingresar el monto.", Toast.LENGTH_LONG).show();
        } else if (documento_string.equals("Tipo de documento")){
            Toast.makeText(getApplicationContext(),"Debe seleccionar el tipo de documento", Toast.LENGTH_LONG).show();

        }
        else {

            valores.add(documento_string);
            valores.add(documentoBeneficiarioReclamarGiro);
            valores.add(montoReclamarGiro);
            tipoDocumento.add(documento_string);

            iconos.add(4);
            iconos.add(3);
            iconos.add(1);

            Intent i = new Intent(getApplicationContext(), pantallaConfirmacion.class);

            i.putExtra("titulo","<b>Reclamar Giro</b>");
            i.putExtra("descripcion","Confirme los datos para continuar");
            i.putExtra("titulos", titulos);
            i.putExtra("valores",valores);
            i.putExtra("tipoDocumento",tipoDocumento);
            i.putExtra("terminos",false);
            i.putExtra("contador",0);
            i.putExtra("iconos",iconos);
            i.putExtra("clase","co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.giros.reclamar.pantallaReclamarGiroNumeroReferencia");
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
        tipoDocumento.clear();

        super.onRestart();
    }
}