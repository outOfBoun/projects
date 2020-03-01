import java.util.Queue;
import java.util.ArrayDeque;
import java.util.Iterator;
/**
 * First in first out cache implementation.
 * @author VOICU Alex-Georgian
 */
public class FIFOCache implements Cache{
    
    final private Queue<Object> q;
    final private int maxSize;
    private int currentSize;

    /**
     * Creates a FIFO cache.
     * @param maxSize
     */
    public FIFOCache(int maxSize) {
        this.maxSize = maxSize;
        this.currentSize = 0;
        this.q = new ArrayDeque<Object>();
    }

    /**
     * Adds an object at the end of the queue.
     * @param o
     */
    @Override
    public void add(Object o) {
        if(this.maxSize == this.currentSize)
            this.remove();
        this.currentSize++;
        q.add(o);
    }
    
    /**
     * Removes the head of the queue.
     */
    @Override
    public void remove() {
        this.currentSize--;
        q.remove();
    }

    /**
     * Removes a specific object from the queue.
     * @param o
     */
    @Override
    public void discardobj(Object o) {
        if(q.remove(o) == true)
        {
            this.currentSize--;
        }
    }

    /**
     * Returns an object from the queue that is equal to the one passed as 
     * argument.
     * @param o
     * @return
     */
    @Override
    public Object search(Object o) {
        Iterator i = q.iterator();
        Object next;
        while(i.hasNext())
        {
            next = i.next();
            if(next.equals(o))
                return next;
        }
        return null;
    }
}
