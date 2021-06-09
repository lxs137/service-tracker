package io.menya.tracker.generator.primitive;

import io.menya.tracker.generator.GenerateException;
import io.menya.tracker.generator.ValueGenerator;

public class FloatGenerator extends ValueGenerator<Float> {
    private final static Float[] boundValues = new Float[]{
            0f, 1f, -1f,  Float.MAX_VALUE, Float.MIN_NORMAL, Float.MIN_VALUE,
            Float.NaN, Float.NEGATIVE_INFINITY, Float.POSITIVE_INFINITY
    };

    @Override
    public Float random() {
        return randomSeed.nextFloat() * Float.MAX_VALUE;
    }

    @Override
    protected Float boundValue() {
        return boundValues[randomSeed.nextInt(boundValues.length)];
    }

    @Override
    protected Float inverseValue(Float src) throws GenerateException {
        return -1 / src;
    }

    @Override
    protected Float addDelta(Float src) throws GenerateException {
        float delta = 32 * randomSeed.nextFloat() - 16;
        return (float) (src + delta);
    }

    @Override
    protected Float singlePointCrossover(Float v1, Float v2) throws GenerateException {
        int bits1 = Float.floatToRawIntBits(v1);
        int bits2 = Float.floatToRawIntBits(v2);
        int mask = (0xffffffff << randomSeed.nextInt(Float.SIZE));
        return Float.intBitsToFloat((bits1 & mask) | (bits2 & ~mask));
    }

    @Override
    protected Float uniformCrossover(Float v1, Float v2) throws GenerateException {
        int bits1 = Float.floatToRawIntBits(v1);
        int bits2 = Float.floatToRawIntBits(v2);
        int mask = Float.floatToRawIntBits(random());
        return Float.intBitsToFloat((bits1 & mask) | (bits2 & ~mask));
    }
}
