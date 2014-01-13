#pragma once

#include "model.h"

class SECollisionBox
{
private:
	Vector3 min_;
	Vector3 max_;
};

class SEAnimation
{
public:
	SEAnimation():state_(NULL){}

	void create(const char* _name);
	void set_enabled(bool _enabled);
	void restart();
	bool is_over();
	void update(float _elapse);

	void unshare_skeleton(SEModel& _model);
	void share_skeleton(SEModel& _model);

private:
	SEModel	model_;
	AnimationState* state_;

	SECollisionBox attack_box_;
	SECollisionBox bound_box;
};