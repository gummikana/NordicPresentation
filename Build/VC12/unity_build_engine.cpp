﻿
// engine
#include "..\..\poro\source\game_utils\actionscript\asset_loading\Animations.cpp"
#include "..\..\poro\source\game_utils\actionscript\asset_loading\AnimationUpdater.cpp"
#include "..\..\poro\source\game_utils\actionscript\asset_loading\Sheet.cpp"
#include "..\..\poro\source\game_utils\actionscript\asset_loading\tests\Animations_testing.cpp"
#include "..\..\poro\source\game_utils\actionscript\asset_loading\tests\Sheet_testing.cpp"
#include "..\..\poro\source\game_utils\actionscript\as_math.cpp"
#include "..\..\poro\source\game_utils\actionscript\displayobjectcontainer.cpp"
#include "..\..\poro\source\game_utils\actionscript\event.cpp"
#include "..\..\poro\source\game_utils\actionscript\eventdispatcher.cpp"
#include "..\..\poro\source\game_utils\actionscript\sprite.cpp"
#include "..\..\poro\source\game_utils\actionscript\tests\displayobjectcontainer_test.cpp"
#include "..\..\poro\source\game_utils\actionscript\tests\eventdispatcher_test.cpp"
#include "..\..\poro\source\game_utils\actionscript\tests\vector_test.cpp"
#include "..\..\poro\source\game_utils\actionscript\textsprite.cpp"
#include "..\..\poro\source\game_utils\camera\ccamera_zoom.cpp"
#include "..\..\poro\source\game_utils\drawlines\drawlines.cpp"
#include "..\..\poro\source\game_utils\font\cfont.cpp"
#include "..\..\poro\source\game_utils\font\ifontalign.cpp"
#include "..\..\poro\source\game_utils\tween\gtween.cpp"
#include "..\..\poro\source\game_utils\tween\gtween_listener.cpp"
#include "..\..\poro\source\game_utils\tween\tests\cinterpolator_test.cpp"
#include "..\..\poro\source\game_utils\tween\tests\gtween_tester.cpp"
#include "..\..\poro\source\game_utils\tween\tween_utils.cpp"
#include "..\..\poro\source\poro\default_application.cpp"
#include "..\..\poro\source\poro\desktop\event_playback_impl.cpp"
#include "..\..\poro\source\poro\desktop\event_recorder_impl.cpp"
#include "..\..\poro\source\poro\desktop\graphics_buffer_opengl.cpp"
#include "..\..\poro\source\poro\desktop\graphics_opengl.cpp"
#include "..\..\poro\source\poro\desktop\joystick_impl.cpp"
#include "..\..\poro\source\poro\desktop\mouse_impl.cpp"
#include "..\..\poro\source\poro\desktop\platform_desktop.cpp"
#include "..\..\poro\source\poro\desktop\render_texture_opengl.cpp"
#include "..\..\poro\source\poro\desktop\shader_opengl.cpp"
#include "..\..\poro\source\poro\desktop\soundplayer_sdl.cpp"
#include "..\..\poro\source\poro\desktop\sound_sdl.cpp"
#include "..\..\poro\source\poro\desktop\texture3d_opengl.cpp"
#include "..\..\poro\source\poro\desktop\texture_opengl.cpp"
#include "..\..\poro\source\poro\desktop\windows\platform_win.cpp"
#include "..\..\poro\source\poro\event_recorder.cpp"
#include "..\..\poro\source\poro\iplatform.cpp"
#include "..\..\poro\source\poro\joystick.cpp"
#include "..\..\poro\source\poro\keyboard.cpp"
#include "..\..\poro\source\poro\mouse.cpp"
#include "..\..\poro\source\poro\touch.cpp"
#include "..\..\poro\source\tester\ctester.cpp"
#include "..\..\poro\source\tester\ctester_numeric.cpp"
#include "..\..\poro\source\tester\float_compare.cpp"
#include "..\..\poro\source\tester\tester_console.cpp"
#include "..\..\poro\source\utils\color\ccolor.cpp"
#include "..\..\poro\source\utils\color\color_utils.cpp"
#include "..\..\poro\source\utils\config_macro\tests\config_macro_test.cpp"
#include "..\..\poro\source\utils\easing\easing.cpp"
#include "..\..\poro\source\utils\easing\tests\easing_test.cpp"
#include "..\..\poro\source\utils\filesystem\filesystem.cpp"
#include "..\..\poro\source\utils\functionptr\tests\cfunctionptr_test.cpp"
#include "..\..\poro\source\utils\imagetoarray\imagetoarray.cpp"
#include "..\..\poro\source\utils\logger\clog.cpp"
#include "..\..\poro\source\utils\logger\cloglistenerforfile.cpp"
#include "..\..\poro\source\utils\logger\logger.cpp"
#include "..\..\poro\source\utils\math\point_inside.cpp"
#include "..\..\poro\source\utils\memcpy\memcpy.c"
#include "..\..\poro\source\utils\network\network_utils.cpp"
#include "..\..\poro\source\utils\pow2assert\pow2assert.cpp"
#include "..\..\poro\source\utils\random\random.cpp"
#include "..\..\poro\source\utils\rect\crect.cpp"
#include "..\..\poro\source\utils\rect\crect_functions.cpp"
#include "..\..\poro\source\utils\safearray\tests\csafearray_test.cpp"
#include "..\..\poro\source\utils\singleton\csingleton.cpp"
#include "..\..\poro\source\utils\string\string.cpp"
#include "..\..\poro\source\utils\timer\ctimer.cpp"
#include "..\..\poro\source\utils\timer\ctimer_impl.cpp"
#include "..\..\poro\source\utils\xml\canycontainer.cpp"
#include "..\..\poro\source\utils\xml\cxml.cpp"
#include "..\..\poro\source\utils\xml\cxmlcast.cpp"
#include "..\..\poro\source\utils\xml\cxmlnode.cpp"
#include "..\..\poro\source\utils\xml\cxmlparser.cpp"
#include "..\..\Source\component_framework\Component.cpp"
#include "..\..\Source\component_framework\ComponentFactory.cpp"
#include "..\..\Source\component_framework\Entity.cpp"
#include "..\..\Source\component_framework\EntityManager.cpp"
#include "..\..\Source\component_framework\EntityProcessingSystem.cpp"
#include "..\..\Source\component_framework\EntityReference.cpp"
#include "..\..\Source\component_framework\EntitySerializer.cpp"
#include "..\..\Source\component_framework\EntitySystem.cpp"
#include "..\..\Source\component_framework\EventManager.cpp"
#include "..\..\Source\component_framework\GComponents.cpp"
#include "..\..\Source\component_framework\LuaBindings.cpp"
#include "..\..\Source\component_framework\SystemManager.cpp"
#include "..\..\Source\misc_utils\config_sliders.cpp"
#include "..\..\Source\misc_utils\debug_layer.cpp"
#include "..\..\Source\misc_utils\metadata.cpp"
#include "..\..\Source\misc_utils\screenshotter.cpp"
#include "..\..\Source\misc_utils\simple_profiler.cpp"
#include "..\..\Source\misc_utils\simple_profiler_viewer.cpp"
#include "..\..\Source\misc_utils\simple_ui.cpp"

#include "..\..\Source\misc_utils\file_dialog.cpp"

#include "..\..\Source\gameplay_utils\game_mouse.cpp"
#include "..\..\Source\gameplay_utils\game_mouse_listener.cpp"
#include "..\..\Source\lua\lua_manager.cpp"
#include "..\..\Source\lua_ui_test.cpp"

#include "..\..\Source\component_updators\cardcontainer_system.cpp"
#include "..\..\Source\component_updators\sprite_system.cpp"

#include "..\..\Source\game_globals.cpp"
#include "..\..\Source\main.cpp"

