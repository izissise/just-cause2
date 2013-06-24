##
## Makefile for copylib in /home/sinet_l//jour58
##
## Made by luc sinet
## Login   <sinet_l@epitech.net>
##
## Started on  Mon Oct 22 10:27:19 2012 luc sinet
## Last update Sun Jun  9 15:37:46 2013 Hugues
##

CC = gcc

RM = rm -f

NAME = jc2mp

TARGET = debug

OBJDIR = obj/
SRCDIR = src/
INCDIR = inc/

ifeq ($(TARGET), debug)
CFLAGS	+= -ggdb3 -pg
endif
ifeq ($(TARGET), opti2)
CFLAGS	+= -O3 -march=native
endif
ifeq ($(TARGET), opti1)
CFLAGS	+= -ffast-math
endif
CFLAGS	+= -Wall -Wextra
CFLAGS	+= -I${INCDIR}

LDFLAGS += -L/usr/lib64 -lmlx_$$HOSTTYPE -L/usr/lib64/X11 -lXext -lX11 -lm

VPATH = ${SRCDIR}

SRC = $(wildcard ${SRCDIR}*.c)

OBJ = $(patsubst ${SRCDIR}%.c,${OBJDIR}%.o,$(SRC)) 

dummy:=	$(shell test -d $(OBJDIR) || mkdir -p $(OBJDIR))
dummy:= $(shell test -d $(INCDIR) || mkdir -p $(INCDIR))
dummy:= $(shell test -d $(SRCDIR) || mkdir -p $(SRCDIR))

$(OBJDIR)%.o:	%.c
		@echo "Compiling $*" | sed 's/[-a-zA-Z]\+/\x1B[31m&\x1B[0m/g' \
		| sed 's/[A-Z]\+/\x1B[32m&\x1B[0m/g' | sed 's/[{}]/\x1B[34m&\x1B[0m/g' \
		| sed 's/[─┬─├─└│]/\x1B[35m&\x1B[0m/g'
		@$(CC) $(CFLAGS) -o $@ -c $<

$(NAME):	$(OBJ)
		@echo "Linking $@ !" | sed 's/[-a-zA-Z]\+/\x1B[34m&\x1B[0m/g'
		@$(CC) $(OBJ) -o $(NAME) $(LDFLAGS)

all:	$(NAME)

clean:
	@echo "Removing object !" | sed 's/[-a-zA-Z]\+/\x1B[35m&\x1B[0m/g'
	@$(RM) $(OBJ)

fclean:	clean
	@echo "Removing ${NAME} !" | sed 's/[-a-zA-Z]\+/\x1B[36m&\x1B[0m/g'
	@$(RM) $(NAME)

re:	fclean all

help:
	@echo "Target available: all, ${NAME}, clean, fclean" \
	| sed 's/[-a-zA-Z]\+/\x1B[37m&\x1B[0m/g'

.PHONY:	all clean fclean re help
