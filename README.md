How to run the minishell:

	*open terminal with current directory as this folder
	*to compile type: "gcc -o minishell minishell.c" 
	*to run the program type: "./minishell"


This minshell has both built-in and external commands.

*Basic Functioning of the minishell:
     
	*When you type a command into the minshell, it first looks for the command in the list of built-ins that it maintains.
	*If present, it will call the corresponding function.
	*If not, it will fork a new process, load the command's image into the child process and wait for the child process to finish execution before displaying the prompt again.
	*If the command is not found (the corresponding .c file is not found), an error message indicating that the command was not found will be displayed.

*Functioning of the code is explained in detail in the source code file "minishell.c"

*Built_in commands:
	cd
	help
	exit
	pwd
	echo
	history


*Most of the external commands work fine. Some of them are man, date, cal, clear, sleep, ls, cat, more, less, touch, cp, mv, rm, script, find, mkdir, rmdir, chmod, grep

Environment Variables: USER, HOME, SHELL, TERM, PATH

*Minishell does work for multiple number of arguments in single command line, 
*Redirection operations >,<,>>,<< can be used.
*Single Piping between commands can be used.
*history.txt file is created in current directory to store history. 
	
