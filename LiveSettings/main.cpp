#pragma comment (lib, "Ws2_32.lib")

#include <iostream>
#include <thread>
#include <chrono>
#include "varsync.hpp"
using namespace std;


varsync::Int y("y", 42);
void PrintValues(varsync::Int& a, varsync::Int& b)
{
	varsync::Float fl("oko", 66.f);
	varsync::String text("txt");

	std::cout << "Values of: \n"
		<< "\tx = " << *a << '\n'
		<< "\ty = " << *y << '\n'
		<< "\toko = " << *fl << '\n'
		<< "\tz = " << *b << '\n'
		<< "\ttext = " << *text << '\n';
}

int main(int argc, char**argv)
{
	varsync::Int x("x");
	varsync::Int z("z");

	for (;;)
	{
		PrintValues(x, z);
		std::this_thread::sleep_for(std::chrono::seconds(5));
	}

	return 0;
}
