package co.com.netcom.corresponsal.pantallas.comunes.logIn;

import androidx.appcompat.app.AppCompatActivity;

import android.app.AlertDialog;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.os.Handler;
import android.text.InputFilter;
import android.text.Spanned;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.Toast;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.funciones.BaseActivity;

public class PantallaCambioContrasena extends BaseActivity {

    private Header header = new Header("<b>Cambio de contraseña</b>");
    private EditText contrasenaAtigua, contrasenaNueva,contrasenaConfirmacion;
    AlertDialog dialog;
    public static Handler respuesta;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_cambio_contrasena);

        //Se crea el header de la vista
        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderPantallaCambioContrasena,header).commit();

        //Se hace la conexión con la interfaz grafica
        contrasenaAtigua = findViewById(R.id.editText_ContrasenaAntigua);
        contrasenaNueva = findViewById(R.id.editText_ContrasenaNueva);
        contrasenaConfirmacion = findViewById(R.id.edittext_ContrasenaConfirmacion);


        //Se crea este filtro para que el usuario no digite espacios en los editText
        InputFilter textFilter = new InputFilter() {

            @Override

            public CharSequence filter (CharSequence source, int start, int end, Spanned dest, int dstart, int dend){

                StringBuilder sbText = new StringBuilder(source);
                String text = sbText.toString();

                if (text.contains(" ")) {

                    return text.substring(start,end-1).toString();
                }
                return source;
            }
        };

        //Se agregan los filtros para que el usuario no pueda digitar espacios y que la longitud maxima sea de 15 caracteres.
        contrasenaAtigua.setFilters(new InputFilter[]{textFilter,new InputFilter.LengthFilter(15)});
        contrasenaNueva.setFilters(new InputFilter[]{textFilter,new InputFilter.LengthFilter(15)});
        contrasenaConfirmacion.setFilters(new InputFilter[]{textFilter,new InputFilter.LengthFilter(15)});

    }

    private boolean validarContrasena(String contrasena,String confirmacion){
        if(contrasena.isEmpty()){
            Toast.makeText(this,"Debe ingresar una contraeña", Toast.LENGTH_SHORT).show();
            return false;
        }else if(confirmacion.isEmpty()){
            Toast.makeText(this,"Debe confirmar la contraseña", Toast.LENGTH_SHORT).show();
            return false;
        }else if (!contrasena.equals(confirmacion)){
            Toast.makeText(this,"La contraseña debe coincidir", Toast.LENGTH_SHORT).show();
            return false;
        }else {

            if(contrasena.length()>=8){
                //Verifica que contenga un número
                Pattern pNumeros = Pattern.compile("[0-9]");
                Matcher mNumeros = pNumeros.matcher(contrasena);
                boolean numero = mNumeros.find();

                //Verifica que contenga letras en minuscula
                Pattern pMinisculas = Pattern.compile("[a-z]");
                Matcher mMinisculas = pMinisculas.matcher(contrasena);
                boolean minusculas = mMinisculas.find();

                //Verifica que contenga letras en minuscula
                Pattern pMayusculas = Pattern.compile("[A-Z]");
                Matcher mMayusculas = pMayusculas.matcher(contrasena);
                boolean mayusculas = mMayusculas.find();

                //Verifica que contenga letras en minuscula
                Pattern pEspeciales = Pattern.compile("[^a-z0-9 ]");
                Matcher mEspeciales = pEspeciales.matcher(contrasena);
                boolean especiales = mEspeciales.find();

                if(!numero){
                    Toast.makeText(this,"La contraseña debe poseer al menos un número", Toast.LENGTH_SHORT).show();
                    return false;
                }else if(!minusculas){
                    Toast.makeText(this,"La contraseña debe poseer minúsculas", Toast.LENGTH_SHORT).show();
                    return false;
                }
                else if(!mayusculas){
                    Toast.makeText(this,"La contraseña debe poseer al menos una mayúscula", Toast.LENGTH_SHORT).show();
                    return false;
                } else if (!especiales) {
                    Toast.makeText(this,"La contraseña debe poseer carácteres especiales", Toast.LENGTH_SHORT).show();
                    return false;
                }else{
                    return true;
                }
            }else{
                Toast.makeText(this,"La contraseña debe contener 8 caracteres", Toast.LENGTH_SHORT).show();
                return false;
            }
        }
    }

    public void cambiarContrasena(View view){
        String contrasena_String = contrasenaNueva.getText().toString();
        String confirmacion_String = contrasenaConfirmacion.getText().toString();
        String contrasenaAntigua_String = contrasenaAtigua.getText().toString();

        if(contrasenaAntigua_String.isEmpty()){
            Toast.makeText(this,"Debe ingresar la contraseña antigua", Toast.LENGTH_SHORT).show();

        }else{
            if(validarContrasena(contrasena_String,confirmacion_String)){

                //Se crea el loader que se mostrara mientras se procesa la transaccion
                AlertDialog.Builder loader = new AlertDialog.Builder(PantallaCambioContrasena.this);

                LayoutInflater inflater = this.getLayoutInflater();

                loader.setView(inflater.inflate(R.layout.loader_procesando_transaccion,null));
                loader.setCancelable(false);

                dialog = loader.create();
                dialog.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));

                Log.d("OPEN"," se abrio el loader");
                dialog.show();
            }
        }
    }


    @Override
    public void onBackPressed() {
    }
}