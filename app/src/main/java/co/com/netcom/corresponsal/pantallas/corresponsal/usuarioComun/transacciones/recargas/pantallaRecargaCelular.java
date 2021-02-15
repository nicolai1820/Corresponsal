package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.recargas;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.icu.text.UnicodeSetSpanner;
import android.os.Bundle;
import android.view.MenuItem;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

import com.google.android.material.bottomnavigation.BottomNavigationView;

import java.util.ArrayList;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.comunes.pantallaConfirmacion.pantallaConfirmacion;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.inicio.pantallaInicialUsuarioComun;
import co.com.netcom.corresponsal.pantallas.funciones.BaseActivity;
import co.com.netcom.corresponsal.pantallas.funciones.CodigosTransacciones;

public class pantallaRecargaCelular extends BaseActivity {

    private Header header;
    private EditText celular;
    private String tipoCuenta = "NEQUI";
    private String convenioNequi = "63703";
    private String [] titulos={"Número de Celular"};
    private ArrayList<Integer> iconos = new ArrayList<Integer>();
    private ArrayList<String> valores = new ArrayList<String>();
    private ArrayList<String> tipoDeCuenta = new ArrayList<String>();
    private CodigosTransacciones codigo = new CodigosTransacciones();
    private BottomNavigationView menuRecargas;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_recarga_celular);

        header= new Header("<b>Recargas</b>");

        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderPantallaRecargaCelular,header).commit();

        celular = (EditText) findViewById(R.id.editText_RecargasCelular);
        menuRecargas = (BottomNavigationView) findViewById(R.id.menuRecargas);

        //Se crea el evento click de la barra de navegacion

        menuRecargas.setOnNavigationItemSelectedListener(new BottomNavigationView.OnNavigationItemSelectedListener() {
            @Override
            public boolean onNavigationItemSelected(@NonNull MenuItem menuItem) {

                if(menuItem.getItemId() == R.id.home_general){
                    Intent i = new Intent(getApplicationContext(), pantallaInicialUsuarioComun.class);
                    startActivity(i);
                    overridePendingTransition(R.anim.slide_in_left,R.anim.slide_out_right);

                }

                return true;
            }
        });
    }


    public void recargasCelularContinuar(View view){

        String celular_String = celular.getText().toString();

        if(celular_String.isEmpty()){
            Toast.makeText(this,"Debe ingresar el número",Toast.LENGTH_SHORT).show();
        }else {

            valores.add(convenioNequi);
            valores.add(celular_String);

            tipoDeCuenta.add(tipoCuenta);


            iconos.add(3);
            //Se realiza el intent a la activity confirmar valores
            Intent i = new Intent(getApplicationContext(), pantallaConfirmacion.class);

            i.putExtra("titulo","<b>Recargas</b>");
            i.putExtra("descripcion","Por favor confirme el número de celular.");
            i.putExtra("titulos",titulos);
            i.putExtra("valores",valores);
            i.putExtra("terminos",false);
            i.putExtra("clase","");
            i.putExtra("contador", 1);
            i.putExtra("iconos",iconos);
            i.putExtra("tipoDeCuenta",tipoDeCuenta);
            i.putExtra("transaccion", codigo.CORRESPONSAL_CONSULTA_FACTURAS);
            startActivity(i);
            overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);
        }
    }

    @Override
    public void onBackPressed() {

    }
}