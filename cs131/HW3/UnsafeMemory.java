class UnsafeMemory {
    //prints specific error
    private static void print_error(String str) {
        System.err.format("%s\n", str);
        System.exit(2);
    }
    //print inputs required
    private static void print_usage() {
        System.err.println("Usage: STATE THREADS TRIALS MAXVAL ARR0 ARR1 ARR2 ...\n");
        System.exit (1);
    }

    //generating error when throws InterruptedException wasn't included
    private static void do_swaps(State state, int threads, int trials)
    throws InterruptedException{
        Thread[] all_threads = new Thread[threads];
        for (int i = 0; i < threads; i++) {
            int thread_trials = (trials / threads);
            if( i< trials % threads)
                thread_trials+=1;
            all_threads[i] = new Thread (new SwapTest (thread_trials, state));
        }
        long start_time = System.nanoTime();
        for (int i = 0; i < threads; i++)
            all_threads[i].start ();
        for (int i = 0; i < threads; i++)
           all_threads[i].join ();
        long end_time = System.nanoTime();
        double elapsed_time_in_ns = end_time - start_time;
        System.out.printf("Threads average %f ns per transition\n",
                   elapsed_time_in_ns * threads / trials);
    }

    private static void get_result(byte[] original, byte[] modified, byte maxval) {
    long original_sum = 0;
    long modified_sum = 0;
    for (int i = 0; i < original.length; i++)
        {
        original_sum += original[i];
        modified_sum += modified[i];
        if (modified[i] < 0)
            print_error("Arr has negative output " + String.valueOf(modified[i]));
        if (modified[i] > maxval)
            print_error("Arr has output greater than maxval "+ String.valueOf(modified[i]));
        }
    if (original_sum != modified_sum){
        print_error("Original and modified sums don't match "+String.valueOf(original_sum)+" "+String.valueOf(modified_sum));
        }
    }

     public static void main(String args[]) {
        if (args.length < 4){
            print_usage();
        }
        try {

            int threads = Integer.parseInt (args[1]);
            if(threads<1 || threads > 1000)
                print_error("Invalid number of threads: "+args[1]);

            int trials = Integer.parseInt (args[2]);
            if(trials<1 )
                print_error("Invalid number of trials: "+args[2]);

            int maxval_int = Integer.parseInt (args[3]);
            if(maxval_int<1 || maxval_int>127 )
                print_error("Invalid maxval: "+args[3]);

            byte maxval=(byte) maxval_int;

            byte[] arr = new byte[args.length - 4];

            for (int i = 0; i < args.length-4; i++){
                int arr_value=Integer.parseInt (args[i+4]);
                if(arr_value<0 || arr_value>maxval_int )
                    print_error("Invalid arr value: "+args[i+4]);
                arr[i] = (byte) arr_value;
            }

            byte[] arr_copy=arr.clone();
            State state = new Nullstate(arr, maxval);
            if (args[0].equals("Null"))
                state = new Nullstate(arr, maxval);
            else if (args[0].equals("Synchronized"))
                state = new SynchronizedState(arr, maxval);
            else if (args[0].equals("Unsynchronized"))
                state = new Unsynchronized(arr, maxval);
            else if (args[0].equals("GetNSet"))
                state = new GetNSet(arr, maxval);
            else if (args[0].equals("BetterSafe"))
                state = new BetterSafe(arr, maxval);
            else if (args[0].equals("BetterSafeTwo"))
                state = new BetterSafeTwo(arr, maxval);
            else
                print_error("Invalid state: "+args[0]);

            do_swaps(state, threads, trials);
            get_result(arr_copy, state.getArr(), maxval);
            System.exit (0);
        }
        catch (Exception exc){
                print_usage();
        }
    }

}