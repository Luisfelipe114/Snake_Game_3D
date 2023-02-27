/*
 * Institui��o: Universidade Federal do Piau�(UFPI)
 * Computacao Grafica
 * Projeto: Paint 2d
 * Autor: Luis Felipe Cabral Brito
 */

// Bibliotecas utilizadas pelo OpenGL
#ifdef __APPLE__
    #define GL_SILENCE_DEPRECATION
    #include <GLUT/glut.h>
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
#else
    #include <GL/glut.h>
    #include <GL/gl.h>
    #include <GL/glu.h>
#endif

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <forward_list>
#include "glut_text.h"
#include <vector>

using namespace std;

// Variaveis Globais
#define ESC 27
#define ENTER 13
#define p 112

//Enumeracao com os tipos de formas geometricas
enum tipo_forma{LIN = 0, TRI, RET, POL, CIR }; // Linha, Triangulo, Retangulo Poligono, Circulo

//Verifica se foi realizado o primeiro clique do mouse
bool click1, clickQuad, clickTri2, clickTri3, clickPol, clickCir = false;

bool concluir_poligono = false;

//Quantidade de vertices do poligono
int count_vertices_pol = 0;

//Coordenadas da posicao atual do mouse
int m_x, m_y;

//Coordenadas do primeiro clique e do segundo clique do mouse
int x_1, y_1, x_2, y_2, x_3, y_3;

//Indica o tipo de forma geometrica ativa para desenhar
int modo = LIN;

//Largura e altura da janela
int width = 512, height = 512;

// Definicao de vertice
struct vertice{
    int x;
    int y;
};

// Definicao das formas geometricas
struct forma{
    int tipo;
    forward_list<vertice> v; //lista encadeada de vertices
};

// Lista encadeada de formas geometricas
forward_list<forma> formas;

// Funcao para armazenar uma forma geometrica na lista de formas
// Armazena sempre no inicio da lista
void pushForma(int tipo){
    forma f;
    f.tipo = tipo;
    formas.push_front(f);
}

// Funcao para armazenar um vertice na forma do inicio da lista de formas geometricas
// Armazena sempre no inicio da lista
void pushVertice(int x, int y){
    vertice v;
    v.x = x;
    v.y = y;
    formas.front().v.push_front(v);
}

//Fucao para armazenar uma Linha na lista de formas geometricas
void pushLinha(int x_1, int y1, int x_2, int y_2){
    pushForma(LIN);
    pushVertice(x_1, y1);
    pushVertice(x_2, y_2);
}

void pushQuadrilatero(int x_1, int y1, int x_2, int y_2) {
	pushForma(RET);
	pushVertice(x_1, y1);
    pushVertice(x_2, y_2);
}

void pushTriangulo(int x_1, int y1, int x_2, int y_2, int x_3, int y_3) {
	pushForma(TRI);
	pushVertice(x_1, y1);
    pushVertice(x_2, y_2);
    pushVertice(x_3, y_3);
}

//Fucao para armazenar um Poligono na lista de formas geometricas
void pushPOL(std::vector<vertice> &pts){
	pushForma(POL);
    for (std::vector<vertice>::iterator v = pts.begin(); v != pts.end(); ++v) {
        pushVertice(v->x, v->y);
    }
}

// Função para armazenar uma circunferência na lista de formas geométricas
void pushCircunferencia(int x_1, int y_1, int x_2, int y_2) {
    pushForma(CIR);
    pushVertice(x_1, y_1);
    pushVertice(x_2, y_2); 
}


void menuQuad(int op){
    switch (op)
    {
    case 0:
        modo = RET;
        //preenchido = false;
        break;
    case 1:
        modo = RET;
        //preenchido = true;
        break;
    
    default:
        break;
    }
}

void menuTri(int op){
    switch (op)
    {
    case 0:
        modo = TRI;
        //preenchido = false;
        break;
    case 1:
        modo = TRI;
        //preenchido = true;
        break;
    
    default:
        break;
    }
}

void menuPol(int op){
    switch (op)
    {
    case 0:
        modo = POL;
        //preenchido = false;
        break;
    case 1:
        modo = POL;
        //preenchido = true;
        break;
    
    default:
        break;
    }
}

void menuCir(int op){
    switch (op)
    {
    case 0:
        modo = CIR;
        //preenchido = false;
        break;
    case 1:
        modo = CIR;
        //preenchido = true;
        break;
    
    default:
        break;
    }
}

