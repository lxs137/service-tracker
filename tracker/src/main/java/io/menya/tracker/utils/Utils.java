package io.menya.tracker.utils;

import java.util.Random;

public class Utils {

    public static final Random random = new Random(System.currentTimeMillis());

    public static interface ConditionChecker {
        boolean check();
    }
    public static boolean waitUntil(int timeoutSec, ConditionChecker checker) {
        int waitSec = 0;
        while(waitSec < timeoutSec) {
            if (checker.check()) {
                return true;
            }
            try {
                Thread.sleep(1000);
            } catch (InterruptedException ignore) {}
            waitSec++;
        }
        return false;
    }
}
