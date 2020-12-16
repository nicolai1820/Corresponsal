package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.inicio.servicios;

import android.content.Intent;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ScrollView;

import androidx.fragment.app.Fragment;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.google.android.material.floatingactionbutton.FloatingActionButton;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.consultaSaldo.pantallaConsultaSaldoLectura;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.deposito.pantallaDepositoDatosDepositante;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.giros.pantallaInicialGiros;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.pagoFacturas.pantallaInicialPagoFacturas;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.pagoProductosBCO.pantallaInicialPagoProductosBCO;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.recargas.pantallaRecargaCelular;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.retiro.PantallaInicialRetiro;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.transferencia.pantallaTransferenciaCantidad;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.transferencia.pantallaTransferenciaLectura;

public class pantallaServiciosUsuarioComun extends Fragment {

    private ScrollView scroll;
    private FloatingActionButton down;
    private View vista;
    private Header header = new Header("<b>Productos</b>");
    private Button giros,consultaDeSaldo,transferencia,pagoProductos,pagoFacturas,deposito,retiro,recargas;


    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {

        // Se crea el header de la actividad con el titulo de la actividad

        getFragmentManager().beginTransaction().replace(R.id.contenedorHeaderServiciosUsuario,header).commit();

        // Se crea la conexion con la interfaz gráfica de el fragmento.

        vista = inflater.inflate(R.layout.fragment_pantalla_servicios_usuario_comun, container, false);

        giros = (Button) vista.findViewById(R.id.button_Giros);
        transferencia = (Button) vista.findViewById(R.id.button_Transferencia);
        pagoProductos = (Button) vista.findViewById(R.id.button_PagoProductos);
        pagoFacturas = (Button) vista.findViewById(R.id.button_PagoFacturas);
        deposito = (Button) vista.findViewById(R.id.button_Deposito);
        retiro = (Button) vista.findViewById(R.id.button_Retiro);
        consultaDeSaldo = (Button) vista.findViewById(R.id.button_ConsultaSaldo);
        recargas = (Button) vista.findViewById(R.id.button_Recargas);


        /** Metodo de tipo void que recibe como parametro una vista para ser implementado por el boton giros.
         * Se encarga de realizar el correspondiente intent a la actividad PantallaInicialGiros*/
        giros.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent i = new Intent(getContext(),pantallaInicialGiros.class);
                startActivity(i);
                getActivity().overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);
            }
        });

        /** Metodo de tipo void que recibe como parametro una vista para ser implementado por el boton transferencia.
         * Se encarga de realizar el correspondiente intent a la actividad pantallaTransferencia */
        transferencia.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent i = new Intent(getContext(), pantallaTransferenciaCantidad.class);
                startActivity(i);
                getActivity().overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);
            }
        });

        /** Metodo de tipo void que recibe como parametro una vista para ser implementado por el boton pagoProductos.
         * Se encarga de realizar el correspondiente intent a la actividad pantallaPagoProductosInicial */
        pagoProductos.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent i = new Intent(getContext(), pantallaInicialPagoProductosBCO.class);
                startActivity(i);
                getActivity().overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);
            }
        });

        /** Metodo de tipo void que recibe como parametro una vista para ser implementado por el boton pagoFacturas.
         * Se encarga de realizar el correspondiente intent a la actividad pantallaInicialPagoFacturas */
        pagoFacturas.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                    Intent i = new Intent(getContext(), pantallaInicialPagoFacturas.class);
                    startActivity(i);
                    getActivity().overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);
            }
        });

        /** Metodo de tipo void que recibe como parametro una vista para ser implementado por el boton deposito.
         * Se encarga de realizar el correspondiente intent a la actividad pantallaDeposito */
        deposito.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent i = new Intent(getContext(), pantallaDepositoDatosDepositante.class);
                startActivity(i);
                getActivity().overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);

            }
        });

        /** Metodo de tipo void que recibe como parametro una vista para ser implementado por el boton retiro.
         * Se encarga de realizar el correspondiente intent a la actividad pantallaRetiro*/

        retiro.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent i = new Intent(getContext(), PantallaInicialRetiro.class);
                startActivity(i);
                getActivity().overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);

            }
        });

        scroll = vista.findViewById(R.id.ScrollViewServicios);
        View scrollDown = vista.findViewById(R.id.scrollAbajo);
        /*scrollDown.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                if(scroll.getChildAt(0).getBottom()>=(scroll.getHeight()+scroll.getScrollY())){

                    scroll.fullScroll(scroll.FOCUS_DOWN);
                    scrollDown.setVisibility(View.INVISIBLE);


                }
            }
        });*/

        /** Metodo de tipo void que recibe como parametro una vista para ser implementado por el boton consultaDeSaldo.
         * Se encarga de realizar el correspondiente intent a la actividad pantallaConsultadeSaldo*/

        consultaDeSaldo.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent i = new Intent(getContext(), pantallaConsultaSaldoLectura.class);
                startActivity(i);
                getActivity().overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);
            }
        });

        /** Metodo de tipo void que recibe como parametro una vista para ser implementado por el boton recargas.
         * Se encarga de realizar el correspondiente intent a la actividad pantallaRecargas */

        recargas.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent i = new Intent(getContext(), pantallaRecargaCelular.class);
                startActivity(i);
                getActivity().overridePendingTransition(R.anim.slide_in_right,R.anim.slide_out_left);
            }
        });

        return vista;

    }




}