void menuTrans(int op){
    switch (op)
    {
    case 0:
        //translacao
        break;
    case 1:
        /* escala */
        break;
    case 2:
        /* cisalhamento */
        break;
    case 3:
        /* reflexao */
        break;
    case 4:
        /* rotacao */
        break;    
    default:
        break;
    }
}

void menuPrincipal(int op){
    switch (op)
    {
    case 0:
        modo = LIN;
        break;
    
    default:
        break;
    }
}

/*
 * Declaracoes antecipadas (forward) das funcoes (assinaturas das funcoes)
 */
void init(void);
void reshape(int w, int h);
void display(void);
void menu_popup();
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void mousePassiveMotion(int x, int y);
void drawPixel(int x, int y);
// Funcao que percorre a lista de formas geometricas, desenhando-as na tela
void drawFormas();
// Funcao que implementa o Algoritmo Imediato para rasterizacao de segmentos de retas
void retaImediata(double x_1,double y_1,double x_2,double y_2);

//altenativa A e B
void bresenhamLine(int x_1, int y_1, int x_2, int y_2);

//Funcao para desenhar quadrilatero(alternativa C)
void quadrilatero(int x_1, int y_1, int x_2, int y_2);

void triangulo(int x_1, int y_1, int x_2, int y_2, int x_3, int y_3);

void poligono(std::vector<int>& x, std::vector<int>& y);

void bresenhamCircle(int xc, int yc, int r);

/*
 * Funcao principal
 */
int main(int argc, char** argv){
    glutInit(&argc, argv); // Passagens de parametro C para o glut
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB); //Selecao do Modo do Display e do Sistema de cor
    glutInitWindowSize (width, height);  // Tamanho da janela do OpenGL
    glutInitWindowPosition (100, 100); //Posicao inicial da janela do OpenGL
    glutCreateWindow ("Computacao Grafica: Paint"); // Da nome para uma janela OpenGL
    init(); // Chama funcao init();
    glutReshapeFunc(reshape); //funcao callback para redesenhar a tela
    glutKeyboardFunc(keyboard); //funcao callback do teclado
    glutMouseFunc(mouse); //funcao callback do mouse
    glutPassiveMotionFunc(mousePassiveMotion); //fucao callback do movimento passivo do mouse
    glutDisplayFunc(display); //funcao callback de desenho
    
    // Define o menu pop-up
    /*
    glutCreateMenu(menu_popup);
    glutAddMenuEntry("Linha", LIN);
//    glutAddMenuEntry("Retangulo", RET);
    glutAddMenuEntry("Sair", 0);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    */

    menu_popup();
    
    glutMainLoop(); // executa o loop do OpenGL
    return EXIT_SUCCESS; // retorna 0 para o tipo inteiro da funcao main();
}

/*
 * Inicializa alguns parametros do GLUT
 */
void init(void){
    glClearColor(1.0, 1.0, 1.0, 1.0); //Limpa a tela com a cor branca;
}

/*
 * Ajusta a projecao para o redesenho da janela
 */
void reshape(int w, int h)
{
	// Muda para o modo de projecao e reinicializa o sistema de coordenadas
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Definindo o Viewport para o tamanho da janela
	glViewport(0, 0, w, h);
	
	width = w;
	height = h;
    glOrtho (0, w, 0, h, -1 ,1);  

   // muda para o modo de desenho
	glMatrixMode(GL_MODELVIEW);
 	glLoadIdentity();

}

/*
 * Controla os desenhos na tela
 */
void display(void){
    glClear(GL_COLOR_BUFFER_BIT); //Limpa o buffer de cores e reinicia a matriz
    glColor3f (0.0, 0.0, 0.0); // Seleciona a cor default como preto
    drawFormas(); // Desenha as formas geometricas da lista
    //Desenha texto com as coordenadas da posicao do mouse
    draw_text_stroke(0, 0, "(" + to_string(m_x) + "," + to_string(m_y) + ")", 0.2);
    glutSwapBuffers(); // manda o OpenGl renderizar as primitivas

}

/*
 * Controla o menu pop-up
 */

