#include "libs/myGLHeaders.h"
#include "components/Aircraft.h"

using namespace std;

Aircraft *myPlane = new Aircraft;

void prepare() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0, 1.0, 1.0, 1.0);

    myPlane->setupShaders("shader/jet.vert.glsl", "shader/jet.frag.glsl");
    myPlane->setupBuffers("source/airbus.obj");
    myPlane->loadIdentity();
}

void renderScene() {
    myPlane->render();
}

void changeSize(int w, int h) {
    myPlane->changeSize(w, h);
}

void processSpecialKeys(int key, int x, int y) {
    myPlane->processSpecialKeys(key, x, y);
}

void processNormalKeys(unsigned char key, int x, int y) {
    myPlane->processNormalKeys(key, x, y);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(1024, 768);
    glutCreateWindow("Project");

    glutDisplayFunc(renderScene);
    glutIdleFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutKeyboardFunc(processNormalKeys);
    glutSpecialFunc(processSpecialKeys);

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