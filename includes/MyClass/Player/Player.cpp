#include "pch.h"
#include "Player.h"


Player::Player(unsigned int id, std::string name) : Id(id), Name(name)
{

}

void Player::AddScore(int delta)
{
	this->score += delta;
}

unsigned int Player::GetScore()
{
	return score;
};
