package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.pagoFacturas.manual.manualCuentaRecaudadora;

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
import co.com.netcom.corresponsal.pantallas.funciones.BaseActivity;
import co.com.netcom.corresponsal.pantallas.funciones.CodigosTransacciones;

public class pantallaPagoFacturasManualCuentaRecaudadora extends BaseActivity {

    private Header header = new Header("<b>Cuenta recaudadora</b>");
    private Spinner spinner_tipoDeCuentaCuentaRecaudadora;
    private EditText editText_PagoFacturasManualNumeroCuentaRecaudadora,editText_PagoFacturasManualCuentaRecaudadoraNumeroReferencia;
    private ArrayList<String> valores = new ArrayList<String>();
    private ArrayList<String> tipoDeCuenta= new ArrayList<String>();
    private String tipoCuenta="";
    private String [] titulos={"Cuenta recaudadora","Número de referencia"};
    private ArrayList<Integer> iconos = new ArrayList<Integer>();
    private CodigosTransacciones codigo = new CodigosTransacciones();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_pago_facturas_manual_cuenta_recaudadora);

        //Se crea el respectivo header con el titulo de la activity
        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderPagoFacturasManualCuentaRecaudadora,header).commit();

        //Se genera la conexion con la interfaz grafica
        spinner_tipoDeCuentaCuentaRecaudadora = (Spinner) findViewById(R.id.spinner_tipoDeCuentaCuentaRecaudadora);
        editText_PagoFacturasManualNumeroCuentaRecaudadora = (EditText) findViewById(R.id.editText_PagoFacturasManualNumeroCuentaRecaudadora);
        editText_PagoFacturasManualCuentaRecaudadoraNumeroReferencia = (EditText) findViewById(R.id.editText_PagoFacturasManualCuentaRecaudadoraNumeroReferencia);


        //se crea un arreglo con los valores del spinner
        String [] opciones = {"Tipo de Cuenta", "Cuenta de ahorros","Cuenta de crédito"};
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
        spinner_tipoDeCuentaCuentaRecaudadora.setAdapter(adapter);
    }

    /**Metodo pagoFacturasManualCuentaRecaudadoraVamos de tipo void, recibo como parametro un View, para poder ser implementado por
     * el button button_pagoFacturasManualCuentaRecaudadoraVamos. Hace el intent a la activity pantalla confirmacion y pasa como
     * extra todos los parametros ingresador por el usuario*/

   public  void pagoFacturasManualCuentaRecaudadoraVamos(View view){

        String numeroCuentaRecaudadora_string = editText_PagoFacturasManualNumeroCuentaRecaudadora.getText().toString();
        String numeroReferencia_string = editText_PagoFacturasManualCuentaRecaudadoraNumeroReferencia.getText().toString();
        String seleccion_tipoCuenta = spinner_tipoDeCuentaCuentaRecaudadora.getSelectedItem().toString();

        if(numeroCuentaRecaudadora_string.isEmpty()){
            Toast.makeText(getApplicationContext(),"Debe ingresar el número de la cuenta",Toast.LENGTH_SHORT).show();
        }else if(numeroReferencia_string.isEmpty()){
            Toast.makeText(getApplicationContext(),"Debe ingresar el número de referencia",Toast.LENGTH_SHORT).show();
        } else if(seleccion_tipoCuenta.equals("Tipo de Cuenta")){
            Toast.makeText(getApplicationContext(),"Debe seleccionar un tipo de cuenta",Toast.LENGTH_SHORT).show();
        } else{

            if (seleccion_tipoCuenta.equalsIgnoreCase("Ahorros")){
                tipoCuenta="10";
            }else{
                tipoCuenta="20";
            }
            valores.add(numeroCuentaRecaudadora_string);
            valores.add(numeroReferencia_string);
            tipoDeCuenta.add(tipoCuenta);

            iconos.add(3);
            iconos.add(3);

            //Se realiza el intent a la activity confirmar valores
            Intent i = new Intent(getApplicationContext(), pantallaConfirmacion.class);

            i.putExtra("titulo","<b>Cuenta recaudadora</b>");
            i.putExtra("descripcion","Por favor confirme los datos ingresados");
            i.putExtra("titulos",titulos);
            i.putExtra("valores",valores);
            i.putExtra("terminos",false);
            i.putExtra("clase","");
            i.putExtra("contador", 0);
            i.putExtra("tipoDeCuenta", tipoDeCuenta);
            i.putExtra("iconos",iconos);
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
        tipoDeCuenta.clear();
        super.onRestart();
    }

}