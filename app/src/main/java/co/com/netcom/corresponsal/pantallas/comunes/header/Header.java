package co.com.netcom.corresponsal.pantallas.comunes.header;

import android.content.Intent;
import android.os.Bundle;

import androidx.fragment.app.Fragment;

import android.text.Html;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.TextView;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.comunes.logIn.LogIn;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.inicio.pantallaInicialUsuarioComun;


public class Header extends Fragment {
    private TextView titulo;
    private View vista;
    private String titulo_string;
    private ImageView logo;
    private LogoComercio logoComercioImage;

    /**Metodo constructor que recibo como parametro un String, el cual va a ser el titulo del header*/

     public Header(String contenidoTitulo) {

        this.titulo_string = contenidoTitulo;

    }


    public Header(){}


    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {

         // Se crea el correspondiente header
        vista = inflater.inflate(R.layout.fragment_header, container, false);

        //Se crea la conexion con la interfaz grafica
        titulo = (TextView) vista.findViewById(R.id.tituloHeader);
        logo = (ImageView) vista.findViewById(R.id.imageViewHeader);


        //logoComercioImage = new LogoComercio();
        //getFragmentManager().beginTransaction().replace(R.id.frameLayoutLogoComercio , logoComercioImage).commit();

        //Se asigna el String recibido como parametro en el constructor como titulo
        titulo.setText(Html.fromHtml(titulo_string));


        //Se crea el evento click del logo
        logo.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                if (getActivity().getLocalClassName().equals("pantallas.corresponsal.usuarioComun.transacciones.inicio.pantallaInicialUsuarioComun")) {

                } else if (getActivity().getLocalClassName().equals("pantallas.comunes.logIn.PantallaCambioContrasena")) {

                    //Se hace el correspondiente intent a la pantalla inicial de la aplicación
                   /* Intent i = new Intent(getContext(), LogIn.class);
                    startActivity(i);
                    Log.d("Actividad", getActivity().toString());
                    getActivity().overridePendingTransition(R.anim.slide_in_left, R.anim.slide_out_right);*/
                } else {
                    //Se hace el correspondiente intent a la pantalla inicial, con la correspondiente animacion
                    Intent i = new Intent(getContext(), pantallaInicialUsuarioComun.class);
                    startActivity(i);
                    Log.d("Actividad", getActivity().toString());
                    getActivity().overridePendingTransition(R.anim.slide_in_left, R.anim.slide_out_right);
                }

            }
        });

        return vista;
    }
}