#pragma once
#include <string>
#include <unordered_map>
#include <mutex>

namespace varsync
{
namespace inner
{

class IVariables
{
public:
	virtual int* Register(const std::string name, int defaultValue) = 0;
	virtual void Set(const std::string name, int newValue) = 0;
	virtual float* Register(const std::string name, float defaultValue) = 0;
	virtual void Set(const std::string name, float newValue) = 0;
	virtual std::string* Register(const std::string name, std::string defaultValue) = 0;
	virtual void Set(const std::string name, std::string newValue) = 0;
	virtual ~IVariables() {}
};  // class IVariables

class Variables : public IVariables
{
public:
	void Set(const std::string name, int newValue)
	{
		std::lock_guard<std::mutex> lock(int_mutex);
		Set<int>(ints_, name, newValue);
	}

	void Set(const std::string name, float newValue)
	{
		std::lock_guard<std::mutex> lock(float_mutex);
		Set<float>(floats_, name, newValue);
	}

	void Set(const std::string name, std::string newValue)
	{
		std::lock_guard<std::mutex> lock(string_mutex);
		Set<std::string>(strings_, name, newValue);
	}

	int* Register(const std::string name, int defaultValue)
	{
		std::lock_guard<std::mutex> lock(int_mutex);
		return Register<int>(ints_, name, defaultValue);
	}

	float* Register(const std::string name, float defaultValue)
	{
		std::lock_guard<std::mutex> lock(float_mutex);
		return Register<float>(floats_, name, defaultValue);
	}

	std::string* Register(const std::string name, std::string defaultValue)
	{
		std::lock_guard<std::mutex> lock(string_mutex);
		return Register<std::string>(strings_, name, defaultValue);
	}

private:
	template<class T>
	void Set(std::unordered_map<std::string, T*> &container, std::string name, T newValue)
	{
		auto it = container.find(name);
		if (it == container.end())
		{
			container[name] = new T{ newValue };
		}
		else
		{
			*it->second = newValue;
		}
	}

	template<class T>
	T* Register(std::unordered_map<std::string, T*> &container, std::string name, T defaultValue)
	{
		auto it = container.find(name);
		if (it == container.end())
		{
			auto res = container.emplace_hint(it, std::make_pair(name, new T{ defaultValue }));
			return res->second;
		}
		else
		{
			return it->second;
		}
	}

	std::unordered_map<std::string, int*>         ints_;
	std::unordered_map<std::string, float*>       floats_;
	std::unordered_map<std::string, std::string*> strings_;
	std::mutex int_mutex, float_mutex, string_mutex;
};  // class Variables

}  // namespace inner
}  // namespace varsync
