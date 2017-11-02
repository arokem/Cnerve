################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Ks_Gillespie.c \
../main_ChanRunner.c \
../mt19937ar.c \
../rateChannels.c 

OBJS += \
./Ks_Gillespie.o \
./main_ChanRunner.o \
./mt19937ar.o \
./rateChannels.o 

C_DEPS += \
./Ks_Gillespie.d \
./main_ChanRunner.d \
./mt19937ar.d \
./rateChannels.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


