# make VERSION=Release
# make -j8 
# ./build/cache.x

# ?= -> can redefine variable
SRC_DIR     = src
INCLUDE_DIR = include
EXECUTABLE  = cache.x

CXXFLAGS_RELEASE  = -std=c++20 -DNDEBUG -O3 -march=native -pie -fPIE -flto

CXXFLAGS_SANITIZE = -fstack-protector -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr 

CXXFLAGS_DEBUG	  = -std=c++20 -O0 -D _DEBUG -ggdb3 -fstrict-overflow -flto-odr-type-merging \
					-fno-omit-frame-pointer -pie -fPIE -fcheck-new -fsized-deallocation 

CXXFLAGS_WARNINGS = -Wall -Wextra -Weffc++ -Wsign-conversion -Waggressive-loop-optimizations \
					-Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual           \
					-Wchar-subscripts -Wconditionally-supported -Wctor-dtor-privacy          \
					-Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security         \
					-Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor  \
					-Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self  \
					-Wredundant-decls -Wshadow -Wsign-promo -Wstrict-null-sentinel           \
					-Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods \
					-Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum  \
					-Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast           \
					-Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers    \
					-Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector        \
					-Wlarger-than=8192 -Wstack-usage=8192 -Werror=vla -Wconversion

ifeq "$(VERSION)" "Release" 
	override CXXFLAGS += -I $(INCLUDE_DIR) $(CXXFLAGS_RELEASE)
	BUILD_DIR ?= build
else
	override CXXFLAGS += -I $(INCLUDE_DIR) $(CXXFLAGS_DEBUG) $(CXXFLAGS_SANITIZE)
	BUILD_DIR ?= debug_build
endif

ifeq ($(origin CXX),default)
	CXX = g++
endif

# := -> set variable value right now
CXXSRC := $(wildcard $(SRC_DIR)/*.cc)

CXXOBJ := $(addprefix $(BUILD_DIR)/,$(CXXSRC:.cc=.o))

DEPS = $(CXXOBJ:.o=.d)

.PHONY: all
all: $(BUILD_DIR)/$(EXECUTABLE)

$(BUILD_DIR)/$(EXECUTABLE): $(CXXOBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(CXXOBJ) : $(BUILD_DIR)/%.o : %.cc
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(DEPS) : $(BUILD_DIR)/%.d : %.cc
	@mkdir -p $(@D)
	$(CXX) -E $(CXXFLAGS) $< -MM -MT $(@:.d=.o) > $@

IWYU = iwyu
IWYU_OUTPUTS = $(CXXOBJ:.o=.iwyu)

.PHONY: diagnose
diagnose: $(IWYU_OUTPUTS)

$(IWYU_OUTPUTS) : $(BUILD_DIR)/%.iwyu : %.cc
	@mkdir -p $(@D)
	$(IWYU) $(CXXFLAGS) $<
	@echo "========================================================================="

.PHONY: clean
clean:
	rm -rf $(CXXOBJ) $(DEPS) $(BUILD_DIR)/$(EXECUTABLE) $(BUILD_DIR)/*.log

.PHONY: run
run: $(BUILD_DIR)/$(EXECUTABLE)
	@echo "Running executable\n----------------------------------------------------"
	@./$<

NODEPS = clean run diagnose

ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
include $(DEPS)
endif
