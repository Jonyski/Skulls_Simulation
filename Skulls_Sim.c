#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Bot;
struct Token;
enum TokenStatus;
struct TrustingLevel;

void simulateGame();
void simulateRound();
struct Bot* initializeBots(int numOfBots);

/* the first argument should be the number of games simulated
   and the second should be the number of bots playing (3 to 6) */
int main(int argc, char const *argv[]) {

	int numOfGames = atoi(argv[1]); // the more games simulated, the more accurate the results, I guess...
	int numOfBots = atoi(argv[2]);
	struct Bot* bots; // pointer to array of pointers to objets (bots)

	if (3 > numOfBots || 6 < numOfBots) {
		printf("you can't simulate with %d bots :[\nplease try again with 3 to 6 bots :]", numOfBots);
		return 1;
	} else {
		bots = initializeBots(numOfBots);
	}

	for(int i = 0; i < numOfBots; i++){
		printf("%s\n", _Generic(bots, struct Bot*: "struct Bot", default: "Unknown Type"));
	}


	for (int i = 0; i < numOfGames; i++){
		//simulateGame();
	}

	return 0;
}

enum TokenStatus {
	AVAILABLE,
	USING,
	REMOVED
};

struct Token {
	char tokenType[10];
	enum TokenStatus status;
};

struct TrustingLevel {
	int targetBotID; // the ID of the bot being judged by the other bot
	float totalTrustLevel; // how much the bot is trusted based on the entire game so far
	float momentaryTrustLevel; // how much do we trust his next token is a flower
};

struct Bot {
	int botID;
	struct Token hand[4];
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

   if (baseTrustLevels == NULL) {
      exit(1);
   }

	for(int targetID = 0; targetID < numOfBots; targetID++) {
		if(targetID == botID){
			baseTrustLevels[targetID].targetBotID = targetID;
			baseTrustLevels[targetID].totalTrustLevel = 1.0; // The bot should trust itself
			baseTrustLevels[targetID].momentaryTrustLevel = 1.0;
		} else {
			baseTrustLevels[targetID].targetBotID = targetID;
			baseTrustLevels[targetID].totalTrustLevel = 0.5; // The bot doesn't know at first if it can trust in the other bot
			baseTrustLevels[targetID].momentaryTrustLevel = 0.5;
		}
	}
	return baseTrustLevels;
}

struct Token* initializeHand() {
	struct Token* baseHand = (struct Token*)malloc(4 * sizeof(struct Token));

	if (baseHand == NULL) {
      exit(1);
   }
	
	strcpy(baseHand[0].tokenType, "skull"); // 1 skull
	baseHand[0].status = AVAILABLE;
	for(int i = 1; i <= 3; i++) { // 3 flowers
		strcpy(baseHand[i].tokenType, "flower");
		baseHand[i].status = REMOVED;
	}
	return baseHand;
}

struct Bot* initializeBots(int numOfBots) {
	int id = 0;
	struct Bot* botsList = (struct Bot*)malloc(numOfBots * sizeof(struct Bot));
	struct Token* baseHand = initializeHand();

	for (int i = 0; i < numOfBots; i++) {
		struct TrustingLevel* baseTrustLevels = initializeTrust(id, numOfBots);
		botsList[id].botID = id;
		memcpy(botsList[id].hand, baseHand, sizeof(baseHand));
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

