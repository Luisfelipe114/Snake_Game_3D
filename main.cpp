#include <GL/glut.h>
#include <GL/glu.h>
#include <math.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include "glut_text.h"
#include <C:\Users\Luis Felipe\Documents\Estudos\UFPI\4_Periodo\Computacao Grafica\trabalhos\trabalho_final\snage_game_3d\image_loader.h>

using namespace std;

// Global variables for the cube position
GLfloat cubeX = 0.0f;
GLfloat cubeY = 0.0f;  // updated to be aligned with the ground
GLfloat cubeZ = 0.0f;
GLfloat cubeSize = 1.0f;
GLfloat cubeRadius = 0.5f;

// Global variables for the ground position and size
GLfloat groundX = 0.0f;
GLfloat groundY = 0.0f;
GLfloat groundZ = 0.0f;
GLfloat groundSize = 20.0f;

// Global variables for the balls
const int maxBalls = 10; // set the maximum number of balls
GLfloat ballPositions[maxBalls][3]; // array to hold the position of each ball
int numBalls = 0; // current number of balls

// Global variables for the speed
GLfloat speed = 0.2f;
GLfloat maxSpeed = 0.5f;

// Size of the wall's opening
GLfloat openingSize = 3.0f;

// Cube's movement
int direction = 0;

// Game Over
bool isGameOver = false;

int score = 0;

const int maxSnakeLength = 300; // set the maximum length of the snake
int cubeLength = 1;; // current length of the snake
GLfloat cubePositions[maxSnakeLength][3];


void init_lightning(void);
void generateBalls(int value);
void ballsColision(void);
void wall(void);
void updateCubePosition(int value);
bool checkSnakeCollision(bool);
void resetGame(void);

void renderText(float x, float y, const char* text, float scale = 0.005f, float line_width = 1.0)
{
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, scale);

    const char *c;
    for (c = text; *c != '\0'; c++)
    {
    	glLineWidth(line_width);
        glutStrokeCharacter(GLUT_STROKE_ROMAN, *c);
    }

    glPopMatrix();
}


// Armazena o ID da textura gerada pelo OpenGL
GLuint textureID;
GLuint textureID1;
GLuint textureID2;

