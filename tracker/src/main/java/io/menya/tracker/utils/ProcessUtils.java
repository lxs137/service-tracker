package io.menya.tracker.utils;

import android.net.LocalSocket;
import android.net.LocalSocketAddress;

import org.apache.commons.lang3.RegExUtils;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

public class ProcessUtils {
    public static String exec(String []commands) throws IOException, InterruptedException {
        Process process = Runtime.getRuntime().exec(commands);
        process.waitFor();

        BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()));

        StringBuilder result = new StringBuilder();
        String line;
        while ((line = reader.readLine()) != null) {
            result.append(line).append('\n');
        }
        return result.toString();
    }

    public static boolean is32bit(int pid) {
        try {
            String []commands = {
                    Config.EXE_PATH_SUDO,
                    "-c",
                    String.format("file -L /proc/%d/exe", pid)
            };
            String result = exec(commands);
            return result.contains("32-bit");
        } catch (IOException | InterruptedException e) {
            return false;
        }
    }

    public static Long upTimestamp(int pid) {
        try {
            String []commands = {
                    Config.EXE_PATH_SUDO,
                    "-c",
                    String.format("stat -t /proc/%d/cmdline", pid)
            };
            String result = exec(commands);
            String []cols = result.split(" ");
            if (cols.length == 15) {
                return Long.parseLong(cols[11]) << 3;
            }
            return null;
        } catch (IOException | InterruptedException e) {
            return null;
        }
    }

    public static boolean testUnixDomainSocket(String path) {
        LocalSocket socket = new LocalSocket();
        try {
            socket.connect(new LocalSocketAddress(path));
        } catch (IOException ignore) {
            return false;
        }
        return true;
    }
}
