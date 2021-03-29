package co.com.netcom.corresponsal.pantallas.comunes.tipoDeUsuario;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.corresponsal.administrador.inicio.pantallaInicialAdministrador;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.inicio.pantallaInicialUsuarioComun;

public class pantallaTipoDeUsuario extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_tipo_de_usuario);
    }

    public void buttonAdministrador(View view){

        Intent i = new Intent(this, pantallaInicialAdministrador.class);
        startActivity(i);
        overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);

    }

    public void buttonOperador(View view){
        Intent i = new Intent(this, pantallaInicialUsuarioComun.class);
        startActivity(i);
        overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);
    }

    @Override
    public void onBackPressed() {

    }
}