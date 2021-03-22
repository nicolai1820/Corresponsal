package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.inicio.informacion;

import android.app.AlertDialog;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;

import androidx.fragment.app.Fragment;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.comunes.logIn.LogIn;
import co.com.netcom.corresponsal.pantallas.comunes.popUp.PopUp;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.duplicado.pantallaInicialDuplicado;
import co.com.netcom.corresponsal.pantallas.funciones.DeviceInformation;
import co.com.netcom.corresponsal.pantallas.funciones.PreferencesUsuario;
import co.com.netcom.corresponsal.pantallas.funciones.Servicios;
import co.com.netcom.corresponsal.pantallas.funciones.TimeOutSesion;

public class pantallaInformacionUsuarioComun extends Fragment {


    private Button preguntasFrecuentes,soporte, duplicado, cerrarSesion;
    private Header header = new Header("<b> Información </b>");
    private Servicios servicio;
    private Thread hilo;
    private String respuesta="";
    public static Handler respuestaCerrar;
    AlertDialog dialog;


    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {

        View view = inflater.inflate(R.layout.fragment_pantalla_informacion_usuario_comun, container, false);

        //Se reemplaza el header con el correspondiente de la pantalla.
        getFragmentManager().beginTransaction().replace(R.id.contenedorHeaderPantallaInformacion,header).commit();

        preguntasFrecuentes = view.findViewById(R.id.button_preguntasFrecuentes);
        soporte = view.findViewById(R.id.button_soporte);
        duplicado = view.findViewById(R.id.button_duplicadoRecibo);
        cerrarSesion = view.findViewById(R.id.button_cerrarSesion);
        servicio= new Servicios(getActivity());


        //Se crea el Handler para manejar la respuesta de los Hilos sin Join y evitar así que se bloquee el aplicativo

        respuestaCerrar = new Handler(){
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what) {
                    case 1:

                        if (respuesta.equals("MQ==")){
                            dialog.dismiss();
                            TimeOutSesion timeOutSesion = new TimeOutSesion();
                            timeOutSesion.stopLogoutTimer();
                            Intent i = new Intent(getActivity(), LogIn.class);
                            startActivity(i);
                            getActivity().overridePendingTransition(R.anim.slide_in_left, R.anim.slide_out_right);
                        }else {
                            PopUp pop = new PopUp(getActivity().getApplicationContext());
                            pop.crearPopUpLogOutFallido();
                        }
                        break;
                }
            }
        };



                        /**Evento click del boton preguntas frecuentes, redirige a la pantalla de preuntas frecuentes.*/
        preguntasFrecuentes.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

            }
        });

        /**Evento click del boton soporte, redirige a la pantalla de soporte */
        soporte.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

            }
        });

        /**Evento click del boton duplicado, redirige a la pantalla principal de duplicado*/
        duplicado.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent i = new Intent(getContext(), pantallaInicialDuplicado.class);
                startActivity(i);
                getActivity().overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);
            }
        });

        /**Evento click del boton  cerrar sesión, cierra sesión frente al servidor*/
        cerrarSesion.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                //Se crea el loader
                AlertDialog.Builder loader = new AlertDialog.Builder(getActivity());
                LayoutInflater inflater = getActivity().getLayoutInflater();

                loader.setView(inflater.inflate(R.layout.loader_procesando_transaccion,null));
                loader.setCancelable(false);

                dialog = loader.create();
                dialog.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));

                Log.d("OPEN"," se abrio el loader");
                dialog.show();

                //Se verifica que se posee conexión a internet

                DeviceInformation device = new DeviceInformation(getActivity().getApplicationContext());

                if(device.getInternetStatus()){
                    hilo=  new Thread(new Runnable() {
                        @Override
                        public void run() {
                            respuesta = servicio.cerrarSesion();
                            try{
                                //Se envia un mensaje al handler para continuar el flujo de cierre de sesión
                                Message cierreSesion = new Message();
                                cierreSesion.what = 1;
                                pantallaInformacionUsuarioComun.respuestaCerrar.sendMessage(cierreSesion);
                            }catch (Exception e){ }

                        }
                    });
                }else{

                    PopUp popUp = new PopUp(getActivity().getApplicationContext());
                    popUp.crearPopUpErrorInternet();
                }


            }
        });



        return view;
    }
}