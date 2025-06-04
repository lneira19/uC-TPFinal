################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccs2001/ccs/tools/compiler/ti-cgt-msp430_21.6.1.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./drv_adc10.obj" \
"./drv_ds18b20.obj" \
"./drv_eeprom.obj" \
"./drv_i2c.obj" \
"./drv_pwm.obj" \
"./drv_serial_comm.obj" \
"./drv_spi.obj" \
"./drv_sw.obj" \
"./drv_uart.obj" \
"./gpio.obj" \
"./main.obj" \
"./protocols.obj" \
"./system.obj" \
"./timer.obj" \
"./utils.obj" \
"../lnk_msp430g2553.cmd" \
$(GEN_CMDS__FLAG) \
-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
lukitesting_v16.out 

EXE_OUTPUTS__QUOTED += \
"lukitesting_v16.out" 

BIN_OUTPUTS += \
lukitesting_v16.txt 

BIN_OUTPUTS__QUOTED += \
"lukitesting_v16.txt" 


# All Target
all: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "lukitesting_v16.out" secondary-outputs

# Tool invocations
lukitesting_v16.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: MSP430 Linker'
	"C:/ti/ccs2001/ccs/tools/compiler/ti-cgt-msp430_21.6.1.LTS/bin/cl430" -vmsp --use_hw_mpy=none --advice:power=all --define=__MSP430G2553__ -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number -z -m"lukitesting_v16.map" --heap_size=80 --stack_size=80 -i"C:/ti/ccs2001/ccs/ccs_base/msp430/include" -i"C:/ti/ccs2001/ccs/tools/compiler/ti-cgt-msp430_21.6.1.LTS/lib" -i"C:/ti/ccs2001/ccs/tools/compiler/ti-cgt-msp430_21.6.1.LTS/include" --reread_libs --diag_wrap=off --display_error_number --warn_sections --xml_link_info="lukitesting_v16_linkInfo.xml" --use_hw_mpy=none --rom_model -o "lukitesting_v16.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

lukitesting_v16.txt: $(EXE_OUTPUTS)
	@echo 'Building secondary target: "$@"'
	@echo 'Invoking: MSP430 Hex Utility'
	"C:/ti/ccs2001/ccs/tools/compiler/ti-cgt-msp430_21.6.1.LTS/bin/hex430" --memwidth=8 --romwidth=8 --diag_wrap=off --ti_txt -o "lukitesting_v16.txt" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building secondary target: "$@"'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(BIN_OUTPUTS__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "drv_adc10.obj" "drv_ds18b20.obj" "drv_eeprom.obj" "drv_i2c.obj" "drv_pwm.obj" "drv_serial_comm.obj" "drv_spi.obj" "drv_sw.obj" "drv_uart.obj" "gpio.obj" "main.obj" "protocols.obj" "system.obj" "timer.obj" "utils.obj" 
	-$(RM) "drv_adc10.d" "drv_ds18b20.d" "drv_eeprom.d" "drv_i2c.d" "drv_pwm.d" "drv_serial_comm.d" "drv_spi.d" "drv_sw.d" "drv_uart.d" "gpio.d" "main.d" "protocols.d" "system.d" "timer.d" "utils.d" 
	-@echo 'Finished clean'
	-@echo ' '

secondary-outputs: $(BIN_OUTPUTS)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

