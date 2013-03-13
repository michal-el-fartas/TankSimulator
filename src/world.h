#ifndef WORLD_H_INCLUDED
#define WORLD_H_INCLUDED
using namespace std;

class World: public ObjectGroup
{
    //klasa która jest odpowiedzialna za 'stworzenie świata'
    //oraz ustawienie odpowiedniej płaszczyzny odcięcia w macierzy
    //perspektywy tak żeby obejmowała cały świat
    float size,bounds;
    ObjectGroup map;
    public:
    World(float size,string ground="Ground",string skybox="Skybox"): ObjectGroup()
    {
        this->size=size;
        map.add(new Object(ground));
        map.add(new Object(skybox));
        map.setScale(size);
        add(&map);
        bounds=0.8*size;
        sightReach=2*size*sqrt(3);
        setupProjection();
    }
    void setBounds(float bound)
        {bounds=bound;}
    bool inBounds(glm::vec3 point)
    {
        bool xOK=((bounds>=point.x)&&(point.x>=(-bounds)));
        bool yOK=((bounds>=point.y)&&(point.y>=0));
        bool zOK=((bounds>=point.z)&&(point.z>=(-bounds)));

        return (xOK and yOK and zOK);
    }
    bool isVisible(glm::vec3 point)
    {
        bool xOK=((size>=point.x)&&(point.x>=(-size)));
        bool yOK=((size>=point.y)&&(point.y>=0));
        bool zOK=((size>=point.z)&&(point.z>=(-size)));

        return (xOK and yOK and zOK);
    }
} *world;

// inBounds zwraca true jeżeli obiekt znajduje się
// w wyznaczonym rejonie - domyślnie 80% wewnętrznej części świata
bool inBounds(glm::vec3 point)
{
    world->inBounds(point);
}
bool inBounds(glm::vec4 point)
{
    inBounds(vec4to3(point));
}
bool inBounds(glm::mat4 object)
{
    inBounds(object*glm::vec4(0,0,0,1));
}

// isVisible zwraca true jeżeli obiekt znajduje się w świecie
// czyli że nie przekroczył skybox'a
bool isVisible(glm::vec3 point)
{
    world->isVisible(point);
}
bool isVisible(glm::vec4 point)
{
    isVisible(vec4to3(point));
}
bool isVisible(glm::mat4 object)
{
    isVisible(object*glm::vec4(0,0,0,1));
}

#endif // WORLD_H_INCLUDED
