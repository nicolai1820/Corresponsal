package co.com.netcom.corresponsal.pantallas.funciones;

import android.content.Context;
import android.content.Intent;
import android.content.res.Resources;
import android.util.Log;

import androidx.appcompat.app.AppCompatActivity;

import java.util.ArrayList;

import co.com.netcom.corresponsal.R;
import co.com.netcom.corresponsal.core.comunicacion.CardDTO;
import co.com.netcom.corresponsal.core.comunicacion.DatosComision;
import co.com.netcom.corresponsal.core.comunicacion.DatosRecaudo;
import co.com.netcom.corresponsal.core.comunicacion.IntegradorC;
import co.com.netcom.corresponsal.core.comunicacion.ParametrosC;
import co.com.netcom.corresponsal.pantallas.comunes.pantallaConfirmacion.pantallaConfirmacion;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.giros.cancelar.pantallaCancelarGiroCantidad;
import co.com.netcom.corresponsal.pantallas.corresponsal.usuarioComun.transacciones.recargas.pantallaRecargaCantidad;

public class Hilos extends AppCompatActivity {

    //switch case con las diferentes transacciones
    private IntegradorC integradorC;
    private String[] respuestaTransacion = new String[4];
    private Context context;
    private  String resultado = null;
/*    private String FIID ="0003";
    private String panVirtual = "8900720000658849";
    private String tipoDeCuenta ="10";*/

    public  Hilos(Context contexto){
        this.integradorC =new IntegradorC(contexto);
      /*  integradorC.cargarInformacionPanVirtual(FIID, tipoDeCuenta,panVirtual);
        String dataEscribir = "089054220178" + ";" + "12345678" + ";" + "B486AFD857A7";
        Log.d("DataFile", "data File " + dataEscribir);
        integradorC.escribirFile(dataEscribir, dataEscribir.length());//DATOS NECESARIOS PARA ARMAR TOKENP58 //ipactual dispositivo, numero de serie, macaddress
        integradorC.setearParametrosC(ParametrosC.NUMERO_TERMINAL,"OCP00014");// se debe asignar al C el numero de terminal correspondiente*/
        this.context =contexto;
    }

