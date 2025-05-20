symnmf: symnmf.o symnmf.h
	@echo "building"
	@gcc -o symnmf symnmf.o -lm

symnmf.o: symnmf.c
	@echo "compiling"
	@gcc -ansi -Wall -Wextra -Werror -pedantic-errors -c symnmf.c 

clean:
	@rm -f *.o symnmf	
	@echo "all cleaned up"