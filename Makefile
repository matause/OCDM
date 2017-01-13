#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITARM)/3ds_rules

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# DATA is a list of directories containing data files
# INCLUDES is a list of directories containing header files
#
# NO_SMDH: if set to anything, no SMDH file is generated.
# APP_TITLE is the name of the app stored in the SMDH file (Optional)
# APP_DESCRIPTION is the description of the app stored in the SMDH file (Optional)
# APP_AUTHOR is the author of the app stored in the SMDH file (Optional)
# ICON is the filename of the icon (.png), relative to the project folder.
#   If not set, it attempts to use one of the following (in this order):
#     - <Project name>.png
#     - icon.png
#     - <libctru folder>/default_icon.png
#---------------------------------------------------------------------------------
TARGET		    :=	$(notdir $(CURDIR))
BUILD		    :=	build
SOURCES		    :=	soos
DATA		    :=	data
INCLUDES	    :=	inc
ROMFS   	    :=	$(TOPDIR)/assets/ROMFS
APP_TITLE       :=  OCDM
APP_DESCRIPTION :=  Online Course Database Manager
APP_AUTHOR      :=  MarcusD
APP_PRODUCT_CODE:=  CTR-P-AOCA
APP_UNIQUE_ID   :=  0xDED666
ICON            :=  assets/logo.png

APP_TITLE       :=  $(shell echo "$(APP_TITLE)" | cut -c1-128)
APP_DESCRIPTION :=  $(shell echo "$(APP_DESCRIPTION)" | cut -c1-256)
APP_AUTHOR      :=  $(shell echo "$(APP_AUTHOR)" | cut -c1-128)
APP_PRODUCT_CODE:=  $(shell echo $(APP_PRODUCT_CODE) | cut -c1-16)
APP_UNIQUE_ID   :=  $(shell echo $(APP_UNIQUE_ID) | cut -c1-7)

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH	:=	-march=armv6k -mtune=mpcore -mfloat-abi=hard

CFLAGS	:=	-g -Wall -O2 -mword-relocations \
			-fomit-frame-pointer -ffast-math \
			$(ARCH) \
		-Wno-format -Wno-write-strings -Wno-unused-variable -Wno-unused-value

CFLAGS	+=	$(INCLUDE) -DARM11 -D_3DS

CXXFLAGS	:= $(CFLAGS) -Wno-reorder -fno-rtti -std=gnu++11

ASFLAGS	:=	-g $(ARCH)
LDFLAGS	=	-specs=3dsx.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

LIBS	:= -l:libsf2d.a -lctru -lm

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:= $(CTRULIB)


#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)
export TOPDIR	:=	$(CURDIR)

export VPATH	:=	$(foreach dir,$(SOURCES) _lzz_temp,$(CURDIR)/$(dir)) \
			$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=  $(shell find $(SOURCES) -name '*.c' -printf "%P\n")
CPPFILES	:=  $(shell find $(SOURCES) -name '*.cpp' -printf "%P\n")
LPPFILES	:=  $(shell find $(SOURCES) -name '*.lpp' -printf "%P\n")
SFILES		:=  $(shell find $(SOURCES) -name '*.s' -printf "%P\n")
BINFILES	:=  $(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

LPPSOOS     :=   $(foreach fil,$(LPPFILES),$(patsubst %.lpp,%.cpp,$(fil)))
CPPFILES    +=   $(foreach fil,$(LPPSOOS),$(fil:.lpp=.cpp))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#---------------------------------------------------------------------------------
	export LD	:=	$(CC)
#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
	export LD	:=	$(CXX)
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

export OFILES	:=	$(addsuffix .o,$(BINFILES)) \
			$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)/include) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			$(foreach dir,$(SOURCES) _lzz_temp,-I$(CURDIR)/$(dir)) \
			-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib) \
			$(foreach dir,$(INCLUDES),-L$(CURDIR)/$(dir)/lib)

ifeq ($(strip $(ICON)),)
	icons := $(wildcard *.png)
	ifneq (,$(findstring $(TARGET).png,$(icons)))
		export APP_ICON := $(TOPDIR)/$(TARGET).png
	else
		ifneq (,$(findstring icon.png,$(icons)))
			export APP_ICON := $(TOPDIR)/icon.png
		endif
	endif
