/***************************************************************************
 *
 * Copyright (c) 2003 - 2011 Petri Purho
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


#ifndef INC_SIGNALSLOT_LIBS_H
#define INC_SIGNALSLOT_LIBS_H

#define CENG_SERIALIZABLE_SIGNALSLOTS

#include "../ceng_macro.h"
#include "../debug.h"

// #define CENG_Join( X, Y ) CENG_DO_JOIN( X, Y )
// #define CENG_DO_JOIN( X, Y ) CENG_DO_JOIN2(X,Y)
// #define CENG_DO_JOIN2( X, Y ) X##Y

#include <string>
#include <list>
#include <iostream>

#include "../autolist/cautolist.h"
#include "../singleton/csingleton.h"
#include "../maphelper/cmaphelper.h"
#include "../xml/canycontainer.h"

#ifdef CENG_SERIALIZABLE_SIGNALSLOTS
#	include "../xml/cxml.h"
#endif
// #include "cautolist.h"
// #include "../../tester/csingleton.h"
// #include "cmaphelper.h"


#endif
