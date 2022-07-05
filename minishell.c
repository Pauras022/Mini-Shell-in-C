
#include <stdio.h>
#include <stdlib.h>     
#include <string.h>     
#include <unistd.h>     
#include <sys/wait.h>	
#include <fcntl.h>	
#include<sys/types.h>
#define READ 0
#define WRITE 1
#define BUILTIN_COMMANDS 6	// Number of builtin commands defined


 // Environment variables
char PWD[1024];		// Present Working Directory
char PATH[1024];

 // Built-in command names
 
char * builtin[] = {"cd", "exit", "help", "pwd", "echo","history"};


 /* Function:  shell_cd
 *  changes current working directory
 * args: arguments to the cd command, will consider only the first argument after the command name
 */
int shell_cd(char ** args){
	if (args[1] == NULL){
		fprintf(stderr, "minshell: one argument required\n");
	}
	else if (chdir(args[1]) < 0){
		perror("minshell");
	}
	getcwd(PWD, sizeof(PWD));	// Update present working directory
	return 1;
}

/*
 * Function:  shell_exit
 *  exits from the shell
 * return: status 0 to indicate termination
 */
int shell_exit(){
        remove("history.txt");//deletes history file
	return 0;
}

int shell_history(){
        FILE *fptr;
        fptr = fopen("history.txt", "r");
        char c;
        c = fgetc(fptr);
        while (c != EOF)
        {
        fprintf(stdout,"%c",c);
        c = fgetc(fptr);
        }
        fclose(fptr);
        return 1;
}

/*
 * Function:  shell_help
 * ---------------------
 *  prints a small description
 *
 * return: status 1 to indicate successful termination
 */
int shell_help(){
	
	printf("\n\t- help");
	printf("\n\t- exit");
	printf("\n\t- cd dir");
	printf("\n\t- pwd");
	printf("\n\t- echo [string to echo]");
	printf("\n\t- clear");
	printf("\n\t- ls [-ail] [dir1 dir2 ...]");
	printf("\n\t- cp source target (or) cp file1 [file2 ...] dir");
	printf("\n\t- mv source target (or) mv file1 [file2 ...] dir");
	printf("\n\t- rm file1 [file2 ...]");
	printf("\n\t- mkdir dir1 [dir2 ...]");
	printf("\n\t- rmdir dir1 [dir2 ...]");
	printf("\n\t- ln [-s] source target");
	printf("\n\t- cat [file1 file2 ...]");
	printf("\n\t- and many more...");
	printf("\n\n");
	printf("Other features : ");
	printf("\n\t* Input, Output and Error Redirection (<, <<, >, >>)  and commands with pipe ");
	printf("\n\n");
	return 1;
}

/*
 * Function:  shell_pwd
 *  prints the present working directory
 * return: status 1 to indicate successful termination
 */
int shell_pwd(){
	printf("%s\n", PWD);
	return 1;
}

/*
 * Function:  shell_echo
 *  displays the string provided
 * return: status 1 to indicate successful termination
 */
int shell_echo(char ** args){
	int i = 1;
	if(strcmp( args[1], "$HOME" ) == 0){
	       printf("HOME : %s", getenv("HOME"));
	}else if(strcmp( args[1], "$USER" ) == 0){
	       printf("USER : %s", getenv("USER"));
	}else if(strcmp( args[1], "$SHELL" ) == 0){
	       printf("SHELL : %s", getenv("SHELL"));
	}else if(strcmp( args[1], "$TERM" ) == 0){
	       printf("TERM : %s", getenv("TERM"));
	}else if(strcmp( args[1], "$PATH" ) == 0){
	       printf("PATH : %s", getenv("PATH"));
	}else{
	while (1){
		// End of arguments
		if (args[i] == NULL){
			break;
		}
		printf("%s ", args[i]);
		i++;
	}
	}
	printf("\n");
	return 1;
}

/*
 * Function:  split_command_line
 *  splits a commandline into tokens using strtok()
 * command: a line of command read from terminal
 * returns: an array of pointers to individual tokens
 */
