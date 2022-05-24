/*---------------------------------------------------------*/
/* ----------------  Proyecto Final -----------------------*/
/*-----------------    2022-2   ---------------------------*/

#include <Windows.h>

#include <glad/glad.h>
#include <glfw3.h>	//main
#include <stdlib.h>		
#include <glm/glm.hpp>	//camera y model
#include <glm/gtc/matrix_transform.hpp>	//camera y model
#include <glm/gtc/type_ptr.hpp>
#include <time.h>


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>	//Texture

#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>

#include <shader_m.h>
#include <camera.h>
#include <modelAnim.h>
#include <model.h>
#include <Skybox.h>
#include <iostream>

//#pragma comment(lib, "winmm.lib")

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//void my_input(GLFWwindow *window);
void my_input(GLFWwindow* window, int key, int scancode, int action, int mods);
void animate(void);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
GLFWmonitor *monitors;

void getResolution(void);

// camera
Camera camera(glm::vec3(0.0f, 10.0f, 90.0f));
float MovementSpeed = 0.1f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
const int FPS = 60;
const int LOOP_TIME = 1000 / FPS; // = 16 milisec // 1000 millisec == 1 sec
double	deltaTime = 0.0f,
		lastFrame = 0.0f;

//Lighting
glm::vec3 lightPosition(0.0f, 4.0f, -10.0f);
glm::vec3 lightDirection(0.0f, -1.0f, -1.0f);


float girollantas = 0.0f;
float	orienta = 0.0f;
float	movAuto_x = 0.0f,
movAuto_z = 0.0f,
movAuto_y = 0.0f,
rotAuto_x = 0.0f,
rotAuto_z = 0.0f,
rotAuto_y = 180.0f,
rotaCabeza = 0.0f,
rotaCola = 0.0f,
OrientaCuerpoIchty = 0.0f,
OrientaColaIchty = 0.0f,
PosIniIchty_x = 200.0f,
PosIniIchty_z = 200.0f,
PosIchty_x = 0.0f,
PosIchty_z = 0.0f,
MiVariableIchty = 0.0f;

//Velo

bool BajaCabeza = true,
SubeCabeza = false,
MovRaptor = false,
MovIchty = false;


bool MueveColaIzq = false,
MueveColaDer = true;
bool avanza = false;

bool	animacion = false;


//Keyframes (Manipulación y dibujo)
float	posX = 0.0f,
		posY = 0.0f,
		posZ = 0.0f,
		aposX = 0.0f,
		aposZ = -200.0f,
		incX = 0.0f,
		incY = 0.0f,
		incZ = 0.0f,
		rotaI = 0.0f,
		rotaD = 0.0f,
		rotaC = -90.f,
		rotaIInc = 0.0f,
		rotaDInc = 0.0f,
		rotaCInc = 0.0f,
		movCXInc = 0.0f,
		movCZInc = 0.0f;
	    

#define MAX_FRAMES 9
int i_max_steps = 60;
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float posX;		//Variable para PosicionX
	float posY;		//Variable para PosicionY
	float posZ;		//Variable para PosicionZ
	float rotRodIzq;
	float giroMonito;
	float movAlaDer;
	float aposX;
	float aposZ;
	float rotaI;
	float rotaD;
	float rotaC;
}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 5;			//introducir datos
bool play = false;
int playIndex = 0;

void saveFrame(void)
{
	//printf("frameindex %d\n", FrameIndex);
	std::cout << "Frame Index = " << FrameIndex << std::endl;

	
	KeyFrame[FrameIndex].posX = posX;
	KeyFrame[FrameIndex].posY = posY;
	KeyFrame[FrameIndex].posZ = posZ;

	FrameIndex++;
}

void resetElements(void)
{
	posX = KeyFrame[0].posX;
	posY = KeyFrame[0].posY;
	posZ = KeyFrame[0].posZ;
	
	rotaI = KeyFrame[0].rotaI;
	rotaD = KeyFrame[0].rotaD;
	rotaC = KeyFrame[0].rotaC;
	aposX = KeyFrame[0].aposX;
	aposZ = KeyFrame[0].aposZ;
}

