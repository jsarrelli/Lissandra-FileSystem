################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libraries/commons/collections/dictionary.c \
../Libraries/commons/collections/list.c \
../Libraries/commons/collections/queue.c 

OBJS += \
./Libraries/commons/collections/dictionary.o \
./Libraries/commons/collections/list.o \
./Libraries/commons/collections/queue.o 

C_DEPS += \
./Libraries/commons/collections/dictionary.d \
./Libraries/commons/collections/list.d \
./Libraries/commons/collections/queue.d 


# Each subdirectory must supply rules for building sources it contributes
Libraries/commons/collections/%.o: ../Libraries/commons/collections/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


