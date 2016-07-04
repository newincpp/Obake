include ./obake.mk

SRC	+= ./main.cpp
EXT     =cpp
NAME	=demo
DEFINE 	=-D THREAD_SHOW #-D FPS_COUNTER
DEBUG 	= -g 
GLM_DEFINE=-DGLM_SWIZZLE -DGLM_FORCE_AVX
OPTIM= $(GLM_DEFINE) -march=native -O2 -fno-rtti -fno-exceptions -Wconversion #-Weffc++ 
CXXFLAGS= -Wall -Wextra -W -std=c++0x $(OPTIM) $(DEFINE) $(DEBUG) $(INCLUDE)
LIBS+= -pthread
LDFLAGS	= $(LIBS) $(OPTIM) $(DEBUG)
OBJS	= $(SRC:.$(EXT)=.o)
RM	= rm -f
CXX	= clang++
LINKER	= $(CXX)

all: $(NAME)

$(NAME): $(OBJS)
	$(LINKER) $(OBJS) $(LDFLAGS) -o $(NAME)

clean:
	$(RM) $(OBJS) *.swp *~ *#

fclean: clean
	$(RM) $(NAME)

re: make -B -j4

.PHONY: all clean fclean re
