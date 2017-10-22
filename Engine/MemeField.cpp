#include "MemeField.h"

#include <assert.h>
#include <random>
#include "SpriteCodex.h"

void MemeField::Tile::SpawnMeme()
{
	assert( !hasMeme );
	hasMeme = true;
}

void MemeField::Tile::Draw( const Vei2& screenPos, GameState gameState, Graphics& gfx ) const
{
	if( gameState == GameState::Running )
	{
		switch ( state )
		{
		case State::Hidden:
			SpriteCodex::DrawTileButton( screenPos, gfx );
			break;
		case State::Flagged:
			SpriteCodex::DrawTileButton( screenPos, gfx );
			SpriteCodex::DrawTileFlag( screenPos, gfx );
			break;
		case State::Revealed:
			if ( !HasMeme() )
			{
				SpriteCodex::DrawTileNumber( screenPos, nNeighborMemes, gfx );
			}
			else
			{
				SpriteCodex::DrawTileMeme( screenPos, gfx );
			}
			break;
		}
	}
	else if ( gameState == GameState::Loose ) // we are fucked
	{
		switch ( state )
		{
		case State::Hidden:
			if ( HasMeme() )
			{
				SpriteCodex::DrawTileMeme( screenPos, gfx );
			}
			else
			{
				SpriteCodex::DrawTileButton( screenPos, gfx );
			}
			break;
		case State::Flagged:
			if ( HasMeme() )
			{
				SpriteCodex::DrawTileMeme( screenPos, gfx );
				SpriteCodex::DrawTileFlag( screenPos, gfx );
			}
			else
			{
				SpriteCodex::DrawTileMeme( screenPos, gfx );
				SpriteCodex::DrawTileCross( screenPos, gfx );
			}
			break;
		case State::Revealed:
			if ( !HasMeme() )
			{
				SpriteCodex::DrawTileNumber( screenPos, nNeighborMemes, gfx );
			}
			else
			{
				SpriteCodex::DrawTileMemeRed( screenPos, gfx );
			}
			break;
		}
	}
	else if ( gameState == GameState::Win ) // winrar
	{
		switch ( state )
		{
		case State::Hidden:
			SpriteCodex::DrawTileButton( screenPos, gfx );
			break;
		case State::Flagged:
			SpriteCodex::DrawTileMeme( screenPos, gfx );
			SpriteCodex::DrawTileFlag( screenPos, gfx );
			break;
		case State::Revealed:
			if ( !HasMeme() )
			{
				SpriteCodex::DrawTileNumber( screenPos, nNeighborMemes, gfx );
			}
			else
			{
				SpriteCodex::DrawTileMeme( screenPos, gfx );
			}
			break;
		}
	}
}

void MemeField::Tile::Reveal( int& remainingHidden )
{
	assert( state == State::Hidden );
	state = State::Revealed;
	remainingHidden--;
}

bool MemeField::Tile::HasMeme() const
{
	return hasMeme;
}

bool MemeField::Tile::IsRevealed() const
{
	return state == State::Revealed;
}

void MemeField::Tile::ToggleFlag()
{
	assert( !IsRevealed() );
	if ( state == State::Hidden )
	{
		state = State::Flagged;
	}
	else
	{
		state = State::Hidden;
	}
}

bool MemeField::Tile::IsFlagged() const
{
	return state == State::Flagged;
}

bool MemeField::Tile::HasNoNeighborMemes() const
{
	return nNeighborMemes == 0;
}

void MemeField::Tile::SetNeighborMemeCount( int memeCount )
{
	assert( nNeighborMemes == -1 );
	nNeighborMemes = memeCount;
}

MemeField::MemeField( int nMemes, Vei2 pos )
	:
	fieldPos(pos - Vei2( width * SpriteCodex::tileSize / 2, height * SpriteCodex::tileSize / 2 )),
	remainingMemes(nMemes),
	remainingHidden( width * height - nMemes )
{
	assert( nMemes > 0 && nMemes < width * height );
	std::random_device rd;
	std::mt19937 rng( rd() );
	std::uniform_int_distribution<int> xDist( 0, width - 1 );
	std::uniform_int_distribution<int> yDist( 0, height - 1 );

	for ( int nSpawned = 0; nSpawned < nMemes; ++nSpawned )
	{
		Vei2 spawnPos;
		do
		{
			spawnPos = { xDist( rng ), yDist( rng ) };
		} while ( TileAt( spawnPos ).HasMeme() );
		{
			TileAt( spawnPos ).SpawnMeme();
		}
	}

	for ( Vei2 gridPos = { 0, 0 }; gridPos.y < height; gridPos.y++ )
	{
		for ( gridPos.x = 0; gridPos.x < width; gridPos.x++ )
		{
			TileAt( gridPos ).SetNeighborMemeCount( CountNeighborMemes( gridPos ) );
		}
	}
}

