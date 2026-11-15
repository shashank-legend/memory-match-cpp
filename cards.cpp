#include "cards.h"
cards::cards(string name)
{
    card_name = name;
}

int cards::compareWithAnotherCard(const cards &otherCard) const
{
    if (card_name == otherCard.card_name)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
string cards::get_card_name()
{
    return card_name;
}