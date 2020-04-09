/* Erik Safford
 * Dining Philosophers Problem using POSIX Threads
 * Spring 2019  */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <time.h>
#include <string.h>

pthread_mutex_t chopsticks[5];  //Set of 5 chopstick mutex semaphores to keep track of in use/not in use

/* successive calls to randomGaussian produce integer return values
   having a gaussian distribution with the given mean and standard
   deviation.  Return values may be negative.   */
int randomGaussian(int mean, int stddev) {
        double mu = 0.5 + (double) mean;
        double sigma = fabs((double) stddev);
        double f1 = sqrt(-2.0 * log((double) rand() / (double) RAND_MAX));
        double f2 = 2.0 * 3.14159265359 * (double) rand() / (double) RAND_MAX;
        if (rand() & (1 << 5)) {
                return (int) floor(mu + sigma * cos(f2) * f1);
		}
        else {
                return (int) floor(mu + sigma * sin(f2) * f1);
		}
}

void *eatThinkTime(void *arg) {
	int j = (int)arg;  //j is the 0,1,2,3,4 philosopher
	int first, second, error;
	int eatTime, thinkTime, totalEatTime = 0, totalThinkTime = 0;

	//Define the order that philosophers will pick up chopsticks
        //Want the last philosopher to pick up in a different order to avoid deadlock.
	if(j < 4) {  //If philosopher 0,1,2,3 , want to pickup LEFT then RIGHT
		first = j;
		second = j + 1;
	}
	else {       //If philosopher 4, want to pickup RIGHT then LEFT
		first = 0;
		second = 4;
	}

	while(totalEatTime < 50) {
		//Think
		thinkTime = randomGaussian(11,7);
		if(thinkTime < 0) {  //Don't want to think for a negative amount of time
			thinkTime = 0;
		}
		totalThinkTime += thinkTime;
		printf("Philosopher %d is thinking for %d seconds... (Total think time:%d)\n",j,thinkTime,totalThinkTime);
		sleep(thinkTime);

		//Get first chopstick
		error = pthread_mutex_lock(&chopsticks[first]);  //'Lock'
		if(error) {
                        fprintf(stderr, "pthread_mutex_lock error: %d %s\n",error,strerror(error));
                        exit(1);
                }

		//Get second chopstick
		error = pthread_mutex_lock(&chopsticks[second]); //'Lock'
		if(error) {
                        fprintf(stderr, "pthread_mutex_lock error: %d %s\n",error,strerror(error));
                        exit(1);
                }

		//Eat
		eatTime = randomGaussian(9,3);
                if(eatTime < 0) {  //Don't want to eat for a negative amount of time
                        eatTime = 0;
                }
                totalEatTime += eatTime;
                printf("Philosopher %d is eating for %d seconds... (Total eat time: %d)\n",j,eatTime,totalEatTime);
		sleep(eatTime);

		//Put down first chopstick
		error = pthread_mutex_unlock(&chopsticks[first]);  //'Unlock'
		if(error) {
                        fprintf(stderr, "pthread_mutex_unlock error: %d %s\n",error,strerror(error));
                        exit(1);
                }

		//Put down second chopstick	
		error = pthread_mutex_unlock(&chopsticks[second]); //'Unlock'
		if(error) {
                        fprintf(stderr, "pthread_mutex_unlock error: %d %s\n",error,strerror(error));
                        exit(1);
                }
	}
	
	printf("\nPHILOSOPHER %d DONE EATING, WAITING... (Eat time: %d, Think time: %d)\n\n",j,totalEatTime,totalThinkTime);
        return(NULL);
}

int main(int argc, char **argv) {
	pthread_t philosopher[5];  //Create a set of threads, one for each philosopher
	
	srand(time(0)); //Seed random number generator for rand()

        for(int i = 0; i < 5; i++) { //Initialize all 5 chopstick mutex semaphores (unlock)
		int error = pthread_mutex_init(&chopsticks[i], NULL);
		if(error) {
			fprintf(stderr, "pthread_mutex_init error: %d %s\n",error,strerror(error));
                        exit(1);
		}
   	}

   	for(int i = 0; i < 5; i++) {  //Start all 5 threads in the calling process
		//Pass i to keep track of which thread is which philosopher
      		int error = pthread_create(&philosopher[i], NULL, eatThinkTime, (void *)i);
		if(error) {
			fprintf(stderr, "pthread_create error: %d %s\n",error,strerror(error));
    			exit(1);
		}
	}

   	for(int i = 0; i < 5; i++) {  //Wait for all 5 threads to terminate (all 5 philosophers must finish eating)
      		int error = pthread_join(philosopher[i], NULL);
		if(error) {
                        fprintf(stderr, "pthread_join error: %d %s\n",error,strerror(error));
                        exit(1);
                }
	}

	printf("All philosophers finished eating! Everyone leaves the table...\n");

   	for(int i = 0; i < 5; i++) {  //'Destroy' (uninitialize) the 5 chopstick mutex semaphores
      		int error = pthread_mutex_destroy(&chopsticks[i]);
		if(error) {
                        fprintf(stderr, "pthread_mutex_destroy error: %d %s\n",error,strerror(error));
                        exit(1);
                }

	}

   	return(0);
}
