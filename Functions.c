/*  ALUNOS
    Gabriel A. Fanha
    Jose Henrique Ivanchechen
    Raian A. Moretti
*/

#include "trabalho3.h"
#include <stdlib.h>
#include <stdio.h>

#define LARGURA_FILTRO_MIN 1 // tamanho da janela para o filtro do mínimo - 2->5x5
#define THRESHOLD 122 //valor para filtro da média
#define DEBUG_MODE  //comente esta linha se não quiser salvar as imagens intermediárias

Coordenada* inverter(Coordenada* v1,int size);
void filtro(Imagem1C* in, Imagem1C* out, unsigned long largura, unsigned long altura, int janela);
Coordenada* caminhos(Imagem1C* in, long largura, long altura, int* size);

int encontraCaminho (Imagem1C *img, Coordenada **caminho, int nro){
    Imagem1C* normalizada = NULL;
    Imagem1C* filtrada = NULL;
    Imagem1C* binarizada = NULL;
    char name[100]; //string utilizada para armazenar o nome das imagens de verificação

    /*----------------------------------------------------------------------*/
    /*
     * Faz o pré-processamento das imagem de entrada!
     */
    if (normalizada && (normalizada->largura < img->largura || normalizada->altura < img->altura)){
        /* Desaloca tudo. */
        destroiImagem1C (normalizada);
        destroiImagem1C (filtrada);
        destroiImagem1C (binarizada);
        normalizada = NULL; /* Marca para criar um novo buffer. */
    }

    if (!normalizada){
        normalizada = criaImagem1C (img->largura, img->altura);
        filtrada = criaImagem1C (img->largura, img->altura);
        binarizada = criaImagem1C (img->largura, img->altura);
    }

    /*
     * Pré-processamento da imagem, de forma a gerar a imagem binarizada,
    a partir da qual pode ser calculada a matriz de custo e, portanto,
    o menor caminho
    */
    normalizaImagem(img, normalizada, img->largura, img->altura); //normaliza
    filtroMin(normalizada, filtrada, img->largura, img->altura); //alarga as regiões escuras
    filtro(filtrada, binarizada, img->largura, img->altura, 2); //binariza

    /*Se o DEBUG_MODE estiver definido, salva as imagens intermediárias!*/
    #ifdef DEBUG_MODE
    sprintf (name, "%d-0norm.bmp", nro);
    salvaImagem1C (normalizada, name);
    sprintf (name, "%d-1filtr.bmp", nro);
    salvaImagem1C (filtrada, name);
    sprintf (name, "%d-2bin.bmp", nro);
    salvaImagem1C (binarizada, name);
    #endif
    /*----------------------------------------------------------------------*/

    int size;
    *caminho = caminhos(binarizada, img->largura, img->altura, &size);
    return size;
}

/*----------------------------------------------------------------------------*/
/** Funçãozinha auxiliar que cria uma coordenada. */

Coordenada criaCoordenada (int x, int y)
{
    Coordenada c;
    c.x = x;
    c.y = y;
    return (c);
}

/*----------------------------------------------------------------------------*/
/** Normaliza uma imagem, deixando todos os pixels no intervalo [0,255].
 * Os parâmetros largura e altura são extras, e são passados porque estamos
 * trabalhando com buffers que podem ser maiores do que o necessário. */

void normalizaImagem (Imagem1C* in, Imagem1C* out, unsigned long largura, unsigned long altura)
{
    int menor, maior, i, j;
    float fator;

    /* Encontra o mínimo e o máximo da imagem de entrada. */
    menor = in->dados [0][0];
    maior = in->dados [0][0];
    for (i = 0; i < altura; i++)
        for (j = 0; j < largura; j++)
        {
            if (in->dados [i][j] < menor)
                menor = in->dados [i][j];
            if (in->dados [i][j] > maior)
                maior = in->dados [i][j];
        }

    /* Calcula o fator de correção. */
    fator = 255.0f / (maior-menor);

    /* Ajusta tudo. */
    for (i = 0; i < altura; i++)
        for (j = 0; j < largura; j++)
            out->dados [i][j] = (unsigned char) (((in->dados[i][j] - menor) * fator) + 0.5f);
}

/*----------------------------------------------------------------------------*/
/** Filtro que substitui cada pixel pelo valor mínimo em uma vizinhança. É
 * equivalente a uma erosão morfológica com um kernel quadrado. Mantive o
 * processamento nas bordas, porque neste programa elas serão importantes. */

