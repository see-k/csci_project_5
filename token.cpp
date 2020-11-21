/// \author Chike Okonta
/// \brief CSCI 311 project 05
/// \detail token algorithm
/// \date 11/20/2020


/// \details the token is an integer object stored in a file. 
/// This value of the token will always be equal to the ID of the philsopher (i.e process) that currently has access to it.
/// Each philsopher will determine if he can start eating or continue eating based on when he recieves the token to his neighbour
#include <iostream> 
#include <string> 
#include <mutex> 
#include <pthread.h> 
#include <errno.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/time.h>
#include <time.h>
using namespace std;

#define token 0
#define philospher 0

