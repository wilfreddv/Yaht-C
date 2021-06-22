#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ncurses.h>

#include "five_dice.h"


char* ntos(enum SCORES score_t) {
    switch(score_t) {
        case ONES: return "Ones";
        case TWOS: return "Twos";
        case THREES: return "Threes";
        case FOURS: return "Fours";
        case FIVES: return "Fives";
        case SIXES: return "Sixes";
        case THREE_OAK: return "3 of a kind";
        case FOUR_OAK: return "4 of a kind";
        case FULL_HOUSE: return "Full house";
        case SM_STRAIGHT: return "Sm. straight";
        case LG_STRAIGHT: return "Lg. straight";
        case YAHTZEE: return "Yahtzee";
        case CHANCE: return "Chance";
        default: return "Error";
    }
}


int sum_of_dice(Dice dice) {
    int total = 0;
    for(int i=0; i<NUM_DICE; i++) {
        total += dice[i];
    }

    return total;
}


int roll_die() {
    return (rand() % 6) + 1;
}


void roll_dice(Dice dice) {
    for(int i=0; i<NUM_DICE; i++) {
        if( dice[i] == 0 )
            dice[i] = roll_die();
    }
}


void print_dice(Dice dice, Dice to_keep) {
    if( to_keep == NULL ) {
        printw("%d  %d  %d  %d  %d",
               dice[0], dice[1], dice[2],
               dice[3], dice[4]);
    }
    else {
        for(int i=0; i<NUM_DICE; i++) {
            if( to_keep[i] ) attron(COLOR_PAIR(1));
            printw("%d", dice[i]);
            attroff(COLOR_PAIR(1));
            printw("  ");
        }
        printw("\n");
    }

    refresh();
}


int sum_upper(Card card) {
    int total = 0;
    for(int i=0; i<THREE_OAK; i++) {
        if( card[i] > 0 ) total += card[i];
    }
    return total;
}


int sum_lower(Card card) {
    int total = 0;
    for(int i=THREE_OAK; i<NUM_SCORES; i++) {
        if( card[i] > 0 ) total += card[i];
    }
    return total;
}


void print_card(Card card, Dice dice, Card possibilities) {
    int longest = 14; // Magic var: longest string ntos can produce
    for(int i=0; i<NUM_SCORES; i++) {
        int subt_upper = sum_upper(card);
        int bonus = subt_upper >= 63 ? 35 : 0;
        int t_upper = subt_upper + bonus;
        int t_lower = sum_lower(card);

        if( i == THREE_OAK ) {
            printw("%*s: %d\n", longest, "Subtotal upper", subt_upper);
            printw("%*s: %d\n", longest, "Bonus", bonus);
            printw("%*s: %d\n\n", longest, "Total upper", t_upper);
        }


        int score = card[i];
        if( score < 0 )
            printw("%*s: %s %d\n", longest, ntos(i), "_", possibilities[i]);
        else
            printw("%*s: %d\n", longest, ntos(i), card[i]);


        if( i == CHANCE ) {
            printw("%*s: %d\n", longest, "Total upper", t_upper);
            printw("%*s: %d\n", longest, "Total lower", t_lower);
            printw("%*s: %d\n\n", longest, "Total", t_lower + t_upper);
        }
    }
    
    refresh();
}


void possible_combinations(Card possibilities, Dice dice) {
    memset(possibilities, 0, NUM_SCORES * sizeof(int));
    
    int eye_count[6] = {0};
    for(int i=0; i<NUM_DICE; i++) {
        possibilities[dice[i]-1] += dice[i];
        eye_count[dice[i]-1]++;
    }

    for(int i=0; i<7; i++) {
        if( eye_count[i] == 3 ) {
            for(int j=0; j<7; j++) {
                if( eye_count[j] == 2 )
                    possibilities[FULL_HOUSE] = 25;
            }
            possibilities[THREE_OAK] = sum_of_dice(dice);
        }
        else if( eye_count[i] == 4 ) {
            possibilities[FOUR_OAK] = sum_of_dice(dice);
            possibilities[THREE_OAK] = sum_of_dice(dice);
        }
        else if( eye_count[i] == 5 ) {
            possibilities[YAHTZEE] = 50;
            // Yahtzee!
            break;
        }
    }

    int consecutive = 0;
    for(int i=0; i<7; i++) {
        if( eye_count[i] ) consecutive++;
        else consecutive = 0;

        if( consecutive == 4 ) possibilities[SM_STRAIGHT] = 30;
        if( consecutive == 5 ) possibilities[LG_STRAIGHT] = 40;
    }

    possibilities[CHANCE] = sum_of_dice(dice);
}


int find_next_possibility(Card card, int pos) {
    // Find next possibility that isn't on the card yet
    int i;
    for(i=pos+1; i<NUM_SCORES+pos; i++) {
        if( card[i % NUM_SCORES] == -1 ) // empty
            break;
    }
    return i % NUM_SCORES;
}


