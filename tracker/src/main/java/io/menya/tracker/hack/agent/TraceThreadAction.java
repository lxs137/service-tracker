package io.menya.tracker.hack.agent;

import android.net.LocalSocket;

import org.json.JSONException;
import org.json.JSONObject;

import java.nio.charset.StandardCharsets;

public class TraceThreadAction extends AgentHackAction {
    int pid, tid;
    TraceThreadAction(int pid, int tid) {
        this.pid = pid;
        this.tid = tid;
    }

    @Override
    public byte[] encodeRequest() throws JSONException {
        JSONObject action = new JSONObject();
        action.put("command", Command.TraceThread.ordinal());
        action.put("pid", this.pid);
        action.put("tid", this.tid);
        return action.toString().getBytes(StandardCharsets.UTF_8);
    }

    @Override
    public JSONObject requestAndWaitReply() throws Exception {
        LocalSocket socket = this.getSocketForPID(this.pid);
        if (socket == null) {
            return null;
        }
        return this.requestAndWaitReplyFromSocket(socket);
    }
}
