# Copyright 2017 Husky Team
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

project(husky-base)
cmake_minimum_required(VERSION 3.0.2)

set(CMAKE_CXX_STANDARD 14)

# Dependencies
find_path(ZMQ_INCLUDE_DIR NAMES zmq.hpp)
find_library(ZMQ_LIBRARY NAMES zmq)
if(ZMQ_INCLUDE_DIR AND ZMQ_LIBRARY)
  message(STATUS "Found ZeroMQ:")
  message(STATUS "  (Headers)       ${ZMQ_INCLUDE_DIR}")
  message(STATUS "  (Library)       ${ZMQ_LIBRARY}")
elseif()
  message(ERROR "ZeroMQ not found")
endif()

include_directories(${ZMQ_INCLUDE_DIR})
include_directories("include")

FILE(GLOB husky-base-sources "src/*.cc")

add_library(husky-base STATIC ${husky-base-sources})
target_link_libraries(husky-base ${ZMQ_LIBRARY})
set_property(TARGET husky-base PROPERTY CXX_STANDARD ${CMAKE_CXX_STANDARD})

add_library(husky-base-static STATIC ${husky-base-sources})
target_link_libraries(husky-base-static ${ZMQ_LIBRARY})
set_property(TARGET husky-base-static PROPERTY CXX_STANDARD ${CMAKE_CXX_STANDARD})

add_library(husky-base-shared SHARED ${husky-base-sources})
target_link_libraries(husky-base-shared ${ZMQ_LIBRARY})
set_property(TARGET husky-base-shared PROPERTY CXX_STANDARD ${CMAKE_CXX_STANDARD})

include(CTest)
add_subdirectory(test)
