package co.com.netcom.corresponsal.pantallas.comunes.correo;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;

public class PantallaCopiaCorreo extends AppCompatActivity {

    private Header header = new Header("<b>Resultado transacción</b>");
    private String idTransaccion;
    private EditText editTextCopiaCorreo;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_copia_correo);

        //Recuperar información transaccion : ID de la transacción
        Bundle i = getIntent().getExtras();
        idTransaccion = i.getString("aprovalCode");

        //Se hace la conexión con el FrontEnd
        editTextCopiaCorreo = findViewById(R.id.editText_PantallaCopiaCorreo);

        //Se carga el Header de la vista
        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderPantallaCopiaCorreo,header).commit();

    }

    public void enviarCopiaCorreo(View view){

        String correoCliente = editTextCopiaCorreo.getText().toString();

        if(correoCliente.isEmpty()){
            Toast.makeText(this,"Debe ingresar un correo",Toast.LENGTH_SHORT).show();
        }else  if(!isEmailValid(correoCliente)){
            Toast.makeText(this,"Debe ingresar un correo valido",Toast.LENGTH_SHORT).show();
        }else{
            //Servicio Correo.
        }

    }


    public static boolean isEmailValid(String email) {
        String expression = "^[\\w\\.-]+@([\\w\\-]+\\.)+[A-Z]{2,4}$";
        Pattern pattern = Pattern.compile(expression, Pattern.CASE_INSENSITIVE);
        Matcher matcher = pattern.matcher(email);
        return matcher.matches();
    }


}