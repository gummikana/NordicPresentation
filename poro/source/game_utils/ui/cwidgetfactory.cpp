/***************************************************************************
 *
 * Copyright (c) 2009 - 2011 Petri Purho, Dennis Belfrage
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ***************************************************************************/


#include "cwidgetfactory.h"
#include "cwidget.h"
#include "cbuttonwidget.h"
#include "ccheckboxwidget.h"
#include "cmodifiedbuttonwidget.h"
#include "csliderwidget.h"
#include "ctextinputwidget.h"
#include "cwindowwidget.h"

namespace ceng {
namespace ui {

void InitUI()
{
	WIDGET_REGISTER( CButtonWidget );
	WIDGET_REGISTER( CCheckboxWidget );
	WIDGET_REGISTER( CModifiedButtonWidget );
	WIDGET_REGISTER( CSliderWidget );
	WIDGET_REGISTER( CTextInputWidget );
	WIDGET_REGISTER( CWidget );
	WIDGET_REGISTER( CWindowWidget );
}


// template< class T > std::string CWidgetFactory::CGenericMakerImpl< T >::name;
} // end of namespace ui
} // end of namespace ceng