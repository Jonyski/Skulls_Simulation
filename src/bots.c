#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "../headers/skullssim.h"
#include "../headers/bots.h"

struct TrustingLevel *initializeTrust(int botID) {
	struct TrustingLevel *baseTrustLevels = (struct TrustingLevel*)malloc(numOfBots * sizeof(struct TrustingLevel));
	if(baseTrustLevels == NULL) {
		exit(1);
	}

	// The bot should trust itself but doesn't know at first if it can trust in the other bots
	for(int targetID = 0; targetID < numOfBots; targetID++) {
		baseTrustLevels[targetID].targetBotID = targetID;
		if(targetID == botID){
			baseTrustLevels[targetID].totalTrustLevel = 1.0;
			baseTrustLevels[targetID].momentaryTrustLevel = 1.0;
		} else {
			baseTrustLevels[targetID].totalTrustLevel = 0.5;
			baseTrustLevels[targetID].momentaryTrustLevel = 0.5;
		}
	}
	return baseTrustLevels;
}

struct Token *initializeHand() {
	struct Token *baseHand = (struct Token*)malloc(HAND_SIZE * sizeof(struct Token));
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

struct Bot *initializeBots() {
	struct Bot *botsList = (struct Bot*)malloc(numOfBots * sizeof(struct Bot));
	struct Token *baseHand = initializeHand();

	for(int id = 0; id < numOfBots; id++) {
		struct TrustingLevel* baseTrustLevels = initializeTrust(id);
		botsList[id].botID = id;
		botsList[id].isAlive = 1;
		botsList[id].roundsWon = 0;
		memcpy(botsList[id].hand, baseHand, HAND_SIZE * sizeof(baseHand[0]));
		botsList[id].playedTokens = NULL; // TODO: initialize the linked list here already (why the fuck is it not?)
		botsList[id].trustLevels = baseTrustLevels;
		botsList[id].oddsToStartWithSkull = 0.5;
		botsList[id].initialOddsToAddToken = 0.5;
		botsList[id].oddsToAddMoreTokens = 0.5;
		botsList[id].oddsToAddSkull = 0.5;
		botsList[id].bettingConfidence = 0.5;
		botsList[id].bluffBoldness = 0.5;
		botsList[id].oddsToGoBerserk = 0.06;
		botsList[id].confidenceToIncreaseBet = 0.4;
	}
	free(baseHand);
	return botsList;
}

void freeBots(struct Bot *bots) {
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

int setTokenToUsing(struct Bot *bot, char tokenType) {
	char *realTokenType = tokenType == 'f' ? "flower" : "skull";

	for(int i = 0; i < HAND_SIZE; i++) {
		if(bot->hand[i].tokenType == realTokenType && bot->hand[i].status == AVAILABLE) {
			bot->hand[i].status = USING;
			return 1;
		}
	}

	return 0;
}

void freePile(struct TokenNode *node) {
	if(node == NULL) {
		return;
	}
	struct TokenNode* nextNode = node->next;
	free(node);
	freePile(nextNode);
}

void softResetBotHand(struct Bot *bot) {
	for(int i = 0; i < HAND_SIZE; i++) {
		if(bot->hand[i].status == USING || bot->hand[i].status == REVEALED) {
			bot->hand[i].status = AVAILABLE;
		}
	}
	freePile(bot->playedTokens);
	bot->playedTokens = NULL;
}

void hardResetBotsHands() {
	for(int i = 0; i < numOfBots; i++) {
		for(int j = 0; j < HAND_SIZE; j++) {
			bots[i].hand[j].status = AVAILABLE;
		}
		freePile(bots[i].playedTokens);
		bots[i].playedTokens = NULL;
	}
}

int getPileSize(struct Bot bot) {
	int pileSize = 0;
	struct TokenNode *currentTokenNode = bot.playedTokens;
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

int getTotalSkullsPlayed() {
	int skullsPlayed = 0;
	for(int i = 0; i < numOfBots; i++) {
		if(bots[i].hand[0].status == USING) {
			skullsPlayed++;
		}
	}
	return skullsPlayed;
}

float generateTrustScore(struct Bot *bot) {
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

int hasPlayedSkull(struct Bot *bot) {
	if(bot->hand[0].status == USING) {
		return 1;
	}
	return 0;
}

// TODO: reconsider this formula
float calculateBotConfidence(struct Bot *bot, int currentBet) {
	float trustScore = generateTrustScore(bot);
	float confidence = bot->bettingConfidence;
	float situationalConfidence = currentBet == 0 ? bot->bettingConfidence : bot->confidenceToIncreaseBet;
	int playedSkull = hasPlayedSkull(bot);
	float totalConfidence = (5 * trustScore + 3 * ((2 * confidence + 3 * situationalConfidence) / 5)) / 8;
	return totalConfidence;
}

int bluff(float boldness, float confidence, int numOfTokens) {
	confidence = (3 * boldness + confidence) / 4;
	int bluff = ceil(confidence * numOfTokens);
	float variance = rand_float;
	if(variance <= 0.1 && bluff > 0) {
		return bluff - 1;
	} else if(variance >= 0.9 && (bluff + 1 <= numOfTokens)) {
		return bluff + 1;
	} else {
		return bluff;
	}
}

int calculateBotBet(struct Bot *bot, int currentBet) {
	int bet;
	int numOfTokens = getTotalTokensPlayed();
	int playedSkull = hasPlayedSkull(bot);
	float confidence = calculateBotConfidence(bot, currentBet);

	if(playedSkull && currentBet == 0) {
		bet = bluff(bot->bluffBoldness, confidence, numOfTokens);
	} else if(playedSkull && currentBet>0) {
		bet = bluff(bot->bluffBoldness/2, confidence, numOfTokens);
	} else if(rand_float <= bot->oddsToGoBerserk) {
		bet = numOfTokens;
	} else {
		int variance = rand_float;
		bet = ceil(confidence * numOfTokens);

		if(variance <= 0.1 && bet > 0) {
			bet += 1;
		} else if(variance >= 0.9 && (bet + 1 <= numOfTokens)) {
			bet -= 1;
		}
	}
	return bet;
}

int getMostTrustedBot(struct Bot bot) {
	int mostTrustedBot = -1;
	for(int i = 0; i < numOfBots; i++){
		if(i == bot.botID) {
			continue; // the bot obviously trusts himself the most ;^]
		}
		if(mostTrustedBot == -1){
			mostTrustedBot = i;
		} else {
			mostTrustedBot = bot.trustLevels[mostTrustedBot].momentaryTrustLevel > bot.trustLevels[i].momentaryTrustLevel ? mostTrustedBot : i;
		}
	}
	return mostTrustedBot;
}

void removeToken(struct Bot *bot) {
	int tokenRemoved = 0;
	while(!tokenRemoved) {
		int randToken = floor(rand_float * HAND_SIZE);
		if(bot->hand[randToken].status == REMOVED) {
			continue;
		} else {
			bot->hand[randToken].status = REMOVED;
			tokenRemoved = 1;
		}
	}
}