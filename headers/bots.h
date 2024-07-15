#ifndef BOTS_H
#define BOTS_H

#define HAND_SIZE 4

// AVAILABLE tokens are on hand, USING tokens are on the pile, and REMOVED tokens were lost
enum TokenStatus {
	AVAILABLE,
	USING,
	REVEALED,
	REMOVED
};

// a singular token that can be of type "flower" or "skull"
struct Token {
	char *tokenType;
	enum TokenStatus status;
};

struct TrustingLevel {
	int targetBotID; // the ID of the bot being judged by the other bot
	float totalTrustLevel; // how much the bot is trusted based on the entire game so far
	float momentaryTrustLevel; // how much do we trust his next token is a flower
};

// nodes of a linked list that represents the pile of played tokens
struct TokenNode {
	struct Token token;
	struct TokenNode *next;
};

// probably the most important struct, it represents the evolving bots
struct Bot {
	int botID;
	int isAlive;
	int roundsWon;
	struct Token hand[HAND_SIZE];
	struct TokenNode *playedTokens;
	struct TrustingLevel *trustLevels; // will be used to determine the bet
	float oddsToStartWithSkull; // when the first token is played
	float initialOddsToAddToken; // basically the odds to put the 2nd token
	float oddsToAddMoreTokens; // decreases as more tokens are played
	float oddsToAddSkull; // if the bot is adding a token instead of betting
	float bettingConfidence; // overall confidence to make bets
	float bluffBoldness; // determines how likely a bot is to bluff a high bet
	float oddsToGoBerserk; // The omega bet: going all in
	float confidenceToIncreaseBet; // it's only part of the formula to determine the odds of increasing the bet
};

struct TrustingLevel *initializeTrust(int botID);

struct Token *initializeHand();

struct Bot *initializeBots();

// for memory management ease
void freeBots(struct Bot *bots);

int hasSkull(struct Bot bot);

int hasFlower(struct Bot bot);

int setTokenToUsing(struct Bot *bot, char tokenType);

// recursive algorithm to free the playedTokens piles
void freePile(struct TokenNode *node);

// used at the end of each round to reset hand and pile
void softResetBotHand(struct Bot *bot);

// sets the bot hand to its initial status (all tokens are available)
void hardResetBotsHands();

int getPileSize(struct Bot bot);

// get the number of tokens played by all bots together
int getTotalTokensPlayed();

// get the number of skulls played by all bots together
int getTotalSkullsPlayed();

float generateTrustScore(struct Bot *bot);

int hasPlayedSkull(struct Bot *bot);

float calculateBotConfidence(struct Bot *bot, int currentBet);

int bluff(float boldness, float confidence, int numOfTokens);

int calculateBotBet(struct Bot *bot, int currentBet);

// returns the botID of the bot most trusted by the one passed as argument
int getMostTrustedBot(struct Bot bot);

void removeToken(struct Bot *bot);

#endif