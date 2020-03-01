
import java.util.ArrayList;
import java.util.Date;
import java.util.StringTokenizer;

/**
 * Class that models the concept of an instance in a database.
 * @author VOICU Alex-Georgian
 */
public class EInstance {
    
    private Entity entity;
    private final ArrayList<Attribute> attr_list;
    private Date timestamp;

    /**
     * Creates an instance linking it to an entity and filling it with the 
     * necessary values.
     * @param entity
     * @param values
     * @param t
     */
    public EInstance(Entity entity, String values, Date t) {
        this.entity = entity;
        this.timestamp = t;
        this.attr_list = new ArrayList<>();

        StringTokenizer tokens = new StringTokenizer(values);
        Entity.AttributeDef attr;
        String attrval;
        for(int i = 0; i < this.entity.getAttr_num(); i++)
        {
            attrval = tokens.nextToken();
            attr = this.entity.getAttr_list().get(i);
            switch (attr.attr_type) {
                case "Integer":
                    attr_list.add(new IntAttribute(Integer.valueOf(attrval)));
                    break;
                case "Float":
                    attr_list.add(new FloatAttribute(Float.valueOf(attrval)));
                    break;
                default:
                    attr_list.add(new StringAttribute(attrval));
                    break;
            }
        }
    }

    /**
     * Gets the entity linked with the instance.
     * @return
     */
    public Entity getEntity() {
        return entity;
    }

    /**
     * Gets the list of attributes containing the values of the instance.
     * @return
     */
    public ArrayList<Attribute> getAttr_list() {
        return attr_list;
    }

    /**
     * Gets the date of creation of the instance.
     * @return
     */
    public Date getTimestamp() {
        return timestamp;
    }

    /**
     * Sets the date of creation of the instance (used when updating values).
     * @param timestamp
     */
    public void setTimestamp(Date timestamp) {
        this.timestamp = timestamp;
    }

}
