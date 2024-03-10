#include <stdio.h>
#include <stdlib.h>
#include "bots.h" // header containing the Bot struct, its dependencies and some handy functions
#include "simgame.h" // header containing the necessary tools to simulate 1 game of Skull

/* the first argument should be the number of games simulated
   and the second should be the number of bots playing (3 to 6) */
int main(int argc, char const *argv[]) {

	int numOfGames = atoi(argv[1]);
	int numOfBots = atoi(argv[2]);
	struct Bot* bots; // pointer to array objets (bots)

	if (3 > numOfBots || 6 < numOfBots) {
		printf("you can't simulate with %d bots :[\nplease try again with 3 to 6 bots :]", numOfBots);
		return 1;
	} else {
		bots = initializeBots(numOfBots);
	}

	for (int i = 0; i < numOfGames; i++){
		//simulateGame();
	}

	return 0;
}