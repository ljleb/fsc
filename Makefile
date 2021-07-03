NAME := fsc
SRC := src
OUT := out

CXX_WINDOWS := x86_64-w64-mingw32-g++
CXX_LINUX := g++
CXX_release_OPTIONS := -static -O3 --std=c++2a
CXX_debug_OPTIONS := -static -g -O0 --std=c++2a
CXX_LIBS := -I./$(SRC)

.PHONY:
all: debug release

.PHONY:
release: $(OUT)/windows/release/$(NAME) $(OUT)/linux/release/$(NAME)

.PHONY:
debug: $(OUT)/windows/debug/$(NAME) $(OUT)/linux/debug/$(NAME)

$(OUT)/windows/%/$(NAME): $(SRC)/main.cpp $(wildcard ./$(SRC)/*.hpp) | $(OUT)/windows/%/
	$(CXX_WINDOWS) $(CXX_$*_OPTIONS) $(CXX_LIBS) -o $@ $<

$(OUT)/linux/%/$(NAME): $(SRC)/main.cpp $(wildcard ./$(SRC)/*.hpp) | $(OUT)/linux/%/
	$(CXX_LINUX) $(CXX_$*_OPTIONS) $(CXX_LIBS) -o $@ $<

.PHONY:
clean:
	rm -rf $(OUT)

.PHONY:
%/:
	mkdir -p $@