    /**Metodo de tipo void que se encarga de procesar todas las transacciones sin tarjeta, excepto pago tarjeta empresarial, recibo como parametro el codigo de la transacccion
     * y un arreglo con los datos.*/
    public String transaccionesSinTarjeta(int transaccion, ArrayList datos,String FIID, String tipoDeCuenta, String panVirtual){
        Log.d("DATA FINAL",datos.toString());
        Log.d("FIID",FIID);
        Log.d("tipoDeCuenta",tipoDeCuenta);
        Log.d("panVirtual",panVirtual);

        switch (transaccion){

            case CodigosTransacciones.CORRESPONSAL_DEPOSITO:{

                cargarInformacionPanVirtual(FIID,tipoDeCuenta,panVirtual);
                                                        //Numero de cuenta, cantidad, codigo tipo de cuenta
                resultado = integradorC.enviarDeposito(datos.get(1).toString(),datos.get(2).toString(),Integer.parseInt(datos.get(3).toString()));
                Log.d("RESPUESTA_HILOS",resultado);
                return resultado;
            }

            //Necesitan PAN Virtual
            case CodigosTransacciones.CORRESPONSAL_RETIRO_SIN_TARJETA:{


                    cargarInformacionPanVirtual(FIID,tipoDeCuenta,panVirtual);

                    //cantidad, numer cuenta, pinblock
                    resultado = integradorC.enviarRetiroSinTarjeta(datos.get(1).toString(), datos.get(0).toString(), datos.get(2).toString());
                    //Hacer intent dependiendo de la respuesta


                return resultado;


            }

            //Listo
            case CodigosTransacciones.CORRESPONSAL_PAGO_PRODUCTOS:{

                    cargarInformacionPanVirtual(FIID,tipoDeCuenta,panVirtual);

                                    //Cartera numero pagare//numero pagare, cantidad, origen
                                    //Tarjeta crédito// numero tarjeta, cantidad, orign
                    resultado = integradorC.enviarPagoPoducto(datos.get(1).toString(),datos.get(2).toString(),Integer.parseInt(datos.get(0).toString()));
                    Log.d("Case 2", "resultado " + resultado);
                    //Hacer intent dependiendo de la respuesta

                return resultado;


            }

            //Listo  creo
            case CodigosTransacciones.CORRESPONSAL_RECLAMACION_GIRO:{

                    cargarInformacionPanVirtual(FIID,tipoDeCuenta,panVirtual);

                                                                    //numero de referencia, tipo documento beneficiario, numero documento beneficiario, monto a reclamar.
                    resultado = integradorC.realizarReclamacionGiro(datos.get(3).toString(),datos.get(4).toString(),datos.get(1).toString(),datos.get(2).toString());
                    //Hacer intent dependiendo de la respuesta

                    Log.d("RESULTADO RECLAMAR GIRO",resultado);

                return resultado;

            }

            //Directo listo
            case CodigosTransacciones.CORRESPONSAL_ENVIO_GIRO:{

                    cargarInformacionPanVirtual(FIID,tipoDeCuenta,panVirtual);



                   DatosComision datosComision = new DatosComision();
                    datosComision.setValorComision(datos.get(1).toString());
                    datosComision.setValorIvaComision(datos.get(3).toString());
                                                                //datos comision, tipo documento girador, tipo documento beneficiario, numero documento girador, numero documento beneficiario, total giro, numero celular girador, numero celular beneficiario
                    resultado = integradorC.realizarEnvioGiroCnb(datosComision, datos.get(10).toString(), datos.get(11).toString(), datos.get(5).toString(), datos.get(8).toString(), datos.get(2).toString(), datos.get(6).toString(), datos.get(7).toString());
                    Log.d("Case 2", "resultado " + resultado);
                    //Hacer intent dependiendo de la respuesta


                return resultado;

            }

            //Listo
            case CodigosTransacciones.CORRESPONSAL_PAGO_FACTURA_TARJETA_EMPRESARIAL:{

                    cargarInformacionPanVirtual(FIID,tipoDeCuenta,panVirtual);


                                                                            //cantidad, track2
                    resultado = integradorC.enviarRecaudoTarjetaEmpresarial(datos.get(0).toString(),datos.get(1).toString());
                    Log.d("TARJETA EMPRESARIAL", "resultado " + resultado);
                    //Hacer intent dependiendo de la respuesta


                return resultado;

            }

            //No esta
            case CodigosTransacciones.CORRESPONSAL_PAGO_FACTURAS_LECTOR:{

                    cargarInformacionPanVirtual(FIID,tipoDeCuenta,panVirtual);



                    //resultado = integradorC.enviarDeposito(datos[0],datos[1],Integer.parseInt(datos[3]));
                    Log.d("Case 2", "resultado " + resultado);


                return resultado;

            }

            //Listo
            case CodigosTransacciones.CORRESPONSAL_CANCELACION_GIRO_CONSULTA:{

                    cargarInformacionPanVirtual(FIID,tipoDeCuenta,panVirtual);

                    //Se crea un objeto de tipo datos recaudo.

                    DatosComision datosComision = new DatosComision();
                                                                            //datosComision,tipo documento girador, tipo documento beneficiario, numero documento girador, numero documento beneficiario, pin referencia
                    resultado = integradorC.realizaConsultaCancelacionGiroCnb(datosComision,datos.get(5).toString(),datos.get(6).toString(),datos.get(1).toString(),datos.get(3).toString(),datos.get(4).toString());

                    if (resultado == null) {
                        //PANTALLA RESULTADO TRANSACCION
                        return resultado;
                    } else {
                        return resultado;

                       /* respuestaTransacion = resultado.split(";");
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
                        }*/
                    }

            }

            //Listo para Nequi
    /*        case CodigosTransacciones.CORRESPONSAL_CONSULTA_FACTURAS:{



                    cargarInformacionPanVirtual(FIID,tipoDeCuenta,panVirtual);



                    DatosRecaudo datosRecaudo = new DatosRecaudo();


                    //Recargas Celular
                    if(datos.get(2).toString().equals("NEQUI")){

                            datosRecaudo.setCodigoConvenio(datos.get(2).toString());
                            datosRecaudo.setNumeroFactura(datos.get(1).toString());
                            datosRecaudo.setTipoRecaudo(datos.get(0).toString());
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
                            *//*Intent intento = new Intent(getApplicationContext(), CorresponsalFacturasPagoActivity.class);
                            if (valores[2].equals("NEQUI")) {
                                intento.putExtra("celular", valores[1]);
                                intento.putExtra("isNequi", true);

                            }
                            Log.d(TAG, "recaudo  " + datosRecaudo.getValorFactura());
                            intento.putExtra("datosRecaudo", datosRecaudo);
                            startActivity(intento);*//*
                            } else {
                                //PANTALLA RESULTADO TRANSACCION
                                // resultadoTransaccion(resultado);
                            }
                        }

                    }
                    else{
                        //Facturas Normales

                        datosRecaudo.setCuentaRecaudadora(datos.get(0).toString());
                        datosRecaudo.setNumeroFactura(datos.get(1).toString());
                        datosRecaudo.setTipoRecaudo(datos.get(2).toString());
                        resultado = integradorC.enviarConsultaRecaudoManual(datosRecaudo);
                        if (resultado == null) {
                            //PANTALLA RESULTADO TRANSACCION
                            //mostrarResultado(false, "PAGO FACTURA MANUAL ERROR", resultado, false);
                        } else {
                            if (resultado.equals("OK")) {
                                //INTENT A ACTIVITY cantidad factura
                                //Intent intento = new Intent(getApplicationContext(), CorresponsalFacturasPagoActivity.class);

                                //Log.d(TAG, "recaudo  " + datosRecaudo.getValorFactura());
                                //intento.putExtra("datosRecaudo", datosRecaudo);
                                //startActivity(intento);
                            } else {
                                //PANTALLA RESULTADO TRANSACCION
                                 //resultadoTransaccion(resultado);
                            }
                        }
                    }





                break;
            }*/
            default:
                return resultado;
        }


    }

