/*
Author: Sebin Shaji Philip (sebin@kth.se)

compile:
    gcc matician_shell.c
    ./a.out 
*/

#include "matician_shell.h"


/**
 * ================    Test cases ==============
 * /bin/ls -l /usr/share
 * cat /proc/mounts
 * /bin/echo extra   spaces will    be     removed
 * echo "but   not if    they're    in    quotes"
 * "but any thing inside thy're ee"
 * '/tmp/"hello"'
 * /usr/bin/printf "The cat's name is %s.\n" 'Theodore Roosevelt'
 * /usr/bin/printf "Missing quote
 * cd /sys
 * ctrl + D - EOF
 * exit
 * head /nonexistent - should print error code on error exit
 */


/*==================State machine implementation for parsing into tokens ===========*/

char* input_string_pointer;

char* start_token_pointer;
char* end_token_pointer;
char *token_list[MAX_CHAR_LIMIT/2] = {NULL};
int token_list_index = 0;
eParseStates eNextState = Default_State;
eParseStates ePreviousState = Default_State;

eParseStates Start_DoubleQuote_Parsed_EventHandler (void)
{
#if DEBUG_STATE_MACHINE
    printf ("+++ %s +++\n", __func__);
#endif
    start_token_pointer = input_string_pointer;
    return Inside_DoubleQuotes_State;

}

eParseStates End_DoubleQuote_Parsed_EventHandler (void)
{
#if DEBUG_STATE_MACHINE
    printf ("+++ %s +++\n", __func__);
#endif
    end_token_pointer = input_string_pointer -1;
    char temp = *end_token_pointer;
    *end_token_pointer = '\0';
#if DEBUG_STATE_MACHINE
    printf ("TOKEN_DOUBLE_QUOTE:%s size:%ld\n", start_token_pointer, start_token_pointer-end_token_pointer);
#endif
    token_list[token_list_index] = strdup(start_token_pointer);
    token_list_index++;

    *end_token_pointer = temp;
    start_token_pointer = end_token_pointer = NULL;
    return Default_State;

}

eParseStates Start_SingleQuote_Parsed_EventHandler (void)
{
#if DEBUG_STATE_MACHINE
    printf ("+++ %s +++\n", __func__);
#endif
    start_token_pointer = input_string_pointer;
    return Inside_SingleQuotes_State;

}
eParseStates End_SingleQuote_Parsed_EventHandler (void)
{
#if DEBUG_STATE_MACHINE
    printf ("+++ %s +++\n", __func__);
#endif
    end_token_pointer = input_string_pointer -1;
    char temp = *end_token_pointer;
    *end_token_pointer = '\0';
#if DEBUG_STATE_MACHINE
    printf ("TOKEN_SINGLE_QUOTE:%s size:%ld\n", start_token_pointer, start_token_pointer-end_token_pointer);
#endif
    token_list[token_list_index] = strdup(start_token_pointer);
    token_list_index++;
    *end_token_pointer = temp;
    start_token_pointer = end_token_pointer = NULL;
    return Default_State;

}

eParseStates Anything_InsideSingleQuote_Parsed_EventHandler (void)
{
#if DEBUG_STATE_MACHINE
    printf ("+++ %s +++\n", __func__);
#endif
    return Inside_SingleQuotes_State;

}
eParseStates Anything_InsideDoubleQuote_Parsed_EventHandler (void)
{
#if DEBUG_STATE_MACHINE
    printf ("+++ %s +++\n", __func__);
#endif
    return Inside_DoubleQuotes_State;

}

eParseStates Space_Parsed_EventHandler (void)
{
#if DEBUG_STATE_MACHINE
    printf ("+++ %s +++\n", __func__);
#endif
    if (ePreviousState == Inside_NormalWord_State)
    {
        end_token_pointer = input_string_pointer -1;
        char temp = *end_token_pointer;
        *end_token_pointer = '\0';
#if DEBUG_STATE_MACHINE
        printf ("TOKEN_WORD_I:%s size:%ld\n", start_token_pointer, start_token_pointer-end_token_pointer);
#endif
        token_list[token_list_index] = strdup(start_token_pointer);
        token_list_index++;
        *end_token_pointer = temp;
    }
    start_token_pointer = end_token_pointer = NULL;
    return Default_State;

}
eParseStates Word_Parsed_EventHandler (void)
{
#if DEBUG_STATE_MACHINE
    printf ("+++ %s +++\n", __func__);
#endif
    start_token_pointer = input_string_pointer -1;
    return Inside_NormalWord_State;

}
eParseStates Anything_InsideNormalWord_Parsed_EventHandler (void)
{
#if DEBUG_STATE_MACHINE
    printf ("+++ %s +++\n", __func__);
#endif
    return Inside_NormalWord_State;

}

