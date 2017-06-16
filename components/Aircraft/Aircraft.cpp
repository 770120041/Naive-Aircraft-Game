//
// Created by Haotian on 17/5/30.
//

#include "Aircraft.h"

Aircraft::Aircraft(glm::mat4 &viewMatrix, glm::mat4 &projMatrix, glm::vec3 &cameraLocation) : viewMatObj(viewMatrix),
                                                                                              projMatObj(projMatrix),
                                                                                              cameraLocation(
                                                                                                      cameraLocation) {
    for (int i = 0; i < 3; i++) {
        planeVelocity[i] = 0;
        planeAcceleration[i] = 0;
    }
    setCameraCoordinate();
}

Aircraft::~Aircraft() {

}

void Aircraft::setupSkyBox(const char *skyShadervs, const char *skyShaderfs) {
    SkyBox::skybox_buffer(skyboxVAO, skyboxVBO);

    //sky box


    //read texture
    std::vector<std::string> faces
            {
                    "./texture/right.tga",
                    "./texture/left.tga",
                    "./texture/top.tga",
                    "./texture/bottom.tga",
                    "./texture/back.tga",
                    "./texture/front.tga"
            };
    cubemapTexture = SkyBox::loadCubemap(faces);

    skyShader = glCreateProgram();
    glAttachShader(skyShader, myGL::loadShader(GL_VERTEX_SHADER, skyShadervs));
    glAttachShader(skyShader, myGL::loadShader(GL_FRAGMENT_SHADER, skyShaderfs));
    glLinkProgram(skyShader);
    myGL::dumpProgramLog(skyShader);
}

void Aircraft::setupShaders(const char *vertBodyFile, const char *fragBodyFile, const char *vertShadowFile,
                            const char *fragShadowFile) {
    vb = myGL::loadShader(GL_VERTEX_SHADER, vertBodyFile);
    fb = myGL::loadShader(GL_FRAGMENT_SHADER, fragBodyFile);

    pb = glCreateProgram();
    glAttachShader(pb, vb);
    glAttachShader(pb, fb);

    glBindFragDataLocation(pb, 0, "FragColor");
    glLinkProgram(pb);
    myGL::dumpProgramLog(pb);

    BodyUniformLoc.vertexLoc = glGetAttribLocation(pb, "vertPosition");
    BodyUniformLoc.normalLoc = glGetAttribLocation(pb, "vertNormal");
    BodyUniformLoc.idLoc = glGetAttribLocation(pb, "materialId");

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
    shadowVert = myGL::loadShader(GL_VERTEX_SHADER, vertShadowFile);
    shadowFrag = myGL::loadShader(GL_FRAGMENT_SHADER, fragShadowFile);

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

    projMatObj = glm::perspective(glm::radians(45.0f), WinRatio, 1.0f, 5000000.0f);
    updateMVP();
}

