#include "stdafx.h"

#include "player.h"

SEPlayer::SEPlayer()
{
	character_ = NULL;
	aspect_ = AT_RIGHT;
}

void SEPlayer::set_type(const char* _type_name)
{
	character_ = g_character_mgr.load(_type_name);
}
void SEPlayer::set_pos(int _pos)
{
	pos_ = _pos;

	character_->set_position(_pos);
}

void SEPlayer::set_aspect(ASPECT_TYPE _aspect)
{
	if(aspect_ != _aspect)
	{
		character_->yaw();
		aspect_ = _aspect;
	}
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