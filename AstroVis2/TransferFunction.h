#pragma once

#include "CVector.h"
#include "Trapez.h"
#include <Cg/cg.h>
#include <Cg/cgGL.h>
#include "globaldefs.h"
#include <iostream>
#include <string>

using namespace std;


#define MAX_NUM_TRAPEZOIDS (8)


class CTransferFunction;
typedef CTransferFunction *PTRANSFERFUNCTION;

class CTransferFunction{

public:
	CTransferFunction(char *p, int start, int mid1, int mid2, int end, int height);
	virtual ~CTransferFunction();

	virtual void render(int offset, int id);
	virtual void activate(CGprogram fragmentShader, int id);
	virtual bool handleButtonEvent(int button, int state, int x, int y);
	virtual bool handleMoveEvent(int x, int y);
	inline int getMin() {return m_nMin;}
	inline int getMax() {return m_nMax;}

protected:
	unsigned char m_pColorTable[TF_TABLE_SIZE*2];
	unsigned char rgb[768];
	int m_nNumTrapezoids;
	int r[256];
	int g[256];
	int b[256];
	char *name;
	int m_nMin, m_nMax;
	CTrapez m_pTrapezoids[MAX_NUM_TRAPEZOIDS];

private:
	void updateColorTable(int nStart, int nEnd);
	void updateMinMax();
	void initTexture();

	GLuint m_nTexId;
	GLuint m_nTexId1;

};
