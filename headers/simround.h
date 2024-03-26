#ifndef SIMROUND_H
#define SIMROUND_H

#define rand_float (float)rand()/(float)RAND_MAX

const int NULL_BOT_ID = -1;
const int NULL_FINAL_BET = -1;

enum RoundPhase {
	SETUP_PHASE, // aka the initial token selection
	PILING_PHASE,
	BETTING_PHASE,
	REVEALING_PHASE
};

struct BotRoundStats {
	int tokensPlayed;
	int skullsPlayed;
	int finalBet;
	int tokensRevealed; // how many of this bot's tokens were revealed
};

struct RoundResults {
	int winnerID;
	int deadBotID;
	int tokensPlayed;
	int skullsPlayed;
	int finalBet;
};

// makes a bot add its first token to its pile
void selectFirstToken(struct Bot* bot) {
	struct Token newToken;
	newToken.status = USING;

	if(rand_float <= bot->oddsToStartWithSkull) {
		newToken.tokenType = "skull";
	} else {
		newToken.tokenType = "flower";
	}

	struct TokenNode firstTokenNode;
	firstTokenNode.token = newToken;
	firstTokenNode.next = NULL;

	bot->playedTokens = firstTokenNode;

	setTokenToUsing(bot, newToken.tokenType == "flower" ? 'f' : 's');
}

// makes all bots add their first token to their piles
void simulateSetupPhase(struct Bot* bots, int numOfBots){
	for(int i = 0; i < numOfBots; i++) {
		selectFirstToken(&bots[i]);
		//testing
		printf("the bot %d selects the first token \"%s\"\n"
			  , i, bots[i].playedTokens.token.tokenType);
	}
}

void pileToken(struct Bot* bot, char tokenType) {
	struct TokenNode currentTokenNode = bot->playedTokens;
	int topOfPileReached = 0;

	while(!topOfPileReached) {
		if(currentTokenNode.next != NULL) {
			currentTokenNode = *(currentTokenNode.next);
		} else {
			topOfPileReached = 1;
		}
	}

	struct Token newToken;
	newToken.tokenType = (tokenType == 'f')? "flower" : "skull";
	newToken.status = USING;

	struct TokenNode* tokenToInsert = malloc(sizeof(struct TokenNode));
	tokenToInsert->token = newToken;
	tokenToInsert->next = NULL;

	currentTokenNode.next = tokenToInsert;

	setTokenToUsing(bot, tokenType);

	printf("the bot %d chose the token %c\n", bot->botID, tokenType);
}


int addSecondToken(struct Bot* bot) {
	int botHasSkull = hasSkull(*bot);
	int botHasFlower = hasFlower(*bot);
	int willAddToken = rand_float < bot->initialOddsToAddToken;

	if(botHasFlower && botHasSkull && willAddToken) {
		pileToken(bot, rand_float < bot->oddsToAddSkull ? 's' : 'f');
	} else if(!botHasSkull && botHasFlower && willAddToken) {
		pileToken(bot, 'f');
	} else if(!botHasFlower && botHasSkull && willAddToken) {
		pileToken(bot, 's');
	} else {
		return 0;
	}

	return 1;
}

int addMoreToken(struct Bot* bot) {
	int botHasSkull = hasSkull(*bot);
	int botHasFlower = hasFlower(*bot);
	int willAddToken = rand_float < bot->oddsToAddMoreTokens;

	if(botHasFlower && botHasSkull && willAddToken) {
		pileToken(bot, rand_float < bot->oddsToAddSkull ? 's' : 'f');
	} else if(!botHasSkull && botHasFlower && willAddToken) {
		pileToken(bot, 'f');
	} else if(!botHasFlower && botHasSkull && willAddToken) {
		pileToken(bot, 's');
	} else {
		return 0;
	}

	return 1;
}

// returns 0 (false) when it refuses to add a token
int tryAddingToken(struct Bot* bot) {
	int result;

	if(bot->playedTokens.next == NULL) {
		result = addSecondToken(bot);
	} else {
		result = addMoreToken(bot);
	}

	return result;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  
	The round goes through 4 phases:
	1- First token selection
	2- Piling Tokens
	3- Betting
	4- Revealing Tokens 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
struct RoundResults* simulateRound(struct Bot bots[], int startingBotID, int numOfBots) {
	struct RoundResults *roundResults = malloc(sizeof(struct RoundResults));
	int currentPlayingBotID = startingBotID;
	int roundIsOver = 0; // 0 as False
	enum RoundPhase currentPhase = SETUP_PHASE;

	// phase 1: done
	simulateSetupPhase(bots, numOfBots);
	currentPhase = PILING_PHASE;

	// phases 2 and 3 are done in a loop because there is a determined sequence to the players actions
	for(int i = currentPlayingBotID; !roundIsOver; i++) {

		if(currentPhase == PILING_PHASE) {
			tryAddingToken(&bots[currentPlayingBotID]);
		}

		// updates the current playing while maintaining their order
		currentPlayingBotID = i % numOfBots;
		roundIsOver = 1;
	}

	return roundResults;
}

#endif