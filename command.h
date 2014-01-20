#pragma once

#define KEY_MAX_LEN 32

class SECommand
{
	class SEKey
	{
	public:
		SEKey(const char* _key, const char* _content, int _priority):key_(_key), content_(_content), priority_(_priority){}

	public:
		int priority_;
		std::string key_;
		std::string content_;
	};

public:
	SECommand();

	void set_command(const char* _content);
	void update(char _key);
	void match(std::string& _out);

private:
	int key_begin_;
	char key_buff_[KEY_MAX_LEN];
	char key_comp_[KEY_MAX_LEN];	//key_buff_ȥ���ո�������

	std::list<SEKey> keys_;
};