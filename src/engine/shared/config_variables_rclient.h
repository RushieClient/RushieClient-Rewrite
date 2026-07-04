// This file can be included several times.

#ifndef MACRO_CONFIG_INT
#error "The config macros must be defined"
#define MACRO_CONFIG_INT(Tcme, ScriptName, Def, Min, Max, Save, Desc) ;
#define MACRO_CONFIG_COL(Tcme, ScriptName, Def, Save, Desc) ;
#define MACRO_CONFIG_STR(Tcme, ScriptName, Len, Def, Save, Desc) ;
#endif

// Dummy change clan
MACRO_CONFIG_STR(RcPlayerClanNoDummy, rc_player_clan_no_dummy, 12, "#NODUMMY", CFGFLAG_CLIENT | CFGFLAG_SAVE, "Thats is clan when dummy not connected")
MACRO_CONFIG_STR(RcPlayerClanWithDummy, rc_player_clan_with_dummy, 12, "#YESDUMMY", CFGFLAG_CLIENT | CFGFLAG_SAVE, "Thats is clan when dummy connected")
MACRO_CONFIG_INT(RcPlayerClanAutoChange, rc_player_clan_auto_change, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Change clan when dummy connected")

// Binds
MACRO_CONFIG_INT(RcToggle45degrees, rc_toggle_45_degrees, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Toggle 45 degrees bind or not")
MACRO_CONFIG_INT(Rc45degreesEcho, rc_45_degrees_echo, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "45 degrees echo")
MACRO_CONFIG_INT(RcToggleSmallSens, rc_toggle_small_sens, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Toggle small sens bind or not")
MACRO_CONFIG_INT(RcSmallSensEcho, rc_small_sens_echo, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Small sens echo")
MACRO_CONFIG_INT(RcDeepFlyOnRMB, rc_deep_fly_on_rmb, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Deepfly on right mouse button")

// Master servers
MACRO_CONFIG_INT(RcUseMasterServerMirrors, rc_use_master_server_mirrors, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Use master server mirrors")

// Chat Filter
MACRO_CONFIG_INT(RcMessageFilterMode, rc_message_filter_mode, 0, 0, 3, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Message filter mode(0-off,1-partial,2-full,3-both")
MACRO_CONFIG_INT(RcMessageFilterPrintBlockedMessage, rc_message_filter_print_blocked_message, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Print in console blocked message")
MACRO_CONFIG_INT(RcMessageFilterMultiplyChangeWordOnFullMatch, rc_message_filter_multiply_change_word_on_full_match, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Multiply count of change word on full match(0-off, 1-on)")
MACRO_CONFIG_STR(RcMessageFilterWordOnFullMatch, rc_message_filter_word_on_full_match, 64, "^", CFGFLAG_CLIENT | CFGFLAG_SAVE, "Word when censor full match")
MACRO_CONFIG_INT(RcMessageFilterMultiplyChangeWordOnPartialMatch, rc_message_filter_multiply_change_word_on_partial_match, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Multiply count of change word on patial match(0-off, 1-on)")
MACRO_CONFIG_STR(RcMessageFilterWordOnPartialMatch, rc_message_filter_word_on_partial_match, 64, "*", CFGFLAG_CLIENT | CFGFLAG_SAVE, "Word when censor partial match")