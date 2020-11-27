package co.com.netcom.corresponsal.core.comunicacion;

import java.io.UnsupportedEncodingException;
import java.math.BigDecimal;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.BitSet;
import java.util.StringTokenizer;

public class ISOUtils {
    public static final String[] hexStrings = new String[256];
    public static final String ENCODING = "ISO8859_1";
    public static final byte[] EBCDIC2ASCII;
    public static final byte[] ASCII2EBCDIC;
    public static final byte STX = 2;
    public static final byte FS = 28;
    public static final byte US = 31;
    public static final byte RS = 29;
    public static final byte GS = 30;
    public static final byte ETX = 3;

    private ISOUtils() {
        throw new AssertionError();
    }

    public static String ebcdicToAscii(byte[] var0) {
        try {
            return new String(ebcdicToAsciiBytes(var0, 0, var0.length), "ISO8859_1");
        } catch (UnsupportedEncodingException var2) {
            return var2.toString();
        }
    }

    public static String ebcdicToAscii(byte[] var0, int var1, int var2) {
        try {
            return new String(ebcdicToAsciiBytes(var0, var1, var2), "ISO8859_1");
        } catch (UnsupportedEncodingException var4) {
            return var4.toString();
        }
    }

    public static byte[] ebcdicToAsciiBytes(byte[] var0) {
        return ebcdicToAsciiBytes(var0, 0, var0.length);
    }

    public static byte[] ebcdicToAsciiBytes(byte[] var0, int var1, int var2) {
        byte[] var3 = new byte[var2];

        for(int var4 = 0; var4 < var2; ++var4) {
            var3[var4] = EBCDIC2ASCII[var0[var1 + var4] & 255];
        }

        return var3;
    }

    public static byte[] asciiToEbcdic(String var0) {
        return asciiToEbcdic(var0.getBytes());
    }

    public static byte[] asciiToEbcdic(byte[] var0) {
        byte[] var1 = new byte[var0.length];

        for(int var2 = 0; var2 < var0.length; ++var2) {
            var1[var2] = ASCII2EBCDIC[var0[var2] & 255];
        }

        return var1;
    }

    public static void asciiToEbcdic(String var0, byte[] var1, int var2) {
        int var3 = var0.length();

        for(int var4 = 0; var4 < var3; ++var4) {
            var1[var2 + var4] = ASCII2EBCDIC[var0.charAt(var4) & 255];
        }

    }

    public static String padleft(String var0, int var1, char var2) {
        var0 = var0.trim();
        if (var0.length() > var1) {
            return "invalid param:" + var0 + " invalid len " + var0.length() + "/" + var1;
        } else {
            StringBuilder var3 = new StringBuilder(var1);
            int var4 = var1 - var0.length();

            while(var4-- > 0) {
                var3.append(var2);
            }

            var3.append(var0);
            return var3.toString();
        }
    }

    public static byte[] padLeft(byte[] var0, int var1, byte var2) {
        if (var0.length >= var1) {
            return var0;
        } else {
            ByteBuffer var3 = ByteBuffer.allocate(var1);
            byte[] var4 = new byte[var1 - var0.length];
            Arrays.fill(var4, var2);
            var3.put(var4);
            var3.put(var0);
            return var3.array();
        }
    }

    public static byte[] unpadLeft(byte[] var0, byte var1) {
        int var2;
        for(var2 = 0; var2 < var0.length && var0[var2] == var1; ++var2) {
        }

        if (var2 < var0.length) {
            byte[] var3 = new byte[var0.length - var2];
            System.arraycopy(var0, var2, var3, 0, var3.length);
            return var3;
        } else {
            return new byte[0];
        }
    }

    public static byte[] unpadRight(byte[] var0, byte var1) {
        int var2;
        for(var2 = var0.length; var2 > 0 && var0[var2 - 1] == var1; --var2) {
        }

        if (var2 > 0) {
            byte[] var3 = new byte[var2];
            System.arraycopy(var0, 0, var3, 0, var2);
            return var3;
        } else {
            return new byte[0];
        }
    }

    public static byte[] padRight(byte[] var0, int var1, byte var2) {
        if (var0.length >= var1) {
            return var0;
        } else {
            byte[] var3 = new byte[var1 - var0.length];
            Arrays.fill(var3, var2);
            ByteBuffer var4 = ByteBuffer.allocate(var1);
            var4.put(var0);
            var4.put(var3);
            return var4.array();
        }
    }

    public static String padright(String var0, int var1, char var2) {
        var0 = var0.trim();
        if (var0.length() > var1) {
            return "invalid param:" + var0 + " invalid len " + var0.length() + "/" + var1;
        } else {
            StringBuilder var3 = new StringBuilder(var1);
            int var4 = var1 - var0.length();
            var3.append(var0);

            while(var4-- > 0) {
                var3.append(var2);
            }

            return var3.toString();
        }
    }

    public static String trim(String var0) {
        return var0 != null ? var0.trim() : null;
    }

    public static String zeropad(String var0, int var1) {
        return padleft(var0, var1, '0');
    }

    public static String zeropad(long var0, int var2) {
        try {
            return padleft(Long.toString((long)((double)var0 % Math.pow(10.0D, (double)var2))), var2, '0');
        } catch (Exception var4) {
            return null;
        }
    }

