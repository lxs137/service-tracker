package io.menya.tracker.generator.primitive;

import io.menya.tracker.generator.GenerateException;
import io.menya.tracker.generator.ValueGenerator;

public class LongGenerator extends ValueGenerator<Long> {
    private final static Long[] boundValues = new Long[]{
            0L, 1L, -1L, Long.MAX_VALUE, Long.MIN_VALUE
    };

    @Override
    public Long random() {
        return randomSeed.nextLong();
    }

    @Override
    protected Long boundValue() {
        return boundValues[randomSeed.nextInt(boundValues.length)];
    }

    @Override
    protected Long inverseValue(Long src) throws GenerateException {
        if (src == 0L) {
            return 0L;
        } else {
            return -src;
        }
    }

    @Override
    protected Long addDelta(Long src) throws GenerateException {
        long delta = randomSeed.nextLong() % 32 - 16;
        return src + delta;
    }

    @Override
    protected Long singlePointCrossover(Long v1, Long v2) throws GenerateException {
        long mask = (0xffffffffffffffffL << randomSeed.nextInt(Long.SIZE));
        return ((v1 & mask) | (v2 & ~mask));
    }

    @Override
    protected Long uniformCrossover(Long v1, Long v2) throws GenerateException {
        long mask = random();
        return ((v1 & mask) | (v2 & ~mask));
    }
}
