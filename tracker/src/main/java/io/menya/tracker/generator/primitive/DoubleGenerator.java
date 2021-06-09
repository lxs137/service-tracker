package io.menya.tracker.generator.primitive;

import io.menya.tracker.generator.GenerateException;
import io.menya.tracker.generator.ValueGenerator;

public class DoubleGenerator extends ValueGenerator<Double> {
    private final static Double[] boundValues = new Double[]{
            0d, (double) 1, (double) -1,  Double.MAX_VALUE, Double.MIN_NORMAL, Double.MIN_VALUE,
            Double.NaN, Double.NEGATIVE_INFINITY, Double.POSITIVE_INFINITY
    };

    @Override
    public Double random() {
        return randomSeed.nextDouble() * Double.MAX_VALUE;
    }

    @Override
    protected Double boundValue() {
        return boundValues[randomSeed.nextInt(boundValues.length)];
    }

    @Override
    protected Double inverseValue(Double src) throws GenerateException {
        return -1 / src;
    }

    @Override
    protected Double addDelta(Double src) throws GenerateException {
        double delta = 32 * randomSeed.nextDouble() - 16;
        return (double) (src + delta);
    }

    @Override
    protected Double singlePointCrossover(Double v1, Double v2) throws GenerateException {
        long bits1 = Double.doubleToRawLongBits(v1);
        long bits2 = Double.doubleToRawLongBits(v2);
        long mask = (0xffffffffffffffffL << randomSeed.nextInt(Double.SIZE));
        return Double.longBitsToDouble((bits1 & mask) | (bits2 & ~mask));
    }

    @Override
    protected Double uniformCrossover(Double v1, Double v2) throws GenerateException {
        long bits1 = Double.doubleToRawLongBits(v1);
        long bits2 = Double.doubleToRawLongBits(v2);
        long mask = Double.doubleToRawLongBits(random());
        return Double.longBitsToDouble((bits1 & mask) | (bits2 & ~mask));
    }
}
