CMD_MAIN := main.cpp
BIN_MAIN := main 
CMD_AUX := aux.cpp
BIN_AUX := aux

CMD_SEQ := sequencial.cpp
BIN_SEQ := sequencial

.PHONY: sequencial sequencial-build sequencial-run aux aux-build aux-run

sequencial: sequencial-build sequencial-run

sequencial-build:
	mkdir dist 2>/dev/null || true
	clang++ $(CMD_SEQ) -o ./dist/$(BIN_SEQ)

sequencial-run:
	./dist/$(BIN_SEQ)

aux: aux-build aux-run

aux-build:
	mkdir dist 2>/dev/null || true
	clang++ $(CMD_AUX) -o ./dist/$(BIN_AUX)

aux-run:
	./dist/$(BIN_AUX)

%:
	@: