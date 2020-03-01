
import java.io.BufferedWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.StringTokenizer;

/**
 * Database object that simulates a NoSQL database.
 * @author VOICU Alex-Georgian
 */
public class Database {
    
    private final String name;
    private final int max_capacity;
    private int nodes_num;
    private final HashMap<String, Entity> entity_map;
    private final ArrayList<Node> node_list;
    
    private class Node implements Comparable{
        int id, utilization;
        LinkedList<EInstance> instance_list;

        public Node(int id) {
            this.id = id;
            this.instance_list = new LinkedList<>();
        }

        @Override
        public int compareTo(Object o) {
            if(this.utilization == ((Node)o).utilization)
                return this.id - ((Node)o).id; //crescator
            return ((Node)o).utilization - this.utilization; //descrescator
        }
    }

    /**
     * Constructor that sets up a database with multiple nodes.
     * @param name
     * @param nodes_num
     * @param max_capacity
     */
    public Database(String name, int nodes_num, int max_capacity) {
        this.name = name;
        this.nodes_num = nodes_num;
        this.max_capacity = max_capacity;
        this.entity_map = new HashMap<>();
        this.node_list = new ArrayList<>();
        
        for(int i = 0; i < nodes_num; i++)
            node_list.add(new Node(i));
    }
    
    /**
     * Method that creates a new entity template saved in the database.
     * @param entity_name
     * @param rf
     * @param attr_num
     * @param attributes
     */
    public void create_entity(String entity_name, int rf, int attr_num, 
            String attributes)
    {
        Entity e = new Entity(entity_name, rf, attr_num, attributes);
        this.entity_map.put(entity_name, e);
    }
    
    /**
     * Method that inserts an instance into a number of nodes equal to the 
     * replication factor of the entity.
     * @param entity_name
     * @param values
     */
    public void insert_instance(String entity_name, String values)
    {
        Date timestamp = new Date();
        Entity e = this.entity_map.get(entity_name);
        
        int added = 0;
        Node n;
        for(int i = 0 ; i < this.nodes_num && added < e.getRf(); i++)
        {
            n = node_list.get(i);
            if(n.utilization == this.max_capacity)
                continue;
            EInstance ins = new EInstance(e, values, timestamp);
            n.instance_list.addFirst(ins);
            n.utilization++;
            added++;
        }
        if(added < e.getRf()) //bonus adaugare de noduri
        {
            int toadd = e.getRf() - added;
            for(int i = nodes_num; i < nodes_num + toadd; i++)
                node_list.add(new Node(i)); //adaugare suplimentara de noduri
            nodes_num += toadd;
            
            added = 0;
            for (int i = nodes_num - toadd; i < this.nodes_num; i++) {
                //inserarea nr de instante ramas
                n = node_list.get(i);
                if (n.utilization == this.max_capacity) {
                    continue;
                }
                EInstance ins = new EInstance(e, values, timestamp);
                n.instance_list.addFirst(ins);
                n.utilization++;
                added++;
            }
        }
        node_list.sort(null);
    }
    
    /**
     * Method that prints out the snapshot of the database.
     * @param out
     * @throws IOException
     */
    public void snapshot(BufferedWriter out) throws IOException
    {
        Node n;
        Entity e;
        int empty = 1;
        
        for(int i = 1; i <= this.nodes_num; i++) //parcurgere noduri
        {
            n = this.node_list.get(i-1);
            if(n.instance_list.isEmpty())
                continue;
            empty = 0;
            out.write("Nod" + i);
            out.newLine();
            for(EInstance ins : n.instance_list) //parcurgere lista de instante
            {
                e = ins.getEntity();
                out.write(e.getName());
                for(int j = 0; j < e.getAttr_num(); j++)
                {
                    out.write(" " + e.getAttr_list().get(j).attr_name + ":" + 
                            ins.getAttr_list().get(j).toString());
                }
                out.newLine();
            }
        }
        if(empty == 1)
        {
            out.write("EMPTY DB");
            out.newLine();
        }
    }
    
