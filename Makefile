#
#  Copyright (C) 2011 Andreas Öman
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

BUILDDIR = build

PROG=${BUILDDIR}/htshttpd

CFLAGS  = -Wall -Werror -Wwrite-strings -Wno-deprecated-declarations 
CFLAGS += -Wmissing-prototypes
LDFLAGS += -lpthread

#
# Core
#
SRCS =  src/main.c \
	src/tcp.c \
	src/http.c \
	src/utils.c \
	src/file.c \
	src/htsbuf.c \

# Various transformations
SRCS  += $(SRCS-yes)
DLIBS += $(DLIBS-yes)
SLIBS += $(SLIBS-yes)
OBJS=    $(SRCS:%.c=$(BUILDDIR)/%.o)
OBJS_EXTRA = $(SRCS_EXTRA:%.c=$(BUILDDIR)/%.so)
DEPS=    ${OBJS:%.o=%.d}
OBJDIRS= $(sort $(dir $(OBJS))) $(sort $(dir $(OBJS_EXTRA)))


# Common CFLAGS for all files
CFLAGS_com  = -g -funsigned-char -O2 
CFLAGS_com += -D_FILE_OFFSET_BITS=64
CFLAGS_com += -I${BUILDDIR} -I${CURDIR}/src -I${CURDIR}

all: ${PROG}

.PHONY:	clean distclean

${PROG}: $(OBJDIRS) $(OBJS) ${OBJS_EXTRA} Makefile
	$(CC) -o $@ $(OBJS) $(LDFLAGS) ${LDFLAGS_cfg}

$(OBJDIRS):
	@mkdir -p $@

${BUILDDIR}/%.o: %.c
	$(CC) -MD $(CFLAGS_com) $(CFLAGS) $(CFLAGS_cfg) -c -o $@ $(CURDIR)/$<

${BUILDDIR}/%.so: ${SRCS_EXTRA}
	${CC} -O -fbuiltin -fomit-frame-pointer -fPIC -shared -o $@ $< -ldl

clean:
	rm -rf ${BUILDDIR}/src
	find . -name "*~" | xargs rm -f

distclean: clean
	rm -rf build.*

ifneq ($(VERSION), $(CURVERSION))
.PHONY:	src/version.c
$(info Version changed)
src/version.c:
	echo $(VERSION) >${BUILDDIR}/ver
endif


# Include dependency files if they exist.
-include $(DEPS)
