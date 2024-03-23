#ifndef SIMROUND_H
#define SIMROUND_H

#define rand_float (float)rand()/(float)RAND_MAX
const int NULL_BOT_ID = -1;
const int NULL_FINAL_BET = -1;

enum RoundPhase {
	SETUP_PHASE,
	TOKENS_PHASE,
	BETTING_PHASE,
	REVEALING_PHASE
};

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

// makes a bot add its first token to its pile
void selectFirtToken(struct Bot bot) {
	struct Token newToken;
	newToken.status = USING;

	if(rand_float <= bots[i].oddsToStartWithSkull) {
		newToken.tokenType = "skull";
	} else {
		newToken.tokenType = "flower";
	}

	struct TokenNode firstTokenNode;
	firstTokenNode.token = newToken;
	firstTokenNode.next = NULL;

	bots[i].playedTokens = firstTokenNode;
}

// makes all bots add their first token to their piles
void simulateFirstTokenSelection(struct Bot* bots, int numOfBots){
	for (int i = 0; i < numOfBots; i++) {
		selectFirtToken(bots[i]);
		//testing
		printf("%s\n", bots[i].playedTokens.token.tokenType);
	}
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  
	The round goes through 4 phases:
	1- First token selection
	2- Piling Tokens
	3- Betting
	4- Revealing Tokens 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
struct RoundResults* simulateRound(struct Bot* bots, int startingBotID) {
	struct RoundResults *roundResults = malloc(sizeof(struct RoundResults));

	int currentPlayingBotID = startingBotID;
	int roundIsOver = 0; // 0 as False
	int numOfBots = sizeof(bots) / sizeof(bots[0]);

	simulateFirstTokenSelection();

	for (int i = currentPlayingBotID; !roundIsOver; i++) {
		

		// updates the current playing bot with their order in mind
		currentPlayingBotID = i % numOfBots;
	}

	return roundResults
}

#endif