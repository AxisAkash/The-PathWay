#include <windows.h>
#include <GL/glut.h>
#include <ctime>
#include <vector>
#include <algorithm>
#include <iostream>
#include <math.h>
#include <fstream>
using namespace std;

#define PI 3.14159265358979323846

bool isSoundPlaying = true;
bool gameOverTriggered=false;
bool timer2= false;
bool timer3= false;
bool timer4= false;
bool timer5= false;
void playSound() {
    PlaySound("background.wav", NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
}

void stopSound() {
    PlaySound(NULL, 0, 0); // Stops the sound
}

void toggleSound() {
    if (isSoundPlaying) {
        stopSound();
        isSoundPlaying = false;
    } else {
        playSound();
        isSoundPlaying = true;
    }
}

// State to determine which screen to render
int currentScreen = 0; // 0 = menu, 1 = info, 2 = levelChoose

// Struct to represent 2D points (vertices)
struct Point {
    float x, y;

    Point(float x, float y) {
        this->x = x;
        this->y = y;
    }
};

// Function definition for rendering text
void renderText(float x, float y, const char* text, void* font) {
    glRasterPos2f(x, y); // Set position for the text
    while (*text) {
        glutBitmapCharacter(font, *text); // Render each character
        text++;
    }
}

namespace backStory{
    // Plane position and animation variables
    float planeOffsetY = 40.0f;
    float planeSpeedY = 0.001f;

    // Star variables
    struct Star {
        float x, y;
    };
    vector<Star> stars;
    const int numStars = 80;
    float starSpeed = 0.02f;

    // Backstory variables
    vector<string> backstory = {
        "In the not-so-distant future, humanity's skies are no longer a place of serenity",
        "but a proving ground for the most skilled pilots.",
        "As the world grows chaotic with storms of debris and rogue drones,",
        "a secret initiative known as The PathWay was created.",
        "To find the next generation of aerial legends",
        "You are a daring pilot thrust into this high-stakes simulation.",
        "It is designed to test your reflexes, precision, and endurance.",
        "Six perilous levels await, each with unpredictable challenges.",
        "Will you navigate the pathway to victory,",
        "or will the skies claim another challenger?"
    };
    size_t lineIndex = 0;
    string currentText = "";
    float lastUpdateTime = 0;

    // Plane structure and colors
    struct Point {
        float x, y;
    };

    vector<vector<Point>> plane = {
        {{0.0f, -58.0f}, {0.3f, -59.0f}, {-0.3f, -59.0f}},
        {{0.3f, -59.0f}, {-0.3f, -59.0f}, {-0.8f, -59.4f}, {-0.8f, -59.6f}, {0.8f, -59.6f}, {0.8f, -59.4f}},
        {{-0.2f, -59.6f}, {0.2f, -59.6f}, {0.2f, -59.8f}, {-0.2f, -59.8f}},
        {{-0.2f, -59.8f}, {-0.4f, -60.0f}, {-0.2f, -60.0f}, {-0.15f, -59.95f}},
        {{-0.2f, -59.8f}, {-0.15f, -59.95f}, {-0.1f, -60.0f}, {0.1f, -60.0f}, {0.15f, -59.95f}, {0.2f, -59.8f}},
        {{0.2f, -59.8f}, {0.15f, -59.95f}, {0.2f, -60.0f}, {0.4f, -60.0f}},
        {{-0.8f, -59.6f}, {-0.8f, -59.2f}},
        {{0.8f, -59.6f}, {0.8f, -59.2f}}
    };

    vector<vector<float>> planeColors = {
        {0.698f, 0.745f, 0.710f}, {0.827f, 0.827f, 0.827f}, {0.698f, 0.745f, 0.710f},
        {0.827f, 0.827f, 0.827f}, {0.827f, 0.827f, 0.827f}, {0.827f, 0.827f, 0.827f},
        {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}
    };

    float randomXPosition();


    // Initialize star positions
    void initStars() {
        for (int i = 0; i < numStars; ++i) {
            stars.push_back({randomXPosition(), static_cast<float>(rand() % 40 - 20)});
        }
    }

    float randomXPosition() {
        return static_cast<float>(rand() % 30 - 15);
    }

    // Draw stars moving downward
    void drawStars() {
        for (auto& star : stars) {
            star.y -= starSpeed;
            if (star.y < -20.0f) {
                star.y = 20.0f;
                star.x = randomXPosition();
            }
            glPointSize(2.0f);
            glBegin(GL_POINTS);
            glColor3f(1.0f, 1.0f, 1.0f);  // White stars
            glVertex2f(star.x, star.y);
            glEnd();
        }
    }

    // Draw the plane
    void drawPlane(const vector<vector<Point>>& plane, const vector<vector<float>>& colors) {
        glPushMatrix();
        glTranslatef(0.0f, planeOffsetY, 0.0f);

        for (size_t i = 0; i < plane.size(); ++i) {
            const auto& vertices = plane[i];
            const auto& color = colors[i];

            if (vertices.size() > 2) {
                glBegin(GL_POLYGON);
                glColor3f(color[0], color[1], color[2]);
                for (const auto& vertex : vertices) {
                    glVertex2f(vertex.x, vertex.y);
                }
                glEnd();
            } else if (vertices.size() == 2) {
                glLineWidth(1.5f);
                glBegin(GL_LINES);
                glColor3f(color[0], color[1], color[2]);
                for (const auto& vertex : vertices) {
                    glVertex2f(vertex.x, vertex.y);
                }
                glEnd();
            }
        }

        glPopMatrix();
    }

    // Display backstory with typewriter effect
    void displayBackstory() {
        float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

        if (lineIndex < backstory.size()) {
            if (currentText.length() < backstory[lineIndex].length()) {
                if (currentTime - lastUpdateTime > 0.05f) {
                    currentText += backstory[lineIndex][currentText.length()];
                    lastUpdateTime = currentTime;
                }
            } else if (currentTime - lastUpdateTime > 1.0f) {
                currentText = "";
                lineIndex++;
                lastUpdateTime = currentTime;
            }
        }

        /*if(currentTime - lastUpdateTime > 3.0f){
            // home button.
        }*/

        glColor3f(1.0f, 1.0f, 1.0f);
        glRasterPos2f(-8.0f, 0.0f); // Adjusted position
        for (char c : currentText) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }
    }


    // Animate the plane upward
    void animatePlane() {
        if (planeOffsetY < 80.0f) {
            planeOffsetY += planeSpeedY;
        }
    }

    // Display function
    void display() {
        glClear(GL_COLOR_BUFFER_BIT);

        // from I1->J1->K1->L1
        glBegin(GL_POLYGON);
        glColor3f(0.5f, 0.5f, 0.5f);
        glVertex2f(-1.5f, -8.0f);
        glVertex2f(1.5f, -8.0f);
        glVertex2f(1.5f, -9.0f);
        glVertex2f(-1.5f, -9.0f);
        glEnd();

        // Render text inside the polygon
        glColor3f(1.0f, 1.0f, 1.0f);
        renderText(-0.4f, -8.5f, "Back", GLUT_BITMAP_9_BY_15);

        drawStars();
        drawPlane(plane, planeColors);
        displayBackstory();
        animatePlane();

        glutSwapBuffers();
        glutPostRedisplay();
    }
}

namespace gameOver{
    void display(){
        glClear(GL_COLOR_BUFFER_BIT);        // Clear the color buffer (background)
        // Projection setup
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-10.0f, 10.0f, -10.0f, 10.0f, -1.0f, 1.0f);
        glMatrixMode(GL_MODELVIEW);

        // from G1->H1->F1->E1
        glBegin(GL_POLYGON);
        glColor3f(0.514f, 0.871f, 0.925f);
        glVertex2f(-10.0f, 10.0f);
        glVertex2f(10.0f, 10.0f);
        glVertex2f(10.0f, -10.0f);
        glVertex2f(-10.0f, -10.0f);
        glEnd();

        // from A->B->D->C
        glBegin(GL_POLYGON);
        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex2f(-4.0f, 6.0f);
        glVertex2f(4.0f, 6.0f);
        glVertex2f(4.0f, 0.0f);
        glVertex2f(-4.0f, 0.0f);
        glEnd();

        // from I->J->K->L
        glBegin(GL_POLYGON);
        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex2f(-2.0f, -3.0f);
        glVertex2f(2.0f, -3.0f);
        glVertex2f(2.0f, -4.0f);
        glVertex2f(-2.0f, -4.0f);
        glEnd();

        // from E2->F2->H2->G2
        glBegin(GL_POLYGON);
        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex2f(-2.0f, -6.0f);
        glVertex2f(2.0f, -6.0f);
        glVertex2f(2.0f, -7.0f);
        glVertex2f(-2.0f, -7.0f);
        glEnd();

        // for N
        GLfloat x2= -9.5f;
        GLfloat y2= 9.5f;
        GLfloat radius = 0.5f;
        int triangleAmount = 100; //# of lines used to draw circle

        //GLfloat radius = 0.8f; //radius
        GLfloat twicePi = 2.0f * PI;

        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_TRIANGLE_FAN);
            glVertex2f(x2, y2); // center of circle
            for(int i = 0; i <= triangleAmount;i++) {
                glVertex2f( x2 + (radius * cos(i *  twicePi / triangleAmount)),
                            y2 + (radius * sin(i * twicePi / triangleAmount)) );
            }
        glEnd();

        // for D1
        GLfloat x3= -9.75f;
        GLfloat y3= 9.25f;
        GLfloat rad = 0.1f;

        glColor3f(0.514f, 0.871f, 0.925f);
        glBegin(GL_TRIANGLE_FAN);
            glVertex2f(x3, y3); // center of circle
            for(int i = 0; i <= triangleAmount;i++) {
                glVertex2f( x3 + (rad * cos(i *  twicePi / triangleAmount)),
                            y3 + (rad * sin(i * twicePi / triangleAmount)) );
            }
        glEnd();

        // for D1
        GLfloat x4= -9.35f;
        GLfloat y4= 9.35f;

        glColor3f(0.514f, 0.871f, 0.925f);
        glBegin(GL_TRIANGLE_FAN);
            glVertex2f(x4, y4); // center of circle
            for(int i = 0; i <= triangleAmount;i++) {
                glVertex2f( x4 + (rad * cos(i *  twicePi / triangleAmount)),
                            y4 + (rad * sin(i * twicePi / triangleAmount)) );
            }
        glEnd();

        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glColor3f(0.514f, 0.871f, 0.925f);
        glVertex2f(-9.7f, 9.3f);
        glVertex2f(-9.7f, 9.6);
        glEnd();

        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glColor3f(0.514f, 0.871f, 0.925f);
        glVertex2f(-9.7f, 9.6);
        glVertex2f(-9.3f, 9.7f);
        glEnd();

        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glColor3f(0.514f, 0.871f, 0.925f);
        glVertex2f(-9.3f, 9.7f);
        glVertex2f(-9.3f, 9.4f);
        glEnd();

        // Render text inside the polygon
        glColor3f(0.0f, 0.0f, 0.0f);
        renderText(-1.5f, 7.5f, "Game Over", GLUT_BITMAP_TIMES_ROMAN_24);

        // Render text inside the polygon
        glColor3f(0.0f, 0.0f, 0.0f);
        renderText(-3.0f, 4.0f, "Your Score: ", GLUT_BITMAP_9_BY_15);

        // Render text inside the polygon
        glColor3f(0.0f, 0.0f, 0.0f);
        renderText(-3.0f, 2.0f, "Higest Score: ", GLUT_BITMAP_9_BY_15);

        // Render text inside the polygon
        glColor3f(0.0f, 0.0f, 0.0f);
        renderText(-1.0f, -3.5f, "Try Again", GLUT_BITMAP_9_BY_15);

        // Render text inside the polygon
        glColor3f(0.0f, 0.0f, 0.0f);
        renderText(-0.5f, -6.5f, "Home", GLUT_BITMAP_9_BY_15);

        glFlush();
        glutPostRedisplay();

    }
}

namespace levelComplete{
    void display(){
        glClear(GL_COLOR_BUFFER_BIT);        // Clear the color buffer (background)
        // Projection setup
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-10.0f, 10.0f, -10.0f, 10.0f, -1.0f, 1.0f);
        glMatrixMode(GL_MODELVIEW);

        // from G1->H1->F1->E1
        glBegin(GL_POLYGON);
        glColor3f(0.514f, 0.871f, 0.925f);
        glVertex2f(-10.0f, 10.0f);
        glVertex2f(10.0f, 10.0f);
        glVertex2f(10.0f, -10.0f);
        glVertex2f(-10.0f, -10.0f);
        glEnd();

        // from A2->B2->D2->C2
        glBegin(GL_POLYGON);
        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex2f(-4.0f, 6.0f);
        glVertex2f(4.0f, 6.0f);
        glVertex2f(4.0f, 0.0f);
        glVertex2f(-4.0f, 0.0f);
        glEnd();

        // from I1->J1->K1->L1
        glBegin(GL_POLYGON);
        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex2f(-2.0f, -3.0f);
        glVertex2f(2.0f, -3.0f);
        glVertex2f(2.0f, -4.0f);
        glVertex2f(-2.0f, -4.0f);
        glEnd();

        // from E3->F3->H3->G3
        glBegin(GL_POLYGON);
        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex2f(-2.0f, -6.0f);
        glVertex2f(2.0f, -6.0f);
        glVertex2f(2.0f, -7.0f);
        glVertex2f(-2.0f, -7.0f);
        glEnd();

        // for N
        GLfloat x2= -9.5f;
        GLfloat y2= 9.5f;
        GLfloat radius = 0.5f;
        int triangleAmount = 100; //# of lines used to draw circle

        //GLfloat radius = 0.8f; //radius
        GLfloat twicePi = 2.0f * PI;

        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_TRIANGLE_FAN);
            glVertex2f(x2, y2); // center of circle
            for(int i = 0; i <= triangleAmount;i++) {
                glVertex2f( x2 + (radius * cos(i *  twicePi / triangleAmount)),
                            y2 + (radius * sin(i * twicePi / triangleAmount)) );
            }
        glEnd();

        // for D1
        GLfloat x3= -9.75f;
        GLfloat y3= 9.25f;
        GLfloat rad = 0.1f;

        glColor3f(0.514f, 0.871f, 0.925f);
        glBegin(GL_TRIANGLE_FAN);
            glVertex2f(x3, y3); // center of circle
            for(int i = 0; i <= triangleAmount;i++) {
                glVertex2f( x3 + (rad * cos(i *  twicePi / triangleAmount)),
                            y3 + (rad * sin(i * twicePi / triangleAmount)) );
            }
        glEnd();

        // for D1
        GLfloat x4= -9.35f;
        GLfloat y4= 9.35f;

        glColor3f(0.514f, 0.871f, 0.925f);
        glBegin(GL_TRIANGLE_FAN);
            glVertex2f(x4, y4); // center of circle
            for(int i = 0; i <= triangleAmount;i++) {
                glVertex2f( x4 + (rad * cos(i *  twicePi / triangleAmount)),
                            y4 + (rad * sin(i * twicePi / triangleAmount)) );
            }
        glEnd();

        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glColor3f(0.514f, 0.871f, 0.925f);
        glVertex2f(-9.7f, 9.3f);
        glVertex2f(-9.7f, 9.6);
        glEnd();

        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glColor3f(0.514f, 0.871f, 0.925f);
        glVertex2f(-9.7f, 9.6);
        glVertex2f(-9.3f, 9.7f);
        glEnd();

        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glColor3f(0.514f, 0.871f, 0.925f);
        glVertex2f(-9.3f, 9.7f);
        glVertex2f(-9.3f, 9.4f);
        glEnd();

        // Render text inside the polygon
        glColor3f(0.0f, 0.0f, 0.0f);
        renderText(-1.5f, 7.5f, "Level Completed", GLUT_BITMAP_TIMES_ROMAN_24);

        // Render text inside the polygon
        glColor3f(0.0f, 0.0f, 0.0f);
        renderText(-3.0f, 4.0f, "Your Score: ", GLUT_BITMAP_9_BY_15);

        // Render text inside the polygon
        glColor3f(0.0f, 0.0f, 0.0f);
        renderText(-3.0f, 2.0f, "Higest Score: ", GLUT_BITMAP_9_BY_15);

        // Render text inside the polygon
        glColor3f(0.0f, 0.0f, 0.0f);
        renderText(-1.0f, -3.5f, "Try Again", GLUT_BITMAP_9_BY_15);

        // Render text inside the polygon
        glColor3f(0.0f, 0.0f, 0.0f);
        renderText(-0.5f, -6.5f, "Levels", GLUT_BITMAP_9_BY_15);

        glFlush();

        glutSwapBuffers();
    }
}

namespace levelTwo{
    int counter = 0;    // Integer to display
    float elapsedTime= 0.0f;
    // Rotation variable
    float rotationAngle = 0.0f;
    // Initial y-axis bounds for the visible region
    float yMin = -60.0f, yMax = -40.0f;
    float step = 0.05;
    int scrollTime = 16;

    // Plane movement variables
    float planeOffsetX = 0.0f; // Offset for the plane to sync with scrolling
    float planeOffsetY = 0.0f; // Offset for polygon movement in the x-axis
    float planeRotation = 0.0f; // Track the rotation of the plane
    bool isKeyPressedA = false;
    bool isKeyPressedD = false;
    float velocityX = 0.0f;
    float targetRotation = 0.0f;
    const float moveSpeed = 0.005f;    // Speed for smooth movement
    const float rotationSpeed = 2.0f; // Speed for smooth rotation

    // Variables to handle translation
    float xOffset1 = 0.0f; // Offset for translation
    float xOffset2 = 0.0f; // Offset for translation
    float xOffset3 = 0.0f; // Offset for translation
    bool startTranslation1 = false; // Flag to start translation
    bool startTranslation2 = false;
    bool startTranslation3 = false;
    float speed = 0.005f; // Speed of translation
    vector<pair<float, float>> translationOffsets = {
        {xOffset1, 0.0f},
        {xOffset2, 0.0f},
        {-xOffset3, 0.0f},
        {xOffset3, 0.0f}
    }; // Corresponding offsets for each polygon

    clock_t startTime;    // Timer to check if certain seconds have passed
    void init() {
        startTime = clock(); // Record the start time
    }

    vector<vector<Point>> transformedRotationPolygons;
    vector<vector<Point>> transformedTranslationPolygons;
    //store generated circle vertices
    vector<vector<Point>> circles{};
    vector<vector<Point>> rotationPolygons = {
        // from Z->L1->S1->R1
        {{8.0f, -18.0f}, {12.0f, -18.0f}, {12.0f, -26.0f}, {8.0f, -26.0f}},
       // from Z2->W2->V2->U2
        {{-15.0f, -8.0f}, {-10.0f, -8.0f}, {-10.0f, -6.0f}, {-15.0f, -6.0f}},

        // from T2->S2->Q2->R2
        {{15.0f, -8.0f}, {10.0f, -8.0f}, {10.0f, -6.0f}, {15.0f, -6.0f}},

        // from M2->N2->P2->O2
        {{-4.0f, 2.0f}, {4.0f, 2.0f}, {4.0f, -2.0f}, {-4.0f, -2.0f}},

        // from F2->G2->L2->H2
        {{-4.0f, 10.0f}, {4.0f, 10.0f}, {4.0f, 6.0f}, {-4.0f, 6.0f}},

        // from H3->G3->E3->F3
        {{15.0f, 12.0f}, {10.0f, 12.0f}, {10.0f, 14.0f}, {15.0f, 14.0f}},

        // from D3->A3->B3->C3
        {{-15.0f, 14.0f}, {-10.0f, 14.0f}, {-10.0f, 12.0f}, {-15.0f, 12.0f}}
    };

    vector<vector<Point>> translationPolygons = {
        // from A4->B4->W3->Z3
        {{15.0f, 22.0f}, {6.0f, 22.0f}, {6.0f, 24.0f}, {15.0f, 24.0f}},

        // from C4->D4->E4->F4
        {{-15.0f, 28.0f}, {-6.0f, 28.0f}, {-6.0f, 30.0f}, {-15.0f, 30.0f}},

        // from N4->K4->L4->M4
        {{-15.0f, 36.0f}, {-6.0f, 36.0f}, {-6.0f, 38.0f}, {-15.0f, 38.0f}},

        // from J4->G4->H4->I4
        {{15.0f, 36.0f}, {6.0f, 36.0f}, {6.0f, 38.0f}, {15.0f, 38.0f}},

    };


    vector<Point> applyRotation(const vector<Point>& polygon, float angle) {
        vector<Point> transformedPolygon;
        float radians = angle * M_PI / 180.0f;

        // Calculate the center of the rectangle
        float minX = polygon[0].x, maxX = polygon[0].x;
        float minY = polygon[0].y, maxY = polygon[0].y;
        for (const auto& vertex : polygon) {                                                                          //////check this logic
            minX = min(minX, vertex.x);
            maxX = max(maxX, vertex.x);
            minY = min(minY, vertex.y);
            maxY = max(maxY, vertex.y);
        }
        float centerX = (minX + maxX) / 2.0f;
        float centerY = (minY + maxY) / 2.0f;

        // Apply rotation around the center
        for (const auto& vertex : polygon) {
            float x = centerX + (vertex.x - centerX) * cos(radians) - (vertex.y - centerY) * sin(radians);
            float y = centerY + (vertex.x - centerX) * sin(radians) + (vertex.y - centerY) * cos(radians);
            transformedPolygon.push_back({x, y});
        }
        return transformedPolygon;
    }

    vector<Point> applyTranslation(const vector<Point>& polygon, float offsetX, float offsetY) {
        vector<Point> transformedPolygon;
        for (const auto& vertex : polygon) {
            transformedPolygon.push_back({vertex.x + offsetX, vertex.y + offsetY});
        }
        return transformedPolygon;
    }

    void applyMovement(){
        // Clear the transformed polygons
        transformedRotationPolygons.clear();
        transformedTranslationPolygons.clear();
        translationOffsets = {{xOffset1, 0.0f}, {xOffset2, 0.0f},{-xOffset3, 0.0f},{xOffset3, 0.0f}};

        for (size_t i = 0; i < translationPolygons.size(); ++i) {
        transformedTranslationPolygons.push_back(
            applyTranslation(translationPolygons[i], translationOffsets[i].first, translationOffsets[i].second)
            );
        }
        for(const auto& polygon : rotationPolygons) {
            transformedRotationPolygons.push_back(
                applyRotation(polygon, rotationAngle)
            );
        }
    }

