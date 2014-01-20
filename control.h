#pragma once

#include "player.h"
#include "command.h";
#include "state.h"

class SEControl : public SEPlayer
{
public:
	void set_type(const char* _type_name);

	void move(int _offset);
	void turn(ASPECT_TYPE _aspect);

	void update(uint _key, float _elapse);

private:
	SECommand	key_input_;
	SEStateMgr	state_mgr_;
};