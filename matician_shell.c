/*

Author: Sebin Shaji Philip (sebin@kth.se)

compile:
    gcc matician_shell.c
    ./a.out 
*/


#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define DEBUG 1 // Keep DEBUG as 0 to improve speed!

/*read input from stdin*/
void mat_readcommands()
{
    char *lineptr = NULL, *lineptrdup = NULL; // let getdelim() allocate enough memory on its own
    char *trimmedptr;
    size_t linesize = 0;
    int i = 0;
    int n_chars_read = -1;
    if (-1 == (n_chars_read = getdelim(&lineptr, &linesize, '\n', stdin)))
    {
#if DEBUG
        fprintf (stderr, "Failed to read the input! Exiting");
#endif
        if (feof(stdin)) 
        {
            //exit() on EOF
            exit(EXIT_SUCCESS);
        } 
        else  
        {
            perror("getdelim()");
            exit(EXIT_FAILURE);
        }
    }
#if DEBUG
    fprintf (stderr, "original input(%s)->its allocated size(%lu), no. of characters read:(%d)", lineptr, linesize, n_chars_read);
#endif
    
    /* trim trailing spaces */
    trimmedptr = lineptr;
    while (lineptr[i] == ' ' && lineptr[i]!= '\0') 
    {
        i++;
        trimmedptr++;
    }
#if DEBUG
    fprintf (stderr, " Trailer Trimmed string(%s)", trimmedptr);
#endif
    /* trim leading spaces */
    lineptrdup = lineptr;
    i = n_chars_read -2; //last position of string in *lineptr
    while (lineptrdup[i] == ' ' && &(lineptrdup[i]) != trimmedptr) 
    {
        //printf ("i:%d", i);
        i--;
    }
    lineptrdup[i+1] = '\0';
    lineptrdup = trimmedptr;
#if DEBUG
    fprintf (stderr, " Leading space Trimmed string(%s)", lineptrdup);
#endif

exit:
    free(lineptr);

}

int main()
{
    while (1)
    {
        fprintf (stderr, "$");
        mat_readcommands ();


    }
    return EXIT_SUCCESS;
}