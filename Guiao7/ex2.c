#include <stdio.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_CHILD 128


int iterator;
int num_childs;
int alive;
int child_id[MAX_CHILD];


void start_child_iteration(int max){

	iterator = 0;
	alive = num_childs = max;

	if (max > 0) {
		kill(child_id[0],SIGCONT);     // kill envia sinal para o processo
		alarm(1);					   // SIGCONT -> desbloqueia um processo se estiver bloqueado
									   // alarm envia ao processo invocador um SIGALRM dentro de 1 segundo
	}
}



void next_child(){
	
	kill(child_id[iterator],SIGSTOP); // SIGSTOP -> bloqueia um processo

	if (++iterator == num_childs){
		iterator = 0;
	}

	kill(child_id[iterator], SIGCONT); // SIGCONT -> desbloqueia um processo se estiver bloqueado

	alarm(1);
}


void kill_child(){

	if(waitpid(-1,NULL,WNOHANG)){
		alive--;
		next_child();
	}
}




int main(int argc, char *argv[]){
	
	int i, id;

	signal(SIGALRM, next_child);  // SIGALRM -> manda o sinal sigalrm a um processo 
	signal(SIGCHLD, kill_child);  // SIGCHLD -> sinal que um processo recebe quando um filho morre

	for(i=1; i < argc; i++){
		id = fork();

		if(id){
			child_id[i-1] = id;
		}
		else{
			kill(getpid(),SIGSTOP); // SIGSTOP -> bloqueia um processo
			execlp(argv[i],argv[i],NULL);
			_exit(1);
		}
	}

	start_child_iteration(argc-1);

	while(alive){ pause(); } // suspende um processo corrente até á chegada do seu sinal

	return 0;
}