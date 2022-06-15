#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <math.h>

// fps do jogo
const float FPS = 100; 

// variaveis de tamanho de tela
const int SCREEN_W = 960;
const int SCREEN_H = 540;

// variaveis das bordas do jogo e largura de obstaculos retangulades
const int SPACE_ABOVE = 50;
const int WIDTH_REC = 20;

// variaveis de velocidade dos tiros e tanks
const float VELOCIDADE_TANK = 1;
const float VELOCIDADE_TIRO = 4;
const float VENTO = 0.3;

// variavel auxiliar de gravidade, que indica a velocidade da queda do tiro tipo 1
const float GRAVIDADE = 0.1;

// variaveis de configuracoes do bitmap dos tanks
const float TANK_W = 60;
const float TANK_H = 60;
const float RAIO_TANK = 20 * pow(2.0, 1.0 / 2.0);

// variaveis de tempo de recarda e tempo para explosao do tiro do tank canhao
const int CD = 1;
const int CD_EXPLOSAO = 1;

// raio de explosao do tiro do tank canhao
const int RAIO_EXPLOSAO = 30;

// obstaculos que foram criados para os mapas
const float BORDAS[4][4] = {{0, SPACE_ABOVE, WIDTH_REC, SCREEN_H},{0, SPACE_ABOVE, SCREEN_W, SPACE_ABOVE + WIDTH_REC},{SCREEN_W - WIDTH_REC, SPACE_ABOVE, SCREEN_W, SCREEN_H},{0, SCREEN_H - WIDTH_REC, SCREEN_W, SCREEN_H}};
const float MEIO[2][4] = {{SCREEN_W / 2 - WIDTH_REC / 2, SPACE_ABOVE + 2.0 * (SCREEN_H - SPACE_ABOVE) / 6.0, SCREEN_W / 2 + WIDTH_REC / 2, SPACE_ABOVE + 4.0 * (SCREEN_H - SPACE_ABOVE) / 6.0},{2.0 * SCREEN_W / 6.0, SPACE_ABOVE + (SCREEN_H - SPACE_ABOVE) / 2 - WIDTH_REC / 2, 4.0 * SCREEN_W / 6.0, SPACE_ABOVE + (SCREEN_H - SPACE_ABOVE) / 2 + WIDTH_REC / 2}};
const float CANTOS[4][4] = {{SCREEN_W / 8.0, SPACE_ABOVE, SCREEN_W / 8.0 + WIDTH_REC, SPACE_ABOVE + 1 * (SCREEN_H - SPACE_ABOVE) / 4.0},{SCREEN_W / 8.0, SPACE_ABOVE + 3 * (SCREEN_H - SPACE_ABOVE) / 4.0, SCREEN_W / 8.0 + WIDTH_REC, SCREEN_H},{7 * SCREEN_W / 8.0 - WIDTH_REC, SPACE_ABOVE, 7 * SCREEN_W / 8.0, SPACE_ABOVE + 1 * (SCREEN_H - SPACE_ABOVE) / 4.0},{7 * SCREEN_W / 8.0 - WIDTH_REC, SPACE_ABOVE + 3 * (SCREEN_H - SPACE_ABOVE) / 4.0, 7 * SCREEN_W / 8.0, SCREEN_H}};
const float CIRCULO[1][3] = {{SCREEN_W / 2.0, SPACE_ABOVE + (SCREEN_H - SPACE_ABOVE) / 2.0, (SCREEN_H - SPACE_ABOVE) / 8.0}};
const float REC[4][4] = {{2 * SCREEN_W / 8.0, SPACE_ABOVE + SCREEN_H / 4.0 - WIDTH_REC, 4 * SCREEN_W / 9.0, SPACE_ABOVE + SCREEN_H / 4.0},{5 * SCREEN_W / 9.0, SPACE_ABOVE + SCREEN_H / 4.0 - WIDTH_REC, 6 * SCREEN_W / 8.0, SPACE_ABOVE + SCREEN_H / 4.0},{2 * SCREEN_W / 8.0,  SPACE_ABOVE + 3 * (SCREEN_H - SPACE_ABOVE) / 4.0, 4 * SCREEN_W / 9.0, SPACE_ABOVE + 3 * (SCREEN_H - SPACE_ABOVE) / 4.0 + WIDTH_REC},{5 * SCREEN_W / 9.0,  SPACE_ABOVE + 3 * (SCREEN_H - SPACE_ABOVE) / 4.0, 6 * SCREEN_W / 8.0, SPACE_ABOVE + 3 * (SCREEN_H - SPACE_ABOVE) / 4.0 + WIDTH_REC}};
const float BOLAS[4][3] = {{3 * SCREEN_W / 9.0, SPACE_ABOVE + 1 * (SCREEN_H - SPACE_ABOVE) / 4.0, 20},{6 * SCREEN_W / 9.0, SPACE_ABOVE + 1 * (SCREEN_H - SPACE_ABOVE) / 4.0, 20},{3 * SCREEN_W / 9.0, SPACE_ABOVE + 3 * (SCREEN_H - SPACE_ABOVE) / 4.0, 20},{6 * SCREEN_W / 9.0, SPACE_ABOVE + 3 * (SCREEN_H - SPACE_ABOVE) / 4.0, 20}};

// cria um novo tipo de variaveel: botao
typedef struct Bot {
	float x, y, height, width;
}Bot;

// cria um novo tipo de variaveel: skin
typedef struct Skin {
	float x, y, raio;
}Skin;

// cria um novo tipo de variaveel: tank
typedef struct Tank {
	float x,y;
	float sen, cos;
	int up, down;
	int canUp, canDown;
	float degree;
	int left, right;
	int pontos;
	ALLEGRO_COLOR cor;
} Tank;

// cria um novo tipo de variaveel: tiro
typedef struct Shot {
	float x,y;
	float width, heigth, raio; 
	float sen, cos;
	float degree;
	int shoot;
	int run;
	float tempo;
	int tipo;
	float nextx, nexty;
	float gravidade;
	int explosao;
	ALLEGRO_COLOR cor;
}Shot;

// funcao que retorna um valor aleaotiro entre 0 e 4
int random()
{
	return rand() % 5;
}

// funcao que desenha o cenario caso o jogador escolha o cenario 0, correspondente a dificuldade facil
void draw_scenario0(ALLEGRO_DISPLAY *display)
{
	al_set_target_bitmap(al_get_backbuffer(display));
	al_clear_to_color(al_map_rgb(205, 133, 63));
	al_draw_filled_rectangle(BORDAS[0][0], BORDAS[0][1], BORDAS[0][2], BORDAS[0][3], al_map_rgb(244, 164, 96));
	al_draw_filled_rectangle(BORDAS[1][0], BORDAS[1][1], BORDAS[1][2], BORDAS[1][3], al_map_rgb(244, 164, 96));
	al_draw_filled_rectangle(BORDAS[2][0], BORDAS[2][1], BORDAS[2][2], BORDAS[2][3], al_map_rgb(244, 164, 96));
	al_draw_filled_rectangle(BORDAS[3][0], BORDAS[3][1], BORDAS[3][2], BORDAS[3][3], al_map_rgb(244, 164, 96));
	al_draw_filled_rectangle(MEIO[0][0], MEIO[0][1], MEIO[0][2], MEIO[0][3], al_map_rgb(244, 164, 96));
	al_draw_filled_rectangle(MEIO[1][0], MEIO[1][1], MEIO[1][2], MEIO[1][3], al_map_rgb(244, 164, 96));
}

// funcao que desenha o cenario caso o jogador escolha o cenario 0, correspondente a dificuldade media
void draw_scenario1(ALLEGRO_DISPLAY *display)
{
	al_set_target_bitmap(al_get_backbuffer(display));
	al_clear_to_color(al_map_rgb(65,105,225));
	al_draw_filled_rectangle(BORDAS[0][0], BORDAS[0][1], BORDAS[0][2], BORDAS[0][3], al_map_rgb(100,149,237));
	al_draw_filled_rectangle(BORDAS[1][0], BORDAS[1][1], BORDAS[1][2], BORDAS[1][3], al_map_rgb(100,149,237));
	al_draw_filled_rectangle(BORDAS[2][0], BORDAS[2][1], BORDAS[2][2], BORDAS[2][3], al_map_rgb(100,149,237));
	al_draw_filled_rectangle(BORDAS[3][0], BORDAS[3][1], BORDAS[3][2], BORDAS[3][3], al_map_rgb(100,149,237));
	al_draw_filled_rectangle(MEIO[0][0], MEIO[0][1], MEIO[0][2], MEIO[0][3], al_map_rgb(100,149,237));
	al_draw_filled_rectangle(MEIO[1][0], MEIO[1][1], MEIO[1][2], MEIO[1][3], al_map_rgb(100,149,237));
	al_draw_filled_circle(BOLAS[0][0], BOLAS[0][1], BOLAS[0][2], al_map_rgb(100,149,237));
	al_draw_filled_circle(BOLAS[1][0], BOLAS[1][1], BOLAS[1][2], al_map_rgb(100,149,237));
	al_draw_filled_circle(BOLAS[2][0], BOLAS[2][1], BOLAS[2][2], al_map_rgb(100,149,237));
	al_draw_filled_circle(BOLAS[3][0], BOLAS[3][1], BOLAS[3][2], al_map_rgb(100,149,237));
	al_draw_filled_rectangle(CANTOS[0][0], CANTOS[0][1], CANTOS[0][2], CANTOS[0][3], al_map_rgb(100,149,237));
	al_draw_filled_rectangle(CANTOS[1][0], CANTOS[1][1], CANTOS[1][2], CANTOS[1][3], al_map_rgb(100,149,237));
	al_draw_filled_rectangle(CANTOS[2][0], CANTOS[2][1], CANTOS[2][2], CANTOS[2][3], al_map_rgb(100,149,237));
	al_draw_filled_rectangle(CANTOS[3][0], CANTOS[3][1], CANTOS[3][2], CANTOS[3][3], al_map_rgb(100,149,237));
}

// funcao que desenha o cenario caso o jogador escolha o cenario 0, correspondente a dificuldade dificil
void draw_scenario2(ALLEGRO_DISPLAY *display)
{
	al_set_target_bitmap(al_get_backbuffer(display));
	al_clear_to_color(al_map_rgb(218,112,214));
	al_draw_filled_rectangle(BORDAS[0][0], BORDAS[0][1], BORDAS[0][2], BORDAS[0][3], al_map_rgb(255,218,185));
	al_draw_filled_rectangle(BORDAS[1][0], BORDAS[1][1], BORDAS[1][2], BORDAS[1][3], al_map_rgb(255,218,185));
	al_draw_filled_rectangle(BORDAS[2][0], BORDAS[2][1], BORDAS[2][2], BORDAS[2][3], al_map_rgb(255,218,185));
	al_draw_filled_rectangle(BORDAS[3][0], BORDAS[3][1], BORDAS[3][2], BORDAS[3][3], al_map_rgb(255,218,185));
	al_draw_filled_rectangle(REC[0][0], REC[0][1], REC[0][2], REC[0][3], al_map_rgb(255,218,185));
	al_draw_filled_rectangle(REC[1][0], REC[1][1], REC[1][2], REC[1][3], al_map_rgb(255,218,185));
	al_draw_filled_rectangle(REC[2][0], REC[2][1], REC[2][2], REC[2][3], al_map_rgb(255,218,185));
	al_draw_filled_rectangle(REC[3][0], REC[3][1], REC[3][2], REC[3][3], al_map_rgb(255,218,185));
	al_draw_filled_circle(CIRCULO[0][0], CIRCULO[0][1], CIRCULO[0][2], al_map_rgb(255,218,185));
	al_draw_filled_rectangle(CANTOS[0][0], CANTOS[0][1], CANTOS[0][2], CANTOS[0][3], al_map_rgb(255,218,185));
	al_draw_filled_rectangle(CANTOS[1][0], CANTOS[1][1], CANTOS[1][2], CANTOS[1][3], al_map_rgb(255,218,185));
	al_draw_filled_rectangle(CANTOS[2][0], CANTOS[2][1], CANTOS[2][2], CANTOS[2][3], al_map_rgb(255,218,185));
	al_draw_filled_rectangle(CANTOS[3][0], CANTOS[3][1], CANTOS[3][2], CANTOS[3][3], al_map_rgb(255,218,185));
}

// funcao que inicia os botoes com suas configuracoes de altura e largura
void initBot(Bot *bot, ALLEGRO_BITMAP *bitmap)
{
	(*bot).width = al_get_bitmap_width(bitmap);
	(*bot).height = al_get_bitmap_height(bitmap);
	(*bot).x = 0;
	(*bot).y = 0;
}

// funcao que inicia as skins com suas configuracoes de localizacao na tela
void initSkins(Skin *skinv1, Skin *skinv2, Skin *skinv3)
{
	(*skinv1).x = SCREEN_W / 2.0;
	(*skinv2).x = SCREEN_W / 2.0;
	(*skinv3).x = SCREEN_W / 2.0;
	(*skinv1).y = SCREEN_H * 2 / 6.0;
	(*skinv2).y = SCREEN_H * 3 / 6.0;
	(*skinv3).y = SCREEN_H * 4 / 6.0;
	(*skinv1).raio = 40 * pow(2.0, 1.0 / 2.0);
	(*skinv2).raio = 40 * pow(2.0, 1.0 / 2.0);
	(*skinv3).raio = 40 * pow(2.0, 1.0 / 2.0);
}

// funcao que inicia os botoes com suas configuracoes de localizacao na tela
void initBots(Bot *playb, Bot *diffb, Bot *playagb, Bot *menub, Bot *backb, Bot *gameoverb, Bot *titleb, Bot *optionsb, Bot *volumeb, Bot *setab)
{
	(*playb).x = SCREEN_W / 2 - (*playb).width / 2;
	(*playb).y = SCREEN_H / 3;
	(*diffb).x = SCREEN_W / 2 - (*diffb).width / 2;
	(*diffb).y = (*playb).y + (*playb).height;
	(*playagb).x = SCREEN_W / 2 - (*playagb).width / 2;
	(*playagb).y = SCREEN_H / 2 + 60;
	(*menub).x = SCREEN_W / 2 - (*menub).width / 2;
	(*menub).y = (*playagb).y + (*playagb).height;
	(*backb).x = 0;
	(*backb).y = SCREEN_H - (*backb).height;
	(*gameoverb).x = SCREEN_W / 2 - (*gameoverb).width / 2;
	(*gameoverb).y = 0;
	(*titleb).x = SCREEN_W / 2 - (*titleb).width / 2;
	(*titleb).y = 20;
	(*optionsb).x = SCREEN_W / 2 - (*optionsb).width / 2;
	(*optionsb).y = (*diffb).y + (*diffb).height;
	(*volumeb).x = SCREEN_W - (*volumeb).width - 10;
	(*volumeb).y = SCREEN_H - (*volumeb).height - 10;
	(*setab).x = (*volumeb).x + 25;
	(*setab).y = (*volumeb).y + 77;
}

// funcao que inicia as configuracoes iniciais do tank
void initTanks(Tank *tank1, Tank *tank2)
{
	(*tank1).x = WIDTH_REC + 20 + TANK_W / 2;
	(*tank1).y = SPACE_ABOVE + ((SCREEN_H - SPACE_ABOVE)/ 2);
	(*tank2).x = SCREEN_W - WIDTH_REC - 20 - TANK_W / 2;
	(*tank2).y = SPACE_ABOVE + ((SCREEN_H - SPACE_ABOVE)/ 2);
	(*tank1).up = 0;
	(*tank1).down = 0;
	(*tank2).up = 0;
	(*tank2).down = 0;
	(*tank1).left = 0;
	(*tank2).left = 0;
	(*tank1).right = 0;
	(*tank2).right = 0;
	(*tank1).degree = 0;
	(*tank2).degree = 180;
	(*tank1).sen = 0;
	(*tank2).sen = 0;
	(*tank1).cos = 0;
	(*tank2).cos = 0;
	(*tank1).canUp = 0;
	(*tank2).canUp = 0;
	(*tank1).canDown = 0;
	(*tank2).canDown = 0;
	(*tank1).pontos = 0;
	(*tank2).pontos = 0;
}

