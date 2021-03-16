package co.com.netcom.corresponsal.pantallas.funciones;

import android.content.Context;
import android.content.SharedPreferences;
import android.util.JsonReader;

import org.json.JSONException;
import org.json.JSONObject;

public class PreferencesUsuario {
    private SharedPreferences sharedPreferences;
    private SharedPreferences.Editor sharedPreferencesEditor;

    public PreferencesUsuario(String ruta,Context context){
        sharedPreferences = context.getSharedPreferences(ruta,context.MODE_PRIVATE);
        sharedPreferencesEditor = sharedPreferences.edit();
    }

    /**Metodo para setear el token en el sharedpreferences*/
    public void setToken(String token) {

        sharedPreferencesEditor.putString("Token",token);
        sharedPreferencesEditor.commit();
    }

    /**Metodo getToken que retorna un String, se encarga de retornar el token para poder realizar transacciones*/
    public String getToken(){
        return sharedPreferences.getString("Token",null);
    }

    /**Metodo setParametricas, el cual se encarga de guardar las parametricas dentro del shared preferences.*/
    public void setParametricas(JSONObject jsonObject) throws JSONException {
        sharedPreferencesEditor.putString("privateAuthorizers",jsonObject.getJSONObject("privateAuthorizers").toString());
        sharedPreferencesEditor.putString("configCommerce",jsonObject.getJSONObject("configCommerce").toString());
        sharedPreferencesEditor.putString("configApp",jsonObject.getJSONObject("configApp").toString());
        sharedPreferencesEditor.putString("taxs",jsonObject.getJSONObject("taxs").toString());
        sharedPreferencesEditor.putString("authorizers",jsonObject.getJSONObject("authorizers").toString());
        sharedPreferencesEditor.putString("bins",jsonObject.getJSONObject("bins").toString());
        sharedPreferencesEditor.putString("pspInfo",jsonObject.getJSONObject("pspInfo").toString());
        sharedPreferencesEditor.putString("rechargeInfo",jsonObject.getJSONObject("rechargeInfo").toString());
        sharedPreferencesEditor.putString("versions",jsonObject.getJSONObject("versions").toString());
        sharedPreferencesEditor.putString("appVersions",jsonObject.getJSONObject("appVersions").toString());
        sharedPreferencesEditor.commit();

    }

    /**Metodo setParametricas, el cual se encarga de obtener un objeto del shared preferences*/
    public JSONObject getParametrica(String parametrica) throws JSONException {
        return new JSONObject(sharedPreferences.getString(parametrica,null));
    }

    /**Metodo setUserid, que recibo un String como parametro y sirve para guardar el userId que se obtiene al realizar el login */
    public void setUserId(String userId){
        sharedPreferencesEditor.putString("userId",userId);
        sharedPreferencesEditor.commit();
    }

    /**Metodo getUserId retorna un string y funciona para obtener el userId que se encuentra en el sharedPreferences*/
    public String getUserId(){
        return sharedPreferences.getString("userId",null);
    }

    /**Metodo encryptionKey, que recibo un String como parametro y sirve para guardar el encryptionKey que se obtiene al realizar el login */
    public void setEncryptionKey(String encryptionKey){
        sharedPreferencesEditor.putString("encryptionKey",encryptionKey);
        sharedPreferencesEditor.commit();
    }

    /**Metodo getEncryptionKey retorna un string y funciona para obtener el encryptionKey que se encuentra en el sharedPreferences*/
    public String getEncryptionKey(){
        return sharedPreferences.getString("encryptionKey",null);
    }


}
