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

public class pantallaServiciosAdministrador extends Fragment {

    private View vista;
    private HeaderAdministrador header = new HeaderAdministrador("<b>Administrador</b>");
    private Button preguntasFrecuentes,soporte,cerrarSesion;

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {


        getFragmentManager().beginTransaction().replace(R.id.contenedorHeaderPantallaServiciosAdministrador,header).commit();

        vista = inflater.inflate(R.layout.fragment_pantalla_servicios_administrador, container, false);


        preguntasFrecuentes = (Button) vista.findViewById(R.id.button_preguntasFrecuentesAdministrador);
        soporte = (Button) vista.findViewById(R.id.button_soporteAdministrador);
        cerrarSesion = (Button) vista.findViewById(R.id.button_cerrarSesionAdministrador);

        preguntasFrecuentes.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

            }
        });

        soporte.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

            }
        });

        cerrarSesion.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

            }
        });


        return vista;
    }
}