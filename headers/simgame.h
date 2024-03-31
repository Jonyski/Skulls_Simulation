#ifndef SIMGAME_H
#define SIMGAME_H

#define MAX_BOTS 6

// The specific results of the winner bot (the one we are most curious about)
struct BotGameStats {
	float avgTokensPerRound;
	float avgSkullsPerRound;
	float avgFinalBet;
	float avgTokensRevealed;
};

// The overall statistics and results of all bots together
struct GameResults {
	struct BotGameStats botsStats[MAX_BOTS];
	int winnerID;
	int deadBots[MAX_BOTS];
	int roundsPlayed;
	float avgTokensPerRound;
	float avgSkullsPerRound;
	float avgFinalBet;
};

void updateGameResults(struct GameResults* gameResults, struct RoundResults* roundResults) {
	gameResults->roundsPlayed++;
	// just to make the rest of the function less cumbersome, trust da master
	int rp = gameResults->roundsPlayed;
	int tp = roundResults->tokensPlayed;
	int sp = roundResults->skullsPlayed;
	int fb = roundResults->finalBet;

	if(gameResults->roundsPlayed == 1) {
		gameResults->avgTokensPerRound = tp;
		gameResults->avgSkullsPerRound = sp;
		gameResults->avgFinalBet = fb;
		printf("roundsPlayed: %d\n", rp);
		printf("avg tokens played per round: %lf\n", gameResults->avgTokensPerRound);
		printf("avg skulls per round: %lf\n", gameResults->avgSkullsPerRound);
		printf("avg final bet: %lf\n\n", gameResults->avgFinalBet);
	} else {
		gameResults->avgTokensPerRound = (float)((gameResults->avgTokensPerRound * (rp - 1)) + tp) / rp;
		gameResults->avgSkullsPerRound = (float)((gameResults->avgSkullsPerRound * (rp - 1)) + sp) / rp;
		gameResults->avgFinalBet = (float)((gameResults->avgFinalBet * (rp - 1)) + fb) / rp;
	}
}

int shouldBotDie(struct Bot bot){
	int cardsRemoved = 0;
	for(int i = 0; i < HAND_SIZE; i++) {
		if(bot.hand[i].status == REMOVED) {
			cardsRemoved++;
		}
	}
	return cardsRemoved == HAND_SIZE;
}

// returns ID of last bot alive or the NULL_BOT_ID if there's more than one alive
int isThereOneSurvivor() {
	int survivorDetected = 0;
	int survivorID;
	for(int i = 0; i < numOfBots; i++) {
		if(bots[i].isAlive && !survivorDetected) {
			survivorDetected = 1;
			survivorID = bots[i].botID;
		} else if(bots[i].isAlive && survivorDetected) {
			return NULL_BOT_ID;
		}
	}
	return survivorID;
}

struct GameResults* simulateGame() {
	struct GameResults* gameResults = malloc(sizeof(struct GameResults));
	gameResults->roundsPlayed = 0;
	int gameIsOver = 0; // 0 as False
	int botsWithPoints[numOfBots]; // botsWithPoints[id] becomes 1 when the bot with that id wins a round
	for (int i = 0; i < numOfBots; i++) {
		botsWithPoints[i] = NULL_BOT_ID;
	}

	while(!gameIsOver) {
		struct RoundResults* roundResults = simulateRound(bots, 0);

		// Process the round result data
		updateGameResults(gameResults, roundResults);


		// check for deaths and win conditions
		if(roundResults->loserID != NULL_BOT_ID) {
			if(shouldBotDie(bots[roundResults->loserID])) {
				bots[roundResults->loserID].isAlive = 0; // kill bot if necessary
				gameResults->deadBots[roundResults->loserID] = 1;
			}
		}

		if(botsWithPoints[roundResults->winnerID] == 0) {
			botsWithPoints[roundResults->winnerID] = 1; // the bot won its first round
		} else {
			gameResults->winnerID = roundResults->winnerID; // the bot won this game
		}

		int survivorID;
		if((survivorID = isThereOneSurvivor()) != NULL_BOT_ID) {
			gameResults->winnerID = survivorID;
		}

		free(roundResults);
		gameIsOver = 1;
	}

	return gameResults;
}


#endif