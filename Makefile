FLAGS = -O3
SOURCES = Core.c Memory.c posix_main.c
CPP_SOURCES = 
MAC_LIB_SOURCES =
OBJECTS = $(addprefix Build/, $(patsubst %.c,%.o,$(SOURCES)))
#MAC_LIB_OBJECTS = $(addprefix Build/, $(patsubst %.m,%.o, $(MAC_LIB_SOURCES)))
#CPP_OBJECTS = $(addprefix Build/, $(patsubst %.cpp,%.o,$(CPP_SOURCES)))
HEADERS = Core.h Memory.h Common.h

all: debug

debug: picboy
debug: FLAGS = -g3 -D _DEBUG=1

$(OBJECTS): Build/%.o : %.c $(HEADERS)
	@mkdir -p $(@D)
	@clang -pedantic -std=gnu11 $(FLAGS) $< -c -o $@

# $(MAC_LIB_OBJECTS): Build/%.o : %.m
# 	@mkdir -p $(@D)
# 	@clang -Wno-everything-fmodules $(FLAGS) $< -c -o $@

# $(CPP_OBJECTS): Build/%.o : %.cpp $(HEADERS)
# 	@mkdir -p $(@D)
# 	@clang++ -pedantic -std=c++14 $(FLAGS) $< -c -o $@

picboy: $(OBJECTS) $(CPP_OBJECTS) $(MAC_LIB_OBJECTS)
	@clang -pedantic $^ -o $@

.PHONY: clean
clean:
	@rm -rf Build/ picboy Core.ll Core.s picboy.dSYM/*
