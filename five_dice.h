#define HELP_MSG "How to play"
#define NUM_DICE 5

enum SCORES {
    ONES,
    TWOS,
    THREES,
    FOURS,
    FIVES,
    SIXES,

    THREE_OAK,
    FOUR_OAK,
    FULL_HOUSE,
    SM_STRAIGHT,
    LG_STRAIGHT,
    YAHTZEE,
    CHANCE,

    NUM_SCORES
};

typedef int Dice[NUM_DICE];
typedef int Card[NUM_SCORES];

