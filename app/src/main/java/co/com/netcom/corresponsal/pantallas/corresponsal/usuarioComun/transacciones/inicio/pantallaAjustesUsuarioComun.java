package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.inicio;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ImageButton;

import androidx.fragment.app.Fragment;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.funciones.MetodosSDKNewland;

public class pantallaAjustesUsuarioComun extends Fragment {

    private Header header = new Header("<b>Configuración</b>");
    private View vista;
    private Button buscarDispositivosAjusterUsuarioComun;
   private MetodosSDKNewland newlandSDK;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {

        getFragmentManager().beginTransaction().replace(R.id.contenedorHeaderAjustesUsuarioComun,header).commit();

        vista = inflater.inflate(R.layout.fragment_pantalla_ajustes_usuario_comun, container, false);

        buscarDispositivosAjusterUsuarioComun = (Button) vista.findViewById(R.id.button_BuscarDispositivosAjusterUsuarioComun);

        newlandSDK = new MetodosSDKNewland(getActivity().getApplicationContext());

        buscarDispositivosAjusterUsuarioComun.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                newlandSDK.scanDevice();
               // newlandSDK.listDevices();
            }
        });

        return vista;

    }


}