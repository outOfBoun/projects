
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.StringTokenizer;

/**
 * Main entry point into the program.
 * @author VOICU Alex-Georgian
 */
public class Tema2 {

    /**
     * @param args the command line arguments
     * @throws java.io.FileNotFoundException
     */
    public static void main(String[] args) throws FileNotFoundException, IOException {
        
        BufferedReader in = new BufferedReader(new FileReader(args[0]));
        BufferedWriter out = new BufferedWriter(new FileWriter(args[0] + "_out"));
        
        Database db = null;
        
        
        String line_read = in.readLine();
        String command;
        StringTokenizer token;
        while(line_read != null && !line_read.equals(""))
        {
            token = new StringTokenizer(line_read);
            command = token.nextToken();
            switch (command) {
                case "CREATEDB":
                    db = new Database(token.nextToken(), 
                            Integer.valueOf(token.nextToken()), 
                            Integer.valueOf(token.nextToken()));
                    break;
                case "CREATE":
                    db.create_entity(token.nextToken(), 
                            Integer.parseInt(token.nextToken()), 
                            Integer.parseInt(token.nextToken()),
                            token.nextToken(""));
                    break;
                case "INSERT":
                    db.insert_instance(token.nextToken(), token.nextToken(""));
                    break;
                case "SNAPSHOTDB":
                    db.snapshot(out);
                    break;
                case "GET":
                    db.get(token.nextToken(), token.nextToken(), out);
                    break;
                case "UPDATE":
                    db.update(token.nextToken(), token.nextToken(), 
                            token.nextToken(""));
                    break;
                case "DELETE":
                    db.delete(token.nextToken(), token.nextToken(), out);
                    break;
                case "CLEANUP":
                    db.cleanup(token.nextToken(), token.nextToken());
                    break;
                default:
                    break;
            }
            line_read = in.readLine();            
        }
        
        in.close();
        out.close();
    }
    
}
