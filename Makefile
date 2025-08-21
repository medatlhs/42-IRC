
SRC = main.cpp \
		Source/Client.cpp \
		Source/IrcServer.cpp

INC = Includes/Client.hpp \
		Includes/IrcServer.hpp \
		Includes/replyCodes.hpp

OBJS = $(SRC:.cpp=.o)
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
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
