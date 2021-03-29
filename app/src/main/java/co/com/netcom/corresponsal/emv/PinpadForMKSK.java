package co.com.netcom.corresponsal.emv;

/**
 * Pinpad for MK/SK API.
 */

public class PinpadForMKSK extends Pinpad {

    /**
     * Creator.
     */
    private static class Creator {
        private static final PinpadForMKSK INSTANCE = new PinpadForMKSK();
    }

    /**
     * Get pinpad for MK/SK instance.
     */
    public static PinpadForMKSK getInstance() {
        return Creator.INSTANCE;
    }

    /**
     * Constructor.
     */
    private PinpadForMKSK() {
        super(null,null);
        // super(App.getContext(), App.getDeviceService().getPinpadForMKSK());
    }
}
