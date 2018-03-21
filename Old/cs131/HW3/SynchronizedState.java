//swaps with synchronized keyword
class SynchronizedState extends Default_State {
    SynchronizedState(byte[] starting_arr, byte my_maxval) {
        super(starting_arr,my_maxval);
    }

    public synchronized boolean swap(int a, int b) {
        return super.swap(a,b);
    }
}
