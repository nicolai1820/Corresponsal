package co.com.netcom.corresponsal.core.comunicacion;

import android.os.Parcel;
import android.os.Parcelable;

public class DatosRecaudo implements  Parcelable{

    private int indicadorBaseDatos;
    private long valorFactura;
    private int indicadorValor;
    private String tipoRecaudo;
    private String valorApagar;
    private String numeroFactura;
    private String codigoConvenio;
    private String cuentaRecaudadora;
    private String tokenP6;
    private String nombreConvenio;
    public DatosRecaudo() {
    }

    protected DatosRecaudo(Parcel in) {
        indicadorBaseDatos = in.readInt();
        valorFactura = in.readLong();
        indicadorValor = in.readInt();
        tipoRecaudo = in.readString();
        valorApagar = in.readString();
        numeroFactura = in.readString();
        codigoConvenio = in.readString();
        cuentaRecaudadora = in.readString();
        tokenP6 = in.readString();
        nombreConvenio = in.readString();
    }

    public static final Creator<DatosRecaudo> CREATOR = new Creator<DatosRecaudo>() {
        @Override
        public DatosRecaudo createFromParcel(Parcel in) {
            return new DatosRecaudo(in);
        }

        @Override
        public DatosRecaudo[] newArray(int size) {
            return new DatosRecaudo[size];
        }
    };

    public int getIndicadorBaseDatos() {
        return indicadorBaseDatos;
    }

    public void setIndicadorBaseDatos(int indicadorBaseDatos) {
        this.indicadorBaseDatos = indicadorBaseDatos;
    }

    public String getNombreConvenio() {
        return nombreConvenio;
    }

    public void setNombreConvenio(String nombreConvenio) {
        this.nombreConvenio = nombreConvenio;
    }

    public long getValorFactura() {
        return valorFactura;
    }

    public void setValorFactura(long valorFactura) {
        this.valorFactura = valorFactura;
    }

    public int getIndicadorValor() {
        return indicadorValor;
    }

    public void setIndicadorValor(int indicadorValor) {
        this.indicadorValor = indicadorValor;
    }

    public String getTipoRecaudo() {
        return tipoRecaudo;
    }

    public void setTipoRecaudo(String tipoRecaudo) {
        this.tipoRecaudo = tipoRecaudo;
    }

    public String getValorApagar() {
        return valorApagar;
    }

    public void setValorApagar(String valorApagar) {
        this.valorApagar = valorApagar;
    }

    public String getNumeroFactura() {
        return numeroFactura;
    }

    public void setNumeroFactura(String numeroFactura) {
        this.numeroFactura = numeroFactura;
    }

    public String getCodigoConvenio() {
        return codigoConvenio;
    }

    public void setCodigoConvenio(String codigoConvenio) {
        this.codigoConvenio = codigoConvenio;
    }

    public String getCuentaRecaudadora() {
        return cuentaRecaudadora;
    }

    public void setCuentaRecaudadora(String cuentaRecaudadora) {
        this.cuentaRecaudadora = cuentaRecaudadora;
    }

    public String getTokenP6() {
        return tokenP6;
    }

    public void setTokenP6(String tokenP6) {
        this.tokenP6 = tokenP6;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel parcel, int i) {
        parcel.writeInt(indicadorBaseDatos);
        parcel.writeLong(valorFactura);
        parcel.writeInt(indicadorValor);
        parcel.writeString(tipoRecaudo);
        parcel.writeString(valorApagar);
        parcel.writeString(numeroFactura);
        parcel.writeString(codigoConvenio);
        parcel.writeString(cuentaRecaudadora);
        parcel.writeString(tokenP6);
        parcel.writeString(nombreConvenio);
    }
}
