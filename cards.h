#include<string>
using namespace std;
class cards
{
private:
    string card_name;
public:
    cards(string name);
    int compareWithAnotherCard(const cards &otherCard) const;
    string get_card_name();
};