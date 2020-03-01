
import java.util.ArrayList;
import java.util.StringTokenizer;

/**
 * Class that models the concept of an entity in a database.
 * @author VOICU Alex-Georgian
 */
public class Entity{
    
    private final String name;
    private final int rf, attr_num;
    private final ArrayList<AttributeDef> attr_list;
    
    class AttributeDef{
        String attr_name, attr_type;
        public AttributeDef(String attr_name, String attr_type) {
            this.attr_name = attr_name;
            this.attr_type = attr_type;
        }
    }

    /**
     * Creates an entity.
     * @param name
     * @param rf
     * @param attr_num
     * @param attributes
     */
    public Entity(String name, int rf, int attr_num, String attributes) {
        this.name = name;
        this.rf = rf;
        this.attr_num = attr_num;
        this.attr_list = new ArrayList<>();
        
        StringTokenizer tokens = new StringTokenizer(attributes);
        AttributeDef attr;
        String attrname, attrtype;
        while(tokens.hasMoreTokens())
        {
            attr = new AttributeDef(tokens.nextToken(), tokens.nextToken());
            this.attr_list.add(attr);
        }
    }

    /**
     * Gets the name of the entity.
     * @return
     */
    public String getName() {
        return name;
    }
    
    /**
     * Gets the replication factor of the entity.
     * @return
     */
    public int getRf() {
        return rf;
    }

    /**
     * Gets the number of attributes of the entity.
     * @return
     */
    public int getAttr_num() {
        return attr_num;
    }

    /**
     * Gets the attribute list containing attribute types and names of the entity.
     * @return
     */
    public ArrayList<AttributeDef> getAttr_list() {
        return attr_list;
    }
    
}
