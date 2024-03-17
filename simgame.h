#ifndef SIMGAME_H
#define SIMGAME_H

const int NULL_BOT_ID = -1;

// The specific results of the winner bot (the one we are most curious about)
struct WinnerStats {
	int winnerID;
	float avgTokensPerRound;
	float avgSkullsPerRound;
	float avgFinalBet;
};

// The overall statistics and results of all bots together
struct GameResults {
	struct WinnerStats winnerStats;
	int* deadBotsIDs;
	int roundsPlayed;
	float avgTokensPerRound;
	int avgSkullsPerRound;
	float avgFinalBet;
	int mostTrustedBotID;
	int leastTrustedBotID;
};

struct GameResults* simulateGame(struct Bot* bots) {
	int gameIsOver = 0;
	int botsWithPoints[6];
	for (int i = 0; i <= 6; i++) {
		botsWithPoints[i] = NULL_BOT_ID;
	}
	struct GameResults *gameResults = malloc(sizeof(struct GameResults));

	while(!gameIsOver) {
		// struct RoundResults roundResults = simulateRound(bots);
		// if(roundResults.winnerID != NULL_BOT_ID) {
		// 	if(isInArray(roundResults.winnerID, )) {
		//		gameResults.winnerStats.winnerID = roundResults.winnerID;
		//		gameisOver = 1;
		// 	} else {
		//		insertToArray(roundResults.winnerID, botsWithPoints);
		//	}
		// }
		gameIsOver = 1;
	}

	return gameResults;
}


#endif