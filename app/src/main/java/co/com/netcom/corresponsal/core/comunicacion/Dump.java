package co.com.netcom.corresponsal.core.comunicacion;

public class Dump {
    private static final byte[] highDigits;
    private static final byte[] lowDigits;

    public Dump() {
    }

    public static String getHexDump(byte[] var0, int var1, int var2) {
        if (var0 != null && var0.length != 0) {
            if (var1 >= var0.length) {
                return "out of length,totallen:" + var0.length + ",offset:" + var1;
            } else {
                StringBuffer var3 = new StringBuffer();
                int var4 = var0[var1] & 255;
                var3.append((char)highDigits[var4]);
                var3.append((char)lowDigits[var4]);

                for(int var5 = var1 + 1; var5 < var0.length && var5 - var1 < var2; ++var5) {
                    var3.append(' ');
                    var4 = var0[var5] & 255;
                    var3.append((char)highDigits[var4]);
                    var3.append((char)lowDigits[var4]);
                }

                return var3.toString();
            }
        } else {
            return "empty";
        }
    }

    public static String getHexDump(byte[] var0) {
        return getHexDump(var0, 0, var0.length);
    }

    static {
        byte[] var0 = new byte[]{48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 65, 66, 67, 68, 69, 70};
        byte[] var2 = new byte[256];
        byte[] var3 = new byte[256];

        for(int var1 = 0; var1 < 256; ++var1) {
            var2[var1] = var0[var1 >>> 4];
            var3[var1] = var0[var1 & 15];
        }

        highDigits = var2;
        lowDigits = var3;
    }
}