    public static String strpad(String var0, int var1) {
        StringBuilder var2 = new StringBuilder(var0);

        while(var2.length() < var1) {
            var2.append(' ');
        }

        return var2.toString();
    }

    public static String zeropadRight(String var0, int var1) {
        StringBuilder var2 = new StringBuilder(var0);

        while(var2.length() < var1) {
            var2.append('0');
        }

        return var2.toString();
    }

    public static byte[] str2bcd(String var0, boolean var1, byte[] var2, int var3) {
        int var4 = var0.length();
        int var5 = (var4 & 1) == 1 && var1 ? 1 : 0;

        for(int var6 = var5; var6 < var4 + var5; ++var6) {
            int var7 = var0.charAt(var6 - var5);
            if (var7 > 64) {
                char var8 = Character.toLowerCase((char)var7);
                var7 = var8 - 97 + 10;
            } else if (var7 >= 48) {
                var7 -= 48;
            }

            var2[var3 + (var6 >> 1)] = (byte)(var2[var3 + (var6 >> 1)] | var7 << ((var6 & 1) == 1 ? 0 : 4));
        }

        return var2;
    }

    public static byte[] str2bcd(String var0, boolean var1) {
        int var2 = var0.length();
        byte[] var3 = new byte[var2 + 1 >> 1];
        return str2bcd(var0, var1, var3, 0);
    }

    public static byte[] str2bcd(String var0, boolean var1, byte var2) {
        int var3 = var0.length();
        byte[] var4 = new byte[var3 + 1 >> 1];
        Arrays.fill(var4, var2);
        int var5 = (var3 & 1) == 1 && var1 ? 1 : 0;

        for(int var6 = var5; var6 < var3 + var5; ++var6) {
            var4[var6 >> 1] = (byte)(var4[var6 >> 1] | var0.charAt(var6 - var5) - 48 << ((var6 & 1) == 1 ? 0 : 4));
        }

        return var4;
    }

    public static String bcd2str(byte[] var0, int var1, int var2, boolean var3) {
        StringBuilder var4 = new StringBuilder(var2);
        int var5 = (var2 & 1) == 1 && var3 ? 1 : 0;

        for(int var6 = var5; var6 < var2 + var5; ++var6) {
            int var7 = (var6 & 1) == 1 ? 0 : 4;
            char var8 = Character.forDigit(var0[var1 + (var6 >> 1)] >> var7 & 15, 16);
            if (var8 == 'd') {
                var8 = '=';
            }

            var4.append(Character.toUpperCase(var8));
        }

        return var4.toString();
    }

    public static String hexString(byte[] var0) {
        StringBuilder var1 = new StringBuilder(var0.length * 2);
        byte[] var2 = var0;
        int var3 = var0.length;

        for(int var4 = 0; var4 < var3; ++var4) {
            byte var5 = var2[var4];
            var1.append(hexStrings[var5 & 255]);
        }

        return var1.toString();
    }

    public static String dumpString(byte[] var0) {
        StringBuilder var1 = new StringBuilder(var0.length * 2);

        for(int var2 = 0; var2 < var0.length; ++var2) {
            char var3 = (char)var0[var2];
            if (Character.isISOControl(var3)) {
                switch(var3) {
                    case '\u0000':
                        var1.append("{NULL}");
                        break;
                    case '\u0001':
                        var1.append("{SOH}");
                        break;
                    case '\u0002':
                        var1.append("{STX}");
                        break;
                    case '\u0003':
                        var1.append("{ETX}");
                        break;
                    case '\u0004':
                        var1.append("{EOT}");
                        break;
                    case '\u0005':
                        var1.append("{ENQ}");
                        break;
                    case '\u0006':
                        var1.append("{ACK}");
                        break;
                    case '\u0007':
                        var1.append("{BEL}");
                        break;
                    case '\b':
                    case '\t':
                    case '\u000b':
                    case '\f':
                    case '\u000e':
                    case '\u000f':
                    case '\u0011':
                    case '\u0012':
                    case '\u0013':
                    case '\u0014':
                    case '\u0017':
                    case '\u0018':
                    case '\u0019':
                    case '\u001a':
                    case '\u001b':
                    case '\u001d':
                    default:
                        var1.append('[');
                        var1.append(hexStrings[var0[var2] & 255]);
                        var1.append(']');
                        break;
                    case '\n':
                        var1.append("{LF}");
                        break;
                    case '\r':
                        var1.append("{CR}");
                        break;
                    case '\u0010':
                        var1.append("{DLE}");
                        break;
                    case '\u0015':
                        var1.append("{NAK}");
                        break;
                    case '\u0016':
                        var1.append("{SYN}");
                        break;
                    case '\u001c':
                        var1.append("{FS}");
                        break;
                    case '\u001e':
                        var1.append("{RS}");
                }
            } else {
                var1.append(var3);
            }
        }

        return var1.toString();
    }

    public static String hexString(byte[] var0, int var1, int var2) {
        StringBuilder var3 = new StringBuilder(var2 * 2);
        var2 += var1;

        for(int var4 = var1; var4 < var2; ++var4) {
            var3.append(hexStrings[var0[var4] & 255]);
        }

        return var3.toString();
    }

