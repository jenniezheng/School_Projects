import java.util.concurrent.locks.ReentrantLock;
//swaps with synchronization locks
class BetterSafeTwo extends Default_State {
    private ReentrantLock[] locks;

    BetterSafeTwo(byte[] starting_arr, byte my_maxval) {
        super(starting_arr,my_maxval);
        locks = new ReentrantLock[starting_arr.length];
        for (int i=0; i< starting_arr.length; i++)
            locks[i] = new ReentrantLock();
    }

    public boolean swap(int a, int b) {
        int high=a>b? a :b;
        int low=a>b? b :a;
       locks[high].lock();
       locks[low].lock();
        boolean res=super.swap(a,b);
       locks[low].unlock();
       locks[high].unlock();
        return res;
    }
}
