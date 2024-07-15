#ifndef SIMGAME_H
#define SIMGAME_H

#define MAX_BOTS 6

// The specific results of the winner bot (the one we are most curious about)
struct BotGameStats {
	float avgTokensPerRound;
	float avgSkullsPerRound;
	float avgFinalBet;
	float avgTokensRevealed;
};

// The overall statistics and results of all bots together
struct GameResults {
	struct BotGameStats botsStats[MAX_BOTS];
	int winnerID;
	int deadBots[MAX_BOTS];
	int roundsPlayed;
	float avgTokensPerRound;
	float avgSkullsPerRound;
	float avgFinalBet;
};

void updateGameResults(struct GameResults *gameResults, struct RoundResults *roundResults);

int shouldBotDie(struct Bot bot);

// returns ID of last bot alive or the NULL_BOT_ID if there's more than one alive
int isThereOneSurvivor();

struct GameResults *simulateGame();


#endif