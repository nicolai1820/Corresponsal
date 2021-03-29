package co.com.netcom.corresponsal.core.comunicacion;

import android.os.Parcel;
import android.os.Parcelable;

public class ParametrosDineroElectronico implements Parcelable {

    private int tipoPinSolicitado;
    private int permiteCuotas;
    private int permiteSeleccionCuenta;
    private int permiteCelular;
    private int permiteDocumento;
    private String itemsTipoDocumento;
    private String tipoDocumento;
    private String codigoEntidad;
    private String nombreBanco;
    private String fiddOtp;
    private String binAsignado;

    public ParametrosDineroElectronico() {
    }

    protected ParametrosDineroElectronico(Parcel in) {
        tipoPinSolicitado = in.readInt();
        permiteCuotas = in.readInt();
        permiteSeleccionCuenta = in.readInt();
        permiteCelular = in.readInt();
        permiteDocumento = in.readInt();
        itemsTipoDocumento = in.readString();
        tipoDocumento = in.readString();
        codigoEntidad = in.readString();
        nombreBanco = in.readString();
        fiddOtp = in.readString();
        binAsignado = in.readString();
    }

    public static final Creator<ParametrosDineroElectronico> CREATOR = new Creator<ParametrosDineroElectronico>() {
        @Override
        public ParametrosDineroElectronico createFromParcel(Parcel in) {
            return new ParametrosDineroElectronico(in);
        }

        @Override
        public ParametrosDineroElectronico[] newArray(int size) {
            return new ParametrosDineroElectronico[size];
        }
    };

    public int getPermiteCuotas() {
        return permiteCuotas;
    }

    public void setPermiteCuotas(int permiteCuotas) {
        this.permiteCuotas = permiteCuotas;
    }

    public int getPermiteSeleccionCuenta() {
        return permiteSeleccionCuenta;
    }

    public void setPermiteSeleccionCuenta(int permiteSeleccionCuenta) {
        this.permiteSeleccionCuenta = permiteSeleccionCuenta;
    }

    public int getPermiteCelular() {
        return permiteCelular;
    }

    public void setPermiteCelular(int permiteCelular) {
        this.permiteCelular = permiteCelular;
    }

    public int getPermiteDocumento() {
        return permiteDocumento;
    }

    public void setPermiteDocumento(int permiteDocumento) {
        this.permiteDocumento = permiteDocumento;
    }

    public String getItemsTipoDocumento() {
        return itemsTipoDocumento;
    }

    public void setItemsTipoDocumento(String itemsTipoDocumento) {
        this.itemsTipoDocumento = itemsTipoDocumento;
    }

    public String getTipoDocumento() {
        return tipoDocumento;
    }

    public void setTipoDocumento(String tipoDocumento) {
        this.tipoDocumento = tipoDocumento;
    }

    public String getCodigoEntidad() {
        return codigoEntidad;
    }

    public void setCodigoEntidad(String codigoEntidad) {
        this.codigoEntidad = codigoEntidad;
    }

    public String getNombreBanco() {
        return nombreBanco;
    }

    public void setNombreBanco(String nombreBanco) {
        this.nombreBanco = nombreBanco;
    }

    public String getFiddOtp() {
        return fiddOtp;
    }

    public void setFiddOtp(String fiddOtp) {
        this.fiddOtp = fiddOtp;
    }

    public String getBinAsignado() {
        return binAsignado;
    }

    public void setBinAsignado(String binAsignado) {
        this.binAsignado = binAsignado;
    }

    public int getTipoPinSolicitado() {
        return tipoPinSolicitado;
    }

    public void setTipoPinSolicitado(int tipoPinSolicitado) {
        this.tipoPinSolicitado = tipoPinSolicitado;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel parcel, int i) {
        parcel.writeInt(tipoPinSolicitado);
        parcel.writeInt(permiteCuotas);
        parcel.writeInt(permiteSeleccionCuenta);
        parcel.writeInt(permiteCelular);
        parcel.writeInt(permiteDocumento);
        parcel.writeString(itemsTipoDocumento);
        parcel.writeString(tipoDocumento);
        parcel.writeString(codigoEntidad);
        parcel.writeString(nombreBanco);
        parcel.writeString(fiddOtp);
        parcel.writeString(binAsignado);
    }
}
