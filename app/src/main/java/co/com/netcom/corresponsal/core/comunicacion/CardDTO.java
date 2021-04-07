package co.com.netcom.corresponsal.core.comunicacion;

import android.os.Parcel;
import android.os.Parcelable;

import org.json.JSONException;
import org.json.JSONObject;

/**
 * Created by david.ordaz on 20/03/2018.
 */

public class CardDTO implements Parcelable {
    /** Indica si la tarjeta se ingreso por banda o por chip **/
    private int tipoEntrada;
    private int isPinAsked;
    private int tamTagVenta;
    private int asklastfour;
    private int askDocument;
    private int askCuotes;
    private int askCvc2;
    private int askAdicionalText;
    /** Indica si la tarjeta en banda o chip **/
    private boolean isBanda ;
    private boolean isChip ;

    private String track1 ;
    private String track2 ;
    private String track3 ;
    private String track1Cifrado;
    private String track2Cifrado;
    private String ksn;
    private String ksnPinBlock;
    private String cuotas;
    private String pan;
    private String panEnmascarado;


    /** Contendra los ultimos 4 digitos del track **/
    private String ultimosCuatro;

    /** Contendra los tag que el servidor va a enviar al autorizador **/
    private String tagVenta;

    private String pinBlock;

    private String tipoCuentaId;

    private String posEntryMode;

    private boolean fallBack;

    private String ksnTrackII;

    private String franquicia;

    private String tarjetaHabiente;

    private String fechaExpiracion;

    private String aid;

    private String arqc;

    private String aplabel;

    private String grupo;
    private String subGrupo;
    private String documento;
    private String cvc2;
    public CardDTO() {
    }

    protected CardDTO(Parcel in) {
        tipoEntrada = in.readInt();
        isPinAsked = in.readInt();
        tamTagVenta = in.readInt();
        asklastfour = in.readInt();
        askDocument = in.readInt();
        askCuotes = in.readInt();
        askCvc2 = in.readInt();
        askAdicionalText = in.readInt();
        isBanda = in.readByte() != 0;
        isChip = in.readByte() != 0;
        track1 = in.readString();
        track2 = in.readString();
        track3 = in.readString();
        track1Cifrado = in.readString();
        track2Cifrado = in.readString();
        ksn = in.readString();
        ksnPinBlock = in.readString();
        cuotas = in.readString();
        pan = in.readString();
        panEnmascarado = in.readString();
        ultimosCuatro = in.readString();
        tagVenta = in.readString();
        pinBlock = in.readString();
        tipoCuentaId = in.readString();
        posEntryMode = in.readString();
        fallBack = in.readByte() != 0;
        ksnTrackII = in.readString();
        franquicia = in.readString();
        tarjetaHabiente = in.readString();
        fechaExpiracion = in.readString();
        aid = in.readString();
        arqc = in.readString();
        aplabel = in.readString();
        grupo = in.readString();
        subGrupo = in.readString();
        documento = in.readString();
        cvc2 = in.readString();
    }

    public static final Creator<CardDTO> CREATOR = new Creator<CardDTO>() {
        @Override
        public CardDTO createFromParcel(Parcel in) {
            return new CardDTO(in);
        }

        @Override
        public CardDTO[] newArray(int size) {
            return new CardDTO[size];
        }
    };

    public String getDocumento() {
        return documento;
    }

    public void setDocumento(String documento) {
        this.documento = documento;
    }

    public String getCvc2() {
        return cvc2;
    }

    public void setCvc2(String cvc2) {
        this.cvc2 = cvc2;
    }

    public String getGrupo() {
        return grupo;
    }

    public void setGrupo(String grupo) {
        this.grupo = grupo;
    }

    public String getSubGrupo() {
        return subGrupo;
    }

    public void setSubGrupo(String subGrupo) {
        this.subGrupo = subGrupo;
    }

    public int getAsklastfour() {
        return asklastfour;
    }

    public void setAsklastfour(int asklastfour) {
        this.asklastfour = asklastfour;
    }

    public int getAskDocument() {
        return askDocument;
    }

    public void setAskDocument(int askDocument) {
        this.askDocument = askDocument;
    }

    public int getAskCuotes() {
        return askCuotes;
    }

    public void setAskCuotes(int askCuotes) {
        this.askCuotes = askCuotes;
    }

    public int getAskCvc2() {
        return askCvc2;
    }

    public void setAskCvc2(int askCvc2) {
        this.askCvc2 = askCvc2;
    }

    public int getAskAdicionalText() {
        return askAdicionalText;
    }

