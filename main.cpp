/*
 * Universidade Federal do Piaui - UFPI
 * Computacao Grafica
 * Trabalho final: snake game 3d em OPENGL
 * Alunos: Luis Felipe Cabral Brito e Ramon Matheus da Silva Fernandes
*/

/* Inclui os headers do OpenGL, GLU, e GLUT */
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

#include <math.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <mmsystem.h>
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
const int maxBalls = 10; 
GLfloat ballPositions[maxBalls][3]; 
GLfloat ballRadius = 0.3f; 
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

// Global variables for the snake
const int maxSnakeLength = 1000; 
int cubeLength = 1;; 
GLfloat cubePositions[maxSnakeLength][3];

// Global variables for double size special ball
GLfloat sizeSpecialBallPosition[3];
bool sizeSpecialBallActive = false;
int intervalSpecialDoubleSize = 40000;
int randSpecialDoubleSize = 60000;
int intervalSize = rand() % 50000 + 15000;

// Global variables for speed special ball
GLfloat speedSpecialBallPosition[3];
bool speedSpecialBallActive = false;
int durationSpeedSpecial = 5000; // 5 seconds duration
bool speedBoostActive = false;
int randSpecialDoubleSpeed = 60000;
int intervalSpecialDoubleSpeed = 30000;
int intervalSpeed = rand() % 50000 + 15000;

// Global variables for self collision special ball
GLfloat noSelfColSpecialBallPosition[3];
bool noSelfColSpecialBallActive = false;
int durationNoSelfColSpecial = 5000; // 5 seconds duration
bool noSelfColBoostActive = false;
int randNoSelfColSpecial = 70000;
int minTimeNoSelfColSpecial = 30000;
int intervalNoSelfCol = rand() % 50000 + 20000;
bool whiteGhostMode = false;


// Armazena o ID da textura gerada pelo OpenGL
GLuint textureID;
GLuint textureID1;
GLuint textureID2;

// Configurar o som
typedef struct {
    const char* nome_arquivo;  // nome do arquivo de som
    const char* tipo;         // tipo de arquivo de som (wav, mp3, etc.)
} som_info;

som_info som = {"./soundtrack/music2.wav", "wav"};


void wall(void);
void display(void);
void drawEyes(void);

void updateCubePosition(int value);
void special(int key, int x, int y);

void resetGame(void);

void generateBalls(int value);
void generateSpecialSizeBall(int value);
void generateSpecialSpeedBall(int value);
void generateSpecialNoSCBall(int value);

void ballsColision(void);
void checkSizeSpecialBallCollision(void);
void checkSpeedSpecialBallCollision(void);
void checkNoSCSpecialBallCollision(void);

void disableSizeSpecialBall(int value);
void disableSpeedSpecialBall(int value);
void disableNoSCSpecialBall(int value);
void disableSpeedBoost(int value);
void disableNoSCBoost(int value);

void init_lightning(void);
void renderText(float x, float y, const char* text, float scale, float line_width);
void lerImagem(const char* filename, int textura);
void drawTexturedCube(GLuint t);
void keyboard(unsigned char key, int x, int y);
void reshape(int w, int h);

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

        // Configura os parâmetros da textura
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

        // Libera a memória da imagem
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
    	if(noSelfColBoostActive == false) {
			if (cubeZ < -openingSize / 2 || cubeZ > openingSize / 2) // Check if the snake is inside the opening
            	isGameOver = true;
	        else
	            cubeX = groundSize - 1;
		} else {
			cubeX = groundSize - 1;
		}
    }

    // Check right wall
    if (cubeX + cubeSize / 2.0f > (groundSize + wallSize / 2.0f) - 1)
    {
    	if(noSelfColBoostActive == false) {
			if (cubeZ < -openingSize / 2 || cubeZ > openingSize / 2)
            	isGameOver = true;
	        else
	            cubeX = -groundSize + 1;
		} else {
			cubeX = -groundSize + 1;
		}
    }

    // Check front wall
    if (cubeZ - cubeSize / 2.0f < (-groundSize - wallSize / 2.0f) + 1)
    {
    	if(noSelfColBoostActive == false) {
			if (cubeX < (-openingSize + 2.0) || cubeX > (openingSize - 2.0)) // Check if the snake is inside the opening
	            isGameOver = true;
	        else
	            cubeZ = groundSize - 1.5;
		} else {
			cubeZ = groundSize - 1.5;
		}
    }

    // Check back wall
    if (cubeZ + cubeSize / 2.0f > (groundSize + wallSize / 2.0f) - 1.5)
    {
    	if(noSelfColBoostActive == false) {
			if (cubeX < (-openingSize + 2.0) || cubeX > (openingSize - 2.0))
	            isGameOver = true;
	        else
	            cubeZ = -groundSize + 1;
		} else {
			cubeZ = -groundSize + 1;
		}
    }

}