void filtroMin (Imagem1C* in, Imagem1C* out, unsigned long largura, unsigned long altura){
    int l, c, i, j, minimo;
    for(l = 0; l < altura; l++)
        for (c = 0; c < largura; c++){
            minimo = 255;
            for(i = l-LARGURA_FILTRO_MIN; i <= l+LARGURA_FILTRO_MIN; i++)
                for(j = c-LARGURA_FILTRO_MIN; j <= c+LARGURA_FILTRO_MIN; j++)
                    if(i >= 0 && i < altura &&
                        j >= 0 && j < largura &&
                        in->dados [i][j] < minimo)
                        minimo = in->dados [i][j];
            out->dados [l][c] = minimo;
        }
}

/*----------------------------------------------------------------------------*/
/** Binarização simples. */

void binariza (Imagem1C* in, Imagem1C* out, unsigned long largura, unsigned long altura){
    int i, j;
    for (i = 0; i < altura; i++)
        for (j = 0; j < largura; j++)
            if (in->dados [i][j] >= THRESHOLD)
                out->dados [i][j] = 255;
            else
                out->dados [i][j] = 0;
}


/*  filtro(Imagem1C* in, Imagem1C* out, unsigned long largura, unsigned long altura, int janela)
 *  Esta função tem como objetivo fazer a binariazação local com janelas específicadas por parâmetro, ela pega o valor máximo e o valor
 *  mínimo da imagem dentro dessa janela e faz a média entre os 2, caso os valores de máximo e minimo estejam muito próximos o algoritmo irá
 *  desconsiderar e deixará tudo como branco, caso contrário o algoritmo irá fazer a binarização local baseado na média.
 *  Parametros:
 *      Imagem1C* in: imagem de entrada.
 *      Imagem1C* out: imagem de saída (binarizada).
 *      long largura: largura da imagem de entrada.
 *      long altura: altura da imahgem de entrada.
 *      int janela: tamanho da janela que irá ser realizado a binarização (mesmo valor para altura e largura).
 *
 */
void filtro(Imagem1C* in, Imagem1C* out, unsigned long largura, unsigned long altura, int janela){

  int eixo_x,eixo_y;
  int limiar=0;
  int valor_max, valor_min;
  int i,j;

   // Faz o loop por janelas dentro da imagem
   for(eixo_x=janela;eixo_x<(altura-janela);eixo_x++)
    {
        for(eixo_y=janela; eixo_y<(largura-janela);eixo_y++)
        {
            valor_max=0; // Assume valor máximo dentro da janela como sendo 0
            valor_min=255; // Assume valor minimo dentro da janela como sendo 255
            // Faz o loop dos pixels dentro da janela
            for(i=(eixo_x-janela);i<=(eixo_x+janela);i++)
            {
                for(j=(eixo_y-janela);j<=(eixo_y+janela);j++)
                {
                    // Seleciona valor máximo e mínimo
                    if(in->dados[i][j]>valor_max)
                        valor_max=in->dados[i][j];
                    else if(in->dados[i][j]<valor_min)
                        valor_min=in->dados[i][j];
                }
            }
            // Verifica se o valor máximo é bem distante (35 pelo menos) do valor minimo
            if((valor_max-35)>valor_min)
            {
                limiar=(valor_max+valor_min)/2;
                limiar+=15; // Adiciona uma maior chance dos pixeis serem pretos
                // Faz a binarização dentro da janela
                for(i=(eixo_x-janela);i<=(eixo_x+janela);i++)
                {
                    for(j=(eixo_y-janela);j<=(eixo_y+janela);j++)
                    {
                        if(in->dados[i][j]>limiar)
                            out->dados[i][j]=255;
                        else if(in->dados[i][j]<=limiar)
                            out->dados[i][j]=0;
                    }
                }
            }
            else // Valor máximo e mínimo estão bem pertos, assume branco
            {
                for(i=(eixo_x-janela);i<=(eixo_x+janela);i++)
                {
                    for(j=(eixo_y-janela);j<=(eixo_y+janela);j++)
                        out->dados[i][j]=255;

                }
            }
        }
    }
}

