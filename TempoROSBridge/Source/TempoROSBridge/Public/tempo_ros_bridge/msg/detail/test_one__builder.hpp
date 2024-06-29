// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from tempo_ros_bridge:msg/TestOne.idl
// generated code does not contain a copyright notice

#ifndef TEMPO_ROS_BRIDGE__MSG__DETAIL__TEST_ONE__BUILDER_HPP_
#define TEMPO_ROS_BRIDGE__MSG__DETAIL__TEST_ONE__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "tempo_ros_bridge/msg/detail/test_one__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace tempo_ros_bridge
{

namespace msg
{

namespace builder
{

class Init_TestOne_num
{
public:
  Init_TestOne_num()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::tempo_ros_bridge::msg::TestOne num(::tempo_ros_bridge::msg::TestOne::_num_type arg)
  {
    msg_.num = std::move(arg);
    return std::move(msg_);
  }

private:
  ::tempo_ros_bridge::msg::TestOne msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::tempo_ros_bridge::msg::TestOne>()
{
  return tempo_ros_bridge::msg::builder::Init_TestOne_num();
}

}  // namespace tempo_ros_bridge

#endif  // TEMPO_ROS_BRIDGE__MSG__DETAIL__TEST_ONE__BUILDER_HPP_
