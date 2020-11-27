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

public class pantallaRetiroSinTarjetaCantidad extends AppCompatActivity {
    Header header = new Header("<b>Retiro sin tarjeta</b>");
    private EditText retiroSinTarjetaCantidad;
    private ArrayList<String> valores = new ArrayList<String>();
    private String titulos [] = {"Cantidad"};

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_retiro_sin_tarjeta_cantidad);

        //Se crea el header de la actividad con el correspondiente titulo
        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderRetiroSinTarjetaCantidad,header).commit();

        //Se crea la conexion con la interfaz grafica

        retiroSinTarjetaCantidad = (EditText) findViewById(R.id.editText_RetiroSinTarjetaCantidad);
    }

    /**Metodo retiroSinTarjetaContinuar de tipo void, recibe como parametro un View para poder ser implementado por el
     * button_retiroSinTarjetaContinuar,este hace un intent a la activity pantalla confirmacion, para que el usuario verifique la
     * cantidad digitada*/

    public void retiroSinTarjetaContinuar(View view){

        String cantidadRetiroSinTarjeta_string = retiroSinTarjetaCantidad.getText().toString();

        if(cantidadRetiroSinTarjeta_string.isEmpty()){
            Toast.makeText(getApplicationContext(),"Debe ingresar la cantidad",Toast.LENGTH_SHORT).show();
        } else {

            valores.add(cantidadRetiroSinTarjeta_string);

            //Se realiza el intent a la activity confirmar valores
            Intent i = new Intent(getApplicationContext(), pantallaConfirmacion.class);
            i.putExtra("titulo","<b>Retiro sin tarjeta.</b>");
            i.putExtra("descripcion","Por favor confirme la cantidad ingresada.");
            i.putExtra("titulos",titulos);
            i.putExtra("valores",valores);
            i.putExtra("terminos",false);
            i.putExtra("clase","co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.retiro.sinTarjeta.pantallaRetiroSinTarjetaPin");
            i.putExtra("contador",0);
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
        super.onRestart();
    }
}