    /* void drawPlane() {
        glPushMatrix();
        glTranslatef(planeOffsetX, planeOffsetY, 0.0f);
        glRotatef(planeRotation, 0.0f, 1.0f, 0.0f); // Apply rotation

        // from B1->D1->C1
        glBegin(GL_POLYGON);
        glColor3f(0.698f, 0.745f, 0.710f);
        glVertex2f(0.0f, -58.0f);
        glVertex2f(0.3f, -59.0f);
        glVertex2f(-0.3f, -59.0f);
        glEnd();

        // from D1->C1->E1->H1->K1->I1
        glBegin(GL_POLYGON);
        glColor3f(0.827f, 0.827f, 0.827f);
        glVertex2f(0.3f, -59.0f);
        glVertex2f(-0.3f, -59.0f);
        glVertex2f(-0.8f, -59.4f);
        glVertex2f(-0.8f, -59.6f);
        glVertex2f(0.8f, -59.6f);
        glVertex2f(0.8f, -59.4f);
        glEnd();

        // from M1->U4->T4->K2
        glBegin(GL_POLYGON);
        glColor3f(0.698f, 0.745f, 0.710f);
        glVertex2f(-0.2f, -59.6f);
        glVertex2f(0.2f, -59.6f);
        glVertex2f(0.2f, -59.8f);
        glVertex2f(-0.2f, -59.8f);
        glEnd();

        // from K2->V4->Z4->A
        glBegin(GL_POLYGON);
        glColor3f(0.827f, 0.827f, 0.827f);
        glVertex2f(-0.2f, -59.8f);
        glVertex2f(-0.4f, -60.0f);
        glVertex2f(-0.2f, -60.0f);
        glVertex2f(-0.15f, -59.95f);
        glEnd();

        // from K2->A->A5->B5->A1->T4
        glBegin(GL_POLYGON);
        glColor3f(0.827f, 0.827f, 0.827f);
        glVertex2f(-0.2f, -59.8f);
        glVertex2f(-0.15f, -59.95f);
        glVertex2f(-0.1f, -60.0f);
        glVertex2f(0.1f, -60.0f);
        glVertex2f(0.15f, -59.95f);
        glVertex2f(0.2f, -59.8f);
        glEnd();

        // from T4->A1->C5->W4
        glBegin(GL_POLYGON);
        glColor3f(0.827f, 0.827f, 0.827f);
        glVertex2f(0.2f, -59.8f);
        glVertex2f(0.15f, -59.95f);
        glVertex2f(0.2f, -60.0f);
        glVertex2f(0.4f, -60.0f);
        glEnd();

        // from E1->G1
        glLineWidth(1.5f);
        glBegin(GL_LINES);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex2f(-0.8f, -59.6f);
        glVertex2f(-0.8f, -59.2f);
        glEnd();

        // from I1->J1
        glLineWidth(1.5f);
        glBegin(GL_LINES);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex2f(0.8f, -59.6f);
        glVertex2f(0.8f, -59.2f);
        glEnd();

        glPopMatrix();
    }
    */
    vector<vector<float>> planeColors = {
        // Color for the first polygon
        {0.698f, 0.745f, 0.710f},

        // Color for the second polygon
        {0.827f, 0.827f, 0.827f},

        // Color for the third polygon
        {0.698f, 0.745f, 0.710f},

        // Color for the fourth polygon
        {0.827f, 0.827f, 0.827f},

        // Color for the fifth polygon
        {0.827f, 0.827f, 0.827f},

        // Color for the sixth polygon
        {0.827f, 0.827f, 0.827f},

        // Color for the first line
        {0.0f, 0.0f, 0.0f},

        // Color for the second line
        {0.0f, 0.0f, 0.0f}
    };

    vector<vector<Point>> plane = {
        // First polygon
        {
            {0.0f, -58.0f}, {0.3f, -59.0f}, {-0.3f, -59.0f}
        },
        // Second polygon
        {
            {0.3f, -59.0f}, {-0.3f, -59.0f}, {-0.8f, -59.4f},
            {-0.8f, -59.6f}, {0.8f, -59.6f}, {0.8f, -59.4f}
        },
        // Third polygon
        {
            {-0.2f, -59.6f}, {0.2f, -59.6f}, {0.2f, -59.8f}, {-0.2f, -59.8f}
        },
        // Fourth polygon
        {
            {-0.2f, -59.8f}, {-0.4f, -60.0f}, {-0.2f, -60.0f}, {-0.15f, -59.95f}
        },
        // Fifth polygon
        {
            {-0.2f, -59.8f}, {-0.15f, -59.95f}, {-0.1f, -60.0f},
            {0.1f, -60.0f}, {0.15f, -59.95f}, {0.2f, -59.8f}
        },
        // Sixth polygon
        {
            {0.2f, -59.8f}, {0.15f, -59.95f}, {0.2f, -60.0f}, {0.4f, -60.0f}
        },
        // First line
        {
            {-0.8f, -59.6f}, {-0.8f, -59.2f}
        },
        // Second line
        {
            {0.8f, -59.6f}, {0.8f, -59.2f}
        }
    };

    // Function to draw a chequered bar
    void drawChequeredBar(float x1, float y1, float x2, float y2, float ySubdivision) {
        float width = x2 - x1; // Total width of the bar
        int numDivisions = static_cast<int>(width); // Number of subdivisions along x-axis

        // Top rectangle: From y1 to ySubdivision
        for (int i = 0; i < numDivisions; i++) {
            float xStart = x1 + i;
            float xEnd = x1 + i + 1;

            if (i % 2 == 0) {
                glColor3f(0.0, 0.0, 0.0); // Black
            } else {
                glColor3f(1.0, 1.0, 1.0); // White
            }

            glBegin(GL_POLYGON);
            glVertex2f(xStart, y1);
            glVertex2f(xEnd, y1);
            glVertex2f(xEnd, ySubdivision);
            glVertex2f(xStart, ySubdivision);
            glEnd();
        }

        // Bottom rectangle: From ySubdivision to y2
        for (int i = 0; i < numDivisions; i++) {
            float xStart = x1 + i;
            float xEnd = x1 + i + 1;

            if (i % 2 != 0) { // Alternate pattern
                glColor3f(0.0, 0.0, 0.0); // Black
            } else {
                glColor3f(1.0, 1.0, 1.0); // White
            }

            glBegin(GL_POLYGON);
            glVertex2f(xStart, ySubdivision);
            glVertex2f(xEnd, ySubdivision);
            glVertex2f(xEnd, y2);
            glVertex2f(xStart, y2);
            glEnd();
        }

        // Draw the top x-axis line
        glColor3f(0.0, 0.0, 0.0); // Black color for lines
        glBegin(GL_LINES);
        glVertex2f(x1, y1); // Start of the top edge
        glVertex2f(x2, y1); // End of the top edge
        glEnd();

        // Draw the bottom x-axis line
        glBegin(GL_LINES);
        glVertex2f(x1, y2); // Start of the bottom edge
        glVertex2f(x2, y2); // End of the bottom edge
        glEnd();
    }


    void drawPlane(const vector<vector<Point>>& plane, const vector<vector<float>>& colors) {
        glPushMatrix();
        glTranslatef(planeOffsetX, planeOffsetY, 0.0f);
        glRotatef(planeRotation, 0.0f, 1.0f, 0.0f); // Apply rotation

        for (size_t i = 0; i < plane.size(); ++i) {
            const auto& vertices = plane[i];
            const auto& color = colors[i];

            if (vertices.size() > 2) {
                // Draw polygons
                glBegin(GL_POLYGON);
                glColor3f(color[0], color[1], color[2]);
                for (const auto& vertex : vertices) {
                    glVertex2f(vertex.x, vertex.y);
                }
                glEnd();
            } else if (vertices.size() == 2) {
                // Draw lines
                glLineWidth(1.5f);
                glBegin(GL_LINES);
                glColor3f(color[0], color[1], color[2]);
                for (const auto& vertex : vertices) {
                    glVertex2f(vertex.x, vertex.y);
                }
                glEnd();
            }
        }

        glPopMatrix();
    }

    vector<vector<Point>> obstacles = {
        // First obstacle (C->E->F->H)
        {
            {5.0f, -45.0f}, {-5.0f, -45.0f}, {-5.0f, -40.0f}, {5.0f, -40.0f}
        },
        // Second obstacle (G->I->J->M)
        {
            {-15.0f, -35.0f}, {-5.0f, -35.0f}, {-5.0f, -30.0f}, {-15.0f, -30.0f}
        },
        // Obstacle (T->U->W->V)
        {
            {-8.0f, -20.0f}, {3.0f, -20.0f}, {3.0f, -24.0f}, {-8.0f, -24.0f}
        },
        // Obstacle (O4->R->T1->W1)
        {
            {-15.0f, -15.0f}, {0.0f, -15.0f}, {0.0f, -10.0f}, {-15.0f, -10.0f}
        },
        // Obstacle (P4->S->U1->V1)
        {
            {15.0f, -15.0f}, {10.0f, -15.0f}, {10.0f, -10.0f}, {15.0f, -10.0f}
        },
        // Obstacle (Z1->B2->D2->J3)
        {
            {-15.0f, 15.0f}, {-5.0f, 15.0f}, {-5.0f, 20.0f}, {-15.0f, 20.0f}
        },
        // Obstacle (A2->C2->E2->I3)
        {
            {15.0f, 15.0f}, {5.0f, 15.0f}, {5.0f, 20.0f}, {15.0f, 20.0f}
        },
        // Obstacle (K3->M3->O3->R4)
        {
            {-15.0f, 40.0f}, {-5.0f, 40.0f}, {-5.0f, 45.0f}, {-15.0f, 45.0f}
        },
        // Obstacle (L3->N3->P3->Q3->Q4)
        {
            {15.0f, 40.0f}, {5.0f, 40.0f}, {5.0f, 45.0f}, {10.0f, 50.0f}, {15.0f, 50.0f}
        },
        // Obstacle (R4->O3->R3->S3->S4)
        {
            {-15.0f, 45.0f}, {-5.0f, 45.0f}, {0.0f, 50.0f}, {-5.0f, 55.0f}, {-15.0f, 55.0f}
        },
        // Obstacle (Q4->Q3->T3->V3->J2)
        {
            {15.0f, 50.0f}, {10.0f, 50.0f}, {5.0f, 55.0f}, {5.0f, 60.0f}, {15.0f, 60.0f}
        },
        // Obstacle (S4->S3->U3->I2)
        {
            {-15.0f, 55.0f}, {-5.0f, 55.0f}, {-5.0f, 60.0f}, {-15.0f, 60.0f}
        },
        // from N1->P1->B->D
        {
            {-15.0f, -60.0f}, {-5.0f, -60.0f}, {-5.0f, -50.0f}, {-15.0f, -50.0f}
        },

        // from O1->Q1->K->L
        {
            {15.0f, -60.0f}, {5.0f, -60.0f}, {5.0f, -30.0f}, {15.0f, -30.0f}
        },

        // from O1->J2
        {
            {{15.0f, -60.0f}, {15.0f, 60.0f}}
        },

        // from N1->I2
        {
            {{-15.0f, -60.0f}, {-15.0f, 60.0f}}
        },
        // from M->O->P->O4
        {
            {-15.0f, -30.0f}, {-14.0f, -30.0f}, {-14.0f, -15.0f}, {-15.0f, -15.0f}
        },

        // from L->N->Q->P4
        {
            {15.0f, -30.0f}, {14.0f, -30.0f}, {14.0f, -15.0f}, {15.0f, -15.0f}
        }
    };

    // Collision detection Start

    // Function to compute the dot product of two vectors
    float dotProduct(const Point& a, const Point& b) {
        return a.x * b.x + a.y * b.y;
    }

    // Function to calculate axes perpendicular to edges of a polygon
    vector<Point> getAxes(const vector<Point>& polygon) {
        vector<Point> axes;
        for (size_t i = 0; i < polygon.size(); ++i) {
            Point edge = {
                polygon[(i + 1) % polygon.size()].x - polygon[i].x,
                polygon[(i + 1) % polygon.size()].y - polygon[i].y
            };
            axes.push_back({-edge.y, edge.x}); // Perpendicular axis
        }
        return axes;
    }

    // Function to project a polygon onto an axis
    void projectOntoAxis(const vector<Point>& polygon, const Point& axis, float& minVal, float& maxVal) {
        minVal = maxVal = dotProduct(polygon[0], axis);
        for (const auto& vertex : polygon) {
            float projection = dotProduct(vertex, axis);
            if (projection < minVal) minVal = projection;
            if (projection > maxVal) maxVal = projection;
        }
    }

    // SAT-based collision detection between two polygons
    bool polygonsCollide(const vector<Point>& poly1, const vector<Point>& poly2) {
        vector<Point> axes1 = getAxes(poly1);
        vector<Point> axes2 = getAxes(poly2);

        for (const auto& axis : axes1) {
            float min1, max1, min2, max2;
            projectOntoAxis(poly1, axis, min1, max1);
            projectOntoAxis(poly2, axis, min2, max2);

            if (max1 < min2 || max2 < min1) {
                return false; // No overlap, polygons don't collide
            }
        }

        for (const auto& axis : axes2) {
            float min1, max1, min2, max2;
            projectOntoAxis(poly1, axis, min1, max1);
            projectOntoAxis(poly2, axis, min2, max2);

            if (max1 < min2 || max2 < min1) {
                return false; // No overlap, polygons don't collide
            }
        }

        return true; // All axes overlap, polygons collide
    }

    vector<vector<Point>> getTransformedPlane() {
        vector<vector<Point>> transformedPlane;
        for (const auto& polygon : plane) {
            vector<Point> transformedPolygon;
            for (const auto& vertex : polygon) {
                transformedPolygon.push_back({vertex.x + planeOffsetX, vertex.y + planeOffsetY});
            }
            transformedPlane.push_back(transformedPolygon);
        }
        return transformedPlane;
    }
    // Check if any polygon in the plane collides with any obstacle
    bool noCollision(const vector<vector<Point>>& plane, const vector<vector<Point>>& obstacles) {
        vector<vector<Point>> transformedPlane = getTransformedPlane(); // Transform the plane

        for (const auto& obstacle : obstacles) {
            for (const auto& transformedPolygon : transformedPlane) {
                if (polygonsCollide(transformedPolygon, obstacle)) {
                    return false; // Collision detected
                }
            }
        }

        return true; // No collisions
    }

    /*

    // Function to calculate the min and max range for a polygon on a given axis (x or y)
    void getRange(const vector<Point>& polygon, bool checkX, float& minVal, float& maxVal){
        if (checkX) {
            minVal = polygon[0].x;
        } else {
            minVal = polygon[0].y;
        }
        maxVal = minVal;

        for (const auto& vertex : polygon) {
            float value;
            if (checkX) {
                value = vertex.x;
            } else {
                value = vertex.y;
            }

            if (value < minVal) minVal = value;
            if (value > maxVal) maxVal = value;
        }
    }

    vector<vector<Point>> getTransformedPlane() {
        vector<vector<Point>> transformedPlane;
        for (const auto& polygon : plane) {
            vector<Point> transformedPolygon;
            for (const auto& vertex : polygon) {
                transformedPolygon.push_back({vertex.x + planeOffsetX, vertex.y + planeOffsetY});
            }
            transformedPlane.push_back(transformedPolygon);
        }
        return transformedPlane;
    }


    // Function to check if two polygons (plane and obstacles) collide
    bool noCollision(const vector<vector<Point>>& plane, const vector<vector<Point>>& obstacles) {
        vector<vector<Point>> transformedPlane = getTransformedPlane(); // Apply translation to plane

        for (const auto& obstacle : obstacles) {
            for (const auto& transformedPolygon : transformedPlane) {
                // Get the range for the transformed polygon
                float planeMinX, planeMaxX, planeMinY, planeMaxY;
                getRange(transformedPolygon, true, planeMinX, planeMaxX);
                getRange(transformedPolygon, false, planeMinY, planeMaxY);

                // Get the range for the obstacle polygon
                float obstacleMinX, obstacleMaxX, obstacleMinY, obstacleMaxY;
                getRange(obstacle, true, obstacleMinX, obstacleMaxX);
                getRange(obstacle, false, obstacleMinY, obstacleMaxY);

                // Check for collision on the x-axis
                if (planeMaxX < obstacleMinX || planeMinX > obstacleMaxX) {
                    continue; // No collision on x-axis
                }

                // Check for collision on the y-axis
                if (planeMaxY < obstacleMinY || planeMinY > obstacleMaxY) {
                    continue; // No collision on y-axis
                }

                // If both axes overlap, there is a collision
                return false;
            }
        }

        return true; // No collision
    }
    */
    //Collision detection End


    void drawPolygons(const vector<vector<Point>>& polygons) {
        for (const auto& polygon : polygons) {
            glBegin(GL_POLYGON);
            glColor3f(0.514f, 0.871f, 0.925f); // Set color (light blue in this case)
            for (const auto& point : polygon) {
                glVertex2f(point.x, point.y); // Add vertex
            }
            glEnd();
        }
    }


    void storeCounter(int counter) {
        ofstream outFile("lib/counter.txt");  // Open the file in write mode
        if (outFile.is_open()) {
            outFile << counter;  // Write the counter value to the file
            outFile.close();  // Close the file after writing
        }
    }

    int retrieveCounter() {
        int counter = 0;
        ifstream inFile("lib/counter.txt");  // Open the file in read mode
        if (inFile.is_open()) {
            inFile >> counter;  // Read the counter value from the file
            inFile.close();  // Close the file after reading
        }
        return counter;
    }


    void renderText(float x, float y, const string& text, void* font = GLUT_BITMAP_HELVETICA_18) {
        glColor3f(0.0f, 0.0f, 0.0f); // Set text color to black
        glRasterPos2f(-14.8f, yMax - 1.0f);         // Position of the text in the window
        for (char c : text) {
            glutBitmapCharacter(font, c); // Render each character
        }
    }
    /* Display callback function */
    void display() {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Set background color to white and opaque
        glClear(GL_COLOR_BUFFER_BIT);        // Clear the color buffer (background)
        // Reset projection matrix for dynamic scrolling
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-15.0f, 15.0f, yMin, yMax, -1.0f, 1.0f); // Set visible region dynamically
        glMatrixMode(GL_MODELVIEW);

        drawChequeredBar(-5.0f, 59.9f, 5.0f, 58.0f, 59.0f);

        drawPlane(plane, planeColors);

        // Update the counter at 2x per second
        counter = (int)(elapsedTime * 2);

        // Design 2: Start
        glLineWidth(5.0f);
        glBegin(GL_LINES);
        // from O1->J2
        glColor3f(0.514f, 0.871f, 0.925f);
        glVertex2f(15.0f, -60.0f);
        glVertex2f(15.0f, 60.0f);
        // from N1->I2
        glColor3f(0.514f, 0.871f, 0.925f);
        glVertex2f(-15.0f, -60.0f);
        glVertex2f(-15.0f, 60.0f);
        // from I2->J2
        glColor3f(0.514f, 0.871f, 0.925f);
        glVertex2f(-15.0f, 60.0f);
        glVertex2f(15.0f, 60.0f);
        glEnd();
        // Draw polygons
        drawPolygons(obstacles);
        drawPolygons(transformedRotationPolygons);
        drawPolygons(transformedTranslationPolygons);
        // Render the counter text in the top-left corner
        renderText(-16.0f, 15.0f, to_string(counter));

        glFlush(); // Render now
        // Design 2: End


        // Check if 10 seconds have passed
        if (((clock() - startTime) / CLOCKS_PER_SEC) >= 25) {
            //startTranslation1 = true; // Start translation after 10 seconds
            xOffset1 -= speed; // Move left
        }
        if (((clock() - startTime) / CLOCKS_PER_SEC) >= 27) {
            //tartTranslation2 = true; // Start translation after 12 seconds
            xOffset2 += speed; // Move left
        }
        if (((clock() - startTime) / CLOCKS_PER_SEC) >= 29) {
            //startTranslation3 = true; // Start translation after 13 seconds
            xOffset3 -= speed; // Move left
        }
    }

    void updatePlane() {
        if (isKeyPressedA) {
            velocityX = -moveSpeed;
            targetRotation = -45.0f;
        } else if (isKeyPressedD) {
            velocityX = moveSpeed;
            targetRotation = 45.0f;
        } else {
            velocityX = 0.0f; // Stop movement if no key is pressed
            targetRotation = 0.0f; // Reset rotation
        }

        // Update position
        planeOffsetX += velocityX;

        // Smoothly interpolate rotation towards the target
        if (planeRotation < targetRotation) {
            planeRotation += rotationSpeed;
            if (planeRotation > targetRotation) planeRotation = targetRotation; // Clamp to target
        } else if (planeRotation > targetRotation) {
            planeRotation -= rotationSpeed;
            if (planeRotation < targetRotation) planeRotation = targetRotation; // Clamp to target
        }

        glutPostRedisplay(); // Redraw the scene
    }

    /* Timer function for scrolling */
    void timer(int value) {

        if(!timer2) return;
        // Update the y-axis bounds
        if (noCollision(plane, obstacles) && noCollision(plane,transformedRotationPolygons) && noCollision(plane,transformedTranslationPolygons) && noCollision(plane,circles)){
            if (yMax <= 60.0f){
                yMin += step;
                yMax += step;
            }
            applyMovement();

            // Calculate elapsed time in seconds for Score
            elapsedTime = (float)(clock() - startTime) / CLOCKS_PER_SEC;
            planeOffsetY += step; // Move the plane up at the same rate as the scrolling
        }
        else if(!gameOverTriggered){
            glutDisplayFunc(gameOver::display);
            gameOverTriggered=true;
        }

        // Update the rotation angle for obstacle animation
        rotationAngle += 0.4f; // Increment the angle (degrees)
        if (rotationAngle >= 360.0f) {
            rotationAngle -= 360.0f; // Keep the angle within 0�360 degrees
        }

        if(planeOffsetY>=120.0f){
                cout << planeOffsetY << endl;
            glutDisplayFunc(levelComplete::display);
        }

        // Trigger a redisplay to reflect the updated viewport
        glutPostRedisplay();

        // Call this timer function again after the specified interval
        glutTimerFunc(scrollTime, timer, 0);

    }

    void keyboard(unsigned char key, int x, int y) {
        if (key == 'a' || key == 'A') {
            isKeyPressedA = true;
        } else if (key == 'd' || key == 'D') {
            isKeyPressedD = true;
        }
    }

    void keyboardUp(unsigned char key, int x, int y) {
        if (key == 'a' || key == 'A') {
            isKeyPressedA = false;
        } else if (key == 'd' || key == 'D') {
            isKeyPressedD = false;
        }
    }

    /*void keyboard(unsigned char key, int x, int y) {
        // Handle A and D keys
        if (key == 'a' || key == 'A') {
            planeOffsetX -= 0.5f; // Move left
            planeRotation = -45.0f; // Rotate 45 degrees to the left

        } else if (key == 'd' || key == 'D') {
            planeOffsetX += 0.5f; // Move right
            planeRotation = 45.0f; // Rotate 45 degrees to the right
        }
        glutPostRedisplay(); // Redraw the scene
    }
    */
    void specialKeys(int key, int x, int y) {
        // Handle arrow keys
        if (key == GLUT_KEY_LEFT) {
            planeOffsetX -= 0.5f; // Move left
        } else if (key == GLUT_KEY_RIGHT) {
            planeOffsetX += 0.5f; // Move right
        }
        glutPostRedisplay(); // Redraw the scene
    }

    /*void keyboardUp(unsigned char key, int x, int y) {
        planeRotation = 0.0f; // Reset rotation
        glutPostRedisplay();
    }
    */

    void specialKeysUp(int key, int x, int y) {
        planeRotation = 0.0f; // Reset rotation
        glutPostRedisplay();
    }

    /* Reshape callback function */
    void reshape(int w, int h) {
        if (h == 0) h = 1; // Prevent divide by zero

        // Set the viewport to cover the new window
        glViewport(0, 0, w, h);
    }

    void idle() {
        updatePlane(); // Continuously update plane position and rotation
    }

    void reset() {
        counter = 0;    // Integer to display
        elapsedTime= 0.0f;
        // Rotation variable
        rotationAngle = 0.0f;

        // Plane movement variables
        planeOffsetX = 0.0f; // Offset for the plane to sync with scrolling
        planeOffsetY = 0.0f; // Offset for polygon movement in the x-axis
        planeRotation = 0.0f; // Track the rotation of the plane
        isKeyPressedA = false;
        isKeyPressedD = false;
        velocityX = 0.0f;
        targetRotation = 0.0f;

        // Variables to handle translation
        xOffset1 = 0.0f; // Offset for translation
        xOffset2 = 0.0f; // Offset for translation
        xOffset3 = 0.0f; // Offset for translation
        startTranslation1 = false; // Flag to start translation
        startTranslation2 = false;
        startTranslation3 = false;
        speed = 0.005f; // Speed of translation
        translationOffsets = {
            {xOffset1, 0.0f},
            {xOffset2, 0.0f},
            {-xOffset3, 0.0f},
            {xOffset3, 0.0f}
        }; // Corresponding offsets for each polygon

        startTime = clock(); // Record the start time

        transformedRotationPolygons.clear();
        transformedTranslationPolygons.clear();
        gameOverTriggered=false;
        plane = {
        // First polygon
        {
            {0.0f, -58.0f}, {0.3f, -59.0f}, {-0.3f, -59.0f}
        },
        // Second polygon
        {
            {0.3f, -59.0f}, {-0.3f, -59.0f}, {-0.8f, -59.4f},
            {-0.8f, -59.6f}, {0.8f, -59.6f}, {0.8f, -59.4f}
        },
        // Third polygon
        {
            {-0.2f, -59.6f}, {0.2f, -59.6f}, {0.2f, -59.8f}, {-0.2f, -59.8f}
        },
        // Fourth polygon
        {
            {-0.2f, -59.8f}, {-0.4f, -60.0f}, {-0.2f, -60.0f}, {-0.15f, -59.95f}
        },
        // Fifth polygon
        {
            {-0.2f, -59.8f}, {-0.15f, -59.95f}, {-0.1f, -60.0f},
            {0.1f, -60.0f}, {0.15f, -59.95f}, {0.2f, -59.8f}
        },
        // Sixth polygon
        {
            {0.2f, -59.8f}, {0.15f, -59.95f}, {0.2f, -60.0f}, {0.4f, -60.0f}
        },
        // First line
        {
            {-0.8f, -59.6f}, {-0.8f, -59.2f}
        },
        // Second line
        {
            {0.8f, -59.6f}, {0.8f, -59.2f}
        }
    };
    yMin = -60.0f, yMax = -40.0f;
    }
}

