package io.menya.tracker.generator.primitive;

import io.menya.tracker.generator.GenerateException;
import io.menya.tracker.generator.ValueGenerator;

public class IntegerGenerator extends ValueGenerator<Integer> {
    private final static Integer[] boundValues = new Integer[]{
            0, 1, -1, Integer.MAX_VALUE, Integer.MIN_VALUE
    };

    @Override
    public Integer random() {
        return randomSeed.nextInt();
    }

    @Override
    protected Integer boundValue() {
        return boundValues[randomSeed.nextInt(boundValues.length)];
    }

    @Override
    protected Integer inverseValue(Integer src) throws GenerateException {
        if (src == 0) {
            return 0;
        } else {
            return -src;
        }
    }

    @Override
    protected Integer addDelta(Integer src) throws GenerateException {
        int delta = randomSeed.nextInt(32) - 16;
        return src + delta;
    }

    @Override
    protected Integer singlePointCrossover(Integer v1, Integer v2) throws GenerateException {
        int mask = (0xffffffff << randomSeed.nextInt(Integer.SIZE));
        return ((v1 & mask) | (v2 & ~mask));
    }

    @Override
    protected Integer uniformCrossover(Integer v1, Integer v2) throws GenerateException {
        int mask = random();
        return ((v1 & mask) | (v2 & ~mask));
    }
}
