# All rights reserved by 
# Durga Keerthi Mandarapu, Vani Nagarajan, Artem Pelenitsyn, and Milind Kulkarni. 2024. 
# Arkade: k-Nearest Neighbor Search With Non-Euclidean Distances using GPU Ray Tracing. 

add_compile_definitions(KN=${KN})
add_compile_definitions(NORM=${NORM})
add_definitions(-DENHANCE)

embed_ptx(
  OUTPUT_TARGET
    sample02-withTrueknn-ptx
  PTX_LINK_LIBRARIES
    owl::owl
  SOURCES
    deviceCode.cu
)

add_executable(sample02-withTrueknn hostCode.cpp)
target_link_libraries(sample02-withTrueknn
  PRIVATE
    sample02-withTrueknn-ptx
    owl::owl
)
