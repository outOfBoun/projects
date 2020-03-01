
/**
 *
 * @author VOICU Alex-Georgian
 */
public class StringAttribute implements Attribute{
    
    private String value;

    /**
     * Creates an attribute storing an String.
     * @param value
     */
    public StringAttribute(String value) {
        this.value = value;
    }
    
    @Override
    public String toString()
    {
        return value;
    }
    
    /**
     * Sets the value of the attribute leaving the data as a String.
     * @param value
     */
    @Override
    public void set(String value) {
        this.value = value;
    }
    
}
