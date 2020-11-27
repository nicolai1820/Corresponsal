package co.com.netcom.corresponsal;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.widget.TextView;

import co.com.netcom.corresponsal.core.comunicacion.IntegradorC;

public class PruebaC extends AppCompatActivity {


    private TextView eTPruebaC;
    private IntegradorC inte;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_prueba_c);

        inte = new IntegradorC(getApplicationContext());
        eTPruebaC = (TextView) findViewById(R.id.textViewc);


    }
}