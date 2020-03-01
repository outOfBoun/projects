import java.io.BufferedWriter;
import java.io.IOException;
import java.util.LinkedList;
import java.util.List;
import java.util.StringTokenizer;

/**
 * An object that recreates the functionality of a file system.
 * @author VOICU Alex-Georgian
 */
public class FileSystem {

    //Singleton pattern
    private static final FileSystem INSTANCE = new FileSystem();

    private FileSystem() {
        //instantiating the root as a directory with no parent
        this.root = new Directory("/", null);
        this.current_dir = this.root;
    }

    /**
     * Returns the instance of the file system.
     * @return
     */
    public static FileSystem getINSTANCE() {
        return INSTANCE;
    }
    
    //Data types for Composite pattern
    private abstract class AbstractFile {

        final String name;
        final boolean directory;
        AbstractFile parent;

        public AbstractFile(String name, boolean directory, AbstractFile parent) {
            this.name = name;
            this.directory = directory;
            this.parent = parent;
        }
    }

    private class File extends AbstractFile {

        public File(String name, AbstractFile parent) {
            super(name, false, parent);
        }
    }

    private class Directory extends AbstractFile {

        final List<AbstractFile> files;

        public Directory(String name, AbstractFile parent) {
            super(name, true, parent);
            files = new LinkedList<>();
        }
        
        public void insertFileOrdered(AbstractFile toIns)
        {
            if(this.files.isEmpty() || 
                    this.files.get(0).name.compareTo(toIns.name) > 0) 
                this.files.add(0, toIns);
            else if(this.files.get(this.files.size() - 1).name.compareTo(toIns.name) < 0) 
                this.files.add(toIns);
            else 
                for (int i = 0; i < this.files.size(); i++) 
                {
                    if (this.files.get(i).name.compareTo(toIns.name) > 0) {
                        this.files.add(i, toIns);
                        break;
                    }
                }
        }
    }

    //Defining the actual file system
    private final Directory root;
    private AbstractFile current_dir;
    
    //finds the absolute path of a file
    private String getAbsolutePathFromFile(AbstractFile file) 
    {
        List<String> path_list = new LinkedList<>();
        while(file != root)
        {
            path_list.add(0, file.name);
            file = file.parent;
        }
        StringBuilder apath = new StringBuilder();
        for(String token : path_list)
        {
            apath = apath.append("/");
            apath = apath.append(token);
        }
        return apath.toString();
    }
    
    //transforms a relative string path into an absolute string path 
    private String getAbsolutePathFromPath(String path) 
    {
        if(path == null)
            return null;
         
        if(path.equals("") || (path.length() == 1 && path.charAt(0) == '/'))
            return ""; //root directory
        
        //if it is an already absolute path (does not contain . or ..)
        if(path.charAt(0) == '/' && !path.contains(".")) 
            return path;
        
        Directory cr;
        //if it is a relative path but starts at root
        if(path.charAt(0) == '/' && path.contains(".")) 
            cr = (Directory)this.root;
        else //relative path starting at current dir
            cr = (Directory)this.current_dir;
        
        StringTokenizer relpath = new StringTokenizer(path, "/");
        String next;
        boolean found;
        while(relpath.hasMoreTokens())
        {
            next = relpath.nextToken();
            if(next.equals("."))
                continue;
            else if(next.equals(".."))
            {
                if(cr == this.root)
                    return null;
                cr = (Directory)cr.parent;
            }
            else
            {
                found = false;
                for(AbstractFile file : cr.files)
                {
                    if(file.name.equals(next))
                    {
                        found = true;
                        if(file.directory == false)
                        {
                            if(relpath.hasMoreTokens())
                                return null;
                            else
                                return getAbsolutePathFromFile(file);
                        }
                        else 
                        {
                            cr = (Directory)file;
                            break;
                        }
                    }
                }
                if(found == false)
                    return null;
            }
        }
        return getAbsolutePathFromFile(cr);
    }
    
