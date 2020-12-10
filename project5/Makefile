.SUFFIXES: .c .o

CC=gcc

SRCDIR=src/
INC=include/
LIBS=lib/

# SRCS:=$(wildcard src/*.c)
# OBJS:=$(SRCS:.c=.o)

# main source file
TARGET_SRC:=$(SRCDIR)test.c
TARGET_OBJ:=$(SRCDIR)test.o
STATIC_LIB:=$(LIBS)libbpt.a

# Include more files if you write another source file.
SRCS_FOR_LIB:= \
    $(SRCDIR)lock_table.c \
    $(SRCDIR)transaction.c \
    $(SRCDIR)db.c \
	$(SRCDIR)index.c \
	$(SRCDIR)buffer.c \
	$(SRCDIR)file.c

OBJS_FOR_LIB:=$(SRCS_FOR_LIB:.c=.o)

CFLAGS+= -g -fPIC -I $(INC)

TARGET=test

all: $(TARGET)

$(TARGET): $(TARGET_OBJ) $(STATIC_LIB)
	$(CC) $(CFLAGS) $< -o $@ -L $(LIBS) -lbpt -lpthread

#%.o: %.c
#    $(CC) $(CFLAGS) $^ -c -o $@


clean:
	rm $(TARGET) $(TARGET_OBJ) $(OBJS_FOR_LIB) $(LIBS)*

d:
	rm DATA00.db log.txt

$(STATIC_LIB): $(OBJS_FOR_LIB)
	ar cr $@ $^
    


    
