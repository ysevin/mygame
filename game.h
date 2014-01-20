#pragma once

#include "control.h"
#include "my_socket.h";

const int SYNC_FRAME_RATE	=	100;

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

	bool net_recv(const char* _content);

public:
	SEControl	ctrl_;
	SEControl	rival_;
	uint		down_key_;

	int curr_frame_;
	int next_frame_;
	int	sync_frame_;
	char send_key_[SYNC_FRAME_RATE];
	std::list<char> recv_key_;
	ClientSocket net_;
};
