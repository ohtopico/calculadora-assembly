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
const size_t MAXIMO_DE_ITENS = 10;  // limite de números e de operadores

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
    while (expressao.numeros.size() < MAXIMO_DE_ITENS &&
           expressao.operadores.size() < MAXIMO_DE_ITENS &&
           entrada >> item && item != FIM_DA_EXPRESSAO) {
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
          << "error_msg: .asciiz \"Não foi possivel calcular\"\n"
          << "num: .word ";
    for (int numero : numeros)
        saida << numero << ",";
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

// Raiz quadrada: subtrai os ímpares 1, 3, 5... até zerar; a quantidade subtraída é a raiz
const string ROTINA_RAIZ = R"(main_raiz:
    lw      $s0, 0($t0)
    li      $s2, 0
raiz_repete:
    mul     $t2, $s2, 2
    add     $t2, $t2, 1
    sub     $s0, $s0, $t2
    add     $s2, $s2, 1
    beq     $s0, $zero, raiz_fim
    slt     $t2, $s0, $zero
    beq     $t2, 1, error
    j       raiz_repete
raiz_fim:
    sw      $s2, 0($t0)
    jr      $ra

)";

const string ROTINA_POTENCIA = R"(main_potencia:
    lw      $s0, 0($t0)
    lw      $s2, 0($t0)
    lw      $s1, 4($t0)
    li      $t2, 1
potencia_repete:
    beq     $t2, $s1, potencia_fim
    mul     $s2, $s2, $s0
    add     $t2, $t2, 1
    j       potencia_repete
potencia_fim:
    sw      $s2, 4($t0)
    add     $t0, $t0, 4
    jr      $ra

)";

const string ROTINA_FATORIAL = R"(main_fatorial:
    lw      $s2, 0($t0)
    lw      $t1, 0($t0)
    beqz    $s2, error
    bltz    $s2, error
fatorial_repete:
    subi    $t1, $t1, 1
    beqz    $t1, fatorial_fim
    mul     $s2, $s2, $t1
    sw      $s2, 0($t0)
    j       fatorial_repete
fatorial_fim:
    sw      $s2, 0($t0)
    jr      $ra

)";

const string ROTINA_FIBONACCI = R"(main_fibonacci:
    lw      $t2, 0($t0)
    blez    $t2, error
    li      $s3, 1
    li      $s4, 1
    add     $s5, $s3, $s4
    addi    $t2, $t2, -3
    sw      $s5, 0($t0)
    la      $s2, ($s5)
    j       fibonacci_repete
fibonacci_fim:
    jr      $ra
fibonacci_repete:
    add     $s3, $s5, $s4
    addi    $t2, $t2, -1
    sw      $s3, 0($t0)
    la      $s2, ($s3)
    blez    $t2, fibonacci_fim
    add     $s4, $s5, $s3
    addi    $t2, $t2, -1
    sw      $s4, 0($t0)
    la      $s2, ($s4)
    blez    $t2, fibonacci_fim
    add     $s5, $s4, $s3
    sw      $s5, 0($t0)
    la      $s2, ($s5)
    addi    $t2, $t2, -1
    blez    $t2, fibonacci_fim
    bgtz    $t2, fibonacci_repete
)";

// Mostra o último resultado ($s2) e encerra; em caso de erro, mostra a mensagem
const string ROTINAS_DE_SAIDA = R"(
exit:
    la      $a0, ($s2)
    li      $v0, 1
    syscall
    li      $v0, 10
    syscall

error:
    la      $a0, error_msg
    la      $v0, 4
    syscall
)";

void escreverRotinas(ostream& saida) {
    saida << rotinaDeDoisNumeros("main_add", "add")
          << rotinaDeDoisNumeros("main_sub", "sub")
          << rotinaDeDoisNumeros("main_mult", "mul")
          << rotinaDeDoisNumeros("main_div", "div")
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
