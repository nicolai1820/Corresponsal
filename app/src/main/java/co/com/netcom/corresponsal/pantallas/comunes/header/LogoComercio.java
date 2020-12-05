package co.com.netcom.corresponsal.pantallas.comunes.header;

import android.os.Bundle;

import androidx.fragment.app.Fragment;

import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ImageView;
import android.widget.Toast;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.comunes.listView.ListViewDispositivos;
import co.com.netcom.corresponsal.pantallas.funciones.MetodosSDKNewland;

/**
 * A simple {@link Fragment} subclass.
 * Use the {@link LogoComercio#} factory method to
 * create an instance of this fragment.
 */
public class LogoComercio extends Fragment {

    private View vista;
    private ImageView estadoActivo, estadoInactivo;
    public static Handler estado;
    private MetodosSDKNewland sdkNewland;


    public LogoComercio() {
        // Required empty public constructor
    }


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {

        vista = inflater.inflate(R.layout.fragment_logo_comercio, container, false);

        estadoActivo = vista.findViewById(R.id.imageViewActivo);
        estadoInactivo = vista.findViewById(R.id.imageViewInactivo);

       // sdkNewland = new MetodosSDKNewland(getActivity());





        // Inflate the layout for this fragment
        return vista;
    }

    @Override
    public void onResume() {
        estado = new Handler() {

            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what) {
                    case 1: {

                        break;}
                    case 2:{

                        getActivity().runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                while (sdkNewland.isConnected()){
                                    estadoActivo.setVisibility(View.VISIBLE);
                                    estadoInactivo.setVisibility(View.INVISIBLE);
                                }
                            }
                        });


                        break;}
                    default:
                        break;
                }
            }

        };
        super.onResume();
    }
}