void lerImagem(const char* filename, int textura) {
    Image* image = load_image(filename);

    if (image) {
    	if(textura == 1){
			glGenTextures(1, &textureID);
        	glBindTexture(GL_TEXTURE_2D, textureID);	
		}else if(textura == 2){
			glGenTextures(1, &textureID1);
        	glBindTexture(GL_TEXTURE_2D, textureID1);
		}else{
			glGenTextures(1, &textureID2);
        	glBindTexture(GL_TEXTURE_2D, textureID2);
		}

        // Configura os par�metros da textura
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Carrega os dados da imagem na textura
        GLenum format = (image->channels == 3) ? GL_RGB : GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, image->width, image->height, 0, format, GL_UNSIGNED_BYTE, image->data);

        // Gera mipmaps para a textura
        gluBuild2DMipmaps(GL_TEXTURE_2D, format, image->width, image->height, format, GL_UNSIGNED_BYTE, image->data);

        // Desvincula a textura
        glBindTexture(GL_TEXTURE_2D, 0);

        // Libera a mem�ria da imagem
        free_image(image);
    }
}
void drawTexturedCube(GLuint t) {
    GLfloat vertices[][3] = {
        {-1.0, -1.0, -1.0}, {1.0, -1.0, -1.0}, {1.0, 1.0, -1.0}, {-1.0, 1.0, -1.0},
        {-1.0, -1.0, 1.0}, {1.0, -1.0, 1.0}, {1.0, 1.0, 1.0}, {-1.0, 1.0, 1.0}
    };

    GLfloat texCoords[][2] = {
        {0.0, 0.0}, {1.0, 0.0}, {1.0, 1.0}, {0.0, 1.0}
    };

    GLint faces[][4] = {
        {0, 1, 2, 3}, {1, 5, 6, 2}, {5, 4, 7, 6}, {4, 0, 3, 7}, {3, 2, 6, 7}, {4, 5, 1, 0}
    };

    glBindTexture(GL_TEXTURE_2D, t);
    glEnable(GL_TEXTURE_2D);

    glBegin(GL_QUADS);
    for (int i = 0; i < 6; ++i) {
        for (int j = 0; j < 4; ++j) {
            glTexCoord2fv(texCoords[j]);
            glVertex3fv(vertices[faces[i][j]]);
        }
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}
void wall() 
{
    // Draw the wall
    glColor3f(0.89f, 0.45f, 0.07f); // set the color of the wall

    // Draw left and right walls
    for (int i = -groundSize; i < groundSize; i++)
    {
        if (i < -openingSize / 2 || i > openingSize / 2) // Create openings
        {
            // Draw left wall
            glPushMatrix();
            glTranslatef(-groundSize, 0.0f, i + 0.5f);
            glScalef(0.5f, 0.375f, 0.5f);
            drawTexturedCube(textureID2);
            glPopMatrix();

            // Draw right wall
            glPushMatrix();
            glTranslatef(groundSize, 0.0f, i + 0.5f);
            glScalef(0.5f, 0.375f, 0.5f);
            drawTexturedCube(textureID2);
            glPopMatrix();
        }
    }

    // Draw front and back walls
    for (int i = -groundSize; i <= groundSize; i++)
    {
        if (i < -openingSize / 2 || i > openingSize / 2) // Create openings
        {
            // Draw front wall
            glPushMatrix();
            glTranslatef(i, 0.0f, -groundSize - 0.5f);
            glScalef(0.5f, 0.375f, 0.5f);
            drawTexturedCube(textureID2);
            glPopMatrix();

            // Draw back wall
            glPushMatrix();
            glTranslatef(i, 0.0f, groundSize - 0.5f);
            glScalef(0.5f, 0.375f, 0.5f);
            drawTexturedCube(textureID2);
            glPopMatrix();
        }
    }
    
    // Check for collisions with the walls
    GLfloat cubeSize = 1.0f;
    GLfloat wallSize = 1.0f;

    // Check left wall
    if (cubeX - cubeSize / 2.0f < (-groundSize - wallSize / 2.0f) + 1)
    {
        if (cubeZ < -openingSize / 2 || cubeZ > openingSize / 2) // Check if the snake is inside the opening
            isGameOver = true;
        else
            cubeX = groundSize - 1;
    }

    // Check right wall
    if (cubeX + cubeSize / 2.0f > (groundSize + wallSize / 2.0f) - 1)
    {
        if (cubeZ < -openingSize / 2 || cubeZ > openingSize / 2)
            isGameOver = true;
        else
            cubeX = -groundSize + 1;
    }

    // Check front wall
    if (cubeZ - cubeSize / 2.0f < (-groundSize - wallSize / 2.0f) + 1)
    {
        if (cubeX < (-openingSize + 2.0) || cubeX > (openingSize - 2.0)) // Check if the snake is inside the opening
            isGameOver = true;
        else
            cubeZ = groundSize - 1.5;
    }

    // Check back wall
    if (cubeZ + cubeSize / 2.0f > (groundSize + wallSize / 2.0f) - 1.5)
    {
        if (cubeX < (-openingSize + 2.0) || cubeX > (openingSize - 2.0))
            isGameOver = true;
        else
            cubeZ = -groundSize + 1;
    }

}

void generateBalls(int value)
{
    // Generate new balls at random positions on the ground
    if (numBalls < maxBalls)
    {
        GLfloat slack = 2.0f; // Adjust this value to control the distance from the wall
        GLfloat ballX = (rand() % (int)(2 * (groundSize - slack))) - (groundSize - slack);
        GLfloat ballZ = (rand() % (int)(2 * (groundSize - slack))) - (groundSize - slack);
        ballPositions[numBalls][0] = ballX;
        ballPositions[numBalls][1] = 0.5f;
        ballPositions[numBalls][2] = ballZ;
        numBalls++;
    }

    // Register the timer function again to call it after 5 seconds
    glutTimerFunc(3500, generateBalls, 0);
}

void ballsColision() {
    // Check for collisions with the balls
    GLfloat cubeRadius = 0.5f; // radius of the cube
    GLfloat ballRadius = 0.3f; // radius of the balls
    for (int i = 0; i < numBalls; i++) {
        GLfloat distX = cubeX - ballPositions[i][0];
        GLfloat distY = cubeY - ballPositions[i][1];
        GLfloat distZ = cubeZ - ballPositions[i][2];
        GLfloat distance = sqrt(distX*distX + distY*distY + distZ*distZ);
        if (distance < (cubeRadius + ballRadius)) {
            // Collision detected, remove the ball
            for (int j = i; j < numBalls - 1; j++) {
                ballPositions[j][0] = ballPositions[j+1][0];
                ballPositions[j][1] = ballPositions[j+1][1];
                ballPositions[j][2] = ballPositions[j+1][2];
            }
            numBalls--;
            if (speed <= maxSpeed) {
                speed += 0.025f;
            }
            score++;
            
            if(cubeLength <= maxSnakeLength) {
                int j = 0;
                while(j < 2) {
                    // Add a new cube to the end of the snake
                    GLfloat newCubePosition[3] = { cubePositions[cubeLength-1][0], cubePositions[cubeLength-1][1], cubePositions[cubeLength-1][2] };
                    switch (direction)
                    {
                        case 1:
                            newCubePosition[2] += cubeSize;
                            break;
                        case 2:
                            //newCubePosition[0] -= cubeSize;
                            newCubePosition[0] -= cubeSize;
                            break;
                        case 3:
                            newCubePosition[2] -= cubeSize;
                            break;
                        case 4:
                            newCubePosition[0] += cubeSize;
                            break;
                    }
                    cubePositions[cubeLength][0] = newCubePosition[0];
                    cubePositions[cubeLength][1] = newCubePosition[1];
                    cubePositions[cubeLength][2] = newCubePosition[2];
                    //{ newCubePosition[0], newCubePosition[1], newCubePosition[2] };
                    cubeLength++;
                    j++;
                }
            }
        }
    }
}

void resetGame() {
	
	// Reset cube position
    cubeX = 0.0f;
    cubeY = 0.0f;
    cubeZ = 0.0f;

    // Reset the number of balls and length of the snake
    numBalls = 0;
    cubeLength = 1;

    // Reset the speed and direction
    speed = 0.2f;
    direction = 0;

    // Reset Game Over state and score
    score = 0;

    // Reset cube positions
    for (int i = 0; i < maxSnakeLength; i++) {
        cubePositions[i][0] = 0.0f;
        cubePositions[i][1] = 0.0f;
        cubePositions[i][2] = 0.0f;
    }
    
}

void display()
{
    // Clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(0.8f, 0.3f, 0.0f);
    
    if(isGameOver) {
		char gameOver[20];
	    sprintf(gameOver, "Game Over!");
		renderText(-7.0f, 3.0f, "Game Over!", 0.02f, 5.0);
	}


    
    // Draw the cube
    for (int i = 0; i < cubeLength; i++) {
		glPushMatrix();
	    glTranslatef(cubePositions[i][0], cubePositions[i][1] + 0.5f, cubePositions[i][2]);
	    glScalef(0.5f, 0.5f, 0.5f);
	    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
        glEnable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_2D);
		drawTexturedCube(textureID);
	    glPopMatrix();
	    glutSwapBuffers();
	}
   
	
    // Draw the ground
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID1);
	
	// Defina as coordenadas de textura
	GLfloat texCoords[][2] = {
	    {0.0f, 0.0f},
	    {1.0f, 0.0f},
	    {0.0f, 1.0f},
	    {1.0f, 1.0f}
	};

	// Desenhe os tri�ngulos com textura
	glBegin(GL_TRIANGLES);
	
	// Tri�ngulo 1
	glTexCoord2fv(texCoords[0]);
	glVertex3f(groundX - groundSize, groundY, groundZ - groundSize);
	glTexCoord2fv(texCoords[1]);
	glVertex3f(groundX + groundSize, groundY, groundZ - groundSize);
	glTexCoord2fv(texCoords[2]);
	glVertex3f(groundX - groundSize, groundY, groundZ + groundSize);
	
	// Tri�ngulo 2
	glTexCoord2fv(texCoords[2]);
	glVertex3f(groundX - groundSize, groundY, groundZ + groundSize);
	glTexCoord2fv(texCoords[1]);
	glVertex3f(groundX + groundSize, groundY, groundZ - groundSize);
	glTexCoord2fv(texCoords[3]);
	glVertex3f(groundX + groundSize, groundY, groundZ + groundSize);
	
	glEnd();
	
	// Desative a textura e desvincule-a ap�s desenhar os tri�ngulos
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

    ballsColision();

    // Draw the balls
    glColor3f(1.0f, 0.0f, 0.0f); // set the color of the balls
    for (int i = 0; i < numBalls; i++)
    {
        glPushMatrix();
        glTranslatef(ballPositions[i][0], ballPositions[i][1], ballPositions[i][2]);
        glutSolidSphere(0.30f, 10, 10); // draw a sphere for each ball
        glPopMatrix();
    }


    init_lightning();
    

    wall();
    
    //Draw the score
    char scoreText[20];
    sprintf(scoreText, "Score: %d", score);
    glColor3f(0.0f, 0.0f, 0.0f);
    renderText(-3.0f, 20.0f, scoreText, 0.01f, 1.0);  // Increase the scale to make the text larger
    

    // Flush the drawing commands
    glFlush();
    

}

