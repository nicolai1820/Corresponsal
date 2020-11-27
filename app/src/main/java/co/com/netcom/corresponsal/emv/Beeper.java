package co.com.netcom.corresponsal.emv;

import android.os.RemoteException;

/**
 * Beeper API.
 */

public class Beeper {

    /**
     * Beeper object.
     */
    //private UBeeper beeper = App.getDeviceService().getBeeper();

    /**
     * Start beep.
     */
//    public void startBeep(int milliseconds) throws RemoteException {
//        beeper.startBeep(milliseconds);
//    }

    /**
     * Stop beep.
     */
    public void stopBeep() throws RemoteException {
//        beeper.stopBeep();
    }

    /**
     * Creator.
     */
    private static class Creator {
        private static final Beeper INSTANCE = new Beeper();
    }

    /**
     * Get beeper instance.
     */
    public static Beeper getInstance() {
        return Creator.INSTANCE;
    }

    /**
     * Constructor.
     */
    private Beeper() {

    }
}