    public void setAskAdicionalText(int askAdicionalText) {
        this.askAdicionalText = askAdicionalText;
    }

    public String getCuotas() {
        return cuotas;
    }

    public void setCuotas(String cuotas) {
        this.cuotas = cuotas;
    }

    public int getTamTagVenta() {
        return tamTagVenta;
    }

    public void setTamTagVenta(int tamTagVenta) {
        this.tamTagVenta = tamTagVenta;
    }

    public int getIsPinAsked() {
        return isPinAsked;
    }

    public void setIsPinAsked(int isPinAsked) {
        this.isPinAsked = isPinAsked;
    }

    public String getAplabel() {
        return aplabel;
    }

    public void setAplabel(String aplabel) {
        this.aplabel = aplabel;
    }

    public boolean isFallBack() {
        return fallBack;
    }

    public String getArqc() {
        return arqc;
    }

    public void setArqc(String arqc) {
        this.arqc = arqc;
    }



    public boolean getIsBanda() {
        return isBanda;
    }

    public void setIsBanda(boolean isBanda) {
        this.isBanda = isBanda;
    }

    public int getTipoEntrada() {
        return tipoEntrada;
    }

    public void setTipoEntrada(int tipoEntrada) {
        this.tipoEntrada = tipoEntrada;
    }

    public boolean getIsChip() {
        return isChip;
    }

    public void setIsChip(boolean isChip) {
        this.isChip = isChip;
    }

    public String getFechaExpiracion() {
        return fechaExpiracion;
    }

    public void setFechaExpiracion(String fechaExpiracion) {
        this.fechaExpiracion = fechaExpiracion;
    }

    public String getTrack1() {
        return track1;
    }

    public void setTrack1(String track1) {
        this.track1 = track1;
    }

    public String getTrack2() {
        return track2;
    }

    public void setTrack2(String track2) {
        this.track2 = track2;
    }

    public String getTrack3() {
        return track3;
    }

    public void setTrack3(String track3) {
        this.track3 = track3;
    }

    public String getTrack1Cifrado() {
        return track1Cifrado;
    }

    public void setTrack1Cifrado(String track1Cifrado) {
        this.track1Cifrado = track1Cifrado;
    }

    public String getKsn() {
        return ksn;
    }

    public void setKsn(String ksn) {
        this.ksn = ksn;
    }

    public String getTrack2Cifrado() {
        return track2Cifrado;
    }

    public void setTrack2Cifrado(String track2Cifrado) {
        this.track2Cifrado = track2Cifrado;
    }

    public String getKsnPinBlock() {
        return ksnPinBlock;
    }

    public void setKsnPinBlock(String ksnPinBlock) {
        this.ksnPinBlock = ksnPinBlock;
    }

    public String getPan() {
        return pan;
    }

    public void setPan(String pan) {
        this.pan = pan;
    }

    public String getPanEnmascarado() {
        return panEnmascarado;
    }

    public void setPanEnmascarado(String panEnmascarado) {
        this.panEnmascarado = panEnmascarado;
    }

    public String getUltimosCuatro() {
        return ultimosCuatro;
    }

    public void setUltimosCuatro(String ultimosCuatro) {
        this.ultimosCuatro = ultimosCuatro;
    }

    public String getTagVenta() {
        return tagVenta;
    }

    public void setTagVenta(String tagVenta) {
        this.tagVenta = tagVenta;
    }

    public String getPinBlock() {
        return pinBlock;
    }

    public void setPinBlock(String pinBlock) {
        this.pinBlock = pinBlock;
    }

    public String getTipoCuentaId() {
        return tipoCuentaId;
    }

    public void setTipoCuentaId(String tipoCuentaId) {
        this.tipoCuentaId = tipoCuentaId;
    }

    public String getPosEntryMode() {
        return posEntryMode;
    }

    public void setPosEntryMode(String posEntryMode) {
        this.posEntryMode = posEntryMode;
    }

    public boolean getFallBack() {
        return fallBack;
    }

    public void setFallBack(boolean fallBack) {
        this.fallBack = fallBack;
    }

    public String getKsnTrackII() {
        return ksnTrackII;
    }

    public void setKsnTrackII(String ksnTrackII) {
        this.ksnTrackII = ksnTrackII;
    }

    public String getFranquicia() {
        return franquicia;
    }

    public void setFranquicia(String franquicia) {
        this.franquicia = franquicia;
    }

    public String getTarjetaHabiente() {
        return tarjetaHabiente;
    }

    public void setTarjetaHabiente(String tarjetaHabiente) {
        this.tarjetaHabiente = tarjetaHabiente;
    }

