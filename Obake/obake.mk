SRC	= ../Obake/ASystem.cpp \
	  ../Obake/EventsManager.cpp \
	  ../Obake/Event.cpp \
	  ../Obake/SharedLibrary.cpp \
	  ../Obake/PluginsManager.cpp \
	  ../Obake/Core.cpp

INCLUDE=-I./ -I./EASTL

LIBS= -ldl -L./ -lEASTL
