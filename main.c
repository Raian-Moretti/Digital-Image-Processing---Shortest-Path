/*============================================================================*/
/* IF61C TRABALHO 3                                                */
/*----------------------------------------------------------------------------*/
/* Autor: Bogdan T. Nassu - nassu@dainf.ct.utfpr.edu.br                       */
/*============================================================================*/
/** Testar a busca do caminho mais curto com arquivos .bmp. */
/*============================================================================*/

#include <stdio.h>
#include <stdlib.h>

#include "trabalho3.h"

/*============================================================================*/

#define SALVA_SAIDA 1

char* ARQUIVOS [] =
{
    "img/teste0.bmp",
    "img/teste1.bmp",
    "img/teste2.bmp",
    "img/teste3.bmp",
    "img/teste4.bmp",
    "img/teste5.bmp",
    "img/teste6.bmp",
    "img/teste7.bmp",
    "img/teste8.bmp",
    "img/teste9.bmp",
    "img/2022672-2090287-2090341-1.bmp",
    "img/2022672-2090287-2090341-2.bmp",
    "img/2022672-2090287-2090341-3.bmp",
    "img/2022672-2090287-2090341-4.bmp",

};

#define N_ARQUIVOS 14

/*============================================================================*/

void criaMatrizDT (Imagem1C* img);
void preencheMatrizDT (Imagem1C* img, int row, int col);
long testaCaminho (Coordenada* caminho, int n, Imagem1C* dt);

/*============================================================================*/

int main ()
{
    int i, n_coordenadas;
    Imagem1C* img; /* A imagem de entrada. */
    Imagem1C* img_dt; /* Matriz da transformada de distância. */
    Imagem3C* out; /* A imagem de saída. */
    Coordenada* caminho; /* O caminho descoberto. */
    char nome_saida [25]; /* String usada para salvar as saídas. */
    unsigned long score;
    FILE* out_file; /* Arquivo de saída. */

    out_file = fopen ("out.txt", "w");

    for (i = 0; i < N_ARQUIVOS; i++)
    {
        img = abreImagem1C (ARQUIVOS [i]);
        if (!img)
        {
            printf ("Nao conseguiu abrir %s\n", ARQUIVOS [i]);
            return (1);
        }

        printf("Processando imagem %s...\n", ARQUIVOS [i]);
        /* Cria a imagem da transformada de distância. */
        img_dt = abreImagem1C (ARQUIVOS [i]);
        criaMatrizDT (img_dt);

        n_coordenadas = encontraCaminho (img, &caminho, i);

        /* Testa se este caminho é um caminho válido, e calcula o score. */
        score = testaCaminho (caminho, n_coordenadas, img_dt);
        fprintf (out_file, "%ld\n", score);

        if (SALVA_SAIDA)
        {
            int c;
            out = abreImagem3C (ARQUIVOS [i]);
            for (c = 0; c < n_coordenadas; c++)
            {
                out->dados [0][caminho [c].y][caminho [c].x] = 255;
                out->dados [1][caminho [c].y][caminho [c].x] = 0;
                out->dados [2][caminho [c].y][caminho [c].x] = 0;
            }

            sprintf (nome_saida, "%d-out.bmp", i);
            salvaImagem3C (out, nome_saida);
            destroiImagem3C (out);
        }

        destroiImagem1C (img_dt);
        destroiImagem1C (img);
        free (caminho);
    }

    fclose (out_file);

    return (0);
}

/*----------------------------------------------------------------------------*/
/** Cria uma matriz com a transformada da distância de uma imagem. Para gerar
 * um score automaticamente para fotografias sem entregar uma solução para o
 * problema original, subverti o conceito da DT normal. Esta DT funciona mesmo
 * se a imagem não tiver apenas bordas. Por outro lado, o algoritmo é pesado
 * e consome bastante memória... Além disso, para manter tudo em uma imagem,
 * a distância máxima é 255 (para esta aplicação, serve). Considerei aqui a
 * distância L1 (Manhattan). */