// funcao que inicia as configuracoes iniciais do tiro
void initTiros1(Shot *shot1, Shot *shot2)
{
	(*shot1).shoot = 0;
	(*shot2).shoot = 0;
	(*shot1).run = 0;
	(*shot2).run = 0;
	(*shot1).tempo = -2 * CD;
	(*shot2).tempo = -2 * CD;
	(*shot1).tipo = 0;
	(*shot2).tipo = 0;
	(*shot1).gravidade = GRAVIDADE;
	(*shot2).gravidade = GRAVIDADE;
	(*shot1).width = 20;
	(*shot2).width = 20;
	(*shot1).heigth = 20;
	(*shot2).heigth = 20;
}

// funcao que inicia as configuracoes iniciais do tiro quando uum tank atirar
void initTiros(Shot *shot1, Shot *shot2, Tank *tank1, Tank *tank2, float tempo, int *som1, int *som2)
{
	(*shot1).cor = al_map_rgb(0, 255, 0);
	if((*shot1).shoot == 1 && (*shot1).run == 0 && tempo - (*shot1).tempo >= CD)
	{
		(*shot1).tempo = tempo;
		(*shot1).x = (*tank1).x + RAIO_TANK * (*tank1).cos + (*shot1).raio * (*tank1).cos;
		(*shot1).y = (*tank1).y + RAIO_TANK * (*tank1).sen + (*shot1).raio * (*tank1).sen;
		(*shot1).degree = (*tank1).degree;
		(*shot1).sen = (*tank1).sen;
		(*shot1).cos = (*tank1).cos;
		(*shot1).run = 1;
		(*som1) = 1;
	}
	(*shot2).cor = al_map_rgb(0, 191, 255);
	if((*shot2).shoot == 1 && (*shot2).run == 0 && tempo - (*shot2).tempo >= CD)
	{
		(*shot2).tempo = tempo;
		(*shot2).x = (*tank2).x + RAIO_TANK * (*tank2).cos + (*shot2).raio * (*tank2).cos;
		(*shot2).y = (*tank2).y + RAIO_TANK * (*tank2).sen + (*shot2).raio * (*tank2).sen;
		(*shot2).degree = (*tank2).degree;
		(*shot2).sen = (*tank2).sen;
		(*shot2).cos = (*tank2).cos;
		(*shot2).run = 1;
		(*som2) = 1;
	}
	
}

// funcao que indica os proximos movimentos do tiro em suas respectivas variaveis de proximo movimento, de acordo com o tipo de tank utilizado
void tipoTiro(Shot *shot)
{	
	if((*shot).run == 1)
	{
		if((*shot).tipo == 0)
		{
			(*shot).nextx = (*shot).cos * VELOCIDADE_TIRO;
			(*shot).nexty = (*shot).sen * VELOCIDADE_TIRO;
		}
		else if((*shot).tipo == 1)
		{
			(*shot).nextx = (*shot).cos * VELOCIDADE_TIRO * 1.5;
			(*shot).gravidade += GRAVIDADE;
			(*shot).nexty = (*shot).sen * VELOCIDADE_TIRO + (*shot).gravidade;
		}
		else if((*shot).tipo == 2)
		{
			(*shot).gravidade = ((*shot).gravidade <= -VELOCIDADE_TIRO)? -4 : (*shot).gravidade - GRAVIDADE/3;
			(*shot).nextx = (*shot).cos * VELOCIDADE_TIRO + (*shot).cos * (*shot).gravidade;
			(*shot).nexty = (*shot).sen * VELOCIDADE_TIRO + (*shot).sen * (*shot).gravidade;
		}
	}
	else
	{
		(*shot).gravidade = 0;
	}
}

// funcao que desenha os bitmaps dos tanks de acordo com o escolhido pelo usuario
void draw_tanks(ALLEGRO_BITMAP *t1, ALLEGRO_BITMAP *t2, Tank *tank1, Tank *tank2, Shot *shot1, Shot *shot2, int cenario)
{
	if((*shot1).tipo == 0)
	{
		al_set_target_bitmap(t1);

		al_draw_filled_rectangle(10, 10, 45, 25, al_map_rgb(0, 100, 0));
		al_draw_filled_rectangle(20, 25, 35, 35, al_map_rgb(0, 100, 0));
		al_draw_filled_rectangle(35, 28, 58, 32, al_map_rgb(0, 100, 0));
		al_draw_filled_rectangle(10, 35, 45, 50, al_map_rgb(0, 100, 0));
		al_draw_circle(30, 30, 20 * pow(2.0, 1.0 / 2.0), al_map_rgb(255, 255, 255), 2);
		
		al_set_target_bitmap(t2);

		al_draw_filled_rectangle(10, 10, 45, 25, al_map_rgb(25, 25, 112));
		al_draw_filled_rectangle(20, 25, 35, 35, al_map_rgb(25, 25, 112));
		al_draw_filled_rectangle(35, 28, 58, 32, al_map_rgb(25, 25, 112));
		al_draw_filled_rectangle(10, 35, 45, 50, al_map_rgb(25, 25, 112));
		al_draw_circle(30, 30, 20 * pow(2.0, 1.0 / 2.0), al_map_rgb(255, 255, 255), 2);
		
		(*tank1).cor = al_map_rgb(0, 100, 0);
		(*tank2).cor = al_map_rgb(25, 25, 112);
		(*shot1).cor = al_map_rgb(0, 255, 0);
		(*shot2).cor = al_map_rgb(25, 25, 255);
	}
	
	else if((*shot1).tipo == 1)
	{
		al_set_target_bitmap(t1);

		al_draw_filled_rectangle(8, 23, 50, 37, al_map_rgb(178,34,34));
		al_draw_filled_rectangle(33, 15, 42, 25, al_map_rgb(178,34,34));
		al_draw_filled_rectangle(33, 35, 42, 45, al_map_rgb(178,34,34));
		al_draw_filled_rectangle(25, 10, 50, 15, al_map_rgb(178,34,34));
		al_draw_filled_rectangle(25, 45, 50, 50, al_map_rgb(178,34,34));
		al_draw_filled_rectangle(8, 15, 17, 37, al_map_rgb(178,34,34));
		al_draw_filled_rectangle(50, 28, 53, 32, al_map_rgb(178,34,34));
		al_draw_filled_rectangle(53, 21, 56, 39, al_map_rgb(178,34,34));
		al_draw_circle(30, 30, 20 * pow(2.0, 1.0 / 2.0), al_map_rgb(255, 255, 255), 2);
		
		al_set_target_bitmap(t2);

		al_draw_filled_rectangle(8, 23, 50, 37, al_map_rgb(34,139,34));
		al_draw_filled_rectangle(33, 15, 42, 25, al_map_rgb(34,139,34));
		al_draw_filled_rectangle(33, 35, 42, 45, al_map_rgb(34,139,34));
		al_draw_filled_rectangle(25, 10, 50, 15, al_map_rgb(34,139,34));
		al_draw_filled_rectangle(25, 45, 50, 50, al_map_rgb(34,139,34));
		al_draw_filled_rectangle(8, 15, 17, 37, al_map_rgb(34,139,34));
		al_draw_filled_rectangle(50, 28, 53, 32, al_map_rgb(34,139,34));
		al_draw_filled_rectangle(53, 21, 56, 39, al_map_rgb(34,139,34));
		al_draw_circle(30, 30, 20 * pow(2.0, 1.0 / 2.0), al_map_rgb(255, 255, 255), 2);
		
		(*tank1).cor = al_map_rgb(178,34,34);
		(*tank2).cor = al_map_rgb(34,139,34);
		(*shot1).cor = al_map_rgb(255, 0, 0);
		(*shot2).cor = al_map_rgb(34, 255, 34);
	}
	
	else if((*shot1).tipo == 2)
	{
		al_set_target_bitmap(t1);

		al_draw_filled_rectangle(8, 20, 54, 40, al_map_rgb(47,79,79));
		al_draw_filled_rectangle(15, 40, 35, 45, al_map_rgb(139,69,19));
		al_draw_filled_rectangle(15, 15, 35, 20, al_map_rgb(139,69,19));
		al_draw_circle(30, 30, 20 * pow(2.0, 1.0 / 2.0), al_map_rgb(255, 255, 255), 2);
		
		al_set_target_bitmap(t2);

		al_draw_filled_rectangle(8, 20, 54, 40, al_map_rgb(139,0,139));
		al_draw_filled_rectangle(15, 40, 35, 45, al_map_rgb(139,69,19));
		al_draw_filled_rectangle(15, 15, 35, 20, al_map_rgb(139,69,19));
		al_draw_circle(30, 30, 20 * pow(2.0, 1.0 / 2.0), al_map_rgb(255, 255, 255), 2);
		
		(*tank1).cor = al_map_rgb(47,79,79);
		(*tank2).cor = al_map_rgb(139,0,139);
		(*shot1).cor = al_map_rgb(0, 200, 200);
		(*shot2).cor = al_map_rgb(255, 30, 150);
	}
}

// funcao que desenha os bitmaps das skins possiveis de tanks
void draw_skins(ALLEGRO_BITMAP *s1, ALLEGRO_BITMAP *s2, ALLEGRO_BITMAP *s3)
{
	al_set_target_bitmap(s1);
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_filled_rectangle(10, 10, 45, 25, al_map_rgb(0, 100, 0));
	al_draw_filled_rectangle(20, 25, 35, 35, al_map_rgb(0, 100, 0));
	al_draw_filled_rectangle(35, 28, 58, 32, al_map_rgb(0, 100, 0));
	al_draw_filled_rectangle(10, 35, 45, 50, al_map_rgb(0, 100, 0));
	al_draw_circle(30, 30, 20 * pow(2.0, 1.0 / 2.0), al_map_rgb(255, 255, 255), 2);
	
	al_set_target_bitmap(s2);
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_filled_rectangle(8, 23, 50, 37, al_map_rgb(178,34,34));
	al_draw_filled_rectangle(33, 15, 42, 25, al_map_rgb(178,34,34));
	al_draw_filled_rectangle(33, 35, 42, 45, al_map_rgb(178,34,34));
	al_draw_filled_rectangle(25, 10, 50, 15, al_map_rgb(178,34,34));
	al_draw_filled_rectangle(25, 45, 50, 50, al_map_rgb(178,34,34));
	al_draw_filled_rectangle(8, 15, 17, 37, al_map_rgb(178,34,34));
	al_draw_filled_rectangle(50, 28, 53, 32, al_map_rgb(178,34,34));
	al_draw_filled_rectangle(53, 21, 56, 39, al_map_rgb(178,34,34));
	al_draw_circle(30, 30, 20 * pow(2.0, 1.0 / 2.0), al_map_rgb(255, 255, 255), 2);
	
	al_set_target_bitmap(s3);
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_filled_rectangle(8, 20, 54, 40, al_map_rgb(47,79,79));
	al_draw_filled_rectangle(15, 40, 35, 45, al_map_rgb(139,69,19));
	al_draw_filled_rectangle(15, 15, 35, 20, al_map_rgb(139,69,19));
	al_draw_circle(30, 30, 20 * pow(2.0, 1.0 / 2.0), al_map_rgb(255, 255, 255), 2);
}

// funcao que desenha os bitmaps dos tiros de acordo com o escolhido pelo usuario
void draw_shots(ALLEGRO_BITMAP *s1, ALLEGRO_BITMAP *s2, Shot *shot1, Shot *shot2)
{
	if((*shot1).tipo == 0)
	{
		al_set_target_bitmap(s1);
		
		al_draw_filled_circle(4, 4, 2.5 * pow(2.0, 1.0 / 2.0), (*shot1).cor);
		
		al_set_target_bitmap(s2);
		al_draw_filled_circle(4, 4, 2.5 * pow(2.0, 1.0 / 2.0), (*shot2).cor);
	}
	else if((*shot1).tipo == 1)
	{
		al_set_target_bitmap(s1);
		al_draw_filled_circle(8, 8, 8, (*shot1).cor);
		
		al_set_target_bitmap(s2);
		al_draw_filled_circle(8, 8, 8, (*shot2).cor);
	}
	else if((*shot1).tipo == 2)
	{
		al_set_target_bitmap(s1);
		al_draw_filled_circle(6, 6, 6, (*shot1).cor);
		
		al_set_target_bitmap(s2);
		al_draw_filled_circle(6, 6, 6, (*shot2).cor);
	}
}

// funcao que indica as configuracoes de raio, altura e largura do tiro de acordo com o escolhido pelo usuario
void draw_shots1(Shot *shot1, Shot *shot2)
{
	if((*shot1).tipo == 0)
	{
		(*shot1).raio = 2.5 * pow(2.0, 1.0 / 2.0);
		(*shot1).width = 8;
		(*shot1).heigth = 8;
		
		(*shot2).raio = 2.5 * pow(2.0, 1.0 / 2.0);
		(*shot2).width = 8;
		(*shot2).heigth = 8;
	}
	else if((*shot1).tipo == 1)
	{
		(*shot1).raio = 8;
		(*shot1).width = 16;
		(*shot1).heigth = 16;
		
		(*shot2).raio = 8;
		(*shot2).width = 16;
		(*shot2).heigth = 16;
	}
	else if((*shot1).tipo == 2)
	{
		(*shot1).raio = 6;
		(*shot1).width = 12;
		(*shot1).heigth = 12;
		
		(*shot2).raio = 6;
		(*shot2).width = 12;
		(*shot2).heigth = 12;
	}
}

// funcao que verifica o botao pressionado pelo usuario e indica o que fazer com isso
void verificaTeclaDown(Tank *tank1, Tank *tank2, Shot *shot1, Shot *shot2, ALLEGRO_EVENT ev, int *state)
{
	switch(ev.keyboard.keycode) 
	{
		case ALLEGRO_KEY_W:
		// movimento do tank para frente ligado
			(*tank1).up = 1;
		break;
		case ALLEGRO_KEY_S:
		// movimento do tank para tras desligado
			(*tank1).down = 1;
		break;
		case ALLEGRO_KEY_A:
		// movimento do giro do tank para esquerda ligado
			(*tank1).left = 1;
		break;
		case ALLEGRO_KEY_D:
		// movimento do giro do tank para direita ligado
			(*tank1).right = 1;
		break;
		case ALLEGRO_KEY_F:
		// caso nao haja um tiro acontecendo a variavel atirar do tiro eh ligada
			if((*shot1).run != 1) (*shot1).shoot = 1;
		break;
		case ALLEGRO_KEY_UP:
		// movimento do tank para frente ligado
			(*tank2).up = 1;
		break;
		case ALLEGRO_KEY_DOWN:
		// movimento do tank para tras desligado
			(*tank2).down = 1;
		break;
		case ALLEGRO_KEY_LEFT:
		// movimento do giro do tank para esquerda ligado
			(*tank2).left = 1;
		break;
		case ALLEGRO_KEY_RIGHT:
		// movimento do giro do tank para direita ligado
			(*tank2).right = 1;
		break;
		case ALLEGRO_KEY_L:
		// caso nao haja um tiro acontecendo a variavel atirar do tiro eh ligada
			if((*shot2).run != 1) (*shot2).shoot = 1;
		break;
		case ALLEGRO_KEY_P:
		// inicia o estado 2 do jogo : pausa
			(*state) = 2;
		break;
	}
}

