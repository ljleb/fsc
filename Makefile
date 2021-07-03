NAME := fsc
SRC := src
OUT := out

CXX := g++
CXX_release_OPTIONS := -O3 --std=c++2a
CXX_debug_OPTIONS := -g -O0 --std=c++2a
CXX_LIBS := -I./$(SRC)

.PHONY:
all: debug release

.PHONY:
release: $(OUT)/release/$(NAME)

.PHONY:
debug: $(OUT)/debug/$(NAME)

$(OUT)/%/$(NAME): $(wildcard $(SRC)/*.hpp) $(SRC)/main.cpp | $(OUT)/%/
	$(CXX) $(CXX_$*_OPTIONS) $(CXX_LIBS) -o $@ $^

.PHONY:
clean:
	rm -rf $(OUT)

.PHONY:
%/:
	mkdir -p $@