void criaMatrizDT (Imagem1C* img)
{
    int i, j, menor;

    /* Acha o menor valor. */
    menor = 255;
    for (i = 0; i < img->altura; i++)
        for (j = 0; j < img->largura; j++)
            if (img->dados [i][j] < menor)
                menor = img->dados [i][j];

    /* "Puxa" todos os valores para baixo, de forma que o mínimo seja = 0. */
    for (i = 0; i < img->altura; i++)
        for (j = 0; j < img->largura; j++)
            img->dados [i][j] -= menor;

    /* Percorre a imagem inteira, procura pontos que mereçam atenção, e ajusta as distâncias recursivamente. */
    for (i = 0; i < img->altura; i++)
        for (j = 0; j < img->largura; j++)
            preencheMatrizDT (img, i, j);
}

/*----------------------------------------------------------------------------*/
/* Sub-função recursiva para preencher a matriz. */

void preencheMatrizDT (Imagem1C* img, int row, int col)
{
    if (img->dados [row][col] == 255)
        return; /* Não tem como melhorar a partir deste ponto! */

    /* Esquerda */
    if (col > 0 && img->dados [row][col-1] > img->dados [row][col] + 1)
    {
        img->dados [row][col-1] = img->dados [row][col]+1;
        preencheMatrizDT (img, row, col-1);
    }

    /* Direita */
    if (col < img->largura-1 && img->dados [row][col+1] > img->dados [row][col] + 1)
    {
        img->dados [row][col+1] = img->dados [row][col]+1;
        preencheMatrizDT (img, row, col+1);
    }

    /* Acima */
    if (row > 0 && img->dados [row-1][col] > img->dados [row][col] + 1)
    {
        img->dados [row-1][col] = img->dados [row][col]+1;
        preencheMatrizDT (img, row-1, col);
    }

    /* Abaixo */
    if (row < img->altura-1 && img->dados [row+1][col] > img->dados [row][col] + 1)
    {
        img->dados [row+1][col] = img->dados [row][col]+1;
        preencheMatrizDT (img, row+1, col);
    }
}

/*----------------------------------------------------------------------------*/
/* Testa um caminho. Computa um score para o mesmo. Um fracasso faz a função
 * retornar -1. Os scores não fazem sentido isoladamente, eles serão
 * posteriormente normalizados pelo desempenho dos programas testados. */

long testaCaminho (Coordenada* caminho, int n, Imagem1C* dt)
{
    int c, vizinho_em_x, vizinho_em_y;
    unsigned long score;

    /* Verifica se o caminho é longo o suficiente, se começa na coluna da esquerda e termina na coluna da direita. */
    if (n < dt->largura || caminho [0].x != 0 || caminho [n-1].x != dt->largura-1 ||
		caminho [0].y < 0 || caminho [0].y >= dt->altura)
        return (-1);

    /* Verifica se todos os pontos são vizinhos. */
    for (c = 1; c < n; c++)
    {
        vizinho_em_x = caminho [c].x == caminho [c-1].x-1 || caminho [c].x == caminho [c-1].x+1;
        vizinho_em_y = caminho [c].y == caminho [c-1].y-1 || caminho [c].y == caminho [c-1].y+1;

        if ((!vizinho_em_x && !vizinho_em_y) || (vizinho_em_x && vizinho_em_y))
            return (-1);

		/* Todos os pontos precisam estar dentro da imagem... */
		if (caminho [c].x < 0 || caminho [c].x >= dt->largura ||
			caminho [c].y < 0 || caminho [c].y >= dt->altura)
			return (-1);
    }

    /* Calcula o score para este caminho. */
    score = 0;
    for (c = 0; c < n; c++)
        score += dt->dados [caminho [c].y][caminho [c].x];
    return (score);
}

/*============================================================================*/
