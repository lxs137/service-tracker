package io.menya.tracker.utils;

import java.util.Arrays;
import java.util.logging.Level;
import java.util.logging.Logger;

public class Log {
    private final static Logger logger = Logger.getLogger("Tracker");
    public static void info(String tag, String format, Object... args) {
        logger.log(Level.INFO, String.format("[%s] %s", tag, String.format(format, args)));
    }
    public static void info(String tag, String msg) {
        logger.log(Level.INFO, String.format("[%s] %s", tag, msg));
    }
    public static void error(String tag, Exception e, String format, Object... args) {
        if (e == null) {
            logger.log(Level.WARNING, String.format("[%s] %s", tag, String.format(format, args)));
        } else {
            logger.log(Level.WARNING, String.format(
                    "[%s] %s \n message: %s\n stacktrace:\n %s",
                    tag, String.format(format, args), e.getMessage(),
                    Arrays.toString(e.getStackTrace())).replace(',', '\n')
            );
        }
    }
    public static void error(String tag, Exception e, String msg) {
        if (e == null) {
            logger.log(Level.WARNING, String.format("[%s] %s", tag, msg));
        } else {
            logger.log(Level.WARNING, String.format(
                    "[%s] %s \n message: %s\n stacktrace:\n %s",
                    tag, msg, e.getMessage(),
                    Arrays.toString(e.getStackTrace())).replace(',', '\n')
            );
        }
    }
    public static void fault(String tag, Exception e, String msg) {
        Log.error(tag, e, msg);
        System.exit(1);
    }
    public static void fault(String tag, Exception e, String format, Object... args) {
        Log.error(tag, e, format, args);
        System.exit(1);
    }
}
