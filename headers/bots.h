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
	struct TokenNode playedTokens;
	struct TrustingLevel* trustLevels;
	float oddsToStartWithSkull; // when the first token is played
	float initialOddsToAddToken; // basically the odds to put the 2nd token
	float oddsToAddMoreTokens; // decreases as more tokens are played
	float oddsToAddSkull; // if the bot is adding a token instead of betting
	float oddsToBluff; // only matters if its skull token is being used
	float oddsToGoBerserk; // The omega bet: going all in
	float confidenceToIncreaseBet; // it's only part of the formula to determine the odds of increasing the bet
};

struct TrustingLevel* initializeTrust(int botID, int numOfBots) {
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

struct Bot* initializeBots(int numOfBots) {
	int id = 0;
	struct Bot* botsList = (struct Bot*)malloc(numOfBots * sizeof(struct Bot));
	struct Token* baseHand = initializeHand();

	for(int i = 0; i < numOfBots; i++) {
		struct TrustingLevel* baseTrustLevels = initializeTrust(id, numOfBots);
		botsList[id].botID = id;
		botsList[id].roundsWon = 0;
		memcpy(botsList[id].hand, baseHand, HAND_SIZE * sizeof(baseHand[0]));
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
	return botsList;
}

// for memory management ease
void freeBots(struct Bot* bots, int numOfBots) {
	for(int i = 0; i < numOfBots; i++) {
		free(bots[i].trustLevels);
	}
	free(bots);
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

#endif