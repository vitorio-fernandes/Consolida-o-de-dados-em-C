/*
*********** PROJETO DE PROGRAMAÇÃO 1 ***********

GRUPO: ANDERSON, EVERTON E VITÓRIO;
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Estrutura de dados
struct dados{
    int dataRegistro, codCidade, codEstado, novosCasos, dataAtual;
    struct dados *proximo;
};

// Tranforma número epoch em Data padrão;
char *epochData(int tempo){
    time_t epoch_time = tempo;
    struct tm *tm_info = localtime(&epoch_time);
    char *data = calloc(20,sizeof(char));
    strftime(data, 20, "%d/%m/%Y",tm_info);
    return data;
}

// Data do sistema em epoch;
time_t dataEpoch(){
    time_t current_time = time(NULL);

    if (current_time == ((time_t)-1)) {
        fprintf(stderr, "Erro ao obter a data e hora atual.\n");
        exit(EXIT_FAILURE);
    }
    return current_time;
}


// Verifica se existe dados repetidos na lista e corrigi ela
int verificarIguais(int cidade, int estado, struct dados *inicio){
    struct dados *noAtual = inicio;

    while (noAtual != NULL){
        if (noAtual->codEstado == estado){
            if (noAtual->codCidade == cidade){
                return 1;
            }
        }
        noAtual = noAtual->proximo;
    }
    return 0;
}


// Ler os arquivos e adiciona a lista
int manipularDados(char nomeArquivo[], struct dados **cabeca, struct dados **rabo) {
    FILE *pontArquivos = fopen(nomeArquivo, "r");
    FILE *arquivoHistorico = fopen("Historico_Cargas.csv", "a");

    if (pontArquivos != NULL) {
        while (!feof(pontArquivos)) {
            int dataAtual, campo1, campo2, campo3, campo4;;
            time_t current_epoch = dataEpoch();
            dataAtual = (current_epoch);

            fscanf(pontArquivos, "%d;%d;%d;%d", &campo1, &campo2, &campo3, &campo4);

            fprintf(arquivoHistorico,"%d;%d;%d;%d;%d\n", campo1, campo2, campo3, campo4,dataAtual);// Cria o arquivo historico de carga

            int data = campo1, codigoCidade = campo2, codigoEstado = campo3, novosCasos = campo4;

            if (verificarIguais(codigoCidade,codigoEstado,*cabeca) == 1){// Caso a função retorne 1 significa que existe algum dado repetido
                struct dados *noAtual = *cabeca;
                while (noAtual != NULL){
                    if (noAtual->codEstado == codigoEstado){
                        if (noAtual->codCidade == codigoCidade){
                            noAtual->novosCasos += novosCasos;
                            if (data > noAtual->dataRegistro){
                                noAtual->dataRegistro = data;
                            }
                        }
                        noAtual = noAtual->proximo;
                    }else{
                        noAtual = noAtual->proximo;
                    }
                }
            }else{// Caso a função retorne 0 significa que não existe algum dado repetido e ele vai adicionar a lista
                struct dados *novoNo = malloc(sizeof(struct dados));
                novoNo->dataRegistro = data;
                novoNo->codCidade = codigoCidade;
                novoNo->codEstado = codigoEstado;
                novoNo->novosCasos = novosCasos;
                novoNo->proximo = NULL;
                if (*cabeca == NULL) {
                    *cabeca = novoNo;
                    *rabo = novoNo;
                } else {
                    (*rabo)->proximo = novoNo;
                    *rabo = novoNo;
                }
            }
        }
    } else {
        printf("------------ Nao foi possivel carregar o arquivo! ------------\n\n");
        return 0;
    }
    fclose(pontArquivos);
    fclose(arquivoHistorico);
    printf("------------ Arquivo carregado com sucesso! ------------\n\n");
    return 1;
}


// Corre pela lista e adiciona ao arquivo os dados já tratados
void gerarArquivoConsolidado(struct dados *inicio){
    FILE *arquivoConsolidado = fopen("ArquivoConsolidado.csv", "w");
    struct dados *NoAtual = inicio;

    while (NoAtual != NULL){
        if (NoAtual->proximo == NULL){
            fprintf(arquivoConsolidado,"%d;%d;%d;%d", NoAtual->codCidade, NoAtual->codEstado, NoAtual->novosCasos, NoAtual->dataRegistro);
        }else{
            fprintf(arquivoConsolidado,"%d;%d;%d;%d\n", NoAtual->codCidade, NoAtual->codEstado, NoAtual->novosCasos, NoAtual->dataRegistro);
        }
        NoAtual = NoAtual->proximo;
    }
    fclose(arquivoConsolidado);
}


// Imprime os dados do estado buscado
int imprimir(int entradaUsuario, struct dados *inicio){
    FILE *arquivoPronto = fopen("ArquivoConsolidado.csv", "r");
    struct dados *novoNo = inicio, *novoNo2 = inicio;
    int somaGeral = 0;

    while (novoNo != NULL) {
        if (novoNo->codEstado == entradaUsuario) {
            somaGeral += novoNo->novosCasos;
        }
        novoNo = novoNo->proximo;
    }
    if (somaGeral == 0){
        return 0;
    }

    printf("*********** RELATORIO ESTADUAL ***********\n\n");
    printf("Codigo Estado %d\n\n", entradaUsuario);
    printf("Total de casos no estado: %d casos\n\n",somaGeral);
    printf("Dados por cidade:\n\n");
    
    printf("CIDADE \t QTD CASOS \t ULTIMA ATUALIZACAO\n");
    printf("--------------------------------------\n");

    rewind(arquivoPronto);

    while (novoNo2 != NULL) {
        int campo1;
        if (novoNo2->codEstado == entradaUsuario){
            campo1 = novoNo2->dataRegistro;
            char *data = epochData(campo1);
            printf("%5d %9d \t %s\n", novoNo2->codCidade,novoNo2->novosCasos, data);
            free(data);
        }
        novoNo2 = novoNo2->proximo;
    }
    printf("-------------------------------------------------\n\n\n");

    fclose(arquivoPronto);
    return 1;
}

int main(){
    struct dados *cabeca = NULL, *rabo = NULL;

    int opcao, resultadoCaso1, resultadoCaso2;
    int entrada;
    char nomeArquivo[50];

    do {// Funcionamento do menu
        printf("************ SISTEMA DE CONSOLIDACAO DE DADOS ************\n");
        printf("\n");
        printf("1- FAZER CARGA DE ARQUIVOS.\n");
        printf("2- IMPRIMIR RELATORIO POR ESTADO.\n");
        printf("0- SAIR.\n");
        printf("DIGITE SUA OPCAO: ");
        scanf("%d", &opcao);

        switch(opcao) {
            case 1:
                printf("DIGITE O NOME DO ARQUIVO: ");
                scanf("%s",nomeArquivo);
                resultadoCaso1 = manipularDados(nomeArquivo, &cabeca,&rabo);
                if (resultadoCaso1 == 0){// Caso retorne 0 quer dizer que não foi possivel carregar o arquivo
                    break;
                }else{// Caso retorne 1 quer dizer que foi possivel carregar o arquivo
                    gerarArquivoConsolidado(cabeca);
                    break;
                }
            case 2:
                printf("DIGTE O CODIGO DO ESTADO: ");
                scanf("%i",&entrada);
                printf("\n");
                resultadoCaso2 = imprimir(entrada, cabeca);
                if (resultadoCaso2 == 0){// Caso retorne 0 quer dizer que o estado buscado não existe
                    printf("Estado nao encontrado!\n\n");
                }
                break;
            case 0:
                printf("Saindo...\n\n");
                break;
            default:
                printf("Opcao invalida. Tente novamente.\n\n");
        }
    } while(opcao != 0);

    while(rabo != NULL){// Limpar a lista após terminar o programa
        struct dados *limpar = rabo->proximo;
        free(rabo);
        rabo = limpar;
    }

    return 0;
}