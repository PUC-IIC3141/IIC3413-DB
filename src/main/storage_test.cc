#include "storage/index/object_file/object_file.h"

#include <chrono>
#include <climits>
#include <iostream>

using namespace std;

void print(std::vector<unsigned char> vec){
	for(unsigned char c: vec){
		std::cout << c;
	}
	std::cout << endl;
}

int main(int argc, char *argv[]) {
	const std::string& filename = argv[1];
	ObjectFile object_file(filename);
	int n;
	std::cin >> n;
	uint64_t ids[n];
	for(int i = 0; i < n; i++){
		std::string str;
		std::cin >> str;
		std::vector<unsigned char> bytevec(str.begin(), str.end());
		ids[i] = object_file.write(bytevec);
		cout << ids[i] << endl;
	}
	int m;
	std::cin >> m;
	for(int i = 0; i < m; i++){
		uint64_t q;
		std::cin >> q;
		auto value = object_file.read(q);
		print(*value);
	}
	return 0;
}
