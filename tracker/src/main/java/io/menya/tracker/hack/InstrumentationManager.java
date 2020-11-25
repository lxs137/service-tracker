package io.menya.tracker.hack;

import java.util.HashMap;
import java.util.Map;

import io.menya.tracker.hack.injector.InjectAgentAction;
import io.menya.tracker.utils.Log;
import io.menya.tracker.utils.ProcessUtils;

public class InstrumentationManager {
    private static class InjectionRecord {
        public int pid;
        public int agentListenPort;
        public Long injectTime;
        @Override
        public String toString() {
            return "InjectionRecord{" +
                    "pid=" + pid +
                    ", agentListenPort=" + agentListenPort +
                    ", injectTime=" + injectTime +
                    '}';
        }
    }
    private final static String TAG = "InstrumentationManager";
    private static int AGENT_LISTEN_PORT_START = 65200;
    private final Map<Integer, InjectionRecord> injectionRecordMap = new HashMap<>();

    public InstrumentationManager() {

    }

    public void injectLibs(int pid) {
        InjectionRecord record = injectionRecordMap.get(pid);
        if (record != null) {
            Long pidUptime = ProcessUtils.upTimestamp(pid);
            if (pidUptime != null && record.injectTime > pidUptime) {
                Log.info(TAG, String.format("target process has been injected: %d", pid));
                return;
            }
        } else {
            record = new InjectionRecord();
            record.pid = pid;
            record.agentListenPort = AGENT_LISTEN_PORT_START++;
        }
        HackAction action = new InjectAgentAction(pid, record.agentListenPort);
        try {
            action.requestAndWaitReply();
            record.injectTime = System.currentTimeMillis();
            injectionRecordMap.put(pid, record);
        } catch (Exception e) {
            Log.error(TAG, e, "HackAction failed");
        }
    }
}
