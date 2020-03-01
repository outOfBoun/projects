import java.util.LinkedList;
import java.util.Iterator;
import java.util.Objects;
/**
 * Least frequently used cache implementation.
 * @author VOICU Alex-Georgian
 */

public class LFUCache implements Cache {
    
    LinkedList<Pair> list;
    final private int maxSize;
    private int currentSize;
    
    class Pair implements Comparable<Pair>{
        final private Object o;
        private int freq;
        public Pair(Object o) {
            this.o = o;
            this.freq = 0;
        }
        void incFreq()
        {
            this.freq++;
        }
        @Override
        public int compareTo(Pair o) {
            return this.freq - o.freq;
        }
        @Override
        public int hashCode() {
            int hash = 7;
            return hash;
        }
        @Override
        public boolean equals(Object other) {
            if (this == other) {
                return true;
            }
            if (other == null) {
                return false;
            }
            return Objects.equals(this.o, ((Pair)other).o);
        }
    }

    /**
     * Creates a LFU cache
     * @param maxSize
     */
    public LFUCache(int maxSize) {
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
        list.add(new Pair(o));
    }

    /**
     * Removes the object with the least amount of use.
     */
    @Override
    public void remove() {
        this.currentSize--;
        Iterator i = list.iterator();
        Pair pmin = (Pair)i.next(), p = pmin;
        while(i.hasNext())
        {
            p = (Pair)i.next();
            if(pmin.freq > p.freq)
            {
                pmin = p;
            }
        }
        list.remove(pmin);
    }

    /**
     * Removes a specific object from the list.
     * @param o
     */
    @Override
    public void discardobj(Object o) {
        if(list.remove(new Pair(o)) == true)
        {
            this.currentSize--;
        }
    }

    /**
     * Returns the object that is equal to the one given as argument.
     * @param o
     * @return
     */
    @Override
    public Object search(Object o) {
        
        Iterator i = list.iterator();
        Pair p, pfind = new Pair(o);
        while(i.hasNext())
        {
            p = (Pair)i.next();
            if(p.equals(pfind))
            {
                p.incFreq();
                return p.o;
            }
        }
        return null;
    }
}

