cmake_minimum_required(VERSION 3.10)

# Set some basic project attributes
project (dmpcrsfuart
	VERSION 0.1
	DESCRIPTION "Read and dump CRSF messages from UART, for testing purposes")

# This project will output an executable file
add_executable(${PROJECT_NAME} dmpcrsfuart.c)

# Create a simple configuration header
configure_file(config.h.in config.h)

# Include the configuration header in the build
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_BINARY_DIR}" ../crsf ../uart)

# Add crsf library
target_link_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/../crsf/build/cmake.run.linux.x86_64.Local") # -L../crsf/build/cmake.run.linux.x86_64.Local
target_link_libraries(${PROJECT_NAME} crsf) # -lcrsf

# Add uart library
target_link_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/../uart/build/cmake.run.linux.x86_64.Local") # -L../uart/build/cmake.run.linux.x86_64.Local
target_link_libraries(${PROJECT_NAME} uart) # -luart