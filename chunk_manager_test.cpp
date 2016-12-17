#include <iostream>
#include "chunk_manager_sys.h"

int main()
{
	gen::StringChunkManager<0x1000, gen::ConservativeStoragePolicy> strings;
	const char *str1 = strings.store("Hello World!");
	const char *str2 = strings.store(std::string("Hello World!"));

	std::cout << str1 << "\n";
	std::cout << str2 << "\n";

	return 0;
}