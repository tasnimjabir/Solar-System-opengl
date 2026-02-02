/*
CSE 420 Project
Solar System
Group Members :
        1.  Tasnim Jabir
        2.  Md Jubayer Ahmed
        3.  Md Ferdows
        4.  Yesmin Akter
*/

#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <cstring>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <string>

using namespace std;

// --- Global Setup ---
struct Star {
    float x, y;
    float brightness;
};
vector<Star> stars;
const int STAR_COUNT = 400;

float globalAngle = 0.0f;
float simulationSpeedFactor = 1.0f;
float zoomScale = 1.0f;
float camX = 0.0f;
float camY = 0.0f;
int windowWidth = 1280;
int windowHeight = 720;

// Speed Constants
const float SPEED_MERCURY = 4.0f;
const float SPEED_VENUS   = 2.5f;
const float SPEED_EARTH   = 1.5f;
const float SPEED_MARS    = 1.2f;
const float SPEED_JUPITER = 0.8f;
const float SPEED_SATURN  = 0.6f;
const float SPEED_URANUS  = 0.4f;
const float SPEED_NEPTUNE = 0.2f;
const float SPEED_MOON    = 5.0f;

// --- Helper Functions ---

void initStars() {
    srand(time(0));
    for(int i=0; i<STAR_COUNT; i++) {
        Star s;
        s.x = (float)(rand() % 400 - 200) / 100.0f;
        s.y = (float)(rand() % 400 - 200) / 100.0f;
        s.brightness = (float)(rand() % 100) / 100.0f;
        stars.push_back(s);
    }
}

void drawBitmapText(float x, float y, const char* text, void* font = GLUT_BITMAP_HELVETICA_10) {
    int len = strlen(text);
    float offset = (len * 0.01f) / 2.0f;
    glRasterPos2f(x - offset, y);
    while (*text) {
        glutBitmapCharacter(font, *text);
        text++;
    }
}

void drawStaticStars() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-2.0, 2.0, -2.0, 2.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glPointSize(1.5f);
    glBegin(GL_POINTS);
    for(size_t i=0; i<stars.size(); i++) {
        float twinkle = 0.6f + 0.4f * sin(globalAngle * 0.05f + i);
        glColor3f(stars[i].brightness * twinkle, stars[i].brightness * twinkle, stars[i].brightness * twinkle);
        glVertex2f(stars[i].x, stars[i].y);
    }
    glEnd();

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void drawCircle(float radius, float r, float g, float b, bool filled) {
    glColor3f(r, g, b);
    if(filled) glBegin(GL_POLYGON);
    else glBegin(GL_LINE_LOOP);

    for (int i = 0; i < 60; i++) {
        float theta = i * 2.0f * 3.14159f / 60.0f;
        glVertex2f(radius * cos(theta), radius * sin(theta));
    }
    glEnd();
}

void drawSunGlow(float radius) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(1.0f, 0.5f, 0.0f, 0.6f);
    glVertex2f(0.0f, 0.0f);
    glColor4f(1.0f, 0.4f, 0.0f, 0.0f);
    for (int i = 0; i <= 60; i++) {
        float theta = i * 2.0f * 3.14159f / 60.0f;
        glVertex2f(radius * 3.0f * cos(theta), radius * 3.0f * sin(theta));
    }
    glEnd();
    glDisable(GL_BLEND);
}

void drawOrbitPath(float radius) {
    glColor4f(0.1f, 0.3f, 0.4f, 0.3f);
    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 100; i++) {
        float theta = i * 2.0f * 3.14159f / 100.0f;
        glVertex2f(radius * cos(theta), radius * sin(theta));
    }
    glEnd();
}

void drawPlanet(float orbitRadius, float speed, float size, float r, float g, float b, const char* name) {
    drawOrbitPath(orbitRadius);

    glPushMatrix();
        float currentAngle = globalAngle * speed * simulationSpeedFactor;
        glRotatef(currentAngle, 0.0f, 0.0f, 1.0f);
        glTranslatef(orbitRadius, 0.0f, 0.0f);

        drawCircle(size, r, g, b, true);

        // Counter-Rotation for horizontal text
        glPushMatrix();
            glRotatef(-currentAngle, 0.0f, 0.0f, 1.0f);
            glColor3f(0.8f, 0.9f, 1.0f);
            drawBitmapText(0.0f, -size - 0.04f, name);
        glPopMatrix();

    glPopMatrix();
}

