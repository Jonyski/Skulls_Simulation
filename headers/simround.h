#ifndef SIMROUND_H
#define SIMROUND_H

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

	struct TokenNode* firstTokenNode = malloc(sizeof(struct TokenNode));
	firstTokenNode->token = newToken;
	firstTokenNode->next = NULL;

	bot->playedTokens = firstTokenNode;

	setTokenToUsing(bot, newToken.tokenType == "flower" ? 'f' : 's');
}

// makes all bots add their first token to their piles
void simulateSetupPhase(struct Bot* bots){
	for(int i = 0; i < numOfBots; i++) {
		selectFirstToken(&bots[i]);
		// testing
		printf("the bot %d selects the first token \"%s\"\n"
			  , i, bots[i].playedTokens->token.tokenType);
	}
}

void pileToken(struct Bot* bot, char tokenType) {
	struct TokenNode* currentTokenNode = bot->playedTokens;
	int topOfPileReached = 0;

	while(!topOfPileReached) {
		if(currentTokenNode->next != NULL) {
			currentTokenNode = currentTokenNode->next;
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

	currentTokenNode->next = tokenToInsert;

	setTokenToUsing(bot, tokenType);
	printf("the bot %d chose the token %s\n", bot->botID, tokenType == 'f' ? "FLOWER" : "SKULL");
}


int addSecondToken(struct Bot* bot) {
	int botHasSkull = hasSkull(*bot);
	int botHasFlower = hasFlower(*bot);
	int willAddToken = rand_float <= bot->initialOddsToAddToken;

	if(botHasFlower && botHasSkull && willAddToken) {
		pileToken(bot, rand_float <= bot->oddsToAddSkull ? 's' : 'f');
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
	int willAddToken = rand_float <= bot->oddsToAddMoreTokens;

	if(botHasFlower && botHasSkull && willAddToken) {
		pileToken(bot, rand_float <= bot->oddsToAddSkull ? 's' : 'f');
	} else if(!botHasSkull && botHasFlower && willAddToken) {
		pileToken(bot, 'f');
	} else if(!botHasFlower && botHasSkull && willAddToken) {
		pileToken(bot, 's');
	} else {
		return 0;
	}

	return 1;
}

// returns 0 (false) when the bot refuses to add a token
int tryAddingToken(struct Bot* bot) {
	int result;

	if(bot->playedTokens->next == NULL) {
		result = addSecondToken(bot);
	} else {
		result = addMoreToken(bot);
	}

	return result;
}

void makeInitialBet(struct Bot* bot, int *currentBet) {
	int bet = calculateBotBet(bot, 0);
	*currentBet = bet;
	printf("bot %d made an initial bet of %d\n", bot->botID, bet);
}

// returns 0 if the bot refuses to increase the bet
int tryBetting(struct Bot* bot, int *currentBet, int *botWithHighestBet) {
	int bet = calculateBotBet(bot, *currentBet);
	if(bet > *currentBet) {
		*currentBet = bet;
		*botWithHighestBet = bot->botID;
		printf("bot %d made a bet of %d\n", bot->botID, bet);
		return 1;
	} else {
		return 0;
	}
}

void initializeSkipCount(int *botsThatSkipped){
	for(int i = 0; i < numOfBots; i++) {
		botsThatSkipped[i] = 0;
	}
}

int isBettingOver(int *botsThatSkipped) {
	for(int i = 0; i < numOfBots; i++) {
		if(botsThatSkipped[i] == 0){
			return 0;
		}
	}
	return 1;
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  
	The round goes through 4 phases:
	1- First token selection
	2- Piling Tokens
	3- Betting
	4- Revealing Tokens 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
struct RoundResults* simulateRound(struct Bot bots[], int startingBotID) {
	struct RoundResults *roundResults = malloc(sizeof(struct RoundResults));
	enum RoundPhase currentPhase = SETUP_PHASE;
	int currentPlayingBotID = startingBotID;
	int roundIsOver = 0; // 0 as False
	int currentBet = 0;
	int botWithHighestBet;

	// PHASE 1: done
	simulateSetupPhase(bots);
	currentPhase = PILING_PHASE;

	// PHASE 2 loop 
	for(int i = currentPlayingBotID; currentPhase == PILING_PHASE; i++) {
		int pilingIsOver = !(tryAddingToken(&bots[currentPlayingBotID]));

		// PHASE 3 transition
		if(pilingIsOver) {
			printf("BETTING PHASE ENTERED\n");
			makeInitialBet(&(bots[currentPlayingBotID]), &currentBet);
			botWithHighestBet = currentPlayingBotID;
		}
		currentPhase = pilingIsOver ? BETTING_PHASE : PILING_PHASE;

		// updates the current playing bot while maintaining their order
		currentPlayingBotID = (i + 1) % numOfBots;
	}

	int botsThatSkipped[numOfBots];
	initializeSkipCount(botsThatSkipped);
	// PHASE 3 loop
	for(int i = currentPlayingBotID; currentPhase == BETTING_PHASE; i++) {
		int botSkipped;

		if(!botsThatSkipped[currentPlayingBotID]){
			int botSkipped = !(tryBetting(&bots[currentPlayingBotID], &currentBet, &botWithHighestBet));
		}
		if(botSkipped) {
			botsThatSkipped[currentPlayingBotID] = 1;
		}
		if(isBettingOver(botsThatSkipped)){
			currentPhase = REVEALING_PHASE;
			printf("BETTING IS OVER \n\n");
		}
		currentPlayingBotID = (i + 1) % numOfBots;
	}

	for(int i = 0; i < numOfBots; i++) {
		softResetBotHand(&(bots[i]));
	}
	roundIsOver = 1;
	return roundResults;
}

#endif