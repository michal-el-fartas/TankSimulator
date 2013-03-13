#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H
#include "global_header.h"
using namespace std;

class ShaderProgram {
private:
	GLuint shaderProgram; //Uchwyt reprezentujący program cieniujacy
	GLuint vertexShader; //Uchwyt reprezentujący vertex shader
	GLuint geometryShader; //Uchwyt reprezentujący geometry shader
	GLuint fragmentShader; //Uchwyt reprezentujący fragment shader
	char* readFile(char* fileName); //metoda wczytująca plik tekstowy do tablicy znaków
	GLuint loadShader(GLenum shaderType,char* fileName); //Metoda wczytuje i kompiluje shader, a następnie zwraca jego uchwyt
public:
	ShaderProgram(char* vertexShaderFile,char* geometryShaderFile,char* fragmentShaderFile);
	~ShaderProgram();
	void use(); //Włącza wykorzystywanie programu cieniującego
	GLuint getUniformLocation(char* variableName); //Pobiera numer slotu związanego z daną zmienną jednorodną
	GLuint getAttribLocation(char* variableName); //Pobiera numer slotu związanego z danym atrybutem
};

//Procedura wczytuje plik do tablicy znaków.
char* ShaderProgram::readFile(char* fileName) {
	int filesize;
	FILE *plik;
	char* result;

	plik=fopen(fileName,"r");
	fseek(plik,0,SEEK_END);
	filesize=ftell(plik);
	fseek(plik,0,SEEK_SET);
	result=new char[filesize+1];
	fread(result,1,filesize,plik);
	result[filesize]=0;
	fclose(plik);

	return result;
}

//Metoda wczytuje i kompiluje shader, a następnie zwraca jego uchwyt
GLuint ShaderProgram::loadShader(GLenum shaderType,char* fileName) {
	//Wygeneruj uchwyt na shader
	GLuint shader=glCreateShader(shaderType);//shaderType to GL_VERTEX_SHADER, GL_GEOMETRY_SHADER lub GL_FRAGMENT_SHADER
	//Wczytaj plik ze źródłem shadera do tablicy znaków
	const GLchar* shaderSource=readFile(fileName);
	//Powiąż źródło z uchwytem shadera
	glShaderSource(shader,1,&shaderSource,NULL);
	//Skompiluj źródło
	glCompileShader(shader);
	//Usuń źródło shadera z pamięci (nie będzie już potrzebne)
	delete []shaderSource;

	//Pobierz log błędów kompilacji i wyświetl
	int infologLength = 0;
	int charsWritten  = 0;
	char *infoLog;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH,&infologLength);

	if (infologLength > 1) {
		infoLog = new char[infologLength];
		glGetShaderInfoLog(shader, infologLength, &charsWritten, infoLog);
		cout << tab;
		printf("%s\n",infoLog);
		delete []infoLog;
	}

	//Zwróć uchwyt wygenerowanego shadera
	return shader;
}

ShaderProgram::ShaderProgram(char* vertexShaderFile,char* geometryShaderFile,char* fragmentShaderFile) {
	//Wczytaj vertex shader
	cout << tab;
	printf("Loading vertex shader...\n");
	vertexShader=loadShader(GL_VERTEX_SHADER,vertexShaderFile);

	//Wczytaj geometry shader
	if (geometryShaderFile!=NULL) {
	    cout << tab;
		printf("Loading geometry shader...\n");
		geometryShader=loadShader(GL_GEOMETRY_SHADER,geometryShaderFile);
	} else {
		geometryShader=0;
	}

	//Wczytaj fragment shader
	cout << tab;
	printf("Loading fragment shader...\n");
	fragmentShader=loadShader(GL_FRAGMENT_SHADER,fragmentShaderFile);

	//Wygeneruj uchwyt programu cieniującego
	shaderProgram=glCreateProgram();

	//Podłącz do niego shadery i zlinkuj program
	glAttachShader(shaderProgram,vertexShader);
	glAttachShader(shaderProgram,fragmentShader);
	if (geometryShaderFile!=NULL) glAttachShader(shaderProgram,geometryShader);
	glLinkProgram(shaderProgram);

	//Pobierz log błędów linkowania i wyświetl
	int infologLength = 0;
	int charsWritten  = 0;
	char *infoLog;

	glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH,&infologLength);

	if (infologLength > 1)
	{
		infoLog = new char[infologLength];
		glGetProgramInfoLog(shaderProgram, infologLength, &charsWritten, infoLog);
		cout << tab;
		printf("%s\n",infoLog);
		delete []infoLog;
	}
	cout << tab;
	printf("Shader program created \n");
}

ShaderProgram::~ShaderProgram() {
	//Odłącz shadery od programu
	glDetachShader(shaderProgram, vertexShader);
	if (geometryShader!=0) glDetachShader(shaderProgram, geometryShader);
	glDetachShader(shaderProgram, fragmentShader);

	//Wykasuj shadery
	glDeleteShader(vertexShader);
	if (geometryShader!=0) glDeleteShader(geometryShader);
	glDeleteShader(fragmentShader);

	//Wykasuj program
	glDeleteProgram(shaderProgram);
}


//Włącz używanie programu cieniującego reprezentowanego przez aktualny obiekt
void ShaderProgram::use() {
	glUseProgram(shaderProgram);
}

//Pobierz numer slotu odpowiadającego zmiennej jednorodnej o nazwie variableName
GLuint ShaderProgram::getUniformLocation(char* variableName) {
	return glGetUniformLocation(shaderProgram,variableName);
}

//Pobierz numer slotu odpowiadającego atrybutowi o nazwie variableName
GLuint ShaderProgram::getAttribLocation(char* variableName) {
	return glGetAttribLocation(shaderProgram,variableName);
}

#endif
