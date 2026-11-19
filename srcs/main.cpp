#include <iostream>
#include <string>
#include <stdexcept>
#include <csignal>
#include <unistd.h>
#include "../includes/Computor.hpp"
#include "../includes/Complex.hpp"

static volatile sig_atomic_t g_interrupted = 0;

static void onSigInt(int) { g_interrupted = 1; }

static void installSigInt()
{
      struct sigaction sa;
      sa.sa_handler = onSigInt;
      sigemptyset(&sa.sa_mask);
      sa.sa_flags = 0;
      sigaction(SIGINT, &sa, nullptr);
}

static std::string trim(const std::string &s)
{
      size_t      b = s.find_first_not_of(" \t\r\n");
      if (b == std::string::npos)
          return "";
      size_t      e = s.find_last_not_of(" \t\r\n");
      return s.substr(b, e - b + 1);
}

int main()
{
	installSigInt();
	const bool  interactive = isatty(STDIN_FILENO);
	Computor    computor;
	std::string line;
	Complex		c;

	while (true)
	{
		if (interactive)
				std::cout << "> " << std::flush;
		if (!std::getline(std::cin, line))
		{
				if (interactive)
					std::cout << std::endl;
				break;
		}
		line = trim(line);
		if (line.empty())
				continue;
		if (line == "exit" || line == "quit")
				break;
		try
		{
				std::cout << computor.process(line) << std::endl;
		}
		catch (const std::exception &e)
		{
				std::cerr << "    Error: " << e.what() << std::endl;
		}
	}
	return 0;
}