
CMD_AUX := aux.cpp
BIN_AUX := aux

CMD_SEQ := sequencial.cpp
BIN_SEQ := sequencial

BIN_THREADS := threads
CMD_THREADS := threads.cpp

CMD_PROCESS := process.cpp
BIN_PROCESS := process 

.PHONY: sequencial sequencial-build sequencial-run aux aux-build aux-run threads threads-build threads-run process process-build process-run

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

threads: threads-build threads-run

threads-build:
	mkdir dist 2>/dev/null || true
	clang++ $(CMD_THREADS) -o ./dist/$(BIN_THREADS)

threads-run:
	./dist/$(BIN_THREADS)

process: process-build process-run

process-build:
	mkdir dist 2>/dev/null || true
	clang++ $(CMD_PROCESS) -o ./dist/$(BIN_PROCESS)

process-run:
	./dist/$(BIN_PROCESS)

%:
	@: