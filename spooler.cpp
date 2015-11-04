#include <iostream>

auto static ADD = "addqueue";
auto static RM = "rmqueue";
auto static LS = "showqueue";


int main(int argc, char** argv)
{
	if(argc < 2)
		std::cout << "Usage: spooler CMD\n"
				  << "CMD:\n"
				  << "\t" << ADD << " [filenames]\n"
				  << "\t" << RM  << " [filenames]\n"
				  << "\t" << LS  << " [filenames]\n";
	std::cout << argv[0];
	return 0;
}
