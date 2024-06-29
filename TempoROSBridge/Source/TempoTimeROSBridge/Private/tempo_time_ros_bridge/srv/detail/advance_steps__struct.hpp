// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from tempo_time_ros_bridge:srv/AdvanceSteps.idl
// generated code does not contain a copyright notice

#ifndef TEMPO_TIME_ROS_BRIDGE__SRV__DETAIL__ADVANCE_STEPS__STRUCT_HPP_
#define TEMPO_TIME_ROS_BRIDGE__SRV__DETAIL__ADVANCE_STEPS__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__tempo_time_ros_bridge__srv__AdvanceSteps_Request __attribute__((deprecated))
#else
# define DEPRECATED__tempo_time_ros_bridge__srv__AdvanceSteps_Request __declspec(deprecated)
#endif

namespace tempo_time_ros_bridge
{

namespace srv
{

// message struct
template<class ContainerAllocator>
struct AdvanceSteps_Request_
{
  using Type = AdvanceSteps_Request_<ContainerAllocator>;

  explicit AdvanceSteps_Request_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->steps = 0l;
    }
  }

  explicit AdvanceSteps_Request_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    (void)_alloc;
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->steps = 0l;
    }
  }

  // field types and members
  using _steps_type =
    int32_t;
  _steps_type steps;

  // setters for named parameter idiom
  Type & set__steps(
    const int32_t & _arg)
  {
    this->steps = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    tempo_time_ros_bridge::srv::AdvanceSteps_Request_<ContainerAllocator> *;
  using ConstRawPtr =
    const tempo_time_ros_bridge::srv::AdvanceSteps_Request_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<tempo_time_ros_bridge::srv::AdvanceSteps_Request_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<tempo_time_ros_bridge::srv::AdvanceSteps_Request_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      tempo_time_ros_bridge::srv::AdvanceSteps_Request_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<tempo_time_ros_bridge::srv::AdvanceSteps_Request_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      tempo_time_ros_bridge::srv::AdvanceSteps_Request_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<tempo_time_ros_bridge::srv::AdvanceSteps_Request_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<tempo_time_ros_bridge::srv::AdvanceSteps_Request_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<tempo_time_ros_bridge::srv::AdvanceSteps_Request_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__tempo_time_ros_bridge__srv__AdvanceSteps_Request
    std::shared_ptr<tempo_time_ros_bridge::srv::AdvanceSteps_Request_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__tempo_time_ros_bridge__srv__AdvanceSteps_Request
    std::shared_ptr<tempo_time_ros_bridge::srv::AdvanceSteps_Request_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const AdvanceSteps_Request_ & other) const
  {
    if (this->steps != other.steps) {
      return false;
    }
    return true;
  }
  bool operator!=(const AdvanceSteps_Request_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct AdvanceSteps_Request_

// alias to use template instance with default allocator
using AdvanceSteps_Request =
  tempo_time_ros_bridge::srv::AdvanceSteps_Request_<std::allocator<void>>;

// constant definitions

}  // namespace srv

}  // namespace tempo_time_ros_bridge


#ifndef _WIN32
# define DEPRECATED__tempo_time_ros_bridge__srv__AdvanceSteps_Response __attribute__((deprecated))
#else
# define DEPRECATED__tempo_time_ros_bridge__srv__AdvanceSteps_Response __declspec(deprecated)
#endif

namespace tempo_time_ros_bridge
{

namespace srv
{

// message struct
template<class ContainerAllocator>
struct AdvanceSteps_Response_
{
  using Type = AdvanceSteps_Response_<ContainerAllocator>;

  explicit AdvanceSteps_Response_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->structure_needs_at_least_one_member = 0;
    }
  }

  explicit AdvanceSteps_Response_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    (void)_alloc;
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->structure_needs_at_least_one_member = 0;
    }
  }

  // field types and members
  using _structure_needs_at_least_one_member_type =
    uint8_t;
  _structure_needs_at_least_one_member_type structure_needs_at_least_one_member;


  // constant declarations

  // pointer types
  using RawPtr =
    tempo_time_ros_bridge::srv::AdvanceSteps_Response_<ContainerAllocator> *;
  using ConstRawPtr =
    const tempo_time_ros_bridge::srv::AdvanceSteps_Response_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<tempo_time_ros_bridge::srv::AdvanceSteps_Response_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<tempo_time_ros_bridge::srv::AdvanceSteps_Response_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      tempo_time_ros_bridge::srv::AdvanceSteps_Response_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<tempo_time_ros_bridge::srv::AdvanceSteps_Response_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      tempo_time_ros_bridge::srv::AdvanceSteps_Response_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<tempo_time_ros_bridge::srv::AdvanceSteps_Response_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<tempo_time_ros_bridge::srv::AdvanceSteps_Response_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<tempo_time_ros_bridge::srv::AdvanceSteps_Response_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__tempo_time_ros_bridge__srv__AdvanceSteps_Response
    std::shared_ptr<tempo_time_ros_bridge::srv::AdvanceSteps_Response_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__tempo_time_ros_bridge__srv__AdvanceSteps_Response
    std::shared_ptr<tempo_time_ros_bridge::srv::AdvanceSteps_Response_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const AdvanceSteps_Response_ & other) const
  {
    if (this->structure_needs_at_least_one_member != other.structure_needs_at_least_one_member) {
      return false;
    }
    return true;
  }
  bool operator!=(const AdvanceSteps_Response_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct AdvanceSteps_Response_

// alias to use template instance with default allocator
using AdvanceSteps_Response =
  tempo_time_ros_bridge::srv::AdvanceSteps_Response_<std::allocator<void>>;

// constant definitions

}  // namespace srv

}  // namespace tempo_time_ros_bridge

namespace tempo_time_ros_bridge
{

namespace srv
{

struct AdvanceSteps
{
  using Request = tempo_time_ros_bridge::srv::AdvanceSteps_Request;
  using Response = tempo_time_ros_bridge::srv::AdvanceSteps_Response;
};

}  // namespace srv

}  // namespace tempo_time_ros_bridge

#endif  // TEMPO_TIME_ROS_BRIDGE__SRV__DETAIL__ADVANCE_STEPS__STRUCT_HPP_
