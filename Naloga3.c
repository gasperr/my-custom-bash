#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <getopt.h>
#include <dirent.h>
#include <sys/file.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/types.h>

int izhod = 0;
char izhodChar[256];
int vhod = 0;
char vhodChar[256];


int izhodniStatus = 0;


int MAX_TOKENS = 50;
int token_count = 0;
int token_count2 = 0;

char ime[64] = "mysh";
char* tokens[50];
char* tokens2[50];
char line[50];

int tokenize(){

	int i = 0;
	int dolzina = 0;
	while(line[i] != '\0'){
		if(isspace(line[i])){
			i++;
			dolzina++;
			continue;
		}
		tokens[token_count] = line+dolzina;
		token_count++;
		if(line[i] == '"'){
			i++;
			dolzina++;
			tokens[token_count-1] = line+dolzina;
			while(line[i] != '"'){
				dolzina++;
				i++;
			}
		}else{
			while(line[i] != ' ' && line[i] != '\0'){
				dolzina++;
				i++;
			}
		}
		
		
		if(line[i] == '\0'){
			line[i-1] = '\0';
			break;
		} 
		line[i] = '\0';
		i++;
		dolzina++;

	}
	
}
int tokenize3(){

	int i = 0;
	int dolzina = 0;
	while(line[i] != '\0'){
		if(isspace(line[i])){
			i++;
			dolzina++;
			continue;
		}
		tokens[token_count] = line+dolzina;
		token_count++;
		if(line[i] == '"'){
			i++;
			dolzina++;
			tokens[token_count-1] = line+dolzina;
			while(line[i] != '"'){
				dolzina++;
				i++;
			}
		}else{
			while(line[i] != ' ' && line[i] != '\0'){
				dolzina++;
				i++;
			}
		}
		
		
		line[i] = '\0';
		i++;
		dolzina++;

	}
	
}
int tokenize2(char* lline){

	int i = 0;
	int dolzina = 0;
	while(lline[i] != '\0'){
		if(isspace(lline[i])){
			i++;
			dolzina++;
			continue;
		}
		tokens2[token_count2] = lline+dolzina;
		token_count2++;
		if(lline[i] == '"'){
			i++;
			dolzina++;
			tokens2[token_count2-1] = lline+dolzina;
			while(lline[i] != '"'){
				dolzina++;
				i++;
			}
		}else{
			while(lline[i] != ' ' && lline[i] != '\0'){
				dolzina++;
				i++;
			}
		}
		
		
		if(lline[i] == '\0'){
			//lline[i-1] = '\0';
			break;
		} 
		lline[i] = '\0';
		i++;
		dolzina++;

	}
	
}
void resetTokens(int tokenSize, char* c, int d){
	int i;
	for(i = 0; i < tokenSize; i++){
		tokens[i] = '\0';
		int j;
		for(j = 0; j < 50; j++){
			tokens[i][j] = '\0';
		}

	}
	memset(c, 0, 268);
	
}

void nastaviIme(char* n){
	strcpy(ime, n);
	izhodniStatus = 0;
}

void izpisiHelp(){
	printf("-------------------------------------------Pomoc------------------------------------------------\n");
	printf("name <> - ukaz brez argumentov vrne trenutno ime; z argumentom spremeni ime\n");
	printf("status - izpise izhodni status zadnjega (v ospredju) izvedenega ukaza\n");
	printf("exit <> - konca lupino s podanim izhodnim statusom,\n");
	printf("print <> - izpise podane argumente na standardni izhod (brez koncnega skoka v novo vrstico)\n");
	printf("echo <> - izpise podane argumente na standardni izhod (s skokom v novo vrstico)\n");
	printf("pid - izpise pid\n");
	printf("ppid - izpise ppid\n\n");
	printf("dir <> - zamenjava trenutnega imenika, brez agr = ~\n");
	printf("dirwhere - izpise trenutni imenik\n");
	printf("dirmake <> - ustvari imenik\n");
	printf("dirremove <> - izbrise imenik\n");
	printf("dirlist <> - izbrise vsebino imenika\n\n");
	printf("linkhard <> - ustvari trdo povezavo na cilj\n");
	printf("linksoft <> - ustvari simbolicno povezavo na cilj\n");
	printf("linkread <> - izpis cilja simb. povezave\n");
	printf("linklist <> - vse trde povezave datoteke z imenom\n");
	printf("unlink <> - brisanje datoteke\n");
	printf("rename <> <> - preimenovanje\n");
	printf("cpcat <> <> - cp in cat zdruzena\n\n");
	printf("pipes <> - ustvarjanje cevovoda\n");
	printf("& - izvajanje v ozadju\n");
	printf("\nhelp - izpise pomoc\n");
	izhodniStatus = 0;
}

