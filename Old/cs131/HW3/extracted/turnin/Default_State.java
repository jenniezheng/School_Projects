//swaps without synchronizing
class Default_State implements State {
    protected byte[] arr;
    protected byte maxval;

    Default_State(byte[] starting_arr, byte my_maxval) {
        arr = starting_arr;
        maxval = my_maxval;
    }

    public int size() { return arr.length; }

    public byte[] current(){
        return arr;
    }

    // attempt to subtract 1 from i and add 1 to j
    public boolean swap(int a, int b) {
        //check i is positive and j is less than maxval
        if (!(arr[a] > 0 && arr[b] < maxval)) {
            return false;
        }
        else{
            arr[a]--;
            arr[b]++;
            return true;
        }
    }

}
