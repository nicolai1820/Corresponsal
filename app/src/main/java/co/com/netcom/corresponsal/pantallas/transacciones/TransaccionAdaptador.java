package co.com.netcom.corresponsal.pantallas.transacciones;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import java.util.List;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.pantallas.funciones.CodificarBase64;
import co.com.netcom.corresponsal.pantallas.funciones.CodigosTransacciones;
import co.com.netcom.corresponsal.pantallas.mapeos.ConsultaVenta;

public class TransaccionAdaptador extends BaseAdapter {
    Context context ;
    List<ConsultaVenta> lst;

    public TransaccionAdaptador(Context context, List<ConsultaVenta> lst) {
        this.context = context;
        this.lst = lst;
    }


    @Override
    public int getCount() {
        return lst.size() ;
    }

    @Override
    public Object getItem(int position) {
        return position;
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        TextView textView_nombre;
        TextView textView_codigoTransaccion;
        TextView textView_montoTransaccion;
        TextView textView_fechaTransaccion;
        String nombre="";

        ConsultaVenta c = lst.get(position);

        if(convertView ==null)
             convertView = LayoutInflater.from(context).inflate(R.layout.listview_element,null);

        textView_nombre = convertView.findViewById(R.id.textView_nombreTransaccion);
        textView_codigoTransaccion = convertView.findViewById(R.id.textView_codigoTransaccion);
        textView_montoTransaccion = convertView.findViewById(R.id.textView_montoTransaccion);
        textView_fechaTransaccion = convertView.findViewById(R.id.textView_fechaTransaccion);

        CodificarBase64 base64 = new CodificarBase64();

        switch (Integer.parseInt(base64.decodificarBase64(c.getTipoTransaccion()))){
            case CodigosTransacciones.RETIRO:
                        nombre = "Retiro";
                        break;

            case CodigosTransacciones.ENVIAR_GIROS:
                        nombre="Envio giro";
                        break;
            case CodigosTransacciones.CANCELAR_GIRO:
                nombre="Cancelar giro";
                break;

            case CodigosTransacciones.RECLAMAR_GIRO:
                nombre = "Reclamar giro";
                break;
            case CodigosTransacciones.PAGO_FACTURAS:
                        nombre ="Pago factura";
                        break;
            case CodigosTransacciones.PAGO_PRODUCTOS:
                            nombre ="Pago productos";
                            break;
            case CodigosTransacciones.RECARGAS:
                nombre="Recarga";
                break;
            case CodigosTransacciones.TRANSFERENCIA:
                nombre="Transferencia";
                break;
        }

        textView_nombre.setText(nombre);
        textView_codigoTransaccion.setText(base64.decodificarBase64(c.getCodigoAprobacion()));
        textView_montoTransaccion.setText("$ "+base64.decodificarBase64(c.getValorCompra()));
        textView_fechaTransaccion.setText(base64.decodificarBase64(c.getFecha()));

        return convertView;
    }
}