/*  Coordenada* caminhos(Imagem1C* in, long largura, long altura, int* size)
 *  Esta função tem como objetivo encontrar o menor caminho em preto de uma ponta (esquerda) até a outra ponta (direita) dentro da imagem.
 *  A função foi dividida em 6 fases:
 *      Fase 1: Criar matriz onde os pixeis em preto irão ser zerados para que o algoritmo possa encontrá-los e colocar a distancia até o início.
 *      Fase 2: Encontrar o começo (lado esquerdo) da imagem (onde a linha começa), e marcá-lo com 1.
 *      Fase 3: Substituir os zeros colocados pela Fase 1 com a distância dele com o pixel incial encontrado pela Fase 2.
 *      Fase 4: Encontrar o final (lado direito) da imagem (onde a linha termina).
 *      Fase 5: Criar o vetor de caminhos que irá ser retornado pela função, encontrando o menor caminho (loop para ir pelos menores números da fase 3).
 *      Fase 6: Retornar tanto o tamanho do vetor (*size) quanto o próprio vetor (invertido da Fase 5).
 *
 *  Parametros:
 *      Imagem1C* in: imagem monocromática de entrada.
 *      long largura: largura da imagem de entrada.
 *      long altura: altura da imagem de entrada.
 *      int* size: ponteiro para o tamanho do vetor de caminhos que a função retorna.
 *
 *  Retorno:
 *      Coordenada*: vetor do tipo Coordenada contendo as coordenadas do menor caminho encontrado pela função.
 */

Coordenada* caminhos(Imagem1C* in, long largura, long altura, int* size){
    // Fase 1
    int** arr = (int**) malloc(altura*sizeof(int*));
    int zeros = 0;
    int brancos = 0;
    for(int i = 0; i < altura; i++){
        arr[i] = (int*) malloc(largura*(sizeof(int)));
        for(int j = 0; j < largura; j++){
            int c = in->dados[i][j];
            if(c == 0){ // Preto, caminho vazio
                arr[i][j] = 0;
                zeros++;
            }else{ // Branco, não pode passar
                arr[i][j] = -1;
                brancos++;
            }
        }
    }

    // Fase 2
    int dist = 1, termino = 0;
    int ultj = 0; // Ultimo pixel (lado direito) percorrido pelo algoritmo
    while(ultj < largura-10){
        dist = 1;
        termino = 0;

        for(int i = 0; i < altura; i++){
            if(arr[i][0] == 0){
                arr[i][0] = 1;
                break; // Achou o começo da linha.
            }
        }

        // Fase 3
        while(termino == 0){
            termino = 1;
            for(int i = 1; i < altura-1; i++){
                for(int j = 0; j < largura-1; j++){
                    if(arr[i][j] == dist){ // última distância colocada
                        if(arr[i-1][j] == 0) arr[i-1][j] = dist+1;
                        if(arr[i+1][j] == 0) arr[i+1][j] = dist+1;
                        if(i > 0)
                            if(arr[i][j-1] == 0) arr[i][j-1] = dist+1;
                        if(arr[i][j+1] == 0) arr[i][j+1] = dist+1;
                        termino = 0;
                        if(ultj < j) ultj = j+1;
                    }
                }
            }
            dist++;
        }
    }

    // Fase 4
    int mi, m = -1;
    for(int i = 0; i < altura; i++){
        if(arr[i][largura-1] <= 0) continue;
        if(arr[i][largura-1] < m || m == -1){
            mi = i;
            m = arr[i][largura-1];
        }
    }

    // Fase 5
    Coordenada* coords = (Coordenada*) malloc(m*sizeof(Coordenada));
    int atual = m, // índice atual (ultimo percorrido)
        index = 0; // índice atual do vetor coords
    int ax = mi, // índice atual da altura
        aj = largura-1; // índice atual da largura

    // Cria a primeira coordenada (do lado direito da imagem)
    coords[index] = criaCoordenada(aj,ax);
    index++;
    while(atual != 1){
        if(arr[ax-1][aj] < atual && arr[ax-1][aj] > 0) ax--;
        else if(arr[ax+1][aj] < atual && arr[ax+1][aj] > 0) ax++;
        else if(arr[ax][aj-1] < atual && arr[ax][aj-1] > 0) aj--;
        else if(arr[ax][aj+1] < atual && arr[ax][aj+1] > 0) aj++;
        atual = arr[ax][aj];
        coords[index] = criaCoordenada(aj,ax);
        index++;
    }

    // Fase 6
    free(*arr);
    free(arr);

    *size=index;
    coords=inverter(coords,index);
    return coords;
}

Coordenada* inverter(Coordenada* v1,int size){
    int i;
    Coordenada* coords=(Coordenada*)malloc(size*sizeof(Coordenada));
    for(i=size-1;i>=0;i--)
        coords[size-(i+1)]=v1[i];

    return coords;
}
