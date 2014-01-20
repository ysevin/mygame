#pragma once

class SEObjectBase
{
public:
	bool create_entity(const char* _mesh_name, bool _visible = false);

	void set_visible(bool _visible);

	void set_position(int _x);
	void yaw();

	Entity*		get_entity();
	SceneNode*	get_scene_node();

	static bool is_mesh_exit(const char* _name);

private:
	std::string object_name_;

	static std::list<std::string> cache_mesh_;
};