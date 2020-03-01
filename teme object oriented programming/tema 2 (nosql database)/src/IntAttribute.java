
/**
 *
 * @author VOICU Alex-Georgian
 */
public class IntAttribute implements Attribute{
    
    private Integer value;

    /**
     * Creates an attribute storing an Integer.
     * @param value
     */
    public IntAttribute(Integer value) {
        this.value = value;
    }
    
    @Override
    public String toString()
    {
        return value.toString();
    }

    /**
     * Sets the value of the attribute, converting the data to an Integer.
     * @param value
     */
    @Override
    public void set(String value) {
        this.value = Integer.valueOf(value);
    }
    
}
