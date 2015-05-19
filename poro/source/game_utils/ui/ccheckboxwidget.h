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


///////////////////////////////////////////////////////////////////////////////
//
// CCheckBoxWidget
// ===============
//
// A basic clickable button, nothing fancy or anything
//
//
// Created 01.01.2006 by Pete
//.............................................................................
//=============================================================================
#ifndef INC_UI_CCHECKBOXWIDGET_H
#define INC_UI_CCHECKBOXWIDGET_H

#include "cwidget.h"
#include "ui_utils.h"

namespace ceng {
namespace ui {

class CCheckboxWidget : public CWidget
{
public:
	CCheckboxWidget();
	CCheckboxWidget( CWidget* parent, const types::rect& rect = types::rect(), bool relative = false, const types::id& id = types::id(), const std::string& sprite_img = "", bool value = false );
	virtual ~CCheckboxWidget();

	//=========================================================================

	void SetAltText( const std::string& alt_text );
	std::string GetAltText() const;

	//=========================================================================

	void				SetClickableButtons( types::mouse_button buttons );
	types::mouse_button GetClickableuttons() const;

	//=========================================================================

	virtual void OnMouseEvent( CMouseEvent* event );

	signal OnClick;
	signal OnValueChange;

	//=========================================================================

	void SetValue( bool value );
	bool GetValue() const;
	void SwapValue();

	//=========================================================================

protected:
	void UpdateCheckbox();

	static void Initialize( CCheckboxWidget* widget );

	std::string			myAltText;
	types::mouse_button myClickableButtons;
	bool				myClicked;
	bool				myValue;
};

} // end of namespace ui
} // end of namespace ceng

#endif