namespace levelThree{
    int counter = 0;    // Integer to display
    float elapsedTime= 0.0f;

    // Rotation variable
    float rotationAngle = 0.0f;

    // Initial y-axis bounds for the visible region
    float yMin = -60.0f, yMax = -40.0f;
    float step = 0.05;
    int scrollTime = 16;

    // Plane movement variables
    float planeOffsetX = 0.0f; // Offset for the plane to sync with scrolling
    float planeOffsetY = 0.0f; // Offset for polygon movement in the x-axis
    float planeRotation = 0.0f; // Track the rotation of the plane
    bool isKeyPressedA = false;
    bool isKeyPressedD = false;
    float velocityX = 0.0f;
    float targetRotation = 0.0f;
    const float moveSpeed = 0.01f;    // Speed for smooth movement
    const float rotationSpeed = 2.0f; // Speed for smooth rotation

    // Variables to handle translation
    float xOffset1 = 0.0f; // Offset for translation
    bool startTranslation1 = false; // Flag to start circle translation
    bool startTranslation2 = false; // Flag to start Polygon translation
    bool startRotation1 = false;
    float speed = 0.01f; // Speed of translation

    //Cricle Variables
    GLfloat radius =2.0f;
    int triangleAmount = 100; //# of lines used to draw circle

    // Corresponding offsets for each polygon
    vector<pair<float, float>> translationOffsets;

    clock_t startTime;    // Timer to check if certain seconds have passed
    void init() {
        startTime = clock(); // Record the start time
    }

    // Struct to represent 2D points (vertices)
    struct Point {
        float x, y;

        Point(float x, float y) {
            this->x = x;
            this->y = y;
        }
    };

    vector<vector<Point>> transformedRotationPolygons;
    vector<vector<Point>> transformedTranslationPolygons;
    vector<vector<Point>> obstacles = {
        // Obstacle 1: A->C->Z2->N1
        {
            {-4.0f, -60.0f}, {-4.0f, -56.0f}, {-15.0f, -56.0f}, {-15.0f, -60.0f}
        },
        // Obstacle 2: G->H->A3->Z2
        {
            {-6.0f, -56.0f}, {-6.0f, -48.0f}, {-15.0f, -48.0f}, {-15.0f, -56.0f}
        },
        // Obstacle 3: F->I->K->A3
        {
            {0.0f, -48.0f}, {0.0f, -45.0f}, {-15.0f, -45.0f}, {-15.0f, -48.0f}
        },
        // Obstacle 4: J->B->B3->O1
        {
            {4.0f, -60.0f}, {4.0f, -54.0f}, {15.0f, -54.0f}, {15.0f, -60.0f}
        },
        // Obstacle 5: B3->D->E->C3
        {
            {15.0f, -54.0f}, {-1.0f, -54.0f}, {-1.0f, -51.0f}, {15.0f, -51.0f}
        },
        // Obstacle 6: C3->M->N->L
        {
            {15.0f, -51.0f}, {4.0f, -51.0f}, {4.0f, -45.0f}, {15.0f, -45.0f}
        },
        // Obstacle 8: O->Q->W->Z->D3
        {
            {-15.0f, -20.0f}, {-5.0f, -20.0f}, {-2.0f, -18.0f}, {-6.0f, -16.0f}, {-15.0f, -16.0f}
        },
        // Obstacle 9: D3->A1->C1->E3
        {
            {-15.0f, -16.0f}, {-10.0f, -16.0f}, {-6.0f, -12.0f}, {-15.0f, -12.0f}
        },
        // Obstacle 10: E3->C1->D1->F3
        {
            {-15.0f, -12.0f}, {-6.0f, -12.0f}, {-6.0f, -10.0f}, {-15.0f, -10.0f}
        },
        // Obstacle 11: F3->D1->K1->G3
        {
            {-15.0f, -10.0f}, {-6.0f, -10.0f}, {-8.0f, -8.0f}, {-15.0f, -8.0f}
        },
        // Obstacle 12: G3->K1->L1->Q1->H3
        {
            {-15.0f, -8.0f}, {-8.0f, -8.0f}, {-4.0f, -6.0f}, {-6.0f, -4.0f}, {-15.0f, -4.0f}
        },
        // Obstacle 13: H3->Q1->S1->U1
        {
            {-15.0f, -4.0f}, {-8.0f, -8.0f}, {-6.0f, 0.0f}, {-15.0f, 0.0f}
        },
        // Obstacle 14: P->R->V->M3
        {
            {15.0f, -20.0f}, {5.0f, -20.0f}, {8.0f, -16.0f}, {15.0f, -16.0f}
        },
        // Obstacle 15: M3->V->B1->L3
        {
            {15.0f, -16.0f}, {8.0f, -16.0f}, {4.0f, -12.0f}, {15.0f, -12.0f}
        },
        // Obstacle 16: L3->E1->G1->K3
        {
            {15.0f, -12.0f}, {2.0f, -12.0f}, {2.0f, -10.0f}, {15.0f, -10.0f}
        },
        // Obstacle 17: K3->G1->H1->J3
        {
            {15.0f, -10.0f}, {2.0f, -10.0f}, {0.0f, -8.0f}, {15.0f, -8.0f}
        },
        // Obstacle 18: J3->H1->M1->I3
        {
            {15.0f, -8.0f}, {0.0f, -8.0f}, {4.0f, -4.0f}, {15.0f, -4.0f}
        },
        // Obstacle 19: I3->M1->P1->T1->V1
        {
            {15.0f, -4.0f}, {4.0f, -4.0f}, {2.0f, -2.0f}, {2.0f, 0.0f}, {15.0f, 0.0f}
        },
        // Line 1: A -> N1
        {
            {-4.0f, -60.0f}, {-15.0f, -60.0f}
        },
        // Line 2: N1 -> I1
        {
            {-15.0f, -60.0f}, {-15.0f, 60.0f}
        },
        // Line 3: I1 -> J1
        {
            {-15.0f, 65.0f}, {15.0f, 65.0f}
        },
        // Line 4: J1 -> O1
        {
            {15.0f, 60.0f}, {15.0f, -60.0f}
        },
        // Line 5: O1 -> A
        {
            {15.0f, -60.0f}, {4.0f, -60.0f}
        },
            // Polygon from E2->C2->D2->F2
        {
            {12.0f, 18.0f}, {-8.0f, 18.0f}, {-8.0f, 24.0f}, {12.0f, 24.0f}
        },
        // Polygon from W1->I2->A2->K2
        {
            {-15.0f, 30.0f}, {-4.0f, 30.0f}, {-4.0f, 34.0f}, {-15.0f, 34.0f}
        },
        // Polygon from Z1->J2->B2->L2
        {
            {15.0f, 30.0f}, {4.0f, 30.0f}, {4.0f, 34.0f}, {15.0f, 34.0f}
        },
        // Polygon from P2->W2->V2->U2
        {
            {-2.0f, 54.0f}, {0.0f, 56.0f}, {2.0f, 54.0f}, {0.0f, 52.0f}
        }
    };
    // Center vertices of the circles
    vector<pair<float, float>> circleCenter{
        {-8.0f, 5.0f}, {8.0f, 8.0f}
    };
    //store generated circle vertices
    vector<vector<Point>> circles{};

    vector<pair<float, float>> velocities{
        {0.002f, 0.00f}, {-0.002f, 0.00f}
    }; // Velocities of the circles
    vector<vector<Point>> rotationPolygons = {
        // Polygon from T2->N2->M2->O2
        {
            {-14.0f, 50.0f}, {-5.0f, 41.0f}, {-1.0f, 45.0f}, {-10.0f, 54.0f}
        },
        // Polygon from M2->Q2->S2->R2
        {
            {1.0f, 45.0f}, {5.0f, 41.0f}, {14.0f, 50.0f}, {10.0f, 54.0f}
        },

        // Obstacle 7: S->U->T
        {
            {0.0f, -25.0f}, {-11.0f, -35.0f}, {11.0f, -35.0f}
        }
    };

    vector<vector<Point>> translationPolygons = {
        //O3-> N3-> V3-> W3
        {{-8.0f,23.5f},{0.0f,23.5f}, {0.0f,22.5f}, {-8.0f,22.5f}},

        //Q3, P3, U3, T3
        {{-8.0f,21.5f},{0.0f,21.5f}, {0.0f,20.5f}, {-8.0f,20.5f}},

        //S3, R3, Z3, C2
        {{-8.0f,19.5f},{0.0f,19.5f},{0.0f,18.5f},{-8.0f,18.5f}}


    };

    void generateCircleVertices(const vector<pair<float, float>>& circleCenters) {
        GLfloat twicePi = 2.0f * M_PI;

        for (const auto& center : circleCenters) {
            vector<Point> circleVertices;
            GLfloat centerX = center.first;
            GLfloat centerY = center.second;

            // Add center of the circle
            circleVertices.push_back({centerX, centerY});

            // Calculate circle vertices
            for (int i = 0; i <= triangleAmount; i++) {
                GLfloat x = centerX + (radius * cos(i * twicePi / triangleAmount));
                GLfloat y = centerY + (radius * sin(i * twicePi / triangleAmount));
                circleVertices.push_back({x, y});
            }

            // Add circle vertices to the collection of circles
            circles.push_back(circleVertices);
        }
    }


    void updatePositions(vector<pair<float, float>>& circleCenters, vector<pair<float, float>>& velocities) {
        if(startTranslation1){
            for (size_t i = 0; i < circleCenters.size(); ++i) {
            // Update each circle's center position based on its velocity
            circleCenters[i].first += velocities[i].first;  // Update x-coordinate
            circleCenters[i].second += velocities[i].second;  // Update y-coordinate
            }
        }
    }


    // Function to handle collisions between two circles
    void handleBallCollision(int i, int j) {
        // Calculate the collision angle (angle of the line connecting the centers)
        float dx = circleCenter[j].first - circleCenter[i].first; // Difference in x-coordinates
        float dy = circleCenter[j].second - circleCenter[i].second; // Difference in y-coordinates
        float distance = sqrt(dx * dx + dy * dy);

        if (distance <= 2 * radius) {
            // Normalize the collision vector
            float nx = dx / distance;
            float ny = dy / distance;

            // Relative velocity
            float dvx = velocities[j].first - velocities[i].first;
            float dvy = velocities[j].second - velocities[i].second;

            // Calculate relative velocity along the collision normal
            float dotProduct = dvx * nx + dvy * ny;

            // If circles are moving apart, no need to resolve collision
            if (dotProduct > 0) return;

            // Impulse scalar
            float impulse = (2 * dotProduct) / (1.0f + 1.0f); // Assuming equal mass

            // Update velocities
            velocities[i].first += impulse * nx;
            velocities[i].second += impulse * ny;
            velocities[j].first -= impulse * nx;
            velocities[j].second -= impulse * ny;
        }
    }


     // Function to check and handle all circle circle collisions
    void checkCollisions() {
        for (size_t i = 0; i < circleCenter.size(); ++i) {
            for (size_t j = i + 1; j < circleCenter.size(); ++j) {
                handleBallCollision(i, j);
            }
        }
    }

    vector<Point> applyRotation(const vector<Point>& polygon, float angle) {
        vector<Point> transformedPolygon;
        float radians = angle * M_PI / 180.0f;

        // Calculate the center of the rectangle
        float minX = polygon[0].x, maxX = polygon[0].x;
        float minY = polygon[0].y, maxY = polygon[0].y;
        for (const auto& vertex : polygon) {
            minX = min(minX, vertex.x);
            maxX = max(maxX, vertex.x);
            minY = min(minY, vertex.y);
            maxY = max(maxY, vertex.y);
        }
        float centerX = (minX + maxX) / 2.0f;
        float centerY = (minY + maxY) / 2.0f;

        // Apply rotation around the center
        for (const auto& vertex : polygon) {
            float x = centerX + (vertex.x - centerX) * cos(radians) - (vertex.y - centerY) * sin(radians);
            float y = centerY + (vertex.x - centerX) * sin(radians) + (vertex.y - centerY) * cos(radians);
            transformedPolygon.push_back({x, y});
        }
        return transformedPolygon;
    }

    vector<Point> applyTranslation(const vector<Point>& polygon, float offsetX, float offsetY) {
        vector<Point> transformedPolygon;
        for (const auto& vertex : polygon) {
            transformedPolygon.push_back({vertex.x + offsetX, vertex.y + offsetY});
        }
        return transformedPolygon;
    }

    void applyMovement(){
        // Clear the transformed polygons
        transformedRotationPolygons.clear();
        transformedTranslationPolygons.clear();
        translationOffsets = {
        {xOffset1, 0.0f}, {xOffset1, 0.0f}, {xOffset1, 0.0f}
        };

        for (size_t i = 0; i < translationPolygons.size(); ++i) {
        transformedTranslationPolygons.push_back(
            applyTranslation(translationPolygons[i], translationOffsets[i].first, translationOffsets[i].second)
            );
        }
        if(startRotation1){
            for(const auto& polygon : rotationPolygons) {
            transformedRotationPolygons.push_back(
            applyRotation(polygon, rotationAngle)
            );
            }
        }
    }


    vector<vector<float>> planeColors = {
        // Color for the first polygon
        {0.698f, 0.745f, 0.710f},

        // Color for the second polygon
        {0.827f, 0.827f, 0.827f},

        // Color for the third polygon
        {0.698f, 0.745f, 0.710f},

        // Color for the fourth polygon
        {0.827f, 0.827f, 0.827f},

        // Color for the fifth polygon
        {0.827f, 0.827f, 0.827f},

        // Color for the sixth polygon
        {0.827f, 0.827f, 0.827f},

        // Color for the first line
        {0.0f, 0.0f, 0.0f},

        // Color for the second line
        {0.0f, 0.0f, 0.0f}
    };

    vector<vector<Point>> plane = {
        // First polygon
        {
            {0.0f, -58.0f}, {0.3f, -59.0f}, {-0.3f, -59.0f}
        },
        // Second polygon
        {
            {0.3f, -59.0f}, {-0.3f, -59.0f}, {-0.8f, -59.4f},
            {-0.8f, -59.6f}, {0.8f, -59.6f}, {0.8f, -59.4f}
        },
        // Third polygon
        {
            {-0.2f, -59.6f}, {0.2f, -59.6f}, {0.2f, -59.8f}, {-0.2f, -59.8f}
        },
        // Fourth polygon
        {
            {-0.2f, -59.8f}, {-0.4f, -60.0f}, {-0.2f, -60.0f}, {-0.15f, -59.95f}
        },
        // Fifth polygon
        {
            {-0.2f, -59.8f}, {-0.15f, -59.95f}, {-0.1f, -60.0f},
            {0.1f, -60.0f}, {0.15f, -59.95f}, {0.2f, -59.8f}
        },
        // Sixth polygon
        {
            {0.2f, -59.8f}, {0.15f, -59.95f}, {0.2f, -60.0f}, {0.4f, -60.0f}
        },
        // First line
        {
            {-0.8f, -59.6f}, {-0.8f, -59.2f}
        },
        // Second line
        {
            {0.8f, -59.6f}, {0.8f, -59.2f}
        }
    };

    // Function to draw a chequered bar
    void drawChequeredBar(float x1, float y1, float x2, float y2, float ySubdivision) {
        float width = x2 - x1; // Total width of the bar
        int numDivisions = static_cast<int>(width); // Number of subdivisions along x-axis

        // Top rectangle: From y1 to ySubdivision
        for (int i = 0; i < numDivisions; i++) {
            float xStart = x1 + i;
            float xEnd = x1 + i + 1;

            if (i % 2 == 0) {
                glColor3f(0.0, 0.0, 0.0); // Black
            } else {
                glColor3f(1.0, 1.0, 1.0); // White
            }

            glBegin(GL_POLYGON);
            glVertex2f(xStart, y1);
            glVertex2f(xEnd, y1);
            glVertex2f(xEnd, ySubdivision);
            glVertex2f(xStart, ySubdivision);
            glEnd();
        }

        // Bottom rectangle: From ySubdivision to y2
        for (int i = 0; i < numDivisions; i++) {
            float xStart = x1 + i;
            float xEnd = x1 + i + 1;

            if (i % 2 != 0) { // Alternate pattern
                glColor3f(0.0, 0.0, 0.0); // Black
            } else {
                glColor3f(1.0, 1.0, 1.0); // White
            }

            glBegin(GL_POLYGON);
            glVertex2f(xStart, ySubdivision);
            glVertex2f(xEnd, ySubdivision);
            glVertex2f(xEnd, y2);
            glVertex2f(xStart, y2);
            glEnd();
        }

        // Draw the top x-axis line
        glColor3f(0.0, 0.0, 0.0); // Black color for lines
        glBegin(GL_LINES);
        glVertex2f(x1, y1); // Start of the top edge
        glVertex2f(x2, y1); // End of the top edge
        glEnd();

        // Draw the bottom x-axis line
        glBegin(GL_LINES);
        glVertex2f(x1, y2); // Start of the bottom edge
        glVertex2f(x2, y2); // End of the bottom edge
        glEnd();
    }

    void drawPlane(const vector<vector<Point>>& plane, const vector<vector<float>>& colors) {
        glPushMatrix();
        glTranslatef(planeOffsetX, planeOffsetY, 0.0f);
        glRotatef(planeRotation, 0.0f, 1.0f, 0.0f); // Apply rotation

        for (size_t i = 0; i < plane.size(); ++i) {
            const auto& vertices = plane[i];
            const auto& color = colors[i];

            if (vertices.size() > 2) {
                // Draw polygons
                glBegin(GL_POLYGON);
                glColor3f(color[0], color[1], color[2]);
                for (const auto& vertex : vertices) {
                    glVertex2f(vertex.x, vertex.y);
                }
                glEnd();
            } else if (vertices.size() == 2) {
                // Draw lines
                glLineWidth(1.5f);
                glBegin(GL_LINES);
                glColor3f(color[0], color[1], color[2]);
                for (const auto& vertex : vertices) {
                    glVertex2f(vertex.x, vertex.y);
                }
                glEnd();
            }
        }

        glPopMatrix();
    }

    vector<vector<Point>> getTransformedPlane() {
        vector<vector<Point>> transformedPlane;
        for (const auto& polygon : plane) {
            vector<Point> transformedPolygon;
            for (const auto& vertex : polygon) {
                transformedPolygon.push_back({vertex.x + planeOffsetX, vertex.y + planeOffsetY});
            }
            transformedPlane.push_back(transformedPolygon);
        }
        return transformedPlane;
    }



    // Collision detection Start

    // Function to compute the dot product of two vectors
    float dotProduct(const Point& a, const Point& b) {
        return a.x * b.x + a.y * b.y;
    }

    // Function to calculate axes perpendicular to edges of a polygon
    vector<Point> getAxes(const vector<Point>& polygon) {
        vector<Point> axes;
        for (size_t i = 0; i < polygon.size(); ++i) {
            Point edge = {
                polygon[(i + 1) % polygon.size()].x - polygon[i].x,
                polygon[(i + 1) % polygon.size()].y - polygon[i].y
            };
            axes.push_back({-edge.y, edge.x}); // Perpendicular axis
        }
        return axes;
    }

