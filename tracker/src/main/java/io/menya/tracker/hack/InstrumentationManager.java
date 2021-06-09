package io.menya.tracker.hack;

import java.util.HashMap;
import java.util.Map;

import io.menya.tracker.hack.injector.InjectAgentAction;
import io.menya.tracker.utils.Config;
import io.menya.tracker.utils.Log;
import io.menya.tracker.utils.ProcessUtils;
import io.menya.tracker.utils.Utils;

public class InstrumentationManager {
    private static class InjectionRecord {
        public int pid;
        public Long injectTime;
        @Override
        public String toString() {
            return "InjectionRecord{" +
                    "pid=" + pid +
                    ", injectTime=" + injectTime +
                    '}';
        }
    }
    private final static String TAG = "InstrumentationManager";
    private final Map<Integer, InjectionRecord> injectionRecordMap = new HashMap<>();

    public InstrumentationManager() {

    }

    public void injectAgent(final int pid) {
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
        }
        HackAction action = new InjectAgentAction(pid);
        try {
            action.requestAndWaitReply();
        } catch (Exception e) {
            Log.error(TAG, e, "HackAction failed");
            return;
        }

        // wait until agent server ready
        final String socketPath = Config.getAgentListenSocket(pid);
        if (!Utils.waitUntil(10, new Utils.ConditionChecker() {
            @Override
            public boolean check() {
                Log.info(TAG, "waiting agent(%d) server ready", pid);
                return ProcessUtils.testUnixDomainSocket(socketPath);
            }
        })) {
            Log.info(TAG, "waiting agent(%d) server timeout", pid);
            return;
        }
        record.injectTime = System.currentTimeMillis();
        injectionRecordMap.put(pid, record);
    }
}
