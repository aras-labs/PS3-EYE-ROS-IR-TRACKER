# Install script for directory: /home/rooholla/ROS/PS3-EYE-ROS-IR-TRACKER/catkin_ws/src/camera_image_publisher

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/rooholla/ROS/PS3-EYE-ROS-IR-TRACKER/catkin_ws/install")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/camera_image_publisher/msg" TYPE FILE FILES "/home/rooholla/ROS/PS3-EYE-ROS-IR-TRACKER/catkin_ws/src/camera_image_publisher/msg/mono_camera.msg")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/camera_image_publisher/cmake" TYPE FILE FILES "/home/rooholla/ROS/PS3-EYE-ROS-IR-TRACKER/catkin_ws/build/camera_image_publisher/catkin_generated/installspace/camera_image_publisher-msg-paths.cmake")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE DIRECTORY FILES "/home/rooholla/ROS/PS3-EYE-ROS-IR-TRACKER/catkin_ws/devel/include/camera_image_publisher")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/roseus/ros" TYPE DIRECTORY FILES "/home/rooholla/ROS/PS3-EYE-ROS-IR-TRACKER/catkin_ws/devel/share/roseus/ros/camera_image_publisher")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/common-lisp/ros" TYPE DIRECTORY FILES "/home/rooholla/ROS/PS3-EYE-ROS-IR-TRACKER/catkin_ws/devel/share/common-lisp/ros/camera_image_publisher")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/gennodejs/ros" TYPE DIRECTORY FILES "/home/rooholla/ROS/PS3-EYE-ROS-IR-TRACKER/catkin_ws/devel/share/gennodejs/ros/camera_image_publisher")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  execute_process(COMMAND "/usr/bin/python2" -m compileall "/home/rooholla/ROS/PS3-EYE-ROS-IR-TRACKER/catkin_ws/devel/lib/python2.7/dist-packages/camera_image_publisher")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/python2.7/dist-packages" TYPE DIRECTORY FILES "/home/rooholla/ROS/PS3-EYE-ROS-IR-TRACKER/catkin_ws/devel/lib/python2.7/dist-packages/camera_image_publisher")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/home/rooholla/ROS/PS3-EYE-ROS-IR-TRACKER/catkin_ws/build/camera_image_publisher/catkin_generated/installspace/camera_image_publisher.pc")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/camera_image_publisher/cmake" TYPE FILE FILES "/home/rooholla/ROS/PS3-EYE-ROS-IR-TRACKER/catkin_ws/build/camera_image_publisher/catkin_generated/installspace/camera_image_publisher-msg-extras.cmake")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/camera_image_publisher/cmake" TYPE FILE FILES
    "/home/rooholla/ROS/PS3-EYE-ROS-IR-TRACKER/catkin_ws/build/camera_image_publisher/catkin_generated/installspace/camera_image_publisherConfig.cmake"
    "/home/rooholla/ROS/PS3-EYE-ROS-IR-TRACKER/catkin_ws/build/camera_image_publisher/catkin_generated/installspace/camera_image_publisherConfig-version.cmake"
    )
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/camera_image_publisher" TYPE FILE FILES "/home/rooholla/ROS/PS3-EYE-ROS-IR-TRACKER/catkin_ws/src/camera_image_publisher/package.xml")
endif()

