/*
* Kuerteil fuer den CG-Kurs SoSe 2016 von
* Benjamin Molnar und Igor Turanin
*
* Umsetzung von 3D Tetris
*/

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <array>
#include <iostream>
// Include GLEW
#include <GL/glew.h>
#include <time.h>
#include <set>

// Include GLFW
#include <GLFW/glfw3.h>

// Include Sound
#include "irrKlang-1.5.0/include/irrKlang.h"
using namespace irrklang;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;
#include "objloader.hpp"
#include "texture.hpp"


// Achtung, die OpenGL-Tutorials nutzen glfw 2.7, glfw kommt mit einem ver�nderten API schon in der Version 3 

// Befindet sich bei den OpenGL-Tutorials unter "common"
#include "shader.hpp"

// Wuerfel und Kugel
#include "objects.hpp"
#include <string>
#include <ctime>
#include "color.h"
#include "cube.h"
#include "stein.h"
#include "timer.h"


void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}
bool cheating;
float turnXg;
float turnYg;
float turnZg;
float turnX;
float turnY;
float turnZ;
float turnX1;
float turnY1;
float turnZ1;
float turnX2;
float turnY2;
float turnZ2;

float zoom;
// Startperspektive=1;

int current_perspective = 0;
std::vector<GLuint> textures;
std::vector<GLuint> ttextures;

// Diese Drei Matrizen global (Singleton-Muster), damit sie jederzeit modifiziert und
// an die Grafikkarte geschickt werden koennen
glm::mat4 Projection;
glm::mat4 View;
glm::mat4 Model;
GLuint programID;



glm::vec3 f1(50.0 / 10.0, 2.5 / 100.0, 2.5 / 100.0);
glm::vec3 f2(2.5 / 100.0, 50.0 / 10.0, 2.5 / 100.0);
glm::vec3 f3(2.5 / 100.0, 2.5 / 100.0, 50.0 / 10.0);

// fuer Punktekalkulation
unsigned int points = 0;
std::string points_str;
char const* points_char_array;

//AUS GL Tutorial 11
unsigned int Text2DTextureID;
unsigned int Text2DVertexBufferID;
unsigned int Text2DUVBufferID;
unsigned int Text2DShaderID;
unsigned int Text2DUniformID;

void initText2D(const char * texturePath) {

	// Initialize texture
	Text2DTextureID = loadDDS(texturePath);

	// Initialize VBO
	glGenBuffers(1, &Text2DVertexBufferID);
	glGenBuffers(1, &Text2DUVBufferID);

	// Initialize Shader
	Text2DShaderID = LoadShaders("TextVertexShader.vertexshader", "TextVertexShader.fragmentshader");

	// Initialize uniforms' IDs
	Text2DUniformID = glGetUniformLocation(Text2DShaderID, "myTextureSampler");

}

void printText2D(const char * text, int x, int y, int size) {

	glDisable(GL_DEPTH_TEST);
	unsigned int length = strlen(text);

	// Fill buffers
	std::vector<glm::vec2> vertices;
	std::vector<glm::vec2> UVs;
	for (unsigned int i = 0; i<length; i++) {

		glm::vec2 vertex_up_left = glm::vec2(x + i*size, y + size);
		glm::vec2 vertex_up_right = glm::vec2(x + i*size + size, y + size);
		glm::vec2 vertex_down_right = glm::vec2(x + i*size + size, y);
		glm::vec2 vertex_down_left = glm::vec2(x + i*size, y);

		vertices.push_back(vertex_up_left);
		vertices.push_back(vertex_down_left);
		vertices.push_back(vertex_up_right);

		vertices.push_back(vertex_down_right);
		vertices.push_back(vertex_up_right);
		vertices.push_back(vertex_down_left);

		char character = text[i];
		float uv_x = (character % 16) / 16.0f;
		float uv_y = (character / 16) / 16.0f;

		glm::vec2 uv_up_left = glm::vec2(uv_x, uv_y);
		glm::vec2 uv_up_right = glm::vec2(uv_x + 1.0f / 16.0f, uv_y);
		glm::vec2 uv_down_right = glm::vec2(uv_x + 1.0f / 16.0f, (uv_y + 1.0f / 16.0f));
		glm::vec2 uv_down_left = glm::vec2(uv_x, (uv_y + 1.0f / 16.0f));
		UVs.push_back(uv_up_left);
		UVs.push_back(uv_down_left);
		UVs.push_back(uv_up_right);

		UVs.push_back(uv_down_right);
		UVs.push_back(uv_up_right);
		UVs.push_back(uv_down_left);
	}
	glBindBuffer(GL_ARRAY_BUFFER, Text2DVertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, Text2DUVBufferID);
	glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs[0], GL_STATIC_DRAW);

	// Bind shader
	glUseProgram(Text2DShaderID);

	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Text2DTextureID);
	// Set our "myTextureSampler" sampler to user Texture Unit 0
	glUniform1i(Text2DUniformID, 0);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, Text2DVertexBufferID);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, Text2DUVBufferID);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Draw call
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	glDisable(GL_BLEND);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glEnable(GL_DEPTH_TEST);

}

