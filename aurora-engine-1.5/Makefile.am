INCLUDES = $(GTK_CFLAGS)

enginedir = $(libdir)/gtk-2.0/$(GTK_VERSION)/engines
engine_LTLIBRARIES = libaurora.la

EXTRA_DIST = ./src/config.h.in


libaurora_la_SOURCES =		\
        ./src/aurora_rc_style.c	\
        ./src/aurora_rc_style.h	\
        ./src/aurora_style.c	\
        ./src/aurora_style.h	\
        ./src/aurora_theme_main.c	\
	./src/support.c			\
	./src/animation.c		\
	./src/animation.h		\
	./src/support.h			\
	./src/aurora_draw.c		\
	./src/aurora_draw.h		\
	./src/aurora_types.h	\
	./src/config.h

libaurora_la_LDFLAGS = -module -avoid-version -no-undefined
libaurora_la_LIBADD =  $(GTK_LIBS)
