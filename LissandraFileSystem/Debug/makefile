################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include Libraries2/commons/collections/subdir.mk
-include Libraries2/commons/subdir.mk
-include Libraries2/Debug/commons/collections/subdir.mk
-include Libraries2/Debug/commons/subdir.mk
-include Libraries2/Debug/subdir.mk
-include Libraries2/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: LissandraFileSystem

dependents:
	-cd /home/utnso/tp-2019-1c-Los-Sisoperadores/Libraries/Debug && $(MAKE) all
	-cd /home/utnso/tp-2019-1c-Los-Sisoperadores/Lib/Debug && $(MAKE) all

# Tool invocations
LissandraFileSystem: $(OBJS) $(USER_OBJS) /home/utnso/tp-2019-1c-Los-Sisoperadores/Libraries/Debug/libLibraries.so /home/utnso/tp-2019-1c-Los-Sisoperadores/Lib/Debug/libLib.so
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/home/utnso/tp-2019-1c-Los-Sisoperadores/Lib/Debug" -L/home/utnso/tp-2019-1c-Los-Sisoperadores/Lib/Funciones -o "LissandraFileSystem" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) LissandraFileSystem
	-@echo ' '

.PHONY: all clean dependents
/home/utnso/tp-2019-1c-Los-Sisoperadores/Libraries/Debug/libLibraries.so:
/home/utnso/tp-2019-1c-Los-Sisoperadores/Lib/Debug/libLib.so:

-include ../makefile.targets