// --- HUD & BORDER ---
void drawHUD() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-1.77, 1.77, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // --- 1. FULL SCREEN BORDER ---
    float borderX = 1.75f;
    float borderY = 0.98f;

    glColor3f(0.0f, 0.8f, 1.0f);
    glLineWidth(5.0f);
    float cornerLen = 0.15f;

    glBegin(GL_LINE_STRIP); // Top Left
        glVertex2f(-borderX, borderY - cornerLen); glVertex2f(-borderX, borderY); glVertex2f(-borderX + cornerLen, borderY);
    glEnd();
    glBegin(GL_LINE_STRIP); // Top Right
        glVertex2f(borderX - cornerLen, borderY); glVertex2f(borderX, borderY); glVertex2f(borderX, borderY - cornerLen);
    glEnd();
    glBegin(GL_LINE_STRIP); // Bottom Right
        glVertex2f(borderX, -borderY + cornerLen); glVertex2f(borderX, -borderY); glVertex2f(borderX - cornerLen, -borderY);
    glEnd();
    glBegin(GL_LINE_STRIP); // Bottom Left
        glVertex2f(-borderX + cornerLen, -borderY); glVertex2f(-borderX, -borderY); glVertex2f(-borderX, -borderY + cornerLen);
    glEnd();
    glLineWidth(1.0f);

    // --- 2. TITLE and MEMBERS (Top) ---
    glColor3f(1.0f, 0.8f, 0.0f); // Orange for titles
    drawBitmapText(0.0f, 0.92f, "SOLAR SYSTEM - CSE420 Project", GLUT_BITMAP_HELVETICA_18);

    // Group Member List (Top Right)
    float memberX = 1.75f;
    float memberY = 0.90f;

    // Avatar Style Border/Background
    glColor4f(0.0f, 0.15f, 0.3f, 0.5f);
    glRectf(memberX - 0.4f, memberY, memberX, memberY - 0.48f);

    glColor3f(0.0f, 0.8f, 1.0f);
    drawBitmapText(memberX - 0.24f, memberY - 0.05f, "GROUP MEMBERS:", GLUT_BITMAP_HELVETICA_12);

    glColor3f(1.0f, 1.0f, 1.0f);
    drawBitmapText(memberX - 0.2f, memberY - 0.15f, "Tasnim Jabir");
    drawBitmapText(memberX - 0.2f, memberY - 0.25f, "Md Jubayer Ahmed");
    drawBitmapText(memberX - 0.2f, memberY - 0.35f, "Md Ferdows");
    drawBitmapText(memberX - 0.2f, memberY - 0.45f, "Yesmin Akter");


    // --- 3. LEFT DATA PANEL (Side HUD) ---
    float panelLeft = -1.75f;
    float panelRight = -1.35f;
    float panelTop = 0.9f;
    float panelBottom = -0.7f;

    glColor4f(0.0f, 0.05f, 0.15f, 0.8f);
    glRectf(panelLeft, panelTop, panelRight, panelBottom);

    glColor3f(0.0f, 0.8f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
        glVertex2f(panelRight, panelTop);
        glVertex2f(panelRight, panelBottom);
    glEnd();

    const char* names[] = {"MERCURY", "VENUS", "EARTH", "MARS", "JUPITER", "SATURN", "URANUS", "NEPTUNE"};
    float speeds[] = {SPEED_MERCURY, SPEED_VENUS, SPEED_EARTH, SPEED_MARS, SPEED_JUPITER, SPEED_SATURN, SPEED_URANUS, SPEED_NEPTUNE};

    drawBitmapText(panelLeft + 0.2f, panelTop - 0.1f, "ORBITAL TELEMETRY", GLUT_BITMAP_HELVETICA_12);

    for(int i=0; i<8; i++) {
        float yPos = panelTop - 0.25f - (i * 0.15f);
        glColor3f(0.7f, 0.9f, 1.0f);
        drawBitmapText(panelLeft + 0.05f, yPos, names[i]);

        float barLen = speeds[i] * 0.1f;
        glColor4f(0.0f, 1.0f, 0.5f, 0.7f);
        glRectf(panelRight - 0.35f, yPos - 0.01f, panelRight - 0.35f + barLen, yPos - 0.04f);

        stringstream ss; ss.precision(1); ss << fixed << speeds[i] << " AU/y";
        glColor3f(1.0f, 1.0f, 1.0f);
        drawBitmapText(panelRight - 0.2f, yPos - 0.03f, ss.str().c_str());
    }

    // --- 4. BOTTOM UI BAR ---
    float bottomY = -0.85f;
    float barH = 0.15f;

    // Left Console (Status)
    glColor4f(0.1f, 0.1f, 0.2f, 0.9f);
    glRectf(-1.2f, bottomY - 0.01f, -0.6f, bottomY - barH + 0.01f);
    glColor3f(0.0f, 0.8f, 1.0f);
    drawBitmapText(-0.9f, bottomY - 0.05f, "STATUS: ONLINE");
    drawBitmapText(-0.9f, bottomY - 0.1f, "TARGET: SUN");


    // Right Console (Controls Info)
    glColor4f(0.1f, 0.1f, 0.2f, 0.9f);
    glRectf(0.6f, bottomY - 0.01f, 1.75f, bottomY - barH + 0.01f);

    glColor3f(1.0f, 0.6f, 0.0f);
    drawBitmapText(1.17f, bottomY - 0.03f, "CONTROLS:");

    glColor3f(1.0f, 1.0f, 1.0f);
    drawBitmapText(1.1f, bottomY - 0.08f, ("[Arrows/QWEASD] Move         [ESC] Exit        [+/-] Speed: " + to_string(simulationSpeedFactor)).c_str());


    // --- 5. CROSSHAIR (Center) ---
    glColor4f(1.0f, 0.0f, 0.0f, 0.8f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
        glVertex2f(0.0f, 0.05f); glVertex2f(0.0f, 0.02f);
        glVertex2f(0.0f, -0.05f); glVertex2f(0.0f, -0.02f);
        glVertex2f(0.05f, 0.0f); glVertex2f(0.02f, 0.0f);
        glVertex2f(-0.05f, 0.0f); glVertex2f(-0.02f, 0.0f);
    glEnd();
    glPointSize(3.0f);
    glBegin(GL_POINTS);
        glVertex2f(0.0f, 0.0f);
    glEnd();

    glDisable(GL_BLEND);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

// --- Main Display ---
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    drawStaticStars();

    glPushMatrix();

    glScalef(zoomScale, zoomScale, 1.0f);
    glTranslatef(-camX, -camY, 0.0f);

    // Sun
    drawSunGlow(0.15f);
    drawCircle(0.12f, 1.0f, 0.9f, 0.2f, true);
    glColor3f(1.0f, 0.8f, 0.4f);
    drawBitmapText(-0.02f, -0.16f, "SUN");

    // Planets
    drawPlanet(0.20f, SPEED_MERCURY, 0.02f, 0.7f, 0.6f, 0.5f, "MERCURY");
    drawPlanet(0.35f, SPEED_VENUS, 0.03f, 0.9f, 0.8f, 0.6f, "VENUS");

    // Earth System (Special case for Moon)
    float earthOrbit = 0.50f;
    drawOrbitPath(earthOrbit);
    glPushMatrix();
        float earthAng = globalAngle * SPEED_EARTH * simulationSpeedFactor;
        glRotatef(earthAng, 0.0f, 0.0f, 1.0f);
        glTranslatef(earthOrbit, 0.0f, 0.0f);
        drawCircle(0.035f, 0.0f, 0.5f, 1.0f, true);

        glPushMatrix();
            glRotatef(-earthAng, 0.0f, 0.0f, 1.0f);
            glColor3f(0.8f, 0.9f, 1.0f);
            drawBitmapText(0.0f, -0.07f, "EARTH");
        glPopMatrix();

        // Moon
        glPushMatrix();
            float moonAng = globalAngle * SPEED_MOON * simulationSpeedFactor;
            glRotatef(moonAng, 0.0f, 0.0f, 1.0f);
            glTranslatef(0.06f, 0.0f, 0.0f);
            drawCircle(0.01f, 0.8f, 0.8f, 0.8f, true);
        glPopMatrix();
    glPopMatrix();

    drawPlanet(0.65f, SPEED_MARS, 0.025f, 1.0f, 0.3f, 0.2f, "MARS");
    drawPlanet(0.80f, SPEED_JUPITER, 0.060f, 0.8f, 0.6f, 0.4f, "JUPITER");

    // Saturn
    float saturnOrbit = 1.00f;
    drawOrbitPath(saturnOrbit);
    glPushMatrix();
        float satAng = globalAngle * SPEED_SATURN * simulationSpeedFactor;
        glRotatef(satAng, 0.0f, 0.0f, 1.0f);
        glTranslatef(saturnOrbit, 0.0f, 0.0f);
        drawCircle(0.05f, 0.9f, 0.8f, 0.5f, true);
        glColor4f(0.8f, 0.7f, 0.6f, 0.6f);
        glBegin(GL_LINE_LOOP);
            for(int i=0; i<60; i++) {
                float th = i*2.0f*3.14159f/60.0f;
                glVertex2f(0.08f*cos(th), 0.08f*sin(th));
            }
        glEnd();

        glPushMatrix();
            glRotatef(-satAng, 0.0f, 0.0f, 1.0f);
            glColor3f(0.8f, 0.9f, 1.0f);
            drawBitmapText(0.0f, -0.09f, "SATURN");
        glPopMatrix();
    glPopMatrix();

    drawPlanet(1.20f, SPEED_URANUS, 0.040f, 0.4f, 0.8f, 0.9f, "URANUS");
    drawPlanet(1.40f, SPEED_NEPTUNE, 0.038f, 0.2f, 0.3f, 0.9f, "NEPTUNE");

    glPopMatrix();

    drawHUD();

    glFlush();
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    float aspect = (float)w / (float)h;
    windowWidth = w;
    windowHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w >= h) gluOrtho2D(-1.77 * aspect, 1.77 * aspect, -1.77, 1.77);
    else gluOrtho2D(-1.77, 1.77, -1.77 / aspect, 1.77 / aspect);
    glMatrixMode(GL_MODELVIEW);
}

