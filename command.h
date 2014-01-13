#pragma once

#define KEY_MAX_LEN 32

class SECommand
{
	class SEKey
	{
	public:
		SEKey(const char* _key, const char* _content, int _index):key_(_key), content_(_content), index_(_index){}

	public:
		int index_;
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
	char key_comp_[KEY_MAX_LEN];	//key_buff_去掉空格后的内容

	std::list<SEKey> keys_;
};