package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.inicio.cupoTarjeta;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.fragment.app.Fragment;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;

public class pantallaCupo extends Fragment {

    private Header header = new Header("<b>Cupo tarjeta</b>");
    private View vista;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {



        getFragmentManager().beginTransaction().replace(R.id.contenedorHeaderCupo,header).commit();

        vista = inflater.inflate(R.layout.fragment_pantalla_cupo, container, false);


        return vista;
    }




}