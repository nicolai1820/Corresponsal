package co.com.netcom.corresponsal.pantallas.funciones;

import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.location.Criteria;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.Message;
import android.provider.Settings;
import android.telephony.TelephonyManager;
import android.text.format.Formatter;
import android.util.Log;

import androidx.core.app.ActivityCompat;

import com.google.android.gms.common.api.GoogleApiClient;
import com.google.android.gms.location.FusedLocationProviderClient;
import com.google.android.gms.location.LocationServices;
import com.google.android.gms.tasks.OnSuccessListener;

import java.util.logging.Handler;

import co.com.netcom.corresponsal.pantallas.comunes.logIn.LogIn;
import co.com.netcom.corresponsal.pantallas.comunes.popUp.PopUp;

public class DeviceInformation implements LocationListener {
    private Context context;
    private LocationManager locationManager;

    private FusedLocationProviderClient fusedLocationProviderClient;

   public DeviceInformation(Context contexto){
        this.context = contexto;
    }

    /**Metodo getInternetStatus de tipo publico que retorna un booleano, sirve para determinar el estado de conexiòn a internet*/
    public boolean getInternetStatus(){
        ConnectivityManager connectivityManager = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo activeNetwork = connectivityManager.getActiveNetworkInfo();
        boolean isConnected = activeNetwork != null && activeNetwork.isConnectedOrConnecting();
        return isConnected;
    }

    /**Metodo getIpAddress de tipo publico que retorna un String, sirve para obtener la ip del dispositivo.*/
    public String getIpAddress() {
        WifiManager wifiManager = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);
        String ipAddress = Formatter.formatIpAddress(wifiManager.getConnectionInfo().getIpAddress());

        return ipAddress;
    }

    /**Metodo getLocationStatus de tipo publico que retorna un booleano, sirve para determinar si la ubicación se encuentra activa.*/
    public boolean getLocationStatus() {
        LocationManager locationManager = (LocationManager) context.getSystemService(Context.LOCATION_SERVICE);
        return locationManager.isProviderEnabled(LocationManager.GPS_PROVIDER);
    }

    public void startLocation(){
        if (ActivityCompat.checkSelfPermission(context, Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED && ActivityCompat.checkSelfPermission(context, Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {

            ActivityCompat.requestPermissions((Activity) context,new String[]{
                    Manifest.permission.ACCESS_COARSE_LOCATION,
                    Manifest.permission.ACCESS_FINE_LOCATION},0);
        }else {
            locationManager = (LocationManager) context.getSystemService(Context.LOCATION_SERVICE);
            Criteria criteria = new Criteria();
            locationManager.requestLocationUpdates(LocationManager.NETWORK_PROVIDER, 5000, 1, this);
            Location location = locationManager.getLastKnownLocation(locationManager.getBestProvider(criteria, false));
        }
    }


    /**Metodo getLatitude de tipo publico que retorna un booleano, sirve para obtener la latitud.*/
    public String getLatitude() {
       PreferencesUsuario prefs = new PreferencesUsuario(ConstantesCorresponsal.SHARED_PREFERENCES_LOCALIZACION,context);
        Log.d("Lat Guardada",prefs.getLatitud());
        return prefs.getLatitud();
    }

    /**Metodo getLatitude de tipo publico que retorna un booleano, sirve para obtener la longitud.*/
    public String getLongitude(){
        PreferencesUsuario prefs = new PreferencesUsuario(ConstantesCorresponsal.SHARED_PREFERENCES_LOCALIZACION,context);
        Log.d("Lon Guardada",prefs.getLongitud());
        return prefs.getLongitud();
    }

    /**Metodo getDeviceUUID de tipo publico que retorna un String, sirve para obtener el identificador unico del dispositivo*/
    public String getDeviceUUID(){
        return Settings.Secure.getString(context.getContentResolver(), Settings.Secure.ANDROID_ID);
    }

    /**Metodo getMacAddress de tipo publico que retora un String, sirve para obtener la dirección MAC del dispositivo*/
    public String getMacAddress(){
        WifiManager wifiManager = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);
        WifiInfo wInfo = wifiManager.getConnectionInfo();
        String macAddress = wInfo.getMacAddress();
        Log.e("TAG","MAC Address : " + macAddress);
        return macAddress;
    }

    /**Metodo getConsecutive de tipo publico que retorna un String, sirve para obtener el consecutivo de la transaccion*/
    public String getNewConsecutive(){
        CodificarBase64 base64 = new CodificarBase64();
        PreferencesUsuario prefs_parametricas = new PreferencesUsuario(ConstantesCorresponsal.SHARED_PREFERENCES_PARAMETRICAS,context);
        int consecutivo=0 ;
        String consecutivo_String="";
        if(!prefs_parametricas.getConsecutive().isEmpty()){
            consecutivo = Integer.parseInt(base64.decodificarBase64(prefs_parametricas.getConsecutive()))+1;
            consecutivo_String = String.valueOf(consecutivo);
            return consecutivo_String;
        }else{
            consecutivo_String = String.valueOf(consecutivo);
            return consecutivo_String;
        }
    }
    @Override
    public void onLocationChanged(Location location) {

        String  latitud = String.valueOf(location.getLatitude());
        String  longitud  = String.valueOf(location.getLongitude());
        Log.d("Latitud",latitud);
        Log.d("Longitud",longitud);

        PreferencesUsuario prefs_localizacion = new PreferencesUsuario(ConstantesCorresponsal.SHARED_PREFERENCES_LOCALIZACION,context);
        prefs_localizacion.setLongitud(longitud);
        prefs_localizacion.setLatitud(latitud);
        locationManager.removeUpdates(this);


        Message usuarioCancela = new Message();
        usuarioCancela.what = 7;
        LogIn.respuesta.sendMessage(usuarioCancela);
    }

    @Override
    public void onStatusChanged(String provider, int status, Bundle extras) {

    }

    @Override
    public void onProviderEnabled(String provider) {

    }

    @Override
    public void onProviderDisabled(String provider) {

    }



}
