# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/12/15 15:28:52 by ertrigna          #+#    #+#              #
#    Updated: 2025/12/15 15:35:46 by ertrigna         ###   ########.fr        #
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
              $(SRCS_DIR)/channel/Channel.cpp \
              $(SRCS_DIR)/channel/Channel_Management.cpp \
              $(SRCS_DIR)/client/Client.cpp \
              $(SRCS_DIR)/client/Command_parsing.cpp \
              $(SRCS_DIR)/client/Command_function.cpp \
              $(SRCS_DIR)/client/Command_utils.cpp \
              $(SRCS_DIR)/reseaux/Server_Management.cpp \
              $(SRCS_DIR)/reseaux/Socket.cpp \
              $(SRCS_DIR)/bot/Bot.cpp

OBJS		= $(SRCS:$(SRCS_DIR)/%.cpp=$(OBJS_DIR)/%.o)
OBJS_COUNT	= $(words $(OBJS))

INCS		= -I$(INCS_DIR)

# Couleurs
RESET		= \033[0m
GREEN		= \033[1;32m
YELLOW		= \033[1;33m
C		= \033[1;36m
WHITE		= \033[1;37m
RED			= \033[1;31m
BLINK		= \033[5m

# Compteur
COMPILED	= 0

all: header $(NAME) footer

header:
	@echo "$(C)╔═══════════════════════════════════════════════════════════════╗$(RESET)"
	@echo "$(C)║$(YELLOW)                🚀 Compiling $(NAME)...                        $(C)║$(RESET)"
	@echo "$(C)╚═══════════════════════════════════════════════════════════════╝$(RESET)"
	@echo ""

$(NAME): $(OBJS)
	@echo ""
	@echo "$(C)🔗 Linking...$(RESET)"
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

footer:
	@echo ""
	@echo "$(GREEN)╔═══════════════════════════════════════════════════════════════╗$(RESET)"
	@echo "$(GREEN)║$(BLINK)$(WHITE)             ✅ COMPILATION SUCCESSFUL! ✅                     $(RESET)$(GREEN)║$(RESET)"
	@echo "$(GREEN)║                                                               ║$(RESET)"
	@echo "$(GREEN)║$(YELLOW)  Executable: $(WHITE)$(NAME)                                          $(GREEN)║$(RESET)"
	@echo "$(GREEN)║$(YELLOW)  Ready to launch your IRC server! 🚀                          $(GREEN)║$(RESET)"
	@echo "$(GREEN)╚═══════════════════════════════════════════════════════════════╝$(RESET)"
	@echo ""

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@$(eval COMPILED=$(shell echo $$(($(COMPILED)+1))))
	@$(eval PERCENT=$(shell echo $$(($(COMPILED)*100/$(OBJS_COUNT)))))
	@$(eval BARS=$(shell echo $$(($(PERCENT)/2))))
	@printf "\r$(C)[$(GREEN)"
	@printf "%0.s█" $$(seq 1 $(BARS))
	@printf "$(C)%0.s░" $$(seq $(BARS) 50)
	@printf "$(C)] $(YELLOW)%3d%% $(WHITE)Compiling...$(RESET)" $(PERCENT)
	@$(CXX) $(CXXFLAGS) $(INCS) -c $< -o $@
	@if [ $(COMPILED) -eq $(OBJS_COUNT) ]; then printf "\n"; fi

clean:
	@echo "$(RED)🧹 Cleaning object files...$(RESET)"
	@$(RM) -r $(OBJS_DIR)
	@echo "$(GREEN)✅ Object files removed.$(RESET)"

fclean: clean
	@echo "$(RED)🧹 Removing $(NAME)...$(RESET)"
	@$(RM) $(NAME)
	@echo "$(GREEN)✅ $(NAME) removed.$(RESET)"

re: fclean all

.PHONY: all clean fclean re header footer