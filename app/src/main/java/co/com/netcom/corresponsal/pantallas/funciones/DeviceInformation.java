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
import android.provider.Settings;
import android.telephony.TelephonyManager;
import android.text.format.Formatter;
import android.util.Log;

import androidx.core.app.ActivityCompat;

import com.google.android.gms.location.FusedLocationProviderClient;
import com.google.android.gms.location.LocationServices;
import com.google.android.gms.tasks.OnSuccessListener;

public class DeviceInformation implements LocationListener {
    private Context context;
    private String latitud;
    private String longitud;

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

/*    *//**Metodo getLatitude de tipo publico que retorna un booleano, sirve para obtener la latitud.*/
    public String getLatitude() {

        /*if (ActivityCompat.checkSelfPermission(context, Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED && ActivityCompat.checkSelfPermission(context, Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {

            ActivityCompat.requestPermissions((Activity) context,new String[]{
                    Manifest.permission.ACCESS_COARSE_LOCATION,
                    Manifest.permission.ACCESS_FINE_LOCATION},0);
            return "";
        }else{
            LocationManager locationManager = (LocationManager) context.getSystemService(Context.LOCATION_SERVICE);
            Criteria criteria = new Criteria();
            locationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER, 5000, 1, this);
            Location location = locationManager.getLastKnownLocation(locationManager.getBestProvider(criteria, false));
            //Log.d("LATITUD",String.valueOf(location.getLatitude()));
            return latitud;//String.valueOf(location.getLatitude());

        }*/
        // Get the location manager
        if (ActivityCompat.checkSelfPermission(context, Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED && ActivityCompat.checkSelfPermission(context, Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {

            ActivityCompat.requestPermissions((Activity) context,new String[]{
                    Manifest.permission.ACCESS_COARSE_LOCATION,
                    Manifest.permission.ACCESS_FINE_LOCATION},0);
            return "";
        }else {

            LocationManager locationManager = (LocationManager) context.getSystemService(context.LOCATION_SERVICE);
            Criteria criteria = new Criteria();
            String bestProvider = locationManager.getBestProvider(criteria, false);
            Location location = locationManager.getLastKnownLocation(bestProvider);
            Double lat, lon;
            try {
                lat = location.getLatitude();
                String resp ="";
                 resp = String.valueOf(lat);
                 return resp;
            } catch (NullPointerException e) {
                e.printStackTrace();
                return null;
            }
        }
    }



    /**Metodo getLatitude de tipo publico que retorna un booleano, sirve para obtener la longitud.*/
    public String getLongitude(){

        /*if (ActivityCompat.checkSelfPermission(context, Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED && ActivityCompat.checkSelfPermission(context, Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {

            ActivityCompat.requestPermissions((Activity) context,new String[]{
                    Manifest.permission.ACCESS_COARSE_LOCATION,
                    Manifest.permission.ACCESS_FINE_LOCATION},0);
            return "";
        }else{
            LocationManager locationManager = (LocationManager) context.getSystemService(Context.LOCATION_SERVICE);
            Criteria criteria = new Criteria();
            locationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER, 5000, 1, this);
            Location location = locationManager.getLastKnownLocation(locationManager.getBestProvider(criteria, false));
            //Log.d("LONGITUD",String.valueOf(location.getLongitude()));

            return longitud;//String.valueOf(location.getLongitude());

        }*/

        if (ActivityCompat.checkSelfPermission(context, Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED && ActivityCompat.checkSelfPermission(context, Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {

            ActivityCompat.requestPermissions((Activity) context,new String[]{
                    Manifest.permission.ACCESS_COARSE_LOCATION,
                    Manifest.permission.ACCESS_FINE_LOCATION},0);
            return "";
        }else {
            LocationManager locationManager = (LocationManager) context.getSystemService(context.LOCATION_SERVICE);
            Criteria criteria = new Criteria();
            String bestProvider = locationManager.getBestProvider(criteria, false);
            Location location = locationManager.getLastKnownLocation(bestProvider);
            Double lat, lon;
            try {
                lon = location.getLongitude();
                String resp = "";
                resp = String.valueOf(lon);
                return resp;
            } catch (NullPointerException e) {
                e.printStackTrace();
                return null;
            }
        }
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
            Log.d("LATITUD",String.valueOf(location.getLatitude()));
            Log.d("LONGITUD",String.valueOf(location.getLongitude()));
            this.latitud = String.valueOf(location.getLatitude());
            this.longitud = String.valueOf(location.getLongitude());
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