void menu_popup(){
    int menu, submenu1, submenu2, submenu3, submenu4;
    //int submenu5;

    submenu1 = glutCreateMenu(menuQuad);
    glutAddMenuEntry("Nao preenchida", 0);
    glutAddMenuEntry("Preenchida", 1);

    submenu2 = glutCreateMenu(menuTri);
    glutAddMenuEntry("Nao preenchida", 0);
    glutAddMenuEntry("Preenchida", 1);

    submenu3 = glutCreateMenu(menuPol);
    glutAddMenuEntry("Nao preenchida", 0);
    glutAddMenuEntry("Preenchida", 1);

    submenu4 = glutCreateMenu(menuCir);
    glutAddMenuEntry("Nao preenchida", 0);
    glutAddMenuEntry("Preenchida", 1);

	/*
    submenu5 = glutCreateMenu(menuTrans);
    glutAddMenuEntry("Translação", 0);
    glutAddMenuEntry("Escala", 1);
    glutAddMenuEntry("Cisalhamento", 2);
    glutAddMenuEntry("Reflexão", 3);
    glutAddMenuEntry("Rotação", 4);
    */

    menu = glutCreateMenu(menuPrincipal);
    glutAddMenuEntry("Reta", LIN);
    glutAddSubMenu("Quadrilatero", submenu1);
    glutAddSubMenu("Triangulo", submenu2);
    glutAddSubMenu("Poligono", submenu3);
    glutAddSubMenu("Circunferencia", submenu4);
    //glutAddSubMenu("Transformação", submenu5);

    glutAttachMenu(GLUT_RIGHT_BUTTON);
}



/*
 * Controle das teclas comuns do teclado
 */
void keyboard(unsigned char key, int x, int y){
    switch (key) { // key - variavel que possui valor ASCII da tecla precionada
        case ESC: exit(EXIT_SUCCESS); break;
        case p: { // 'p' para finalizar o polígono
            if(count_vertices_pol >= 4) {
                clickPol = false;
                concluir_poligono = true;
            }
            break;
        }
    
    }
}

/*
 * Controle dos botoes do mouse
 */
void mouse(int button, int state, int x, int y){
    switch (button) {
        case GLUT_LEFT_BUTTON: {
			switch(modo){
                	case LIN: {
                        if (state == GLUT_DOWN) {
	                        if(click1){
	                            x_2 = x;
	                            y_2 = height - y - 1;
	                            pushLinha(x_1, y_1, x_2, y_2);
	                            click1 = false;
	                            glutPostRedisplay();
	                        }else{
	                            click1 = true;
	                            x_1 = x;
	                            y_1 = height - y - 1;
	                        }
	                    }
					    break;
                    }
					case RET: {
                        if (state == GLUT_DOWN) {
	                        if(clickQuad){
	                            x_2 = x;
	                            y_2 = height - y - 1;
	                            pushQuadrilatero(x_1, y_1, x_2, y_2);
	                            clickQuad = false;
	                            glutPostRedisplay();
	                        }else{
	                            clickQuad = true;
	                            x_1 = x;
	                            y_1 = height - y - 1;
						   	}
	  	  	           }	
                	    break;
                    }
                    case TRI: {
                        if (state == GLUT_DOWN) {
	                        if(!clickTri2 && !clickTri3){
                                clickTri2 = true;
					            x_1 = x;
					            y_1 = height - y - 1;
                            } else if(clickTri2) {
                                x_2 = x;
                                y_2 = height - y - 1;
                                clickTri2 = false;
                                clickTri3 = true;
                                // glutPostRedisplay();
                            } else if(clickTri3) {
                                x_3 = x;
                                y_3 = height - y - 1;
                                pushTriangulo(x_1, y_1, x_2, y_2, x_3, y_3);
                                clickTri2 = false;
                                clickTri3 = false;
                                glutPostRedisplay();
                            }
	  	  	           }	
                	    break;
                    }
                    case POL: {
                        if (state == GLUT_DOWN){
                            vertice vi = {x, height - y - 1};
                            vector<vertice> pol; 
                            if(!clickPol){ 
                                count_vertices_pol = 0;
                                concluir_poligono = false;
                                pol.push_back(vi);
                                pushPOL(pol);
                                clickPol = true;
                                count_vertices_pol++;
                            }else{ 
                                formas.front().v.push_front(vi);
                                pol.clear();
                                glutPostRedisplay();	
                                count_vertices_pol++;
                            }
                        }
                        break;
                    }
                    case CIR: {
                        if(!clickCir){
                            x_1 = x;
                            y_1 = height - y - 1;
                            clickCir = true;
                        }else {
                            x_2 = x;
                            y_2 = height - y - 1;
                            pushCircunferencia(x_1, y_1, x_2, y_2);
                            clickCir = false;
                            glutPostRedisplay();
                        }
                    }
                    
            }
		}
        break;

//        case GLUT_MIDDLE_BUTTON:
//            if (state == GLUT_DOWN) {
//                glutPostRedisplay();
//            }
//        break;
//
//        case GLUT_RIGHT_BUTTON:
//            if (state == GLUT_DOWN) {
//                glutPostRedisplay();
//            }
//        break;
            
    }
}