char ** split_command_line(char * command){
        int position = 0;
        int no_of_tokens = 64;
        char ** tokens = malloc(sizeof(char *) * no_of_tokens);
        char delim[2] = " ";

        // Split the command line into tokens with space as delimiter
        char * token = strtok(command, delim);
        while (token != NULL){
                tokens[position] = token;
                position++;
                token = strtok(NULL, delim);
        }
        tokens[position] = NULL;
        return tokens;
}

/*
 * Function:  read_command_line
 *  reads a commandline from terminal
 * returns: a line of command read from terminal
 */
char * read_command_line(void){
        int position = 0;
        int buf_size = 1024;
        char * command = (char *)malloc(sizeof(char) * 1024);
        char c;
        FILE *fp;
        fp  = fopen ("history.txt", "a");

        // Read the command line character by character
        c = getchar();
        fputc(c, fp);
        while (c != EOF && c != '\n'){
                command[position] = c;

                // Reallocate buffer as and when needed
                if (position >= buf_size){
                        buf_size += 64;
                        command = realloc(command, buf_size);
                }

                position++;
                c = getchar();
                fputc(c, fp);
        }
        fclose(fp);
        return command;
}
 
 /*
 *Execution of built_in functions
 */
int Built_in_commands(char ** argss){
     int i=0;
     for(i = 0 ; i < BUILTIN_COMMANDS ; i++){
             
		if ( strcmp(argss[0], builtin[i]) == 0 ){
		       int ret_status;
			if(i==0){
			    ret_status=shell_cd(argss);
			}
			else if(i==1){
			    ret_status=shell_exit();
			}
			else if(i==2){
			    ret_status=shell_help();
			}
			else if(i==3){
			    ret_status=shell_pwd();
			}
			else if(i==4){
			    ret_status=shell_echo(argss);
			}
			else if(i==5){
			    ret_status=shell_history();
			}
			return ret_status;
	}
    }
    return -1;
}    	
/*
Function for execution of external commands
*/
int execArgs(char** parsed)
{
    // Forking a child
    pid_t pid = fork(); 
  
    if (pid == -1) {
        printf("Failed forking child..\n");
        return 1;
    } else if (pid == 0) {
        if (execvp(parsed[0], parsed) < 0) {
            printf("Could not execute command..\n");
        }
        exit(0);
    } else {
        // waiting for child to terminate
        wait(NULL); 
        return 1;
    }
}
/*
Function for execution of commands with pipe
*/
int execArgsPiped(char ** argss, int q)
{
    char * parsed[100];//command arguments before "|"
    char * parsedpipe[100];// command arguments after "|"
    int g=0;
    for(g=0;g<q;g++){
        parsed[g]=argss[g];
    }
    int c=0;
    g++;
    while(argss[g]!=NULL){
        parsedpipe[c]=argss[g];
        g++;
        c++;
    }    
    parsedpipe[c]=NULL;
    
    int p2c[2];//pipe parent to child
    int c2p[2];//pipe child to parent
    int i;
    int status;
    int pid;
    char buf[4096];
    memset(buf, 0, 4096);

    for(i=0; i < 2; i++){
        pipe(p2c);
        pipe(c2p);

        pid = fork();
        if(pid < 0) {
            exit (1);
        }
        if(pid == 0){ //in child
            close(1);
            dup2(c2p[1],1); // make child write to c2p pipe instead of stdout
            close(0);
            dup2(p2c[0],0); // make child read from p2c pipe instead of stdin
            close(p2c[1]);
            close(c2p[0]);
            if(i==0){
            execvp(parsed[0], parsed);
            fprintf(stderr, "Could not execute\n");
            exit(1);
            }else{
            execvp(parsedpipe[0],parsedpipe);
            fprintf(stderr, "Could not execute\n");
            exit(1);
            }
        }
        //in parent
        write(p2c[1], buf, strlen(buf)); //write the last output to child
        close(p2c[1]);
        close(c2p[1]);
        memset(buf,0,4096);
        if(read(c2p[0], buf, 4096) > 0){ //read output from child
            if(i == 1){
                printf("%s\n", buf);
            }
        }
    }
    return 1;
}    
/*
 * Function:  shell_execute
 *  determines and executes a command as a built-in command or an external command
 * args: arguments tokenized from the command line
 * return: return status of the command
 */
