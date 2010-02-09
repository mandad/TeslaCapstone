################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../msp430x2xx_fet_1.c 

CMD_SRCS += \
../lnk_msp430f2013.cmd 

OBJS += \
./msp430x2xx_fet_1.obj 

C_DEPS += \
./msp430x2xx_fet_1.pp 

OBJS__QTD += \
".\msp430x2xx_fet_1.obj" 

C_DEPS__QTD += \
".\msp430x2xx_fet_1.pp" 

C_SRCS_QUOTED += \
"../msp430x2xx_fet_1.c" 


# Each subdirectory must supply rules for building sources it contributes
msp430x2xx_fet_1.obj: ../msp430x2xx_fet_1.c $(GEN_SRCS) $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/msp430/bin/cl430" --silicon_version=msp -g --include_path="C:/Program Files/Texas Instruments/ccsv4/msp430/include" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/msp430/include" --diag_warning=225 --printf_support=minimal --preproc_with_compile --preproc_dependency="msp430x2xx_fet_1.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '


