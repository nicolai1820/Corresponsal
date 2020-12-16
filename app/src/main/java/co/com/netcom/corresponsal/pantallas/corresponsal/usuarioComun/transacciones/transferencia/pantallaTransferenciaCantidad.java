package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.transferencia;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.Toast;

import com.google.android.material.bottomnavigation.BottomNavigationView;

import java.util.ArrayList;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.core.comunicacion.CardDTO;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.comunes.pantallaConfirmacion.pantallaConfirmacion;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.inicio.pantallaInicialUsuarioComun;

public class pantallaTransferenciaCantidad extends AppCompatActivity {

    private Header header;
    private ArrayList<String> valores = new ArrayList<String>();
    private ArrayList<String> tipoDeCuenta = new ArrayList<String>();
    private String [] titulos={"Tipo cuenta destino","Número cuenta destino","Cantidad"};
    private EditText editText_NumeroCuentaDestinoTransferencia, editText_CantidadTransferencia;
    private Spinner spinner_tipoCuentaDestinoTransferencia;
    private String tipoCuentaDestino ="0";
    private BottomNavigationView menuTransferencia;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_transferencia_cantidad);

        //Se hace la conexión con la interfaz gráfica
        editText_CantidadTransferencia = findViewById(R.id.editText_CantidadTransferencia);
        editText_NumeroCuentaDestinoTransferencia = findViewById(R.id.editText_NumeroCuentaDestinoTransferencia);
        spinner_tipoCuentaDestinoTransferencia = findViewById(R.id.spinner_tipoCuentaDestinoTransferencia);
        menuTransferencia = (BottomNavigationView) findViewById(R.id.menuTransferencia);

        //Se inicializa el fragmento del titulo
        header = new Header("<b>Transferencia</b>");

        //Se reemplaza el fragmento del titulo en la vista
        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderPantallaTransferenciaCantidad,header).commit();


        //Se crea el evento click de la barra de navegacion

        menuTransferencia.setOnNavigationItemSelectedListener(new BottomNavigationView.OnNavigationItemSelectedListener() {
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

        //Se agregan las opciones al spinner de cuenta destino
        ArrayAdapter<String> adapter2 = new ArrayAdapter<String>(getApplication(), R.layout.spinner_elements_style, getResources().getStringArray(R.array.array_TiposDeCuentaTranferenciaDestino)){

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
        spinner_tipoCuentaDestinoTransferencia.setAdapter(adapter2);
    }

    public void continuarCantidadTransaccion(View view){

        String cantidad = editText_CantidadTransferencia.getText().toString();
        String numeroCuentaDestino = editText_NumeroCuentaDestinoTransferencia.getText().toString();
        String seleccionCuentaDestino = spinner_tipoCuentaDestinoTransferencia.getSelectedItem().toString();

        if (seleccionCuentaDestino.equalsIgnoreCase("tipo de cuenta")){
            Toast.makeText(this,"Debe seleccionar un tipo de cuenta",Toast.LENGTH_SHORT).show();
        } else if (numeroCuentaDestino.isEmpty()){
            Toast.makeText(this,"Debe ingresar el número de cuenta",Toast.LENGTH_SHORT).show();
        }else if(cantidad.isEmpty()){
            Toast.makeText(this,"Debe ingresar la cantidad a transferir",Toast.LENGTH_SHORT).show();
        }
        else {

            if(seleccionCuentaDestino.equals("Ahorros")){
                tipoCuentaDestino = "10";
            }else{
                tipoCuentaDestino = "20";
            }
            valores.add(seleccionCuentaDestino);
            valores.add(numeroCuentaDestino);
            valores.add(cantidad);

            tipoDeCuenta.add(tipoCuentaDestino);

            Log.d("VALORES",valores.toString());
            Log.d("TIPO CUENTAS",tipoDeCuenta.toString());

            //Se realiza el intent a la activity confirmar valores
            Intent i = new Intent(getApplicationContext(), pantallaConfirmacion.class);

            i.putExtra("titulo","<b>Transferencia</b>");
            i.putExtra("descripcion","Por favor confirme los valores");
            i.putExtra("titulos",titulos);
            i.putExtra("valores",valores);
            i.putExtra("terminos",false);
            i.putExtra("clase","co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.transferencia.pantallaTransferenciaLectura");
            i.putExtra("contador", 0);
            i.putExtra("tipoDeCuenta", tipoDeCuenta);
            i.putExtra("transaccion", "");
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
        tipoDeCuenta.clear();

        super.onRestart();
    }


}