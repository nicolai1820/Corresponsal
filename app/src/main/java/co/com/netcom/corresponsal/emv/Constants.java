package co.com.netcom.corresponsal.emv;

/**
 * Created by alvaroriosch on 29/11/18.
 */

public class Constants {

    //Firebase constants
    public final static String INITIAL_PATH = "Configuraciones/";
    public final static String SUBMENU_PATH = "submenu/";

    //Terminal info
    public final static int INGENICO_TERMINAL = 1;
    public final static int NEWLAND_TERMINAL = 2;
    public final static int POYNT_TERMINAL = 3;
    public final static String TERMINAL = "ABCD1234";

    //Card info constants
    public static final int MAGNETIC_STRIPE_MODE = 1;
    public static final int CHIP_CARD_MODE = 2;
    public static final int CONTACTLESS_MODE = 3;

    //transacciones
    public static final String CHARSET_ISO = "ISO8859-1";
    public static final String SOCKET_TIMEOUT = "No response: Socket timeout";
    //pruebas netcom
    //public static final String SERVER_IP = "172.17.1.201";
    //public static final int SERVER_PORT = 5681;
    //pruebas redeban
    public static final String SERVER_IP = "190.24.137.204";
    public static final int SERVER_PORT = 6000;

    //Intent keys
    public static final String INDEX_NUMBER = "index";

    //sale constants
    public static final int EMV_EVENT_WAIT_CARD = 0;
    public static final int EMV_EVENT_CARD_CHECKED = 1;
    public static final int EMV_EVENT_FINAL_SELECT = 2;
    public static final int EMV_EVENT_READ_RECORD = 3;
    public static final int EMV_EVENT_CARD_HOLDER_VERIFY = 4;
    public static final int EMV_EVENT_ONLINE_PROCESS = 5;
    public static final int EMV_EVENT_END_PROCESS = 6;
    public static final int EMV_EVENT_SEND_OUT = 7;
    public static final int MAG_EVENT_SWIPE_CARD = 8;
    public static final int MAG_EVENT_ONLINE_PROCESS = 9;
    public static final int PIN_EVENT_INPUT_CONFIRM = 10;
    public static final int PIN_EVENT_INPUT_CANCEL = 11;
    public static final int PIN_EVENT_INPUT_ERROR = 12;
    public static final int PRT_EVENT_FINISHED = 13;
    public static final int PRT_EVENT_ERROR = 14;
    public static final int PRT_EVENT_HTML_ERROR = 15;
    public static final int COM_EVENT_COMPLETED = 16;

    //Newland
    public static final int TRACK2_BUS_CODE = 3010;
    public static final int TAG55_BUS_CODE = 3011;
}
