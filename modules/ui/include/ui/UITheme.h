//---------------------------------------------------------------------------

#pragma once

#ifndef UI_THEME_H
#define UI_THEME_H

//---------------------------------------------------------------------------

#include <ui/UIPalette.h>

//---------------------------------------------------------------------------

namespace asd
{
	class UITheme
	{
	public:
		virtual void apply(UIPalette * palette) = 0;
	};
}

//---------------------------------------------------------------------------
#endif
