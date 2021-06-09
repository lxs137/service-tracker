package io.menya.tracker.generator;

public class GenerateException extends Exception {

    public GenerateException(String message, Throwable cause) {
        super(message, cause);
    }

    public GenerateException(Throwable cause) {
        super(cause);
    }

    public GenerateException(String message) {
        super(message);
    }
}