void disableSizeSpecialBall(int value) {
    sizeSpecialBallActive = false;
}

void disableSpeedSpecialBall(int value) {
    speedSpecialBallActive = false;
}

void disableNoSCSpecialBall(int value) {
    noSelfColSpecialBallActive = false;
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

    // Register the timer function again to call it after 3.5 seconds
    glutTimerFunc(3500, generateBalls, 0);
}

void generateSpecialSizeBall(int value)
{    
    
    if (!sizeSpecialBallActive)
    {
        GLfloat slack = 2.0f; // Adjust this value to control the distance from the wall
        GLfloat ballX = (rand() % (int)(2 * (groundSize - slack))) - (groundSize - slack);
        GLfloat ballZ = (rand() % (int)(2 * (groundSize - slack))) - (groundSize - slack);
        sizeSpecialBallPosition[0] = ballX;
        sizeSpecialBallPosition[1] = 0.5f;
        sizeSpecialBallPosition[2] = ballZ;
        sizeSpecialBallActive = true;
        glutTimerFunc(7500, disableSizeSpecialBall, 0);
    }

    // Register the timer function again to call it after a random interval
    intervalSize = rand() % randSpecialDoubleSize + intervalSpecialDoubleSize;
    intervalSpecialDoubleSize += 10000;
    randSpecialDoubleSize += 10000;
    glutTimerFunc(intervalSize, generateSpecialSizeBall, 0);
}

void generateSpecialSpeedBall(int value)
{
    if (!speedSpecialBallActive)
    {
        GLfloat slack = 2.0f;
        GLfloat ballX = (rand() % (int)(2 * (groundSize - slack))) - (groundSize - slack);
        GLfloat ballZ = (rand() % (int)(2 * (groundSize - slack))) - (groundSize - slack);
        speedSpecialBallPosition[0] = ballX;
        speedSpecialBallPosition[1] = 0.5f;
        speedSpecialBallPosition[2] = ballZ;
        speedSpecialBallActive = true;
        glutTimerFunc(7500, disableSpeedSpecialBall, 0);
    }
    
    // Register the timer function again to call it after a random interval
    intervalSpeed = rand() % randSpecialDoubleSpeed + intervalSpecialDoubleSpeed;
    intervalSpecialDoubleSpeed += 10000;
    randSpecialDoubleSpeed += 10000;
    glutTimerFunc(intervalSpeed, generateSpecialSpeedBall, 0);
}

void generateSpecialNoSCBall(int value)
{
    if (!noSelfColSpecialBallActive)
    {
        GLfloat slack = 2.0f;
        GLfloat ballX = (rand() % (int)(2 * (groundSize - slack))) - (groundSize - slack);
        GLfloat ballZ = (rand() % (int)(2 * (groundSize - slack))) - (groundSize - slack);
        noSelfColSpecialBallPosition[0] = ballX;
        noSelfColSpecialBallPosition[1] = 0.5f;
        noSelfColSpecialBallPosition[2] = ballZ;
        noSelfColSpecialBallActive = true;
        glutTimerFunc(7500, disableNoSCSpecialBall, 0);
    }
    
    // Register the timer function again to call it after a random interval
    intervalNoSelfCol = rand() % randNoSelfColSpecial + minTimeNoSelfColSpecial;
    glutTimerFunc(intervalNoSelfCol, generateSpecialNoSCBall, 0);
}

