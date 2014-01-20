#include "StdAfx.h"

#include "state.h"

SEStateMgr::~SEStateMgr()
{
	StateList::iterator it = states_.begin();
	for(; it != states_.end(); it++)
		delete *it;
	states_.clear();
}

void SEStateMgr::update(void* _param)
{
	if(curr_state_ != NULL)
		curr_state_->update(_param);
}

void SEStateMgr::add_state(SEState* _state)
{
	states_.push_back(_state);
}

void SEStateMgr::set_state(const char* _name)
{
	if(curr_state_ != NULL)
		curr_state_->stop();

	StateList::iterator it;
	if(strcmp(_name, "idle") == 0)
		it = states_.begin();
	else
	{
		for(it = states_.begin(); it != states_.end(); it++)
			if((*it)->name_.compare(_name) == 0)
				break;
	}
	if(it != states_.end())
	{
		curr_state_ = *it;
		curr_state_->start();
	}
}
std::string SEStateMgr::get_state_name()
{
	if(curr_state_ != NULL)
		return curr_state_->name_;

	return "";
}

SEMotionState::SEMotionState(SEStateMgr& _mgr, SEPlayer* _player, const char* _name) : SEState(_mgr)
{
	player_ = _player;
	can_break_ = false;

	name_ = _name;
	motion_name_ = _name;
}

void SEMotionState::start()
{
	player_->action(motion_name_.c_str());
}

void SEMotionState::update(void* _param)
{
	//动作播放完时, 放空闲动画
	char* name = (char*)_param;
	if(!player_->is_action())
		state_mgr_.set_state("idle");

	if(*name == 0 && name_.compare("walk") == 0)		//没操作, 且是走路状态时, 回到待机动画.
		state_mgr_.set_state("idle");

	if(*name != 0 && can_break_ && name_.compare(name) != 0)
		state_mgr_.set_state(name);
}