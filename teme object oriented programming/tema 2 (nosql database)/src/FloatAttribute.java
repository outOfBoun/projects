
import java.text.DecimalFormat;

/**
 *
 * @author VOICU Alex-Georgian
 */
public class FloatAttribute implements Attribute{
    
    private Float value;

    /**
     * Creates an attribute storing an Float.
     * @param value
     */
    public FloatAttribute(Float value) {
        this.value = value;
    }
    
    @Override
    public String toString()
    {
        DecimalFormat d = new DecimalFormat("#.##");
        return d.format(value);
    }
    
    /**
     * Sets the value of the attribute, converting the data to a Float.
     * @param value
     */
    @Override
    public void set(String value) {
        this.value = Float.valueOf(value);
    }
    
}
