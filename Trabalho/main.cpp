// Gerador de código para uma calculadora em assembly MIPS (simulador MARS).
//
// Lê uma expressão do teclado, um item por linha (números e operadores),
// e escreve em Cod_Gerado.asm um programa que calcula o resultado.

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

const string ARQUIVO_DE_SAIDA = "Cod_Gerado.asm";
const string FIM_DA_EXPRESSAO = "e";

// Prefixo para digitar números negativos (n5 = -5), já que '-' é o operador de subtração
const char PREFIXO_NEGATIVO = 'n';

// Cada operador chama uma rotina do programa gerado
const map<char, string> ROTINA_DO_OPERADOR = {
    {'+', "main_add"},
    {'-', "main_sub"},
    {'*', "main_mult"},
    {'/', "main_div"},
    {'r', "main_raiz"},
    {'^', "main_potencia"},
    {'!', "main_fatorial"},
    {'f', "main_fibonacci"},
};

struct Expressao {
    vector<int> numeros;
    vector<char> operadores;
};


bool ehOperador(const string& item) {
    return ROTINA_DO_OPERADOR.count(item[0]) > 0;
}

int converterNumero(const string& item) {
    if (item[0] == PREFIXO_NEGATIVO)
        return -atoi(item.c_str() + 1);
    return atoi(item.c_str());
}

Expressao lerExpressao(istream& entrada) {
    Expressao expressao;
    string item;

    cout << "Digite sua equacao ('" << FIM_DA_EXPRESSAO << "' para encerrar): ";
    while (entrada >> item && item != FIM_DA_EXPRESSAO) {
        if (ehOperador(item))
            expressao.operadores.push_back(item[0]);
        else
            expressao.numeros.push_back(converterNumero(item));
    }
    return expressao;
}


// Os números ficam numa lista na memória. $t0 aponta para o primeiro número ainda não usado.
// Operações com dois números guardam o resultado no lugar do segundo e avançam $t0;
// operações com um número guardam o resultado no lugar dele.
void escreverDados(ostream& saida, const vector<int>& numeros) {
    saida << ".data\n"
          << "error_msg: .asciiz \"Nao foi possivel calcular\"\n"
          << "num: .word ";
    for (size_t i = 0; i < numeros.size(); i++)
        saida << (i > 0 ? ", " : "") << numeros[i];
    saida << "\n\n";
}

void escreverPrincipal(ostream& saida, const vector<char>& operadores) {
    saida << ".text\n"
          << "main:\n"
          << "    la      $t0, num\n";
    for (char operador : operadores)
        saida << "    jal     " << ROTINA_DO_OPERADOR.at(operador) << "\n";
    saida << "    jal     exit\n\n";
}

// Rotina para as operações de dois números que o MIPS faz numa instrução só
string rotinaDeDoisNumeros(const string& nome, const string& instrucao) {
    return nome + ":\n"
           "    lw      $s0, 0($t0)\n"
           "    lw      $s1, 4($t0)\n"
           "    " + instrucao + "     $s2, $s0, $s1\n"
           "    sw      $s2, 4($t0)\n"
           "    addi    $t0, $t0, 4\n"
           "    jr      $ra\n\n";
}

const string ROTINA_DIVISAO = R"(main_div:
    lw      $s0, 0($t0)
    lw      $s1, 4($t0)
    beqz    $s1, error              # divisao por zero
    div     $s2, $s0, $s1           # divisao inteira
    sw      $s2, 4($t0)
    addi    $t0, $t0, 4
    jr      $ra

)";

// Raiz quadrada inteira (arredondada para baixo): maior $s2 tal que ($s2 + 1)² > número
const string ROTINA_RAIZ = R"(main_raiz:
    lw      $s0, 0($t0)
    bltz    $s0, error              # raiz de numero negativo
    li      $s2, 0
raiz_repete:
    addi    $t1, $s2, 1
    mul     $t2, $t1, $t1
    bgt     $t2, $s0, raiz_fim
    move    $s2, $t1
    j       raiz_repete
raiz_fim:
    sw      $s2, 0($t0)
    jr      $ra

)";

const string ROTINA_POTENCIA = R"(main_potencia:
    lw      $s0, 0($t0)             # base
    lw      $s1, 4($t0)             # expoente
    bltz    $s1, error              # expoente negativo nao da resultado inteiro
    li      $s2, 1
    li      $t2, 0
potencia_repete:
    beq     $t2, $s1, potencia_fim
    mul     $s2, $s2, $s0
    addi    $t2, $t2, 1
    j       potencia_repete
potencia_fim:
    sw      $s2, 4($t0)
    addi    $t0, $t0, 4
    jr      $ra

)";

const string ROTINA_FATORIAL = R"(main_fatorial:
    lw      $t1, 0($t0)
    bltz    $t1, error              # fatorial de numero negativo
    li      $s2, 1                  # 0! = 1
fatorial_repete:
    blez    $t1, fatorial_fim
    mul     $s2, $s2, $t1
    addi    $t1, $t1, -1
    j       fatorial_repete
fatorial_fim:
    sw      $s2, 0($t0)
    jr      $ra

)";

// N-ésimo termo da sequência 1, 1, 2, 3, 5, 8...
const string ROTINA_FIBONACCI = R"(main_fibonacci:
    lw      $t2, 0($t0)
    blez    $t2, error              # a sequencia comeca no 1o termo
    li      $s3, 0                  # termo anterior
    li      $s2, 1                  # termo atual
fibonacci_repete:
    addi    $t2, $t2, -1
    blez    $t2, fibonacci_fim
    add     $s4, $s3, $s2
    move    $s3, $s2
    move    $s2, $s4
    j       fibonacci_repete
fibonacci_fim:
    sw      $s2, 0($t0)
    jr      $ra

)";

// Mostra o último resultado ($s2) e encerra; em caso de erro, mostra a mensagem e encerra
const string ROTINAS_DE_SAIDA = R"(exit:
    move    $a0, $s2
    li      $v0, 1                  # imprime inteiro
    syscall
    li      $v0, 10                 # encerra
    syscall

error:
    la      $a0, error_msg
    li      $v0, 4                  # imprime texto
    syscall
    li      $v0, 10                 # encerra
    syscall
)";

void escreverRotinas(ostream& saida) {
    saida << rotinaDeDoisNumeros("main_add", "add")
          << rotinaDeDoisNumeros("main_sub", "sub")
          << rotinaDeDoisNumeros("main_mult", "mul")
          << ROTINA_DIVISAO
          << ROTINA_RAIZ
          << ROTINA_POTENCIA
          << ROTINA_FATORIAL
          << ROTINA_FIBONACCI
          << ROTINAS_DE_SAIDA;
}


int main() {
    Expressao expressao = lerExpressao(cin);

    ofstream saida(ARQUIVO_DE_SAIDA);
    escreverDados(saida, expressao.numeros);
    escreverPrincipal(saida, expressao.operadores);
    escreverRotinas(saida);

    cout << "\nArquivo Criado!";
}