    private Directory getDirectoryFromAbsolutePath(String path)
    {
        if(path == null)
            return null;
        Directory dir = root;
        StringTokenizer tokens = new StringTokenizer(path, "/");
        String nextdir;
        boolean found = true;
        while(tokens.hasMoreTokens())
        {
            nextdir = tokens.nextToken();
            found = false;
            for(AbstractFile file : dir.files)
            {
                if(file.name.equals(nextdir) && file.directory == true)
                {
                    dir = (Directory)file;
                    found = true;
                    break;
                }
            }
            if(found == false)
                break;
        }
        if(found == false)
            return null;
        return dir;
    }
    
    private AbstractFile getDirOrFileFromAbsolutePath(String path)
    {
        if(path == null)
            return null;
        Directory dir = root;
        StringTokenizer tokens = new StringTokenizer(path, "/");
        String nextdir;
        boolean found = true;
        while(tokens.hasMoreTokens())
        {
            nextdir = tokens.nextToken();
            found = false;
            for(AbstractFile file : dir.files)
            {
                if(file.name.equals(nextdir))
                {
                    if(file.directory == true)
                    {
                        dir = (Directory)file;
                        found = true;
                        break;
                    }
                    else if(!tokens.hasMoreTokens())
                        return file;     
                }
            }
            if(found == false)
                break;
        }
        if(found == false)
            return null;
        return dir;
    }
    
    private void listrec(String path_to_here, Directory cr_dir, 
            BufferedWriter out, BufferedWriter err) throws IOException
    {
        out.write(path_to_here + ":\n");
        
        //printing all files inside the directory
        if(!cr_dir.files.isEmpty())
            out.write(path_to_here + "/" + cr_dir.files.get(0).name);
        for (int i = 1; i < cr_dir.files.size(); i++) 
        {
            out.write(" " + path_to_here + "/" + cr_dir.files.get(i).name);
        }
        
        out.write("\n\n");

        //for recursive option call a recursive modification of list
        for (AbstractFile file : cr_dir.files) 
        {
            if (file.directory == true) {
                listrec(path_to_here + "/" + file.name, (Directory) file,
                        out, err);
            }
        }
    }
    
    /**
     * Lists all of the files (recursively or not) found at the specified path.
     * @param path
     * @param recursive
     * @param out
     * @param err
     * @throws IOException
     */
    public void list(String path, boolean recursive, BufferedWriter out, 
            BufferedWriter err) throws IOException
    {
        //directory to perform listing upon
        Directory cr_dir;
        
        //string path up to the current directory
        String path_to_here;
        
        if(path == null)
        {
            cr_dir = (Directory)current_dir;
            path_to_here = getAbsolutePathFromFile(cr_dir);
        }
        else
        {
            //retrieving absolute path from possible relative path
            String apath = getAbsolutePathFromPath(path);
            cr_dir = getDirectoryFromAbsolutePath(apath);
            if(cr_dir == null)
            {
                err.write("ls: " + path + ": No such directory\n");
                return;
            }
            path_to_here = apath;
        }
        
        //root path exception when formatting
        if(path_to_here.equals(""))
            out.write("/:\n");
        else
            out.write(path_to_here + ":\n");
        
        //printing all files inside the directory
        if(!cr_dir.files.isEmpty())
            out.write(path_to_here + "/" + cr_dir.files.get(0).name);
        for (int i = 1; i < cr_dir.files.size(); i++) 
        {
            out.write(" " + path_to_here + "/" + cr_dir.files.get(i).name);
        }
        
        out.write("\n\n");

        //for recursive option call a recursive modification of list
        if(recursive == true)
        {
            for(AbstractFile file : cr_dir.files)
            {
                if(file.directory == true)
                    listrec(path_to_here + "/" + file.name, (Directory)file, 
                            out, err);
            }
        }
    }
    