void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27: // Escape key
        exit(0);
        break;
    }

    glutPostRedisplay();
}

void special(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_LEFT:
        if (direction != 2 && cubeLength > 1) // Prevent going in the opposite direction
            direction = 4;
        else if (cubeLength == 1)
            direction = 4;
        break;
    case GLUT_KEY_RIGHT:
        if (direction != 4 && cubeLength > 1) // Prevent going in the opposite direction
            direction = 2;
        else if (cubeLength == 1)
            direction = 2;
        break;
    case GLUT_KEY_UP:
        if (direction != 1 && cubeLength > 1) // Prevent going in the opposite direction
            direction = 3;
        else if (cubeLength == 1)
            direction = 3;
        break;
    case GLUT_KEY_DOWN:
        if (direction != 3 && cubeLength > 1) // Prevent going in the opposite direction
            direction = 1;
        else if (cubeLength == 1)
            direction = 1;
        break;
    }
    isGameOver = false;

    glutPostRedisplay();
}


void updateCubePosition(int value)
{
    if (isGameOver) {
        resetGame();
    };

    switch (direction)
    {
        case 1:
            cubeZ += speed;
            break;
        case 2:
            cubeX += speed;
            break;
        case 3:
            cubeZ -= speed;
            break;
        case 4:
            cubeX -= speed;
            break;
    }

    // Update the head's position
    cubePositions[0][0] = cubeX;
    cubePositions[0][1] = cubeY;
    cubePositions[0][2] = cubeZ;

    // Move the rest of the snake
    for (int i = cubeLength - 1; i > 0; i--)
    {
        cubePositions[i][0] = cubePositions[i-1][0];
        cubePositions[i][1] = cubePositions[i-1][1];
        cubePositions[i][2] = cubePositions[i-1][2];
        
        // Check for self-collision
        if(i > 6) {
			GLfloat distX = cubePositions[0][0] - cubePositions[i][0];
	        GLfloat distY = cubePositions[0][1] - cubePositions[i][1];
	        GLfloat distZ = cubePositions[0][2] - cubePositions[i][2];
	        GLfloat distance = sqrt(distX * distX + distY * distY + distZ * distZ);
	        if (distance < cubeRadius) {
	        	isGameOver = true;
	        }
		}
    }
    

    glutPostRedisplay();
    // Call the update function again after a delay
    glutTimerFunc(25, updateCubePosition, 0);
}


