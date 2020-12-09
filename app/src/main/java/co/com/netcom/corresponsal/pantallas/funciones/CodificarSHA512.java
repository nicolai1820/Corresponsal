package co.com.netcom.corresponsal.pantallas.funciones;

import android.util.Log;

import java.io.UnsupportedEncodingException;
import java.math.BigInteger;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

public class CodificarSHA512 {


    public String codificarSHA512(String textoSinSHA512){

        String sha512 = null;
        try {
            MessageDigest digest = MessageDigest.getInstance("SHA-512");
            digest.reset();
            digest.update(textoSinSHA512.getBytes("utf8"));
            sha512 =String.format("%0128x", new BigInteger(1, digest.digest()));
            return sha512;
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }

    }


}
