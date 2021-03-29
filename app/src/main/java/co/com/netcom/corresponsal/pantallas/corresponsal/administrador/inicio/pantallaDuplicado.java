package co.com.netcom.corresponsal.pantallas.corresponsal.administrador.inicio;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import android.widget.Button;

import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;



import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.corresponsal.administrador.headerAdministrador.HeaderAdministrador;

public class pantallaDuplicado extends Fragment {

    private HeaderAdministrador header = new HeaderAdministrador("<b>Duplicado</b>");
    private View vista;
    private Button ultimoRecibo, otroRecibo;

    @Nullable
    @Override
    public View onCreateView( LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {

        getFragmentManager().beginTransaction().replace(R.id.contenedorHeaderDuplicado,header).commit();

        vista = inflater.inflate(R.layout.fragment_pantalla_duplicado, container, false);

        ultimoRecibo = (Button) vista.findViewById(R.id.button_UltimoRecibo);
        otroRecibo = (Button) vista.findViewById(R.id.button_OtroRecibo);


        ultimoRecibo.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

            }
        });



        otroRecibo.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

            }
        });

        return vista;
    }

}