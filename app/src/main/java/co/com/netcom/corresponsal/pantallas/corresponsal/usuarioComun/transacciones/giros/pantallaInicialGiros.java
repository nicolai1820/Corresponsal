package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.giros;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.MenuItem;
import android.view.View;

import com.google.android.material.bottomnavigation.BottomNavigationView;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.giros.cancelar.pantallaCancelarGiroDatosGirador;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.giros.enviar.pantallaEnviarGiroCantidad;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.giros.reclamar.pantallaReclamarGiroDatosBeneficiario;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.inicio.pantallaInicialUsuarioComun;

public class pantallaInicialGiros extends AppCompatActivity {
    private Header header = new Header("<b>Giros</b>");
    private BottomNavigationView menuGiros;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_inicial_giros);

        //Se crea el header de la actividad con su correspondiente titulo
        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderGiros,header).commit();

        //Se hace la conexion con la interfaz grafica
        menuGiros = (BottomNavigationView) findViewById(R.id.menuUserGiros);

        //Se crea el evento click de la barra de navegacion

        menuGiros.setOnNavigationItemSelectedListener(new BottomNavigationView.OnNavigationItemSelectedListener() {
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

    /**Metodo enviarGiro de tipo void, recibo como parametro un View, para poder ser implementado por
     * el button EnviarGiro. Hace el intent a la activity pantalla enviarCantidadGiro */

    public void enviarGiro(View view){

        Intent i = new Intent(getApplicationContext(), pantallaEnviarGiroCantidad.class);
        startActivity(i);
        overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);
    }

    /**Metodo reclamarGiro de tipo void, recibo como parametro un View, para poder ser implementado por
     * el button ReclamarGiro. Hace el intent a la activity pantalla ReclamarGiroDatosBeneficiario*/

    public void reclamarGiro(View view){

        Intent i = new Intent(getApplicationContext(), pantallaReclamarGiroDatosBeneficiario.class);
        startActivity(i);
        overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);
    }

    /**Metodo cancelarGiro de tipo void, recibo como parametro un View, para poder ser implementado por
     * el button CancelarGiro. Hace el intent a la activity pantalla CancelarGiroDatosGirador*/

    public void cancelarGiro(View view){
        Intent i = new Intent(getApplicationContext(), pantallaCancelarGiroDatosGirador.class);
        startActivity(i);
        overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);
    }

    /**Metodo salirGiros de tipo void, recibo como parametro un View, para poder ser implementado por
     * el button salirGiros. Hace el intent a la activity pantalla InicialUsuarioComun*/

    public void salirGiros(View view){
        Intent i = new Intent(getApplicationContext(), pantallaInicialUsuarioComun.class);
        startActivity(i);
        overridePendingTransition(R.anim.slide_in_left,R.anim.slide_out_right);

    }

    /**Metodo nativo de android onBackPressed. Se sobreescribe para que el usuario no se pueda desplazar hacia atras
     * por medio del menu del celular.*/
    @Override
    public void onBackPressed() {

    }

}