else
	export APP_ICON := $(TOPDIR)/$(ICON)
endif

export _3DSXFLAGS += --smdh=$(OUTPUT).smdh
export _3DSXFLAGS += --romfs=$(ROMFS)

.PHONY: $(BUILD) clean all

#---------------------------------------------------------------------------------
all: $(BUILD)

$(LPPSOOS): $(LPPFILES)
	@mkdir -p _lzz_temp/$(shell dirname $@)
	lzz -hx hpp -hd -sd -c -o _lzz_temp/$(shell dirname $@) $(CURDIR)/soos/$(patsubst %.cpp,%.lpp,$@)

$(BUILD): $(LPPSOOS)
	@[ -d $@ ] || mkdir -p $@
	@find $(SOURCES) -type d -printf "%P\0" | xargs -0 -I {} mkdir -p $(BUILD)/{}
	@[ ! -d _lzz_temp ] || find _lzz_temp -type d -printf "%P\0" | xargs -0 -I {} mkdir -p $(BUILD)/{}
	@make --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -rf $(BUILD) _lzz_temp $(OUTPUT).3dsx $(OUTPUT).smdh $(OUTPUT).elf $(OUTPUT)-stripped.elf $(OUTPUT).cia


#---------------------------------------------------------------------------------
else

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
.PHONY: all
all	:	$(OUTPUT).3dsx $(OUTPUT).cia

$(OUTPUT).3dsx	:	$(OUTPUT).elf $(OUTPUT).smdh

$(OUTPUT).elf	:	$(OFILES)

$(TOPDIR)/assets/banner.bin: $(TOPDIR)/assets/banner.png $(TOPDIR)/assets/banner.wav
	@bannertool makebanner -i $(TOPDIR)/assets/banner.png -a $(TOPDIR)/assets/banner.wav -o $(TOPDIR)/assets/banner.bin

$(TOPDIR)/assets/image.bin: $(TOPDIR)/$(ICON)
	@bannertool makesmdh -s "$(APP_TITLE)" -l "$(APP_DESCRIPTION)" -p "$(APP_AUTHOR)" -i $(TOPDIR)/$(ICON) -o $(TOPDIR)/assets/image.bin


$(OUTPUT)-stripped.elf: $(OUTPUT).elf
	@cp $(OUTPUT).elf $(OUTPUT)-stripped.elf
	@$(PREFIX)strip $(OUTPUT)-stripped.elf

$(OUTPUT).cia: $(OUTPUT)-stripped.elf $(TOPDIR)/assets/banner.bin $(TOPDIR)/assets/image.bin
	@makerom -f cia -o $(OUTPUT).cia -rsf $(TOPDIR)/assets/cia.rsf -target t -exefslogo -elf $(OUTPUT)-stripped.elf -icon $(TOPDIR)/assets/image.bin -banner $(TOPDIR)/assets/banner.bin -DAPP_TITLE="$(APP_TITLE)" -DAPP_PRODUCT_CODE="$(APP_PRODUCT_CODE)" -DAPP_UNIQUE_ID="$(APP_UNIQUE_ID)" -DAPP_ROMFS="$(ROMFS)"
	@echo "built ... $(notdir $@)"

#---------------------------------------------------------------------------------
# you need a rule like this for each extension you use as binary data
#---------------------------------------------------------------------------------
%.png.o	:	%.png
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)
	

# WARNING: This is not the right way to do this! TODO: Do it right!
#---------------------------------------------------------------------------------
%.vsh.o	:	%.vsh
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@python $(AEMSTRO)/aemstro_as.py $< ../$(notdir $<).shbin
	@bin2s ../$(notdir $<).shbin | $(PREFIX)as -o $@
	@echo "extern const u8" `(echo $(notdir $<).shbin | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`"_end[];" > `(echo $(notdir $<).shbin | tr . _)`.h
	@echo "extern const u8" `(echo $(notdir $<).shbin | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`"[];" >> `(echo $(notdir $<).shbin | tr . _)`.h
	@echo "extern const u32" `(echo $(notdir $<).shbin | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`_size";" >> `(echo $(notdir $<).shbin | tr . _)`.h
	@rm ../$(notdir $<).shbin

-include $(DEPENDS)

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
