#include <windows.h>
#include <GL/glew.h> 
#include <GL/glut.h>
#include <cmath>
#include <vector>

/* CONTROLS: GOOD LAK GAIZ
A and D > move ufo x axis
up and down arrow > move alien and ufo y axis
scroll mouse > alien size
moving continuously > car*/


int windowWidth = 1920;
int windowHeight = 1080;

// Object transformations
float carPositionZ = 0.0f;
float alienScale = 1.0f;
float ufoRotation = 0.0f;
float alienPositionY = 0.0f;
float alienDirection = 1.0f;
float ufoPositionX = 0.0f;

// Animation variables
int lastTime = 0;

// VBO structures
struct VBO {
    GLuint id = 0;
    std::vector<float> vertices;
    int vertexCount;
};

VBO roadVBO;
VBO carBodyVBO;
VBO ufoVBO;
VBO alienBodyVBO;

void setupRoadVBO() {
    roadVBO.vertices = {
        -0.4f, -0.95f,  
        0.4f, -0.95f,   
        0.15f, 0.1f,    
        -0.15f, 0.1f    
    };
    roadVBO.vertexCount = 4;

    glGenBuffers(1, &roadVBO.id);
    glBindBuffer(GL_ARRAY_BUFFER, roadVBO.id);
    glBufferData(GL_ARRAY_BUFFER, roadVBO.vertices.size() * sizeof(float),
        roadVBO.vertices.data(), GL_STATIC_DRAW);
}

void setupCarVBO() {
    carBodyVBO.vertices = {
        -1.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 0.8f,
        0.6f, 1.3f,
        -0.6f, 1.3f,
        -1.0f, 0.8f
    };
    carBodyVBO.vertexCount = 6;

    glGenBuffers(1, &carBodyVBO.id);
    glBindBuffer(GL_ARRAY_BUFFER, carBodyVBO.id);
    glBufferData(GL_ARRAY_BUFFER, carBodyVBO.vertices.size() * sizeof(float),
        carBodyVBO.vertices.data(), GL_STATIC_DRAW);
}

void setupUFOVBO() {
    ufoVBO.vertices.clear();
    for (int i = 0; i <= 30; i++) {
        float angle = i * 2.0f * 3.14159f / 30.0f;
        ufoVBO.vertices.push_back(cos(angle) * 1.2f);
        ufoVBO.vertices.push_back(sin(angle) * 0.25f);
    }
    ufoVBO.vertexCount = 31;

    glGenBuffers(1, &ufoVBO.id);
    glBindBuffer(GL_ARRAY_BUFFER, ufoVBO.id);
    glBufferData(GL_ARRAY_BUFFER, ufoVBO.vertices.size() * sizeof(float),
        ufoVBO.vertices.data(), GL_STATIC_DRAW);
}

void setupAlienVBO() {
    alienBodyVBO.vertices.clear();
    for (int i = 0; i <= 30; i++) {
        float angle = i * 2.0f * 3.14159f / 30.0f;
        alienBodyVBO.vertices.push_back(cos(angle) * 0.9f);
        alienBodyVBO.vertices.push_back(sin(angle) * 1.3f - 0.3f);
    }
    alienBodyVBO.vertexCount = 31;

    glGenBuffers(1, &alienBodyVBO.id);
    glBindBuffer(GL_ARRAY_BUFFER, alienBodyVBO.id);
    glBufferData(GL_ARRAY_BUFFER, alienBodyVBO.vertices.size() * sizeof(float),
        alienBodyVBO.vertices.data(), GL_STATIC_DRAW);
}

void setupVBOData() {
    setupRoadVBO();
    setupCarVBO();
    setupUFOVBO();
    setupAlienVBO();
}

void drawRoad() {
    // Road surface using VBO
    glColor3f(0.15f, 0.15f, 0.15f);
    glBindBuffer(GL_ARRAY_BUFFER, roadVBO.id);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, 0);
    glDrawArrays(GL_QUADS, 0, roadVBO.vertexCount);
    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Yellow center lines
    glColor3f(1.0f, 0.9f, 0.0f);
    float lineSpacing = 0.15f;
    for (float y = -0.9f; y < 0.1f; y += lineSpacing) {
        float width = 0.015f * (1.0f - (y + 0.9f) / 1.0f);
        float xOffset = (y + 0.9f) * 0.25f / 1.0f;

        glBegin(GL_QUADS);
        glVertex2f(-width, y);
        glVertex2f(width, y);
        glVertex2f(width * 0.5f, y + 0.08f);
        glVertex2f(-width * 0.5f, y + 0.08f);
        glEnd();
    }
}

