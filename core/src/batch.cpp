#include "gamelib/batch.h"
#include "gamelib/shader.h"


IBatch::IBatch(IShader* shader) : _shader(shader) {
}


void IBatch::startShader() {
	_shader->use();
}