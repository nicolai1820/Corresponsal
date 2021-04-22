package co.com.netcom.corresponsal.pantallas.transacciones;

public class Transaccion {

    private String nombreTransaccion;
    private String fechaTransaccion;
    private String codigoTransaccion;
    private String montoTransaccion;

    public Transaccion(String nombreTransaccion, String fechaTransaccion, String codigoTransaccion, String montoTransaccion) {
        this.nombreTransaccion = nombreTransaccion;
        this.fechaTransaccion = fechaTransaccion;
        this.codigoTransaccion = codigoTransaccion;
        this.montoTransaccion = montoTransaccion;
    }


    public String getNombreTransaccion() {
        return nombreTransaccion;
    }

    public void setNombreTransaccion(String nombreTransaccion) {
        this.nombreTransaccion = nombreTransaccion;
    }

    public String getFechaTransaccion() {
        return fechaTransaccion;
    }

    public void setFechaTransaccion(String fechaTransaccion) {
        this.fechaTransaccion = fechaTransaccion;
    }

    public String getCodigoTransaccion() {
        return codigoTransaccion;
    }

    public void setCodigoTransaccion(String codigoTransaccion) {
        this.codigoTransaccion = codigoTransaccion;
    }

    public String getMontoTransaccion() {
        return montoTransaccion;
    }

    public void setMontoTransaccion(String montoTransaccion) {
        this.montoTransaccion = montoTransaccion;
    }
}