    /**
     * Creates a directory at the specified location if there isn't an identical
     * named one already.
     * @param path
     * @param err
     * @throws IOException
     */
    public void makedir(String path, BufferedWriter err) 
            throws IOException
    {
        //saving the name of the directory to create
        String newdir_name = path.substring(path.lastIndexOf('/') + 1);
        
        String apath;
        
        if(path.contains("/") == false) //only contains the name of the file
            apath = this.getAbsolutePathFromFile(this.current_dir);
        else
            //truncating the path to remove the last element and transforming 
            //into an absolute path
            apath = getAbsolutePathFromPath(
                path.substring(0, path.lastIndexOf('/')));
        
        if(apath == null)
        {
            err.write("mkdir: " + path.substring(0, path.lastIndexOf('/')) +
                    ": No such directory\n");
            return;
        }
        
        if(apath.equals("") && newdir_name.equals(""))
        {
            err.write("mkdir: cannot create directory /: Node exists\n");
            return;
        } 
        
        //finding the path to the parent
        Directory cr;
        cr = getDirectoryFromAbsolutePath(apath);
        //if the parent path is invalid
        if(cr == null)
        {
            err.write("mkdir: " + path.substring(0, path.lastIndexOf('/')) +
                    ": No such directory\n");
            return;
        }
        
        //searching for duplicate file name
        AbstractFile dup = null;
        for (AbstractFile file : cr.files) 
        {
            if (file.name.equals(newdir_name)) {
                dup = file;
                break;
            }
        }
        //if the file is a duplicate
        if(dup != null)
        {    
            err.write("mkdir: cannot create directory " + apath + "/" 
                    + newdir_name + ": Node exists\n");
            return;
        }
        
        //creating the new directory in the ordered list and insert it
        Directory newdir = new Directory(newdir_name, cr);
        cr.insertFileOrdered(newdir);
    }
    
    /**
     * Creates a file at the specified location if there isn't an identical
     * named one already.
     * @param path
     * @param err
     * @throws IOException
     */
    public void touchfile(String path, BufferedWriter err) 
            throws IOException
    {
        //saving the name of the file to create
        String newfile_name = path.substring(path.lastIndexOf('/') + 1);
        
        String apath;
        
        if(path.contains("/") == false) //only contains the name of the file
            apath = this.getAbsolutePathFromFile(this.current_dir);
        else
            //truncating the path to remove the last element and transforming 
            //into an absolute path
            apath = getAbsolutePathFromPath(
                path.substring(0, path.lastIndexOf('/')));
        
        if(apath == null)
        {
            err.write("touch: " + path.substring(0, path.lastIndexOf('/')) +
                    ": No such directory\n");
            return;
        }
        
        if(apath.equals("") && newfile_name.equals(""))
        {
            err.write("touch: cannot create file /: Node exists\n");
            return;
        } 
        
        //finding the path to the parent
        Directory cr;
        cr = getDirectoryFromAbsolutePath(apath);
        //if the parent path is invalid
        if(cr == null)
        {
            err.write("touch: " + path.substring(0, path.lastIndexOf('/')) +
                    ": No such directory\n");
            return;
        }
        
        //searching for duplicate file name
        AbstractFile dup = null;
        for (AbstractFile file : cr.files) 
        {
            if (file.name.equals(newfile_name)) {
                dup = file;
                break;
            }
        }
        //if the file is a duplicate
        if(dup != null)
        {    
            err.write("touch: cannot create file " + apath + "/" 
                    + newfile_name + ": Node exists\n");
            return;
        }
        
        //creating the new file in the ordered list and insert it
        File newfile = new File(newfile_name, cr);
        cr.insertFileOrdered(newfile);
    }
    
    /**
     * Changes the current working directory to the specified location.
     * @param path
     * @param err
     * @throws IOException
     */
    public void changedir(String path, BufferedWriter err) 
            throws IOException
    {
        String apath = this.getAbsolutePathFromPath(path);
        Directory dir = this.getDirectoryFromAbsolutePath(apath);

        if(dir == null)
        {
            err.write("cd: " + path + ": No such directory\n");
            return;
        }
        this.current_dir = dir;
    }
    
    /**
     * Prints the location of the current working directory.
     * @param out
     * @throws IOException
     */
    public void printworkingdir(BufferedWriter out) 
            throws IOException
    {
        if(this.current_dir == this.root)
            out.write("/\n");
        else    
            out.write(this.getAbsolutePathFromFile(this.current_dir) + "\n");
    }
    
    private void copyrec(String path, Directory dir, BufferedWriter err) 
            throws IOException
    {
        for(AbstractFile file : dir.files)
        {
            if (file.directory == false) 
            {
                this.touchfile(path + "/" + file.name, err);
            } 
            else 
            {
                this.makedir(path + "/" + file.name, err);
                this.copyrec(path + "/" + file.name, (Directory)file, err);
            }
        }
    }
    
