package io.menya.tracker;

import java.util.Arrays;
import java.util.List;

import io.menya.tracker.hack.InstrumentationManager;
import io.menya.tracker.reflect.ServiceManagerHelper;
import io.menya.tracker.reflect.ServiceInfo;
import io.menya.tracker.utils.Log;
import io.menya.tracker.utils.OptionsHelper;

public class Main {
    public static void main(String[] args) {
        OptionsHelper.init(args);
        ServiceManagerHelper.init();
        switch (OptionsHelper.getAction()) {
            case ParseServiceInterface:
                List<ServiceInfo> metadataList = ServiceManagerHelper.parseAllServiceInfo();
                Log.info("Main", Arrays.toString(metadataList.toArray()).replace(',', '\n'));
                break;
            case InjectAgent:
                new InstrumentationManager().injectAgent(OptionsHelper.getTargetPid());
                break;
            case Fuzzing:

        }
    }


}