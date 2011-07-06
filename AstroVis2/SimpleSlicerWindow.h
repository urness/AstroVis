#pragma once
#include "GlutCgWindow.h"
#include "globaldefs.h"

class CTransferFunction;

struct Edge {
	public:
		Edge(int i, int j) {nV1=i; nV2=j;}
		Edge() {nV1=0; nV2=0;}
		int nV1, nV2;
};


class CSimpleSlicerWindow :
	public CGlutCgWindow
{
	

public:
	CSimpleSlicerWindow();
	virtual ~CSimpleSlicerWindow(void);

	virtual void keyEvent(unsigned char key,int x,int y);
	void cgRenderGeometry();
	void cgDisplayTF();
	void drawString(char *s);

	virtual bool handleButtonEvent(int button, int state, int x, int y);
	virtual bool handleMoveEvent(int x, int y);

protected:
	int      count;
	//int      m_bDisplayTF;
	GLuint   m_pTextureids[6];
	int      m_nNumSlices;
	CVector  m_pVertices[8];
	Edge     m_pEdges[12];

	CGprogram *m_pVertexPrograms;
	CGprogram *m_pFragmentPrograms;

	CTransferFunction *m_pTransferFunction;
	CTransferFunction *m_pTransferFunction1;
protected:
	bool loadTextures();
	void loadTex1();
	void loadTex2();
	void loadTex3();
	void loadTex4();
	void loadTex5();
	void loadTex6();
	bool createPrograms();

};