void drawCar() {
    float scale = 0.12f * (1.0f - carPositionZ * 0.85f);
    float yPos = -0.65f + carPositionZ * 0.75f;
    float xPos = 0.0f;
    float roadWidth = 0.4f - carPositionZ * 0.25f;
    xPos *= roadWidth / 0.4f;

    glPushMatrix();
    glTranslatef(xPos, yPos, 0.0f);
    glScalef(scale, scale, 1.0f);

    // Car body using VBO
    glColor3f(0.2f, 0.4f, 0.9f);
    glBindBuffer(GL_ARRAY_BUFFER, carBodyVBO.id);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, 0);
    glDrawArrays(GL_POLYGON, 0, carBodyVBO.vertexCount);
    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Rear window
    glColor3f(0.3f, 0.6f, 0.9f);
    glBegin(GL_QUADS);
    glVertex2f(-0.7f, 0.85f);
    glVertex2f(0.7f, 0.85f);
    glVertex2f(0.5f, 1.2f);
    glVertex2f(-0.5f, 1.2f);
    glEnd();

    // Rear lights
    glColor3f(1.0f, 0.1f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(-0.95f, 0.15f);
    glVertex2f(-0.75f, 0.15f);
    glVertex2f(-0.75f, 0.35f);
    glVertex2f(-0.95f, 0.35f);
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(0.75f, 0.15f);
    glVertex2f(0.95f, 0.15f);
    glVertex2f(0.95f, 0.35f);
    glVertex2f(0.75f, 0.35f);
    glEnd();

    // Spoiler
    glColor3f(1.0f, 0.5f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-0.9f, 1.3f);
    glVertex2f(0.9f, 1.3f);
    glVertex2f(0.8f, 1.45f);
    glVertex2f(-0.8f, 1.45f);
    glEnd();

    glPopMatrix();
}

void drawUFO() {
    glPushMatrix();
    glTranslatef(-0.15f + ufoPositionX, 0.55f + alienPositionY, 0.0f);
    glRotatef(ufoRotation, 0.0f, 0.0f, 1.0f);
    glScalef(0.12f, 0.12f, 1.0f);

    // UFO dome
    glColor3f(0.7f, 0.75f, 0.8f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.0f, 0.6f);
    for (int i = 0; i <= 20; i++) {
        float angle = 3.14159f + i * 3.14159f / 20.0f;
        glVertex2f(cos(angle) * 0.7f, sin(angle) * 0.4f + 0.1f);
    }
    glEnd();

    // UFO base using VBO
    glColor3f(0.5f, 0.55f, 0.65f);
    glBindBuffer(GL_ARRAY_BUFFER, ufoVBO.id);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, 0);
    glDrawArrays(GL_POLYGON, 0, ufoVBO.vertexCount);
    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Glowing lights
    for (int i = 0; i < 8; i++) {
        float angle = i * 2.0f * 3.14159f / 8.0f;
        glColor3f(0.0f, 0.9f, 1.0f);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cos(angle) * 0.9f, sin(angle) * 0.2f - 0.1f);
        for (int j = -3; j <= 3; j++) {
            float a = angle + j * 0.15f;
            glVertex2f(cos(a) * 0.85f, sin(a) * 0.15f - 0.15f);
        }
        glEnd();
    }

    glPopMatrix();
}

