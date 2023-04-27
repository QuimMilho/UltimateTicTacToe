
#include "jogo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../componentes/consts.h"
#include "../componentes/structs.h"
#include "../libs/utils.h"
#include "../componentes/menu.h"

//Gestao do jogo
int loadGame(Jogo* jogo);
int createGame(Byte modo, Jogo* jogo);
int verifyWinner(Byte *tab);
int verifyTie(Byte *tab);
int saveGame(Jogo *jogo);
int savePlays(FILE *file, Jogadas *jog);
int end(Jogo *jogo);
unsigned int readFileName(char *str);

//Gestão de jogadas
int nextPlay(Jogo *jogo);
int playerPlay(Jogo *jogo);
int aiPlay(Jogo *jogo);
int validPlay(int pos, Jogo *jogo);
int play(Byte player, int pos, Jogo *jogo);

//Gestao de memoria
int createTab(Tabuleiro *tab);
int createPlay(Byte pos, Byte tab, Jogo *jogo);
void clean(Jogo *jogo);
void cleanTab(Tabuleiro *tab);
void deletePlays(Jogadas *jogada);

//Gestao do tab
Byte* getTab(Byte* tabs, Byte n);
void printTab(Tabuleiro* tab, Byte nextTab);
char getChar(Tabuleiro* tab, Byte x, Byte y, Byte nextTab);
int getPos(int x, int y);
void setPiece(Byte piece, int pos, int tab, Jogo *jogo);
void clearTab(Byte *tab);
void setCircle(Byte *tab);
void setCross(Byte *tab);

//Gestao principal do jogo

int jogarJogo(int modo) {
    int err, running;
    Jogo jogo;
    switch (modo) {
        case RELOAD:
            err = loadGame(&jogo);
            break;
        case JOGO_NORMAL:
            err = createGame(JOGO_NORMAL, &jogo);
            break;
        case JOGO_AI:
            err = createGame(JOGO_AI, &jogo);
            break;
        default:
            err = 1;
    }
    if (err)
        return err;
    do {
        running = nextPlay(&jogo);
        saveGame(&jogo);
        if (verifyTie(jogo.tab.win)) running == 0;
    } while (running);
    end(&jogo);
    clean(&jogo);
    return 0;
}

//Gestao do jogo

int createGame(Byte modo, Jogo* jogo) {
    int err;
    err = createTab(&jogo->tab);
    if (err)
        return err;
    jogo->nextTab = 5;
    jogo->gamemode = modo;
    jogo->nJogadas = 0;
    jogo->jogadas = NULL;
    jogo->lastPlay = NULL;
    return 0;
}

int loadGame(Jogo* jogo) {
    int err, i;
    err = createTab(&jogo->tab);
    if (err)
        return err;
    FILE *file = fopen(TEMP_FILE, "rb");
    short data[2];
    unsigned int k = fread(&data, sizeof(short), 2, file);
    jogo->gamemode = data[0];
    jogo->nJogadas = 0;
    int nJogadas = data[1];
    if (!k) return 1;
    for (i = 0; i < nJogadas; i++) {
        k = fread(&data, sizeof(short), 2, file);
        if (!k) return 2;
        if (i == 0) jogo->nextTab = 5;
        else jogo->nextTab = data[1];
        play(i % 2 == 0 ? 1 : 2, data[0], jogo);
    }
    fclose(file);
    return 0;
}

int saveGame(Jogo *jogo) {
    FILE *file = fopen(TEMP_FILE, "wb");
    if (!file) return 1;
    short b[2] = {jogo->gamemode, jogo->nJogadas};
    unsigned int k = fwrite(&b, sizeof(short), 2, file);
    if (!k) return 1;
    if (jogo->jogadas)
        savePlays(file, jogo->jogadas);
    fclose(file);
    return 0;
}

int savePlays(FILE *file, Jogadas *jog) {
    short data[2] = {jog->pos, jog->tab};
    unsigned int k = fwrite(&data, sizeof(short), 2, file);
    if (!k) return 1;
    if (jog->next) savePlays(file, jog->next);
    return 0;
}