    public static String bitSet2String(BitSet var0) {
        int var1 = var0.size();
        var1 = var1 > 128 ? 128 : var1;
        StringBuilder var2 = new StringBuilder(var1);

        for(int var3 = 0; var3 < var1; ++var3) {
            var2.append((char)(var0.get(var3) ? '1' : '0'));
        }

        return var2.toString();
    }

    public static byte[] bitSet2byte(BitSet var0) {
        int var1 = var0.length() + 62 >> 6 << 6;
        byte[] var2 = new byte[var1 >> 3];

        for(int var3 = 0; var3 < var1; ++var3) {
            if (var0.get(var3 + 1)) {
                var2[var3 >> 3] = (byte)(var2[var3 >> 3] | 128 >> var3 % 8);
            }
        }

        if (var1 > 64) {
            var2[0] = (byte)(var2[0] | 128);
        }

        if (var1 > 128) {
            var2[8] = (byte)(var2[8] | 128);
        }

        return var2;
    }

    public static byte[] bitSet2byte(BitSet var0, int var1) {
        int var2 = var1 * 8;
        byte[] var3 = new byte[var1];

        for(int var4 = 0; var4 < var2; ++var4) {
            if (var0.get(var4 + 1)) {
                var3[var4 >> 3] = (byte)(var3[var4 >> 3] | 128 >> var4 % 8);
            }
        }

        if (var2 > 64) {
            var3[0] = (byte)(var3[0] | 128);
        }

        if (var2 > 128) {
            var3[8] = (byte)(var3[8] | 128);
        }

        return var3;
    }

    public static int bitSet2Int(BitSet var0) {
        int var1 = 0;
        int var2 = var0.length() - 1;
        if (var2 > 0) {
            int var3 = (int)Math.pow(2.0D, (double)var2);

            for(int var4 = 0; var4 <= var2; ++var4) {
                if (var0.get(var4)) {
                    var1 += var3;
                }

                var3 >>= 1;
            }
        }

        return var1;
    }

    public static BitSet int2BitSet(int var0) {
        return int2BitSet(var0, 0);
    }

    public static BitSet int2BitSet(int var0, int var1) {
        BitSet var2 = new BitSet();
        String var3 = Integer.toHexString(var0);
        hex2BitSet(var2, var3.getBytes(), var1);
        return var2;
    }

    public static BitSet byte2BitSet(byte[] var0, int var1, boolean var2) {
        int var3 = var2 ? ((var0[var1] & 128) == 128 ? 128 : 64) : 64;
        BitSet var4 = new BitSet(var3);

        for(int var5 = 0; var5 < var3; ++var5) {
            if ((var0[var1 + (var5 >> 3)] & 128 >> var5 % 8) > 0) {
                var4.set(var5 + 1);
            }
        }

        return var4;
    }

    public static BitSet byte2BitSet(byte[] var0, int var1, int var2) {
        int var3 = var2 > 64 ? ((var0[var1] & 128) == 128 ? 128 : 64) : var2;
        if (var2 > 128 && var0.length > var1 + 8 && (var0[var1 + 8] & 128) == 128) {
            var3 = 192;
        }

        BitSet var4 = new BitSet(var3);

        for(int var5 = 0; var5 < var3; ++var5) {
            if ((var0[var1 + (var5 >> 3)] & 128 >> var5 % 8) > 0) {
                var4.set(var5 + 1);
            }
        }

        return var4;
    }

    public static BitSet byte2BitSet(BitSet var0, byte[] var1, int var2) {
        int var3 = var1.length << 3;

        for(int var4 = 0; var4 < var3; ++var4) {
            if ((var1[var4 >> 3] & 128 >> var4 % 8) > 0) {
                var0.set(var2 + var4 + 1);
            }
        }

        return var0;
    }

    public static BitSet hex2BitSet(byte[] var0, int var1, boolean var2) {
        int var3 = var2 ? ((Character.digit((char)var0[var1], 16) & 8) == 8 ? 128 : 64) : 64;
        BitSet var4 = new BitSet(var3);

        for(int var5 = 0; var5 < var3; ++var5) {
            int var6 = Character.digit((char)var0[var1 + (var5 >> 2)], 16);
            if ((var6 & 8 >> var5 % 4) > 0) {
                var4.set(var5 + 1);
            }
        }

        return var4;
    }

    public static BitSet hex2BitSet(byte[] var0, int var1, int var2) {
        int var3 = var2 > 64 ? ((Character.digit((char)var0[var1], 16) & 8) == 8 ? 128 : 64) : var2;
        BitSet var4 = new BitSet(var3);

        for(int var5 = 0; var5 < var3; ++var5) {
            int var6 = Character.digit((char)var0[var1 + (var5 >> 2)], 16);
            if ((var6 & 8 >> var5 % 4) > 0) {
                var4.set(var5 + 1);
                if (var5 == 65 && var2 > 128) {
                    var3 = 192;
                }
            }
        }

        return var4;
    }

