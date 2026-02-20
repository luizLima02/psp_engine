TARGET = main
OBJS = ../main.o \
	   ../core/core.o \
	   ../core/Linear_Algebra/trans_proj.o \
	   ../render/renderer.o \
	   ../render/UI/font.o \
	   ../render/Camera/camera.o \
	   ../physics/physics.o
#	   ../render/renderer3D.o \
#	   ../render/Anim/animation.o \
#	   ../render/Anim/animation3D.o \
#	   ../render/PrimitivasCG/primitivas.o \
#	   ../transfProj/transproj.o \
#	   ../transfProj/camera.o \
#	   ../fisica/fisica.o \
#	   ../render/PhysicsRender/physics_render.o

INCDIR =
CFLAGS = -Wall -O2
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS =
LIBS= -lpspgum -lpspgu -lpspge -lpspdisplay -lpspdebug -lpspctrl -lpsputility

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Title
PSP_EBOOT_ICON = "../resources/ICON0.png"
PSP_EBOOT_PIC1 = "../resources/PIC1.png"

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak