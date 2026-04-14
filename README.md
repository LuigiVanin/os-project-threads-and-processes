# Multiplicação de Matrizes — Análise de Desempenho entre Paralelismo e Execução Sequencial

Trabalho prático da disciplina de **Sistemas Operacionais**, com o objetivo de comparar o desempenho de três abordagens de execução para a multiplicação de matrizes: sequencial, paralela com threads(POSIX) e paralela com processos nativos do Linux.

---

## Estrutura do Projeto

```
.
├── aux.cpp          # Gerador de matrizes de entrada aleatórias
├── sequencial.cpp   # Implementação sequencial
├── threads.cpp      # Implementação paralela com POSIX threads
├── process.cpp      # Implementação paralela com processos (fork + memória compartilhada)
├── data/
│   ├── sequencial/  # Resultados da execução sequencial
│   ├── threads/     # Resultados por número de threads (2x, 4x, 8x, 16x)
│   └── process/     # Resultados por número de processos (2x, 4x, 8x, 16x)
│       └── {dimensão}/
│           └── {nº workers}/
│               └── *.txt   # 10 amostras por cenário
└── assets/
    ├── 100x100/
    ├── 200x200/
    ├── 400x400/
    └── 800x800/     # Gráficos de Threads/Processos × Tempo por dimensão
```

---

## Descrição das Implementações

### `aux.cpp` — Gerador de Matrizes

Gera duas matrizes aleatórias com dimensões fornecidas pelo usuário, escrevendo-as nos arquivos `matrix_1.txt` e `matrix_2.txt`. Verifica a compatibilidade das dimensões para multiplicação (colunas de A = linhas de B) antes de prosseguir.

### `sequencial.cpp` — Execução Sequencial

Realiza a multiplicação `C = A × B` em um único thread, percorrendo todas as linhas e colunas de forma iterativa. Serve como baseline para comparação de desempenho.

### `threads.cpp` — Paralelismo com POSIX Threads

Divide as linhas da matriz resultado `C` igualmente entre N threads POSIX. Cada thread calcula sua fatia de linhas de forma independente, acessando memória compartilhada do mesmo processo sem a necessidade de IPC explícito.

### `process.cpp` — Paralelismo com Processos (fork)

Divide as linhas de `C` entre N processos filhos criados via `fork()`. A matriz resultado é armazenada em uma região de **memória compartilhada** (`shmget`/`shmat`) acessível por todos os processos. As matrizes de entrada herdadas do processo pai via copy-on-write permanecem como páginas físicas compartilhadas enquanto não são modificadas.

---

## Metodologia de Coleta de Dados

Para cada combinação de (abordagem × dimensão × número de workers), foram realizadas **10 execuções independentes**. O tempo medido cobre exclusivamente o cálculo da multiplicação, usando `clock_gettime(CLOCK_MONOTONIC)` com resolução em microssegundos. Os cenários testados foram:

| Dimensão das Matrizes | Threads testados          |
|-----------------------|---------------------------|
| 100 × 100             | 1 (seq.), 2, 4, 8, 16     |
| 200 × 200             | 1 (seq.), 2, 4, 8, 16     |
| 400 × 400             | 1 (seq.), 2, 4, 8, 16     |
| 800 × 800             | 1 (seq.), 2, 4, 8, 16     |

Cada arquivo de saída em `data/` segue o formato:

```
<linhas> <colunas>
c<i><j> <valor>
...
<tempo em µs>
<número de threads>
```

---

## Resultados e Análise

Os gráficos gerados a partir das médias de cada cenário estão disponíveis na pasta `assets/`.

### Ganho de desempenho com paralelismo

Em todos os tamanhos de matriz testados, aumentar o número de threads ou processos reduziu consistentemente o tempo de execução. O ganho mais expressivo ocorreu ao passar de 1 (sequencial) para 2 workers, com redução de tempo próxima a **50%** — resultado esperado para uma divisão de trabalho quase ideal quando o overhead de criação ainda é pequeno em relação ao volume de cálculo.

### Retornos decrescentes (Lei de Amdahl)

A melhoria de desempenho foi progressivamente menor conforme o número de workers aumentava. O salto de 8 para 16 threads/processos trouxe ganho marginal em todos os cenários. Isso é explicado pela **Lei de Amdahl**: o speedup máximo de um programa paralelo é limitado pela fração sequencial inevitável (criação de workers, sincronização, join/wait, overhead de escalonamento). À medida que se aumentam os workers, cada um recebe uma fatia menor de trabalho, e o custo fixo de gerenciamento passa a dominar o ganho computacional. Além disso, com muitos workers operando simultaneamente, a **largura de banda de memória** torna-se um gargalo compartilhado, reduzindo o ganho efetivo de adicionar mais paralelismo.

### Processos × Threads: por que processos foram mais rápidos?

Contra a intuição inicial — de que threads devem ser mais leves que processos — os resultados mostraram que a abordagem com `fork()` superou consistentemente a abordagem com POSIX threads. Alguns fatores que explicam esse comportamento:

1. **Isolamento de cache por processo**: cada processo filho possui seu próprio espaço de endereçamento virtual e, em sistemas multicore, tende a ocupar regiões distintas dos caches L1/L2. Threads do mesmo processo competem pelo mesmo cache, gerando maior pressão de cache (*cache thrashing*) especialmente no acesso às linhas da matriz B, que é lida por todos os workers.

2. **Copy-on-write eficiente**: ao fazer `fork()`, o kernel Linux não copia as páginas de memória das matrizes de entrada — apenas marca as páginas como compartilhadas e somente leitura. Como os filhos nunca escrevem em A ou B, as páginas nunca precisam ser duplicadas fisicamente, mantendo o overhead de memória baixo.

3. **Escalonamento independente**: processos são unidades de escalonamento independentes com maior isolamento de recursos do kernel, o que pode resultar em distribuição mais uniforme entre núcleos físicos do que threads de um mesmo processo em determinadas configurações do escalonador Linux.

---

## Como Compilar e Executar

```bash
# Compilar
g++ -O2 -o aux       aux.cpp
g++ -O2 -o sequencial sequencial.cpp
g++ -O2 -o threads   threads.cpp -lpthread
g++ -O2 -o process   process.cpp

# Gerar matrizes de entrada (ex: 200x200 × 200x200)
./aux
# -> informe as dimensões quando solicitado

# Executar cada abordagem
./sequencial
./threads       # solicita número de threads
./process       # solicita número de processos
```

Os resultados são salvos automaticamente em `output/sequencial/`, `output/threads/` e `output/process/` respectivamente.
