#include "StdAfx.h"

#include "character.h"
#include "util.h"
#include "config.h"

SEBody::~SEBody()
{
	std::list<SEModel*>::iterator it;
	for(it = parts_.begin(); it != parts_.end(); it++)
		delete *it;
	parts_.clear();
}

void SEBody::create(const char* _name)
{
	std::vector<std::string> vec_part = SEUtil::splite(_name);
	std::vector<std::string>::iterator it = vec_part.begin();
	for(; it != vec_part.end(); it++)
	{
		SEModel* model = new SEModel;
		model->create_entity(it->c_str(), false);
		parts_.push_back(model);
	}
	//for(int i = CP_HEAD; i < CP_NUM; i++)
	//{
	//	if(i < vec_part.size())
	//		model_part_[i].create_entity(vec_part[i].c_str(), false);
	//}
}

void SEBody::set_visible(bool _visible)
{
	std::list<SEModel*>::iterator it;
	for(it = parts_.begin(); it != parts_.end(); it++)
		(*it)->set_visible(_visible);

	//for(int i = CP_HEAD; i < CP_NUM; i++)
	//	model_part_[i].set_visible(_visible);
}

void SEBody::set_animation(SEAnimation* _ani)
{
	std::list<SEModel*>::iterator it;
	for(it = parts_.begin(); it != parts_.end(); it++)
		_ani->share_skeleton(*(*it));

	//for(int i = CP_HEAD; i < CP_NUM; i++)
	//	_ani->share_skeleton(model_part_[i]);		//会把原来的骨络删掉, 绑定共享的骨络.
}
void SEBody::set_position(int _x)
{
	std::list<SEModel*>::iterator it;
	for(it = parts_.begin(); it != parts_.end(); it++)
		(*it)->set_position(_x);

	//for(int i = CP_HEAD; i < CP_NUM; i++)
	//	model_part_[i].set_position(_x);
}
void SEBody::yaw()
{
	std::list<SEModel*>::iterator it;
	for(it = parts_.begin(); it != parts_.end(); it++)
		(*it)->yaw();

	//for(int i = CP_HEAD; i < CP_NUM; i++)
	//	model_part_[i].yaw();
}

SECharacter::~SECharacter()
{
	AnimantionMAP::iterator it = animantions_.begin();
	for(; it != animantions_.end(); it++)
	{
		SEAnimation* temp_ani = it->second;
		delete temp_ani;
	}
	animantions_.clear();

	BodyMap::iterator it_body = bodys_.begin();
	for(; it_body != bodys_.end(); it_body++)
	{
		SEBody* temp_body = it_body->second;
		delete temp_body;
	}
	bodys_.clear();
};

void SECharacter::create(const char* _name)
{
	uint begin_time = GetTickCount();
	//动作加载
	std::string model_name = SEConfig::read(_name, "model").to_string();
	std::string ani_name = SEConfig::read(_name, "animation").to_string().c_str();
	std::vector<std::string> vec_ani = SEUtil::splite(ani_name.c_str());
	for(int i = 0; i < vec_ani.size(); i++)
	{
		SEAnimation* ani = new SEAnimation();
		ani->create(vec_ani[i].c_str());
		animantions_.insert(std::make_pair<std::string, SEAnimation*>(vec_ani[i], ani));

		//模型加载, 有多少个动作就创建多少个模型
		SEBody* body = new SEBody();
		body->create(model_name.c_str());
		body->set_animation(ani);
		bodys_.insert(std::make_pair<std::string, SEBody*>(vec_ani[i], body));
	}
	printf("load time %d\n", GetTickCount() - begin_time);
}

void SECharacter::set_animation(const char* _name)
{
	AnimantionMAP::iterator it;
	BodyMap::iterator it_body;
	if(active_.compare(_name) == 0)
	{
		it = animantions_.find(active_);
		if(it != animantions_.end())
		{
			it->second->restart();

			return;
		}
	}

	//停掉之前的
	it = animantions_.find(active_);
	it_body = bodys_.find(active_);
	if(it != animantions_.end())
	{
		it->second->set_enabled(false);
		it_body->second->set_visible(false);
	}

	//播放现在的
	it = animantions_.find(_name);
	it_body = bodys_.find(_name);
	if(it != animantions_.end())
	{
		it->second->set_enabled(true);
		it_body->second->set_visible(true);
	}

	active_ = _name;
}

void SECharacter::update_animation(float _elapse)
{
	AnimantionMAP::iterator it = animantions_.find(active_);
	if(it != animantions_.end())
		it->second->update(_elapse);
}

bool SECharacter::is_animation_over()
{
	AnimantionMAP::iterator it = animantions_.find(active_);
	if(it != animantions_.end())
		return it->second->is_over();

	return true;
}

void SECharacter::set_position(int _x)
{
	BodyMap::iterator it;		// = bodys_.find(active_);
	for(it = bodys_.begin(); it != bodys_.end(); it++)
		it->second->set_position(_x);
}
void SECharacter::yaw()
{
	BodyMap::iterator it;		// = bodys_.find(active_);
	for(it = bodys_.begin(); it != bodys_.end(); it++)
		it->second->yaw();
}

SECharacterMgr::~SECharacterMgr()
{
	CharacterList::iterator it;
	while(characters_.size())
	{
		SECharacter* temp_ch =  characters_.front();
		delete temp_ch;

		characters_.pop_front();
	}
}

SECharacter* SECharacterMgr::load(const char* _name)
{
	SECharacter* ch = new SECharacter;
	ch->create(_name);
	ch->set_animation(SEConfig::read(_name, "action").to_string().c_str());

	characters_.push_back(ch);

	return ch;
}

void SECharacterMgr::update_animation(float _elapse)
{
	CharacterList::iterator it = characters_.begin();
	for(; it != characters_.end(); it++)
		(*it)->update_animation(_elapse);
}