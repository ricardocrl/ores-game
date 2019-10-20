################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/App.cpp \
../src/Box.cpp \
../src/BoxContainer.cpp \
../src/LevelManager.cpp \
../src/main.cpp 

OBJS += \
./src/App.o \
./src/Box.o \
./src/BoxContainer.o \
./src/LevelManager.o \
./src/main.o 

CPP_DEPS += \
./src/App.d \
./src/Box.d \
./src/BoxContainer.d \
./src/LevelManager.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++1y -D__cplusplus=201402L -D__GXX_EXPERIMENTAL_CXX0X__ -I/usr/include -O0 -g3 -Wall -c -fmessage-length=0 `sdl2-config --cflags` -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