void disableSpeedBoost(int value) {
    speedBoostActive = false;
}

void disableNoSCBoost(int value) {
    noSelfColBoostActive = false;
    whiteGhostMode = false;
}

void addCube() {
	GLfloat newCubePosition[3] = { cubePositions[cubeLength-1][0], cubePositions[cubeLength-1][1], cubePositions[cubeLength-1][2] };
    cubePositions[cubeLength][0] = newCubePosition[0];
    cubePositions[cubeLength][1] = newCubePosition[1];
    cubePositions[cubeLength][2] = newCubePosition[2];
    cubeLength++;
}

void checkSizeSpecialBallCollision() {
    if (sizeSpecialBallActive)
    {
        GLfloat distX = cubeX - sizeSpecialBallPosition[0];
        GLfloat distY = cubeY - sizeSpecialBallPosition[1];
        GLfloat distZ = cubeZ - sizeSpecialBallPosition[2];
        GLfloat distance = sqrt(distX * distX + distY * distY + distZ * distZ);
        if (distance < (cubeRadius + ballRadius))
        {
            sizeSpecialBallActive = false;
            int currentLength = cubeLength;
            while(cubeLength < min(currentLength * 2, maxSnakeLength)) {
				addCube();
			}
			score = score * 2;
        }
    }
}

void checkSpeedSpecialBallCollision() {
    if (speedSpecialBallActive)
    {
        GLfloat distX = cubeX - speedSpecialBallPosition[0];
        GLfloat distY = cubeY - speedSpecialBallPosition[1];
        GLfloat distZ = cubeZ - speedSpecialBallPosition[2];
        GLfloat distance = sqrt(distX * distX + distY * distY + distZ * distZ);
        if (distance < (cubeRadius + ballRadius))
        {
        	score++;
            speedSpecialBallActive = false;
            speedBoostActive = true;
            glutTimerFunc(durationSpeedSpecial, disableSpeedBoost, 0);
        }
    }
}

void checkNoSCSpecialBallCollision() {
    if (noSelfColSpecialBallActive)
    {
        GLfloat distX = cubeX - noSelfColSpecialBallPosition[0];
        GLfloat distY = cubeY - noSelfColSpecialBallPosition[1];
        GLfloat distZ = cubeZ - noSelfColSpecialBallPosition[2];
        GLfloat distance = sqrt(distX * distX + distY * distY + distZ * distZ);
        if (distance < (cubeRadius + ballRadius))
        {
        	score++;
            noSelfColSpecialBallActive = false;
            noSelfColBoostActive = true;
            whiteGhostMode = true;
            glutTimerFunc(durationNoSelfColSpecial, disableNoSCBoost, 0);
        }
    }
}


