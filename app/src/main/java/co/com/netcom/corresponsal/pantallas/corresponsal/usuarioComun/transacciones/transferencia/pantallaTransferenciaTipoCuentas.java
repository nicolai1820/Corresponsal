package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.transferencia;

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
import java.util.Arrays;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.core.comunicacion.CardDTO;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.comunes.pantallaConfirmacion.pantallaConfirmacion;
import co.com.netcom.corresponsal.pantallas.funciones.BaseActivity;
import co.com.netcom.corresponsal.pantallas.funciones.CodigosTransacciones;

public class pantallaTransferenciaTipoCuentas extends BaseActivity {

    private Header header;
    private CardDTO tarjeta;
    private ArrayList<String> valores = new ArrayList<String>();
    private ArrayList<String> valoresRespaldo = new ArrayList<String>();
    private ArrayList<String> tipoDeCuenta = new ArrayList<String>();
    private ArrayList<String> tipoDeCuentaRespaldo = new ArrayList<String>();
    private int contador;
    private int contadorRespaldo;
    private String [] titulos={"Tipo de cuenta origen"};
    private String [] titulosOtraCuenta={"Tipo de cuenta origen","Número cuenta origen"};
    private ArrayList<Integer> iconos = new ArrayList<Integer>();
    private Spinner spinner_tipoCuentaOrigenTransferencia;
    private EditText editText_PantallaTransferenciaTipoCuentasOtraCuenta;
    private String tipoCuentaOrigen ="0";

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
        editText_PantallaTransferenciaTipoCuentasOtraCuenta = findViewById(R.id.editText_PantallaTransferenciaTipoCuentasOtraCuenta);


        //Se agregan las opciones al spinner de cuenta origen
        ArrayAdapter<String> adapter = new ArrayAdapter<String>(getApplication(), R.layout.spinner_elements_style, getResources().getStringArray(R.array.array_TiposDeCuenta)){

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

        //Se habilita y deshabilita el campo numero otra cuenta dependiendo de la seleccion del spinner
        spinner_tipoCuentaOrigenTransferencia.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                if (position==3){
                    editText_PantallaTransferenciaTipoCuentasOtraCuenta.setVisibility(View.VISIBLE);
                } else {
                    editText_PantallaTransferenciaTipoCuentasOtraCuenta.setVisibility(View.INVISIBLE);

                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }
        });

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

    /**Metodo de tipo void, que recibe como parametro un elemento de tipo View, para poder ser implementado por el boton
     * Continuar Consulta Saldo. Se encarga de realizar el correspondiente intent con los extras correspondientes a la clase
     * pantallaConfirmación*/

    public void continuarTipoCuentaTransferencia(View view){

        String seleccionCuentaOrigen = spinner_tipoCuentaOrigenTransferencia.getSelectedItem().toString();
        String numeroOtraCuenta = editText_PantallaTransferenciaTipoCuentasOtraCuenta.getText().toString();


        //Se verifica que los campos no esten vacios
        if (seleccionCuentaOrigen.equalsIgnoreCase("Tipo de cuenta")) {
            Toast.makeText(getApplicationContext(), "Debe seleccionar el tipo de cuenta de origen", Toast.LENGTH_SHORT).show();
        }else if (seleccionCuentaOrigen.equalsIgnoreCase("otra cuenta")&& numeroOtraCuenta.isEmpty()){
            Toast.makeText(getApplicationContext(), "Debe ingresar el número de cuenta de origen", Toast.LENGTH_SHORT).show();

        }
        else{
            if (seleccionCuentaOrigen.equalsIgnoreCase("otra cuenta")){

                valores.add(seleccionCuentaOrigen);
                valores.add(numeroOtraCuenta);
                tipoDeCuenta.add(tipoCuentaOrigen);

                iconos.add(2);
                iconos.add(3);

                //Se realiza el intent a la activity confirmar valores
                Intent i = new Intent(getApplicationContext(), pantallaConfirmacion.class);

                i.putExtra("titulo","<b>Transferencia</b>");
                i.putExtra("descripcion","Por favor confirme el tipo y número de cuenta origen");
                i.putExtra("titulos",titulosOtraCuenta);
                i.putExtra("valores",valores);
                i.putExtra("terminos",false);
                i.putExtra("clase","");
                i.putExtra("contador", contador);
                i.putExtra("tipoDeCuenta", tipoDeCuenta);
                i.putExtra("transaccion", "");
                i.putExtra("iconos",iconos);
                i.putExtra("tarjeta",tarjeta);
                startActivity(i);
                overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);
            } else{
                if(seleccionCuentaOrigen.equals("Ahorros")){
                    tipoCuentaOrigen = "10";
                }else{
                    tipoCuentaOrigen = "20";
                }


                valores.add(seleccionCuentaOrigen);
                tipoDeCuenta.add(tipoCuentaOrigen);

                iconos.add(2);
                //Se realiza el intent a la activity confirmar valores
                Intent i = new Intent(getApplicationContext(), pantallaConfirmacion.class);

                i.putExtra("titulo","<b>Transferencia</b>");
                i.putExtra("descripcion","Por favor confirme el tipo de cuenta origen");
                i.putExtra("titulos",titulos);
                i.putExtra("valores",valores);
                i.putExtra("terminos",false);
                i.putExtra("clase","");
                i.putExtra("contador", contador);
                i.putExtra("tipoDeCuenta", tipoDeCuenta);
                i.putExtra("transaccion", "");
                i.putExtra("iconos",iconos);
                i.putExtra("tarjeta",tarjeta);
                startActivity(i);
                overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);
            }

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

        for(int i =0; i< valoresRespaldo.size();i++){
            this.valores.add(valoresRespaldo.get(i));
        }

        for(int i =0; i< tipoDeCuentaRespaldo.size();i++){
            this.tipoDeCuenta.add(tipoDeCuentaRespaldo.get(i));
        }

        this.contador =contadorRespaldo;
        super.onRestart();
    }
}