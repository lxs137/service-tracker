package io.menya.tracker.utils;

import org.apache.commons.cli.CommandLine;
import org.apache.commons.cli.DefaultParser;
import org.apache.commons.cli.Option;
import org.apache.commons.cli.Options;
import org.apache.commons.cli.ParseException;

public class OptionsHelper {
    private static CommandLine cmd;

    public static void init(String[] args) {
        Options options = new Options();

        Option action = new Option("a", "action", true, "what to do");
        action.setRequired(true);
        options.addOption(action);
        Option targetPid = new Option("pid", "pid", true, "trace pid");
        targetPid.setRequired(true);
        options.addOption(targetPid);

        try {
            OptionsHelper.cmd = new DefaultParser().parse(options, args);
        } catch (ParseException e) {
            Log.fault("OptionsHelper", e,"invalid options");
        }
    }

    public static Action getAction() {
        Action action = Action.fromString(cmd.getOptionValue("action"));
        if (action == null) {
            Log.fault("OptionsHelper", null, "unsupported action: %s", cmd.getOptionValue("action"));
        }
        return action;
    }

    public static int getTargetPid() {
        return Integer.parseInt(cmd.getOptionValue("pid"));
    }
}