void ballsColision() {
    checkSizeSpecialBallCollision();
    checkSpeedSpecialBallCollision();
    checkNoSCSpecialBallCollision();
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
                speed += 0.015f;
            }
            score++;
            
            if(cubeLength <= maxSnakeLength) {
                int j = 0;
                while(j < 2) {
                    // Add a new cube to the end of the snake
                    addCube();
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
    
    // Reset variables for size special ball
    sizeSpecialBallPosition[3];
	sizeSpecialBallActive = false;
	intervalSpecialDoubleSize = 40000;
	randSpecialDoubleSize = 60000;
	intervalSize = rand() % 50000 + 15000;
    
    // Reset variables for speed special ball
	speedSpecialBallActive = false;
	durationSpeedSpecial = 5000; // 5 seconds duration
	speedBoostActive = false;
	randSpecialDoubleSpeed = 60000;
	intervalSpecialDoubleSpeed = 30000;
	intervalSpeed = rand() % 50000 + 15000;
    
    // Reset variables for no self collision special ball
    GLfloat noSelfColSpecialBallPosition[3];
	noSelfColSpecialBallActive = false;
	durationNoSelfColSpecial = 5000; // 5 seconds duration
	noSelfColBoostActive = false;
	intervalNoSelfCol = rand() % 50000 + 20000;
	whiteGhostMode = false;
	
	for (int i = 0; i < 3; i++) {
		sizeSpecialBallPosition[i] = 0.0f;
        speedSpecialBallPosition[i] = 0.0f;
        noSelfColSpecialBallPosition[i] = 0.0f;
    }
    
}

void drawEyes() {
	glColor3f(1.0f, 1.0f, 1.0f); // set color to white
    glPushMatrix();
    // adjust the eye position based on the direction of the snake
    switch (direction) {
        case 1: // up
            glTranslatef(0.4f, 0.8f, 1.0f);
            break;
        case 2: // right
            glTranslatef(1.0f, 0.8f, 0.4f);
            break;
        case 3: // down
            glTranslatef(-0.4f, 0.8f, -1.0f);
            break;
        case 4: // left
            glTranslatef(-1.0f, 0.8f, -0.4f);
            break;
        default:
        	glTranslatef(0.4f, 0.8f, 1.0f);
    }
    glutSolidSphere(0.35f, 10, 10); // draw the first eye
    glPopMatrix();

	glColor3f(1.0f, 1.0f, 1.0f); // set color to white
    glPushMatrix();
    // adjust the eye position based on the direction of the snake
    switch (direction) {
        case 1: // up
            glTranslatef(-0.4f, 0.8f, 1.0f);
            break;
        case 2: // right
            glTranslatef(1.0f, 0.8f, -0.4f);
            break;
        case 3: // down
            glTranslatef(0.4f, 0.8f, -1.0f);
            break;
        case 4: // left
            glTranslatef(-1.0f, 0.8f, 0.4f);
            break;
        default:
        	glTranslatef(-0.4f, 0.8f, 1.0f);
    }
    glutSolidSphere(0.35f, 10, 10); // draw the second eye
    glPopMatrix();
    
    // Draw the pupils
    glColor3f(0.0f, 0.0f, 1.0f); // set color to black
    glPushMatrix();
    // adjust the pupil position based on the direction of the snake
    switch (direction) {
        case 1: // up
            glTranslatef(0.4f, 0.9f, 1.20f);
            break;
        case 2: // right
            glTranslatef(1.15f, 0.9f, 0.4f);
            break;
        case 3: // down
            glTranslatef(-0.4f, 0.9f, -1.20f);
            break;
        case 4: // left
            glTranslatef(-1.15f, 0.9f, -0.4f);
            break;
        default:
            glTranslatef(0.4f, 0.9f, 1.20f);
    }
    glutSolidSphere(0.2f, 10, 10); // draw the first pupil
    glPopMatrix();

    glPushMatrix();
    // adjust the pupil position based on the direction of the snake
    switch (direction) {
        case 1: // up
            glTranslatef(-0.4f, 0.9f, 1.20f);
            break;
        case 2: // right
            glTranslatef(1.15f, 0.9f, -0.4f);
            break;
        case 3: // down
            glTranslatef(0.4f, 0.9f, -1.20f);
            break;
        case 4: // left
            glTranslatef(-1.15f, 0.9f, 0.4f);
            break;
        default:
            glTranslatef(-0.4f, 0.9f, 1.20f);
    }
    glutSolidSphere(0.2f, 10, 10); // draw the second pupil
    glPopMatrix();
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
	    if (whiteGhostMode) {
	        glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
	        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
	    } else {
	        glColor3f(0.8f, 0.3f, 0.0f);
	        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    	}
	    //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	    glEnable(GL_DEPTH_TEST);
	    glEnable(GL_TEXTURE_2D);
	    drawTexturedCube(textureID);
	    // Draw the eyes
	    if (i == 0) {
	        drawEyes();
	    }
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

	// Desenhe os triângulos com textura
	glBegin(GL_TRIANGLES);
	
	// Triângulo 1
	glTexCoord2fv(texCoords[0]);
	glVertex3f(groundX - groundSize, groundY, groundZ - groundSize);
	glTexCoord2fv(texCoords[1]);
	glVertex3f(groundX + groundSize, groundY, groundZ - groundSize);
	glTexCoord2fv(texCoords[2]);
	glVertex3f(groundX - groundSize, groundY, groundZ + groundSize);
	
	// Triângulo 2
	glTexCoord2fv(texCoords[2]);
	glVertex3f(groundX - groundSize, groundY, groundZ + groundSize);
	glTexCoord2fv(texCoords[1]);
	glVertex3f(groundX + groundSize, groundY, groundZ - groundSize);
	glTexCoord2fv(texCoords[3]);
	glVertex3f(groundX + groundSize, groundY, groundZ + groundSize);
	
	glEnd();
	
	// Desative a textura e desvincule-a após desenhar os triângulos
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	// Draw the balls
    glColor3f(1.0f, 0.0f, 0.0f); // set the color of the balls
    for (int i = 0; i < numBalls; i++)
    {
        glPushMatrix();
        glTranslatef(ballPositions[i][0], ballPositions[i][1], ballPositions[i][2]);
        glutSolidSphere(0.30f, 10, 10); // draw a sphere for each ball
        glPopMatrix();
    }
	
	// Draw the special ball
    if (sizeSpecialBallActive)
    {
        glColor3f(0.0f, 0.0f, 1.0f); // set the color of the special ball
        glPushMatrix();
        glTranslatef(sizeSpecialBallPosition[0], sizeSpecialBallPosition[1], sizeSpecialBallPosition[2]);
        glutSolidSphere(0.30f, 10, 10); // draw a sphere for the special ball
        glPopMatrix();
    }
    
    // Draw the speed special ball
	if (speedSpecialBallActive)
	{
	    glColor3f(0.0f, 1.0f, 0.0f); // set the color of the speed special ball
	    glPushMatrix();
	    glTranslatef(speedSpecialBallPosition[0], speedSpecialBallPosition[1], speedSpecialBallPosition[2]);
	    glutSolidSphere(0.30f, 10, 10); // draw a sphere for the speed special ball
	    glPopMatrix();
	}
	
	// Draw the no self collision special ball
	if (noSelfColSpecialBallActive)
	{
	    glColor3f(1.0f, 1.0f, 1.0f); // set the color of the speed special ball
	    glPushMatrix();
	    glTranslatef(noSelfColSpecialBallPosition[0], noSelfColSpecialBallPosition[1], noSelfColSpecialBallPosition[2]);
	    glutSolidSphere(0.30f, 10, 10); // draw a sphere for the speed special ball
	    glPopMatrix();
	}

    ballsColision();
    

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
    
    GLfloat currentSpeed = speed;
    if (speedBoostActive)
    {
        currentSpeed *= 2;
    }

    switch (direction)
    {
        case 1:
            cubeZ += currentSpeed;
            break;
        case 2:
            cubeX += currentSpeed;
            break;
        case 3:
            cubeZ -= currentSpeed;
            break;
        case 4:
            cubeX -= currentSpeed;
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
        if(i > 8 && noSelfColBoostActive == false) {
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

void playSound()
{
    PlaySound(som.nome_arquivo, NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
}

void loadSound()
{
    PlaySound(som.nome_arquivo, NULL, SND_FILENAME | SND_ASYNC | SND_LOOP | SND_NOSTOP);
    // Configura o evento de fim da reprodução para chamar a função playSound
    PlaySound(NULL, NULL, SND_ASYNC | SND_FILENAME | SND_LOOP | SND_NOSTOP | SND_PURGE);
    PlaySound(som.nome_arquivo, NULL, SND_FILENAME | SND_ASYNC | SND_LOOP | SND_NOSTOP | SND_NOWAIT);
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
	
	lerImagem("./textures/snake_body.bmp", 1);
	lerImagem("./textures/ground.bmp", 2);
	lerImagem("./textures/wall.bmp", 3);

    // Register the display, keyboard, and reshape callback functions
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutReshapeFunc(reshape);
    
    loadSound(); // carrega o som
    playSound(); // inicia a reprodução do som

    glutTimerFunc(3500, generateBalls, 0);
    glutTimerFunc(25, updateCubePosition, 0);
    glutTimerFunc(intervalSize, generateSpecialSizeBall, 0);
    glutTimerFunc(intervalSpeed, generateSpecialSpeedBall, 0);
    glutTimerFunc(intervalNoSelfCol, generateSpecialNoSCBall, 0);


    // Enter the main loop
    glutMainLoop();

    return 0;
}