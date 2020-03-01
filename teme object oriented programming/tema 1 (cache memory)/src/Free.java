/**
 * A Subscriptie descendant that implements a Free subscription type.
 * @author VOICU Alex-Georgian
 */
public class Free extends Subscriptie{

    /**
     * Constructor of Free subscription.
     * @param name
     */
    public Free(String name) {
        super(name);
    }

    /**
     * Function that returns the type of this object as a string.
     * @return
     */
    @Override
    public String getType() {
        return "Free";
    }

    /**
     * Function that decreases the number of left requests.
     */
    @Override
    public void decrease() {
    }
}
