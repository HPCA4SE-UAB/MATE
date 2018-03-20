################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../src/DMLib/Address.o \
../src/DMLib/ByteStream.o \
../src/DMLib/Config.o \
../src/DMLib/ConfigException.o \
../src/DMLib/ConfigMap.o \
../src/DMLib/ConfigReader.o \
../src/DMLib/DMLibApi.o \
../src/DMLib/DateTime.o \
../src/DMLib/ECPMsg.o \
../src/DMLib/ECPMsgHeader.o \
../src/DMLib/ECPProxy.o \
../src/DMLib/Env.o \
../src/DMLib/EventMsgWriter.o \
../src/DMLib/Exception.o \
../src/DMLib/NetSer.o \
../src/DMLib/Paths.o \
../src/DMLib/Pipe.o \
../src/DMLib/Reactor.o \
../src/DMLib/Socket.o \
../src/DMLib/SysException.o \
../src/DMLib/Syslog.o \
../src/DMLib/TimeValue.o \
../src/DMLib/Utils.o \
../src/DMLib/sync.o 

CPP_SRCS += \
../src/DMLib/DMLibApi.cpp \
../src/DMLib/ECPProxy.cpp \
../src/DMLib/EventMsgWriter.cpp 

OBJS += \
./src/DMLib/DMLibApi.o \
./src/DMLib/ECPProxy.o \
./src/DMLib/EventMsgWriter.o 

CPP_DEPS += \
./src/DMLib/DMLibApi.d \
./src/DMLib/ECPProxy.d \
./src/DMLib/EventMsgWriter.d 


# Each subdirectory must supply rules for building sources it contributes
src/DMLib/%.o: ../src/DMLib/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


