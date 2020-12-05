package co.com.netcom.corresponsal.pantallas.corresponsal.administrador.headerAdministrador;

import android.content.Intent;
import android.os.Bundle;
import android.text.Html;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.fragment.app.Fragment;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.comunes.header.LogoComercio;
import co.com.netcom.corresponsal.pantallas.corresponsal.administrador.inicio.pantallaInicialAdministrador;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.inicio.pantallaInicialUsuarioComun;

public class HeaderAdministrador extends Fragment {

    private TextView titulo;
    private View vista;
    private String titulo_string;
    private ImageView logo;
    private LogoComercio logoComercioImage;



    /**Metodo constructor que recibo como parametro un String, el cual va a ser el titulo del header*/

    public HeaderAdministrador(String contenidoTitulo) {

        this.titulo_string = contenidoTitulo;

    }

    public HeaderAdministrador(){}



    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {

        // Se crea el correspondiente header
        vista = inflater.inflate(R.layout.fragment_header_administrador, container, false);

        //Se crea la conexion con la interfaz grafica
        titulo = (TextView) vista.findViewById(R.id.tituloHeaderAdministrador);
        logo = (ImageView) vista.findViewById(R.id.imageViewHeaderAdministrador);

        //logoComercioImage = new LogoComercio();

        //getFragmentManager().beginTransaction().replace(R.id.frameLayoutLogoComercioAdministrador , logoComercioImage).commit();
        //Se asigna el String recibido como parametro en el constructor como titulo
        titulo.setText(Html.fromHtml(titulo_string));


        //Se crea el evento click del logo
        logo.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                if (getActivity().getLocalClassName().equals("co.com.netcom.corresponsal.pantallas.corresponsal.administrador.inicio.pantallaInicialAdministrador")) {

                }
                else {
                    //Se hace el correspondiente intent a la pantalla inicial, con la correspondiente animacion
                    Intent i = new Intent(getContext(), pantallaInicialAdministrador.class);
                    startActivity(i);
                    Log.d("Actividad", getActivity().toString());
                    getActivity().overridePendingTransition(R.anim.slide_in_left,R.anim.slide_out_right);
                }

            }
        });


        return vista;
    }
}