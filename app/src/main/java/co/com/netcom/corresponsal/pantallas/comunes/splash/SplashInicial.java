package co.com.netcom.corresponsal.pantallas.comunes.splash;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.content.Intent;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;

import com.ingenico.lar.bc.Pinpad;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.core.comunicacion.IntegradorC;
import co.com.netcom.corresponsal.core.comunicacion.ParametrosC;
import co.com.netcom.corresponsal.pantallas.comunes.logIn.LogIn;

public class SplashInicial extends AppCompatActivity {
    private Intent a;
    private IntegradorC integradorC;
    private String terminalInicializado;
    private String tipoTelecarga;
    private Pinpad pinpad = null;
    private String tipoComunicacion;
    WifiManager adminWifi;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_splash);

        integradorC = new IntegradorC(SplashInicial.this);
        adminWifi= (WifiManager) this.getApplicationContext().getSystemService(Context.WIFI_SERVICE);
        tipoComunicacion = integradorC.obtenerParametrosC(ParametrosC.TIPO_COMUNICACION);
        if(tipoComunicacion.equals("4")){
            adminWifi.setWifiEnabled(false);
        }
        String path = this.getFilesDir().getAbsolutePath();
        Log.d("PATH",path);
        integradorC.crearRutaArchivo(path);
        IntegradorC.inicializarJava();
        IntegradorC.copyFiletoExternalStorage(R.raw.caroot, "caroot.pem", path + "/", SplashInicial.this);
        integradorC.copyFiletoExternalStorage(R.raw.ticketbcl, "TICKETBCL.TPL", path + "/", SplashInicial.this);
        integradorC.copyFiletoExternalStorage(R.raw.tbcldecli, "TBCLDECLI.TPL", path + "/", SplashInicial.this);
        integradorC.copyFiletoExternalStorage(R.raw.comsgiro, "COMSGIRO.TXT", path + "/", SplashInicial.this);
        integradorC.copyFiletoExternalStorage(R.raw.tdoc, "TDOC.TXT", path + "/", SplashInicial.this);
        integradorC.copyFiletoExternalStorage(R.raw.ticket, "TICKET.TPL", path + "/", SplashInicial.this);
        integradorC.copyFiletoExternalStorage(R.raw.tdelect, "TDELECT.TPL", path + "/", SplashInicial.this);
        integradorC.copyFiletoExternalStorage(R.raw.lealtad, "LEALTAD.TPL", path + "/", SplashInicial.this);
        integradorC.copyFiletoExternalStorage(R.raw.urlf, "URLF.TXT", path + "/", SplashInicial.this);
        integradorC.copyFiletoExternalStorage(R.raw.reverso, "REVERSO.TPL", path + "/", SplashInicial.this);

        a= new Intent(this, LogIn.class);
        new Handler().postDelayed(
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        startActivity(a);
                        overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);
                    }
                }),3000
        );




    }
}