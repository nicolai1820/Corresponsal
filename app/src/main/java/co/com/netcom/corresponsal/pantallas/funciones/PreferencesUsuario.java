package co.com.netcom.corresponsal.pantallas.funciones;

import android.content.Context;
import android.content.SharedPreferences;
import android.util.JsonReader;
import android.util.Log;

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
        Log.d("json",jsonObject.toString());
        //Obtener cada parametric cuando viene dentro de un objeto...
        Log.d("configCommerce",jsonObject.getJSONObject("configCommerce").getString("commerceId"));


        sharedPreferencesEditor.putString("commerceId",jsonObject.getJSONObject("configCommerce").getString("commerceId"));
        sharedPreferencesEditor.putString("address",jsonObject.getJSONObject("configCommerce").getString("address"));
        sharedPreferencesEditor.putString("commerceCode",jsonObject.getJSONObject("configCommerce").getString("commerceCode"));
        sharedPreferencesEditor.putString("name",jsonObject.getJSONObject("configCommerce").getString("name"));
        sharedPreferencesEditor.putString("terminalId",jsonObject.getJSONObject("configCommerce").getString("terminalId"));
        sharedPreferencesEditor.putString("terminalCode",jsonObject.getJSONObject("configCommerce").getString("terminalCode"));
        sharedPreferencesEditor.putString("commerceMail",jsonObject.getJSONObject("configCommerce").getString("commerceMail"));
        sharedPreferencesEditor.putString("consecutive",jsonObject.getJSONObject("configCommerce").getString("consecutive"));

        sharedPreferencesEditor.putString("configApp",jsonObject.getJSONObject("parametrics").getJSONObject("configApp").toString());
        sharedPreferencesEditor.putString("taxList",jsonObject.getJSONObject("parametrics").getJSONObject("taxs").getJSONArray("taxList").toString());
        sharedPreferencesEditor.putString("tipList",jsonObject.getJSONObject("parametrics").getJSONObject("taxs").getJSONArray("tipList").toString());
        sharedPreferencesEditor.putString("authorizers",jsonObject.getJSONObject("parametrics").getJSONObject("authorizers").getJSONArray("authorizerList").toString());
        sharedPreferencesEditor.putString("cityList",jsonObject.getJSONObject("parametrics").getJSONObject("pspInfo").getJSONArray("cityList").toString());
        sharedPreferencesEditor.putString("agreementCityList",jsonObject.getJSONObject("parametrics").getJSONObject("pspInfo").getJSONArray("agreementCityList").toString());
        sharedPreferencesEditor.putString("typeAgreementList",jsonObject.getJSONObject("parametrics").getJSONObject("pspInfo").getJSONArray("typeAgreementList").toString());
        sharedPreferencesEditor.putString("agreementList",jsonObject.getJSONObject("parametrics").getJSONObject("pspInfo").getJSONArray("agreementList").toString());
        sharedPreferencesEditor.putString("operatorList",jsonObject.getJSONObject("parametrics").getJSONObject("rechargeInfo").getJSONArray("operatorList").toString());
        sharedPreferencesEditor.putString("versionList",jsonObject.getJSONObject("parametrics").getJSONObject("versions").getJSONArray("versionList").toString());
        sharedPreferencesEditor.putString("binPrivadoList",jsonObject.getJSONObject("parametrics").getJSONObject("bins").getJSONArray("binPrivadoList").toString());
        sharedPreferencesEditor.commit();

        Log.d("Parametricas","Parametricas cargadas correctamente");

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

    /**Metodo setTerminalCode, que recibe un String como parametro y sirve para guardar el TerminalCode que es el nombre de usuario*/
    public void setTerminalCode(String terminalCode){
        sharedPreferencesEditor.putString("terminalCode",terminalCode);
    }

    /**Metodo getTerminalCode el cual retornal un String y funciona para obtener el userId que se encuentra en el sharedPreferences*/
    public String getTerminalCode(){
        return sharedPreferences.getString("terminalCode",null);
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
