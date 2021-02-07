
#include <iostream>
#include <gl/glew/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <gl/glm/glm.hpp>
#include<gl\glm\gtc\type_ptr.hpp>
#include<gl\glm\gtx\transform.hpp>

using namespace std;

struct vec3
{
	float x, y, z;
};

struct vertex
{
	vec3 position;
	vec3 color;
};

const GLint WIDTH = 600, HEIGHT = 600;
GLuint VAO, VBO, programId;
GLuint InitShader(const char* vertex_shader_file_name, const char* fragment_shader_file_name);
GLuint modMatLocation, viewMatLoc, projMatLoc;
float theta = 90;

float x = 0.01f, y = 0.01f, z = 0.01f;
float a = 10, b = 28, c = 8.0f / 3.0f;
int index = 0;
float hu = 0;


vector<vertex> vertices;
vector<vec3> randomColors;

void CreateTriangle()
{
	vertices.clear();
	vertices.push_back({ {-1,-1,0}, {0,1,0} });
	vertices.push_back({ {1,-1,0}, {0,0,1} });
	vertices.push_back({ {0,1,0}, {1,0,0} });

	// create VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(vec3), ((char*)(3 * sizeof(vec3))));
	glEnableVertexAttribArray(1);

}

void CreateLorenzeAttractor()
{
	float dt = 0.01f;
	float dx = (a * (y - x)) * dt;
	float dy = (x * (b - z) - y) * dt;
	float dz = (x * y - c * z) * dt;

	x = x + dx;
	y = y + dy;
	z = z + dz;

	vertices.push_back({ {x,y,z},{hu,1 - hu,1 - hu} });

	/*index++;
	if (index >= randomColors.size())
		index = 0;*/

	hu += 0.01f;
	if (hu >= 1)
		hu = 0;
}


void GenerateRandomColors()
{
	randomColors.clear();
	for (float y = 0; y < 100; y++)
	{
		for (float x = 0; x <= 100; x++)
		{
			randomColors.push_back({ (float)(rand() % 255) / 255.0f, (float)(rand() % 255) / 255.0f, (float)(rand() % 255) / 255.0f });
		}
	}
}


void CompileShader()
{
	programId = InitShader("VS.glsl", "FS.glsl");
	glUseProgram(programId);
}

int Init()
{
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		cout << "Error";
		getchar();
		return 1;
	}
	else
	{
		if (GLEW_VERSION_3_0)
			cout << "Driver support OpenGL 3.0\nDetails:\n";
	}
	cout << "\tUsing glew " << glewGetString(GLEW_VERSION) << endl;
	cout << "\tVendor: " << glGetString(GL_VENDOR) << endl;
	cout << "\tRenderer: " << glGetString(GL_RENDERER) << endl;
	cout << "\tVersion: " << glGetString(GL_VERSION) << endl;
	cout << "\tGLSL:" << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

	glGenBuffers(1, &VBO);
	CompileShader();
	GenerateRandomColors();

	modMatLocation = glGetUniformLocation(programId, "modelMat");
	viewMatLoc = glGetUniformLocation(programId, "viewMat");
	projMatLoc = glGetUniformLocation(programId, "projMat");

	glm::mat4 viewMat = glm::lookAt(glm::vec3(0, 0, 50), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 projMat = glm::perspectiveFov(45.f, 20.0f, 20.0f, 0.1f, 100.0f);

	glUniformMatrix4fv(viewMatLoc, 1, GL_FALSE, glm::value_ptr(viewMat));
	glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, glm::value_ptr(projMat));


	glClearColor(0, 0, 0, 1);

	return 0;

}


void Update()
{
	/*theta += 0.001f;
	GLuint thetaUniformLocation = glGetUniformLocation(programId, "theta");
	glUniform1f(thetaUniformLocation, theta);*/
}

void Render()
{
	glClear(GL_COLOR_BUFFER_BIT);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	CreateLorenzeAttractor();

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), vertices.data(), GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(vec3), ((char*)(3 * sizeof(vec3))));
	glEnableVertexAttribArray(1);

	glm::mat4 ModelMat = glm::translate(glm::vec3(0,10,0)) * glm::rotate(theta, glm::vec3(1, 0, 0)) * glm::scale(glm::vec3(0.5f, 0.5f, 0.5f));
	glUniformMatrix4fv(modMatLocation, 1, GL_FALSE, glm::value_ptr(ModelMat));

	glPointSize(2);
	glDrawArrays(GL_POINTS, 0, vertices.size());

}

int main()
{
	sf::Window window(sf::VideoMode(WIDTH, HEIGHT), "Lorenz Attractor!");

	if (Init()) return 1;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{

			case sf::Event::Closed:
				window.close();
				break;
			}
		}

		Update();
		Render();

		window.display();
	}
	return 0;
}