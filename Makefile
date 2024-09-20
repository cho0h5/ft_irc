RM = rm -f
CXX = c++
NAME = ircserv
SRCS = main.cpp				\
	   Server.cpp			\
	   Client.cpp			\
	   Channel.cpp			\
	   Error.cpp			\
	   Command_join_topic_invite_kick.cpp	\
	   Command_mode.cpp	\
	   Command_nick_user_privmsg.cpp
OBJS = $(SRCS:.cpp=.o)
CFLAGS = -Wall -Werror -Wextra -std=c++98

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CFLAGS) -o $@ $^

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

.PHONY: all clean fclean re test
