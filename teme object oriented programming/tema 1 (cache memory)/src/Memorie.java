import java.io.BufferedWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
/**
 * LinkedList simulation of a memory object that makes use of a cache to better
 * respond to requests. 
 * @author VOICU Alex-Georgian
 */
public class Memorie {
    
    final private ArrayList<Subscriptie> list;
    final private Cache cache;
    
    /**
     * Constructs a list representing the memory and also associates it to 
     * an already created cache type.
     * @param cache
     */
    public Memorie(Cache cache) {
        this.list = new ArrayList<Subscriptie>();
        this.cache = cache;
    }
    
    /**
     * Creates an object of type Basic and appends it in memory.
     * @param name
     * @param basicRequests
     */
    public void add(String name, int basicRequests)
    {
        Subscriptie sub = new Basic(basicRequests, name);
        addToMem(sub);
    }
    
    /**
     * Creates an object of type Premium and appends it in memory.
     * @param name
     * @param basicRequests
     * @param premiumRequests
     */
    public void add(String name, int basicRequests, int premiumRequests)
    {
        Subscriptie sub = new Premium(premiumRequests, basicRequests, name);
        addToMem(sub);
    }
    
    //private function that actually adds or replaces an element in 
    //the memory list
    private void addToMem(Subscriptie sub_new)
    {
        Subscriptie sub_iter = null;
        int i, len = list.size();
        String name = sub_new.getName();
        
        //searching for an already existing object by name
        for(i = 0; i < len; i++)
            if(list.get(i).getName().equals(name))
            {
                sub_iter = list.get(i);
                break;
            }
        //if found it is replaced in memory and just removed from cache
        if(sub_iter != null)
        {
            list.remove(i);
            cache.discardobj(sub_iter);
        }
        list.add(sub_new);
    }
    
    /**
     * Prints out the answer to whether a Subscriptie object is in memory, 
     * in cache, or nowhere; also prints the used type of subscription.
     * @param name
     * @param out
     * @throws IOException
     */
    public void get(String name, BufferedWriter out) throws IOException
    {
        Subscriptie sub_search = new Free(name);
        Subscriptie sub_found;
        
        //search in cache
        sub_found = (Subscriptie)cache.search(sub_search);
        if(sub_found != null)
        {
            out.write("0 " + sub_found.getType());
            out.newLine();
            sub_found.decrease();
            return;
        }
        
        //search in memory
        sub_found = (Subscriptie)searchInMem(sub_search);
        if(sub_found != null)
        {
            out.write("1 " + sub_found.getType());
            out.newLine();
            sub_found.decrease();
            cache.add(sub_found);
            return;
        }
        
        out.write("2");
        out.newLine();
    }
    
    //private function used to actually search in memory
    private Object searchInMem(Object o)
    {
        Iterator i = list.iterator();
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
