package co.com.netcom.corresponsal.pantallas.funciones;

import android.util.Base64;

public class ConvertirBase64 {

    private String base64;

    public String convertirBase64(String textoSinBase64){

        return base64 = Base64.encodeToString(textoSinBase64.getBytes(),Base64.DEFAULT);
    }


}
