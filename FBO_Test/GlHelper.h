#pragma once

class GlHelper
{
public:
	~GlHelper();
	
	void setupCamera(void);
	void setupResource(void);
	void render(void);
	void renderToTarget(void);
	void saveCVImage(const char *file_name);

	static GlHelper* getSingleInstance();

private:
	GlHelper();

public:

	const int SCREEN_WIDTH = 800;
	const int SCREEN_HEIGHT = 600;
	const int TEXTURE_WIDTH = 512;
	const int TEXTURE_HEIGHT = 512;
	const double NEAR_PLANE = 1.0f;
	const double FAR_PLANE = 1000.0f;

	int which_frame_buffer; // 0: �Զ����framebuffer�� 1��ϵͳframebuffer�������򲻽�ͼ

private:

	unsigned int fbo ;							// FBO����ľ��
	unsigned int depthbuffer ;
	unsigned int rendertarget ;			// �������ľ��

	static GlHelper* s_glh;
};

