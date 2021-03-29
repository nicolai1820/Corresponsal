package co.com.netcom.corresponsal.emv;

/**
 * EMV parameter.
 */

interface EmvParameter {

    String pack() throws EmvParameterException;
}