eParseStates Accept_EventHandler (void)
{
#if DEBUG_STATE_MACHINE
    printf ("+++ %s +++\n", __func__);
#endif
    if (ePreviousState == Inside_NormalWord_State)
    {
        end_token_pointer = input_string_pointer -1;
        char temp = *end_token_pointer;
        *end_token_pointer = '\0';
#if DEBUG_STATE_MACHINE
        printf ("TOKEN_WORD_II:%s size:%ld\n", start_token_pointer, start_token_pointer-end_token_pointer);
#endif
        token_list[token_list_index] = strdup(start_token_pointer);
        token_list_index++;
        *end_token_pointer = temp;
    }
    start_token_pointer = end_token_pointer = NULL;
    return Accept_State;

}

eParseStates Reject_EventHandler (void)
{
#if DEBUG_STATE_MACHINE
    printf ("+++ %s +++\n", __func__);
#endif
    fprintf (stderr, "error: mismatched quotes \n");
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
/*==================State machine implementation for parsing into tokens ===========*/

/*==================BUILT INs===========*/
int mat_builtin_cd (char **arg)
{
#if DEBUG_GENERIC
    printf ("+++ %s +++\n", __func__);
#endif
    if (NULL != arg[2])
    {
        fprintf (stderr, "error: Too many arguments\n");
    }
    else
    {
        //Have just one argument, continue changing directory
        if (0 != chdir(arg[1]))
        {
            perror ("error:");
        }
    }
    return 1;
}
int mat_builtin_exit (char **arg)
{
#if DEBUG_GENERIC
    printf ("+++ %s +++\n", __func__);
#endif

    return 0;
}

mat_builtins builtins [] = {
    {"cd", mat_builtin_cd},
    {"exit", mat_builtin_exit},
};

/*====================BUILT INs===========*/

eParseEvents read_next_letter ()
{
#if DEBUG_GENERIC
    printf ("+++ %s +++\n", __func__);
#endif

    eParseEvents event = UndefinedCode_Parsed_Event;
    if (input_string_pointer != NULL)
    {
        if ((*input_string_pointer == ' '))
        {
            event = Space_Parsed_Event;
#if DEBUG_STATE_MACHINE
            printf ("space seen \n");
#endif
        }
        else if ((*input_string_pointer == '\"'))
        {
            event = DoubleQuote_Parsed_Event;
#if DEBUG_STATE_MACHINE
            printf ("double quote seen \n");
#endif
        }
        else if ((*input_string_pointer == '\''))
        {
            event = SingleQuote_Parsed_Event;
#if DEBUG_STATE_MACHINE
            printf ("single quote seen \n");
#endif
        }
        else if ((*input_string_pointer == '\0') || (*input_string_pointer == '\n'))
        {
            event = NewLine_Parsed_Event;
#if DEBUG_STATE_MACHINE
            printf ("Newline seen \n");
#endif
        }
        else
        {
            event = NormalCharacter_Parsed_Event;
#if DEBUG_STATE_MACHINE
            printf ("normal character seen \n");
#endif
        }
    }
    else
    {
            event = NewLine_Parsed_Event;
#if DEBUG_STATE_MACHINE
            printf ("NULL detected \n");
#endif
    }

    input_string_pointer++;
    return event;
}

/*read input from stdin*/
int mat_readcommands()
{
#if DEBUG_GENERIC
    printf ("+++ %s +++\n", __func__);
#endif
    char *lineptr = NULL, *lineptrdup = NULL;
    char *trimmedptr;
    size_t linesize = 0;
    int i = 0, exit_code = 1;
    int n_chars_read = -1;
    if (-1 == (n_chars_read = getdelim(&lineptr, &linesize, '\n', stdin)))
    {
        //fprintf (stderr, "mat_shell: Failed to read the input! Exiting"); // Ctrl + D
        free (lineptr); // free() even if failed. Ref: 'man getdelim'
        if (feof(stdin)) 
        {
            //exit() on EOF
            fprintf (stderr, "error: EOF received, gracefully exiting!\n");
            exit(EXIT_SUCCESS);
        } 
        else  
        {
            perror("error:");
            exit(EXIT_FAILURE);
        }
    }
#if DEBUG_GENERIC
    fprintf (stderr, "original input(%s)->its allocated size(%lu), no. of characters read:(%d)", lineptr, linesize, n_chars_read);
#endif

    if (MAX_CHAR_LIMIT < n_chars_read)
    {
        fprintf (stderr, "error: Input exceeded 1000 characters");
        exit_code = -1;
        goto exit_label;
        
    }
    start_token_pointer = end_token_pointer = NULL;
    input_string_pointer = lineptr;
    eNextState = Default_State;
    ePreviousState = Default_State;
    eParseEvents eNewEvent = UndefinedCode_Parsed_Event;

    while (eNextState != Accept_State)
    {
        eNewEvent = read_next_letter ();
#if DEBUG_STATE_MACHINE
        printf ("eNewEvent: %d, eNextState: %d\n", eNewEvent, eNextState);
        //printf ("TRANSITION_COUNT:%ld\n", TRANSITION_COUNT);
#endif
        for (i = 0; i < TRANSITION_COUNT; i++)
        {
            if ((eNextState == asParserStateMachine[i].eParserStateMachine))
            {
#if DEBUG_STATE_MACHINE
                printf ("State: i: %d, [%d,%d]\n", i, eNextState, asParserStateMachine[i].eParserStateMachine);
#endif
                if (eNewEvent == asParserStateMachine[i].eParserStateMachineEvents)
                {
#if DEBUG_STATE_MACHINE
                    printf ("Event: [%d,%d]\n", eNewEvent, asParserStateMachine[i].eParserStateMachineEvents);
#endif
                    ePreviousState = eNextState;
                    eNextState = (*asParserStateMachine[i].pfParserStateMachineEventHandler)();
                    break;

                }
            }

        }

        if (i >= TRANSITION_COUNT)
        {
#if DEBUG_STATE_MACHINE
            printf ("REJECT\n");
#endif
            exit_code = -1;
            eNextState = Error_State;
            break;

        }
        
    }

#if DEBUG_GENERIC
    printf ("Tokens:\n");
    for (int idx= 0 ; idx < token_list_index; idx++)
    {
        printf ("%s\n", token_list[idx]);
    }
#endif

exit_label:

    if (NULL != lineptr)
    {
        free(lineptr);
    }
    return exit_code;
}

void clear_token_list ()
{
#if DEBUG_GENERIC
    printf ("+++ %s +++\n", __func__);
#endif

    for (int idx= 0 ; idx < token_list_index; idx++)
    {
        if (NULL != token_list[idx])
        {
            free (token_list[idx]);
            token_list[idx] = NULL;
        }
        
    }
    token_list_index = 0;

}

int mat_execute_command(char **args)
{
#if DEBUG_GENERIC
    printf("+++ %s +++\n", __func__);
#endif
    pid_t pid, wpid;
    int status, counter = 0;

#if DEBUG_GENERIC
    if (NULL == token_list[0])
    {
        fprintf(stderr, "NULL = token_list[0] detected \n");
    }
#endif
    if (0 == token_list_index)
    {
#if DEBUG_GENERIC /*Do nothing , because only spaces and/or newline detected*/
        fprintf(stderr, "0 = token_list_index detected \n");
#endif
    }
    else if (token_list_index > 100)
    {
        fprintf (stderr, "error: more than 100 arguments detected! \n");
    }
    else
    {
        while (builtins[counter].command_name != NULL && args[0] != NULL)
        {
            if (strcmp(args[0], builtins[counter].command_name) == 0)
            {
                return (*builtins[counter].func)(args);
            }
            counter++;
        }

        pid = fork();
        if (pid == 0)
        {
            /* Forked child process */

            if (execvp(args[0], args) == -1)
            {
                perror("error:");
            }
            exit(EXIT_FAILURE);
        }
        else if (pid < 0)
        {
            /* Forking failed */
            perror("error:");
        }
        else /*Parent process */
        {
            do
            {
                wpid = waitpid(pid, &status, WUNTRACED);
                if (WIFSIGNALED(status))
                {
                    fprintf(stderr, "error: command signalled with code %d \n", WTERMSIG(status));
                }
                else if (WIFSTOPPED(status))
                {
                    fprintf(stderr, "error: command stopped with code %d \n", WSTOPSIG(status));
                }
                else if (WIFEXITED(status))
                {
                    if (0 != WEXITSTATUS(status))
                    {
                        fprintf(stderr, "error: command exited with code %d \n", WEXITSTATUS(status));
                    }
                }
                else
                {
                    fprintf(stderr, "error: unrecognised waitpid() status:%d \n", status);
                }
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }
    }

    return 1;
}

int main()
{
    int return_val = 1, valid_command = -1;
    while (return_val)
    {
        fprintf (stderr, "$");
        start_token_pointer = end_token_pointer = NULL;
        valid_command = mat_readcommands ();
        if (1 == valid_command)
        {
            return_val = mat_execute_command (token_list);
        }
        clear_token_list();
    }
    return EXIT_SUCCESS;
}