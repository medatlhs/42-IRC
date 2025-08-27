
SRC = main.cpp \
		Source/Channel.cpp \
		Source/Client.cpp \
		Source/Server/client_logic.cpp \
		Source/Server/message_parser.cpp \
		Source/Server/ServerReplies.cpp \
		Source/Server/ServerSetup.cpp \
		Source/Server/ServerUtils.cpp \
		Source/Server/Join.cpp \
		Source/Server/Nick.cpp \
		Source/Server/Privmsj.cpp \
		Source/Server/User.cpp \
		Source/Server/Part.cpp

INC = Includes/Client.hpp \
		Includes/IrcServer.hpp \
		Includes/Channel.hpp \
		Includes/replyCodes.hpp

OBJS = $(SRC:.cpp=.o)
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
NAME = ircserv
# -fsanitize=address -g 
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