int find_prev_possibility(Card card, int pos) {
    for(int i=0; i<NUM_SCORES; i++) {
        pos = pos ? pos-1 : NUM_SCORES-1;
        if( card[pos] == -1 )
            break;
    }
    return pos;
}


int choose_slot(Card card, Dice dice, Card possibilities, int can_exit) {
    int pos = find_next_possibility(card, -1);

    while(1) {
        clear();
        int longest = 14; // Magic var: longest string ntos can produce
        for(int i=0; i<NUM_SCORES; i++) {
            int score = card[i];
            int subt_upper = sum_upper(card);
            int bonus = subt_upper >= 63 ? 35 : 0;
            int t_upper = bonus + subt_upper;
            int t_lower = sum_lower(card);

            if( i == THREE_OAK ) {
                printw("%*s: %d\n", longest, "Subtotal upper", subt_upper);
                printw("%*s: %d\n", longest, "Bonus", bonus);
                printw("%*s: %d\n\n", longest, "Total upper", t_upper);
            }

            if( i == pos )
                attron(COLOR_PAIR(1));

            if( score < 0 )
                printw("%*s: %s %d\n", longest, ntos(i), "_", possibilities[i]);
            else
                printw("%*s: %d\n", longest, ntos(i), card[i]);
            
            if( i == pos )
                attroff(COLOR_PAIR(1));

            
            if( i == CHANCE ) {
                printw("%*s: %d\n", longest, "Total upper", t_upper);
                printw("%*s: %d\n", longest, "Total lower", t_lower);
                printw("%*s: %d\n\n", longest, "Total", t_lower + t_upper);
            }
        }
    
        refresh();
        

        printw("Choose a slot to fill out. <ENTER> to select.");
        if( can_exit ) printw(" `q` to go back.");
        printw("\n");
        print_dice(dice, NULL);


        int c = getch();
        switch( c ) {
            case 'q':
                if( can_exit ) return 0;
            case KEY_UP:
                pos = find_prev_possibility(card, pos);
                break;
            case KEY_DOWN:
                pos = find_next_possibility(card, pos);
                break;
            case '\n':
                card[pos] = possibilities[pos];
                clear();
                return 1;
        }

    }

    clear();
    return 1;
}


void play_turn(Card card, Dice dice) {
    for(int roll=1; roll<=3; roll++) {
        printw("Roll %d/3\n", roll);
        roll_dice(dice);
        Card possibilities;
        possible_combinations(possibilities, dice);
        
        print_card(card, dice, possibilities);

        // Choose dice to keep
        if( roll != 3 ) {
            printw("Select dice you want to keep <1..5>, or fill out a slot <SPACE>.\n");
            print_dice(dice, NULL);
            int x, y;
            getyx(stdscr, y, x);
            char c;
            Dice to_keep = {0};

            while( (c = getch()) != '\n' ) {
                switch(c) {
                    case '1':
                        to_keep[0] = !to_keep[0];
                        break;
                    case '2':
                        to_keep[1] = !to_keep[1];
                        break;
                    case '3':
                        to_keep[2] = !to_keep[2];
                        break;
                    case '4':
                        to_keep[3] = !to_keep[3];
                        break;
                    case '5':
                        to_keep[4] = !to_keep[4];
                        break;
                    case ' ':
                        move(y-1, 0);
                        clrtoeol();
                        move(y, 0);
                        clrtoeol();
                        move(y-1, 0);
                        if( choose_slot(card, dice, possibilities, 1) ) return;
                        clear();
                        printw("Roll %d/3\n", roll);
                        print_card(card, dice, possibilities);
                        printw("Select dice you want to keep <1..5>, or fill out a slot <SPACE>.\n");
                        break;
                }

                move(y, 0);
                print_dice(dice, to_keep);
            }

            // Zero out dice to reroll
            for(int i=0; i<NUM_DICE; i++) {
                if( !to_keep[i] )
                    dice[i] = 0;
            }
        }
        else {
            choose_slot(card, dice, possibilities, 0);
        } // Dice to keep

        clear();
    }
}


int main(int argc, char** argv) {
    if( argc == 2 && strcmp(argv[1], "help") == 0 ) {
        printf("%s\n", HELP_MSG);
        return 0;
    }

    initscr();
    start_color();
    noecho();
    curs_set(0);
    keypad(stdscr, 1);

    init_pair(1, COLOR_BLACK, COLOR_YELLOW);

    srand(time(NULL));

    Dice dice;
    Card card;
    memset(card, -1, sizeof(card));

    for(int i=0; i<NUM_SCORES; i++) {
        memset(dice, 0, sizeof(dice));
        play_turn(card, dice);
    }

    int t_upper = sum_upper(card);
    t_upper += t_upper >= 63 ? 35 : 0;
    int t_lower = sum_lower(card);

    printw("Total upper: %d\n", t_upper);
    printw("Total lower: %d\n", t_lower);
    printw("Final score: %d\n", t_upper+t_lower);

    getch();
    endwin();
}
