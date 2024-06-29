// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from tempo_ros_bridge:srv/TestService.idl
// generated code does not contain a copyright notice

#ifndef TEMPO_ROS_BRIDGE__SRV__DETAIL__TEST_SERVICE__STRUCT_HPP_
#define TEMPO_ROS_BRIDGE__SRV__DETAIL__TEST_SERVICE__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


// Include directives for member types
// Member 'test_one'
#include "tempo_ros_bridge/msg/detail/test_one__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__tempo_ros_bridge__srv__TestService_Request __attribute__((deprecated))
#else
# define DEPRECATED__tempo_ros_bridge__srv__TestService_Request __declspec(deprecated)
#endif

namespace tempo_ros_bridge
{

namespace srv
{

// message struct
template<class ContainerAllocator>
struct TestService_Request_
{
  using Type = TestService_Request_<ContainerAllocator>;

  explicit TestService_Request_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : test_one(_init)
  {
    (void)_init;
  }

  explicit TestService_Request_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : test_one(_alloc, _init)
  {
    (void)_init;
  }

  // field types and members
  using _test_one_type =
    tempo_ros_bridge::msg::TestOne_<ContainerAllocator>;
  _test_one_type test_one;

  // setters for named parameter idiom
  Type & set__test_one(
    const tempo_ros_bridge::msg::TestOne_<ContainerAllocator> & _arg)
  {
    this->test_one = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    tempo_ros_bridge::srv::TestService_Request_<ContainerAllocator> *;
  using ConstRawPtr =
    const tempo_ros_bridge::srv::TestService_Request_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<tempo_ros_bridge::srv::TestService_Request_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<tempo_ros_bridge::srv::TestService_Request_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      tempo_ros_bridge::srv::TestService_Request_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<tempo_ros_bridge::srv::TestService_Request_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      tempo_ros_bridge::srv::TestService_Request_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<tempo_ros_bridge::srv::TestService_Request_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<tempo_ros_bridge::srv::TestService_Request_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<tempo_ros_bridge::srv::TestService_Request_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__tempo_ros_bridge__srv__TestService_Request
    std::shared_ptr<tempo_ros_bridge::srv::TestService_Request_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__tempo_ros_bridge__srv__TestService_Request
    std::shared_ptr<tempo_ros_bridge::srv::TestService_Request_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const TestService_Request_ & other) const
  {
    if (this->test_one != other.test_one) {
      return false;
    }
    return true;
  }
  bool operator!=(const TestService_Request_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct TestService_Request_

// alias to use template instance with default allocator
using TestService_Request =
  tempo_ros_bridge::srv::TestService_Request_<std::allocator<void>>;

// constant definitions

}  // namespace srv

}  // namespace tempo_ros_bridge


#ifndef _WIN32
# define DEPRECATED__tempo_ros_bridge__srv__TestService_Response __attribute__((deprecated))
#else
# define DEPRECATED__tempo_ros_bridge__srv__TestService_Response __declspec(deprecated)
#endif

namespace tempo_ros_bridge
{

namespace srv
{

// message struct
template<class ContainerAllocator>
struct TestService_Response_
{
  using Type = TestService_Response_<ContainerAllocator>;

  explicit TestService_Response_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->structure_needs_at_least_one_member = 0;
    }
  }

  explicit TestService_Response_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
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
    tempo_ros_bridge::srv::TestService_Response_<ContainerAllocator> *;
  using ConstRawPtr =
    const tempo_ros_bridge::srv::TestService_Response_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<tempo_ros_bridge::srv::TestService_Response_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<tempo_ros_bridge::srv::TestService_Response_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      tempo_ros_bridge::srv::TestService_Response_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<tempo_ros_bridge::srv::TestService_Response_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      tempo_ros_bridge::srv::TestService_Response_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<tempo_ros_bridge::srv::TestService_Response_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<tempo_ros_bridge::srv::TestService_Response_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<tempo_ros_bridge::srv::TestService_Response_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__tempo_ros_bridge__srv__TestService_Response
    std::shared_ptr<tempo_ros_bridge::srv::TestService_Response_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__tempo_ros_bridge__srv__TestService_Response
    std::shared_ptr<tempo_ros_bridge::srv::TestService_Response_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const TestService_Response_ & other) const
  {
    if (this->structure_needs_at_least_one_member != other.structure_needs_at_least_one_member) {
      return false;
    }
    return true;
  }
  bool operator!=(const TestService_Response_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct TestService_Response_

// alias to use template instance with default allocator
using TestService_Response =
  tempo_ros_bridge::srv::TestService_Response_<std::allocator<void>>;

// constant definitions

}  // namespace srv

}  // namespace tempo_ros_bridge

namespace tempo_ros_bridge
{

namespace srv
{

struct TestService
{
  using Request = tempo_ros_bridge::srv::TestService_Request;
  using Response = tempo_ros_bridge::srv::TestService_Response;
};

}  // namespace srv

}  // namespace tempo_ros_bridge

#endif  // TEMPO_ROS_BRIDGE__SRV__DETAIL__TEST_SERVICE__STRUCT_HPP_
