#include "Appearance.h"


Appearance::Appearance() {

}

Appearance::~Appearance() {

}

void Appearance::Init(Geometry geometry, Material material)
{
	_geometry = geometry;
	_material = material;
}

