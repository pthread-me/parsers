// Skeletal file with SFINAE structs to check
// for formatter specializations

#ifndef FORMATTER_SPECIALIZERS
#define FORMATTER_SPECIALIZERS

#include <format>
#include <type_traits>

template <typename T, typename = void>
struct is_formatted : std::false_type{};

template <typename T>
struct is_formatted<T, std::void_t<decltype(std::formatter<T, char>{})>>: std::true_type{};

#endif