void MemeField::Draw( Graphics& gfx ) const
{
	gfx.DrawRect( GetRect(), SpriteCodex::baseColor );
	for ( Vei2 gridPos = { 0, 0 }; gridPos.y < height; gridPos.y++ )
	{
		for ( gridPos.x = 0; gridPos.x < width; gridPos.x++ )
		{
			TileAt( gridPos ).Draw( fieldPos + gridPos * SpriteCodex::tileSize, gameState, gfx );
		}
	}
	if ( gameState == GameState::Win )
	{
		SpriteCodex::DrawWin( { gfx.ScreenWidth / 2, gfx.ScreenHeight / 2 }, gfx );
	}
}

RectI MemeField::GetRect() const
{
	return RectI( fieldPos.x, fieldPos.x + width * SpriteCodex::tileSize, fieldPos.y, fieldPos.y + height * SpriteCodex::tileSize );
}

void MemeField::OnRevealClick( const Vei2& screenPos )
{
	if( gameState == GameState::Running )
	{
		const Vei2 gridPos = ScreenToGrid( screenPos );
		assert( gridPos.x >= 0 && gridPos.x < width && gridPos.y >= 0 && gridPos.y < height );
		RevealTile( gridPos );
		if ( remainingHidden == 0 && remainingMemes == 0 )
		{
			gameState = GameState::Win;
		}
	}
}

void MemeField::OnFlagClick( const Vei2& screenPos )
{
	if( gameState == GameState::Running )
	{
		const Vei2 gridPos = ScreenToGrid( screenPos );
		assert( gridPos.x >= 0 && gridPos.x < width && gridPos.y >= 0 && gridPos.y < height );
		Tile& tile = TileAt( gridPos );
		if ( !tile.IsRevealed() )
		{
			if ( !tile.IsFlagged() && tile.HasMeme() )
			{
				remainingMemes--;
				if ( remainingMemes == 0 && remainingHidden == 0)
				{
					gameState = GameState::Win;
				}
			}
			else if ( tile.IsFlagged() && tile.HasMeme() )
			{
				remainingMemes++;
			}
			tile.ToggleFlag();
		}
	}
}

int MemeField::CountNeighborMemes( const Vei2& gridPos )
{
	const int xStart = std::max( 0, gridPos.x - 1 );
	const int yStart = std::max( 0, gridPos.y - 1 );
	const int xEnd = std::min( width - 1, gridPos.x + 1 );
	const int yEnd = std::min( height - 1, gridPos.y + 1 );
	
	int count = 0;
	for ( Vei2 gridPos = { xStart, yStart }; gridPos.y <= yEnd; gridPos.y++ )
	{
		for ( gridPos.x = xStart; gridPos.x <= xEnd; gridPos.x++ )
		{
			if ( TileAt( gridPos ).HasMeme() )
			{
				count++;
			}
		}
	}
	return count;
}

MemeField::GameState MemeField::GetState() const
{
	return gameState;
}

void MemeField::RevealTile( const Vei2& gridPos )
{
	Tile& tile = TileAt( gridPos );
	if ( !tile.IsRevealed() && !tile.IsFlagged() )
	{
		tile.Reveal( remainingHidden );
		if ( tile.HasMeme() )
		{
			gameState = GameState::Loose;
		}
		else if ( tile.HasNoNeighborMemes() )
		{
			const int xStart = std::max( 0, gridPos.x - 1 );
			const int yStart = std::max( 0, gridPos.y - 1 );
			const int xEnd = std::min( width - 1, gridPos.x + 1 );
			const int yEnd = std::min( height - 1, gridPos.y + 1 );

			for ( Vei2 gridPos = { xStart, yStart }; gridPos.y <= yEnd; gridPos.y++ )
			{
				for ( gridPos.x = xStart; gridPos.x <= xEnd; gridPos.x++ )
				{
					RevealTile( gridPos );
				}
			}
		}
	}
}

MemeField::Tile& MemeField::TileAt( const Vei2& gridPos )
{
	return field[gridPos.y * width + gridPos.x];
}

const MemeField::Tile& MemeField::TileAt( const Vei2& gridPos ) const
{
	return field[gridPos.y * width + gridPos.x];
}

Vei2 MemeField::ScreenToGrid( const Vei2& screenPos )
{
	return (screenPos - fieldPos) / SpriteCodex::tileSize;
}
