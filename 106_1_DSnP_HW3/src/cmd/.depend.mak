cmdCharDef.o: cmdCharDef.cpp cmdParser.h cmdCharDef.h
cmdCommon.o: cmdCommon.cpp ../util/util.h cmdCommon.h cmdParser.h \
 cmdCharDef.h
cmdParser.o: cmdParser.cpp ../util/util.h cmdParser.h cmdCharDef.h
