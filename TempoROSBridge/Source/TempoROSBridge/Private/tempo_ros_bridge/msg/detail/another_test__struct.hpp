// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from tempo_ros_bridge:msg/AnotherTest.idl
// generated code does not contain a copyright notice

#ifndef TEMPO_ROS_BRIDGE__MSG__DETAIL__ANOTHER_TEST__STRUCT_HPP_
#define TEMPO_ROS_BRIDGE__MSG__DETAIL__ANOTHER_TEST__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__tempo_ros_bridge__msg__AnotherTest __attribute__((deprecated))
#else
# define DEPRECATED__tempo_ros_bridge__msg__AnotherTest __declspec(deprecated)
#endif

namespace tempo_ros_bridge
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct AnotherTest_
{
  using Type = AnotherTest_<ContainerAllocator>;

  explicit AnotherTest_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->num = 0ll;
    }
  }

  explicit AnotherTest_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    (void)_alloc;
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->num = 0ll;
    }
  }

  // field types and members
  using _num_type =
    int64_t;
  _num_type num;

  // setters for named parameter idiom
  Type & set__num(
    const int64_t & _arg)
  {
    this->num = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    tempo_ros_bridge::msg::AnotherTest_<ContainerAllocator> *;
  using ConstRawPtr =
    const tempo_ros_bridge::msg::AnotherTest_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<tempo_ros_bridge::msg::AnotherTest_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<tempo_ros_bridge::msg::AnotherTest_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      tempo_ros_bridge::msg::AnotherTest_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<tempo_ros_bridge::msg::AnotherTest_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      tempo_ros_bridge::msg::AnotherTest_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<tempo_ros_bridge::msg::AnotherTest_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<tempo_ros_bridge::msg::AnotherTest_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<tempo_ros_bridge::msg::AnotherTest_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__tempo_ros_bridge__msg__AnotherTest
    std::shared_ptr<tempo_ros_bridge::msg::AnotherTest_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__tempo_ros_bridge__msg__AnotherTest
    std::shared_ptr<tempo_ros_bridge::msg::AnotherTest_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const AnotherTest_ & other) const
  {
    if (this->num != other.num) {
      return false;
    }
    return true;
  }
  bool operator!=(const AnotherTest_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct AnotherTest_

// alias to use template instance with default allocator
using AnotherTest =
  tempo_ros_bridge::msg::AnotherTest_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace tempo_ros_bridge

#endif  // TEMPO_ROS_BRIDGE__MSG__DETAIL__ANOTHER_TEST__STRUCT_HPP_
