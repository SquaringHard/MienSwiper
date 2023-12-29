#include <conio.h>    // controls and cls()
#include <windows.h>  // option

#include <algorithm>  // shuffle
#include <chrono>     // seeds for rand
#include <iostream>   // output and moai
#include <random>     // rands for shuffle
#include <string>     // output management
#define cout \
    if (text) cout    // option
typedef unsigned short int us;
using namespace std;

bool text = 1,        // option
     cellnum[80][40];  // checks if the cell has bomb

string board, shownboard;
// both store info to cout the mine field, board is for modifying the cells, shownboard copies board and add cursor

us z, numcell;  // z is num of bombs on the field, numcell is num of unrevealed non-bomb cells on the field
char p2;        // player input
struct coord {  // coordinates
    us x, y;
} xy;  // this is the board/field size

void cls() {
    system("cls");
}

void quit() {   // endscreen
    cls();
    cout << (char)-55 << string(80, -51) << (char)-69 << endl
         << (char)-70 << string(29, ' ') << "Thank you for playing!" << string(29, ' ') << (char)-70 << endl
         << (char)-56 << string(80, -51) << (char)-68 << endl;
    getch();
    exit(0);
}

class cursor {  // handles true coord of cursor, cursor shown on screen (dot/arrow) is based on this
public:
    coord board, now;
    // now: coordinate of cursor, board: limits of cursor's coordinates
    // now<board in both x and y

    void update() {  // when player tries to move around
        switch (p2) {
        case 'a':
        case 'A':
            if (now.x) now.x--;
            return;
        case 'w':
        case 'W':
            if (now.y) now.y--;
            return;
        case 'd':
        case 'D':
            if (now.x < board.x - 1) now.x++;
            return;
        case 's':
        case 'S':
            if (now.y < board.y - 1) now.y++;
            return;
        case 27: {
            quit();
        }  // for the "Press ESC at anytime to quit."
        }
    }
    us code() {
        return now.x + now.y * board.x;    // some functions use 1 var, this is for those functions
    }
} g_sor,                                           // Game cursor
o_sor = {{4, 1}, {3, 0}},                      // Option cursor
p_sor = {{2, 2}, {0, 0}},                      // Choose-your-board cursor
s_sor = {{5, 1}, {0, 0}};                      // Start menu cursor

int main();                               // Start menu screen
us &cx = g_sor.now.x, &cy = g_sor.now.y;  // g_sor.now.x is too long, so I added references with shorter names

bool enter() {  // for the "choose by pressing SPACE/ENTER"
    p2 = getch();
    return (p2 == ' ' || p2 == '\r');
}

// because board/shownboard is 1D array, there should be a function translating 2D coordinates into those 2.
void modi(us &x, us &y, char c, bool m = 1) {      // bool m is to know which to be modified
    if (m)
        shownboard[(x + y * xy.x) * 2] = c;
    else
        board[(x + y * xy.x) * 2] = c;
}

// function for revealing a non-bomb cell and (possibly) its neighbors
void show(coord c) {
    if (board[(c.x + c.y * xy.x) * 2] != -2) return;  // check if it's revealed

    char i = '0';
    numcell--;
    // check if cell is at any of the left-right-up-down border
    bool le = c.x,
         ri = (c.x) != xy.x - 1,
         up = c.y,
         dn = (c.y) != xy.y - 1;

    // check if cell's neighbors are bombs
    if (le) {
        if (up) i += (cellnum[c.x - 1][c.y - 1] == 1);
        if (dn) i += (cellnum[c.x - 1][c.y + 1] == 1);
        i += (cellnum[c.x - 1][c.y] == 1);
    }
    if (ri) {
        if (up) i += (cellnum[c.x + 1][c.y - 1] == 1);
        if (dn) i += (cellnum[c.x + 1][c.y + 1] == 1);
        i += (cellnum[c.x + 1][c.y] == 1);
    }
    if (up) i += (cellnum[c.x][c.y - 1] == 1);
    if (dn) i += (cellnum[c.x][c.y + 1] == 1);

    if (i != '0') {
        modi(c.x, c.y, i, 0);
        return;
    }

    // cell has 0 neighboring bombs => reveals its neighbors
    modi(c.x, c.y, ' ', 0);
    if (le) {
        show({us(c.x - 1), c.y});
        if (up) show({us(c.x - 1), us(c.y - 1)});
        if (dn) show({us(c.x - 1), us(c.y + 1)});
    }
    if (ri) {
        show({us(c.x + 1), c.y});
        if (up) show({us(c.x + 1), us(c.y - 1)});
        if (dn) show({us(c.x + 1), us(c.y + 1)});
    }
    if (up) show({c.x, us(c.y - 1)});
    if (dn) show({c.x, us(c.y + 1)});
}

