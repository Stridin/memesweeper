#pragma once

#include "Vei2.h"
#include "Graphics.h"

class MemeField
{
public:
	enum class GameState
	{
		Running,
		Win,
		Loose
	};
public:
	class Tile
	{
	public:
		enum class State
		{
			Hidden,
			Revealed,
			Flagged
		};
	public:
		void SpawnMeme();
		void Draw( const Vei2& screenPos, GameState gameState, Graphics& gfx ) const;
		void Reveal( int& remainingHidden );
		bool HasMeme() const;
		bool IsRevealed() const;
		void ToggleFlag();
		bool IsFlagged() const;
		void SetNeighborMemeCount( int memeCount );

	private:
		State state = State::Hidden;
		bool hasMeme = false;
		int nNeighborMemes = -1;
	};
public:
	MemeField( int nMemes, Vei2 pos );
	void Draw( Graphics& gfx ) const;
	RectI GetRect() const;
	void OnRevealClick( const Vei2& screenPos );
	void OnFlagClick( const Vei2& screenPos );
	int CountNeighborMemes( const Vei2& gridPos );
	GameState GetState() const;

private:
	Tile& TileAt( const Vei2& gridPos );
	const Tile& TileAt( const Vei2& gridPos ) const;
	Vei2 ScreenToGrid( const Vei2& screenPos );
private:
	static constexpr int width = 20;
	static constexpr int height = 16;
	int remainingHidden;
	int remainingMemes;
	GameState gameState = GameState::Running;
	Tile field[ width * height ];
	Vei2 fieldPos;
};
