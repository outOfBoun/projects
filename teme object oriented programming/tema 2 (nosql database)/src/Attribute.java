
/**
 * Class representing an attribute of an instance.
 * @author VOICU Alex-Georgian
 */
public interface Attribute {
    
    /**
     * Returns the value stored in the attribute as a string.
     * @return
     */
    @Override
    public String toString();

    /**
     * Sets the value of the attribute, converting the data to the 
     * corresponding type.
     * @param value
     */
    public void set(String value);
    
}
