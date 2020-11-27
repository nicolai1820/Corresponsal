package co.com.netcom.corresponsal.emv;

import java.util.ArrayList;
import java.util.List;

import co.com.netcom.corresponsal.core.comunicacion.BytesUtil2;

public class TLVList {
    /**
     * List data.
     */
    private List<TLV> data = new ArrayList<>();

    /**
     * Make to binary.
     */
    public byte[] toBinary() {
        byte[][] allData = new byte[this.data.size()][];
        for (int i = 0; i < this.data.size(); i++) {
            allData[i] = this.data.get(i).getRawData();
        }
        return BytesUtil2.merge(allData);
    }

    /**
     * Add TLV.
     */
    public void addTLV(TLV tlv) {
        if (tlv.isValid()) {
            this.data.add(tlv);
        } else {
            throw new IllegalArgumentException();
        }
    }

    /**
     * Add TLV.
     */
    public void addTLV(String tag, byte[] value) {
        this.addTLV(TLV.fromData(tag, value));
    }

    /**
     * Make to string data.
     */
    public String toString() {
        if (this.data.isEmpty()) {
            return super.toString();
        }
        return BytesUtil2.bytes2HexString(toBinary());
    }
}

