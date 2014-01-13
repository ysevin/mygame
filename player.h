#pragma once

#include "character.h"

class SEPlayer
{
	enum ASPECT_TYPE
	{
		AT_LEFT,
		AT_RIGHT,
	};

public:
	SEPlayer();
	virtual ~SEPlayer(){}

	void set_pos(int pos);
	void change_aspect();

	virtual void set_type(const char* _type_name);
	void change_part_model(int _part, const char* _name);

	void action(const char* _name);
	bool is_action();

	void update();

private:
	SECharacter* character_;

	ASPECT_TYPE aspect_;
};