    public static BitSet hex2BitSet(BitSet var0, byte[] var1, int var2) {
        int var3 = var1.length << 2;

        for(int var4 = 0; var4 < var3; ++var4) {
            int var5 = Character.digit((char)var1[var4 >> 2], 16);
            if ((var5 & 8 >> var4 % 4) > 0) {
                var0.set(var2 + var4 + 1);
            }
        }

        return var0;
    }

    public static byte[] hex2byte(byte[] var0, int var1, int var2) {
        byte[] var3 = new byte[var2];

        for(int var4 = 0; var4 < var2 * 2; ++var4) {
            int var5 = var4 % 2 == 1 ? 0 : 4;
            var3[var4 >> 1] = (byte)(var3[var4 >> 1] | Character.digit((char)var0[var1 + var4], 16) << var5);
        }

        return var3;
    }

    public static byte[] hex2byte(String var0) {
        return var0.length() % 2 == 0 ? hex2byte(var0.getBytes(), 0, var0.length() >> 1) : hex2byte("0" + var0);
    }

    public static String formatDouble(double var0, int var2) {
        String var3 = Long.toString((long)var0);
        String var4 = Integer.toString((int)(Math.round(var0 * 100.0D) % 100L));

        try {
            if (var2 > 3) {
                var3 = padleft(var3, var2 - 3, ' ');
            }

            var4 = zeropad(var4, 2);
        } catch (Exception var6) {
        }

        return var3 + "." + var4;
    }

    public static String formatAmount(long var0, int var2) {
        String var3 = Long.toString(var0);
        if (var0 < 100L) {
            var3 = zeropad(var3, 3);
        }

        StringBuilder var4 = new StringBuilder(padleft(var3, var2 - 1, ' '));
        var4.insert(var2 - 3, '.');
        return var4.toString();
    }

    public static String normalize(String var0, boolean var1) {
        StringBuilder var2 = new StringBuilder();
        int var3 = var0 != null ? var0.length() : 0;

        for(int var4 = 0; var4 < var3; ++var4) {
            char var5 = var0.charAt(var4);
            switch(var5) {
                case '\n':
                case '\r':
                    if (var1) {
                        var2.append("&#");
                        var2.append(Integer.toString(var5 & 255));
                        var2.append(';');
                        break;
                    }
                default:
                    if (var5 < ' ') {
                        var2.append("&#");
                        var2.append(Integer.toString(var5 & 255));
                        var2.append(';');
                    } else if (var5 > '\uff00') {
                        var2.append((char)(var5 & 255));
                    } else {
                        var2.append(var5);
                    }
                    break;
                case '"':
                    var2.append("&quot;");
                    break;
                case '&':
                    var2.append("&amp;");
                    break;
                case '<':
                    var2.append("&lt;");
                    break;
                case '>':
                    var2.append("&gt;");
            }
        }

        return var2.toString();
    }

    public static String normalize(String var0) {
        return normalize(var0, true);
    }

    public static String protect(String var0) {
        StringBuilder var1 = new StringBuilder();
        int var2 = var0.length();
        int var3 = var2 > 6 ? 6 : 0;
        int var4 = -1;
        if (var3 > 0) {
            var4 = var0.indexOf(61) - 4;
            if (var4 < 0) {
                var4 = var0.indexOf(94) - 4;
            }

            if (var4 < 0 && var0.indexOf(94) < 0) {
                var4 = var0.indexOf(68) - 4;
            }

            if (var4 < 0) {
                var4 = var2 - 4;
            }
        }

        int var5;
        for(var5 = 0; var5 < var2; ++var5) {
            if (var0.charAt(var5) != '=' && (var0.charAt(var5) != 'D' || var0.indexOf(94) >= 0)) {
                if (var0.charAt(var5) == '^') {
                    var4 = 0;
                    var3 = var2 - var5;
                } else if (var5 == var4) {
                    var3 = 4;
                }
            } else {
                var3 = 1;
            }

            var1.append(var3-- > 0 ? var0.charAt(var5) : '_');
        }

        var0 = var1.toString();

        try {
            var5 = var0.replaceAll("[^\\^]", "").length();
            if (var5 == 2) {
                var0 = var0.substring(0, var0.lastIndexOf("^") + 1);
                var0 = padright(var0, var2, '_');
            }
        } catch (Exception var6) {
        }

        return var0;
    }

    public static int[] toIntArray(String var0) {
        StringTokenizer var1 = new StringTokenizer(var0);
        int[] var2 = new int[var1.countTokens()];

        for(int var3 = 0; var1.hasMoreTokens(); ++var3) {
            var2[var3] = Integer.parseInt(var1.nextToken());
        }

        return var2;
    }

    public static String[] toStringArray(String var0) {
        StringTokenizer var1 = new StringTokenizer(var0);
        String[] var2 = new String[var1.countTokens()];

        for(int var3 = 0; var1.hasMoreTokens(); ++var3) {
            var2[var3] = var1.nextToken();
        }

        return var2;
    }

    public static byte[] xor(byte[] var0, byte[] var1) {
        Object var2 = null;
        byte[] var4;
        if (var1.length > var0.length) {
            var4 = new byte[var0.length];
        } else {
            var4 = new byte[var1.length];
        }

        for(int var3 = 0; var3 < var4.length; ++var3) {
            var4[var3] = (byte)(var0[var3] ^ var1[var3]);
        }

        return var4;
    }

