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
bool seat[CHOPSTICKS];

pthread_mutex_t Out = PTHREAD_MUTEX_INITIALIZER;

/// \brief Class task
/// \details Implements the following functions
class Task
{
private:
	int id;
	int eatingTime;
	int thinkingTime;
public:
	Task();
	void set_time();
	int get_eatingTime();
	int get_thinkingTime();
};

Task::Task()
{
	eatingTime = 0;
	thinkingTime = 0;
}

void Task::set_time()
{
	eatingTime = rand()% 10 + 2;
	thinkingTime = rand()%10 + 2;
}
int Task::get_eatingTime() 
{
	return eatingTime;
}
int Task::get_thinkingTime() 
{
	return thinkingTime;
}



/// \brief Class Process
/// \details Implements the following functions:
/// Process() & run()
class Process
{
private:
	int id;
	unsigned int sid; /// seed for srand
	Task* task;
public:
	Process(int, Task*);
	void run();
};

/// \brief Constructor
Process::Process(int identifer, Task* curTask)
{
	id = identifer;
	task = curTask;

}

void Process::run() 
{
	task->set_time();
	int philosThinking = task->get_thinkingTime();
	int philosEating = task->get_eatingTime();
	bool busy = true;
	bool right = false;
	bool left = false;
	while (busy)
	{
		
		

		pthread_mutex_lock(&Out);
		cout << "Philospher " << id << " was thinking for " <<philosThinking<<"ms"<< endl;
		pthread_mutex_unlock(&Out);

		sleep(philosThinking); /// start thinking


		if (token == id)
		{
			cout << "Philospher " << id << " has the token " << endl;	

			/// check to see if both chopstick are available
			//else if ((chopstickPosition[id] == true) && (chopstickPosition[(id + 1) % 5] == true))
			if ((left) && (right))
			{
				chopstickPosition[id] = false;
				chopstickPosition[(id + 1) % 5] = false;

				cout << "Philospher " << id << " grabbed left and right chopsticks." << endl;

				/// start eating
				pthread_mutex_lock(&Out);
				cout << "Philospher " << id << " is now eating." << endl;
				sleep(philosEating); 
				cout << "Philospher " << id << " ate for "<<philosEating<<"ms" << endl;
				pthread_mutex_unlock(&Out);

				///  release chopsticks
				chopstickPosition[id] = true;
				chopstickPosition[(id + 1) % 5] = true;
				cout << "Philospher " << id << " dropped all chopsticks." << endl;

				seat[id] = false;
				busy = false;
				
			}

			else
			{
				if (chopstickPosition[(id + 1) % 5] == false && (right))
				{
					right = false;
					chopstickPosition[(id + 1) % 5] = true; ///  return chopstick to neighbour
					cout << "Philospher " << id << " dropped right chopstick." << endl;
				}

				if (chopstickPosition[(id)] == false && (left))
				{
					left = false;
					chopstickPosition[id] = true; ///  drop chopstick 
					cout << "Philospher " << id << " dropped left chopstick" << endl;
				}
			}

			pthread_mutex_lock(&Out);
			token = (token > 3) ? 0 : token+1; /// set token 
			pthread_mutex_unlock(&Out);
			while ((seat[token] == false) && (token < 5))
			{
				pthread_mutex_lock(&Out);
				cout << "Philospher " << id << " left!" << endl;
				token++;
				pthread_mutex_unlock(&Out);
			}

			if (token == 5)
				token = 0;

			cout << "Philospher " << id << " gave the token to philospher " << token << endl;
			
		}

		else 
		{
			if (chopstickPosition[id] == true)
			{
				pthread_mutex_lock(&Out);
				chopstickPosition[id] = false; ///  grab personal chopstick
				left = true;
				cout << "Philospher " << id << " grabbed right chopstick!" << endl;
				pthread_mutex_unlock(&Out);
			}

			else if (chopstickPosition[(id + 1) % 5] == true)
			{
				pthread_mutex_lock(&Out);
				chopstickPosition[id] = false; ///  grab neighbours chopstick
				right = true;
				cout << "Philospher " << id << " grabbed left chopstick!" << endl;
				pthread_mutex_unlock(&Out);
			}
		}
	}
}

/// \brief Run Thread
/// \param void
void* callRun(void* process) {
	((Process*)process)->run();
}


int main(int argc, const char* argv[]) {
	int err;
	Task* task = new Task();
	pthread_t philos[MAXPHILOS];

	/// Give every philospher a chopstick
	for (int i = 0; i < CHOPSTICKS; i++)
	{
		seat[i] = true;
		chopstickPosition[i] = true;
	}

	//Create and launch all threads
	for (int i = 0; i < MAXPHILOS; i++)
	{
		Process* process_exec = new Process(i, task);

		err = pthread_create(&philos[i], NULL, callRun, process_exec);
		if (err)
		{
			printf("Error: failed to seat all philosophers\n");
			exit(1);
		}

	}

	pthread_mutex_lock(&Out);
	cout << "All philosophers are seated" << endl;
	pthread_mutex_unlock(&Out);

	//Wait for threads to complete
	for (int i = 0; i < MAXPHILOS; i++)
	{
		pthread_join(philos[i], NULL);
	}

	cout << "All philiosphers are done." << endl;

	return 0;
}