    /**
     * Copies the files between the specified locations.
     * @param spath
     * @param dpath
     * @param err
     * @throws IOException
     */
    public void copy(String spath, String dpath, BufferedWriter err) 
            throws IOException
    {
        //source absolute path retrieval
        String sapath;
        AbstractFile source_fd;
        
        sapath = getAbsolutePathFromPath(spath);
        source_fd = this.getDirOrFileFromAbsolutePath(sapath);
        
        if(source_fd == null)
        {
            err.write("cp: cannot copy " + spath + ": No such file or "
                    + "directory\n");
            return;
        }
        
        //destination absolute path retrieval
        String dapath = getAbsolutePathFromPath(dpath);
        Directory dest_dir = this.getDirectoryFromAbsolutePath(dapath);
        
        if(dest_dir == null)
        {
            err.write("cp: cannot copy into " + dpath + ": No such directory\n");
            return;
        }
        
        String source_name = sapath.substring(sapath.lastIndexOf("/") + 1);
        for(AbstractFile file : dest_dir.files)
        {
            if(file.name.equals(source_name))
            {
                err.write("cp: cannot copy " + spath + ": Node exists at "
                        + "destination\n");
                return;
            }
        }
        
        if(source_fd.directory == false)
        {
            this.touchfile(dapath + "/" + source_name, err);
        }
        else
        {
            this.makedir(dapath + "/" + source_name, err);
            this.copyrec(dapath + "/" + source_name, (Directory)source_fd, err);
        }
    }
    
    /**
     * Moves the files between the specified locations.
     * @param spath
     * @param dpath
     * @param err
     * @throws IOException
     */
    public void move(String spath, String dpath, BufferedWriter err) 
            throws IOException
    {
        //source absolute path retrieval
        String sapath;
        AbstractFile source_fd;
        
        sapath = getAbsolutePathFromPath(spath);
        source_fd = this.getDirOrFileFromAbsolutePath(sapath);
        
        if(source_fd == null)
        {
            err.write("mv: cannot move " + spath + ": No such file or "
                    + "directory\n");
            return;
        }
        
        //destination absolute path retrieval
        String dapath = getAbsolutePathFromPath(dpath);
        Directory dest_dir = this.getDirectoryFromAbsolutePath(dapath);
        
        if(dest_dir == null)
        {
            err.write("mv: cannot move into " + dpath + ": No such directory\n");
            return;
        }
        
        String source_name = sapath.substring(sapath.lastIndexOf("/") + 1);
        for(AbstractFile file : dest_dir.files)
        {
            if(file.name.equals(source_name))
            {
                err.write("mv: cannot move " + spath + ": Node exists at "
                        + "destination\n");
                return;
            }
        }
        
        //remove the file from the parent's list
        ((Directory)source_fd.parent).files.remove(source_fd);
        source_fd.parent = dest_dir;
        dest_dir.insertFileOrdered(source_fd);
    }
    
    /**
     * Removes the files at the specified location.
     * @param path
     * @param err
     * @throws IOException
     */
    public void remove(String path, BufferedWriter err) throws IOException
    {
        String apath;
        AbstractFile toRemove;
        
        apath = getAbsolutePathFromPath(path);
        toRemove = this.getDirOrFileFromAbsolutePath(apath);
        
        if(toRemove == null)
        {
            err.write("rm: cannot remove " + path + ": No such file or "
                    + "directory\n");
            return;
        }

        Directory parent;
        //if the file is dir and includes current working directory then stop
        if(toRemove.directory == true)
        {
            Directory ancestor = (Directory)this.current_dir;
            while (ancestor != this.root) 
            {
                if (ancestor == toRemove) 
                    return;
                ancestor = (Directory) ancestor.parent;
            }
        }
        //remove the file from the parent's list
        parent = (Directory)toRemove.parent;
        toRemove.parent = null;
        parent.files.remove(toRemove);
    }
    
    
    //bonus functions start here
    
