import java.io.BufferedWriter;
import java.io.IOException;
import java.util.StringTokenizer;

/**
 * Command interfaces, command objects, command factory.
 * @author VOICU Alex-Georgian
 */

interface Command {
    void execute() throws IOException;
}

class CommandsFactory {
    
    //Factory pattern
    private final FileSystem fs;

    public CommandsFactory(FileSystem fs) {
        this.fs = fs;
    }
    
    public Command getCommand(String input, BufferedWriter output,
            BufferedWriter errors) {
        if(input == null) 
        {
            return null;
        }
        StringTokenizer tokens = new StringTokenizer(input, " ");
        String next = tokens.nextToken();
        switch (next) {
            case "ls":
                return new CommandLS(fs, tokens, output, errors);
            case "mkdir":
                return new CommandMKDIR(fs, tokens, errors);
            case "cd":
                return new CommandCD(fs, tokens, errors);
            case "pwd":
                return new CommandPWD(fs, output);
            case "touch":
                return new CommandTOUCH(fs, tokens, errors);
            case "cp":
                return new CommandCP(fs, tokens, errors);
            case "mv":
                return new CommandMV(fs, tokens, errors);
            case "rm":
                return new CommandRM(fs, tokens, errors);
            default:
                break;
        }
        return null;
    }
}

class CommandLS implements Command{

    private final FileSystem fs;
    private String path, pattern;
    private boolean recursive, grep, star;
    private final BufferedWriter output, errors;

    public CommandLS(FileSystem fs, StringTokenizer args, BufferedWriter output, 
            BufferedWriter errors) {
        this.fs = fs;
        this.output = output;
        this.errors = errors;
        this.recursive = false;
        this.grep = false;
        this.star = false;
        
        String arg;
        while(args.hasMoreTokens())
        {
            arg = args.nextToken();
            if(arg.equals("-R"))
                this.recursive = true;
            else if(arg.equals("|"))
                break;
            else
                this.path = arg;
        }
        if(args.hasMoreTokens())
        {
            arg = args.nextToken();
            if(arg.equals("grep"))
            {
                this.grep = true;
                this.pattern = args.nextToken();
                if(this.pattern.charAt(0) == '"' && 
                        this.pattern.charAt(this.pattern.length()-1) == '"')
                    this.pattern = this.pattern.substring(1, this.pattern.length()-1);
            }
        }
        if(path != null && path.contains("*"))
            this.star = true;
    }

    @Override
    public void execute() throws IOException{
        if(grep == false && star == false)
            fs.list(path, recursive, output, errors);
        else if(grep == true && star == false)
            fs.listgrep(path, pattern, recursive, output, errors);
        else if(grep == false && star == true)
            fs.liststar(path, output, errors);
    }
}

class CommandMKDIR implements Command{

    private final FileSystem fs;
    private String path;
    private final BufferedWriter errors;

    public CommandMKDIR(FileSystem fs, StringTokenizer args, 
            BufferedWriter errors) {
        this.fs = fs;
        this.errors = errors;
        this.path = args.nextToken();
    }

    @Override
    public void execute() throws IOException{
        if(!path.contains("*"))
            fs.makedir(path, errors);
        else fs.makedirstar(path, errors);
    }
}

class CommandCD implements Command{

    private final FileSystem fs;
    private String path;
    private final BufferedWriter errors;

    public CommandCD(FileSystem fs, StringTokenizer args, 
            BufferedWriter errors) {
        this.fs = fs;
        this.errors = errors;
        this.path = args.nextToken();
    }

    @Override
    public void execute() throws IOException{
        fs.changedir(path, errors);
    }
}

class CommandPWD implements Command{

    private final FileSystem fs;
    private final BufferedWriter output;

    public CommandPWD(FileSystem fs, BufferedWriter output) {
        this.fs = fs;
        this.output = output;
    }

    @Override
    public void execute() throws IOException{
        fs.printworkingdir(output);
    }
}

class CommandTOUCH implements Command{

    private final FileSystem fs;
    private String path;
    private final BufferedWriter errors;

    public CommandTOUCH(FileSystem fs, StringTokenizer args, 
            BufferedWriter errors) {
        this.fs = fs;
        this.errors = errors;
        this.path = args.nextToken();
    }

    @Override
    public void execute() throws IOException{
        if(!path.contains("*"))
            fs.touchfile(path, errors);
        else fs.touchfilestar(path, errors);
    }
}

class CommandCP implements Command{

    private final FileSystem fs;
    private String spath, dpath;
    private final BufferedWriter errors;

    public CommandCP(FileSystem fs, StringTokenizer args, 
            BufferedWriter errors) {
        this.fs = fs;
        this.errors = errors;
        this.spath = args.nextToken();
        this.dpath = args.nextToken();
    }

    @Override
    public void execute() throws IOException{
        fs.copy(spath, dpath, errors);
    }
}

class CommandMV implements Command{

    private final FileSystem fs;
    private String spath, dpath;
    private final BufferedWriter errors;

    public CommandMV(FileSystem fs, StringTokenizer args, 
            BufferedWriter errors) {
        this.fs = fs;
        this.errors = errors;
        this.spath = args.nextToken();
        this.dpath = args.nextToken();
    }

    @Override
    public void execute() throws IOException{
        fs.move(spath, dpath, errors);
    }
}

class CommandRM implements Command{

    private final FileSystem fs;
    private String path;
    private final BufferedWriter errors;

    public CommandRM(FileSystem fs, StringTokenizer args, 
            BufferedWriter errors) {
        this.fs = fs;
        this.errors = errors;
        this.path = args.nextToken();
    }

    @Override
    public void execute() throws IOException{
        if(!path.contains("*"))
            fs.remove(path, errors);
        else
            fs.removestar(path, errors);
    }
}
