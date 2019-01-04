#include "pch.h"
#include "Player.h"


Player::Player(unsigned int id, std::string name) : Id(id), Name(name)
{
	CurrentControl = Id * 3;
}

void Player::ResetScore()
{
	score = 0;
}

void Player::AddScore(int delta)
{
	this->score += delta;
}

unsigned int Player::GetScore()
{
	return score;
};

void Player::SetControl(int index)
{
	CurrentControl = index;
}