    // Function to project a polygon onto an axis
    void projectOntoAxis(const vector<Point>& polygon, const Point& axis, float& minVal, float& maxVal) {
        minVal = maxVal = dotProduct(polygon[0], axis);
        for (const auto& vertex : polygon) {
            float projection = dotProduct(vertex, axis);
            if (projection < minVal) minVal = projection;
            if (projection > maxVal) maxVal = projection;
        }
    }

    // SAT-based collision detection between two polygons
    bool polygonsCollide(const vector<Point>& poly1, const vector<Point>& poly2) {
        vector<Point> axes1 = getAxes(poly1);
        vector<Point> axes2 = getAxes(poly2);

        for (const auto& axis : axes1) {
            float min1, max1, min2, max2;
            projectOntoAxis(poly1, axis, min1, max1);
            projectOntoAxis(poly2, axis, min2, max2);

            if (max1 < min2 || max2 < min1) {
                return false; // No overlap, polygons don't collide
            }
        }

        for (const auto& axis : axes2) {
            float min1, max1, min2, max2;
            projectOntoAxis(poly1, axis, min1, max1);
            projectOntoAxis(poly2, axis, min2, max2);

            if (max1 < min2 || max2 < min1) {
                return false; // No overlap, polygons don't collide
            }
        }

        return true; // All axes overlap, polygons collide
    }


    // Check if any polygon in the plane collides with any obstacle
    bool noCollision(const vector<vector<Point>>& plane, const vector<vector<Point>>& obstacles) {
        vector<vector<Point>> transformedPlane = getTransformedPlane(); // Transform the plane

        for (const auto& obstacle : obstacles) {
            for (const auto& transformedPolygon : transformedPlane) {
                if (polygonsCollide(transformedPolygon, obstacle)) {
                    return false; // Collision detected
                }
            }
        }

        return true; // No collisions
    }

    //Collision detection End


    void drawPolygons(const vector<vector<Point>>& polygons) {
        for (const auto& polygon : polygons) {
            glBegin(GL_POLYGON);
            glColor3f(0.514f, 0.871f, 0.925f); // Set color (light blue in this case)
            for (const auto& point : polygon) {
                glVertex2f(point.x, point.y); // Add vertex
            }
            glEnd();
        }
    }


    void storeCounter(int counter) {
        ofstream outFile("lib/counter.txt");  // Open the file in write mode
        if (outFile.is_open()) {
            outFile << counter;  // Write the counter value to the file
            outFile.close();  // Close the file after writing
        }
    }

    int retrieveCounter() {
        int counter = 0;
        ifstream inFile("lib/counter.txt");  // Open the file in read mode
        if (inFile.is_open()) {
            inFile >> counter;  // Read the counter value from the file
            inFile.close();  // Close the file after reading
        }
        return counter;
    }


    void renderText(float x, float y, const string& text, void* font = GLUT_BITMAP_HELVETICA_18) {
        glColor3f(0.0f, 0.0f, 0.0f); // Set text color to black
        glRasterPos2f(-14.8f, yMax - 1.0f);         // Position of the text in the window
        for (char c : text) {
            glutBitmapCharacter(font, c); // Render each character
        }
    }

    /* Display callback function */
    void display() {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Set background color to gray and opaque
        glClear(GL_COLOR_BUFFER_BIT);        // Clear the color buffer (background)

        // Reset projection matrix for dynamic scrolling
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-15.0f, 15.0f, yMin, yMax, -1.0f, 1.0f); // Set visible region dynamically
        glMatrixMode(GL_MODELVIEW);

        drawChequeredBar(-15.0f, 59.9f, 15.0f, 58.0f, 59.0f);

        drawPlane(plane, planeColors);


        // Update the counter at 2x per second
        counter = (int)(elapsedTime * 2);

         // Design 3: Start

        glLineWidth(15.0f);
        glBegin(GL_LINES);
        glColor3f(0.514f, 0.871f, 0.925f);
        // Line 1: A -> N1
        glVertex2f(-4.0f, -60.0f);
        glVertex2f(-15.0f, -60.0f);

        // Line 2: N1 -> I1
        glVertex2f(-15.0f, -60.0f);
        glVertex2f(-15.0f, 60.0f);

        // Line 3: I1 -> J1
        glVertex2f(-15.0f, 60.0f);
        glVertex2f(15.0f, 60.0f);

        // Line 4: J1 -> O1
        glVertex2f(15.0f, 60.0f);
        glVertex2f(15.0f, -60.0f);

        // Line 5: O1 -> A
        glVertex2f(15.0f, -60.0f);
        glVertex2f(4.0f, -60.0f);

        glEnd();

        // Draw Polygons
        drawPolygons(obstacles);
        drawPolygons(transformedTranslationPolygons);
        drawPolygons(transformedRotationPolygons);

        // Render the counter text in the top-left corner
        renderText(-16.0f, 15.0f, to_string(counter));

        // Update positions
        updatePositions(circleCenter, velocities);

        // Clear previous vertices
        circles.clear();

        // Generate vertices for updated positions
        generateCircleVertices(circleCenter);


        // Check and resolve collisions
        checkCollisions();

        // Render each circle using stored vertices
        glColor3f(0.514f, 0.871f, 0.925f);
        for (const auto& circle : circles) {
            glBegin(GL_TRIANGLE_FAN);
            for (const auto& vertex : circle) {
                glVertex2f(vertex.x, vertex.y);
            }
            glEnd();
        }

        // Render the counter text in the top-left corner
        renderText(-16.0f, 15.0f, to_string(counter));

        glFlush(); // Render now

        // Check if 10 seconds have passed
        if (((clock() - startTime) / CLOCKS_PER_SEC) >= 25) {
            xOffset1 -= speed; // Move left
        }
        if (((clock() - startTime) / CLOCKS_PER_SEC) >= 19) {
            startTranslation1 = true; // Start translation after 13 seconds
        }
        if (((clock() - startTime) / CLOCKS_PER_SEC) >= 0) {
            startRotation1 = true;
        }

    }

    void updatePlane() {
        if (isKeyPressedA) {
            velocityX = -moveSpeed;
            targetRotation = -45.0f;
        } else if (isKeyPressedD) {
            velocityX = moveSpeed;
            targetRotation = 45.0f;
        } else {
            velocityX = 0.0f; // Stop movement if no key is pressed
            targetRotation = 0.0f; // Reset rotation
        }

        // Update position
        planeOffsetX += velocityX;

        // Smoothly interpolate rotation towards the target
        if (planeRotation < targetRotation) {
            planeRotation += rotationSpeed;
            if (planeRotation > targetRotation) planeRotation = targetRotation; // Clamp to target
        } else if (planeRotation > targetRotation) {
            planeRotation -= rotationSpeed;
            if (planeRotation < targetRotation) planeRotation = targetRotation; // Clamp to target
        }

        glutPostRedisplay(); // Redraw the scene
    }

    /* Timer function for scrolling */
    void timer(int value) {
        if(!timer3) return;
        // Update the y-axis bounds
        if (noCollision(plane, obstacles) && noCollision(plane,transformedRotationPolygons) && noCollision(plane,transformedTranslationPolygons) && noCollision(plane,circles)){
            if (yMax <= 60.0f){
                yMin += step;
                yMax += step;
            }
            applyMovement();

            // Calculate elapsed time in seconds for Score
            elapsedTime = (float)(clock() - startTime) / CLOCKS_PER_SEC;
            planeOffsetY += step; // Move the plane up at the same rate as the scrolling
        }
        else if(!gameOverTriggered){
            glutDisplayFunc(gameOver::display);
            gameOverTriggered=true;
        }



        // Update the rotation angle for obstacle animation
        rotationAngle += 0.4f; // Increment the angle (degrees)
        if (rotationAngle >= 360.0f) {
            rotationAngle -= 360.0f; // Keep the angle within 0�360 degrees
        }
        if(planeOffsetY>=120.0f){
            glutDisplayFunc(levelComplete::display);
        }

        // Trigger a redisplay to reflect the updated viewport
        glutPostRedisplay();

        // Call this timer function again after the specified interval
        glutTimerFunc(scrollTime, timer, 0);

    }

    void keyboard(unsigned char key, int x, int y) {
        if (key == 'a' || key == 'A') {
            isKeyPressedA = true;
        } else if (key == 'd' || key == 'D') {
            isKeyPressedD = true;
        }
    }

    void keyboardUp(unsigned char key, int x, int y) {
        if (key == 'a' || key == 'A') {
            isKeyPressedA = false;
        } else if (key == 'd' || key == 'D') {
            isKeyPressedD = false;
        }
    }

    /* Reshape callback function */
    void reshape(int w, int h) {
        if (h == 0) h = 1; // Prevent divide by zero

        // Set the viewport to cover the new window
        glViewport(0, 0, w, h);
    }
    void idle() {
        updatePlane(); // Continuously update plane position and rotation
    }

    void reset(){
        counter = 0;    // Integer to display
        elapsedTime= 0.0f;

        // Rotation variable
        rotationAngle = 0.0f;

        // Initial y-axis bounds for the visible region
        yMin = -60.0f, yMax = -40.0f;

        // Plane movement variables
        planeOffsetX = 0.0f; // Offset for the plane to sync with scrolling
        planeOffsetY = 0.0f; // Offset for polygon movement in the x-axis
        planeRotation = 0.0f; // Track the rotation of the plane
        isKeyPressedA = false;
        isKeyPressedD = false;
        velocityX = 0.0f;
        targetRotation = 0.0f;

        // Variables to handle translation
        xOffset1 = 0.0f; // Offset for translation
        startTranslation1 = false; // Flag to start circle translation
        startTranslation2 = false; // Flag to start Polygon translation
        startRotation1 = false;
        speed = 0.01f; // Speed of translation

        // Corresponding offsets for each polygon
        translationOffsets.clear();
        transformedRotationPolygons.clear();
        transformedTranslationPolygons.clear();
        circles.clear();
    }


}

namespace levelFour{
    int counter = 0;    // Integer to display
    float elapsedTime= 0.0f;
    // Rotation variable
    float rotationAngle = 0.0f;
    // Initial y-axis bounds for the visible region
    float yMin = -60.0f, yMax = -40.0f;
    float step = 0.05;
    int scrollTime = 16;

    // Plane movement variables
    float planeOffsetX = 0.0f; // Offset for the plane to sync with scrolling
    float planeOffsetY = 0.0f; // Offset for polygon movement in the x-axis
    float planeRotation = 0.0f; // Track the rotation of the plane
    bool isKeyPressedA = false;
    bool isKeyPressedD = false;
    float velocityX = 0.0f;
    float targetRotation = 0.0f;
    const float moveSpeed = 0.01f;    // Speed for smooth movement
    const float rotationSpeed = 2.0f; // Speed for smooth rotation

    // Variables to handle translation
    float xOffset1 = 0.0f; // Offset for translation
    float xOffset2 = 0.0f; // Offset for translation
    float yOffset1;

    bool startTranslation1 = false; // Flag to start translation
    bool startTranslation2 = false;
    bool startTranslation3 = false;
    bool startTranslation4 = false;
    bool startRotation1 = false;
    float speed = 0.005f; // Speed of translation

    //Cricle Variables
    GLfloat radius =2.0f;
    int triangleAmount = 100; //# of lines used to draw circle

    vector<pair<float, float>> translationOffsets;// Corresponding offsets for each polygon

    clock_t startTime;    // Timer to check if certain seconds have passed
    void init() {
        startTime = clock(); // Record the start time
    }

    // Struct to represent 2D points (vertices)
    struct Point {
        float x, y;

        Point(float x, float y) {
            this->x = x;
            this->y = y;
        }
    };

    vector<vector<Point>> transformedRotationPolygons;
    vector<vector<Point>> transformedTranslationPolygons;
    // Center vertices of the circles
    vector<pair<float, float>> circleCenter{
        {-10.0f, -36.0f}, {-2.0f, -32.0f},
        {2.0f,-32.0f},{-4.0f,-28.0f},
        {0.0f,-28.0f},{4.0f,-28.0f},
        {-10.0f,24.0f},{10.0f,24.0f}
    };
    //store generated circle vertices
    vector<vector<Point>> circles;
    // Velocities of the circles
    vector<pair<float, float>> velocities{
        {0.063f, 0.06f}, {0.00f, 0.00f}, {0.00f, 0.00f}, {0.00f, 0.00f}, {0.00f, 0.00f}, {0.00f, 0.00f}, {0.01f, 0.01f}, {-0.01f, 0.01f}
    };
    vector<vector<Point>> rotationPolygons = {
        // Obstacle 11: L2->Q1->R1->L1
        {
            {4.0f, -6.0f}, {4.0f, -12.0f}, {-12.0f, -12.0f}, {-12.0f, -6.0f }
        },
        // Obstacle 12: P3->R3->S3->Q3
        {
            {4.0f, 18.0f}, {6.0f, 20.0f}, {1.0f, 25.0f}, {-1.0f, 23.0f}
        }
    };

    vector<vector<Point>> translationPolygons = {
        // Obstacle 13: D4->T4->U4->V4
        { {10.0f, 40.0f}, {10.0f, 38.0f}, {6.0f, 38.0f}, {6.0f, 40.0f} },

        // Obstacle 14: Q4->P4->S4->R4
        { {10.0f, 46.0f}, {10.0f, 44.0f}, {6.0f, 44.0f}, {6.0f, 46.0f} }


    };

    void decrementVelocities(vector<pair<float, float>>& velocities, float decrementValue) {
        static int decr = 0;
        decr++;
        if (decr < 10)
            return;
        decr = 0; // Reset the counter

        for (auto& velocity : velocities) {
            // Handle the x-component of velocity
            if (velocity.first > 0.0f) {
                velocity.first -= decrementValue; // Decrement positive values
            } else if (velocity.first < 0.0f) {
                velocity.first += decrementValue; // Increment negative values
            }

            // Handle the y-component of velocity
            if (velocity.second > 0.0f) {
                velocity.second -= decrementValue; // Decrement positive values
            } else if (velocity.second < 0.0f) {
                velocity.second += decrementValue; // Increment negative values
            }
        }
    }



    void generateCircleVertices(const vector<pair<float, float>>& circleCenters) {
        GLfloat twicePi = 2.0f * M_PI;

        for (const auto& center : circleCenters) {
            vector<Point> circleVertices;
            GLfloat centerX = center.first;
            GLfloat centerY = center.second;

            // Add center of the circle
            circleVertices.push_back({centerX, centerY});

            // Calculate circle vertices
            for (int i = 0; i <= triangleAmount; i++) {
                GLfloat x = centerX + (radius * cos(i * twicePi / triangleAmount));
                GLfloat y = centerY + (radius * sin(i * twicePi / triangleAmount));
                circleVertices.push_back({x, y});
            }

            // Add circle vertices to the collection of circles
            circles.push_back(circleVertices);
        }
    }

    void updatePositions(vector<pair<float, float>>& circleCenters, vector<pair<float, float>>& velocities) {
        if(startTranslation1){
            int iter=0;
            for (size_t i = 0; i < circleCenters.size(); ++i) {
                iter++;
            if(iter<=6){
                // Update each circle's center position based on its velocity
                circleCenters[i].first += velocities[i].first;  // Update x-coordinate
                circleCenters[i].second += velocities[i].second;  // Update y-coordinate
            }
            }
        }


        if(startTranslation2){
                int iter=0;
            for (size_t i = 0; i < circleCenters.size(); ++i) {
                iter++;
            if(iter>=6){
                // Update each circle's center position based on its velocity
                circleCenters[i].first += velocities[i].first;  // Update x-coordinate
                circleCenters[i].second += velocities[i].second;  // Update y-coordinate
            }
            }
        }
    }

    // Function to handle collisions between two circles
    void handleBallCollision(int i, int j) {
        // Calculate the collision angle (angle of the line connecting the centers)
        float dx = circleCenter[j].first - circleCenter[i].first; // Difference in x-coordinates
        float dy = circleCenter[j].second - circleCenter[i].second; // Difference in y-coordinates
        float distance = sqrt(dx * dx + dy * dy);

        if (distance <= 2 * radius) {
            // Normalize the collision vector
            float nx = dx / distance;
            float ny = dy / distance;

            // Relative velocity
            float dvx = velocities[j].first - velocities[i].first;
            float dvy = velocities[j].second - velocities[i].second;

            // Calculate relative velocity along the collision normal
            float dotProduct = dvx * nx + dvy * ny;

            // If circles are moving apart, no need to resolve collision
            if (dotProduct > 0) return;

            // Impulse scalar
            float impulse = (2 * dotProduct) / (1.0f + 1.0f); // Assuming equal mass

            // Update velocities
            velocities[i].first += impulse * nx;
            velocities[i].second += impulse * ny;
            velocities[j].first -= impulse * nx;
            velocities[j].second -= impulse * ny;
        }
    }


     // Function to check and handle all circle circle collisions
    void checkCollisions() {
        for (size_t i = 0; i < circleCenter.size(); ++i) {
            for (size_t j = i + 1; j < circleCenter.size(); ++j) {
                handleBallCollision(i, j);
            }
        }
    }

    vector<Point> applyRotation(const vector<Point>& polygon, float angle) {
        vector<Point> transformedPolygon;
        float radians = angle * M_PI / 180.0f;

        // Calculate the center of the rectangle
        float minX = polygon[0].x, maxX = polygon[0].x;
        float minY = polygon[0].y, maxY = polygon[0].y;
        for (const auto& vertex : polygon) {
            minX = min(minX, vertex.x);
            maxX = max(maxX, vertex.x);
            minY = min(minY, vertex.y);
            maxY = max(maxY, vertex.y);
        }
        float centerX = (minX + maxX) / 2.0f;
        float centerY = (minY + maxY) / 2.0f;

        // Apply rotation around the center
        for (const auto& vertex : polygon) {
            float x = centerX + (vertex.x - centerX) * cos(radians) - (vertex.y - centerY) * sin(radians);
            float y = centerY + (vertex.x - centerX) * sin(radians) + (vertex.y - centerY) * cos(radians);
            transformedPolygon.push_back({x, y});
        }
        return transformedPolygon;
    }

    vector<Point> applyTranslation(const vector<Point>& polygon, float offsetX, float offsetY) {
        vector<Point> transformedPolygon;
        for (const auto& vertex : polygon) {
            transformedPolygon.push_back({vertex.x + offsetX, vertex.y + offsetY});
        }
        return transformedPolygon;
    }


    void applyMovement(){
        // Clear the transformed polygons
        transformedRotationPolygons.clear();
        transformedTranslationPolygons.clear();
        translationOffsets = {{xOffset1, 0.0f}, {xOffset2, 0.0f}, {0.0f, 0.0f}};

        if(startTranslation3){
            for (size_t i = 0; i < 2; i++) {
            transformedTranslationPolygons.push_back(
            applyTranslation(translationPolygons[i], translationOffsets[i].first, translationOffsets[i].second)
            );
            }
        }
        if(startTranslation4){
            for (size_t i = 2; i < 3; i++) {
            transformedTranslationPolygons.push_back(
            applyTranslation(translationPolygons[i], translationOffsets[i].first, translationOffsets[i].second)
            );
            }
        }
        if(startRotation1){
                for(const auto& polygon : rotationPolygons) {
                        transformedRotationPolygons.push_back(
                        applyRotation(polygon, rotationAngle)
                        );
                }
        }
    }

    vector<vector<float>> planeColors = {
        // Color for the first polygon
        {0.698f, 0.745f, 0.710f},

        // Color for the second polygon
        {0.827f, 0.827f, 0.827f},

        // Color for the third polygon
        {0.698f, 0.745f, 0.710f},

        // Color for the fourth polygon
        {0.827f, 0.827f, 0.827f},

        // Color for the fifth polygon
        {0.827f, 0.827f, 0.827f},

        // Color for the sixth polygon
        {0.827f, 0.827f, 0.827f},

        // Color for the first line
        {0.0f, 0.0f, 0.0f},

        // Color for the second line
        {0.0f, 0.0f, 0.0f}
    };

    vector<vector<Point>> plane = {
        // First polygon
        {
            {0.0f, -58.0f}, {0.3f, -59.0f}, {-0.3f, -59.0f}
        },
        // Second polygon
        {
            {0.3f, -59.0f}, {-0.3f, -59.0f}, {-0.8f, -59.4f},
            {-0.8f, -59.6f}, {0.8f, -59.6f}, {0.8f, -59.4f}
        },
        // Third polygon
        {
            {-0.2f, -59.6f}, {0.2f, -59.6f}, {0.2f, -59.8f}, {-0.2f, -59.8f}
        },
        // Fourth polygon
        {
            {-0.2f, -59.8f}, {-0.4f, -60.0f}, {-0.2f, -60.0f}, {-0.15f, -59.95f}
        },
        // Fifth polygon
        {
            {-0.2f, -59.8f}, {-0.15f, -59.95f}, {-0.1f, -60.0f},
            {0.1f, -60.0f}, {0.15f, -59.95f}, {0.2f, -59.8f}
        },
        // Sixth polygon
        {
            {0.2f, -59.8f}, {0.15f, -59.95f}, {0.2f, -60.0f}, {0.4f, -60.0f}
        },
        // First line
        {
            {-0.8f, -59.6f}, {-0.8f, -59.2f}
        },
        // Second line
        {
            {0.8f, -59.6f}, {0.8f, -59.2f}
        }
    };

