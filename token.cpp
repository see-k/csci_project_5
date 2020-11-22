/// \author Chike Okonta
/// \brief CSCI 311 project 05
/// \detail token algorithm
/// \date 11/20/2020


/// \details the token is an integer object stored in a file. 
/// This value of the token will always be equal to the ID of the philsopher (i.e process) that currently has access to it.
/// Each philsopher will determine if he can start eating or continue eating based on when he recieves the token to his neighbour
/// We will be implementing the second solution provided in the text for preventing deadlock:
/// Allow a philosopher to pick up her chopstick to eat only if both chopsticks are available
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
using namespace std;


/// <summary>
/// Global variables
/// </summary>
const int MAXPHILOS = 5;
int token = 0;
bool chopstickPosition[CHOPSTICKS];

/// \brief Class Process
/// \details Implements the following functions:
/// Process() & run()
class Process
{
private:
	int id;
	unsigned int sid; /// seed for srand
public:
	void run();
};

/// \brief Constructor
void Process::Process(int identifer)
{
	id = identifer;
}

void Process::run() 
{

}

/// \brief Run Thread
/// \param void
void* callRun(void* process) {
	((Process*)process)->run();
}


int main(int argc, const char* argv[]) {
	int err;

	pthread_t tid[MAXPHILOS];

	//create all threads
	for (int i = 0; i < MAXPHILOS; i++)
	{


		Process* process_exec = new Process(i);

		err = pthread_create(&tid[i], NULL, callRun, process_exec);
		if (err)
		{
			printf("Error: Failed to create processes\n");
			exit(1);
		}

	}

	pthread_mutex_lock(&Out);
	cout << "All processes are active." << endl;
	pthread_mutex_unlock(&Out);

	//Wait for threads to complete
	for (int i = 0; i < MAXPHILOS; i++)
	{
		pthread_join(tid[i], NULL);
	}

	cout << "All processes have completed." << endl;

	return 0;
}
