SRC_FILES = \
jarray.c \
jdecode.c \
jele.c \
jencode.c \
jpair.c \
json.c

SRC = $(addprefix srcs/, $(SRC_FILES))

OBJ_FOLDER = objs/
OBJ_FILES = $(SRC_FILES:.c=.o)
OBJ = $(addprefix $(OBJ_FOLDER), $(OBJ_FILES))

TEST_SRC = test.c
TEST_OBJ = $(TEST_SRC:.c=.o)
TEST = test

LIB = libcjson.a

AR = ar
CC = gcc
CFLAGS = -g -O3 -Wall -Wextra -Wpedantic -Werror

all:	$(LIB)

$(LIB):	$(OBJ_FOLDER) $(OBJ)
	$(AR) -r $(LIB) $(OBJ)

$(OBJ_FOLDER):
	mkdir $(OBJ_FOLDER)

$(OBJ_FOLDER)%.o:	srcs/%.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(TEST_OBJ)
	rm -f $(OBJ_FILES)
	rm -rf $(OBJ_FOLDER)

fclean:	clean
	rm -f $(LIB)
	rm -f $(TEST)

re:	fclean all

test:	$(TEST)

$(TEST):	$(TEST_OBJ)
	$(CC) $(TEST_OBJ) -o $(TEST) $(LIB)

%.o:	%.c
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY:	test all clean fclean re
