#ifndef LOADER_H_INCLUDED
#define LOADER_H_INCLUDED
#include "global_header.h"
using namespace std;


class ModelLoader {
    public:
        ModelLoader(string fileName);
        float* vertices;
        float* normals;
        float* textureCoords;
        int* triangles;

        float* verticesTab; // 4
        float* normalsTab; // 4
        float* textureCoordsTab; // 3

        bool isFileFound;
        bool isTriangleTextureSet;

        struct MtlStruct {
            int Ns, d, Tr, illum;
            int Tf[3];
            float Ka[3];
            float Kd[3];
            float Ks[3];
        };

        vector<MtlStruct> vMtl;

        void displayData();

        string getFileName();
        int getVerticesNum();
        int getNormalsNum();
        int getTextureCoordsNum();
        int getTrianglesNum();

    private:
        string fileName_;

        int verticesNum_;
        int normalsNum_;
        int textureCoordsNum_;
        int trianglesNum_;

        void toTablesExport();
};


ModelLoader::ModelLoader(string fileName) {
    // initializing ModelLoader's variables
    verticesNum_ = 0;
    normalsNum_ = 0;
    textureCoordsNum_ = 0;
    trianglesNum_ = 0;
    fileName_ = fileName;
    isFileFound = true;
    isTriangleTextureSet = true;

    vector<float> vVertices;
    vector<float> vNormals;
    vector<float> vTextureCoords;
    vector<int> vTriangles;

    /* Loading from OBJ file */
    ifstream obj(fileName.c_str());
    string buffer, tempStr;
    stringstream ss, tempSs;
    char c, cc;
    float tempA, tempB, tempC;
    int triangleTempTab[9];
    int tempCounter;

    if(obj== 0) { // obj == NULL?
        isFileFound = false;
        return;
    }

    while(getline(obj, buffer)) {
        #ifdef OBJ_DEBUG
            cout << "Line: "+buffer+"\n";
        #endif

        if(buffer.substr(0,2) == "v ") { // vertex
            ss.clear();
            ss.str("");
            ss.flush(); // all 3 "cleaners" are necessary

            ss << buffer;
            ss >> c >> tempA >> tempB >> tempC;
            vVertices.push_back(tempA);
            vVertices.push_back(tempB);
            vVertices.push_back(tempC);
            ++verticesNum_;
        }
        else if(buffer.substr(0,2) == "vn") {// vertexNormal
            ss.clear();
            ss.str("");
            ss.flush(); // all 3 "cleaners" are necessary

            ss << buffer;
            ss >> c >> c >> tempA >> tempB >> tempC;
            vNormals.push_back(tempA);
            vNormals.push_back(tempB);
            vNormals.push_back(tempC);
            ++normalsNum_;
        }
        else if(buffer.substr(0,2) == "vt") {// textureCoord
            ss.clear();
            ss.str("");
            ss.flush(); // all 3 "cleaners" are necessary

            ss << buffer;
            ss >> c >> c >> tempA >> tempB >> tempC;
            vTextureCoords.push_back(tempA);
            vTextureCoords.push_back(tempB);
            vTextureCoords.push_back(tempC);
            ++textureCoordsNum_;
        }
        else if(buffer.substr(0,2) == "f ") {// triangle
            ss.clear();
            ss.str("");
            ss.flush(); // all 3 "cleaners" are necessary

            ss << buffer;

            if(trianglesNum_==0) { // first triangle - checking notation
                c = 'p'; // as not assigned yet
                for(unsigned int i=0; i<buffer.length(); ++i) {
                    if(c=='p') {
                        c = 'e';
                        cc = buffer[i];
                    }
                    else {
                        c = cc;
                        cc = buffer[i];
                    }

                    if(static_cast<int>(c) == 47 && static_cast<int>(cc) == 47) {
                        //tex not required: vert//norm
                        isTriangleTextureSet = false;
                        break;
                    }
                }
            }

            if(isTriangleTextureSet) { // vert/tex/norm
                tempCounter = 0;
                ss >> c;
                for(int i=0; i<3; ++i) {
                    tempSs.clear();
                    tempSs.str("");
                    tempSs.flush();

                    ss >> tempStr;

                    for(int j=0; j<3; ++j) {
                        tempSs.clear();
                        tempSs.str("");
                        tempSs.flush();

                        while(!(tempStr=="" || static_cast<int>(tempStr[0])==47)) {
                            tempSs << tempStr[0];
                            if(tempStr.length()!=1) tempStr = tempStr.substr(1);
                            else tempStr="";

                        }
                        if(tempStr!="") tempStr = tempStr.substr(1);
                        tempSs >> triangleTempTab[tempCounter];
                        ++tempCounter;
                    }

                    tempStr = "";
                }
                tempCounter = 0;
                for(int i=0; i<9; ++i) {
                    vTriangles.push_back(triangleTempTab[i]);
                }
            }

            else { // v//n notation
                tempCounter = 0;
                ss >> c;
                for(int i=0; i<3; ++i) {
                    tempSs.clear();
                    tempSs.str("");
                    tempSs.flush();

                    ss >> tempStr;

                    while(!(tempStr=="" || static_cast<int>(tempStr[0])==47)) {
                        tempSs << tempStr[0];
                        tempStr = tempStr.substr(1);
                    }

                    tempSs >> triangleTempTab[tempCounter];
                    ++tempCounter;

                    tempStr = tempStr.substr(2);

                    tempSs.clear();
                    tempSs.str("");
                    tempSs.flush();

                    while(!(tempStr=="" || static_cast<int>(tempStr[0])==47)) {
                        tempSs << tempStr[0];
                        if(tempStr.length()>1) tempStr = tempStr.substr(1);
                        else tempStr = "";
                    }

                    tempSs >> triangleTempTab[tempCounter];
                    ++tempCounter;

                    tempStr = "";
                }
                tempCounter = 0;
                for(int i=0; i<6; ++i) {
                    vTriangles.push_back(triangleTempTab[i]);
                }

            }

            ++trianglesNum_;
        }
        buffer.clear();
        ss.str("");
    }

    obj.close();

    /* Loading from MTL file */
/*
    MtlStruct tempMtl;

    obj.open((fileName.substr(0, fileName.length()-3)+"mtl").c_str(), ios::in | ios::out);
    if(obj == 0) { // obj==NULL?
        isFileFound = false;
        return;
    }

    int counter = 0;

    while(getline(obj, buffer)) {
        #ifdef OBJ_DEBUG
            cout << "Line: \""+buffer+"\"\n";
        #endif

        while(buffer[0]!='\n' && (buffer[0]==' ' || int(buffer[0])==9)) {
        // delete whitespaces from the beginning of line
            buffer.erase(buffer.begin());
        }

        if(buffer.substr(0,2) == "Ns") {
            ++counter;
            ss.clear();
            ss.str("");
            ss.flush();

            ss << buffer.substr(2, buffer.length());
            ss >> tempMtl.Ns;
        }
        else if(buffer.substr(0,2) == "d ") {
            ss.clear();
            ss.str("");
            ss.flush();

            ss << buffer.substr(2, buffer.length());
            ss >> tempMtl.d;
        }
        else if(buffer.substr(0,3) == "Tr ") {
            ss.clear();
            ss.str("");
            ss.flush();

            ss << buffer.substr(3, buffer.length());
            ss >> tempMtl.Tr;
        }
        else if(buffer.substr(0,3) == "Tf ") {
            ss.clear();
            ss.str("");
            ss.flush();

            ss << buffer.substr(3, buffer.length());
            ss >> tempMtl.Tf[0] >> tempMtl.Tf[1] >> tempMtl.Tf[2];
        }
        else if(buffer.substr(0,6) == "illum ") {
            ss.clear();
            ss.str("");
            ss.flush();

            ss << buffer.substr(6, buffer.length());
            ss >> tempMtl.illum;
        }
        else if(buffer.substr(0,3) == "Ka ")
        {
            ss.clear();
            ss.str("");
            ss.flush();

            ss << buffer.substr(3, buffer.length());
            ss >> tempMtl.Ka[0] >> tempMtl.Ka[1] >> tempMtl.Ka[2];
         }
         else if(buffer.substr(0,3) == "Kd ")
         {
            ss.clear();
            ss.str("");
            ss.flush();

            ss << buffer.substr(3, buffer.length());
            ss >> tempMtl.Kd[0] >> tempMtl.Kd[1] >> tempMtl.Kd[2];
         }
         else if(buffer.substr(0,3) == "Ks ") {
            ss.clear();
            ss.str("");
            ss.flush();

            ss << buffer.substr(3, buffer.length());
            ss >> tempMtl.Ks[0] >> tempMtl.Ks[1] >> tempMtl.Ks[2];
         }
         else { // comment or new Mtl structure
            if(buffer.substr(0,1)!="#" && buffer.substr(0,1)!="") {
                if(counter>0) vMtl.push_back(tempMtl);
            }
         }

         buffer.clear();
         ss.str("");
         ss.flush();
         ss.clear();

    }

    vMtl.push_back(tempMtl);
*/
    // Let's fill ModelLoader's numeric matrices
    vector<float>::iterator itFloat;

    vertices = new float[verticesNum_*3];
    // I'm pretty proud of this loop
    for(itFloat = vVertices.begin(); itFloat != vVertices.end(); ++itFloat) {
        vertices[distance(vVertices.begin(), itFloat)] = *itFloat;
    }

    normals = new float[normalsNum_*3];
    for(itFloat = vNormals.begin(); itFloat != vNormals.end(); ++itFloat) {
        normals[distance(vNormals.begin(), itFloat)] = *itFloat;
    }

    textureCoords = new float[textureCoordsNum_*3];
    for(itFloat = vTextureCoords.begin(); itFloat != vTextureCoords.end(); ++itFloat) {
        textureCoords[distance(vTextureCoords.begin(), itFloat)] = *itFloat;
    }

    vector<int>::iterator itInt;

    if(isTriangleTextureSet) triangles = new int[trianglesNum_*9];
    else triangles = new int[trianglesNum_*6];
    for(itInt = vTriangles.begin(); itInt != vTriangles.end(); ++itInt) {
        triangles[distance(vTriangles.begin(), itInt)] = *itInt;
    }

    // Small clean-up - maybe unnecessary but so sophisticated
    obj.close();
    buffer.clear();
    ss.clear();
    ss.flush();
    ss.str("");
    vVertices.clear();
    vNormals.clear();
    vTextureCoords.clear();
    vTriangles.clear();

    toTablesExport();
}

