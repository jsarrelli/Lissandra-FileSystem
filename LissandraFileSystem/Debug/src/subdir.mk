################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/AdministradorConsultasLFS.c \
../src/ApiLFS.c \
../src/Compactador.c \
../src/FileSystem.c \
../src/LissandraFileSystem.c \
../src/SocketServidor.c \
../src/funcionesLFS.c 

OBJS += \
./src/AdministradorConsultasLFS.o \
./src/ApiLFS.o \
./src/Compactador.o \
./src/FileSystem.o \
./src/LissandraFileSystem.o \
./src/SocketServidor.o \
./src/funcionesLFS.o 

C_DEPS += \
./src/AdministradorConsultasLFS.d \
./src/ApiLFS.d \
./src/Compactador.d \
./src/FileSystem.d \
./src/LissandraFileSystem.d \
./src/SocketServidor.d \
./src/funcionesLFS.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2019-1c-Los-Sisoperadores/Libraries" -I/home/utnso/tp-2019-1c-Los-Sisoperadores/Lib -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