void drawAlien() {
    glPushMatrix();
    glTranslatef(-0.15f, 0.15f + alienPositionY, 0.0f);
    glScalef(alienScale * 0.08f, alienScale * 0.08f, 1.0f);

    // Alien body using VBO
    glColor3f(0.4f, 1.0f, 0.3f);
    glBindBuffer(GL_ARRAY_BUFFER, alienBodyVBO.id);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, 0);
    glDrawArrays(GL_POLYGON, 0, alienBodyVBO.vertexCount);
    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Arms, legs, head, etc. 
    glColor3f(0.4f, 1.0f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(-0.9f, 0.6f);
    glVertex2f(-0.75f, 0.6f);
    glVertex2f(-1.2f, -0.1f);
    glVertex2f(-1.35f, -0.1f);
    glEnd();

    glBegin(GL_POLYGON);
    for (int i = 0; i <= 20; i++) {
        float angle = i * 2.0f * 3.14159f / 20.0f;
        glVertex2f(cos(angle) * 0.2f - 1.275f, sin(angle) * 0.2f - 0.2f);
    }
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(0.75f, 0.6f);
    glVertex2f(0.9f, 0.6f);
    glVertex2f(1.35f, -0.1f);
    glVertex2f(1.2f, -0.1f);
    glEnd();

    glBegin(GL_POLYGON);
    for (int i = 0; i <= 20; i++) {
        float angle = i * 2.0f * 3.14159f / 20.0f;
        glVertex2f(cos(angle) * 0.2f + 1.275f, sin(angle) * 0.2f - 0.2f);
    }
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(-0.5f, -1.0f);
    glVertex2f(-0.3f, -1.0f);
    glVertex2f(-0.4f, -1.8f);
    glVertex2f(-0.6f, -1.8f);
    glEnd();

    glBegin(GL_POLYGON);
    for (int i = 0; i <= 20; i++) {
        float angle = 3.14159f + i * 3.14159f / 20.0f;
        glVertex2f(cos(angle) * 0.25f - 0.5f, sin(angle) * 0.15f - 1.85f);
    }
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(-0.75f, -1.85f);
    glVertex2f(-0.25f, -1.85f);
    glVertex2f(-0.25f, -2.0f);
    glVertex2f(-0.75f, -2.0f);
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(0.3f, -1.0f);
    glVertex2f(0.5f, -1.0f);
    glVertex2f(0.6f, -1.8f);
    glVertex2f(0.4f, -1.8f);
    glEnd();

    glBegin(GL_POLYGON);
    for (int i = 0; i <= 20; i++) {
        float angle = 3.14159f + i * 3.14159f / 20.0f;
        glVertex2f(cos(angle) * 0.25f + 0.5f, sin(angle) * 0.15f - 1.85f);
    }
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(0.25f, -1.85f);
    glVertex2f(0.75f, -1.85f);
    glVertex2f(0.75f, -2.0f);
    glVertex2f(0.25f, -2.0f);
    glEnd();

    glColor3f(0.5f, 1.0f, 0.4f);
    glBegin(GL_POLYGON);
    for (int i = 0; i <= 30; i++) {
        float angle = i * 2.0f * 3.14159f / 30.0f;
        glVertex2f(cos(angle) * 1.1f, sin(angle) * 1.2f + 1.3f);
    }
    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
    for (int i = 0; i <= 20; i++) {
        float angle = i * 2.0f * 3.14159f / 20.0f;
        glVertex2f(cos(angle) * 0.25f - 0.4f, sin(angle) * 0.4f + 1.5f);
    }
    glEnd();

    glBegin(GL_POLYGON);
    for (int i = 0; i <= 20; i++) {
        float angle = i * 2.0f * 3.14159f / 20.0f;
        glVertex2f(cos(angle) * 0.25f + 0.4f, sin(angle) * 0.4f + 1.5f);
    }
    glEnd();

    glColor3f(0.4f, 1.0f, 0.3f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.05f, 2.4f);
    glVertex2f(0.05f, 2.4f);
    glVertex2f(0.0f, 2.9f);
    glEnd();

    glColor3f(1.0f, 0.0f, 0.8f);
    glBegin(GL_POLYGON);
    for (int i = 0; i <= 20; i++) {
        float angle = i * 2.0f * 3.14159f / 20.0f;
        glVertex2f(cos(angle) * 0.15f, sin(angle) * 0.15f + 2.95f);
    }
    glEnd();

    glPopMatrix();
}

