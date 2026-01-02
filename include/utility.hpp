#ifndef CONSTANTS
#define CONSTANTS

#include <formatters.hpp>

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <print>
#include <iostream>
#include <string>
#include <ranges>
#include <type_traits>


using byte = std::byte;
using u8 = uint8_t; // not to be used in place of a byte
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

static constexpr u64 MAX_CASTABLE_SIZE = 64; //bytes


template <typename T>
concept vec_type = std::ranges::contiguous_range<T> && requires(T& t){
	t.size() < MAX_CASTABLE_SIZE; t.data();
};

template <typename T>
concept uint_type = std::is_arithmetic_v<T>; 


template <typename T>
concept has_formatter = !uint_type<T> && requires (T& t) {
	is_formatted<T>{} == std::true_type{};
};




template <typename T>
concept safe_cast_to_str = vec_type<T> || uint_type<T> || has_formatter<T>;


template <vec_type T>
std::string field_to_str(const T& vec){
	u64 n = std::min(vec.size(), MAX_CASTABLE_SIZE);
	char buf[MAX_CASTABLE_SIZE]= {};
	std::memcpy(buf, vec.data(), n);
	return std::string(buf);
}

template <uint_type T>
std::string field_to_str(const T& val){
	u64 n = std::min(sizeof(val), MAX_CASTABLE_SIZE);
	char buf[MAX_CASTABLE_SIZE]= {};
	std::memcpy(buf, &val, n);
	return std::string(buf);
} 

template <has_formatter T>
std:: string field_to_str(const T& t){
	return std::format("{}", t);
}


template <typename First, typename ...Rest>
void log(std::ostream& stream, First& f, Rest&... r) {
  std::print(stream, "{}", f);
  (std::print(stream, " {}", r), ...);
  std::println(stream);
}

template <safe_cast_to_str First, safe_cast_to_str ...Rest>
void log_str(std::ostream& stream, First& f, Rest&... r) {
  std::print(stream, "{}", field_to_str(f));
  (std::print(stream, " {}", field_to_str(r)), ...);
  std::println(stream);
}





/*
 * Fun templating from stack overflow
 * all checks pass if the program is not illformed (aka they exist)
 * std::decval allows us to deal with a type's value without evaluating it (so call memb functions)
 */
/*
template <typename T>
concept is_iterable = requires(T& t) {
  std::begin(t) != std::end(t);
	std::declval<decltype(t)>().begin() != nullptr;
	//std::declval<decltype(std::begin(t))>()++;
	//std::declval<decltype(std::begin(t))>().size();	
  *std::begin(t);
	std::declval<decltype(t)>()[0];
};


template <is_iterable T> 
void log(std::ostream& stream, T& iter){
  if (*iter == nullptr) return;
  std::print(stream, "{}", *(iter++));
  for(auto t=iter.begin(); t!=iter.end(); t+=1){
    std::print(stream, " {}", *t);
  }
}
*/

#endif
