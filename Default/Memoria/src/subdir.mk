################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Memoria/src/API.c \
../Memoria/src/Memoria.c 

OBJS += \
./Memoria/src/API.o \
./Memoria/src/Memoria.o 

C_DEPS += \
./Memoria/src/API.d \
./Memoria/src/Memoria.d 


# Each subdirectory must supply rules for building sources it contributes
Memoria/src/%.o: ../Memoria/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


