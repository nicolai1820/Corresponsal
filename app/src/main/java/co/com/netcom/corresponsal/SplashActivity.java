/*package co.com.netcom.apos;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;

import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;

import java.util.Timer;
import java.util.TimerTask;

public class SplashActivity extends AppCompatActivity {
    public static final String TAG = "SplashActivity";
    private void fatalError(final String title, final String message, final DialogInterface.OnClickListener onQuit) {
        new AlertDialog.Builder(SplashActivity.this)
                .setTitle(title)
                .setMessage(message)
                .setCancelable(false)
                .setPositiveButton("QUIT", onQuit)
                .create()
                .show();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_splash);
        new Timer().schedule(new TimerTask() {
            @Override
            public void run() {
                try {
                    // initialize PinpadManager that will, in turn, initialize Pinpad
                    PinpadManager.init(getApplicationContext());
                    Log.d(TAG, "run: PinpadManager.init");

                    runOnUiThread(() -> {
                        Log.d(TAG, "Hilo: StartActivity a: MainActivity");
                        startActivity(new Intent(SplashActivity.this, MainActivity.class));
                        finish();
                    });

                } catch (Exception e) {
                    Log.e(TAG, "Initialization error", e);

                    // Fatal error: could not connect with USDKApiService or start PinpadManager.
                    // In this case, only show an AlertDialog and shutdown.
                    runOnUiThread(() -> fatalError("Initialization Error", "Could not connect to POS services, aborting!", (dialogInterface, i) -> SplashActivity.this.finish()));
                }
            }
        }, 500);
    }
}
*/