void drawBuildings() {
    for (int i = 0; i < 12; i++) {
        float xPos = -0.5f + i * 0.085f;
        float height = 0.12f + (i % 4) * 0.04f;
        float width = 0.07f;

        glColor3f(0.1f, 0.15f, 0.35f);
        glBegin(GL_QUADS);
        glVertex2f(xPos, 0.1f);
        glVertex2f(xPos + width, 0.1f);
        glVertex2f(xPos + width * 0.95f, height);
        glVertex2f(xPos + width * 0.05f, height);
        glEnd();

        glColor3f(0.8f, 0.8f, 0.3f);
        for (float y = 0.11f; y < height - 0.02f; y += 0.025f) {
            glBegin(GL_POINTS);
            glVertex2f(xPos + width * 0.3f, y);
            glVertex2f(xPos + width * 0.7f, y);
            glEnd();
        }
    }

    for (int i = 0; i < 3; i++) {
        float xPos = -0.85f + i * 0.2f;
        float height = 0.4f + (i % 3) * 0.15f;
        float width = 0.15f;

        glColor3f(0.12f, 0.2f, 0.4f);
        glBegin(GL_QUADS);
        glVertex2f(xPos, 0.1f);
        glVertex2f(xPos + width, 0.1f);
        glVertex2f(xPos + width * 0.9f, height);
        glVertex2f(xPos + width * 0.1f, height);
        glEnd();

        glColor3f(0.9f, 0.8f, 0.2f);
        for (float y = 0.15f; y < height - 0.05f; y += 0.08f) {
            for (float x = 0; x < 1; x++) {
                glBegin(GL_QUADS);
                glVertex2f(xPos + 0.05f + x * 0.08f, y);
                glVertex2f(xPos + 0.08f + x * 0.08f, y);
                glVertex2f(xPos + 0.08f + x * 0.08f, y + 0.05f);
                glVertex2f(xPos + 0.05f + x * 0.08f, y + 0.05f);
                glEnd();
            }
        }
    }

    for (int i = 0; i < 3; i++) {
        float xPos = 0.45f + i * 0.2f;
        float height = 0.35f + (i % 3) * 0.2f;
        float width = 0.15f;

        glColor3f(0.18f, 0.12f, 0.4f);
        glBegin(GL_QUADS);
        glVertex2f(xPos, 0.1f);
        glVertex2f(xPos + width, 0.1f);
        glVertex2f(xPos + width * 0.9f, height);
        glVertex2f(xPos + width * 0.1f, height);
        glEnd();

        glColor3f(1.0f, 0.3f, 0.9f);
        for (float y = 0.15f; y < height - 0.05f; y += 0.08f) {
            for (float x = 0; x < 1; x++) {
                glBegin(GL_QUADS);
                glVertex2f(xPos + 0.05f + x * 0.08f, y);
                glVertex2f(xPos + 0.08f + x * 0.08f, y);
                glVertex2f(xPos + 0.08f + x * 0.08f, y + 0.05f);
                glVertex2f(xPos + 0.05f + x * 0.08f, y + 0.05f);
                glEnd();
            }
        }
    }

    for (int i = 0; i < 2; i++) {
        float xPos = -1.0f + i * 0.35f;
        float height = 0.85f + (i % 2) * 0.15f;
        float width = 0.3f;

        glColor3f(0.15f, 0.25f, 0.5f);
        glBegin(GL_QUADS);
        glVertex2f(xPos, -0.95f);
        glVertex2f(xPos + width, -0.95f);
        glVertex2f(xPos + width * 0.85f, height);
        glVertex2f(xPos + width * 0.15f, height);
        glEnd();

        glColor3f(0.9f, 0.9f, 0.3f);
        for (float y = -0.85f; y < height - 0.08f; y += 0.12f) {
            for (float x = 0; x < 2; x++) {
                glBegin(GL_QUADS);
                glVertex2f(xPos + 0.08f + x * 0.12f, y);
                glVertex2f(xPos + 0.14f + x * 0.12f, y);
                glVertex2f(xPos + 0.14f + x * 0.12f, y + 0.09f);
                glVertex2f(xPos + 0.08f + x * 0.12f, y + 0.09f);
                glEnd();
            }
        }
    }

    for (int i = 0; i < 2; i++) {
        float xPos = 0.7f + i * 0.35f;
        float height = 0.9f + (i % 2) * 0.1f;
        float width = 0.3f;

        glColor3f(0.2f, 0.15f, 0.45f);
        glBegin(GL_QUADS);
        glVertex2f(xPos, -0.95f);
        glVertex2f(xPos + width, -0.95f);
        glVertex2f(xPos + width * 0.85f, height);
        glVertex2f(xPos + width * 0.15f, height);
        glEnd();

        glColor3f(1.0f, 0.2f, 0.8f);
        for (float y = -0.85f; y < height - 0.08f; y += 0.12f) {
            for (float x = 0; x < 2; x++) {
                glBegin(GL_QUADS);
                glVertex2f(xPos + 0.08f + x * 0.12f, y);
                glVertex2f(xPos + 0.14f + x * 0.12f, y);
                glVertex2f(xPos + 0.14f + x * 0.12f, y + 0.09f);
                glVertex2f(xPos + 0.08f + x * 0.12f, y + 0.09f);
                glEnd();
            }
        }
    }
}