    private void listrecgrep(String path_to_here, String pattern, Directory cr_dir, 
            BufferedWriter out, BufferedWriter err) throws IOException
    {
        out.write(path_to_here + ":\n");
        
        //printing all files inside the directory
        boolean first_printed = false;
        for (int i = 0; i < cr_dir.files.size(); i++) 
        {
            if(cr_dir.files.get(i).name.matches(pattern))
            {
                if(first_printed == true)
                    out.write(" ");
                out.write(path_to_here + "/" + cr_dir.files.get(i).name);
                first_printed = true;
            }
        }
        
        out.write("\n\n");

        //for recursive option call a recursive modification of list
        for (AbstractFile file : cr_dir.files) 
        {
            if (file.directory == true) {
                listrecgrep(path_to_here + "/" + file.name, pattern,
                        (Directory) file, out, err);
            }
        }
    }
    
    /**
     * List the files at the specified location checking for matching regex 
     * pattern.
     * @param path
     * @param pattern
     * @param recursive
     * @param out
     * @param err
     * @throws IOException
     */
    public void listgrep(String path, String pattern, boolean recursive, 
            BufferedWriter out, BufferedWriter err) throws IOException
    {
        //directory to perform listing upon
        Directory cr_dir;
        
        //string path up to the current directory
        String path_to_here;
        
        if(path == null)
        {
            cr_dir = (Directory)current_dir;
            path_to_here = getAbsolutePathFromFile(cr_dir);
        }
        else
        {
            //retrieving absolute path from possible relative path
            String apath = getAbsolutePathFromPath(path);
            cr_dir = getDirectoryFromAbsolutePath(apath);
            if(cr_dir == null)
            {
                err.write("ls: " + path + ": No such directory\n");
                return;
            }
            path_to_here = apath;
        }
        
        //root path exception when formatting
        if(path_to_here.equals(""))
            out.write("/:\n");
        else
            out.write(path_to_here + ":\n");
        
        //printing all files inside the directory that match
        boolean first_printed = false;
        for (int i = 0; i < cr_dir.files.size(); i++) 
        {
            if(cr_dir.files.get(i).name.matches(pattern))
            {
                if(first_printed == true)
                    out.write(" ");
                out.write(path_to_here + "/" + cr_dir.files.get(i).name);
                first_printed = true;
            }
        }
        
        out.write("\n\n");

        //for recursive option call a recursive modification of list
        if(recursive == true)
        {
            for(AbstractFile file : cr_dir.files)
            {
                if(file.directory == true)
                    listrecgrep(path_to_here + "/" + file.name, pattern, 
                            (Directory)file, out, err);
            }
        }
    }
    
    private boolean checkValidTokenStar(String toCheck, String pattern)
    {
        if(pattern == null || pattern.equals("*"))
            return true;
        
        if(!pattern.contains("*"))
            return toCheck.equals(pattern);
        
        //check if the ending is the same
        if(pattern.charAt(0) == '*')
            if(toCheck.length() - pattern.length() + 1 >= 0 && 
                    toCheck.length() - pattern.length() + 1 <= toCheck.length())
                return toCheck.substring(toCheck.length() - pattern.length() + 1)
                        .equals(pattern.substring(1));
            else 
                return false;
        
        //check if the beginning is the same
        if(pattern.charAt(pattern.length() - 1) == '*')
            if(pattern.length() - 1 >= 0 && 
                    pattern.length() - 1 <= toCheck.length())
                return toCheck.substring(0, pattern.length() - 1)
                        .equals(pattern.substring(0, pattern.length() - 1));
            else 
                return false;
        
        //if the wildcard star is in the middle
        String head = pattern.substring(0, pattern.indexOf("*"));
        String tail = pattern.substring(pattern.indexOf("*") + 1);
        
        if(toCheck.length() - tail.length() <= toCheck.length() && 
                toCheck.length() - tail.length() >=0)
            return toCheck.substring(toCheck.length() - tail.length()).equals(tail) 
                &&
                toCheck.substring(0, head.length()).equals(head);
        else 
            return false;
    }
    
