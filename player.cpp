#include "stdafx.h"

#include "player.h"

SEPlayer::SEPlayer()
{
	character_ = NULL;
	aspect_ = AT_LEFT;
}

void SEPlayer::set_type(const char* _type_name)
{
	character_ = g_character_mgr.load(_type_name);
}
void SEPlayer::set_pos(int pos)
{

}

void SEPlayer::change_aspect()
{
	aspect_ = (aspect_ == AT_LEFT ? AT_RIGHT : AT_LEFT);
}

void SEPlayer::change_part_model(int _part, const char* _name)
{

}
void SEPlayer::action(const char* _name)
{
	character_->set_animation(_name);
}
bool SEPlayer::is_action()
{
	return !character_->is_animation_over();
}