    public boolean isChip() {
        return isChip;
    }

    public void setChip(boolean chip) {
        isChip = chip;
    }

    public boolean isBanda() {
        return isBanda;
    }

    public void setBanda(boolean banda) {
        isBanda = banda;
    }

    public String getAid() {
        return aid;
    }

    public void setAid(String aid) {
        this.aid = aid;
    }

    public String toJsonString() throws JSONException {
        JSONObject obj = new JSONObject();
        obj.put("tarjetaHabiente",tarjetaHabiente);
        obj.put("fechaExp",fechaExpiracion);
        obj.put("ksn",ksn);
        obj.put("ksnPinBlock",ksnPinBlock);
        obj.put("panEnmascarado",panEnmascarado);
        obj.put("pan",panEnmascarado);
        obj.put("track2",track2Cifrado);
        obj.put("isBanda",isBanda);
        obj.put("isChip",isChip);
        obj.put("PosEntryMode",posEntryMode);
        obj.put("pinBlock",pinBlock);
        obj.put("tipoCuentaId",tipoCuentaId);
        obj.put("ultimosCuatro",ultimosCuatro);
        obj.put("franquicia",franquicia);
        obj.put("tipoEntrada",tipoEntrada);
        obj.put("ultimosCuatro",ultimosCuatro);
        obj.put("fallBack",fallBack);
        obj.put("aid",aid);
        obj.put("tagVenta",tagVenta);
        obj.put("arqc",arqc);
        return obj.toString();
    }

    @Override
    public String toString() {
        return "CardDTO{" +
                "tipoEntrada=" + tipoEntrada +
                ", isBanda=" + isBanda +
                ", isChip=" + isChip +
                ", track1='" + track1 + '\'' +
                ", track2='" + track2 + '\'' +
                ", track3='" + track3 + '\'' +
                ", track1Cifrado='" + track1Cifrado + '\'' +
                ", track2Cifrado='" + track2Cifrado + '\'' +
                ", ksn='" + ksn + '\'' +
                ", ksnPinBlock='" + ksnPinBlock + '\'' +
                ", pan='" + pan + '\'' +
                ", panEnmascarado='" + panEnmascarado + '\'' +
                ", ultimosCuatro='" + ultimosCuatro + '\'' +
                ", tagVenta='" + tagVenta + '\'' +
                ", pinBlock='" + pinBlock + '\'' +
                ", tipoCuentaId='" + tipoCuentaId + '\'' +
                ", posEntryMode='" + posEntryMode + '\'' +
                ", fallBack=" + fallBack +
                ", ksnTrackII='" + ksnTrackII + '\'' +
                ", franquicia='" + franquicia + '\'' +
                ", tarjetaHabiente='" + tarjetaHabiente + '\'' +
                ", fechaExpiracion='" + fechaExpiracion + '\'' +
                ", aid='" + aid + '\'' +
                ", arqc='" + arqc + '\'' +
                '}';
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel parcel, int i) {
        parcel.writeInt(tipoEntrada);
        parcel.writeInt(isPinAsked);
        parcel.writeInt(tamTagVenta);
        parcel.writeInt(asklastfour);
        parcel.writeInt(askDocument);
        parcel.writeInt(askCuotes);
        parcel.writeInt(askCvc2);
        parcel.writeInt(askAdicionalText);
        parcel.writeByte((byte) (isBanda ? 1 : 0));
        parcel.writeByte((byte) (isChip ? 1 : 0));
        parcel.writeString(track1);
        parcel.writeString(track2);
        parcel.writeString(track3);
        parcel.writeString(track1Cifrado);
        parcel.writeString(track2Cifrado);
        parcel.writeString(ksn);
        parcel.writeString(ksnPinBlock);
        parcel.writeString(cuotas);
        parcel.writeString(pan);
        parcel.writeString(panEnmascarado);
        parcel.writeString(ultimosCuatro);
        parcel.writeString(tagVenta);
        parcel.writeString(pinBlock);
        parcel.writeString(tipoCuentaId);
        parcel.writeString(posEntryMode);
        parcel.writeByte((byte) (fallBack ? 1 : 0));
        parcel.writeString(ksnTrackII);
        parcel.writeString(franquicia);
        parcel.writeString(tarjetaHabiente);
        parcel.writeString(fechaExpiracion);
        parcel.writeString(aid);
        parcel.writeString(arqc);
        parcel.writeString(aplabel);
        parcel.writeString(grupo);
        parcel.writeString(subGrupo);
        parcel.writeString(documento);
        parcel.writeString(cvc2);
    }
}