    /**
     * Method that prints out the specified instance.
     * @param entity_name
     * @param pkey
     * @param out
     * @throws IOException
     */
    public void get(String entity_name, String pkey, BufferedWriter out) throws IOException
    {
        Node n;
        Entity e;
        EInstance found = null;
        
        for(int i = 1; i <= this.nodes_num; i++) //parcurgere noduri
        {
            n = this.node_list.get(i-1);
            if(n.instance_list.isEmpty())
                continue;
            
            for(EInstance ins : n.instance_list) //parcurgere lista de instante
            {
                if(ins.getEntity().getName().equals(entity_name) && 
                        ins.getAttr_list().get(0).toString().equals(pkey))
                { //daca au acelasi nume si aceeasi cheie primara
                    found = ins;
                    out.write("Nod" + i + " ");
                    break;
                }
            }
        }
        if(found == null)
        {
            out.write("NO INSTANCE FOUND");
            out.newLine();
            return;
        }
        e = found.getEntity(); //printare atribute
        out.write(e.getName());
        for (int j = 0; j < e.getAttr_num(); j++) {
            out.write(" " + e.getAttr_list().get(j).attr_name + ":"
                    + found.getAttr_list().get(j).toString());
        }
        out.newLine();
    }
    
    /**
     * Method that updates the values of an instance.
     * @param entity_name
     * @param pkey
     * @param attributes
     */
    public void update(String entity_name, String pkey, String attributes)
    {
        Date timestamp = new Date();
        Node n;
        Entity e;
        StringTokenizer tokens;
        for(int i = 1; i <= this.nodes_num; i++) //parcurgere noduri
        {
            n = this.node_list.get(i-1);
            if(n.instance_list.isEmpty())
                continue;
            
            for(EInstance ins : n.instance_list) //parcurgere lista instante
            {
                //daca gasim instanta in lista de instante ale unui nod
                if(ins.getEntity().getName().equals(entity_name) && 
                        ins.getAttr_list().get(0).toString().equals(pkey))
                {
                    e = ins.getEntity();
                    tokens = new StringTokenizer(attributes);
                    String attr_name = tokens.nextToken();
                    String attr_value = tokens.nextToken();
                    //cautam printre atributele instantei cele care trebuie schimbate 
                    for (int j = 0; j < e.getAttr_num(); j++) {
                        if(e.getAttr_list().get(j).attr_name.equals(attr_name))
                        {   //schimbam atributul ce trebuie schimbat
                            ins.getAttr_list().get(j).set(attr_value);
                            if(tokens.hasMoreTokens())
                            {
                                attr_name = tokens.nextToken();
                                attr_value = tokens.nextToken();
                            }
                        }
                    }
                    ins.setTimestamp(timestamp);
                    //punem la inceputul listei instanta modificata
                    n.instance_list.remove(ins);
                    n.instance_list.addFirst(ins);
                    break;
                }
            }
        }
    }
    
    /**
     * Method that deletes an instance from the database.
     * @param entity_name
     * @param pkey
     * @param out
     * @throws IOException
     */
    public void delete(String entity_name, String pkey, BufferedWriter out) throws IOException
    {
        Node n;
        Entity e;
        int sters = 0;
        
        for(int i = 1; i <= this.nodes_num; i++) //parcurgere noduri
        {
            n = this.node_list.get(i-1);
            if(n.instance_list.isEmpty())
                continue;
            
            for(EInstance ins : n.instance_list) //parcurgere lista instante
            {
                if(ins.getEntity().getName().equals(entity_name) && 
                        ins.getAttr_list().get(0).toString().equals(pkey))
                {
                    n.instance_list.remove(ins);
                    n.utilization--;
                    sters = 1;
                    break;
                }
            }
        }
        if(sters == 0)
        {
            out.write("NO INSTANCE TO DELETE");
            out.newLine();
        }
    }
    
    /**
     * Cleans up the elements older than the specified time.
     * @param dbname
     * @param time
     */
    public void cleanup(String dbname, String time)
    {
        Date time_now = new Date();
        Node n;
        Entity e;
        int sters = 0;
        long delta_time = Long.parseLong(time);
        delta_time = delta_time/1000000;
        
        for(int i = 1; i <= this.nodes_num; i++)
        {
            n = this.node_list.get(i-1);
            if(n.instance_list.isEmpty())
                continue;
            
            for(EInstance ins : n.instance_list)
            {
                if(time_now.getTime() - ins.getTimestamp().getTime() >= delta_time)
                {
                    n.instance_list.remove(ins);
                    n.utilization--;
                    sters = 1;
                    break;
                }
            }
        }
    }
}
