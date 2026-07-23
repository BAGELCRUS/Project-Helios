//------------------------------------------------------------------------------------------------
//! Categories for grouping ENotification types in the Interface settings.
//! Used to provide granular notification filtering per-category.
enum SCR_ENotificationCategory
{
	PLAYER_JOIN_LEAVE,       //!< Player joined, left, kicked, banned, GM joined/left
	KILLFEED,                //!< Kills, team kills, suicides, friendly fire
	OBJECTIVES,              //!< Task created, completed, failed, updated
	BASE_CAPTURE,            //!< Base captured, contested, lost
	FACTION_CHANGES,         //!< Player joined faction, faction changes
	EDITOR_GM,               //!< Game Master / editor-specific notifications
	WEATHER_ENVIRONMENT,     //!< Weather changes, environment updates
	COUNT                    //!< Used for iteration
}
