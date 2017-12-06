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

include_directories("include")

FILE(GLOB husky-base-sources "src/*.cc")

if(ENABLE_SHARED)
  add_library(husky-base SHARED ${husky-base-sources})
  set_property(TARGET husky-base PROPERTY CXX_STANDARD ${CMAKE_CXX_STANDARD})
else()
  add_library(husky-base STATIC ${husky-base-sources})
  set_property(TARGET husky-base PROPERTY CXX_STANDARD ${CMAKE_CXX_STANDARD})
endif()

include(CTest)
add_subdirectory(test)
