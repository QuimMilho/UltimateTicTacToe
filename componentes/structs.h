
#ifndef TRABALHOCAMILO_STRUCTS_H
#define TRABALHOCAMILO_STRUCTS_H

//Definir 1 numero entre 0 e 255 para poupar memória
typedef unsigned char Byte;

//Tabuleiro
typedef struct tabuleiro Tabuleiro;

struct tabuleiro {
    Byte *win;
    Byte *tabs;
};

//Jogadas
typedef struct jogadas Jogadas;

struct jogadas {
    //Tab
    Byte tab;
    //Pos
    Byte pos;
    //Numero da jogada
    short n;
    //Lista ligada
    Jogadas *next;
    Jogadas *prev;
};

//Jogo
typedef struct jogo Jogo;

struct jogo {
    //Tabuleiro
    Tabuleiro tab;
    Byte nextTab;
    //Modo de jogo
    Byte gamemode;
    //Jogadas
    Jogadas *jogadas;
    Jogadas *lastPlay;
    short nJogadas;
}; //219 176

#endif //TRABALHOCAMILO_STRUCTS_H
