
#include <stdio.h>

#include "menu.h"
#include "consts.h"

void printOption();

void mainMenu() {
    printf("Escolhe uma das seguintes opcoes:\n");
    printOption(OPT_SAIR, "Sair");
    printOption(OPT_NORMAL, "Jogo normal");
    printOption(OPT_AI, "Jogo contra AI");
    printOption(OPT_OLD, "Ver jogo antigo");
}

void loadBin() {
    printf("Foi encontrado um jogo guardado!\nDeseja retomar?\n");
    printOption(OPT_SIM, "Sim");
    printOption(OPT_NAO, "Nao");
}

void saveMenu() {
    printf("Queres guardar o jogo?\n");
    printOption(OPT_SIM, "Sim");
    printOption(OPT_NAO, "Nao");
}

void printOption(int n, char* option) {
    printf("%d -> %s\n", n, option);
}