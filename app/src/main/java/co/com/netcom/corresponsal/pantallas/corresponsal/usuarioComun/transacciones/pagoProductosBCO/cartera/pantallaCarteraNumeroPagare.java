package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.pagoProductosBCO.cartera;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.text.InputFilter;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

import java.util.ArrayList;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.comunes.pantallaConfirmacion.pantallaConfirmacion;
import co.com.netcom.corresponsal.pantallas.funciones.BaseActivity;
import co.com.netcom.corresponsal.pantallas.funciones.CodigosTransacciones;

public class pantallaCarteraNumeroPagare extends BaseActivity {

    private Header header = new Header("<b>Pago productos banco.</b>");
    private EditText editTextCarteraNumeroPagare, editText_CarteraCantidad;
    private String [] titulos={"Número de pagaré","Cantidad"};
    private ArrayList<String> valores = new ArrayList<String>();
    private CodigosTransacciones codigo = new CodigosTransacciones();
    private ArrayList<Integer> iconos = new ArrayList<Integer>();
    private String origen ="2";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_cartera_numero_pagare);

        //Se crea el header con el correspondiente titulo

        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderCarteraNumeroPagare,header).commit();

        //se realiz a la respectiva conexion con la interfaz grafica
        editTextCarteraNumeroPagare = (EditText) findViewById(R.id.editText_CarteraNumeroPagare);

        editText_CarteraCantidad = (EditText) findViewById(R.id.editText_CarteraCantidad);

        editTextCarteraNumeroPagare.setFilters(new InputFilter[]{new InputFilter.LengthFilter(15)});
        editText_CarteraCantidad.setFilters(new InputFilter[]{new InputFilter.LengthFilter(15)});

    }


    /**Metodo carteraNumeroParagareContinuar de tipo void, recibo como parametro un View, para poder ser implementado por
     * el button carteraNumeroPagareContinuar. Hace el intent a la activity pantalla confirmacion y pasa como
     * extra todos los parametros ingresador por el usuario para ser confirmados*/

    public void carteraNumeroPagareContinuar(View view){

        String numeroPagare_string = editTextCarteraNumeroPagare.getText().toString();
        String cantidad_String = editText_CarteraCantidad.getText().toString();
        if(numeroPagare_string.isEmpty()){
            Toast.makeText(getApplicationContext(),"Debe ingresar el número del pagaré",Toast.LENGTH_SHORT).show();
        } else if (cantidad_String.isEmpty()) {
            Toast.makeText(getApplicationContext(),"Debe ingresar la cantidad",Toast.LENGTH_SHORT).show();
        } else {

            valores.add(origen);
            valores.add(numeroPagare_string);
            valores.add(cantidad_String);

            iconos.add(3);
            iconos.add(1);

            //Se realiza el intent a la activity confirmar valores
            Intent i = new Intent(getApplicationContext(), pantallaConfirmacion.class);

            i.putExtra("titulo", "<b>Pago productos banco</b>");
            i.putExtra("descripcion", "Por favor confirme los datos del pagaré");
            i.putExtra("titulos", titulos);
            i.putExtra("valores", valores);
            i.putExtra("terminos", false);
            i.putExtra("clase", "");
            i.putExtra("contador", 1);
            i.putExtra("iconos",iconos);
            i.putExtra("transaccion", codigo.CORRESPONSAL_PAGO_PRODUCTOS);
            startActivity(i);
            overridePendingTransition(R.anim.slide_in_right, R.anim.slide_out_left);

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