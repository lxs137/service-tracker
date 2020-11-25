package io.menya.tracker.hack.injector;

import org.json.JSONException;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

import io.menya.tracker.hack.HackAction;
import io.menya.tracker.utils.Config;
import io.menya.tracker.utils.Log;

public abstract class InjectorHackAction extends HackAction {
    private static final String TAG = "InjectorHackAction";
    protected static Socket socketFor32bit;
    protected final static Lock lockFor32bitInjector = new ReentrantLock();
    protected static Socket socketFor64bit;
    protected final static Lock lockFor64bitInjector = new ReentrantLock();
    static {
        try {
            socketFor32bit = new Socket("127.0.0.1", Config.PORT_INJECTOR_32BIT);
//            socketFor64bit = new Socket("127.0.0.1", Config.PORT_INJECTOR_64BIT);
        } catch (IOException e) {
            Log.fault(TAG, e, "can not establish connection to injector");
        }
    }

    protected void requestAndWaitReplyFromSocket(Socket socket) throws Exception {
        OutputStream output = socket.getOutputStream();
        byte[] request = this.encodeRequest();
        output.write(this.encodeRequest());

        InputStream input = socket.getInputStream();
        BufferedReader reader = new BufferedReader(new InputStreamReader(input));
        String line;
        while ((line = reader.readLine()) != null) {
            Log.info(TAG, "RPC got reply: %s", line);
            this.decodeReply(line);
        }
    }
}
