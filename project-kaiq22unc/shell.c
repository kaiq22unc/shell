// PID: 730531208
// I pledge the COMP211 honor code.

// ----------------------------------------------
// These are the only libraries that can be
// used. Under no circumstances can additional
// libraries be included

#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "shell.h"

const char *PATH_SEPARATOR = ":";

// --------------------------------------------
// Currently only two builtin commands for this
// assignment exist
// --------------------------------------------
const char* BUILT_IN_COMMANDS[] = { "cd", "exit", NULL };


/* ------------------------------------------------------------------------------
 *
 * YOU NEED TO COMPLETE THIS FUNCTION.
 *
 * Allocate memory for the command. The last element in `p_cmd->argv` should be
 * NULL.
 *
 * The length of `p_cmd->argv` should be `argc+1`: the first `argc` slots are
 * used to store the arguments and the last one is set to NULL.
 *
 * Arguments:
 *      p_cmd : pointer to the command need to allocate memory.
 *      argc :  the number of arguments.
 *
 * Return:
 *      None
 */
void alloc_mem_for_command(command_t* p_cmd, int argc) {

    // TODO: COMPLETE YOUR CODE HERE.
   p_cmd->argv = (char**)malloc(sizeof(char*)*(argc+1));
   for(int i = 0; i < argc + 1; i++)
      p_cmd->argv[i] = (char*)malloc(MAX_ARG_LEN*sizeof(char)); 
   //p_cmd->argv = argv;
   p_cmd->argc = argc;
   //p_cmd = malloc(sizeof(**argv) + sizeof(int));

} // end alloc_mem_for_command function

/* ------------------------------------------------------------------------------
 *
 * YOU NEED TO COMPLETE THIS FUNCTION.
 *
 * This function is used to free memory that may be malloc'd for the name and
 * argv fields in the command_t structure.  To be safe, you should also set the
 * pointer values to NULL.
 *
 * HINT(s): See man page for more information about free function
 *
 * Arguments:
 *         p_cmd : pointer to a command_t structure
 * Return: 
 *        None
 *
 */
void cleanup(command_t* p_cmd) {

    // TODO: COMPLETE YOUR CODE HERE.
   //printf("start freeing");
   for(int i = 0; i < p_cmd->argc + 1; i++)
       free(p_cmd->argv[i]);
   //printf("first freed");
   free(p_cmd->argv);
   //printf("second freed");
   //p_cmd = NULL;

} // end cleanup function

/* ------------------------------------------------------------------------------
 *
 * YOU NEED TO COMPLETE THIS FUNCTION.
 *
 * This function will parse the command (cmd for short) and its arguments
 * specified by the user.
 *
 * HINT(s): This function is "essentially" an exercise in string parsing.
 *
 *         EXAMPLE#1, if the user enters
 *
 *                                 cd /mnt/cdrom
 *
 *                 at the shell prompt, line would contain "cd /mnt/cdrom".
 *                 Using the space character as the delimiter, the fields in
 *                 the command struct would be:
 *
 *                                 - argc = 2
 *                                 - argv = {"cd", "/mnt/cdrom", NULL}
 *
 *         EXAMPLE#2, if the user enters
 *
 *                                 ls -la
 *
 *                 at the shell prompt, line would contain "ls -la". Using the
 *                 space character as the delimiter, the fields in the command
 *                 struct would be:
 *
 *                                 - argc = 2
 *                                 - argv = {"ls", "-la", NULL}
 *
 *         EXAMPLE#3, if the user enters nothing at the shell prompt (i.e.
 *         simply hits the return key), line would NULL, and the fields in
 *         the command struct would be:
 *
 *                                 - argc = 0
 *                                 - argv = {NULL}
 *
 *  Arguments:
 *      line: pointer to the string containing the cmd.
 *      p_cmd: pointer to the command_t structure
 *
 *  Return:
 *      N/A
 *
 */
