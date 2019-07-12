################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/API_kernel.c \
../src/ConfigKernel.c \
../src/Kernel.c \
../src/ProcesoExec.c \
../src/SocketClienteMemoria.c 

OBJS += \
./src/API_kernel.o \
./src/ConfigKernel.o \
./src/Kernel.o \
./src/ProcesoExec.o \
./src/SocketClienteMemoria.o 

C_DEPS += \
./src/API_kernel.d \
./src/ConfigKernel.d \
./src/Kernel.d \
./src/ProcesoExec.d \
./src/SocketClienteMemoria.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2019-1c-Los-Sisoperadores/Libraries" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


