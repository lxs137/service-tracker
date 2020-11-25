package io.menya.tracker;

import java.util.Arrays;
import java.util.List;

import io.menya.tracker.hack.InstrumentationManager;
import io.menya.tracker.reflect.ServiceManager;
import io.menya.tracker.reflect.ServiceInfo;
import io.menya.tracker.utils.Log;
import io.menya.tracker.utils.OptionsHelper;

public class Main {
    public static void main(String[] args) {
        OptionsHelper.init(args);

        switch (OptionsHelper.getAction()) {
            case ParseServiceInterface:
                ServiceManager.init();
                List<ServiceInfo> metadataList = ServiceManager.parseAllServiceInfo();
                Log.info("Main", Arrays.toString(metadataList.toArray()).replace(',', '\n'));
                break;
            case InjectAgent:
                new InstrumentationManager().injectLibs(OptionsHelper.getTargetPid());
                break;
        }
    }


}