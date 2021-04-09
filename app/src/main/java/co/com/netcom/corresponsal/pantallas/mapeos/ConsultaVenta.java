package co.com.netcom.corresponsal.pantallas.mapeos;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;

public class ConsultaVenta {

    private String  descripcionRespuesta;
    private String  nombreComercio;
    private String  ksn;
    private String  valorCompra;
    private String  fechaGeneracion;
    private String  valorBaseDevolucion;
    private String  terminalId;
    private String  origen;
    private String  usuarioId;
    private String  valorIva;
    private String  codigoAprobacion;
    private String  anuladoIndicador;
    private String  valorInc;
    private String  cuotas;
    private String  track2;
    private String  codigo;
    private String  comercioId;
    private String  fechaVencimiento;
    private String  valorAdicional;
    private String  consecutivo;
    private String  fecha;
    private String  movimientoId;
    private String  tipoCuenta;
    private String  posEntryMode;
    private String  tipoTransaccion;
    private String  codigoRespuesta;

    public String getDescripcionRespuesta() {
        return descripcionRespuesta;
    }

    public void setDescripcionRespuesta(String descripcionRespuesta) {
        this.descripcionRespuesta = descripcionRespuesta;
    }

    public String getNombreComercio() {
        return nombreComercio;
    }

    public void setNombreComercio(String nombreComercio) {
        this.nombreComercio = nombreComercio;
    }

    public String getKsn() {
        return ksn;
    }

    public void setKsn(String ksn) {
        this.ksn = ksn;
    }

    public String getValorCompra() {
        return valorCompra;
    }

    public void setValorCompra(String valorCompra) {
        this.valorCompra = valorCompra;
    }

    public String getFechaGeneracion() {
        return fechaGeneracion;
    }

    public void setFechaGeneracion(String fechaGeneracion) {
        this.fechaGeneracion = fechaGeneracion;
    }

    public String getValorBaseDevolucion() {
        return valorBaseDevolucion;
    }

    public void setValorBaseDevolucion(String valorBaseDevolucion) {
        this.valorBaseDevolucion = valorBaseDevolucion;
    }

    public String getTerminalId() {
        return terminalId;
    }

    public void setTerminalId(String terminalId) {
        this.terminalId = terminalId;
    }

    public String getOrigen() {
        return origen;
    }

    public void setOrigen(String origen) {
        this.origen = origen;
    }

    public String getUsuarioId() {
        return usuarioId;
    }

    public void setUsuarioId(String usuarioId) {
        this.usuarioId = usuarioId;
    }

    public String getValorIva() {
        return valorIva;
    }

    public void setValorIva(String valorIva) {
        this.valorIva = valorIva;
    }

    public String getCodigoAprobacion() {
        return codigoAprobacion;
    }

    public void setCodigoAprobacion(String codigoAprobacion) {
        this.codigoAprobacion = codigoAprobacion;
    }

    public String getAnuladoIndicador() {
        return anuladoIndicador;
    }

    public void setAnuladoIndicador(String anuladoIndicador) {
        this.anuladoIndicador = anuladoIndicador;
    }

    public String getValorInc() {
        return valorInc;
    }

    public void setValorInc(String valorInc) {
        this.valorInc = valorInc;
    }

    public String getCuotas() {
        return cuotas;
    }

    public void setCuotas(String cuotas) {
        this.cuotas = cuotas;
    }

    public String getTrack2() {
        return track2;
    }

    public void setTrack2(String track2) {
        this.track2 = track2;
    }

    public String getCodigo() {
        return codigo;
    }

    public void setCodigo(String codigo) {
        this.codigo = codigo;
    }

    public String getComercioId() {
        return comercioId;
    }

    public void setComercioId(String comercioId) {
        this.comercioId = comercioId;
    }

    public String getFechaVencimiento() {
        return fechaVencimiento;
    }

    public void setFechaVencimiento(String fechaVencimiento) {
        this.fechaVencimiento = fechaVencimiento;
    }

    public String getValorAdicional() {
        return valorAdicional;
    }

    public void setValorAdicional(String valorAdicional) {
        this.valorAdicional = valorAdicional;
    }

    public String getConsecutivo() {
        return consecutivo;
    }

    public void setConsecutivo(String consecutivo) {
        this.consecutivo = consecutivo;
    }

    public String getFecha() {
        return fecha;
    }

    public void setFecha(String fecha) {
        this.fecha = fecha;
    }

    public String getMovimientoId() {
        return movimientoId;
    }

    public void setMovimientoId(String movimientoId) {
        this.movimientoId = movimientoId;
    }

    public String getTipoCuenta() {
        return tipoCuenta;
    }

    public void setTipoCuenta(String tipoCuenta) {
        this.tipoCuenta = tipoCuenta;
    }

    public String getPosEntryMode() {
        return posEntryMode;
    }

    public void setPosEntryMode(String posEntryMode) {
        this.posEntryMode = posEntryMode;
    }

    public String getTipoTransaccion() {
        return tipoTransaccion;
    }

    public void setTipoTransaccion(String tipoTransaccion) {
        this.tipoTransaccion = tipoTransaccion;
    }

    public String getCodigoRespuesta() {
        return codigoRespuesta;
    }

    public void setCodigoRespuesta(String codigoRespuesta) {
        this.codigoRespuesta = codigoRespuesta;
    }


    public ConsultaVenta fromJsonMap(JSONObject json) throws JSONException {
        ConsultaVenta venta = new ConsultaVenta();

        venta.setDescripcionRespuesta(json.getString("descripcionRespuesta"));
        venta.setNombreComercio(json.getString("nombreComercio"));
        venta.setKsn(json.getString("ksn"));
        venta.setValorCompra(json.getString("valorCompra"));
        venta.setFechaGeneracion(json.getString("fechaGeneracion"));
        venta.setValorBaseDevolucion(json.getString("valorBaseDevolucion"));
        venta.setTerminalId(json.getString("terminalId"));
        venta.setOrigen(json.getString("origen"));
        venta.setUsuarioId(json.getString("usuarioId"));
        venta.setValorIva(json.getString("valorIva"));
        venta.setCodigoAprobacion(json.getString("codigoAprobacion"));
        venta.setAnuladoIndicador(json.getString("anuladoIndicador"));
        venta.setValorInc(json.getString("valorInc"));
        venta.setCuotas(json.getString("cuotas"));
        venta.setTrack2(json.getString("track2"));
        venta.setCodigo(json.getString("codigo"));
        venta.setComercioId(json.getString("comercioId"));
        venta.setFechaVencimiento(json.getString("fechaVencimiento"));
        venta.setValorAdicional(json.getString("valorAdicional"));
        venta.setConsecutivo(json.getString("consecutivo"));
        venta.setFecha(json.getString("fecha"));
        venta.setMovimientoId(json.getString("movimientoId"));
        venta.setTipoCuenta(json.getString("tipoCuenta"));
        venta.setPosEntryMode(json.getString("posEntryMode"));
        venta.setTipoTransaccion(json.getString("tipoTransaccion"));
        venta.setCodigoRespuesta(json.getString("codigoRespuesta"));

        return venta;
    }



    public ArrayList<ConsultaVenta> fromJsonList(JSONObject [] listaInicial ){

        ArrayList<ConsultaVenta> ventas = new ArrayList<ConsultaVenta>();

        for (int i = 0; i<listaInicial.length;i++){

            ConsultaVenta consulta = new ConsultaVenta();
            try {
                consulta = fromJsonMap(listaInicial[i]);
            } catch (JSONException e) {
                e.printStackTrace();
            }

            ventas.add(consulta);
        }

        return ventas;
    }












}
