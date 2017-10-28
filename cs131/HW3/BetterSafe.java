import java.util.concurrent.locks.ReentrantLock;
//swaps with synchronization locks
class BetterSafe extends Default_State {
    private ReentrantLock lock;

    BetterSafe(byte[] starting_arr, byte my_maxval) {
        super(starting_arr,my_maxval);
        lock = new ReentrantLock();
    }

    public boolean swap(int a, int b) {
       lock.lock();
        boolean res=super.swap(a,b);
       lock.unlock();
        return res;
    }
}
