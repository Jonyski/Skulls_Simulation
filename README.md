#Skull Simulation

##General observations

This project is being created as an attempt to find the mythological "optimal strategy" that the game's creators claim exists.

I've played Skull a lot, but it's still conceptually hard to create bots for this game (as in I'm not sure about the properties i chose to give them), so if you have any suggestions please create an issue or make a push request.

##What is Skull and how is it played?

Skull is a game about betting and bluffing played by 3 to 6 players.

###Objective

In the game, your objective is to win 2 rounds by turning an X amount of *flower tokens* face up (this X amount is determined in the betting phase of each round, but we will se that later). if nobody manages to win 2 rounds, the last person to survive wins.

###Start

At the start, each player has 4 tokens, of which 3 are *flower tokens* and 1 is a *skull token*. They also have a little platform to pile their tokens whenever they are played.

The first thing that happens in each round is that every player will put 1 token of their choice face down in their platform. Then, the first player will have the choice to put one more token on top of his already played token OR to begin the *betting phase* (that will be explained later). If he choses to add a token to his pile, then the player to his left will have the same choice and so on until someone decides to make a bet and start the betting phase.

In short, the players will pile their tokens one after the other until someone decides to make a bet.

###Betting phase

A bet in Skull is how many flower tokens someone thinks they can turn face up without encountering a skull token. There are some conditions to the order in which the tokens can be revealed that will be explained later.

So, when someone makes a bet and says, for example, "_I make a bet of 3_", we enter the betting phase and no one can add tokens to their pile anymore. From that moment onwards, each consecutive player will have 2 options:

- Make a higher bet
- Fold and move their pile towards the center of the table

and the players will continue doing this until there is only one person has not moved their pile (the person with the highest bet). Now we enter the final phase of the round, because if the person made a bet of 5, they will have to reveal 5 flowers without being killed by a skull.

###Final phase

The person with the highest bet will start revealing the tokens in the order they please as long as they follow these rules

1. You can't start revealing other people's tokens before revealing all of your own tokens
2. You always start revealing tokens from the top of the pile to the bottom, e.g. you can't turn the bottom-most token face up before all of the above have been revealed.

besides that, the choice is yours. For example, you can reveal all of your tokens, then reveal 1 token from P2, then reveal 1 token from P3, go back and reveal another one from P2 and finish revealing 2 tokens from P4. It all depends on who you are trusting the most in the moment.

What happens if you reveal a skull? If the skull is yours, you will chose a token from your hand to be removed, and if the skull is not yours, the owner will pick a random token from your hand to remove. If all your tokens are removed you stop playing.

But, if you manage to reveal as many flowers as you had bet you would reveal, then you won a round. And as has already been stated before, winning 2 rounds means you win the game.

###Common human strategies

The game seems simple, but there are many many many ways to mess with your opponents, bluff, surprise people and optimize your chances. Here's a list of some common tactics:

- Putting a skull as your first token and immediately betting 1 (to make your opponents bet more and reveal your skull, ultimately losing a token)]
- Putting a flower as your first token and immediately betting 1 (to make people think you are doing the previous bluff and fold, giving you a free win)
- Not looking what you are putting in your pile (if even you don't know, how would anyone else?)
- Putting your skull as your top-most token (this is simply good, because tokens are always revealed from top to bottom)
- Putting the same token many times in a row (so that when you change it nobody expects)
- Increasing the bet by 1 even though you have a skull in your pile (basic bluff)
- Being super safe until other players have lost some tokens
- Immediately folding a low bet even though you don't have a skull in your pile (to make people think you DO have a skull and not reveal your flowers)
- Starting the betting phase with an all-in when there aren't many tokens on the table (so that no one can bet higher)
- Pile many flowers (so that you can make high bets)
- etc...