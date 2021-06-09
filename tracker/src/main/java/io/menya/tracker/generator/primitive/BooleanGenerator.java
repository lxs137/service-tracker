package io.menya.tracker.generator.primitive;

import io.menya.tracker.generator.GenerateException;
import io.menya.tracker.generator.ValueGenerator;
import io.menya.tracker.utils.Utils;

public class BooleanGenerator extends ValueGenerator<Boolean> {
    @Override
    protected Boolean random() {
        return Utils.random.nextBoolean();
    }

    @Override
    protected Boolean boundValue() {
        return false;
    }

    @Override
    protected Boolean inverseValue(Boolean src) throws GenerateException {
        return !src;
    }

    @Override
    protected Boolean addDelta(Boolean src) throws GenerateException {
        return !src;
    }

    @Override
    protected Boolean singlePointCrossover(Boolean v1, Boolean v2) throws GenerateException {
        return v1 ^ v2;
    }

    @Override
    protected Boolean uniformCrossover(Boolean v1, Boolean v2) throws GenerateException {
        return v1 & v2;
    }
}