void interpolation(void)
{
	incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
	incY = (KeyFrame[playIndex + 1].posY - KeyFrame[playIndex].posY) / i_max_steps;
	incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;

	rotaDInc = (KeyFrame[playIndex + 1].rotaI - KeyFrame[playIndex].rotaI) / i_max_steps;
	rotaIInc = (KeyFrame[playIndex + 1].rotaD - KeyFrame[playIndex].rotaD) / i_max_steps;
	rotaCInc = (KeyFrame[playIndex + 1].rotaC - KeyFrame[playIndex].rotaC) / i_max_steps;
	movCXInc = (KeyFrame[playIndex + 1].aposX - KeyFrame[playIndex].aposX) / i_max_steps;
	movCZInc = (KeyFrame[playIndex + 1].aposZ - KeyFrame[playIndex].aposZ) / i_max_steps;
}
int re = 1;
void animate(void)
{
	
	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				std::cout << "Animation ended" << std::endl;
				//printf("termina anim\n");
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				i_curr_steps = 0; //Reset counter
								  //Interpolation
				interpolation();
			}
		}
		else
		{
			//Draw animation
			posX += incX;
			posY += incY;
			posZ += incZ;

			
			rotaD += rotaDInc;
			rotaI += rotaIInc;
			rotaC += rotaCInc;
			aposX += movCXInc;
			aposZ += movCZInc;

			i_curr_steps++;
		}
	}
	
	//Vehículo
	if (animacion)
	{
		
			if (movAuto_z <= 50.0f) {
				
				movAuto_z += 1.0f;
				girollantas += 3.0f;
				avanza = true;
			} 
			
			
			else
				avanza = false;
		
		
	}

	if (MovRaptor)
	{
		if (BajaCabeza)
		{
			rotaCabeza += 1.0f;
			if (rotaCabeza >= 20.0f)
			{
				SubeCabeza = true;
				BajaCabeza = false;
			}
		}
		if (SubeCabeza)
		{
		   rotaCabeza -= 1.0f;
		   if (rotaCabeza <= -30.0f)
				{
					SubeCabeza = false;
					BajaCabeza = true;
				}
		}
	}
	if (MovIchty)
	{

		//MiVariableIchty = 0.0;

		PosIchty_x = 100.0f * cos(MiVariableIchty);
		PosIchty_z = 100.0f * sin(MiVariableIchty);

		MiVariableIchty += 0.02;
		OrientaCuerpoIchty -= 1.15f;
		OrientaColaIchty -= 1.15f;

		if (MueveColaDer)
		{
			rotaCola += 3.0;
			if (rotaCola >= 35.0f)
			{
				MueveColaDer = false;
				MueveColaIzq = true;
			}
		}
		if (MueveColaIzq)
		{
			rotaCola -= 1.0f;
			if (rotaCola <= -35.0f)
			{
				MueveColaDer = true;
				MueveColaIzq = false;
			}
		}
	}
}


void getResolution()
{
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	SCR_WIDTH = mode->width;
	SCR_HEIGHT = (mode->height) - 80;
}


