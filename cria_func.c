#include <stdio.h>
#include <stdlib.h>
#include "cria_func.h"
#include "fun_auxiliares.h"

void cria_func(void *f, DescParam params[], int n, unsigned char codigo[])
{
    int pos = 0;
    absPos = pos;

    initFunc(codigo, &pos);
    printf("Init: \n");
    printCodigo(codigo, pos);

    setParamsToRegisters(codigo, &pos); // guarda valor de parametros em r8,9, e 10 respectivamente
    printf("paramsToRegisters: \n");
    printCodigo(codigo, pos);

    unsigned char paramNew = 0;
    for (int paramOrig = 0; paramOrig < n; paramOrig++)
    {
        DescParam param = params[paramOrig];
        if (param.orig_val == PARAM)
        {
            moveParam(codigo, paramOrig, paramNew, &pos);
            printf("PARAM: \n");
            printCodigo(codigo, pos);
            paramNew++;
        }
        else if (param.orig_val == FIX)
        {
            void *val = (param.tipo_val == PTR_PAR) ? param.valor.v_ptr : &(param.valor.v_int);
            setParamOrigToFix(codigo, param.tipo_val, val, paramOrig, &pos);
            printf("FIX: \n");
            printCodigo(codigo, pos);
        }
        else if (param.orig_val == IND)
        {
            printf("IND: \n");
            if (param.tipo_val == PTR_PAR)
            {
                void *val = param.valor.v_ptr;
                setParamOrigToFix(codigo, param.tipo_val, val, paramOrig, &pos);
                printCodigo(codigo, pos);
            }
            else if (param.tipo_val == INT_PAR)
            {
                unsigned char movAddr[10];
                printf("Valor: %p (%d)\n", param.valor.v_ptr, *((int *)param.valor.v_ptr));
                movQ(movAddr, param.valor.v_ptr, 10, 5, 1);
                addBytes(movAddr, 10, codigo, &pos);
                unsigned char bytes[] = {0x8b, 0x00};
                if (paramOrig == 0)
                {
                    bytes[1] = 0x38;
                }
                else if (paramOrig == 1)
                {
                    bytes[1] = 0x30;
                }
                else if (paramOrig == 2)
                {
                    bytes[1] = 0x10;
                }

                addBytes(bytes, 2, codigo, &pos);
                printCodigo(codigo, pos);
            }
        }
    }

    callInstruction(f, codigo, &pos);
    addByteAtPos(codigo, 0xc9, &pos); // leave
    addByteAtPos(codigo, 0xc3, &pos); // ret
    absPos = pos;
}
