#ifndef SIMGAME_H
#define SIMGAME_H

#include <stdlib.h>
#include "simround.h"

const int NULL_BOT_ID = -1;
const int NULL_FINAL_BET = -1;

// The specific results of the winner bot (the one we are most curious about)
struct BotGameStats {
	float avgTokensPerRound;
	float avgSkullsPerRound;
	float avgFinalBet;
	float avgTokensRevealed;
};

// The overall statistics and results of all bots together
struct GameResults {
	struct BotStats botsStats[6];
	int winnerID;
	int* deadBotsIDs;
	int roundsPlayed;
	float avgTokensPerRound;
	int avgSkullsPerRound;
	float avgFinalBet;
};

struct GameResults* simulateGame(struct Bot* bots) {
	struct GameResults* gameResults = malloc(sizeof(struct GameResults));
	int gameIsOver = 0; // 0 as False
	int botsWithPoints[6];
	for (int i = 0; i <= 6; i++) {
		botsWithPoints[i] = NULL_BOT_ID;
	}

	while(!gameIsOver) {
		struct RoundResults roundResults = simulateRound(bots);
		// struct RoundResults roundResults = simulateRound(bots);
		// if(roundResults.winnerID != NULL_BOT_ID) {
		// 	if(isInArray(roundResults.winnerID, )) {
		// 		gameResults.winnerStats.winnerID = roundResults.winnerID;
		// 		gameisOver = 1;
		// 	} else {
		// 		insertToArray(roundResults.winnerID, botsWithPoints);
		// 	}
		// }

		gameIsOver = 1;
	}

	return gameResults;
}


#endif