package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.inicio;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;


import android.os.Bundle;
import android.view.MenuItem;

import androidx.fragment.app.Fragment;

import com.google.android.material.bottomnavigation.BottomNavigationView;

import co.com.netcom.corresponsal.R;

public class pantallaInicialUsuarioComun extends AppCompatActivity {



    private BottomNavigationView menuUser;

    @Override
    protected void onCreate(Bundle savedInstanceState) {


        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_inicial_usuario_comun);

        //Se realiza la conexion con la interfaz grafica
        menuUser = (BottomNavigationView) findViewById(R.id.menuUser);

        //Se inicializa el fragment con la pantalla de servicios
        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorFragmentUser, new pantallaServiciosUsuarioComun()).commit();

        //Se crea el evento click de los elementos de la barra de navegacion
        menuUser.setOnNavigationItemSelectedListener(new BottomNavigationView.OnNavigationItemSelectedListener() {
            @Override
            public boolean onNavigationItemSelected(@NonNull MenuItem menuItem) {


                Fragment selectedFragment= null;

                switch (menuItem.getItemId()){
                    case R.id.home_user:
                        selectedFragment = new pantallaServiciosUsuarioComun();
                        break;
                    case R.id.information_user:
                        selectedFragment = new pantallaInformacionUsuarioComun();

                        break;

                    case R.id.settings_user:
                        selectedFragment = new pantallaAjustesUsuarioComun();

                        break;

                    default:

                }

                //   getSupportFragmentManager().beginTransaction().replace(R.id.contenedorFragment,selectedFragment).commit();
                //   transaction.replace(R.id.contenedorFragment, selectedFragment);
                //   transaction.addToBackStack(null);

                //Se redirige al fragmento correspondiente, dependiendo de la la seleccion del usuario
                getSupportFragmentManager().beginTransaction().replace(R.id.contenedorFragmentUser, selectedFragment).commit();


                return true;
            }
        });
    }

    /**Metodo nativo de android onBackPressed. Se sobreescribe para que el usuario no se pueda desplazar hacia atras
     * por medio del menu del celular.*/

    @Override
    public void onBackPressed() {

    }


}