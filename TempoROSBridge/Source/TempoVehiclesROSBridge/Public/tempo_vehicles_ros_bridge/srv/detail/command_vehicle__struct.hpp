// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from tempo_vehicles_ros_bridge:srv/CommandVehicle.idl
// generated code does not contain a copyright notice

#ifndef TEMPO_VEHICLES_ROS_BRIDGE__SRV__DETAIL__COMMAND_VEHICLE__STRUCT_HPP_
#define TEMPO_VEHICLES_ROS_BRIDGE__SRV__DETAIL__COMMAND_VEHICLE__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__tempo_vehicles_ros_bridge__srv__CommandVehicle_Request __attribute__((deprecated))
#else
# define DEPRECATED__tempo_vehicles_ros_bridge__srv__CommandVehicle_Request __declspec(deprecated)
#endif

namespace tempo_vehicles_ros_bridge
{

namespace srv
{

// message struct
template<class ContainerAllocator>
struct CommandVehicle_Request_
{
  using Type = CommandVehicle_Request_<ContainerAllocator>;

  explicit CommandVehicle_Request_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->vehicle_name = "";
      this->acceleration = 0.0f;
      this->steering = 0.0f;
    }
  }

  explicit CommandVehicle_Request_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : vehicle_name(_alloc)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->vehicle_name = "";
      this->acceleration = 0.0f;
      this->steering = 0.0f;
    }
  }

  // field types and members
  using _vehicle_name_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _vehicle_name_type vehicle_name;
  using _acceleration_type =
    float;
  _acceleration_type acceleration;
  using _steering_type =
    float;
  _steering_type steering;

  // setters for named parameter idiom
  Type & set__vehicle_name(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->vehicle_name = _arg;
    return *this;
  }
  Type & set__acceleration(
    const float & _arg)
  {
    this->acceleration = _arg;
    return *this;
  }
  Type & set__steering(
    const float & _arg)
  {
    this->steering = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    tempo_vehicles_ros_bridge::srv::CommandVehicle_Request_<ContainerAllocator> *;
  using ConstRawPtr =
    const tempo_vehicles_ros_bridge::srv::CommandVehicle_Request_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<tempo_vehicles_ros_bridge::srv::CommandVehicle_Request_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<tempo_vehicles_ros_bridge::srv::CommandVehicle_Request_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      tempo_vehicles_ros_bridge::srv::CommandVehicle_Request_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<tempo_vehicles_ros_bridge::srv::CommandVehicle_Request_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      tempo_vehicles_ros_bridge::srv::CommandVehicle_Request_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<tempo_vehicles_ros_bridge::srv::CommandVehicle_Request_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<tempo_vehicles_ros_bridge::srv::CommandVehicle_Request_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<tempo_vehicles_ros_bridge::srv::CommandVehicle_Request_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__tempo_vehicles_ros_bridge__srv__CommandVehicle_Request
    std::shared_ptr<tempo_vehicles_ros_bridge::srv::CommandVehicle_Request_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__tempo_vehicles_ros_bridge__srv__CommandVehicle_Request
    std::shared_ptr<tempo_vehicles_ros_bridge::srv::CommandVehicle_Request_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const CommandVehicle_Request_ & other) const
  {
    if (this->vehicle_name != other.vehicle_name) {
      return false;
    }
    if (this->acceleration != other.acceleration) {
      return false;
    }
    if (this->steering != other.steering) {
      return false;
    }
    return true;
  }
  bool operator!=(const CommandVehicle_Request_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct CommandVehicle_Request_

// alias to use template instance with default allocator
using CommandVehicle_Request =
  tempo_vehicles_ros_bridge::srv::CommandVehicle_Request_<std::allocator<void>>;

// constant definitions

}  // namespace srv

}  // namespace tempo_vehicles_ros_bridge


#ifndef _WIN32
# define DEPRECATED__tempo_vehicles_ros_bridge__srv__CommandVehicle_Response __attribute__((deprecated))
#else
# define DEPRECATED__tempo_vehicles_ros_bridge__srv__CommandVehicle_Response __declspec(deprecated)
#endif

namespace tempo_vehicles_ros_bridge
{

namespace srv
{

// message struct
template<class ContainerAllocator>
struct CommandVehicle_Response_
{
  using Type = CommandVehicle_Response_<ContainerAllocator>;

  explicit CommandVehicle_Response_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->structure_needs_at_least_one_member = 0;
    }
  }

  explicit CommandVehicle_Response_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
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
    tempo_vehicles_ros_bridge::srv::CommandVehicle_Response_<ContainerAllocator> *;
  using ConstRawPtr =
    const tempo_vehicles_ros_bridge::srv::CommandVehicle_Response_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<tempo_vehicles_ros_bridge::srv::CommandVehicle_Response_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<tempo_vehicles_ros_bridge::srv::CommandVehicle_Response_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      tempo_vehicles_ros_bridge::srv::CommandVehicle_Response_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<tempo_vehicles_ros_bridge::srv::CommandVehicle_Response_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      tempo_vehicles_ros_bridge::srv::CommandVehicle_Response_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<tempo_vehicles_ros_bridge::srv::CommandVehicle_Response_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<tempo_vehicles_ros_bridge::srv::CommandVehicle_Response_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<tempo_vehicles_ros_bridge::srv::CommandVehicle_Response_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__tempo_vehicles_ros_bridge__srv__CommandVehicle_Response
    std::shared_ptr<tempo_vehicles_ros_bridge::srv::CommandVehicle_Response_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__tempo_vehicles_ros_bridge__srv__CommandVehicle_Response
    std::shared_ptr<tempo_vehicles_ros_bridge::srv::CommandVehicle_Response_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const CommandVehicle_Response_ & other) const
  {
    if (this->structure_needs_at_least_one_member != other.structure_needs_at_least_one_member) {
      return false;
    }
    return true;
  }
  bool operator!=(const CommandVehicle_Response_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct CommandVehicle_Response_

// alias to use template instance with default allocator
using CommandVehicle_Response =
  tempo_vehicles_ros_bridge::srv::CommandVehicle_Response_<std::allocator<void>>;

// constant definitions

}  // namespace srv

}  // namespace tempo_vehicles_ros_bridge

namespace tempo_vehicles_ros_bridge
{

namespace srv
{

struct CommandVehicle
{
  using Request = tempo_vehicles_ros_bridge::srv::CommandVehicle_Request;
  using Response = tempo_vehicles_ros_bridge::srv::CommandVehicle_Response;
};

}  // namespace srv

}  // namespace tempo_vehicles_ros_bridge

#endif  // TEMPO_VEHICLES_ROS_BRIDGE__SRV__DETAIL__COMMAND_VEHICLE__STRUCT_HPP_