void update(int value) {
    globalAngle += 0.5f;
    if (globalAngle > 360000.0f) globalAngle -= 360000.0f;
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void keyboard(unsigned char key, int x, int y) {
    float panSpeed = 0.1f / zoomScale;
    if (key == 27) exit(0);
    if (key == 'q' || key == 'Q') camY += panSpeed;
    if (key == 'e' || key == 'E') camY -= panSpeed;
    if (key == '+' || key == '=') simulationSpeedFactor += 1.05f;
    if (key == '-' || key == '_') simulationSpeedFactor -= 1.05f;
    if (key == 'w' || key == 'W') zoomScale *= 1.05f;
    if (key == 's' || key == 'S') zoomScale /= 1.05f;
    if (key == 'a' || key == 'A') camX -= panSpeed;
    if (key == 'd' || key == 'D') camX += panSpeed;
    bool f = false;
    if ((key == 'f'|| key == 'F') && !f){ glutFullScreen(); f = true; }
    if ((key == 'f'|| key == 'F') && f){ glutReshapeWindow(1280, 720); f = false;}
    glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
    float panSpeed = 0.1f / zoomScale;
    switch (key) {
        case GLUT_KEY_UP:    zoomScale *= 1.05f; break;
        case GLUT_KEY_DOWN:  zoomScale /= 1.05f; break;
        case GLUT_KEY_LEFT:  camX -= panSpeed; break;
        case GLUT_KEY_RIGHT: camX += panSpeed; break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    initStars();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
    int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
    glutInitWindowPosition((screenWidth-windowWidth)/2, (screenHeight-windowHeight)/2);
    glutCreateWindow("Solar System - Final Project");
//    glutFullScreen();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutTimerFunc(16, update, 0);

    glutMainLoop();
    return 0;
}
