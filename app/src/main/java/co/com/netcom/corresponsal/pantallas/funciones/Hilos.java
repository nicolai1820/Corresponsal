package co.com.netcom.corresponsal.pantallas.funciones;

import android.content.Context;
import android.content.Intent;
import android.content.res.Resources;
import android.util.Log;

import androidx.appcompat.app.AppCompatActivity;

import java.util.ArrayList;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.core.comunicacion.DatosComision;
import co.com.netcom.corresponsal.core.comunicacion.DatosRecaudo;
import co.com.netcom.corresponsal.core.comunicacion.IntegradorC;
import co.com.netcom.corresponsal.pantallas.comunes.pantallaConfirmacion.pantallaConfirmacion;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.giros.cancelar.pantallaCancelarGiroCantidad;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.recargas.pantallaRecargaCantidad;

public class Hilos extends AppCompatActivity {

//switch case con las diferentes transacciones
private IntegradorC integradorC;
private String[] respuestaTransacion = new String[4];
private Context context;

public  Hilos(Context contexto){
    this.integradorC =new IntegradorC(contexto);
    this.context =contexto;
}

public void transacciones(int transaccion, ArrayList datos){

    switch (transaccion){

        //Listo
        case CodigosTransacciones.CORRESPONSAL_DEPOSITO:{
           new Thread(() -> {
                String resultado = null;

               resultado = integradorC.enviarDeposito(datos.get(0).toString(),datos.get(1).toString(),Integer.parseInt(datos.get(2).toString()));

               Log.d("RESULTADO DEPOSITO",resultado);
            }).start();

            /*try {
                Thread.sleep(10000);
                Log.d("Hilo"," Termino Hilo");


            }catch (Exception e){
                Log.d("Error",e.toString());
            }*/
            break;
        }

        //Necesitan PAN Virtual
        case CodigosTransacciones.CORRESPONSAL_RETIRO_SIN_TARJETA:{
            new Thread(() -> {
                String resultado = null;

                //funcion en c retiro con tarjeta
                Log.d("Case 2", "resultado " + resultado);

            }).start();
            break;
        }

        case CodigosTransacciones.CORRESPONSAL_RETIRO_CON_TARJETA:{
            new Thread(() -> {
                String resultado = null;

                ////funcion en c retiro con tarjeta
                Log.d("Case 2", "resultado " + resultado);

            }).start();
            break;
        }

        case CodigosTransacciones.CORRESPONSAL_CONSULTA_SALDO:{
            new Thread(() -> {
                String resultado = null;

                //resultado = integradorC.enviarDeposito(datos[0],datos[1],Integer.parseInt(datos[3]));
                Log.d("Case 2", "resultado " + resultado);

            }).start();
            break;
        }

        case CodigosTransacciones.CORRESPONSAL_TRANSFERENCIA:{
            new Thread(() -> {
                String resultado = null;

                //resultado = integradorC.enviarDeposito(datos[0],datos[1],Integer.parseInt(datos[3]));
                Log.d("Case 2", "resultado " + resultado);

            }).start();
            break;
        }

        //Listo
        case CodigosTransacciones.CORRESPONSAL_PAGO_PRODUCTOS:{
            new Thread(() -> {
                String resultado = null;

                resultado = integradorC.enviarPagoPoducto(datos.get(1).toString(),datos.get(2).toString(),Integer.parseInt(datos.get(0).toString()));
                Log.d("Case 2", "resultado " + resultado);

            }).start();
            break;
        }

        //Necesitan PAN Virtual
        case CodigosTransacciones.CORRESPONSAL_RECARGAS:{
            new Thread(() -> {
                String resultado = null;

                //resultado = integradorC.enviarDeposito(datos[0],datos[1],Integer.parseInt(datos[3]));
                Log.d("Case 2", "resultado " + resultado);

            }).start();
            break;
        }

        //Listo  creo
        case CodigosTransacciones.CORRESPONSAL_RECLAMACION_GIRO:{
            new Thread(() -> {
                String resultado = null;

                resultado = integradorC.realizarReclamacionGiro(datos.get(2).toString(),datos.get(3).toString(),datos.get(0).toString(),datos.get(1).toString());

                Log.d("RESULTADO RECLAMAR GIRO",resultado);
            }).start();
            break;
        }

        //Directo listo
        case CodigosTransacciones.CORRESPONSAL_ENVIO_GIRO:{
            new Thread(() -> {
                String resultado = null;

               DatosComision datosComision = new DatosComision();
                datosComision.setValorComision(datos.get(1).toString());
                datosComision.setValorIvaComision(datos.get(3).toString());
                resultado = integradorC.realizarEnvioGiroCnb(datosComision, datos.get(8).toString(), datos.get(9).toString(), datos.get(4).toString(), datos.get(6).toString(), datos.get(2).toString(), datos.get(5).toString(), datos.get(7).toString());
                Log.d("Case 2", "resultado " + resultado);

            }).start();
            break;
        }

        case CodigosTransacciones.CORRESPONSAL_PAGO_FACTURA_TARJETA_EMPRESARIAL:{
            new Thread(() -> {
                String resultado = null;

                //resultado = integradorC.enviarDeposito(datos[0],datos[1],Integer.parseInt(datos[3]));
                Log.d("Case 2", "resultado " + resultado);

            }).start();
            break;
        }

        case CodigosTransacciones.CORRESPONSAL_PAGO_FACTURAS_LECTOR:{
            new Thread(() -> {
                String resultado = null;

                //resultado = integradorC.enviarDeposito(datos[0],datos[1],Integer.parseInt(datos[3]));
                Log.d("Case 2", "resultado " + resultado);

            }).start();
            break;
        }

        //Listo
        case CodigosTransacciones.CORRESPONSAL_CANCELACION_GIRO_CONSULTA:{
            new Thread(() -> {
                String resultado = null;
                //Se crea un objeto de tipo datos recaudo.

                DatosComision datosComision = new DatosComision();
                resultado = integradorC.realizaConsultaCancelacionGiroCnb(datosComision,datos.get(3).toString(),datos.get(4).toString(),datos.get(0).toString(),datos.get(1).toString(),datos.get(2).toString());

                if (resultado == null) {
                    //PANTALLA RESULTADO TRANSACCION
                } else {
                    respuestaTransacion = resultado.split(";");
                    if (respuestaTransacion[0].equals("00")) {

                        Intent intento = new Intent(context, pantallaCancelarGiroCantidad.class);
                        intento.putExtra("valorGiro", respuestaTransacion[1]);
                        intento.putExtra("valores", datos);
                        intento.putExtra("datosComision", datosComision);
                        intento.putExtra("transaccion",CodigosTransacciones.CORRESPONSAL_CANCELACION_GIRO);
                        context.startActivity(intento);

                    } else {
                        //PANTALLA RESULTADO TRANSACCION

                        //resultadoTransaccion(resultado);
                    }
                }

            }).start();
            break;
        }

        case CodigosTransacciones.CORRESPONSAL_CONSULTA_FACTURAS:{

            new Thread(() -> {
                String resultado = null;

                DatosRecaudo datosRecaudo = new DatosRecaudo();

                //Recargas Celular

                if(datos.get(0).toString().equals("NEQUI")){
                    datosRecaudo.setCodigoConvenio(datos.get(0).toString());
                    datosRecaudo.setNumeroFactura(datos.get(2).toString());
                    datosRecaudo.setTipoRecaudo(datos.get(1).toString());
                    resultado = integradorC.enviarConsultaRecaudoManual(datosRecaudo);

                    if (resultado == null) {
                        //PANTALLA RESULTADO TRANSACCION
                        //mostrarResultado(false, "PAGO FACTURA MANUAL ERROR", resultado, false);
                    } else {
                        if (resultado.equals("OK")) {

                            Intent i = new Intent(getApplicationContext(), pantallaRecargaCantidad.class);

                            i.putExtra("valores",datos);
                            i.putExtra("datosRecaudo",datosRecaudo);
                            startActivity(i);


                            //INTENT A ACTIVITY DONDE SE HACE LA RECARGA A NEQUI
                        /*Intent intento = new Intent(getApplicationContext(), CorresponsalFacturasPagoActivity.class);
                        if (valores[2].equals("NEQUI")) {
                            intento.putExtra("celular", valores[1]);
                            intento.putExtra("isNequi", true);

                        }
                        Log.d(TAG, "recaudo  " + datosRecaudo.getValorFactura());
                        intento.putExtra("datosRecaudo", datosRecaudo);
                        startActivity(intento);*/
                        } else {
                            //PANTALLA RESULTADO TRANSACCION
                            // resultadoTransaccion(resultado);
                        }
                    }

                }else{

                    //  if (datos.get(2).equals("00") || datos.get(2).equals("NEQUI")) {
                    //  datosRecaudo.setCodigoConvenio(datos.get(0).toString());
                    // } else {
                    //    datosRecaudo.setCuentaRecaudadora(datos.get(0).toString());
                    //}
                    // datosRecaudo.setNumeroFactura(datos.get(1).toString());
                    // datosRecaudo.setTipoRecaudo(datos.get(2).toString());
                    //resultado = integradorC.enviarConsultaRecaudoManual(datosRecaudo);
                    //if (resultado == null) {
                        //PANTALLA RESULTADO TRANSACCION
                        //mostrarResultado(false, "PAGO FACTURA MANUAL ERROR", resultado, false);
                   // } else {
                     //   if (resultado.equals("OK")) {
                            //INTENT A ACTIVITY DONDE SE HACE LA RECARGA A NEQUI
                        /*Intent intento = new Intent(getApplicationContext(), CorresponsalFacturasPagoActivity.class);
                        if (valores[2].equals("NEQUI")) {
                            intento.putExtra("celular", valores[1]);
                            intento.putExtra("isNequi", true);

                        }
                        Log.d(TAG, "recaudo  " + datosRecaudo.getValorFactura());
                        intento.putExtra("datosRecaudo", datosRecaudo);
                        startActivity(intento);*/
                 //       } else {
                            //PANTALLA RESULTADO TRANSACCION
                            // resultadoTransaccion(resultado);
                   //     }
                   // }
                }

            }).start();

            break;
        }
    }

}

public void cancelarGiro(ArrayList datos, DatosComision datosComision){

    new Thread(() -> {
        String resultado = null;
        Log.d("HILO CANCELAR", "LLegue a la clase hilos");
        resultado = integradorC.realizarCancelacionGiro(datosComision,datos.get(3).toString(),datos.get(4).toString(),datos.get(0).toString(),datos.get(1).toString());

        Log.d("RESULT",resultado);
    }).start();

}

public void pagoFacturaManual(DatosRecaudo datosRecaudo){

    new Thread(() -> {
        String resultado = null;
        Log.d("HILO CANCELAR", "LLegue a la clase hilos");
        resultado = integradorC.enviarPagoRecaudoLector(datosRecaudo.getNumeroFactura(), datosRecaudo);

        Log.d("RESULT",resultado);
    }).start();

}

}