    public static String hexor(String var0, String var1) {
        byte[] var2 = xor(hex2byte(var0), hex2byte(var1));
        return hexString(var2);
    }

    public static byte[] trim(byte[] var0, int var1) {
        byte[] var2 = new byte[var1];
        System.arraycopy(var0, 0, var2, 0, var1);
        return var2;
    }

    public static byte[] concat(byte[] var0, byte[] var1) {
        byte[] var2 = new byte[var0.length + var1.length];
        System.arraycopy(var0, 0, var2, 0, var0.length);
        System.arraycopy(var1, 0, var2, var0.length, var1.length);
        return var2;
    }

    public static byte[] concat(byte[] var0, int var1, int var2, byte[] var3, int var4, int var5) {
        byte[] var6 = new byte[var2 + var5];
        System.arraycopy(var0, var1, var6, 0, var2);
        System.arraycopy(var3, var4, var6, var2, var5);
        return var6;
    }

    public static void sleep(long var0) {
        try {
            Thread.sleep(var0);
        } catch (InterruptedException var3) {
        }

    }

    public static String zeroUnPad(String var0) {
        return unPadLeft(var0, '0');
    }

    public static String blankUnPad(String var0) {
        return unPadRight(var0, ' ');
    }

    public static String unPadRight(String var0, char var1) {
        int var2 = var0.length();
        if (var2 == 0) {
            return var0;
        } else {
            while(0 < var2 && var0.charAt(var2 - 1) == var1) {
                --var2;
            }

            return 0 < var2 ? var0.substring(0, var2) : var0.substring(0, 1);
        }
    }

    public static String unPadLeft(String var0, char var1) {
        int var2 = 0;
        int var3 = var0.length();
        if (var3 == 0) {
            return var0;
        } else {
            while(var2 < var3 && var0.charAt(var2) == var1) {
                ++var2;
            }

            return var2 < var3 ? var0.substring(var2, var3) : var0.substring(var2 - 1, var3);
        }
    }

    public static boolean isZero(String var0) {
        int var1 = 0;

        int var2;
        for(var2 = var0.length(); var1 < var2 && var0.charAt(var1) == '0'; ++var1) {
        }

        return var1 >= var2;
    }

    public static boolean isBlank(String var0) {
        return var0.trim().length() == 0;
    }

    public static boolean isAlphaNumeric(String var0) {
        int var1 = 0;

        int var2;
        for(var2 = var0.length(); var1 < var2 && (Character.isLetterOrDigit(var0.charAt(var1)) || var0.charAt(var1) == ' ' || var0.charAt(var1) == '.' || var0.charAt(var1) == '-' || var0.charAt(var1) == '_') || var0.charAt(var1) == '?'; ++var1) {
        }

        return var1 >= var2;
    }

    public static boolean isNumeric(String var0, int var1) {
        int var2 = 0;

        int var3;
        for(var3 = var0.length(); var2 < var3 && Character.digit(var0.charAt(var2), var1) > -1; ++var2) {
        }

        return var2 >= var3 && var3 > 0;
    }

    public static byte[] bitSet2extendedByte(BitSet var0) {
        short var1 = 128;
        byte[] var2 = new byte[var1 >> 3];

        for(int var3 = 0; var3 < var1; ++var3) {
            if (var0.get(var3 + 1)) {
                var2[var3 >> 3] = (byte)(var2[var3 >> 3] | 128 >> var3 % 8);
            }
        }

        var2[0] = (byte)(var2[0] | 128);
        return var2;
    }

    public static int parseInt(String var0, int var1) throws NumberFormatException {
        int var2 = var0.length();
        if (var2 > 9) {
            throw new NumberFormatException("Number can have maximum 9 digits");
        } else {
            boolean var3 = false;
            byte var4 = 0;
            int var7 = var4 + 1;
            int var5 = Character.digit(var0.charAt(var4), var1);
            if (var5 == -1) {
                throw new NumberFormatException("String contains non-digit");
            } else {
                int var6;
                for(var6 = var5; var7 < var2; var6 += var5) {
                    var6 *= var1;
                    var5 = Character.digit(var0.charAt(var7++), var1);
                    if (var5 == -1) {
                        throw new NumberFormatException("String contains non-digit");
                    }
                }

                return var6;
            }
        }
    }

    public static int parseInt(String var0) throws NumberFormatException {
        return parseInt((String)var0, 10);
    }

    public static int parseInt(char[] var0, int var1) throws NumberFormatException {
        int var2 = var0.length;
        if (var2 > 9) {
            throw new NumberFormatException("Number can have maximum 9 digits");
        } else {
            boolean var3 = false;
            byte var4 = 0;
            int var7 = var4 + 1;
            int var5 = Character.digit(var0[var4], var1);
            if (var5 == -1) {
                throw new NumberFormatException("Char array contains non-digit");
            } else {
                int var6;
                for(var6 = var5; var7 < var2; var6 += var5) {
                    var6 *= var1;
                    var5 = Character.digit(var0[var7++], var1);
                    if (var5 == -1) {
                        throw new NumberFormatException("Char array contains non-digit");
                    }
                }

                return var6;
            }
        }
    }

