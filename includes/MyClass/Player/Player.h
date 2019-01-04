#include <stdlib.h>
#include <string>

class Player
{
public:
	Player(unsigned int id, std::string name);
	~Player();
	
	unsigned int Id;
	std::string Name;

	void ResetScore();
	void AddScore(int delta);
	unsigned int GetScore();


private: 

	unsigned int score = 0;

};