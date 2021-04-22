package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.inicio.transacciones;

import android.app.AlertDialog;
import android.app.Dialog;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ListView;
import android.widget.Toast;

import androidx.fragment.app.Fragment;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.util.ArrayList;
import java.util.List;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.funciones.Servicios;
import co.com.netcom.corresponsal.pantallas.mapeos.ConsultaVenta;
import co.com.netcom.corresponsal.pantallas.transacciones.Transaccion;
import co.com.netcom.corresponsal.pantallas.transacciones.TransaccionAdaptador;

public class pantallaTransacciones extends Fragment {
    private View vista;
    private Header header = new Header("<b>Movimientos del día</b>");
    private ArrayList<ConsultaVenta> ventas= new ArrayList();
    private Dialog dialog;
    private static Handler resp;
    private ListView listView_ventas;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {



        getFragmentManager().beginTransaction().replace(R.id.headerTransacciones,header).commit();

        vista = inflater.inflate(R.layout.fragment_pantalla_transacciones, container, false);

        listView_ventas = vista.findViewById(R.id.listView_ventas);



         resp = new Handler(){
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what) {
                    case 1:

                        //Poner pop up resultado correo
                        if(ventas.isEmpty()){
                            Toast.makeText(getActivity().getApplicationContext(),"No existen transacciones",Toast.LENGTH_SHORT).show();
                        }else{
                            TransaccionAdaptador tx = new TransaccionAdaptador(getActivity().getApplicationContext(),ventas);
                            listView_ventas.setAdapter(tx);
                            dialog.dismiss();
                        }

                }}};

        Servicios servicio = new Servicios(getActivity().getApplicationContext());

        dialog = new Dialog(getActivity());
        dialog.setCancelable(false);

        View view  = getActivity().getLayoutInflater().inflate(R.layout.loader_procesando_transaccion, null);
        dialog.setContentView(view);
        dialog.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));

        dialog.show();

        new Thread(new Runnable() {
            @Override
            public void run() {
                ventas =servicio.obtenerTransacciones();
                try{
                    //Se envia un mensaje al handler de la clase consulta saldo, indicando que el usuario cancelo la transaccion
                    Message usuarioCancela = new Message();
                    usuarioCancela.what = 1;
                    resp.sendMessage(usuarioCancela);
                }catch (Exception e){ }
            }
        }).start();

        return vista;
    }


    @Override
    public void onStart() {
        super.onStart();
    }
}