// funcao que verifica o botao soltado pelo usuario e indica o que fazer com isso
void verificaTeclaUp(Tank *tank1, Tank *tank2, Shot *shot1, Shot *shot2, ALLEGRO_EVENT ev)
{
	switch(ev.keyboard.keycode) 
	{
		case ALLEGRO_KEY_W:
		// movimento do tank para frente desligado
			(*tank1).up = 0;
		break;
		case ALLEGRO_KEY_S:
		// movimento do tank para tras desligado
			(*tank1).down = 0;
		break;
		case ALLEGRO_KEY_A:
		// giro do tank para esquerda desligado
			(*tank1).left = 0;
		break;
		case ALLEGRO_KEY_D:
		// giro do tank para direita desligado
			(*tank1).right = 0;
		break;
		case ALLEGRO_KEY_F:
		// atirar desligado
			(*shot1).shoot = 0;
		break;
		case ALLEGRO_KEY_UP:
		// movimento do tank para frente desligado
			(*tank2).up = 0;
		break;
		case ALLEGRO_KEY_DOWN:
		// movimento do tank para tras desligado
			(*tank2).down = 0;
		break;
		case ALLEGRO_KEY_LEFT:
		// giro do tank para esquerda desligado
			(*tank2).left = 0;
		break;
		case ALLEGRO_KEY_RIGHT:
		// giro do tank para direita desligado
			(*tank2).right = 0;
		break;
		case ALLEGRO_KEY_L:
		// atirar desligado
			(*shot2).shoot = 0;
		break;
	}
}

// funcao que calcula os senos e cossenos do tank de acordo com seu giro
void calculaSenCos(Tank *tank1, Tank *tank2)
{
	(*tank1).sen = sin((*tank1).degree * M_PI / 180.0);
	(*tank1).cos = cos((*tank1).degree * M_PI / 180.0);
	(*tank2).sen = sin((*tank2).degree * M_PI / 180.0);
	(*tank2).cos = cos((*tank2).degree * M_PI / 180.0);
}

// funcao para o tank tipo canhao que verifica se houve colisao com a explosao
void verificaExplosao(Shot *shot, int raio, Tank *tank1, Tank *tank2)
{
	float a;
	float b;
	a = pow(pow(-(*tank1).x + (*shot).x, 2.0) + pow(-(*tank1).y + (*shot).y, 2.0), 1.0 / 2.0);
	b = pow(pow(-(*tank2).x + (*shot).x, 2.0) + pow(-(*tank2).y + (*shot).y, 2.0), 1.0 / 2.0);
	
	if(a >= RAIO_TANK + raio);
	else (*tank2).pontos ++;

	if(b >= RAIO_TANK + raio);
	else (*tank1).pontos ++;
}

// funcao que verifica colisao do tank com obstaculos movendo para frente
int verificaUp(Tank tank, float x1, float y1, float x2, float y2)
{
	float a, b, c, d;
	if(tank.up == 1 && ((tank.x < x1 && tank.y < y1) || (tank.x > x2 && tank.y > y2) || (tank.x < x1 && tank.y > y2) || (tank.x > x2 && tank.y < y1)))
	{
		a = pow(pow(-x1 + tank.x + tank.cos * VELOCIDADE_TANK, 2.0) + pow(-y1 + tank.y + tank.sen * VELOCIDADE_TANK, 2.0), 1.0 / 2.0);
		b = pow(pow(-x2 + tank.x + tank.cos * VELOCIDADE_TANK, 2.0) + pow(-y2 + tank.y + tank.sen * VELOCIDADE_TANK, 2.0), 1.0 / 2.0);
		c = pow(pow(-x1 + tank.x + tank.cos * VELOCIDADE_TANK, 2.0) + pow(-y2 + tank.y + tank.sen * VELOCIDADE_TANK, 2.0), 1.0 / 2.0);
		d = pow(pow(-x2 + tank.x + tank.cos * VELOCIDADE_TANK, 2.0) + pow(-y1 + tank.y + tank.sen * VELOCIDADE_TANK, 2.0), 1.0 / 2.0);
		
		if((tank.x < x1 && tank.y < y1) && (a >= RAIO_TANK))
		{
			return 1;
		}
		else if((tank.x > x2 && tank.y > y2) && (b >= RAIO_TANK))
		{
			return 1;
		}
		else if((tank.x < x1 && tank.y > y2) && (c >= RAIO_TANK))
		{
			return 1;
		}
		else if((tank.x > x2 && tank.y < y1) && (d >= RAIO_TANK))
		{
			return 1;
		}
		else return 0;
	}
	else if(tank.up == 1 && ((tank.x + tank.cos * VELOCIDADE_TANK + RAIO_TANK <= x1 || tank.y + tank.sen * VELOCIDADE_TANK + RAIO_TANK <= y1) || (tank.x + tank.cos * VELOCIDADE_TANK - RAIO_TANK >= x2 || tank.y + tank.sen * VELOCIDADE_TANK - RAIO_TANK >= y2)))
	{
		return 1;
	}
	return 0;
}

// funcao que verifica colisao do tank com obstaculos movendo para tras
int verificaDown(Tank tank, float x1, float y1, float x2, float y2)
{
	float a, b, c, d;
	if(tank.down == 1 && ((tank.x < x1 && tank.y < y1) || (tank.x > x2 && tank.y > y2) || (tank.x < x1 && tank.y > y2) || (tank.x > x2 && tank.y < y1)))
	{
		a = pow(pow(-x1 + tank.x - tank.cos * VELOCIDADE_TANK, 2.0) + pow(-y1 + tank.y - tank.sen * VELOCIDADE_TANK, 2.0), 1.0 / 2.0);
		b = pow(pow(-x2 + tank.x - tank.cos * VELOCIDADE_TANK, 2.0) + pow(-y2 + tank.y - tank.sen * VELOCIDADE_TANK, 2.0), 1.0 / 2.0);
		c = pow(pow(-x1 + tank.x - tank.cos * VELOCIDADE_TANK, 2.0) + pow(-y2 + tank.y - tank.sen * VELOCIDADE_TANK, 2.0), 1.0 / 2.0);
		d = pow(pow(-x2 + tank.x - tank.cos * VELOCIDADE_TANK, 2.0) + pow(-y1 + tank.y - tank.sen * VELOCIDADE_TANK, 2.0), 1.0 / 2.0);
		
		if((tank.x < x1 && tank.y < y1) && (a >= RAIO_TANK))
		{
			return 1;
		}
		else if((tank.x > x2 && tank.y > y2) && (b >= RAIO_TANK))
		{
			return 1;
		}
		else if((tank.x < x1 && tank.y > y2) && (c >= RAIO_TANK))
		{
			return 1;
		}
		else if((tank.x > x2 && tank.y < y1) && (d >= RAIO_TANK))
		{
			return 1;
		}
		else return 0;
	}
	if(tank.down == 1 && ((tank.x - tank.cos * VELOCIDADE_TANK + RAIO_TANK <= x1 || tank.y - tank.sen * VELOCIDADE_TANK + RAIO_TANK <= y1) || (tank.x - tank.cos * VELOCIDADE_TANK - RAIO_TANK >= x2 || tank.y - tank.sen * VELOCIDADE_TANK - RAIO_TANK >= y2)))
	{
		return 1;
	}
	return 0;
}

// funcao que verifica colisao do tank com circulos e com o outro tank, movendo para frente
int verificaTankUp(Tank tank, float x1, float y1, float raio)
{
	float a;
	if(tank.up == 1)
	{
		a = pow(pow(-x1 + tank.x + tank.cos * VELOCIDADE_TANK, 2.0) + pow(-y1 + tank.y + tank.sen * VELOCIDADE_TANK, 2.0), 1.0 / 2.0);
		
		if(a >= RAIO_TANK + raio)
		{
			return 1;
		}
	}
	return 0;
}

// funcao que verifica colisao do tank com circulos e com o outro tank, movendo para tras
int verificaTankDown(Tank tank, float x1, float y1, float raio)
{
	float a;
	if(tank.down == 1)
	{
		a = pow(pow(-x1 + tank.x - tank.cos * VELOCIDADE_TANK, 2.0) + pow(-y1 + tank.y - tank.sen * VELOCIDADE_TANK, 2.0), 1.0 / 2.0);
		
		if(a >= RAIO_TANK + raio)
		{
			return 1;
		}
	}
	return 0;
}

// funcao que verifica colisao de tiros
int verificaTiro(Shot shot, float x1, float y1, float x2, float y2)
{
	float a, b, c, d;
	if(shot.run == 1 && ((shot.x < x1 && shot.y < y1) || (shot.x > x2 && shot.y > y2) || (shot.x < x1 && shot.y > y2) || (shot.x > x2 && shot.y < y1)))
	{
		a = pow(pow(-x1 + shot.x + shot.nextx, 2.0) + pow(-y1 + shot.y + shot.nexty, 2.0), 1.0 / 2.0);
		b = pow(pow(-x2 + shot.x + shot.nextx, 2.0) + pow(-y2 + shot.y + shot.nexty, 2.0), 1.0 / 2.0);
		c = pow(pow(-x1 + shot.x + shot.nextx, 2.0) + pow(-y2 + shot.y + shot.nexty, 2.0), 1.0 / 2.0);
		d = pow(pow(-x2 + shot.x + shot.nextx, 2.0) + pow(-y1 + shot.y + shot.nexty, 2.0), 1.0 / 2.0);
		
		if((shot.x < x1 && shot.y < y1) && (a >= shot.raio))
		{
			return 1;
		}
		else if((shot.x > x2 && shot.y > y2) && (b >= shot.raio))
		{
			return 1;
		}
		else if((shot.x < x1 && shot.y > y2) && (c >= shot.raio))
		{
			return 1;
		}
		else if((shot.x > x2 && shot.y < y1) && (d >= shot.raio))
		{
			return 1;
		}
		else return 0;
	}
	else if(shot.run == 1 && ((shot.x + shot.nextx + shot.raio <= x1 || shot.y + shot.nexty + shot.raio <= y1) || (shot.x + shot.nextx - shot.raio >= x2 || shot.y + shot.nexty - shot.raio >= y2)))
	{
		return 1;
	}
	return 0;
}

// funcao que verifica colisao de tiros com circulos e com o outro tank
int verificaTiroTank(Shot shot, float x1, float y1, float raio)
{
	float a;
	if(shot.run == 1)
	{
		a = pow(pow(-x1 + shot.x + shot.cos * VELOCIDADE_TIRO, 2.0) + pow(-y1 + shot.y + shot.sen * VELOCIDADE_TIRO, 2.0), 1.0 / 2.0);
		
		if(a >= shot.raio + raio)
		{
			return 1;
		}
	}
	return 0;
}

// funcao que verifica se existira colisao pelo vento ou nao
int verificaVento(Tank tank, float x1, float y1, float x2, float y2)
{
	float a, b;
	if((tank.x > x2 && tank.y > y2) || (tank.x > x2 && tank.y < y1))
	{
		a = pow(pow(-x2 + tank.x + VENTO, 2.0) + pow(-y2 + tank.y, 2.0), 1.0 / 2.0);
		b = pow(pow(-x2 + tank.x + VENTO, 2.0) + pow(-y1 + tank.y, 2.0), 1.0 / 2.0);
		
		if((tank.x > x2 && tank.y > y2) && (a <= RAIO_TANK))
		{
			return 0;
		}
		else if((tank.x > x2 && tank.y < y1) && (b <= RAIO_TANK))
		{
			return 0;
		}
		else return 1;
	}
	else if(tank.x > x2)
	{
		if(x2 + RAIO_TANK >= tank.x)
		{
			return 0;
		}
		else return 1;
	}
	return 1;
}

// funcao que verifica se existira colisao pelo vento ou nao com circulos
int verificaVentoCirculos(Tank tank, float x, float y, float raio)
{
	float a;
	a = pow(pow(-x + tank.x + VENTO, 2.0) + pow(-y + tank.y, 2.0), 1.0 / 2.0);
	
	if(a <= raio + RAIO_TANK)
	{
		return 0;
	}
	return 1;
}