    /**Metodo de tipo void que se encarga de hacer la cancelación de un giro, recibe como parametros un arreglo con los datos y un objeto
     * de tipo DatosComision*/
    public void cancelarGiro(ArrayList datos, DatosComision datosComision,String FIID, String tipoDeCuenta, String panVirtual){

        new Thread(() -> {
            cargarInformacionPanVirtual(FIID,tipoDeCuenta,panVirtual);

            String resultado = null;
            Log.d("HILO CANCELAR", "LLegue a la clase hilos");
                                                            //datos comision, tipo documento girador, tipo documento beneficiario,,numero documento girador, numero documento beneficiario
            resultado = integradorC.realizarCancelacionGiro(datosComision,datos.get(5).toString(),datos.get(6).toString(),datos.get(1).toString(),datos.get(3).toString());
            //Hacer intent dependiendo de la respuesta

            Log.d("RESULT",resultado);
        }).start();

    }

    /**Metodo de tipo void que se encarga del pago de facutras, recibo como parametro un objeto de tipo datos recuado*/
    public void pagoFacturaManual(DatosRecaudo datosRecaudo,String FIID, String tipoDeCuenta, String panVirtual){

        new Thread(() -> {
            cargarInformacionPanVirtual(FIID,tipoDeCuenta,panVirtual);

            String resultado = null;
            Log.d("HILO CANCELAR", "LLegue a la clase hilos");
            resultado = integradorC.enviarPagoRecaudoLector(datosRecaudo.getNumeroFactura(), datosRecaudo);
            //Hacer intent dependiendo de la respuesta

            Log.d("RESULT",resultado);
        }).start();

    }

