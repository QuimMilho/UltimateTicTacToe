#include <stdio.h>

#include "libs/utils.h"

#include "jogo/jogo.h"

#include "componentes/menu.h"
#include "componentes/consts.h"

#include "ficheiros/files.h"

int main() {
    int opt = 0, err = 0;

    //Inicializa a componente random do trabalho
    initRandom();

    //Verifica se há algum ficheiro de jogo antigo
    if (fExists(TEMP_FILE, "rb")) {
        do {
            loadBin();
            if (scanf("%d", &opt)) {
                switch (opt) {
                    case OPT_SIM:
                        err = jogarJogo(RELOAD);
                        break;
                    case OPT_NAO:
                        remove(TEMP_FILE);
                        break;
                    default:
                        printf("Tens de inserir um valor valido!\n");
                }
            } else {
                printf("Tens de inserir um valor valido!\n");
            }
        } while (opt != OPT_SIM && opt != OPT_NAO);
    }

    //Menu principal
    do {
        mainMenu();
        if (scanf("%d", &opt)) {
            switch (opt) {
                case OPT_SAIR:
                    printf("Adeus!\n");
                    break;
                case OPT_NORMAL:
                    jogarJogo(JOGO_NORMAL);
                    break;
                case OPT_AI:
                    jogarJogo(JOGO_AI);
                    break;
                case OPT_OLD:
                    jogarJogo(RELOAD);
                    break;
                default:
                    printf("Tens de inserir um valor valido!\n");
            }
        } else {
            printf("Tens de inserir um valor valido!\n");
        }
    } while(opt != OPT_SAIR);

    return 0;
}