void play() {  // choose difficulty and start game
    cls();
    cout << "Choose the board:" << endl
         << "Beginner (10x10, 8 bombs)    Intermediate (13x15, 40 bombs)\n"
         << ((!p_sor.now.y) ? (((p_sor.now.x) ? string(44, ' ') : "           ") + (char)24 + '\n') : "\n")  // check if the arrow should be on this line
         << "Expert (16x30, 100 bombs)    Custom (Whatever you want man)\n"
         << ((p_sor.now.y) ? (((p_sor.now.x) ? string(44, ' ') : "           ") + (char)24 + '\n') : "\n");

    // player moves around
    if (!enter()) {
        p_sor.update();
        play();
        return;
    }

    // player picks a board
    switch (p_sor.code()) {
    case 0:
        xy = {10, 10};
        z = 8;
        break;
    case 1:
        xy = {15, 13};
        z = 40;
        break;
    case 2:
        xy = {30, 16};
        z = 100;
        break;
    default:
        while (1) {  // while loop ends when player's input is valid
            cout << "Enter the width and height of the board, and how many bombs you want: ";
            cin >> xy.x >> xy.y >> z;

            if (cin.fail()) {
                cin.clear();
                cin.ignore();
                cin.clear();
                cin.ignore();
                cin.clear();
                cin.ignore();
                cout << "\nERROR! Code: OwO"
                     << "\nA cat is stepping on your keyboard.\n\n";
                continue;
            }

            if (!(xy.x && xy.y)) {
                cout << "\nERROR! Code: O"
                     << "\nVery funny.\n\n";
                continue;
            }

            if (z > xy.x * xy.y) {
                cout << "\nERROR! Code: 911"
                     << "\nToo many bombs, the board explodes.\n\n";
                continue;
            }

            if (max(xy.x, xy.y) > 80 || min(xy.x, xy.y) > 40) {
                cout << "\nERROR! Code: -1"
                     << "\nThe width and height are capped at 80 and 40."
                     << "\nI mean, who can play on such a huge board?\n\n";
                continue;
            }

            // Input is valid, but to make a nice board (imo) x shouldn't be less than y
            if (xy.x < xy.y) swap(xy.x, xy.y);
            break;
        }
    }

retry:  // for retrying at this difficulty
    cout << "\nYou are going to mienswipe a " << xy.x << 'x' << xy.y << " board with "
         << z << " bomb" << ((z == 1) ? "." : "s.")
         << "\nPress anything to continue...\n";
    if (getch() == 27) quit();

    // setting up the board and bombs
    numcell = xy.x * xy.y;
    g_sor.board = xy;
    g_sor.now = {(us)(xy.x / 2), (us)(xy.y / 2)};

    // picking random coordinates for z+1 bombs. last bomb is for when player unluckily reveals a bomb 1st try
    us bomb[numcell];
    for (us i = 0; i < numcell; i++) bomb[i] = i;
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    shuffle(&bomb[0], &bomb[numcell], default_random_engine(seed));
    for (us i = 0; i < z; i++) cellnum[bomb[i] % xy.x][bomb[i] / xy.x] = 1;

    for (us i = 0; i < xy.y; i++) {
        for (us j = 0; j < xy.x - 1; j++) {
            board += (char)254;
            board += ' ';
        }
        board += (char)254;
        board += '\n';
    }
    numcell -= z;
    // setup completes

    // loops until all non-bomb cells are revealed, or a bomb is revealed
    while (numcell) {
        shownboard = board;
        modi(cx, cy, -7);  // shownboard copies board, add a dot as the screen's cursor
        cls();
        cout << shownboard;
        if (enter()) {
            if (cellnum[cx][cy] == 1) {
                if (xy.x * xy.y - z == numcell) {  // if player unluckily picks a bomb 1st try
                    cellnum[cx][cy] = 0;
                    cellnum[bomb[z] % xy.x][bomb[z] / xy.x] = 1;
                    show(g_sor.now);
                    continue;
                }
                for (us i = 0; i < z; i++)
                    board[(bomb[i] % xy.x + bomb[i] / xy.x * xy.x) * 2] = '#';
                modi(cx, cy, 'x', 0);
                break;
            }
            show(g_sor.now);
        } else
            g_sor.update();
    }

    // game ends. cout game over message and reset some global variables
    cls();
    cout << board
         << ((numcell) ? "\nYou got swiped and lose! :(" : "\nCongratulation, you won!")
         << "\nType \"MENU\" to go back to menu,\n"
         << "     \"RETRY\" to try this difficulty again,\n"
         << "  or \"QUIT\" to quit.\n";
    board = {};
    for (us i = 0; i <= z; i++) cellnum[bomb[i] % xy.x][bomb[i] / xy.x] = 0;
    while (1) {
        cin >> shownboard;
        if (shownboard == "MENU" || shownboard == "Menu" || shownboard == "menu") {
            main();
            exit(0);
        }
        if (shownboard == "RETRY" || shownboard == "Retry" || shownboard == "retry") {
            goto retry;
            exit(0);
        }
        if (shownboard == "QUIT" || shownboard == "Quit" || shownboard == "quit") quit();
    }
}

