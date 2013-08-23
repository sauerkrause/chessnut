#include "PieceTypes.h"


bool GamePiece::Move(int x, int y)
{
	xPos = x;
	yPos = y;
	hasMoved = true;
#if 0
	if(pieceType == Rook)
		static_cast<RookPiece *>(this)->Move(x,y);
	if(pieceType == King)
		static_cast<KingPiece *>(this)->Move(x,y);
#endif
	return true;
}


bool GamePiece::CanMove(int x,int y)
{
	if(x == xPos && y == yPos)
		return false;
	if(x < 0 || x > 8 || y < 0 || y > 8)
		return false;
	return true;
#if 0
	switch(pieceType)
	{
	case Pawn:
		return static_cast<PawnPiece *>(this)->CanMove(x,y,true);
		break;
	case Rook:
		return static_cast<RookPiece *>(this)->CanMove(x,y);
		break;
	case Bishop:
		return static_cast<BishopPiece *>(this)->CanMove(x,y);
		break;
	case Queen:
		return static_cast<QueenPiece *>(this)->CanMove(x,y);
		break;
	case Knight:
		return static_cast<KnightPiece *>(this)->CanMove(x,y);
		break;
	case King:
		return static_cast<KingPiece *>(this)->CanMove(x,y);
		break;
	}
#endif
}


PawnPiece::PawnPiece()
{
	pieceType = Pawn;
}

#if 0
bool PawnPiece::CanMove(int x,int y, bool firstMove)
{
	if(x == xPos)
	{
		if(pieceTeam == White)
		{
			if(yPos - y == 1 || (yPos - y == 2 && firstMove))
				return true;
		}
		else
		{
			if(yPos - y == -1 || (yPos - y == -2 && firstMove))
				return true;
		}
	}
	return false;
}
#endif

RookPiece::RookPiece()
{
	pieceType = Rook;
	m_bCanCastle = true;
}


#if 0
bool RookPiece::CanMove(int x,int y)
{
	if(x == xPos || y == yPos)
		return true;
	return false;
}
#endif

bool RookPiece::Move(int x, int y, bool firstMove)
{
	xPos = x;
	yPos = y;
	m_bCanCastle = false;
	return true;
}

KnightPiece::KnightPiece()
{
	pieceType = Knight;
}

#if 0
bool KnightPiece::CanMove(int x,int y)
{
	int xDisp = abs(x-xPos);
	int yDisp = abs(y-yPos);

	if((xDisp == 1 && yDisp == 2) || (xDisp == 2 && yDisp == 1))
		return true;

	return false;
}
#endif

BishopPiece::BishopPiece()
{
	pieceType = Bishop;
}
#if 0
bool BishopPiece::CanMove(int x,int y)
{

	int xDisp = abs(x-xPos);
	int yDisp = abs(y-yPos);
	if(xDisp == yDisp)
		return true;
	return false;
}
#endif

QueenPiece::QueenPiece()
{
	pieceType = Queen;
}

#if 0
bool QueenPiece::CanMove(int x,int y)
{
	
	int xDisp = abs(x-xPos);
	int yDisp = abs(y-yPos);

	if(yDisp == 0 || xDisp == 0 || xDisp == yDisp)
		return true;
	return false;
}
#endif

KingPiece::KingPiece()
{
	pieceType = King;
	m_bCanCastle = true;
}

#if 0
bool KingPiece::CanMove(int x,int y)
{
	
	int xDisp = abs(x-xPos);
	int yDisp = abs(y-yPos);

	if(xDisp <= 1 && yDisp <= 1)
		return true;

	return false;
}
#endif

bool KingPiece::Move(int x, int y, bool firstMove)
{
	xPos = x;
	yPos = y;
	m_bCanCastle = false;
	return true;
}