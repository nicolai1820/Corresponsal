package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.transferencia;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.Toast;

import java.util.ArrayList;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.core.comunicacion.CardDTO;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.comunes.pantallaConfirmacion.pantallaConfirmacion;

public class pantallaTransferenciaCantidad extends AppCompatActivity {

    private Header header;
    private CardDTO tarjeta;
    private ArrayList<String> valores = new ArrayList<String>();
    private ArrayList<String> valoresRespaldo = new ArrayList<String>();
    private ArrayList<String> tipoDeCuenta = new ArrayList<String>();
    private ArrayList<String> tipoDeCuentaRespaldo = new ArrayList<String>();
    private int contador;
    private int contadorRespaldo;
    private String [] titulos={"Cuenta destino","Cantidad"};
    private EditText editText_NumeroCuentaDestinoTransferencia, editText_CantidadTransferencia;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_transferencia_cantidad);

        //Se hace la conexión con la interfaz gráfica
        editText_CantidadTransferencia = findViewById(R.id.editText_CantidadTransferencia);
        editText_NumeroCuentaDestinoTransferencia = findViewById(R.id.editText_NumeroCuentaDestinoTransferencia);

        //Se inicializa el fragmento del titulo
        header = new Header("<b>Transferencia</b>");

        //Se reemplaza el fragmento del titulo en la vista
        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderPantallaTransferenciaCantidad,header).commit();

        //Se rescatan los extras que vienen de la pantalla anterior
        Bundle extras = getIntent().getExtras();

        tarjeta = new CardDTO();
        tarjeta = extras.getParcelable("tarjeta");
        valores = extras.getStringArrayList("valores");
        contador = extras.getInt("contador");
        tipoDeCuenta = extras.getStringArrayList("tipoDeCuenta");

        //Se capturan los datos de respaldo
        for(int i =0; i< valores.size();i++){
            valoresRespaldo.add(valores.get(i));
        }
        for(int i =0; i< tipoDeCuenta.size();i++){
            tipoDeCuentaRespaldo.add(tipoDeCuenta.get(i));
        }
        contadorRespaldo = contador;

        Log.d("CUENTAS TRANSFERENCIA",tarjeta.toString());

    }

    public void continuarCantidadTransaccion(View view){

        String cantidad = editText_CantidadTransferencia.getText().toString();
        String numeroCuentaDestino = editText_NumeroCuentaDestinoTransferencia.getText().toString();

        if (numeroCuentaDestino.isEmpty()){
            Toast.makeText(this,"Debe ingresar el número de cuenta",Toast.LENGTH_SHORT).show();
        }else if(cantidad.isEmpty()){
            Toast.makeText(this,"Debe ingresar la cantidad a transferir",Toast.LENGTH_SHORT).show();
        } else {

            valores.add(numeroCuentaDestino);
            valores.add(cantidad);

            Log.d("VALORES",valores.toString());
            Log.d("TIPO CUENTAS",tipoDeCuenta.toString());
            Log.d("TARJETA",tarjeta.toString());

            //Se realiza el intent a la activity confirmar valores
            Intent i = new Intent(getApplicationContext(), pantallaConfirmacion.class);

            i.putExtra("titulo","<b>Transferencia</b>");
            i.putExtra("descripcion","Por favor confirme los valores");
            i.putExtra("titulos",titulos);
            i.putExtra("valores",valores);
            i.putExtra("terminos",false);
            i.putExtra("clase","");
            i.putExtra("contador", contador);
            i.putExtra("tipoDeCuenta", tipoDeCuenta);
            i.putExtra("tarjeta",tarjeta);
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
        contador =0;
        for(int i =0; i< valoresRespaldo.size();i++){
            this.valores.add(valoresRespaldo.get(i));
        }

        this.contador =contadorRespaldo;

        super.onRestart();
    }


}