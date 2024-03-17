#ifndef SIMROUND_H
#define SIMROUND_H

#include <stdlib.h>

const int NULL_BOT_ID = -1;
const int NULL_FINAL_BET = -1;

struct BotRoundStats {
	int tokensPlayed;
	int skullsPlayed;
	int finalBet;
	int tokensRevealed; // how many of this bot's tokens were revealed
}

struct RoundResults {
	int winnerID;
	int deadBotID;
	int tokensPlayed;
	int skullsPlayed;
	int finalBet;
};

struct RoundResults* simulateRound(struct Bot* bots) {
	struct RoundResults *roundResults = malloc(sizeof(struct RoundResults))
}

#endif