void ModelLoader::displayData() {
    // display OBJ data
    cout << "\n\t\t\t*** OBJ file - " << fileName_ << " ***\n";
    int w = cout.width();
    char c = cout.fill();
    int floatWidth = 15;
    int intWidth = 6;

    cout << "\n\tVertices:\n";

    for(int i=0; i<verticesNum_; ++i) {
        cout << "\t\t#" << i << ":";
        for(int j=0; j<3; ++j) {
            cout.width(floatWidth);
            cout.fill(' ');
            cout << vertices[i*3+j];
        }
        cout << "\n";
    }

    cout << "\n\tNormals:\n";

    for(int i=0; i<normalsNum_; ++i) {
        cout << "\t\t#" << i << ":";
        for(int j=0; j<3; ++j) {
            cout.width(floatWidth);
            cout.fill(' ');
            cout << normals[i*3+j];
        }
        cout << "\n";
    }

    cout << "\n\tTexture Coords:\n";
    for(int i=0; i<textureCoordsNum_; ++i) {
        cout << "\t\t#" << i << ":";
        for(int j=0; j<3; ++j) {
            cout.width(floatWidth);
            cout.fill(' ');
            cout << textureCoords[i*3+j];
        }
        cout << "\n";
    }

    // clean-up
    cout.width(w);
    cout.fill(c);
    cout << "\n";

    cout << "\tTriangles:";
    if(isTriangleTextureSet) cout << " (v/t/n)\n";
    else cout << "(v\/\/t)\n";
    for(int i=0; i<trianglesNum_; ++i) {
        if(isTriangleTextureSet) { // 9
            cout << "\t\t#" << i << ":\t";
            cout.width(intWidth);
            cout.fill(' ');
            for(int j=0; j<9; ++j) {
                cout.width(intWidth);
                cout.fill(' ');
                cout << triangles[i*9+j]-1;
            }
            cout << "\n";

        }
        else { // 6
            cout << "\t\t#" << i << ":\t";
            cout.width(intWidth);
            cout.fill(' ');
            for(int j=0; j<6; ++j) {
                cout.width(intWidth);
                cout.fill(' ');
                cout << triangles[i*6+j]-1;
            }
            cout << "\n";
        }
    }

    cout.width(w);
    cout.fill(c);

    // display MTL data

    vector<MtlStruct>::iterator itMtl;

    cout << "\n\n\t\t\t*** OBJ file - " <<
        (fileName_.substr(0, fileName_.length()-3)+"mtl") << " ***\n";

    for(itMtl=vMtl.begin(); itMtl!=vMtl.end(); ++itMtl) {

        cout << "\n\t#" << distance(vMtl.begin(), itMtl) << ":";
        cout << "\n\t\t   Ns:\t";
        cout.width(floatWidth);
        cout.fill(' ');
        cout << (*itMtl).Ns;

        cout << "\n\t\t    d:\t";
        cout.width(floatWidth);
        cout.fill(' ');
        cout << (*itMtl).d;

        cout << "\n\t\t   Tr:\t";
        cout.width(floatWidth);
        cout.fill(' ');
        cout << (*itMtl).Tr;

        cout << "\n\t\t   Tf:\t";
        for(int i=0; i<3; ++i) {
            cout.width(floatWidth);
            cout.fill(' ');
            cout << (*itMtl).Tf[i];
        }
        cout << "\n\t\tillum:\t";
        cout.width(floatWidth);
        cout.fill(' ');
        cout << (*itMtl).illum;

        cout << "\n\t\t   Ka:\t";
        for(int i=0; i<3; ++i) {
            cout.width(floatWidth);
            cout.fill(' ');
            cout << (*itMtl).Ka[i];
        }
        cout << "\n\t\t   Kd:\t";
        for(int i=0; i<3; ++i) {
            cout.width(floatWidth);
            cout.fill(' ');
            cout << (*itMtl).Kd[i];
        }
        cout << "\n\t\t   Ks:\t";
        for(int i=0; i<3; ++i) {
            cout.width(floatWidth);
            cout.fill(' ');
            cout << (*itMtl).Ks[i];
        }
    }

    // clean-up
    cout.width(w);
    cout.fill(c);
    cout << "\n";

}

