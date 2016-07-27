SRC	+= ../Obake/ASystem.cpp \
	  ../Obake/EventsManager.cpp \
	  ../Obake/Event.cpp \
	  ../Obake/SharedLibrary.cpp \
	  ../Obake/PluginsManager.cpp \
	  ../Obake/Core.cpp

INCLUDE+=-I./../Obake/external -I./../Obake/external/EASTL -I./../Obake

LIBS+= -ldl -L./../Obake -L../Obake/external/EASTL -lEASTL -pthread 
