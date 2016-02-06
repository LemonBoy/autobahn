XDIFF=xdiff/xdiffi.c \
      xdiff/xpatience.c \
      xdiff/xemit.c \
      xdiff/xprepare.c \
      xdiff/xutils.c \
      xdiff/xmerge.c \
      xdiff/xhistogram.c

all:
	$(CC) $(XDIFF) main.c -Os -Wall -Werror -o autobahn