// funcao que chama todas as funcoes de colisao com todos os obstaculos e faz suas respectivas alteracoes no movimento dos tanks e dos tiros
void obstaculo(Tank *tank1, Tank *tank2, Shot *shot1, Shot *shot2, int cenario)
{	
	if(cenario == 0)
	{
		if (verificaUp((*tank1), BORDAS[0][0], BORDAS[0][1], BORDAS[0][2], BORDAS[0][3]) == 1 && 
		verificaUp((*tank1), BORDAS[1][0], BORDAS[1][1], BORDAS[1][2], BORDAS[1][3]) == 1 && 
		verificaUp((*tank1), BORDAS[2][0], BORDAS[2][1], BORDAS[2][2], BORDAS[2][3]) == 1 && 
		verificaUp((*tank1), BORDAS[3][0], BORDAS[3][1], BORDAS[3][2], BORDAS[3][3]) == 1 && 
		verificaTankUp((*tank1), (*tank2).x,  (*tank2).y, RAIO_TANK) == 1 && 
		verificaUp((*tank1), MEIO[0][0], MEIO[0][1], MEIO[0][2], MEIO[0][3]) == 1 && 
		verificaUp((*tank1), MEIO[1][0], MEIO[1][1], MEIO[1][2], MEIO[1][3]) == 1)
		{
			(*tank1).canUp = 1;
		}
		else
		{
			(*tank1).canUp = 0;
		}
		
		if (verificaDown((*tank1), BORDAS[0][0], BORDAS[0][1], BORDAS[0][2], BORDAS[0][3]) == 1 && 
		verificaDown((*tank1), BORDAS[1][0], BORDAS[1][1], BORDAS[1][2], BORDAS[1][3]) == 1 && 
		verificaDown((*tank1), BORDAS[2][0], BORDAS[2][1], BORDAS[2][2], BORDAS[2][3]) == 1 && 
		verificaDown((*tank1), BORDAS[3][0], BORDAS[3][1], BORDAS[3][2], BORDAS[3][3]) == 1 && 
		verificaTankDown((*tank1), (*tank2).x, (*tank2).y, RAIO_TANK) == 1 && 
		verificaDown((*tank1), MEIO[0][0], MEIO[0][1], MEIO[0][2], MEIO[0][3]) == 1 && 
		verificaDown((*tank1), MEIO[1][0], MEIO[1][1], MEIO[1][2], MEIO[1][3]) == 1)
		{
			(*tank1).canDown = 1;
		}
		else
		{
			(*tank1).canDown = 0;
		}
		
		if (verificaUp((*tank2), BORDAS[0][0], BORDAS[0][1], BORDAS[0][2], BORDAS[0][3]) == 1 && 
		verificaUp((*tank2), BORDAS[1][0], BORDAS[1][1], BORDAS[1][2], BORDAS[1][3]) == 1 && 
		verificaUp((*tank2), BORDAS[2][0], BORDAS[2][1], BORDAS[2][2], BORDAS[2][3]) == 1 && 
		verificaUp((*tank2), BORDAS[3][0], BORDAS[3][1], BORDAS[3][2], BORDAS[3][3]) == 1 && 
		verificaTankUp((*tank2), (*tank1).x, (*tank1).y, RAIO_TANK) == 1 && 
		verificaUp((*tank2), MEIO[0][0], MEIO[0][1], MEIO[0][2], MEIO[0][3]) == 1 && 
		verificaUp((*tank2), MEIO[1][0], MEIO[1][1], MEIO[1][2], MEIO[1][3]) == 1)
		{
			(*tank2).canUp = 1;
		}
		else
		{
			(*tank2).canUp = 0;
		}
		
		if (verificaDown((*tank2), BORDAS[0][0], BORDAS[0][1], BORDAS[0][2], BORDAS[0][3]) == 1 && 
		verificaDown((*tank2), BORDAS[1][0], BORDAS[1][1], BORDAS[1][2], BORDAS[1][3]) == 1 && 
		verificaDown((*tank2), BORDAS[2][0], BORDAS[2][1], BORDAS[2][2], BORDAS[2][3]) == 1 && 
		verificaDown((*tank2), BORDAS[3][0], BORDAS[3][1], BORDAS[3][2], BORDAS[3][3]) == 1 && 
		verificaTankDown((*tank2), (*tank1).x, (*tank1).y, RAIO_TANK) == 1 && 
		verificaDown((*tank2), MEIO[0][0], MEIO[0][1], MEIO[0][2], MEIO[0][3]) == 1 && 
		verificaDown((*tank2), MEIO[1][0], MEIO[1][1], MEIO[1][2], MEIO[1][3]) == 1)
		{
			(*tank2).canDown = 1;
		}
		else
		{
			(*tank2).canDown = 0;
		}
		
		if (verificaTiro((*shot1), BORDAS[0][0], BORDAS[0][1], BORDAS[0][2], BORDAS[0][3]) == 1 && 
		verificaTiro((*shot1), BORDAS[1][0], BORDAS[1][1], BORDAS[1][2], BORDAS[1][3]) == 1 && 
		verificaTiro((*shot1), BORDAS[2][0], BORDAS[2][1], BORDAS[2][2], BORDAS[2][3]) == 1 && 
		verificaTiro((*shot1), BORDAS[3][0], BORDAS[3][1], BORDAS[3][2], BORDAS[3][3]) == 1 && 
		verificaTiroTank((*shot1), (*tank2).x, (*tank2).y, RAIO_TANK) == 1 && 
		verificaTiro((*shot1), MEIO[0][0], MEIO[0][1], MEIO[0][2], MEIO[0][3]) == 1 && 
		verificaTiro((*shot1), MEIO[1][0], MEIO[1][1], MEIO[1][2], MEIO[1][3]) == 1)
		{
			(*shot1).run = 1;
		}
		else if((*shot1).run == 1 && verificaTiroTank((*shot1), (*tank2).x, (*tank2).y, RAIO_TANK) == 0)
		{
			if((*shot1).run == 1 && (*shot1).tipo == 2)
			{
				al_draw_filled_circle((*shot1).x, (*shot1).y, RAIO_EXPLOSAO, al_map_rgb(255, 0, 0));
				verificaExplosao(&(*shot1), RAIO_EXPLOSAO, &(*tank1), &(*tank2));
				(*shot1).explosao = 1;
			}
			else (*tank1).pontos ++;
			(*shot1).run = 0;
		}
		else
		{
			if((*shot1).run == 1 && (*shot1).tipo == 2)
			{
				al_draw_filled_circle((*shot1).x, (*shot1).y, RAIO_EXPLOSAO, al_map_rgb(255, 0, 0));
				verificaExplosao(&(*shot1), RAIO_EXPLOSAO, &(*tank1), &(*tank2));
				(*shot1).explosao = 1;
			}
			(*shot1).run = 0;
		}
		
		if (verificaTiro((*shot2), BORDAS[0][0], BORDAS[0][1], BORDAS[0][2], BORDAS[0][3]) == 1 && 
		verificaTiro((*shot2), BORDAS[1][0], BORDAS[1][1], BORDAS[1][2], BORDAS[1][3]) == 1 && 
		verificaTiro((*shot2), BORDAS[2][0], BORDAS[2][1], BORDAS[2][2], BORDAS[2][3]) == 1 && 
		verificaTiro((*shot2), BORDAS[3][0], BORDAS[3][1], BORDAS[3][2], BORDAS[3][3]) == 1 && 
		verificaTiroTank((*shot2), (*tank1).x, (*tank1).y, RAIO_TANK) == 1 && 
		verificaTiro((*shot2), MEIO[0][0], MEIO[0][1], MEIO[0][2], MEIO[0][3]) == 1 && 
		verificaTiro((*shot2), MEIO[1][0], MEIO[1][1], MEIO[1][2], MEIO[1][3]) == 1)
		{
			(*shot2).run = 1;
		}
		else if((*shot2).run == 1 && verificaTiroTank((*shot2), (*tank1).x, (*tank1).y, RAIO_TANK) == 0)
		{
			if((*shot2).run == 1 && (*shot2).tipo == 2)
			{
				al_draw_filled_circle((*shot2).x, (*shot2).y, RAIO_EXPLOSAO, al_map_rgb(255, 0, 0));
				verificaExplosao(&(*shot2), RAIO_EXPLOSAO, &(*tank1), &(*tank2));
				(*shot2).explosao = 1;
			}
			else (*tank2).pontos ++;
			(*shot2).run = 0;
		}
		else
		{
			if((*shot2).run == 1 && (*shot2).tipo == 2)
			{
				al_draw_filled_circle((*shot2).x, (*shot2).y, RAIO_EXPLOSAO, al_map_rgb(255, 0, 0));
				verificaExplosao(&(*shot2), RAIO_EXPLOSAO, &(*tank1), &(*tank2));
				(*shot2).explosao = 1;
			}
			(*shot2).run = 0;
		}
	}
	
	if(cenario == 1)
	{
		if (verificaUp((*tank1), BORDAS[0][0], BORDAS[0][1], BORDAS[0][2], BORDAS[0][3]) == 1 && 
		verificaUp((*tank1), BORDAS[1][0], BORDAS[1][1], BORDAS[1][2], BORDAS[1][3]) == 1 && 
		verificaUp((*tank1), BORDAS[2][0], BORDAS[2][1], BORDAS[2][2], BORDAS[2][3]) == 1 && 
		verificaUp((*tank1), BORDAS[3][0], BORDAS[3][1], BORDAS[3][2], BORDAS[3][3]) == 1 && 
		verificaTankUp((*tank1), (*tank2).x,  (*tank2).y, RAIO_TANK) == 1 && 
		verificaTankUp((*tank1), BOLAS[0][0], BOLAS[0][1], BOLAS[0][2]) == 1 && 
		verificaTankUp((*tank1), BOLAS[1][0], BOLAS[1][1], BOLAS[1][2]) == 1 && 
		verificaTankUp((*tank1), BOLAS[2][0], BOLAS[2][1], BOLAS[2][2]) == 1 &&
		verificaTankUp((*tank1), BOLAS[3][0], BOLAS[3][1], BOLAS[3][2]) == 1 && 
		verificaUp((*tank1), MEIO[0][0], MEIO[0][1], MEIO[0][2], MEIO[0][3]) == 1 && 
		verificaUp((*tank1), MEIO[1][0], MEIO[1][1], MEIO[1][2], MEIO[1][3]) == 1 &&
		verificaUp((*tank1), CANTOS[0][0], CANTOS[0][1], CANTOS[0][2], CANTOS[0][3]) == 1 &&
		verificaUp((*tank1), CANTOS[1][0], CANTOS[1][1], CANTOS[1][2], CANTOS[1][3]) == 1 &&
		verificaUp((*tank1), CANTOS[2][0], CANTOS[2][1], CANTOS[2][2], CANTOS[2][3]) == 1 &&
		verificaUp((*tank1), CANTOS[3][0], CANTOS[3][1], CANTOS[3][2], CANTOS[3][3]) == 1)
		{
			(*tank1).canUp = 1;
		}
		else
		{
			(*tank1).canUp = 0;
		}
		
		if (verificaDown((*tank1), BORDAS[0][0], BORDAS[0][1], BORDAS[0][2], BORDAS[0][3]) == 1 && 
		verificaDown((*tank1), BORDAS[1][0], BORDAS[1][1], BORDAS[1][2], BORDAS[1][3]) == 1 && 
		verificaDown((*tank1), BORDAS[2][0], BORDAS[2][1], BORDAS[2][2], BORDAS[2][3]) == 1 && 
		verificaDown((*tank1), BORDAS[3][0], BORDAS[3][1], BORDAS[3][2], BORDAS[3][3]) == 1 && 
		verificaTankDown((*tank1), (*tank2).x, (*tank2).y, RAIO_TANK) == 1 && 
		verificaTankDown((*tank1), BOLAS[0][0], BOLAS[0][1], BOLAS[0][2]) == 1 && 
		verificaTankDown((*tank1), BOLAS[1][0], BOLAS[1][1], BOLAS[1][2]) == 1 && 
		verificaTankDown((*tank1), BOLAS[2][0], BOLAS[2][1], BOLAS[2][2]) == 1 && 
		verificaTankDown((*tank1), BOLAS[3][0], BOLAS[3][1], BOLAS[3][2]) == 1 && 
		verificaDown((*tank1), MEIO[0][0], MEIO[0][1], MEIO[0][2], MEIO[0][3]) == 1 && 
		verificaDown((*tank1), MEIO[1][0], MEIO[1][1], MEIO[1][2], MEIO[1][3]) == 1 &&
		verificaDown((*tank1), CANTOS[0][0], CANTOS[0][1], CANTOS[0][2], CANTOS[0][3]) == 1 &&
		verificaDown((*tank1), CANTOS[1][0], CANTOS[1][1], CANTOS[1][2], CANTOS[1][3]) == 1 &&
		verificaDown((*tank1), CANTOS[2][0], CANTOS[2][1], CANTOS[2][2], CANTOS[2][3]) == 1 &&
		verificaDown((*tank1), CANTOS[3][0], CANTOS[3][1], CANTOS[3][2], CANTOS[3][3]) == 1)
		{
			(*tank1).canDown = 1;
		}
		else
		{
			(*tank1).canDown = 0;
		}
		
		if (verificaUp((*tank2), BORDAS[0][0], BORDAS[0][1], BORDAS[0][2], BORDAS[0][3]) == 1 && 
		verificaUp((*tank2), BORDAS[1][0], BORDAS[1][1], BORDAS[1][2], BORDAS[1][3]) == 1 && 
		verificaUp((*tank2), BORDAS[2][0], BORDAS[2][1], BORDAS[2][2], BORDAS[2][3]) == 1 && 
		verificaUp((*tank2), BORDAS[3][0], BORDAS[3][1], BORDAS[3][2], BORDAS[3][3]) == 1 && 
		verificaTankUp((*tank2), (*tank1).x, (*tank1).y, RAIO_TANK) == 1 && 
		verificaTankUp((*tank2), BOLAS[0][0], BOLAS[0][1], BOLAS[0][2]) == 1 && 
		verificaTankUp((*tank2), BOLAS[1][0], BOLAS[1][1], BOLAS[1][2]) == 1 && 
		verificaTankUp((*tank2), BOLAS[2][0], BOLAS[2][1], BOLAS[2][2]) == 1 && 
		verificaTankUp((*tank2), BOLAS[3][0], BOLAS[3][1], BOLAS[3][2]) == 1 && 
		verificaUp((*tank2), MEIO[0][0], MEIO[0][1], MEIO[0][2], MEIO[0][3]) == 1 && 
		verificaUp((*tank2), MEIO[1][0], MEIO[1][1], MEIO[1][2], MEIO[1][3]) == 1 &&
		verificaUp((*tank2), CANTOS[0][0], CANTOS[0][1], CANTOS[0][2], CANTOS[0][3]) == 1 &&
		verificaUp((*tank2), CANTOS[1][0], CANTOS[1][1], CANTOS[1][2], CANTOS[1][3]) == 1 &&
		verificaUp((*tank2), CANTOS[2][0], CANTOS[2][1], CANTOS[2][2], CANTOS[2][3]) == 1 &&
		verificaUp((*tank2), CANTOS[3][0], CANTOS[3][1], CANTOS[3][2], CANTOS[3][3]) == 1)
		{
			(*tank2).canUp = 1;
		}
		else
		{
			(*tank2).canUp = 0;
		}
		
		if (verificaDown((*tank2), BORDAS[0][0], BORDAS[0][1], BORDAS[0][2], BORDAS[0][3]) == 1 && 
		verificaDown((*tank2), BORDAS[1][0], BORDAS[1][1], BORDAS[1][2], BORDAS[1][3]) == 1 && 
		verificaDown((*tank2), BORDAS[2][0], BORDAS[2][1], BORDAS[2][2], BORDAS[2][3]) == 1 && 
		verificaDown((*tank2), BORDAS[3][0], BORDAS[3][1], BORDAS[3][2], BORDAS[3][3]) == 1 && 
		verificaTankDown((*tank2), (*tank1).x, (*tank1).y, RAIO_TANK) == 1 && 
		verificaTankDown((*tank2), BOLAS[0][0], BOLAS[0][1], BOLAS[0][2]) == 1 && 
		verificaTankDown((*tank2), BOLAS[1][0], BOLAS[1][1], BOLAS[1][2]) == 1 && 
		verificaTankDown((*tank2), BOLAS[2][0], BOLAS[2][1], BOLAS[2][2]) == 1 && 
		verificaTankDown((*tank2), BOLAS[3][0], BOLAS[3][1], BOLAS[3][2]) == 1 && 
		verificaDown((*tank2), MEIO[0][0], MEIO[0][1], MEIO[0][2], MEIO[0][3]) == 1 && 
		verificaDown((*tank2), MEIO[1][0], MEIO[1][1], MEIO[1][2], MEIO[1][3]) == 1 &&
		verificaDown((*tank2), CANTOS[0][0], CANTOS[0][1], CANTOS[0][2], CANTOS[0][3]) == 1 &&
		verificaDown((*tank2), CANTOS[1][0], CANTOS[1][1], CANTOS[1][2], CANTOS[1][3]) == 1 &&
		verificaDown((*tank2), CANTOS[2][0], CANTOS[2][1], CANTOS[2][2], CANTOS[2][3]) == 1 &&
		verificaDown((*tank2), CANTOS[3][0], CANTOS[3][1], CANTOS[3][2], CANTOS[3][3]) == 1)
		{
			(*tank2).canDown = 1;
		}
		else
		{
			(*tank2).canDown = 0;
		}
		
		if (verificaTiro((*shot1), BORDAS[0][0], BORDAS[0][1], BORDAS[0][2], BORDAS[0][3]) == 1 && 
		verificaTiro((*shot1), BORDAS[1][0], BORDAS[1][1], BORDAS[1][2], BORDAS[1][3]) == 1 && 
		verificaTiro((*shot1), BORDAS[2][0], BORDAS[2][1], BORDAS[2][2], BORDAS[2][3]) == 1 && 
		verificaTiro((*shot1), BORDAS[3][0], BORDAS[3][1], BORDAS[3][2], BORDAS[3][3]) == 1 && 
		verificaTiroTank((*shot1), (*tank2).x, (*tank2).y, RAIO_TANK) == 1 && 
		verificaTiroTank((*shot1), BOLAS[0][0], BOLAS[0][1], BOLAS[0][2]) == 1 && 
		verificaTiroTank((*shot1), BOLAS[1][0], BOLAS[1][1], BOLAS[1][2]) == 1 && 
		verificaTiroTank((*shot1), BOLAS[2][0], BOLAS[2][1], BOLAS[2][2]) == 1 && 
		verificaTiroTank((*shot1), BOLAS[3][0], BOLAS[3][1], BOLAS[3][2]) == 1 && 
		verificaTiro((*shot1), MEIO[0][0], MEIO[0][1], MEIO[0][2], MEIO[0][3]) == 1 && 
		verificaTiro((*shot1), MEIO[1][0], MEIO[1][1], MEIO[1][2], MEIO[1][3]) == 1 &&
		verificaTiro((*shot1), CANTOS[0][0], CANTOS[0][1], CANTOS[0][2], CANTOS[0][3]) == 1 &&
		verificaTiro((*shot1), CANTOS[1][0], CANTOS[1][1], CANTOS[1][2], CANTOS[1][3]) == 1 &&
		verificaTiro((*shot1), CANTOS[2][0], CANTOS[2][1], CANTOS[2][2], CANTOS[2][3]) == 1 &&
		verificaTiro((*shot1), CANTOS[3][0], CANTOS[3][1], CANTOS[3][2], CANTOS[3][3]) == 1)
		{
			(*shot1).run = 1;
		}
		else if((*shot1).run == 1 && verificaTiroTank((*shot1), (*tank2).x, (*tank2).y, RAIO_TANK) == 0)
		{
			if((*shot1).run == 1 && (*shot1).tipo == 2)
			{
				al_draw_filled_circle((*shot1).x, (*shot1).y, RAIO_EXPLOSAO, al_map_rgb(255, 0, 0));
				verificaExplosao(&(*shot1), RAIO_EXPLOSAO, &(*tank1), &(*tank2));
				(*shot1).explosao = 1;
			}
			else (*tank1).pontos ++;
			(*shot1).run = 0;
		}
		else
		{
			if((*shot1).run == 1 && (*shot1).tipo == 2)
			{
				al_draw_filled_circle((*shot1).x, (*shot1).y, RAIO_EXPLOSAO, al_map_rgb(255, 0, 0));
				verificaExplosao(&(*shot1), RAIO_EXPLOSAO, &(*tank1), &(*tank2));
				(*shot1).explosao = 1;
			}
			(*shot1).run = 0;
		}
		
		if (verificaTiro((*shot2), BORDAS[0][0], BORDAS[0][1], BORDAS[0][2], BORDAS[0][3]) == 1 && 
		verificaTiro((*shot2), BORDAS[1][0], BORDAS[1][1], BORDAS[1][2], BORDAS[1][3]) == 1 && 
		verificaTiro((*shot2), BORDAS[2][0], BORDAS[2][1], BORDAS[2][2], BORDAS[2][3]) == 1 && 
		verificaTiro((*shot2), BORDAS[3][0], BORDAS[3][1], BORDAS[3][2], BORDAS[3][3]) == 1 && 
		verificaTiroTank((*shot2), (*tank1).x, (*tank1).y, RAIO_TANK) == 1 && 
		verificaTiroTank((*shot2), BOLAS[0][0], BOLAS[0][1], BOLAS[0][2]) == 1 && 
		verificaTiroTank((*shot2), BOLAS[1][0], BOLAS[1][1], BOLAS[1][2]) == 1 && 
		verificaTiroTank((*shot2), BOLAS[2][0], BOLAS[2][1], BOLAS[2][2]) == 1 && 
		verificaTiroTank((*shot2), BOLAS[3][0], BOLAS[3][1], BOLAS[3][2]) == 1 && 
		verificaTiro((*shot2), MEIO[0][0], MEIO[0][1], MEIO[0][2], MEIO[0][3]) == 1 && 
		verificaTiro((*shot2), MEIO[1][0], MEIO[1][1], MEIO[1][2], MEIO[1][3]) == 1 &&
		verificaTiro((*shot2), CANTOS[0][0], CANTOS[0][1], CANTOS[0][2], CANTOS[0][3]) == 1 &&
		verificaTiro((*shot2), CANTOS[1][0], CANTOS[1][1], CANTOS[1][2], CANTOS[1][3]) == 1 &&
		verificaTiro((*shot2), CANTOS[2][0], CANTOS[2][1], CANTOS[2][2], CANTOS[2][3]) == 1 &&
		verificaTiro((*shot2), CANTOS[3][0], CANTOS[3][1], CANTOS[3][2], CANTOS[3][3]) == 1)
		{
			(*shot2).run = 1;
		}
		else if((*shot2).run == 1 && verificaTiroTank((*shot2), (*tank1).x, (*tank1).y, RAIO_TANK) == 0)
		{
			if((*shot2).run == 1 && (*shot2).tipo == 2)
			{
				al_draw_filled_circle((*shot2).x, (*shot2).y, RAIO_EXPLOSAO, al_map_rgb(255, 0, 0));
				verificaExplosao(&(*shot2), RAIO_EXPLOSAO, &(*tank1), &(*tank2));
				(*shot2).explosao = 1;
			}
			else (*tank2).pontos ++;
			(*shot2).run = 0;
		}
		else
		{
			if((*shot2).run == 1 && (*shot2).tipo == 2)
			{
				al_draw_filled_circle((*shot2).x, (*shot2).y, RAIO_EXPLOSAO, al_map_rgb(255, 0, 0));
				verificaExplosao(&(*shot2), RAIO_EXPLOSAO, &(*tank1), &(*tank2));
				(*shot2).explosao = 1;
			}
			(*shot2).run = 0;
		}
	}
	
	if(cenario == 2)
	{
		if (verificaUp((*tank1), BORDAS[0][0], BORDAS[0][1], BORDAS[0][2], BORDAS[0][3]) == 1 && 
		verificaUp((*tank1), BORDAS[1][0], BORDAS[1][1], BORDAS[1][2], BORDAS[1][3]) == 1 && 
		verificaUp((*tank1), BORDAS[2][0], BORDAS[2][1], BORDAS[2][2], BORDAS[2][3]) == 1 && 
		verificaUp((*tank1), BORDAS[3][0], BORDAS[3][1], BORDAS[3][2], BORDAS[3][3]) == 1 && 
		verificaTankUp((*tank1), (*tank2).x,  (*tank2).y, RAIO_TANK) == 1 && 
		verificaTankUp((*tank1), CIRCULO[0][0], CIRCULO[0][1], CIRCULO[0][2]) == 1 && 
		verificaUp((*tank1), REC[0][0], REC[0][1], REC[0][2], REC[0][3]) == 1 && 
		verificaUp((*tank1), REC[1][0], REC[1][1], REC[1][2], REC[1][3]) == 1 &&
		verificaUp((*tank1), REC[2][0], REC[2][1], REC[2][2], REC[2][3]) == 1 && 
		verificaUp((*tank1), REC[3][0], REC[3][1], REC[3][2], REC[3][3]) == 1 &&
		verificaUp((*tank1), CANTOS[0][0], CANTOS[0][1], CANTOS[0][2], CANTOS[0][3]) == 1 &&
		verificaUp((*tank1), CANTOS[1][0], CANTOS[1][1], CANTOS[1][2], CANTOS[1][3]) == 1 &&
		verificaUp((*tank1), CANTOS[2][0], CANTOS[2][1], CANTOS[2][2], CANTOS[2][3]) == 1 &&
		verificaUp((*tank1), CANTOS[3][0], CANTOS[3][1], CANTOS[3][2], CANTOS[3][3]) == 1)
		{
			(*tank1).canUp = 1;
		}
		else
		{
			(*tank1).canUp = 0;
		}
		
		if (verificaDown((*tank1), BORDAS[0][0], BORDAS[0][1], BORDAS[0][2], BORDAS[0][3]) == 1 && 
		verificaDown((*tank1), BORDAS[1][0], BORDAS[1][1], BORDAS[1][2], BORDAS[1][3]) == 1 && 
		verificaDown((*tank1), BORDAS[2][0], BORDAS[2][1], BORDAS[2][2], BORDAS[2][3]) == 1 && 
		verificaDown((*tank1), BORDAS[3][0], BORDAS[3][1], BORDAS[3][2], BORDAS[3][3]) == 1 && 
		verificaTankDown((*tank1), (*tank2).x, (*tank2).y, RAIO_TANK) == 1 && 
		verificaTankDown((*tank1), CIRCULO[0][0], CIRCULO[0][1], CIRCULO[0][2]) == 1 && 
		verificaDown((*tank1), REC[0][0], REC[0][1], REC[0][2], REC[0][3]) == 1 && 
		verificaDown((*tank1), REC[1][0], REC[1][1], REC[1][2], REC[1][3]) == 1 &&
		verificaDown((*tank1), REC[2][0], REC[2][1], REC[2][2], REC[2][3]) == 1 && 
		verificaDown((*tank1), REC[3][0], REC[3][1], REC[3][2], REC[3][3]) == 1 &&
		verificaDown((*tank1), CANTOS[0][0], CANTOS[0][1], CANTOS[0][2], CANTOS[0][3]) == 1 &&
		verificaDown((*tank1), CANTOS[1][0], CANTOS[1][1], CANTOS[1][2], CANTOS[1][3]) == 1 &&
		verificaDown((*tank1), CANTOS[2][0], CANTOS[2][1], CANTOS[2][2], CANTOS[2][3]) == 1 &&
		verificaDown((*tank1), CANTOS[3][0], CANTOS[3][1], CANTOS[3][2], CANTOS[3][3]) == 1)
		{
			(*tank1).canDown = 1;
		}
		else
		{
			(*tank1).canDown = 0;
		}
		
		if (verificaUp((*tank2), BORDAS[0][0], BORDAS[0][1], BORDAS[0][2], BORDAS[0][3]) == 1 && 
		verificaUp((*tank2), BORDAS[1][0], BORDAS[1][1], BORDAS[1][2], BORDAS[1][3]) == 1 && 
		verificaUp((*tank2), BORDAS[2][0], BORDAS[2][1], BORDAS[2][2], BORDAS[2][3]) == 1 && 
		verificaUp((*tank2), BORDAS[3][0], BORDAS[3][1], BORDAS[3][2], BORDAS[3][3]) == 1 && 
		verificaTankUp((*tank2), (*tank1).x, (*tank1).y, RAIO_TANK) == 1 && 
		verificaTankUp((*tank2), CIRCULO[0][0], CIRCULO[0][1], CIRCULO[0][2]) == 1 && 
		verificaUp((*tank2), REC[0][0], REC[0][1], REC[0][2], REC[0][3]) == 1 && 
		verificaUp((*tank2), REC[1][0], REC[1][1], REC[1][2], REC[1][3]) == 1 &&
		verificaUp((*tank2), REC[2][0], REC[2][1], REC[2][2], REC[2][3]) == 1 && 
		verificaUp((*tank2), REC[3][0], REC[3][1], REC[3][2], REC[3][3]) == 1 &&
		verificaUp((*tank2), CANTOS[0][0], CANTOS[0][1], CANTOS[0][2], CANTOS[0][3]) == 1 &&
		verificaUp((*tank2), CANTOS[1][0], CANTOS[1][1], CANTOS[1][2], CANTOS[1][3]) == 1 &&
		verificaUp((*tank2), CANTOS[2][0], CANTOS[2][1], CANTOS[2][2], CANTOS[2][3]) == 1 &&
		verificaUp((*tank2), CANTOS[3][0], CANTOS[3][1], CANTOS[3][2], CANTOS[3][3]) == 1)
		{
			(*tank2).canUp = 1;
		}
		else
		{
			(*tank2).canUp = 0;
		}
		
		if (verificaDown((*tank2), BORDAS[0][0], BORDAS[0][1], BORDAS[0][2], BORDAS[0][3]) == 1 && 
		verificaDown((*tank2), BORDAS[1][0], BORDAS[1][1], BORDAS[1][2], BORDAS[1][3]) == 1 && 
		verificaDown((*tank2), BORDAS[2][0], BORDAS[2][1], BORDAS[2][2], BORDAS[2][3]) == 1 && 
		verificaDown((*tank2), BORDAS[3][0], BORDAS[3][1], BORDAS[3][2], BORDAS[3][3]) == 1 && 
		verificaTankDown((*tank2), (*tank1).x, (*tank1).y, RAIO_TANK) == 1 && 
		verificaTankDown((*tank2), CIRCULO[0][0], CIRCULO[0][1], CIRCULO[0][2]) == 1 && 
		verificaDown((*tank2), REC[0][0], REC[0][1], REC[0][2], REC[0][3]) == 1 && 
		verificaDown((*tank2), REC[1][0], REC[1][1], REC[1][2], REC[1][3]) == 1 &&
		verificaDown((*tank2), REC[2][0], REC[2][1], REC[2][2], REC[2][3]) == 1 && 
		verificaDown((*tank2), REC[3][0], REC[3][1], REC[3][2], REC[3][3]) == 1 &&
		verificaDown((*tank2), CANTOS[0][0], CANTOS[0][1], CANTOS[0][2], CANTOS[0][3]) == 1 &&
		verificaDown((*tank2), CANTOS[1][0], CANTOS[1][1], CANTOS[1][2], CANTOS[1][3]) == 1 &&
		verificaDown((*tank2), CANTOS[2][0], CANTOS[2][1], CANTOS[2][2], CANTOS[2][3]) == 1 &&
		verificaDown((*tank2), CANTOS[3][0], CANTOS[3][1], CANTOS[3][2], CANTOS[3][3]) == 1)
		{
			(*tank2).canDown = 1;
		}
		else
		{
			(*tank2).canDown = 0;
		}
		
		if (verificaTiro((*shot1), BORDAS[0][0], BORDAS[0][1], BORDAS[0][2], BORDAS[0][3]) == 1 && 
		verificaTiro((*shot1), BORDAS[1][0], BORDAS[1][1], BORDAS[1][2], BORDAS[1][3]) == 1 && 
		verificaTiro((*shot1), BORDAS[2][0], BORDAS[2][1], BORDAS[2][2], BORDAS[2][3]) == 1 && 
		verificaTiro((*shot1), BORDAS[3][0], BORDAS[3][1], BORDAS[3][2], BORDAS[3][3]) == 1 && 
		verificaTiroTank((*shot1), (*tank2).x, (*tank2).y, RAIO_TANK) == 1 && 
		verificaTiroTank((*shot1), CIRCULO[0][0], CIRCULO[0][1], CIRCULO[0][2]) == 1 && 
		verificaTiro((*shot1), REC[0][0], REC[0][1], REC[0][2], REC[0][3]) == 1 && 
		verificaTiro((*shot1), REC[1][0], REC[1][1], REC[1][2], REC[1][3]) == 1 &&
		verificaTiro((*shot1), REC[2][0], REC[2][1], REC[2][2], REC[2][3]) == 1 && 
		verificaTiro((*shot1), REC[3][0], REC[3][1], REC[3][2], REC[3][3]) == 1 &&
		verificaTiro((*shot1), CANTOS[0][0], CANTOS[0][1], CANTOS[0][2], CANTOS[0][3]) == 1 &&
		verificaTiro((*shot1), CANTOS[1][0], CANTOS[1][1], CANTOS[1][2], CANTOS[1][3]) == 1 &&
		verificaTiro((*shot1), CANTOS[2][0], CANTOS[2][1], CANTOS[2][2], CANTOS[2][3]) == 1 &&
		verificaTiro((*shot1), CANTOS[3][0], CANTOS[3][1], CANTOS[3][2], CANTOS[3][3]) == 1)
		{
			(*shot1).run = 1;
		}
		else if((*shot1).run == 1 && verificaTiroTank((*shot1), (*tank2).x, (*tank2).y, RAIO_TANK) == 0)
		{
			if((*shot1).run == 1 && (*shot1).tipo == 2)
			{
				al_draw_filled_circle((*shot1).x, (*shot1).y, RAIO_EXPLOSAO, al_map_rgb(255, 0, 0));
				verificaExplosao(&(*shot1), RAIO_EXPLOSAO, &(*tank1), &(*tank2));
				(*shot1).explosao = 1;
			}
			else (*tank1).pontos ++;
			(*shot1).run = 0;
		}
		else
		{
			if((*shot1).run == 1 && (*shot1).tipo == 2)
			{
				al_draw_filled_circle((*shot1).x, (*shot1).y, RAIO_EXPLOSAO, al_map_rgb(255, 0, 0));
				verificaExplosao(&(*shot1), RAIO_EXPLOSAO, &(*tank1), &(*tank2));
				(*shot1).explosao = 1;
			}
			(*shot1).run = 0;
		}
		
		if (verificaTiro((*shot2), BORDAS[0][0], BORDAS[0][1], BORDAS[0][2], BORDAS[0][3]) == 1 && 
		verificaTiro((*shot2), BORDAS[1][0], BORDAS[1][1], BORDAS[1][2], BORDAS[1][3]) == 1 && 
		verificaTiro((*shot2), BORDAS[2][0], BORDAS[2][1], BORDAS[2][2], BORDAS[2][3]) == 1 && 
		verificaTiro((*shot2), BORDAS[3][0], BORDAS[3][1], BORDAS[3][2], BORDAS[3][3]) == 1 && 
		verificaTiroTank((*shot2), (*tank1).x, (*tank1).y, RAIO_TANK) == 1 && 
		verificaTiroTank((*shot2), CIRCULO[0][0], CIRCULO[0][1], CIRCULO[0][2]) == 1 && 
		verificaTiro((*shot2), REC[0][0], REC[0][1], REC[0][2], REC[0][3]) == 1 && 
		verificaTiro((*shot2), REC[1][0], REC[1][1], REC[1][2], REC[1][3]) == 1 &&
		verificaTiro((*shot2), REC[2][0], REC[2][1], REC[2][2], REC[2][3]) == 1 && 
		verificaTiro((*shot2), REC[3][0], REC[3][1], REC[3][2], REC[3][3]) == 1 &&
		verificaTiro((*shot2), CANTOS[0][0], CANTOS[0][1], CANTOS[0][2], CANTOS[0][3]) == 1 &&
		verificaTiro((*shot2), CANTOS[1][0], CANTOS[1][1], CANTOS[1][2], CANTOS[1][3]) == 1 &&
		verificaTiro((*shot2), CANTOS[2][0], CANTOS[2][1], CANTOS[2][2], CANTOS[2][3]) == 1 &&
		verificaTiro((*shot2), CANTOS[3][0], CANTOS[3][1], CANTOS[3][2], CANTOS[3][3]) == 1)
		{
			(*shot2).run = 1;
		}
		else if((*shot2).run == 1 && verificaTiroTank((*shot2), (*tank1).x, (*tank1).y, RAIO_TANK) == 0)
		{
			if((*shot2).run == 1 && (*shot2).tipo == 2)
			{
				al_draw_filled_circle((*shot2).x, (*shot2).y, RAIO_EXPLOSAO, al_map_rgb(255, 0, 0));
				verificaExplosao(&(*shot2), RAIO_EXPLOSAO, &(*tank1), &(*tank2));
				(*shot2).explosao = 1;
			}
			else (*tank2).pontos ++;
			(*shot2).run = 0;
		}
		else
		{
			if((*shot2).run == 1 && (*shot2).tipo == 2)
			{
				al_draw_filled_circle((*shot2).x, (*shot2).y, RAIO_EXPLOSAO, al_map_rgb(255, 0, 0));
				verificaExplosao(&(*shot2), RAIO_EXPLOSAO, &(*tank1), &(*tank2));
				(*shot2).explosao = 1;
			}
			(*shot2).run = 0;
		}
	}
}

