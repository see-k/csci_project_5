#include <iostream> 
#include <string> 
#include <mutex> 
#include <pthread.h> 
#include <errno.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/time.h>
#include <time.h>
#define CHOPSTICKS 5

//Project 5
//Centralized algorithm
//Don Lieu
//ei9700fv

using namespace std; 

const int TOTALMEM = 100; 
const int MAXDEGREEMP = 4; 
const int MAXPROCESSES = 5;
bool chopstickPosition[CHOPSTICKS];

pthread_mutex_t Out = PTHREAD_MUTEX_INITIALIZER; 

void println (char *s) 
{ 
	 pthread_mutex_lock (&Out); 
	 cout << s << endl; 
	 pthread_mutex_unlock (&Out); 
} 
struct Memory 
{ 
	 int id; 
	 int size; 
}; 
class Scheduler 
{ 
	private: 
		 int id; 
		 int totMem; 
		 int freeMem; 
		 Memory memory[MAXDEGREEMP+1]; 
		 int next; 
	public: 
		 Scheduler (); 
		 Scheduler (int); 
		 void displayProcesses (); 
		 void getMemory (int id, int need); 
		 void returnMemory (int id, int need); 
}; 
Scheduler::Scheduler () 
{ 
	 totMem = 0; 
	 freeMem = 0; 
	 next = 0; 
} 
Scheduler::Scheduler (int totMem) 
{ 
	totMem= totMem; 
	freeMem = totMem; 
	 for (int i = 0; i < MAXDEGREEMP+1; i++) 
	{ 
	 	memory[i].id = 0; memory[i].size = 0; 
 	} 
 	next = 0; 
} 

class Process 
{ 
	private: 
	 int id; 
	 unsigned int sid; /* seed for rand */ 
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
	 sid = id; 
	 scheduler = schd; 
	 srand (time(NULL));
	 eatingTime = rand() % 10; 
	 thinkingTime = rand() % 10;
} 
void Process::run () //This was changed
{ 
	 //bool chopstick1, chopstick2;
	 char rstr[100];
	 
	 sprintf (rstr, "Process %d thinking...\n", id); 
	 println ( rstr );
	 //cout << "Process " << id << " thinking..." << endl;	
	 sleep(thinkingTime); 
	 if (id == (MAXPROCESSES-1))
	 {
	 	while ( (chopstickPosition[(MAXPROCESSES-1)] == false) || (chopstickPosition[0] == false) )
		{
			sleep(1);
		}
	 }
	 else
	 {
		while ( (chopstickPosition[id] == false) || (chopstickPosition[(id + 1)] == false) )
		{
			sleep(1);
		}
	 }
	 if (id == (MAXPROCESSES-1))
	 {
	 	chopstickPosition[(MAXPROCESSES-1)] = false;
		chopstickPosition[0] = false;
		sprintf (rstr, "Process %d eating with chopsticks %d and %d...\n", id, (MAXPROCESSES-1), 0); 
	 	println ( rstr );
	 }
	 else
	 {
		chopstickPosition[id] = false;
		chopstickPosition[(id + 1)] = false;
		sprintf (rstr, "Process %d eating with chopsticks %d and %d...\n", id, id, (id+1)); 
	 	println ( rstr );
	 }
	 sleep(eatingTime);
	 if (id == (MAXPROCESSES-1))
	 {
	 	chopstickPosition[(MAXPROCESSES-1)] = true;
		chopstickPosition[0] = true;
		sprintf (rstr, "Process %d finished eating and returned chopsticks %d and %d\n", id, (MAXPROCESSES-1), 0); 
	 	println ( rstr );
	 }
	 else
	 {
		chopstickPosition[id] = true;
		chopstickPosition[(id + 1)] = true;
		sprintf (rstr, "Process %d finished eating and returned chopsticks %d and %d\n", id, id, (id+1)); 
	 	println ( rstr );
	 }
} 
 
void *callRun (void* process) 
{ 
 	((Process *)process) -> run(); 
} 

int main (int argc, const char * argv[]) //This was changed
{ 
	 int err; 
	 pthread_t tidArray[MAXPROCESSES];
	 Process *processArray[MAXPROCESSES];
	 
	 for(int i = 0; i < CHOPSTICKS; i++)
	{
		chopstickPosition[i] = true;
	}

	 
	 Scheduler* schd = new Scheduler (TOTALMEM); 
	 for(int i = 0; i < MAXPROCESSES; i++)
	 {
	 	processArray[i] = new Process(i, schd);
	 	err = pthread_create(&tidArray[i], NULL, callRun, processArray[i]);
	 	if (err)
	 	{
	 		cout << "Create of thread " << i << " failed." << endl;
	 		exit(i);
	 	}
	 } 
	 
	 pthread_mutex_lock (&Out); 
	 cout << "All processes are active." << endl;
	 pthread_mutex_unlock (&Out); 
	 
	 for(int i = 0; i < MAXPROCESSES; i++)
	 {
	 	pthread_join(tidArray[i], NULL);
	 }
	 
	 cout << "All processes have completed." << endl; 
	 
	 return 0; 
} 

