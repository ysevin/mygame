#pragma once

#include "model.h"
#include "animation.h"
#include "single.h"

enum CHARACTER_PART
{
	CP_HEAD = 0,
	CP_HAIR,
	CP_HELMET,
	CP_CHEST,
	CP_HAND,
	CP_WAIST,
	CP_LEG,
	CP_FOOT,
	CP_GLASS,
	CP_WING,
	CP_SCAPULA,
	CP_NUM,
};

class SEBody
{
public:
	void create(const char* _name);
	void set_visible(bool _visible);
	void set_animation(SEAnimation* _ani);

	void translate(int offset);
	void turn();

private:
	SEModel	model_part_[CP_NUM];
};

class SECharacter 
{
public:
	~SECharacter();

	void create(const char* _name);

	void set_animation(const char* _name);
	void update_animation(float _elapse);
	bool is_animation_over();

	void translate(int offset);
	void turn();

private:
	typedef std::map<std::string, SEBody*>		BodyMap;
	typedef std::map<std::string, SEAnimation*>	AnimantionMAP;

	BodyMap				bodys_;
	AnimantionMAP		animantions_;

	std::string			active_;
};

class SECharacterMgr : public SESingleton<SECharacterMgr>
{
public:
	~SECharacterMgr();

	SECharacter* load(const char* _name);

	void	update_animation(float _elapse);

private:
	typedef std::list<SECharacter*> CharacterList;

	CharacterList characters_;
};

#define g_character_mgr SECharacterMgr::get_singleton()