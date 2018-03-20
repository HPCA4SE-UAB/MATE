################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../src/AC/ActiveObject.o \
../src/AC/Address.o \
../src/AC/ByteStream.o \
../src/AC/Config.o \
../src/AC/ConfigException.o \
../src/AC/ConfigMap.o \
../src/AC/ConfigReader.o \
../src/AC/Ctrl.o \
../src/AC/DateTime.o \
../src/AC/ECPMsg.o \
../src/AC/Env.o \
../src/AC/Exception.o \
../src/AC/InstrSet.o \
../src/AC/Monitor.o \
../src/AC/PTPAcceptor.o \
../src/AC/PTPHandler.o \
../src/AC/PTPMsg.o \
../src/AC/PTPMsgHeader.o \
../src/AC/PTPProtocol.o \
../src/AC/Paths.o \
../src/AC/Process.o \
../src/AC/Reactor.o \
../src/AC/SnippetMaker.o \
../src/AC/Socket.o \
../src/AC/SysException.o \
../src/AC/Syslog.o \
../src/AC/Task.o \
../src/AC/TaskInstr.o \
../src/AC/TaskManager.o \
../src/AC/Terminator.o \
../src/AC/Thread.o \
../src/AC/TimeValue.o \
../src/AC/Tuner.o \
../src/AC/Utils.o \
../src/AC/di.o \
../src/AC/main.o 

CPP_SRCS += \
../src/AC/Ctrl.cpp \
../src/AC/InstrSet.cpp \
../src/AC/Monitor.cpp \
../src/AC/PTPAcceptor.cpp \
../src/AC/PTPHandler.cpp \
../src/AC/SnippetMaker.cpp \
../src/AC/Task.cpp \
../src/AC/TaskInstr.cpp \
../src/AC/TaskManager.cpp \
../src/AC/Terminator.cpp \
../src/AC/Tuner.cpp \
../src/AC/main.cpp 

OBJS += \
./src/AC/Ctrl.o \
./src/AC/InstrSet.o \
./src/AC/Monitor.o \
./src/AC/PTPAcceptor.o \
./src/AC/PTPHandler.o \
./src/AC/SnippetMaker.o \
./src/AC/Task.o \
./src/AC/TaskInstr.o \
./src/AC/TaskManager.o \
./src/AC/Terminator.o \
./src/AC/Tuner.o \
./src/AC/main.o 

CPP_DEPS += \
./src/AC/Ctrl.d \
./src/AC/InstrSet.d \
./src/AC/Monitor.d \
./src/AC/PTPAcceptor.d \
./src/AC/PTPHandler.d \
./src/AC/SnippetMaker.d \
./src/AC/Task.d \
./src/AC/TaskInstr.d \
./src/AC/TaskManager.d \
./src/AC/Terminator.d \
./src/AC/Tuner.d \
./src/AC/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/AC/%.o: ../src/AC/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


