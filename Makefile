# Alternative GNU Make workspace makefile autogenerated by Premake

ifndef config
  config=debug
endif

ifndef verbose
  SILENT = @
endif

ifeq ($(config),debug)
  raytracer_cpp_config = debug

else ifeq ($(config),release)
  raytracer_cpp_config = release

else
  $(error "invalid configuration $(config)")
endif

PROJECTS := raytracer-cpp

.PHONY: all clean help $(PROJECTS) 

all: $(PROJECTS)

raytracer-cpp:
ifneq (,$(raytracer_cpp_config))
	@echo "==== Building raytracer-cpp ($(raytracer_cpp_config)) ===="
	@${MAKE} --no-print-directory -C . -f raytracer-cpp.make config=$(raytracer_cpp_config)
endif

clean:
	@${MAKE} --no-print-directory -C . -f raytracer-cpp.make clean

help:
	@echo "Usage: make [config=name] [target]"
	@echo ""
	@echo "CONFIGURATIONS:"
	@echo "  debug"
	@echo "  release"
	@echo ""
	@echo "TARGETS:"
	@echo "   all (default)"
	@echo "   clean"
	@echo "   raytracer-cpp"
	@echo ""
	@echo "For more information, see https://github.com/premake/premake-core/wiki"