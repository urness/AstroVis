#pragma once

#include "CVector.h"
#include "CPosition.h"
//#include <windows.h>

class CArcBall;

class CGlutWindow
{
public:
	CGlutWindow(void);
	~CGlutWindow(void);

	enum {
		MOVE_CAMERA,
		MOVE_LIGHT,
	};

	enum {
		MBUTTON_LEFT   = 0x01, 
		MBUTTON_MIDDLE = 0x02,
		MBUTTON_RIGHT  = 0x04,
		KBUTTON_CTRL   = 0x08,
		KBUTTON_SHIFT  = 0x10
	};

	virtual void renderFrame();
	virtual void idle();
	virtual void resize(int width, int height);
	virtual void keyEvent(unsigned char key,int x,int y);
	virtual void mouseButtonEvent(int button, int state,int x,int y);
	virtual void mouseMoveEvent(int x,int y);
	virtual void renderGeometry();
	virtual void displayTF();
	virtual bool handleButtonEvent(int button, int state, int x, int y);
	virtual bool handleMoveEvent(int x, int y);

protected:
	int m_nWidth, m_nHeight;
	int main_window;
	double m_dFieldOfView;
	double m_dCenter, m_dRadius, m_dZoom, dZoom;
	
	CArcBall* m_pCameraArcball;
	CArcBall* m_pLightArcball;

	CPosition m_vecMouseDown;
	bool m_bDragging;
	bool m_bZooming;
	bool m_bRotating;
	bool m_bExternalMouseHandling;
	
	CVector m_vecCameraPosition;
	double m_dTranslateX, m_dTranslateY;

	int m_nMouseState;
	int m_nInteractionMode;

	int r_div_h;
	double xCent, yCent, zCent;

	void initializeGL();

};



