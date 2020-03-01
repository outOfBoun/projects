import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;

/**
 * Main entry point into the program.
 * @author VOICU Alex-Georgian
 */
public class Main {

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        
        FileReader fin;
        FileWriter fout, ferr;
        try {
            fin = new FileReader(args[0]);
        } catch (FileNotFoundException ex) {
            System.out.println("Input file not found!");
            return;
        }
        try {
            fout = new FileWriter(args[1]);
        }
        catch (IOException ex) {
            System.out.println("Output file not found!");
            return;
        }
        try {
            ferr = new FileWriter(args[2]);
        }
        catch (IOException ex) {
            System.out.println("Error file not found!");
            return;
        }
        BufferedReader in = new BufferedReader(fin);
        BufferedWriter out = new BufferedWriter(fout);
        BufferedWriter err = new BufferedWriter(ferr);
        
        //creating a factory of commands for our filesystem
        CommandsFactory factory = new CommandsFactory(FileSystem.getINSTANCE());
        int counter = 1;
        Command cmd;
        try{
            String line = in.readLine();
            while(line != null)
            {
                out.write(counter + "\n");
                err.write(counter + "\n");
                counter++;
                //get the command from the factory and execute it
                cmd = factory.getCommand(line, out, err);
                if(cmd != null)
                    cmd.execute();
                line = in.readLine();
            }
        }
        catch(IOException ex){
            System.out.println("Something went wrong!");
            return;
        }
        
        try {
            in.close();
            out.close();
            err.close();
        } catch (IOException ex) {
            System.out.println("Couldn't close file streams, data might be "
                    + "incomplete!");
        }
    }
}
