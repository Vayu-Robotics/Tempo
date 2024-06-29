// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from tempo_ros_bridge:srv/TestService.idl
// generated code does not contain a copyright notice

#ifndef TEMPO_ROS_BRIDGE__SRV__DETAIL__TEST_SERVICE__TRAITS_HPP_
#define TEMPO_ROS_BRIDGE__SRV__DETAIL__TEST_SERVICE__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "tempo_ros_bridge/srv/detail/test_service__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'test_one'
#include "tempo_ros_bridge/msg/detail/test_one__traits.hpp"

namespace tempo_ros_bridge
{

namespace srv
{

inline void to_flow_style_yaml(
  const TestService_Request & msg,
  std::ostream & out)
{
  out << "{";
  // member: test_one
  {
    out << "test_one: ";
    to_flow_style_yaml(msg.test_one, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const TestService_Request & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: test_one
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "test_one:\n";
    to_block_style_yaml(msg.test_one, out, indentation + 2);
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const TestService_Request & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace srv

}  // namespace tempo_ros_bridge

namespace rosidl_generator_traits
{

[[deprecated("use tempo_ros_bridge::srv::to_block_style_yaml() instead")]]
inline void to_yaml(
  const tempo_ros_bridge::srv::TestService_Request & msg,
  std::ostream & out, size_t indentation = 0)
{
  tempo_ros_bridge::srv::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use tempo_ros_bridge::srv::to_yaml() instead")]]
inline std::string to_yaml(const tempo_ros_bridge::srv::TestService_Request & msg)
{
  return tempo_ros_bridge::srv::to_yaml(msg);
}

template<>
inline const char * data_type<tempo_ros_bridge::srv::TestService_Request>()
{
  return "tempo_ros_bridge::srv::TestService_Request";
}

template<>
inline const char * name<tempo_ros_bridge::srv::TestService_Request>()
{
  return "tempo_ros_bridge/srv/TestService_Request";
}

template<>
struct has_fixed_size<tempo_ros_bridge::srv::TestService_Request>
  : std::integral_constant<bool, has_fixed_size<tempo_ros_bridge::msg::TestOne>::value> {};

template<>
struct has_bounded_size<tempo_ros_bridge::srv::TestService_Request>
  : std::integral_constant<bool, has_bounded_size<tempo_ros_bridge::msg::TestOne>::value> {};

template<>
struct is_message<tempo_ros_bridge::srv::TestService_Request>
  : std::true_type {};

}  // namespace rosidl_generator_traits

namespace tempo_ros_bridge
{

namespace srv
{

inline void to_flow_style_yaml(
  const TestService_Response & msg,
  std::ostream & out)
{
  (void)msg;
  out << "null";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const TestService_Response & msg,
  std::ostream & out, size_t indentation = 0)
{
  (void)msg;
  (void)indentation;
  out << "null\n";
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const TestService_Response & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace srv

}  // namespace tempo_ros_bridge

namespace rosidl_generator_traits
{

[[deprecated("use tempo_ros_bridge::srv::to_block_style_yaml() instead")]]
inline void to_yaml(
  const tempo_ros_bridge::srv::TestService_Response & msg,
  std::ostream & out, size_t indentation = 0)
{
  tempo_ros_bridge::srv::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use tempo_ros_bridge::srv::to_yaml() instead")]]
inline std::string to_yaml(const tempo_ros_bridge::srv::TestService_Response & msg)
{
  return tempo_ros_bridge::srv::to_yaml(msg);
}

template<>
inline const char * data_type<tempo_ros_bridge::srv::TestService_Response>()
{
  return "tempo_ros_bridge::srv::TestService_Response";
}

template<>
inline const char * name<tempo_ros_bridge::srv::TestService_Response>()
{
  return "tempo_ros_bridge/srv/TestService_Response";
}

template<>
struct has_fixed_size<tempo_ros_bridge::srv::TestService_Response>
  : std::integral_constant<bool, true> {};

template<>
struct has_bounded_size<tempo_ros_bridge::srv::TestService_Response>
  : std::integral_constant<bool, true> {};

template<>
struct is_message<tempo_ros_bridge::srv::TestService_Response>
  : std::true_type {};

}  // namespace rosidl_generator_traits

namespace rosidl_generator_traits
{

template<>
inline const char * data_type<tempo_ros_bridge::srv::TestService>()
{
  return "tempo_ros_bridge::srv::TestService";
}

template<>
inline const char * name<tempo_ros_bridge::srv::TestService>()
{
  return "tempo_ros_bridge/srv/TestService";
}

template<>
struct has_fixed_size<tempo_ros_bridge::srv::TestService>
  : std::integral_constant<
    bool,
    has_fixed_size<tempo_ros_bridge::srv::TestService_Request>::value &&
    has_fixed_size<tempo_ros_bridge::srv::TestService_Response>::value
  >
{
};

template<>
struct has_bounded_size<tempo_ros_bridge::srv::TestService>
  : std::integral_constant<
    bool,
    has_bounded_size<tempo_ros_bridge::srv::TestService_Request>::value &&
    has_bounded_size<tempo_ros_bridge::srv::TestService_Response>::value
  >
{
};

template<>
struct is_service<tempo_ros_bridge::srv::TestService>
  : std::true_type
{
};

template<>
struct is_service_request<tempo_ros_bridge::srv::TestService_Request>
  : std::true_type
{
};

template<>
struct is_service_response<tempo_ros_bridge::srv::TestService_Response>
  : std::true_type
{
};

}  // namespace rosidl_generator_traits

#endif  // TEMPO_ROS_BRIDGE__SRV__DETAIL__TEST_SERVICE__TRAITS_HPP_