void op() {  // screen brightness option
    cls();
    cout << "Set brightness:\t\tDark\t\tNormal\t\tBright\t\tBack-to-Menu\n"
         << string(3 + o_sor.now.x * 2, '\t')
         << ((o_sor.now.x == 3) ? "      " : ((o_sor.now.x) ? "  " : " "))
         << (char)24 << endl;
    if (enter()) switch (o_sor.code()) {
        case 0: {
            text = 0;
            system("Color 07");
            break;
        }
        case 1: {
            text = 1;
            system("Color 07");
            break;
        }
        case 2: {
            text = 0;
            system("Color 70");
            break;
        }
        default: {
            main();
            exit(0);
        }
        }
    o_sor.update();
    op();
}

void moai() {  // moai
    cout << "      ..'',,,,,,,,,,,,''..\n"
         << "    .,,,,;;::cccccc::;;;,,,.\n"
         << "    ';coxO0KXXNNNNXXK0Oxoc;.\n"
         << "  'clkXWMMMMMMWWWWMMMMMMWXkoc'\n"
         << " .xkok0KXNWWKxooooxKWWNXK0koOx.\n"
         << " ;0k;,;:clokd;,,,,;dOolc:;,;kK;\n"
         << " lXx;,,,,,;od:,,,,;oo;,,,,,;xXl\n"
         << ".dNd;,,,,,,co:,,,,;ol;,,,,,;dNd.\n"
         << ".ONd;,,,,,,cl:;;;;;lc,,,,,,;dNO.\n"
         << ",KNd;,;,,,,:dk0000kd:,,,,;,;dNK,\n"
         << ";XNd;,,,,,:kNMMMMMMNk:,,,,;;dNX;\n"
         << ":XNd;,;,,,;okOO00OOxo;,,;,,;xWX:\n"
         << ",KWx;,,,;:codxxxxxxdoc:;,,,;kWK,\n"
         << ".cOd;,,,:dO0KKXXXXKK0Od:,,,;x0c.\n"
         << "  .',,,,cxO0000000000Oxc,,,,..\n"
         << "   ';,,,;,;;;;;;;;;;;;,,;,,:'\n"
         << "  ,kd;,,,,,,,,,,,,,,,,,,,,;dk,\n"
         << "  dWXOxdolcc::::::::cclldxOXWd\n"
         << "  :0NWMWWNNXXXXXXXXXXNNWWMWN0:\n"
         << "   .:dOKXNWWMMMMMMMMWWNXKOd:.\n"
         << "\nPlease \"MOAI\" to continue..." << endl;
    do cin >> shownboard;
    while (shownboard != "MOAI");
    main();
}
int main() {
    cls();
    cout << "Welcome to Mienswiper! Move around by pressing ASWD, and choose by pressing SPACE/ENTER. Press ESC at anytime to quit.\n"
         << "\tPlay!\t\tOptions\t\tCredits\t\tQuit\t\tMOAI\n"
         << string(s_sor.now.x * 2 + 1, '\t') << ((s_sor.now.x == 1 || s_sor.now.x == 2) ? "   " : "  ") << (char)24 << endl;
    while (1) {
        if (enter()) switch (s_sor.now.x) {
            case 0:
                play();
                return 0;
            case 1:
                op();
                return 0;
            case 2:
                cls();
                cout << "Creator: Square (1 credit)\n"
                     << "Player:  You    (0 credit)\n"
                     << "That's all.\n\n"
                     << "Press anything to continue...\n";
                if (getch() == 27) quit();
                main();
                return 0;
            case 3:
                quit();
            default:
                moai();
                return 0;
            }
        s_sor.update();
        main();
        return 0;
    }
}
