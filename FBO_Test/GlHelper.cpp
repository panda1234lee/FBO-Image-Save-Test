#include "stdafx.h"

#include <GL/glew.h>
#include <GL/glu.h>
#include <opencv2/opencv.hpp>

#include "GlHelper.h"

GlHelper* GlHelper::s_glh = NULL;

GlHelper::GlHelper(): fbo(0), depthbuffer(0), rendertarget(0)
{
}

GlHelper::~GlHelper()
{
}

// ��ʼ�������
void GlHelper::setupCamera(void)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (double)SCREEN_WIDTH / (double)SCREEN_HEIGHT, NEAR_PLANE, FAR_PLANE);
	gluLookAt(5, 5, 5, 0, 0, 0, 0, 1, 0);

	// ���ֱ任Ӧ����GL_MODELVIEWģʽ�½���
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Z-buffer
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// ����2D��ͼ
	glEnable(GL_TEXTURE_2D);
}

// ��ʼ����������
void GlHelper::setupResource(void)
{
	// ��������
	glGenTextures(1, &rendertarget);
	glBindTexture(GL_TEXTURE_2D, rendertarget);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// ������Ȼ�����
	glGenRenderbuffersEXT(1, &depthbuffer);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthbuffer);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, TEXTURE_WIDTH, TEXTURE_HEIGHT);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

	// ����FBO����
	glGenFramebuffersEXT(1, &fbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, rendertarget, 0);
	//glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthbuffer);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
	{
		std::cout << "Failed to create framebuffer ! " << std::endl;
	}
}

// ��Ⱦ������
void GlHelper::render(void)
{
	// ��Ĭ��FBO������֡��������ID��0��
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glBindTexture(GL_TEXTURE_2D, rendertarget);
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// ��Ⱦ
	glClearColor(0, 0, 1, 0);	// ��ɫ
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// --------------------------------------------------------
	glBegin(GL_POLYGON);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor3f(1, 1, 1);	// ��ɫ
						// ����
	glTexCoord2f(1, 1);
	glVertex3d(1, 1, 0);

	glTexCoord2f(0, 1);
	glVertex3d(-1, 1, 0);

	glTexCoord2f(0, 0);
	glVertex3d(-1, -1, 0);

	glTexCoord2f(1, 0);
	glVertex3d(1, -1, 0);
	glEnd();
	// --------------------------------------------------------

	// ��
	if (1 == which_frame_buffer)
		saveCVImage("../image_1.bmp");	// �м���glutSwapBuffer֮ǰglReadPixels

}

// ��Ⱦ������
void GlHelper::renderToTarget(void)
{
	glBindTexture(GL_TEXTURE_2D, 0); // ȡ���󶨣���Ϊ�����ȡ������Ⱦ�������ʱ���ʹ��������

									 // ����ȾĿ��
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
	glViewport(0, 0, TEXTURE_WIDTH, TEXTURE_HEIGHT);

	// ��Ⱦ
	glClearColor(1, 1, 0, 1);	// ��ɫ
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// --------------------------------------------------------
	glBegin(GL_POLYGON);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// ��ɫ������
	glColor4f(1, 0, 0, 1);
	glVertex3d(0, 1, 0);
	glVertex3d(-1, -1, 0);
	glVertex3d(1, -1, 0);

	glEnd();
	// --------------------------------------------------------
	// ��
	if(0 == which_frame_buffer)
		saveCVImage("../image_0.bmp");	// �м���glutSwapBuffer֮ǰglReadPixels

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

// ��ȡ��ǰ�󶨵�FrameBuffer��������ͼƬ
void GlHelper::saveCVImage(const char *file_name)
{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	int width = viewport[2];
	int height = viewport[3];

	// �������صĶ����ʽ����Word����(4�ֽ�)
	glPixelStorei(GL_PACK_ALIGNMENT, 4);
	// �����������ʵ���
	int nAlignWidth = (width * 24 + 31) / 32;
	// ���仺����
	unsigned char *pdata = new unsigned char[nAlignWidth * height * 4];
	memset(pdata, 0, nAlignWidth * height * 4);

	// �ӵ�ǰ�󶨵� frame buffer ��ȡ pixels  
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pdata);

	cv::Mat src = cv::Mat(height, width, CV_8UC3, pdata);
	cv::Mat bgr;
	cvtColor(src, bgr, CV_RGB2BGR);
	cv::Mat dst;
	flip(bgr, dst, 0);	// 0:x; 1:y, -u:x,y;
	imwrite(file_name, dst);
}

GlHelper* GlHelper::getSingleInstance()
{
	if (NULL == s_glh)
	{
		s_glh = new GlHelper();
	}
	else
		return s_glh;
}
