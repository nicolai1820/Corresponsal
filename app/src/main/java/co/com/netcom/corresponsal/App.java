package co.com.netcom.corresponsal;

import android.app.Application;
import android.content.Context;

import co.com.netcom.corresponsal.emv.Constants;
import co.com.netcom.corresponsal.emv.DeviceService;

public class App extends Application {
    private static final String TAG = "Aplicación";

    //Marca del datafono. TODO obtener en ejecución
    private static int terminalType = Constants.INGENICO_TERMINAL;

    //Atributos Ingenico
    private static final String USDK_ACTION_NAME = "com.usdk.apiservice";
    private static final String USDK_PACKAGE_NAME = "com.usdk.apiservice";
    private static DeviceService deviceService;
    private static Context context;

    //Atributos Newland
    private static App mApplication;

    /* comentatario agregador por mi

    @Override
    public void onCreate() {
        super.onCreate();
        DeviceHelper.me().init(getApplicationContext());



        if (!Procesos.validarDispositivo()) {
            Log.d("Tipo dispositivo: ", "Real");
            PrinterUtils.initWebView(getApplicationContext(), 372);
            new Thread(() -> DeviceHelper.me().bindService()).start();
        } else {
            Log.d("Tipo dispositivo: ", "Virtual");
        }

        if (terminalType == Constants.INGENICO_TERMINAL) {
            Log.d(TAG, "Constants.INGENICO_TERMINAL: "+Constants.INGENICO_TERMINAL);
            context = getApplicationContext();

            // Bind sdk device service.
            bindSdkDeviceService();
    */
            // Create a global webView to load print template
            //Printer.initWebView(context);

            //Completable.complete().delay(3, TimeUnit.SECONDS)
            //        .andThen(Printer.preLoadHtml("multi-languages-template", "{}"))
             //       .subscribe();
/*  x2
            if (BuildConfig.DEBUG) {
                Timber.plant(new Timber.DebugTree());
            }
        }
    }
*/
    /*@Override
    public void onTerminate() {
        DeviceHelper.me().unbindService();
        super.onTerminate();
    }*/

    /*  x3
    @Override
    public void onTerminate() {
        super.onTerminate();

        try {
            deviceService.unregister();
        } catch (RemoteException e) {
            e.printStackTrace();
        }
        unbindService(serviceConnection);
        System.exit(0);
    }*/

    /**
     * Get context.
     */

    /*  x4
    public static Context getContext() {
        if (context == null) {
            throw new RuntimeException("Initiate context failed");
        }

        return context;
    }*/

    /**
     * Get device service instance.
     */
    /* x5
    public static DeviceService getDeviceService() {
        if (deviceService == null) {
            throw new RuntimeException("SDK service is still not connected.");
        }

        return deviceService;
    }
*/
    /**
     * Bind sdk service.
     */

  /* x6
    private void bindSdkDeviceService() {
        Intent intent = new Intent();
        intent.setAction(USDK_ACTION_NAME);
        intent.setPackage(USDK_PACKAGE_NAME);

        Log.d(TAG, "binding sdk device service...");
        boolean flag = bindService(intent, serviceConnection, Context.BIND_AUTO_CREATE);
        if (!flag) {
            Log.d(TAG, "SDK service binding failed.");
            return;
        }

        Log.d(TAG, "SDK service binding successfully.");
    }
*/
    /**
     * Service connection.
     */

  /* x7
    private ServiceConnection serviceConnection = new ServiceConnection() {

        @Override
        public void onServiceDisconnected(ComponentName name) {
            Log.d(TAG, "SDK service disconnected.");
            deviceService = null;
        }

        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            Log.d(TAG, "SDK service connected.");

            try {
                deviceService = new DeviceService(UDeviceService.Stub.asInterface(service));
                deviceService.register();
                deviceService.debugLog(true, true);
                Log.d(TAG, "SDK deviceService initiated version:" + deviceService.getVersion() + ".");
            } catch (RemoteException e) {
                throw new RuntimeException("SDK deviceService initiating failed.", e);
            }

            try {
                linkToDeath(service);
            } catch (RemoteException e) {
                throw new RuntimeException("SDK service link to death error.", e);
            }
        }

        private void linkToDeath(IBinder service) throws RemoteException {
            service.linkToDeath(() -> {
                Log.d(TAG, "SDK service is dead. Reconnecting...");
                bindSdkDeviceService();
            }, 0);
        }
    };

    public static int getTerminalType() {
        return terminalType;
    }

    public static Application getApplication() {
        if (mApplication == null)
            Log.e("Application:","App->getContext error！");
        return mApplication;
    }
*/
}
