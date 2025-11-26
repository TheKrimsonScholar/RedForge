#include "Archetype.h"

#include <iostream>

void Player::OnPlayerEvent1(const Entity& entity, Player::PlayerEvent1& event)
{
	std::cout << "PlayerEvent1" << std::endl;
}
void Player::OnPlayerEvent2(const Entity& entity, Player::PlayerEvent2& event)
{
	std::cout << "PlayerEvent2" << std::endl;
}

void Enemy::OnPlayerEvent1(const Entity& entity, Enemy::PlayerEvent1& event)
{
	std::cout << "PlayerEvent1" << std::endl;
}