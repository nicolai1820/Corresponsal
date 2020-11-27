package co.com.netcom.corresponsal.core.comunicacion;

public class DatosArchivoPar {
    private String tipoDocumento;
    private String descripcion;
    private String longitudMinima;
    private String longitudMaxima;
    private String tipoIngreso;

    public DatosArchivoPar() {
    }

    public String getTipoDocumento() {
        return tipoDocumento;
    }

    public void setTipoDocumento(String tipoDocumento) {
        this.tipoDocumento = tipoDocumento;
    }

    public String getDescripcion() {
        return descripcion;
    }

    public void setDescripcion(String descripcion) {
        this.descripcion = descripcion;
    }

    public String getLongitudMinima() {
        return longitudMinima;
    }

    public void setLongitudMinima(String longitudMinima) {
        this.longitudMinima = longitudMinima;
    }

    public String getLongitudMaxima() {
        return longitudMaxima;
    }

    public void setLongitudMaxima(String longitudMaxima) {
        this.longitudMaxima = longitudMaxima;
    }

    public String getTipoIngreso() {
        return tipoIngreso;
    }

    public void setTipoIngreso(String tipoIngreso) {
        this.tipoIngreso = tipoIngreso;
    }
}
