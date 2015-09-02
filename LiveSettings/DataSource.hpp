#pragma once
#include <atomic>
#include <thread>
#include <cassert>

namespace varsync
{
namespace inner
{
class IVariables;

class IDataSource
{
public:
	IDataSource() : variables_(nullptr), isWorking_(false) {}

	void SetTarget(IVariables* variables)
	{
		variables_ = variables;
	}

	void Start()
	{
		assert(variables_ != nullptr);
		isWorking_ = true;
		thread_ = std::thread([this]() {
			Setup();

			while (isWorking_)
			{
				FetchData(variables_);
			}

			Cleanup();
		});
	}

	void Stop()
	{
		isWorking_ = false;
		thread_.join();
	}
protected:
	std::atomic<bool> isWorking_;
private:
	virtual void FetchData(IVariables* variables) = 0;
	virtual void Setup() {}
	virtual void Cleanup() {}

	std::thread thread_;
	IVariables* variables_;
};

class EmptyDataSource : public IDataSource
{
private:
	void FetchData(IVariables* variables)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		variables->Set("x", rand() % 10);
		variables->Set("jakas tam", rand() % 10);
	}
};

}  // namespace inner
}  // namespace varsync
