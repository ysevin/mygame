#pragma once

#include "character.h"

class SEPlayer
{
public:
	enum ASPECT_TYPE
	{
		AT_LEFT,
		AT_RIGHT,
	};

public:
	SEPlayer();
	virtual ~SEPlayer(){}

	void set_pos(int _pos);
	void set_aspect(ASPECT_TYPE _aspect);

	virtual void set_type(const char* _type_name);
	void change_part_model(int _part, const char* _name);

	void action(const char* _name);
	bool is_action();

	void update();

protected:
	SECharacter* character_;

	int	pos_;
	ASPECT_TYPE aspect_;
};