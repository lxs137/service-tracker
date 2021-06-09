package io.menya.tracker.generator.primitive;

import io.menya.tracker.generator.GenerateException;
import io.menya.tracker.generator.ValueGenerator;
import io.menya.tracker.utils.Utils;

public class ByteGenerator extends ValueGenerator<Byte> {
    private final static Byte[] boundValues = new Byte[]{
            0, 1, -1, Byte.MAX_VALUE, Byte.MIN_VALUE
    };

    @Override
    public Byte random() {
        byte[] bytes = new byte[1];
        Utils.random.nextBytes(bytes);
        return bytes[0];
    }

    @Override
    protected Byte boundValue() {
        return boundValues[randomSeed.nextInt(boundValues.length)];
    }

    @Override
    protected Byte inverseValue(Byte src) throws GenerateException {
        return (byte) ~src;
    }

    @Override
    protected Byte addDelta(Byte src) throws GenerateException {
        return (byte) (src ^ (1 << randomSeed.nextInt(Byte.SIZE)));
    }

    @Override
    protected Byte singlePointCrossover(Byte v1, Byte v2) throws GenerateException {
        byte mask = (byte) (0xff << randomSeed.nextInt(Byte.SIZE));
        return (byte) ((v1 & mask) | (v2 & ~mask));
    }

    @Override
    protected Byte uniformCrossover(Byte v1, Byte v2) throws GenerateException {
        byte mask = random();
        return (byte) ((v1 & mask) | (v2 & ~mask));
    }
}
