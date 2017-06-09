//
// Created by Haotian on 17/5/30.
//

#include "Aircraft.h"

Aircraft::Aircraft() {

}

Aircraft::~Aircraft() {

}

void Aircraft::setupShaders(const char *vertBodyFile, const char *fragBodyFile, const char *vertShadowFile,
                            const char *fragShadowFile) {
    std::string vs, fs;
    const char *vv, *ff;

    vb = glCreateShader(GL_VERTEX_SHADER);
    fb = glCreateShader(GL_FRAGMENT_SHADER);

    vs = myGL::readShader(vertBodyFile);
    fs = myGL::readShader(fragBodyFile);

    vv = vs.c_str();
    ff = fs.c_str();

    glShaderSource(vb, 1, &vv, NULL);
    glShaderSource(fb, 1, &ff, NULL);

    glCompileShader(vb);
    glCompileShader(fb);

    myGL::dumpShaderLog(vb);
    myGL::dumpShaderLog(fb);

    pb = glCreateProgram();
    glAttachShader(pb, vb);
    glAttachShader(pb, fb);

    glBindFragDataLocation(pb, 0, "FragColor");
    glLinkProgram(pb);
    myGL::dumpProgramLog(pb);

    BodyUniformLoc.vertexLoc = glGetAttribLocation(pb, "vertPosition");
    BodyUniformLoc.normalLoc = glGetAttribLocation(pb, "vertNormal");

    BodyUniformLoc.ModelMatrixLoc = glGetUniformLocation(pb, "modelMatrix");
    BodyUniformLoc.ViewMatrixLoc = glGetUniformLocation(pb, "viewMatrix");
    BodyUniformLoc.ProjectionMatrixLoc = glGetUniformLocation(pb, "projectionMatrix");
    BodyUniformLoc.MVPMatrixLoc = glGetUniformLocation(pb, "shadowMatrix");

    BodyUniformLoc.AmbientLoc = glGetUniformLocation(pb, "Ambient");
    BodyUniformLoc.LightColorLoc = glGetUniformLocation(pb, "LightColor");
    BodyUniformLoc.LightDirectionLoc = glGetUniformLocation(pb, "LightDirection");
    BodyUniformLoc.HalfVectorLoc = glGetUniformLocation(pb, "HalfVector");
    BodyUniformLoc.ShininessLoc = glGetUniformLocation(pb, "Shininess");
    BodyUniformLoc.StrengthLoc = glGetUniformLocation(pb, "Strength");

    // new shader
    shadowVert = glCreateShader(GL_VERTEX_SHADER);
    shadowFrag = glCreateShader(GL_FRAGMENT_SHADER);

    vs = myGL::readShader(vertShadowFile);
    fs = myGL::readShader(fragShadowFile);

    vv = vs.c_str();
    ff = fs.c_str();

    glShaderSource(shadowVert, 1, &vv, NULL);
    glShaderSource(shadowFrag, 1, &ff, NULL);

    glCompileShader(shadowVert);
    glCompileShader(shadowFrag);

    myGL::dumpShaderLog(shadowVert);
    myGL::dumpShaderLog(shadowFrag);

    shadowProgram = glCreateProgram();
    glAttachShader(shadowProgram, shadowVert);
    glAttachShader(shadowProgram, shadowFrag);

    glBindFragDataLocation(shadowProgram, 0, "FragColor");
    glLinkProgram(shadowProgram);
    myGL::dumpProgramLog(shadowProgram);

    ShadowUniformLoc.MVPMatrixLoc = glGetUniformLocation(shadowProgram, "MVPMatrix");
    ShadowUniformLoc.vertexLoc = glGetAttribLocation(shadowProgram, "vertPosition");
}

void Aircraft::updateMVP() {
    MVPMatObj = projMatObj * viewMatObj;
}

void Aircraft::changeSize(int w, int h) {
    if (h == 0) { h = 1; }

    winW = w;
    winH = h;
    glViewport(0, 0, w, h);

    cout << "Current viewport: " << w << " " << h << endl;

    WinRatio = (GLfloat) w / h;

    projMatObj = glm::perspective(glm::radians(45.0f), WinRatio, 1.0f, 5000.0f);
    updateMVP();
}

void Aircraft::setupBuffers(const char *objFile) {
    myGL::loadObj(objFile, vertices, uvs, normals);

    GLuint buffers[2];
    glGenVertexArrays(3, vao);
    glBindVertexArray(vao[0]);
    glGenBuffers(2, buffers);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(BodyUniformLoc.vertexLoc);
    glVertexAttribPointer(BodyUniformLoc.vertexLoc, 3, GL_FLOAT, 0, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(BodyUniformLoc.normalLoc);
    glVertexAttribPointer(BodyUniformLoc.normalLoc, 3, GL_FLOAT, 0, 0, 0);

    glBindVertexArray(vao[1]);
    glGenBuffers(1, buffers);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(ShadowUniformLoc.vertexLoc);
    glVertexAttribPointer(ShadowUniformLoc.vertexLoc, 3, GL_FLOAT, 0, 0, 0);

    glUseProgram(pb);
    glUniform1i(glGetUniformLocation(pb, "depthTexture"), 0);

    glGenTextures(1, &depth_texture);
    glBindTexture(GL_TEXTURE_2D, depth_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE,
                 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
                    GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &depth_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_texture, 0);

    glDrawBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    GLuint ebo[5];
    static const GLushort vertex_indices[] = {0, 1, 2};
    glGenBuffers(2, ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertex_indices), vertex_indices, GL_STATIC_DRAW);
}

