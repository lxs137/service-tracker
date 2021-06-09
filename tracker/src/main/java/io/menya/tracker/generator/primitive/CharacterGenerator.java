package io.menya.tracker.generator.primitive;

import io.menya.tracker.generator.GenerateException;
import io.menya.tracker.generator.ValueGenerator;

public class CharacterGenerator extends ValueGenerator<Character> {
    private final static Character[] boundValues = new Character[]{
            0, Character.MAX_HIGH_SURROGATE, Character.MAX_LOW_SURROGATE, Character.MAX_SURROGATE,
            Character.MAX_VALUE, Character.MIN_HIGH_SURROGATE, Character.MIN_LOW_SURROGATE,
            Character.MIN_SURROGATE, Character.MIN_VALUE, '!', '\\', '|', '"', '£', '$', '%', '&',
            '/', '(', ')', '=', '?', '\'', '^', '*', '+', '@', '#', '.', ',', ';', '-', '_'
    };

    @Override
    public Character random() {
        return (char) randomSeed.nextInt(Character.MAX_VALUE);
    }

    @Override
    protected Character boundValue() {
        return boundValues[randomSeed.nextInt(boundValues.length)];
    }

    @Override
    protected Character inverseValue(Character src) throws GenerateException {
        return (char) ~src;
    }

    @Override
    protected Character addDelta(Character src) throws GenerateException {
        int delta = randomSeed.nextInt(32) - 16;
        return (char) (src + delta);
    }

    @Override
    protected Character singlePointCrossover(Character v1, Character v2) throws GenerateException {
        byte mask = (byte) (0xff << randomSeed.nextInt(Byte.SIZE));
        return (char) ((v1 & mask) | (v2 & ~mask));
    }

    @Override
    protected Character uniformCrossover(Character v1, Character v2) throws GenerateException {
        byte mask = (byte) random().charValue();
        return (char) ((v1 & mask) | (v2 & ~mask));
    }
}
