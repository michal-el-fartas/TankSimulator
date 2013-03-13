#ifndef CLASS_LIB_H_INCLUDED
#define CLASS_LIB_H_INCLUDED
#include "global_header.h"
using namespace std;

Library <Object*> libObject;
Library <ObjectGroup*> libGroup;

//Grupa konstruktorów kopiujących które tworzą
//kopię obiektu zdefiniowanego pod tą nazwą
Object::Object(string obj): Position()
{
    memGet(this,libObject.get(obj));
    //*this=*libObject.get(obj);
}

ObjectGroup::ObjectGroup(string obj): Position()
{
    memGet(this,libGroup.get(obj));
}


Turret::Turret(string turret_name): ObjectGroup()
{
    memGet(this,libTurret.get(turret_name));
}

Armor::Armor(string name): Object()
{
    memGet(this,libArmor.get(name));
}

class FileLoader
{
    //klasa która odpowiada za ładowanie elementów gry
    //z plików definicji
    string path;

    ShaderProgram *loadShader(ifstream &file)
    {
        string str,src;
        char *vsh=NULL,*fsh=NULL,*gsh=NULL;
        do{
            file >> str;
            if(str=="vsh:")
            {
                file >> src;
                vsh=strclone(path+src);
            }
            else  if(str=="fsh:")
            {
                file >> src;
                fsh=strclone(path+src);
            }
            else  if(str=="gsh:")
            {
                file >> src;
                gsh=strclone(path+src);
            }
        }while(str!="end");
        return new ShaderProgram(vsh,gsh,fsh);
    }

    Model *loadModel(ifstream &file)
    {
        string str,src,sh="default";
        do{
            file >> str;
            if(str=="source:")
            {
                file >> src;
            }
            else if(str=="shader:")
            {
                file >> sh;
            }
        }while(str!="end");
        return new Model(path+src,sh);
    }

    Texture *loadTexture(ifstream &file)
    {
        string str,tex;
        do{
            file >> str;
            if(str=="tex:")
            {
                file >> tex;
            }
        }while(str!="end");
        return new Texture(path+tex);
    }

    Object *loadObject(ifstream &file)
    {
        string str,model,tex;
        float x=0,y=0,z=0,a=0,s=1;
        float scx=0,scy=0,scz=0;
        do{
            file >> str;
            if(str=="model:")
            {
                file >> model;
            }
            else if(str=="texture:")
            {
                file >> tex;
            }
            else if(str=="xyz:")
            {
                file >> x >> y >> z;
            }
            else if(str=="socket:")
            {
                file >> scx >> scy >> scz;
            }
            else if(str=="angle:")
            {
                file >> a;
            }
            else if(str=="scale:")
            {
                file >> s;
            }
        }while(str!="end");
        Object *object=new Object(model,tex);
        object->setLocation(x,y,z);
        object->setSocket(scx,scy,scz);
        object->turn(a);
        object->setScale(s);
        return object;
    }

    ObjectGroup *loadGroup(ifstream &file)
    {
        string str;
        float x=0,y=0,z=0,a=0,s=1;
        float scx=0,scy=0,scz=0;
        do{
            file >> str;
            if(str=="xyz:")
            {
                file >> x >> y >> z;
            }
            else if(str=="angle:")
            {
                file >> a;
            }
            else if(str=="scale:")
            {
                file >> s;
            }
            else if(str=="socket:")
            {
                file >> scx >> scy >> scz;
            }
            else if(str=="contains:")
            {
                break;
            }
        }while(str!="end");
        ObjectGroup *group=new ObjectGroup();
        group->setLocation(x,y,z);
        group->setSocket(scx,scy,scz);
        group->turn(a);
        group->setScale(s);
        while(str!="end")
        {
            file >> str;
            if(str=="object:")
            {
                string obj;
                file >> obj;
                group->add(new Object(obj));
            }
            else if(str=="group:")
            {
                string obj;
                file >> obj;
                group->add(new ObjectGroup(obj));
            }
        }
        return group;
    }

