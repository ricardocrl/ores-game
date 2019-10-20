################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/App.cpp \
../src/Box.cpp \
../src/BoxContainer.cpp \
../src/Level.cpp \
../src/main.cpp 

OBJS += \
./src/App.o \
./src/Box.o \
./src/BoxContainer.o \
./src/Level.o \
./src/main.o 

CPP_DEPS += \
./src/App.d \
./src/Box.d \
./src/BoxContainer.d \
./src/Level.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I/usr/include/c++/5.4.0 -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


