# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: amakino <amakino@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/06/26 01:36:55 by amakino           #+#    #+#              #
#    Updated: 2026/06/26 13:24:44 by amakino          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME    = codexion
CC      = cc
CFLAGS  = -Wall -Wextra -Werror -pthread

SRCS    = main.c \
          code2_parsing.c \
          code3_init.c \
          code4_utils.c \
          code5_actions.c \
          code6_monitor.c \
          code7_queue.c

OBJS    = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
