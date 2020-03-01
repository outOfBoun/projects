import java.io.FileReader;
import java.io.BufferedReader;
import java.util.Scanner;
import java.io.FileWriter;
import java.io.BufferedWriter;
import java.io.IOException;
import java.util.StringTokenizer;
/**
 * Main class of the project.
 * @author VOICU Alex-Georgian
 */
public class Main {

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) throws IOException {
        
        //creare obiecte pentru citire si scriere
        BufferedReader in;
        in = new BufferedReader(new FileReader(args[0]));
        Scanner scan = new Scanner(in);
        
        BufferedWriter out;
        out = new BufferedWriter(new FileWriter(args[1]));

        //citirea primelor date
        String cacheType;
        cacheType = scan.nextLine();
        
        int cacheSize;
        cacheSize = scan.nextInt();
        
        int calls;
        calls = scan.nextInt();
        
        //crearea memoriei atribuind cache-ul corespunzator
        Memorie mem;
        switch (cacheType) {
            case "FIFO":
                mem = new Memorie(new FIFOCache(cacheSize));
                break;
            case "LRU":
                mem = new Memorie(new LRUCache(cacheSize));
                break;
            case "LFU":
                mem = new Memorie(new LFUCache(cacheSize));
                break;
            default:
                return;
        }
        
        //parsarea instructiunilor de add si get
        String word;
        StringTokenizer token;
        word = scan.nextLine();
        for(int i = 1; i <= calls; i++)
        {
            token = new StringTokenizer(scan.nextLine(), " ");
            word = token.nextToken();
            if(word.equals("GET"))
                mem.get(token.nextToken(), out);
            else if(word.equals("ADD"))
            {
                if(token.countTokens() == 2)
                    mem.add(token.nextToken(), 
                            Integer.parseInt(token.nextToken()));
                else if(token.countTokens() == 3)
                    mem.add(token.nextToken(), 
                            Integer.parseInt(token.nextToken()), 
                            Integer.parseInt(token.nextToken()));
            }
        }
        in.close();
        out.close();
    }
}
