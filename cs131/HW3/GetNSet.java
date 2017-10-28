import java.util.concurrent.atomic.AtomicIntegerArray;

//use volatile memory
class GetNSet implements State{
    private AtomicIntegerArray arr;
    private byte maxval;

    GetNSet(byte[] starting_arr, byte my_maxval) {
        maxval = my_maxval;
        arr = new AtomicIntegerArray(starting_arr.length);
        for (int i = 0; i < starting_arr.length; i++)
            arr.set(i, starting_arr[i]);
    }

    public int getMaxval() { return maxval; }

    public byte[] getArr() {
        byte[] byte_arr= new byte[arr.length()];
        for (int i = 0; i < byte_arr.length; i++)
            byte_arr[i]=(byte)arr.get(i);
        return byte_arr;
    }

    // attempt to subtract 1 from a and add 1 to b
    public boolean swap(int a, int b) {
        //check i is positive and j is less than maxval
        if (!(arr.get(a) > 0 && arr.get(b) < maxval)){
            return false;
        }
        else{
            arr.decrementAndGet(a);
            arr.incrementAndGet(b);
            return true;
        }
    }
}