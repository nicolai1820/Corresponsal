package co.com.netcom.corresponsal.pantallas.corresponsal.administrador.inicio;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import android.widget.ScrollView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;


import com.google.android.material.floatingactionbutton.FloatingActionButton;

import co.com.netcom.corresponsal.R;

public class pantallaAjustesAdministrador extends Fragment {

    private  ScrollView scroll;
    private FloatingActionButton down;
    private View vista;

    @Nullable
    @Override
    public View onCreateView( LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
         vista = inflater.inflate(R.layout.fragment_pantalla_ajustes_administrador, container, false);


        return vista;
    }

}