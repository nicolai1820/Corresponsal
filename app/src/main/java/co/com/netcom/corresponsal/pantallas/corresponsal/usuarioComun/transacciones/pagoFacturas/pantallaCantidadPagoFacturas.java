package co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.pagoFacturas;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;

import java.util.ArrayList;

import cn.pedant.SweetAlert.SweetAlertDialog;
import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.core.comunicacion.DatosRecaudo;
import co.com.netcom.corresponsal.pantallas.comunes.header.Header;
import co.com.netcom.corresponsal.pantallas.funciones.BaseActivity;

public class pantallaCantidadPagoFacturas extends BaseActivity {



    private final static int PAGO_VALOR_EXACTO = 0;
    private final static int PAGO_CERO = 1;
    private final static int PAGO_MAYOR = 2;
    private final static int PAGO_MAYOR_O_CERO = 3;
    private final static int PAGO_MENOR_NO_CERO = 4;
    private final static int PAGO_MENOR_O_CERO = 5;
    private final static int PAGO_CUALQUIER_NO_CERO = 6;
    private final static int PAGO_CUALQUIER_O_CERO = 7;
    private final static int NO_BASE_DATOS = 0;
    private final static int BASE_DATOS = 1;
    private final static String INDICADOR_LECTOR = "1";

    private DatosRecaudo datosRecaudo;
    private String valFactura;
    private String mensaje1 = "";
    private String mensaje2 = "";
    private String totalVenta = "";
    private boolean montoCorrecto = false;
    private boolean montoFijo = false;
    private boolean isLector;

