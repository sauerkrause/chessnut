#include "Chess.h"

Chess::Chess()
{
}

Chess::~Chess()
{
}

bool Chess::Init()
{
	for(int i = 0; i < 16; i++)
	{
		if(i < 8)
		{
			m_pieces[i] = (GamePiece *)new PawnPiece();
			m_pieces[i]->pieceTeam = White;
			m_pieces[i]->Move(i,6);
		}
		else
		{
			m_pieces[i] = (GamePiece *)new PawnPiece();
			m_pieces[i]->pieceTeam = Black;
			m_pieces[i]->Move(i-8,1);
		}

		m_pieces[i]->alive = true;
		m_pieces[i]->selected = false;
		m_pieces[i]->pieceType = Pawn;

	}
	for(int i = 16; i < 24; i++)
	{
		switch(i%8)
		{
		case 0:
		case 7:
			m_pieces[i] = (GamePiece *)new RookPiece();
			break;
		case 1:
		case 6:
			m_pieces[i] = (GamePiece *)new KnightPiece();
			break;
		case 2:
		case 5:
			m_pieces[i] = (GamePiece *)new BishopPiece();
			break;
		case 3:
			m_pieces[i] = (GamePiece *)new QueenPiece();
			break;
		case 4:
			m_pieces[i] = (GamePiece *)new KingPiece();
			break;

		}
		m_pieces[i]->selected = false;
		m_pieces[i]->pieceTeam = White;
		m_pieces[i]->alive = true;
		m_pieces[i]->Move(i-16, 7);

	}
	for(int i = 24; i < 32; i++)
	{
		switch(i%8)
		{
		case 0:
		case 7:
			m_pieces[i] = (GamePiece *)new RookPiece();
			break;
		case 1:
		case 6:
			m_pieces[i] = (GamePiece *)new KnightPiece();
			break;
		case 2:
		case 5:
			m_pieces[i] = (GamePiece *)new BishopPiece();
			break;
		case 3:
			m_pieces[i] = (GamePiece *)new QueenPiece();
			break;
		case 4:
			m_pieces[i] = (GamePiece *)new KingPiece();
			break;

		}
		m_pieces[i]->selected = false;
		m_pieces[i]->pieceTeam = Black;
		m_pieces[i]->alive = true;
		m_pieces[i]->Move(i-24, 0);

	}
	return true;
}

bool Chess::Process()
{
	for(int i = 0; i < 32; i++)
	{
		if(m_pieces[i]->alive)
		{
			m_pieces[i]->Render();
			if(m_pieces[i]->pieceType == Rook)
			{
				if(m_pieces[i]->IsSelected())
					DEBUGPRINTF("Rook status: %d %d %d %d %d\n", (int)m_pieces[i]->xPos, (int)m_pieces[i]->yPos, (int)m_pieces[i]->alive, (int)m_pieces[i]->selected, (int)m_pieces[i]->pieceTeam);
			}
		}		
	}

	int x,y;
	//#pragma omp parallel num_threads(4)
	//#pragma omp parallel for private(x,y)
	for(int i = 0; i < 32; i++)
	{
		if(m_pieces[i]->selected)
		{
			m_pieces[i]->Render(2);
			for(x = 0; x < 8; x++)
			{
				for(y = 0; y < 8; y++)
				{
					int indexOfPiece = PieceAt(x,y) - 1;

					if(m_pieces[i]->CanMove(x,y))
					{
						if(indexOfPiece != -1)
						{
							if(m_pieces[i]->pieceTeam != m_pieces[indexOfPiece]->pieceTeam)
								gRenderer->RenderTileByXY(x,y,4);
						}
						if(indexOfPiece == -1)
						{
							gRenderer->RenderTileByXY(x,y,4);
						}
					}
				}
			}
		}
	}


	return true;
}
int Chess::PieceAt(int x, int y)
{
	for(int i = 0; i < 32; i++)
	{
		if(m_pieces[i]->alive)
		{
			if(m_pieces[i]->xPos == x && m_pieces[i]->yPos == y)
			{
				return i + 1;
			}
		}
	}
	return 0;
}

bool Chess::SelectPiece(int x, int y)
{
	bool haveSelected = false;
	if(x == -1 && y == -1)
	{
		for(int i = 0; i < 32; i++)
		{
			if(m_pieces[i]->selected)
			{
				DEBUGPRINTF("Deselected %d\n", (int)m_pieces[i]->pieceType);
				m_pieces[i]->selected = false;
				
				return haveSelected;
			}
		}
	}

	for(int i = 0; i < 32; i++)
	{
		
		m_pieces[i]->selected = false;
		if(m_pieces[i]->xPos == x && m_pieces[i]->yPos == y && m_pieces[i]->alive)
		{
			DEBUGPRINTF("Selecting %d\n", (int)m_pieces[i]->pieceType);
			m_pieces[i]->selected = true;
			haveSelected = true;
			
		}

	}
	return haveSelected;

	
}

bool Chess::MovePiece(int x, int y)
{
	for(int i = 0; i < 32; i++)
	{
		if(m_pieces[i]->alive && m_pieces[i]->CanMove(x,y) && m_pieces[i]->selected)
		{
			int possibleKill = PieceAt(x,y);
			if(possibleKill)
			{
				if(m_pieces[possibleKill - 1]->pieceTeam == m_pieces[i]->pieceTeam)
					return false;
			}
			if(possibleKill != 0 && possibleKill - 1 != i)
			{
				m_pieces[possibleKill - 1]->Kill();
			}
			m_pieces[i]->Move(x,y);
			m_pieces[i]->selected = false;
			SelectPiece(-1,-1);
			return true;
		}
	}
	return false;
}

void Chess::Release()
{
	delete * m_pieces;
}