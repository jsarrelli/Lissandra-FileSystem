################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Compartidas/src/Compartidas.c 

OBJS += \
./Compartidas/src/Compartidas.o 

C_DEPS += \
./Compartidas/src/Compartidas.d 


# Each subdirectory must supply rules for building sources it contributes
Compartidas/src/%.o: ../Compartidas/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


