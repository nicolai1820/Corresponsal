package co.com.netcom.corresponsal.pantallas.funciones;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.comunes.logIn.LogIn;

public class BaseActivity extends AppCompatActivity implements TimeOutSesion.TimeOutSesionListener {
    @Override
    protected void onStart() {
        super.onStart();
        //TimeOutSesion.startLogoutTimer(this, this);
        Log.e("TAG", "OnStart () &&& Starting timer");
    }

    @Override
    public void onUserInteraction() {
        super.onUserInteraction();
        //TimeOutSesion.startLogoutTimer(this, this);
        Log.e("TAG", "User interacting with screen");
    }


    @Override
    protected void onPause() {
        super.onPause();
        Log.e("TAG", "onPause()");
    }

    @Override
    protected void onResume() {
        super.onResume();

        Log.e("TAG", "onResume()");
    }

    /**
     * Performing idle time logout
     */
    @Override
    public void doLogout() {
        // write your stuff here
        Intent i = new Intent(this, LogIn.class);
        i.putExtra("sesion",1);
        startActivity(i);
        overridePendingTransition(R.anim.slide_in_left, R.anim.slide_out_right);
    }





}