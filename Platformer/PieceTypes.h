#ifndef __PIECETYPE_H_INCLUDED__
#define __PIECETYPE_H_INCLUDED__

#include "Renderer.h"
#include "Common.h"

enum Type 
{
	Pawn, Rook, Knight, Bishop, Queen, King
};
enum Team { Black, White };


class GamePiece
{
public:
	Type pieceType;
	Team pieceTeam;
	bool alive;
	int xPos;
	int yPos;
	bool selected;
	bool hasMoved;

	GamePiece()
	{
		alive = false;
		xPos = 0;
		yPos = 0;
		selected = false;
		hasMoved = false;
		Move(0,0);
	}

	virtual bool Move(int x, int y);

	virtual bool CanMove(int x,int y);

	virtual void Render()
	{
		gRenderer->RenderTileByXY(xPos,yPos,(int)pieceType * 2 + (int)pieceTeam + 5 );
		if(selected)
			gRenderer->RenderTileByXY(xPos,yPos, 3);
	}
	virtual void Render(int tileNumber)
	{
		gRenderer->RenderTileByXY(xPos,yPos,tileNumber);
	}

	bool IsSelected()
	{
		return selected;
	}

	void Kill()
	{
		alive = false;
	}

};

class PawnPiece : GamePiece
{
public:
	PawnPiece();
//	bool CanMove(int x,int y, bool firstMove = false);
};

class RookPiece : GamePiece
{
public:
	RookPiece();
//	bool CanMove(int x,int y);
	bool CanCastle();
	bool Move(int x, int y, bool firstMove = false);
private:
	bool m_bCanCastle;
};

class KnightPiece : GamePiece
{
public:
	KnightPiece();
//	bool CanMove(int x,int y);
};

class BishopPiece : GamePiece
{
public:
	BishopPiece();
//	bool CanMove(int x,int y);
};

class QueenPiece : GamePiece
{
public:
	QueenPiece();
//	bool CanMove(int x,int y);
};

class KingPiece : GamePiece
{
public:
	KingPiece();
//	bool CanMove(int x,int y);
	bool CanCastle();
	bool Move(int x, int y, bool firstMove = false);
private:
	bool m_bCanCastle;
};

#endif