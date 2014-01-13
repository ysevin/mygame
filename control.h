#pragma once

#include "player.h"
#include "command.h";
#include "state.h"

class SEControl : public SEPlayer
{
public:
	void set_type(const char* _type_name);

	void update(uint _key);

private:
	SECommand	key_input_;
	SEStateMgr	state_mgr_;
};