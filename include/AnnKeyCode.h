#ifndef ANNKEYCODE
#define ANNKEYCODE

#include <OIS.h>

namespace Annwvyn
{
	namespace KeyCode
	{
		///Every key usable on the keyboard is described here.
		///These keycode are layout independant on Windows, but layout dependent on Linux
		enum code
		{
			unassigned		= OIS::KC_UNASSIGNED  ,
			escape			= OIS::KC_ESCAPE      ,
			one				= OIS::KC_1           ,
			two				= OIS::KC_2           ,
			three			= OIS::KC_3           ,
			four			= OIS::KC_4           ,
			five			= OIS::KC_5           ,
			six				= OIS::KC_6           ,
			seven			= OIS::KC_7           ,
			eight			= OIS::KC_8           ,
			nine			= OIS::KC_9           ,
			zero			= OIS::KC_0           ,
			minus			= OIS::KC_MINUS       ,    // - on main keyboard
			equals			= OIS::KC_EQUALS      ,
			back			= OIS::KC_BACK        ,      // backspace
			tab				= OIS::KC_TAB         ,
			q				= OIS::KC_Q           ,
			w				= OIS::KC_W           ,
			e				= OIS::KC_E           ,
			r				= OIS::KC_R           ,
			t				= OIS::KC_T           ,
			y				= OIS::KC_Y           ,
			u				= OIS::KC_U           ,
			i				= OIS::KC_I           ,
			o				= OIS::KC_O           ,
			p				= OIS::KC_P           ,
			lbracket		= OIS::KC_LBRACKET    ,
			rbracket		= OIS::KC_RBRACKET    ,
			enter			= OIS::KC_RETURN      ,  // Enter on main keyboard
			lcontrol		= OIS::KC_LCONTROL    ,
			a				= OIS::KC_A           ,
			s				= OIS::KC_S           ,
			d				= OIS::KC_D           ,
			f				= OIS::KC_F           ,
			g				= OIS::KC_G           ,
			h				= OIS::KC_H           , 
			j				= OIS::KC_J           ,
			k				= OIS::KC_K           ,
			l				= OIS::KC_L           ,
			semicolon		= OIS::KC_SEMICOLON   ,
			apostrofe		= OIS::KC_APOSTROPHE  ,
			grave			= OIS::KC_GRAVE       ,    // accent
			lshift			= OIS::KC_LSHIFT      ,
			backslash		= OIS::KC_BACKSLASH   ,
			z				= OIS::KC_Z           ,
			x				= OIS::KC_X           ,
			c				= OIS::KC_C           ,
			v				= OIS::KC_V           ,
			b				= OIS::KC_B           ,
			n				= OIS::KC_N           ,
			m				= OIS::KC_M           ,
			comma			= OIS::KC_COMMA       ,
			period			= OIS::KC_PERIOD      ,    // . on main keyboard
			slash			= OIS::KC_SLASH       ,    // / on main keyboard
			rshift			= OIS::KC_RSHIFT      ,
			multiply		= OIS::KC_MULTIPLY    ,    // * on numeric keypad
			lmenu			= OIS::KC_LMENU       ,      // left Alt
			space			= OIS::KC_SPACE       ,
			capital			= OIS::KC_CAPITAL     ,
			f1				= OIS::KC_F1          ,
			f2				= OIS::KC_F2          ,
			f3				= OIS::KC_F3          ,
			f4				= OIS::KC_F4          ,
			f5				= OIS::KC_F5          ,
			f6				= OIS::KC_F6          ,
			f7				= OIS::KC_F7          ,
			f8				= OIS::KC_F8          ,
			f9				= OIS::KC_F9          ,
			f10				= OIS::KC_F10         ,
			numlock			= OIS::KC_NUMLOCK     ,
			scroll			= OIS::KC_SCROLL      ,    // Scroll Lock
			num_seven		= OIS::KC_NUMPAD7     ,
			num_eight		= OIS::KC_NUMPAD8     ,
			num_nine		= OIS::KC_NUMPAD9     ,
			substract		= OIS::KC_SUBTRACT    ,    // - on numeric keypad
			num_four		= OIS::KC_NUMPAD4     ,
			num_five		= OIS::KC_NUMPAD5     ,
			num_six			= OIS::KC_NUMPAD6     ,
			add				= OIS::KC_ADD         ,    // + on numeric keypad
			num_one			= OIS::KC_NUMPAD1     ,
			num_two			= OIS::KC_NUMPAD2     ,
			num_three		= OIS::KC_NUMPAD3     ,
			num_zero		= OIS::KC_NUMPAD0     ,
			decimal			= OIS::KC_DECIMAL     ,    // . on numeric keypad
			oem_102			= OIS::KC_OEM_102     ,    // < > | on UK/Germany keyboards
			f11				= OIS::KC_F11         ,
			f12				= OIS::KC_F12         ,
			f13				= OIS::KC_F13         ,    //                     (NEC PC98)
			f14				= OIS::KC_F14         ,    //                     (NEC PC98)
			f15				= OIS::KC_F15         ,    //                     (NEC PC98)
			kana			= OIS::KC_KANA        ,    // (Japanese keyboard)
			anbt_c1			= OIS::KC_ABNT_C1     ,    // / ? on Portugese (Brazilian) keyboards
			convert			= OIS::KC_CONVERT     ,    // (Japanese keyboard)
			noconvert		= OIS::KC_NOCONVERT   ,    // (Japanese keyboard)
			yen				= OIS::KC_YEN         ,    // (Japanese keyboard)
			abnt_C2			= OIS::KC_ABNT_C2	  ,    // Numpad . on Portugese (Brazilian) keyboards
			num_equals		= OIS::KC_NUMPADEQUALS,    // = on numeric keypad (NEC PC98)
			prevtrack		= OIS::KC_PREVTRACK   ,    // Previous Track (KC_CIRCUMFLEX on Japanese keyboard)
			at				= OIS::KC_AT          ,    //                     (NEC PC98)
			colon			= OIS::KC_COLON       ,    //                     (NEC PC98)
			underline		= OIS::KC_UNDERLINE   ,    //                     (NEC PC98)
			kanji			= OIS::KC_KANJI       ,    // (Japanese keyboard)
			stop			= OIS::KC_STOP        ,    //                     (NEC PC98)
			ax				= OIS::KC_AX          ,    //                     (Japan AX)
			unlabeled		= OIS::KC_UNLABELED   ,    //                        (J3100)
			nexttrack		= OIS::KC_NEXTTRACK   ,    // Next Track
			num_enter		= OIS::KC_NUMPADENTER ,    // Enter on numeric keypad
			rcontrol		= OIS::KC_RCONTROL    ,
			mute			= OIS::KC_MUTE        ,    // Mute
			calculator		= OIS::KC_CALCULATOR  ,    // Calculator
			playpause		= OIS::KC_PLAYPAUSE   ,    // Play / Pause
			mediastop		= OIS::KC_MEDIASTOP   ,    // Media Stop
			volumedown		= OIS::KC_VOLUMEDOWN  ,    // Volume -
			volumeup		= OIS::KC_VOLUMEUP    ,    // Volume +
			webhome			= OIS::KC_WEBHOME     ,    // Web home
			num_comma		= OIS::KC_NUMPADCOMMA ,    // , on numeric keypad (NEC PC98)
			divide			= OIS::KC_DIVIDE      ,    // / on numeric keypad
			sysrq			= OIS::KC_SYSRQ       ,
			rmenu			= OIS::KC_RMENU       ,    // right Alt
			pause			= OIS::KC_PAUSE       ,    // Pause
			home			= OIS::KC_HOME        ,    // Home on arrow keypad
			up				= OIS::KC_UP          ,    // UpArrow on arrow keypad
			pageup			= OIS::KC_PGUP        ,    // PgUp on arrow keypad
			left			= OIS::KC_LEFT        ,    // LeftArrow on arrow keypad
			right			= OIS::KC_RIGHT       ,    // RightArrow on arrow keypad
			end				= OIS::KC_END         ,    // End on arrow keypad
			down			= OIS::KC_DOWN        ,    // DownArrow on arrow keypad
			pagedown		= OIS::KC_PGDOWN      ,    // PgDn on arrow keypad
			insert			= OIS::KC_INSERT      ,    // Insert on arrow keypad
			del				= OIS::KC_DELETE      ,    // Delete on arrow keypad
			lwin			= OIS::KC_LWIN        ,    // Left Windows key
			rwin			= OIS::KC_RWIN        ,    // Right Windows key
			apps			= OIS::KC_APPS        ,    // AppMenu key
			power			= OIS::KC_POWER       ,    // System Power
			sleep			= OIS::KC_SLEEP       ,    // System Sleep
			wake			= OIS::KC_WAKE        ,    // System Wake
			websearch		= OIS::KC_WEBSEARCH   ,    // Web Search
			webfavorites	= OIS::KC_WEBFAVORITES,    // Web Favorites
			webrefresh		= OIS::KC_WEBREFRESH  ,    // Web Refresh
			webstop			= OIS::KC_WEBSTOP     ,    // Web Stop
			webfoward		= OIS::KC_WEBFORWARD  ,    // Web Forward
			webback			= OIS::KC_WEBBACK     ,    // Web Back
			mycomputer		= OIS::KC_MYCOMPUTER  ,    // My Computer
			mail			= OIS::KC_MAIL        ,    // Mail
			mediaselect		= OIS::KC_MEDIASELECT ,    // Media Select
			SIZE
		};
	}
}

#endif //ANNKEYCODE