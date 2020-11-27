package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.retiro.sinTarjeta;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

import java.util.ArrayList;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.comunes.pantallaConfirmacion.pantallaConfirmacion;
import co.com.netcom.corresponsal.pantallas.funciones.CodigosTransacciones;

public class pantallaRetiroSinTarjetaPin extends AppCompatActivity {

    private EditText editText_RetiroSinTarjetaPin;
    private Header header = new Header("<b>Retiro sin tarjeta.</b>");
    private ArrayList<String> valores = new ArrayList<String>();
    private ArrayList<String> valoresRespaldo = new ArrayList<String>();
    private String [] titulos={"Pin"};
    private int contador;
    private int contadorRespaldo;
    private CodigosTransacciones codigo = new CodigosTransacciones();


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_retiro_sin_tarjeta_pin);

        //Se recuperan los valores de la vista anterior junto con el contador para recorrerlos
        Bundle extras = getIntent().getExtras();
        valores = extras.getStringArrayList("valores");
        contador = extras.getInt("contador");

        //Se capturan los datos de respaldo
        for(int i =0; i< valores.size();i++){
            valoresRespaldo.add(valores.get(i));
        }

        contadorRespaldo = contador;

        //Se crea el respectivo header con el titulo de la activity
        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderRetiroSinTarjetaPin,header).commit();


        //Se crea la conexion con la interfaz grafica
        editText_RetiroSinTarjetaPin = (EditText) findViewById(R.id.editText_RetiroSinTarjetaPin);

    }


    public void retiroSinTarjetaPinVamos(View view){

        String retiroSinTarjetaPin_string = editText_RetiroSinTarjetaPin.getText().toString();

        if(retiroSinTarjetaPin_string.isEmpty()){
            Toast.makeText(getApplicationContext(),"Debe ingresar el pin",Toast.LENGTH_SHORT).show();
        }else {

            valores.add(retiroSinTarjetaPin_string);

            //Se realiza el intent a la activity confirmar valores
            Intent i = new Intent(getApplicationContext(), pantallaConfirmacion.class);
            i.putExtra("titulo","<b>Retiro sin tarjeta</b>");
            i.putExtra("descripcion","Por favor confirme el pin ingresado");
            i.putExtra("titulos",titulos);
            i.putExtra("valores",valores);
            i.putExtra("terminos",false);
            i.putExtra("clase","");
            i.putExtra("contador", contador);
            i.putExtra("transaccion", "");
            i.putExtra("transaccion", codigo.CORRESPONSAL_RETIRO_SIN_TARJETA);
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
        valores.clear();
        contador =0;

        for(int i =0; i< valoresRespaldo.size();i++){
            this.valores.add(valoresRespaldo.get(i));
        }

        this.contador =contadorRespaldo;
        super.onRestart();
    }
}