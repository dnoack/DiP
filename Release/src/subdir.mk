################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/I2c.cpp \
../src/I2cPlugin.cpp 

OBJS += \
./src/I2c.o \
./src/I2cPlugin.o 

CPP_DEPS += \
./src/I2c.d \
./src/I2cPlugin.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/dave2/git/rpcUtils/include" -I/home/dave2/git/rapidjson/include/rapidjson -I"/home/dave2/git/I2C-Plugin/include" -O0 -Wall -c -fmessage-length=0 ${CXXFLAGS} -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


