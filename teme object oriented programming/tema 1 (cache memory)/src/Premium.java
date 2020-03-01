/**
 * Subscriptie descendant that implements a Premium subscription type.
 * @author VOICU Alex-Georgian
 */
public class Premium extends Basic{

    private int premiumRequests;

    /**
     * Constructor of Premium subscription.
     * @param premiumRequests
     * @param basicRequests
     * @param name
     */
    public Premium(int premiumRequests, int basicRequests, String name) {
        super(basicRequests, name);
        this.premiumRequests = premiumRequests;
    }

    /**
     * Function that returns the type of this object as a string based on the 
     * number of remaining premium requests.
     * @return
     */
    @Override
    public String getType() {
        if(this.premiumRequests > 0)
            return "Premium";
        return super.getType();
    }
    
    /**
     * Function that decreases the number of left requests.
     */
    @Override
    public void decrease() {
        if(this.premiumRequests > 0)
            this.premiumRequests--;
        else
            super.decrease();
    }
}
