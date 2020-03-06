################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Lab1.1.3AVSA2020.cpp \
../src/ShowManyImages.cpp \
../src/fgseg.cpp 

OBJS += \
./src/Lab1.1.3AVSA2020.o \
./src/ShowManyImages.o \
./src/fgseg.o 

CPP_DEPS += \
./src/Lab1.1.3AVSA2020.d \
./src/ShowManyImages.d \
./src/fgseg.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/opt/installation/OpenCV-3.4.4/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