    public static int parseInt(char[] var0) throws NumberFormatException {
        return parseInt((char[])var0, 10);
    }

    public static int parseInt(byte[] var0, int var1) throws NumberFormatException {
        int var2 = var0.length;
        if (var2 > 9) {
            throw new NumberFormatException("Number can have maximum 9 digits");
        } else {
            boolean var3 = false;
            byte var4 = 0;
            int var7 = var4 + 1;
            int var5 = Character.digit((char)var0[var4], var1);
            if (var5 == -1) {
                throw new NumberFormatException("Byte array contains non-digit");
            } else {
                int var6;
                for(var6 = var5; var7 < var2; var6 += var5) {
                    var6 *= var1;
                    var5 = Character.digit((char)var0[var7++], var1);
                    if (var5 == -1) {
                        throw new NumberFormatException("Byte array contains non-digit");
                    }
                }

                return var6;
            }
        }
    }

    public static int parseInt(byte[] var0) throws NumberFormatException {
        return parseInt((byte[])var0, 10);
    }

    private static String hexOffset(int var0) {
        var0 = var0 >> 4 << 4;
        int var1 = var0 > 65535 ? 8 : 4;

        try {
            return zeropad(Integer.toString(var0, 16), var1);
        } catch (Exception var3) {
            return var3.getMessage();
        }
    }

    public static String hexdump(byte[] var0) {
        return hexdump(var0, 0, var0.length);
    }

    public static String hexdump(byte[] var0, int var1, int var2) {
        StringBuilder var3 = new StringBuilder();
        StringBuilder var4 = new StringBuilder();
        StringBuilder var5 = new StringBuilder();
        String var6 = "  ";
        String var7 = System.getProperty("line.separator");

        for(int var8 = var1; var8 < var2; ++var8) {
            var4.append(hexStrings[var0[var8] & 255]);
            var4.append(' ');
            char var9 = (char)var0[var8];
            var5.append(var9 >= ' ' && var9 < 127 ? var9 : '.');
            int var10 = var8 % 16;
            switch(var10) {
                case 7:
                    var4.append(' ');
                    break;
                case 15:
                    var3.append(hexOffset(var8));
                    var3.append(var6);
                    var3.append(var4.toString());
                    var3.append(' ');
                    var3.append(var5.toString());
                    var3.append(var7);
                    var4 = new StringBuilder();
                    var5 = new StringBuilder();
            }
        }

        if (var4.length() > 0) {
            while(var4.length() < 49) {
                var4.append(' ');
            }

            var3.append(hexOffset(var2));
            var3.append(var6);
            var3.append(var4.toString());
            var3.append(' ');
            var3.append(var5.toString());
            var3.append(var7);
        }

        return var3.toString();
    }

    public static String strpadf(String var0, int var1) {
        StringBuilder var2 = new StringBuilder(var0);

        while(var2.length() < var1) {
            var2.append('F');
        }

        return var2.toString();
    }

    public static String trimf(String var0) {
        if (var0 != null) {
            int var1 = var0.length();
            if (var1 > 0) {
                do {
                    --var1;
                } while(var1 >= 0 && var0.charAt(var1) == 'F');

                var0 = var1 == 0 ? "" : var0.substring(0, var1 + 1);
            }
        }

        return var0;
    }

    public static String takeLastN(String var0, int var1) {
        if (var0.length() > var1) {
            return var0.substring(var0.length() - var1);
        } else {
            return var0.length() < var1 ? zeropad(var0, var1) : var0;
        }
    }

    public static String takeFirstN(String var0, int var1) {
        if (var0.length() > var1) {
            return var0.substring(0, var1);
        } else {
            return var0.length() < var1 ? zeropad(var0, var1) : var0;
        }
    }

    public static String millisToString(long var0) {
        StringBuilder var2 = new StringBuilder();
        int var3 = (int)(var0 % 1000L);
        var0 /= 1000L;
        int var4 = (int)(var0 / 86400L);
        var0 -= (long)(var4 * 86400);
        int var5 = (int)(var0 / 3600L);
        var0 -= (long)(var5 * 3600);
        int var6 = (int)(var0 / 60L);
        var0 -= (long)(var6 * 60);
        int var7 = (int)var0;
        if (var4 > 0) {
            var2.append(Long.toString((long)var4));
            var2.append("d ");
        }

        var2.append(zeropad((long)var5, 2));
        var2.append(':');
        var2.append(zeropad((long)var6, 2));
        var2.append(':');
        var2.append(zeropad((long)var7, 2));
        var2.append('.');
        var2.append(zeropad((long)var3, 3));
        return var2.toString();
    }

    public static String formatAmountConversionRate(double var0) {
        if (var0 == 0.0D) {
            return null;
        } else {
            BigDecimal var2 = new BigDecimal(var0);
            int var3 = 7 - var2.precision() + var2.scale();
            String var4 = var2.movePointRight(var2.scale()).toString();
            if (var3 > 9) {
                var4 = zeropad(var4, var4.length() + var3 - 9);
            }

            String var5 = zeropadRight(var4, 7);
            return Math.min(9, var3) + takeFirstN(var5, 7);
        }
    }

