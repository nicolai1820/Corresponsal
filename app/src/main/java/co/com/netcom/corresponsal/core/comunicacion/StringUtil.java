package co.com.netcom.corresponsal.core.comunicacion;

import java.nio.charset.Charset;
import java.util.regex.Pattern;

import static android.text.TextUtils.isEmpty;

public class StringUtil {
    /**
     * Platform default encoding.
     */
    private static final String PLATFORM_DEFAULT_ENCODING = Charset.defaultCharset().name();

    /**
     * Assume shift JIS.
     */
    private static final boolean ASSUME_SHIFT_JIS = ("SJIS".equalsIgnoreCase(PLATFORM_DEFAULT_ENCODING)) ||
            ("EUC_JP".equalsIgnoreCase(PLATFORM_DEFAULT_ENCODING));

    /**
     * Whether is number.
     */
    public static boolean isNumber(String string) {
        Pattern pattern = Pattern.compile("[0-9]*");
        return pattern.matcher(string).matches();
    }

    /**
     * Filter digits string from a raw string.
     */
    public static String getDigits(String data) {
        StringBuilder sb = new StringBuilder();
        if (isNumber(data)) {
            return data;
        }

        for (int i = 0; i < data.length(); i++) {
            char c = data.charAt(i);
            if (c <= '9' && c >= '0') {
                sb.append(c);
            }
        }
        return sb.toString();
    }

    /**
     * Format string data.
     */
    public static String formatString(String value, String formatString, boolean isLeftDirection) {
        StringBuilder result = new StringBuilder();

        if (isLeftDirection) {
            for (int i=0, n=0; i<formatString.length() && n<value.length(); i++) {
                char cur = formatString.charAt(i);

                if (cur == 'x' || cur == 'X') {
                    result.append(value.charAt(n));
                    n++;
                } else {
                    result.append(cur);
                }
            }
        } else {
            for (int i=formatString.length()-1, n=value.length()-1; i>=0 && n>=0; i--) {
                char cur = formatString.charAt(i);
                if (cur == 'x' || cur == 'X') {
                    result.insert(0, value.charAt(n));
                    n--;
                } else {
                    result.insert(0, cur);
                }
            }
        }

        return result.toString();
    }

    /**
     * Get readable amount.
     */
    public static String getReadableAmount(String amount) {
        String formattedAmount;
        if (amount.length() > 2) {
            formattedAmount = amount.substring(0, amount.length() - 2);
            formattedAmount += ".";
            formattedAmount += amount.substring(amount.length() - 2);
        } else if (amount.length() == 2) {
            formattedAmount = "0.";
            formattedAmount += amount;
        } else if (amount.length() == 1) {
            formattedAmount = "0.0";
            formattedAmount += amount;
        } else {
            formattedAmount = "0.00";
        }

        return formattedAmount;
    }

    public static String leftPad(String str, int size, String padStr) {
        if(str == null) {
            return null;
        } else {
            if(isEmpty(padStr)) {
                padStr = " ";
            }

            int padLen = padStr.length();
            int strLen = str.length();
            int pads = size - strLen;
            if(pads <= 0) {
                return str;
            } else if(padLen == 1 && pads <= 8192) {
                return leftPad(str, size, padStr.charAt(0));
            } else if(pads == padLen) {
                return padStr.concat(str);
            } else if(pads < padLen) {
                return padStr.substring(0, pads).concat(str);
            } else {
                char[] padding = new char[pads];
                char[] padChars = padStr.toCharArray();

                for(int i = 0; i < pads; ++i) {
                    padding[i] = padChars[i % padLen];
                }

                return (new String(padding)).concat(str);
            }
        }
    }
    public static String leftPad(String str, int size) {
        return leftPad(str, size, ' ');
    }

    public static String leftPad(String str, int size, char padChar) {
        if(str == null) {
            return null;
        } else {
            int pads = size - str.length();
            return pads <= 0?str:(pads > 8192?leftPad(str, size, String.valueOf(padChar)):padding(pads, padChar).concat(str));
        }
    }
    private static String padding(int repeat, char padChar) throws IndexOutOfBoundsException {
        if(repeat < 0) {
            throw new IndexOutOfBoundsException("Cannot pad a negative amount: " + repeat);
        } else {
            char[] buf = new char[repeat];

            for(int i = 0; i < buf.length; ++i) {
                buf[i] = padChar;
            }

            return new String(buf);
        }
    }

    public static String hexToAscii(String hexStr) {
        StringBuilder output = new StringBuilder("");

        for (int i = 0; i < hexStr.length(); i += 2) {
            String str = hexStr.substring(i, i + 2);
            output.append((char) Integer.parseInt(str, 16));
        }

        return output.toString();
    }
}
