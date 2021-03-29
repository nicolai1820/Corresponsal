package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.retiro.conTarjeta;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.Toast;

import java.util.ArrayList;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.core.comunicacion.CardDTO;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.comunes.pantallaConfirmacion.pantallaConfirmacion;
import co.com.netcom.corresponsal.pantallas.funciones.BaseActivity;

public class PantallaRetiroConTarjetaTipoDeCuenta extends BaseActivity {

    private Header header = new Header("<b>Retiro Con Tarjeta</b>");
    private Spinner spinner_tipoCuentaRetiroConTarjeta;
    private ArrayList<String> valores = new ArrayList<String>();
    private ArrayList<String> valoresRespaldo = new ArrayList<String>();
    private ArrayList<String> tipoDeCuenta = new ArrayList<String>();
    private String tipoCuenta="0";
    private String [] titulos={"Tipo de cuenta"};
    private String [] titulosOtraCuenta={"Tipo de cuenta","Número de cuenta"};
    private ArrayList<Integer> iconos = new ArrayList<Integer>();
    private EditText editText_pantallaRetiroConTarjetaOtraCuenta;
    private int contador;
    private int contadorRespaldo;
    private CardDTO tarjeta;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_retiro_con_tarjeta_tipo_de_cuenta);

        //Se crea el header de la actividad con el titulo correspondiente.
        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderPantallaRetiroConTarjetaTipoDeCuenta,header).commit();

        //Se hace la conexión con la interfaz gráfica
        editText_pantallaRetiroConTarjetaOtraCuenta = (EditText) findViewById(R.id.editText_pantallaRetiroConTarjetaOtraCuenta);
        spinner_tipoCuentaRetiroConTarjeta = findViewById(R.id.spinner_tipoCuentaRetiroConTarjeta);


        //Se agregan las opciones al spinner
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

        spinner_tipoCuentaRetiroConTarjeta.setAdapter(adapter);

        //Se habilita y deshabilita el campo numero otra cuenta dependiendo de la seleccion del spinner
        spinner_tipoCuentaRetiroConTarjeta.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                if (position==4){
                    editText_pantallaRetiroConTarjetaOtraCuenta.setVisibility(View.VISIBLE);
                } else {
                    editText_pantallaRetiroConTarjetaOtraCuenta.setVisibility(View.INVISIBLE);

                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }
        });

        //Se recuperan los valores de la vista anterior junto con el contador para recorrerlos
        Bundle extras = getIntent().getExtras();
        valores = extras.getStringArrayList("valores");
        contador = extras.getInt("contador");
        tarjeta = new CardDTO();
        tarjeta = extras.getParcelable("tarjeta");

        //Se capturan los datos de respaldo
        for(int i =0; i< valores.size();i++){
            valoresRespaldo.add(valores.get(i));
        }

        contadorRespaldo = contador;


    }

    public void continuarTipoDeCuentaRetiroConTarjeta(View view){

        String otraCuenta = editText_pantallaRetiroConTarjetaOtraCuenta.getText().toString();
        String seleccionCuenta = spinner_tipoCuentaRetiroConTarjeta.getSelectedItem().toString();

        if (seleccionCuenta.equals("Tipo de cuenta")){
            Toast.makeText(getApplicationContext(),"Debe seleccionar un tipo de cuenta",Toast.LENGTH_SHORT).show();
        }else if(otraCuenta.isEmpty()&&seleccionCuenta.equalsIgnoreCase("otra cuenta")){
            Toast.makeText(getApplicationContext(),"Debe ingresar el número de cuenta", Toast.LENGTH_SHORT).show();
        }else{
            if(seleccionCuenta.equalsIgnoreCase("otra cuenta")){

                valores.add(seleccionCuenta);
                valores.add(otraCuenta);
                tipoDeCuenta.add(tipoCuenta);

                iconos.add(2);
                iconos.add(3);
                //Se realiza el intent a la activity confirmar valores
                Intent i = new Intent(getApplicationContext(), pantallaConfirmacion.class);

                i.putExtra("titulo","<b>Retiro Con Tarjeta</b>");
                i.putExtra("descripcion","Por favor confirme el tipo y número de cuenta.");
                i.putExtra("titulos",titulosOtraCuenta);
                i.putExtra("valores",valores);
                i.putExtra("terminos",false);
                i.putExtra("clase","");
                i.putExtra("contador", contador);
                i.putExtra("transaccion", "");
                i.putExtra("tarjeta",tarjeta);
                i.putExtra("iconos",iconos);
                startActivity(i);
                overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);

            }else{

                if(seleccionCuenta.equals("Ahorros")){
                    tipoCuenta = "10";
                }else if (seleccionCuenta.equalsIgnoreCase("Corriente")) {
                    tipoCuenta = "20";
                } else {
                    tipoCuenta ="30";
                }

                valores.add(seleccionCuenta);
                tipoDeCuenta.add(tipoCuenta);

                iconos.add(2);

                //Se realiza el intent a la activity confirmar valores
                Intent i = new Intent(getApplicationContext(), pantallaConfirmacion.class);

                i.putExtra("titulo","<b>Retiro Con Tarjeta</b>");
                i.putExtra("descripcion","Por favor confirme el tipo de cuenta.");
                i.putExtra("titulos",titulos);
                i.putExtra("valores",valores);
                i.putExtra("terminos",false);
                i.putExtra("clase","");
                i.putExtra("contador", contador);
                i.putExtra("iconos",iconos);
                i.putExtra("tipoDeCuenta", tipoDeCuenta);
                i.putExtra("transaccion", "");
                i.putExtra("tarjeta",tarjeta);
                startActivity(i);
                overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);
            }

        }
    }


    /** Se sobreescribe el metodo nativo onBackPressed para que el usuario no pueda retroceder dentro de la aplicación*/
    @Override
    public void onBackPressed() {

    }

    /**Metodo nativo de android onRestart(). Se sobreescribe este metodo para eliminar los datos que el usuario
     * digito de forma erronea.*/

    @Override
    public void onRestart(){

        valores.clear();
        tipoDeCuenta.clear();
        contador =0;

        for(int i =0; i< valoresRespaldo.size();i++){
            this.valores.add(valoresRespaldo.get(i));
        }

        this.contador =contadorRespaldo;

        super.onRestart();
    }


}