    vector<vector<Point>> obstacles = {
        //Obstacle 0: N1->A4->C3->W4
        {{-15.0f, -60.0f}, {-4.0f, -60.0f}, {-4.0f, -56.0f}, {-15.0f, -56.0f} },
        // Obstacle 1: A->C->Z2->N1
        { {-4.0f, -60.0f}, {-4.0f, -56.0f}, {-15.0f, -56.0f}, {-15.0f, -60.0f} },

        // Obstacle 2: W4->G2->H2->Z4
        { {-6.0f, -56.0f}, {-6.0f, -48.0f}, {-15.0f, -48.0f}, {-15.0f, -56.0f} },

        // Obstacle 3: Z4->F2->B->A5
        { {0.0f, -48.0f}, {0.0f, -46.0f}, {-15.0f, -46.0f}, {-15.0f, -48.0f} },

        // Obstacle 4: A5->C->D->F
        { {-4.0f, -46.0f}, {-4.0f, -40.0f}, {-15.0f, -40.0f}, {-15.0f, -46.0f} },

        // Obstacle 5: O1->J3->B3->B5
        { {4.0f, -60.0f}, {4.0f, -54.0f}, {15.0f, -54.0f}, {15.0f, -60.0f} },

        // Obstacle 6: B5->D2->E2->C5
        { {-1.0f, -54.0f}, {-1.0f, -51.0f}, {15.0f, -51.0f}, {15.0f, -54.0f} },

        // Obstacle 7: C5->M2->A->E
        { {4.0f, -51.0f}, {4.0f, -40.0f}, {15.0f, -40.0f}, {15.0f, -51.0f} },

        // Obstacle 8: G->H->Q->T
        { {-5.0f, -20.0f}, {-5.0f, -15.0f}, {-15.0f, -15.0f}, {-15.0f, -20.0f} },

        // Obstacle 9: J->I->R->S
        { {5.0f, -20.0f}, {5.0f, -15.0f}, {15.0f, -15.0f}, {15.0f, -20.0f} },

        // Obstacle 10: O2->N2->P2->S1
        { {8.0f, -12.0f}, {8.0f, -6.0f}, {15.0f, -6.0f}, {15.0f, -12.0f} },

        // Obstacle 15: B4->Z3->E4->J1
        { {5.0f, 30.0f}, {5.0f, 60.0f}, {15.0f, 60.0f}, {15.0f, 30.0f} },

        // Obstacle 16: N4->O4->M4->L4
        { {4.0f, 54.0f}, {4.0f, 58.0f}, {2.0f, 58.0f}, {2.0f, 54.0f} },

        // Obstacle 17: I4->H4->J4->K4
        { {-6.0f, 52.0f}, {-6.0f, 48.0f}, {-2.0f, 48.0f}, {-2.0f, 52.0f} },

        // Obstacle 18: V3->W3->C4->D5
        { {-5.0f, 30.0f}, {-5.0f, 40.0f}, {-15.0f, 40.0f}, {-15.0f, 30.0f} },

        // Obstacle 19: D5->C4->F4->E5
        { {-5.0f, 40.0f}, {-10.0f, 50.0f}, {-15.0f, 50.0f}, {-15.0f, 40.0f} },

        // Obstacle 20: E5->F4->G4->I1
        { {-10.0f, 50.0f}, {-2.0f, 60.0f}, {-15.0f, 60.0f}, {-15.0f, 50.0f} },

        // Obstacle 21: V->Q2->F5->U
        { {-5.0f, 2.0f}, {-15.0f, 2.0f}, {-15.0f, 0.0f}, {-5.0f, 0.0f} },

        // Obstacle 22: Q2->T2->G5->F5
        { {-6.0f, 4.0f}, {-15.0f, 4.0f}, {-15.0f, 2.0f}, {-5.0f, 0.0f} },

        // Obstacle 23: T2->U2->H5->G5
        { {-4.6f, 6.0f}, {-15.0f, 6.0f}, {-15.0f, 4.0f}, {-6.0f, 4.0f} },
        // Obstacle 24: from V2->D3->I5->H5
        {{-6.0f, 6.0f}, {-8.0f, 8.0f}, {-15.0f, 8.0f}, {-15.0f, 6.0f}},

        // Obstacle 25: from D3->E3->J5->I5
        {{-8.0f, 8.0f}, {-8.0f, 10.0f}, {-15.0f, 10.0f}, {-15.0f, 8.0f}},

        // Obstacle 26: from E3->G3->K5->J5
        {{-8.0f, 10.0f}, {-6.0f, 12.0f}, {-15.0f, 12.0f}, {-15.0f, 10.0f}},

        // Obstacle 27: from G3->I3->O3->K5
        {{-6.0f, 12.0f}, {-6.0f, 16.0f}, {-15.0f, 16.0f}, {-15.0f, 12.0f}},

        // Obstacle 28: from Z->W->P2->L5
        {{15.0f, 0.0f}, {5.0f, 0.0f}, {5.0f, 2.0f}, {15.0f, 2.0f}},

        // Obstacle 29: from L5->P2->R2->M5
        {{15.0f, 2.0f}, {5.0f, 2.0f}, {2.0f, 4.0f}, {15.0f, 4.0f}},

        // Obstacle 30: from M5->R2->S2->N5
        {{15.0f, 4.0f}, {2.0f, 4.0f}, {0.0f, 6.0f}, {15.0f, 6.0f}},

        // Obstacle 31: from N5->S2->W2->O5
        {{15.0f, 6.0f}, {0.0f, 6.0f}, {-2.0f, 8.0f}, {15.0f, 8.0f}},

        // Obstacle 32: from O5->W2->Z2->P5
        {{15.0f, 8.0f}, {-2.0f, 8.0f}, {-4.0f, 10.0f}, {15.0f, 10.0f}},

        // Obstacle 33: from P5->Z2->F3->Q5
        {{15.0f, 10.0f}, {-4.0f, 10.0f}, {-2.0f, 12.0f}, {15.0f, 12.0f}},

        // Obstacle 34: from Q5->H3->L3->R5
        {{15.0f, 12.0f}, {2.0f, 12.0f}, {4.0f, 14.0f}, {15.0f, 14.0f}},

        // Obstacle 35: from R5->L3->M3->N3
        {{15.0f, 14.0f}, {4.0f, 14.0f}, {4.0f, 16.0f}, {15.0f, 16.0f}},
        // Line 1: A -> N1
        {
            {-4.0f, -60.0f}, {-15.0f, -60.0f}
        },
        // Line 2: N1 -> I1
        {
            {-15.0f, -60.0f}, {-15.0f, 60.0f}
        },
        // Line 3: I1 -> J1
        {
            {-15.0f, 65.0f}, {15.0f, 65.0f}
        },
        // Line 4: J1 -> O1
        {
            {15.0f, 60.0f}, {15.0f, -60.0f}
        },
        // Line 5: O1 -> A
        {
            {15.0f, -60.0f}, {4.0f, -60.0f}
        },
        // Line 6: I3 -> K3
        {
            {-6.0f, 16.0f}, {-2.0f, 16.0f}
        }
    };

    // Function to draw a chequered bar
    void drawChequeredBar(float x1, float y1, float x2, float y2, float ySubdivision) {
        float width = x2 - x1; // Total width of the bar
        int numDivisions = static_cast<int>(width); // Number of subdivisions along x-axis

        // Top rectangle: From y1 to ySubdivision
        for (int i = 0; i < numDivisions; i++) {
            float xStart = x1 + i;
            float xEnd = x1 + i + 1;

            if (i % 2 == 0) {
                glColor3f(0.0, 0.0, 0.0); // Black
            } else {
                glColor3f(1.0, 1.0, 1.0); // White
            }

            glBegin(GL_POLYGON);
            glVertex2f(xStart, y1);
            glVertex2f(xEnd, y1);
            glVertex2f(xEnd, ySubdivision);
            glVertex2f(xStart, ySubdivision);
            glEnd();
        }

        // Bottom rectangle: From ySubdivision to y2
        for (int i = 0; i < numDivisions; i++) {
            float xStart = x1 + i;
            float xEnd = x1 + i + 1;

            if (i % 2 != 0) { // Alternate pattern
                glColor3f(0.0, 0.0, 0.0); // Black
            } else {
                glColor3f(1.0, 1.0, 1.0); // White
            }

            glBegin(GL_POLYGON);
            glVertex2f(xStart, ySubdivision);
            glVertex2f(xEnd, ySubdivision);
            glVertex2f(xEnd, y2);
            glVertex2f(xStart, y2);
            glEnd();
        }

        // Draw the top x-axis line
        glColor3f(0.0, 0.0, 0.0); // Black color for lines
        glBegin(GL_LINES);
        glVertex2f(x1, y1); // Start of the top edge
        glVertex2f(x2, y1); // End of the top edge
        glEnd();

        // Draw the bottom x-axis line
        glBegin(GL_LINES);
        glVertex2f(x1, y2); // Start of the bottom edge
        glVertex2f(x2, y2); // End of the bottom edge
        glEnd();
    }

    void drawPlane(const vector<vector<Point>>& plane, const vector<vector<float>>& colors) {
        glPushMatrix();
        glTranslatef(planeOffsetX, planeOffsetY, 0.0f);
        glRotatef(planeRotation, 0.0f, 1.0f, 0.0f); // Apply rotation

        for (size_t i = 0; i < plane.size(); ++i) {
            const auto& vertices = plane[i];
            const auto& color = colors[i];

            if (vertices.size() > 2) {
                // Draw polygons
                glBegin(GL_POLYGON);
                glColor3f(color[0], color[1], color[2]);
                for (const auto& vertex : vertices) {
                    glVertex2f(vertex.x, vertex.y);
                }
                glEnd();
            } else if (vertices.size() == 2) {
                // Draw lines
                glLineWidth(1.5f);
                glBegin(GL_LINES);
                glColor3f(color[0], color[1], color[2]);
                for (const auto& vertex : vertices) {
                    glVertex2f(vertex.x, vertex.y);
                }
                glEnd();
            }
        }

        glPopMatrix();
    }

    vector<vector<Point>> getTransformedPlane() {
        vector<vector<Point>> transformedPlane;
        for (const auto& polygon : plane) {
            vector<Point> transformedPolygon;
            for (const auto& vertex : polygon) {
                transformedPolygon.push_back({vertex.x + planeOffsetX, vertex.y + planeOffsetY});
            }
            transformedPlane.push_back(transformedPolygon);
        }
        return transformedPlane;
    }

    // Collision detection Start

    // Function to compute the dot product of two vectors
    float dotProduct(const Point& a, const Point& b) {
        return a.x * b.x + a.y * b.y;
    }

    // Function to calculate axes perpendicular to edges of a polygon
    vector<Point> getAxes(const vector<Point>& polygon) {
        vector<Point> axes;
        for (size_t i = 0; i < polygon.size(); ++i) {
            Point edge = {
                polygon[(i + 1) % polygon.size()].x - polygon[i].x,
                polygon[(i + 1) % polygon.size()].y - polygon[i].y
            };
            axes.push_back({-edge.y, edge.x}); // Perpendicular axis
        }
        return axes;
    }

    // Function to project a polygon onto an axis
    void projectOntoAxis(const vector<Point>& polygon, const Point& axis, float& minVal, float& maxVal) {
        minVal = maxVal = dotProduct(polygon[0], axis);
        for (const auto& vertex : polygon) {
            float projection = dotProduct(vertex, axis);
            if (projection < minVal) minVal = projection;
            if (projection > maxVal) maxVal = projection;
        }
    }

    // SAT-based collision detection between two polygons
    bool polygonsCollide(const vector<Point>& poly1, const vector<Point>& poly2) {
        vector<Point> axes1 = getAxes(poly1);
        vector<Point> axes2 = getAxes(poly2);

        for (const auto& axis : axes1) {
            float min1, max1, min2, max2;
            projectOntoAxis(poly1, axis, min1, max1);
            projectOntoAxis(poly2, axis, min2, max2);

            if (max1 < min2 || max2 < min1) {
                return false; // No overlap, polygons don't collide
            }
        }

        for (const auto& axis : axes2) {
            float min1, max1, min2, max2;
            projectOntoAxis(poly1, axis, min1, max1);
            projectOntoAxis(poly2, axis, min2, max2);

            if (max1 < min2 || max2 < min1) {
                return false; // No overlap, polygons don't collide
            }
        }

        return true; // All axes overlap, polygons collide
    }


    // Check if any polygon in the plane collides with any obstacle
    bool noCollision(const vector<vector<Point>>& plane, const vector<vector<Point>>& obstacles) {
        vector<vector<Point>> transformedPlane = getTransformedPlane(); // Transform the plane

        for (const auto& obstacle : obstacles) {
            for (const auto& transformedPolygon : transformedPlane) {
                if (polygonsCollide(transformedPolygon, obstacle)) {
                    return true; // Collision detected
                }
            }
        }

        return true; // No collisions
    }

    //Collision detection End


    void drawPolygons(const vector<vector<Point>>& polygons) {
        for (const auto& polygon : polygons) {
            glBegin(GL_POLYGON);
            glColor3f(0.514f, 0.871f, 0.925f); // Set color (light blue in this case)
            for (const auto& point : polygon) {
                glVertex2f(point.x, point.y); // Add vertex
            }
            glEnd();
        }
    }


    void storeCounter(int counter) {
        ofstream outFile("lib/counter.txt");  // Open the file in write mode
        if (outFile.is_open()) {
            outFile << counter;  // Write the counter value to the file
            outFile.close();  // Close the file after writing
        }
    }

    int retrieveCounter() {
        int counter = 0;
        ifstream inFile("lib/counter.txt");  // Open the file in read mode
        if (inFile.is_open()) {
            inFile >> counter;  // Read the counter value from the file
            inFile.close();  // Close the file after reading
        }
        return counter;
    }


    void renderText(float x, float y, const string& text, void* font = GLUT_BITMAP_HELVETICA_18) {
        glColor3f(0.0f, 0.0f, 0.0f); // Set text color to black
        glRasterPos2f(-14.8f, yMax - 1.0f);         // Position of the text in the window
        for (char c : text) {
            glutBitmapCharacter(font, c); // Render each character
        }
    }

    /* Display callback function */
    void display() {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Set background color to gray and opaque
        glClear(GL_COLOR_BUFFER_BIT);        // Clear the color buffer (background)

        // Reset projection matrix for dynamic scrolling
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-15.0f, 15.0f, yMin, yMax, -1.0f, 1.0f); // Set visible region dynamically
        glMatrixMode(GL_MODELVIEW);

        drawChequeredBar(-15.0f, 59.9f, 15.0f, 58.0f, 59.0f);
        drawPlane(plane, planeColors);

        // Update the counter at 2x per second
        counter = (int)(elapsedTime * 2);

        // Design 4: Start

        glLineWidth(15.0f);
        glBegin(GL_LINES);
        glColor3f(0.514f, 0.871f, 0.925f);
        // Line 1: A -> N1
        glVertex2f(-4.0f, -60.0f);
        glVertex2f(-15.0f, -60.0f);

        // Line 2: N1 -> I1
        glVertex2f(-15.0f, -60.0f);
        glVertex2f(-15.0f, 60.0f);

        // Line 3: I1 -> J1
        glVertex2f(-15.0f, 65.0f);
        glVertex2f(15.0f, 65.0f);

        // Line 4: J1 -> O1
        glVertex2f(15.0f, 60.0f);
        glVertex2f(15.0f, -60.0f);

        // Line 5: O1 -> A
        glVertex2f(15.0f, -60.0f);
        glVertex2f(4.0f, -60.0f);

        // Line 6: I3->K3
        glVertex2f(-6.0f, 16.0f);
        glVertex2f(-2.0f, 16.0f);

        glEnd();

        // Draw Polygons
        drawPolygons(obstacles);
        if(startRotation1)
            drawPolygons(transformedRotationPolygons);
        else
            drawPolygons(rotationPolygons);

            drawPolygons(transformedTranslationPolygons);

        // Update positions
        updatePositions(circleCenter, velocities);


        decrementVelocities(velocities, 0.000005f);

        // Clear previous vertices
        circles.clear();

        // Generate vertices for updated positions
        generateCircleVertices(circleCenter);

        // Check and resolve collisions
        checkCollisions();

        // Render each circle using stored vertices
        glColor3f(0.514f, 0.871f, 0.925f);
        for (const auto& circle : circles) {
            glBegin(GL_TRIANGLE_FAN);
            for (const auto& vertex : circle) {
                glVertex2f(vertex.x, vertex.y);
            }
            glEnd();
        }
        // Render each circle using stored vertices
        glColor3f(0.514f, 0.871f, 0.925f);
        for (const auto& circle : circles) {
            glBegin(GL_TRIANGLE_FAN);
            for (const auto& vertex : circle) {
                glVertex2f(vertex.x, vertex.y);
            }
            glEnd();
        }
        // Render the counter text in the top-left corner
        renderText(-16.0f, 15.0f, to_string(counter));

        glFlush(); // Render now

       if (((clock() - startTime) / CLOCKS_PER_SEC) >= 5) {
            startRotation1 = true; // Start translation after 13 seconds
        }

        if (((clock() - startTime) / CLOCKS_PER_SEC) >= 5) {
            startTranslation1 = true; // Start translation after 13 seconds
        }
        if (((clock() - startTime) / CLOCKS_PER_SEC) >= 31) {
            xOffset1 -= speed; // Move left
            startTranslation2 = true; // Start translation after 13 seconds
        }
        if (((clock() - startTime) / CLOCKS_PER_SEC) >= 30) {
            xOffset2 -= speed; // Move left
            startTranslation3 = true; // Start translation after 13 seconds
        }
    }

    void updatePlane() {
        if (isKeyPressedA) {
            velocityX = -moveSpeed;
            targetRotation = -45.0f;
        } else if (isKeyPressedD) {
            velocityX = moveSpeed;
            targetRotation = 45.0f;
        } else {
            velocityX = 0.0f; // Stop movement if no key is pressed
            targetRotation = 0.0f; // Reset rotation
        }

        // Update position
        planeOffsetX += velocityX;

        // Smoothly interpolate rotation towards the target
        if (planeRotation < targetRotation) {
            planeRotation += rotationSpeed;
            if (planeRotation > targetRotation) planeRotation = targetRotation; // Clamp to target
        } else if (planeRotation > targetRotation) {
            planeRotation -= rotationSpeed;
            if (planeRotation < targetRotation) planeRotation = targetRotation; // Clamp to target
        }

        glutPostRedisplay(); // Redraw the scene
    }

    /* Timer function for scrolling */
    void timer(int value) {
        if(!timer4) return;
        // Update the y-axis bounds
        if (noCollision(plane, obstacles) && noCollision(plane,transformedRotationPolygons) && noCollision(plane,transformedTranslationPolygons) && noCollision(plane,circles)){
            if (yMax <= 60.0f){
                yMin += step;
                yMax += step;
            }
            applyMovement();

            // Calculate elapsed time in seconds for Score
            elapsedTime = (float)(clock() - startTime) / CLOCKS_PER_SEC;
            planeOffsetY += step; // Move the plane up at the same rate as the scrolling
        }
        else if(!gameOverTriggered){
            glutDisplayFunc(gameOver::display);
            gameOverTriggered=true;
        }

        // Update the rotation angle for obstacle animation
        rotationAngle += 1.0f; // Increment the angle (degrees)
        if (rotationAngle >= 360.0f) {
            rotationAngle -= 360.0f; // Keep the angle within 0�360 degrees
        }

        if(planeOffsetY>=120.0f){
            glutDisplayFunc(levelComplete::display);
        }

        // Call this timer function again after the specified interval
        glutTimerFunc(scrollTime, timer, 0);

    }

    void keyboard(unsigned char key, int x, int y) {
        if (key == 'a' || key == 'A') {
            isKeyPressedA = true;
        } else if (key == 'd' || key == 'D') {
            isKeyPressedD = true;
        }
    }

    void keyboardUp(unsigned char key, int x, int y) {
        if (key == 'a' || key == 'A') {
            isKeyPressedA = false;
        } else if (key == 'd' || key == 'D') {
            isKeyPressedD = false;
        }
    }


    /* Reshape callback function */
    void reshape(int w, int h) {
        if (h == 0) h = 1; // Prevent divide by zero

        // Set the viewport to cover the new window
        glViewport(0, 0, w, h);
    }
    void idle() {
        updatePlane(); // Continuously update plane position and rotation
    }
    void reset(){
        counter = 0;    // Integer to display
        elapsedTime= 0.0f;
        // Rotation variable
        rotationAngle = 0.0f;
        // Initial y-axis bounds for the visible region
        yMin = -60.0f, yMax = -40.0f;
        step = 0.05;
        scrollTime = 16;

        // Plane movement variables
        planeOffsetX = 0.0f; // Offset for the plane to sync with scrolling
        planeOffsetY = 0.0f; // Offset for polygon movement in the x-axis
        planeRotation = 0.0f; // Track the rotation of the plane
        isKeyPressedA = false;
        isKeyPressedD = false;
        velocityX = 0.0f;
        targetRotation = 0.0f;

        // Variables to handle translation
        xOffset1 = 0.0f; // Offset for translation
        xOffset2 = 0.0f; // Offset for translation
        yOffset1;

        startTranslation1 = false; // Flag to start translation
        startTranslation2 = false;
        startTranslation3 = false;
        startTranslation4 = false;
        startRotation1 = false;
        speed = 0.005f; // Speed of translation

        translationOffsets.clear();// Corresponding offsets for each polygon

        transformedRotationPolygons.clear();
        transformedTranslationPolygons.clear();
        circles.clear();
        // Velocities of the circles
        velocities = {
    {0.063f, 0.06f}, {0.00f, 0.00f}, {0.00f, 0.00f}, {0.00f, 0.00f}, {0.00f, 0.00f}, {0.00f, 0.00f}, {0.01f, 0.01f}, {-0.01f, 0.01f}
        };

    }
}

namespace levelFive{
    int counter = 0;    // Integer to display
    float elapsedTime= 0.0f;
    // Rotation variable
    float rotationAngle = 0.0f;
    // Initial y-axis bounds for the visible region
    float yMin = -60.0f, yMax = -40.0f;
    float step = 0.05;
    int scrollTime = 16;

    // Plane movement variables
    float planeOffsetX = 0.0f; // Offset for the plane to sync with scrolling
    float planeOffsetY = 0.0f; // Offset for polygon movement in the x-axis
    float planeRotation = 0.0f; // Track the rotation of the plane
    bool isKeyPressedA = false;
    bool isKeyPressedD = false;
    float velocityX = 0.0f;
    float targetRotation = 0.0f;
    const float moveSpeed = 0.01f;    // Speed for smooth movement
    const float rotationSpeed = 2.0f; // Speed for smooth rotation

    // Variables to handle translation
    float xOffset1 = 0.0f; // Offset for translation
    float xOffset2 = 0.0f; // Offset for translation
    float yOffset1;

    bool startTranslation1 = false; // Flag to start translation
    bool startTranslation2 = false;
    bool startTranslation3 = false;
    bool startTranslation4 = false;
    bool startRotation1 = true;
    float speed = 0.005f; // Speed of translation

    //Cricle Variables
    GLfloat radius =0.9f;
    int triangleAmount = 100; //# of lines used to draw circle

    vector<pair<float, float>> translationOffsets;// Corresponding offsets for each polygon

    clock_t startTime;    // Timer to check if certain seconds have passed
    void init() {
        startTime = clock(); // Record the start time
    }

