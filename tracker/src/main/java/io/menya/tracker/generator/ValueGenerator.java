package io.menya.tracker.generator;

import java.util.Random;

public abstract class ValueGenerator<T> {
    protected static final Random randomSeed = new Random(System.currentTimeMillis());

    public T init() {
        return random();
    }
    public T mutate(T src) throws GenerateException {
        int probability = randomSeed.nextInt(100);
        if (probability < 5) {
            return boundValue();
        } else if (probability < 20) {
            return inverseValue(src);
        } else {
            return addDelta(src);
        }
    }
    public T crossover(T v1, T v2) throws GenerateException {
        int probability = randomSeed.nextInt(10);
        if (probability < 4) {
            return singlePointCrossover(v1, v2);
        } else {
            return uniformCrossover(v1, v2);
        }
    }

    protected abstract T random();
    protected abstract T boundValue();
    protected abstract T inverseValue(T src) throws GenerateException;
    protected abstract T addDelta(T src) throws GenerateException;

    protected abstract T singlePointCrossover(T v1, T v2) throws GenerateException;
    /**
     * In uniform crossover, typically, each bit is chosen from either parent with equal probability
     */
    protected abstract T uniformCrossover(T v1, T v2) throws GenerateException;
}
