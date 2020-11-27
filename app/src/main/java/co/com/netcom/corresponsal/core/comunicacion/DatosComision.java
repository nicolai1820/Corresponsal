package co.com.netcom.corresponsal.core.comunicacion;

import android.os.Parcel;
import android.os.Parcelable;

public class DatosComision implements Parcelable {
    private String montoMinimo;
    private String montoMaximo;
    private String comisionFija;
    private String porcentajeComision;
    private String porcentajeIva;
    private String fechaInicio;
    private String valorComision;
    private String valorIvaComision;
    private String referenciaCancelacion;
    private String referenciaReclamacion;

    public DatosComision() {
    }

    protected DatosComision(Parcel in) {
        montoMinimo = in.readString();
        montoMaximo = in.readString();
        comisionFija = in.readString();
        porcentajeComision = in.readString();
        porcentajeIva = in.readString();
        fechaInicio = in.readString();
        valorComision = in.readString();
        valorIvaComision = in.readString();
        referenciaCancelacion = in.readString();
        referenciaReclamacion = in.readString();
    }

    public static final Creator<DatosComision> CREATOR = new Creator<DatosComision>() {
        @Override
        public DatosComision createFromParcel(Parcel in) {
            return new DatosComision(in);
        }

        @Override
        public DatosComision[] newArray(int size) {
            return new DatosComision[size];
        }
    };

    public String getMontoMinimo() {
        return montoMinimo;
    }

    public void setMontoMinimo(String montoMinimo) {
        this.montoMinimo = montoMinimo;
    }

    public String getMontoMaximo() {
        return montoMaximo;
    }

    public void setMontoMaximo(String montoMaximo) {
        this.montoMaximo = montoMaximo;
    }

    public String getComisionFija() {
        return comisionFija;
    }

    public void setComisionFija(String comisionFija) {
        this.comisionFija = comisionFija;
    }

    public String getPorcentajeComision() {
        return porcentajeComision;
    }

    public void setPorcentajeComision(String porcentajeComision) {
        this.porcentajeComision = porcentajeComision;
    }

    public String getPorcentajeIva() {
        return porcentajeIva;
    }

    public void setPorcentajeIva(String porcentajeIva) {
        this.porcentajeIva = porcentajeIva;
    }

    public String getFechaInicio() {
        return fechaInicio;
    }

    public void setFechaInicio(String fechaInicio) {
        this.fechaInicio = fechaInicio;
    }

    public String getValorComision() {
        return valorComision;
    }

    public void setValorComision(String valorComision) {
        this.valorComision = valorComision;
    }

    public String getValorIvaComision() {
        return valorIvaComision;
    }

    public void setValorIvaComision(String valorIvaComision) {
        this.valorIvaComision = valorIvaComision;
    }

    public String getReferenciaCancelacion() {
        return referenciaCancelacion;
    }

    public void setReferenciaCancelacion(String referenciaCancelacion) {
        this.referenciaCancelacion = referenciaCancelacion;
    }

    public String getReferenciaReclamacion() {
        return referenciaReclamacion;
    }

    public void setReferenciaReclamacion(String referenciaReclamacion) {
        this.referenciaReclamacion = referenciaReclamacion;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel parcel, int i) {
        parcel.writeString(montoMinimo);
        parcel.writeString(montoMaximo);
        parcel.writeString(comisionFija);
        parcel.writeString(porcentajeComision);
        parcel.writeString(porcentajeIva);
        parcel.writeString(fechaInicio);
        parcel.writeString(valorComision);
        parcel.writeString(valorIvaComision);
        parcel.writeString(referenciaCancelacion);
        parcel.writeString(referenciaReclamacion);
    }
}
