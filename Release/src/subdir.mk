################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/ComServer.cpp \
../src/I2c.cpp \
../src/I2cPlugin.cpp \
../src/RegClient.cpp 

OBJS += \
./src/ComServer.o \
./src/I2c.o \
./src/I2cPlugin.o \
./src/RegClient.o 

CPP_DEPS += \
./src/ComServer.d \
./src/I2c.d \
./src/I2cPlugin.d \
./src/RegClient.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/dave2/git/rpcUtils/include" -I/home/dave2/git/rapidjson/include/rapidjson -I"/home/dave2/git/I2C-Plugin/include" -O0 -Wall -c -fmessage-length=0 ${CFLAGS} -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


