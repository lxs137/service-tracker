package io.menya.tracker.hack.agent;

import android.net.LocalSocket;
import android.net.LocalSocketAddress;

import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.Socket;
import java.util.HashMap;
import java.util.Map;

import io.menya.tracker.hack.HackAction;
import io.menya.tracker.utils.Config;
import io.menya.tracker.utils.Log;

public abstract class AgentHackAction extends HackAction {
    private static final String TAG = "AgentHackAction";
    private static final Map<Integer, LocalSocket> socketMap = new HashMap<>();

    protected synchronized LocalSocket getSocketForPID(int pid) {
        LocalSocket socket = socketMap.get(pid);
        if (socket != null) {
            return socket;
        }
        String socketPath = Config.getAgentListenSocket(pid);
        socket = new LocalSocket();
        try {
            socket.connect(new LocalSocketAddress(socketPath));
        } catch (IOException e) {
            Log.error(TAG, e, "can not connect to %s", socketPath);
            return null;
        }
        socketMap.put(pid, socket);
        return socket;
    }

    protected JSONObject requestAndWaitReplyFromSocket(LocalSocket socket) throws Exception {
        OutputStream output = socket.getOutputStream();
        output.write(this.encodeRequest());

        InputStream input = socket.getInputStream();
        BufferedReader reader = new BufferedReader(new InputStreamReader(input));
        String line;
        while ((line = reader.readLine()) != null) {
            Log.info(TAG, "RPC got reply: %s", line);
            return new JSONObject(line);
        }
        return null;
    }
}