    private void getAbsolutePathsFromStarPathRec(String []relpath, int pos, 
            int max, Directory cr_dir, List<String> paths)
    {
        if(pos >= max)
        {
            paths.add(getAbsolutePathFromFile(cr_dir));
            return;
        }
        
        String next = null;
        boolean found = false;
        
        do 
        {
            next = relpath[pos++];
            
            if (next.equals(".")) 
            {
                continue;
            } 
            else if (next.equals("..")) 
            {
                if (cr_dir == this.root) 
                {
                    return;
                }
                cr_dir = (Directory) cr_dir.parent;
            }
            else
                break;
        }
        while(pos < max);
                
        if(pos >= max && next.contains("."))
        {
            paths.add(getAbsolutePathFromFile(cr_dir));
            return;
        }
        
        found = false;
        for(AbstractFile file : cr_dir.files) 
        {
            if(this.checkValidTokenStar(file.name, next)) 
            {
                found = true;
                if (file.directory == false) 
                {
                    if(pos < max) 
                        continue;
                    else 
                    {
                        paths.add(getAbsolutePathFromFile(file));
                    }
                } 
                else 
                {
                    this.getAbsolutePathsFromStarPathRec(relpath, pos, max, 
                            (Directory)file, paths);
                }
            }
        }
    }
    
    //transforms a star string path into an absolute string path array
    private List<String> getAbsolutePathsFromStarPath(String path) 
    {
        List<String> paths = new LinkedList<>();
        if(path == null)
            return null;
        
        //root directory
        if(path.equals("") || (path.length() == 1 && path.charAt(0) == '/'))
        {
            paths.add("");
            return paths;
        }
        
        Directory cr;
        //if it is an absolute or relative path that starts at root
        if(path.charAt(0) == '/') 
        {
            cr = (Directory)this.root;
            path = path.substring(1);
        }
        else //relative path starting at current dir
            cr = (Directory)this.current_dir;
        
        //StringTokenizer relpath = new StringTokenizer(path, "/");
        String []relpath = path.split("/");
        this.getAbsolutePathsFromStarPathRec(relpath, 0, relpath.length,
                cr, paths);
        
        return paths;
    }
    
    /**
     * Lists wildcard including paths.
     * @param path
     * @param out
     * @param err
     * @throws IOException
     */
    public void liststar(String path, BufferedWriter out, 
            BufferedWriter err) throws IOException
    {
        List<String> paths;
        paths = this.getAbsolutePathsFromStarPath(path);
        if(paths.isEmpty())
        {    
            err.write("ls: " + path + ": No such directory\n");
            return;
        }
        for(String s : paths)
        {
            if(this.getDirectoryFromAbsolutePath(s) != null)
                this.list(s, false, out, err);
        }
    }
    
    /**
     * Removes wildcard including paths.
     * @param path
     * @param err
     * @throws IOException
     */
    public void removestar(String path, BufferedWriter err) throws IOException
    {
        List<String> paths;
        paths = this.getAbsolutePathsFromStarPath(path);
        if(paths.isEmpty())
        {    
            err.write("rm: cannot remove " + path + ": No such file or "
                    + "directory\n");
            return;
        }
        for(String s : paths)
        {
            this.remove(s, err);
        }
    }
    
    /**
     * Makes directories for wildcard including paths.
     * @param path
     * @param err
     * @throws IOException
     */
    public void makedirstar(String path, BufferedWriter err) throws IOException
    {
        List<String> paths;
        String filename = path.substring(path.lastIndexOf("/") + 1);
        paths = this.getAbsolutePathsFromStarPath(path.substring(0, 
                path.lastIndexOf("/")));
        
        if(paths.isEmpty())
        {    
            err.write("mkdir: " + path.substring(0, path.lastIndexOf('/')) +
                    ": No such directory\n");
            return;
        }
        
        for(String s : paths)
        {
            if(this.getDirectoryFromAbsolutePath(s) != null)
                this.makedir(s.concat("/" + filename), err);
        }
    }
    
    /**
     * Creates files for wildcard including paths.
     * @param path
     * @param err
     * @throws IOException
     */
    public void touchfilestar(String path, BufferedWriter err) throws IOException
    {
        List<String> paths;
        String filename = path.substring(path.lastIndexOf("/") + 1);
        paths = this.getAbsolutePathsFromStarPath(path.substring(0, 
                path.lastIndexOf("/")));
        
        if(paths.isEmpty())
        {    
            err.write("touch: " + path.substring(0, path.lastIndexOf('/')) +
                    ": No such directory\n");
            return;
        }
        for(String s : paths)
        {
            this.touchfile(s.concat("/" + filename), err);
        }
    }
}
