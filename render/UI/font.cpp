#include "font.hpp"


Font* create_font(Texture* tex, int char_size_W, int char_size_H, int qnt_colunas, int qnt_linhas){
    Font* font = (Font*)malloc(sizeof(Font));
    if(!font){return NULL;} //Erro na Alocação
    font->character_count = qnt_colunas*qnt_linhas;
    font->textura = tex;
    font->caracteres = (sub_texture**)malloc(qnt_colunas*qnt_linhas*sizeof(sub_texture*));
    if(!font->caracteres){return NULL;} //Erro na Alocação
    //cria cada font
    for(int i = 0; i < qnt_linhas; i++){
        for(int j = 0; j < qnt_colunas; j++){
            font->caracteres[j + (i * qnt_colunas)] = create_sub_texture(font->textura, j*char_size_W, i*char_size_H, char_size_W, char_size_H);
        }
    }
    return font;
}

void destroy_Font(Font* fonte){
    free(fonte->textura);
    for(int i = 0; i < fonte->character_count; i++){
        free(fonte->caracteres[i]);
    }
    free(fonte->caracteres);
}

int char_pos(char c){
   int pos = -1;
   int offset = 0;
   //--------------------------------------------------------------
   /*0 - 9*/
   if(c >= 48 && c <= 57){
    pos = c - 48;
    return pos;
   }
   offset += (57 - 48)+1;
   //--------------------------------------------------------------
   /*A - Z*/
   if(c >= 65 && c <= 90){
    pos = (c - 65); //coloca de 0 - 25, avança [0 - 9]
    return pos+offset;
   }
   offset += (90 - 65)+1;
   //--------------------------------------------------------------
   /*a - z*/
   if(c >= 97 && c <= 122){
    pos = (c - 97); //coloca de 0 - 25, avança ([0 - 9] + [A - Z])
    return pos + offset;
   }
   offset += (122 - 97)+1;
   //--------------------------------------------------------------
   /* ! - / */
   if(c >= 33 && c <= 47){
    pos = (c - 33); //coloca de 0 - 25, avança ([0 - 9] + [A - Z])
    return pos + offset;
   }
   offset += (47 - 33);
   //--------------------------------------------------------------
   /* : - ? */
   if(c >= 58 && c <= 63){
    pos = (c - 58); //coloca de 0 - 25, avança ([0 - 9] + [A - Z])
    return pos + offset;
   }
   offset += (63 - 58)+1;
   //--------------------------------------------------------------
   /* [ - _ */
   if(c >= 91 && c <= 95){
    pos = (c - 91); //coloca de 0 - 25, avança ([0 - 9] + [A - Z])
    return pos + offset;
   }
   offset += (95 - 91)+1;
   //--------------------------------------------------------------
   /* { - ~ */
   if(c >= 123 && c <= 126){
    pos = (c - 123); //coloca de 0 - 25, avança ([0 - 9] + [A - Z])
    return pos + offset;
   }
   offset += (126 - 123)+1;
   //--------------------------------------------------------------
   // @
   if(c == 64){
    pos = (c - 64);
    return pos+offset;
   }
   offset++;
   //--------------------------------------------------------------
   // `
   if(c == 96){
    pos = (c - 96);
    return pos+offset;
   }
   //Não está definido
   return pos;
}

int qnt_digitos(int i){
    int valor = i;
    if(valor < 0){valor *= -1;}
    int contador = 0;
    while(valor >= 1){
        valor = valor / 10;
        contador++;
    }
    if(contador == 0){return 1;}
    else{
        return contador;
    }
}

char* ParaString(int i){
    int qnt_d = qnt_digitos(i);
    int valor = i;
    if(valor < 0){
        valor *= -1;
        qnt_d++;
    }
    char* saida = (char*)malloc((qnt_d+1)*sizeof(char));
    int indice = qnt_d - 1;
    while(valor >= 1){
        int atual = valor % 10;
        valor = valor / 10;
        char v = 48 + atual;
        saida[indice] = v;
        indice--;
    }
    if(i == 0){
        saida[0] = '0';
    }
    if(i < 0){
        saida[0] = '-';
    }
    saida[qnt_d] = '\0';
    return saida;
}

void draw_caracter(Font* font, char caractere, unsigned int cor, int x, int y, int w, int h){
    static Vertex v[2];
    int pos = char_pos(caractere);
    if(pos == -1){return;}

    v[0].u = font->caracteres[pos]->x;//texture->x;
    v[0].v = font->caracteres[pos]->y;
    v[0].cor = cor;
    v[0].x = x;
    v[0].y = y;
    v[0].z = 0.0f;

    v[1].u = font->caracteres[pos]->x + font->caracteres[pos]->w;
    v[1].v = font->caracteres[pos]->y + font->caracteres[pos]->h;
    v[1].cor = cor;
    v[1].x = x + w;
    v[1].y = y + h;
    v[1].z = 0.0f;

    bind_texture(font->textura, GU_TFX_MODULATE);
    sceGuDrawArray(GU_SPRITES, GU_COLOR_8888 | GU_TEXTURE_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 2, 0, v);
    sceGuDisable(GU_TEXTURE_2D);
}

void draw_string(Font* font, const char* str, int count, unsigned int cor, int x, int y, int w, int h){
    int linha = 0;
    int coluna = 0;
    for(int i = 0; i < count; i++){
        if(str[i] != '\n'){
            draw_caracter(font, str[i], cor, x + (w*coluna), y + (h*linha), w, h);
            coluna++;
        }else{
            linha++;
            coluna = 0;
        }
    }
}