.data
error_msg: .asciiz "Nao foi possivel calcular"
num: .word 3, 4, 2

.text
main:
    la      $t0, num
    jal     main_add
    jal     main_potencia
    jal     exit

main_add:
    lw      $s0, 0($t0)
    lw      $s1, 4($t0)
    add     $s2, $s0, $s1
    sw      $s2, 4($t0)
    addi    $t0, $t0, 4
    jr      $ra

main_sub:
    lw      $s0, 0($t0)
    lw      $s1, 4($t0)
    sub     $s2, $s0, $s1
    sw      $s2, 4($t0)
    addi    $t0, $t0, 4
    jr      $ra

main_mult:
    lw      $s0, 0($t0)
    lw      $s1, 4($t0)
    mul     $s2, $s0, $s1
    sw      $s2, 4($t0)
    addi    $t0, $t0, 4
    jr      $ra

main_div:
    lw      $s0, 0($t0)
    lw      $s1, 4($t0)
    beqz    $s1, error              # divisao por zero
    div     $s2, $s0, $s1           # divisao inteira
    sw      $s2, 4($t0)
    addi    $t0, $t0, 4
    jr      $ra

main_raiz:
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

main_potencia:
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

main_fatorial:
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

main_fibonacci:
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

exit:
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
