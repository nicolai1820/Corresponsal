package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.retiro.conTarjeta;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.Toast;

import java.util.ArrayList;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.comunes.pantallaConfirmacion.pantallaConfirmacion;

public class PantallaRetiroConTarjetaCantidad extends AppCompatActivity {


    private Header header = new Header("<b>Retiro Con Tarjeta</b>");
    private EditText editText_pantallaRetiroConTarjetaCantidad;
    private String [] titulos={"Cantidad"};
    private ArrayList<String> valores = new ArrayList<String>();




    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_retiro_con_tarjeta_cantidad);

        //Se crea el header de la actividad con el titulo correspondiente.

        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderPantallaRetiroConTarjetaCantidad,header).commit();

        //Se crea la conexion con la interfaz grafica
        editText_pantallaRetiroConTarjetaCantidad = (EditText) findViewById(R.id.editText_pantallaRetiroConTarjetaCantidad);

    }

    public void continuarRetiroConTarjeta(View view){

        String cantidad = editText_pantallaRetiroConTarjetaCantidad.getText().toString();



      if(cantidad.isEmpty()){
            Toast.makeText(getApplicationContext(),"Debe ingresar una cantidad", Toast.LENGTH_SHORT).show();
        }else{


            valores.add(cantidad);

            //Se realiza el intent a la activity confirmar valores
            Intent i = new Intent(getApplicationContext(), pantallaConfirmacion.class);

            i.putExtra("titulo","<b>Retiro Con Tarjeta</b>");
            i.putExtra("descripcion","Por favor confirme los valores");
            i.putExtra("titulos",titulos);
            i.putExtra("valores",valores);
            i.putExtra("terminos",false);
            i.putExtra("clase","co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.retiro.conTarjeta.PantallaRetiroConTarjetaLoader");
            i.putExtra("contador", 0);
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

    /**Metodo nativo de android onRestart. Se sobreescribe este metodo para que se vacie el arrego de los datos
     * en caso de que se vuelva desde la pantalla de confirmacion*/
    @Override
    protected void onRestart() {
        valores.clear();
        super.onRestart();
    }

}