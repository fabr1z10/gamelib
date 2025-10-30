#pragma once

#include <GL/glew.h>
#include <vector>
#include <list>
#include <memory>
#include "gamelib/camera.h"

class IShader;
class SpriteSheet;
class IModel;

class IBatch {
public:
	IBatch(IShader* shader, Camera* camera);

	virtual ~IBatch() = default;

	virtual void setupUniforms() = 0;

	virtual void configure() = 0;

	virtual void draw() = 0;

	virtual void startShader();

	int getPrimitiveId();

	void releasePrimitiveId(int);

	virtual const std::type_info& vertexType() const = 0;

	void addSpriteSheet(std::shared_ptr<SpriteSheet> texture);

	const SpriteSheet* getSpriteSheet() const;

	std::shared_ptr<IModel> getModel(const std::string& id);

protected:
	GLuint _vao;
	GLuint _vbo;
	GLuint _ebo;
	// primitive type used for rendering
	GLenum _prim;
	IShader* _shader;
	Camera* _cam;
	int _nPrimitive;					// next element to be allocated
	std::list<int> _deallocated;		// list of element id to recycle
	std::shared_ptr<SpriteSheet> _spriteSheet;
};

inline const SpriteSheet *IBatch::getSpriteSheet() const {
	return _spriteSheet.get();
}

