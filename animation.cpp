#include "StdAfx.h"

#include "animation.h"

void SEAnimation::create(const char* _name)
{
	if(!model_.create_entity(_name))
		return;
	
	//停掉所有动画
	Entity* entity = model_.get_entity();
	AnimationStateSet* ass = entity->getAllAnimationStates();
	if(ass != NULL)
	{
		AnimationState* state;
		AnimationStateIterator it = ass->getAnimationStateIterator();
		while(it.hasMoreElements())
		{
			state = it.getNext();
			state->setEnabled(false);
			state->setLoop(false);

			if(state_ == NULL)		//默认取第一个动画
				state_ = state;
		}
	}
}

void SEAnimation::set_enabled(bool _enabled)
{
	restart();
	if(state_)
		state_->setEnabled(_enabled);
}

void SEAnimation::restart()
{
	if(state_)
		state_->setTimePosition(0);
}

bool SEAnimation::is_over()
{
	if(state_)
		return state_->getTimePosition() >= state_->getLength();

	return false;
}

void SEAnimation::update(float _elapse)
{
	if(state_)
		state_->addTime(_elapse);
}

void SEAnimation::unshare_skeleton(SEModel& _model)
{
	Entity* share_entity = _model.get_entity();
	share_entity->stopSharingSkeletonInstance();
}

void SEAnimation::share_skeleton(SEModel& _model)
{
	Entity* share_entity = _model.get_entity();
	if(share_entity != NULL)
	{
		Entity* self_entity = model_.get_entity();
		SkeletonPtr sp = self_entity->getMesh()->getSkeleton();

		share_entity->getMesh()->_notifySkeleton(sp);
		share_entity->shareSkeletonInstanceWith(self_entity);
	}
}