/***
    w coordinate:
        v - 1
        n - 0
*/

void ModelLoader::toTablesExport() {
    int v, t, n;
    int counter = 0;
    int counter2 = 0;

    #ifdef MODEL_EXPORT_DEBUG
    ofstream exp;
    exp.open("export.txt");
    #endif

    if(!isTriangleTextureSet) { // v//n
        verticesTab = new float[trianglesNum_*12]; // 12 = 4 coods * 3 v in triangle
        normalsTab =  new float[trianglesNum_*12];

        for(int i=0; i<trianglesNum_; ++i) {
            for(int j=0; j<3; ++j) {
                v = triangles[(i*6)+(2*j)]-1;
                n = triangles[(i*6)+(2*j)+1]-1;

                for(int k=0; k<3; ++k) {
                    verticesTab[counter] = vertices[(v*3)+k];
                    normalsTab[counter]  =  normals[(n*3)+k];
                    ++counter;
                }
                verticesTab[counter] = 1.0f;
                normalsTab[counter]  = 0.0f;
                ++counter;
            }
        }

        #ifdef MODEL_EXPORT_DEBUG
        for(int i=0; i<trianglesNum_*3; ++i) {
            exp << "v:\t";
            for(int j=0; j<4; ++j) {
                exp << verticesTab[(i*4)+j] << "\t";
            }
            exp << "\nn:\t";
            for(int j=0; j<4; ++j) {
                exp << normalsTab[(i*4)+j] << "\t";
            }
            exp << "\n\n";
        }
        #endif

    }
    else {
        verticesTab = new float[trianglesNum_*12]; // 12 = 4 coords * 3 v in triangle
        textureCoordsTab = new float[trianglesNum_*9];
        normalsTab =  new float[trianglesNum_*12];

        for(int i=0; i<trianglesNum_; ++i) {
            for(int j=0; j<3; ++j) {
                v = triangles[(i*9)+(3*j)]-1;
                t = triangles[(i*9)+(3*j)+1]-1;
                n = triangles[(i*9)+(3*j)+2]-1;

                for(int k=0; k<3; ++k) {
                    verticesTab[counter] = vertices[(v*3)+k];
                    normalsTab[counter]  =  normals[(n*3)+k];
                    textureCoordsTab[counter2] = textureCoords[(t*3)+k];
                    ++counter;
                    ++counter2;
                }
                verticesTab[counter] = 1.0f;
                normalsTab[counter]  = 0.0f;
                ++counter;
            }
        }

        #ifdef MODEL_EXPORT_DEBUG
        for(int i=0; i<trianglesNum_*3; ++i) {
            exp << "v:\t";
            for(int j=0; j<4; ++j) {
                exp << verticesTab[(i*4)+j] << "\t";
            }
            exp << "\nt:\t";
            for(int j=0; j<3; ++j) {
                exp << textureCoordsTab[(i*3)+j] << "\t";
            }
            exp << "\nn:\t";
            for(int j=0; j<4; ++j) {
                exp << normalsTab[(i*4)+j] << "\t";
            }
            exp << "\n\n";
        }
        #endif
    }
    #ifdef MODEL_EXPORT_DEBUG
    exp.close();
    #endif
}

string ModelLoader::getFileName() {
    return fileName_;
}

int ModelLoader::getVerticesNum() {
    return verticesNum_;
}

int ModelLoader::getNormalsNum() {
    return normalsNum_;
}

int ModelLoader::getTextureCoordsNum() {
    return textureCoordsNum_;
}

int ModelLoader::getTrianglesNum() {
    return trianglesNum_;
}


#endif // LOADER_H_INCLUDED
