#pragma once
#ifndef MY_PSP_FONT
#define MY_PSP_FONT

#include "../renderer.hpp"


struct Font{
    int character_count;
    Texture* textura;
    sub_texture** caracteres;
};


Font* create_font(Texture* tex, int char_size_W, int char_size_H, int qnt_colunas, int qnt_linhas);

void destroy_Font(Font* fonte);

int char_pos(char c);

int qnt_digitos(int i);

char* ParaString(int i);

void draw_caracter(Font* font, char caractere, unsigned int cor, int x, int y, int w = 0, int h = 0);

void draw_string(Font* font, const char* str, int count, unsigned int cor, int x, int y, int w = 0, int h = 0);

#endif