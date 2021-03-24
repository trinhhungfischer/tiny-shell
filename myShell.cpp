#include <windows.h>
#include <bits/stdc++.h>
#include <string.h>
#include <ctime>
#include <tchar.h>
using namespace std;

#define LSH_RL_BUFFSIZE 1024
#define LSH_TOK_BUFSIZE 64
#define NUM_OF_CMD 12
#define LSH_TOK_DELIM " \n\r\t"

bool status = true;

struct processStruct
{
    /* data */
    int id;
    char *cmdName;
    int status;
    PROCESS_INFORMATION pi;
};

// This is global variable to store ID, and process Info
processStruct listProcess[64];
processStruct foregroundProcess;
int ID = 0;

void lsh_loop();
char * lsh_read_line();
char ** lsh_split_line(char *line);
int execute_line(char **argv);
void removeProcessFromList(int id);

int startCmd(char **argv);
int helpCmd(char **argv);
int timeCmd(char **argv);
int helpCmd(char **argv);
int exitCmd(char **argv);
int listCmd(char **argv);
int dirCmd(char **argv);
int stopCmd(char **argv);
int resumeCmd(char **argv);
int pathCmd(char **argv);
int addpathCmd(char **argv);
int dateCmd(char **argv);
int killCmd(char **argv);

int (*ptr_func[])(char **) = {
    &exitCmd,
    &helpCmd,
    &startCmd,
    &dateCmd,
    &timeCmd,
    &dirCmd,
    &listCmd,
    &stopCmd,
    &resumeCmd,
    &pathCmd,
    &addpathCmd,
    &killCmd 
};

const char * listLsh[] = {"exit", 
                        "help", 
                        "start",
                        "date", 
                        "time", 
                        "dir", 
                        "list", 
                        "stop", 
                        "resume", 
                        "path", 
                        "addpath",
                        "kill"};

const char *listInstruction[] = {"Write instruction here", 
                            "Write instruction here", 
                            "Write instruction here", 
                            "Write instruction here", 
                            "Write instruction here", 
                            "Write instruction here", 
                            "Write instruction here", 
                            "Write instruction here", 
                            "Write instruction here", 
                            "Write instruction here", 
                            "Write instruction here", 
                            "Write instruction here",
                            "Write instruction here"};

int main(int argc, char const *argv[])
{
    lsh_loop();
    
    return EXIT_SUCCESS;
}

void lsh_loop()
{
    char* line;
    char **args;

    
    do {
        cout << "myShell> ";
        line = lsh_read_line();
        args = lsh_split_line(line);


        int error = execute_line(args);

        free(line);
        free(args);
        
        if (error == 1)
        {
            cout << "The command end in some errors" << endl;
        }
    }
    while (status);
}

char *lsh_read_line(void)
{
    int bufsize = LSH_RL_BUFFSIZE;
    int position = 0;
    char *buffer = (char*) malloc(sizeof(char) * bufsize);
    int c;

    if (!buffer) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

        while (1) {
            // Read a character
            c = getchar();

            // If we hit EOF, replace it with a null character and return.
            if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
        buffer[position] = c;
        }
        position++;

    // If we have exceeded the buffer, reallocate.
    if (position >= bufsize) {
        bufsize += LSH_RL_BUFFSIZE;
        buffer = (char *) realloc(buffer, bufsize);
        if (!buffer) {
            fprintf(stderr, "lsh: allocation error\n");
            exit(EXIT_FAILURE);
      }
    }
  }
}
 
