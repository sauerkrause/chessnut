#ifndef __CHESS_H_INCLUDED__
#define __CHESS_H_INCLUDED__

#include "Common.h"
#include "Renderer.h"

#include "PieceTypes.h"
#include <vector>

class Chess : UniversalInterface<Chess>
{
public:
	Chess();
	~Chess();
	bool Init();
	bool Process();
	void Release();
	bool SelectPiece(int x, int y);
	int PieceAt(int x, int y);
	bool MovePiece(int x, int y);

private:
	GamePiece* m_pieces[32];
	int m_nSelectedPiece;
	
};


#endif