// funcao que chama as funcoes de colisao de vendo e altera o movimento dos tanks com isso
void obstaculoVento(Tank *tank1, Tank *tank2, int cenario)
{
	if(cenario == 2)
	{
		if (verificaVento((*tank1), BORDAS[0][0], BORDAS[0][1], BORDAS[0][2], BORDAS[0][3]) == 1 && 
		verificaVento((*tank1), BORDAS[1][0], BORDAS[1][1], BORDAS[1][2], BORDAS[1][3]) == 1 && 
		verificaVento((*tank1), BORDAS[2][0], BORDAS[2][1], BORDAS[2][2], BORDAS[2][3]) == 1 && 
		verificaVento((*tank1), BORDAS[3][0], BORDAS[3][1], BORDAS[3][2], BORDAS[3][3]) == 1 && 
		verificaVentoCirculos((*tank1), (*tank2).x,  (*tank2).y, RAIO_TANK) == 1 && 
		verificaVentoCirculos((*tank1), CIRCULO[0][0], CIRCULO[0][1], CIRCULO[0][2]) == 1 && 
		verificaVento((*tank1), REC[0][0], REC[0][1], REC[0][2], REC[0][3]) == 1 && 
		verificaVento((*tank1), REC[1][0], REC[1][1], REC[1][2], REC[1][3]) == 1 &&
		verificaVento((*tank1), REC[2][0], REC[2][1], REC[2][2], REC[2][3]) == 1 && 
		verificaVento((*tank1), REC[3][0], REC[3][1], REC[3][2], REC[3][3]) == 1 &&
		verificaVento((*tank1), CANTOS[0][0], CANTOS[0][1], CANTOS[0][2], CANTOS[0][3]) == 1 &&
		verificaVento((*tank1), CANTOS[1][0], CANTOS[1][1], CANTOS[1][2], CANTOS[1][3]) == 1 &&
		verificaVento((*tank1), CANTOS[2][0], CANTOS[2][1], CANTOS[2][2], CANTOS[2][3]) == 1 &&
		verificaVento((*tank1), CANTOS[3][0], CANTOS[3][1], CANTOS[3][2], CANTOS[3][3]) == 1)
		{
			(*tank1).x -= VENTO;
		}
		if (verificaVento((*tank2), BORDAS[0][0], BORDAS[0][1], BORDAS[0][2], BORDAS[0][3]) == 1 && 
		verificaVento((*tank2), BORDAS[1][0], BORDAS[1][1], BORDAS[1][2], BORDAS[1][3]) == 1 && 
		verificaVento((*tank2), BORDAS[2][0], BORDAS[2][1], BORDAS[2][2], BORDAS[2][3]) == 1 && 
		verificaVento((*tank2), BORDAS[3][0], BORDAS[3][1], BORDAS[3][2], BORDAS[3][3]) == 1 && 
		verificaVentoCirculos((*tank2), (*tank1).x,  (*tank1).y, RAIO_TANK) == 1 && 
		verificaVentoCirculos((*tank2), CIRCULO[0][0], CIRCULO[0][1], CIRCULO[0][2]) == 1 && 
		verificaVento((*tank2), REC[0][0], REC[0][1], REC[0][2], REC[0][3]) == 1 && 
		verificaVento((*tank2), REC[1][0], REC[1][1], REC[1][2], REC[1][3]) == 1 &&
		verificaVento((*tank2), REC[2][0], REC[2][1], REC[2][2], REC[2][3]) == 1 && 
		verificaVento((*tank2), REC[3][0], REC[3][1], REC[3][2], REC[3][3]) == 1 &&
		verificaVento((*tank2), CANTOS[0][0], CANTOS[0][1], CANTOS[0][2], CANTOS[0][3]) == 1 &&
		verificaVento((*tank2), CANTOS[1][0], CANTOS[1][1], CANTOS[1][2], CANTOS[1][3]) == 1 &&
		verificaVento((*tank2), CANTOS[2][0], CANTOS[2][1], CANTOS[2][2], CANTOS[2][3]) == 1 &&
		verificaVento((*tank2), CANTOS[3][0], CANTOS[3][1], CANTOS[3][2], CANTOS[3][3]) == 1)
		{
			(*tank2).x -= VENTO;
		}
	}
}

