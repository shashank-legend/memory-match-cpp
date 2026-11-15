#include "player.h"
Player::Player(string Name)
{
    name = Name;
    score = 0;
}
string Player::get_name()
{
    return name;
}
void Player::set_score()
{
    score = 0;
}
void Player::increase_score()
{
    score = score + 1;
}
int Player::get_score()
{
    return score;
}