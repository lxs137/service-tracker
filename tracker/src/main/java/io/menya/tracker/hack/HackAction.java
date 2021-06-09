package io.menya.tracker.hack;

import org.json.JSONException;
import org.json.JSONObject;

public abstract class HackAction {
    public abstract byte[] encodeRequest() throws JSONException;
    public abstract JSONObject requestAndWaitReply() throws Exception;

    public enum Command {
        InjectAgent,
        TraceThread,
    }
}
