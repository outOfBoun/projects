import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Scanner;

public class ReducereHC {

    public static void main(String[] args) throws IOException {
        
        FileReader fisierIntrare;
        FileWriter fisierIesire;
        fisierIntrare = new FileReader("graph.in");
        fisierIesire = new FileWriter("bexpr.out");
        Scanner in = new Scanner(fisierIntrare);
        BufferedWriter out = new BufferedWriter(fisierIesire);
        
        int i, j, k, l;
        boolean skip_first1, skip_first2;
        int N = in.nextInt();
        int distmax = N/2 + 1;
        
        ArrayList<Integer> []listaM = new ArrayList[N + 1];
        ArrayList<Integer> []listaM_unic = new ArrayList[N + 1];
        for(i = 1; i <= N; i++)
        {
            listaM[i] = new ArrayList<>();
            listaM_unic[i] = new ArrayList<>();
        }
        
        int x, y;
        while(true) 
        {
            x = in.nextInt();
            if(x == -1)
                break;
            y = in.nextInt();
            listaM[x].add(y);
            listaM[y].add(x);
            listaM_unic[x].add(y);
        }
        
        //verificare daca exista cel putin 2 muchii pt fiecare nod altfel esec
        for(i = 1; i <= N; i++)
        {
            if(listaM[i].size() < 2)
            {
                out.write("x1-1 & ~x1-1");
                out.close();
                in.close();
                return;
            }
        }
        
        //formula pentru alegerea a doar 2 muchii pentru fiecare nod + 
        //existenta distantei pana la nod pentru n > 1
        skip_first1 = false;
        for(i = 1; i <= N; i++)
        {
            if(skip_first1 == true)
                out.write('&');
            else
                skip_first1 = true;
            
            skip_first2 = false;
            out.write('(');
            for(j = 0; j < listaM[i].size() - 1; j++)
            {    
                for(k = j + 1; k < listaM[i].size(); k++)
                {
                    if(skip_first2 == true)
                        out.write('|');
                    else
                        skip_first2 = true;
                    
                    out.write('(');
                    out.write("x" + i + "-" + listaM[i].get(j));
                    out.write('&');
                    out.write("x" + i + "-" + listaM[i].get(k));
                    for(l = 0; l < listaM[i].size(); l++)
                    {
                        if(l == j || l == k)
                            continue;
                        out.write('&');
                        out.write("~x" + i + "-" + listaM[i].get(l));
                    }
                    out.write(')');
                }
            }
            out.write(')');
            
            //formula pentru existenta a cel putin unei distante pana la nod
            if(i > 1)
            {
                out.write('&');
                skip_first2 = false;
                out.write('(');
                for(j = 1; j <= distmax; j++)
                {
                    if(skip_first2 == true)
                        out.write('|');
                    else
                        skip_first2 = true;
                    
                    out.write("a" + j + "-" + i);
                }
                out.write(')');
            }
        }
        
        out.write('&');
        
        //formula pentru echivalenta muchiilor
        skip_first1 = false;
        for(i = 1; i <= N; i++)
        {
            if(listaM_unic[i].size() == 0)
                continue;
            
            if(skip_first1 == true)
                out.write('&');
            else
                skip_first1 = true;
            
            skip_first2 = false;
            for(j = 0; j < listaM_unic[i].size(); j++)
            {
                if(skip_first2 == true)
                    out.write('&');
                else
                    skip_first2 = true;
                
                out.write("((x" + i + "-" + listaM_unic[i].get(j) + "|" + 
                        "~x" + listaM_unic[i].get(j) + "-" + i + ")");
                out.write('&');
                out.write("(~x" + i + "-" + listaM_unic[i].get(j) + "|" + 
                        "x" + listaM_unic[i].get(j) + "-" + i + "))");
            }
        }
        
        out.write('&');
        
        //formula pentru echivalenta dintre alegerea muchiilor ce se leaga de 
        //nodul 1 si distanta catre aceste noduri avand valoarea 1
        skip_first2 = false;
        for (j = 0; j < listaM[1].size(); j++) 
        {
            if (skip_first2 == true) 
                out.write('&');
            else 
                skip_first2 = true;

            out.write("((a" + 1 + "-" + listaM[1].get(j) + "|"
                    + "~x" + 1 + "-" + listaM[1].get(j) + ")");
            out.write('&');
            out.write("(~a" + 1 + "-" + listaM[1].get(j) + "|"
                    + "x" + 1 + "-" + listaM[1].get(j) + "))");
        }
        
        //nu poate exista drum de lungime 1 de la nodul 1 la el insusi
        out.write("&~a1-1");
        //nu poate exista drum de lungime 1 de la nodurile cu care nu se leaga
        //direct
        boolean gasit;
        for(i = 2; i <= N; i++)
        {
            gasit = false;
            for(Integer nod : listaM[1])
            {
                if(nod == i)
                {
                    gasit = true;
                    break;
                }
            }
            if(gasit == false)
                out.write("&~a1-" + i);
        }
        
        //formula pentru
        for(k = 2; k <= distmax; k++)
        {
            for(i = 2; i <= N; i++)
            {
                //muchiile de care se leaga ar putea avea legatura cu nodul 1
                StringBuilder buffer = new StringBuilder();
                buffer.append("((");
                skip_first2 = false;
                for(j = 0; j < listaM[i].size(); j++)
                {
                    if(listaM[i].get(j) == 1)
                        continue;
                    if (skip_first2 == true) 
                        buffer.append('|');
                    else 
                        skip_first2 = true;
                    
                    buffer.append('(');
                    buffer.append('a').append(k - 1).append('-');
                    buffer.append(listaM[i].get(j));
                    buffer.append('&');
                    buffer.append('x').append(listaM[i].get(j)).append('-');
                    buffer.append(i);
                    buffer.append(')');
                }
                buffer.append(')');
                
                //nu exista alte dist to in buffer
                buffer.append("&~(");
                skip_first2 = false;
                for(j = 1; j < k; j++)
                {
                    if (skip_first2 == true) 
                        buffer.append('|');
                    else 
                        skip_first2 = true;
                    
                    buffer.append('a').append(j).append('-').append(i);
                }
                buffer.append("))");
                
                //crearea formulei de echivalenta
                String ech = buffer.toString();
                out.write("&((");
                out.write("a" + k + "-" + i + "|~");
                out.write(ech);
                out.write(")&(");
                out.write("~a" + k + "-" + i + "|");
                out.write(ech);
                out.write("))");
            }
        }
        
        out.close();
        in.close();
    }
}
