//swaps without synchronizing
class Unsynchronized extends Default_State {
    Unsynchronized(byte[] starting_arr, byte my_maxval) {
        super(starting_arr,my_maxval);}


     // attempt to subtract 1 from i and add 1 to j
    public boolean swap(int a, int b) {
        boolean res=super.swap(a,b);
        if(!res){
            //prevent loop by checking least 2 values not same and not both
            // > maxval or < 0
            for (int index=1; index<arr.length; index++)
                if(arr[index-1]!=arr[index] &&
                     (arr[index-1]<maxval || arr[index] < maxval ) &&
                      (arr[index-1]> 0 || arr[index] > 0 ) )
                    return false;
            //no valid swaps
            //System.err.printf("No valid swaps! Resetting.\n");
            byte target=(byte) ((int)maxval/2);
            for (int index=0; index<arr.length; index++)
                arr[index]=target;
        }
        return res;
    }
}