    /**Metodo de tipo void que se encarga de las transacciones con tarjeta, recibo como parametros el codigo de la transaccion,
     * un arreglo con los datos y un objeto de tipo CardDTO*/
    public String transaccionesConTarjeta(int transaccion, ArrayList datos, CardDTO tarjeta,String FIID, String tipoDeCuenta, String panVirtual){

        switch (transaccion){
            //Listo
            case CodigosTransacciones.CORRESPONSAL_CONSULTA_SALDO:{

                    String resultado = null;
                    cargarInformacionPanVirtual(FIID,tipoDeCuenta,panVirtual);
                    resultado = integradorC.enviarConsultaSaldoBclObj(tarjeta);
                    Log.d("CONSULTA SALDO", "resultado " + resultado);
                    return resultado;

            }


            //Listo
            case CodigosTransacciones.CORRESPONSAL_RETIRO_CON_TARJETA:{

                    String resultado = null;
                    int otraCuenta;
                    String numeroCuenta;

                    //Si el arreglo es mayor a 2, se escogio otra cuenta.
                    if (datos.size()>3){
                         otraCuenta = 1;
                         numeroCuenta = datos.get(2).toString();
                    }else{
                        otraCuenta = 0;
                        numeroCuenta = "0";
                    }
                    cargarInformacionPanVirtual(FIID,tipoDeCuenta,panVirtual);
                                                                    //objeto CardDTO, cantidad, otra cuenta, numero cuenta
                    resultado = integradorC.enviarRetiroTarjetaBCLObj(tarjeta, datos.get(0).toString(), otraCuenta, numeroCuenta);
                    //Hacer intent dependiendo de la respuesta
                    Log.d("RETIRO CON TARJETA", "resultado " + resultado);
                    return resultado;
            }

            //Listo
            case CodigosTransacciones.CORRESPONSAL_TRANSFERENCIA:{

                    String resultado = null;
                    int otraCuentaOrigen;
                    String numeroCuentaOrigen;

                    cargarInformacionPanVirtual(FIID,tipoDeCuenta,panVirtual);

                    if (datos.size()>6){
                        otraCuentaOrigen=1;
                        numeroCuentaOrigen = datos.get(4).toString();
                        //objeto CardDTO, tipo cuenta origen, codigo cuenta destino,numero cuenta destino,numero cuenta origen, cantidad
                        resultado = integradorC.enviarTransferencia(tarjeta, "10", datos.get(5).toString(), datos.get(1).toString(), numeroCuentaOrigen, datos.get(2).toString());
                    }else{
                        otraCuentaOrigen = 0;
                        numeroCuentaOrigen = "00000";
                        //objeto CardDTO, tipo cuenta origen, codigo cuenta destino,numero cuenta destino,numero cuenta origen, cantidad
                        resultado = integradorC.enviarTransferencia(tarjeta, "10", datos.get(4).toString(), datos.get(1).toString(), numeroCuentaOrigen, datos.get(2).toString());
                    }

                    //Hacer intent dependiendo de la respuesta


                    Log.d("Transferencia", "resultado " + resultado);
                    return resultado;


            }

            default:
                return "";


        }


    }

    public void cargarInformacionPanVirtual(String FIID, String tipoDeCuenta, String panVirtual){
        DeviceInformation deviceInformation= new DeviceInformation(context);
        Log.d("IP",deviceInformation.getIpAddress());
        Log.d("MAC",deviceInformation.getDeviceUUID().substring(0,8));
        Log.d("UID",deviceInformation.getMacAddress().replaceAll(":","").substring(0,8));

        integradorC.cargarInformacionPanVirtual(FIID, tipoDeCuenta,panVirtual);
        String dataEscribir = deviceInformation.getIpAddress().replaceAll(".","") + ";" + deviceInformation.getDeviceUUID().substring(0,8) + ";" + deviceInformation.getMacAddress().replaceAll(":","").substring(0,8);
        Log.d("DataFile", "data File " + dataEscribir);
        integradorC.escribirFile(dataEscribir, dataEscribir.length());//DATOS NECESARIOS PARA ARMAR TOKENP58 //ipactual dispositivo, numero de serie, macaddress
        integradorC.setearParametrosC(ParametrosC.NUMERO_TERMINAL,"OCP00014");// se debe asignar al C el numero de terminal correspondiente*/
    }

}
