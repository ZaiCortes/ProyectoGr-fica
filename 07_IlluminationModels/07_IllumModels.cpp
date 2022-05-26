/*
* Laboratorio de Computación Gráfica e Interacción Humano-Computadora
* 06 - Modelos de Iluminación
*/

#include <iostream>
#include <stdlib.h>

// GLAD: Multi-Language GL/GLES/EGL/GLX/WGL Loader-Generator
// https://glad.dav1d.de/
#include <glad/glad.h>

// GLFW: https://www.glfw.org/
#include <GLFW/glfw3.h>

// GLM: OpenGL Math library
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Model loading classes
#include <shader_m.h>
#include <camera.h>
#include <model.h>
#include <material.h>
#include <light.h>
#include <cubemap.h>


// Max number of bones
#define MAX_RIGGING_BONES 100

// Functions
bool Start();
bool Update();

// Definición de callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// Gobals
GLFWwindow* window;

// Tamaño en pixeles de la ventana
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

// Definición de cámara (posición en XYZ)
Camera camera(glm::vec3(33.0f, 3.5f, 0.0f));
Camera camera3rd(glm::vec3(33.0f, 3.5f, 0.0f));

// Controladores para el movimiento del mouse
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Variables para la velocidad de reproducción
// de la animación
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float elapsedTime = 0.0f;
glm::vec3 positionChar(-17.0f, 0.0f, -32.0f);
glm::vec3 position(33.0f, 0.0f, 0.0f);
glm::vec3 forwardView(0.0f, 0.0f, 1.0f);
float     scaleV = 0.05f;

float     rotateCharacter = 0.0f;
float     rotateCharacter1 = 0.0f;
float     rotateDino1 = 0.0f;
float	  rotatePuerta1 = 0.0f;
float	  rotatePuerta2 = 0.0f;
float	  rotatePuerta3 = 0.0f;
float	  rotatePuerta4 = 0.0f;
float	  rotatePuerta5 = 0.0f;
float	  rotatePuerta6 = 0.0f;
float	  rotateMesa = 0.0f;

glm::vec3 tamanoTric(1.0f, 1.0f, 1.0f);
glm::vec3 positionTable(0.0f, 0.0f, 0.0f);
glm::vec3 boundingboxMin(-38.0f, 0.0f, -39.0f);
glm::vec3 boundingboxMax(39.0f, 0.0f, 39.0f);

// Shaders
Shader* ourShader;
Shader* museoShader;
Shader* phongShader;
Shader* phongPuertas;
Shader* fresnelShader;
Shader* cubemapShader;

// Carga la información del modelo
Model* character;
Model* character1;
Model* museo;
Model* dinosaurio;
Model* huevosGrandes;
Model* huevosPeques;
Model* huella;
Model* triceratops;
Model* trexveloci;
Model* techo;
Model* puerta;
Model* habitacion;
Model* vidrio;
Model* cuchara;
Model* cactus;

float tradius = 3.0f;
float theta = 0.0f;
float alpha = 0.0f;

float rotac = 0.0f;


// Cubemap
CubeMap* mainCubeMap;

// Materiales
Material material01;
Light    light01;
Light	 light02;
Light	 light03;
Light	 light04;

// Pose inicial del modelo
glm::mat4 gBones[MAX_RIGGING_BONES];
glm::mat4 gBonesChar[MAX_RIGGING_BONES];
glm::mat4 gBonesBar[MAX_RIGGING_BONES];

float	fps = 0.0f, fpsBar = 0.0f, fps1 = 0.0f;
int		keys = 0, keysBar = 0, keys1 = 0;
int		animationCount = 0, animationCountBar = 0, animationCountChar = 0;

glm::vec3 force(0.0f, 0.0f, 0.0f);
glm::vec3 acceleration(0.0f, 0.0f, 0.0f);
glm::vec3 velocity(0.0f, 0.0f, 0.0f);
glm::vec3 gravity(0.0f, -9.81f, 0.0f);
float mass = 1.0f;


//selección de camara
bool	activeCamera = 0; //Primera camara
// Entrada a función principal
int main()
{
	if (!Start())
		return -1;

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		if (!Update())
			break;
	}

	glfwTerminate();
	return 0;

}

bool Start() {
	// Inicialización de GLFW

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Creación de la ventana con GLFW
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "MUSEO DE DINOSAURIOS", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);



	// glad: Cargar todos los apuntadores
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	// Activación de buffer de profundidad
	glEnable(GL_DEPTH_TEST);

	// Compilación y enlace de shaders
	ourShader = new Shader("shaders/10_vertex_skinning-IT.vs", "shaders/10_fragment_skinning-IT.fs");
	museoShader = new Shader("shaders/10_vertex_simple.vs", "shaders/10_fragment_simple.fs");
	phongShader = new Shader("shaders/11_BasicPhongShader.vs", "shaders/11_BasicPhongShader.fs");
	phongPuertas = new Shader("shaders/11_BasicPhongShader.vs", "shaders/11_BasicPhongShader.fs");
	fresnelShader = new Shader("shaders/11_Fresnel.vs", "shaders/11_Fresnel.fs");
	cubemapShader = new Shader("shaders/10_vertex_cubemap.vs", "shaders/10_fragment_cubemap.fs");

	// Máximo número de huesos: 100
	ourShader->setBonesIDs(MAX_RIGGING_BONES);

	habitacion = new Model("labproyecto/Habitacion.fbx");
	vidrio = new Model("labproyecto/Entrada.fbx");
	cuchara = new Model("labproyecto/Cuchara.fbx");
	museo = new Model("labproyecto/museoNormal.fbx");
	character = new Model("labproyecto/personaje1.fbx");
	character1 = new Model("labproyecto/pensando.fbx");
	dinosaurio = new Model("labproyecto/Parasaurolophus2.fbx");
	huevosGrandes = new Model("labproyecto/huevosGrandes.fbx");
	huevosPeques = new Model("labproyecto/huevos.fbx");
	huella = new Model("labproyecto/Huella.fbx");
	triceratops = new Model("labproyecto/triceratops.fbx");
	triceratops = new Model("labproyecto/triceratops.fbx");
	trexveloci = new Model("labproyecto/trexveloci.fbx");
	techo = new Model("labproyecto/techo.fbx");
	puerta = new Model("labproyecto/Puerta.fbx");
	cactus = new Model("labproyecto/Cactus.fbx");
	// Cubemap
	vector<std::string> faces
	{
		"textures/cubemap/03/posx.png",
		"textures/cubemap/03/negx.png",
		"textures/cubemap/03/posy.png",
		"textures/cubemap/03/negy.png",
		"textures/cubemap/03/posz.png",
		"textures/cubemap/03/negz.png"
	};
	mainCubeMap = new CubeMap();
	mainCubeMap->loadCubemap(faces);

	// time, arrays
	character->SetPose(0.0f, gBones);
	character1->SetPose(0.0f, gBonesChar);

	fps = (float)character->getFramerate();
	keys = (int)character->getNumFrames();

	fps1 = (float)character1->getFramerate();
	keys1 = (int)character1->getNumFrames();


	camera3rd.Position = position;
	camera3rd.Position.y += 4.02f;
	camera3rd.Position -= forwardView;
	camera3rd.Front = forwardView;

	return true;
}

