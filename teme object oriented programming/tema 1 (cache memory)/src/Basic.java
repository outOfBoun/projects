/**
 * Subscriptie descendant that implements a Basic subscription type.
 * @author VOICU Alex-Georgian
 */
public class Basic extends Free{
    
    private int basicRequests;

    /**
     * Constructor of Basic subscription.
     * @param basicRequests
     * @param name
     */
    public Basic(int basicRequests, String name) {
        super(name);
        this.basicRequests = basicRequests;
    }

    /**
     * Function that returns the type of this object as a string based on the 
     * number of remaining basic requests.
     * @return
     */
    @Override
    public String getType() {
        if(this.basicRequests > 0)
            return "Basic";
        return super.getType();
    }

    /**
     * Function that decreases the number of left requests.
     */
    @Override
    public void decrease() {
        if(this.basicRequests > 0)
            this.basicRequests--;
        else 
            super.decrease();
    }
}
