
SRC = main.cpp \
		Channel/Channel.cpp \
		Client/Client.cpp \
		ServerCore/ClientLogic.cpp \
		ServerCore/MessageParser.cpp \
		ServerCore/ServerReplies.cpp \
		ServerCore/ServerSetup.cpp \
		ServerCore/ServerUtils.cpp \
		ServerCore/Commands/JoinCommand.cpp \
		ServerCore/Commands/NickCommand.cpp \
		ServerCore/Commands/PrivateMessage.cpp \
		ServerCore/Commands/UserCommand.cpp

INC = Includes/Client.hpp \
		Includes/IrcServer.hpp \
		Includes/Channel.hpp \
		Includes/replyCodes.hpp

OBJS = $(SRC:.cpp=.o)
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -fsanitize=address -g -std=c++98
NAME = ircserv

all: $(NAME)
%.o: %.cpp $(INC)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(NAME): $(OBJS) $(INC)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

clean:
	rm -f $(OBJS)
fclean: clean
	rm -f $(NAME)
re: fclean $(NAME)

.PHONY: clean
