package io.menya.tracker.utils;

public enum Action {
    ParseServiceInterface("parse-service-interface"),
    InjectAgent("inject-agent"),
    TraceThread("trace-thread"),
    Fuzzing("fuzzing");

    final private String stringVal;
    Action(String val) {
        this.stringVal = val;
    }
    public String val() {
        return stringVal;
    }
    public static Action fromString(String text) {
        for (Action b : Action.values()) {
            if (b.stringVal.equalsIgnoreCase(text)) {
                return b;
            }
        }
        return null;
    }
}