void parse(char* line, command_t* p_cmd) {

    // TODO: COMPLETE YOUR CODE HERE.
    int i = 0;
    char* temp2 = malloc(sizeof(char)*MAX_ARG_LEN);
    strcpy(temp2, line);
    //printf("t is %s \n", t);
    char* str = strtok(temp2, " ");
    while(str!=NULL){
        i++;
        str=strtok(NULL, " ");
    }
    alloc_mem_for_command(p_cmd, i);
    p_cmd->argc = i;
    int index = 0;
    char* temp = malloc(sizeof(char)*MAX_ARG_LEN);
    strcpy(temp, line);
    //printf("temp is %s \n", temp);
    char* arg = strtok(temp, " ");
    while(arg != NULL){
        //printf("arg is %s \n", arg);
        strcpy(p_cmd->argv[index++],arg);
        arg = strtok(NULL, " ");
    }
    //printf("array: %s \n", p_cmd->argv[0]);
    //printf("argc: %d \n", p_cmd->argc);

} // end parse function

/* ------------------------------------------------------------------------------
 
 * YOU NEED TO COMPLETE THIS FUNCTION.
 *
 * This function is used determine if the named command (cmd for short) entered
 * by the user in the shell can be found in one of the folders defined in the
 * PATH environment (env or short) variable. If the file exists, then the name
 * of the executable at argv[0] is replaced with the location (fully qualified
 * path) of the executable.
 *
 * For example,
 *
 * if command_t.argv[0] is "ls", and "ls" is in the "/usr/bin" folder,
 * then command_t.argv[0] would be changed to "/usr/bin/ls"
 *
 * HINT(s): Use getenv() system function to retrieve the folders defined in the
 *                 PATH variable. An small code segment is shown below that
 * demonstrates how to retrieve folders defined in your PATH.
 *
 *                         char* path_env_variable;
 *                         path_env_variable = getenv( "PATH" );
 *                         printf("PATH = %s\n", path_env_variable );
 *
 *                 The format of the PATH is very simple, the ':' character is
 * delimiter, i.e. used to mark the beginning and end of a folder defined in the
 * path.
 *
 *                 Write a loop that parses each folder defined in the path,
 * then uses this folder along with the stat function (see "File/Directory
 * Status" section in the assignment PDF).
 *
 *  Arguments:
 *         p_cmd: pointer to the command_t structure
 *  Return:
 *      TRUE: if cmd is in the PATH
 *      FALSE:  if not in the PATH.
 *
 */
int find_fullpath( command_t* p_cmd ) {

    // TODO: COMPLETE YOUR CODE HERE.
    char* path_env_var;
    path_env_var = getenv( "PATH" );
    //printf("%s \n", path_env_var);
    int len = strlen(path_env_var);
    int n_col = 0;
    for(int j = 0; j < len; j++)
    {
        if(path_env_var[j] == ':'){
            n_col++;
        }
    }
    char** arr = (char**)malloc(sizeof(char*)*(n_col+2));
    for(int j = 0; j < n_col+2; j++)
        arr[j] = (char*)malloc(100*sizeof(char));
    int i = 0;
    char*temp3 = malloc(sizeof(char)*1000);
    strcpy(temp3, path_env_var);
    char* path = strtok(temp3, ":");
    while(path != NULL){
        arr[i++] = path;
        path = strtok(NULL, ":");
    }
    //for(int j = 0; j < n_col+1; j++)
        //printf("PATH: %s \n",arr[j]);
    struct stat buff;
    char* first = malloc(sizeof(char)*MAX_ARG_LEN);
    strcpy(first, p_cmd->argv[0]);
    //char ch[2] = "/";
    //strcat(ch, first);
    for(int j = 0; j < n_col+1; j++){
        char* res = malloc(sizeof(char)*MAX_ARG_LEN);
        strcpy(res, arr[j]);
        strcat(res, "/");
        strcat(res, first);
        //printf("res: %s \n", res);
        //strcat(arr[j], first);
        //printf("res: %s \n", res);
        if(stat(res, &buff) == 0 && (S_IFREG & buff.st_mode))
        {
           strcpy(p_cmd->argv[0],res);
           //printf("FULLPATH: %s \n", p_cmd->argv[0]);
           return TRUE;
        } 
    }
    //printf("full path failed! \n");
    return FALSE; // temporary place holder

} // end find_fullpath function

