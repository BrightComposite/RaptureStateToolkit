//---------------------------------------------------------------------------

#pragma once

#ifndef DISPLAY_H
#define DISPLAY_H

//---------------------------------------------------------------------------

#include <meta/macro.h>

//---------------------------------------------------------------------------

namespace asd
{
	class Display
	{
	public:
		api(platform)
		int dispatchMessages();
	};
}

//---------------------------------------------------------------------------
#endif