void cleanupText2D() {

	// Delete buffers
	glDeleteBuffers(1, &Text2DVertexBufferID);
	glDeleteBuffers(1, &Text2DUVBufferID);

	// Delete texture
	glDeleteTextures(1, &Text2DTextureID);

	// Delete shader
	glDeleteProgram(Text2DShaderID);
}
//GL Tutorial 11 ENDE

void sendMVP()
{
	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 MVP = Projection * View * Model;
	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform, konstant fuer alle Eckpunkte
	glUniformMatrix4fv(glGetUniformLocation(programID, "M"), 1, GL_FALSE, &Model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(programID, "V"), 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(programID, "P"), 1, GL_FALSE, &Projection[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(programID, "MVP"), 1, GL_FALSE, &MVP[0][0]); //schick ich Matrix in Shader rein (zun�chst Vertexshader)
}


//------------------------------------------------------------------
//------------------------------------------------------------------ New Code Ben


bool blinkFlag = false;
bool blinkFlag2 = false;



void initSchacht() {
	for (int i = 0; i < xLen; i++) {
		for (int j = 0; j < yLen; j++) {
			for (int k = 0; k < zLen; k++) {
				schacht_array[i][j][k] = transparent;
			}
		}
	}
};



void drawShadowCube() {
	glm::mat4 Save = Model;
	glBindTexture(GL_TEXTURE_2D, 0);
	Model = glm::translate(Model, glm::vec3(0, -.99, 0));
	Model = glm::scale(Model, glm::vec3(1, .01, 1));
	sendMVP();
	drawCube();
	Model = Save;
	//sendMVP();
}

void drawTexCube(int c) {
	glBindTexture(GL_TEXTURE_2D, textures.at(c));
	drawCube();
}

void drawTexTile(int c) {
	glBindTexture(GL_TEXTURE_2D, ttextures.at(c));
	drawCube();
}


float cus = 1.;			// cube-unit-size
float tilesize = .005;	// dicke des bodens

int randTileColor;
std::vector<int> tileTextures;
void randTiles() {
	for (int i = 0; i < xLen*zLen; i++) {
		randTileColor = rand() % 15 + 8;
		tileTextures.push_back(randTileColor);
	}
}


void drawTiles() {

	//int randColor = rand() % 5 + 1;

	glm::mat4 Save = Model;
	Model = glm::translate(Model, glm::vec3(0, -10, 0));

	/*Model = glm::translate(Model, glm::vec3(xLen-cus, tilesize -cus, zLen+cus));
	Model = glm::scale(Model, glm::vec3((1*xLen), tilesize, 1*zLen));
	sendMVP();
	glBindTexture(GL_TEXTURE_2D, textures.at(6));
	drawCube();*/
	Model = glm::translate(Model, glm::vec3(0, -cus - tilesize, 0));
	//Model = glm::rotate(Model, 45.0f, glm::vec3(0, 1, 0));

	Model = glm::scale(Model, glm::vec3(1, tilesize, 1));
	for (int i = 0; i < xLen; i++) {
		for (int j = 0; j < zLen; j++) {

			Model = glm::translate(Model, glm::vec3(0, 0, 2 * cus));
			sendMVP();
			//drawWireCube();

			drawTexCube(tileTextures[5 + (i*(xLen - 1) + j)]);
			//			drawTexCube(tileTextures[15]);


		}
		Model = glm::translate(Model, glm::vec3(2 * cus, 0, 2 * cus*(-zLen)));
	}; //alternativer Code zum Zeichnen von einzelnen Bodenkacheln
	Model = Save;

}
float wallsize = .1;
GLuint backgroundID = 0;
GLuint vertexbuffer = 0;
GLuint uvbuffer = 0;

void createBackground() {


	static const GLfloat backgroundBuffer[] = {
		-1.0f, 1.0f, -0.75f,    -1.0f,-1.0f, -0.75f,          1.0f, -1.0f, -0.75f,
		-1.0f, 1.0f, -0.75f,     1.0f, 1.0f, -0.75f,          1.0f, -1.0f, -0.75f,
	};

	static const GLfloat backgroundImageBuffer[] = {
		0,1,    0,0,   1,0,
		0,1,    1,1,   1,0
	};

	glGenVertexArrays(1, &backgroundID);
	glBindVertexArray(backgroundID);

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundBuffer), backgroundBuffer, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);


	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
		);




	//glGenVertexArrays(1, &backgroundID);
	//glBindVertexArray(backgroundID);

	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundImageBuffer), backgroundImageBuffer, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);


	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(
		1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		2,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
		);


}

