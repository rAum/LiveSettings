#include <iostream>
#include "Variables.hpp"
#include "DataSource.hpp"
#include "varsync.hpp"
#include <sstream>
#include <windows.h>
#include <winsock.h>

namespace varsync
{

void BindDataSource(const std::string dataSourceName)
{
	inner::VarSync::GetInstance().SetDataSource(dataSourceName);
}

namespace inner
{

	class NetworkDataSource : public IDataSource
	{
	public:
	private:
		void Setup();
		void FetchData(IVariables* variables);
		void Cleanup();

		SOCKET socket_;
		WSADATA wsadata_;
		static constexpr int BUFF_SIZE = 65;
		char inputBuff_[BUFF_SIZE];
	};

VarSync::~VarSync()
{
	std::cout << "[VARSYNC] ~VarSync()\n";
	dataSource_->Stop();
}

VarSync::VarSync(IVariables* variables, IDataSource* dataSource) : variables_(variables), dataSource_(dataSource)
{
	std::cout << "[VARSYNC] VarSync()\n";
	dataSource_->SetTarget(variables);
	dataSource_->Start();
}

VarSync& VarSync::GetInstance()
{
	static VarSync instance(new Variables, new NetworkDataSource);
	return instance;
}

void VarSync::SetDataSource(const std::string dataSourceName)
{
	std::cout << "[VARSYNC] Setting data source to " << dataSourceName;

	if (dataSourceName == "network")
	{
		//GetInstance().dataSource_->Stop();
		//GetInstance().dataSource_.reset(new NetworkDataSource);
	}
}

int* VarSync::Register(const std::string name, int defaultValue)
{
	return variables_->Register(name, defaultValue);
}

std::string* VarSync::Register(const std::string name, std::string defaultValue)
{
	return variables_->Register(name, defaultValue);
}

float* VarSync::Register(const std::string name, float defaultValue)
{
	return variables_->Register(name, defaultValue);
}

//////////////////////////////////////////////////////////////////////

void NetworkDataSource::Setup()
{
	int error = WSAStartup(0x0202, &wsadata_);

	if (error) throw "Error while opening socket!";

	const int PORT_NUMBER = 31337;

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT_NUMBER);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (socket_ == INVALID_SOCKET) throw "Invalid socket!";

	if (bind(socket_, (LPSOCKADDR)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		throw "Bind failed";
	}

	if (listen(socket_, SOMAXCONN) == SOCKET_ERROR)
	{
		throw "Listen error";
	}

	SOCKET clientSocket;
	if ((clientSocket = accept(socket_, NULL, NULL)) == SOCKET_ERROR)
	{
		throw "Accept failed";
	}

	closesocket(socket_);

	socket_ = clientSocket;

	std::cout << "\n>>>LISTENING<<<\n";
}

void NetworkDataSource::FetchData(IVariables* variables)
{
	static std::stringstream str;
	auto result = recv(socket_, inputBuff_, BUFF_SIZE - 1, 0);
	if (result <= 0) return;

	inputBuff_[result] = '\0';

	if (str.str().size() > 10000)
	{
		str.clear();
	}

	bool newline = false;
	for (int i = 0; i < result; ++i)
		if (inputBuff_[i] == '\n')
		{
			newline = true;
		}

	str << inputBuff_;

	if (newline)
	{
		std::string command;
		std::getline(str, command);
		std::cout << "\n-------\nRECV:\n\t" << command << "\n------------\n";

		std::stringstream cmd(command);

		std::string type, name, value;
		char eqSign = 0;
		cmd >> type >> name >> eqSign >> value;

		if (eqSign != '=') return;
		if (type == "int")
		{
			variables->Set(name, std::stoi(value));
		}
		else if (type == "float")
		{
			variables->Set(name, std::stof(value));
		}
		else if (type == "string")
		{
			std::string tmp;
			while (cmd >> tmp)
			{
				value += " ";
				value += tmp;
			}
			variables->Set(name, value);
		}
	}
}

void NetworkDataSource::Cleanup()
{
	closesocket(socket_);
	WSACleanup();
}

}  // namespace inner
}  // namespace varsync
