#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED
#include "global_header.h"
using namespace std;
Library <ShaderProgram*> libShader;

class Model
{
    // klasa przechowuje sam szkielet modelu 3D
    // dany model jest tworzony tylko raz a następnie powtórnie rysowany
    public:
    ShaderProgram *shaderProgram;

    GLuint vao;
    GLuint bufVertices;
    GLuint bufColors;
    GLuint bufNormals;
    GLuint bufTriangles;
    GLuint bufTexCoords;
    int vertexCount;

    Model()
    {
        glGenVertexArrays(1,&vao);
    }
    Model(string model,string shader)
    {
        glGenVertexArrays(1,&vao);
        shaderProgram=libShader.get(shader);
        ModelLoader obj(model);
        setVertexCount(3*obj.getTrianglesNum());
        loadVertices(obj.verticesTab);
        loadNormals(obj.normalsTab);
        loadTexture(obj.textureCoordsTab);
    }

    void setVertexCount(int count)
    {
        vertexCount=count;
    }

    void loadVertices(float vertices[])
    {
        //Współrzędne wierzchołków
        glGenBuffers(1,&bufVertices);//Wygeneruj uchwyt na Vertex Buffer Object (VBO), który będzie zawierał tablicę wierzchołków
        glBindBuffer(GL_ARRAY_BUFFER,bufVertices);  //Uaktywnij wygenerowany uchwyt VBO
        glBufferData(GL_ARRAY_BUFFER, vertexCount*sizeof(float)*4, vertices, GL_STATIC_DRAW); //wgraj tablicę wierzchołków do VBO

        GLuint locVertex=shaderProgram->getAttribLocation("vertex"); //"vertex" odnosi się do deklaracji "in vec4 vertex;" w vertex shaderze

        glBindVertexArray(vao); //Uaktywnij VAO

        glBindBuffer(GL_ARRAY_BUFFER,bufVertices);  //Uaktywnij wygenerowany uchwyt VBO
        glEnableVertexAttribArray(locVertex); //Włącz używanie atrybutu o numerze slotu zapisanym w zmiennej locVertex (atrybut "vertex")
        glVertexAttribPointer(locVertex,4,GL_FLOAT, GL_FALSE, 0, NULL); //Dane do slotu locVertex mają być brane z aktywnego VBO

        glBindVertexArray(0);
    }

    void loadNormals(float normals[])
    {
        //Wektory normalne wierzchołków
        glGenBuffers(1,&bufNormals);//Wygeneruj uchwyt na Vertex Buffer Object (VBO), który będzie zawierał tablicę wektorów normalnych
        glBindBuffer(GL_ARRAY_BUFFER,bufNormals);  //Uaktywnij wygenerowany uchwyt VBO
        glBufferData(GL_ARRAY_BUFFER, vertexCount*sizeof(float)*4, normals, GL_STATIC_DRAW); //wgraj tablicę wektorów normalnych do VBO

        GLuint locNormal=shaderProgram->getAttribLocation("normal"); //"normal" odnosi się do deklaracji "in vec4 normal;" w vertex shaderze

        glBindVertexArray(vao); //Uaktywnij VAO

        glBindBuffer(GL_ARRAY_BUFFER,bufNormals);  //Uaktywnij wygenerowany uchwyt VBO
        glEnableVertexAttribArray(locNormal); //Włącz używanie atrybutu o numerze slotu zapisanym w zmiennej locNormal (atrybut "normal")
        glVertexAttribPointer(locNormal,4,GL_FLOAT, GL_FALSE, 0, NULL); //Dane do slotu locNormal mają być brane z aktywnego VBO

        glBindVertexArray(0);
    }

    void loadColors(float colors[])
    {
        //Kolory wierzchołków
        glGenBuffers(1,&bufColors);//Wygeneruj uchwyt na Vertex Buffer Object (VBO), który będzie zawierał tablicę kolorów
        glBindBuffer(GL_ARRAY_BUFFER,bufColors);  //Uaktywnij wygenerowany uchwyt VBO
        glBufferData(GL_ARRAY_BUFFER, vertexCount*sizeof(float)*4, colors, GL_STATIC_DRAW); //wgraj tablicę kolorów do VBO

        GLuint locColor=shaderProgram->getAttribLocation("color"); //"color" odnosi się do deklaracji "in vec4 color;" w vertex shaderze

        glBindVertexArray(vao); //Uaktywnij VAO

        glBindBuffer(GL_ARRAY_BUFFER,bufColors);  //Uaktywnij wygenerowany uchwyt VBO
        glEnableVertexAttribArray(locColor); //Włącz używanie atrybutu o numerze slotu zapisanym w zmiennej locColor (atrybut "color")
        glVertexAttribPointer(locColor,4,GL_FLOAT, GL_FALSE, 0, NULL); //Dane do slotu locColor mają być brane z aktywnego VBO

        glBindVertexArray(0);
    }

