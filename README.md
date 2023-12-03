1. Visão Geral:
O código em questão é uma implementação para a criação dinâmica de funções em linguagem de montagem x86_64. Ele é projetado para trabalhar com parâmetros de diferentes tipos (inteiro e ponteiro) e suporta três tipos de origens para os parâmetros: PARAM, FIX e IND.

2. Estrutura do Código:
2.1. Função Principal (cria_func):
A função cria_func é a função principal que cria a função dinâmica.
A função recebe o endereço da função de destino (f), uma lista de descrição de parâmetros (params), o número de parâmetros (n), e um buffer para armazenar o código gerado (codigo).
O código é construído começando com a configuração da pilha e registradores, seguido pela manipulação dos parâmetros, e termina com um epílogo que inclui uma instrução call para a função de destino.
2.2. Outras Funções:
Existem funções auxiliares como arruma_bytes, printCodigo, call_instruction, paramPARAM, paramIND, e armazenaFIX que são utilizadas para diversas operações no código.
3. Principais Pontos do Código:
3.1. Inicialização:
O código começa com a inicialização da pilha e registradores, movendo os parâmetros iniciais para os registradores necessários.
3.2. Manipulação de Parâmetros:
Os parâmetros são manipulados de acordo com sua origem: PARAM, FIX ou IND.
A manipulação inclui movimentação dos valores para os registradores e/ou pilha.
3.3. Chamada de Função de Destino:
A chamada para a função de destino é realizada usando a instrução call.
O cálculo do deslocamento para a função de destino é feito de forma apropriada.
3.4. Epílogo:
O código termina com um epílogo padrão que inclui as instruções pop e ret.
4. Observações e Melhorias Possíveis:
Cada parâmetro movido para a pilha é atualmente seguido por uma instrução push. Dependendo do contexto, isso pode ser otimizado.

TESTES:

#include <stdio.h>
#include "cria_func.h"

typedef int (*func_ptr) (int x);

int mult(int x, int y) {
  return x * y;
}

int main (void) {
  DescParam params[2];
  func_ptr f_mult;
  int i;
  unsigned char codigo[500];

  params[0].tipo_val = INT_PAR; /* o primeiro parãmetro de mult é int */
  params[0].orig_val = PARAM;   /* a nova função repassa seu parämetro */

  params[1].tipo_val = INT_PAR; /* o segundo parâmetro de mult é int */
  params[1].orig_val = FIX;     /* a nova função passa para mult a constante 10 */
  params[1].valor.v_int = 10;

  cria_func (mult, params, 2, codigo);
  f_mult = (func_ptr) codigo;   

  for (i = 1; i <=10; i++) {
    printf("%d\n", f_mult(i)); /* a nova função só recebe um argumento */
  }

  return 0;
}


TEVE COMO SAÍDA DO CODIGO:

CODIGO = 55 48 89 E5 49 89 F8 49 89 F1 49 89 D2 48 C7 C0 00 00 00 00 48 C7 F8 0A 00 00 00 57 FF D1 C9 C3 
TRADUZIDO

pushq %rbp            ; 55 - Salva o valor de %rbp na pilha
movq %rsp, %rbp       ; 48 89 E5 - Atualiza %rbp para apontar para o início da pilha

movq %rdi, %r8        ; 49 89 F8 - Move o valor de %rdi para %r8
movq %rsi, %r9        ; 49 89 F1 - Move o valor de %rsi para %r9
movq %rdx, %r10       ; 49 89 D2 - Move o valor de %rdx para %r10

movq $0, %rax          ; 48 C7 C0 00 00 00 00 - Move zero para %rax
movq $10, %rax         ; 48 C7 F8 0A 00 00 00 - Move 10 para %rax

pushq %rdi            ; 57 - Salva o valor de %rdi na pilha
callq *%rdi           ; FF D1 - Chama a função cujo endereço está em %rdi

leave                ; C9 - Equivalente a movq %rbp, %rsp ; popq %rbp
ret                  ; C3 - Retorna do procedimento

2 TESTE:

#include <stdio.h>
#include "cria_func.h"

typedef int (*func_ptr) ();

int mult(int x, int y) {
  return x * y;
}

int main (void) {
  DescParam params[2];
  func_ptr f_mult;
  int i;
  unsigned char codigo[500];

  params[0].tipo_val = INT_PAR; /* a nova função passa para mult um valor inteiro */
  params[0].orig_val = IND;     /* que é o valor corrente da variavel i */
  params[0].valor.v_ptr = &i;

  params[1].tipo_val = INT_PAR; /* o segundo argumento passado para mult é a constante 10 */
  params[1].orig_val = FIX;
  params[1].valor.v_int = 10;

  cria_func (mult, params, 2, codigo);
  f_mult = (func_ptr) codigo;
  
  for (i = 1; i <=10; i++) {
    printf("%d\n", f_mult()); /* a nova função não recebe argumentos */
  }

  //libera_func(f_mult); <== removido
  return 0;
}
TEVE COMO SAÍDA DO CODIGO:
CODIGO = 55 48 89 E5 49 89 F8 49 89 F1 49 89 D2 48 BF 80 20 60 00 00 00 00 00 57 48 B8 00 00 00 00 00 00 00 00 48 C7 C0 30 05 40 00 FF D1 C9 C3
TRADUZIDO

