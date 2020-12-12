package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.transferencia;

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

public class pantallaTransferenciaTipoCuentas extends AppCompatActivity {

    private Header header;
    private CardDTO tarjeta;
    private ArrayList<String> valores = new ArrayList<String>();
    private ArrayList<String> tipoDeCuenta = new ArrayList<String>();
    private String [] titulos={"Tipo de cuenta origen","Tipo de cuenta destino"};
    private Spinner spinner_tipoCuentaOrigenTransferencia,spinner_tipoCuentaDestinoTransferencia;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_transferencia_tipo_cuentas);

        //Se inicializa el fragmento del titulo
        header = new Header("<b>Transferencia</b>");

        //Se reemplaza el fragmento del titulo en la vista
        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderPantallaTransferenciaTipoCuenta,header).commit();

        //Se hace la conexión con la interfaz grafica
        spinner_tipoCuentaOrigenTransferencia = findViewById(R.id.spinner_tipoCuentaOrigenTransferencia);
        spinner_tipoCuentaDestinoTransferencia = findViewById(R.id.spinner_tipoCuentaDestinoTransferencia);


        //se crea un arreglo con los valores del spinner
        String [] opciones = {"Tipo de Cuenta", "Ahorros","Corriente"};
        ArrayList<String> arrayList = new ArrayList<>(Arrays.asList(opciones));


        //Se agregan las opciones al spinner de cuenta origen
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
        spinner_tipoCuentaOrigenTransferencia.setAdapter(adapter);



        //Se agregan las opciones al spinner de cuenta destino
        ArrayAdapter<String> adapter2 = new ArrayAdapter<String>(getApplication(), R.layout.spinner_elements_style, arrayList){

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


        //Se rescatan los extras que vienen de la pantalla anterior
        Bundle i = getIntent().getExtras();

        tarjeta = new CardDTO();
        tarjeta = i.getParcelable("tarjeta");

        Log.d("CUENTAS TRANSFERENCIA",tarjeta.toString());
    }

    /**Metodo de tipo void, que recibe como parametro un elemento de tipo View, para poder ser implementado por el boton
     * Continuar Consulta Saldo. Se encarga de realizar el correspondiente intent con los extras correspondientes a la clase
     * pantallaConfirmación*/

    public void continuarTipoCuentaTransferencia(View view){

        String seleccionCuentaOrigen = spinner_tipoCuentaOrigenTransferencia.getSelectedItem().toString();
        String seleccionCuentaDestino = spinner_tipoCuentaDestinoTransferencia.getSelectedItem().toString();

        String tipoCuentaOrigen ="";
        String tipoCuentaDestino ="";

        //Se verifica que los campos no esten vacios
        if (seleccionCuentaOrigen.equalsIgnoreCase("Tipo de cuenta")) {
            Toast.makeText(getApplicationContext(), "Debe seleccionar el tipo de cuenta de origen", Toast.LENGTH_SHORT).show();
        }else if(seleccionCuentaDestino.equalsIgnoreCase("Tipo de cuenta")){
            Toast.makeText(getApplicationContext(), "Debe seleccionar el tipo de cuenta de destino", Toast.LENGTH_SHORT).show();
        }
        else{
            if(seleccionCuentaOrigen.equals("Ahorros")){
                tipoCuentaOrigen = "10";
            }else{
                tipoCuentaOrigen = "20";
            }

            if(seleccionCuentaDestino.equals("Ahorros")){
                tipoCuentaDestino = "10";
            }else{
                tipoCuentaDestino = "20";
            }

            valores.add(seleccionCuentaOrigen);
            tipoDeCuenta.add(tipoCuentaOrigen);

            valores.add(seleccionCuentaDestino);
            tipoDeCuenta.add(tipoCuentaDestino);


            //Se realiza el intent a la activity confirmar valores
            Intent i = new Intent(getApplicationContext(), pantallaConfirmacion.class);

            i.putExtra("titulo","<b>Transferencia</b>");
            i.putExtra("descripcion","Por favor confirme los tipos de cuenta");
            i.putExtra("titulos",titulos);
            i.putExtra("valores",valores);
            i.putExtra("terminos",false);
            i.putExtra("clase","co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.transferencia.pantallaTransferenciaCantidad");
            i.putExtra("contador", 0);
            i.putExtra("tipoDeCuenta", tipoDeCuenta);
            i.putExtra("transaccion", "");
            i.putExtra("tarjeta",tarjeta);
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