#ifndef BOTS_H
#define BOTS_H

#include <stdlib.h>
#include <string.h>

#define HAND_SIZE 4

// AVAILABLE tokens are on hand, USING tokens are on the pile, and REMOVED tokens were lost
enum TokenStatus {
	AVAILABLE,
	USING,
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
	struct TokenNode* next;
};

struct Bot {
	int botID;
	int roundsWon;
	struct Token hand[HAND_SIZE];
	struct TokenNode* playedTokens;
	struct TrustingLevel* trustLevels; // will be used to determine the bet
	float oddsToStartWithSkull; // when the first token is played
	float initialOddsToAddToken; // basically the odds to put the 2nd token
	float oddsToAddMoreTokens; // decreases as more tokens are played
	float oddsToAddSkull; // if the bot is adding a token instead of betting
	float bettingConfidence; // overall confidence to make bets
	float oddsToBluff; // only matters if its skull token is being used
	float oddsToGoBerserk; // The omega bet: going all in
	float confidenceToIncreaseBet; // it's only part of the formula to determine the odds of increasing the bet
};

struct TrustingLevel* initializeTrust(int botID) {
	struct TrustingLevel* baseTrustLevels = (struct TrustingLevel*)malloc(numOfBots * sizeof(struct TrustingLevel));
	if(baseTrustLevels == NULL) {
		exit(1);
	}

   // The bot should trust itself but doesn't know at first if it can trust in the other bots
	for(int targetID = 0; targetID < numOfBots; targetID++) {
		if(targetID == botID){
			baseTrustLevels[targetID].targetBotID = targetID;
			baseTrustLevels[targetID].totalTrustLevel = 1.0;
			baseTrustLevels[targetID].momentaryTrustLevel = 1.0;
		} else {
			baseTrustLevels[targetID].targetBotID = targetID;
			baseTrustLevels[targetID].totalTrustLevel = 0.5;
			baseTrustLevels[targetID].momentaryTrustLevel = 0.5;
		}
	}
	return baseTrustLevels;
}

struct Token* initializeHand() {
	struct Token* baseHand = (struct Token*)malloc(HAND_SIZE * sizeof(struct Token));
	if(baseHand == NULL) {
		exit(1);
	}

	// 1 skull
	baseHand[0].tokenType = "skull";
	baseHand[0].status = AVAILABLE;
	// 3 flowers
	for(int i = 1; i <= 3; i++) {
		baseHand[i].tokenType = "flower";
		baseHand[i].status = AVAILABLE;
	}
	return baseHand;
}

struct Bot* initializeBots() {
	int id = 0;
	struct Bot* botsList = (struct Bot*)malloc(numOfBots * sizeof(struct Bot));
	struct Token* baseHand = initializeHand();

	for(int i = 0; i < numOfBots; i++) {
		struct TrustingLevel* baseTrustLevels = initializeTrust(id);
		botsList[id].botID = id;
		botsList[id].roundsWon = 0;
		memcpy(botsList[id].hand, baseHand, HAND_SIZE * sizeof(baseHand[0]));
		botsList[id].playedTokens = NULL;
		botsList[id].trustLevels = baseTrustLevels;
		botsList[id].oddsToStartWithSkull = 0.5;
		botsList[id].initialOddsToAddToken = 0.5;
		botsList[id].oddsToAddMoreTokens = 0.5;
		botsList[id].oddsToAddSkull = 0.5;
		botsList[id].oddsToBluff = 0.5;
		botsList[id].oddsToGoBerserk = 0.06;
		botsList[id].confidenceToIncreaseBet = 0.4;
		id++;
	}
	free(baseHand);
	return botsList;
}

// for memory management ease
void freeBots(struct Bot* bots) {
	for(int i = 0; i < numOfBots; i++) {
		free(bots[i].trustLevels);
		bots[i].trustLevels = NULL;
	}
	free(bots);
	bots = NULL;
}

int hasSkull(struct Bot bot) {
	for(int i = 0; i < HAND_SIZE; i++) {
		if(bot.hand[i].tokenType == "skull") {
			return (bot.hand[i].status == AVAILABLE)? 1 : 0;
		}
	}
}

int hasFlower(struct Bot bot) {
	for(int i = 0; i < HAND_SIZE; i++) {
		if(bot.hand[i].tokenType == "flower" && bot.hand[i].status == AVAILABLE) {
			return 1;
		}
	}
	return 0;
}

int setTokenToUsing(struct Bot* bot, char tokenType) {
	char* realTokenType = tokenType == 'f' ? "flower" : "skull";

	for(int i = 0; i < HAND_SIZE; i++) {
		if(bot->hand[i].tokenType == realTokenType && bot->hand[i].status == AVAILABLE) {
			bot->hand[i].status = USING;
			return 1;
		}
	}

	return 0;
}

void freePile(struct TokenNode* node) {
	if(node == NULL) {
		return;
	}
	struct TokenNode* nextNode = node->next;
	free(node);
	freePile(nextNode);
}

// used at the end of each round to reset hand and
void softResetBotHand(struct Bot* bot) {
	for(int i = 0; i < HAND_SIZE; i++) {
		if(bot->hand[i].status == USING) {
			bot->hand[i].status = AVAILABLE;
		}
	}
	freePile(bot->playedTokens);
	bot->playedTokens = NULL;
}

int getPileSize(struct Bot bot) {
	int pileSize = 0;
	struct TokenNode* currentTokenNode = bot.playedTokens;
	while(currentTokenNode != NULL) {
		pileSize++;
		currentTokenNode = currentTokenNode->next;
	}
	return pileSize;
}

int getTotalTokensPlayed() {
	int totalTokensPlayed = 0;
	for(int i = 0; i < numOfBots; i++) {
		totalTokensPlayed += getPileSize(bots[i]);
	}
	return totalTokensPlayed;
}

float generateTrustScore(struct Bot* bot) {
	float trustScore = 0;
	int totalTokensPlayed = 0;
	for(int i = 0; i < numOfBots; i++) {
		int pileSize = getPileSize(bots[i]);
		trustScore += pileSize * bot->trustLevels[i].totalTrustLevel;
		totalTokensPlayed += pileSize;
	}
	trustScore = trustScore/totalTokensPlayed;
	return trustScore;
}

int calculateBotBet(struct Bot* bot, int currentBet) {
	// probably goes something like this:
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	|	int bet;
	|	trustScore = generateBotTrustScore()
	|	confidence = bot->bettingConfidence
	|	numOfTokens = getTotalTokensPlayed(bots)
	|	hasPlayedSkull = hasPlayedSkull()
	|	
	|	currentConfidence = trustScore * (confidence * confidence)
	|	
	|	currentConfidence = currentBet > 0 ? currentConfidence * (confidenceToIncrease * confidenceToIncrease) : currentConfidence
	|
	|	if(hasPlayedSkull) {
	|		currentCofidence *= pow(bot->oddsToBluff, 2)
	|   }
	|
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int bet;
	int trustScore = generateTrustScore(bot);
	int confidence = bot->bettingConfidence;
	int numOfTokens = getTotalTokensPlayed();
}

#endif