#pragma once

#include "player.h"

class SEState;

class SEStateMgr
{
public:
	SEStateMgr() : curr_state_(NULL){}
	~SEStateMgr();

	void update(void* _param);

	void add_state(SEState* _state);	//内部释放
	void set_state(const char* _name);
	std::string get_state_name();

private:
	typedef std::list<SEState*> StateList;

	StateList states_;
	SEState* curr_state_;
};


class SEState
{
	friend SEStateMgr;
public:
	SEState(SEStateMgr& _mgr):state_mgr_(_mgr){}
	virtual ~SEState(){}

	virtual void start() {}
	virtual void update(void* _param) {}
	virtual void stop() {}

	const std::string& get_name()
	{
		return name_;
	}

protected:
	std::string		name_;
	SEStateMgr&		state_mgr_;
};

class SEMotionState : public SEState
{
public:
	SEMotionState(SEStateMgr& _mgr, SEPlayer* _player, const char* _name = "SEMotionState");

	void start();
	void update(void* _param);

private:
	SEPlayer*	player_;
	std::string motion_name_;
public:
	bool		can_break_;
};

//可用宏的方式生成状态的类