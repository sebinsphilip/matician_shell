/*

Author: Sebin Shaji Philip (sebin@kth.se)

compile:
    gcc matician_shell.c
    ./a.out 
*/

#include "matician_shell.h"

char* input_string_pointer;

eParseStates Start_DoubleQuote_Parsed_EventHandler (void)
{
#if DEBUG
    printf ("+++ %s +++\n", __func__);
#endif
    return Inside_DoubleQuotes_State;

}

eParseStates End_DoubleQuote_Parsed_EventHandler (void)
{
#if DEBUG
    printf ("+++ %s +++\n", __func__);
#endif
    return Default_State;

}

eParseStates Start_SingleQuote_Parsed_EventHandler (void)
{
#if DEBUG
    printf ("+++ %s +++\n", __func__);
#endif
    return Inside_SingleQuotes_State;

}
eParseStates End_SingleQuote_Parsed_EventHandler (void)
{
#if DEBUG
    printf ("+++ %s +++\n", __func__);
#endif
    return Default_State;

}

eParseStates Anything_InsideSingleQuote_Parsed_EventHandler (void)
{
#if DEBUG
    printf ("+++ %s +++\n", __func__);
#endif
    return Inside_SingleQuotes_State;

}
eParseStates Anything_InsideDoubleQuote_Parsed_EventHandler (void)
{
#if DEBUG
    printf ("+++ %s +++\n", __func__);
#endif
    return Inside_DoubleQuotes_State;

}

eParseStates Space_Parsed_EventHandler (void)
{
#if DEBUG
    printf ("+++ %s +++\n", __func__);
#endif
    return Default_State;

}
eParseStates Word_Parsed_EventHandler (void)
{
#if DEBUG
    printf ("+++ %s +++\n", __func__);
#endif
    return Inside_NormalWord_State;

}
eParseStates Anything_InsideNormalWord_Parsed_EventHandler (void)
{
#if DEBUG
    printf ("+++ %s +++\n", __func__);
#endif
    return Inside_NormalWord_State;

}

eParseStates Accept_EventHandler (void)
{
#if DEBUG
    printf ("+++ %s +++\n", __func__);
#endif
    return Accept_State;

}

eParseStates Reject_EventHandler (void)
{
#if DEBUG
    printf ("+++ %s +++\n", __func__);
#endif
    return Error_State;

}

sParserStateMachine asParserStateMachine [] =
{
    {Default_State, Space_Parsed_Event, Space_Parsed_EventHandler},
    {Default_State, SingleQuote_Parsed_Event, Start_SingleQuote_Parsed_EventHandler},
    {Default_State, DoubleQuote_Parsed_Event, Start_DoubleQuote_Parsed_EventHandler},
    {Default_State, NormalCharacter_Parsed_Event, Word_Parsed_EventHandler},
    {Default_State, NewLine_Parsed_Event, Accept_EventHandler}, //accept

    {Inside_DoubleQuotes_State, Space_Parsed_Event, Anything_InsideDoubleQuote_Parsed_EventHandler},
    {Inside_DoubleQuotes_State, SingleQuote_Parsed_Event, Anything_InsideDoubleQuote_Parsed_EventHandler},
    {Inside_DoubleQuotes_State, DoubleQuote_Parsed_Event, End_DoubleQuote_Parsed_EventHandler},
    {Inside_DoubleQuotes_State, NormalCharacter_Parsed_Event, Anything_InsideDoubleQuote_Parsed_EventHandler},
    {Inside_DoubleQuotes_State, NewLine_Parsed_Event, Reject_EventHandler}, //Error unmatching double quote

    {Inside_SingleQuotes_State, Space_Parsed_Event, Anything_InsideSingleQuote_Parsed_EventHandler},
    {Inside_SingleQuotes_State, SingleQuote_Parsed_Event, End_SingleQuote_Parsed_EventHandler},
    {Inside_SingleQuotes_State, DoubleQuote_Parsed_Event, Anything_InsideSingleQuote_Parsed_EventHandler},
    {Inside_SingleQuotes_State, NormalCharacter_Parsed_Event, Anything_InsideSingleQuote_Parsed_EventHandler},
    {Inside_SingleQuotes_State, NewLine_Parsed_Event, Reject_EventHandler}, //Error unmatching single quote

    {Inside_NormalWord_State, Space_Parsed_Event, Space_Parsed_EventHandler},
    {Inside_NormalWord_State, SingleQuote_Parsed_Event, Anything_InsideNormalWord_Parsed_EventHandler},
    {Inside_NormalWord_State, DoubleQuote_Parsed_Event, Anything_InsideNormalWord_Parsed_EventHandler},
    {Inside_NormalWord_State, NormalCharacter_Parsed_Event, Anything_InsideNormalWord_Parsed_EventHandler},
    {Inside_NormalWord_State, NewLine_Parsed_Event, Accept_EventHandler}, //accept

};
#define TRANSITION_COUNT (sizeof(asParserStateMachine)/sizeof(*asParserStateMachine))

