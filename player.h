#include<string>
using namespace std;
class Player
{
private:
    string name;
    int score;

public:
    Player(string Name);
    string get_name();
    void set_score();
    void increase_score();
    int get_score();
};
