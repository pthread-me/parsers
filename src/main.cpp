#include <string>

#include <wave.hpp>
#include <formatters.hpp>
#include <utility.hpp>



template <uint_type T>
std::string fun(const T& val){
	u64 n = std::min(sizeof(val), MAX_CASTABLE_SIZE);
	char buf[MAX_CASTABLE_SIZE]= {};
	std::memcpy(buf, &val, n);
	return std::string(buf);
} 


using namespace std;
int main(){
	wave::File wave("tests/M1F1-Alaw-AFsp.wav");
}



