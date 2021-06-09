package io.menya.tracker.testing;

import java.util.concurrent.atomic.AtomicBoolean;

import io.menya.tracker.executor.ServiceMethodCaller;
import io.menya.tracker.utils.Log;

public class Fuzzer {
    private static final String TAG = "Fuzzer";

    public static void run(String serviceName, String processName) throws InfiltrationException, InstrumentationException {
        AtomicBoolean isServiceDead = new AtomicBoolean(false);

        ServiceMethodCaller caller = ServiceMethodCaller.buildFromName(serviceName, processName);
        if (caller == null) {
            Log.info(TAG, "service method caller can not created");
            return;
        }

        /*
         * initialize the test executor and the test analyzer
         */
        TestExecutor testExecutor = new TestExecutor(processName, isServiceDead);

        L.i("Initialized - testExecutor");

        BlackBoxTestAnalyzer testAnalyzer = new BlackBoxTestAnalyzer();

        L.i("Initialized - testAnalyzer");

        /*
         * build a method caller for each (filtered) method of the service
         */
        List<MethodCaller> methodCallers = new MethodCallerBuilder()
                .setCalledObject(serviceManagerWrapper
                        .getServiceObjectByName(serviceName, isServiceDead))
                .createMethodCallers(new MethodCallerBuilderFuzzableMethodFilter());

        L.i("Initialized - method callers");


        Queue<BlackBoxTest> queue = new ArrayDeque<>();
        for (MethodCaller methodCaller : methodCallers){
            ParametersGenerator generator = new ParametersGenerator(methodCaller.getParameterTypes());
            queue.add(new BlackBoxTest(methodCaller, generator, generator.random()));
        }

        L.i("Initialized - parameters generator");

        // in black box maxGeneration is the number of experiment
        int numberOfExperiment = ConfigurationManager.getMaxGeneration();

        for (int i = 0; i< numberOfExperiment; i++){
            if (queue.isEmpty()){
                L.w("queue is empty!");
                break;
            }

            if (i % 20 == 0) checkBattery(serviceManagerWrapper);

            BlackBoxTest test = queue.remove();

            try {
                testExecutor.execute(test);
            } catch (TestExecutor.DeadServiceException e) {
                L.e("trying to execute with a dead service", e);
                handleDeath(testExecutor, serviceManagerWrapper, methodCallers);
            }

            testAnalyzer.startAnalysis(test);

            /*
             * if the test killed the service, re-initialize
             */
            if (test.isKiller()){
                handleDeath(testExecutor, serviceManagerWrapper, methodCallers);
            }

            testAnalyzer.startSave(test);

            try {
                queue.add(test.fuzz());
            } catch (ValueGeneratorException e) {
                L.e("problem in fuzzing the blackboxtest", e);
            }

        }

        L.i("End of the fuzzing");

    }

    /*
     * it handles the death of the service under test by re-initializing the instrumentation and by
     * updating the service object in the method callers
     */
    private static void handleDeath(TestExecutor testExecutor,
                                    ServiceManagerWrapper serviceManagerWrapper,
                                    List<MethodCaller> methodCallers)
            throws InstrumentationException, InfiltrationException {
        L.i("handling the target death...");

        /*
         * retrieve main parameters
         */
        String serviceName = ConfigurationManager.getServiceName();
        String processName = ConfigurationManager.getProcessName();

        /*
         * wait the smartphone to inizialize all its services
         */
        AtomicBoolean isServiceDead = new AtomicBoolean(false);

        int trials = 1;
        do {
            try {
                try {
                    Thread.sleep(trials
                            * ConfigurationManager.getRecoveryTimeInMilliseconds());
                } catch (InterruptedException ignored) {}

                L.d(trials + "^ trial");
                testExecutor.recycle();
                testExecutor.initInstrumentation(processName, isServiceDead);

                break;
            } catch (InstrumentationException e) {
                L.e("can't restore fuzzing... retry");
            }
        } while (trials++ <= ConfigurationManager.getRestoreMaxTrial());

        Object calledObject = serviceManagerWrapper
                .getServiceObjectByName(serviceName, isServiceDead);
        for (MethodCaller methodCaller : methodCallers){
            methodCaller.setCalledObject(calledObject);
        }

        L.i("restored fuzzing");

    }
}
