# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/01/13 00:00:00 by ertrigna          #+#    #+#              #
#    Updated: 2025/12/02 16:00:02 by ertrigna         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= ircserv

CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98
RM			= rm -f

SRCS_DIR	= srcs
OBJS_DIR	= objs
INCS_DIR	= inc

SRCS		= $(SRCS_DIR)/main.cpp \
			  $(SRCS_DIR)/client/Channel.cpp \
			  $(SRCS_DIR)/client/Client.cpp \
			  $(SRCS_DIR)/client/Command_parsing.cpp \
			  $(SRCS_DIR)/client/Command_function.cpp \
			  $(SRCS_DIR)/client/Command_utils.cpp \
			  $(SRCS_DIR)/reseaux/Server_Management.cpp \
			  $(SRCS_DIR)/reseaux/Channel_Management.cpp \
			  $(SRCS_DIR)/reseaux/Socket.cpp

OBJS		= $(SRCS:$(SRCS_DIR)/%.cpp=$(OBJS_DIR)/%.o)

INCS		= -I$(INCS_DIR)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "✅ $(NAME) compiled successfully!"

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCS) -c $< -o $@

clean:
	$(RM) -r $(OBJS_DIR)
	@echo "🧹 Object files removed."

fclean: clean
	$(RM) $(NAME)
	@echo "🧹 $(NAME) removed."

re: fclean all

.PHONY: all clean fclean re
