#pragma once

#define GLOBAL_MULT (0.0001)

Vector3 getRealCoordinates(Vector3 coord);

class Renderable
{
public:
  enum RenderableType {UnKnownType, AstralType, DynamicType, PlanetType, StarType, ShipType, StationType};

  Renderable();
  virtual ~Renderable();
  virtual void render() = 0;
  virtual bool isStatic() = 0;
  virtual bool isLanded() {return false;}
  virtual Vector3 getCoord() = 0;
  virtual void setCoord(Vector3 coord) = 0;
  virtual CString getName() = 0;
  virtual Vector3 getVelocity() = 0;
  RenderableType getType() const {return type_;}
protected:
  RenderableType type_;
};