int end(Jogo *jogo) {
    printTab(&jogo->tab, 0);
    int k = verifyWinner(jogo->tab.tabs);
    if (k) {
        if (jogo->gamemode == JOGO_NORMAL) {
            printf("O jogador %c ganhou!\n", (char) ('A' + (k - 1)));
        } else {
            if (k == 1) {
                printf("O jogador ganhou!\n");
            } else {
                printf("A AI ganhou!\n");
            }
        }
    } else {
        printf("Foi um empate!\n");
    }
    int a;
    do {
        saveMenu();
        unsigned int err = scanf("%d", &a);
        if (err != 1) return 1;
        char str[100];
        switch (a) {
            case OPT_SIM:
                if (readFileName(str)) {
                    FILE *file = fopen(str, "w");
                    if (jogo->gamemode == JOGO_NORMAL) {
                        fprintf(file, "Jogo multi player\n");
                    } else {
                        fprintf(file, "Jogo single player\n");
                    }
                    Jogadas *jog = jogo->jogadas;
                    while (jog) {
                        if (jogo->gamemode == JOGO_NORMAL) {
                            fprintf(file, "O jogador %c jogou na posicao %d do tabuleiro %d\n", (char) ('A' + (jog->n % 2)), jog->pos, jog->tab);
                        } else {
                            if (jog->n % 2 == 0) {
                                fprintf(file, "O jogador escolheu a posicao %d do tabuleiro %d\n", jog->pos, jog->tab);
                            } else {
                                fprintf(file, "A AI escolheu a posicao %d do tabuleiro %d\n", jog->pos, jog->tab);
                            }
                        }
                        jog = jog->next;
                    }
                    fclose(file);
                } else {
                    a = 3;
                    printf("Nome inválido!");
                }
                break;
            case OPT_NAO:
                break;
            default:
                printf("Esse valor é inválido!");
        }
    } while (a != OPT_SIM && a != OPT_NAO);
    return 0;
}

unsigned int readFileName(char *str) {
    printf("Escolhe o nome do ficheiro:");
    unsigned int k = scanf("%s", str);
    strcat(str, ".txt");
    return k;
}

//Gestão das jogadas

int nextPlay(Jogo *jogo) {
    int err;
    printTab(&jogo->tab, jogo->nextTab);
    if (jogo->nJogadas % 2 == 0) {
        err = playerPlay(jogo);
    } else {
        if (jogo->gamemode == JOGO_AI) {
            err = aiPlay(jogo);
        } else {
            err = playerPlay(jogo);
        }
    }
    if (err) return 0;
    return verifyWinner(jogo->tab.win) ? 0 : 1;
}

int verifyWinner(Byte *tab) {
    //Linhas
    for (int i = 0; i < 3; i++) {
        Byte start = tab[i*3];
        if (start != 0) {
            if (tab[i * 3] == tab[i * 3 + 1] && tab[i * 3 + 1] == tab[i * 3 + 2]) return tab[i * 3 + 1];
        }
    }
    //Colunas
    for (int i = 0; i < 3; i++) {
        Byte start = tab[i];
        if (start != 0) {
            if (tab[i] == tab[3 + i] && tab[3 + i] == tab[6 + i]) return tab[3 + i];
        }
    }
    //Diagonais
    if (tab[4] != 0) {
        if (tab[0] == tab[4] && tab[4] == tab[8]) return tab[4];
        if (tab[2] == tab[4] && tab[4] == tab[6]) return tab[4];
    }
    return 0;
}

int verifyTie(Byte *tab) {
    int i;
    for (i = 0; i < 9; i++) {
        if (!tab[i]) return 0;
    }
    return 1;
}

int playerPlay(Jogo *jogo) {
    int res1, res2;
    int pos;
    do {
        printf("Player %c: Escolhe um sitio onde jogar (x y):", (char) ('A' + jogo->nJogadas % 2));
        int a = scanf("%d %d", &res1, &res2);
        if (a != 2) {
            clean(jogo);
            exit(1);
        }
        pos = getPos(res1, res2);
    } while(!validPlay(pos, jogo));
    return play(jogo->nJogadas % 2 == 0 ? 1 : 2 , pos, jogo);
}

int aiPlay(Jogo *jogo) {
    int pos, k = 9, i;
    Byte *tab = getTab(jogo->tab.tabs, jogo->nextTab);
    for (i = 0; i < 9; i++) if (tab[i]) k--;
    k = intUniformRnd(k);
    for (i = 0; i < 9; i++) {
        k--;
        if (k == 0)
            pos = i + 1;
    }
    return play(2, pos, jogo);
}

int getPos(int x, int y) {
    return (y - 1) * 3 + x;
}

int validPlay(int pos, Jogo *jogo) {
    if (pos > 9 || pos < 1) return 0;
    Byte *tab = getTab(jogo->tab.tabs, jogo->nextTab);
    if (tab[pos - 1]) return 0;
    return 1;
}

