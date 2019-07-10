################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sockets/Conexiones.c \
../Sockets/Serializacion.c 

OBJS += \
./Sockets/Conexiones.o \
./Sockets/Serializacion.o 

C_DEPS += \
./Sockets/Conexiones.d \
./Sockets/Serializacion.d 


# Each subdirectory must supply rules for building sources it contributes
Sockets/%.o: ../Sockets/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


