// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from tempo_ros_bridge:msg/TestOne.idl
// generated code does not contain a copyright notice

#ifndef TEMPO_ROS_BRIDGE__MSG__DETAIL__TEST_ONE__TRAITS_HPP_
#define TEMPO_ROS_BRIDGE__MSG__DETAIL__TEST_ONE__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "tempo_ros_bridge/msg/detail/test_one__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

namespace tempo_ros_bridge
{

namespace msg
{

inline void to_flow_style_yaml(
  const TestOne & msg,
  std::ostream & out)
{
  out << "{";
  // member: num
  {
    out << "num: ";
    rosidl_generator_traits::value_to_yaml(msg.num, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const TestOne & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: num
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "num: ";
    rosidl_generator_traits::value_to_yaml(msg.num, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const TestOne & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace msg

}  // namespace tempo_ros_bridge

namespace rosidl_generator_traits
{

[[deprecated("use tempo_ros_bridge::msg::to_block_style_yaml() instead")]]
inline void to_yaml(
  const tempo_ros_bridge::msg::TestOne & msg,
  std::ostream & out, size_t indentation = 0)
{
  tempo_ros_bridge::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use tempo_ros_bridge::msg::to_yaml() instead")]]
inline std::string to_yaml(const tempo_ros_bridge::msg::TestOne & msg)
{
  return tempo_ros_bridge::msg::to_yaml(msg);
}

template<>
inline const char * data_type<tempo_ros_bridge::msg::TestOne>()
{
  return "tempo_ros_bridge::msg::TestOne";
}

template<>
inline const char * name<tempo_ros_bridge::msg::TestOne>()
{
  return "tempo_ros_bridge/msg/TestOne";
}

template<>
struct has_fixed_size<tempo_ros_bridge::msg::TestOne>
  : std::integral_constant<bool, true> {};

template<>
struct has_bounded_size<tempo_ros_bridge::msg::TestOne>
  : std::integral_constant<bool, true> {};

template<>
struct is_message<tempo_ros_bridge::msg::TestOne>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // TEMPO_ROS_BRIDGE__MSG__DETAIL__TEST_ONE__TRAITS_HPP_