/*==================BUILT INs===========*/
int mat_builtin_cd (char **arg)
{
    return 0;
}
int mat_builtin_exit (char **arg)
{
    return 0;
}

mat_builtins builtins [] = {
    {"cd", mat_builtin_cd},
    {"exit", mat_builtin_exit},
};

/*====================BUILT INs===========*/

eParseEvents read_next_letter ()
{
    eParseEvents event = UndefinedCode_Parsed_Event;
    if (input_string_pointer != NULL)
    {
        if ((*input_string_pointer == ' '))
        {
            event = Space_Parsed_Event;
#if DEBUG
            printf ("space seen \n");
#endif
        }
        else if ((*input_string_pointer == '\"'))
        {
            event = DoubleQuote_Parsed_Event;
#if DEBUG
            printf ("double quote seen \n");
#endif
        }
        else if ((*input_string_pointer == '\''))
        {
            event = SingleQuote_Parsed_Event;
#if DEBUG
            printf ("single quote seen \n");
#endif
        }
        else if ((*input_string_pointer == '\0') || (*input_string_pointer == '\n'))
        {
            event = NewLine_Parsed_Event;
#if DEBUG
            printf ("Newline seen \n");
#endif
        }
        else
        {
            event = NormalCharacter_Parsed_Event;
#if DEBUG
            printf ("normal character seen \n");
#endif
        }
    }
    else
    {
            event = NewLine_Parsed_Event;
#if DEBUG
            printf ("NULL detected \n");
#endif
    }

    input_string_pointer++;
    return event;
}

/*read input from stdin*/
void mat_readcommands()
{
    char *lineptr = NULL, *lineptrdup = NULL;
    char *trimmedptr;
    size_t linesize = 0;
    int i = 0;
    int n_chars_read = -1;
    if (-1 == (n_chars_read = getdelim(&lineptr, &linesize, '\n', stdin)))
    {
#if DEBUG
        fprintf (stderr, "Failed to read the input! Exiting"); // Ctrl + D
#endif
        free (lineptr); // free() even if failed. Ref: 'man getdelim'
        if (feof(stdin)) 
        {
            //exit() on EOF
#if DEBUG
        fprintf (stderr, "EOF received, gracefully exiting");
#endif 
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

#if 0
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
#endif
    input_string_pointer = lineptr;
    eParseStates eNextState = Default_State;
    eParseEvents eNewEvent = UndefinedCode_Parsed_Event;

    while (eNextState != Accept_State)
    {
        eNewEvent = read_next_letter ();
#if DEBUG
        printf ("eNewEvent: %d, eNextState: %d\n", eNewEvent, eNextState);
        //printf ("TRANSITION_COUNT:%ld\n", TRANSITION_COUNT);
#endif
        for (i = 0; i < TRANSITION_COUNT; i++)
        {
            if ((eNextState == asParserStateMachine[i].eParserStateMachine))
            {
#if DEBUG
                printf ("State: i: %d, [%d,%d]\n", i, eNextState, asParserStateMachine[i].eParserStateMachine);
#endif
                if (eNewEvent == asParserStateMachine[i].eParserStateMachineEvents)
                {
#if DEBUG
                    printf ("Event: [%d,%d]\n", eNewEvent, asParserStateMachine[i].eParserStateMachineEvents);
#endif
                    eNextState = (*asParserStateMachine[i].pfParserStateMachineEventHandler)();
                    break;

                }
            }

        }

        if (i >= TRANSITION_COUNT)
        {
            printf ("REJECT\n");
            eNextState = Error_State;
            break;

        }
        
    }

/**
 * "but any thing inside thy're ee"
 * '/tmp/"hello"'
 * /usr/bin/printf "The cat's name is %s.\n" 'Theodore Roosevelt'
 * /usr/bin/printf "Missing quote
 * 
 */

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