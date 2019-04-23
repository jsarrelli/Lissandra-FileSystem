################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libraries/commons/bitarray.c \
../Libraries/commons/config.c \
../Libraries/commons/error.c \
../Libraries/commons/log.c \
../Libraries/commons/process.c \
../Libraries/commons/string.c \
../Libraries/commons/temporal.c \
../Libraries/commons/txt.c 

OBJS += \
./Libraries/commons/bitarray.o \
./Libraries/commons/config.o \
./Libraries/commons/error.o \
./Libraries/commons/log.o \
./Libraries/commons/process.o \
./Libraries/commons/string.o \
./Libraries/commons/temporal.o \
./Libraries/commons/txt.o 

C_DEPS += \
./Libraries/commons/bitarray.d \
./Libraries/commons/config.d \
./Libraries/commons/error.d \
./Libraries/commons/log.d \
./Libraries/commons/process.d \
./Libraries/commons/string.d \
./Libraries/commons/temporal.d \
./Libraries/commons/txt.d 


# Each subdirectory must supply rules for building sources it contributes
Libraries/commons/%.o: ../Libraries/commons/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


