#include "libs/myGLHeaders.h"
#include "components/Aircraft/Aircraft.h"
#include "components/Terrain/Terrain.h"

using namespace std;

glm::mat4 viewMatrix, projMatrix;
glm::vec3 cameraLocation;

Aircraft *myPlane = new Aircraft(viewMatrix, projMatrix, cameraLocation);
AABB *myFloor=new AABB;
glm::vec3 Floormin = glm::vec3(-20000.f, 500.f, -26000.f), Floormax = glm::vec3(20000.f, 5000.f, 25000.f);
Terrain *myTerrain = new Terrain(viewMatrix, projMatrix, cameraLocation);

void prepare() {
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearColor(1.0, 1.0, 1.0, 1.0);

    myFloor->setAABB(Floormin, Floormax);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    myPlane->setupShaders("shader/jet/body.vert", "shader/jet/body.frag",
                          "shader/jet/shadow.vert", "shader/jet/shadow.frag");
    myPlane->setupBuffers("source/", "myplane.obj");
    myPlane->setupSkyBox("shader/cube/vert.glsl", "shader/cube/frag.glsl");

    myPlane->loadIdentity();
    myPlane->processMouseMotion(0, 0);

    //myFloor->setupShaders("shader/floor/vert.glsl", "shader/floor/tc.glsl", "shader/floor/te.glsl", "shader/floor/frag.glsl");

    myTerrain->setupShaders("shader/terrain/vert.glsl", "shader/terrain/frag.glsl");
    myTerrain->setupBuffers("source/rock2.jpg", "source/airport.jpg");

    myGL::dumpGLErrorLog();
}

void idleFunc() {
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    myPlane->idle();
    if (myPlane->isCollided(myFloor)) {
        exit(0);
    }
    myPlane->motion();
    myPlane->render();

    //myFloor->render();

    myTerrain->render();

    glutSwapBuffers();
    myGL::dumpGLErrorLog();
}

void changeSize(int w, int h) {
    myPlane->changeSize(w, h);
}

void processSpecialKeys(int key, int x, int y) {
    //myPlane->processSpecialKeys(key, x, y);
}

void processNormalKeys(unsigned char key, int x, int y) {
    myPlane->processNormalKeys(key, x, y);
}

void onMouseMove(int x, int y) {
    myPlane->processMouseMotion(x, y);
}

void onMouseAction(int button, int state, int x, int y) {
    myPlane->processMouseAction(button, state, x, y);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(1024, 768);
    glutCreateWindow("Project");

    glutDisplayFunc(idleFunc);
    glutIdleFunc(idleFunc);
    glutReshapeFunc(changeSize);
    glutKeyboardFunc(processNormalKeys);
    glutSpecialFunc(processSpecialKeys);
    glutMotionFunc(onMouseMove);
    glutMouseFunc(onMouseAction);

    glewInit();
    if (glewIsSupported("GL_VERSION_4_1"))
        printf("Ready for OpenGL 4.1\n");
    else {
        fprintf(stderr, "OpenGL 4.1 not supported!!\n");
        exit(1);
    }
    prepare();
    glutMainLoop();
    return 0;
}