    vector<vector<Point>> transformedRotationPolygons;
    vector<vector<Point>> transformedTranslationPolygons;
    // Center vertices of the circles
    vector<pair<float, float>> circleCenter{
        {-7.0f, -23.0f}, {-6.0f, -28.0f},
        {-9.0f,-31.0f},{5.0f,-22.0f},
        {10.0f,-23.0f},{11.0f,-25.0f},
        {7.0f,-27.0f},{4.0f,-29.0f},
        {8.0f,-30.0f},
        //2nd block
        {-8.0f,16.0f},{-4.0f,16.0f},{4.0f,16.0f},{8.0f,16.0f},
        //3rd block
        {4.0f,25.0f},{6.0f,25.0f},{5.0f,23.0f},{7.0f,23.0f}, {8.0f,25.0f},{1.5f,30.0f},{2.0f,34.0f}
    };
    //store generated circle vertices
    vector<vector<Point>> circles;
    // Velocities of the circles
    vector<pair<float, float>> velocities{
        {0.05f, 0.05f}, {0.08f, 0.00f}, {0.00f, 0.00f}, {0.00f, 0.00f}, {0.00f, 0.00f}, {0.00f, 0.00f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f},
        {0.09f, 0.0f}, {0.00f, 0.00f}, {0.0f, 0.00f}, {0.02f, 0.05f}, {0.00f, 0.00f}, {0.00f, 0.00f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}
    };
    vector<vector<Point>> rotationPolygons = {
        //FROM E->F
    {{-8.0f, -53.0f},{-7.5f, -53.0f},{-7.5f, -58.0f}, {-8.0f, -58.0f}},

    //FROM B->C
    {{-11.0f, -55.5f},{-11.0f, -55.0f},{-5.0f, -55.0f}, {-5.0f, -55.5f}},

    //FROM H->K
    {{7.0f, -53.0f},{7.5f, -53.0f},{12.5f, -57.0f}, {12.0f, -57.0f}},

    //FROM J->I
    {{7.0f, -57.0f}, {7.5f, -57.0f},{12.5f, -53.0f}, {12.0f, -53.0f}},

    //FROM W->V
    {{-10.0f, -42.0f},{-10.5f, -42.0f},{-4.5f, -46.0f}, {-4.0f, -46.0f}},

    //FROM S->R
    {{-10.0f, -46.0f},{-10.5f, -46.0f},{-4.5f, -42.0f}, {-4.0f, -42.0f}},

    //FROM R1->S1
    {{8.5f, -40.0f}, {8.5f, -46.0f}},

    //FROM L1->Z
    {{12.0f, -43.0f}, {5.0f, -43.0f}}
    };

    vector<vector<Point>> translationPolygons = {


    };

    void decrementVelocities(vector<pair<float, float>>& velocities, float decrementValue) {
        static int decr = 0;
        decr++;
        if (decr < 10)
            return;
        decr = 0; // Reset the counter

        for (auto& velocity : velocities) {
            // Handle the x-component of velocity
            if (velocity.first > 0.0f) {
                velocity.first -= decrementValue; // Decrement positive values
            } else if (velocity.first < 0.0f) {
                velocity.first += decrementValue; // Increment negative values
            }

            // Handle the y-component of velocity
            if (velocity.second > 0.0f) {
                velocity.second -= decrementValue; // Decrement positive values
            } else if (velocity.second < 0.0f) {
                velocity.second += decrementValue; // Increment negative values
            }
        }
    }



    void generateCircleVertices(const vector<pair<float, float>>& circleCenters) {
        GLfloat twicePi = 2.0f * M_PI;

        for (const auto& center : circleCenters) {
            vector<Point> circleVertices;
            GLfloat centerX = center.first;
            GLfloat centerY = center.second;

            // Add center of the circle
            circleVertices.push_back({centerX, centerY});

            // Calculate circle vertices
            for (int i = 0; i <= triangleAmount; i++) {
                GLfloat x = centerX + (radius * cos(i * twicePi / triangleAmount));
                GLfloat y = centerY + (radius * sin(i * twicePi / triangleAmount));
                circleVertices.push_back({x, y});
            }

            // Add circle vertices to the collection of circles
            circles.push_back(circleVertices);
        }
    }

    void updatePositions(vector<pair<float, float>>& circleCenters, vector<pair<float, float>>& velocities) {
        if(startTranslation1){
            int iter=0;
            for (int i = 0; i < circleCenters.size(); ++i) {
                iter++;
            if(iter<=9){
                // Update each circle's center position based on its velocity
                circleCenters[i].first += velocities[i].first;  // Update x-coordinate
                circleCenters[i].second += velocities[i].second;  // Update y-coordinate
            }
            }
        }


        if(startTranslation2){
                int iter2=0;
            for (int i = 0; i < circleCenters.size(); ++i) {
                iter2++;
            if(iter2>9){
                // Update each circle's center position based on its velocity
                circleCenters[i].first += velocities[i].first;  // Update x-coordinate
                circleCenters[i].second += velocities[i].second;  // Update y-coordinate
            }
            }
        }

        if(startTranslation3){
                int iter3=0;
            for (int i = 0; i < circleCenters.size(); ++i) {
                iter3++;
            if(iter3>13){
                // Update each circle's center position based on its velocity
                circleCenters[i].first += velocities[i].first;  // Update x-coordinate
                circleCenters[i].second += velocities[i].second;  // Update y-coordinate
            }
            }
        }
    }

    // Function to handle collisions between two circles
    void handleBallCollision(int i, int j) {
        // Calculate the collision angle (angle of the line connecting the centers)
        float dx = circleCenter[j].first - circleCenter[i].first; // Difference in x-coordinates
        float dy = circleCenter[j].second - circleCenter[i].second; // Difference in y-coordinates
        float distance = sqrt(dx * dx + dy * dy);

        if (distance <= 2 * radius) {
            // Normalize the collision vector
            float nx = dx / distance;
            float ny = dy / distance;

            // Relative velocity
            float dvx = velocities[j].first - velocities[i].first;
            float dvy = velocities[j].second - velocities[i].second;

            // Calculate relative velocity along the collision normal
            float dotProduct = dvx * nx + dvy * ny;

            // If circles are moving apart, no need to resolve collision
            if (dotProduct > 0) return;

            // Impulse scalar
            float impulse = (2 * dotProduct) / (1.0f + 1.0f); // Assuming equal mass

            // Update velocities
            velocities[i].first += impulse * nx;
            velocities[i].second += impulse * ny;
            velocities[j].first -= impulse * nx;
            velocities[j].second -= impulse * ny;
        }
    }


     // Function to check and handle all circle circle collisions
    void checkCollisions() {
        for (size_t i = 0; i < circleCenter.size(); ++i) {
            for (size_t j = i + 1; j < circleCenter.size(); ++j) {
                handleBallCollision(i, j);
            }
        }
    }

    vector<Point> applyRotation(const vector<Point>& polygon, float angle) {
        vector<Point> transformedPolygon;
        float radians = angle * M_PI / 180.0f;

        // Calculate the center of the rectangle
        float minX = polygon[0].x, maxX = polygon[0].x;
        float minY = polygon[0].y, maxY = polygon[0].y;
        for (const auto& vertex : polygon) {
            minX = min(minX, vertex.x);
            maxX = max(maxX, vertex.x);
            minY = min(minY, vertex.y);
            maxY = max(maxY, vertex.y);
        }
        float centerX = (minX + maxX) / 2.0f;
        float centerY = (minY + maxY) / 2.0f;

        // Apply rotation around the center
        for (const auto& vertex : polygon) {
            float x = centerX + (vertex.x - centerX) * cos(radians) - (vertex.y - centerY) * sin(radians);
            float y = centerY + (vertex.x - centerX) * sin(radians) + (vertex.y - centerY) * cos(radians);
            transformedPolygon.push_back({x, y});
        }
        return transformedPolygon;
    }

    vector<Point> applyTranslation(const vector<Point>& polygon, float offsetX, float offsetY) {
        vector<Point> transformedPolygon;
        for (const auto& vertex : polygon) {
            transformedPolygon.push_back({vertex.x + offsetX, vertex.y + offsetY});
        }
        return transformedPolygon;
    }


    void applyMovement(){
        // Clear the transformed polygons
        transformedRotationPolygons.clear();
        transformedTranslationPolygons.clear();
        translationOffsets = {{xOffset1, 0.0f}, {xOffset2, 0.0f}, {0.0f, 0.0f}};

        if(startTranslation3){
            for (size_t i = 0; i < 2; i++) {
            transformedTranslationPolygons.push_back(
            applyTranslation(translationPolygons[i], translationOffsets[i].first, translationOffsets[i].second)
            );
            }
        }
        if(startTranslation4){
            for (size_t i = 2; i < 3; i++) {
            transformedTranslationPolygons.push_back(
            applyTranslation(translationPolygons[i], translationOffsets[i].first, translationOffsets[i].second)
            );
            }
        }
        if(startRotation1){
                for(const auto& polygon : rotationPolygons) {
                        transformedRotationPolygons.push_back(
                        applyRotation(polygon, rotationAngle)
                        );
                }
        }
    }

    vector<vector<float>> planeColors = {
        // Color for the first polygon
        {0.698f, 0.745f, 0.710f},

        // Color for the second polygon
        {0.827f, 0.827f, 0.827f},

        // Color for the third polygon
        {0.698f, 0.745f, 0.710f},

        // Color for the fourth polygon
        {0.827f, 0.827f, 0.827f},

        // Color for the fifth polygon
        {0.827f, 0.827f, 0.827f},

        // Color for the sixth polygon
        {0.827f, 0.827f, 0.827f},

        // Color for the first line
        {0.0f, 0.0f, 0.0f},

        // Color for the second line
        {0.0f, 0.0f, 0.0f}
    };

    vector<vector<Point>> plane = {
        // First polygon
        {
            {0.0f, -58.0f}, {0.3f, -59.0f}, {-0.3f, -59.0f}
        },
        // Second polygon
        {
            {0.3f, -59.0f}, {-0.3f, -59.0f}, {-0.8f, -59.4f},
            {-0.8f, -59.6f}, {0.8f, -59.6f}, {0.8f, -59.4f}
        },
        // Third polygon
        {
            {-0.2f, -59.6f}, {0.2f, -59.6f}, {0.2f, -59.8f}, {-0.2f, -59.8f}
        },
        // Fourth polygon
        {
            {-0.2f, -59.8f}, {-0.4f, -60.0f}, {-0.2f, -60.0f}, {-0.15f, -59.95f}
        },
        // Fifth polygon
        {
            {-0.2f, -59.8f}, {-0.15f, -59.95f}, {-0.1f, -60.0f},
            {0.1f, -60.0f}, {0.15f, -59.95f}, {0.2f, -59.8f}
        },
        // Sixth polygon
        {
            {0.2f, -59.8f}, {0.15f, -59.95f}, {0.2f, -60.0f}, {0.4f, -60.0f}
        },
        // First line
        {
            {-0.8f, -59.6f}, {-0.8f, -59.2f}
        },
        // Second line
        {
            {0.8f, -59.6f}, {0.8f, -59.2f}
        }
    };

    vector<vector<Point>> obstacles = {
        //FROM I2->K8->J8->P8
     {{-15.0f, 60.0f}, {-9.0f, 60.0f}, {-9.0f, 56.0f}, {-15.0f, 56.0f}},

    //FROM M8->J2->P3->L8
     {{0.0f, 60.0f}, {15.0f, 60.0f}, {15.0f, 58.0f}, {0.0f, 58.0f}},

    //FROM P8->R4->E8->H8
     {{-15.0f, 56.0f}, {-15.0f, 51.0f}, {1.0f, 51.0f}, {-6.5f, 56.0f}},

    //FROM R4, O8, D8, E8
     {{-15.0f, 51.0f}, {-15.0f, 42.0f}, {-11.0f, 42.0f}, {1.0f, 51.0f}} ,

    //FROM I8->P3->S4->G8
     {{0.0f, 58.0f}, {15.0f, 58.0f}, {15.0f, 52.0f}, {13.0f, 52.0f}} ,

    //FROM N8->F8->G8->S4
     {{15.0f, 42.0f}, {-2.0f, 42.0f}, {13.0f, 52.0f}, {15.0f, 52.0f}} ,

    //FROM O8->D8->A7->K3
     {{-15.0f, 42.0f}, {-11.0f, 42.0f}, {-4.5f, 39.0f}, {-15.0f, 39.0f}},

    //FROM F8->N8->N4->L7
     {{-2.0f, 42.0f}, {15.0f, 42.0f}, {15.0f, 39.0f}, {0.5f, 39.0f}} ,

    //FROM Q4->K7->L7->N4
     {{15.0f, 38.0f}, {3.0f, 38.0f}, {0.5f, 39.0f}, {15.0f, 39.0f}} ,

    //FROM K3->Z6->W6->L3
     {{-15.0f, 39.0f}, {-10.0f, 39.0f}, {-10.0f, 34.0f}, {-15.0f, 34.0f}} ,

    //FROM H7->Q4->R8->G7
     {{7.0f, 38.0f}, {15.0f, 38.0f}, {15.0f, 28.5f}, {7.0f, 28.5f}} ,
    //FROM L3->W6->V6->Z3
     {{-15.0f, 34.0f}, {-10.0f, 34.0f}, {-6.0f, 30.0f}, {-15.0f, 30.0f}},

    //FROM Z3->U6->T6->A4
     {{-15.0f, 30.0f}, {-10.0f, 30.0f}, {-10.0f, 27.0f}, {-15.0f, 27.0f}},

    //FROM A4->S6->R6->C4
     {{-15.0f, 27.0f}, {-7.5f, 27.0f}, {-7.5f, 24.0f}, {-15.0f, 24.0f}} ,

    //FROM F7->R8->S8->E7
     {{3.0f, 28.5f}, {15.0f, 28.5f}, {15.0f, 26.0f}, {3.0f, 26.0f}} ,

    //FROM D7->S8->T8->C7
     {{12.0f, 26.0f}, {15.0f, 26.0f}, {15.0f, 21.0f}, {12.0f, 21.0f}},

    //FROM C4->Q6->P6->F9
     {{-15.0f, 24.0f}, {-13.0f, 24.0f}, {-13.0f, 21.0f}, {-15.0f, 21.0f}},

    //FROM F9->D9->O6->J3
     {{-15.0f, 21.0f}, {-7.0f, 21.0f}, {-7.0f, 20.0f}, {-15.0f, 20.0f}} ,

    //FROM B7-> T8->I3->E9
     {{2.0f, 21.0f}, {15.0f, 21.0f}, {15.0f, 20.0f}, {2.0f, 20.0f}} ,

    //FROM J6->I3->A2->I6
     {{12.0f, 20.0f}, {15.0f, 20.0f}, {15.0f, 13.0f}, {12.0f, 13.0f}},

    //FROM H6->A2->R2->G6
     {{7.0f, 13.0f}, {15.0f, 13.0f}, {15.0f, 6.0f}, {7.0f, 6.0f}} ,

    //FROM F6->R2->T2->E6
     {{12.0f, 6.0f}, {15.0f, 6.0f}, {15.0f, 2.0f}, {12.0f, 2.0f}} ,

    //FROM Q8->G4->D6->T2
     {{15.0f, -2.0f}, {4.0f, -2.0f}, {4.0f, 2.0f}, {15.0f, 2.0f}} ,

    //FROM I9->E4->G4->Q8
     {{15.0f, -4.0f}, {5.0f, -4.0f}, {4.0f, -2.0f}, {15.0f, -2.0f}},

    //FROM P4->W3->E4->I9
     {{15.0f, -5.0f}, {7.0f, -5.0f}, {5.0f, -4.0f}, {15.0f, -4.0f}} ,

    //FROM P4->W3->V3->V1
     {{15.0f, -5.0f}, {7.0f, -5.0f}, {7.0f, -10.0f}, {15.0f, -10.0f}},

    //FROM V1->V3->U3->H9
     {{15.0f, -10.0f}, {7.0f, -10.0f}, {10.0f, -14.0f}, {15.0f, -14.0f}},

    //FROM H9->U3->Q3->G9
     {{15.0f, -14.0f}, {10.0f, -14.0f}, {9.0f, -18.0f}, {15.0f, -18.0f}} ,

    //FROM G9->Q3->K4->P
     {{15.0f, -18.0f}, {9.0f, -18.0f}, {4.0f, -19.0f}, {15.0f, -19.0f}} ,

    //FROM P->U1->L2->O
     {{15.0f, -19.0f}, {12.0f, -19.0f}, {12.0f, -33.0f}, {15.0f, -33.0f}},

    //FROM O->H2->G2->B2
     {{15.0f, -33.0f}, {10.0f, -33.0f}, {10.0f, -36.0f}, {15.0f, -36.0f}} ,

    //FROM B2->F2->C2->W1
     {{15.0f, -36.0f}, {4.0f, -36.0f}, {4.0f, -38.0f}, {15.0f, -38.0f}} ,

    //FROM W1->U8->B9->O1
     {{15.0f, -38.0f}, {13.0f, -38.0f}, {13.0f, -60.0f}, {15.0f, -60.0f}},

    //FROM W8->V8->T->U
     {{13.0f, -49.0f}, {4.0f, -49.0f}, {4.0f, -50.0f}, {13.0f, -50.0f}} ,

    //FROM T5->S5->Q1->B9
     {{13.0f, -59.0f}, {5.0f, -59.0f}, {5.0f, -60.0f}, {13.0f, -60.0f}},


    //FROM J3-> N6->M6->Z1
     {{-15.0f, 20.0f}, {-12.0f, 20.0f}, {-12.0f, 13.0f}, {-15.0f, 13.0f}},

    //FROM Z1->L6->K6->U2
     {{-15.0f, 13.0f}, {-7.0f, 13.0f}, {-7.0f, 5.5f}, {-15.0f, 5.5f}} ,

    //FROM U2->C6->B6->Z2
     {{-15.0f, 5.5f}, {-12.0f, 5.5f}, {-12.0f, 2.0f}, {-15.0f, 2.0f}} ,

    //FROM Z2->A6->D4->L
     {{-15.0f, 2.0f}, {-3.0f, 2.0f}, {-3.0f, -2.0f}, {-15.0f, -2.0f}} ,

    //FROM D4->L->J9->B4
     {{-3.0f, -2.0f}, {-15.0f, -2.0f}, {-15.0f, -5.0f}, {-4.0f, -5.0f}},

    //FROM B4->J9->F3->T3
     {{-4.0f, -5.0f},  {-15.0f, -5.0f}, {-15.0f, -7.0f}, {-9.0f, -7.0f}},

    //FROM T3->F3->H3->S3
     {{-9.0f, -7.0f}, {-15.0f, -7.0f}, {-15.0f, -11.5f}, {-6.5f, -11.5f}},

    //FROM S3->H3->O4->R3
     {{-6.5f, -11.5f}, {-15.0f, -11.5f}, {-15.0f, -15.0f}, {-7.0f, -15.0f}},

    //FROM M->O4->C3->W5
     {{-10.0f, -15.0f}, {-15.0f, -15.0f}, {-15.0f, -25.0f}, {-10.0f, -25.0f}},

    //FROM W5->C3->M2->S2
     {{-10.0f, -25.0f}, {-15.0f, -25.0f}, {-15.0f, -28.5f}, {-12.0f, -28.5f}} ,

    //FROM S2->M2->E2->Q2
     {{-12.0f, -28.5f}, {-15.0f, -28.5f}, {-15.0f, -33.5f}, {-12.0f, -33.5f}} ,

    //FROM P2->E2->D2->O2
     {{-9.0f, -33.5f}, {-15.0f, -33.5f}, {-15.0f, -36.0f}, {-9.0f, -36.0f}} ,

    //FROM N2->D2->G->T1
     {{-4.5f, -36.0f}, {-15.0f, -36.0f}, {-15.0f, -38.0f}, {-4.5f, -38.0f}} ,

    //FROM V5->G->N1->A9
     {{-12.0f, -38.0f}, {-15.0f, -38.0f}, {-15.0f, -60.0f}, {-12.0f, -60.0f}},

    //FROM Z8->U5->Q->N
     {{-12.0f, -49.0f}, {-2.0f, -49.0f}, {-2.0f, -50.0f}, {-12.0f, -50.0f}} ,

    //FROM H4->D3->P1->A9
     {{-12.0f, -59.0f}, {-5.0f, -59.0f}, {-5.0f, -60.0f}, {-12.0f, -60.0f}} ,

    //FROM M7->Q7->R7->P7->O7->N7
     {{-2.0f, 28.0f}, {0.0f, 27.0f}, {0.0f, 25.0f}, {-1.2f, 23.6f}, {-3.5f, 24.5f}, {-4.0f, 27.0f}},

    //FROM L4->F5->E5->D5
     {{-4.0f, -8.0f}, {-2.0f, -8.0f}, {-2.0f, -10.0f}, {-4.0f, -10.0f}} ,

    //FROM K5->N5->M5->L5
     {{3.0f, -9.0f}, {6.0f, -9.0f}, {6.0f, -11.0f}, {3.0f, -11.0f}} ,

    //FROM O5->R5->Q5->P5
     {{-5.2f, -14.2f}, {-2.8f, -14.0f}, {-2.0f, -16.5f}, {-5.0f, -17.5f}}



    };

    // Function to draw a chequered bar
    void drawChequeredBar(float x1, float y1, float x2, float y2, float ySubdivision) {
        float width = x2 - x1; // Total width of the bar
        int numDivisions = (int)(width); // Number of subdivisions along x-axis

        // Top rectangle: From y1 to ySubdivision
        for (int i = 0; i < numDivisions; i++) {
            float xStart = x1 + i;
            float xEnd = x1 + i + 1;

            if (i % 2 == 0) {
                glColor3f(0.0, 0.0, 0.0); // Black
            } else {
                glColor3f(1.0, 1.0, 1.0); // White
            }

            glBegin(GL_POLYGON);
            glVertex2f(xStart, y1);
            glVertex2f(xEnd, y1);
            glVertex2f(xEnd, ySubdivision);
            glVertex2f(xStart, ySubdivision);
            glEnd();
        }

        // Bottom rectangle: From ySubdivision to y2
        for (int i = 0; i < numDivisions; i++) {
            float xStart = x1 + i;
            float xEnd = x1 + i + 1;

            if (i % 2 != 0) { // Alternate pattern
                glColor3f(0.0, 0.0, 0.0); // Black
            } else {
                glColor3f(1.0, 1.0, 1.0); // White
            }

            glBegin(GL_POLYGON);
            glVertex2f(xStart, ySubdivision);
            glVertex2f(xEnd, ySubdivision);
            glVertex2f(xEnd, y2);
            glVertex2f(xStart, y2);
            glEnd();
        }

        // Draw the top x-axis line
        glColor3f(0.0, 0.0, 0.0); // Black color for lines
        glBegin(GL_LINES);
        glVertex2f(x1, y1); // Start of the top edge
        glVertex2f(x2, y1); // End of the top edge
        glEnd();

        // Draw the bottom x-axis line
        glBegin(GL_LINES);
        glVertex2f(x1, y2); // Start of the bottom edge
        glVertex2f(x2, y2); // End of the bottom edge
        glEnd();
    }

