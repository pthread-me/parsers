#include <string>

#include <wave.hpp>
#include <formatters.hpp>
#include <utility.hpp>


using namespace std;
int main(){
	wave::File wave("tests/sample-9s.wav");
  wave.save_to_file("result/sample.wav");
}



