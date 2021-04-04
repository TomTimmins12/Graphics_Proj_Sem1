///////////////////////////////////////////////
//Filename: ObjectClass.h
///////////////////////////////////////////////
#pragma once
//////////////
// INCLUDES //
//////////////
#include "Structures.h"

#include "Appearance.h"
#include "Transform.h"

using namespace std;

///////////////////////////////////////////////
//Classname: ObjectClass
///////////////////////////////////////////////
class GameObj
{
public:
    GameObj(string type, Appearance* appearance, Transform* transform);
    ~GameObj();

    string GetType() const { return _type; }

    Appearance* GetAppearance() const { return _appearance; }
    Transform* GetTransform() const { return _transform; }

    void Update(const float deltaTime);
    void Draw(ID3D11DeviceContext* pImmediateContext);

private:
    string _type;
    Appearance* _appearance;
    Transform* _transform;

};

