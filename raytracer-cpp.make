# Alternative GNU Make project makefile autogenerated by Premake

ifndef config
  config=debug
endif

ifndef verbose
  SILENT = @
endif

.PHONY: clean prebuild

SHELLTYPE := posix
ifeq (.exe,$(findstring .exe,$(ComSpec)))
	SHELLTYPE := msdos
endif

# Configurations
# #############################################

RESCOMP = windres
DEFINES +=
INCLUDES += -Ilib -Isrc
FORCE_INCLUDE +=
ALL_CPPFLAGS += $(CPPFLAGS) -MMD -MP $(DEFINES) $(INCLUDES)
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -O3 -g
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -O3 -g -std=c++17
ALL_RESFLAGS += $(RESFLAGS) $(DEFINES) $(INCLUDES)
LIBS +=
LDDEPS +=
ALL_LDFLAGS += $(LDFLAGS)
LINKCMD = $(CXX) -o "$@" $(OBJECTS) $(RESOURCES) $(ALL_LDFLAGS) $(LIBS)
define PREBUILDCMDS
endef
define PRELINKCMDS
endef
define POSTBUILDCMDS
endef

ifeq ($(config),debug)
TARGETDIR = bin/debug
TARGET = $(TARGETDIR)/raytracer-cpp.exe
OBJDIR = obj/debug

else ifeq ($(config),release)
TARGETDIR = bin/release
TARGET = $(TARGETDIR)/raytracer-cpp.exe
OBJDIR = obj/release

endif

# Per File Configurations
# #############################################


# File sets
# #############################################

GENERATED :=
OBJECTS :=

GENERATED += $(OBJDIR)/bbox.o
GENERATED += $(OBJDIR)/bvh.o
GENERATED += $(OBJDIR)/diffuse.o
GENERATED += $(OBJDIR)/emission.o
GENERATED += $(OBJDIR)/gltf_loader.o
GENERATED += $(OBJDIR)/lib.o
GENERATED += $(OBJDIR)/main.o
GENERATED += $(OBJDIR)/mesh.o
GENERATED += $(OBJDIR)/nee.o
GENERATED += $(OBJDIR)/reflection.o
GENERATED += $(OBJDIR)/scene.o
GENERATED += $(OBJDIR)/textures.o
GENERATED += $(OBJDIR)/triangle.o
OBJECTS += $(OBJDIR)/bbox.o
OBJECTS += $(OBJDIR)/bvh.o
OBJECTS += $(OBJDIR)/diffuse.o
OBJECTS += $(OBJDIR)/emission.o
OBJECTS += $(OBJDIR)/gltf_loader.o
OBJECTS += $(OBJDIR)/lib.o
OBJECTS += $(OBJDIR)/main.o
OBJECTS += $(OBJDIR)/mesh.o
OBJECTS += $(OBJDIR)/nee.o
OBJECTS += $(OBJDIR)/reflection.o
OBJECTS += $(OBJDIR)/scene.o
OBJECTS += $(OBJDIR)/textures.o
OBJECTS += $(OBJDIR)/triangle.o

# Rules
# #############################################

all: $(TARGET)
	@:

$(TARGET): $(GENERATED) $(OBJECTS) $(LDDEPS) | $(TARGETDIR)
	$(PRELINKCMDS)
	@echo Linking raytracer-cpp
	$(SILENT) $(LINKCMD)
	$(POSTBUILDCMDS)

$(TARGETDIR):
	@echo Creating $(TARGETDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(TARGETDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(TARGETDIR))
endif

$(OBJDIR):
	@echo Creating $(OBJDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(OBJDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(OBJDIR))
endif

clean:
	@echo Cleaning raytracer-cpp
ifeq (posix,$(SHELLTYPE))
	$(SILENT) rm -f  $(TARGET)
	$(SILENT) rm -rf $(GENERATED)
	$(SILENT) rm -rf $(OBJDIR)
else
	$(SILENT) if exist $(subst /,\\,$(TARGET)) del $(subst /,\\,$(TARGET))
	$(SILENT) if exist $(subst /,\\,$(GENERATED)) rmdir /s /q $(subst /,\\,$(GENERATED))
	$(SILENT) if exist $(subst /,\\,$(OBJDIR)) rmdir /s /q $(subst /,\\,$(OBJDIR))
endif

prebuild: | $(OBJDIR)
	$(PREBUILDCMDS)

ifneq (,$(PCH))
$(OBJECTS): $(GCH) | $(PCH_PLACEHOLDER)
$(GCH): $(PCH) | prebuild
	@echo $(notdir $<)
	$(SILENT) $(CXX) -x c++-header $(ALL_CXXFLAGS) -o "$@" -MF "$(@:%.gch=%.d)" -c "$<"
$(PCH_PLACEHOLDER): $(GCH) | $(OBJDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) touch "$@"
else
	$(SILENT) echo $null >> "$@"
endif
else
$(OBJECTS): | prebuild
endif


# File Rules
# #############################################

$(OBJDIR)/gltf_loader.o: src/assets/gltf_loader.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/bvh.o: src/core/bvh.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/scene.o: src/core/scene.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/bbox.o: src/geometry/bbox.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/mesh.o: src/geometry/mesh.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/triangle.o: src/geometry/triangle.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/nee.o: src/integrator/nee.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/lib.o: src/lib.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/main.o: src/main.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/diffuse.o: src/shading/materials/diffuse.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/emission.o: src/shading/materials/emission.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/reflection.o: src/shading/materials/reflection.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/textures.o: src/shading/textures.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"

-include $(OBJECTS:%.o=%.d)
ifneq (,$(PCH))
  -include $(PCH_PLACEHOLDER).d
endif