#include "StdAfx.h"
#include "config.h"
#include "util.h"

SEConfig::SECfgValue::SECfgValue(const char* _str)
{
	ss_value_ << _str;
}
uint SEConfig::SECfgValue::to_int()
{
	uint v(0);
	ss_value_ >> v;

	return v;
}
Real SEConfig::SECfgValue::to_float()
{
	Real v;
	ss_value_ >> v;

	return v;
}
std::string SEConfig::SECfgValue::to_string(uint _index/* = -1*/)
{
	if(_index == -1)
		return ss_value_.str();

	std::vector<std::string> vec;
	vec = SEUtil::splite(ss_value_.str().c_str(), " ,");
	if(_index < vec.size())
		return vec[_index];

	return "";
}
Vector3 SEConfig::SECfgValue::to_vector3()
{
	int v1(0), v2(0), v3(0);
	ss_value_ >> v1 >> v2 >> v3;

	return Vector3(v1, v2, v3);
}

SEConfig::SECfgValue SEConfig::read(const char* _section, const char* _key)
{
	const static std::string ini_path = "./game_config.ini";
	const int BUFF_SIZE = 256;

	char buff[BUFF_SIZE] = {0};
	GetPrivateProfileStringA(_section , _key, "", buff, BUFF_SIZE, ini_path.c_str());

	return SECfgValue(buff);
}