cirGate.o: cirGate.cpp cirGate.h cirDef.h cirMgr.h ../util/util.h \
 ../util/rnGen.h ../util/myUsage.h
cirMgr.o: cirMgr.cpp cirMgr.h cirDef.h cirGate.h ../util/util.h \
 ../util/rnGen.h ../util/myUsage.h
cirCmd.o: cirCmd.cpp cirMgr.h cirDef.h cirGate.h cirCmd.h \
 ../cmd/cmdParser.h ../cmd/cmdCharDef.h ../util/util.h ../util/rnGen.h \
 ../util/myUsage.h