void drawPalmTrees() {
    glPushMatrix();
    glTranslatef(-0.65f, 0.1f, 0.0f);
    glScalef(0.8f, 1.0f, 1.0f);

    glColor3f(0.4f, 0.2f, 0.4f);
    glBegin(GL_QUADS);
    glVertex2f(-0.015f, 0.0f);
    glVertex2f(0.015f, 0.0f);
    glVertex2f(0.012f, 0.25f);
    glVertex2f(-0.012f, 0.25f);
    glEnd();

    glColor3f(1.0f, 0.0f, 0.8f);
    for (int i = 0; i < 6; i++) {
        float angle = i * 60.0f;
        glPushMatrix();
        glTranslatef(0.0f, 0.25f, 0.0f);
        glRotatef(angle - 90, 0.0f, 0.0f, 1.0f);
        glBegin(GL_TRIANGLES);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(-0.02f, 0.15f);
        glVertex2f(0.02f, 0.15f);
        glEnd();
        glPopMatrix();
    }

    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.65f, 0.1f, 0.0f);
    glScalef(0.8f, 1.0f, 1.0f);

    glColor3f(0.4f, 0.2f, 0.4f);
    glBegin(GL_QUADS);
    glVertex2f(-0.015f, 0.0f);
    glVertex2f(0.015f, 0.0f);
    glVertex2f(0.012f, 0.25f);
    glVertex2f(-0.012f, 0.25f);
    glEnd();

    glColor3f(1.0f, 0.0f, 0.8f);
    for (int i = 0; i < 6; i++) {
        float angle = i * 60.0f;
        glPushMatrix();
        glTranslatef(0.0f, 0.25f, 0.0f);
        glRotatef(angle - 90, 0.0f, 0.0f, 1.0f);
        glBegin(GL_TRIANGLES);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(-0.02f, 0.15f);
        glVertex2f(0.02f, 0.15f);
        glEnd();
        glPopMatrix();
    }

    glPopMatrix();
}

void drawMoon() {
    glColor3f(0.95f, 0.95f, 0.85f);
    glPushMatrix();
    glTranslatef(0.7f, 0.8f, 0.0f);
    glBegin(GL_POLYGON);
    for (int i = 0; i <= 30; i++) {
        float angle = i * 2.0f * 3.14159f / 30.0f;
        glVertex2f(cos(angle) * 0.08f, sin(angle) * 0.08f);
    }
    glEnd();
    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_QUADS);
    glColor3f(0.02f, 0.02f, 0.15f);
    glVertex2f(-1.0f, 0.1f);
    glVertex2f(1.0f, 0.1f);
    glColor3f(0.05f, 0.05f, 0.25f);
    glVertex2f(1.0f, 1.0f);
    glVertex2f(-1.0f, 1.0f);
    glEnd();

    drawMoon();
    drawBuildings();
    drawPalmTrees();
    drawAlien();
    drawUFO();
    drawRoad();
    drawCar();

    glutSwapBuffers();
}

void timer(int value) {
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    float deltaTime = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;

    carPositionZ += 0.25f * deltaTime;
    if (carPositionZ > 1.0f) {
        carPositionZ = 0.0f;
    }

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void mouseWheel(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        if (button == 3) {
            alienScale += 0.1f;
            if (alienScale > 2.0f) alienScale = 2.0f;
            glutPostRedisplay();
        }
        else if (button == 4) {
            alienScale -= 0.1f;
            if (alienScale < 0.5f) alienScale = 0.5f;
            glutPostRedisplay();
        }
    }
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        ufoRotation += 45.0f;
        if (ufoRotation >= 360.0f) {
            ufoRotation -= 360.0f;
        }
        glutPostRedisplay();
    }

    mouseWheel(button, state, x, y);
}

void keyboard(unsigned char key, int x, int y) {
    const float moveSpeed = 0.05f;

    if (key == 27) {
        exit(0);
    }
    else if (key == 'a' || key == 'A') {
        ufoPositionX -= moveSpeed;
        if (ufoPositionX < -0.7f) ufoPositionX = -0.7f;
        glutPostRedisplay();
    }
    else if (key == 'd' || key == 'D') {
        ufoPositionX += moveSpeed;
        if (ufoPositionX > 0.7f) ufoPositionX = 0.7f;
        glutPostRedisplay();
    }
}

void specialKeys(int key, int x, int y) {
    const float moveSpeed = 0.05f;

    if (key == GLUT_KEY_UP) {
        alienPositionY += moveSpeed;
        if (alienPositionY > 0.5f) alienPositionY = 0.5f;
        glutPostRedisplay();
    }
    else if (key == GLUT_KEY_DOWN) {
        alienPositionY -= moveSpeed;
        if (alienPositionY < -0.5f) alienPositionY = -0.5f;
        glutPostRedisplay();
    }
}

void init() {
    glClearColor(0.02f, 0.02f, 0.15f, 1.0f);

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "GLEW init error: %s\n", glewGetErrorString(err));
        exit(1);
    }

    setupVBOData();

    lastTime = glutGet(GLUT_ELAPSED_TIME);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Cyber City - SADO");

    glutFullScreen();

    init();

    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutTimerFunc(0, timer, 0);

    glutMainLoop();
    return 0;
}