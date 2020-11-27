package co.com.netcom.corresponsal.core.comunicacion;

import android.os.Parcel;
import android.os.Parcelable;

public class DatosBasicosVenta implements Parcelable {
    private String  totalVenta;
    private String  compraNeta;
    private String  baseDevolucion;
    private String  iva;
    private String  inc;
    private String  propina;
    private String totalVentaIva;
    private String totalVentaInc;
    private int indicePagos;
    private int indicePagados;
    private int tipoPagoDivididas;

    public DatosBasicosVenta() {
    }

    protected DatosBasicosVenta(Parcel in) {
        totalVenta = in.readString();
        compraNeta = in.readString();
        baseDevolucion = in.readString();
        iva = in.readString();
        inc = in.readString();
        propina = in.readString();
        indicePagos = in.readInt();
        indicePagados = in.readInt();
        tipoPagoDivididas = in.readInt();
        totalVentaIva = in.readString();
        totalVentaInc = in.readString();
    }

    public static final Creator<DatosBasicosVenta> CREATOR = new Creator<DatosBasicosVenta>() {
        @Override
        public DatosBasicosVenta createFromParcel(Parcel in) {
            return new DatosBasicosVenta(in);
        }

        @Override
        public DatosBasicosVenta[] newArray(int size) {
            return new DatosBasicosVenta[size];
        }
    };

    public String getTotalVentaIva() {
        return totalVentaIva;
    }

    public void setTotalVentaIva(String totalVentaIva) {
        this.totalVentaIva = totalVentaIva;
    }

    public String getTotalVentaInc() {
        return totalVentaInc;
    }

    public void setTotalVentaInc(String totalVentaInc) {
        this.totalVentaInc = totalVentaInc;
    }

    public String getTotalVenta() {
        return totalVenta;
    }

    public void setTotalVenta(String totalVenta) {
        this.totalVenta = totalVenta;
    }

    public String getCompraNeta() {
        return compraNeta;
    }

    public void setCompraNeta(String compraNeta) {
        this.compraNeta = compraNeta;
    }

    public String getBaseDevolucion() {
        return baseDevolucion;
    }

    public void setBaseDevolucion(String baseDevolucion) {
        this.baseDevolucion = baseDevolucion;
    }

    public String getIva() {
        return iva;
    }

    public void setIva(String iva) {
        this.iva = iva;
    }

    public String getInc() {
        return inc;
    }

    public void setInc(String inc) {
        this.inc = inc;
    }

    public String getPropina() {
        return propina;
    }

    public void setPropina(String propina) {
        this.propina = propina;
    }

    public int getIndicePagos() {
        return indicePagos;
    }

    public void setIndicePagos(int indicePagos) {
        this.indicePagos = indicePagos;
    }

    public int getIndicePagados() {
        return indicePagados;
    }

    public void setIndicePagados(int indicePagados) {
        this.indicePagados = indicePagados;
    }

    public int getTipoPagoDivididas() {
        return tipoPagoDivididas;
    }

    public void setTipoPagoDivididas(int tipoPagoDivididas) {
        this.tipoPagoDivididas = tipoPagoDivididas;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel parcel, int i) {
        parcel.writeString(totalVenta);
        parcel.writeString(compraNeta);
        parcel.writeString(baseDevolucion);
        parcel.writeString(iva);
        parcel.writeString(inc);
        parcel.writeString(propina);
        parcel.writeInt(indicePagos);
        parcel.writeInt(indicePagados);
        parcel.writeInt(tipoPagoDivididas);
        parcel.writeString(totalVentaIva);
        parcel.writeString(totalVentaInc);
    }
}