void Aircraft::setBodyUniforms() {
    glUseProgram(pb);

    glUniformMatrix4fv(BodyUniformLoc.ModelMatrixLoc, 1, 0, &modelMatObj[0][0]);
    glUniformMatrix4fv(BodyUniformLoc.ViewMatrixLoc, 1, 0, &viewMatObj[0][0]);
    glUniformMatrix4fv(BodyUniformLoc.ProjectionMatrixLoc, 1, 0, &projMatObj[0][0]);

    glUniformMatrix4fv(BodyUniformLoc.MVPMatrixLoc, 1, 0, &MVPMatObj[0][0]);

    // light param
    glUniform3fv(BodyUniformLoc.AmbientLoc, 1, &Ambient[0]);
    glUniform3fv(BodyUniformLoc.LightColorLoc, 1, &LightColor[0]);
    glUniform3fv(BodyUniformLoc.LightDirectionLoc, 1, &LightDirection[0]);
    glUniform3fv(BodyUniformLoc.HalfVectorLoc, 1, &HalfVector[0]);
    glUniform1f(BodyUniformLoc.ShininessLoc, Shininess);
    glUniform1f(BodyUniformLoc.StrengthLoc, Strength);
}

void Aircraft::setCameraCoordinate() {
    glm::vec3 dir = viewDirVect * polar_r;

    LightDirection = normalize(glm::vec3(-100.f, -200.f, -100.f));

    HalfVector = glm::normalize(LightDirection + viewDirVect);

    LightDirection = LightDirection * polar_r;

    viewMatObj = glm::lookAt(
            dir,
            glm::vec3(0.f),
            lookAtVect
    );

    updateMVP();
}

void Aircraft::idle() {
    upVector = glm::rotate(upVector, glm::radians(1.f), X);
    currentPos += glm::vec3(.3f);
}

void Aircraft::motion() {
    // todo !!! change plane pos and location to be rendered
    glm::vec3 rotateAxis(glm::cross(Z, upVector));
    GLfloat rotateDotProd = glm::dot(Z, glm::normalize(upVector));
    GLfloat rotateAngle = glm::acos(rotateDotProd);
    glm::mat4 rotateMat(1.f);
    if (rotateAngle > 0.01) {
        rotateMat = glm::rotate(rotateAngle, rotateAxis);
    }
    modelMatObj = glm::translate(currentPos) * rotateMat;
    // todo !!! motion matrix
}

void Aircraft::render() {
    glm::mat4 lightViewMatrix = glm::lookAt(LightDirection, glm::vec3(0.f), Y),
            lightProjectionMatrix(glm::ortho(-1500.f, 1500.f, -1500.f, 1500.f, 1000.f, 2500.f));
    //lightProjectionMatrix(glm::perspective(glm::radians(45.0f), 1.f, 1.0f, 5000.f));
    // todo !!!! ortho param quite strange still

    shadowMVPMatObj = lightProjectionMatrix * lightViewMatrix;
    MVPMatObj = scaleBiasMatrix * shadowMVPMatObj;

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
    glViewport(0, 0, DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE);

    glClearDepth(1.f);
    glClear(GL_DEPTH_BUFFER_BIT);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(20.f, 40.f);

    glUseProgram(shadowProgram);

    glUniformMatrix4fv(ShadowUniformLoc.MVPMatrixLoc, 1, 0, &shadowMVPMatObj[0][0]);
    glBindVertexArray(vao[1]);

    glDrawArrays(GL_TRIANGLES, 0, vertices.size());

    glDisable(GL_POLYGON_OFFSET_FILL);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, winW, winH);

    glUseProgram(pb);
    setBodyUniforms();

    glBindTexture(GL_TEXTURE_2D, depth_texture);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindVertexArray(vao[0]);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());

    glutSwapBuffers();

    myGL::dumpGLErrorLog();

}

void Aircraft::processSpecialKeys(int key, int x, int y) {
    glm::vec3 AxisX, AxisY(-viewDirVect.z, 0.f, viewDirVect.x), testVect;
    testVect = viewDirVect;
    switch (key) {
        case GLUT_KEY_UP:
            viewDirVect = glm::rotate(viewDirVect, glm::radians(1.0f), lookAtVect);
            break;
        case GLUT_KEY_DOWN:
            viewDirVect = glm::rotate(viewDirVect, glm::radians(-1.0f), lookAtVect);
            break;
        case GLUT_KEY_LEFT:
            lookAtVect = glm::rotate(lookAtVect, glm::radians(1.0f), viewDirVect);
            break;
        case GLUT_KEY_RIGHT:
            lookAtVect = glm::rotate(lookAtVect, glm::radians(-1.0f), viewDirVect);
            break;
        default:
            break;
    }
    setCameraCoordinate();
}

void Aircraft::processNormalKeys(unsigned char key, int x, int y) {
    if (key == 27) {
        glDeleteVertexArrays(3, vao);
        glDeleteProgram(pb);
        glDeleteShader(vb);
        glDeleteShader(fb);
        exit(0);
    }

    switch (key) {
        case 'w':
        case 'W':
            polar_r *= 1.05;
            break;
        case 's':
        case 'S':
            polar_r /= 1.05;
            break;
        default:
            break;
    }

    setCameraCoordinate();
}