/*
 * Controle da posicao do cursor do mouse
 */
void mousePassiveMotion(int x, int y){
    m_x = x; m_y = height - y - 1;
    glutPostRedisplay();
}

/*
 * Funcao para desenhar apenas um pixel na tela
 */
void drawPixel(int x, int y){
    glBegin(GL_POINTS); // Seleciona a primitiva GL_POINTS para desenhar
	glVertex2i(x, y);
    glEnd();  // indica o fim do ponto
}

/*
 *Funcao que desenha a lista de formas geometricas
 */
void drawFormas(){
    int raio = sqrt(pow((x_1 - m_x), 2) + pow((y_1 - m_y), 2));
    //Apos o primeiro clique, desenha a reta com a posicao atual do mouse
    if(click1) bresenhamLine(x_1, y_1, m_x, m_y);
    else if(clickQuad) quadrilatero(x_1, y_1, m_x, m_y);
    else if(clickTri2) bresenhamLine(x_1, y_1, m_x, m_y);
    else if(clickTri3) triangulo(x_1, y_1, x_2, y_2 ,m_x, m_y);
    else if(clickPol) bresenhamLine(formas.front().v.front().x, formas.front().v.front().y, m_x, m_y);
    else if(clickCir) bresenhamCircle(m_x, m_y, raio);
    
    //Percorre a lista de formas geometricas para desenhar
    for(forward_list<forma>::iterator f = formas.begin(); f != formas.end(); f++){
        switch (f->tipo) {
            case LIN: 
			{
				int i = 0, x[2], y[2];
                //Percorre a lista de vertices da forma linha para desenhar
                for(forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++, i++){
                    x[i] = v->x;
                    y[i] = v->y;
                }
                //Desenha o segmento de reta apos dois cliques
                bresenhamLine(x[0], y[0], x[1], y[1]);
            	break;
            }
            case RET: 
			{
				int r = 0, x[2], y[2];
                //Percorre a lista de vertices da forma linha para desenhar
                for(forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++, r++){
                    x[r] = v->x;
                    y[r] = v->y;
                }
                //Desenha o quadrilatero
                quadrilatero(x[0], y[0], x[1], y[1]);
                break;
            }
            case TRI: {
                int t = 0, x[3], y[3];
                 //Percorre a lista de vertices da forma linha para desenhar
                for(forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++, t++){
                    x[t] = v->x;
                    y[t] = v->y;
                }
                //Desenha o triangulo
                triangulo(x[0], y[0], x[1], y[1], x[2], y[2]);
                break;
            }
            case POL: {
                int pg = 0;
                std::vector<int> x;
	            std::vector<int> y;
                for(forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++, pg++)
				{
					x.push_back(v->x); 
					y.push_back(v->y); 
					// printf("x: %d, y: %d \n",v->x, v->y);
				}
				poligono(x, y);
                break;
            }
        
            case CIR: { // Desenha um círculo
                int c = 0, x[2], y[2];
                 //Percorre a lista de vertices da forma linha para desenhar
                for(forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++, c++){
                    x[c] = v->x;
                    y[c] = v->y;
                }
                raio = sqrt(pow((x[1] - x[0]), 2) + pow((y[1] - y[0]), 2));
                //Desenha o triangulo
               // triangulo(x[0], y[0], x[1], y[1], x[2], y[2]);
                bresenhamCircle(x[1], y[1], raio);
                break;
            }
            
        }
    }
}

void bresenhamLine(int x_1, int y_1, int x_2, int y_2){
    bool declive, simetrico;
    int dx = x_2 - x_1;
    int dy = y_2 - y_1;
   // int dx_ = dx, dy_ = dy, x_1_ = x_1, x_2_ = x_2;
    int xi , yi;
    int x_, y_;
    
    //Redu��o de octante
    declive = false;
    simetrico = false;
    if ((dx * dy) < 0) {
	    y_1 = (-1) * y_1;
	    y_2 = (-1) * y_2;
	    dy = (-1) * dy;
	    simetrico = true;
	}
	
	if (abs(dx) < abs(dy)) {
	    std::swap(x_1, y_1);
	    std::swap(x_2, y_2);
	    std::swap(dx, dy);
	    declive = true;
	}
	
	if (x_1 > x_2) {
	    std::swap(x_1, x_2);
	    std::swap(y_1, y_2);
	    dx = (-1) * dx;
	    dy = (-1) * dy;
	}
    
    xi = x_1;
    yi = y_1;
    //Algoritmo de Bresenham
    int delta = (2 * dy) - dx;
    int incE = 2 * dy;
    int incNE = 2 * (dy - dx);
    
   // drawPixel(xi, yi);
    while(xi < x_2){
        //incE
        if(delta <= 0){
            delta += incE;
        }else{ //incNE
            delta += incNE;
            yi += 1;
        }
        xi += 1;
        
        //Transforma��o para o octante original
        x_ = xi;
        y_ = yi;
        if(declive == true){
            x_ = yi;
            y_ = xi;
        }

        if(simetrico == true){
            y_ = (-1) * y_;
        }
        //pontos = pushPonto(x_, y_);
        drawPixel(x_, y_);
        //pushVertice(x_,y_);
    }
}