int sh_execute(char ** args){

	if (args[0] == NULL){	// Empty command
		return 1;
	}
	int d=1;
	while( args[d] != NULL ){
	       if ( strcmp( args[d], "|" ) == 0 ){
	            return execArgsPiped(args,d);
	       }else{
	            d++;
	       }
	}	
	// Copy the current Standard Input and Output file descriptors
	// so they can be restored after executing the current command
	int std_in, std_out, std_err;
	std_in = dup(0);
	std_out = dup(1);
	std_err = dup(2);

	// Check if redirection operators are present
	int i = 1;

	while ( args[i] != NULL ){
		if ( strcmp( args[i], "<" ) == 0 ){	// Input redirection
			int inp = open( args[i+1], O_RDONLY );
			if ( inp < 0 ){
				perror("minshell error");
				return 1;
			}

			if ( dup2(inp, 0) < 0 ){
				perror("minshell error");
				return 1;
			}
			close(inp);
			args[i] = NULL;
			args[i+1] = NULL;
			i += 2;
		}
		else if ( strcmp( args[i], "<<" ) == 0 ){	// Input redirection
			int inp = open( args[i+1], O_RDONLY );
			if ( inp < 0 ){

				perror("minshell error");
				return 1;
			}

			if ( dup2(inp, 0) < 0 ){
				perror("minshell error");
				return 1;
			}
			close(inp);
			args[i] = NULL;
			args[i+1] = NULL;
			i += 2;
		}
		else if( strcmp( args[i], ">") == 0 ){	// Output redirection

			int out = open( args[i+1], O_WRONLY | O_TRUNC | O_CREAT, 0755 );
			if ( out < 0 ){
				perror("minshell error");
				return 1;
			}

			if ( dup2(out, 1) < 0 ){
				perror("minshell error");
				return 1;
			}
			close(out);
			args[i] = NULL;
			args[i+1] = NULL;
			i += 2;
		}
		else if( strcmp( args[i], ">>") == 0 ){	// Output redirection (append)
			int out = open( args[i+1], O_WRONLY | O_APPEND | O_CREAT, 0755 );
			if ( out < 0 ){
				perror("minshell error");
				return 1;
			}

			if ( dup2(out, 1) < 0 ){
				perror("minshell error");
				return 1;

			}
			close(out);
			args[i] = NULL;
			args[i+1] = NULL;
			i += 2;
		}
		else{
			i++;
		}
	}
	
	
	// If the command is a built-in command, execute that function
	int k=Built_in_commands(args);

	// For external commands, execute a child process in a different function "execArgs()"
	
	if(k==-1){                                        //when its not a built_in function
			int ret_status = execArgs(args);
			// Restore the Standard Input and Output file descriptors
			dup2(std_in, 0);
			dup2(std_out, 1);
			dup2(std_err, 2);

			return ret_status;
	}else if(k==0){                                    //when exit command is executed
	               return 0;
	}else{                                             //after built_in commands are executed
	               // Restore the Standard Input and Output file descriptors
			dup2(std_in, 0);
			dup2(std_out, 1);
			dup2(std_err, 2);

	                return 1;
	}

}

void printDir()
{
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("Dir: %s\n>>", cwd);
}

/*
 * Function:  shell_loop
 *  main loop of the Mini-Shell
 */
void sh_loop(void){

	int status = 1;

        char * command_line;
        char ** arguments;


        while (status){
                printDir();
                command_line = read_command_line();

		if ( strcmp(command_line, "") == 0 ){
			continue;
		}
                arguments = split_command_line(command_line);
                status = sh_execute(arguments);
        }
}

/*
 * Function:  main
 */
int main(int argc, char ** argv){
        // Shell initialization
	getcwd(PWD, sizeof(PWD));	// Initialize PWD Environment Variable
	strcpy(PATH, PWD);		// Initialize the command PATH
        FILE *fp1;
        fp1 = fopen("history.txt","w"); //just to create a file to store history
        fclose(fp1);
        // Main loop of the shell
        sh_loop();

        return 0;
}
