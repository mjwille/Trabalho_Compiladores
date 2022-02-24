# Trabalho de Compiladores 2020/ERE

Etapas:

- [x] (26/08/2020) Análise Léxica e Inicialização de Tabela de Símbolos
- [x] (09/09/2020) Análise Sintática
- [x] (30/09/2020) Geração de Árvore Sintática Abstrata (AST)
- [x] (26/10/2020) Verificações Semânticas
- [x] (04/11/2020) Geração de Código Intermediário (TACs)
- [x] (16/11/2020) Geração de Assembly

[Especificações das Etapas](specs)  

## Como Rodar?

```bash
# Compila o compilador chamado 'etapa6'
make
# Compila um programa fonte 'test1.txt' usando o compilador 'etapa6'
./etapa6 tests/test1.txt
# Gera binário linkado Linux x86_64 do assembly gerado pelo compilador 'etapa6'
gcc asm_code.s
# Roda o programa
./a.out

# Deleta o executável e o compilador gerados
rm a.out
make clean
```

**Marcelo Jantsch Wille**  
*Universidade Federal do Rio Grande do Sul (UFRGS)*
