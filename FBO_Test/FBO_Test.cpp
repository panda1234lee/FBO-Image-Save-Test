// FBO_Test.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "GlHelper.h"
#include "GlutHelper.h"

//#include <iostream>
//using namespace std;

int main(int argc, char *argv[])
{
	GlutHelper gluh;
    gluh.setupWindow(argc, argv);

	GlHelper* glh = GlHelper::getSingleInstance();;
	gluh.setGlHelper(glh);
	gluh.setupOthers();

	gluh.display();
	gluh.loop();
    
    return 0;
}
