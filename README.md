# Calculadora em Assembly
<img src="CalculadoraAssembly.PNG" alt="Tela do simulador MARS executando o código assembly gerado pela calculadora">

## Sobre
O projeto é um gerador de código, escrito em C++, que monta um programa em assembly MIPS para calcular uma expressão. A calculadora faz soma, subtração, multiplicação, divisão, potenciação, raiz quadrada, fatorial e Fibonacci.

No arquivo Documentação.pdf há mais informações sobre o desenvolvimento do projeto.

## Como usar
1. Compile e execute `Trabalho/main.cpp` (no Code::Blocks, abra `Trabalho/TrabArqI.cbp`; ou no terminal, `g++ -o calculadora main.cpp`).
2. Digite a expressão, um item por linha, e `e` para encerrar.
3. Abra o arquivo `Cod_Gerado.asm` no simulador [MARS](https://dpetersanderson.github.io/) (ou no SPIM) e execute. O resultado aparece no console.

| Operador | Operação | Usa |
|---|---|---|
| `+` `-` `*` `/` | soma, subtração, multiplicação e divisão inteira | dois números |
| `^` | potência (o segundo número é o expoente) | dois números |
| `r` | raiz quadrada inteira, arredondada para baixo | um número |
| `!` | fatorial | um número |
| `f` | n-ésimo termo de Fibonacci (1, 1, 2, 3, 5...) | um número |

Números negativos são digitados com `n` na frente (`n5` = -5), porque `-` é o operador de subtração.

As operações são feitas da esquerda para a direita, na ordem em que os operadores foram digitados. Por exemplo, a entrada `3`, `4`, `+`, `2`, `^`, `e` calcula (3 + 4)² e mostra 49. O arquivo `Trabalho/Cod_Gerado.asm` é o código gerado para esse exemplo.

Quando não é possível calcular (divisão por zero, raiz ou fatorial de número negativo, expoente negativo), o programa mostra "Nao foi possivel calcular".

## Tecnologias utilizadas
- C++
- Assembly MIPS (simuladores MARS e SPIM)
- Code::Blocks

## Autora
Júlia Roberta Quoos Alves
