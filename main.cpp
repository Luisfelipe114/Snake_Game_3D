#include <GL/glut.h>
#include <math.h>

// Global variables for the cube position
GLfloat cubeX = 0.0f;
GLfloat cubeY = 0.0f;  // updated to be aligned with the ground
GLfloat cubeZ = 0.0f;

// Global variables for the ground position and size
GLfloat groundX = 0.0f;
GLfloat groundY = 0.0f;
GLfloat groundZ = 0.0f;
GLfloat groundSize = 10.0f;

// Global variables for the balls
const int maxBalls = 10; // set the maximum number of balls
GLfloat ballPositions[maxBalls][3]; // array to hold the position of each ball
int numBalls = 0; // current number of balls

// Cube's movement
bool moveForward = false;
bool moveBackward = false;
bool moveLeft = false;
bool moveRight = false;


void init_lightning(void);
void generateBalls(int value);
void ballsColision(void);
void wall(void);
void updateCubePosition(int value);

void wall() 
{
    // Draw the wall
    glColor3f(0.89f, 0.45f, 0.07f); // set the color of the wall

    // Draw left and right walls
    for (int i = -groundSize; i < groundSize; i++)
    {
        // Draw left wall
        glPushMatrix();
        glTranslatef(-groundSize, 0.0f, i + 0.5f);
        glScalef(1.0f, 0.75f, 1.0f);
        glutSolidCube(1.0f);
        glPopMatrix();

        // Draw right wall
        glPushMatrix();
        glTranslatef(groundSize, 0.0f, i + 0.5f);
        glScalef(1.0f, 0.75f, 1.0f);
        glutSolidCube(1.0f);
        glPopMatrix();
    }

    // Draw front and back walls
    for (int i = -groundSize; i <= groundSize; i++)
    {
        // Draw front wall
        glPushMatrix();
        glTranslatef(i, 0.0f, -groundSize - 0.5f);
        glScalef(1.0f, 0.75f, 1.0f);
        glutSolidCube(1.0f);
        glPopMatrix();

        // Draw back wall
        glPushMatrix();
        glTranslatef(i, 0.0f, groundSize - 0.5f);
        glScalef(1.0f, 0.75f, 1.0f);
        glutSolidCube(1.0f);
        glPopMatrix();
    }
    
    // Check for collisions with the walls
    GLfloat cubeSize = 1.0f;
    GLfloat wallSize = 1.0f;

    // Check left wall
    if (cubeX - cubeSize / 2.0f < (-groundSize - wallSize / 2.0f) + 1)
    {
        cubeX = (-groundSize - wallSize / 2.0f + cubeSize / 2.0f) + 1;
    }

    // Check right wall
    if (cubeX + cubeSize / 2.0f > (groundSize + wallSize / 2.0f) - 1)
    {
        cubeX = (groundSize + wallSize / 2.0f - cubeSize / 2.0f) - 1;
    }

    // Check front wall
    if (cubeZ - cubeSize / 2.0f < (-groundSize - wallSize / 2.0f) + 1)
    {
        cubeZ = (-groundSize - wallSize / 2.0f + cubeSize / 2.0f) + 1;
    }

    // Check back wall
    if (cubeZ + cubeSize / 2.0f > (groundSize + wallSize / 2.0f) - 1.5)
    {
        cubeZ = (groundSize + wallSize / 2.0f - cubeSize / 2.0f) - 1.5;
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
        }
    }
}


void display()
{
    // Clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set the color of the cube
    glColor3f(1.0f, 0.0f, 0.0f);

    // Draw the cube
    glPushMatrix();
    glTranslatef(cubeX, cubeY + 0.5f, cubeZ);
    glScalef(1.0f, 1.0f, 1.0f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Draw the ground
    glColor3f(0.0f, 0.5f, 0.0f); // set the color of the ground
    glBegin(GL_TRIANGLES);
    glVertex3f(groundX - groundSize, groundY, groundZ - groundSize);
    glVertex3f(groundX + groundSize, groundY, groundZ - groundSize);
    glVertex3f(groundX - groundSize, groundY, groundZ + groundSize);

    glVertex3f(groundX - groundSize, groundY, groundZ + groundSize);
    glVertex3f(groundX + groundSize, groundY, groundZ - groundSize);
    glVertex3f(groundX + groundSize, groundY, groundZ + groundSize);
    glEnd();

    ballsColision();

    // Draw the balls
    glColor3f(0.0f, 0.0f, 1.0f); // set the color of the balls
    for (int i = 0; i < numBalls; i++)
    {
        glPushMatrix();
        glTranslatef(ballPositions[i][0], ballPositions[i][1], ballPositions[i][2]);
        glutSolidSphere(0.25f, 10, 10); // draw a sphere for each ball
        glPopMatrix();
    }

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    init_lightning();

    wall();

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
       // cubeX -= 0.1f;
        moveForward = false;
        moveBackward = false;
        moveLeft = true;
        moveRight = false;
        break;
    case GLUT_KEY_RIGHT:
       // cubeX += 0.1f;
        moveForward = false;
        moveBackward = false;
        moveLeft = false;
        moveRight = true;
        break;
    case GLUT_KEY_UP:
       // cubeZ += 0.1f; // changed from cubeZ -= 0.1f;
        moveForward = true;
        moveBackward = false;
        moveLeft = false;
        moveRight = false;
        break;
    case GLUT_KEY_DOWN:
        moveForward = false;
        moveBackward = true;
        moveLeft = false;
        moveRight = false;
       /// cubeZ -= 0.1f; // changed from cubeZ += 0.1f;
        break;
    }

    glutPostRedisplay();
}

void updateCubePosition(int value)
{
    if (moveForward)
    {
        cubeZ -= 0.1f;
    }
    else if (moveBackward)
    {
        cubeZ += 0.1f;
    }
    else if (moveLeft)
    {
        cubeX -= 0.1f;
    }
    else if (moveRight)
    {
        cubeX += 0.1f;
    }
    
    glutPostRedisplay();
    // Call the update function again after a delay
    glutTimerFunc(25, updateCubePosition, 0);

    // Call the update function again after a delay
   // glutTimerFunc(50, update, 0);
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
    gluLookAt(cubeX, 20.0f, cubeZ + 25.0f,  // camera position
              cubeX, 0.0f, cubeZ,        // look-at point
              0.0f, 1.0f, 0.0f);         // up vector

    // Adjust the perspective matrix to fit the ground
    /*)
    if (w <= h)
        glOrtho(-groundSize/2, groundSize/2, -groundSize*(GLfloat)h / (GLfloat)w/2, groundSize*(GLfloat)h / (GLfloat)w/2, -groundSize/2, groundSize/2);
    else
        glOrtho(-groundSize*(GLfloat)w / (GLfloat)h/2, groundSize*(GLfloat)w / (GLfloat)h/2, -groundSize/2, groundSize/2, -groundSize/2, groundSize/2);
     */   

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