void mymkdir(char* path){

	int rtrn = mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO);    
	if(rtrn != 0){
		perror("Error: ");
		izhodniStatus = 1;
		return;
	}
	izhodniStatus = 0;
}
void dirremove(char* path){
	int rtrn = remove(path);
	if(rtrn != 0){
		perror("Error: ");
		izhodniStatus = 1;
		return;
	}
	izhodniStatus = 0;
}
void dirlist(char* pp){
	
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));

	if(pp == NULL){
		pp = cwd;
	}
	
	//printf("---%s---\n", pp);
	//printf("Q%sQ\n", pp);
	DIR* dir = opendir(pp);

	if(dir == NULL){
		perror("Error DIRLIST");
		izhodniStatus = 1;
		return;
	}

	struct dirent* zac = malloc(sizeof(struct dirent*));
	


	zac = readdir(dir);
	while(zac != NULL){
		printf("%s  ", zac -> d_name);
		zac = readdir(dir);
	}
	printf("\n");
	closedir(dir);
	izhodniStatus = 0;
}

void linklist(char* ime){
	struct stat file;
	stat(ime, &file);
	//printf("Number of hard links: %d\n", file.st_nlink);
	//printf("inode: %d\n", file.st_ino);
	int iskanNode = (int)file.st_ino;
	/*int rtrn = execl("/usr/bin/find", "find", "-inum", str, NULL);
	if(errno != 0){
		perror("Error");
		izhodniStatus = 1;
	}*/
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));
	char* pp = cwd;

	DIR* dir = opendir(pp);

	if(dir == NULL){
		perror("Error LINKLIST");
		izhodniStatus = 1;
		return;
	}

	struct dirent* zac = malloc(sizeof(struct dirent*));
	


	zac = readdir(dir);
	while(zac != NULL){
		stat(zac -> d_name, &file);
		if(file.st_ino == iskanNode){
			printf("%s  ", zac -> d_name);
		}
		zac = readdir(dir);
	}
	printf("\n");
	closedir(dir);
	izhodniStatus = 0;
}

