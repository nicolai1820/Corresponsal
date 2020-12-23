package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.inicio;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;


import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.util.Log;
import android.view.MenuItem;

import androidx.fragment.app.Fragment;

import com.google.android.material.bottomnavigation.BottomNavigationView;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.core.comunicacion.IntegradorC;
import co.com.netcom.corresponsal.core.comunicacion.ParametrosC;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.inicio.ajustes.pantallaAjustesUsuarioComun;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.inicio.cupoTarjeta.pantallaCupo;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.inicio.informacion.pantallaInformacionUsuarioComun;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.inicio.servicios.pantallaServiciosUsuarioComun;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.inicio.transacciones.pantallaTransacciones;
import co.com.netcom.corresponsal.pantallas.funciones.MetodosSDKNewland;

public class pantallaInicialUsuarioComun extends AppCompatActivity {


    private IntegradorC integradorC;
    private BottomNavigationView menuUser;
    private String terminalInicializado;
    private MetodosSDKNewland metodosSDKNewland;
    private String direcciónAutoconexion;
    private int fragmento;
    @Override
    protected void onCreate(Bundle savedInstanceState) {


        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_inicial_usuario_comun);

        //Se realiza la conexion con la interfaz grafica
        menuUser = (BottomNavigationView) findViewById(R.id.menuUser);

        Bundle i = getIntent().getExtras();
        try {
             fragmento = i.getInt("Fragmento",0);
        }catch (Exception e){}

        Log.d("FRAGMENTO",String.valueOf(fragmento));
        if (fragmento==1){
            //Se inicializa el fragment con la pantalla de servicios
            getSupportFragmentManager().beginTransaction().replace(R.id.contenedorFragmentUser, new pantallaAjustesUsuarioComun()).commit();

        }else{
            //Se inicializa el fragment con la pantalla de servicios
            getSupportFragmentManager().beginTransaction().replace(R.id.contenedorFragmentUser, new pantallaServiciosUsuarioComun()).commit();

        }


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
                    case R.id.cupoTarjeta:
                        selectedFragment = new pantallaCupo();
                        break;

                    case R.id.transacciones_menu:
                        selectedFragment = new pantallaTransacciones();
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


        //Se intenta conectar al ultimo dispositivo al cual se conecto
        metodosSDKNewland = new MetodosSDKNewland(pantallaInicialUsuarioComun.this);

        SharedPreferences sharedPreferences = getSharedPreferences("dispositivos", Context.MODE_PRIVATE);
        direcciónAutoconexion = sharedPreferences.getString("DireccionUltimaConexion",null);

        if (direcciónAutoconexion!=null && !metodosSDKNewland.isConnected()){
            new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        metodosSDKNewland.connectDevice(60,direcciónAutoconexion);
                    }catch (Exception e){ }

                }
            }).start();
        }


        //Se crea el objeto de c y se cargan los archivos que permiten acceder  a la conexión de los servicios
        integradorC = new IntegradorC(this);

        String path = this.getFilesDir().getAbsolutePath();
        integradorC.crearRutaArchivo(path);
        IntegradorC.inicializarJava();
        IntegradorC.copyFiletoExternalStorage(R.raw.caroot, "caroot.pem", path + "/", pantallaInicialUsuarioComun.this);
        IntegradorC.copyFiletoExternalStorage(R.raw.caroot, "caroot.pem", path + "/", pantallaInicialUsuarioComun.this);
        integradorC.copyFiletoExternalStorage(R.raw.ticketbcl, "TICKETBCL.TPL", path + "/", pantallaInicialUsuarioComun.this);
        integradorC.copyFiletoExternalStorage(R.raw.tbcldecli, "TBCLDECLI.TPL", path + "/", pantallaInicialUsuarioComun.this);
        integradorC.copyFiletoExternalStorage(R.raw.comsgiro, "COMSGIRO.TXT", path + "/", pantallaInicialUsuarioComun.this);
        integradorC.copyFiletoExternalStorage(R.raw.tdoc, "TDOC.TXT", path + "/", pantallaInicialUsuarioComun.this);
        integradorC.copyFiletoExternalStorage(R.raw.ticket, "TICKET.TPL", path + "/", pantallaInicialUsuarioComun.this);
        integradorC.copyFiletoExternalStorage(R.raw.tdelect, "TDELECT.TPL", path + "/", pantallaInicialUsuarioComun.this);
        integradorC.copyFiletoExternalStorage(R.raw.lealtad, "LEALTAD.TPL", path + "/", pantallaInicialUsuarioComun.this);
        integradorC.copyFiletoExternalStorage(R.raw.urlf, "URLF.TXT", path + "/", pantallaInicialUsuarioComun.this);
        integradorC.copyFiletoExternalStorage(R.raw.reverso, "REVERSO.TPL", path + "/", pantallaInicialUsuarioComun.this);

        terminalInicializado = integradorC.obtenerParametrosC(ParametrosC.TERMINAL_INICIALIZADA);
        Log.d("Case 2", "terminalInicializado " + terminalInicializado);
        if (terminalInicializado.equals("2")) {
            Log.d("Case 2", "Error en la inicializacion ");
            integradorC.restaurarInicializacion();
        }


    }

    /**Metodo nativo de android onBackPressed. Se sobreescribe para que el usuario no se pueda desplazar hacia atras
     * por medio del menu del celular.*/

    @Override
    public void onBackPressed() {

    }


}