    Turret *loadTurret(ifstream &file)
    {
        Turret *turret=new Turret();
        string str;
        float val;
        do{
            file >> str;
            //cout << str << endl;
            if(str=="type:")
            {
                file >> str;
                if(str=="parallel")
                    {turret->mode=PARALLEL;}
                else if(str=="serial")
                    {turret->mode=SERIAL;}
            }
            else if(str=="gun:"||str=="gun1:")
            {
                file >> str;
                turret->gun1=new Object(str);
            }
            else if(str=="gun2:")
            {
                file >> str;
                turret->gun2=new Object(str);
            }
            else if(str=="base:")
            {
                file >> str;
                turret->base=new Object(str);
            }
            else if(str=="scale:")
            {
                file >> val;
                turret->setScale(val);
            }
            else if(str=="recoil:")
            {
                file >> turret->recoil;
            }
            else if(str=="reload:")
            {
                file >> turret->reloadTime;
            }
            else if(str=="turnSpeed:")
            {
                file >> turret->tSpeed;
            }
            else if(str=="spinSpeed:")
            {
                file >> turret->sSpeed;
            }
            else if(str=="riseSpeed:")
            {
                file >> turret->rSpeed;
            }
            else if(str=="riseAngle:")
            {
                file >> turret->rAngle;
            }
            else if(str=="bullet:")
            {
                file >> turret->ammo;
            }
            else if(str=="range:")
            {
                file >> turret->range;
            }
            else if(str=="fast:")
            {
                file >> turret->fastFire;
            }
            else if(str=="slow:")
            {
                file >> turret->slowFire;
            }
        }while(str!="end");

        turret->init();
        return turret;
    }

    Armor *loadArmor(ifstream &file)
    {
        Armor *armor=NULL;
        string str;
        float w,hp;
        do{
            file >> str;
            if(str=="object:")
            {
                file >> str;
                armor=new Armor(*libObject.get(str));
            }
            else if(str=="weight:")
            {
                file >> w;
            }
            else if(str=="hp:")
            {
                file >> hp;
            }
        }while(str!="end");

        armor->hp=hp;
        armor->weight=w;
        return armor;
    }


    public:
    FileLoader(string fname,string fpath="")
    {
        path=fpath+getPath(fname);
        fname=getFileName(fname);
        ifstream file;
        string str,def;
        file.open((path+fname).c_str());
        if(file.is_open())
            {cout << tab << "loading file " << fname << "..." << endl;}
        else
            {cout << tab << fname <<" does not exist!!!" << endl;}
        inTab();
        while(!file.eof())
        {
            bool confirm=true;
            file >> str;
            if(str=="include")
            {
                file >> def;
                new FileLoader(def,path);
                confirm=false;
            }
            else if(str=="shader")
            {
                file >> def;
                cout << tab << "loading " << def << endl;
                inTab();
                libShader.add(def,loadShader(file));
                outTab();
                confirm=false;
            }
            else if(str=="model")
            {
                file >> def;
                cout << tab << "loading " << def << " ";
                libModel.add(def,loadModel(file));
            }
            else if(str=="texture")
            {
                file >> def;
                cout << tab << "loading " << def << " ";
                libTexture.add(def,loadTexture(file));
            }
            else if(str=="object")
            {
                file >> def;
                cout << tab << "loading " << def << " ";
                libObject.add(def,loadObject(file));
            }
            else if(str=="group")
            {
                file >> def;
                cout << tab << "loading " << def << " ";
                libGroup.add(def,loadGroup(file));
            }
            else if(str=="turret")
            {
                file >> def;
                cout << tab << "loading " << def << " ";
                libTurret.add(def,loadTurret(file));
            }
            else if(str=="armor")
            {
                file >> def;
                cout << tab << "loading " << def << " ";
                libArmor.add(def,loadArmor(file));
            }
            if(confirm)
                {cout << "OK" << endl;}
        }
        outTab();
        cout << tab << fname <<" loaded" << endl << endl;
        file.close();
    }
};

#endif // CLASS_LIB_H_INCLUDED
