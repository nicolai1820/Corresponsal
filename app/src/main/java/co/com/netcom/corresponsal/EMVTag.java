/*package co.com.netcom.apos;

import android.util.Log;

public class EMVTag{

    private static final String TAG = MainActivity.class.getSimpleName();
    private String key;
    private int     len;
    private String value;
    EMVTag(String theKey, int theLen, String theValue){
        this.key  = theKey;
        this.len = theLen;
        this.value = theValue;
    }

    public String getKey() {
        return key;
    }

    public int getLen() {
        return len;
    }

    public String getValue() {
        return value;
    }

    public static EMVTag parseTLV(String tlvStr, String tagToFind) {
        if (tlvStr == null || tlvStr.length()%2!=0) {
            throw new RuntimeException("Invalid tlv, null or odd length");
        }
        EMVTag tagFound = null;
        int lenInBytes = 0;

        for (int i=0; i<tlvStr.length();) {
            try {
                String key = tlvStr.substring(i, i=i+2);

                if ((Integer.parseInt(key,16) & 0x1F) == 0x1F) {
                    // extra byte for TAG field
                    key += tlvStr.substring(i, i=i+2);
                }
                String len = tlvStr.substring(i, i=i+2);
                int length = Integer.parseInt(len,16);

                if (length > 127) {
                    // more than 1 byte for lenth
                    int bytesLength = length-128;
                    len = tlvStr.substring(i, i=i+(bytesLength*2));
                    length = Integer.parseInt(len,16);
                }
                lenInBytes = length;
                length*=2;  //To read two chars per byte

                String value = tlvStr.substring(i, i=i+length);
                //System.out.println(key+" = "+value);
                if(tagToFind.equals(key)){
                    tagFound = new EMVTag(key, lenInBytes, value);
                    Log.d(TAG, "key = " + key + " len " +  lenInBytes + value);
                    break;
                }
            } catch (NumberFormatException e) {
                throw new RuntimeException("Error parsing number",e);
            } catch (IndexOutOfBoundsException e) {
                throw new RuntimeException("Error processing field",e);
            }
        }
        return tagFound;
    }
}*/