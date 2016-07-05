SRC	= ../Obake/ASystem.cpp \
	  ../Obake/EventsManager.cpp \
	  ../Obake/SharedLibrary.cpp \
	  ../Obake/Core.cpp

INCLUDE=-I./ -I./EASTL

LIBS= -ldl #-L./ -lEASTL
