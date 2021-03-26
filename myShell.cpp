#include <windows.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <ctime>
#include <tchar.h>
#include <stdio.h>
#include <signal.h>
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
void my_handler(sig_atomic_t s);
void runDotBat(char *nameFile);


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

const char *listInstruction[] = {"Quit the myShell.exe program", 
                            "Provide Help information for myShell commands", 
                            "Start new window to run process with background(default) or foreground", 
                            "Display the current date", 
                            "Display the current time", 
                            "Display a list of files and subdirectories in a directory", 
                            "List of all background proccesses", 
                            "Stop a single process with its ID (Know its ID by list command)", 
                            "Resume a suspended process with its ID (Know its ID by list command)", 
                            "Display all global environment path", 
                            "Add a variable to the global environment path", 
                            "Kill a single process with its ID (Know its ID by list command)"};

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
        return 1;
    }
    else
    {
        for (i = 0; i < NUM_OF_CMD; i++)
        {
            if (strcmp(args[0], listLsh[i]) == 0)
            {
                (*ptr_func[i])(args);
                return 0;
            }
        }
    }   
    cout << "'"<< args[0] << "' is not recognized as a command, use help command to get more information" << endl;
    return 1;
}

void my_handler(sig_atomic_t s){
    TerminateProcess(foregroundProcess.pi.hProcess, 0);

    CloseHandle(foregroundProcess.pi.hProcess);
    CloseHandle(foregroundProcess.pi.hThread);
    cout << "You use Ctrl + C to kill " << foregroundProcess.cmdName <<" foreground process" << endl;
}

int helpCmd(char **argv)
{
    cout << "_________________________________________________________________________" << endl;
    cout << "                             Project 1" << endl;
    cout << "_________________________________________________________________________" << endl;
    
    int n = sizeof(listLsh) / sizeof(char *);
 
    for (int i =0; i<n; i++)
    {
        printf("%-15s%s\n", listLsh[i], listInstruction[i]);
    }
    return 0;
}

int startCmd(char **argv)
{
    if (argv[1] == NULL) cout << "List of process this shell support is .bat and .exe" << endl;

    else
    {
        if (strstr(argv[1], ".exe") != NULL)
        {
            char *processName = argv[1];

            if (argv[2] == NULL || strcmp(argv[2], "background") == 0)
            {
                STARTUPINFO si ;
                PROCESS_INFORMATION pi;
                ZeroMemory(&si, sizeof(si));
                si.cb = sizeof(si);

                CreateProcessA(processName,NULL,NULL,NULL,FALSE,
                   CREATE_NEW_CONSOLE,NULL,NULL,&si,&pi);
                listProcess[ID] = processStruct{ID, argv[1], 0, pi};
            
                ID ++;
            
            }
            else if (strcmp(argv[2], "foreground") == 0)
            {
                STARTUPINFO si ;
                PROCESS_INFORMATION pi;
                ZeroMemory(&si, sizeof(si));
                si.cb = sizeof(si);

                CreateProcessA(processName,NULL,NULL,NULL,FALSE,
                    CREATE_NEW_CONSOLE,NULL,NULL,&si,&pi);
                foregroundProcess = processStruct{0, processName, 0, pi};
                signal (SIGINT,my_handler);
                WaitForSingleObject(pi.hProcess, INFINITE);
                TerminateProcess(pi.hProcess, 0);
            } else
            {
                cout << "Write fail Instruction here";
            }
        }
        else if (strstr(argv[1], ".bat") != NULL)
        {
            runDotBat(argv[1]);
        }
        
    }
    return 0;
}

int killCmd(char **argv)
{   
    if (argv[1] == NULL)
    {
        cout << "Please kill + 'ID_of_Process'" << endl;
        return 1;
    }
    else
    {
        int i = (int) atoi(argv[1]);
        if (i > ID)
        {
            cout << "The ID of process is not exist" << endl;
            return 1;
        }

        TerminateProcess(listProcess[i - 1].pi.hProcess, 0);

        CloseHandle(listProcess[i - 1].pi.hProcess);
        CloseHandle(listProcess[i - 1].pi.hThread);
        cout << "Kill " << listProcess[i-1].cmdName << " success\n";
    
        removeProcessFromList(i - 1);
    
    return 0;

    }
     
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
    cout << "Quit shell success" << endl;
    status = false;
    return 0;
}

int listCmd(char **argv)
{
    if (ID == 0)
    {
        cout << "There are no background proccess" << endl;
        return 0;
    }

    for (int i = 0; i< ID; i++)
    {
        DWORD dwExitCode;
        GetExitCodeProcess(listProcess[i].pi.hProcess, &dwExitCode);
        if ( dwExitCode != 259 )
        {
            removeProcessFromList(i);
        }
        else{
            const char *status = (listProcess[i].status == 0)?"running":"stop";
            printf("%-9d%-16s%s", listProcess[i].id + 1, listProcess[i].cmdName, status);
            cout << endl;
        }
    }
    return 0;
}

