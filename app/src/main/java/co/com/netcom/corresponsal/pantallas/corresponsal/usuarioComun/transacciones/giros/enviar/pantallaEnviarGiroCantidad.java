package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.giros.enviar;

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

public class pantallaEnviarGiroCantidad extends AppCompatActivity {

    private Header header = new Header("<b>Enviar Giros</b>");
    private EditText cantidad;
    private pantallaConfirmacion confirmacion;
    private ArrayList<String> valores = new ArrayList<String>();
    private String titulos [] = {"Valor del Giro","Valor de comisión","Valor total del giro"};

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_enviar_giro_cantidad);

        //Se realiza la conexion con los elementos de la interfaz grafica
        cantidad = (EditText) findViewById(R.id.editText_CantidadGirosEnviar);

        //Se crea el header con el respectivo titulo de la actividad
        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderGirosEnviar,header).commit();
    }



    /**Metodo continuarCantidad de tipo void, recibe como parametro un View para poder ser implementado por el boton continuar,
     * este hace un intent a la activity pantalla confirmacion, para que el usuario verifique los valores digitados*/


    public void continuarCantidad(View view){

        //Se obtiene la cantidad digitada por el usuario
        String cantidad_string = cantidad.getText().toString();

        //Se evalua que el campo no este vacio

        if(cantidad_string.isEmpty()){

            Toast.makeText(getApplicationContext(),"Debe ingresar una cantidad",Toast.LENGTH_LONG).show();

        } else {


            valores.add(cantidad_string);
            valores.add("10.000");
            valores.add(String.valueOf(Integer.parseInt(cantidad_string)+10000));
             /*confirmacion = new pantallaConfirmacion("Enviar Giro","Por favor confirme los datos del girador.",
                titulos,valores,false, pantallaInicialUsuarioComun.class);*/

            //Se realiza el intent a la activity confirmar valores
            Intent i = new Intent(getApplicationContext(),pantallaConfirmacion.class);
            i.putExtra("titulo","<b>Enviar Giro</b>");
            i.putExtra("descripcion","Por favor confirme con el girador los datos de la comision.");
            i.putExtra("titulos",titulos);
            i.putExtra("valores",valores);
            i.putExtra("terminos",false);
            i.putExtra("clase","co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.giros.enviar.pantallaEnviarGiroDatosGirador");
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

    /**Metodo nativo de android onRestart.Se sobreescribe para eliminar los datos erroneos ingresados por el usuario*/

    @Override
    protected void onRestart() {
        valores.clear();
        super.onRestart();
    }
}