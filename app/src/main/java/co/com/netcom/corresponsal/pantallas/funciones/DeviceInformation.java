package co.com.netcom.corresponsal.pantallas.funciones;

import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.location.Criteria;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.provider.Settings;
import android.telephony.TelephonyManager;
import android.text.format.Formatter;

import androidx.core.app.ActivityCompat;

public class DeviceInformation implements LocationListener {
    private Context context;
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

    /**Metodo getLatitude de tipo publico que retorna un booleano, sirve para obtener la latitud.*/
    public String getLatitude() {

        if (ActivityCompat.checkSelfPermission(context, Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED && ActivityCompat.checkSelfPermission(context, Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {

            ActivityCompat.requestPermissions((Activity) context,new String[]{
                    Manifest.permission.ACCESS_COARSE_LOCATION,
                    Manifest.permission.ACCESS_FINE_LOCATION},0);
            return "";
        }else{
            LocationManager locationManager = (LocationManager) context.getSystemService(Context.LOCATION_SERVICE);
            Criteria criteria = new Criteria();
            locationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER, 5000, 5, this);
            Location location = locationManager.getLastKnownLocation(locationManager.getBestProvider(criteria, false));
            return String.valueOf(location.getLatitude());

        }
    }

    /**Metodo getLatitude de tipo publico que retorna un booleano, sirve para obtener la longitud.*/
    public String getLongitude(){

        if (ActivityCompat.checkSelfPermission(context, Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED && ActivityCompat.checkSelfPermission(context, Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {

            ActivityCompat.requestPermissions((Activity) context,new String[]{
                    Manifest.permission.ACCESS_COARSE_LOCATION,
                    Manifest.permission.ACCESS_FINE_LOCATION},0);
            return "";
        }else{
            LocationManager locationManager = (LocationManager) context.getSystemService(Context.LOCATION_SERVICE);
            Criteria criteria = new Criteria();
            locationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER, 5000, 5, this);
            Location location = locationManager.getLastKnownLocation(locationManager.getBestProvider(criteria, false));
            return String.valueOf(location.getLongitude());

        }
    }

    /**Metodo getDeviceUUID de tipo publico que retorna un String, sirve para obtener el identificador unico del dispositivo*/
    public String getDeviceUUID(){
        return Settings.Secure.getString(context.getContentResolver(), Settings.Secure.ANDROID_ID);
    }


    @Override
    public void onLocationChanged(Location location) {

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
