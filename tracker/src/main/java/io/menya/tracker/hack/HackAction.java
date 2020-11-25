package io.menya.tracker.hack;

import org.json.JSONException;

public abstract class HackAction {
    public abstract byte[] encodeRequest() throws JSONException;
    public abstract void decodeReply(String rawReply) throws JSONException;
    public abstract void requestAndWaitReply() throws Exception;

    public enum Command {
        InjectAgent,
    }
}
