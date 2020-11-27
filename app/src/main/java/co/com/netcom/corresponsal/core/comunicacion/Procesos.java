package co.com.netcom.corresponsal.core.comunicacion;

import android.os.Build;

public class Procesos {
    //Tabla de procesos para ejecución contra el integrador

    //Transacciones 1000
    public final static int TRANSACCIONES_ECHO_TEST = 1000;
    public final static int TRANSACCIONES_LOGON = 1010;
    public final static int TRANSACCIONES_INICIALIZACION = 1020;
    public final static int TRANSACCIONES_INICIALIZACION_INIPARAMETROS = 1030;
    public final static int TRANSACCIONES_CIERRE = 1040;
    public final static int TRANSACCIONES_DUPLICADO = 1050;
    public final static int TRANSACCIONES_REPORTE = 1060;
    public final static int TRANSACCIONES_ANULACION = 1070;
    public final static int TRANSACCIONES_CONSULTA_SALDO = 1080;
    public final static int TRANSACCIONES_CAMBIO_TERMINAL = 1090;

    //Corresponsal 2000
    public final static int CORRESPONSAL_RETIRO_CON_TARJETA = 2000;
    public final static int CORRESPONSAL_RETIRO_SIN_TARJETA = 2010;
    public final static int CORRESPONSAL_DEPOSITO = 2020;
    public final static int CORRESPONSAL_DUPLICADO = 2030;
    public final static int CORRESPONSAL_CONSULTA_FACTURAS_LECTOR = 2040;
    public final static int CORRESPONSAL_PAGO_FACTURAS_LECTOR = 2041;
    public final static int CORRESPONSAL_CONSULTA_FACTURAS = 2050;
    public final static int CORRESPONSAL_PAGO_FACTURA_MANUAL = 2051;
    public final static int CORRESPONSAL_PAGO_FACTURA_TARJETA_EMPRESARIAL = 2060;
    public final static int CORRESPONSAL_RECARGAS = 2070;
    public final static int CORRESPONSAL_PAGO_PRODUCTOS = 2080;
    public final static int CORRESPONSAL_TRANSFERENCIA = 2090;
    public final static int CORRESPONSAL_CONSULTA_SALDO = 2100;
    public final static int CORRESPONSAL_ENVIO_GIRO = 2110;
    public final static int CORRESPONSAL_RECLAMACION_GIRO = 2120;
    public final static int CORRESPONSAL_CANCELACION_GIRO = 2130;
    public final static int CORRESPONSAL_CANCELACION_GIRO_CONSULTA = 2131;
    public final static int CORRESPONSAL_CONSULTA_CUPO = 2140;
    public final static int CORRESPONSAL_CIERRE_CNB = 2150;
    public final static int CORRESPONSAL_ECHO_TEST = 2160;
    public final static int CORRESPONSAL_INICIALIZACION = 2170;
    public final static int CORRESPONSAL_INICIALIZACION_INIPARAMETROS = 2180;
    public final static int CORRESPONSAL_ANULACION = 2190;

    //Adminastrador 3000

    //Configuracion 4000
    public final static int CONFIGURACION_ENVIO_SERIAL = 4000;
    public final static int CONFIGURACION_BORRADO_TRANSACCIONES = 4010;
    public final static int CONFIGURACION_BORRADO_PARAMETROS = 4020;
    public final static int CONFIGURACION_SEMBRADO_TARJETA = 4030;


    //Otras Tarns. 5000

    // Flujo Unificado

    public final static int FLUJO_UNIFICADO = 6000;
    public final static int QR_CODE_SOLICITUD = 7000;
    public final static int QR_CODE_CONSULTA = 7100;
    public final static int DINERO_ELECTRONICO = 8000;
    public final static int LEALTAD_LIFEMILES = 9000;
    public final static int TRANSACCION_REVERSO = 5000;

    // Global
    public static boolean validarDispositivo() {
        if (Build.FINGERPRINT.startsWith("generic")
                || Build.FINGERPRINT.startsWith("unknown")
                || Build.MODEL.contains("google_sdk")
                || Build.MODEL.contains("Emulator")
                || Build.MODEL.contains("Android SDK built for x86")
                || Build.BOARD == "QC_Reference_Phone" //bluestacks
                || Build.MANUFACTURER.contains("Genymotion")
                || Build.HOST.startsWith("Build") //MSI App Player
                || (Build.BRAND.startsWith("generic") && Build.DEVICE.startsWith("generic"))
                || "google_sdk" == Build.PRODUCT) {
            return true;
        } else {
            return false;
        }
    }
}
