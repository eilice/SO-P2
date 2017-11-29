// Alicja Kołodziejska

#define _BSD_SOURCE // do usleep()

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>

// w spiżarni osadnicy trzymają mięso, w kuchni przyrządza i przechowuje się jedzenie
pthread_mutex_t kitchen = PTHREAD_MUTEX_INITIALIZER, pantry = PTHREAD_MUTEX_INITIALIZER; 
int food, quarry;
const int night_length = 1000;
const int days = 365;

int people_left;

void eat() {
	pthread_mutex_lock(&kitchen);			// obiad
	if(food) {
		food--;
		pthread_mutex_unlock(&kitchen);
	}
	else {
		pthread_mutex_unlock(&kitchen);
		people_left--;
		pthread_exit(NULL);
	}
}

void *cook(void *name) {
	int my_meat = 0;
	for(int i=0; i<days; i++) {
		pthread_mutex_lock(&pantry);		// pobranie mięsa
		if(quarry) {
			my_meat = 1;
			quarry--;
		}
		pthread_mutex_unlock(&pantry);
		if(my_meat) {						// przyrządzenie posiłku
			pthread_mutex_lock(&kitchen);
			food += rand()%6 + 1;
			pthread_mutex_unlock(&kitchen);
			my_meat = 0;
		}
		eat();
		usleep(night_length);
	}
	pthread_exit(NULL);
}

void *hunt(void *name) {
	for(int i=0; i<days; i++) {
		if(rand()%6 > rand()%6) {			// polowanie
			pthread_mutex_lock(&pantry);
			quarry++;
			pthread_mutex_unlock(&pantry);
		}
		eat();
		usleep(night_length);
	}
	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	if(argc != 5) {
		printf("Niepoprawne parametry programu\n");
		return 0;
	}
	
	srand(time(0));
	
	int no_hunters = atoi(argv[1]), no_cooks = atoi(argv[2]);
	people_left = no_hunters + no_cooks;
	
	quarry = atoi(argv[3]);
	food = atoi(argv[4]);
	
	pthread_t hunters[no_hunters], cooks[no_cooks];
	
	for(int i=0; i<no_hunters; i++){
		pthread_create(hunters+i, NULL, hunt, NULL);
	}
	for(int i=0; i<no_cooks; i++){
		pthread_create(cooks+i, NULL, cook, NULL);
	}
	
	for(int i=0; i<no_hunters; i++) {
		pthread_join(hunters[i], NULL);
	}
	for(int i=0; i<no_cooks; i++) {
		pthread_join(cooks[i], NULL);
	}
	
	//printf("Na koniec: %d miesa, %d jedzenia, %d ludzi\n\n", quarry, food, people_left);
	
	return 0;
}
