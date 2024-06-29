// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from tempo_ros_bridge:srv/TestService.idl
// generated code does not contain a copyright notice

#ifndef TEMPO_ROS_BRIDGE__SRV__DETAIL__TEST_SERVICE__BUILDER_HPP_
#define TEMPO_ROS_BRIDGE__SRV__DETAIL__TEST_SERVICE__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "tempo_ros_bridge/srv/detail/test_service__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace tempo_ros_bridge
{

namespace srv
{

namespace builder
{

class Init_TestService_Request_test_one
{
public:
  Init_TestService_Request_test_one()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::tempo_ros_bridge::srv::TestService_Request test_one(::tempo_ros_bridge::srv::TestService_Request::_test_one_type arg)
  {
    msg_.test_one = std::move(arg);
    return std::move(msg_);
  }

private:
  ::tempo_ros_bridge::srv::TestService_Request msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::tempo_ros_bridge::srv::TestService_Request>()
{
  return tempo_ros_bridge::srv::builder::Init_TestService_Request_test_one();
}

}  // namespace tempo_ros_bridge


namespace tempo_ros_bridge
{

namespace srv
{


}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::tempo_ros_bridge::srv::TestService_Response>()
{
  return ::tempo_ros_bridge::srv::TestService_Response(rosidl_runtime_cpp::MessageInitialization::ZERO);
}

}  // namespace tempo_ros_bridge

#endif  // TEMPO_ROS_BRIDGE__SRV__DETAIL__TEST_SERVICE__BUILDER_HPP_