bool Update() {
	// Cálculo del framerate
	float currentFrame = (float)glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	elapsedTime += deltaTime;
	if (elapsedTime > 1.0f / fps) {
		animationCount++;
		if (animationCount > keys - 1) {
			animationCount = 0;
		}
		animationCountChar++;
		if (animationCountChar > keys1 - 1) {
			animationCountChar = 0;
		}
		// Configuración de la pose en el instante t
		character->SetPose((float)animationCount, gBones);
		character1->SetPose((float)animationCountChar, gBonesChar); //Recorrido 
		elapsedTime = 0.0f;
	}

	// Procesa la entrada del teclado o mouse
	processInput(window);

	// Renderizado R - G - B - A
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Cubemap (fondo)
	{
		glm::mat4 projection;
		glm::mat4 view;
		if (activeCamera) {
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
			view = camera.GetViewMatrix();
		}
		else {
			projection = glm::perspective(glm::radians(camera3rd.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
			view = camera3rd.GetViewMatrix();
		}
		mainCubeMap->drawCubeMap(*cubemapShader, projection, view);
	}

	//Cucharas
	{
		// Activamos el shader de Phong
		phongShader->use();

		// Activamos para objetos transparentes
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// Aplicamos transformaciones de proyección y cámara (si las hubiera)
		glm::mat4 projection;
		glm::mat4 view;
		if (activeCamera) {
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
			view = camera.GetViewMatrix();
		}
		else {
			projection = glm::perspective(glm::radians(camera3rd.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
			view = camera3rd.GetViewMatrix();
		}
		phongShader->setMat4("projection", projection);
		phongShader->setMat4("view", view);

		light01.Position = glm::vec3(30.0f, 9.8f, -30.0f);
		light01.Direction = glm::vec3(1.0f, 1.0f, 1.0f);
		light01.Power = glm::vec4(60.0f, 60.0f, 60.0f, 1.0f);
		light01.alphaIndex = 10.0;
		light01.Color = glm::vec4(1.0f, 1.0f, 1.0, 1.0f);
		light01.distance = 6.0f;

		phongShader->setVec4("LightColor", light01.Color);
		phongShader->setVec4("LightPower", light01.Power);
		phongShader->setInt("alphaIndex", light01.alphaIndex);
		phongShader->setFloat("distance", light01.distance);
		phongShader->setVec3("lightPosition", light01.Position);
		phongShader->setVec3("lightDirection", light01.Direction);

		// Configuramos propiedades de fuentes de luz (Luz02)
		phongShader->setVec4("LightColor2", glm::vec4(1.0f, 1.0f, 1.0, 1.0f));
		phongShader->setVec4("LightPower2", glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
		phongShader->setInt("alphaIndex2", 10.0);
		phongShader->setFloat("distance2", 6.0f);
		phongShader->setVec3("lightPosition2", glm::vec3(-25.0f, 9.8f, -15.0f));
		phongShader->setVec3("lightDirection2", glm::vec3(1.0f, 1.0f, 1.0f));

		phongShader->setVec4("LightColor3", glm::vec4(1.0f, 1.0f, 1.0, 1.0f));
		phongShader->setVec4("LightPower3", glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
		phongShader->setInt("alphaIndex3", 10.0);
		phongShader->setFloat("distance3", 6.0f);
		phongShader->setVec3("lightPosition3", glm::vec3(-25.0f, 9.8f, -15.0f));
		phongShader->setVec3("lightDirection3", glm::vec3(1.0f, 1.0f, 1.0f));

		phongShader->setVec4("LightColor4", glm::vec4(1.0f, 1.0f, 1.0, 1.0f));
		phongShader->setVec4("LightPower4", glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
		phongShader->setInt("alphaIndex4", 10.0);
		phongShader->setFloat("distance4", 6.0f);
		phongShader->setVec3("lightPosition4", glm::vec3(-25.0f, 9.8f, -15.0f));
		phongShader->setVec3("lightDirection4", glm::vec3(1.0f, 1.0f, 1.0f));


		// Aplicamos propiedades materiales
		phongShader->setVec4("MaterialAmbientColor", glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
		phongShader->setVec4("MaterialDiffuseColor", glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
		phongShader->setVec4("MaterialSpecularColor", glm::vec4(0.7f, 0.7f, 0.7f, 1.0f));
		phongShader->setFloat("transparency", material01.transparency);

		// Aplicamos transformaciones del modelo
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(19.0f, 1.2f, -24.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotateMesa), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		phongShader->setMat4("model", model);
		cuchara->Draw(*phongShader);

		glm::mat4 model2 = glm::mat4(1.0f);
		model2 = glm::translate(model2, glm::vec3(19.0f, 1.2f, -34.0f));
		model2 = glm::rotate(model2, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model2 = glm::rotate(model2, glm::radians(rotateMesa), glm::vec3(0.0f, 0.0f, 1.0f));
		model2 = glm::scale(model2, glm::vec3(0.5f, 0.5f, 0.5f));
		phongShader->setMat4("model", model2);
		cuchara->Draw(*phongShader);

		glm::mat4 model3 = glm::mat4(1.0f);
		model3 = glm::translate(model3, glm::vec3(28.0f, 1.2f, -24.0f));
		model3 = glm::rotate(model3, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model3 = glm::rotate(model3, glm::radians(rotateMesa), glm::vec3(0.0f, 0.0f, 1.0f));
		model3 = glm::scale(model3, glm::vec3(0.5f, 0.5f, 0.5f));
		phongShader->setMat4("model", model3);
		cuchara->Draw(*phongShader);


		glm::mat4 model4 = glm::mat4(1.0f);
		model4 = glm::translate(model4, glm::vec3(28.0f, 1.2f, -34.0f));
		model4 = glm::rotate(model4, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model4 = glm::rotate(model4, glm::radians(rotateMesa), glm::vec3(0.0f, 0.0f, 1.0f));
		model4 = glm::scale(model4, glm::vec3(0.5f, 0.5f, 0.5f));
		phongShader->setMat4("model", model4);
		cuchara->Draw(*phongShader);


		glm::mat4 model5 = glm::mat4(1.0f);
		model5 = glm::translate(model5, glm::vec3(35.0f, 1.2f, -24.0f));
		model5 = glm::rotate(model5, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model5 = glm::rotate(model5, glm::radians(rotateMesa), glm::vec3(0.0f, 0.0f, 1.0f));
		model5 = glm::scale(model5, glm::vec3(0.5f, 0.5f, 0.5f));
		phongShader->setMat4("model", model5);
		cuchara->Draw(*phongShader);

		glm::mat4 model6 = glm::mat4(1.0f);
		model6 = glm::translate(model6, glm::vec3(35.0f, 1.2f, -34.0f));
		model6 = glm::rotate(model6, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model6 = glm::rotate(model6, glm::radians(rotateMesa), glm::vec3(0.0f, 0.0f, 1.0f));
		model6 = glm::scale(model6, glm::vec3(0.5f, 0.5f, 0.5f));
		phongShader->setMat4("model", model6);
		cuchara->Draw(*phongShader);


	}glUseProgram(0);


	//Mesa
	{
		// Activamos el shader de Phong
		phongShader->use();

		// Activamos para objetos transparentes
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// Aplicamos transformaciones de proyección y cámara (si las hubiera)
		glm::mat4 projection;
		glm::mat4 view;
		if (activeCamera) {
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
			view = camera.GetViewMatrix();
		}
		else {
			projection = glm::perspective(glm::radians(camera3rd.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
			view = camera3rd.GetViewMatrix();
		}
		phongShader->setMat4("projection", projection);
		phongShader->setMat4("view", view);

		light01.Position = glm::vec3(30.0f, 9.8f, -30.0f);
		light01.Direction = glm::vec3(1.0f, 1.0f, 1.0f);
		light01.Power = glm::vec4(60.0f, 60.0f, 60.0f, 1.0f);
		light01.alphaIndex = 10.0;
		light01.Color = glm::vec4(1.0f, 1.0f, 1.0, 1.0f);
		light01.distance = 6.0f;

		phongShader->setVec4("LightColor", light01.Color);
		phongShader->setVec4("LightPower", light01.Power);
		phongShader->setInt("alphaIndex", light01.alphaIndex);
		phongShader->setFloat("distance", light01.distance);
		phongShader->setVec3("lightPosition", light01.Position);
		phongShader->setVec3("lightDirection", light01.Direction);
		//phongShader->setVec3("eye", camera.Position);

		// Configuramos propiedades de fuentes de luz (Luz02)
		phongShader->setVec4("LightColor2", glm::vec4(1.0f, 1.0f, 1.0, 1.0f));
		phongShader->setVec4("LightPower2", glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
		phongShader->setInt("alphaIndex2", 10.0);
		phongShader->setFloat("distance2", 6.0f);
		phongShader->setVec3("lightPosition2", glm::vec3(-25.0f, 9.8f, -15.0f));
		phongShader->setVec3("lightDirection2", glm::vec3(1.0f, 1.0f, 1.0f));

		phongShader->setVec4("LightColor3", glm::vec4(1.0f, 1.0f, 1.0, 1.0f));
		phongShader->setVec4("LightPower3", glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
		phongShader->setInt("alphaIndex3", 10.0);
		phongShader->setFloat("distance3", 6.0f);
		phongShader->setVec3("lightPosition3", glm::vec3(-25.0f, 9.8f, -15.0f));
		phongShader->setVec3("lightDirection3", glm::vec3(1.0f, 1.0f, 1.0f));

		phongShader->setVec4("LightColor4", glm::vec4(1.0f, 1.0f, 1.0, 1.0f));
		phongShader->setVec4("LightPower4", glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
		phongShader->setInt("alphaIndex4", 10.0);
		phongShader->setFloat("distance4", 6.0f);
		phongShader->setVec3("lightPosition4", glm::vec3(-25.0f, 9.8f, -15.0f));
		phongShader->setVec3("lightDirection4", glm::vec3(1.0f, 1.0f, 1.0f));


		// Aplicamos propiedades materiales
		phongShader->setVec4("MaterialAmbientColor", material01.ambient);
		phongShader->setVec4("MaterialDiffuseColor", material01.diffuse);
		phongShader->setVec4("MaterialSpecularColor", material01.specular);
		phongShader->setFloat("transparency", material01.transparency);

		// Aplicamos transformaciones del modelo
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(19.0f, 0.0f, -25.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotateMesa), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.2f, 1.2f, 1.2f));
		phongShader->setMat4("model", model);
		habitacion->Draw(*phongShader);

		glm::mat4 model2 = glm::mat4(1.0f);
		model2 = glm::translate(model2, glm::vec3(19.0f, 0.0f, -35.0f));
		model2 = glm::rotate(model2, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model2 = glm::rotate(model2, glm::radians(rotateMesa), glm::vec3(0.0f, 0.0f, 1.0f));
		model2 = glm::scale(model2, glm::vec3(1.2f, 1.2f, 1.2f));
		phongShader->setMat4("model", model2);
		habitacion->Draw(*phongShader);

		glm::mat4 model3 = glm::mat4(1.0f);
		model3 = glm::translate(model3, glm::vec3(28.0f, 0.0f, -25.0f));
		model3 = glm::rotate(model3, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model3 = glm::rotate(model3, glm::radians(rotateMesa), glm::vec3(0.0f, 0.0f, 1.0f));
		model3 = glm::scale(model3, glm::vec3(1.2f, 1.2f, 1.2f));
		phongShader->setMat4("model", model3);
		habitacion->Draw(*phongShader);


		glm::mat4 model4 = glm::mat4(1.0f);
		model4 = glm::translate(model4, glm::vec3(28.0f, 0.0f, -35.0f));
		model4 = glm::rotate(model4, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model4 = glm::rotate(model4, glm::radians(rotateMesa), glm::vec3(0.0f, 0.0f, 1.0f));
		model4 = glm::scale(model4, glm::vec3(1.2f, 1.2f, 1.2f));
		phongShader->setMat4("model", model4);
		habitacion->Draw(*phongShader);


		glm::mat4 model5 = glm::mat4(1.0f);
		model5 = glm::translate(model5, glm::vec3(35.0f, 0.0f, -25.0f));
		model5 = glm::rotate(model5, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model5 = glm::rotate(model5, glm::radians(rotateMesa), glm::vec3(0.0f, 0.0f, 1.0f));
		model5 = glm::scale(model5, glm::vec3(1.2f, 1.2f, 1.2f));
		phongShader->setMat4("model", model5);
		habitacion->Draw(*phongShader);

		glm::mat4 model6 = glm::mat4(1.0f);
		model6 = glm::translate(model6, glm::vec3(35.0f, 0.0f, -35.0f));
		model6 = glm::rotate(model6, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model6 = glm::rotate(model6, glm::radians(rotateMesa), glm::vec3(0.0f, 0.0f, 1.0f));
		model6 = glm::scale(model6, glm::vec3(1.2f, 1.2f, 1.2f));
		phongShader->setMat4("model", model6);
		habitacion->Draw(*phongShader);


	}glUseProgram(0);


	//Parasaurolophus
	{
		// Activamos el shader de Phong
		phongShader->use();

		// Activamos para objetos transparentes
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// Aplicamos transformaciones de proyección y cámara (si las hubiera)
		glm::mat4 projection;
		glm::mat4 view;
		if (activeCamera) {
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
			view = camera.GetViewMatrix();
		}
		else {
			projection = glm::perspective(glm::radians(camera3rd.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
			view = camera3rd.GetViewMatrix();
		}
		phongShader->setMat4("projection", projection);
		phongShader->setMat4("view", view);

		// Aplicamos transformaciones del modelo
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-25.0f, 1.0f, -20.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotateDino1), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		phongShader->setMat4("model", model);


		light01.Position = glm::vec3(30.0f, 9.8f, -30.0f);
		light01.Direction = glm::vec3(1.0f, 1.0f, 1.0f);
		light01.Power = glm::vec4(60.0f, 60.0f, 60.0f, 1.0f);
		light01.alphaIndex = 10.0;
		light01.Color = glm::vec4(1.0f, 1.0f, 1.0, 1.0f);
		light01.distance = 6.0f;

		phongShader->setVec4("LightColor", light01.Color);
		phongShader->setVec4("LightPower", light01.Power);
		phongShader->setInt("alphaIndex", light01.alphaIndex);
		phongShader->setFloat("distance", light01.distance);
		phongShader->setVec3("lightPosition", light01.Position);
		phongShader->setVec3("lightDirection", light01.Direction);
		//phongShader->setVec3("eye", camera.Position);

		// Configuramos propiedades de fuentes de luz (Luz02)
		phongShader->setVec4("LightColor2", glm::vec4(1.0f, 1.0f, 1.0, 1.0f));
		phongShader->setVec4("LightPower2", glm::vec4(50.0f, 50.0f, 50.0f, 1.0f));
		phongShader->setInt("alphaIndex2", 10.0);
		phongShader->setFloat("distance2", 6.0f);
		phongShader->setVec3("lightPosition2", glm::vec3(-30.0f, 9.8f, -30.0f));
		phongShader->setVec3("lightDirection2", glm::vec3(1.0f, 1.0f, 1.0f));

		phongShader->setVec4("LightColor3", glm::vec4(1.0f, 1.0f, 1.0, 1.0f));
		phongShader->setVec4("LightPower3", glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
		phongShader->setInt("alphaIndex3", 10.0);
		phongShader->setFloat("distance3", 6.0f);
		phongShader->setVec3("lightPosition3", glm::vec3(-25.0f, 9.8f, -15.0f));
		phongShader->setVec3("lightDirection3", glm::vec3(1.0f, 1.0f, 1.0f));

		phongShader->setVec4("LightColor4", glm::vec4(1.0f, 1.0f, 1.0, 1.0f));
		phongShader->setVec4("LightPower4", glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
		phongShader->setInt("alphaIndex4", 10.0);
		phongShader->setFloat("distance4", 6.0f);
		phongShader->setVec3("lightPosition4", glm::vec3(-25.0f, 9.8f, -15.0f));
		phongShader->setVec3("lightDirection4", glm::vec3(1.0f, 1.0f, 1.0f));


		// Aplicamos propiedades materiales
		phongShader->setVec4("MaterialAmbientColor", material01.ambient);
		phongShader->setVec4("MaterialDiffuseColor", material01.diffuse);
		phongShader->setVec4("MaterialSpecularColor", material01.specular);
		phongShader->setFloat("transparency", material01.transparency);

		dinosaurio->Draw(*phongShader);

	}glUseProgram(0);

	//Huevos Grandes
	{
		// Activamos el shader de Phong
		phongShader->use();

		// Activamos para objetos transparentes
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// Aplicamos transformaciones de proyección y cámara (si las hubiera)
		glm::mat4 projection;
		glm::mat4 view;
		if (activeCamera) {
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
			view = camera.GetViewMatrix();
		}
		else {
			projection = glm::perspective(glm::radians(camera3rd.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
			view = camera3rd.GetViewMatrix();
		}
		phongShader->setMat4("projection", projection);
		phongShader->setMat4("view", view);

		// Aplicamos transformaciones del modelo
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-25.0f, 1.0f, -15.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotateDino1), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		phongShader->setMat4("model", model);

		light01.Position = glm::vec3(-24.0f, 9.8f, -10.0f);
		light01.Direction = glm::vec3(1.0f, 1.0f, 1.0f);
		light01.Power = glm::vec4(60.0f, 60.0f, 60.0f, 1.0f);
		light01.alphaIndex = 10.0;
		light01.Color = glm::vec4(1.0f, 1.0f, 1.0, 1.0f);
		light01.distance = 6.0f;

		phongShader->setVec4("LightColor", light01.Color);
		phongShader->setVec4("LightPower", light01.Power);
		phongShader->setInt("alphaIndex", light01.alphaIndex);
		phongShader->setFloat("distance", light01.distance);
		phongShader->setVec3("lightPosition", light01.Position);
		phongShader->setVec3("lightDirection", light01.Direction);
		//phongShader->setVec3("eye", camera.Position);

		// Configuramos propiedades de fuentes de luz (Luz02)
		phongShader->setVec4("LightColor2", glm::vec4(1.0f, 1.0f, 1.0, 1.0f));
		phongShader->setVec4("LightPower2", glm::vec4(50.0f, 50.0f, 50.0f, 1.0f));
		phongShader->setInt("alphaIndex2", 10.0);
		phongShader->setFloat("distance2", 6.0f);
		phongShader->setVec3("lightPosition2", glm::vec3(-24.0f, 9.8f, 10.0f));
		phongShader->setVec3("lightDirection2", glm::vec3(1.0f, 1.0f, 1.0f));

		phongShader->setVec4("LightColor3", glm::vec4(1.0f, 1.0f, 1.0, 1.0f));
		phongShader->setVec4("LightPower3", glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
		phongShader->setInt("alphaIndex3", 10.0);
		phongShader->setFloat("distance3", 6.0f);
		phongShader->setVec3("lightPosition3", glm::vec3(-25.0f, 9.8f, -15.0f));
		phongShader->setVec3("lightDirection3", glm::vec3(1.0f, 1.0f, 1.0f));

		phongShader->setVec4("LightColor4", glm::vec4(1.0f, 1.0f, 1.0, 1.0f));
		phongShader->setVec4("LightPower4", glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
		phongShader->setInt("alphaIndex4", 10.0);
		phongShader->setFloat("distance4", 6.0f);
		phongShader->setVec3("lightPosition4", glm::vec3(-25.0f, 9.8f, -15.0f));
		phongShader->setVec3("lightDirection4", glm::vec3(1.0f, 1.0f, 1.0f));


		// Aplicamos propiedades materiales
		phongShader->setVec4("MaterialAmbientColor", material01.ambient);
		phongShader->setVec4("MaterialDiffuseColor", material01.diffuse);
		phongShader->setVec4("MaterialSpecularColor", material01.specular);
		phongShader->setFloat("transparency", material01.transparency);

		huevosGrandes->Draw(*phongShader);

	}glUseProgram(0);

	//Fosil Huella
	{
		// Activamos el shader de Phong
		phongShader->use();

		// Activamos para objetos transparentes
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// Aplicamos transformaciones de proyección y cámara (si las hubiera)
		glm::mat4 projection;
		glm::mat4 view;
		if (activeCamera) {
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
			view = camera.GetViewMatrix();
		}
		else {
			projection = glm::perspective(glm::radians(camera3rd.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
			view = camera3rd.GetViewMatrix();
		}
		phongShader->setMat4("projection", projection);
		phongShader->setMat4("view", view);

		// Aplicamos transformaciones del modelo
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-25.6f, 2.4f, 17.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(rotateDino1), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 0.4f));
		phongShader->setMat4("model", model);

		light01.Position = glm::vec3(-24.0f, 9.8f, -10.0f);
		light01.Direction = glm::vec3(1.0f, 1.0f, 1.0f);
		light01.Power = glm::vec4(60.0f, 60.0f, 60.0f, 1.0f);
		light01.alphaIndex = 10.0;
		light01.Color = glm::vec4(1.0f, 1.0f, 1.0, 1.0f);
		light01.distance = 10.0f;

		phongShader->setVec4("LightColor", light01.Color);
		phongShader->setVec4("LightPower", light01.Power);
		phongShader->setInt("alphaIndex", light01.alphaIndex);
		phongShader->setFloat("distance", light01.distance);
		phongShader->setVec3("lightPosition", light01.Position);
		phongShader->setVec3("lightDirection", light01.Direction);
		//phongShader->setVec3("eye", camera.Position);

		// Configuramos propiedades de fuentes de luz (Luz02)
		phongShader->setVec4("LightColor2", glm::vec4(1.0f, 1.0f, 1.0, 1.0f));
		phongShader->setVec4("LightPower2", glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
		phongShader->setInt("alphaIndex2", 10.0);
		phongShader->setFloat("distance2", 6.0f);
		phongShader->setVec3("lightPosition2", glm::vec3(-25.0f, 9.8f, -15.0f));
		phongShader->setVec3("lightDirection2", glm::vec3(1.0f, 1.0f, 1.0f));

		phongShader->setVec4("LightColor3", glm::vec4(1.0f, 1.0f, 1.0, 1.0f));
		phongShader->setVec4("LightPower3", glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
		phongShader->setInt("alphaIndex3", 10.0);
		phongShader->setFloat("distance3", 6.0f);
		phongShader->setVec3("lightPosition3", glm::vec3(-25.0f, 9.8f, -15.0f));
		phongShader->setVec3("lightDirection3", glm::vec3(1.0f, 1.0f, 1.0f));

		phongShader->setVec4("LightColor4", glm::vec4(1.0f, 1.0f, 1.0, 1.0f));
		phongShader->setVec4("LightPower4", glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
		phongShader->setInt("alphaIndex4", 10.0);
		phongShader->setFloat("distance4", 6.0f);
		phongShader->setVec3("lightPosition4", glm::vec3(-25.0f, 9.8f, -15.0f));
		phongShader->setVec3("lightDirection4", glm::vec3(1.0f, 1.0f, 1.0f));


		// Aplicamos propiedades materiales
		phongShader->setVec4("MaterialAmbientColor", material01.ambient);
		phongShader->setVec4("MaterialDiffuseColor", material01.diffuse);
		phongShader->setVec4("MaterialSpecularColor", material01.specular);
		phongShader->setFloat("transparency", material01.transparency);

		// Aplicamos propiedades materiales
		phongShader->setVec4("MaterialAmbientColor", material01.ambient);
		phongShader->setVec4("MaterialDiffuseColor", material01.diffuse);
		phongShader->setVec4("MaterialSpecularColor", material01.specular);
		phongShader->setFloat("transparency", material01.transparency);

		huella->Draw(*phongShader);

	}glUseProgram(0);

	//Huevos pequeños
	{
		// Activamos el shader de Phong
		phongShader->use();

		// Activamos para objetos transparentes
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// Aplicamos transformaciones de proyección y cámara (si las hubiera)
		glm::mat4 projection;
		glm::mat4 view;
		if (activeCamera) {
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
			view = camera.GetViewMatrix();
		}
		else {
			projection = glm::perspective(glm::radians(camera3rd.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
			view = camera3rd.GetViewMatrix();
		}
		phongShader->setMat4("projection", projection);
		phongShader->setMat4("view", view);

		// Aplicamos transformaciones del modelo
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-25.0f, 1.5f, 23.0f));//-25.0f, 2.0f, 23.0f
		model = glm::rotate(model, glm::radians(-105.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		phongShader->setMat4("model", model);

		light01.Position = glm::vec3(-24.0f, 9.8f, -10.0f);
		light01.Direction = glm::vec3(1.0f, 1.0f, 1.0f);
		light01.Power = glm::vec4(60.0f, 60.0f, 60.0f, 1.0f);
		light01.alphaIndex = 10.0;
		light01.Color = glm::vec4(1.0f, 1.0f, 1.0, 1.0f);
		light01.distance = 10.0f;

		phongShader->setVec4("LightColor", light01.Color);
		phongShader->setVec4("LightPower", light01.Power);
		phongShader->setInt("alphaIndex", light01.alphaIndex);
		phongShader->setFloat("distance", light01.distance);
		phongShader->setVec3("lightPosition", light01.Position);
		phongShader->setVec3("lightDirection", light01.Direction);
		//phongShader->setVec3("eye", camera.Position);

		// Configuramos propiedades de fuentes de luz (Luz02)
		phongShader->setVec4("LightColor2", glm::vec4(1.0f, 1.0f, 1.0, 1.0f));
		phongShader->setVec4("LightPower2", glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
		phongShader->setInt("alphaIndex2", 10.0);
		phongShader->setFloat("distance2", 6.0f);
		phongShader->setVec3("lightPosition2", glm::vec3(-25.0f, 9.8f, -15.0f));
		phongShader->setVec3("lightDirection2", glm::vec3(1.0f, 1.0f, 1.0f));

		phongShader->setVec4("LightColor3", glm::vec4(1.0f, 1.0f, 1.0, 1.0f));
		phongShader->setVec4("LightPower3", glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
		phongShader->setInt("alphaIndex3", 10.0);
		phongShader->setFloat("distance3", 6.0f);
		phongShader->setVec3("lightPosition3", glm::vec3(-25.0f, 9.8f, -15.0f));
		phongShader->setVec3("lightDirection3", glm::vec3(1.0f, 1.0f, 1.0f));

		phongShader->setVec4("LightColor4", glm::vec4(1.0f, 1.0f, 1.0, 1.0f));
		phongShader->setVec4("LightPower4", glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
		phongShader->setInt("alphaIndex4", 10.0);
		phongShader->setFloat("distance4", 6.0f);
		phongShader->setVec3("lightPosition4", glm::vec3(-25.0f, 9.8f, -15.0f));
		phongShader->setVec3("lightDirection4", glm::vec3(1.0f, 1.0f, 1.0f));


		// Aplicamos propiedades materiales
		phongShader->setVec4("MaterialAmbientColor", material01.ambient);
		phongShader->setVec4("MaterialDiffuseColor", material01.diffuse);
		phongShader->setVec4("MaterialSpecularColor", material01.specular);
		phongShader->setFloat("transparency", material01.transparency);

		// Aplicamos propiedades materiales
		phongShader->setVec4("MaterialAmbientColor", material01.ambient);
		phongShader->setVec4("MaterialDiffuseColor", material01.diffuse);
		phongShader->setVec4("MaterialSpecularColor", material01.specular);
		phongShader->setFloat("transparency", material01.transparency);

		huevosPeques->Draw(*phongShader);

	}glUseProgram(0);

	//Triceratops
	{
		// Activamos el shader de Phong
		phongShader->use();

		// Activamos para objetos transparentes
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// Aplicamos transformaciones de proyección y cámara (si las hubiera)
		glm::mat4 projection;
		glm::mat4 view;
		if (activeCamera) {
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
			view = camera.GetViewMatrix();
		}
		else {
			projection = glm::perspective(glm::radians(camera3rd.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
			view = camera3rd.GetViewMatrix();
		}
		phongShader->setMat4("projection", projection);
		phongShader->setMat4("view", view);

		// Aplicamos transformaciones del modelo
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(5.0f, 0.75f, 22.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::scale(model, tamanoTric);
		phongShader->setMat4("model", model);

		light01.Position = glm::vec3(30.0f, 9.8f, 30.0f);
		light01.Direction = glm::vec3(1.0f, 1.0f, 1.0f);
		light01.Power = glm::vec4(100.0f, 100.0f, 100.0f, 1.0f);
		light01.alphaIndex = 10.0;
		light01.Color = glm::vec4(1.0f, 1.0f, 1.0, 1.0f);
		light01.distance = 10.0f;

		light02.Position = glm::vec3(25.0f, 9.8f, 25.0f);
		light02.Direction = glm::vec3(1.0f, 1.0f, 1.0f);
		light02.Power = glm::vec4(100.0f, 100.0f, 100.0f, 1.0f);
		light02.alphaIndex = 10.0;
		light02.Color = glm::vec4(1.0f, 1.0f, 1.0, 1.0f);
		light02.distance = 10.0f;

		phongShader->setVec4("LightColor", light01.Color);
		phongShader->setVec4("LightPower", light01.Power);
		phongShader->setInt("alphaIndex", light01.alphaIndex);
		phongShader->setFloat("distance", light01.distance);
		phongShader->setVec3("lightPosition", light01.Position);
		phongShader->setVec3("lightDirection", light01.Direction);
		//phongShader->setVec3("eye", camera.Position);

		// Configuramos propiedades de fuentes de luz (Luz02)
		phongShader->setVec4("LightColor2", light02.Color);
		phongShader->setVec4("LightPower2", light02.Power);
		phongShader->setInt("alphaIndex2", light02.alphaIndex);
		phongShader->setFloat("distance2", light02.distance);
		phongShader->setVec3("lightPosition2", light02.Position);
		phongShader->setVec3("lightDirection2", light02.Direction);

		phongShader->setVec4("LightColor3", glm::vec4(1.0f, 1.0f, 1.0, 1.0f));
		phongShader->setVec4("LightPower3", glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
		phongShader->setInt("alphaIndex3", 10.0);
		phongShader->setFloat("distance3", 6.0f);
		phongShader->setVec3("lightPosition3", glm::vec3(-25.0f, 9.8f, -15.0f));
		phongShader->setVec3("lightDirection3", glm::vec3(1.0f, 1.0f, 1.0f));

		phongShader->setVec4("LightColor4", glm::vec4(1.0f, 1.0f, 1.0, 1.0f));
		phongShader->setVec4("LightPower4", glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
		phongShader->setInt("alphaIndex4", 10.0);
		phongShader->setFloat("distance4", 6.0f);
		phongShader->setVec3("lightPosition4", glm::vec3(-25.0f, 9.8f, -15.0f));
		phongShader->setVec3("lightDirection4", glm::vec3(1.0f, 1.0f, 1.0f));


		// Aplicamos propiedades materiales
		phongShader->setVec4("MaterialAmbientColor", glm::vec4(0.5f, 0.5f, 0.3f, 1.0f));
		phongShader->setVec4("MaterialDiffuseColor", glm::vec4(0.4f, 0.4f, 0.4f, 1.0f));
		phongShader->setVec4("MaterialSpecularColor", glm::vec4(0.05f, 0.05f, 0.05f, 1.0f));
		phongShader->setFloat("transparency", material01.transparency);

		triceratops->Draw(*phongShader);

	}glUseProgram(0);

	//T-Rex y Velociraptor
	{
		// Activamos el shader del plano
		phongShader->use();

		// Activamos para objetos transparentes
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Aplicamos transformaciones de proyección y cámara (si las hubiera)		
		glm::mat4 projection;
		glm::mat4 view;
		if (activeCamera) {
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
			view = camera.GetViewMatrix();
		}
		else {
			projection = glm::perspective(glm::radians(camera3rd.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
			view = camera3rd.GetViewMatrix();
		}


		phongShader->setMat4("projection", projection);
		phongShader->setMat4("view", view);

		// Aplicamos transformaciones del modelo
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
		phongShader->setMat4("model", model);

		// Configuramos propiedades de fuentes de luz (Luz01)
		phongShader->setVec3("eye", camera.Position);

		phongShader->setVec4("LightColor", glm::vec4(1.0f, 1.0f, 1.0, 1.0f));
		phongShader->setVec4("LightPower", glm::vec4(25.0f, 25.0f, 25.0f, 1.0f));
		phongShader->setInt("alphaIndex", 10.0);
		phongShader->setFloat("distance", 3.0f);
		phongShader->setVec3("lightPosition", glm::vec3(11.7f, 7.0f, -15.0f));
		phongShader->setVec3("lightDirection", glm::vec3(1.0f, 1.0f, 1.0f));

		// Configuramos propiedades de fuentes de luz (Luz02)
		phongShader->setVec4("LightColor2", glm::vec4(1.0f, 1.0f, 1.0, 1.0f));
		phongShader->setVec4("LightPower2", glm::vec4(25.0f, 25.0f, 25.0f, 1.0f));
		phongShader->setInt("alphaIndex2", 10.0);
		phongShader->setFloat("distance2", 3.0f);
		phongShader->setVec3("lightPosition2", glm::vec3(-30.0f, 7.0f, -30.0f));
		phongShader->setVec3("lightDirection2", glm::vec3(1.0f, 1.0f, 1.0f));

		// Configuramos propiedades de fuentes de luz (Luz03)
		phongShader->setVec4("LightColor3", glm::vec4(1.0f, 1.0f, 1.0, 1.0f));
		phongShader->setVec4("LightPower3", glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
		phongShader->setInt("alphaIndex3", 10.0);
		phongShader->setFloat("distance3", 6.0f);
		phongShader->setVec3("lightPosition3", glm::vec3(-25.0f, 9.8f, -15.0f));
		phongShader->setVec3("lightDirection3", glm::vec3(1.0f, 1.0f, 1.0f));

		phongShader->setVec4("LightColor4", glm::vec4(1.0f, 1.0f, 1.0, 1.0f));
		phongShader->setVec4("LightPower4", glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
		phongShader->setInt("alphaIndex4", 10.0);
		phongShader->setFloat("distance4", 6.0f);
		phongShader->setVec3("lightPosition4", glm::vec3(-25.0f, 9.8f, -15.0f));
		phongShader->setVec3("lightDirection4", glm::vec3(1.0f, 1.0f, 1.0f));

		// Aplicamos propiedades materiales
		phongShader->setVec4("MaterialAmbientColor", material01.ambient);
		phongShader->setVec4("MaterialDiffuseColor", material01.diffuse);
		phongShader->setVec4("MaterialSpecularColor", material01.specular);
		phongShader->setFloat("transparency", material01.transparency);

		//puerta->Draw(*museoShader);
		trexveloci->Draw(*phongShader);
	}glUseProgram(0);

	//Techo
	{
		// Activamos el shader del plano
		phongShader->use();

		// Activamos para objetos transparentes
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Aplicamos transformaciones de proyección y cámara (si las hubiera)		
		glm::mat4 projection;
		glm::mat4 view;
		if (activeCamera) {
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
			view = camera.GetViewMatrix();
		}
		else {
			projection = glm::perspective(glm::radians(camera3rd.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
			view = camera3rd.GetViewMatrix();
		}


		phongShader->setMat4("projection", projection);
		phongShader->setMat4("view", view);

		// Aplicamos transformaciones del modelo
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
		phongShader->setMat4("model", model);

		// Configuramos propiedades de fuentes de luz (Luz01)

		light01.Position = glm::vec3(30.0f, 9.8f, 30.0f);
		light01.Direction = glm::vec3(1.0f, 1.0f, 1.0f);
		light01.Power = glm::vec4(85.0f, 85.0f, 85.0f, 1.0f);
		light01.alphaIndex = 15.0;
		light01.Color = glm::vec4(1.0f, 1.0f, 1.0, 1.0f);
		light01.distance = 3.0f;

		phongShader->setVec4("LightColor", light01.Color);
		phongShader->setVec4("LightPower", light01.Power);
		phongShader->setInt("alphaIndex", light01.alphaIndex);
		phongShader->setFloat("distance", light01.distance);
		phongShader->setVec3("lightPosition", light01.Position);
		phongShader->setVec3("lightDirection", light01.Direction);
		phongShader->setVec3("eye", camera.Position);

		// Configuramos propiedades de fuentes de luz (Luz02)
		light02.Position = glm::vec3(-30.0f, 9.8f, -30.0f);
		light02.Direction = glm::vec3(1.0f, 1.0f, 1.0f);
		light02.Power = glm::vec4(85.0f, 85.0f, 85.0f, 1.0f);
		light02.alphaIndex = 20.0;
		light02.Color = glm::vec4(1.0f, 1.0f, 1.0, 1.0f);
		light02.distance = 3.0f;

		phongShader->setVec4("LightColor2", light02.Color);
		phongShader->setVec4("LightPower2", light02.Power);
		phongShader->setInt("alphaIndex2", light02.alphaIndex);
		phongShader->setFloat("distance2", light02.distance);
		phongShader->setVec3("lightPosition2", light02.Position);
		phongShader->setVec3("lightDirection2", light02.Direction);
		phongShader->setVec3("eye", camera.Position);

		// Configuramos propiedades de fuentes de luz (Luz03)
		light03.Position = glm::vec3(-24.0f, 9.8f, 10.0f);
		light03.Direction = glm::vec3(1.0f, 1.0f, 1.0f);
		light03.Power = glm::vec4(85.0f, 85.0f, 85.0f, 1.0f);
		light03.alphaIndex = 20.0;
		light03.Color = glm::vec4(1.0f, 1.0f, 1.0, 1.0f);
		light03.distance = 3.0f;

		phongShader->setVec4("LightColor3", light03.Color);
		phongShader->setVec4("LightPower3", light03.Power);
		phongShader->setInt("alphaIndex3", light03.alphaIndex);
		phongShader->setFloat("distance3", light03.distance);
		phongShader->setVec3("lightPosition3", light03.Position);
		phongShader->setVec3("lightDirection3", light03.Direction);
		phongShader->setVec3("eye", camera.Position);

		// Configuramos propiedades de fuentes de luz (Luz04)
		light04.Position = glm::vec3(30.0f, 9.8f, -30.0f);
		light04.Direction = glm::vec3(1.0f, 1.0f, 1.0f);
		light04.Power = glm::vec4(85.0f, 85.0f, 85.0f, 1.0f);
		light04.alphaIndex = 20.0;
		light04.Color = glm::vec4(1.0f, 1.0f, 1.0, 1.0f);
		light04.distance = 3.0f;

		phongShader->setVec4("LightColor4", light04.Color);
		phongShader->setVec4("LightPower4", light04.Power);
		phongShader->setInt("alphaIndex4", light04.alphaIndex);
		phongShader->setFloat("distance4", light04.distance);
		phongShader->setVec3("lightPosition4", light04.Position);
		phongShader->setVec3("lightDirection4", light04.Direction);
		phongShader->setVec3("eye", camera.Position);

		// Aplicamos propiedades materiales
		phongShader->setVec4("MaterialAmbientColor", material01.ambient);
		phongShader->setVec4("MaterialDiffuseColor", material01.diffuse);
		phongShader->setVec4("MaterialSpecularColor", material01.specular);
		phongShader->setFloat("transparency", material01.transparency);

		//puerta->Draw(*museoShader);
		techo->Draw(*phongShader);
	}

	glUseProgram(0);
	// Objeto estático (EDIFICIO)
	{
		// Activamos el shader del plano
		museoShader->use();

		// Activamos para objetos transparentes
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Aplicamos transformaciones de proyección y cámara (si las hubiera)		
		glm::mat4 projection;
		glm::mat4 view;
		if (activeCamera) {
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
			view = camera.GetViewMatrix();
		}
		else {
			projection = glm::perspective(glm::radians(camera3rd.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
			view = camera3rd.GetViewMatrix();
		}


		museoShader->setMat4("projection", projection);
		museoShader->setMat4("view", view);

		// Aplicamos transformaciones del modelo
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
		museoShader->setMat4("model", model);

		museo->Draw(*museoShader);
	}

	glUseProgram(0);

	{
		// Activamos el shader del plano
		museoShader->use();

		// Activamos para objetos transparentes
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Aplicamos transformaciones de proyección y cámara (si las hubiera)		
		glm::mat4 projection;
		glm::mat4 view;
		if (activeCamera) {
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
			view = camera.GetViewMatrix();
		}
		else {
			projection = glm::perspective(glm::radians(camera3rd.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
			view = camera3rd.GetViewMatrix();
		}


		museoShader->setMat4("projection", projection);
		museoShader->setMat4("view", view);

		// Aplicamos transformaciones del modelo
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
		museoShader->setMat4("model", model);

		cactus->Draw(*museoShader);
	}

	//Puertas
	{
		// Activamos el shader del plano
		phongPuertas->use();

		// Activamos para objetos transparentes
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Aplicamos transformaciones de proyección y cámara (si las hubiera)		
		glm::mat4 projection;
		glm::mat4 view;
		if (activeCamera) {
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
			view = camera.GetViewMatrix();
		}
		else {
			projection = glm::perspective(glm::radians(camera3rd.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
			view = camera3rd.GetViewMatrix();
		}


		phongPuertas->setMat4("projection", projection);
		phongPuertas->setMat4("view", view);

		// Aplicamos transformaciones del modelo
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(32.45f, 0.0f, 15.0f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotatePuerta1), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
		phongPuertas->setMat4("model", model);
		puerta->Draw(*phongPuertas);


		glm::mat4 model2 = glm::mat4(1.0f);
		model2 = glm::translate(model2, glm::vec3(-12.214f, 0.0f, 39.3f)); // translate it down so it's at the center of the scene
		model2 = glm::rotate(model2, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model2 = glm::rotate(model2, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model2 = glm::rotate(model2, glm::radians(rotatePuerta2), glm::vec3(0.0f, 0.0f, 1.0f));
		model2 = glm::scale(model2, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
		phongPuertas->setMat4("model", model2);
		puerta->Draw(*phongPuertas);

		glm::mat4 model3 = glm::mat4(1.0f);
		model3 = glm::translate(model3, glm::vec3(-12.214f, 0.0f, -34.447f)); // translate it down so it's at the center of the scene
		model3 = glm::rotate(model3, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model3 = glm::rotate(model3, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model3 = glm::rotate(model3, glm::radians(rotatePuerta3), glm::vec3(0.0f, 0.0f, 1.0f));
		model3 = glm::scale(model3, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
		phongPuertas->setMat4("model", model3);
		puerta->Draw(*phongPuertas);

		glm::mat4 model4 = glm::mat4(1.0f);
		model4 = glm::translate(model4, glm::vec3(12.0f, 0.0f, -18.973f)); // translate it down so it's at the center of the scene
		model4 = glm::rotate(model4, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model4 = glm::rotate(model4, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model4 = glm::rotate(model4, glm::radians(rotatePuerta4), glm::vec3(0.0f, 0.0f, 1.0f));
		model4 = glm::scale(model4, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
		phongPuertas->setMat4("model", model4);
		puerta->Draw(*phongPuertas);

		glm::mat4 model5 = glm::mat4(1.0f);
		model5 = glm::translate(model5, glm::vec3(19.139f, 0.0f, -15.0f)); // translate it down so it's at the center of the scene
		model5 = glm::rotate(model5, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model5 = glm::rotate(model5, glm::radians(-180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model5 = glm::rotate(model5, glm::radians(rotatePuerta5), glm::vec3(0.0f, 0.0f, 1.0f));
		model5 = glm::scale(model5, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
		phongPuertas->setMat4("model", model5);
		puerta->Draw(*phongPuertas);

		// Configuramos propiedades de fuentes de luz (Luz01)
		phongPuertas->setVec3("eye", camera.Position);

		phongPuertas->setVec4("LightColor", glm::vec4(1.0f, 1.0f, 1.0, 1.0f));
		phongPuertas->setVec4("LightPower", glm::vec4(25.0f, 25.0f, 25.0f, 1.0f));
		phongPuertas->setInt("alphaIndex", 10.0);
		phongPuertas->setFloat("distance", 3.0f);
		phongPuertas->setVec3("lightPosition", glm::vec3(30.0f, 7.0f, 30.0f));
		phongPuertas->setVec3("lightDirection", glm::vec3(1.0f, 1.0f, 1.0f));

		// Configuramos propiedades de fuentes de luz (Luz02)
		phongPuertas->setVec4("LightColor2", glm::vec4(1.0f, 1.0f, 1.0, 1.0f));
		phongPuertas->setVec4("LightPower2", glm::vec4(30.0f, 30.0f, 30.0f, 1.0f));
		phongPuertas->setInt("alphaIndex2", 10.0);
		phongPuertas->setFloat("distance2", 3.0f);
		phongPuertas->setVec3("lightPosition2", glm::vec3(-30.0f, 7.0f, -30.0f));
		phongPuertas->setVec3("lightDirection2", glm::vec3(1.0f, 1.0f, 1.0f));

		// Configuramos propiedades de fuentes de luz (Luz03)
		phongPuertas->setVec4("LightColor3", glm::vec4(1.0f, 1.0f, 1.0, 1.0f));
		phongPuertas->setVec4("LightPower3", glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
		phongPuertas->setInt("alphaIndex3", 10.0);
		phongPuertas->setFloat("distance3", 6.0f);
		phongPuertas->setVec3("lightPosition3", glm::vec3(-25.0f, 9.8f, -15.0f));
		phongPuertas->setVec3("lightDirection3", glm::vec3(1.0f, 1.0f, 1.0f));

		phongPuertas->setVec4("LightColor4", glm::vec4(1.0f, 1.0f, 1.0, 1.0f));
		phongPuertas->setVec4("LightPower4", glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
		phongPuertas->setInt("alphaIndex4", 10.0);
		phongPuertas->setFloat("distance4", 6.0f);
		phongPuertas->setVec3("lightPosition4", glm::vec3(-25.0f, 9.8f, -15.0f));
		phongPuertas->setVec3("lightDirection4", glm::vec3(1.0f, 1.0f, 1.0f));

		// Aplicamos propiedades materiales
		phongPuertas->setVec4("MaterialAmbientColor", material01.ambient);
		phongPuertas->setVec4("MaterialDiffuseColor", material01.diffuse);
		phongPuertas->setVec4("MaterialSpecularColor", material01.specular);
		phongPuertas->setFloat("transparency", material01.transparency);

	}glUseProgram(0);






	// Objeto animado
	{
		// Activación del shader del personaje
		ourShader->use();

		// Aplicamos transformaciones de proyección y cámara (si las hubiera)
		glm::mat4 projection;
		glm::mat4 view;
		if (activeCamera) {
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
			view = camera.GetViewMatrix();
		}
		else {
			projection = glm::perspective(glm::radians(camera3rd.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
			view = camera3rd.GetViewMatrix();
		}
		ourShader->setMat4("projection", projection);
		ourShader->setMat4("view", view);

		// Aplicamos transformaciones del modelo
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(rotateCharacter), glm::vec3(0.0, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));	// it's a bit too big for our scene, so scale it down

		ourShader->setMat4("model", model);

		ourShader->setMat4("gBones", MAX_RIGGING_BONES, gBones);

		// Dibujamos el modelo
		character->Draw(*ourShader);
	}glUseProgram(0);
	{
		// Activación del shader del personaje
		ourShader->use();

		// Aplicamos transformaciones de proyección y cámara (si las hubiera)
		glm::mat4 projection;
		glm::mat4 view;
		if (activeCamera) {
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
			view = camera.GetViewMatrix();
		}
		else {
			projection = glm::perspective(glm::radians(camera3rd.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
			view = camera3rd.GetViewMatrix();
		}
		ourShader->setMat4("projection", projection);
		ourShader->setMat4("view", view);

		// Aplicamos transformaciones del modelo
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(1.5f, 0.0f, -20.0f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));	// it's a bit too big for our scene, so scale it down

		ourShader->setMat4("model", model);

		ourShader->setMat4("gBones", MAX_RIGGING_BONES, gBonesChar);


		// Dibujamos el modelo
		character1->Draw(*ourShader);
	}
	glUseProgram(0);


	//Cuchara
	{
		// Activamos el shader de fresnel
		fresnelShader->use();

		// Activamos para objetos transparentes
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Aplicamos transformaciones de proyección y cámara (si las hubiera)
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		fresnelShader->setMat4("projection", projection);
		fresnelShader->setMat4("view", view);

		// Aplicamos transformaciones del modelo
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(39.8f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		fresnelShader->setMat4("model", model);
		fresnelShader->setFloat("transparency", 0.3f);
		//material01.transparency
		glDepthMask(GL_FALSE);
		glBindTexture(GL_TEXTURE_CUBE_MAP, mainCubeMap->textureID);
		glDepthMask(GL_TRUE);
		vidrio->Draw(*fresnelShader);

	}

	glUseProgram(0);

	// glfw: swap buffers 
	glfwSwapBuffers(window);
	glfwPollEvents();

	return true;
}

// Procesamos entradas del teclado
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime + 0.05);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime + 0.05);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);



	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		// LIMITES
		if (position.x > boundingboxMin.x && position.x	< boundingboxMax.x
			&& position.z > boundingboxMin.z && position.z < boundingboxMax.z) {
			// OBJETOS
			// Huesos T-Rex
			if ((position.x < -5 || position.x > 5) || (position.z < -8 || position.z > 8))
				// Velociraptor
				if ((position.x < -9.5 || position.x > -2) || (position.z < -26.5 || position.z > -16))
					// Triceratops
					if ((position.x < -10 || position.x > 11) || (position.z < 17 || position.z > 27))
						// Dino 1
						if ((position.x < -30.5 || position.x > -21.5) || (position.z < -28 || position.z > -12))
							// Dino 2
							if ((position.x < -30.5 || position.x > -21.5) || (position.z < 12 || position.z > 28.2))
								// Mesa 1
								if ((position.x < 17 || position.x > 21) || (position.z < -37.5 || position.z > -32))
									// Mesa 2
									if ((position.x < 17 || position.x > 21) || (position.z < -27.7 || position.z > -21.8))
										// Mesa 3
										if ((position.x < 26 || position.x > 30) || (position.z < -37.5 || position.z > -32))
											// Mesa 4
											if ((position.x < 26 || position.x > 30) || (position.z < -27.5 || position.z > -21.8))
												// Mesa 5
												if ((position.x < 33 || position.x > 37) || (position.z < -37.5 || position.z > -32))
													// Mesa 6
													if ((position.x < 33 || position.x > 37) || (position.z < -27.5 || position.z > -21.8))
											
											// PAREDES
											// Pared 1
								if ((position.x < -13 || position.x > -11) || (position.z < -34.2 || position.z > 34.5))
									// Pared 2
									if ((position.x < -10 || position.x > 28) || (position.z < 14 || position.z > 16))
										// Pared 3
										if ((position.x < 32 || position.x > 39) || (position.z < 14 || position.z > 16))
											// Pared 4
											if ((position.x < -10 || position.x > 19) || (position.z < -16 || position.z > -14))
												// Pared 5
												if ((position.x < 24 || position.x > 39) || (position.z < -16 || position.z > -14))
													// Pared 6
													if ((position.x < 11 || position.x > 13) || (position.z < -39 || position.z > -24))
														// Pared 7
														if ((position.x < 11 || position.x > 13) || (position.z < -19 || position.z > -16))
															//CAMARA EN TERCERA PERSONA
															position = position + scaleV * forwardView;
			camera3rd.Front = forwardView;
			camera3rd.ProcessKeyboard(FORWARD, deltaTime);
			camera3rd.Position = position;
			camera3rd.Position.y += 3.5f;
			camera3rd.Position -= forwardView;
		}
	}




	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		//CAMARA EN TERCERA PERSONA
		position = position - scaleV * forwardView;
		camera3rd.Front = forwardView;
		camera3rd.ProcessKeyboard(BACKWARD, deltaTime);
		camera3rd.Position = position;
		camera3rd.Position.y += 3.5f;
		camera3rd.Position -= forwardView;
	}


	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		rotateCharacter += 0.5f;

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(rotateCharacter), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::vec4 viewVector = model * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		forwardView = glm::vec3(viewVector);
		forwardView = glm::normalize(forwardView);

		//CAMARA EN TERCERA PERSONA
		camera3rd.Front = forwardView;
		camera3rd.Position = position;
		camera3rd.Position.y += 3.5f;
		camera3rd.Position -= forwardView;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		rotateCharacter -= 0.5f;

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(rotateCharacter), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::vec4 viewVector = model * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		forwardView = glm::vec3(viewVector);
		forwardView = glm::normalize(forwardView);

		//CAMARA EN TERCERA PERSONA
		camera3rd.Front = forwardView;
		camera3rd.Position = position;
		camera3rd.Position.y += 3.5f;
		camera3rd.Position -= forwardView;
	}

	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
	{
		rotateDino1 += 0.05f;
	}
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
	{
		rotateDino1 -= 0.05f;
	}
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
	{
		if (tamanoTric.x < 1.3)
		{
			tamanoTric += 0.005f;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		if (tamanoTric.x > 1.0)
		{
			tamanoTric -= 0.005f;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		if (rotatePuerta1 < 90.0f)
			rotatePuerta1 += 1.0f;
	}

	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
		if (rotatePuerta1 > 0.0f)
			rotatePuerta1 -= 1.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
		if (rotatePuerta2 < 90.0f)
			rotatePuerta2 += 1.0f;
	}

	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
		if (rotatePuerta2 > 0.0f)
			rotatePuerta2 -= 1.0f;
	}

	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
		if (rotatePuerta3 < 90.0f)
			rotatePuerta3 += 1.0f;
	}

	if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
		if (rotatePuerta3 > 0.0f)
			rotatePuerta3 -= 1.0f;
	}

	if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) {
		if (rotatePuerta4 < 90.0f)
			rotatePuerta4 += 1.0f;
	}

	if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS) {
		if (rotatePuerta4 > 0.0f)
			rotatePuerta4 -= 1.0f;
	}

	if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS) {
		if (rotatePuerta5 < 90.0f)
			rotatePuerta5 += 1.0f;
	}

	if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
		if (rotatePuerta5 > 0.0f)
			rotatePuerta5 -= 1.0f;
	}

	if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS)
		activeCamera = 0;
	if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS)
		activeCamera = 1;
}

// glfw: Actualizamos el puerto de vista si hay cambios del tamaño
// de la ventana
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// glfw: Callback del movimiento y eventos del mouse
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstMouse = false;
	}

	float xoffset = (float)xpos - lastX;
	float yoffset = lastY - (float)ypos;

	lastX = (float)xpos;
	lastY = (float)ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: Complemento para el movimiento y eventos del mouse
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll((float)yoffset);
}