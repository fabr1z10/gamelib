#pragma once

#include "gamelib/dynamic_batch.h"


template<typename VERTEX, typename PRIMITIVE>
class AGIBatch : public DynamicBatch<VERTEX, PRIMITIVE> {

public:
	using DynamicBatch<VERTEX, PRIMITIVE>::DynamicBatch;

	void addPriority(std::shared_ptr<Tex> texture) {
		_priority = texture;
	}

	virtual void setupUniforms() {
		DynamicBatch<VERTEX, PRIMITIVE>::setupUniforms();

		// add priority texture
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, _priority->getId());
		GLint locPriority = glGetUniformLocation(IBatch::_shader->getProgramId(), "uPriorityTex");
		glUniform1i(locPriority, 2);
	}
private:
	std::shared_ptr<Tex> _priority;
};




