package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.deposito;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.MenuItem;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.Toast;

import com.google.android.material.bottomnavigation.BottomNavigationView;

import java.util.ArrayList;
import java.util.Arrays;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.comunes.pantallaConfirmacion.pantallaConfirmacion;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.inicio.pantallaInicialUsuarioComun;
import co.com.netcom.corresponsal.pantallas.funciones.CodigosTransacciones;

public class pantallaDepositoDatosDepositante extends AppCompatActivity {

    private Header header = new Header("<b>Depósito</b>");
    private Spinner spinner_tipoCuentaDepositoDatosDepositante;
    private EditText editText_DepositoDatosDepositanteNumeroCuenta,editText_DepositoDatosDepositanteCantidad;
    private String [] titulos={"Número de cuenta","Cantidad"};
    private ArrayList<String> valores = new ArrayList<String>();
    private ArrayList<String> tipoDeCuenta = new ArrayList<String>();
    private BottomNavigationView menuDeposito;
    private CodigosTransacciones codigo = new CodigosTransacciones();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_deposito_datos_depositante);

        //Se crea el header de la actividad con el titulo correspondiente.

        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderDepositoDatosDepositante,header).commit();

        //Se crea la conexion con la interfaz grafica

        editText_DepositoDatosDepositanteNumeroCuenta = (EditText) findViewById(R.id.editText_DepositoDatosDepositanteNumeroCuenta);
        editText_DepositoDatosDepositanteCantidad = (EditText) findViewById(R.id.editText_DepositoDatosDepositanteCantidad);
        spinner_tipoCuentaDepositoDatosDepositante = (Spinner) findViewById(R.id.spinner_tipoCuentaDepositoDatosDepositante);
        menuDeposito = (BottomNavigationView) findViewById(R.id.menuDeposito);

        //Se crea el evento click de la barra de navegacion

        menuDeposito.setOnNavigationItemSelectedListener(new BottomNavigationView.OnNavigationItemSelectedListener() {
            @Override
            public boolean onNavigationItemSelected(@NonNull MenuItem menuItem) {

                if(menuItem.getItemId() == R.id.home_general){
                    Intent i = new Intent(getApplicationContext(), pantallaInicialUsuarioComun.class);
                    startActivity(i);
                    overridePendingTransition(R.anim.slide_in_left,R.anim.slide_out_right);
                }

                return true;
            }
        });


        //se crea un arreglo con los valores del spinner
        String [] opciones = {"Tipo de Cuenta", "Ahorros","Corriente"};
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
        spinner_tipoCuentaDepositoDatosDepositante.setAdapter(adapter);

    }

    /**Metodo continuarDatosDepositanteDeposito de tipo void, recibo como parametro un View, para poder ser implementado por
     * el button ContinuarDatosDepositanteDeposito. Hace el intent a la activity pantalla confirmacion y pasa como
     * extra todos los parametros ingresador por el usuario*/

    public void continuarDatosDepositanteDeposito(View view){

        //Se obtienen los datos ingresados por el usuario
        String numeroDeCuenta_string = editText_DepositoDatosDepositanteNumeroCuenta.getText().toString();
        String cantidad_string = editText_DepositoDatosDepositanteCantidad.getText().toString();
        String seleccion = spinner_tipoCuentaDepositoDatosDepositante.getSelectedItem().toString();
        String tipoCuenta ="";
        //Se verifica que los campos no esten vacios
        if (seleccion.equals("Tipo de cuenta")){
            Toast.makeText(getApplicationContext(),"Debe seleccionar un tipo de cuenta",Toast.LENGTH_SHORT).show();
        }else if(numeroDeCuenta_string.isEmpty()){
            Toast.makeText(getApplicationContext(),"Debe ingresar el número de cuenta", Toast.LENGTH_SHORT).show();
        }else if(cantidad_string.isEmpty()){
            Toast.makeText(getApplicationContext(),"Debe ingresar la cantidad",Toast.LENGTH_SHORT).show();
        } else{
            if(seleccion.equals("Ahorros")){
                 tipoCuenta = "10";
            }else{
                 tipoCuenta = "20";
            }

            valores.add(cantidad_string);
            valores.add(numeroDeCuenta_string);
            tipoDeCuenta.add(tipoCuenta);

            //Se realiza el intent a la activity confirmar valores
            Intent i = new Intent(getApplicationContext(), pantallaConfirmacion.class);

            i.putExtra("titulo","<b>Depósito</b>");
            i.putExtra("descripcion","Por favor confirme los datos del depositante.");
            i.putExtra("titulos",titulos);
            i.putExtra("valores",valores);
            i.putExtra("terminos",false);
            i.putExtra("clase","");
            i.putExtra("contador", 0);
            i.putExtra("tipoDeCuenta", tipoDeCuenta);
            i.putExtra("transaccion", codigo.CORRESPONSAL_DEPOSITO);
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