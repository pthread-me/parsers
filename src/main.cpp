#include <constants.hpp>
#include <cstdint>
#include <string>
#include <type_traits>
#include <wave.hpp>
#include <formatters.hpp>



template <uint_type T>
std::string fun(const T& val){
	u64 n = std::min(sizeof(val), MAX_CASTABLE_SIZE);
	char buf[MAX_CASTABLE_SIZE]= {};
	std::memcpy(buf, &val, n);
	return std::string(buf);
} 


using namespace std;
int main(){
  //wave::File wave_file("tests/sample-9s.wav");
  //std::array<u32,1> test{83};
  u32 test = 82;
  println("{}",field_to_str(test));
  log_str(std::cout, test);
}