    void loadTexture(float texCoords[])
    {
        //Współrzędne teksturowania
        glGenBuffers(1,&bufTexCoords);//Wygeneruj uchwyt na Vertex Buffer Object (VBO), który będzie zawierał tablicę współrzednych teksturowania
        glBindBuffer(GL_ARRAY_BUFFER,bufTexCoords);  //Uaktywnij wygenerowany uchwyt VBO
        glBufferData(GL_ARRAY_BUFFER, vertexCount*sizeof(float)*3, texCoords, GL_STATIC_DRAW); //wgraj tablicę współrzednych teksturowania\

        GLuint locTexCoords=shaderProgram->getAttribLocation("texCoords0"); //"texCoords0" odnosi się do deklaracji "in vec2 texCoords0;" w vertex shaderze

        glBindVertexArray(vao); //Uaktywnij VAO

        glBindBuffer(GL_ARRAY_BUFFER,bufTexCoords);  //Uaktywnij wygenerowany uchwyt VBO
        glEnableVertexAttribArray(locTexCoords); //Włącz używanie atrybutu o numerze slotu zapisanym w zmiennej locTexCoords (atrybut "normal")
        glVertexAttribPointer(locTexCoords,3,GL_FLOAT, GL_FALSE, 0, NULL); //Dane do slotu locTexCoords mają być brane z aktywnego VBO

        glBindVertexArray(0);
    }

    ~Model()
    {
        glDeleteBuffers(1,&bufVertices);
        glDeleteBuffers(1,&bufColors);
        glDeleteBuffers(1,&bufNormals);
        glDeleteVertexArrays(1,&vao);
    }

    void drawModel()
    {
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES,0,vertexCount);
        glBindVertexArray(0);
    }
};

class Texture
{
    // klasa zawiera teksturę
    GLuint tex0; //Uchwyt na teksturę
    GLuint tex1; //Uchwyt na teksturę
    bool dual;
    public:
    GLuint readTexture(char* filename)
    {
        GLuint tex;
        TGAImg img;
        glActiveTexture(GL_TEXTURE0);
        if (img.Load(filename)==IMG_OK) {
            glGenTextures(1,&tex); //Zainicjuj uchwyt tex
            glBindTexture(GL_TEXTURE_2D,tex); //Przetwarzaj uchwyt tex
            if (img.GetBPP()==24)
                //Obrazek 24bit
                glTexImage2D(GL_TEXTURE_2D,0,3,img.GetWidth(),img.GetHeight(),0,
                    GL_RGB,GL_UNSIGNED_BYTE,img.GetImg());
            else if (img.GetBPP()==32)
                //Obrazek 32bit
                glTexImage2D(GL_TEXTURE_2D,0,4,img.GetWidth(),img.GetHeight(),0,
                    GL_RGBA,GL_UNSIGNED_BYTE,img.GetImg());
            else {
                printf("Nieobsługiwany format obrazka w pliku: %s \n",filename);
            }
        } else {
            printf("Błąd przy wczytywaniu pliku: %s\n",filename);
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT);
        return tex;
    }
    public:
    Texture(string tga0)
    {
        dual=false;
        tex0=readTexture(strclone(tga0));
    }
    Texture(string tga0,string tga1)
    {
        dual=true;
        tex0=readTexture(strclone(tga0));
        tex1=readTexture(strclone(tga1));
    }

    ~Texture()
    {
        glDeleteTextures(1,&tex0);
        if(dual)
            {glDeleteTextures(1,&tex1);}
    }

    void use()
    {
        glActiveTexture(GL_TEXTURE0); //Uaktywnij jednostkę teksturującą numer 0
        glBindTexture(GL_TEXTURE_2D,tex0); //Zwiąż aktywną jednostkę teksturującą z teksturą o uchwycie zapisanym w tex
        if(dual)
        {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D,tex1);
        }
    }
};
#endif // MODEL_H_INCLUDED