char **lsh_split_line(char *line)
{
    int bufsize = LSH_TOK_BUFSIZE, position = 0;
    char **tokens =(char**) malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, LSH_TOK_DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += LSH_TOK_BUFSIZE;
            tokens = (char **)realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, LSH_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

int execute_line(char **args)
{
    int i = 0;

    if (args[0] == NULL) {
    // An empty command was entered.
        return 1;
    }
    else
    {
        for (i = 0; i < NUM_OF_CMD; i++)
        {
            if (strcmp(args[0], listLsh[i]) == 0)
            {
                (*ptr_func[i])(args);
                break;
            }
        }
    }   
    return 0;
}


int helpCmd(char **argv)
{
    cout << "____________________________________________________________________" << endl;
    cout << "Team - Project 1" << endl;
    int n = sizeof(listLsh) / sizeof(char *);
 
    for (int i =0; i<n; i++)
    {
        printf("%-15s%s\n", listLsh[i], listInstruction[i]);
    }
    return 0;
}

int startCmd(char **argv)
{
    if (argv[1] == NULL) cout << "List of process this shell support" << endl;

    else
    {
        char *processName = argv[1];
        STARTUPINFO si ;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));

        si.cb = sizeof(si);
        if (argv[2] == NULL || strcmp(argv[2], "background") == 0)
        {
            listProcess[ID] = processStruct{ID, argv[1], 0, pi };
            ID ++;
            CreateProcessA(processName,NULL,NULL,NULL,FALSE,
               CREATE_NEW_CONSOLE,NULL,NULL,&si,&pi);
            
        }
        else if (strcmp(argv[2], "foreground") == 0)
        {
            CreateProcessA(processName,NULL,NULL,NULL,FALSE,
                CREATE_NEW_CONSOLE,NULL,NULL,&si,&pi);
            foregroundProcess = processStruct{0, argv[1], 0, pi};
            WaitForSingleObject(pi.hProcess, INFINITE);
            TerminateProcess(pi.hProcess, 0);
        } else
        {
            cout << "Write fail Instruction here";
        }
    }
    return 0;
}

int killCmd(char **argv)
{   
    cout << "Kill" << endl;
    int i = (int) atoi(argv[1]);
    
    cout << i << endl; 
    TerminateProcess(listProcess[i - 1].pi.hProcess, 0);

    CloseHandle(listProcess[i - 1].pi.hProcess);
    CloseHandle(listProcess[i - 1].pi.hThread);
    // // Block of code to try
    // }
    // catch (int exception) {
    //     // Block of code to handle errors
    //     if (exception == 1)
    //         cout << "Invalid concept of kill command. Please input 'kill processId' ";
    //     else if (exception == 2) cout << "Invalid Process ID. Please input 'list' to get process ID";
    // }
    return 0;
 
}

int timeCmd(char **argv)
{
    int seconds, minutes, hours;
    string str;
  
    //storing total seconds
    time_t total_seconds=time(0);
  
    //getting values of seconds, minutes and hours
    struct tm* ct=localtime(&total_seconds);
  
    seconds=ct->tm_sec;
    minutes=ct->tm_min;
    hours=ct->tm_hour;
    
    //converting it into 12 hour format
    if(hours>=12)
      str="PM";
    else
      str="AM";
    hours=hours>12?hours-12:hours;  
    
    
    //printing the result
    cout<< (hours<10?"0":"") << hours <<":" << (minutes<10?"0":"") << minutes << ":" << (seconds<10?"0":"") << seconds << " " << str <<endl;
    return 0;
}

int exitCmd(char **argv)
{
    cout << "Exit" << endl;
    status = false;
    return 0;
}

int listCmd(char **argv)
{
    for (int i = 0; i< ID; i++)
    {
        const char *status = (listProcess[i].status == 0)?"running":"stop";
        printf("%-9d%-16s%s", listProcess[i].id + 1, listProcess[i].cmdName, status);
        cout << endl;
    }
    return 0;
}

int dirCmd(char **argv)
{
    cout << "Dir" << endl;
    return 0;
}

int stopCmd(char **argv)
{
    cout << "Stop" << endl;
    return 0;
}

int resumeCmd(char **argv)
{

    cout << "Resume" << endl;
    return 0;
}

int pathCmd(char **argv)
{
    cout << "Path" << endl;
    return 0;

}

int addpathCmd(char **argv)
{
    cout << "Add path" << endl;
    return 0;
}

int dateCmd(char **argv)
{

    string Month[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
	string wDay[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
	//storing total seconds
   	time_t total_seconds=time(0);
  
 	struct tm* ct=localtime(&total_seconds);
 		
    cout<< wDay[ct->tm_wday] << ", " << Month[ct->tm_mon] << " " << ct->tm_mday << ", " << ct->tm_year + 1900<<endl;
    return 0;
}

void removeProcessFromList(int Id)
{

}