// funcao que move o tank para frente e para tras quando puder e quando for solicitado
void moveUpDown(Tank *tank1, Tank *tank2)
{
	if((*tank1).up == 1 && (*tank1).down == 1);
	else if((*tank1).up == 1 && (*tank1).canUp == 1)
	{
		(*tank1).x += (*tank1).cos * VELOCIDADE_TANK;
		(*tank1).y += (*tank1).sen * VELOCIDADE_TANK;
	}
	else if((*tank1).down == 1 && (*tank1).canDown == 1)
	{
		(*tank1).x -= (*tank1).cos * VELOCIDADE_TANK;
		(*tank1).y -= (*tank1).sen * VELOCIDADE_TANK;
	}
	
	if((*tank2).up == 1 && (*tank2).down == 1);
	else if((*tank2).up == 1 && (*tank2).canUp == 1)
	{
		(*tank2).x += (*tank2).cos * VELOCIDADE_TANK;
		(*tank2).y += (*tank2).sen * VELOCIDADE_TANK;
	}
	else if((*tank2).down == 1 && (*tank2).canDown == 1)
	{
		(*tank2).x -= (*tank2).cos * VELOCIDADE_TANK;
		(*tank2).y -= (*tank2).sen * VELOCIDADE_TANK;
	}
}

// funcao que gira o tank para direita e esquerda
void moveLeftRight(Tank *tank1, Tank *tank2)
{
	if((*tank1).left == 1 && (*tank1).right == 1);
	else if((*tank1).left == 1)
	{
		(*tank1).degree --;
	}
	else if((*tank1).right == 1)
	{
		(*tank1).degree ++;
	}
	
	if((*tank2).left == 1 && (*tank2).right == 1);
	else if((*tank2).left == 1)
	{
		(*tank2).degree --;
	}
	else if((*tank2).right == 1)
	{
		(*tank2).degree ++;
	}
}

// funcao que arquiva o historico visivelmente
void arquivaHistorico(FILE *historico, char h[], int v1, int v2, int p1, int p2)
{
	historico = fopen(h, "w");
	fprintf(historico, "HISTORICO DE PARTIDAS\nJogador 1 Jogador 2\n");
	fprintf(historico, "%9d %9d\n\n", v1, v2);
	fprintf(historico, "ULTIMA PARTIDA\nJogador 1 Jogador 2\n");
	fprintf(historico, "%9d %9d\n", p1, p2);
	fclose(historico);
}

// funcao que arquiva somente os valores de vitorias e os valores da ultima partida
void arquivaHistorico2(FILE *historico, char h[], int v1, int v2, int p1, int p2)
{
	historico = fopen(h, "w");
	fprintf(historico, "%d %d\n", v1, v2);
	fprintf(historico, "%d %d\n", p1, p2);
	fclose(historico);
}

// funcao que mostra o historico na tela de derrota
void imprimeHistorico(FILE *historico, char h[], ALLEGRO_FONT *fonte3)
{
	historico = fopen(h, "r");	
	int p[4];
	int i;
	for(i = 0; i < 4; i++)
	{
		fscanf(historico,"%d", &p[i]);
	}

	al_draw_textf(fonte3, al_map_rgb(255,255,255), SCREEN_W / 2.0, SCREEN_H / 4, ALLEGRO_ALIGN_LEFT, "%s", "PLAYER 1 PLAYER 2");
	al_draw_textf(fonte3, al_map_rgb(255,255,255), SCREEN_W / 2.0 - 300, SCREEN_H / 4 + 60, ALLEGRO_ALIGN_LEFT, "%s", "VITORIAS");
	al_draw_textf(fonte3, al_map_rgb(255,255,255), SCREEN_W / 2.0 - 40, SCREEN_H / 4 + 60, ALLEGRO_ALIGN_LEFT, "%8d", p[0]);
	al_draw_textf(fonte3, al_map_rgb(255,255,255), SCREEN_W / 2.0 + 130, SCREEN_H / 4 + 60, ALLEGRO_ALIGN_LEFT, "%8d", p[1]);
	al_draw_textf(fonte3, al_map_rgb(255,255,255), SCREEN_W / 2.0 - 300, SCREEN_H / 4 + 100, ALLEGRO_ALIGN_LEFT, "%s", "ULTIMA PARTIDA");
	al_draw_textf(fonte3, al_map_rgb(255,255,255), SCREEN_W / 2.0 - 40, SCREEN_H / 4 + 100, ALLEGRO_ALIGN_LEFT, "%8d", p[2]);
	al_draw_textf(fonte3, al_map_rgb(255,255,255), SCREEN_W / 2.0 + 130, SCREEN_H / 4 + 100, ALLEGRO_ALIGN_LEFT, "%8d", p[3]);

	fclose(historico);
}

