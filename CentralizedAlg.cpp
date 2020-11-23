//CentralizedAlg.cpp
//Centralized algorithm
//Don Lieu
//ei9700fv
//edited by Zeph Johnson

#include <iostream>
#include <string>
#include <mutex>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
//#define CHOPSTICKS 5   //Zeph: redundant constant, MAXDEGREEMP+1 is identical

//using namespace std;

//const int TOTALMEM = 100;
const int MAXDEGREEMP = 4; //Zeph: this probably needs to be sent through the process creation
//Zeph: these need to be simulated with sockets:
bool chopstick[MAXDEGREEMP+1]; //true if available for use, false if reserved
pthread_mutex_t Out = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t Stick = PTHREAD_MUTEX_INITIALIZER;

void println (char *s)
{
	pthread_mutex_lock (&Out);
	printf ("%s", s);
	pthread_mutex_unlock (&Out);
}

/*struct Memory
{
	 int id;
	 int size;
};*/

class Scheduler
{//Zeph: useless class that might be useful for the token ring alg?
	private:
		//int id;
		//int totMem;
		//int freeMem;
		//Memory memory[MAXDEGREEMP+1];
		int next;
	public:
		Scheduler ();
		//Scheduler (int);
		//void displayProcesses ();
		//void getMemory (int id, int need);
		//void returnMemory (int id, int need);
};

Scheduler::Scheduler ()
{
    //totMem = 0;
    //freeMem = 0;
	next = 0;
}

/*Scheduler::Scheduler (int totMem)
{
    totMem= totMem;
    freeMem = totMem;
    for (int i = 0; i < MAXDEGREEMP+1; i++)
    {
        memory[i].id = 0; memory[i].size = 0;
    }
    next = 0;
}*/

class Process 
{
    private:
      int id;
      Scheduler* scheduler;
      int eatingTime;
      int thinkingTime;
    public:
      Process (int, Scheduler*);
      void operator () ();
      void run ();
};

Process::Process (int identifier, Scheduler* schd)
{
	id = identifier;
	scheduler = schd;
	srand (id);
	eatingTime = rand() % 10;
	thinkingTime = rand() % 5;
}

void Process::run () //This was changed
{
	//bool chopstick1, chopstick2;
	char rstr[100];
	
	sprintf (rstr, "Process %d thinking...\n", id);
	println ( rstr );
	//cout << "Process " << id << " thinking..." << endl;	
	sleep(thinkingTime);
	
	//Zeph: implemented mutual exclusion, cleaned up the next few lines using % instead of if statements.
	//While either nearby chopstick isn't available, sleep:
	pthread_mutex_lock (&Stick); //mutual exclusion for checking/changing chopsticks
	while ( (chopstick[(id)] == false) || (chopstick[(id+1) % (MAXDEGREEMP+1)] == false) )
    {
        pthread_mutex_unlock (&Stick); //unlock while sleeping
        sleep(1);
        pthread_mutex_lock (&Stick); //lock again to check chopsticks then take if necessary
	}
	
	//reserve chopsticks and eat:
	chopstick[id] = false;
	chopstick[(id + 1) % (MAXDEGREEMP+1)] = false;
	pthread_mutex_unlock (&Stick);
	sprintf (rstr, "Process %d eating with chopsticks %d and %d...\n", id, id, ((id+1) % (MAXDEGREEMP+1)));
	println ( rstr );
	sleep(eatingTime); //simulate eating
	
	//return chopsticks:
	pthread_mutex_lock (&Stick);
    chopstick[id] = true;
    chopstick[(id + 1) % (MAXDEGREEMP+1)] = true;
	pthread_mutex_unlock (&Stick);
    sprintf (rstr, "Process %d finished eating and returned chopsticks %d and %d\n", id, id, ((id+1) % (MAXDEGREEMP+1)));
	println ( rstr );
}
 
void *callRun (void* process)
{
 	((Process *)process) -> run();
}

int main (int argc, const char * argv[]) //This was changed
{
	int err;
	char rstr[100];
	pthread_t tidArray[MAXDEGREEMP+1];
	Process *processArray[MAXDEGREEMP+1];
	
	for(int i = 0; i < MAXDEGREEMP+1; i++)
	{
		chopstick[i] = true;
	}

	
	Scheduler* schd = new Scheduler (); //TOTALMEM);
	for(int i = 0; i < MAXDEGREEMP+1; i++)
	{
		processArray[i] = new Process(i, schd);
		err = pthread_create(&tidArray[i], NULL, callRun, processArray[i]);
		if (err)
		{
			sprintf (rstr, "Create of thread %d failed.\n", i);
			println (rstr);
			exit(i);
		}
	}
	
	sprintf (rstr, "All processes are active.\n");
	println (rstr);
    
	for(int i = 0; i < MAXDEGREEMP+1; i++)
	{
		pthread_join(tidArray[i], NULL);
	}
	
	sprintf (rstr, "All processes have completed.\n");
	println (rstr);
	
	return 0;
}
