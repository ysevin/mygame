#include "StdAfx.h"

#include "command.h"
#include "util.h"

SECommand::SECommand():key_begin_(0)
{
	memset(key_buff_, 0, KEY_MAX_LEN);
}

void SECommand::set_command(const char* _content)
{
	key_begin_ = KEY_MAX_LEN - 1;
	keys_.clear();

	int index = 0;
	int key_len = 0;
	std::vector<std::string> vec = SEUtil::splite(_content);
	std::vector<std::string>::iterator it;
	for(it = vec.begin(); it != vec.end(); it++)
	{
		std::vector<std::string> vec2 = SEUtil::splite(it->c_str(), "|");
		if(vec2.size() > 1)
		{
			std::transform (vec2[0].begin(),vec2[0].end(), vec2[0].begin(), toupper);
			keys_.push_back(SEKey(vec2[0].c_str(), vec2[1].c_str(), index++));

			if(key_len < vec2[0].length())
				key_len = vec2[0].length();
		}
	}
	key_len = 20;		//测试用, 要不手残按不过来.
	key_begin_ = KEY_MAX_LEN - key_len - 1;	//最后一位固定为0
	assert(key_begin_ >= 0);
}

void SECommand::update(char key)
{
	int i = key_begin_;
	for(; i < KEY_MAX_LEN - 1; i++)
		key_buff_[i] = key_buff_[i+1];
	key_buff_[i] = key;

	i = key_begin_;
	int j = 0;
	memset(key_comp_, 0, KEY_MAX_LEN);
	for(; i < KEY_MAX_LEN - 1; i++)
	{
		if(key_buff_[i] != 0)
			key_comp_[j++] = key_buff_[i];
	}
}

void SECommand::match(std::string& _out)
{
	int len = strlen(key_comp_);

	int max_index = 0;
	SEKey* match_key = NULL;
	for(int i = 0; i < len; i++)
	{
		char* key = &(key_comp_[i]);

		std::list<SEKey>::iterator it = keys_.begin();
		for(; it != keys_.end(); it++)		//可优化, 比如只比较位数一样
		{
			if(it->key_.compare(key) == 0)
				break;
		}
		if(it != keys_.end())
		{
			if(max_index < it->index_)
			{
				match_key = &(*it);
				max_index = it->index_;
			}

			continue;
		}
	}
	if(match_key != NULL)
		_out = match_key->content_;
}