void cpcat(int argc, char* prvi, char* drugi){
	int file1;
	int file2;

	//printf("prvi: %s\n", prvi);
	//printf("drugi: %s\n", drugi);
	if(vhod==1 || prvi == NULL){
		argc = 1;
	}
	else if(izhod == 1 || drugi == NULL){
		argc = 2;
	}
	else{
		argc = 3;
	}


	


	if(argc >= 2 && strcmp(prvi, "-") != 0){
		//printf("%s\n", prvi);
		file1 = open(prvi, O_RDONLY);
		if(file1 < 0){
			perror("ERROR opening file1");
			izhodniStatus = 1;
			return;
		}
	}
	else if(argc <= 1 || strcmp(prvi, "-") == 0){
		file1 = 0;
	}
	if(argc < 3){
		file2 = 1;
	}
	else{
		file2 = open(drugi, O_RDWR | O_CREAT | O_APPEND, 
                        S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

		if(file2 < 0){
			perror("ERROR opening file2");
			izhodniStatus = 1;
			return;
		}
	}


	char* f = malloc(1024*sizeof(char));
	int readd = 0;
	while(readd = read(file1, f, 1024)){

		if(readd == 0) break;
		//printf("%d", file2);
		write(file2, f, readd);
	}
		
	
	izhodniStatus = 0;

	//flock(file1, LOCK_SH);
	//flock(file2, LOCK_EX);

	//close(file1);
	//close(file2);
}

void waitingForChild()
{
		
  int pid, status, serrno;
  serrno = errno;
  while (1)
    {
      pid = waitpid (WAIT_ANY, &status, WNOHANG);
      if (pid < 0)
        {
          perror ("waitpid");
          break;
        }
      if (pid == 0)
        break;
      //notice_termination (pid, status);
    }
  errno = serrno;

		
}

void vhodIzhod(){
	
	if(token_count > 2){
		if(tokens[token_count-1][0] == '>'){
			strcpy(izhodChar, tokens[token_count-1]+1);
			//izhodChar = tokens[token_count-1];
			izhod = 1;
		}

		if(tokens[token_count-2][0] == '<'){
			strcpy(vhodChar, tokens[token_count-2]+1);
			//vhodChar = tokens[token_count-2];
			vhod = 1;
		}
	}
	else{
		if(tokens[token_count-1][0] == '>'){
			strcpy(izhodChar, tokens[token_count-1]+1);
			//izhodChar = tokens[token_count-1];
			izhod = 1;
		}
		else if(tokens[token_count-1][0] == '<'){
			strcpy(vhodChar, tokens[token_count-1]+1);
			//izhodChar = tokens[token_count-1];
			vhod = 1;
		}
	}

	


}

void izvediUkaze(){
	int vOzadju = 0;
	int i = 0;
	if(!isspace(tokens[0][0]) && strcmp(tokens[token_count-1], "&") == 0){
			vOzadju = 1;
			token_count--;
	}

	if(!isspace(tokens[0][0])){
			vhodIzhod();
			//printf("\n");
			if(vhod != 0 || izhod != 0){

				if(izhod == 1){
					int izh = open(izhodChar, O_RDWR | O_CREAT | O_APPEND, 
                        S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
					dup2(izh, 1);
					token_count--;
				}
				if(vhod == 1){
					int vh = open(vhodChar, O_RDONLY);
					dup2(vh, 0);
					token_count--;
				}


			}
		}

		for(i = 0; i < token_count; i++){
			//printf("%d --- %s\n", i, tokens[i]);
		}
		//printf("%s aaa\n", tokens[1]);
		if(strcmp(tokens[0], "name") == 0){
			
			if(tokens[1] != NULL && strcmp(tokens[1], "") != 0){
				nastaviIme(tokens[1]);
			}
			else{
				printf("%s\n", ime);
			}
			izhodniStatus = 0;
		}

		else if(strcmp(tokens[0], "help") == 0){
			if(vOzadju == 1){
				if(fork() == 0){
					izpisiHelp();
				}
				signal (SIGCHLD, waitingForChild);
				izhodniStatus = 0;

			}
			else{
				izpisiHelp();
				izhodniStatus = 0;

			} 
			
		}
		else if(strcmp(tokens[0], "status") == 0){
			printf("%d\n", izhodniStatus);
			izhodniStatus = 0;
		}
		else if(strcmp(tokens[0], "exit") == 0){
			exit(atoi(tokens[1]));

		}
		else if(strcmp(tokens[0], "print") == 0){
			if(vOzadju == 1){
				if(fork() == 0){
					for(i = 1; i < token_count; i++){
						if(i == token_count-1){
							printf("%s", tokens[i]);
						}else{
							printf("%s ", tokens[i]);
						}
						
					}
					izhodniStatus = 0;
				}
				signal (SIGCHLD, waitingForChild);
				izhodniStatus = 0;

			}
			else{
				for(i = 1; i < token_count; i++){
					if(i == token_count-1){
						printf("%s", tokens[i]);
					}else{
						printf("%s ", tokens[i]);
					}
					
				}
				izhodniStatus = 0;

			} 
			
		}
		else if(strcmp(tokens[0], "echo") == 0){

				if(vOzadju == 1){
				if(fork() == 0){
					for(i = 1; i < token_count; i++){
						if(i == token_count-1){
							printf("%s", tokens[i]);
						}else{
							printf("%s ", tokens[i]);
						}
						
					}
					izhodniStatus = 0;
					printf("\n");
				}
				signal (SIGCHLD, waitingForChild);
				izhodniStatus = 0;

			}
			else{
				for(i = 1; i < token_count; i++){
					if(i == token_count-1){
						printf("%s", tokens[i]);
					}else{
						printf("%s ", tokens[i]);
					}
					
				}
				izhodniStatus = 0;
				printf("\n");

			} 
		}

		else if(strcmp(tokens[0], "pid") == 0){
			if(vOzadju == 1){
				if(fork() == 0){
					printf("%d\n", getpid());
				}
				signal (SIGCHLD, waitingForChild);
				izhodniStatus = 0;
			}else{
				printf("%d\n", getpid());
				izhodniStatus = 0;
			}
		}
		else if(strcmp(tokens[0], "ppid") == 0){
			if(vOzadju == 1){
				if(fork() == 0){
					printf("%d\n", getppid());
				}
				signal (SIGCHLD, waitingForChild);
				izhodniStatus = 0;
			}else{
				printf("%d\n", getppid());
				izhodniStatus = 0;
			}
		}

		//------------------------delo z datotekami---------------------

		else if(strcmp(tokens[0], "dir") == 0){
			char* directory = "/";
			if(tokens[1] != NULL && strcmp(tokens[1], "") != 0){
				directory = tokens[1];
			}
			//printf("%s\n", directory);
			int rtrn = chdir(directory);

			if(rtrn != 0){
          		perror("Error:");
          		izhodniStatus = 1;
        	}
        	else izhodniStatus = 0;
		}
		else if(strcmp(tokens[0], "dirwhere") == 0){
			char cwd[1024];
			if (getcwd(cwd, sizeof(cwd)) != NULL){
				printf("%s\n", cwd);
				izhodniStatus = 0;
			}else{
       			perror("Error:");
       			izhodniStatus = 1;
			}
		}
		else if(strcmp(tokens[0], "dirmake") == 0){
			if(vOzadju == 1){
				if(fork() == 0){
					mymkdir(tokens[1]);
				}
				signal (SIGCHLD, waitingForChild);
				izhodniStatus = 0;

			}
			else{
				mymkdir(tokens[1]);
				izhodniStatus = 0;

			} 
			
		}
		else if(strcmp(tokens[0], "dirremove") == 0){
			if(vOzadju == 1){
				if(fork() == 0){
					dirremove(tokens[1]);
				}
				signal (SIGCHLD, waitingForChild);
				izhodniStatus = 0;

			}
			else{
				dirremove(tokens[1]);
				izhodniStatus = 0;

			} 
			
		}
		else if(strcmp(tokens[0], "dirlist") == 0){
			if(vOzadju == 1){
				if(fork() == 0){
					if(token_count == 1){
						dirlist(NULL);
					}else{
						dirlist(tokens[1]);
					}
				}
				signal (SIGCHLD, waitingForChild);
				izhodniStatus = 0;

			}
			else{
				if(token_count == 1){
					dirlist(NULL);
				}else{
					dirlist(tokens[1]);
				}
				izhodniStatus = 0;

			} 
			
			
		}

		//-----------------------delo z linki-----------------------------
		else if(strcmp(tokens[0], "linkhard") == 0){
			int rtrn;
			if(vOzadju == 1){
				if(fork() == 0){
					rtrn = link(tokens[1], tokens[2]);
				}
				signal (SIGCHLD, waitingForChild);
				

			}
			else{
				rtrn = link(tokens[1], tokens[2]);
				

			}
			
			if(rtrn != 0){
				perror("Error");
       			izhodniStatus = 1;
			}
			else izhodniStatus = 0;
		}

		else if(strcmp(tokens[0], "linksoft") == 0){
			int rtrn;
			if(vOzadju == 1){
				if(fork() == 0){
					rtrn = symlink(tokens[1], tokens[2]);
				}
				signal (SIGCHLD, waitingForChild);
				

			}
			else{
				rtrn = symlink(tokens[1], tokens[2]);
				

			}
			if(rtrn != 0){
				perror("Error");
       			izhodniStatus = 1;
			}
			else izhodniStatus = 0;
		}
		else if(strcmp(tokens[0], "linkread") == 0){
			char buf[1024];
			int len = readlink(tokens[1], buf, sizeof(buf));
			buf[len] = '\0';
			printf("%s\n", buf);
			izhodniStatus = 0;
		}
		else if(strcmp(tokens[0], "linklist") == 0){

			if(vOzadju == 1){
				if(fork() == 0){
					linklist(tokens[1]);
				}
				signal (SIGCHLD, waitingForChild);
				

			}
			else{
				linklist(tokens[1]);
				

			}
			
		}
		else if(strcmp(tokens[0], "unlink") == 0){
			int rtrn;
			if(vOzadju == 1){
				if(fork() == 0){
					rtrn = unlink(tokens[1]);
				}
				signal (SIGCHLD, waitingForChild);
				

			}
			else{
				rtrn = unlink(tokens[1]);
				

			}
			if(rtrn != 0){
				perror("Error");
       			izhodniStatus = 1;
			}
			else izhodniStatus = 0;
		}
		else if(strcmp(tokens[0], "rename") == 0){
			int rtrn;
			if(vOzadju == 1){
				if(fork() == 0){
					rtrn = rename(tokens[1], tokens[2]);
				}
				signal (SIGCHLD, waitingForChild);
				
			}
			else{
				rtrn = rename(tokens[1], tokens[2]);
				

			}
			if(rtrn != 0){
				perror("Error");
       			izhodniStatus = 1;
			}
			else izhodniStatus = 0;
		}

		else if(strcmp(tokens[0], "cpcat") == 0){
			if(vOzadju == 1){
				if(fork() == 0){
					if(token_count == 1){
						cpcat(1, NULL, NULL);
					}
					else if(token_count == 2){
						cpcat(2, tokens[1], NULL);
					}
					else if(token_count == 3){
						cpcat(3, tokens[1], tokens[2]);
					}
				}
				signal (SIGCHLD, waitingForChild);
				
			}
			else{
				if(token_count == 1){
				cpcat(1, NULL, NULL);
				}
				else if(token_count == 2){
					cpcat(2, tokens[1], NULL);
				}
				else if(token_count == 3){
					cpcat(3, tokens[1], tokens[2]);
				}
				

			}
			
			
		}
		


		else if(!isspace(tokens[0][0])){
			
			if(vOzadju == 1){
				//printf("in here\n");
				int pid = fork();
				if(pid == 0){
					//printf("in child\n");
					char ex[256];
					sprintf(ex,"/usr/bin/%s",tokens[0]); 
					
					tokens[token_count] = (char*)NULL;
					execvp(tokens[0], tokens);
					//execl("/usr/bin/ls", "ls", NULL);
					
				}
				signal (SIGCHLD, waitingForChild);
				//wait();
			}else{
				//printf("in here\n");
				int pid = fork();
				if(pid == 0){
					//printf("in child\n");
					char ex[256];
					sprintf(ex,"/usr/bin/%s",tokens[0]); 
					
					tokens[token_count] = (char*)NULL;
					//printf("tokens: %s\n", tokens[2]);

					//printf("%d, izhod: %d\n", vhod, izhod);

					execvp(tokens[0], tokens);
					//execl("/usr/bin/ls", "ls", NULL);
					
				}
				int status;
				waitpid(pid,&status,0);
			}
			

		}
	
}

void izvediUkaze2(){
	int vOzadju = 0;
	int i = 0;
	if(!isspace(tokens2[0][0])){
			vhodIzhod();
			//printf("\n");
			if(vhod != 0 || izhod != 0){

				if(izhod == 1){
					int izh = open(izhodChar, O_RDWR | O_CREAT | O_APPEND, 
                        S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
					dup2(izh, 1);
					token_count2--;
				}
				if(vhod == 1){
					int vh = open(vhodChar, O_RDONLY);
					dup2(vh, 0);
					token_count2--;
				}


			}
		}
		
		
		
		

		if(!isspace(tokens2[0][0]) && strcmp(tokens2[token_count2-1], "&") == 0){
			vOzadju = 1;
			token_count2--;
		}

		for(i = 0; i < token_count2; i++){
			//printf("%d --- %s\n", i, tokens2[i]);
		}
		//printf("%s aaa\n", tokens2[1]);
		if(strcmp(tokens2[0], "name") == 0){
			
			if(tokens2[1] != NULL && strcmp(tokens2[1], "") != 0){
				nastaviIme(tokens2[1]);
			}
			else{
				printf("%s\n", ime);
			}
		}

		else if(strcmp(tokens2[0], "help") == 0){
			izpisiHelp();
		}
		else if(strcmp(tokens2[0], "status") == 0){
			printf("%d\n", izhodniStatus);
			izhodniStatus = 0;
		}
		else if(strcmp(tokens2[0], "exit") == 0){
			exit(atoi(tokens2[1]));
		}
		else if(strcmp(tokens2[0], "print") == 0){
			for(i = 1; i < token_count2; i++){
				if(i == token_count2-1){
					printf("%s", tokens2[i]);
				}else{
					printf("%s ", tokens2[i]);
				}
				
			}
			izhodniStatus = 0;
		}
		else if(strcmp(tokens2[0], "echo") == 0){

			for(i = 1; i < token_count2; i++){
				if(i == token_count2-1){
					printf("%s", tokens2[i]);
				}else{
					printf("%s ", tokens2[i]);
				}
			}
			printf("\n");
			izhodniStatus = 0;
		}

		else if(strcmp(tokens2[0], "pid") == 0){
			printf("%d\n", getpid());
			izhodniStatus = 0;
		}
		else if(strcmp(tokens2[0], "ppid") == 0){
			printf("%d\n", getppid());
			izhodniStatus = 0;
		}

		//------------------------delo z datotekami---------------------

		else if(strcmp(tokens2[0], "dir") == 0){
			char* directory = "/";
			if(tokens2[1] != NULL && strcmp(tokens2[1], "") != 0){
				directory = tokens2[1];
			}
			//printf("%s\n", directory);
			int rtrn = chdir(directory);

			if(rtrn != 0){
          		perror("Error:");
          		izhodniStatus = 1;
        	}
        	else izhodniStatus = 0; 
		}
		else if(strcmp(tokens2[0], "dirwhere") == 0){
			char cwd[1024];
			if (getcwd(cwd, sizeof(cwd)) != NULL){
				printf("%s\n", cwd);
				izhodniStatus = 0;
			}else{
       			perror("Error:");
       			izhodniStatus = 1;
			}
		}
		else if(strcmp(tokens2[0], "dirmake") == 0){
			mymkdir(tokens2[1]);
		}
		else if(strcmp(tokens2[0], "dirremove") == 0){
			dirremove(tokens2[1]);
		}
		else if(strcmp(tokens2[0], "dirlist") == 0){
			if(token_count2 == 1){
				dirlist(NULL);
			}else{
				dirlist(tokens2[1]);
			}
			
		}

		//-----------------------delo z linki-----------------------------
		else if(strcmp(tokens2[0], "linkhard") == 0){
			int rtrn = link(tokens2[1], tokens2[2]);
			if(rtrn != 0){
				perror("Error");
       			izhodniStatus = 1;
			}else izhodniStatus = 0;
		}

		else if(strcmp(tokens2[0], "linksoft") == 0){
			int rtrn = symlink(tokens2[1], tokens2[2]);
			if(rtrn != 0){
				perror("Error");
       			izhodniStatus = 1;
			} else izhodniStatus = 0;
		}
		else if(strcmp(tokens2[0], "linkread") == 0){
			char buf[1024];
			int len = readlink(tokens2[1], buf, sizeof(buf));
			buf[len] = '\0';
			printf("%s\n", buf);
			izhodniStatus = 0;
		}
		else if(strcmp(tokens2[0], "linklist") == 0){
			linklist(tokens2[1]);

		}
		else if(strcmp(tokens2[0], "unlink") == 0){
			int rtrn = unlink(tokens2[1]);
			if(rtrn != 0){
				perror("Error");
       			izhodniStatus = 1;
			}
			else izhodniStatus = 0;
		}
		else if(strcmp(tokens2[0], "rename") == 0){
			int rtrn = rename(tokens2[1], tokens2[2]);
			if(rtrn != 0){
				perror("Error");
       			izhodniStatus = 1;
			}
			else izhodniStatus = 0;
		}

		else if(strcmp(tokens2[0], "cpcat") == 0){
			if(token_count2 == 1){
				cpcat(1, NULL, NULL);
			}
			else if(token_count2 == 2){
				cpcat(2, tokens2[1], NULL);
			}
			else if(token_count2 == 3){
				cpcat(3, tokens2[1], tokens2[2]);
			}
			
		}
		


		else if(!isspace(tokens2[0][0])){

			


			if(vOzadju == 1){
				//printf("in here\n");
				int pid = fork();
				if(pid == 0){
					//printf("in child\n");
					char ex[256];
					sprintf(ex,"/usr/bin/%s",tokens2[0]); 
					
					tokens2[token_count2] = (char*)NULL;
					execvp(tokens2[0], tokens2);
					//execl("/usr/bin/ls", "ls", NULL);
					
				}
				signal (SIGCHLD, waitingForChild);
				//wait();
			}else{
				
				//int pid = fork();
				//if(pid == 0){
					//printf("in child\n");
					char ex[256];
					sprintf(ex,"/usr/bin/%s",tokens2[0]); 
					
					tokens2[token_count2] = (char*)NULL;
					//printf("tokens2: %s\n", tokens2[2]);

					//printf("%d, izhod: %d\n", vhod, izhod);

					execvp(tokens2[0], tokens2);
					//execl("/usr/bin/ls", "ls", NULL);
					
				//}
				//int status;
				//waitpid(pid,&status,0);
			}
			

		}
	
}


void piping(int trenutnaStopnja, int stStopenj, int pipes[], int stPipes, int notranji, int zunanji){
	if(trenutnaStopnja > stStopenj) return;
	int i;
	if(trenutnaStopnja == 0){
		int pid = fork();
		if(pid == 0){
			//zamenjamo cat's stdout z write-end of 1st pipe
			dup2(pipes[1], 1);
			//IMPORTANT! CLOSE ALL PIPES
			for(i = 0; i < stPipes; i++){
				close(pipes[i]);
			}
			
			tokenize2(tokens[1]);
			izvediUkaze2();
		}else{
			piping(trenutnaStopnja+1, stStopenj, pipes, stPipes, 0, 3);
		}
	}
	else if(trenutnaStopnja == stStopenj){
		//fork to execute cut
		int pid = fork();
		if(pid == 0){
			//zamenjamo cut's stdin z normalnim stdin
			dup2(pipes[stPipes - 2], 0);
			//close all pipes
			for(i = 0; i < stPipes; i++){
				close(pipes[i]);
			}
			tokenize2(tokens[stStopenj+1]);
			izvediUkaze2();
		}
	}
	else{
		int pid = fork();
		if(pid == 0){
			//zamenjamo greps stdin z normalnim stdin
			dup2(pipes[notranji], 0);
			//zamenjamo greps stdout z normalnim stdout
			dup2(pipes[zunanji], 1);
			//close all pipes
			for(i = 0; i < stPipes; i++){
				close(pipes[i]);
			}

			tokenize2(tokens[trenutnaStopnja+1]);
			izvediUkaze2();
		}else{
			piping(trenutnaStopnja+1, stStopenj, pipes, stPipes, notranji+2, zunanji+2);
		}
	}
}

int main(int argc, char** argv){
	
	char input[268];
	if(isatty(0)){
		printf("%s> ", ime);
	}
	while(fgets(input, 268, stdin) != 0){
		fflush(stdout);
		fflush(stdin);
		

		int current_stdout = dup(1);
		int current_stdin = dup(0);

		izhod = 0;
		vhod = 0;

		
		
		strcpy(line, input);
		//printf("input:--%s--\n", input);
		int i = 0;
		while(isspace(line[i])){
			i++;
		}

		if(line[i] == '#'){
			if(isatty(0)){
				printf("%s> ", ime);
			}
			continue;
		} 
		tokenize();
		
		//printf("%d token count: d\n", token_count);



		if(strcmp(tokens[0], "pipes") == 0){
			//printf("%d\n", token_count);
			
			if(token_count == 2){
				izhod = 1;
				strcpy(izhodChar, "/dev/null");
			
				strcpy(line, tokens[1]);
				
				token_count = 0;
				
				tokenize3();
		
				izvediUkaze();
			}
			else{



				//stevilo pipov
				int stPipes = (token_count-2)*2;
				//printf("%d\n", stPipes);
				int pipes[stPipes];

				for(i = 0; i < stPipes; i=i+2){
					//printf("%d\n", stPipes);
					pipe(pipes+i);
				}

				//klici funkcijo
				
				piping(0, token_count-2, pipes, stPipes, 0, 0);
				for(i = 0; i < stPipes; i++){
					//printf("%d\n", i);
					close(pipes[i]);
				}
				int i;
				int status;
				for (i = 0; i < token_count-1; i++)
					wait(&status);
			}
			
		}

		else{
			izvediUkaze();
		}
		
		//resetTokens(token_count, input, 268);
		memset(input, 0, 268);
		for(i = 0; i < token_count; i++){
			memset(tokens[i], 0, 50);
		}
		token_count = 0;

		for(i = 0; i < token_count2; i++){
			memset(tokens2[i], 0, 50);
		}
		token_count2 = 0;
		if(vhod != 0){
			fflush(stdin);
			dup2(current_stdin, 0);
			close(current_stdin);
		}
		if(izhod != 0){
			fflush(stdout);
			dup2(current_stdout, 1);
			close(current_stdout);
		}

		if(isatty(0)){
			printf("%s> ", ime);
		}
		fflush(stdin);
		fflush(stdout);
		dup2(current_stdin, 0);
		dup2(current_stdout, 1);
		close(current_stdout);
		close(current_stdin);
		
		
	}
	return 0;
}
