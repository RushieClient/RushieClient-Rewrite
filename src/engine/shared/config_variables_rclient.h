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
MACRO_CONFIG_INT(RcMessageFilterMultiplyChangeWordOnPartialMatch, rc_message_filter_multiply_change_word_on_partial_match, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Multiply count of change word on partial match(0-off, 1-on)")
MACRO_CONFIG_STR(RcMessageFilterWordOnPartialMatch, rc_message_filter_word_on_partial_match, 64, "*", CFGFLAG_CLIENT | CFGFLAG_SAVE, "Word when censor partial match")

// Translate
MACRO_CONFIG_INT(RcTranslateSend, rc_translate_send, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Translate sending message")
MACRO_CONFIG_STR(RcTranslateSendTarget, rc_translate_send_target, 16, "en", CFGFLAG_CLIENT | CFGFLAG_SAVE, "Sending message translate target language (must be 2 character ISO 639 code)")

// Heart
MACRO_CONFIG_INT(RcShowHeartInScoreboard, rc_show_heart_in_scoreboard, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show heart in scoreboard")
MACRO_CONFIG_INT(RcSizeOfHeart, rc_size_of_heart, 100, 0, 200, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Heart size")

// Scoreboard
MACRO_CONFIG_INT(RcScoreboardAlwaysShowQuickActions, rc_scoreboard_always_show_quick_actions, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show quick actions in scoreboard always")
MACRO_CONFIG_INT(RcScoreboardFreezeInputs, rc_scoreboard_freeze_inputs, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Freeze inputs when unlock mouse")

// Chatbubbles
MACRO_CONFIG_INT(RcChatBubbles, rc_chat_bubbles, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show Chatbubbles above players")
MACRO_CONFIG_INT(RcChatBubbleSize, rc_chat_bubble_size, 20, 15, 30, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Size of the chat bubble")
MACRO_CONFIG_INT(RcChatBubbleShowTime, rc_chat_bubble_showtime, 200, 200, 1000, CFGFLAG_CLIENT | CFGFLAG_SAVE, "How long to show the bubble for")
MACRO_CONFIG_INT(RcChatBubbleFadeOut, rc_chat_bubble_fadeout, 35, 15, 100, CFGFLAG_CLIENT | CFGFLAG_SAVE, "How long it fades out")
MACRO_CONFIG_INT(RcChatBubbleFadeIn, rc_chat_bubble_fadein, 15, 15, 100, CFGFLAG_CLIENT | CFGFLAG_SAVE, "how long it fades in")