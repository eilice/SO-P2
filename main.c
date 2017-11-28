// Alicja Kołodziejska

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

// w spiżarni osadnicy trzymają mięso, w kuchni przyrządza i przechowuje się jedzenie
sem_t kitchen, pantry; 
int food, quarry;
const int night_length = 1;
const int days = 4;
char *ck, *ht;

int people_left;

void eat(char *type, char *name) {
	sem_wait(&kitchen);		// obiad
	if(food) {
		food--;
		printf("%s %s je obiad, zostalo %d\n", type, name, food); 
		sem_post(&kitchen);
	}
	else {
		printf("%s %s obrazil sie i odszedl\n", type, name);
		sem_post(&kitchen);
		people_left--;
		pthread_exit(NULL);
	}
}

void *cook(void *name) {
	int my_meat = 0;
	for(int i=0; i<days; i++) {
		printf("Kucharz %s wstaje\n", (char*) name); 
		sem_wait(&pantry);		// pobranie mięsa
		if(quarry) {
			my_meat = 1;
			quarry--;
			printf("Kucharz %s wzial mieso, zostalo %d\n", (char*) name, quarry);
		}
		sem_post(&pantry);
		if(my_meat) {				// przyrządzenie posiłku
			sem_wait(&kitchen);
			food += rand()%6 + 1;
			printf("Kucharz %s przygotowal obiad, mamy %d\n", (char*) name, food);
			sem_post(&kitchen);
			my_meat = 0;
		}
		eat(ck, (char*)name);
		printf("Kucharz %s idzie spac\n", (char*) name); 
		sleep(night_length);
	}
	pthread_exit(NULL);
}

void *hunt(void *name) {
	for(int i=0; i<days; i++) {
		printf("Mysliwy %s wstaje\n", (char*) name); 
		if(rand()%6 > rand()%6) {	// polowanie
			sem_wait(&pantry);
			quarry++;
			printf("Mysliwy %s upolowal, mamy %d miesa\n", (char*) name, quarry);
			sem_post(&pantry);
		}
		eat(ht, (char*) name);
		printf("Mysliwy %s idzie spac\n", (char*) name); 
		sleep(night_length);
	}
	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	if(argc != 5) {
		printf("Niepoprawne parametry programu\n");
		return 0;
	}
	
	sem_init(&kitchen, 0, 1);
	sem_init(&pantry, 0, 1);
	
	ck = "Kucharz", ht = "Mysliwy";
	
	srand(time(0));
	
	int no_hunters = atoi(argv[1]), no_cooks = atoi(argv[2]);
	people_left = no_hunters + no_cooks;
	
	quarry = atoi(argv[3]);
	food = atoi(argv[4]);
	
	printf("%d mysliwych\n%d kucharzy\n%d miesa\n%d jedzenia\n\n", no_hunters, no_cooks, quarry, food);
	
	pthread_t hunters[no_hunters], cooks[no_cooks];
	char hunters_names[no_hunters][20], cooks_names[no_cooks][20];
	
	for(int i=0; i<no_hunters; i++){
		sprintf(hunters_names[i], "%d", i+1);
		pthread_create(hunters+i, NULL, hunt, (void*) (hunters_names + i));
	}
	for(int i=0; i<no_cooks; i++){
		sprintf(cooks_names[i], "%d", i+1);
		pthread_create(cooks+i, NULL, cook, (void*) (cooks_names + i));
	}
	
	for(int i=0; i<no_hunters; i++) {
		pthread_join(hunters[i], NULL);
	}
	for(int i=0; i<no_cooks; i++) {
		pthread_join(cooks[i], NULL);
	}
	
	printf("\nNa koniec: %d miesa, %d jedzenia, %d ludzi\n\n", quarry, food, people_left);
	
	return 0;
}
