#include <iostream>

#include <onion/Timer.hpp>

void SayHello()
{
	auto s = std::format("{:%F %T}", std::chrono::system_clock::now());

	std::cout << s << std::endl;
}

int main()
{
	std::cout << "-------------- Demo Timer --------------" << std::endl;

	onion::Timer timer;

	timer.setElapsedPeriod(std::chrono::seconds(3));
	timer.setRepeat(true);
	timer.setTimeoutFunction([]() { SayHello(); });

	timer.Start();

	std::this_thread::sleep_for(std::chrono::seconds(11));

	timer.setElapsedPeriod(std::chrono::milliseconds(100));

	// Wait for user input to stop.
	int x;
	std::cin >> x;

	return 0;
}