int main()
{
	//----------------------- CRIACAO DAS VARIAVEIS DA BIBLIOTECA ALLEGRO ---------------------------------------
	ALLEGRO_DISPLAY *display = NULL; // cria a tela
	ALLEGRO_EVENT_QUEUE *event_queue = NULL; // cria a fila de eventos
	ALLEGRO_TIMER *timer = NULL; // cria o timer
	ALLEGRO_BITMAP *t1 = NULL; // cria o bitmap do tanque 1
	ALLEGRO_BITMAP *t2 = NULL; // cria o bitmap do tanque 2
	ALLEGRO_BITMAP *s1 = NULL; // cria o bitmap do tiro 1
	ALLEGRO_BITMAP *s2 = NULL; // cria o bitmap do tiro 2
	ALLEGRO_BITMAP *play = NULL; // cria o bitmap do botao play
	ALLEGRO_BITMAP *diff = NULL; // cria o bitmap do botao de dificuldade
	ALLEGRO_BITMAP *playag = NULL; // cria o bitmap do botao play again
	ALLEGRO_BITMAP *menu = NULL; // cria o bitmap do botao menu
	ALLEGRO_BITMAP *back = NULL; // cria o bitmap do botao back
	ALLEGRO_BITMAP *gameover = NULL; // cria o bitmap do game over
	ALLEGRO_BITMAP *title = NULL; // cria o bitmap do titulo
	ALLEGRO_BITMAP *volume = NULL; // cria o bitmap do volume
	ALLEGRO_BITMAP *seta = NULL; // cria o bitmap da seta do volume
	ALLEGRO_BITMAP *options = NULL; // cria o bitmap do botao options
	ALLEGRO_BITMAP *skin1 = NULL; // cria o bitmap da skin1 do menu
	ALLEGRO_BITMAP *skin2 = NULL; // cria o bitmap da skin2 do menu
	ALLEGRO_BITMAP *skin3 = NULL; // cria o bitmap da skin3 do menu
	ALLEGRO_AUDIO_STREAM *trilha_sonora = NULL; // cria o sample do som da trilha sonora
	ALLEGRO_AUDIO_STREAM *trilha_final = NULL; // cria o sample do som da trilha final
	ALLEGRO_AUDIO_STREAM *trilha_pause = NULL; // cria o sample do som da trilha pause
	ALLEGRO_AUDIO_STREAM *trilha_menu = NULL; // cria o sample do som da trilha menu
	ALLEGRO_SAMPLE *tiro = NULL; // cria o sample do som do tiro
	ALLEGRO_SAMPLE *explosao = NULL; // cria o sample do som da explosao
	ALLEGRO_SAMPLE *ponto = NULL; // cria o sample do som de ponto
	
	int cenario = 0; // inicia o cenario como 0, que eh o default
	int state = 0; // inicia o estado no 0, que eh o menu
	
	
	FILE *historico; // inicializa o arquivo com o historico de partidas
	FILE *historico2; // inicializa o arquivo 2 com o historico de partidas

	char h[] = "historico.txt"; // criacao da variavel com o endereco do arquivo do historico
	char h2[] = "historico2.txt"; // criacao da variavel com o endereco do arquivo do historico2
	int vitorias[] = {0,0}; // variavel com as vitorias dos players
	
	// cria algumas variaveis auxiliares
	float i = 0; // auxilia no giro das skins
	int som_tiro1 = 0; // auxilia para identificar o momento do tiro e implementar o som
	int som_tiro2 = 0; // auxilia para identificar o momento do tiro e implementar o som
	int som_explosao1 = 0; // auxilia para identificar o momento de colisao do tiro e implementar o som
	int som_explosao2 = 0; // auxilia para identificar o momento de colisao do tiro e implementar o som
	int som_ponto1 = 0; // auxilia para identificar o momento de colisao do tiro com tank e implementar o som
	int som_ponto2 = 0; // auxilia para identificar o momento de colisao do tiro com tank e implementar o som
	float volume_trilha = 0.6; // sera utilizada para alteracao de volume da trilha
	float volume_sons = 0.4; // sera utilizada para alteracao de volume dos sons
	
	srand(time(NULL));
	//----------------------- ROTINAS DE INICIALIZACAO ---------------------------------------

	//inicializa o Allegro
	if(!al_init()) 
	{
	fprintf(stderr, "failed to initialize allegro!\n");
	return -1;
	}

	//inicializa o módulo de primitivas do Allegro
	if(!al_init_primitives_addon())
	{
	fprintf(stderr, "failed to initialize primitives!\n");
	return -1;
	}	

	//inicializa o modulo que permite carregar imagens no jogo
	if(!al_init_image_addon())
	{
	fprintf(stderr, "failed to initialize image module!\n");
	return -1;
	}
	
	//inicializa os modulos que permitem carregar sons no jogo
	al_install_audio();
	al_init_acodec_addon();
	al_reserve_samples(15);
	
	//----------------------- INICIALIZACAO DAS VARIAVEIS DO JOGO E SUAS AUXILIARES ---------------------------------------
	// cria as variaveis dos dados dos botoes do jogo 
	Bot playb, diffb, playagb, menub, backb, gameoverb, titleb, optionsb, volumeb, setab;
	
	// estabelece cada botao aa sua imagem
	play = al_load_bitmap("Imagens\\play.jpg");
	diff = al_load_bitmap("Imagens\\dificulty.jpg");
	playag = al_load_bitmap("Imagens\\playagain.jpg");
	menu = al_load_bitmap("Imagens\\menu.jpg");
	back = al_load_bitmap("Imagens\\back.jpg");
	gameover = al_load_bitmap("Imagens\\gameover.jpg");
	title = al_load_bitmap("Imagens\\title.jpg");
	options = al_load_bitmap("Imagens\\options.jpg");
	volume = al_load_bitmap("Imagens\\volume.jpg");
	seta = al_load_bitmap("Imagens\\seta.jpg");
	
	// atribuia todas os dados do botao aas suas respectivas variaveis 
	initBot(&playb, play);
	initBot(&diffb, diff);
	initBot(&playagb, playag);
	initBot(&menub, menu);
	initBot(&backb, back);
	initBot(&gameoverb, gameover);
	initBot(&titleb, title);
	initBot(&optionsb, options);
	initBot(&volumeb, volume);
	initBot(&setab, seta);
	initBots(&playb, &diffb, &playagb, &menub, &backb, &gameoverb, &titleb, &optionsb, &volumeb, &setab);
	
	// cria os bitmaps das skins do menu
	skin1 = al_create_bitmap(TANK_W, TANK_H);
	skin2 = al_create_bitmap(TANK_W, TANK_H);
	skin3 = al_create_bitmap(TANK_W, TANK_H);
	
	// cria as variaveis dos dados das skins do menu
	Skin skinv1;
	Skin skinv2;
	Skin skinv3;
	
	// atribuie os dados as respectivas variaveis de skins
	initSkins(&skinv1, &skinv2, &skinv3);
	
	// desenha as skins
	draw_skins(skin1, skin2, skin3);
	
	// testes de falhas na criacao de botoes e skins
	if(!play) 
	{
      fprintf(stderr, "failed to create play bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	if(!diff) 
	{
      fprintf(stderr, "failed to create diff bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	if(!playag) 
	{
      fprintf(stderr, "failed to create playag bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	if(!menu) 
	{
      fprintf(stderr, "failed to create menu bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	if(!back) 
	{
      fprintf(stderr, "failed to create back bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	if(!gameover) 
	{
      fprintf(stderr, "failed to create gameover bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	if(!skin1) 
	{
      fprintf(stderr, "failed to create skin1 bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	if(!skin2) 
	{
      fprintf(stderr, "failed to create skin2 bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	if(!skin3) 
	{
      fprintf(stderr, "failed to create skin3 bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	if(!title) 
	{
      fprintf(stderr, "failed to create title bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	if(!options) 
	{
      fprintf(stderr, "failed to create options bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	
	// inicializacao dos sons e das trilhas sonoras
	trilha_menu = al_load_audio_stream("Sons\\startmenu.ogg", 4, 1024);
	al_attach_audio_stream_to_mixer(trilha_menu, al_get_default_mixer());
	al_set_audio_stream_playmode(trilha_menu, ALLEGRO_PLAYMODE_LOOP);
	al_set_audio_stream_gain(trilha_menu, volume_trilha);
	
	trilha_pause = al_load_audio_stream("Sons\\sans.ogg", 4, 1024);
	al_attach_audio_stream_to_mixer(trilha_pause, al_get_default_mixer());
	al_set_audio_stream_playmode(trilha_pause, ALLEGRO_PLAYMODE_LOOP);
	al_set_audio_stream_gain(trilha_pause, volume_trilha);
	
	trilha_final = al_load_audio_stream("Sons\\finale.ogg", 4, 1024);
	al_attach_audio_stream_to_mixer(trilha_final, al_get_default_mixer());
	al_set_audio_stream_playmode(trilha_final, ALLEGRO_PLAYMODE_LOOP);
	al_set_audio_stream_gain(trilha_final, volume_trilha);
	
	trilha_sonora = al_load_audio_stream("Sons\\megalovania.ogg", 4, 1024);
	al_attach_audio_stream_to_mixer(trilha_sonora, al_get_default_mixer());
	al_set_audio_stream_playmode(trilha_sonora, ALLEGRO_PLAYMODE_LOOP);
	al_set_audio_stream_gain(trilha_sonora, volume_trilha);
	
	explosao = al_load_sample("Sons\\explosao.ogg");
	
	tiro = al_load_sample("Sons\\laser.ogg");
	
	ponto = al_load_sample("Sons\\ponto.ogg");
	
	if(!trilha_menu || !trilha_pause || !trilha_final || !explosao || !tiro || !ponto) 
	{
      fprintf(stderr, "failed to create audio streams!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	
	if(!explosao || !tiro || !ponto)
	{
      fprintf(stderr, "failed to create samples!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	
	// cria os tanques e os tiros
	Tank tank1;
	Tank tank2;
	Shot shot1;
	Shot shot2;
	
	// inicializa os tanques e tiros com suas configuracoes iniciais
	initTanks(&tank1, &tank2);
	
	initTiros1(&shot1, &shot2);
	draw_shots1(&shot1, &shot2);
	int aux1, aux2; // variavel auxiliar que so eh usada para que seja possivel iniciar a funcao seguinte
	initTiros(&shot1, &shot2, &tank1, &tank2, -CD, &aux1, &aux2);
	
	// cria os bitmaps dos tanques e dos tiros
	t1 = al_create_bitmap(TANK_W, TANK_H);
	t2 = al_create_bitmap(TANK_W, TANK_H);
	s1 = al_create_bitmap(shot1.width, shot1.heigth);
	s2 = al_create_bitmap(shot2.width, shot2.heigth);
	
	// verifica se os bitmaps foram criados
    if(!t1) 
	{
      fprintf(stderr, "failed to create t1 bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
    if(!t2) 
	{
      fprintf(stderr, "failed to create t2 bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
	}
	if(!s1) 
	{
      fprintf(stderr, "failed to create s1 bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
    if(!s2) 
	{
      fprintf(stderr, "failed to create s2 bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
	}
	
	//----------------------- ROTINAS DE INICIALIZACAO ---------------------------------------
	//cria um temporizador que incrementa uma unidade a cada 1.0/FPS segundos
	timer = al_create_timer(1.0 / FPS);
	if(!timer)
	{
	fprintf(stderr, "failed to create timer!\n");
	return -1;
	}

	//cria uma tela com dimensoes de SCREEN_W, SCREEN_H pixels
	display = al_create_display(SCREEN_W, SCREEN_H);
	if(!display) 
	{
	fprintf(stderr, "failed to create display!\n");
	al_destroy_timer(timer);
	return -1;
	}
	//instala o teclado
	if(!al_install_keyboard()) 
	{
	fprintf(stderr, "failed to install keyboard!\n");
	return -1;
	}
	//instala o mouse
	if(!al_install_mouse()) 
	{
	fprintf(stderr, "failed to initialize mouse!\n");
	return -1;
	}

	//inicializa o modulo allegro que carrega as fontes
	al_init_font_addon();

	//inicializa o modulo allegro que entende arquivos tff de fontes
	if(!al_init_ttf_addon()) 
	{
	fprintf(stderr, "failed to load tff font module!\n");
	return -1;
	}

	//carrega o arquivo fonte.ttf da fonte namco e define que sera usado os tamanhos 40 e 70
	ALLEGRO_FONT *fonte1 = al_load_font("Fontes\\fonte.ttf", 40, 3);
	ALLEGRO_FONT *fonte2 = al_load_font("Fontes\\fonte.ttf", 40, 3);
	ALLEGRO_FONT *fonte3 = al_load_font("Fontes\\alterebro-pixel-font.ttf", 70, 3);
	
	//verifica se houve erro na criacao das fontes
	if(fonte1 == NULL || fonte2 == NULL || fonte3 == NULL) 
	{
	fprintf(stderr, "font file does not exist or cannot be accessed!\n");
	}

	//cria a fila de eventos
	event_queue = al_create_event_queue();
	if(!event_queue) 
	{
	fprintf(stderr, "failed to create event_queue!\n");
	al_destroy_display(display);
	return -1;
	}

	//registra na fila os eventos de tela (ex: clicar no X na janela)
	al_register_event_source(event_queue, al_get_display_event_source(display));
	//registra na fila os eventos de tempo: quando o tempo altera de t para t+1
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	//registra na fila os eventos de teclado (ex: pressionar uma tecla)
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	//registra na fila os eventos de mouse (ex: clicar em um botao do mouse)
	al_register_event_source(event_queue, al_get_mouse_event_source());  	

	//inicia o temporizador
	al_start_timer(timer);
	
	int playing = 1;
	
	//----------------------- INICIO DO JOGO ---------------------------------------
	while(playing)
	{
	ALLEGRO_EVENT ev;
	// espera por um evento e o armazena na variavel de evento ev
	al_wait_for_event(event_queue, &ev);

	// se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
	if(ev.type == ALLEGRO_EVENT_TIMER) 
	{
		//----------------------- ESTADO 0 DO JOGO - MENU ---------------------------------------
		if(state == 0)
		{
			// zera as variaveis auxiliares de pontuacao
			som_ponto1 = 0;
			som_ponto2 = 0;
			
			// configuracao atual do som
			al_set_audio_stream_playing(trilha_sonora, 0);
			al_set_audio_stream_playing(trilha_pause, 0);
			al_set_audio_stream_playing(trilha_final, 0);
			al_set_audio_stream_playing(trilha_menu, 1);
			
			// os bitmaps dos tanks sao desfeitos para que ele seja feito com a escolha do jogador
			t1 = NULL;
			t2 = NULL;
			s1 = NULL;
			s2 = NULL;

			// sao desenhados os botoes do menu
			al_clear_to_color(al_map_rgb(0, 0, 0));
			al_draw_bitmap(play, playb.x, playb.y, 0);
			al_draw_bitmap(diff, diffb.x, diffb.y, 0);
			al_draw_bitmap(title, titleb.x, titleb.y, 0);
			al_draw_bitmap(options, optionsb.x, optionsb.y, 0);

			// caso uma dificuldade seja selecionada ela eh indicada com um circulo
			if(cenario == 0)
			{
				al_draw_filled_circle(diffb.x + diffb.width + 20, diffb.y + diffb.height / 6.0, 7, al_map_rgb(255,255,255));
			}
			else if(cenario == 1)
			{
				al_draw_filled_circle(diffb.x + diffb.width + 20, diffb.y + 3 * diffb.height / 6.0, 8, al_map_rgb(255,255,255));
			}
			else if(cenario == 2)
			{
				al_draw_filled_circle(diffb.x + diffb.width + 20, diffb.y + 5 * diffb.height / 6.0, 8, al_map_rgb(255,255,255));
			}
		}
		//----------------------- ESTADO 4 DO JOGO - OPTIONS ---------------------------------------
		if(state == 4)
		{
			// aumenta a variavel i para o giro das skins
			i += 0.3;
			// sao desenhadas as skins na tela
			al_clear_to_color(al_map_rgb(0, 0, 0));
			al_draw_rotated_bitmap(skin1, TANK_W / 2.0, TANK_H / 2.0, skinv1.x, skinv1.y, i * M_PI / 180.0, 0);
			al_draw_rotated_bitmap(skin2, TANK_W / 2.0, TANK_H / 2.0, skinv2.x, skinv2.y, i * M_PI / 180.0, 0);
			al_draw_rotated_bitmap(skin3, TANK_W / 2.0, TANK_H / 2.0, skinv3.x, skinv3.y, i * M_PI / 180.0, 0);
			
			// eh desenhado o icone de mudanca de volume
			al_draw_bitmap(volume, volumeb.x, volumeb.y, 0);
			al_draw_bitmap(seta, setab.x, setab.y, 0);
			
			// botao de back eh desenhado
			al_draw_bitmap(back, backb.x, backb.y, 0);
			
			// caso uma skin seja selecionada ela eh indicada com um quadrado envolta
			if(shot1.tipo == 0)
			{
				al_draw_rectangle(skinv1.x - TANK_W / 2.0 - 15, skinv1.y - TANK_H / 2.0 - 15, skinv1.x + TANK_W / 2.0 + 15, skinv1.y + TANK_H / 2.0 + 15, al_map_rgb(0,255,0), 3);
			}
			else if(shot1.tipo == 1)
			{
				al_draw_rectangle(skinv2.x - TANK_W / 2.0 - 15, skinv2.y - TANK_H / 2.0 - 15, skinv2.x + TANK_W / 2.0 + 15, skinv2.y + TANK_H / 2.0 + 15, al_map_rgb(0,255,0), 3);
			}
			else if(shot1.tipo == 2)
			{
				al_draw_rectangle(skinv3.x - TANK_W / 2.0 - 15, skinv3.y - TANK_H / 2.0 - 15, skinv3.x + TANK_W / 2.0 + 15, skinv3.y + TANK_H / 2.0 + 15, al_map_rgb(0,255,0), 3);
			}
		}
		//----------------------- ESTADO 1 DO JOGO - JOGO ---------------------------------------
		if(state == 1)
		{
			// configuracao atual do som
			al_set_audio_stream_playing(trilha_final, 0);
			al_set_audio_stream_playing(trilha_pause, 0);
			al_set_audio_stream_playing(trilha_menu, 0);
			al_set_audio_stream_playing(trilha_sonora, 1);
			
			// desenha o cenario escolhido
			if(cenario == 0)
			{
				draw_scenario0(display);
			}
			else if(cenario == 1)
			{
				draw_scenario1(display);
			}
			else if(cenario == 2)
			{
				draw_scenario2(display);
			}
			// desenha as fontes de fontuacao do jogo
			al_draw_textf(fonte1, tank1.cor, SCREEN_W / 2 - 120, 3, ALLEGRO_ALIGN_CENTER, "%d", tank1.pontos);
			al_draw_textf(fonte2, tank2.cor, SCREEN_W / 2 + 120, 3, ALLEGRO_ALIGN_CENTER, "%d", tank2.pontos);
		}
		//----------------------- ESTADO 2 DO JOGO - PAUSE ---------------------------------------
		if(state == 2)
		{
			// configuracao atual do som
			al_set_audio_stream_playing(trilha_sonora, 0);
			al_set_audio_stream_playing(trilha_pause, 1);

			// desenha os botoes da tela de pausa
			al_clear_to_color(al_map_rgb(0, 0, 0));
			al_draw_bitmap(playag, playagb.x, playagb.y, 0);
			al_draw_bitmap(menu, menub.x, menub.y, 0);
			al_draw_bitmap(back, backb.x, backb.y, 0);
			al_draw_bitmap(volume, volumeb.x, volumeb.y, 0);
			al_draw_bitmap(seta, setab.x, setab.y, 0);
		}
		//----------------------- ESTADO 0 DO JOGO - GAMEOVER ---------------------------------------
		if(state == 3)
		{
			// zera as variaveis auxiliares de pontuacao
			som_ponto1 = 0;
			som_ponto2 = 0;
			
			// configuracao atual do som
			
			al_set_audio_stream_playing(trilha_sonora, 0);
			al_set_audio_stream_playing(trilha_final, 1);

			// desenha os botoes da tela de derrota
			al_clear_to_color(al_map_rgb(0, 0, 0));
			al_draw_bitmap(playag, playagb.x, playagb.y, 0);
			al_draw_bitmap(menu, menub.x, menub.y, 0);
			al_draw_bitmap(gameover, gameoverb.x, gameoverb.y, 0);
			al_draw_bitmap(volume, volumeb.x, volumeb.y, 0);
			al_draw_bitmap(seta, setab.x, setab.y, 0);
			
			// desenha o historico na tela
			imprimeHistorico(historico2, h2, fonte3);
		}
		
		// quando alguem conseguir 5 pontos os pontos sao zerados e vai para tela de derrota
		if(tank1.pontos > 4 || tank2.pontos > 4)
		{
			if(tank1.pontos == 5)
			{
				vitorias[0]++;
			}
			if(tank2.pontos == 5)
			{
				vitorias[1]++;
			}
			// arquiva-se os historicos
			arquivaHistorico(historico, h, vitorias[0], vitorias[1], tank1.pontos, tank2.pontos);
			arquivaHistorico2(historico2, h2, vitorias[0], vitorias[1], tank1.pontos, tank2.pontos);
			// desliga as variaveis deatirar e pontos
			shot1.shoot = 0;
			shot2.shoot = 0;
			tank1.pontos = 0;
			tank2.pontos = 0;	
			state = 3; // passa direto para o estado de derrota
		}
		//----------------------- ESTADO 1 DO JOGO - JOGO ---------------------------------------
		if(state == 1)
		{
			// caso haja explosao para o tiro, toca o som de explosao, e desenha a explosao novamente
			if(shot1.explosao == 1)
			{
				al_play_sample(explosao, volume_sons, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
				if(shot1.run == 1) verificaExplosao(&shot1, RAIO_EXPLOSAO, &tank1, &tank2);
				al_draw_filled_circle(shot1.x, shot1.y, RAIO_EXPLOSAO, al_map_rgb(255, 0, 0));
				shot1.explosao = 0;
				shot1.run = 0;
			}
			if(shot2.explosao == 1)
			{
				al_play_sample(explosao, volume_sons, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
				if(shot2.run == 1) verificaExplosao(&shot2, RAIO_EXPLOSAO, &tank1, &tank2);
				al_draw_filled_circle(shot2.x, shot2.y, RAIO_EXPLOSAO, al_map_rgb(255, 0, 0));
				shot2.explosao = 0;
				shot2.run = 0;
			}
			// configuracoes de movimento do tank
			moveLeftRight(&tank1, &tank2);
			calculaSenCos(&tank1, &tank2);
			obstaculo(&tank1, &tank2, &shot1, &shot2, cenario);
			moveUpDown(&tank1, &tank2);
			obstaculoVento(&tank1, &tank2, cenario);
			// desenha os tanques
			al_draw_rotated_bitmap(t1, TANK_W / 2, TANK_H / 2, tank1.x, tank1.y, tank1.degree * M_PI / 180, 0);
			al_draw_rotated_bitmap(t2, TANK_W / 2, TANK_H / 2, tank2.x, tank2.y, tank2.degree * M_PI / 180, 0);
			// desenha os tiros caso estejam ocorrendo
			if(shot1.run == 1)
			{
				al_draw_rotated_bitmap(s1, shot1.width / 2, shot1.heigth / 2, shot1.x, shot1.y, shot1.degree * M_PI / 180, 0);
				shot1.x += shot1.nextx;
				shot1.y += shot1.nexty;
				som_explosao1 = 1;
			}
			if(shot2.run == 1)
			{
				al_draw_rotated_bitmap(s2, shot2.width / 2, shot2.heigth / 2, shot2.x, shot2.y, shot2.degree * M_PI / 180, 0);
				shot2.x += shot2.nextx;
				shot2.y += shot2.nexty;
				som_explosao2 = 1;
			}
			// implementa o som do tiro do tank
			if(shot1.run == 1 && som_tiro1 == 1)
			{
				al_play_sample(tiro, volume_sons, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
				som_tiro1 = 0;
			}
			if(shot2.run == 1 && som_tiro2 == 1)
			{
				al_play_sample(tiro, volume_sons, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
				som_tiro2 = 0;
			}
			// implementa o som da explosao do tiro do tank
			if(som_explosao1 == 1 && shot1.run == 0 && shot1.tipo != 2)
			{
				al_play_sample(explosao, volume_sons, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
				som_explosao1 = 0;
			}
			if(som_explosao2 == 1 && shot2.run == 0 && shot2.tipo != 2)
			{
				al_play_sample(explosao, volume_sons, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
				som_explosao2 = 0;
			}
			// implementa o som da pontuacao
			if(som_ponto1 != tank1.pontos)
			{
				al_play_sample(ponto, volume_sons, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
			}
			if(som_ponto2 != tank2.pontos)
			{
				al_play_sample(ponto, volume_sons, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
			}
			som_ponto1 = tank1.pontos;
			som_ponto2 = tank2.pontos;
			// configuracao do tempo de explosao no modo tank canhao
			if(shot1.run == 1 && shot1.tipo == 2 && (float)(al_get_timer_count(timer)/FPS) - shot1.tempo >= CD_EXPLOSAO)
			{
				al_draw_filled_circle(shot1.x, shot1.y, 30, al_map_rgb(255, 0, 0));
				shot1.explosao = 1;
			}
			if(shot2.run == 1 && shot2.tipo == 2 && (float)(al_get_timer_count(timer)/FPS) - shot2.tempo >= CD_EXPLOSAO)
			{
				al_draw_filled_circle(shot2.x, shot2.y, 30, al_map_rgb(255, 0, 0));
				shot2.explosao = 1;
			}
			
			// configuracoes de movimento do tiro
			initTiros(&shot1, &shot2, &tank1, &tank2, (float)(al_get_timer_count(timer)/FPS), &som_tiro1, &som_tiro2);
			tipoTiro(&shot1);
			tipoTiro(&shot2);
		}
		// atualiza a tela (quando houver algo para mostrar)
		al_flip_display();

		// if(al_get_timer_count(timer)%(int)FPS == 0) printf("\n%d segundos se passaram...", (int)(al_get_timer_count(timer)/FPS));
	}
	// se o tipo de evento for o fechamento da tela (clique no x da janela)
	else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) 
	{
		playing = 0;
	}
	// se o tipo de evento for um clique de mouse
	else if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) 
	{
		// printf("\nmouse clicado em: %d, %d", ev.mouse.x, ev.mouse.y);
		//----------------------- ESTADO 0 DO JOGO - MENU ---------------------------------------
		if(state == 0)
		{
			// estado 0: menu
			// verifica o cenario escolhido pelos jogadores
			if(ev.mouse.x > diffb.x && ev.mouse.x < diffb.x + diffb.width && ev.mouse.y >= diffb.y && ev.mouse.y <= diffb.y + diffb.height / 3.0)
			{
				cenario = 0;
			}
			else if(ev.mouse.x > diffb.x && ev.mouse.x < diffb.x + diffb.width && ev.mouse.y >= diffb.y + diffb.height / 3.0 && ev.mouse.y <= diffb.y + 2 * diffb.height / 3.0)
			{
				cenario = 1;
			}
			else if(ev.mouse.x > diffb.x && ev.mouse.x < diffb.x + diffb.width && ev.mouse.y >= diffb.y + 2 * diffb.height / 3.0 && ev.mouse.y <= diffb.y + diffb.height)
			{
				cenario = 2;
			}
			
			// verifica se o botao options foi apertado
			if(ev.mouse.x > optionsb.x && ev.mouse.x < optionsb.x + diffb.width && ev.mouse.y >= optionsb.y && ev.mouse.y <= optionsb.y + optionsb.height)
			{
				state = 4; // passa ao estado de options
			}
			
			// verifica o botao de play, quando clicado inicia o jogo com as configuracoes estabelecidas
			if(ev.mouse.x > playb.x && ev.mouse.x < playb.x + playb.width && ev.mouse.y >= playb.y && ev.mouse.y <= playb.y + playb.height)
			{
				state = 1; // inicia o jogo
				// cria os tanks e tiros de acordo com as configuracoes
				t1 = al_create_bitmap(TANK_W, TANK_H);
				t2 = al_create_bitmap(TANK_W, TANK_H);
				s1 = al_create_bitmap(shot1.width, shot1.heigth);
				s2 = al_create_bitmap(shot2.width, shot2.heigth);
				draw_tanks(t1, t2, &tank1, &tank2, &shot1, &shot2, cenario);
				al_set_target_bitmap(al_get_backbuffer(display));
				draw_shots(s1, s2, &shot1, &shot2);
				al_set_target_bitmap(al_get_backbuffer(display));
				
				trilha_sonora = NULL;
				
				// seleciona uma trilha sonora aleatoria pra um jogo
				int trilha = random();
				if(trilha == 0) trilha_sonora = al_load_audio_stream("Sons\\megalovania.ogg", 4, 1024);
				else if(trilha == 1) trilha_sonora = al_load_audio_stream("Sons\\deathbyglamour.ogg", 4, 1024);
				else if(trilha == 2) trilha_sonora = al_load_audio_stream("Sons\\spiderdance.ogg", 4, 1024);
				else if(trilha == 3) trilha_sonora = al_load_audio_stream("Sons\\battleagainst.ogg", 4, 1024);
				else if(trilha == 4) trilha_sonora = al_load_audio_stream("Sons\\asgore.ogg", 4, 1024);
				
				al_attach_audio_stream_to_mixer(trilha_sonora, al_get_default_mixer());
				al_set_audio_stream_playmode(trilha_sonora, ALLEGRO_PLAYMODE_LOOP);
				al_set_audio_stream_gain(trilha_sonora, volume_trilha);
			}
		}
		//----------------------- ESTADO 2 DO JOGO - PAUSE ---------------------------------------
		if(state == 2)
		{
			// estado 2: pausa
			// verifica o que os jogadores querem com a pausa:
			// inicar o jogo novamente
			if(ev.mouse.x > playagb.x && ev.mouse.x < playagb.x + playagb.width && ev.mouse.y >= playagb.y && ev.mouse.y <= playagb.y + playagb.height)
			{
				som_explosao1 = 0;
				som_explosao2 = 0;
				shot1.explosao = 0;
				shot2.explosao = 0;
				shot1.run = 0;
				shot2.run = 0;
				initTanks(&tank1, &tank2);
				state = 1;
			}
			// voltar ao menu
			if(ev.mouse.x > menub.x && ev.mouse.x < menub.x + menub.width && ev.mouse.y >= menub.y && ev.mouse.y <= menub.y + menub.height)
			{
				som_explosao1 = 0;
				som_explosao2 = 0;
				shot1.explosao = 0;
				shot2.explosao = 0;
				shot1.run = 0;
				shot2.run = 0;
				initTanks(&tank1, &tank2);
				state = 0;
			}
			// aumenta e diminui o volume caso clique na barra de volume
			if(ev.mouse.x > volumeb.x + 12 && ev.mouse.x < volumeb.x + volumeb.width - 13 && ev.mouse.y >= volumeb.y && ev.mouse.y <= volumeb.y + 164)
			{
				setab.y = ev.mouse.y - 5;
				volume_trilha = 1.2 - (ev.mouse.y - volumeb.y) / 136.0;
				volume_sons = 0.8 - (ev.mouse.y - volumeb.y) / 197.0;
				al_set_audio_stream_gain(trilha_menu, volume_trilha);
				al_set_audio_stream_gain(trilha_final, volume_trilha);
				al_set_audio_stream_gain(trilha_sonora, volume_trilha);
				al_set_audio_stream_gain(trilha_pause, volume_trilha);
			}
			// voltar ao jogo
			if(ev.mouse.x > backb.x && ev.mouse.x < backb.x + backb.width && ev.mouse.y >= backb.y && ev.mouse.y <= backb.y + backb.height)
			{
				state = 1;
			}
		}
		//----------------------- ESTADO 3 DO JOGO - GAMEOVER ---------------------------------------
		if(state == 3)
		{
			// verifica o que os jogadores querem com a pausa:
			// inicar o jogo novamente
			if(ev.mouse.x > playagb.x && ev.mouse.x < playagb.x + playagb.width && ev.mouse.y >= playagb.y && ev.mouse.y <= playagb.y + playagb.height)
			{
				som_explosao1 = 0;
				som_explosao2 = 0;
				shot1.explosao = 0;
				shot2.explosao = 0;
				shot1.run = 0;
				shot2.run = 0;
				initTanks(&tank1, &tank2);
				state = 1;
			}
			// voltar ao menu
			if(ev.mouse.x > menub.x && ev.mouse.x < menub.x + menub.width && ev.mouse.y >= menub.y && ev.mouse.y <= menub.y + menub.height)
			{
				som_explosao1 = 0;
				som_explosao2 = 0;
				shot1.explosao = 0;
				shot2.explosao = 0;
				shot1.run = 0;
				shot2.run = 0;
				initTanks(&tank1, &tank2);
				state = 0;
			}
			// aumenta e diminui o volume caso clique na barra de volume
			if(ev.mouse.x > volumeb.x + 12 && ev.mouse.x < volumeb.x + volumeb.width - 13 && ev.mouse.y >= volumeb.y && ev.mouse.y <= volumeb.y + 164)
			{
				setab.y = ev.mouse.y - 5;
				volume_trilha = 1.2 - (ev.mouse.y - volumeb.y) / 136.0;
				volume_sons = 0.8 - (ev.mouse.y - volumeb.y) / 197.0;
				al_set_audio_stream_gain(trilha_menu, volume_trilha);
				al_set_audio_stream_gain(trilha_final, volume_trilha);
				al_set_audio_stream_gain(trilha_sonora, volume_trilha);
				al_set_audio_stream_gain(trilha_pause, volume_trilha);
			}
		}
		//----------------------- ESTADO 4 DO JOGO - OPTIONS ---------------------------------------
		if(state == 4)
		{
			// verifica a skin escolhida pelos jogadores
			if(pow(pow(ev.mouse.x - skinv1.x, 2) + pow(ev.mouse.y - skinv1.y, 2), 1 / 2.0) <= skinv1.raio)
			{
				shot1.tipo = 0;
				shot2.tipo = 0;
				draw_shots1(&shot1, &shot2);
			}
			else if(pow(pow(ev.mouse.x - skinv2.x, 2) + pow(ev.mouse.y - skinv2.y, 2), 1 / 2.0) <= skinv2.raio)
			{
				shot1.tipo = 1;
				shot2.tipo = 1;
				draw_shots1(&shot1, &shot2);
			}
			else if(pow(pow(ev.mouse.x - skinv3.x, 2) + pow(ev.mouse.y - skinv3.y, 2), 1 / 2.0) <= skinv3.raio)
			{
				shot1.tipo = 2;
				shot2.tipo = 2;
				draw_shots1(&shot1, &shot2);
			}
			
			// aumenta e diminui o volume caso clique na barra de volume
			if(ev.mouse.x > volumeb.x + 12 && ev.mouse.x < volumeb.x + volumeb.width - 13 && ev.mouse.y >= volumeb.y && ev.mouse.y <= volumeb.y + 164)
			{
				setab.y = ev.mouse.y - 5;
				volume_trilha = 1.2 - (ev.mouse.y - volumeb.y) / 136.0;
				volume_sons = 0.8 - (ev.mouse.y - volumeb.y) / 197.0;
				al_set_audio_stream_gain(trilha_menu, volume_trilha);
				al_set_audio_stream_gain(trilha_final, volume_trilha);
				al_set_audio_stream_gain(trilha_sonora, volume_trilha);
				al_set_audio_stream_gain(trilha_pause, volume_trilha);
			}
			
			// voltar ao menu
			if(ev.mouse.x > backb.x && ev.mouse.x < backb.x + backb.width && ev.mouse.y >= backb.y && ev.mouse.y <= backb.y + backb.height)
			{
				state = 0;
			}
		}
		
	}
	// se o tipo de evento for um pressionar de uma tecla
	else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) 
	{
		//----------------------- ESTADO 1 DO JOGO - JOGO ---------------------------------------
		if(state == 1)
		{
			//verifica a tecla clicada
			verificaTeclaDown(&tank1, &tank2, &shot1, &shot2, ev, &state);
		}
	}	 
	else if(ev.type == ALLEGRO_EVENT_KEY_UP) 
	{
		//----------------------- ESTADO 1 DO JOGO - JOGO ---------------------------------------
		if(state == 1)
		{
			//verifica a tecla soltada
			verificaTeclaUp(&tank1, &tank2, &shot1, &shot2, ev);
		}
	}
	} //fim do while

	//procedimentos de fim de jogo (fecha a tela, limpa a memoria, etc)
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
	al_destroy_bitmap(t1);
	al_destroy_bitmap(t2);
	al_destroy_font(fonte1);
	al_destroy_font(fonte2);
	al_destroy_bitmap(play);
	al_destroy_bitmap(diff);
	al_destroy_bitmap(playag);
	al_destroy_bitmap(menu);
	al_destroy_bitmap(back);
	al_destroy_bitmap(title);
	al_destroy_bitmap(gameover);
	al_destroy_bitmap(skin1);
	al_destroy_bitmap(skin2);
	al_destroy_bitmap(skin3);
	al_destroy_audio_stream(trilha_sonora);
	al_destroy_audio_stream(trilha_pause);
	al_destroy_audio_stream(trilha_menu);
	al_destroy_audio_stream(trilha_final);
	al_destroy_sample(explosao);
	al_destroy_sample(tiro);
	al_destroy_sample(ponto);
	
	return 0;
}