    void drawPlane(const vector<vector<Point>>& plane, const vector<vector<float>>& colors) {
        glPushMatrix();
        glTranslatef(planeOffsetX, planeOffsetY, 0.0f);
        glRotatef(planeRotation, 0.0f, 1.0f, 0.0f); // Apply rotation

        for (size_t i = 0; i < plane.size(); ++i) {
            const auto& vertices = plane[i];
            const auto& color = colors[i];

            if (vertices.size() > 2) {
                // Draw polygons
                glBegin(GL_POLYGON);
                glColor3f(color[0], color[1], color[2]);
                for (const auto& vertex : vertices) {
                    glVertex2f(vertex.x, vertex.y);
                }
                glEnd();
            } else if (vertices.size() == 2) {
                // Draw lines
                glLineWidth(1.5f);
                glBegin(GL_LINES);
                glColor3f(color[0], color[1], color[2]);
                for (const auto& vertex : vertices) {
                    glVertex2f(vertex.x, vertex.y);
                }
                glEnd();
            }
        }

        glPopMatrix();
    }

    vector<vector<Point>> getTransformedPlane() {
        vector<vector<Point>> transformedPlane;
        for (const auto& polygon : plane) {
            vector<Point> transformedPolygon;
            for (const auto& vertex : polygon) {
                transformedPolygon.push_back({vertex.x + planeOffsetX, vertex.y + planeOffsetY});
            }
            transformedPlane.push_back(transformedPolygon);
        }
        return transformedPlane;
    }

    // Collision detection Start

    // Function to compute the dot product of two vectors
    float dotProduct(const Point& a, const Point& b) {
        return a.x * b.x + a.y * b.y;
    }

    // Function to calculate axes perpendicular to edges of a polygon
    vector<Point> getAxes(const vector<Point>& polygon) {
        vector<Point> axes;
        for (size_t i = 0; i < polygon.size(); ++i) {
            Point edge = {
                polygon[(i + 1) % polygon.size()].x - polygon[i].x,
                polygon[(i + 1) % polygon.size()].y - polygon[i].y
            };
            axes.push_back({-edge.y, edge.x}); // Perpendicular axis
        }
        return axes;
    }

    // Function to project a polygon onto an axis
    void projectOntoAxis(const vector<Point>& polygon, const Point& axis, float& minVal, float& maxVal) {
        minVal = maxVal = dotProduct(polygon[0], axis);
        for (const auto& vertex : polygon) {
            float projection = dotProduct(vertex, axis);
            if (projection < minVal) minVal = projection;
            if (projection > maxVal) maxVal = projection;
        }
    }

    // SAT-based collision detection between two polygons
    bool polygonsCollide(const vector<Point>& poly1, const vector<Point>& poly2) {
        vector<Point> axes1 = getAxes(poly1);
        vector<Point> axes2 = getAxes(poly2);

        for (const auto& axis : axes1) {
            float min1, max1, min2, max2;
            projectOntoAxis(poly1, axis, min1, max1);
            projectOntoAxis(poly2, axis, min2, max2);

            if (max1 < min2 || max2 < min1) {
                return false; // No overlap, polygons don't collide
            }
        }

        for (const auto& axis : axes2) {
            float min1, max1, min2, max2;
            projectOntoAxis(poly1, axis, min1, max1);
            projectOntoAxis(poly2, axis, min2, max2);

            if (max1 < min2 || max2 < min1) {
                return false; // No overlap, polygons don't collide
            }
        }

        return true; // All axes overlap, polygons collide
    }


    // Check if any polygon in the plane collides with any obstacle
    bool noCollision(const vector<vector<Point>>& plane, const vector<vector<Point>>& obstacles) {
        vector<vector<Point>> transformedPlane = getTransformedPlane(); // Transform the plane

        for (const auto& obstacle : obstacles) {
            for (const auto& transformedPolygon : transformedPlane) {
                if (polygonsCollide(transformedPolygon, obstacle)) {
                    return false; // Collision detected
                }
            }
        }

        return true; // No collisions
    }

    //Collision detection End


    void drawPolygons(const vector<vector<Point>>& polygons) {
        for (const auto& polygon : polygons) {
            glBegin(GL_POLYGON);
            glColor3f(0.514f, 0.871f, 0.925f); // Set color (light blue in this case)
            for (const auto& point : polygon) {
                glVertex2f(point.x, point.y); // Add vertex
            }
            glEnd();
        }
    }


    void storeCounter(int counter) {
        ofstream outFile("lib/counter.txt");  // Open the file in write mode
        if (outFile.is_open()) {
            outFile << counter;  // Write the counter value to the file
            outFile.close();  // Close the file after writing
        }
    }

    int retrieveCounter() {
        int counter = 0;
        ifstream inFile("lib/counter.txt");  // Open the file in read mode
        if (inFile.is_open()) {
            inFile >> counter;  // Read the counter value from the file
            inFile.close();  // Close the file after reading
        }
        return counter;
    }


    void renderText(float x, float y, const string& text, void* font = GLUT_BITMAP_HELVETICA_18) {
        glColor3f(0.0f, 0.0f, 0.0f); // Set text color to black
        glRasterPos2f(-14.8f, yMax - 1.0f);         // Position of the text in the window
        for (char c : text) {
            glutBitmapCharacter(font, c); // Render each character
        }
    }

    /* Display callback function */
    void display() {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Set background color to gray and opaque
        glClear(GL_COLOR_BUFFER_BIT);        // Clear the color buffer (background)

        // Reset projection matrix for dynamic scrolling
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-15.0f, 15.0f, yMin, yMax, -1.0f, 1.0f); // Set visible region dynamically
        glMatrixMode(GL_MODELVIEW);

        drawChequeredBar(-15.0f, 59.9f, 15.0f, 58.0f, 59.0f);
        drawPlane(plane, planeColors);

        // Update the counter at 2x per second
        counter = (int)(elapsedTime * 2);

        // Design 4: Start

        glLineWidth(15.0f);
        glBegin(GL_LINES);
        glColor3f(0.514f, 0.871f, 0.925f);

        // Line 6: R1->S1
        glVertex2f(8.5f, -40.0f);
        glVertex2f(8.5f, -46.0f);

        // Line 6: L1->Z
        glVertex2f(12.0f, -43.0f);
        glVertex2f(5.0f, -43.0f);

        glEnd();

        // Draw Polygons
        drawPolygons(obstacles);
        if(startRotation1)
            drawPolygons(transformedRotationPolygons);
        else
            drawPolygons(rotationPolygons);

            drawPolygons(transformedTranslationPolygons);

        // Update positions
        updatePositions(circleCenter, velocities);


        decrementVelocities(velocities, 0.00005f);

        // Clear previous vertices
        circles.clear();

        // Generate vertices for updated positions
        generateCircleVertices(circleCenter);

        // Check and resolve collisions
        checkCollisions();

        // Render each circle using stored vertices
        glColor3f(0.514f, 0.871f, 0.925f);
        for (const auto& circle : circles) {
            glBegin(GL_TRIANGLE_FAN);
            for (const auto& vertex : circle) {
                glVertex2f(vertex.x, vertex.y);
            }
            glEnd();
        }
        // Render each circle using stored vertices
        glColor3f(0.514f, 0.871f, 0.925f);
        for (const auto& circle : circles) {
            glBegin(GL_TRIANGLE_FAN);
            for (const auto& vertex : circle) {
                glVertex2f(vertex.x, vertex.y);
            }
            glEnd();
        }
        // Render the counter text in the top-left corner
        renderText(-16.0f, 15.0f, to_string(counter));

        glFlush(); // Render now
    /*
       if (((clock() - startTime) / CLOCKS_PER_SEC) >= 7) {
            startRotation1 = true; // Start translation after 13 seconds
        }
        */

        if (((clock() - startTime) / CLOCKS_PER_SEC) >= 7) {
            startTranslation1 = true; // Start translation after 13 seconds
        }
        if (((clock() - startTime) / CLOCKS_PER_SEC) >=25) {
            startTranslation2 = true; // Start translation after 13 seconds
        }
        if (((clock() - startTime) / CLOCKS_PER_SEC) >= 27) {
            startTranslation3 = false; // Start translation after 13 seconds
        }
    }

    void updatePlane() {
        if (isKeyPressedA) {
            velocityX = -moveSpeed;
            targetRotation = -45.0f;
        } else if (isKeyPressedD) {
            velocityX = moveSpeed;
            targetRotation = 45.0f;
        } else {
            velocityX = 0.0f; // Stop movement if no key is pressed
            targetRotation = 0.0f; // Reset rotation
        }

        // Update position
        planeOffsetX += velocityX;

        // Smoothly interpolate rotation towards the target
        if (planeRotation < targetRotation) {
            planeRotation += rotationSpeed;
            if (planeRotation > targetRotation) planeRotation = targetRotation; // Clamp to target
        } else if (planeRotation > targetRotation) {
            planeRotation -= rotationSpeed;
            if (planeRotation < targetRotation) planeRotation = targetRotation; // Clamp to target
        }

        glutPostRedisplay(); // Redraw the scene
    }

    /* Timer function for scrolling */
    void timer(int value) {
        if(!timer5) return;
        // Update the y-axis bounds
        if (noCollision(plane, obstacles) && noCollision(plane,transformedRotationPolygons) && noCollision(plane,transformedTranslationPolygons) && noCollision(plane,circles)){
            if (yMax <= 60.0f){
                yMin += step;
                yMax += step;
            }
            applyMovement();

            // Calculate elapsed time in seconds for Score
            elapsedTime = (float)(clock() - startTime) / CLOCKS_PER_SEC;
            planeOffsetY += step; // Move the plane up at the same rate as the scrolling
        }
        else if(!gameOverTriggered){
            glutDisplayFunc(gameOver::display);
            gameOverTriggered=true;
        }

        // Update the rotation angle for obstacle animation
        rotationAngle += 1.0f; // Increment the angle (degrees)
        if (rotationAngle >= 360.0f) {
            rotationAngle -= 360.0f; // Keep the angle within 0�360 degrees
        }
        if(planeOffsetY>=120.0f){
            glutDisplayFunc(levelComplete::display);
        }

        // Call this timer function again after the specified interval
        glutTimerFunc(scrollTime, timer, 0);

    }

    void keyboard(unsigned char key, int x, int y) {
        if (key == 'a' || key == 'A') {
            isKeyPressedA = true;
        } else if (key == 'd' || key == 'D') {
            isKeyPressedD = true;
        }
    }

    void keyboardUp(unsigned char key, int x, int y) {
        if (key == 'a' || key == 'A') {
            isKeyPressedA = false;
        } else if (key == 'd' || key == 'D') {
            isKeyPressedD = false;
        }
    }


    /* Reshape callback function */
    void reshape(int w, int h) {
        if (h == 0) h = 1; // Prevent divide by zero

        // Set the viewport to cover the new window
        glViewport(0, 0, w, h);
    }
    void idle() {
        updatePlane(); // Continuously update plane position and rotation
    }

    void reset(){
    counter = 0;    // Integer to display
    elapsedTime= 0.0f;
    // Rotation variable
    rotationAngle = 0.0f;
    // Initial y-axis bounds for the visible region
    yMin = -60.0f, yMax = -40.0f;
    step = 0.05;
    scrollTime = 16;

    // Plane movement variables
    planeOffsetX = 0.0f; // Offset for the plane to sync with scrolling
    planeOffsetY = 0.0f; // Offset for polygon movement in the x-axis
    planeRotation = 0.0f; // Track the rotation of the plane
    isKeyPressedA = false;
    isKeyPressedD = false;
    velocityX = 0.0f;
    targetRotation = 0.0f;

    // Variables to handle translation
    xOffset1 = 0.0f; // Offset for translation
    xOffset2 = 0.0f; // Offset for translation
    yOffset1;

    startTranslation1 = false; // Flag to start translation
    startTranslation2 = false;
    startTranslation3 = false;
    startTranslation4 = false;
    startRotation1 = true;
    speed = 0.005f; // Speed of translation

    translationOffsets.clear();// Corresponding offsets for each polygon

    transformedRotationPolygons.clear();
    transformedTranslationPolygons.clear();

    //store generated circle vertices
    circles.clear();
    // Velocities of the circles
    velocities = {
        {0.05f, 0.05f}, {0.08f, 0.00f}, {0.00f, 0.00f}, {0.00f, 0.00f}, {0.00f, 0.00f}, {0.00f, 0.00f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f},
        {0.09f, 0.0f}, {0.00f, 0.00f}, {0.0f, 0.00f}, {0.02f, 0.05f}, {0.00f, 0.00f}, {0.00f, 0.00f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}
    };
    }
}

namespace infoShow{
    // Star variables
    struct Star {
        float x, y;
    };
    vector<Star> stars;
    const int numStars = 80;
    float starSpeed = 0.01f;

    // Info variables
    vector<string> info = {
            "                           The PathWay",
            "",
            "                               Group 7",
            "                       Group Members:                        Contribution",
            "",
            "         Al-Amin Hossain Nahid [22-49627-3]             35%",
            "           Ferdus Hossain [22-49619-3]                        35%",
            "          Rakibul Islam Akash[23-50363-1]                   30%",

            "",
            "                         Supervised by:",
            "                   MAHFUJUR RAHMAN"
        };

    size_t lineIndex = 0;
    string currentText = "";
    float lastUpdateTime = 0;


    float randomXPosition() {
        return (float)(rand() % 30 - 15);
    }

    // Initialize star positions
    void initStars() {
        for (int i = 0; i < numStars; ++i) {
            stars.push_back({randomXPosition(), float(rand() % 40 - 20)});
        }
    }

    // Draw stars moving downward
    void drawStars() {
        for (auto& star : stars) {
            star.y -= starSpeed;
            if (star.y < -20.0f) {
                star.y = 20.0f;
                star.x = randomXPosition();
            }
            glPointSize(2.0f);
            glBegin(GL_POINTS);
            glColor3f(1.0f, 1.0f, 1.0f);  // White stars
            glVertex2f(star.x, star.y);
            glEnd();
        }
    }


    // Display info with typewriter effect
    void infotxt() {
    float startY = 8.0f;  // Start Y position
        for (const auto& line : info) {
            glRasterPos2f(-5.0f, startY);
            for (char c : line) {
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
            }
            startY -= 1.5f;  // Move down for the next line
        }
    }

    // Display function
    void display() {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Black background
        glClear(GL_COLOR_BUFFER_BIT);        // Clear the color buffer (background)

        // from I1->J1->K1->L1
        glBegin(GL_POLYGON);
        glColor3f(0.5f, 0.5f, 0.5f);
        glVertex2f(-1.5f, -8.0f);
        glVertex2f(1.5f, -8.0f);
        glVertex2f(1.5f, -9.0f);
        glVertex2f(-1.5f, -9.0f);
        glEnd();

        // Render text inside the polygon
        glColor3f(1.0f, 1.0f, 1.0f);
        renderText(-0.4f, -8.5f, "Back", GLUT_BITMAP_9_BY_15);

        drawStars();
        infotxt();

        glutSwapBuffers();
        glutPostRedisplay();
    }
}

namespace menuBar{
    vector<vector<float>> planeColors = {
        // Color for the first polygon
        {0.698f, 0.745f, 0.710f},

        // Color for the second polygon
        {0.827f, 0.827f, 0.827f},

        // Color for the third polygon
        {0.698f, 0.745f, 0.710f},

        // Color for the fourth polygon
        {0.827f, 0.827f, 0.827f},

        // Color for the fifth polygon
        {0.827f, 0.827f, 0.827f},

        // Color for the sixth polygon
        {0.827f, 0.827f, 0.827f},

        // Color for the first line
        {0.0f, 0.0f, 0.0f},

        // Color for the second line
        {0.0f, 0.0f, 0.0f}
    };

    vector<vector<Point>> menuPlaneOne = {
        // First polygon
        {
            {-6.0f, 7.0f}, {-5.7f, 6.0f}, {-6.3f, 6.0f}
        },
        // Second polygon
        {
            {-5.7f, 6.0f}, {-6.3f, 6.0f}, {-6.8f, 5.6f},
            {-6.8f, 5.4f}, {-5.2f, 5.4f}, {-5.2f, 5.6f}
        },
        // Third polygon
        {
            {-6.2f, 5.4f}, {-5.8f, 5.4f}, {-5.8f, 5.2f}, {-6.2f, 5.2f}
        },
        // Fourth polygon
        {
            {-5.8f, 5.2f}, {-5.85f, 5.05f}, {-5.8f, 5.0f}, {-5.6f, 5.0f}
        },
        // Fifth polygon
        {
            {-6.2f, 5.2f}, {-6.15f, 5.05f}, {-6.1f, 5.0f},
            {-5.9f, 5.0f}, {-5.85f, 5.05f}, {-5.8f, 5.2f}
        },
        // Sixth polygon
        {
            {-6.2f, 5.2f}, {-6.4f, 5.0f}, {-6.2f, 5.0f}, {-6.15f, 5.05f}
        },
        // First line
        {
            {-6.8f, 5.6f}, {-6.8f, 5.8f}
        },
        // Second line
        {
            {-5.2f, 5.6f}, {-5.2f, 5.8f}
        }
    };

    vector<vector<Point>> menuPlaneTwo = {
        // First polygon
        {
            {0.0f, 7.0f}, {0.3f, 6.0f}, {-0.3f, 6.0f}
        },
        // Second polygon
        {
            {0.3f, 6.0f}, {-0.3f, 6.0f}, {-0.8f, 5.6f},
            {-0.8f, 5.4f}, {0.8f, 5.4f}, {0.8f, 5.6f}
        },
        // Third polygon
        {
            {-0.2f, 5.4f}, {0.2f, 5.4f}, {0.2f, 5.2f}, {-0.2f, 5.2f}
        },
        // Fourth polygon
        {
            {-0.2f, 5.2f}, {-0.4f, 5.0f}, {-0.2f, 5.0f}, {-0.15f, 5.05}
        },
        // Fifth polygon
        {
            {-0.2f, 5.2f}, {-0.15f, 5.05f}, {-0.1f, 5.0f},
            {0.1f, 5.0f}, {0.15f, 5.05f}, {0.2f, 5.2f}
        },
        // Sixth polygon
        {
            {0.2f, 5.2f}, {0.4f, 5.0f}, {0.2f, 5.0f}, {0.15f, 5.05f}
        },
        // First line
        {
            {-0.8f, 5.6f}, {-0.8f, 5.8f}
        },
        // Second line
        {
            {0.8f, 5.6f}, {0.8f, 5.8f}
        }
    };

    vector<vector<Point>> menuPlaneThree = {
        // First polygon
        {
            {6.0f, 7.0f}, {6.3f, 6.0f}, {5.7f, 6.0f}
        },
        // Second polygon
        {
            {6.3f, 6.0f}, {5.7f, 6.0f}, {5.2f, 5.6f},
            {5.2f, 5.4f}, {6.8f, 5.4f}, {6.8f, 5.6f}
        },
        // Third polygon
        {
            {5.8f, 5.4f}, {6.2f, 5.4f}, {6.2f, 5.2f}, {5.8f, 5.2f}
        },
        // Fourth polygon
        {
            {5.8f, 5.2f}, {5.6f, 5.0f}, {5.8f, 5.0f}, {5.85f, 5.05}
        },
        // Fifth polygon
        {
            {5.8f, 5.2f}, {5.85f, 5.05f}, {5.9f, 5.0f},
            {6.1f, 5.0f}, {6.15f, 5.05f}, {6.2f, 5.2f}
        },
        // Sixth polygon
        {
            {6.2f, 5.2f}, {6.15f, 5.05f}, {6.2f, 5.0f}, {6.4f, 5.0f}
        },
        // First line
        {
            {5.2f, 5.6f}, {5.2f, 5.8f}
        },
        // Second line
        {
            {6.8f, 5.6f}, {6.8f, 5.8f}
        }
    };

    vector<vector<Point>> menuPlaneFour = {
        // First polygon
        {
            {-6.0f, 0.0f}, {-5.7f, -1.0f}, {-6.3f, -1.0f}
        },
        // Second polygon
        {
            {-5.7f, -1.0f}, {-6.3f, -1.0f}, {-6.8f, -1.4f},
            {-6.8f, -1.6f}, {-5.2f, -1.6f}, {-5.2f, -1.4f}
        },
        // Third polygon
        {
            {-6.2f, -1.6f}, {-5.8f, -1.6f}, {-5.8f, -1.8f}, {-6.2f, -1.8f}
        },
        // Fourth polygon
        {
            {-6.2f, -1.8f}, {-6.4f, -2.0f}, {-6.2f, -2.0f}, {-6.15f, -1.95f}
        },
        // Fifth polygon
        {
            {-6.2f, -1.8f}, {-6.15f, -1.95f}, {-6.1f, -2.0f},
            {-5.9f, -2.0f}, {-5.85f, -1.95f}, {-5.8f, -1.8f}
        },
        // Sixth polygon
        {
            {-5.8f, -1.8f}, {-5.85f, -1.95f}, {-5.8f, -2.0f}, {-5.6f, -2.0f}
        },
        // First line
        {
            {-6.8f, -1.4f}, {-6.8f, -1.2f}
        },
        // Second line
        {
            {-5.2f, -1.4f}, {-5.2f, -1.2f}
        }
    };

    vector<vector<Point>> menuPlaneFive = {
        // First polygon
        {
            {0.0f, 0.0f}, {0.3f, -1.0f}, {-0.3f, -1.0f}
        },
        // Second polygon
        {
            {0.3f, -1.0f}, {-0.3f, -1.0f}, {-0.8f, -1.4f},
            {-0.8f, -1.6f}, {0.8f, -1.6f}, {0.8f, -1.4f}
        },
        // Third polygon
        {
            {-0.2f, -1.6f}, {0.2f, -1.6f}, {0.2f, -1.8f}, {-0.2f, -1.8f}
        },
        // Fourth polygon
        {
            {-0.2f, -1.8f}, {-0.4f, -2.0f}, {-0.2f, -2.0f}, {-0.15f, -1.95}
        },
        // Fifth polygon
        {
            {-0.2f, -1.8f}, {-0.15f, -1.95f}, {-0.1f, -2.0f},
            {0.1f, -2.0f}, {0.15f, -1.95f}, {0.2f, -1.8f}
        },
        // Sixth polygon
        {
            {0.2f, -1.8f}, {0.15f, -1.95f}, {0.2f, -2.0f}, {0.4f, -2.0f}
        },
        // First line
        {
            {-0.8f, -1.4f}, {-0.8f, -1.2f}
        },
        // Second line
        {
            {0.8f, -1.4f}, {0.8f, -1.2f}
        }
    };