    public static double parseAmountConversionRate(String var0) {
        if (var0 != null && var0.length() == 8) {
            BigDecimal var1 = new BigDecimal(var0);
            int var2 = var1.movePointLeft(7).intValue();
            var1 = new BigDecimal(var0.substring(1));
            return var1.movePointLeft(var2).doubleValue();
        } else {
            throw new IllegalArgumentException("Invalid amount converion rate argument: '" + var0 + "'");
        }
    }

    public static String commaEncode(String[] var0) {
        StringBuilder var1 = new StringBuilder();
        String[] var2 = var0;
        int var3 = var0.length;

        for(int var4 = 0; var4 < var3; ++var4) {
            String var5 = var2[var4];
            if (var1.length() > 0) {
                var1.append(',');
            }

            if (var5 != null) {
                int var6 = 0;

                while(var6 < var5.length()) {
                    char var7 = var5.charAt(var6);
                    switch(var7) {
                        case ',':
                        case '\\':
                            var1.append('\\');
                        default:
                            var1.append(var7);
                            ++var6;
                    }
                }
            }
        }

        return var1.toString();
    }

    public static String[] commaDecode(String var0) {
        ArrayList var1 = new ArrayList();
        StringBuilder var2 = new StringBuilder();
        boolean var3 = false;

        for(int var4 = 0; var4 < var0.length(); ++var4) {
            char var5 = var0.charAt(var4);
            if (!var3) {
                switch(var5) {
                    case ',':
                        var1.add(var2.toString());
                        var2 = new StringBuilder();
                        continue;
                    case '\\':
                        var3 = true;
                        continue;
                }
            }

            var2.append(var5);
            var3 = false;
        }

        if (var2.length() > 0) {
            var1.add(var2.toString());
        }

        return (String[])var1.toArray(new String[var1.size()]);
    }

    public static int bytesToInt(byte[] var0, int var1, int var2, boolean var3) {
        if (var2 > 4) {
            throw new IllegalArgumentException("len should be under 4!");
        } else {
            int var4 = 0;

            for(int var5 = 0; var5 < var2; ++var5) {
                int var6 = var5;
                if (var3) {
                    var6 = var2 - var5 - 1;
                }

                var4 += (var0[var6] & 255) << var5 * 8;
            }

            return var4;
        }
    }

    public static byte[] intToBytes(int var0, boolean var1) {
        boolean var2 = false;
        byte var3;
        if ((var0 & -256) == 0) {
            var3 = 1;
        } else if ((var0 & -65536) == 0) {
            var3 = 2;
        } else if ((var0 & -16777216) == 0) {
            var3 = 3;
        } else {
            var3 = 4;
        }

        return intToBytes(var0, var3, var1);
    }

    public static byte[] intToBytes(int var0, int var1, boolean var2) {
        byte[] var3 = new byte[var1];
        Arrays.fill(var3, (byte)0);

        for(int var4 = 0; var4 < var1; ++var4) {
            int var5 = var4;
            if (var2) {
                var5 = var1 - var4 - 1;
            }

            var3[var5] = (byte)(var0 >> var4 * 8 & 255);
        }

        return var3;
    }

    /** @deprecated */
    @Deprecated
    public static byte[] packIntToBytes(int var0, int var1, boolean var2) {
        return intToBytes(var0, var1, var2);
    }

    /** @deprecated */
    @Deprecated
    public static int unPackIntFromBytes(byte[] var0, int var1, int var2, boolean var3) {
        return bytesToInt(var0, var1, var2, var3);
    }

    public static byte[] intToBCD(int var0, int var1, boolean var2) {
        String var3 = null;
        if (var2) {
            var3 = padleft(String.valueOf(var0), var1, '0');
        } else {
            var3 = padright(String.valueOf(var0), var1, '0');
        }

        return str2bcd(var3, var2);
    }

    public static int bcdToInt(byte[] var0, int var1, int var2, boolean var3) {
        String var4 = bcd2str(var0, var1, var2, var3);
        return Integer.valueOf(var4);
    }

    public static long bcdToLong(byte[] var0, int var1, int var2, boolean var3) {
        String var4 = bcd2str(var0, var1, var2, var3);
        return Long.parseLong(var4);
    }



    public static int toBERTLVTag(int var0) {
        if (var0 < 0) {
            throw new IllegalArgumentException("illegal input:" + var0);
        } else {
            int var1 = 0;
            int var2 = 0;

            for(int var3 = 0; var3 < 3; ++var3) {
                int var4 = var0 >> var3 * 8 & 255;
                if (var4 != 0 && var4 != 255) {
                    var1 += var4 << var2 * 8;
                    ++var2;
                }
            }

            if (var1 > 16777215) {
                throw new IllegalArgumentException("only support 3 bytes tag" + Dump.getHexDump(intToBytes(var1, 4, true)));
            } else if (var1 > 65535) {
                if ((var1 & 2064384) == 2064384 && (var1 & 128) != 128) {
                    return var1;
                } else {
                    throw new IllegalArgumentException("illegal tag format:" + Dump.getHexDump(intToBytes(var1, 3, true)));
                }
            } else if (var1 > 255) {
                if ((var1 & 7936) == 7936 && (var1 & 128) != 128) {
                    return var1;
                } else {
                    throw new IllegalArgumentException("illegal tag format:" + Dump.getHexDump(intToBytes(var1, 2, true)));
                }
            } else if ((var1 & 31) == 31) {
                throw new IllegalArgumentException("illegal tag format:" + Dump.getHexDump(intToBytes(var1, 1, true)));
            } else {
                return var1;
            }
        }
    }

