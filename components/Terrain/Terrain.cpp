//
// Created by Haotian on 17/6/14.
//

#include "Terrain.h"

Terrain::Terrain(glm::mat4 &viewMatrix, glm::mat4 &projMatrix, glm::vec3 &cameraLocation) : viewMatObj(viewMatrix), projMatObj(projMatrix), globalOffset(cameraLocation) {

}

void Terrain::setupShaders(const char *vertFile, const char *fragFile) {
    program = glCreateProgram();

    myGL::attachShader(program, GL_VERTEX_SHADER, vertFile);
    myGL::attachShader(program, GL_FRAGMENT_SHADER, fragFile);

    glBindFragDataLocation(program, 0, "FragColor");
    glLinkProgram(program);
    myGL::dumpProgramLog(program);

    BodyUniformLoc.ViewMatrixLoc = glGetUniformLocation(program, "viewMatrix");
    BodyUniformLoc.ProjectionMatrixLoc = glGetUniformLocation(program, "projectionMatrix");

    BodyUniformLoc.uEdgeMorphLoc = glGetUniformLocation(program, "uEdgeMorph");
    BodyUniformLoc.globalOffsetLoc = glGetUniformLocation(program, "globalOffset");
    BodyUniformLoc.uOffsetLoc = glGetUniformLocation(program, "uOffset");
    BodyUniformLoc.uScaleLoc = glGetUniformLocation(program, "uScale");
}

void Terrain::setupBuffers() {
    this->noise();
    this->setupPlane();

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);

    GLint vertloc = glGetAttribLocation(program, "vertPosition");

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, plane.size() * sizeof(glm::vec2), &plane[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(vertloc);
    glVertexAttribPointer(vertloc, 2, GL_FLOAT, 0, 0, 0);
}

void Terrain::render() {
    glUseProgram(program);
    glBindTexture(GL_TEXTURE_2D, tbo);
    glBindVertexArray(vao);
    glUniformMatrix4fv(BodyUniformLoc.ViewMatrixLoc, 1, 0, glm::value_ptr(viewMatObj));
    glUniformMatrix4fv(BodyUniformLoc.ProjectionMatrixLoc, 1, 0, glm::value_ptr(projMatObj));
    glUniform3fv(BodyUniformLoc.globalOffsetLoc, 1, glm::value_ptr(globalOffset));

    GLfloat initialScale = worldWidth / pow( 2, levels );
    this->createTile(glm::vec2(-initialScale, -initialScale), initialScale, Edge.NONE);
    this->createTile(glm::vec2(-initialScale, 0), initialScale, Edge.NONE );
    this->createTile(glm::vec2(0, 0), initialScale, Edge.NONE );
    this->createTile(glm::vec2(0, -initialScale), initialScale, Edge.NONE );

    for ( GLfloat scale = initialScale; scale < worldWidth; scale *= 2 ) {
        this->createTile(glm::vec2( -2 * scale, -2 * scale), scale, Edge.BOTTOM | Edge.LEFT );
        this->createTile(glm::vec2( -2 * scale, -scale), scale, Edge.LEFT );
        this->createTile(glm::vec2( -2 * scale, 0), scale, Edge.LEFT );
        this->createTile(glm::vec2( -2 * scale, scale), scale, Edge.TOP | Edge.LEFT );

        this->createTile(glm::vec2( -scale, -2 * scale), scale, Edge.BOTTOM );
        // 2 tiles 'missing' here are in previous layer
        this->createTile(glm::vec2( -scale, scale), scale, Edge.TOP );

        this->createTile(glm::vec2( 0, -2 * scale), scale, Edge.BOTTOM );
        // 2 tiles 'missing' here are in previous layer
        this->createTile(glm::vec2( 0, scale), scale, Edge.TOP );

        this->createTile(glm::vec2( scale, -2 * scale), scale, Edge.BOTTOM | Edge.RIGHT );
        this->createTile(glm::vec2( scale, -scale), scale, Edge.RIGHT );
        this->createTile(glm::vec2( scale, 0), scale, Edge.RIGHT );
        this->createTile(glm::vec2( scale, scale), scale, Edge.TOP | Edge.RIGHT );
    }

    tiles.clear();
}

void Terrain::createTile(glm::vec2 offset, GLfloat scale, GLint edgeMorph) {
    int i = tiles.size();
    tiles.emplace_back(offset, scale, edgeMorph);
    glUniform2fv(BodyUniformLoc.uOffsetLoc, 1, glm::value_ptr(tiles[i].offset));
    glUniform1f(BodyUniformLoc.uScaleLoc, tiles[i].scale);
    glUniform1i(BodyUniformLoc.uEdgeMorphLoc, tiles[i].edgeMorph);

    glDrawArrays(GL_TRIANGLES, 0, plane.size());
}

void Terrain::setupPlane() {
    int resolution = 128;

    double ax, bx, cx, dx, ay, by, cy, dy;

    for (int i=0; i<resolution; i++) {
        for (int j=0; j<resolution; j++) {
            ax = (double)(i) / resolution;
            bx = (double)(i+1) / resolution;
            cx = ax;

            ay = (double)(j) / resolution;
            by = ay;
            cy = (double)(j+1) / resolution;

            plane.emplace_back(ax, ay);
            plane.emplace_back(bx, by);
            plane.emplace_back(cx, cy);

            dx = (double)(i+1) / resolution;
            dy = (double)(j+1) / resolution;

            plane.emplace_back(bx, by);
            plane.emplace_back(dx, dy);
            plane.emplace_back(cx, cy);
        }
    }
}

void Terrain::noise() {
    const GLuint width = 1024;
    GLuint size = width * width;

    PerlinNoise *noiseGen = new PerlinNoise();

    if (noiseData != nullptr) {
        delete []noiseData;
    }

    noiseData = new GLfloat [size];

    memset(noiseData, 0, sizeof(GLfloat) * size);

    srand((unsigned)time(0));

    int quality = 1;
    int z = rand() % 100;

    for (int iteration = 0; iteration < 4; iteration++) {
        for (int i=0; i<size; i++) {
            double x = i % width;
            double y = i / width;
            noiseData[i] += fabs(noiseGen->noise(x / quality, y / quality, z) * quality);
        }
        quality *= 5;
    }

    glGenTextures(1, &tbo);
    glBindTexture(GL_TEXTURE_2D, tbo);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, width, width);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, width, GL_RED, GL_FLOAT, &noiseData[0]);
}
