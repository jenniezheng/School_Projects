// doesn't swap at all
class Nullstate extends Unsynchronized {
    Nullstate(byte[] starting_arr, byte my_maxval) {
        super(starting_arr,my_maxval);}
    //overrides default swap
    //must return true since failed swaps not counted
    public boolean swap(int a, int b) { return true; }
}