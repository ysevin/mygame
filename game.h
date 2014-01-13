#pragma once

#include "control.h"

class game
{
public:
	game(void);
	~game(void);

public:
	bool init(const HWND& hWnd);
	bool update();
	bool release();

	bool resize(const HWND& hWnd);

	bool key_down(const uint _key);
	bool key_up(const uint _key);

	bool create_character();
	bool destroy_character();

public:
	SEControl	ctrl_;
	uint		down_key_;
};
