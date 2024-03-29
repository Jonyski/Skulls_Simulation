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

// botsThatSkipped[botID] is 0 if the bots[botID] didn't skip and 1 otherwise
void initializeSkipCount(int *botsThatSkipped){
	for(int i = 0; i < numOfBots; i++) {
		botsThatSkipped[i] = 0;
	}
}

int isBettingOver(int *botsThatSkipped) {
	int numOfSkips = 0;
	for(int i = 0; i < numOfBots; i++) {
		if(botsThatSkipped[i] == 1){
			numOfSkips++;
		}
	}
	return (numOfSkips == (numOfBots - 1)) ? 1 : 0;
}

void initializeRevealingArr(struct TokenNode** tokensToBeRevealed, int length) {
	for(int i = 0; i < length; i++) {
		tokensToBeRevealed[i] = NULL;
	}
}

int getNextEmptyIndex(struct TokenNode** tokensArray) {
	for(int i = 0; ; i++){
		if(tokensArray[i] == NULL) {
			return i;
		}
	}
}

void revealOwnTokens(struct TokenNode* currentNode, struct TokenNode** tokensToBeRevealed, int *tokensLeft) {
	if(currentNode->next != NULL) {
		revealOwnTokens(currentNode->next, tokensToBeRevealed, tokensLeft);
	}
	if(*tokensLeft > 0) {
		tokensToBeRevealed[getNextEmptyIndex(tokensToBeRevealed)] = currentNode;
		*tokensLeft -= 1;
		printf("the bot revealed his own token: %s\n", currentNode->token.tokenType);
	}
}

void revealNextToken(struct Bot* bot, struct TokenNode** tokensToBeRevealed) {
	int mostTrustedBotID = getMostTrustedBot(*bot);
	int topOfPileReached = 0;
	struct TokenNode* nodeToReveal = bots[mostTrustedBotID].playedTokens;

	while(!topOfPileReached) {
		if(nodeToReveal->next == NULL || nodeToReveal->token.status == REVEALED) {
			topOfPileReached = 1;
		} else {
			nodeToReveal = nodeToReveal->next;
		}
	}

	tokensToBeRevealed[getNextEmptyIndex(tokensToBeRevealed)] = nodeToReveal;
	nodeToReveal->token.status = REVEALED;
	bot->trustLevels[mostTrustedBotID].momentaryTrustLevel -= 0.1;
	printf("bot %d revealed the token of bot %d: %s\n", bot->botID, mostTrustedBotID, nodeToReveal->token.tokenType);
}

int simulateRevealingPhase(struct Bot* bot, int currentBet) {
	int result = 1;
	struct TokenNode* tokensToBeRevealed[currentBet];
	initializeRevealingArr(tokensToBeRevealed, currentBet);
	int tokensLeft = currentBet;

	// reveal its own tokens first
	revealOwnTokens(bot->playedTokens, tokensToBeRevealed, &tokensLeft);

	// reveals others players tokens one by one
	while(tokensLeft > 0) {
		revealNextToken(bot, tokensToBeRevealed);
		tokensLeft--;
	}

	// checks if a skull was revealed and the bot
	for(int i = 0; i < currentBet; i++) {
		if(tokensToBeRevealed[i]->token.tokenType == "skull") {
			result = 0;
		}
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
struct RoundResults* simulateRound(struct Bot bots[], int startingBotID) {
	struct RoundResults *roundResults = malloc(sizeof(struct RoundResults));
	enum RoundPhase currentPhase = SETUP_PHASE;
	int playingBotID = startingBotID;
	int roundIsOver = 0; // 0 as False
	int currentBet = 0;
	int botWithHighestBet;

	printf("THE SETUP PHASE HAS BEGAN\n");
	simulateSetupPhase(bots);
	// PHASE 1: done
	printf("THE SETUP IS OVER\n\n");
	currentPhase = PILING_PHASE;

	printf("THE PILING PHASE PHASE HAS BEGAN\n");
	// PHASE 2 loop 
	for(int i = playingBotID; currentPhase == PILING_PHASE; i++) {
		int pilingIsOver = !(tryAddingToken(&bots[playingBotID]));

		// PHASE 3 transition
		if(pilingIsOver) {
			printf("THE PILINGE IS OVER\n\n");
			printf("THE BETTING PHASE HAS BEGAN\n");
			makeInitialBet(&(bots[playingBotID]), &currentBet);
			botWithHighestBet = playingBotID;
		}
		currentPhase = pilingIsOver ? BETTING_PHASE : PILING_PHASE;

		// updates the current playing bot while maintaining their order
		playingBotID = (i + 1) % numOfBots;
	}

	int botsThatSkipped[numOfBots];
	initializeSkipCount(botsThatSkipped);
	// PHASE 3 loop
	for(int i = playingBotID; currentPhase == BETTING_PHASE; i++) {
		int botSkipped;

		if(!botsThatSkipped[playingBotID]){
			botSkipped = !(tryBetting(&bots[playingBotID], &currentBet, &botWithHighestBet));
		}
		if(botSkipped) {
			botsThatSkipped[playingBotID] = 1;
		}
		if(isBettingOver(botsThatSkipped)){
			currentPhase = REVEALING_PHASE;
			printf("THE BETTING IS OVER \n\n");
			continue;
		}
		playingBotID = (i + 1) % numOfBots;
	}

	playingBotID = botWithHighestBet;
	printf("THE REVEALING PHASE HAS BEGAN\n");
	int revealWasSuccessfull = simulateRevealingPhase(&bots[playingBotID], currentBet);
	printf("THE REVEALING IS OVER\n\n");

	if(revealWasSuccessfull){
		printf("BOT %d WON A ROUND BY REVEALING %d FLOWERS\n\n", playingBotID, currentBet);
	} else {
		printf("BOT %d REVEALED A SKULL AND LOST A ROUND\n\n", playingBotID);
	}

	for(int i = 0; i < numOfBots; i++) {
		softResetBotHand(&(bots[i]));
	}
	roundIsOver = 1;
	return roundResults;
}

#endif