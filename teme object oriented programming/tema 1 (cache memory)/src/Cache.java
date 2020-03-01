/**
 * A small memory capable of responding to requests faster than main memory.
 * @author VOICU Alex-Georgian
 */
public interface Cache {
    
    /**
     * Appends an object to cache if its size allows; otherwise will first 
     * remove an element based on the method of caching used.
     * @param o
     */
    public abstract void add(Object o);

    /**
     * Removes an element from the cache based on the method of caching used.
     */
    public abstract void remove();

    /**
     * Removes a specific element from the cache regardless of the method of 
     * caching used.
     * @param o
     */
    public abstract void discardobj(Object o);

    /**
     * Returns an object that is equal to the one passed as argument.
     * @param o
     * @return
     */
    public abstract Object search(Object o);
    
}
