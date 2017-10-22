#pragma once

#include "Vei2.h"
#include "Graphics.h"

class MemeField
{
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
		void Draw( const Vei2& screenPos, Graphics& gfx ) const;
		bool HasMeme() const;

	private:
		State state = State::Hidden;
		bool hasMeme = false;
	};
public:
	MemeField( int nMemes );
	void Draw( Graphics& gfx ) const;
	RectI GetRect() const;

private:
	Tile& TileAt( const Vei2& gridPos );
	const Tile& TileAt( const Vei2& gridPos ) const;
private:
	static constexpr int width = 20;
	static constexpr int height = 16;
	Tile field[ width * height ];
};