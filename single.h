#pragma once

template<typename T>
class SESingleton
{
public:
	static inline T& get_singleton()
	{
		return instance_;
	}

private:
	static T instance_;
};

template<typename T> T SESingleton<T>::instance_;