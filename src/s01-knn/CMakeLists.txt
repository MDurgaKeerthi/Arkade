# ======================================================================== #
# Copyright 2019 Ingo Wald                                                 #
#                                                                          #
# Licensed under the Apache License, Version 2.0 (the "License");          #
# you may not use this file except in compliance with the License.         #
# You may obtain a copy of the License at                                  #
#                                                                          #
#     http://www.apache.org/licenses/LICENSE-2.0                           #
#                                                                          #
# Unless required by applicable law or agreed to in writing, software      #
# distributed under the License is distributed on an "AS IS" BASIS,        #
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. #
# See the License for the specific language governing permissions and      #
# limitations under the License.                                           #
# ======================================================================== #

add_compile_definitions(KN=${KN})
add_compile_definitions(NORM=${NORM})
add_definitions(-DENHANCE)

embed_ptx(
  OUTPUT_TARGET
    sample01-knn-ptx
  PTX_LINK_LIBRARIES
    owl::owl
  SOURCES
    deviceCode.cu
)

add_executable(sample01-knn hostCode.cpp)
target_link_libraries(sample01-knn
  PRIVATE
    sample01-knn-ptx
    owl::owl
)
add_test(sample01-knn ${CMAKE_BINARY_DIR}/sample01-knn)