void Aircraft::setupBuffers(const char *objPath, const char *objFile) {
    myGL::loadObj(objPath, objFile, vertices, uvs, normals, materials, material_ids);
    aabb.getAABB(vertices, scale);

    GLuint buffers[3];
    glGenVertexArrays(3, vao);
    glBindVertexArray(vao[0]);
    glGenBuffers(3, buffers);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(BodyUniformLoc.vertexLoc);
    glVertexAttribPointer(BodyUniformLoc.vertexLoc, 3, GL_FLOAT, 0, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(BodyUniformLoc.normalLoc);
    glVertexAttribPointer(BodyUniformLoc.normalLoc, 3, GL_FLOAT, 0, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
    glBufferData(GL_ARRAY_BUFFER, material_ids.size() * sizeof(GLfloat), &material_ids[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(BodyUniformLoc.idLoc);
    glVertexAttribPointer(BodyUniformLoc.idLoc, 1, GL_FLOAT, 0, 0, 0);


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

    for (int i = 0; i < materials.size(); i++) {
        std::string materialId;
        materialId = "Ka[";
        materialId += std::to_string(i);
        materialId += "]";
        glUniform3fv(glGetUniformLocation(pb, materialId.c_str()), 1,
                     glm::value_ptr(materials[i].Ka));

        materialId = "Kd[";
        materialId += std::to_string(i);
        materialId += "]";
        glUniform3fv(glGetUniformLocation(pb, materialId.c_str()), 1,
                     glm::value_ptr(materials[i].Kd));

        materialId = "Ks[";
        materialId += std::to_string(i);
        materialId += "]";
        glUniform3fv(glGetUniformLocation(pb, materialId.c_str()), 1,
                     glm::value_ptr(materials[i].Ks));
    }

    // light param
    glUniform3fv(BodyUniformLoc.AmbientLoc, 1, &Ambient[0]);
    glUniform3fv(BodyUniformLoc.LightColorLoc, 1, &LightColor[0]);
    glUniform3fv(BodyUniformLoc.LightDirectionLoc, 1, &LightDirection[0]);
    glUniform3fv(BodyUniformLoc.HalfVectorLoc, 1, &HalfVector[0]);
    glUniform1f(BodyUniformLoc.ShininessLoc, Shininess);
    glUniform1f(BodyUniformLoc.StrengthLoc, Strength);
}

void Aircraft::setCameraCoordinate() {
    glm::mat4 rotateMat = glm::rotate(rotateLR, Z) * glm::rotate(rotateQE, Y);
    glm::vec4 viewDirect4 = glm::vec4(glm::vec3(viewDirVect), 1.0f);
    glm::vec3 afterRotate = rotateMat * viewDirect4;
    dir = afterRotate * polar_r;
    LightDirection = normalize(glm::vec3(100.f, 200.f, 100.f));

    HalfVector = glm::normalize(LightDirection + viewDirVect);

    LightDirection = LightDirection * polar_r;

    dir += currentPos;

//    cout << lastPosition.x << " " << lastPosition.y << " " << lastPosition.z << endl;
    if (stopCameraTracing) {
        cameraLocation = lastPosition + cameraFront;
        viewMatObj = glm::lookAt(
                lastPosition,
                lastPosition + cameraFront,
                lookAtVect
        );

    } else if (firstPerson) {
        glm::vec3 firstPersonSight;
        firstPersonSight[0] = dir[0];
        firstPersonSight[1] = dir[1] - 475.0f;
        firstPersonSight[2] = dir[2] + 3500.0f;
        cameraLocation = firstPersonSight + cameraFront;
        viewMatObj = glm::lookAt(
                firstPersonSight,
                firstPersonSight + cameraFront,
                lookAtVect
        );
    } else {
        cameraLocation = dir + cameraFront;
        viewMatObj = glm::lookAt(
                dir,
                dir + cameraFront,
                lookAtVect
        );
    }


    updateMVP();
}

void Aircraft::idle() {
    GLfloat squareVelocity;
    GLfloat deltaT = 0.0001;
    for (int i = 0; i < 1000; i++) {
        currentPos[0] += planeVelocity[0] * deltaT;
        currentPos[1] += planeVelocity[1] * deltaT;
        currentPos[2] += planeVelocity[2] * deltaT;
        aabb.updataAABB(currentPos);
        planeVelocity[0] += planeAcceleration[0] * deltaT;
        planeVelocity[1] += planeAcceleration[1] * deltaT;
        planeVelocity[2] += planeAcceleration[2] * deltaT;

        squareVelocity = planeVelocity[0] * planeVelocity[0] + planeVelocity[1] * planeVelocity[1] +
                         planeVelocity[2] * planeVelocity[2];
        airFriction[0] = airFricConst * planeVelocity[0] * sqrt(squareVelocity);
        airFriction[1] = airFricConst * planeVelocity[1] * sqrt(squareVelocity);
        airFriction[2] = airFricConst * planeVelocity[2] * sqrt(squareVelocity);


        pullForce[0] = engineForce * upVector[0];
        pullForce[1] = engineForce * upVector[1];
        pullForce[2] = engineForce * upVector[2];


        squareVelocity = planeVelocity[0] * planeVelocity[0] + planeVelocity[1] * planeVelocity[1] +
                         planeVelocity[2] * planeVelocity[2];

        planeAcceleration[0] = (airFriction[0] + pullForce[0] + gravityForce[0]) / planeWeight;
        planeAcceleration[1] = (airFriction[1] + pullForce[1] + gravityForce[1]) / planeWeight;
        planeAcceleration[2] = (airFriction[2] + pullForce[2] + gravityForce[2]) / planeWeight;
    }
    //    printf("up x y z at %.3f %.3f %.3f\n",upVector[0],upVector[1],upVector[2]);
    aabb.updataAABB(currentPos);

    setCameraCoordinate();
}

void Aircraft::motion() {
    // todo !!! change plane pos and location to be rendered
//    glm::vec3 rotateAxis(glm::cross(Y, upVector));
//    GLfloat rotateDotProd = glm::dot(Z, glm::normalize());
//    GLfloat rotateAngle = glm::acos(rotateDotProd);
//    glm::mat4 rotateMat(1.f);
//    if (rotateAngle > 0.01f) {
//        rotateMat = glm::rotate(rotateAngle, rotateAxis);
//    }
//    modelMatObj = glm::translate(currentPos) * rotateMat;


    glm::mat4 rotateMat = glm::rotate(rotateLR, Z) * glm::rotate(rotateUD, X) * glm::rotate(rotateQE, Y);
    modelMatObj = glm::translate(currentPos) * rotateMat * glm::scale(glm::vec3(scale));

    upVector = rotateMat * glm::vec4(Z, 1.f);

    //cout << engineForce << " " << planeAcceleration.x << " " << planeAcceleration.y << " " << planeAcceleration.z << endl;
    //printf("!!up x y z at %.3f %.3f %.3f\n",planeAcceleration[0],planeAcceleration[1],planeAcceleration[2]);
    // todo !!! motion matrix
}

void Aircraft::render() {
//    cout << currentPos.x << " " << currentPos.y << " " << currentPos.z << endl;
    glm::mat4 lightViewMatrix = glm::lookAt(LightDirection + currentPos, glm::vec3(currentPos), Y),
            lightProjectionMatrix(glm::ortho(-1200.f, 1200.f, -1200.f, 1200.f, 1000.f, 2500.f));
    //lightProjectionMatrix(glm::perspective(glm::radians(45.0f), 1.f, 1.0f, 5000.f));
    // todo !!!! ortho param quite strange still

    shadowMVPMatObj = lightProjectionMatrix * lightViewMatrix * modelMatObj;
    MVPMatObj = scaleBiasMatrix * lightProjectionMatrix * lightViewMatrix;

    glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
    glViewport(0, 0, DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE);

    glClearDepth(1.f);
    glClear(GL_DEPTH_BUFFER_BIT);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(3.f, 3.f);

    glUseProgram(shadowProgram);

    glUniformMatrix4fv(ShadowUniformLoc.MVPMatrixLoc, 1, 0, &shadowMVPMatObj[0][0]);
    glBindVertexArray(vao[1]);

    glDrawArrays(GL_TRIANGLES, 0, vertices.size());

    glDisable(GL_POLYGON_OFFSET_FILL);

    // return to GLUT frame
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, winW, winH);

    glUseProgram(pb);
    setBodyUniforms();

    glBindTexture(GL_TEXTURE_2D, depth_texture);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindVertexArray(vao[0]);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    glBindTexture(GL_TEXTURE_2D, 0);

    //sky box
    glUseProgram(skyShader);
    glm::mat4 view = glm::mat4(glm::mat3(viewMatObj));
    glUniformMatrix4fv(glGetUniformLocation(skyShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(skyShader, "projection"), 1, GL_FALSE, glm::value_ptr(projMatObj));
    SkyBox::render_skybox(skyboxVAO, cubemapTexture);
    //end of skybox
}

void Aircraft::processNormalKeys(unsigned char key, int x, int y) {
    if (key == 27) {
        glDeleteVertexArrays(3, vao);
        glDeleteProgram(pb);
        glDeleteShader(vb);
        glDeleteShader(fb);
        exit(0);
    }
    GLfloat rotateAngle = 0.2f * 3.1415926f / 180.f;
    glm::vec3 tempVector = upVector;
    // double  engineForce = 9.8000001;
    switch (key) {


        //engine star up speed
        // case 'F':
        // case 'f':
        //     engineForce = 12.8;
        //     break;

        //upVector (x,y,z)
        //X-axis rotate matrix
        //[1 0 0] [0 cos -sin] [0 sin cos]
        case 'F':
        case 'f':
            firstPerson = !firstPerson;
            break;
        case 'H':
        case 'h':
            lastPosition = dir;
            stopCameraTracing = !stopCameraTracing;
            break;
        case 'I':
        case 'i':
            if (stopCameraTracing) {
                lastPosition[2] += 70;
            }
            break;
        case 'K':
        case 'k':
            if (stopCameraTracing) {
                lastPosition[2] -= 70;
            }
            break;
        case 'O':
        case 'o':
            if (stopCameraTracing) {
                lastPosition[1] -= 70;
            }
            break;
        case 'U':
        case 'u':
            if (stopCameraTracing) {
                lastPosition[1] += 70;
            }
            break;
        case 'J':
        case 'j':
            if (stopCameraTracing) {
                lastPosition[0] += 30;
            }
            break;
        case 'L':
        case 'l':
            if (stopCameraTracing) {
                lastPosition[0] -= 30;
            }
            break;
        case 's':
        case 'S':
            rotateUD += glm::radians(0.5f);

            break;
        case 'W':
        case 'w':
            rotateUD -= glm::radians(0.5f);

            break;

            //¥-axis rotate matrix
            //[cos 0 sin(-)] [0 1 0] [-sin(-) 0 cos]

        case 'D':
        case 'd':
            rotateLR += glm::radians(0.25f);
            break;

        case 'A':
        case 'a':
            rotateLR -= glm::radians(0.25f);
            break;
        case 'Q':
        case 'q':
            rotateQE += glm::radians(0.25f);
            break;
        case 'E':
        case 'e':
            rotateQE -= glm::radians(0.25f);
            break;

        case '[':
            engineForce += 40;
            break;

        case ']':
            engineForce -= 40;
            break;

        case 'Z':
        case 'z':
            polar_r *= 1.05;
            break;
        case 'X':
        case 'x':
            polar_r /= 1.05;
            break;
        default:
            break;
    }

    setCameraCoordinate();
}

void Aircraft::processMouseAction(int button, int state, int x, int y) {
    if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
        resetPos = true;
    }
}

void Aircraft::processMouseMotion(int xpos, int ypos) {
    if (resetPos) {
        lastX = xpos;
        lastY = ypos;
        resetPos = false;
    }
    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    GLfloat sensitivity = 0.5;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    if (Pitch > 89.0)
        Pitch = 89.0;
    if (Pitch < -89.0)
        Pitch = -89.0;

    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    cameraFront = glm::normalize(front);
    setCameraCoordinate();
}

bool Aircraft::isCollided(AABB *a) {
    return aabb.isCollided(a);
}
