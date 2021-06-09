package io.menya.tracker.generator.primitive;

import io.menya.tracker.generator.GenerateException;
import io.menya.tracker.generator.ValueGenerator;

public class ShortGenerator extends ValueGenerator<Short> {
    private final static Short[] boundValues = new Short[]{
            0, 1, -1, Short.MAX_VALUE, Short.MIN_VALUE
    };

    @Override
    public Short random() {
        return (short) randomSeed.nextInt();
    }

    @Override
    protected Short boundValue() {
        return boundValues[randomSeed.nextInt(boundValues.length)];
    }

    @Override
    protected Short inverseValue(Short src) throws GenerateException {
        if (src == 0) {
            return 0;
        } else {
            return (short) -src;
        }
    }

    @Override
    protected Short addDelta(Short src) throws GenerateException {
        int delta = randomSeed.nextInt(32) - 16;
        return (short) (src + delta);
    }

    @Override
    protected Short singlePointCrossover(Short v1, Short v2) throws GenerateException {
        short mask = (short) (0xffff << randomSeed.nextInt(Short.SIZE));
        return (short) ((v1 & mask) | (v2 & ~mask));
    }

    @Override
    protected Short uniformCrossover(Short v1, Short v2) throws GenerateException {
        short mask = random();
        return (short) ((v1 & mask) | (v2 & ~mask));
    }
}
