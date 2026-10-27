NAME = computorv2

SRCS = srcs/main.cpp srcs/Rational.cpp srcs/Complex.cpp srcs/Matrix.cpp

OBJS = ${SRCS:.cpp=.o}

CXX = c++ -std=c++11

CXXFLAGS = -Wall -Werror -Wextra

# ---------- Logo / progress ----------
define PROGRESS_BAR
	@printf "\033[1;36mCompiling sources...\033[0m\n"
	@total=$(words ${SRCS}); \
	i=0; \
	for f in ${SRCS}; do \
		i=$$((i+1)); \
		width=40; \
		fill=$$(( i * width / total )); \
		empty=$$(( width - fill )); \
		bar=$$(printf '%*s' $$fill '' | tr ' ' '#'); \
		printf "\r\033[1;32m[%-*s]\033[0m %3d%%  \033[0;37m%-30s\033[0m\033[K" \
			$$width "$$bar" $$(( i * 100 / total )) "$$f"; \
		sleep 0.15; \
	done; \
	printf "\n"
endef

define LOGO
	@printf "\033[1;36m\n"
	@printf "        ╔════════════════════════════╗\n"
	@printf "        ║  ┌──────────────────────┐  ║\n"
	@printf "        ║  │  42.                 │  ║\n"
	@printf "        ║  └──────────────────────┘  ║\n"
	@printf "        ║  ┌────┐┌────┐┌────┐┌────┐  ║\n"
	@printf "        ║  │ 7  ││ 8  ││ 9  ││ ÷  │  ║\n"
	@printf "        ║  ├────┤├────┤├────┤├────┤  ║\n"
	@printf "        ║  │ 4  ││ 5  ││ 6  ││ ×  │  ║\n"
	@printf "        ║  ├────┤├────┤├────┤├────┤  ║\n"
	@printf "        ║  │ 1  ││ 2  ││ 3  ││ −  │  ║\n"
	@printf "        ║  ├────┤├────┤├────┤├────┤  ║\n"
	@printf "        ║  │ 0  ││ .  ││ =  ││ +  │  ║\n"
	@printf "        ║  └────┘└────┘└────┘└────┘  ║\n"
	@printf "        ╚════════════════════════════╝\n"
	@printf "\033[0m\n"
	@printf "\033[1;33m               ── ComputorV2 ──\033[0m\n\n"
endef

# ---------- Targets ----------
${NAME}:	${OBJS}
		@${CXX} ${CXXFLAGS} ${OBJS} -o ${NAME}
		$(PROGRESS_BAR)
		$(LOGO)

# Silences make's implicit compile echo
%.o: %.cpp
		@${CXX} ${CXXFLAGS} -c $< -o $@

all:	${NAME}

bonus:	all

clean:
	@rm -rf ${OBJS}

fclean: clean
	@rm -rf ${NAME}

re: fclean all

# --- Debug target ---
debug: CXXFLAGS += -g -O0
debug: fclean ${NAME}

.PHONY: all bonus clean fclean re debug