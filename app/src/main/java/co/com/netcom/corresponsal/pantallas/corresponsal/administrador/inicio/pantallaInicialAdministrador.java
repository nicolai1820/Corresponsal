package co.com.netcom.corresponsal.pantallas.corresponsal.administrador.inicio;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.Fragment;

import android.os.Bundle;
import android.view.MenuItem;

import com.google.android.material.bottomnavigation.BottomNavigationView;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.inicio.cupoTarjeta.pantallaCupo;

public class pantallaInicialAdministrador extends AppCompatActivity {

    private BottomNavigationView menuAdmin;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_inicial_administrador);

/*
        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorFragmentAdmin, new pantallaServiciosAdministrador()).commit();

       menuAdmin = (BottomNavigationView) findViewById(R.id.menuAdmin);
        menuAdmin.setOnNavigationItemSelectedListener(new BottomNavigationView.OnNavigationItemSelectedListener() {
            @Override
            public boolean onNavigationItemSelected(@NonNull MenuItem menuItem) {

                Fragment selectedFragment = null;


                switch (menuItem.getItemId()){
                    case R.id.home_admin:
                        selectedFragment = new pantallaServiciosAdministrador();
                        break;

                    case R.id.usuarios_admin:
                        selectedFragment = new pantallaAdministracionUsuarios();
                        break;
                    case R.id.settings_admin:
                        selectedFragment = new pantallaDuplicado();
                        break;

                    case R.id.cupoTarjeta_admin:
                        selectedFragment = new pantallaCupo();
                        break;

                    case R.id.transaction_admin:
                        selectedFragment = new pantallaTransaccionesAdministrador();
                        break;
                }

                getSupportFragmentManager().beginTransaction().replace(R.id.contenedorFragmentAdmin, selectedFragment).commit();


                return true;
            }
        });*/



    }

}