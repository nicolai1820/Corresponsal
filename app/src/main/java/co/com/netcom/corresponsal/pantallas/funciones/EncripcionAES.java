package co.com.netcom.corresponsal.pantallas.funciones;

import android.util.Base64;

import javax.crypto.Cipher;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.SecretKeySpec;

public class EncripcionAES {


    // Algoritmo (AES, DES, RSA)
    private final static String algoritmo = "AES";
    // Tipo de cifrado, por bloques, padding etc.
    private final static String tipoCifrado = "AES/CBC/PKCS5Padding";
    // IV -------  "7581C14831F0D9AFB72346AC3F625814";
    private static byte[]  mInitVec = hexToByte1("7581C14831F0D9AFB72346AC3F625814");

    /**Metodo encrypt que recibe como parametros el String key y el String texto, sirve para encriptar el texto en AES*/
    public  String encrypt(String llave, String texto) throws Exception {
        Cipher cipher = Cipher.getInstance(tipoCifrado);
        SecretKeySpec secretKeySpec = new SecretKeySpec(llave.getBytes(), algoritmo);

        IvParameterSpec ivParameterSpec = new IvParameterSpec(mInitVec);
        cipher.init(Cipher.ENCRYPT_MODE, secretKeySpec, ivParameterSpec);
        byte[] encrypted = cipher.doFinal(texto.getBytes());
        return Base64.encodeToString(encrypted,Base64.DEFAULT);
    }

    /**Metodo encrypt que recibe como parametros el String key y el String encrypted, sirve para desencriptar la de AES a texto en limpio*/

    public  String decrypt(String llave,String encrypted) throws Exception {
        Cipher cipher = Cipher.getInstance(tipoCifrado);
        SecretKeySpec secretKeySpec = new SecretKeySpec(llave.getBytes(), algoritmo);
        IvParameterSpec ivParameterSpec = new IvParameterSpec(mInitVec);

        byte[] enc = Base64.decode(encrypted,Base64.DEFAULT);
        cipher.init(Cipher.DECRYPT_MODE, secretKeySpec, ivParameterSpec);
        byte[] decrypted = cipher.doFinal(enc);
        return new String(decrypted);
    }


    static final String     HEXES        = "0123456789ABCDEF";


    public static byte[] hexToByte1(String hexString) {
        int    len = hexString.length();
        byte[] ba  = new byte[len / 2];

        for (int i = 0; i < len; i += 2) {
            ba[i / 2] = (byte) ((Character.digit(hexString.charAt(i), 16) << 4)
                    + Character.digit(hexString.charAt(i + 1), 16));
        }

        return ba;
    }

}

