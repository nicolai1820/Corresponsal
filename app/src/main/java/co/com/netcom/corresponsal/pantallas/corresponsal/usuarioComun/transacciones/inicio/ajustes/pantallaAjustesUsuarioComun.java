package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.inicio.ajustes;

import android.Manifest;
import android.app.AlertDialog;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.Toast;

import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.fragment.app.Fragment;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.util.ArrayList;

import co.com.netcom.corresponsal.pantallas.comunes.header.Header;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.comunes.listView.ListViewDispositivos;
import co.com.netcom.corresponsal.pantallas.funciones.MetodosSDKNewland;

public class pantallaAjustesUsuarioComun extends Fragment {

    private Header header = new Header("<b>Configuración</b>");
    private View vista;
    private Button buscarDispositivosAjusterUsuarioComun;
    private MetodosSDKNewland newlandSDK;
    private ArrayList<String> dispositivos= new ArrayList<String>();;
    private Handler handler;
    private AlertDialog dialog;
    public  static Handler procesoTransacion;
    private ListView listViewDispositivos;


    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {

        getFragmentManager().beginTransaction().replace(R.id.contenedorHeaderAjustesUsuarioComun,header).commit();

        vista = inflater.inflate(R.layout.fragment_pantalla_ajustes_usuario_comun, container, false);

        //Conexion con la interfaz grafica
        buscarDispositivosAjusterUsuarioComun = (Button) vista.findViewById(R.id.button_BuscarDispositivosAjusterUsuarioComun);
        listViewDispositivos = (ListView) vista.findViewById(R.id.listView_Dispositivos);


        newlandSDK = new MetodosSDKNewland(getActivity().getApplicationContext());


        //Se crea un elemento para verificar que los permisos esten activados
        int permisoUbicacionCoarse = ContextCompat.checkSelfPermission(getActivity().getApplicationContext(), Manifest.permission.ACCESS_COARSE_LOCATION);
        int permisoUbicacionFine = ContextCompat.checkSelfPermission(getActivity().getApplicationContext(), Manifest.permission.ACCESS_FINE_LOCATION);
        int permisoAlmacenamiento = ContextCompat.checkSelfPermission(getActivity().getApplicationContext(), Manifest.permission.WRITE_EXTERNAL_STORAGE);

        //Se verifica que se tengan los permisos
        Log.d("PERMISOS",Manifest.permission.ACCESS_COARSE_LOCATION);
        if (permisoAlmacenamiento != PackageManager.PERMISSION_GRANTED || permisoUbicacionCoarse!=PackageManager.PERMISSION_GRANTED ||permisoUbicacionFine!=PackageManager.PERMISSION_GRANTED){
            ActivityCompat.requestPermissions(getActivity(),new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE,
                    Manifest.permission.ACCESS_COARSE_LOCATION,
                    Manifest.permission.ACCESS_FINE_LOCATION},0);
        } else{
            Log.d("PERMISOS","Si hay permisos");
        }





        buscarDispositivosAjusterUsuarioComun.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                if (newlandSDK.isConnected()){
                    Toast.makeText(getActivity(),"Ya tiene un dispositivo conectado",Toast.LENGTH_SHORT).show();
                }
                else {
                    //Loader de buscando dispositivos
                    AlertDialog.Builder loader = new AlertDialog.Builder(getActivity());

                    LayoutInflater inflater = getActivity().getLayoutInflater();

                    loader.setView(inflater.inflate(R.layout.loader_buscando_dispositivos,null));
                    loader.setCancelable(false);


                    AlertDialog dialog = loader.create();
                    dialog.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));

                    Log.d("OPEN", " se abrio el loader");

                    dialog.show();

                    //Loader de conectando dispositivos
                    AlertDialog.Builder loader2 = new AlertDialog.Builder(getActivity());

                    LayoutInflater inflater2 = getActivity().getLayoutInflater();

                    loader2.setView(inflater2.inflate(R.layout.loader_conectando_dispositivo, null));
                    loader2.setCancelable(false);

                    AlertDialog dialog2 = loader2.create();
                    dialog2.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));
                    Log.d("OPEN", " se abrio el loader");


                    procesoTransacion = new Handler() {

                        @Override
                        public void handleMessage(Message msg) {
                            super.handleMessage(msg);
                            switch (msg.what) {
                                case 1: {
                                    dialog2.dismiss();
                                    Toast.makeText(getActivity(), "Conexión Exitosa", Toast.LENGTH_LONG).show();

                                    break;
                                }
                                case 2: {
                                    try {
                                        dialog2.dismiss();
                                        Toast.makeText(getActivity(), "Conexión Fallida", Toast.LENGTH_SHORT).show();
                                    }catch (Exception e){}
                                    break;
                                }
                                case 3: {
                                    //Se utiliza para conectar al mpos que se elija de la lista.

                                    dispositivos = newlandSDK.getListDevices();
                                    Log.d("DISPOSITIVOS", newlandSDK.getListDevices().toString());


                                    if (dispositivos.size() <= 0) {
                                        dialog.dismiss();
                                        listViewDispositivos.setAdapter(null);
                                        Toast.makeText(getActivity(), "No se encontraron dispositivos", Toast.LENGTH_SHORT).show();
                                    } else {


                                        String[] listaDispositivos = new String[dispositivos.size()];
                                        listaDispositivos = dispositivos.toArray(listaDispositivos);
                                        String[] nombres = new String[dispositivos.size()];
                                        String[] direcciones = new String[dispositivos.size()];

                                        for (int i = 0; i < dispositivos.size(); i++) {

                                            String[] childItems = listaDispositivos[i].split("\n");
                                            nombres[i] = childItems[0];
                                            direcciones[i] = childItems[1];
                                        }

                                        for (int i = 0; i < nombres.length; i++) {
                                            Log.d("NOMBRES", nombres[i]);
                                            Log.d("DIRECCIONES", direcciones[i]);
                                        }


                                        ListViewDispositivos adapter = new ListViewDispositivos(getActivity(), nombres, direcciones);
                                        listViewDispositivos.setAdapter(adapter);

                                        listViewDispositivos.setOnItemClickListener(new AdapterView.OnItemClickListener() {
                                            @Override
                                            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                                                Log.d("DIRECCION", direcciones[position]);

                                                if(newlandSDK.isConnected()){
                                                    Toast.makeText(getActivity(),"Ya tiene un dispositivo conectado",Toast.LENGTH_SHORT).show();
                                                }else{
                                                    dialog2.show();

                                                    new Thread(new Runnable() {
                                                        @Override
                                                        public void run() {
                                                            newlandSDK.connectDevice(60, direcciones[position]);

                                                        }
                                                    }).start();
                                                }

                                            }
                                        });

                                        dialog.dismiss();
                                    }
                                    break;
                                }
                                case 4:
                                    dialog.dismiss();


                                default:
                                    break;
                            }
                        }

                    };

                    getActivity().runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            newlandSDK.scanDevice();

                        }
                    });
                    //Se realiza la busqueda de dispostivos
                  /*  Thread t = new Thread(new Runnable() {
                        @Override
                        public void run() {
                        }
                    });

                    t.start();*/

                }
            }
        });



        return vista;

    }


}