/* JEDEAN 1813196 3WB */
#include <stdio.h>
#include <stdlib.h>
#include "cria_func.h"
#include <string.h>
#include <stdint.h>
void arruma_bytes(unsigned char *bytes, int size, unsigned char *codigo, int *pos);
void armazenaFIX(DescParam param, unsigned char *codigo, int *pos);
void paramPARAM(DescParam param, unsigned char *codigo, int *pos);
void paramIND(DescParam param, unsigned char *codigo, int *pos);  
void printCodigo (unsigned char *codigo, int pos);
void paramMatch (unsigned char *codigo, int paramNew, int paramOrig, int *pos);


void cria_func(void *f, DescParam params[], int n, unsigned char codigo[])
{
    int pos = 0;

    // Inicializa o quadro de pilha
    unsigned char pilha[] = {
        0x55,            // pushq %rbp
        0x48, 0x89, 0xe5 // movq %rsp,%rbp
    };
    arruma_bytes(pilha, 4, codigo, &pos);

    // Salva os registradores RDI, RSI e RDX
    unsigned char parametrosReg[] = {
        0x49, 0x89, 0xf8, // movq %rdi, %r8
        0x49, 0x89, 0xf1, // movq %rsi, %r9
        0x49, 0x89, 0xd2  // movq %rdx, %r10
    };
    arruma_bytes(parametrosReg, 9, codigo, &pos);

    // Itera sobre os parâmetros e gera o código correspondente
    for (int origP = 0; origP < n; origP++)
    {
        DescParam param = params[origP];
        if (param.orig_val == IND)
        {
            paramIND(param, codigo, &pos); // Gera código para parâmetro indireto
        }
        else if (param.orig_val == FIX)
        {
            armazenaFIX(param, codigo, &pos); // Gera código para parâmetro fixo
        }
        else if (param.orig_val == PARAM)
        {
            paramPARAM(param, codigo, &pos); // Gera código para parâmetro normal
        }
    }

    // Chama a função com os parâmetros fornecidos
    unsigned char call[] = {0xff, 0xd1};
    arruma_bytes(call, 2, codigo, &pos);

    // Desfaz o quadro de pilha e retorna
    unsigned char saida[] = {
        0xc9, // leave
        0xc3  // ret
    };
    arruma_bytes(saida, 2, codigo, &pos);

    // Debug: imprime o código gerado
  //printCodigo(codigo, pos);
}




void arruma_bytes(unsigned char *bytes, int size, unsigned char *codigo, int *pos)
{
    // Itera sobre cada byte no array de bytes
    for (int i = 0; i < size; i++)
    {
        // Copia o byte atual para a posição atual no array de códigos de máquina
        codigo[(*pos)++] = bytes[i];
    }
    // Ao final, pos contém a próxima posição disponível no array de códigos de máquina.
}




void armazenaFIX(DescParam param, unsigned char *codigo, int *pos) {
    // Verifica se o tipo do parâmetro é inteiro (INT_PAR)
    if (param.tipo_val == INT_PAR) {
        unsigned char moveParam[] = {0x48, 0xc7, 0xf8}; // Instrução: movq $valor, %rdi
        arruma_bytes(moveParam, 3, codigo, pos);
        
        // Insere os bytes representando o valor inteiro do parâmetro
        arruma_bytes((unsigned char*)&param.valor.v_int, sizeof(int), codigo, pos);

        unsigned char pushParam[] = {0x57}; // Instrução: push %rdi
        arruma_bytes(pushParam, 1, codigo, pos);
    } 
    // Verifica se o tipo do parâmetro é ponteiro (PTR_PAR)
    else if (param.tipo_val == PTR_PAR) {
        unsigned char moveParam[] = {0x48, 0xbf}; // Instrução: movq $endereço, %rdi
        arruma_bytes(moveParam, 2, codigo, pos);
        
        // Insere os bytes representando o valor do ponteiro do parâmetro
        arruma_bytes((unsigned char*)&param.valor.v_ptr, sizeof(void *), codigo, pos);

        unsigned char pushParam[] = {0x57}; // Instrução: push %rdi
        arruma_bytes(pushParam, 1, codigo, pos);
    }
}


//void intCodigo (unsigned char *codigo, int pos)
//
//  for (int i = 0; i < pos; i++)
//  {
//      printf("%02X ", codigo[i]);
//  }
//  printf("\n");
// }




void paramPARAM(DescParam param, unsigned char *codigo, int *pos) {
    // Verifica se a origem do parâmetro é PARAM
    if (param.orig_val == PARAM) {
        // Verifica se o tipo do parâmetro é inteiro (INT_PAR)
        if (param.tipo_val == INT_PAR) {
            unsigned char moveParam[] = {0x48, 0xc7, 0xc0}; // Instrução: movq $valor, %rax
            arruma_bytes(moveParam, 3, codigo, pos);
            
            // Insere os bytes representando o valor inteiro do parâmetro
            arruma_bytes((unsigned char*)&param.valor.v_int, sizeof(int), codigo, pos);
        } 
        // Verifica se o tipo do parâmetro é ponteiro (PTR_PAR)
        else if (param.tipo_val == PTR_PAR) {
            unsigned char moveParam[] = {0x48, 0xb8}; // Instrução: movq $endereço, %rax
            arruma_bytes(moveParam, 2, codigo, pos);
            
            // Insere os bytes representando o valor do ponteiro do parâmetro
            arruma_bytes((unsigned char*)&param.valor.v_ptr, sizeof(void *), codigo, pos);
        }
    }
}


void paramIND(DescParam param, unsigned char *codigo, int *pos) {
    // Verifica se é um parâmetro indireto
    if (param.orig_val == IND) {
        if (param.tipo_val == INT_PAR) {
            // Se for um parâmetro inteiro indireto, você precisa obter o valor da variável
            // no endereço fornecido e colocá-lo na pilha.
            unsigned char moveParam[] = {0x8b, 0x47, 0x00}; // movl 0(%rdi), %eax
            moveParam[2] = (param.valor.v_ptr == NULL) ? 0 : (unsigned char)(intptr_t)param.valor.v_ptr;
            // Inserindo o offset
            arruma_bytes(moveParam, 3, codigo, pos);
            // Colocando o valor na pilha
            unsigned char pushParam[] = {0x50}; // push %rax
            arruma_bytes(pushParam, 1, codigo, pos);
        } else if (param.tipo_val == PTR_PAR) {
            // Se for um parâmetro ponteiro indireto, você precisa obter o valor da variável
            // no endereço fornecido e colocá-lo na pilha.
            unsigned char moveParam[] = {0x48, 0x8b, 0x07}; // movq (%rdi), %rax
            arruma_bytes(moveParam, 3, codigo, pos);
            // Colocando o valor na pilha
            unsigned char pushParam[] = {0x50}; // push %rax
            arruma_bytes(pushParam, 1, codigo, pos);
        }
    }
}