void reshape(int w, int h)
{
    // Set the viewport to cover the entire window
    glViewport(0, 0, w, h);

    // Set the projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (GLfloat)w / (GLfloat)h, 0.1f, 100.0f);

    // Set the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Set the camera position and orientation
    gluLookAt(0.0f, 35.0f, 40.0f,  // camera position
              0.0f, 0.0f, 0.0f,        // look-at point
              0.0f, 1.0f, 0.0f);         // up vector

}

void init_lightning() 
{
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Inicia a iluminacao
    GLfloat light_position[] = {-1.0, 1.0, 1.0, 0.0};
	GLfloat light_color[] = {1.0, 1.0, 1.0, 0.0};
    glLightfv(GL_LIGHT0, GL_AMBIENT_AND_DIFFUSE, light_color);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_LIGHTING); //Ativa o modelo de iluminacao
    glEnable(GL_LIGHT0); //Ativa a Luz 0. O OpenGL suporta pelo menos 8 pontos de luz

    // Inicia as caracteristicas gerais dos materiais
	GLfloat mat_ambient_diffuse[] = {1.0, 1.0, 1.0, 1.0};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_ambient_diffuse);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL); //Um ou mais parametros do material rastreiam a cor atual do glColor.

    // Ativa o modelo de sombreamento de Gouraud.
    glShadeModel(GL_SMOOTH);
    
}


int main(int argc, char **argv)
{
    // Initialize GLUT and create a window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(400, 400);
    glutCreateWindow("3D Cube");

    // Set the background color
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
	
	lerImagem("textura1.bmp", 1);
	lerImagem("textura3.bmp", 2);
	lerImagem("textura5.bmp", 3);

    // Register the display, keyboard, and reshape callback functions
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutReshapeFunc(reshape);

    glutTimerFunc(3500, generateBalls, 0);
    glutTimerFunc(25, updateCubePosition, 0);


    // Enter the main loop
    glutMainLoop();

    return 0;
}