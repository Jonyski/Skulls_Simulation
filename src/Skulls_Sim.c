#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct Bot* bots; // pointer to array objets (bots)
int numOfBots;

#include "../headers/bots.h" // contains the Bot struct, its dependencies and some handy functions
#include "../headers/simround.h" // contains the necessary tools to simulate 1 round of Skull
#include "../headers/simgame.h" // contains the necessary tools to simulate 1 game of Skull


/* the first argument should be the number of games simulated
   and the second should be the number of bots playing (3 to 6) */
int main(int argc, char const *argv[]) {
	int numOfGames = atoi(argv[1]);
	numOfBots = atoi(argv[2]);

	if (3 > numOfBots || 6 < numOfBots) {
		printf("you can't simulate with %d bots :[\nplease try again with 3 to 6 bots :]\n", numOfBots);
		return 1;
	} else {
		bots = initializeBots();
	}

	srand(time(NULL));

	for (int i = 0; i < numOfGames; i++){
		struct GameResults* gameResults = simulateGame();
		free(gameResults);
	}



	freeBots(bots);
	return 0;
}