package co.com.netcom.corresponsal.core.comunicacion;

import android.os.RemoteException;

public interface EMVCallback {
    void procesarTarjetaCallback(CardDTO infoTarjeta) throws RemoteException;
    void procesarDatosAdicionalesCallback(CardDTO infoTarjeta) throws RemoteException;
    void finalizarProcesoTimeOutStop() throws RemoteException;
    void onErrorMagneticCard(int error)throws RemoteException;
}
