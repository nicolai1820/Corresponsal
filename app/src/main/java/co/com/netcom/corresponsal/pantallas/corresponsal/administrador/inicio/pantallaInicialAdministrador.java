package co.com.netcom.corresponsal.pantallas.corresponsal.administrador.inicio;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.Fragment;

import androidx.fragment.app.FragmentTransaction;
import androidx.fragment.app.FragmentManager;

import android.os.Bundle;
import android.view.MenuItem;

import com.google.android.material.bottomnavigation.BottomNavigationView;

import co.com.netcom.corresponsal.R;

public class pantallaInicialAdministrador extends AppCompatActivity {

    private BottomNavigationView menuAdmin;
    long animationDuration = 300;








    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_inicial_administrador);


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
                    case R.id.settings_admin:
                        selectedFragment = new pantallaAjustesAdministrador();
                        break;
                }

                getSupportFragmentManager().beginTransaction().replace(R.id.contenedorFragmentAdmin, selectedFragment).commit();


                return true;
            }
        });



    }

   /* public void mostrarMenu(View v) {

        ObjectAnimator animatorup = ObjectAnimator.ofFloat(menu, "y", 1500f);


        animatorup.setDuration(animationDuration);

        AnimatorSet animatorSetUp = new AnimatorSet();


        animatorSetUp.playTogether(animatorup);
        animatorSetUp.start();
    }*/


    /*private BottomNavigationView.OnNavigationItemSelectedListener  navListener =

            new BottomNavigationView.OnNavigationItemSelectedListener() {
                @Override
                public boolean onNavigationItemSelected(@NonNull MenuItem menuItem) {
                    Fragment selectedFragment = null;


                    switch (menuItem.getItemId()){
                        case R.id.home_icon:
                            selectedFragment = new servicios();


                            break;
                        case R.id.settings_icon:
                            Intent y = new Intent(getApplicationContext(),PantallaInicialUsuario.class);
                            startActivity(y);
                            break;
                    }

                    getSupportFragmentManager().beginTransaction().replace(R.id.contenedorFragment,selectedFragment).commit();

                    return true;
                }
            };*/
}