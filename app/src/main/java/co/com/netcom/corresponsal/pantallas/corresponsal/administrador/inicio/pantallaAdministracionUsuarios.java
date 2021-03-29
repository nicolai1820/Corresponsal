package co.com.netcom.corresponsal.pantallas.corresponsal.administrador.inicio;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;

import androidx.fragment.app.Fragment;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.corresponsal.administrador.headerAdministrador.HeaderAdministrador;

public class pantallaAdministracionUsuarios extends Fragment {

    private HeaderAdministrador header = new HeaderAdministrador("<b>Administrador</b>");
    private View vista;
    private Button crear,editar,listar;



    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {


        getFragmentManager().beginTransaction().replace(R.id.contenedorHeaderPantallaAdministracionUsuarios,header).commit();

        vista = inflater.inflate(R.layout.fragment_pantalla_administracion_usuarios, container, false);

        crear = (Button) vista.findViewById(R.id.button_CrearUsuarios);
        editar = (Button) vista.findViewById(R.id.button_EditarUsuarios);
        listar = (Button) vista.findViewById(R.id.button_ListarUsuarios);


        crear.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

            }
        });

        editar.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

            }
        });

        listar.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

            }
        });

        return vista;
    }


}