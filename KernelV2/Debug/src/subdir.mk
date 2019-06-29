################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/APIKernel.c \
../src/KernelMainFunctions.c \
../src/KernelOtherFunctions.c \
../src/KernelV2.c 

OBJS += \
./src/APIKernel.o \
./src/KernelMainFunctions.o \
./src/KernelOtherFunctions.o \
./src/KernelV2.o 

C_DEPS += \
./src/APIKernel.d \
./src/KernelMainFunctions.d \
./src/KernelOtherFunctions.d \
./src/KernelV2.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2019-1c-Los-Sisoperadores/Libraries" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


