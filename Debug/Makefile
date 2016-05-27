################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL := cmd.exe
RM := rm -rf

USER_OBJS :=

LIBS := 
PROJ := 

O_SRCS := 
C_SRCS := 
S_SRCS := 
S_UPPER_SRCS := 
OBJ_SRCS := 
ASM_SRCS := 
PREPROCESSING_SRCS := 
OBJS := 
OBJS_AS_ARGS := 
C_DEPS := 
C_DEPS_AS_ARGS := 
EXECUTABLES := 
OUTPUT_FILE_PATH :=
OUTPUT_FILE_PATH_AS_ARGS :=
AVR_APP_PATH :=$$$AVR_APP_PATH$$$
QUOTE := "
ADDITIONAL_DEPENDENCIES:=
OUTPUT_FILE_DEP:=
LIB_DEP:=
LINKER_SCRIPT_DEP:=

# Every subdirectory with source files must be described here
SUBDIRS := 


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../blocks.c \
../buttons.c \
../game.c \
../ledmatrix.c \
../project.c \
../score.c \
../scrolling_char_display.c \
../serialio.c \
../spi.c \
../terminalio.c \
../timer0.c


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
blocks.o \
buttons.o \
game.o \
ledmatrix.o \
project.o \
score.o \
scrolling_char_display.o \
serialio.o \
spi.o \
terminalio.o \
timer0.o

OBJS_AS_ARGS +=  \
blocks.o \
buttons.o \
game.o \
ledmatrix.o \
project.o \
score.o \
scrolling_char_display.o \
serialio.o \
spi.o \
terminalio.o \
timer0.o

C_DEPS +=  \
blocks.d \
buttons.d \
game.d \
ledmatrix.d \
project.d \
score.d \
scrolling_char_display.d \
serialio.d \
spi.d \
terminalio.d \
timer0.d

C_DEPS_AS_ARGS +=  \
blocks.d \
buttons.d \
game.d \
ledmatrix.d \
project.d \
score.d \
scrolling_char_display.d \
serialio.d \
spi.d \
terminalio.d \
timer0.d

OUTPUT_FILE_PATH +=assignment.elf

OUTPUT_FILE_PATH_AS_ARGS +=assignment.elf

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

LIB_DEP+= 

LINKER_SCRIPT_DEP+= 


# AVR32/GNU C Compiler























./%.o: .././%.c
	@echo Building file: $<
	@echo Invoking: AVR8/GNU C Compiler : 4.9.2
	$(QUOTE)C:\Users\Benedict\Box Sync\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -x c -funsigned-char -funsigned-bitfields -DDEBUG  -I"C:\Users\Benedict\Box Sync\7.0\Packs\atmel\ATmega_DFP\1.0.106\include"  -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -g2 -Wall -mmcu=atmega324a -B "C:\Users\Benedict\Box Sync\7.0\Packs\atmel\ATmega_DFP\1.0.106\gcc\dev\atmega324a" -c -std=gnu99 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	



# AVR32/GNU Preprocessing Assembler



# AVR32/GNU Assembler




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)

$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP) $(LIB_DEP) $(LINKER_SCRIPT_DEP)
	@echo Building target: $@
	@echo Invoking: AVR8/GNU Linker : 4.9.2
	$(QUOTE)C:\Users\Benedict\Box Sync\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-g++.exe$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -Wl,-Map="assignment.map" -Wl,--start-group -Wl,-lm  -Wl,--end-group -Wl,--gc-sections -mmcu=atmega324a -B "C:\Users\Benedict\Box Sync\7.0\Packs\atmel\ATmega_DFP\1.0.106\gcc\dev\atmega324a"  
	@echo Finished building target: $@
	"C:\Users\Benedict\Box Sync\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures  "assignment.elf" "assignment.hex"
	"C:\Users\Benedict\Box Sync\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex "assignment.elf" "assignment.eep" || exit 0
	"C:\Users\Benedict\Box Sync\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objdump.exe" -h -S "assignment.elf" > "assignment.lss"
	"C:\Users\Benedict\Box Sync\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O srec -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures "assignment.elf" "assignment.srec"
	"C:\Users\Benedict\Box Sync\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-size.exe" "assignment.elf"
	
	





# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS) $(EXECUTABLES)  
	-$(RM) $(C_DEPS_AS_ARGS)   
	rm -rf "assignment.elf" "assignment.a" "assignment.hex" "assignment.lss" "assignment.eep" "assignment.map" "assignment.srec" "assignment.usersignatures"
	