    private LinearLayout LinearLayoutContenidoPagoFactura;
    private EditText cantidadFactura;
    private TextView textViewMensaje1,textViewMensaje2;
    private Header header = new Header("<b>Pago Facturas</b>");
    ArrayList<String> valores = new ArrayList<String>();
    private ArrayList<Integer> iconos = new ArrayList<Integer>();




    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pantalla_cantidad_pago_facturas);

        //Se cambiar el header por el correspondiente de la actividad
        getSupportFragmentManager().beginTransaction().replace(R.id.contenedorHeaderPantallaCantidadPagoFacturas,header).commit();


        //Se hace la conexión con la interfaz gráfica
        LinearLayoutContenidoPagoFactura = findViewById(R.id.LinearLayoutCantidadFactura);
        cantidadFactura = findViewById(R.id.editText_CantidadPagoFacturas);
        textViewMensaje1 = findViewById(R.id.textView_Mensaje1CantidadFacturas);
        textViewMensaje2 = findViewById(R.id.textView_Mensaje2CantidadFacturas);

        Bundle recibidor = getIntent().getExtras();
        datosRecaudo = new DatosRecaudo();
        datosRecaudo = recibidor.getParcelable("datosRecaudo");
        isLector =recibidor.getBoolean("isLector");

        capturarMontosIndicadores(datosRecaudo);
    }

    public int capturarMontosIndicadores(DatosRecaudo datosRecaudo) {
        int iRet = 0;
        int contador = 1;
        long valorIngresado = 0;
        boolean montoCorrecto = false;
        String variableAux;
        boolean indicadorValorPermitido = true;

        valFactura = Long.toString(datosRecaudo.getValorFactura());

        switch (datosRecaudo.getIndicadorValor()) {

            case PAGO_MAYOR:
                mensaje1 = "PAGO IGUAL O MAYOR ";
                mensaje2 = "DESEA MODIFICAR VALOR? ";
                break;
            case PAGO_MAYOR_O_CERO:
                mensaje1 = "PAGO IGUAL O MAYOR ";
                mensaje2 = "DESEA MODIFICAR VALOR? ";
                break;
            case PAGO_MENOR_NO_CERO:
                mensaje1 = "PAGO IGUAL O MENOR ";
                mensaje2 = "DESEA MODIFICAR VALOR? ";
                break;

            case PAGO_MENOR_O_CERO:
                mensaje1 = "PAGO IGUAL O MENOR ";
                mensaje2 = "DESEA MODIFICAR VALOR? ";
                break;

            case PAGO_CUALQUIER_NO_CERO:
            case PAGO_CUALQUIER_O_CERO:
                mensaje1 = "DESEA MODIFICAR VALOR? ";
                break;

            default:
                break;
        }

        if (datosRecaudo.getIndicadorBaseDatos() == BASE_DATOS
                ||
                (datosRecaudo.getIndicadorBaseDatos() == NO_BASE_DATOS && datosRecaudo.getTipoRecaudo().equals(INDICADOR_LECTOR))) {

            switch (datosRecaudo.getIndicadorValor()) {
                case PAGO_VALOR_EXACTO:
                case PAGO_CERO:
                    iRet = 1;
                    break;
                case PAGO_MAYOR:
                case PAGO_MAYOR_O_CERO:
                case PAGO_MENOR_NO_CERO:
                case PAGO_MENOR_O_CERO:
                case PAGO_CUALQUIER_NO_CERO:
                case PAGO_CUALQUIER_O_CERO:
                    if (indicadorValorPermitido) {
                        String mensaje = valFactura + " " + mensaje1 + mensaje2;
                        confirmarCambioValor("VALOR PAGO DE FACTURAS", mensaje, 1);
                        iRet = 1;
                    }
                    break;
                default:
                    iRet = 0;
                    break;
            }
        } else {
            iRet = 1;
        }
        if (((datosRecaudo.getIndicadorValor() != PAGO_VALOR_EXACTO && datosRecaudo.getIndicadorValor() != PAGO_CERO)
                || datosRecaudo.getIndicadorBaseDatos() == NO_BASE_DATOS)) {

            if (datosRecaudo.getIndicadorBaseDatos() == NO_BASE_DATOS &&
                    datosRecaudo.getTipoRecaudo().equals(INDICADOR_LECTOR)) {
                if (datosRecaudo.getValorFactura() == 0) {
                    LinearLayoutContenidoPagoFactura.setVisibility(View.VISIBLE);
                    textViewMensaje1.setText("PAGO DE FACTURAS");
                    textViewMensaje2.setText("VALOR PAGO DE FACTURAS");
                    cantidadFactura.setEnabled(true);
                } else {
                    if (datosRecaudo.getIndicadorValor() == PAGO_VALOR_EXACTO || datosRecaudo.getIndicadorValor() == PAGO_CERO) {
                        totalVenta = Long.toString(datosRecaudo.getValorFactura());
                        LinearLayoutContenidoPagoFactura.setVisibility(View.VISIBLE);
                        textViewMensaje1.setText("PAGO DE FACTURAS");
                        textViewMensaje2.setText("VALOR PAGO DE FACTURAS");
                        cantidadFactura.setText(totalVenta);
                        cantidadFactura.setEnabled(false);
                        montoFijo = true;
                    } else {
                        LinearLayoutContenidoPagoFactura.setVisibility(View.VISIBLE);
                        textViewMensaje1.setText("PAGO DE FACTURAS");
                        textViewMensaje2.setText("VALOR PAGO DE FACTURAS");
                        cantidadFactura.setEnabled(true);
                    }
                }
            } else {
                LinearLayoutContenidoPagoFactura.setVisibility(View.VISIBLE);
                textViewMensaje1.setText("PAGO DE FACTURAS");
                textViewMensaje2.setText("VALOR PAGO DE FACTURAS");
                cantidadFactura.setEnabled(true);
            }
        }

        return iRet;
    }


    private void confirmarCambioValor(String titulo, String mensaje, int action) {

        Log.d("SolictarRecibo", "onItemClick: solicitar");

        new SweetAlertDialog(pantallaCantidadPagoFacturas.this, SweetAlertDialog.WARNING_TYPE)
                .setTitleText(titulo)
                .setContentText(mensaje)
                .setConfirmText("SI")
                .setCancelText("NO")
                .showCancelButton(true)
                .setConfirmClickListener(new SweetAlertDialog.OnSweetClickListener() {
                    @Override
                    public void onClick(SweetAlertDialog sDialog) {
                        if (((datosRecaudo.getIndicadorValor() != PAGO_VALOR_EXACTO && datosRecaudo.getIndicadorValor() != PAGO_CERO)
                                || datosRecaudo.getIndicadorBaseDatos() == NO_BASE_DATOS)) {

                            if (datosRecaudo.getIndicadorBaseDatos() == NO_BASE_DATOS &&
                                    datosRecaudo.getTipoRecaudo().equals(INDICADOR_LECTOR)) {
                                if (datosRecaudo.getValorFactura() == 0) {
                                    LinearLayoutContenidoPagoFactura.setVisibility(View.VISIBLE);
                                    textViewMensaje1.setText(" PAGO DE FACTURAS ");
                                    textViewMensaje2.setText(" VALOR PAGO DE FACTURAS ");
                                    cantidadFactura.setEnabled(true);
                                } else {
                                    if (datosRecaudo.getIndicadorValor() == PAGO_VALOR_EXACTO || datosRecaudo.getIndicadorValor() == PAGO_CERO) {
                                        totalVenta = Long.toString(datosRecaudo.getValorFactura());
                                    } else {
                                        LinearLayoutContenidoPagoFactura.setVisibility(View.VISIBLE);
                                        textViewMensaje1.setText("PAGO DE FACTURAS");
                                        textViewMensaje2.setText("VALOR PAGO DE FACTURAS");
                                        cantidadFactura.setEnabled(true);
                                    }
                                }
                            } else {
                                LinearLayoutContenidoPagoFactura.setVisibility(View.VISIBLE);
                                textViewMensaje1.setText("PAGO DE FACTURAS");
                                textViewMensaje2.setText("VALOR PAGO DE FACTURAS");
                                cantidadFactura.setEnabled(true);
                            }
                        }


                        sDialog.dismissWithAnimation();
                    }
                })
                .setCancelClickListener(new SweetAlertDialog.OnSweetClickListener() {
                    @Override
                    public void onClick(SweetAlertDialog sDialog) {
                        LinearLayoutContenidoPagoFactura.setVisibility(View.VISIBLE);
                        textViewMensaje1.setText("PAGO DE FACTURAS");
                        textViewMensaje2.setText("VALOR PAGO DE FACTURAS");
                        cantidadFactura.setEnabled(false);
                        totalVenta = Long.toString(datosRecaudo.getValorFactura());
                        cantidadFactura.setText(totalVenta);
                        datosRecaudo.setValorApagar(totalVenta);
                        sDialog.cancel();
                    }
                })
                .show();
    }

    public void verificarValorCapturado() {

        long valorIngresado = 0;
        if ((datosRecaudo.getIndicadorBaseDatos() == BASE_DATOS
                || (datosRecaudo.getIndicadorBaseDatos() == NO_BASE_DATOS
                && datosRecaudo.getTipoRecaudo().equals(INDICADOR_LECTOR)))) {

            valorIngresado = Long.parseLong(cantidadFactura.getText().toString());

            switch (datosRecaudo.getIndicadorValor()) {
                case PAGO_VALOR_EXACTO:
                case PAGO_CERO:
                    if (datosRecaudo.getIndicadorBaseDatos() == NO_BASE_DATOS) {
                        montoCorrecto = true;
                    }
                    break;

                case PAGO_MENOR_NO_CERO:
                    if (valorIngresado <= datosRecaudo.getValorFactura() && valorIngresado != 0) {
                        montoCorrecto = true;
                    }
                    break;

                case PAGO_MENOR_O_CERO:
                    if (valorIngresado <= datosRecaudo.getValorFactura() && valorIngresado >= 0) {
                        montoCorrecto = true;
                    }
                    break;

                case PAGO_MAYOR:
                    if (valorIngresado >= datosRecaudo.getValorFactura()) {
                        montoCorrecto = true;
                    }
                    break;

                case PAGO_CUALQUIER_NO_CERO:
                    if (valorIngresado > 0) {
                        montoCorrecto = true;
                    }
                    break;

                case PAGO_CUALQUIER_O_CERO:
                    if (valorIngresado >= 0) {
                        montoCorrecto = true;
                    }
                    break;
                case PAGO_MAYOR_O_CERO:
                    if (valorIngresado == 0 || valorIngresado >= datosRecaudo.getValorFactura()) {
                        montoCorrecto = true;
                    }
                    break;
                default:
                    break;
            }
        } else {
            montoCorrecto = true;
        }

    }

    public void confirmarFactura(View view) {

            verificarValorCapturado();

            if (montoCorrecto == true || montoFijo == true) {
                datosRecaudo.setValorApagar(cantidadFactura.getText().toString());
                confirmarPago2();
            } else {
                String mensaje = valFactura + " " + mensaje1 + mensaje2;
                confirmarCambioValor(" VALOR PAGO DE FACTURAS ", mensaje, 1);
            }
    }

    public void confirmarPago2() {
        String valorConvenio = datosRecaudo.getCodigoConvenio();
        String valorReferencia = datosRecaudo.getNumeroFactura();
        String desc = "Confirme los datos de la factura";
        String monto = datosRecaudo.getValorApagar();
        String nombreConvenio = datosRecaudo.getNombreConvenio();

        String titulo = getString(R.string.popup_titulo);

            if(isLector){
                String [] titulos = {"Valor"};
                valores.add(monto);
                iconos.add(1);
                //Intent a confirmar valores

                //String[][] valores = {{"valor", monto}};
                //confirmarValoresRecaudo.showPopupWindow(vista, titulo, desc, valores, ProcesarActivity.class, Procesos.CORRESPONSAL_PAGO_FACTURA_MANUAL, datosRecaudo);
            } else {
                String [] titulos = {"Nº convenio","valor","Nº referencia","nombreConvenio"};
                valores.add(valorConvenio);
                valores.add(monto);
                valores.add(valorReferencia);
                valores.add(nombreConvenio);


                iconos.add(3);
                iconos.add(1);
                iconos.add(3);
                iconos.add(3);
                //Intent a confirmar valores


                //String[][] valores = {{"Nº convenio", valorConvenio}, {"valor", monto}, {"Nº referencia", valorReferencia}, {"nombreConvenio", nombreConvenio}};
                //confirmarValoresRecaudo.showPopupWindow(vista, titulo, desc, valores, ProcesarActivity.class, Procesos.CORRESPONSAL_PAGO_FACTURA_MANUAL, datosRecaudo);
            }




    }




}