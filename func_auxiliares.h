#ifndef FUN_AUXILIARES_H
#define FUN_AUXILIARES_H

#include <stdio.h>

void initFunc(unsigned char *c, int *pos);
void addByteAtPos(unsigned char *codigo, char c, int *pos);
void moveParam(unsigned char *codigo, int srcParam, int destParam, int *pos);
void setParamOrigToFix(unsigned char *codigo, TipoValor tpVal, void *valor, unsigned int paramOrig, int *pos);
void callInstruction(void *f, unsigned char *codigo, int *pos);
void addBytes(unsigned char *bytes, int size, unsigned char *codigo, int *pos);
void printCodigo(unsigned char *codigo, int pos);
void movQ(unsigned char *bytes, void *valor, int size, int paramOrig, int isAbs);
void setParamsToRegisters(unsigned char *codigo, int *pos);

#endif  // FUN_AUXILIARES_H