int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	// --------------------
	monitors = glfwGetPrimaryMonitor();
	getResolution();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CGeIHC", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetWindowPos(window, 0, 30);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, my_input);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader staticShader("Shaders/shader_Lights.vs", "Shaders/shader_Lights.fs");
	Shader skyboxShader("Shaders/skybox.vs", "Shaders/skybox.fs");
	Shader animShader("Shaders/anim.vs", "Shaders/anim.fs");

	vector<std::string> faces
	{
		"resources/skybox/right.jpg",
		"resources/skybox/left.jpg",
		"resources/skybox/top.jpg",
		"resources/skybox/bottom.jpg",
		"resources/skybox/front.jpg",
		"resources/skybox/back.jpg"
	};

	Skybox skybox = Skybox(faces);

	// Shader configuration
	// --------------------
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	// load models
	// -----------
	//Model piso("resources/objects/piso/piso.obj");
	Model piso("resources/objects/piso/piso.obj");
	Model Silla("resources/objects/Silla/old_table.obj");
	Model Raptor("resources/ObjectsRodrigo/Raptor/Raptor.obj");
	Model Roca("resources/ObjectsRodrigo/Arboles/LowPolyTree.obj");
	Model Edificio1("resources/ObjectsRodrigo/Edificios/Edif1.obj");
	//Volador
	Model AlaDer("resources/objects/Dinos/Ptera/AlaDerPtera.obj");
	Model AlaIzq("resources/objects/Dinos/Ptera/AlaIzqPtera.obj");
	Model CueDino("resources/objects/Dinos/Ptera/torso.obj");
	//T-Rex
	Model DinoT("resources/objects/Dinos/T-Rex/T-Rex.obj");
	Model DinoTDer("resources/objects/Dinos/T-Rex/PataDerechaTRex.obj");
	Model DinoTIzq("resources/objects/Dinos/T-Rex/PataIzqTRex.obj");
	//Carro de Helados
	Model Helados("resources/objects/Carro de helados/helados.obj");
	Model HeladosDer("resources/objects/Carro de helados/RuedaDer.obj");
	Model HeladosIzq("resources/objects/Carro de helados/RuedaIzq.obj");
	//Velociraptos
	Model CabezaRap("resources/objects/Dinos/Vel/cabeza.obj");
	Model CuerpoRap("resources/objects/Dinos/Vel/cuerpo.obj");
	Model CuerpoIchty("resources/objects/Dinos/Nadar/Cuerpo.obj");
	Model ColaIchty("resources/objects/Dinos/Nadar/Cola.obj");
	//Nadar
	Model Pecera("resources/objects/pecera/AguaL.obj");
	Model Cerca("resources/objects/pecera/Cerca.obj");

	Model Kiosko("resources/objects/Kiosko/kiosko.obj");
	Model Arbol("resources/objects/Arboles/arboles.obj");
	Model Cafe("resources/objects/Coffee Shop/Coffee Shop.obj");
	Model Faro("resources/objects/Lamparas/Faros/streetlight.obj");
	//ModelAnim Walking("resources/objects/Walking/Walking.dae");
	//Walking.initShaders(animShader.ID);
	
	KeyFrame[0].posX = 0;
	KeyFrame[0].posY = 0;
	KeyFrame[0].posZ = 0;
	KeyFrame[0].rotaI = 0;
	KeyFrame[0].rotaC = 0;
	KeyFrame[0].rotaD = 0;
	KeyFrame[0].aposX = 0;
	KeyFrame[0].aposZ = 0;


	KeyFrame[1].posX = 200;
	KeyFrame[1].posY = 12;
	KeyFrame[1].posZ = -170;
	KeyFrame[1].rotaI = 45;
	KeyFrame[1].rotaD = 45;
	KeyFrame[1].rotaC = -72;
	KeyFrame[1].aposX = -100;
	KeyFrame[1].aposZ = -170;
    
	KeyFrame[2].posX = 200;
	KeyFrame[2].posY = 12;
	KeyFrame[2].posZ = -160;
	KeyFrame[2].rotaI = -15;
	KeyFrame[2].rotaD = -15;
	KeyFrame[2].rotaC = -54;
	KeyFrame[2].aposX = -200;
	KeyFrame[2].aposZ = -140;
	
	KeyFrame[3].posX = 200;
	KeyFrame[3].posY = 12;
	KeyFrame[3].posZ = -150;
	KeyFrame[3].rotaI = 45;
	KeyFrame[3].rotaD = 45;
	KeyFrame[3].rotaC = -36;
	KeyFrame[3].aposX = -300;
	KeyFrame[3].aposZ = -500;

	KeyFrame[4].posX = 200;
	KeyFrame[4].posY = 12;
	KeyFrame[4].posZ = -280;
	KeyFrame[4].rotaI = -15;
	KeyFrame[4].rotaD = -15;
	KeyFrame[4].rotaC = -18;
	KeyFrame[4].aposX = 0;
	KeyFrame[4].aposZ = -100;

	

	while (!glfwWindowShouldClose(window))
	{
		skyboxShader.setInt("skybox", 0);
		
		// per-frame time logic
		// --------------------
		lastFrame = SDL_GetTicks();

		// input
		// -----
		//my_input(window);
		animate();

		// render
		// ------
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		staticShader.use();
		//Setup Advanced Lights
		staticShader.setVec3("viewPos", camera.Position);
		staticShader.setVec3("dirLight.direction", lightDirection);
		staticShader.setVec3("dirLight.ambient", glm::vec3(1.0f, 1.0f, 1.0f));
		staticShader.setVec3("dirLight.diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));

		staticShader.setVec3("pointLight[0].position", lightPosition);
		staticShader.setVec3("pointLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[0].constant", 0.08f);
		staticShader.setFloat("pointLight[0].linear", 0.009f);
		staticShader.setFloat("pointLight[0].quadratic", 0.032f);

		staticShader.setVec3("pointLight[1].position", glm::vec3(-80.0, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[1].constant", 1.0f);
		staticShader.setFloat("pointLight[1].linear", 0.009f);
		staticShader.setFloat("pointLight[1].quadratic", 0.032f);

		staticShader.setFloat("material_shininess", 32.0f);

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 tmp = glm::mat4(1.0f);
		glm::mat4 tmpH = glm::mat4(1.0f);
		glm::mat4 tmpDinoT = glm::mat4(1.0f);
		glm::mat4 tmDino = glm::mat4(1.0f);
		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);

		//// Light
		glm::vec3 lightColor = glm::vec3(0.6f);
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.75f);
		

		// -------------------------------------------------------------------------------------------------------------------------
		// Personaje Animacion
		// -------------------------------------------------------------------------------------------------------------------------
		//Remember to activate the shader with the animation
		animShader.use();
		animShader.setMat4("projection", projection);
		animShader.setMat4("view", view);
	
		animShader.setVec3("material.specular", glm::vec3(0.5f));
		animShader.setFloat("material.shininess", 32.0f);
		animShader.setVec3("light.ambient", ambientColor);
		animShader.setVec3("light.diffuse", diffuseColor);
		animShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		animShader.setVec3("light.direction", lightDirection);
		animShader.setVec3("viewPos", camera.Position);

		/*model = glm::translate(glm::mat4(1.0f), glm::vec3(100.3f, 10.75f, 10.3f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(1.2f));	// it's a bit too big for our scene, so scale it down
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		animShader.setMat4("model", model);
		Walking.Draw(animShader);*/
		// -------------------------------------------------------------------------------------------------------------------------
		// Escenario
		// -------------------------------------------------------------------------------------------------------------------------
		staticShader.use();//Dibujar las cosas después de este shader
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		staticShader.setMat4("model", model);
		piso.Draw(staticShader);


		// -------------------------------------------------------------------------------------------------------------------------
		// Dinosaurio Volador
		// -------------------------------------------------------------------------------------------------------------------------
			
		tmpDinoT = model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 165.0f, 0));
		model = glm::rotate(model, glm::radians(rotaC), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(4.0f));
		staticShader.setMat4("model", model);
		CueDino.Draw(staticShader);
		

		model = glm::translate(tmpDinoT, glm::vec3(0.0f, 5.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotaC), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotaI), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(3.0f));
		staticShader.setMat4("model", model);
		AlaIzq.Draw(staticShader);
		
		model = glm::translate(tmpDinoT, glm::vec3(0.0f, 5.4f, 0.0f));
		model = glm::rotate(model, glm::radians(rotaC), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotaD - 20.0f), glm::vec3(0.0f, 0.0f, -1.0f));
		model = glm::scale(model, glm::vec3(3.0f));
		staticShader.setMat4("model", model);
		AlaDer.Draw(staticShader);
		
		
		
		//HELADOS
		
		model = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f));
		model = glm::translate(model, glm::vec3(10.0f + movAuto_x, -1.0f + movAuto_y, movAuto_z));
		tmpH = model = glm::rotate(model, glm::radians(orienta), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model); //
		Helados.Draw(staticShader); // 
		
		model = glm::translate(tmpH, glm::vec3(7.7f, 5.0f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		HeladosDer.Draw(staticShader);

		model = glm::translate(tmpH, glm::vec3(-7.7f, 5.0f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		HeladosIzq.Draw(staticShader); 
		
		//RAPTOR
		model = glm::translate(glm::mat4(1.0f), glm::vec3(60.0f, 40.0f, 100.0f));
		model = glm::rotate(model, glm::radians(rotaCabeza), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		CabezaRap.Draw(staticShader);	//Raptor dino extra

		model = glm::translate(glm::mat4(1.0f), glm::vec3(60.0f, 40.0f, 100.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		CuerpoRap.Draw(staticShader);	//Raptor dino extra
		
		//NADAR
		model = glm::translate(glm::mat4(1.0f), glm::vec3(PosIniIchty_x + PosIchty_x, 20.0f, PosIniIchty_z + PosIchty_z));
		model = glm::rotate(model, glm::radians(rotaCola), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(OrientaColaIchty), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f));
		staticShader.setMat4("model", model);
		ColaIchty.Draw(staticShader);	//Ichty dino extra

		model = glm::translate(glm::mat4(1.0f), glm::vec3(PosIniIchty_x + PosIchty_x, 20.0f, PosIniIchty_z + PosIchty_z));	//600	20	600 PosIniIchty_z +
		model = glm::rotate(model, glm::radians(OrientaCuerpoIchty), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f));
		staticShader.setMat4("model", model);
		CuerpoIchty.Draw(staticShader);	//Ichty dino extra
		
		//PECERA
		model = glm::translate(glm::mat4(1.0f), glm::vec3(400.0f,20.0f,200.0f));
		model = glm::scale(model, glm::vec3(2.0f));
		staticShader.setMat4("model", model);
		Pecera.Draw(staticShader);	
		model = glm::translate(glm::mat4(1.0f), glm::vec3(400.0f, 20.0f, 200.0f));
		model = glm::scale(model, glm::vec3(2.0f));
		staticShader.setMat4("model", model);
		Cerca.Draw(staticShader);
		
		//KIOSKO
		model = glm::translate(glm::mat4(1.0f), glm::vec3(100.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f));
		staticShader.setMat4("model", model);
		Kiosko.Draw(staticShader);

		// ARBOLES
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-350.0f, 0.0f, -250.0f));
		model = glm::scale(model, glm::vec3(0.5f));
		staticShader.setMat4("model", model);
		Arbol.Draw(staticShader);

		//COFFE 
		model = glm::translate(glm::mat4(1.0f), glm::vec3(150.0f, 0.0f, -250.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		staticShader.setMat4("model", model);
		Cafe.Draw(staticShader);
		//FAROS
		model = glm::translate(glm::mat4(1.0f), glm::vec3(500.0f, 0.0f, 250.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		staticShader.setMat4("model", model);
		Faro.Draw(staticShader);
		// -------------------------------------------------------------------------------------------------------------------------
		// Termina Escenario
		// -------------------------------------------------------------------------------------------------------------------------

		//-------------------------------------------------------------------------------------
		// draw skybox as last
		// -------------------
		skyboxShader.use();
		skybox.Draw(skyboxShader, view, projection, camera);

		// Limitar el framerate a 60
		deltaTime = SDL_GetTicks() - lastFrame; // time for full 1 loop
		//std::cout <<"frame time = " << frameTime << " milli sec"<< std::endl;
		if (deltaTime < LOOP_TIME)
		{
			SDL_Delay((int)(LOOP_TIME - deltaTime));
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();

		if (play == false && (FrameIndex > 1))
		{
			std::cout << "Play animation" << std::endl;
			resetElements();
			//First Interpolation
			interpolation();

			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
	}

	skybox.Terminate();

	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void my_input(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, (float)deltaTime);
	//To Configure Model
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
		posZ++;
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		posZ--;
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		posX--;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		posX++;
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		lightPosition.x++;
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		lightPosition.x--;
	
	//Carro
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		animacion ^= true;
	}
	//Raptor
	if (key == GLFW_KEY_6 && action == GLFW_PRESS)
	{
		MovRaptor ^= true;
	}
	//Nadar
	if (key == GLFW_KEY_7 && action == GLFW_PRESS)
	{
		MovIchty ^= true;
		//PosIchty_x = 0;
		PosIniIchty_x = 400.0f;
		PosIniIchty_z = 100.0f;
		rotaCola = 0.0f;
		rotaCabeza = 0.0f;
		OrientaCuerpoIchty = 0.0f;
		OrientaColaIchty = 0.0f;
		MiVariableIchty = 0.0f;
	}
	/*/To play KeyFrame animation 
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		if (play == false && (FrameIndex > 1))
		{
			std::cout << "Play animation" << std::endl;
			resetElements();
			//First Interpolation				
			interpolation();

			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else
		{
			play = false;
			std::cout << "Not enough Key Frames" << std::endl;
		}
	}*/

	//To Save a KeyFrame
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		if (FrameIndex < MAX_FRAMES)
		{
			saveFrame();
		}
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}
// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}