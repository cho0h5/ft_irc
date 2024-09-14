RM = rm -f
CXX = c++
NAME = ircserv
SRCS = main.cpp Server.cpp Client.cpp Channel.cpp Command_hyowchoi.cpp Command_siychoi.cpp Command_younghoc.cpp
OBJS = $(SRCS:.cpp=.o)
CFLAGS = -Wall -Werror -Wextra -std=c++98

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CFLAGS) -o $@ $(SRCS)

%.o : %.cpp
	$(CXX) $(CFLAGS) -c $^ -o $@ -I./

clean:
	rm -f $(OBJS)

fclean:
	make clean
	rm -f $(NAME)

re:
	make fclean
	make all

test: all
	./$(NAME) 6667 password

.PHONY: all clean fclean re
