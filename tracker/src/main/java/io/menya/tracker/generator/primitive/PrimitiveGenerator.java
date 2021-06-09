package io.menya.tracker.generator.primitive;

import io.menya.tracker.generator.ValueGenerator;

public class PrimitiveGenerator {
    public static ValueGenerator<?> build(Class<?> type) {
        if (type.equals(Byte.TYPE)) {
            return new ByteGenerator();
        } else if (type.equals(Short.TYPE)) {
            return new ShortGenerator();
        } else if (type.equals(Integer.TYPE)) {
            return new IntegerGenerator();
        } else if (type.equals(Long.TYPE)) {
            return new LongGenerator();
        } else if (type.equals(Float.TYPE)) {
            return new FloatGenerator();
        } else if (type.equals(Double.TYPE)) {
            return new DoubleGenerator();
        } else if (type.equals(Boolean.TYPE)) {
            return new BooleanGenerator();
        } else if (type.equals(Character.TYPE)) {
            return new CharacterGenerator();
        } else {
            throw new RuntimeException("not supported primitive type " + type.getName());
        }
    }
}