/* ------------------------------------------------------------------------------
 *
 * YOU NEED TO COMPLETE THIS FUNCTION.
 *
 * This function will execute external commands (cmd for short).
 *
 *
 * Arguments:
 *      p_cmd: pointer to the command_t structor
 *
 * Return:
 *      SUCCESS: successfully execute the command.
 *      ERROR: error occurred.
 *
 */
int execute( command_t* p_cmd ) {

    int status = SUCCESS;
    int child_process_status;
    pid_t child_pid;

    // TODO: COMPLETE YOUR CODE HERE.
    //find_fullpath(p_cmd);
    if(is_builtin(p_cmd) == TRUE)
    {
        //printf("Is BUILDIN");
        status = do_builtin(p_cmd);
        //printf("BUILDIN DONE");
        return status;
    } else {
        child_pid = fork();
        if(child_pid  == 0){
            if(find_fullpath(p_cmd) == FALSE){
                printf("Command '%s' not found!\n", p_cmd->argv[0]);
                return -1;
            }
            p_cmd->argv[p_cmd->argc] = NULL;
            execv(p_cmd->argv[0], p_cmd->argv);
            child_process_status = SUCCESS;
            exit(SUCCESS);
        }else{
            wait(&child_process_status);
        }
        
    } 

    return status;

} // end execute function

/* ------------------------------------------------------------------------------
 *
 * This function will determine if command (cmd for short) entered in the shell
 * by the user is a valid builtin command.
 *
 * HINT(s): Use BUILT_IN_COMMANDS array defined in shell.c
 *
 * Arguments:
 *      p_cmd: pointer to the command_t structure
 * Return:
 *      TRUE:  the cmd is in array `valid_builtin_commands`.
 *      FALSE: not in array `valid_builtin_commands`.
 *
 */
int is_builtin(command_t* p_cmd) {

    int cnt = 0;

    while ( BUILT_IN_COMMANDS[cnt] != NULL ) {

        if ( strcmp( p_cmd->argv[0], BUILT_IN_COMMANDS[cnt] ) == 0 ) {
            return TRUE;
        }

        cnt++;
    }

    return FALSE;

} // end is_builtin function

/* ------------------------------------------------------------------------------
 *
 * This function is used execute built-in commands such as change directory (cd)
 *
 * HINT(s): See man page for more information about chdir function
 *
 * Arguments:
 *      p_cmd: pointer to the command_t structure
 * Return:
 *      SUCCESS: no error occurred during chdir operation.
 *      ERROR: an error occured during chdir operation.
 *
 */
int do_builtin(command_t* p_cmd) {
    // If the command was 'cd', then change directories
    // otherwise, tell the program to EXIT

    struct stat buff;
    int status = SUCCESS;

    // exit
    if (strcmp( p_cmd->argv[0], "exit") == 0) {
        exit(status);
    }

    // cd
    if (p_cmd->argc == 1) {
        // -----------------------
        // cd with no arg
        // -----------------------
        // change working directory to that
        // specified in HOME environmental
        // variable

        status = chdir(getenv("HOME"));
    } else if ( (status = stat(p_cmd->argv[1], &buff)) == 0 && (S_IFDIR & buff.st_mode) ) {
        // -----------------------
        // cd with one arg
        // -----------------------
        // only perform this operation if the requested
        // folder exists

        status = chdir(p_cmd->argv[1]);
    }

    return status;

} // end do_builtin function
