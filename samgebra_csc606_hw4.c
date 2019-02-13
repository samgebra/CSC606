/* Dining Philosophers										*/
/* Code Reference: "Modern  Operating Systems, Tanenbaum	*/

#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>

#define N			5			// number of philosophers
#define THINKING	0			// philosopher is thinking
#define HUNGRY		1			// philosopher is trying to get forks
#define EATING		2			// philosopher is eating
#define RIGHT		(i+1)%N		// number of i's right neighbor
#define LEFT		(i+N-1)%N	// number of i's left neighbor

typedef sem_t semaphore;		// using real semaphores
int state[N];					// array to keep track of philosopher states
semaphore mutex;				// mutual exclusion for critical regions
semaphore s[N];					// one semaphore per philosopher
int philos[N] = { 0, 1, 2, 3, 4 };

/* function prototype declaration */
void think();
void eat();
void take_forks(int i);
void put_forks(int i);
void test(int i);

void* philosopher(int* i)		// i: philosopher number, from 0 to N-1
{
	while (1) {					// repeat forever
		think();				// philosopher is thinking
		take_forks(*i);			// acquire two forks or block
		eat();					// philosopher is eating
 		put_forks(*i);			// put both forks back on the table
	}							// end while loop
}								// end void philosopher()

void take_forks(int i) 			// i: philosopher number, from 0 to N-1
{
	sem_wait(&mutex);
 	state[i] = HUNGRY;			// record fact that philosopher is hungry	
	/* display state */
	printf("Philosopher %d is hungry\n", i + 1); 	
	test(i);                    // try to acquire 2 forks
 	sem_post(&mutex);			// exit critical region
 	sem_wait(&s[i]);			// block if forks not acquired
}								// end void take_forks()

void put_forks(int i)    		// i: philosopher number, from 0 to N-1
{
	sem_wait(&mutex);			
	state[i]=THINKING;			// philosopher has finished eating	
	/* display state */
	printf("Philosopher %d putting fork %d and %d down!\n", i + 1, LEFT + 1, i + 1);
	printf("Philosopher %d is thinking\n", i + 1);
	test(LEFT);                 // see if left neighbor can eat now
	test(RIGHT);                // see if right neighbor can eat now
	sem_post(&mutex);			// exit critical region
}

void test(int i)				// i: philosopher number, from 0 to N-1
{
	if (state[i]==HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) {
		state[i] = EATING;		
		/* display state */
		printf("Philosopher %d picks fork %d and %d up!\n", i + 1, LEFT + 1, i + 1);
		printf("Philosopher %d is eating\n", i + 1);		
		sem_post(&s[i]);
	}
}

void think() {
	sleep(1);}

void eat() {
	sleep(0);}

int main()
{
    pthread_t thd[N];		// an array for threads
    int index;				// for loop counter
	
	/* Creating our semaphores */
    sem_init(&mutex, 0, 1);
	for (index = 0; index < N; index++)		// initializing the s[] semaphore array
	{
		sem_init(&s[index], 0, 0);
	}
	/* creating our N Philosopher-threads here */
	for (index = 0; index < N; index++)	// create thread for each philosopher
	{
		pthread_create(&thd[index], NULL, philosopher, &philos[index]);
		printf("Philosopher %d is thinking\n", index + 1);
	}
    /* the main thread to wait for the philosopher threads */
	for (index = 0; index < N; index++)
	{
		pthread_join(thd[index], NULL);
	}		
	/* Destroy your semaphores here by uncommenting the below block of code
	for (index = 0; index < N; index++)
	{
		sem_destroy(&s[index]);
	} */
}