    vector<vector<Point>> menuPlaneSix = {
        // First polygon
        {
            {6.0f, 0.0f}, {6.3f, -1.0f}, {5.7f, -1.0f}
        },
        // Second polygon
        {
            {6.3f, -1.0f}, {5.7f, -1.0f}, {5.2f, -1.4f},
            {5.2f, -1.6f}, {6.8f, -1.6f}, {6.8f, -1.4f}
        },
        // Third polygon
        {
            {5.8f, -1.6f}, {6.2f, -1.6f}, {6.2f, -1.8f}, {5.8f, -1.8f}
        },
        // Fourth polygon
        {
            {5.8f, -1.8f}, {5.6f, -2.0f}, {5.8f, -2.0f}, {5.85f, -1.95f}
        },
        // Fifth polygon
        {
            {5.8f, -1.8f}, {5.85f, -1.95f}, {5.9f, -2.0f},
            {6.1f, -2.0f}, {6.15f, -1.95f}, {6.2f, -1.8f}
        },
        // Sixth polygon
        {
            {6.2f, -1.8f}, {6.15f, -1.95f}, {6.2f, -2.0f}, {6.4f, -2.0f}
        },
        // First line
        {
            {5.2f, -1.4f}, {5.2f, -1.2f}
        },
        // Second line
        {
            {6.8f, -1.4f}, {6.8f, -1.2f}
        }
    };

    // Function to draw the plane
    void drawPlaneForLevel(const vector<std::vector<Point>>& plane, const vector<std::vector<float>>& colors) {
        for (size_t i = 0; i < plane.size(); ++i) {
            // Set the color for the current polygon or line
            glColor3f(colors[i][0], colors[i][1], colors[i][2]);

            // Decide whether to draw polygons or lines
            if (plane[i].size() > 2) {
                // Draw polygon
                glBegin(GL_POLYGON);
            } else {
                // Draw line
                glBegin(GL_LINES);
            }

            // Specify vertices for the current polygon or line
            for (const auto& vertex : plane[i]) {
                glVertex2f(vertex.x, vertex.y);
            }

            glEnd();
        }
    }

    void menuShow(){
        // from G->H->F->E
        glBegin(GL_POLYGON);
        glColor3f(0.514f, 0.871f, 0.925f);
        glVertex2f(-10.0f, 10.0f);
        glVertex2f(10.0f, 10.0f);
        glVertex2f(10.0f, -10.0f);
        glVertex2f(-10.0f, -10.0f);
        glEnd();

        // from I->J->K->L
        glBegin(GL_POLYGON);
        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex2f(-1.5f, -6.5f);
        glVertex2f(1.5f, -6.5f);
        glVertex2f(1.5f, -7.5f);
        glVertex2f(-1.5f, -7.5f);
        glEnd();

        // from P->O->M->U
        glBegin(GL_POLYGON);
        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex2f(0.0f, 2.0f);
        glVertex2f(-2.0f, 2.0f);
        glVertex2f(-2.0f, -4.0f);
        glVertex2f(0.0f, -4.0f);
        glEnd();

        // from Q->R->T->V
        glBegin(GL_POLYGON);
        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex2f(2.0f, 7.5f);
        glVertex2f(0.0f, 7.5f);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(2.0f, 0.0f);
        glEnd();

        // for S
        GLfloat x1= 9.5f;
        GLfloat y1= 9.5f;
        GLfloat radius =0.5f;
        int triangleAmount = 100; //# of lines used to draw circle

        //GLfloat radius = 0.8f; //radius
        GLfloat twicePi = 2.0f * PI;

        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_TRIANGLE_FAN);
            glVertex2f(x1, y1); // center of circle
            for(int i = 0; i <= triangleAmount;i++) {
                glVertex2f( x1 + (radius * cos(i *  twicePi / triangleAmount)),
                            y1 + (radius * sin(i * twicePi / triangleAmount)) );
            }
        glEnd();

        // for N
        GLfloat x2= -9.5f;
        GLfloat y2= 9.5f;

        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_TRIANGLE_FAN);
            glVertex2f(x2, y2); // center of circle
            for(int i = 0; i <= triangleAmount;i++) {
                glVertex2f( x2 + (radius * cos(i *  twicePi / triangleAmount)),
                            y2 + (radius * sin(i * twicePi / triangleAmount)) );
            }
        glEnd();

        // for D1
        GLfloat x3= -9.75f;
        GLfloat y3= 9.25f;
        GLfloat rad = 0.1f;

        glColor3f(0.514f, 0.871f, 0.925f);
        glBegin(GL_TRIANGLE_FAN);
            glVertex2f(x3, y3); // center of circle
            for(int i = 0; i <= triangleAmount;i++) {
                glVertex2f( x3 + (rad * cos(i *  twicePi / triangleAmount)),
                            y3 + (rad * sin(i * twicePi / triangleAmount)) );
            }
        glEnd();

        // for D1
        GLfloat x4= -9.35f;
        GLfloat y4= 9.35f;

        glColor3f(0.514f, 0.871f, 0.925f);
        glBegin(GL_TRIANGLE_FAN);
            glVertex2f(x4, y4); // center of circle
            for(int i = 0; i <= triangleAmount;i++) {
                glVertex2f( x4 + (rad * cos(i *  twicePi / triangleAmount)),
                            y4 + (rad * sin(i * twicePi / triangleAmount)) );
            }
        glEnd();

        glLineWidth(4.0f);
        glBegin(GL_LINES);
        glColor3f(0.514f, 0.871f, 0.925f);
        glVertex2f(9.5f, 9.3f);
        glVertex2f(9.5f, 9.6f);
        glEnd();

        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glColor3f(0.514f, 0.871f, 0.925f);
        glVertex2f(-9.7f, 9.3f);
        glVertex2f(-9.7f, 9.6);
        glEnd();

        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glColor3f(0.514f, 0.871f, 0.925f);
        glVertex2f(-9.7f, 9.6);
        glVertex2f(-9.3f, 9.7f);
        glEnd();

        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glColor3f(0.514f, 0.871f, 0.925f);
        glVertex2f(-9.3f, 9.7f);
        glVertex2f(-9.3f, 9.4f);
        glEnd();

        glPointSize(4.0f);
        glBegin(GL_POINTS);
        glColor3f(0.514f, 0.871f, 0.925f);
        glVertex2f(9.5f, 9.7f);
        glEnd();

        // Render text inside the polygon
        glColor3f(0.0f, 0.0f, 0.0f);
        renderText(-1.5f, 8.5f, "The Path Way", GLUT_BITMAP_TIMES_ROMAN_24);

        // Render text inside the polygon A1
        glColor3f(0.0f, 0.0f, 0.0f);
        renderText(-0.5f, -7.0f, "Start", GLUT_BITMAP_9_BY_15);

        drawPlaneForLevel(menuPlaneFour, planeColors);

        // Render text inside the polygon A1
        glColor3f(0.0f, 0.0f, 0.0f);
        renderText(-7.0f, -3.0f, "Back Story", GLUT_BITMAP_9_BY_15);
    }

    void levelShow(){
        // from G1->H1->F1->E1
        glBegin(GL_POLYGON);
        glColor3f(0.514f, 0.871f, 0.925f);
        glVertex2f(-10.0f, 10.0f);
        glVertex2f(10.0f, 10.0f);
        glVertex2f(10.0f, -10.0f);
        glVertex2f(-10.0f, -10.0f);
        glEnd();

        // from I1->J1->K1->L1
        glBegin(GL_POLYGON);
        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex2f(-1.5f, -6.5f);
        glVertex2f(1.5f, -6.5f);
        glVertex2f(1.5f, -7.5f);
        glVertex2f(-1.5f, -7.5f);
        glEnd();

        // Render text inside the polygon
        glColor3f(0.0f, 0.0f, 0.0f);
        renderText(-0.5f, -7.0f, "Back", GLUT_BITMAP_9_BY_15);

        // Render text inside the polygon
        glColor3f(0.0f, 0.0f, 0.0f);
        renderText(-3.0f, 8.0f, "The Path Way Levels", GLUT_BITMAP_TIMES_ROMAN_24);
        renderText(-6.5f, 4.0f, "One", GLUT_BITMAP_HELVETICA_18);
        renderText(-0.5f, 4.0f, "Two", GLUT_BITMAP_HELVETICA_18);
        renderText(5.4f, 4.0f, "Three", GLUT_BITMAP_HELVETICA_18);
        renderText(-6.5f, -3.0f, "Four", GLUT_BITMAP_HELVETICA_18);
        renderText(-0.4f, -3.0f, "Five", GLUT_BITMAP_HELVETICA_18);
        renderText(5.7f, -3.0f, "Six", GLUT_BITMAP_HELVETICA_18);

        drawPlaneForLevel(menuPlaneOne, planeColors);
        drawPlaneForLevel(menuPlaneTwo, planeColors);
        drawPlaneForLevel(menuPlaneThree, planeColors);
        drawPlaneForLevel(menuPlaneFour, planeColors);
        drawPlaneForLevel(menuPlaneFive, planeColors);
        drawPlaneForLevel(menuPlaneSix, planeColors);
    }

    void display(){
        cout << "Rendering Menu Screen" << endl;
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to gray and opaque
        glClear(GL_COLOR_BUFFER_BIT);        // Clear the color buffer (background)

        if(currentScreen == 0){
            // Projection setup
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(-10.0f, 10.0f, -10.0f, 10.0f, -1.0f, 1.0f);
            glMatrixMode(GL_MODELVIEW);

            menuShow();
        }
        else if(currentScreen == 1){
            // Projection setup
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(-10.0f, 10.0f, -10.0f, 10.0f, -1.0f, 1.0f); // Match the polygon's range
            glMatrixMode(GL_MODELVIEW);

            infoShow::display();
        }
        else if(currentScreen == 2){
            // Projection setup
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(-10.0f, 10.0f, -10.0f, 10.0f, -1.0f, 1.0f); // Match the polygon's range
            glMatrixMode(GL_MODELVIEW);
            levelShow();
        }

        glFlush(); // Render now
    }

    bool isInsideRect(float x, float y, float xMin, float xMax, float yMin, float yMax) {
        return (x >= xMin && x <= xMax && y >= yMin && y <= yMax);
    }

    bool isInsideCircle(float x, float y, float cx, float cy, float radius) {
        float dx = x - cx;
        float dy = y - cy;
        return (dx * dx + dy * dy) <= (radius * radius);
    }

    void mouseClickMenu(int button, int state, int x, int y) {
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            // Convert screen coordinates to OpenGL coordinates
            float normalizedX = (x / (float)750) * 20.0f - 10.0f; // Map x to [-10, 10]
            float normalizedY = -((y / (float)650) * 20.0f - 10.0f); // Map y to [-10, 10]

            // Check if click is inside the Start button rectangle
            if (isInsideRect(normalizedX, normalizedY, -1.5f, 1.5f, -7.5f, -6.5f)) {
                cout << "Start button clicked!" << endl;
                currentScreen = 2;
            }

            // Check if click is inside the circle S
            if (isInsideCircle(normalizedX, normalizedY, 9.5f, 9.5f, 0.5f)) {
                cout << "Information button clicked!" << endl;
                currentScreen = 1;
            }
            else if(isInsideCircle(normalizedX, normalizedY, -9.5f, 9.5f, 0.5f)){
                    toggleSound();
                cout << "Music button clicked!" << endl;
            }
            else if(isInsideRect(normalizedX, normalizedY, -7.0f, -5.0f, -2.0f, 0.0f)){
                cout << "Back Story button clicked!" << endl;
                currentScreen = 10;
                glutDisplayFunc(backStory::display);
            }
        }
    }

    void mouseClickInfo(int button, int state, int x, int y) {
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            // Convert screen coordinates to OpenGL coordinates
            float normalizedX = (x / (float)750) * 20.0f - 10.0f; // Adjust as per your screen size
            float normalizedY = -((y / (float)650) * 20.0f - 10.0f); // Adjust as per your screen size

            // Define the bounds of the "Back" button polygon
            float xMin = -1.5f, xMax = 1.5f; // Horizontal bounds of the "Back" button
            float yMin = -9.0f, yMax = -8.0f; // Vertical bounds of the "Back" button

            // Check if the click is inside the "Back" button polygon
            if (isInsideRect(normalizedX, normalizedY, xMin, xMax, yMin, yMax)) {
                cout << "Back button clicked! Returning to menu." << endl;
                currentScreen = 0; // Go back to the main menu
                glutPostRedisplay(); // Ensure screen refresh
            }
        }
    }

    void mouseClickLevel(int button, int state, int x, int y) {
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            // Convert screen coordinates to OpenGL coordinates
            float normalizedX = (x / (float)750) * 20.0f - 10.0f; // Adjust as per your screen size
            float normalizedY = -((y / (float)650) * 20.0f - 10.0f); // Adjust as per your screen size

            // Define the bounds of the "Back" button polygon
            float x1Min = -1.5f, x1Max = 1.5f; // Horizontal bounds of the "Back" button
            float y1Min = -7.5f, y1Max = -6.5f; // Vertical bounds of the "Back" button

            // Define the bounds for the "Level 1" button polygon plane
            float x2Min = -7.0f, x2Max = -5.0f;
            float y2Min = 5.0f, y2Max = 7.0f;

            // Define the bounds for the "Level 2" button polygon plane
            float x3Min = -1.0f, x3Max = 1.0f;
            float y3Min = 5.0f, y3Max = 7.0f;

            // Define the bounds for the "Level 3" button polygon plane
            float x4Min = 5.0f, x4Max = 7.0f;
            float y4Min = 5.0f, y4Max = 7.0f;

            // Define the bounds for the "Level 4" button polygon plane
            float x5Min = -7.0f, x5Max = -5.0f;
            float y5Min = -2.0f, y5Max = 0.0f;

            // Define the bounds for the "Level 5" button polygon plane
            float x6Min = -1.0f, x6Max = 1.0f;
            float y6Min = -2.0f, y6Max = 0.0f;

            // Define the bounds for the "Level 6" button polygon plane
            float x7Min = 5.0f, x7Max = 7.0f;
            float y7Min = -2.0f, y7Max = 0.0f;

            // Check if the click is inside the polygon
            if (isInsideRect(normalizedX, normalizedY, x1Min, x1Max, y1Min, y1Max)) {
                cout << "Back button clicked! Returning to menu." << endl;
                currentScreen = 0; // Go back to the main menu
                glutPostRedisplay(); // Ensure screen refresh
            }
            else if(isInsideRect(normalizedX, normalizedY, x2Min, x2Max, y2Min, y2Max)){
                cout << "Level 1 button clicked!" << endl;
                timer2=true;
                timer3=false;
                timer4=false;
                timer5=false;
                currentScreen = 3;

                if (currentScreen == 3){
                    // Projection setup
                    glMatrixMode(GL_PROJECTION);
                    glLoadIdentity();
                    glOrtho(-15.0f, 15.0f, -60.0f, -40.0f, -1.0f, 1.0f); // Match the polygon's range
                    glMatrixMode(GL_MODELVIEW);
                    glLoadIdentity();
                    glutKeyboardFunc(levelTwo::keyboard);
                    glutKeyboardUpFunc(levelTwo::keyboardUp);
                    glutDisplayFunc(levelTwo::display);
                    glutReshapeFunc(levelTwo::reshape);
                    glutTimerFunc(16, levelTwo::timer, 0);
                    levelTwo::init();
                    glutIdleFunc(levelTwo::idle);
                }

                glutPostRedisplay();   // Request a screen refresh
            }
            else if(isInsideRect(normalizedX, normalizedY, x3Min, x3Max, y3Min, y3Max)){
                cout << "Level 2 button clicked!" << endl;
                currentScreen = 4;
                timer2=false;
                timer3=true;
                timer4=false;
                timer5=false;

                if (currentScreen == 4){
                    // Projection setup
                    glMatrixMode(GL_PROJECTION);
                    glLoadIdentity();
                    glOrtho(-15.0f, 15.0f, -60.0f, -40.0f, -1.0f, 1.0f); // Match the polygon's range
                    glMatrixMode(GL_MODELVIEW);
                    glLoadIdentity();
                    glutKeyboardFunc(levelThree::keyboard);
                    glutKeyboardUpFunc(levelThree::keyboardUp);
                    glutDisplayFunc(levelThree::display);
                    glutReshapeFunc(levelThree::reshape);
                    glutTimerFunc(16, levelThree::timer, 0);
                    levelThree::init();
                    glutIdleFunc(levelThree::idle);
                }

                glutPostRedisplay();
            }
            else if(isInsideRect(normalizedX, normalizedY, x4Min, x4Max, y4Min, y4Max)){
                cout << "Level 3 button clicked!" << endl;
                currentScreen = 5;
                timer2=false;
                timer3=false;
                timer4=true;
                timer5=false;

                if (currentScreen == 5){
                    // Projection setup
                    glMatrixMode(GL_PROJECTION);
                    glLoadIdentity();
                    glOrtho(-15.0f, 15.0f, -60.0f, -40.0f, -1.0f, 1.0f); // Match the polygon's range
                    glMatrixMode(GL_MODELVIEW);
                    glLoadIdentity();
                    glutKeyboardFunc(levelFour::keyboard);
                    glutKeyboardUpFunc(levelFour::keyboardUp);
                    glutDisplayFunc(levelFour::display);
                    glutReshapeFunc(levelFour::reshape);
                    glutTimerFunc(16, levelFour::timer, 0);
                    levelFour::init();
                    glutIdleFunc(levelFour::idle);
                }

                glutPostRedisplay();
            }
            else if(isInsideRect(normalizedX, normalizedY, x5Min, x5Max, y5Min, y5Max)){
                cout << "Level 4 button clicked!" << endl;
                currentScreen = 6;
                timer2=false;
                timer3=false;
                timer4=false;
                timer5=true;

                if(currentScreen == 6){
                    // Projection setup
                    glMatrixMode(GL_PROJECTION);
                    glLoadIdentity();
                    glOrtho(-15.0f, 15.0f, -60.0f, -40.0f, -1.0f, 1.0f); // Match the polygon's range
                    glMatrixMode(GL_MODELVIEW);
                    glLoadIdentity();
                    glutKeyboardFunc(levelFive::keyboard);
                    glutKeyboardUpFunc(levelFive::keyboardUp);
                    glutDisplayFunc(levelFive::display);
                    glutReshapeFunc(levelFive::reshape);
                    glutTimerFunc(16, levelFive::timer, 0);
                    levelFive::init();
                    glutIdleFunc(levelFive::idle);
                }

                glutPostRedisplay();
            }
            else if(isInsideRect(normalizedX, normalizedY, x6Min, x6Max, y6Min, y6Max)){
                cout << "Level 5 button clicked!" << endl;
                glutPostRedisplay();
            }
            else if(isInsideRect(normalizedX, normalizedY, x7Min, x7Max, y7Min, y7Max)){
                cout << "Level 6 button clicked!" << endl;
                glutPostRedisplay();
            }
        }
    }

    void mouseClickStory(int button, int state, int x, int y) {
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            // Convert screen coordinates to OpenGL coordinates
            float normalizedX = (x / (float)750) * 20.0f - 10.0f; // Map x to [-10, 10]
            float normalizedY = -((y / (float)650) * 20.0f - 10.0f); // Map y to [-10, 10]

            // Define the bounds of the "Back" button polygon
            float xMin = -1.5f, xMax = 1.5f; // Horizontal bounds of the "Back" button
            float yMin = -9.0f, yMax = -8.0f; // Vertical bounds of the "Back" button

            // Check if click is inside the Start button rectangle
            if (isInsideRect(normalizedX, normalizedY, xMin, xMax, yMin, yMax)) {
                cout << "Back button clicked!" << endl;
                currentScreen = 0;
                glutDisplayFunc(menuBar::display);
            }
        }
    }

    void mouseClickGameOver(int button, int state, int x, int y){
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            // Convert screen coordinates to OpenGL coordinates
            float normalizedX = (x / (float)750) * 20.0f - 10.0f; // Adjust as per your screen size
            float normalizedY = -((y / (float)650) * 20.0f - 10.0f); // Adjust as per your screen size

            // Define the bounds of the "Back" button polygon
            float x1Min = -2.0f, x1Max = 2.0f; // Horizontal bounds of the "Back" button
            float y1Min = -4.0f, y1Max = -3.0f; // Vertical bounds of the "Back" button

            // Define the bounds of the "Back" button polygon
            float x2Min = -2.0f, x2Max = 2.0f; // Horizontal bounds of the "Back" button
            float y2Min = -7.0f, y2Max = -6.0f; // Vertical bounds of the "Back" button

            // Check if the click is inside the polygon
            if (isInsideRect(normalizedX, normalizedY, x1Min, x1Max, y1Min, y1Max)) {
                cout << "Try Again button clicked!" << endl;
                    if (currentScreen == 3){
                    // Projection setup
                    levelTwo::reset();
                    levelThree::reset();
                    levelFour::reset();
                    levelFive::reset();
                    glMatrixMode(GL_PROJECTION);
                    glLoadIdentity();
                    glOrtho(-15.0f, 15.0f, -60.0f, -40.0f, -1.0f, 1.0f); // Match the polygon's range
                    glMatrixMode(GL_MODELVIEW);
                    glLoadIdentity();
                    glutKeyboardFunc(levelTwo::keyboard);
                    glutKeyboardUpFunc(levelTwo::keyboardUp);
                    glutDisplayFunc(levelTwo::display);
                    glutReshapeFunc(levelTwo::reshape);
                    //glutTimerFunc(16, levelTwo::timer, 0);
                    levelTwo::init();
                    glutIdleFunc(levelTwo::idle);
                    glutPostRedisplay(); // Force redraw
                }
            }
            // Check if the click is inside the polygon
            else if (isInsideRect(normalizedX, normalizedY, x1Min, x1Max, y2Min, y2Max)) {
                cout << "Menu button clicked!" << endl;
                currentScreen = 0;
                glutDisplayFunc(menuBar::display);
                levelTwo::reset();
                levelThree::reset();
                levelFour::reset();
                levelFive::reset();
                glutPostRedisplay(); // Force redraw
                timer2=false;
                timer3=false;
                timer4=false;
                timer5=false;
            }
        }
}
    /* Reshape callback function */
    void reshape(int w, int h) {
        if (h == 0) h = 1; // Prevent divide by zero

        // Set the viewport to cover the new window
        glViewport(0, 0, w, h);
    }
}

int main(int argc, char** argv) {
    srand(static_cast<unsigned>(time(0)));

    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(750, 650); // Window size
    glutInitWindowPosition(100, 100); // Window position
    glutCreateWindow("The Path Way"); // Create window with title
    glLoadIdentity();

    glutDisplayFunc(menuBar::display);

    // Mouse interaction based on the screen
    glutMouseFunc([](int button, int state, int x, int y) {
        if (currentScreen == 0) {
            menuBar::mouseClickMenu(button, state, x, y);
        }
        else if (currentScreen == 1) {
            menuBar::mouseClickInfo(button, state, x, y);
        }
        else if (currentScreen == 2) {
            menuBar::mouseClickLevel(button, state, x, y);
        }
        else if(currentScreen == 3){
            menuBar::mouseClickGameOver(button, state, x, y);
        }
        else if(currentScreen == 10){
            menuBar::mouseClickStory(button, state, x, y);
        }
    }
    );

    infoShow::initStars();
    backStory::initStars();
    playSound();
    // Enter the GLUT event-processing loop
    glutMainLoop();


    return 0;
}

