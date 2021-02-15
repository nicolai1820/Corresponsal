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
import co.com.netcom.corresponsal.pantallas.funciones.BaseActivity;

public class pantallaRetiroSinTarjetaCantidad extends BaseActivity {
    Header header = new Header("<b>Retiro sin tarjeta</b>");
    private EditText retiroSinTarjetaCantidad, retiroSinTarjetaNumeroCuenta;
    private ArrayList<String> valores = new ArrayList<String>();
    private ArrayList<Integer> iconos = new ArrayList<Integer>();
    private String titulos [] = {"Número de cuenta","Cantidad"};

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_retiro_sin_tarjeta_cantidad);

        //Se crea el header de la actividad con el correspondiente titulo
        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderRetiroSinTarjetaCantidad,header).commit();

        //Se crea la conexion con la interfaz grafica

        retiroSinTarjetaCantidad = (EditText) findViewById(R.id.editText_RetiroSinTarjetaCantidad);
        retiroSinTarjetaNumeroCuenta = (EditText) findViewById(R.id.editText_RetiroSinTarjetaNumeroCuenta);
    }

    /**Metodo retiroSinTarjetaContinuar de tipo void, recibe como parametro un View para poder ser implementado por el
     * button_retiroSinTarjetaContinuar,este hace un intent a la activity pantalla confirmacion, para que el usuario verifique la
     * cantidad digitada y el número de cuenta.*/

    public void retiroSinTarjetaContinuar(View view){

        String cantidadRetiroSinTarjeta_string = retiroSinTarjetaCantidad.getText().toString();
        String numeroDeCuenta_string = retiroSinTarjetaNumeroCuenta.getText().toString();

        if(numeroDeCuenta_string.isEmpty()){
            Toast.makeText(getApplicationContext(),"Debe ingresar el número de cuenta",Toast.LENGTH_SHORT).show();
        } else if(cantidadRetiroSinTarjeta_string.isEmpty()){
            Toast.makeText(getApplicationContext(),"Debe ingresar la cantidad",Toast.LENGTH_SHORT).show();
        } else {

            valores.add(numeroDeCuenta_string);
            valores.add(cantidadRetiroSinTarjeta_string);

            iconos.add(3);
            iconos.add(1);
            //Se realiza el intent a la activity confirmar valores
            Intent i = new Intent(getApplicationContext(), pantallaConfirmacion.class);
            i.putExtra("titulo","<b>Retiro sin tarjeta.</b>");
            i.putExtra("descripcion","Por favor confirme la cantidad ingresada.");
            i.putExtra("titulos",titulos);
            i.putExtra("valores",valores);
            i.putExtra("terminos",false);
            i.putExtra("clase","co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.retiro.sinTarjeta.pantallaRetiroSinTarjetaPin");
            i.putExtra("contador",0);
            i.putExtra("iconos",iconos);
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