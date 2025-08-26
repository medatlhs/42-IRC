
SRC = main.cpp \
		Source/Channel.cpp \
		Source/Client.cpp \
		Source/Server/ClientLogic.cpp \
		Source/Server/MessageParser.cpp \
		Source/Server/ServerReplies.cpp \
		Source/Server/ServerSetup.cpp \
		Source/Server/ServerUtils.cpp \
		Source/Server/JoinCommand.cpp \
		Source/Server/NickCommand.cpp \
		Source/Server/PrivateMessage.cpp \
		Source/Server/UserCommand.cpp

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