int dirCmd(char **argv)
{
    WIN32_FIND_DATA FindFileData;
	HANDLE hFindFile;
	SYSTEMTIME createdday;
	char str[1024];
	
    if(argv[1]==NULL) strcpy(str,"*");
	else
    {
	    int i;
	    for(i=0;argv[1][i]!='\0';i++) str[i]=argv[1][i];
	    str[i]=0;
	    strcat(str,"\\*");
    }
	LPCSTR file = str;
	double sum = 0;int countfile=0,countfolder=0;
	hFindFile = FindFirstFileA(file,&FindFileData);
	
    if(INVALID_HANDLE_VALUE == hFindFile)
    {
	    cout<< "The directory has no files";
	    return 0;
    } else 
    {
	    cout<<	"Directory" << endl;
	    wcout<< FindFileData.cFileName << "\t";

    	if ((int)FindFileData.nFileSizeLow)
        {	
            wcout << FindFileData.nFileSizeLow << " bytes" << "\t";	
            countfile ++;
            sum += (double) FindFileData.nFileSizeLow;
        } else 
        {
            countfolder++;
            cout << "\t" << "<Dir>" << "\t\t";
        }
	    FileTimeToSystemTime(&FindFileData.ftCreationTime,&createdday);
	    printf("%02d/%02d/%04d   %02d:%02d:%02d\n",createdday.wDay,createdday.wMonth,createdday.wYear,createdday.wHour,createdday.wMinute,createdday.wSecond);
    }
	while(FindNextFileA(hFindFile,&FindFileData)){
	wcout<< FindFileData.cFileName<<"\t";
	

	if ((int)FindFileData.nFileSizeLow)
    {
        wcout<< FindFileData.nFileSizeLow<<" bytes"<<"\t";
        countfile++;sum+=(double)FindFileData.nFileSizeLow;
    } else 
    {
        countfolder++;
        cout<<"\t"<<"<Dir>"<<"\t\t";
    }
	FileTimeToSystemTime(&FindFileData.ftCreationTime,&createdday);
	printf("%02d/%02d/%04d   %02d:%02d:%02d\n",createdday.wDay,createdday.wMonth,createdday.wYear,createdday.wHour,createdday.wMinute,createdday.wSecond);
    }	
	printf("\t\t%d files: %20.0lf bytes\n",countfile,sum);
	cout<<"\t\t"<<countfolder <<" Dirs"<<endl;

	FindClose(hFindFile);
	return 0;
}

int stopCmd(char **argv)
{
    if (argv[1] == NULL)
    {
        cout << "Please stop + 'ID_of_Process'" << endl;
        return 1;
    }
    else
    {
        int i = (int) atoi(argv[1]);
        if (i > ID)
        {
            cout << "The ID of process is not exist" << endl;
            return 1;
        }
    
        SuspendThread(listProcess[i - 1].pi.hThread);
        listProcess[i - 1].status = 1;

        cout << "Stop " << listProcess[i - 1].cmdName << " success\n";
        return 0;
    }
    
}

int resumeCmd(char **argv)
{
    if (argv[1] == NULL)
    {
        cout << "Please resume + 'ID_of_Process'" << endl;
        return 1;
    }
    else
    {
        int i = (int) atoi(argv[1]);
        if (i > ID)
        {
            cout << "The ID of process is not exist" << endl;
            return 1;
        }
    
        ResumeThread(listProcess[i - 1].pi.hThread);
        listProcess[i - 1].status = 0;
    
        cout << "Resume " << listProcess[i - 1].cmdName << " success\n";
        return 0;
    }
}

int pathCmd(char **argv)
{
       // cout << "Path" << endl;
    LPTSTR lpszVariable; 
    LPTCH lpvEnv; 
 
    // Get a pointer to the environment block. 
 
    lpvEnv = GetEnvironmentStrings();

    // If the returned pointer is NULL, exit.
    if (lpvEnv == NULL)
    {
        printf("GetEnvironmentStrings failed (%d)\n", GetLastError()); 
        return 0;
    }
 
    // Variable strings are separated by NULL byte, and the block is 
    // terminated by a NULL byte. 

   lpszVariable = (LPTSTR) lpvEnv;

	while(*lpszVariable!='P'&&*lpszVariable!='p')lpszVariable += lstrlen(lpszVariable) + 1;
        printf("%s\n", lpszVariable);

	FreeEnvironmentStrings(lpvEnv);
   
	return 0;
}

void SetUserVariablePath(char* value){
    HKEY hkey;
    long regOpenResult;
    const char key_name[] = "Environment";
	LPTSTR lpszVariable; 
    LPTCH lpvEnv; 
	lpvEnv = GetEnvironmentStrings();
    lpszVariable = (LPTSTR) lpvEnv;
	while(*lpszVariable!='P'&&*lpszVariable!='p')lpszVariable += lstrlen(lpszVariable) + 1;
	lpszVariable += 5;
	string strval = lpszVariable;strval+=";"+string(value);
	const char *path=(char*) strval.c_str();
	FreeEnvironmentStrings(lpvEnv);                                               //new_value path need to update 
    regOpenResult = RegOpenKeyEx(HKEY_CURRENT_USER,key_name, 0, KEY_ALL_ACCESS, &hkey);
    LPCSTR stuff = "path";                                                   //Variable Name 
    RegSetValueEx(hkey,stuff,0,REG_SZ,(BYTE*) path, strlen(path));
    RegCloseKey(hkey);
}

int addpathCmd(char **argv)
{
    SetUserVariablePath(argv[1]);
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

void runDotBat(char *fileName)
{
    ifstream file(fileName);
    if (file.is_open())
    {
        string line;
        while ( getline(file, line))
        {
            char * cstr = new char [line.length() - 1];
            strcpy(cstr, line.c_str());
            char ** args = lsh_split_line(cstr);
            execute_line(args);
        }
    }
    else{
        cout << "Don not exit " << fileName << " in this direcory";
    }
   
}

void removeProcessFromList(int Id)
{
    listProcess[Id].cmdName = listProcess[ID - 1].cmdName;
    listProcess[Id].pi = listProcess[ID - 1].pi;
    listProcess[Id].status = listProcess[ID - 1].status;
    ID --;
}