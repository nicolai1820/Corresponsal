package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.consultaSaldo;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Spinner;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.Arrays;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.core.comunicacion.CardDTO;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.comunes.pantallaConfirmacion.pantallaConfirmacion;
import co.com.netcom.corresponsal.pantallas.funciones.CodigosTransacciones;

public class pantallaConsultaSaldoTipoCuenta extends AppCompatActivity {

    private Header header;
    private CardDTO tarjeta;
    private ArrayList<String> valores = new ArrayList<String>();
    private ArrayList<String> tipoDeCuenta = new ArrayList<String>();
    private String [] titulos={"Tipo de cuenta"};
    private CodigosTransacciones codigo = new CodigosTransacciones();
    private Spinner spinner_ConsultaSaldo;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_consulta_saldo_tipo_cuenta);

        //Se inicializa el fragmento del titulo
        header = new Header("<b>Consulta de Saldo</b>");

        //Se reemplaza el fragmento del titulo en la vista
        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderPantallaConsultaSaldoTipoCuenta,header).commit();

        //Se hace la conexión con la interfaz grafica
        spinner_ConsultaSaldo = findViewById(R.id.spinner_tipoDeCuentaConsultaSaldo);



        //Se agregan las opciones al spinner
        ArrayAdapter<String> adapter = new ArrayAdapter<String>(getApplication(), R.layout.spinner_elements_style, getResources().getStringArray(R.array.array_TiposDeCuentaConsultaSaldo)){

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
        spinner_ConsultaSaldo.setAdapter(adapter);



        //Se rescatan los extras que vienen de la pantalla anterior
        Bundle i = getIntent().getExtras();

        tarjeta = new CardDTO();
        tarjeta = i.getParcelable("tarjeta");

        Log.d("TARJETA TIPO CUENTA",tarjeta.toString());


    }

    /**Metodo de tipo void, que recibe como parametro un elemento de tipo View, para poder ser implementado por el boton
     * Continuar Consulta Saldo. Se encarga de realizar el correspondiente intent con los extras correspondientes a la clase
     * pantallaConfirmación*/

    public void continuarConsultaSaldo(View view){

        String seleccion = spinner_ConsultaSaldo.getSelectedItem().toString();
        String tipoCuenta ="";
        //Se verifica que los campos no esten vacios
        if (seleccion.equalsIgnoreCase("Tipo de cuenta")) {
            Toast.makeText(getApplicationContext(), "Debe seleccionar un tipo de cuenta", Toast.LENGTH_SHORT).show();
        }
        else{
            if(seleccion.equals("Ahorros")){
                tipoCuenta = "10";
            }else{
                tipoCuenta = "20";
            }


            tipoDeCuenta.add(tipoCuenta);
            valores.add(seleccion);

            //Se realiza el intent a la activity confirmar valores
            Intent i = new Intent(getApplicationContext(), pantallaConfirmacion.class);

            i.putExtra("titulo","<b>Consulta de Saldo</b>");
            i.putExtra("descripcion","Por favor confirme el tipo de cuenta");
            i.putExtra("titulos",titulos);
            i.putExtra("valores",valores);
            i.putExtra("terminos",false);
            i.putExtra("clase","");
            i.putExtra("contador", 0);
            i.putExtra("tipoDeCuenta", tipoDeCuenta);
            i.putExtra("transaccion", codigo.CORRESPONSAL_CONSULTA_SALDO);
            startActivity(i);
            overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);
        }
    }



    /**Metodo nativo de android onBackPressed. Se sobreescribe para que el usuario no se pueda desplazar hacia atras
     * por medio del menu del celular.*/
    @Override
    public void onBackPressed() {

    }

    /**Metodo nativo de android onRestart. Se sobreescribe este metodo para que se vacie el arrego de los datos
     * en caso de que se vuelva desde la pantalla de confirmacion*/
    @Override
    protected void onRestart() {
        valores.clear();
        tipoDeCuenta.clear();
        super.onRestart();
    }

}