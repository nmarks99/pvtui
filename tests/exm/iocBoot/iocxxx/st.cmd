# ../../bin/${EPICS_HOST_ARCH}/exm st.cmd
< envPaths

dbLoadDatabase("../../dbd/iocexmLinux.dbd")
iocexmLinux_registerRecordDeviceDriver(pdbbase)

epicsEnvSet("PREFIX", "exm:")


# Load simulated motors
epicsEnvSet("INSTANCE", "motorSim")
epicsEnvSet("NUM_AXES", "16")
epicsEnvSet("DHLM", "100000000")
epicsEnvSet("DLLM", "-100000000")
epicsEnvSet("HIGH_LIM", "50000")
epicsEnvSet("LOW_LIM", "-50000")

motorSimCreate($(CONTROLLER=0), 0, $(LOW_LIM=-32000), $(HIGH_LIM=32000), $(HOME_POS=0), 1, $(NUM_AXES=1))
drvAsynMotorConfigure("$(INSTANCE)$(CONTROLLER=0)", "$(INSTANCE)", $(CONTROLLER=0), $(NUM_AXES=1))
dbLoadTemplate("motorSim.substitutions", "P=$(PREFIX), DTYP='asynMotor', PORT=$(INSTANCE)$(CONTROLLER=0),DHLM=$(DHLM=320.0),DLLM=$(DLLM=-320.0)")

dbLoadRecords("example.db", "")

###############################################################################
iocInit
###############################################################################

# print the time our boot was finished
date