void drawBackground() {
	glDisable(GL_DEPTH_TEST);

	if (!backgroundID) {
		createBackground();
	}
	Model = glm::mat4(1.0);
	sendMVP();
	glBindTexture(GL_TEXTURE_2D, textures.at(7));
	glBindVertexArray(backgroundID);
	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 6); // Starting from vertex 0; 3 vertices total -> 1 triangle
	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);
	//glDisableVertexAttribArray(0);
}



void drawWallX() {
	glm::mat4 Save = Model;
	Model = glm::translate(Model, glm::vec3(0, 0, 0));
	Model = glm::rotate(Model, 45.0f, glm::vec3(0, 1, 0));
	Model = glm::scale(Model, glm::vec3(2, 2, 2));
	/*for (int i = 0; i < yLen; i++) {
	for (int j = 0; j < xLen; j++) {
	Model = glm::translate(Model, glm::vec3(2 * cus, 0, 0));
	sendMVP();
	*/
	glBindTexture(GL_TEXTURE_2D, textures.at(1));
	sendMVP();
	drawCube();

	/*}
	Model = glm::translate(Model, glm::vec3(2 * cus*(-xLen), 2 * cus, 0));
	};*/
	Model = Save;

}

struct compareVec3 {
	bool operator() (vec3 const &a, vec3 const &b) {
		if (a.y != b.y) {
			return a.y < b.y;
		}
		else if (a.z != b.z) {
			return a.z < b.z;
		}
		else
			return a.x < b.x;
	}
};

