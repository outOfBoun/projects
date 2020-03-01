import java.util.LinkedList;
import java.util.Iterator;
/**
 * Least recently used cache implementation.
 * @author VOICU Alex-Georgian
 */
public class LRUCache implements Cache {
    
    LinkedList<Object> list;
    final private int maxSize;
    private int currentSize;

    /**
     * Creates a LRU cache.
     * @param maxSize
     */
    public LRUCache(int maxSize) {
        this.maxSize = maxSize;
        this.currentSize = 0;
        this.list = new LinkedList<>();
    }

    /**
     * Adds an object at the end of the list.
     * @param o
     */
    @Override
    public void add(Object o) {
        if(this.maxSize == this.currentSize)
            this.remove();
        this.currentSize++;
        list.add(o);
    }

    /**
     * Removes the first element in the list.
     */
    @Override
    public void remove() {
        this.currentSize--;
        list.removeFirst();
    }

    /**
     * Removes a specific object from the list.
     * @param o
     */
    @Override
    public void discardobj(Object o) {
        if(list.remove(o) == true)
        {
            this.currentSize--;
        }
    }

    /**
     * Returns an object from the list that is equal to the one passed as 
     * argument.
     * @param o
     * @return
     */
    @Override
    public Object search(Object o) {
        Iterator i = list.iterator();
        Object next = null;
        Boolean found = false;
        while(i.hasNext())
        {
            next = i.next();
            if(next.equals(o))
            {
                found = true;
                break;
            }
        }
        if(found)
        {   
            //moving the recently found obj to the end of the list
            i.remove();
            list.add(next);
            return next;
        }
        return null;
    }
}
