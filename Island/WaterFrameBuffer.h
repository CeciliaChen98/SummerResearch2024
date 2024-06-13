#ifndef WATERFRAMEBUFFER_H
#define WATERFRAMEBUFFER_H
#include "Mesh.h"

class WaterFrameBuffer {
private:
	const int reflectionWidth = 800;
	const int reflectionHeight = 600;
	const int refractionWidth = 800;
	const int refractionHeight = 600;

	GLuint reflectionFrameBuffer, reflectionDepthBuffer;
	GLuint refractionFrameBuffer, refractionDepthTexture;
	Texture* reflectionTexture, * refractionTexture;

	GLuint CreateFrameBuffer() {
		GLuint frameBuffer;
		glGenFramebuffers(1, &frameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);

		return frameBuffer;
	}
	GLuint CreateDepthTextureAttachment(int width, int height) {
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture, 0);

		return texture;
	}
	GLuint CreateDepthBufferAttachment(int width, int height) {
		GLuint depthBuffer;
		glGenRenderbuffers(1, &depthBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

		return depthBuffer;
	}
	void BindFrameBuffer(GLuint buffer, int width, int height) {
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, buffer);
		glViewport(0, 0, width, height);
	}

public:
	WaterFrameBuffer() {
		reflectionFrameBuffer = CreateFrameBuffer();
		reflectionTexture = CreateTextureAttachment(reflectionWidth, reflectionHeight);
		reflectionDepthBuffer = CreateDepthBufferAttachment(reflectionWidth, reflectionHeight);
		UnbindBuffer();

		refractionFrameBuffer = CreateFrameBuffer();
		refractionTexture = CreateTextureAttachment(refractionWidth, refractionHeight);
		refractionDepthTexture = CreateDepthTextureAttachment(refractionWidth, refractionHeight);
		UnbindBuffer();
	}

	~WaterFrameBuffer() {
		glDeleteFramebuffers(1, &reflectionFrameBuffer);
		glDeleteFramebuffers(1, &refractionFrameBuffer);
		glDeleteRenderbuffers(1, &reflectionDepthBuffer);
		glDeleteTextures(1, &refractionDepthTexture);

		delete reflectionTexture;
		delete refractionTexture;
	}

	void BindReflectionBuffer() {
		BindFrameBuffer(reflectionFrameBuffer, reflectionWidth, reflectionHeight);
	}
	void BindRefractionBuffer() {
		BindFrameBuffer(refractionFrameBuffer, refractionWidth, refractionHeight);
	}
	void UnbindBuffer() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, 800, 600);
	}
	Texture* GetReflectionTexture() {
		return reflectionTexture;
	}
	Texture* GetRefractionTexture() {
		return refractionTexture;
	}
	GLuint GetRefractionDepthTexture() {
		return refractionDepthTexture;
	}
};

#endif