package io.menya.tracker.hack.injector;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.UnsupportedEncodingException;
import java.nio.charset.StandardCharsets;
import java.util.concurrent.locks.Lock;

import io.menya.tracker.utils.Config;
import io.menya.tracker.utils.Log;
import io.menya.tracker.utils.ProcessUtils;

public class InjectAgentAction extends InjectorHackAction {
    boolean is32bit;
    // request
    int pid;
    String agentPath;
    String agentEntry;
    int agentListenPort;

    public InjectAgentAction(int targetPid, int listenPort) {
        this.pid = targetPid;
        this.is32bit = ProcessUtils.is32bit(this.pid);
        this.agentPath = this.is32bit ? Config.AGENT_LIB_PATH_32BIT : Config.AGENT_LIB_PATH_64BIT;
        this.agentEntry = "agent_server";
        this.agentListenPort = listenPort;
    }

    @Override
    public byte[] encodeRequest() throws JSONException {
        JSONObject action = new JSONObject();
        action.put("command", Command.InjectAgent.ordinal());
        action.put("pid", this.pid);
        action.put("agentPath", this.agentPath);
        action.put("agentEntry", this.agentEntry);
        action.put("agentListenPort", this.agentListenPort);
        return action.toString().getBytes(StandardCharsets.UTF_8);
    }

    @Override
    public void decodeReply(String rawReply) throws JSONException {
        JSONObject res = new JSONObject(rawReply);
        this.is32bit = res.optBoolean("is32bit", false);
    }

    @Override
    public void requestAndWaitReply() throws Exception {
        Lock lock = this.is32bit ? lockFor32bitInjector : lockFor64bitInjector;
        try {
            lock.lock();
            Log.info("InjectAgentAction", "RPC begin, target: 127.0.0.1:%d",
                    this.is32bit ? socketFor32bit.getPort() : socketFor64bit.getPort());
            this.requestAndWaitReplyFromSocket(this.is32bit ? socketFor32bit : socketFor64bit);
        } catch (Exception e) {
            throw new Exception(String.format("RPC failed: %s", e.getMessage()), e);
        } finally {
            lock.unlock();
        }
    }
}
