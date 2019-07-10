################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/AdministradorDeConsultas.c \
../src/FuncionesGossiping.c \
../src/Memoria.c \
../src/MemoriaPrincipal.c \
../src/Parser.c \
../src/SocketClienteMemoria.c \
../src/SocketServidorMemoria.c 

OBJS += \
./src/AdministradorDeConsultas.o \
./src/FuncionesGossiping.o \
./src/Memoria.o \
./src/MemoriaPrincipal.o \
./src/Parser.o \
./src/SocketClienteMemoria.o \
./src/SocketServidorMemoria.o 

C_DEPS += \
./src/AdministradorDeConsultas.d \
./src/FuncionesGossiping.d \
./src/Memoria.d \
./src/MemoriaPrincipal.d \
./src/Parser.d \
./src/SocketClienteMemoria.d \
./src/SocketServidorMemoria.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2019-1c-Los-Sisoperadores/Libraries" -I/home/utnso/tp-2019-1c-Los-Sisoperadores/Lib -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