std::set<vec3, compareVec3> completedLines() {
	std::set<glm::vec3, compareVec3> vanishing_cubes;
	int xline = 0;
	int zline = 0;
	int lines = 0;

	for (int y = 1; y <= yLen; y++) {
		for (int x = 1; x <= xLen; x++) {
			for (int z = 1; z <= zLen; z++) {
				if (schacht_array[x - 1][y - 1][z - 1] != transparent) {
					zline += 1;
				}
			}
			if (zline == zLen) {
				lines++;
				for (int i = 1; i <= zLen; i++) {
					vanishing_cubes.insert(glm::vec3(x, y, i));

				}
			}
			zline = 0;

		}
		for (int z = 1; z <= zLen; z++) {
			for (int x = 1; x <= xLen; x++) {
				if (schacht_array[x - 1][y - 1][z - 1] != transparent) {
					xline += 1;
				}
			}
			if (xline == xLen) {
				lines++;
				for (int i = 1; i <= xLen; i++) {
					vanishing_cubes.insert(glm::vec3(i, y, z));
				}
			}
			xline = 0;
		}

	}
	if (vanishing_cubes.empty() == false) {
		blinkFlag = true;
		blinkFlag2 = true;
	}
	int multiplikator = pow(1.5, lines);
	// multiplikator first attempt
	//int multiplikator = lines;
	/*if(lines > 1) {
	for (int i = 2; i <= lines; i++ ) {
	multiplikator *= 1.5;
	}
	}*/
	int points_to_add = (vanishing_cubes.size() / 2) * 10;
	points_to_add *= multiplikator;
	points += points_to_add;
	return vanishing_cubes;
}
timer timer1 = timer();
timer timer2 = timer();
timer timer3 = timer();
/*
�berschreibt gel�schte cubes mit den von oben nachr�ckenden cubes
*/
void deleteCompleteLines(std::set<vec3, compareVec3> delcubes) {
	blinkFlag = false;
	if (!gameover)SoundEngine->play2D("irrKlang-1.5.0/media/yay.mp3", false, false, false);


	old_schacht = schacht_array;
	uebergangs_schacht = schacht_array;

	std::set<vec3, compareVec3>::iterator it1;
	for (it1 = delcubes.begin(); it1 != delcubes.end(); ++it1) {
		uebergangs_schacht[(it1->x) - 1][(it1->y) - 1][(it1->z) - 1] = transparent;
	}

	std::set<vec3, compareVec3>::reverse_iterator it;
	fresh_schacht = schacht_array;
	for (it = delcubes.rbegin(); it != delcubes.rend(); ++it)
	{
		int x = it->x;
		int y = it->y;
		int z = it->z;

		for (int i = y; i < yLen; ++i) {
			fresh_schacht[x - 1][i - 1][z - 1] = fresh_schacht[x - 1][i][z - 1];
		}
		fresh_schacht[x - 1][yLen - 1][z - 1] = transparent;

	}

	timer1.reset();
	timer2.reset();

}
void turningY(bool direction) {
	for (int i = 0; i <= 90; i = i + 2) {
		if (direction) turnY = turnY + i;
		else turnY = turnY - i;
	}
	if (direction)current_perspective = (current_perspective + 1) % 4;
	else {
		if ((current_perspective - 1) % 4 == -1) {
			current_perspective = current_perspective + 3;
		}
		else current_perspective = (current_perspective - 1) % 4;
	};
	std::cout << current_perspective << std::endl;
}

