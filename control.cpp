#include "stdafx.h"

#include "control.h"
#include "config.h"
#include "util.h"

void SEControl::set_type(const char* _type_name)
{
	SEPlayer::set_type(_type_name);

	std::string command = SEConfig::read(_type_name, "command").to_string();
	key_input_.set_command(command.c_str());


	std::string ani_name = SEConfig::read(_type_name, "animation").to_string().c_str();
	std::vector<std::string> vec_ani = SEUtil::splite(ani_name.c_str());
	for(int i = 0; i < vec_ani.size(); i++)
	{
		SEMotionState* state = new SEMotionState(state_mgr_, this, vec_ani[i].c_str());
		state_mgr_.add_state(state);

		if(i < 3)
		{
			state->can_break_ = true;
			state_mgr_.set_state(vec_ani[0].c_str());
		}
	}
}

void SEControl::move(int _offset)
{
	if(state_mgr_.get_state_name().compare("walk") == 0)		//移动状态才能设置位移
		set_pos(pos_ + _offset);
}
void SEControl::turn(ASPECT_TYPE _aspect)
{
	if(state_mgr_.get_state_name().compare("idle") == 0
		|| state_mgr_.get_state_name().compare("walk") == 0)
	{
		set_aspect(_aspect);
	}
}

const int VELOCITY_SEC = 100;

void SEControl::update(uint _key, float _elapse)
{
	//移动
	if(char(_key) == 'D')
	{
		turn(AT_RIGHT);
		move(_elapse * VELOCITY_SEC);
	}
	if(char(_key) == 'A')
	{
		turn(AT_LEFT);
		move(-_elapse * VELOCITY_SEC);
	}

	//动画
	std::string state_name;
	key_input_.update(_key);
	key_input_.match(state_name);
	state_mgr_.update((void*)state_name.c_str());
}