    public static boolean isBitSet(byte var0, int var1) {
        if (var1 >= 1 && var1 <= 8) {
            return (var0 >> var1 - 1 & 1) == 1;
        } else {
            throw new IllegalArgumentException("parameter 'bitPos' must be between 1 and 8. bitPos=" + var1);
        }
    }

    static {
        for(int var0 = 0; var0 < 256; ++var0) {
            StringBuilder var1 = new StringBuilder(2);
            char var2 = Character.forDigit((byte)var0 >> 4 & 15, 16);
            var1.append(Character.toUpperCase(var2));
            var2 = Character.forDigit((byte)var0 & 15, 16);
            var1.append(Character.toUpperCase(var2));
            hexStrings[var0] = var1.toString();
        }

        EBCDIC2ASCII = new byte[]{0, 1, 2, 3, -100, 9, -122, 127, -105, -115, -114, 11, 12, 13, 14, 15, 16, 17, 18, 19, -99, 10, 8, -121, 24, 25, -110, -113, 28, 29, 30, 31, -128, -127, -126, -125, -124, -123, 23, 27, -120, -119, -118, -117, -116, 5, 6, 7, -112, -111, 22, -109, -108, -107, -106, 4, -104, -103, -102, -101, 20, 21, -98, 26, 32, -96, -30, -28, -32, -31, -29, -27, -25, -15, -94, 46, 60, 40, 43, 124, 38, -23, -22, -21, -24, -19, -18, -17, -20, -33, 33, 36, 42, 41, 59, 94, 45, 47, -62, -60, -64, -63, -61, -59, -57, -47, -90, 44, 37, 95, 62, 63, -8, -55, -54, -53, -56, -51, -50, -49, -52, 96, 58, 35, 64, 39, 61, 34, -40, 97, 98, 99, 100, 101, 102, 103, 104, 105, -85, -69, -16, -3, -2, -79, -80, 106, 107, 108, 109, 110, 111, 112, 113, 114, -86, -70, -26, -72, -58, -92, -75, 126, 115, 116, 117, 118, 119, 120, 121, 122, -95, -65, -48, 91, -34, -82, -84, -93, -91, -73, -87, -89, -74, -68, -67, -66, -35, -88, -81, 93, -76, -41, 123, 65, 66, 67, 68, 69, 70, 71, 72, 73, -83, -12, -10, -14, -13, -11, 125, 74, 75, 76, 77, 78, 79, 80, 81, 82, -71, -5, -4, -7, -6, -1, 92, -9, 83, 84, 85, 86, 87, 88, 89, 90, -78, -44, -42, -46, -45, -43, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, -77, -37, -36, -39, -38, -97};
        ASCII2EBCDIC = new byte[]{0, 1, 2, 3, 55, 45, 46, 47, 22, 5, 21, 11, 12, 13, 14, 15, 16, 17, 18, 19, 60, 61, 50, 38, 24, 25, 63, 39, 28, 29, 30, 31, 64, 90, 127, 123, 91, 108, 80, 125, 77, 93, 92, 78, 107, 96, 75, 97, -16, -15, -14, -13, -12, -11, -10, -9, -8, -7, 122, 94, 76, 126, 110, 111, 124, -63, -62, -61, -60, -59, -58, -57, -56, -55, -47, -46, -45, -44, -43, -42, -41, -40, -39, -30, -29, -28, -27, -26, -25, -24, -23, -83, -32, -67, 95, 109, 121, -127, -126, -125, -124, -123, -122, -121, -120, -119, -111, -110, -109, -108, -107, -106, -105, -104, -103, -94, -93, -92, -91, -90, -89, -88, -87, -64, 79, -48, -95, 7, 32, 33, 34, 35, 36, 37, 6, 23, 40, 41, 42, 43, 44, 9, 10, 27, 48, 49, 26, 51, 52, 53, 54, 8, 56, 57, 58, 59, 4, 20, 62, -1, 65, -86, 74, -79, -97, -78, 106, -75, -69, -76, -102, -118, -80, -54, -81, -68, -112, -113, -22, -6, -66, -96, -74, -77, -99, -38, -101, -117, -73, -72, -71, -85, 100, 101, 98, 102, 99, 103, -98, 104, 116, 113, 114, 115, 120, 117, 118, 119, -84, 105, -19, -18, -21, -17, -20, -65, -128, -3, -2, -5, -4, -70, -82, 89, 68, 69, 66, 70, 67, 71, -100, 72, 84, 81, 82, 83, 88, 85, 86, 87, -116, 73, -51, -50, -53, -49, -52, -31, 112, -35, -34, -37, -36, -115, -114, -33};
    }
}
