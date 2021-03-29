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

    /**Metodo getCommerceId, el cual sirve para obtener el commerceId, el cual se encuentra guardado en el sharedPreferences*/
    public String getCommerceId() {
        return sharedPreferences.getString("commerceId","");
    }

    /**Metodo getaddress, el cual sirve para obtener el address, el cual se encuentra guardado en el sharedPreferences*/
    public String getAddress()  {
        return sharedPreferences.getString("address","");
    }

    /**Metodo getcommerceCode, el cual sirve para obtener el commerceCode, el cual se encuentra guardado en el sharedPreferences*/
    public String getCommerceCode()  {
        return sharedPreferences.getString("commerceCode","");
    }

    /**Metodo getname, el cual sirve para obtener el name, el cual se encuentra guardado en el sharedPreferences*/
    public String getName()  {
        return sharedPreferences.getString("name","");
    }

    /**Metodo getterminalId, el cual sirve para obtener el terminalId, el cual se encuentra guardado en el sharedPreferences*/
    public String getTerminalId()  {
        return sharedPreferences.getString("terminalId","");
    }

    /**Metodo getterminalCode, el cual sirve para obtener el terminalCode, el cual se encuentra guardado en el sharedPreferences*/
    public String getTerminalCodeParametricas()  {
        return sharedPreferences.getString("terminalCode","");
    }

    /**Metodo getcommerceMail, el cual sirve para obtener el commerceMail, el cual se encuentra guardado en el sharedPreferences*/
    public String getCommerceMail()  {
        return sharedPreferences.getString("commerceMail","");
    }

    /**Metodo getconsecutive, el cual sirve para obtener el consecutive, el cual se encuentra guardado en el sharedPreferences*/
    public String getConsecutive()  {
        return sharedPreferences.getString("consecutive","");
    }

    /**Metodo getconfigApp, el cual sirve para obtener el configApp, el cual se encuentra guardado en el sharedPreferences*/
    public String getConfigApp()  {
        return sharedPreferences.getString("configApp","");
    }

    /**Metodo gettaxList, el cual sirve para obtener el taxList, el cual se encuentra guardado en el sharedPreferences*/
    public String getTaxList()  {
        return sharedPreferences.getString("taxList","");
    }

    /**Metodo gettipList, el cual sirve para obtener el tipList, el cual se encuentra guardado en el sharedPreferences*/
    public String getTipList()  {
        return sharedPreferences.getString("tipList","");
    }

    /**Metodo getauthorizers, el cual sirve para obtener el authorizers, el cual se encuentra guardado en el sharedPreferences*/
    public String getAuthorizers()  {
        return sharedPreferences.getString("authorizers","");
    }

    /**Metodo getcityList, el cual sirve para obtener el cityList, el cual se encuentra guardado en el sharedPreferences*/
    public String getCityList()  {
        return sharedPreferences.getString("cityList","");
    }

    /**Metodo getagreementCityList, el cual sirve para obtener el agreementCityList, el cual se encuentra guardado en el sharedPreferences*/
    public String getAgreementCityList()  {
        return sharedPreferences.getString("agreementCityList","");
    }

    /**Metodo gettypeAgreementList, el cual sirve para obtener el typeAgreementList, el cual se encuentra guardado en el sharedPreferences*/
    public String getTypeAgreementList()  {
        return sharedPreferences.getString("typeAgreementList","");
    }

    /**Metodo getagreementList, el cual sirve para obtener el agreementList, el cual se encuentra guardado en el sharedPreferences*/
    public String getAgreementList()  {
        return sharedPreferences.getString("agreementList","");
    }

    /**Metodo getoperatorList, el cual sirve para obtener el operatorList, el cual se encuentra guardado en el sharedPreferences*/
    public String getOperatorList()  {
        return sharedPreferences.getString("operatorList","");
    }

    /**Metodo getversionList, el cual sirve para obtener el versionList, el cual se encuentra guardado en el sharedPreferences*/
    public String getVersionList()  {
        return sharedPreferences.getString("versionList","");
    }

    /**Metodo getbinPrivadoList, el cual sirve para obtener el binPrivadoList, el cual se encuentra guardado en el sharedPreferences*/
    public String getBinPrivadoList()  {
        return sharedPreferences.getString("binPrivadoList","");
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

    /**Metodo setParametricasBanco, que recibe como parametro un JsonObject para guardar el FIID y el tipo de cuenta */

    public void setParametricasBanco(JSONObject jsonObject)throws JSONException{
        sharedPreferencesEditor.putString("fiidId",jsonObject.getString("fiidId"));
        sharedPreferencesEditor.putString("tipoCuenta",jsonObject.getString("tipoCuenta"));
        sharedPreferencesEditor.commit();
    }


}
