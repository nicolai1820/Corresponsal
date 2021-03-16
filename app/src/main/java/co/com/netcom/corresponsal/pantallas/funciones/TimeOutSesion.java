package co.com.netcom.corresponsal.pantallas.funciones;

import android.app.Activity;
import android.app.ActivityManager;
import android.content.Context;
import android.os.AsyncTask;
import android.util.Log;

import java.util.Calendar;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.ExecutionException;

public class TimeOutSesion extends Activity {

    public interface TimeOutSesionListener {

        void doLogout();

    }

    static long hora1, minutos1, segundos1, horainicial, hora2, minutos2, segundos2, segundahora, diferencia;
    static int contador =0;

    static Timer longTimer;
    static final int LOGOUT_TIME = 300000; // delay in milliseconds i.e. 5 min = 300000 ms or use timeout argument

    public static synchronized void startLogoutTimer(final Context context, final TimeOutSesionListener logOutListener) {
        if (longTimer != null) {
            longTimer.cancel();
            longTimer = null;
        }
        if (longTimer == null) {

            //Primera interacción en cuanto a creación de la primera pantalla o
            //interaación del usuario
            if(contador==0) {
                Calendar calendar = Calendar.getInstance();
                hora1 =calendar.get(Calendar.HOUR_OF_DAY)*60*60*1000;
                minutos1 = calendar.get(Calendar.MINUTE)*60*1000;
                segundos1 = calendar.get(Calendar.SECOND)*1000;
                horainicial = hora1 + minutos1 +segundos1;
                Log.d("Primera hora",String.valueOf(horainicial));
                contador++;
            }else {
                //Segunda interacción del usuario
                Calendar calendar2 = Calendar.getInstance();
                hora2 =calendar2.get(Calendar.HOUR_OF_DAY)*60*60*1000;
                minutos2 = calendar2.get(Calendar.MINUTE)*60*1000;
                segundos2 = calendar2.get(Calendar.SECOND)*1000;
                segundahora = hora2 + minutos2 +segundos2;

                Log.d("HORA INICIAL",String.valueOf(horainicial));
                Log.d("HORA 2",String.valueOf(horainicial));
                diferencia = segundahora - horainicial;
                Log.d("DIFETENCA HORAS",String.valueOf(diferencia));

                if(diferencia>=230000&&diferencia<=295000){
                    horainicial = segundahora;
                    segundahora=0;

                    Log.d("SOLICITANDO TOKEN","token");
                    Servicios servicios = new Servicios(context);
                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            PreferencesUsuario prefs = new PreferencesUsuario(ConstantesCorresponsal.SHARED_PREFERENCES_INFO_USUARIO,context);

                            servicios.refrescarToken(prefs.getUserId());
                        }
                    }).start();
                    /*Token t  = new Token(context);

                    t.refreshToken();

                    Log.d("SOLICITANDO TOKEN","token");*/
                }

            }

            longTimer = new Timer();

            longTimer.schedule(new TimerTask() {

                public void run() {

                    cancel();

                    longTimer = null;

                    try {
                        boolean foreGround = new ForegroundCheckTask().execute(context).get();

                        if (foreGround) {
                            stopLogoutTimer();
                            logOutListener.doLogout();
                        }

                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    } catch (ExecutionException e) {
                        e.printStackTrace();
                    }

                }
            }, LOGOUT_TIME);
        }
    }

    public static synchronized void stopLogoutTimer() {
        if (longTimer != null) {
            longTimer.cancel();
            longTimer = null;
        }
    }

    static class ForegroundCheckTask extends AsyncTask<Context, Void, Boolean> {

        @Override
        protected Boolean doInBackground(Context... params) {
            final Context context = params[0].getApplicationContext();
            return isAppOnForeground(context);
        }

        private boolean isAppOnForeground(Context context) {
            ActivityManager activityManager = (ActivityManager) context.getSystemService(Context.ACTIVITY_SERVICE);
            List<ActivityManager.RunningAppProcessInfo> appProcesses = activityManager.getRunningAppProcesses();
            if (appProcesses == null) {
                return false;
            }
            final String packageName = context.getPackageName();
            for (ActivityManager.RunningAppProcessInfo appProcess : appProcesses) {
                if (appProcess.importance == ActivityManager.RunningAppProcessInfo.IMPORTANCE_FOREGROUND && appProcess.processName.equals(packageName)||
                        appProcess.importance == ActivityManager.RunningAppProcessInfo.IMPORTANCE_BACKGROUND||appProcess.importance == ActivityManager.RunningAppProcessInfo.IMPORTANCE_TOP_SLEEPING) {
                    return true;
                }
            }
            return false;
        }
    }




}
