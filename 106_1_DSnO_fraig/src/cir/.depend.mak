cirGate.o: cirGate.cpp cirGate.h cirDef.h ../util/myHashMap.h \
 ../sat/sat.h ../sat/Solver.h ../sat/SolverTypes.h ../sat/Global.h \
 ../sat/VarOrder.h ../sat/Heap.h ../sat/Proof.h ../sat/File.h cirMgr.h \
 ../util/util.h ../util/rnGen.h ../util/myUsage.h
cirMgr.o: cirMgr.cpp cirMgr.h cirDef.h ../util/myHashMap.h cirGate.h \
 ../sat/sat.h ../sat/Solver.h ../sat/SolverTypes.h ../sat/Global.h \
 ../sat/VarOrder.h ../sat/Heap.h ../sat/Proof.h ../sat/File.h \
 ../util/util.h ../util/rnGen.h ../util/myUsage.h
cirSim.o: cirSim.cpp cirMgr.h cirDef.h ../util/myHashMap.h cirGate.h \
 ../sat/sat.h ../sat/Solver.h ../sat/SolverTypes.h ../sat/Global.h \
 ../sat/VarOrder.h ../sat/Heap.h ../sat/Proof.h ../sat/File.h \
 ../util/util.h ../util/rnGen.h ../util/myUsage.h
cirFraig.o: cirFraig.cpp cirMgr.h cirDef.h ../util/myHashMap.h cirGate.h \
 ../sat/sat.h ../sat/Solver.h ../sat/SolverTypes.h ../sat/Global.h \
 ../sat/VarOrder.h ../sat/Heap.h ../sat/Proof.h ../sat/File.h \
 ../util/util.h ../util/rnGen.h ../util/myUsage.h
cirCmd.o: cirCmd.cpp cirMgr.h cirDef.h ../util/myHashMap.h cirGate.h \
 ../sat/sat.h ../sat/Solver.h ../sat/SolverTypes.h ../sat/Global.h \
 ../sat/VarOrder.h ../sat/Heap.h ../sat/Proof.h ../sat/File.h cirCmd.h \
 ../../include/cmdParser.h ../../include/cmdCharDef.h \
 ../../include/util.h ../../include/rnGen.h ../../include/myUsage.h
cirOpt.o: cirOpt.cpp cirMgr.h cirDef.h ../util/myHashMap.h cirGate.h \
 ../sat/sat.h ../sat/Solver.h ../sat/SolverTypes.h ../sat/Global.h \
 ../sat/VarOrder.h ../sat/Heap.h ../sat/Proof.h ../sat/File.h \
 ../util/util.h ../util/rnGen.h ../util/myUsage.h
