#pragma once
#include <string>
#include <memory>
#include <mutex>

namespace varsync
{
namespace inner
{
	class IVariables;
	class IDataSource;

	class VarSync
	{
	public:
		static VarSync& GetInstance();
		~VarSync();

		void SetDataSource(const std::string dataSourceName);

		int* Register(const std::string, int);

		std::string* Register(const std::string, std::string);

		float* Register(const std::string, float);
	private:
		VarSync(IVariables* variables, IDataSource* dataSource);
		VarSync(void);
		VarSync(const VarSync& src);
		VarSync& operator=(const VarSync& rhs);

		std::unique_ptr<IVariables>  variables_;
		std::unique_ptr<IDataSource> dataSource_;
	};

	template<class T>
	class Var
	{
	public:
		Var(const std::string name, T defaultValue)
		{
			var_ = VarSync::GetInstance().Register(name, defaultValue);
		}
		const T operator*() { return *var_; }
	private:
		T* var_;
	};

}  // namespace inner

void BindDataSource(const std::string dataSourceName);

	class Int : public inner::Var<int>
	{
	public:
		explicit Int(const std::string name, int defaultValue = 0) : Var<int>(name, defaultValue) {}
	};

	class Float : public inner::Var<float>
	{
	public:
		explicit Float(const std::string name, float defaultValue = 0.0f) : Var<float>(name, defaultValue) {}
	};

	class String : public inner::Var<std::string>
	{
	public:
		explicit String(const std::string name, const std::string defaultValue = "") : Var<std::string>(name, defaultValue) {}
	};

}  // namespace varsync
