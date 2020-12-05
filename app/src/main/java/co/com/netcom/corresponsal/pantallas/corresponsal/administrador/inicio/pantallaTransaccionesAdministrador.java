package co.com.netcom.corresponsal.pantallas.corresponsal.administrador.inicio;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.fragment.app.Fragment;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.corresponsal.administrador.headerAdministrador.HeaderAdministrador;

public class pantallaTransaccionesAdministrador extends Fragment {
    private View vista;
    private HeaderAdministrador header = new HeaderAdministrador("<b>Movimientos del día</b>");


    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {



        getFragmentManager().beginTransaction().replace(R.id.headerTransacciones,header).commit();

        vista = inflater.inflate(R.layout.fragment_pantalla_transacciones_administrador, container, false);

        return vista;
    }
}