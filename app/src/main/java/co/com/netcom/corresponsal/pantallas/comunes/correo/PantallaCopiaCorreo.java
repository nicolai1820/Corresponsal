package co.com.netcom.corresponsal.pantallas.comunes.correo;

import androidx.appcompat.app.AppCompatActivity;

import android.app.AlertDialog;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.comunes.pantallaConfirmacion.pantallaConfirmacion;
import co.com.netcom.corresponsal.pantallas.comunes.popUp.PopUp;
import co.com.netcom.corresponsal.pantallas.comunes.resultadoTransaccion.PantallaResultadoTransaccionExitosa;
import co.com.netcom.corresponsal.pantallas.funciones.BaseActivity;
import co.com.netcom.corresponsal.pantallas.funciones.CodificarBase64;
import co.com.netcom.corresponsal.pantallas.funciones.Servicios;

public class PantallaCopiaCorreo extends BaseActivity {

    private Header header = new Header("<b>Resultado transacción</b>");
    private String idTransaccion;
    private int transaccion;
    private EditText editTextCopiaCorreo;
    private CodificarBase64 base64 = new CodificarBase64();
    private AlertDialog dialog;
    public static Handler resp;
    private Map<String,String > respuestaCorreo;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_copia_correo);

        //Recuperar información transaccion : ID de la transacción
        Bundle i = getIntent().getExtras();
        idTransaccion = i.getString("aprovalCode");
        transaccion = i.getInt("transaccion");
        Log.d("idTransaccion",idTransaccion);
        Log.d("transaccion",String.valueOf(transaccion));
        Log.d("CopiaCorreo","copia");


        //Se hace la conexión con el FrontEnd
        editTextCopiaCorreo = findViewById(R.id.editText_PantallaCopiaCorreo);

        //Se carga el Header de la vista
        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderPantallaCopiaCorreo,header).commit();
         resp = new Handler(){
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what) {
                    case 1:
                        dialog.dismiss();
                        PopUp pop = new PopUp(PantallaCopiaCorreo.this);

                        if(respuestaCorreo.get("responseCode").equals("MQ==")){
                            pop.crearPopUpResultadoCorreoCLiente("Correo enviado exitosamente al cliente",transaccion,base64.decodificarBase64(idTransaccion));
                        }else{
                            pop.crearPopUpResultadoCorreoCLiente("Hubo un error en el envio del correo al cliente",transaccion,base64.decodificarBase64(idTransaccion));

                        }

                }}};

    }

    public void enviarCopiaCorreo(View view){

        String correoCliente = editTextCopiaCorreo.getText().toString();

        if(correoCliente.isEmpty()){
            Toast.makeText(this,"Debe ingresar un correo",Toast.LENGTH_SHORT).show();
        }else  if(!isEmailValid(correoCliente)){
            Toast.makeText(this,"Debe ingresar un correo valido",Toast.LENGTH_SHORT).show();
        }else{
            AlertDialog.Builder loader = new AlertDialog.Builder(PantallaCopiaCorreo.this);

            LayoutInflater inflater = this.getLayoutInflater();

            loader.setView(inflater.inflate(R.layout.loader_procesando_transaccion,null));
            loader.setCancelable(false);

            dialog = loader.create();
            dialog.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));

            Log.d("OPEN"," se abrio el loader");


            dialog.show();

            //Servicio Correo.
            Servicios servicio = new Servicios(this);
            new Thread(new Runnable() {
                @Override
                public void run() {
                    respuestaCorreo =servicio.envioCorreo(base64.decodificarBase64(idTransaccion),correoCliente,transaccion);

                    try{
                        Message usuarioCancela = new Message();
                        usuarioCancela.what = 1;
                        resp.sendMessage(usuarioCancela);
                    }catch(Exception e){

                    }
                }
            }).start();
        }

    }

    public void cancelarEnvioCorreo(View view){
        PopUp pop = new PopUp(PantallaCopiaCorreo.this);
        pop.crearPopUpConfirmarEnvioCorreo(transaccion,idTransaccion);
    }


    public static boolean isEmailValid(String email) {
        String expression = "^[\\w\\.-]+@([\\w\\-]+\\.)+[A-Z]{2,4}$";
        Pattern pattern = Pattern.compile(expression, Pattern.CASE_INSENSITIVE);
        Matcher matcher = pattern.matcher(email);
        return matcher.matches();
    }


    @Override
    public void onBackPressed() {
    }
}
