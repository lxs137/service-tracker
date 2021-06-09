package io.menya.tracker.hack.injector;

import org.json.JSONException;
import org.json.JSONObject;

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
    String entryPoint;
    String listenSocketPath;

    public InjectAgentAction(int targetPid) {
        this.pid = targetPid;
        this.is32bit = ProcessUtils.is32bit(this.pid);
        this.agentPath = this.is32bit ? Config.AGENT_LIB_PATH_32BIT : Config.AGENT_LIB_PATH_64BIT;
        this.entryPoint = "agent_server";
        this.listenSocketPath = Config.getAgentListenSocket(targetPid);
    }

    @Override
    public byte[] encodeRequest() throws JSONException {
        JSONObject action = new JSONObject();
        action.put("command", Command.InjectAgent.ordinal());
        action.put("pid", this.pid);
        action.put("libPath", this.agentPath);
        action.put("entryPoint", this.entryPoint);
        action.put("agentArgs", this.listenSocketPath);
        return action.toString().getBytes(StandardCharsets.UTF_8);
    }

    @Override
    public JSONObject requestAndWaitReply() throws Exception {
        Lock lock = this.is32bit ? lockFor32bitInjector : lockFor64bitInjector;
        JSONObject reply = null;
        try {
            lock.lock();
            Log.info("InjectAgentAction", "RPC begin, target: 127.0.0.1:%d",
                    this.is32bit ? socketFor32bit.getPort() : socketFor64bit.getPort());
            reply = this.requestAndWaitReplyFromSocket(this.is32bit ? socketFor32bit : socketFor64bit);
        } catch (Exception e) {
            throw new Exception(String.format("RPC failed: %s", e.getMessage()), e);
        } finally {
            lock.unlock();
            return reply;
        }
    }
}