void quadrilatero(int x1, int y1, int x2, int y2){
    bresenhamLine(x1, y1, x2, y1);
    bresenhamLine(x2, y1, x2, y2);
    bresenhamLine(x2, y2, x1, y2);
    bresenhamLine(x1, y2, x1, y1);
}

void triangulo(int x_1, int y_1, int x_2, int y_2, int x_3, int y_3) {
    bresenhamLine(x_1, y_1, x_2, y_2);
    bresenhamLine(x_2, y_2, x_3, y_3);
    bresenhamLine(x_3, y_3, x_1, y_1);
}

void poligono(std::vector<int>& x, std::vector<int>& y) {
    int n = x.size();

    for (int i = 0; i < n - 1; ++i) {
        bresenhamLine(x[i], y[i], x[i+1], y[i+1]);
    }
    
    if(count_vertices_pol >= 4 && concluir_poligono == true) {
        bresenhamLine(x.back(), y.back(), x.front(), y.front());
    } 
   
}

//Funcao que rasteriza uma circunferencia com o algoritmo de Bresenham
void bresenhamCircle(int xc, int yc, int r){
    int d = 1 - r;
    int de = 3;
    int dse = (-2 * r) + 5;
    int x = 0, y = r;
    drawPixel(0+xc,r+yc);  //1
    drawPixel(0 + xc, -r  + yc);
    drawPixel(r + xc, 0  + yc);
    drawPixel(-r + xc, 0 + yc);
    while(y > x){
        if( d < 0){
            d += de;
            de += 2;
            dse += 2;
        }else{
            d += dse;
            de += 2;
            dse += 4;
            y--;
        }
        x++;
        drawPixel(x + xc, y + yc);
        drawPixel(-x + xc, y + yc);
        drawPixel(x + xc, -y + yc);
        drawPixel(-x + xc, -y + yc);
        drawPixel(y + xc, x + yc);
        drawPixel(-y + xc, x + yc);
        drawPixel(y + xc, -x + yc);
        drawPixel(-y + xc, -x + yc);
    }
}



/*
 * Fucao que implementa o Algoritmo de Rasterizacao da Reta Imediata
 */
/*
void retaImediata(double x_1, double y1, double x_2, double y_2){
    double m, b, yd, xd;
    double xmin, xmax,ymin,ymax;
    
    drawPixel((int)x_1,(int)y1);
    if(x_2-x_1 != 0){ //Evita a divisao por zero
        m = (y_2-y1)/(x_2-x_1);
        b = y1 - (m*x_1);

        if(m>=-1 && m <= 1){ // Verifica se o declive da reta tem tg de -1 a 1, se verdadeira calcula incrementando x
            xmin = (x_1 < x_2)? x_1 : x_2;
            xmax = (x_1 > x_2)? x_1 : x_2;

            for(int x = (int)xmin+1; x < xmax; x++){
                yd = (m*x)+b;
                yd = floor(0.5+yd);
                drawPixel(x,(int)yd);
            }
        }else{ // Se tg menor que -1 ou maior que 1, calcula incrementado os valores de y
            ymin = (y1 < y_2)? y1 : y_2;
            ymax = (y1 > y_2)? y1 : y_2;

            for(int y = (int)ymin + 1; y < ymax; y++){
                xd = (y - b)/m;
                xd = floor(0.5+xd);
                drawPixel((int)xd,y);
            }
        }

    }else{ // se x_2-x_1 == 0, reta perpendicular ao eixo x
        ymin = (y1 < y_2)? y1 : y_2;
        ymax = (y1 > y_2)? y1 : y_2;
        for(int y = (int)ymin + 1; y < ymax; y++){
            drawPixel((int)x_1,y);
        }
    }
    drawPixel((int)x_2,(int)y_2);
}

*/