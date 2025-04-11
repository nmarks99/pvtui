TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

# PROD_NAME is used by ADCORE to add libraries
# DBD_PREFIX is used by ADCORE to add dbd files
PROD_NAME = xxx
DBD_PREFIX = iocxxx
PROD_IOC = $(PROD_NAME)

# Note that the order in which .dbd files and libraries are added to a list
# variable is important.  For .dbd files, things must be defined before they
# are referenced by a later .dbd file.  For libraries, things must be referenced
# before they are searched for; otherwise they won't be included.

##################################################################
# First, define $(DBD_NAME)_DBD and $(PROD_NAME)_LIBS from the modules
# defined in RELEASE
##################################################################
DBD_SUFFIX_Linux = Linux
DBD_SUFFIX_WIN32 = Win$(if $(findstring 32, $(T_A)),32,64)
DBD_NAME = $(DBD_PREFIX)$(DBD_SUFFIX_$(OS_CLASS))

DBD  += $(DBD_NAME).dbd

$(DBD_NAME)_DBD += base.dbd

ifdef ASYN
  $(DBD_NAME)_DBD += asyn.dbd
  $(DBD_NAME)_DBD += drvAsynSerialPort.dbd drvAsynIPPort.dbd drvVxi11.dbd devGpib.dbd
  $(PROD_NAME)_LIBS := asyn $($(PROD_NAME)_LIBS)
endif

ifdef LUA
  $(DBD_NAME)_DBD += luaSupport.dbd
  $(PROD_NAME)_LIBS := lua $($(PROD_NAME)_LIBS)
endif

ifdef BUSY
  $(DBD_NAME)_DBD += busySupport.dbd
  $(PROD_NAME)_LIBS := busy $($(PROD_NAME)_LIBS)
endif

ifdef SNCSEQ
  ifeq (seq-2-1, $(findstring seq-2-1, $(SNCSEQ)))
    $(DBD_NAME)_DBD += devSequencer.dbd
    $(PROD_NAME)_LIBS := seqDev seq pv $($(PROD_NAME)_LIBS)
  else ifeq (seq-2.1, $(findstring seq-2.1, $(SNCSEQ)))
    $(DBD_NAME)_DBD += devSequencer.dbd
    $(PROD_NAME)_LIBS := seqDev seq pv $($(PROD_NAME)_LIBS)
  else
    # seq 2.2.
    $(PROD_NAME)_LIBS := seq pv $($(PROD_NAME)_LIBS)
  endif
endif

# ifdef IPAC
  # $(DBD_NAME)_DBD += tyGSOctal.dbd
  # $(DBD_NAME)_DBD += drvIpac.dbd
  # $(PROD_NAME)_LIBS := TyGSOctal Ipac $($(PROD_NAME)_LIBS)
  # $(DBD_NAME)_DBD += IP520.dbd
  # $(PROD_NAME)_LIBS := IP520 $($(PROD_NAME)_LIBS)
# endif

ifdef MOTOR
  $(DBD_NAME)_DBD += motorSupport.dbd
  $(DBD_NAME)_DBD += motorSimSupport.dbd
  $(PROD_NAME)_LIBS := motor $($(PROD_NAME)_LIBS)
  $(PROD_NAME)_LIBS := motorSimSupport $($(PROD_NAME)_LIBS)
endif

$(PROD_NAME)_LIBS += $(EPICS_BASE_IOC_LIBS)

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
$(PROD_NAME)_SRCS  += $(DBD_NAME)_registerRecordDeviceDriver.cpp $(PROD_NAME)Main.cpp

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
