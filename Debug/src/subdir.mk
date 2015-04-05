################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/I2c.cpp \
../src/I2cPlugin.cpp \
../src/JsonRPC.cpp \
../src/UdsComWorker.cpp \
../src/UdsRegClient.cpp \
../src/UdsRegWorker.cpp \
../src/UdsServer.cpp 

OBJS += \
./src/I2c.o \
./src/I2cPlugin.o \
./src/JsonRPC.o \
./src/UdsComWorker.o \
./src/UdsRegClient.o \
./src/UdsRegWorker.o \
./src/UdsServer.o 

CPP_DEPS += \
./src/I2c.d \
./src/I2cPlugin.d \
./src/JsonRPC.d \
./src/UdsComWorker.d \
./src/UdsRegClient.d \
./src/UdsRegWorker.d \
./src/UdsServer.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/Dave/workspace/DiP/include" -I/home/dnoack/libs/rapidjson/include/rapidjson -I/home/Dave/libs/rapidjson/include/rapidjson -O0 -g3 -Wall -c -fmessage-length=0 ${CFLAGS} -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