void draw() {								//Zeichnet den Schacht

	glm::mat4 Save = Model;
	Model = glm::translate(Model, glm::vec3(0, -10, 0));
	//	Model = glm::rotate(Model, 45.0f, glm::vec3(0, 1, 0));
	for (int y = 1; y <= yLen; y++) {
		for (int x = 1; x <= xLen; x++) {
			for (int z = 1; z <= zLen; z++) {
				Model = glm::translate(Model, glm::vec3(0, 0, 2 * cus));
				sendMVP();
				vec3* temp = new vec3(x, y, z);

				if (
					*temp == falling->cube_elems[0].koordinate ||
					*temp == falling->cube_elems[1].koordinate ||
					*temp == falling->cube_elems[2].koordinate ||
					*temp == falling->cube_elems[3].koordinate) {
					drawTexCube(falling->cube_elems[0].col);
					//drawCube();
				}
				else if (schacht_array[x - 1][y - 1][z - 1] != transparent) {
					drawTexCube(schacht_array[x - 1][y - 1][z - 1]);
					//drawCube();
					//drawCube(schacht_array[x][y][z].col)
				}
				else {
					//	drawWireCube();
				}
				if (cheating) {
					if (*temp == falling->helpstone.shadow_elems[0].koordinate ||
						*temp == falling->helpstone.shadow_elems[1].koordinate ||
						*temp == falling->helpstone.shadow_elems[2].koordinate ||
						*temp == falling->helpstone.shadow_elems[3].koordinate) {
						glBindTexture(GL_TEXTURE_2D, 0);
						drawWireCube();
					}
				}
				if (!cheating) {
					if (*temp == falling->shadowstone.shadow_elems[0].koordinate ||
						*temp == falling->shadowstone.shadow_elems[1].koordinate ||
						*temp == falling->shadowstone.shadow_elems[2].koordinate ||
						*temp == falling->shadowstone.shadow_elems[3].koordinate) {
						drawShadowCube();
					}
				}
				delete temp;

			}
			Model = glm::translate(Model, glm::vec3(2 * cus, 0, 2 * cus*(-zLen)));
		}

		Model = glm::translate(Model, glm::vec3(2 * cus*(-xLen), 2 * cus, 0));
	};
	//drawCubeUnit(cus);

	Model = Save;

}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

	switch (key)
	{
	case GLFW_KEY_A:
		break;

	case GLFW_KEY_S:
		break;
	case GLFW_KEY_Q:
		turnXg = turnYg = turnZg = turnX = turnY = turnZ = turnX1 = turnY1 = turnZ1 = turnX2 = turnY2 = turnZ2 = zoom = 0.0;
		current_perspective = 0;
		break;
	case GLFW_KEY_3:
		zoom += 2.5;
		break;
	case GLFW_KEY_4:
		zoom -= 2.5;
		break;
	case GLFW_KEY_SPACE:
		turboDrop();
		break;
	}

	if (action == GLFW_PRESS) {
		switch (key) {

		case GLFW_KEY_1:
			turningY(true);
			break;
		case GLFW_KEY_2:
			turningY(false);
			break;
		case GLFW_KEY_W:
			cheating = !cheating;
			break;
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		case GLFW_KEY_RIGHT:
			switch (current_perspective) {
			case 0:
				falling->bewegen('x', 1);
				break;
			case 1:
				falling->bewegen('z', -1);
				break;
			case 2:
				falling->bewegen('x', -1);
				break;
			case 3:
				falling->bewegen('z', 1);
				break;
			}

			break;
		case GLFW_KEY_LEFT:
			switch (current_perspective) {
			case 0:
				falling->bewegen('x', -1);
				break;
			case 1:
				falling->bewegen('z', 1);
				break;
			case 2:
				falling->bewegen('x', 1);
				break;
			case 3:
				falling->bewegen('z', -1);
				break;
			}
			break;
		case GLFW_KEY_UP:
			switch (current_perspective) {
			case 0:
				falling->bewegen('z', -1);
				break;
			case 1:
				falling->bewegen('x', -1);
				break;
			case 2:
				falling->bewegen('z', 1);
				break;
			case 3:
				falling->bewegen('x', 1);
				break;
			}
			break;
		case GLFW_KEY_DOWN:
			switch (current_perspective) {
			case 0:
				falling->bewegen('z', 1);
				break;
			case 1:
				falling->bewegen('x', 1);
				break;
			case 2:
				falling->bewegen('z', -1);
				break;
			case 3:
				falling->bewegen('x', -1);
				break;
			}
			break;

		case GLFW_KEY_E:
			falling->drehen('x', true, false);
			break;
		case GLFW_KEY_R:
			falling->drehen('x', false, false);
			break;
		case GLFW_KEY_D:
			falling->drehen('y', true, false);
			break;
		case GLFW_KEY_F:
			falling->drehen('y', false, false);
			break;
		case GLFW_KEY_C:
			falling->drehen('z', true, false);
			break;
		case GLFW_KEY_V:
			falling->drehen('z', false, false);
			break;

		default:
			break;
		}
	}
}



// Zeichnet einzelne Koordinatensystemachsen
void drawCS(glm::vec3 factor) {
	glm::mat4 Save = Model;
	Model = glm::scale(Model, factor);
	sendMVP();
	drawWireCube();
	Model = Save;
}


