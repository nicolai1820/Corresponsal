package co.com.netcom.corresponsal.pantallas.funciones;


import android.util.Base64;

import java.io.UnsupportedEncodingException;
import java.nio.charset.StandardCharsets;

public class CodificarBase64 {

    String cadena;


    public String convertirBase64(String textoSinBase64){
        byte [] data = textoSinBase64.getBytes(StandardCharsets.UTF_8);
        cadena = Base64.encodeToString(data,Base64.NO_WRAP);
        return cadena;
    }

    public String decodificarBase64(String textoConBase64){

        byte[] dataEncode = Base64.decode(textoConBase64, Base64.NO_WRAP);
        String dataDecode = new String(dataEncode,StandardCharsets.UTF_8);
        return dataDecode;
    }




}
