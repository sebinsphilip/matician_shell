#ifndef MATICIAN_SHELL_H
#define MATICIAN_SHELL_H


#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define DEBUG 0 // Keep DEBUG as 0 to improve speed!
#define  MAX_CHAR_LIMIT 1000


/*==================STATE MACHINE===========*/
typedef enum {
    Default_State,
    Inside_DoubleQuotes_State,
    Inside_SingleQuotes_State,
    Inside_NormalWord_State,
    Error_State,
    Accept_State,
} eParseStates;

typedef enum {
    DoubleQuote_Parsed_Event,
    SingleQuote_Parsed_Event,
    NormalCharacter_Parsed_Event,
    Space_Parsed_Event,
    UndefinedCode_Parsed_Event,
    EndOfString_Parsed_Event,
    NewLine_Parsed_Event,
    Last_Event
} eParseEvents;

typedef eParseStates (*pfEventHandler) (void);

typedef struct {
    eParseStates eParserStateMachine;
    eParseEvents eParserStateMachineEvents;
    pfEventHandler pfParserStateMachineEventHandler;
} sParserStateMachine;


/*  Event handler function definitions */
eParseStates Start_DoubleQuote_Parsed_EventHandler (void);
eParseStates End_DoubleQuote_Parsed_EventHandler (void);

eParseStates Start_SingleQuote_Parsed_EventHandler (void);
eParseStates End_SingleQuote_Parsed_EventHandler (void);

eParseStates Anything_InsideSingleQuote_Parsed_EventHandler (void);
eParseStates Anything_InsideDoubleQuote_Parsed_EventHandler (void);

eParseStates Space_Parsed_EventHandler (void);
eParseStates Word_Parsed_EventHandler (void);
/*==================BUILT INs===========*/
typedef struct mat_builtins
{
    char *command_name;
    int (*func)(char **);
} mat_builtins;

int mat_builtin_cd (char **);
int mat_builtin_exit (char **);

/*==================BUILT INs===========*/

#endif //MATICIAN_SHELL_H