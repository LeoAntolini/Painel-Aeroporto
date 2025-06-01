#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Estrutura para armazenar as informações dos voos
typedef struct Voo {
    int numero;
    char destino[50];
    char horario[6]; // formato HH:MM
    char status[20];
    char companhia[50];
    struct Voo *proximo;
} Voo;

// Função para criar um novo voo
Voo* criarVoo(int numero, char *destino, char *horario, char *companhia) {
    Voo *novoVoo = (Voo*)malloc(sizeof(Voo));
    novoVoo->numero = numero;
    strcpy(novoVoo->destino, destino);
    strcpy(novoVoo->horario, horario);
    strcpy(novoVoo->status, "No horario");
    strcpy(novoVoo->companhia, companhia);
    novoVoo->proximo = NULL;
    return novoVoo;
}

// Converte string de horario ("HH:MM") para minutos desde 00:00
int horarioParaMinutos(char *horario) {
    int h, m;
    sscanf(horario, "%d:%d", &h, &m);
    return h * 60 + m;
}

// Verifica se o horário é futuro em relação ao horário atual
int horarioFuturo(char *horario) {
    time_t agora = time(NULL);
    struct tm *tm_info = localtime(&agora);
    int minutos_atual = tm_info->tm_hour * 60 + tm_info->tm_min;
    int minutos_voo = horarioParaMinutos(horario);
    return minutos_voo > minutos_atual;
}

// Adiciona voo de forma ordenada por horário
void adicionarVoo(Voo **cabeca, int numero, char *destino, char *horario, char *companhia) {
    if (!horarioFuturo(horario)) {
        printf("Horário inválido. Só é possível adicionar voos futuros.\n");
        return;
    }

    Voo *novo = criarVoo(numero, destino, horario, companhia);
    if (*cabeca == NULL || horarioParaMinutos(horario) < horarioParaMinutos((*cabeca)->horario)) {
        novo->proximo = *cabeca;
        *cabeca = novo;
    } else {
        Voo *atual = *cabeca;
        while (atual->proximo != NULL && horarioParaMinutos(atual->proximo->horario) < horarioParaMinutos(horario)) {
            atual = atual->proximo;
        }
        novo->proximo = atual->proximo;
        atual->proximo = novo;
    }
    printf("Voo %d para %s adicionado com sucesso!\n", numero, destino);
}

// Lista voos
void listarVoos(Voo *cabeca) {
    printf("\nPainel de Voos:\n");
    printf("Num | Destino         | Hora  | Status      | Companhia\n");
    printf("-------------------------------------------------------\n");
    while (cabeca != NULL) {
        printf("%3d | %-15s | %-5s | %-10s | %-10s\n",
               cabeca->numero, cabeca->destino, cabeca->horario, cabeca->status, cabeca->companhia);
        cabeca = cabeca->proximo;
    }
}

// Atualiza status e remove voos decolados
void atualizarPainel(Voo **cabeca) {
    time_t agora = time(NULL);
    struct tm *tm_info = localtime(&agora);
    int minutos_atual = tm_info->tm_hour * 60 + tm_info->tm_min;

    while (*cabeca && horarioParaMinutos((*cabeca)->horario) <= minutos_atual) {
        printf("Voo %d para %s decolou. Removendo do painel...\n", (*cabeca)->numero, (*cabeca)->destino);
        Voo *remover = *cabeca;
        *cabeca = (*cabeca)->proximo;
        free(remover);
    }
}

// Editar voo
void editarVoo(Voo *cabeca, int numero) {
    while (cabeca) {
        if (cabeca->numero == numero) {
            printf("Novo destino: ");
            scanf("%s", cabeca->destino);
            printf("Novo horário (HH:MM): ");
            scanf("%s", cabeca->horario);
            if (!horarioFuturo(cabeca->horario)) {
                printf("Horário inválido.\n");
                return;
            }
            printf("Nova companhia: ");
            scanf("%s", cabeca->companhia);
            printf("Voo %d editado.\n", numero);
            return;
        }
        cabeca = cabeca->proximo;
    }
    printf("Voo %d não encontrado.\n", numero);
}

// Remover voo manualmente
void removerVoo(Voo **cabeca, int numero) {
    Voo *atual = *cabeca, *anterior = NULL;
    while (atual) {
        if (atual->numero == numero) {
            if (anterior == NULL) {
                *cabeca = atual->proximo;
            } else {
                anterior->proximo = atual->proximo;
            }
            free(atual);
            printf("Voo %d removido.\n", numero);
            return;
        }
        anterior = atual;
        atual = atual->proximo;
    }
    printf("Voo %d não encontrado.\n", numero);
}

// Libera memória
void liberarVoos(Voo *cabeca) {
    while (cabeca) {
        Voo *temp = cabeca;
        cabeca = cabeca->proximo;
        free(temp);
    }
}

// Adiciona voos de exemplo
void inicializarVoosExemplo(Voo **painel) {
    adicionarVoo(painel, 101, "Recife", "23:00", "Azul");
    adicionarVoo(painel, 202, "SãoPaulo", "22:30", "Gol");
    adicionarVoo(painel, 303, "Salvador", "23:15", "Latam");
}

// Menu principal
int main() {
    Voo *painel = NULL;
    int opcao, numero;
    char destino[50], horario[6], companhia[50];

    inicializarVoosExemplo(&painel);

    do {
        printf("\n=== Painel de Voos ===\n");
        printf("1. Adicionar voo\n");
        printf("2. Listar voos\n");
        printf("3. Editar voo\n");
        printf("4. Remover voo\n");
        printf("5. Atualizar painel\n");
        printf("0. Sair\n");
        printf("Escolha: ");
        scanf("%d", &opcao);

        switch(opcao) {
            case 1:
                printf("Número do voo: ");
                scanf("%d", &numero);
                printf("Destino: ");
                scanf("%s", destino);
                printf("Horário (HH:MM): ");
                scanf("%s", horario);
                printf("Companhia: ");
                scanf("%s", companhia);
                adicionarVoo(&painel, numero, destino, horario, companhia);
                break;
            case 2:
                listarVoos(painel);
                break;
            case 3:
                printf("Número do voo a editar: ");
                scanf("%d", &numero);
                editarVoo(painel, numero);
                break;
            case 4:
                printf("Número do voo a remover: ");
                scanf("%d", &numero);
                removerVoo(&painel, numero);
                break;
            case 5:
                atualizarPainel(&painel);
                break;
            case 0:
                break;
            default:
                printf("Opção inválida.\n");
        }
    } while(opcao != 0);

    liberarVoos(painel);
    return 0;
}
