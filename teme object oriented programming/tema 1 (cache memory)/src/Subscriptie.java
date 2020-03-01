import java.util.Objects;
/**
 * Abstract class that sets out the base of a subscription.
 * @author VOICU Alex-Georgian
 */


public abstract class Subscriptie {
    
    /**
     * Name of the subscription.
     */
    protected String name;

    /**
     * Base constructor that takes a name to create a subscription.
     * @param name
     */
    public Subscriptie(String name) {
        this.name = name;
    }
    
    /**
     * Returns the name of the subscription.
     * @return
     */
    public String getName() {
        return name;
    }

    @Override
    public boolean equals(Object other) {
        if (this == other) {
            return true;
        }
        if (other == null) {
            return false;
        }
        return Objects.equals(this.name, ((Subscriptie)other).name);
    }

    @Override
    public int hashCode() {
        int hash = 7;
        hash = 97 * hash + Objects.hashCode(this.name);
        return hash;
    }
    
    /**
     * Returns the type of the current object if it inherits the Subscriptie 
     * class.
     * @return
     */
    public abstract String getType();

    /**
     * Decreases the number of available requests of an object that inherits 
     * the Subscriptie class. 
     */
    public abstract void decrease();
}
