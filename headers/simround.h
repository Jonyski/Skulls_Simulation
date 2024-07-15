#ifndef SIMROUND_H
#define SIMROUND_H

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
	int loserID;
	int tokensPlayed;
	int skullsPlayed;
	int finalBet;
};

// makes a bot add its first token to its pile
void selectFirstToken(struct Bot *bot);

// makes all bots add their first token to their piles
void simulateSetupPhase(struct Bot *bots);

void pileToken(struct Bot *bot, char tokenType);

int addSecondToken(struct Bot *bot);

int addMoreToken(struct Bot *bot);

// returns 0 (false) when the bot refuses to add a token
int tryAddingToken(struct Bot *bot);

void makeInitialBet(struct Bot *bot, int *currentBet);

// returns 0 if the bot refuses to increase the bet
int tryBetting(struct Bot *bot, int *currentBet, int *botWithHighestBet);

// botsThatSkipped[botID] is 0 if the bots[botID] didn't skip and 1 otherwise
void initializeSkipCount(int *botsThatSkipped);

int isBettingOver(int *botsThatSkipped);

void initializeRevealingArr(struct TokenNode **tokensToBeRevealed, int length);

int getNextEmptyIndex(struct TokenNode **tokensArray);

void revealOwnTokens(struct TokenNode *currentNode, struct TokenNode **tokensToBeRevealed, int *tokensLeft);

void revealNextToken(struct Bot *bot, struct TokenNode **tokensToBeRevealed);

int simulateRevealingPhase(struct Bot *bot, int currentBet);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  
	The round goes through 4 phases:
	1- First token selection
	2- Piling Tokens
	3- Betting
	4- Revealing Tokens
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
struct RoundResults *simulateRound(struct Bot *bots, int startingBotID);

#endif