int main(void)
{

	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		exit(EXIT_FAILURE);
	}

	// Fehler werden auf stderr ausgegeben, s. o.
	glfwSetErrorCallback(error_callback);

	// Open a window and create its OpenGL context
	// glfwWindowHint vorher aufrufen, um erforderliche Resourcen festzulegen
	GLFWwindow* window = glfwCreateWindow(1024, // Breite
		708,  // Hoehe
		"3D Tetris", // Ueberschrift
		NULL,  // windowed mode
		NULL); // shared windoe

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// Make the window's context current (wird nicht automatisch gemacht)
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	// GLEW erm�glicht Zugriff auf OpenGL-API > 1.1
	glewExperimental = true; // Needed for core profile

	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// alle Texturen im Vector unterbringen
	textures.push_back((GLuint)0);
	textures.push_back(loadBMP_custom("standardtextur.bmp"));
	textures.push_back(loadBMP_custom("tex_rot.bmp"));
	textures.push_back(loadBMP_custom("tex_gruen.bmp"));
	textures.push_back(loadBMP_custom("tex_blau.bmp"));
	textures.push_back(loadBMP_custom("tex_gelb.bmp"));
	textures.push_back(loadBMP_custom("tex_gelb.bmp"));
	textures.push_back(loadBMP_custom("tetrisBckgr2.bmp"));

	// alle TileTextures
	//ttextures.push_back((GLuint)0);
	textures.push_back(loadBMP_custom("tiles/tile1.bmp"));
	textures.push_back(loadBMP_custom("tiles/tile2.bmp"));
	textures.push_back(loadBMP_custom("tiles/tile3.bmp"));
	textures.push_back(loadBMP_custom("tiles/tile4.bmp"));
	textures.push_back(loadBMP_custom("tiles/tile5.bmp"));
	textures.push_back(loadBMP_custom("tiles/tile6.bmp"));
	textures.push_back(loadBMP_custom("tiles/tile7.bmp"));
	textures.push_back(loadBMP_custom("tiles/tile8.bmp"));
	textures.push_back(loadBMP_custom("tiles/tile9.bmp"));
	textures.push_back(loadBMP_custom("tiles/tile10.bmp"));
	textures.push_back(loadBMP_custom("tiles/tile11.bmp"));
	textures.push_back(loadBMP_custom("tiles/tile12.bmp"));
	textures.push_back(loadBMP_custom("tiles/tile13.bmp"));
	textures.push_back(loadBMP_custom("tiles/tile14.bmp"));

	textures.push_back(loadBMP_custom("tiles/tile15.bmp"));

	//HIER GEHTS EIGENTLICH LOS

	glm::vec3 lightPos = glm::vec3(-10, 10, -10);


	// Load the texture
	GLuint Texture = loadBMP_custom("standardtextur.bmp");

	// Auf Keyboard-Events reagieren
	glfwSetKeyCallback(window, key_callback);

	// Dark blue background: R, G, B, Alpha
	// glClearColor(0.5f, 0.1f, 0.8f, 0.5f);
	glClearColor(0.0f, 0.6f, 1.0f, 1.0f);

	// Create and compile our GLSL program from the shaders//	programID = LoadShaders("TransformVertexShader.vertexshader","ColorFragmentShader.fragmentshader");
	programID = LoadShaders("StandardShading.vertexshader", "StandardShading.fragmentshader");

	// Text initialisieren
	initText2D("Holstein.DDS");
	char text[256] = "Punkte: ";

	// Shader auch benutzen !
	glUseProgram(programID);

	glEnable(GL_DEPTH_TEST);	//Aktivierung Tiefenbuffer
	glDepthFunc(GL_LESS);		//kleinster zWert ist n�her an Kamera

	glUniform3f(glGetUniformLocation(programID, "LightPosition_worldspace"), lightPos.x, lightPos.y, lightPos.z);


	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);

	glUniform1i(glGetUniformLocation(programID, "myTextureSampler"), 0);
	//initSchacht();
	falling = new stein(4, color(5));
	time_t start2 = time(0);
	time_t end2 = time(0);
	float mult = 2.;
	double timeStep = 2.;
	srand(time(NULL));
	randTiles();
	SoundEngine = createIrrKlangDevice();
	SoundEngine->play2D("irrKlang-1.5.0/media/music.mp3", GL_TRUE);
	// Eventloop
	while (!glfwWindowShouldClose(window))
	{
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//	std::cout << (float) timer2.getStart()<<"     "<<(double)timer2.duration() << std::endl;

		View = glm::lookAt(glm::vec3(0, 0, 2.5), // Camera is at (0,0,-10), in World Space
			glm::vec3(0, 0, 0),  // and looks at the origin---- bildschirmmitte
			glm::vec3(0, 1, 0)); // Head is up (set to 0,-1,0 to look upside-down) ---- in welche richtung gehts nach oben

		drawBackground();

		//print 2d text
		points_str = std::to_string(points);
		points_char_array = points_str.c_str();

		if (!gameover) {
			printText2D(text, 0, 500, 30);

			printText2D(points_char_array, 60, 460, 30);
		}
		//strncat(text, points_char_array, sizeof(points_char_array)); 

		glUseProgram(programID);

		// Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
		Projection = glm::perspective(25.0f, 4.0f / 3.0f, 0.1f, 150.0f); //Stichpunkt: Frontplane und Backplane

																		 // Camera matrix, ---- hier linksh�ndig (manipulierbar durch erste zeile. wenn positiver z wert dann rechtsh�ndig
		View = glm::lookAt(glm::vec3(40 + zoom, 40, 40 + zoom), // Camera is at (0,0,-10), in World Space
			glm::vec3(0, 0, 0),  // and looks at the origin---- bildschirmmitte
			glm::vec3(0, 1, 0)); // Head is up (set to 0,-1,0 to look upside-down) ---- in welche richtung gehts nach oben


								 // Model matrix : an identity matrix (model will be at the origin) erzeugt 4x4 Einheitsmatrix
		Model = glm::mat4(1.0f);
		Model = glm::rotate(Model, turnY, glm::vec3(0, 1, 0));
		Model = glm::rotate(Model, turnX, glm::vec3(1, 0, 0));
		Model = glm::rotate(Model, turnZ, glm::vec3(0, 0, 1));
		//	Hier wird die Position des Objekts verschoben
		Model = glm::translate(Model, glm::vec3(0, 0, -2 * cus));
		Model = glm::translate(Model, glm::vec3(-xLen *cus + cus, 0, -zLen *cus + cus)); // Hier wird die Position des Objekts verschoben

		glm::mat4 Save = Model;
		//	Model = glm::scale(Model, glm::vec3(1.0 / 1000.0, 1.0 / 1000.0, 1.0 / 1000.0));

		sendMVP();


		drawTiles();
		//	drawWallX();
		draw();

		if (blinkFlag2 == false) {
			if (end2 - start2 > timeStep) {
				start2 = time(0);
				dropStein(-1);
			}
			end2 = time(0);
			completedLines();
		}

		if (blinkFlag) {
			deleteCompleteLines(completedLines());
			//schacht_array = fresh_schacht;
			//blinkFlag2 = false;
		}
		if (blinkFlag2) {
			//start = time(0);
			//end = time(0);
			if (timer1.duration() <= 0.5) {
				std::cout << "null" << "     " << timer1.duration() << std::endl;
				schacht_array = uebergangs_schacht;
				//			timer1.reset();
			}

			else if ((timer1.duration() <= 1.0) && (timer1.duration()>0.5)) {
				//	std::cout << "eins" << "     " << end - start<<std::endl;
				schacht_array = old_schacht;


				//	end = time(0);

			}

			else if ((timer1.duration() <= 1.5) && (timer1.duration()>1.0)) {
				//				std::cout << "zwei" << "     " << end - start << std::endl;
				schacht_array = uebergangs_schacht;
				//			end = time(0);
			}

			else if ((timer1.duration() <= 2.0) && (timer1.duration()>1.5)) {
				//		std::cout << "drei" << "     " << end - start << std::endl;
				schacht_array = old_schacht;
				//	end = time(0);

			}

			else if ((timer1.duration() <= 2.5) && (timer1.duration()>2.0)) {
				//		std::cout << "vier" << "     " << end - start << std::endl;
				schacht_array = uebergangs_schacht;
				//	end = time(0);
			}
			else {
				schacht_array = fresh_schacht;
				//			std::cout << "final" << "     " << end - start << std::endl;
				//	end = time(0);
				//	start = time(0);
				blinkFlag2 = false;
			}


		}


		if (gameover) {
			printText2D("Game Over!", 0, 300, 80);
			printText2D(text, 300, 240, 30);

			printText2D(points_char_array, 300, 200, 30);
		}

		//printText2D(text, 0, 0, 30);
		//glUseProgram(programID);

		// Swap buffers
		glfwSwapBuffers(window);

		// Poll for and process events 
		glfwPollEvents();
	}

	cleanupText2D();
	glDeleteBuffers(1, &vertexbuffer);
	//glDeleteBuffers(1, &normalbuffer);

	glDeleteBuffers(1, &uvbuffer);
	glDeleteTextures(1, &Texture);
	glDeleteProgram(programID);
	delete falling;
	// Close OpenGL window and terminate GLFW
	glfwTerminate();
	return 0;
}

