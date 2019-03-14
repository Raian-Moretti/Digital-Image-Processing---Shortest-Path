/*============================================================================*/
/* DOIS TIPOS PARA MANIPULAÇÃO DE ARQUIVOS BMP                                */
/*----------------------------------------------------------------------------*/
/* Autor: Bogdan T. Nassu - nassu@dainf.ct.utfpr.edu.br                       */
/*============================================================================*/
/** Este arquivo traz declarações para dois tipos e rotinas para manipulação de
 * arquivos bmp. Como temos um propósito puramente didático, apenas um sub-
 * conjunto mínimo do formato foi implementado. Matrizes são usadas para
 * representar os dados. Vetores seriam computacionalmente mais eficientes, mas
 * aqui procuramos priorizar a clareza e a facilidade de uso. */
/*============================================================================*/

#ifndef __IMAGEM_H
#define __IMAGEM_H

/*============================================================================*/
/* Imagem em escala de cinza (1 canal). */

typedef struct
{
	unsigned long largura;
	unsigned long altura;
	unsigned char** dados; /* Matriz de dados. */
} Imagem1C;

/*----------------------------------------------------------------------------*/
/* Por simplicidade e compatibilidade, as funções para ler e escrever imagens
 * de 1 canal na verdade convertem a imagem para 3 canais. Isto quer dizer que
 * só podemos ler imagens com 24bpp. */

Imagem1C* criaImagem1C (int largura, int altura);
void destroiImagem1C (Imagem1C* img);
Imagem1C* abreImagem1C (char* arquivo);
int salvaImagem1C (Imagem1C* img, char* arquivo);

/*============================================================================*/
/* Imagem RGB (3 canais). */

typedef struct
{
	unsigned long largura;
	unsigned long altura;
	unsigned char*** dados; /* 3 matrizes de dados. */
} Imagem3C;

/*----------------------------------------------------------------------------*/

Imagem3C* criaImagem3C (int largura, int altura);
void destroiImagem3C (Imagem3C* img);
Imagem3C* abreImagem3C (char* arquivo);
int salvaImagem3C (Imagem3C* img, char* arquivo);

/*============================================================================*/
#endif /* __IMAGEM_H */
