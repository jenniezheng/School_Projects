import java.util.concurrent.ThreadLocalRandom;

class SwapTest implements Runnable {
    private int trials;
    private State state;

    SwapTest(int mytrials, State mystate) {
        trials = mytrials;
        state = mystate;
    }

    public void run() {
        int maxLength = state.getArr().length;
        int i=0;
        while (i<trials){
            int a = ThreadLocalRandom.current().nextInt(0, maxLength);
            int b = ThreadLocalRandom.current().nextInt(0, maxLength);
            if (a != b && state.swap(a, b))
                i++;
        }
    }
}