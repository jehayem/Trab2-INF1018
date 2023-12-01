#include <stdio.h>
#include <stdlib.h>
#include "fun_auxiliares.h"

void moveParam(unsigned char *codigo, int srcParam, int destParam, int *pos)
{
    if (destParam < srcParam)
    {
        unsigned char moveParam[] = {0x4c, 0x89, 0xc0}; // mov %r8, %rax
        moveParam[2] += destParam * 2;                  // Adjust the destination register
        addBytes(moveParam, 3, codigo, pos);
    }
}

void initFunc(unsigned char *c, int *pos)
{
    unsigned char start[] = {0x55,
                             0x48, 0x89, 0xe5};
    addBytes(start, 4, c, pos);
}

void addByteAtPos(unsigned char *codigo, char c, int *pos)
{
    *(codigo + *pos) = c;
    (*pos)++;
}

void setParamOrigToFix(unsigned char *codigo, TipoValor tpVal, void *valor, unsigned int paramOrig, int *pos)
{
    int size = 6;
    unsigned char *setFixBytes = (unsigned char *)malloc(size);
    if (tpVal == PTR_PAR)
    {
        size = 10;
        setFixBytes = realloc(setFixBytes, size);
        movQ(setFixBytes, valor, size, paramOrig, 1);
        //
        printf("\nvalor: %p (%s)\n", valor, (unsigned char *)valor);
        //
    }
    else if (tpVal == INT_PAR)
    {
        // movl $valor, %edi/%esi/%edx
        size = 5;
        setFixBytes = realloc(setFixBytes, size);
        int *val = (int *)(setFixBytes + 1);
        *val = *((int *)valor);
        addByteToStringAtLen(setFixBytes, paramOrig, 0, 1);
    }
    else
    {
        printf("Illegal TipoValor %d error: non-existent TipoValor!!", tpVal);
        free(setFixBytes);
        exit(1);
    }
    addBytes(setFixBytes, size, codigo, pos);
    free(setFixBytes);
}

void callInstruction(void *f, unsigned char *codigo, int *pos)
{
    int size = 10;
    movQ(codigo + *pos, f, size, 3, 1);
    *pos += size;
    unsigned char callRcx[] = {0xff, 0xd1};
    addBytes(callRcx, 2, codigo, pos);
}

// Restante do código permanece inalterado...

void addBytes(unsigned char *bytes, int size, unsigned char *codigo, int *pos)
{
    for (int i = 0; i < size; i++)
    {
        addByteAtPos(codigo, bytes[i], pos);
    }
}

void printCodigo(unsigned char *codigo, int pos)
{
    for (int i = 0; i < pos; i++)
    {
        printf("%02X ", codigo[i]);
    }
    printf("\n");
}

void movQ(unsigned char *bytes, void *valor, int size, int paramOrig, int isAbs)
{
    bytes[0] = 0x48;

    *((void **)(bytes + 2)) = valor;
    addByteToStringAtLen(bytes, paramOrig, 1, isAbs);
}

void setParamsToRegisters(unsigned char *codigo, int *pos)
{
    unsigned
