#pragma once

namespace SEConfig
{
	class SECfgValue
	{
	public:
		SECfgValue(){}

		SECfgValue(const char* _str);
		uint to_int();
		Real to_float();
		std::string to_string(uint _index = -1);
		Vector3 to_vector3();

	private:
		std::stringstream ss_value_;
	};


	SECfgValue read(const char* _section, const char* _key);
};
