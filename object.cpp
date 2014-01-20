#include "StdAfx.h"
#include "object.h"

extern SceneManager    *m_scene_mgr;

std::list<std::string> SEObjectBase::cache_mesh_;

bool SEObjectBase::create_entity(const char* _mesh_name, bool _visible/* = false*/)
{
	int i = 0;
	std::stringstream new_name;
	while(i < 100)
	{
		new_name.clear();
		new_name << _mesh_name << i++;
		if(is_mesh_exit(new_name.str().c_str()) == NULL)
			break;
	}

	object_name_ = new_name.str();
	std::string mesh_name = _mesh_name;
	mesh_name += ".mesh";
	m_scene_mgr->createEntity(object_name_, mesh_name);		//重名的会抛异常
	SceneNode* node = m_scene_mgr->getRootSceneNode()->createChildSceneNode(object_name_);
	//node->showBoundingBox(true);

	set_visible(_visible);

	cache_mesh_.push_back(object_name_);

	return true;
}

void SEObjectBase::set_visible(bool _visible)
{
	Entity*	entity = get_entity();
	SceneNode *node = get_scene_node();
	if(_visible)
		node->attachObject(entity);
	else
		node->detachObject(entity);
}

void SEObjectBase::set_position(int _x)
{
	SceneNode *node = get_scene_node();
	node->setPosition(_x, 0, 0);
}
void SEObjectBase::yaw()
{
	SceneNode *node = get_scene_node();
	node->yaw(Radian(-Math::PI));
}


Entity* SEObjectBase::get_entity()
{
	return m_scene_mgr->getEntity(object_name_);
}
SceneNode* SEObjectBase::get_scene_node()
{
	return m_scene_mgr->getSceneNode(object_name_);
}
bool SEObjectBase::is_mesh_exit(const char* _name)
{
	std::list<std::string>::iterator it = std::find(cache_mesh_.begin(), cache_mesh_.end(), _name);
	if(it == cache_mesh_.end())
		return false;
	return true;
}