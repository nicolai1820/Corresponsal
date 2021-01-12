package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.inicio.informacion;

import android.content.Intent;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;

import androidx.fragment.app.Fragment;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.duplicado.pantallaInicialDuplicado;
import co.com.netcom.corresponsal.pantallas.funciones.Servicios;

public class pantallaInformacionUsuarioComun extends Fragment {


    private Button preguntasFrecuentes,soporte, duplicado, cerrarSesion;
    private Header header = new Header("<b> Información </b>");
    private Servicios servicio = new Servicios(getActivity());

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {

        View view = inflater.inflate(R.layout.fragment_pantalla_informacion_usuario_comun, container, false);

        //Se reemplaza el header con el correspondiente de la pantalla.
        getFragmentManager().beginTransaction().replace(R.id.contenedorHeaderPantallaInformacion,header).commit();

        preguntasFrecuentes = view.findViewById(R.id.button_preguntasFrecuentes);
        soporte = view.findViewById(R.id.button_soporte);
        duplicado = view.findViewById(R.id.button_duplicadoRecibo);

        /**Evento click del boton preguntas frecuentes, redirige a la pantalla de preuntas frecuentes.*/
        preguntasFrecuentes.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

            }
        });

        /**Evento click del boton soporte, redirige a la pantalla de soporte */
        soporte.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

            }
        });

        /**Evento click del boton duplicado, redirige a la pantalla principal de duplicado*/
        duplicado.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent i = new Intent(getContext(), pantallaInicialDuplicado.class);
                startActivity(i);
                getActivity().overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);
            }
        });

        /**Evento click del boton  cerrar sesión, cierra sesión frente al servidor*/
        cerrarSesion.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                servicio.cerrarSesion(servicio.getToken());
            }
        });

        return view;
    }
}