int play(Byte player, int pos, Jogo *jogo) {
    createPlay(pos, jogo->nextTab, jogo);
    Byte *tabul = getTab(jogo->tab.tabs, jogo->nextTab);
    setPiece(player, pos, jogo->nextTab, jogo);
    int l, m;
    if ((l = verifyWinner(tabul))) {
        if (l == 1) setCross(tabul);
        else if (l == 2) setCircle(tabul);
        jogo->tab.win[jogo->nextTab - 1] = l;
        if ((m = verifyWinner(jogo->tab.win))) {
            return m;
        } else if (verifyTie(jogo->tab.win)) {
            return 3;
        }
    } else if (verifyTie(tabul)) {
        clearTab(tabul);
    }
    if (jogo->tab.win[pos - 1]) {
        int a = 9, i;
        for (i = 0; i < 9; i++) {
            if (jogo->tab.win[i]) a--;
        }
        int k = intUniformRnd(a);
        jogo->nextTab = k;
    } else {
        jogo->nextTab = pos;
    }
    jogo->nJogadas++;
    return 0;
}

//Gestao de memoria

int createTab(Tabuleiro *tab) {
    int i;
    //Inicializa os tabuleiros
    tab->tabs = malloc(sizeof(Byte) * 81);
    if (tab->tabs == NULL) {
        return -1;
    }
    //Inicializa o tabuleiro principal
    tab->win = malloc(sizeof(Byte) * 9);
    if (tab->win == NULL) {
        free(tab->tabs);
        return -2;
    }
    //Inicializa os valores dos tabuleiros
    for (i = 0; i < 81; i++) {
        if (i < 9)
            tab->win[i] = 0;
        tab->tabs[i] = 0;
    }
    return 0;
}

int createPlay(Byte pos, Byte tab, Jogo *jogo) {
    Jogadas *jog = malloc(sizeof(Jogadas));
    if (!jog) return -4;
    if (jogo->nJogadas == 0) jogo->jogadas = jog;
    jog->next = NULL;
    jog->prev = jogo->lastPlay;
    jog->n = jogo->nJogadas;
    jog->tab = tab;
    jog->pos = pos;
    if (jogo->lastPlay)
        jogo->lastPlay->next = jog;
    jogo->lastPlay = jog;
    return 0;
}

void clean(Jogo *jogo) {
    cleanTab(&jogo->tab);
    deletePlays(jogo->lastPlay);
    remove(TEMP_FILE);
}

void cleanTab(Tabuleiro *tab) {
    free(tab->tabs);
    free(tab->win);
}

void deletePlays(Jogadas *jogada) {
    if (jogada->prev) deletePlays(jogada->prev);
    free(jogada);
}

//Gestao do tab

Byte* getTab(Byte* tabs, Byte n) {
    return &tabs[(n - 1) * 9];
}

void printTab(Tabuleiro* tab, Byte nextTab) {
    int i, h;
    for (i = 0; i < 13; i++) {
        for (h = 0; h < 17; h++) {
            printf("%c", getChar(tab, h, i, nextTab % 3 == 0 ? nextTab - 3 : nextTab));
        }
        printf("\n");
    }
}

char getChar(Tabuleiro* tab, Byte x, Byte y, Byte nextTab) {
    int nextTabCol = nextTab % 3, nextTabLin = nextTab / 3 + 1;
    if (nextTabCol == 0)
        nextTabCol = 3;
    if (x % 5 == 0 || x % 5 == 1 || y % 4 == 0) {
        if (x <= 5 * nextTabCol + 1 && x >= 5 * (nextTabCol - 1) && y >= 4 * (nextTabLin - 1) && y <= 4 * nextTabLin)
            return (char) 219;
        else
            return (char) 176;
    } else {
        Byte nTab = ((x - 2) / 5 + 1) + (y - 1) / 4 * 3;
        Byte pos = (x - 2) % 5 + (y - 1) % 4 * 3;
        Byte peca = getTab(tab->tabs, nTab)[pos];
        if (peca == 0)
            return '#';
        else if (peca == 1)
            return 'X';
        else if (peca == 2)
            return 'O';
        else if (peca == 3)
            return ' ';
    }
    return '#';
}

void setPiece(Byte piece, int pos, int tab, Jogo *jogo) {
    Byte *tabul = getTab(jogo->tab.tabs, tab);
    tabul[pos - 1] = piece;
}

void clearTab(Byte *tab) {
    int i;
    for (i = 0; i < 9; i++) tab[i] = 0;
}

void setCircle(Byte *tab) {
    int i;
    for (i = 0; i < 9; i++) if (i != 4) tab[i] = 2; else tab[i] = 3;
}

void setCross(Byte *tab) {
    int i;
    for (i = 0; i < 9; i++) if (i % 2 == 0) tab[i] = 1; else tab[i] = 3;
}