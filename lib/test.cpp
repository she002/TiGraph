#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>

using namespace std;

pthread_barrier_t* my_barrier = new pthread_barrier_t;
pthread_mutex_t my_mutex = PTHREAD_MUTEX_INITIALIZER;
void *print_message( void *ptr)
{
	string* message;
	message = static_cast<string*>(ptr);
	printf("%s \n",message->c_str());
	pthread_mutex_lock(&my_mutex);
	*message = *message + " changed";
	pthread_mutex_unlock(&my_mutex);
	pthread_barrier_wait(my_barrier);
	printf("barrier called\n");
}

int main()
{
	
	pthread_t* threadlist = new pthread_t[3];
	string* messages = new string[3];
	messages[0] = "thread 1 created";
	messages[1] = "thread 2 created";
	messages[2] = "thread 3 created";
	pthread_barrier_init(my_barrier, NULL, 3);
	for(int i = 0; i < 3; i++)	
	{
		int err = pthread_create(&threadlist[i], NULL, print_message, &messages[i]);
		if(err)
		{
			printf("Error = return code: %d\n", err);
			exit(EXIT_FAILURE);
		}
	}

	for(int i = 0; i < 3; i++)
	{
		pthread_join(threadlist[i], NULL);
	}
	for(int i = 0; i < 3; i++)
	{
		printf("%s\n", messages[i].c_str());
	}
	delete [] threadlist;
	delete [] messages;
	delete my_barrier;
	return 0;

}
