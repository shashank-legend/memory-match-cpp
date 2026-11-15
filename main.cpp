#include <iostream>
#include "cards.h"
#include "player.h"
#include <random>
string card[4][4] = {{"| 01 |", "| 02 |", "| 03 |", "| 04 |"},
                     {"| 05 |", "| 06 |", "| 07 |", "| 08 |"},
                     {"| 09 |", "| 10 |", "| 11 |", "| 12 |"},
                     {"| 13 |", "| 14 |", "| 15 |", "| 16 |"}};
int flipped_cards[4][4] = {{-1, -1, -1, -1},
                           {-1, -1, -1, -1},
                           {-1, -1, -1, -1},
                           {-1, -1, -1, -1}};
string emoji[] = {"| \U0001F60A |", "| \U0001F603 |", "| \U0001F92F |", "| \U0001F44D |", "| \U0001F44E |", "| \U0001F914 |", "| \U0001F62D |", "| \U0001F602 |", "| \U0001F60A |", "| \U0001F603 |", "| \U0001F92F |", "| \U0001F44D |", "| \U0001F44E |", "| \U0001F914 |", "| \U0001F62D |", "| \U0001F602 |"};
cards *allcardsmatrix[4][4];
void displayCards();
void ShuffleCards();
void hand_cards();
int get_index_q(int n);
int get_index_r(int n);
void createPlayers(int n, Player *players[]);
int inputFirstcard();
int inputSecondcard();
void start_game(Player *players[], int n)
{
    int i = 0;
    while (1)
    {
        hand_cards();
        displayCards();
        cout << players[i]->get_name() << "'s turn.\t" << " score : " << players[i]->get_score() << endl;
        int n1;
        int r1, c1;
        do
        {
            cout << "Enter the card number:";
            cin >> n1;
            r1 = get_index_q(n1);
            cout << "row is : " << r1 << endl;
            int c1 = get_index_r(n1);
            cout << "column is : " << c1 << endl;
            cout << flipped_cards[r1][c1] << endl;
        } while (!(flipped_cards[r1][c1] == 0));
        int t;
        cout << "enter a trach number : ";
        cin >> t;
        flipped_cards[r1][c1] = 0;
        int n2 = inputSecondcard();
        int q2 = get_index_q(n1);
        int r2 = get_index_r(n1);
        flipped_cards[q2][r2] = 0;
        if (allcardsmatrix[r1][c1]->compareWithAnotherCard(*allcardsmatrix[q2][r2]))
        {
            cout << "You Got it Right!!!" << endl;
            players[i]->increase_score();
            cout << "current_score = " << players[i]->get_score() << endl;
        }
        i = i + 1;
    }
}
int main()
{
    int no_of_players;
    cout << "Enter the number of players : ";
    cin >> no_of_players;
    Player *players[no_of_players];
    createPlayers(no_of_players, players);
    start_game(players, no_of_players);
    return 0;
}
void createPlayers(int n, Player *players[])
{
    for (int i = 0; i < n; i++)
    {
        string name;
        cout << "Enter the name of the player:";
        cin >> ws;
        getline(cin, name);
        players[i] = new Player(name);
    }
}
void displayCards()
{
    int i, j, m = 0, n = 0;
    for (i = 0; i < 4; i++)
    {
        n = 0;
        cout << "  ____   ____   ____   ____" << endl;
        cout << " |    | |    | |    | |    |" << endl;
        for (j = 0; j < 4; j++)
        {
            if (flipped_cards[m][n] == -1)
            {
                cout << " | ?? |";
            }
            else
            {
                cout << " " << allcardsmatrix[m][n]->get_card_name();
            }
            n++;
        }
        cout << m << " " << n;
        cout << endl
             << " |____| |____| |____| |____|" << endl
             << endl;
        m++;
    }
}
void ShuffleCards()
{
    int n = sizeof(emoji) / sizeof(emoji[0]);

    // Seed the random number generator
    std::random_device rd;
    std::mt19937 gen(rd());

    // Shuffle the emoji
    std::shuffle(emoji, emoji + n, gen);
}
void hand_cards()
{
    for (int i = 0; i < 16;)
    {
        for (int j = 0; j < 4; j++)
        {
            if (j % 2 == 0)
            {
                ShuffleCards();
            }
            for (int k = 0; k < 4; k++)
            {
                allcardsmatrix[j][k] = new cards(emoji[i % 8]);
                i++;
            }
        }
    }
}
int get_index_q(int n)
{
    int q = n / 4;
    int r = n % 4;
    if (r == 0)
    {
        r = 3;
        q = q - 1;
    }
    else
        r = r - 1;
    return q;
}

int get_index_r(int n)
{
    int q = n / 4;
    int r = n % 4;
    if (r == 0)
    {
        r = 3;
        q = q - 1;
    }
    else
        r = r - 1;
    return r;
}
int inputFirstcard()
{
    int card_number;
    int m, n;
    do
    {
        cout << "Enter the card number:";
        cin >> card_number;
        int m = get_index_q(card_number);
        cout << "row is : " << m << endl;
        int n = get_index_r(card_number);
        cout << "column is : " << n << endl;
        cout << flipped_cards[m][n] << endl;
    } while (flipped_cards[m][n] == 0);
    return card_number;
}
int inputSecondcard()
{
    int card_number;
    int m, n;
    do
    {
        cout << "Enter the card number:";
        cin >> card_number;
        int m = get_index_q(card_number);
        int n = get_index_r(card_number);
    } while (flipped_cards[m][n] == 0);
    return card_number;
}