pushq %rbp            ; 55 - Salva o valor de %rbp na pilha
movq %rsp, %rbp       ; 48 89 E5 - Atualiza %rbp para apontar para o início da pilha

movq %rdi, %r8        ; 49 89 F8 - Move o valor de %rdi para %r8
movq %rsi, %r9        ; 49 89 F1 - Move o valor de %rsi para %r9
movq %rdx, %r10       ; 49 89 D2 - Move o valor de %rdx para %r10

movq $0x0000000000206080, %rdi ; 48 BF 80 20 60 00 00 00 00 00 - Move o valor para %rdi
pushq %rdi            ; 57 - Salva o valor de %rdi na pilha

movq $0x0000000000000000, %rax ; 48 B8 00 00 00 00 00 00 00 00 - Move o valor para %rax

movq $0x0000000040050030, %rax ; 48 C7 C0 30 05 40 00 - Move o valor para %rax

callq *%rdi           ; FF D1 - Chama a função cujo endereço está em %rdi

leave                ; C9 - Equivalente a movq %rbp, %rsp ; popq %rbp
ret                  ; C3 - Retorna do procedimento

3 TESTE

#include <stdio.h>
#include <string.h>
#include "cria_func.h"

typedef int (*func_ptr) (void* candidata, size_t n);

char fixa[] = "quero saber se a outra string é um prefixo dessa";

int main (void) {
  DescParam params[3];
  func_ptr mesmo_prefixo;
  char s[] = "quero saber tudo";
  int tam;
  unsigned char codigo[500];

  params[0].tipo_val = PTR_PAR; /* o primeiro parâmetro de memcmp é um ponteiro para char */
  params[0].orig_val = FIX;     /* a nova função passa para memcmp o endereço da string "fixa" */
  params[0].valor.v_ptr = fixa;

  params[1].tipo_val = PTR_PAR; /* o segundo parâmetro de memcmp é também um ponteiro para char */
  params[1].orig_val = PARAM;   /* a nova função recebe esse ponteiro e repassa para memcmp */

  params[2].tipo_val = INT_PAR; /* o terceiro parâmetro de memcmp é um inteiro */
  params[2].orig_val = PARAM;   /* a nova função recebe esse inteiro e repassa para memcmp */

  cria_func (memcmp, params, 3, codigo);
  mesmo_prefixo = (func_ptr) codigo;

  tam = 12;
  printf ("'%s' tem mesmo prefixo-%d de '%s'? %s\n", s, tam, fixa, mesmo_prefixo (s, tam)?"NAO":"SIM");
  tam = strlen(s);
  printf ("'%s' tem mesmo prefixo-%d de '%s'? %s\n", s, tam, fixa, mesmo_prefixo (s, tam)?"NAO":"SIM");

  return 0;
}
TEVE COMO SAÍDA DO CODIGO:
CODIGO = 55 48 89 E5 49 89 F8 49 89 F1 49 89 D2 8B 47 1C 50 48 C7 F8 0A 00 00 00 57 FF D1 C9 C3
TRADUZIDO

pushq %rbp          ; 55 - Salva o valor de %rbp na pilha
movq %rsp, %rbp     ; 48 89 E5 - Atualiza %rbp para apontar para o início da pilha

movq %rdi, %r8      ; 49 89 F8 - Move o valor de %rdi para %r8
movq %rsi, %r9      ; 49 89 F1 - Move o valor de %rsi para %r9
movq %rdx, %r10     ; 49 89 D2 - Move o valor de %rdx para %r10

movl 0x1C(%rdi), %eax ; 8B 47 1C - Move o valor em 0x1C(%rdi) para %eax
pushq %rax          ; 50 - Salva o valor de %rax na pilha

movq $0x000000000000000A, %rax ; 48 C7 F8 0A 00 00 00 - Move o valor para %rax
pushq %rax          ; 57 - Salva o valor de %rax na pilha

callq *%rdi         ; FF D1 - Chama a função cujo endereço está em %rdi

leave              ; C9 - Equivalente a movq %rbp, %rsp ; popq %rbp
ret                ; C3 - Retorna do procedimento




6. Conclusão:
O código apresenta uma implementação não funcional para a criação dinâmica de funções em linguagem de montagem x86_64, apesar de estar gerando comandos em assembly. Porem nao consigo executar a chamada da funcao que sera modificada,
não tendo saida, dando sempre o mesmo aviso : Instrução ilegal (imagem do núcleo gravada). No código foi deixado comentado
uma função para visualização do que a função está fazendo.

