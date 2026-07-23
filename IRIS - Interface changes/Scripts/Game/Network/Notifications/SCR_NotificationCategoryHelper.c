//------------------------------------------------------------------------------------------------
//! Static helper class that maps ENotification enum values to SCR_ENotificationCategory.
//! Uses enum name string matching to categorize notifications.
//! Unrecognized notifications return -1, meaning they pass through unfiltered.
class SCR_NotificationCategoryHelper
{
	//------------------------------------------------------------------------------------------------
	//! Maps an ENotification value to its SCR_ENotificationCategory.
	//! Editor-specific notifications take priority over content keywords
	//! (e.g., EDITOR_CHANGED_KILLFEED_TYPE → EDITOR_GM, not KILLFEED).
	//! \param notification ENotification enum value
	//! \return SCR_ENotificationCategory, or -1 if unrecognized (always shown)
	static int GetCategory(ENotification notification)
	{
		string name = typename.EnumToString(ENotification, notification);
		
		if (name.IsEmpty())
			return -1;
		
		// Editor/GM notifications — check first because editor notifications
		// may contain keywords from other categories (e.g., EDITOR_CHANGED_KILLFEED_TYPE)
		if (name.Contains("EDITOR") || name.Contains("_GM"))
			return SCR_ENotificationCategory.EDITOR_GM;
		
		// Player join/leave/kick/ban
		if (name.Contains("PLAYER_JOINED") || name.Contains("PLAYER_LEFT") ||
			name.Contains("PLAYER_KICKED") || name.Contains("PLAYER_BANNED") ||
			name.Contains("PLAYER_DISCONNECTED"))
			return SCR_ENotificationCategory.PLAYER_JOIN_LEAVE;
		
		// Killfeed — kills, deaths, suicides, friendly fire
		if (name.Contains("KILL") || name.Contains("SUICIDE") || 
			name.Contains("FRIENDLY_FIRE") || name.Contains("DEATH") ||
			name.Contains("DESTROYED"))
			return SCR_ENotificationCategory.KILLFEED;
		
		// Objectives / Tasks
		if (name.Contains("TASK") || name.Contains("OBJECTIVE") || 
			name.Contains("MISSION"))
			return SCR_ENotificationCategory.OBJECTIVES;
		
		// Base capture
		if (name.Contains("BASE") || name.Contains("CAPTURE") || 
			name.Contains("SEIZ") || name.Contains("CONTESTED"))
			return SCR_ENotificationCategory.BASE_CAPTURE;
		
		// Faction changes
		if (name.Contains("FACTION"))
			return SCR_ENotificationCategory.FACTION_CHANGES;
		
		// Weather / Environment
		if (name.Contains("WEATHER") || name.Contains("TIME_OF_DAY") ||
			name.Contains("DAYTIME"))
			return SCR_ENotificationCategory.WEATHER_ENVIRONMENT;
		
		// Unknown category — always show
		return -1;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Gets the user settings field name for a given notification category.
	//! \param category SCR_ENotificationCategory
	//! \return String name of the corresponding bool field in SCR_InterfaceSettings
	static string GetSettingName(int category)
	{
		switch (category)
		{
			case SCR_ENotificationCategory.PLAYER_JOIN_LEAVE:
				return "m_bShowNotif_PlayerJoinLeave";
			case SCR_ENotificationCategory.KILLFEED:
				return "m_bShowNotif_Killfeed";
			case SCR_ENotificationCategory.OBJECTIVES:
				return "m_bShowNotif_Objectives";
			case SCR_ENotificationCategory.BASE_CAPTURE:
				return "m_bShowNotif_BaseCapture";
			case SCR_ENotificationCategory.FACTION_CHANGES:
				return "m_bShowNotif_FactionChanges";
			case SCR_ENotificationCategory.EDITOR_GM:
				return "m_bShowNotif_EditorGM";
			case SCR_ENotificationCategory.WEATHER_ENVIRONMENT:
				return "m_bShowNotif_Weather";
		}
		
		return string.Empty;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Gets a display-friendly label for a notification category.
	//! \param category SCR_ENotificationCategory
	//! \return Localized or readable name for the category
	static string GetCategoryDisplayName(int category)
	{
		switch (category)
		{
			case SCR_ENotificationCategory.PLAYER_JOIN_LEAVE:
				return "Player Join/Leave";
			case SCR_ENotificationCategory.KILLFEED:
				return "Kill Feed";
			case SCR_ENotificationCategory.OBJECTIVES:
				return "Objectives";
			case SCR_ENotificationCategory.BASE_CAPTURE:
				return "Base Capture";
			case SCR_ENotificationCategory.FACTION_CHANGES:
				return "Faction Changes";
			case SCR_ENotificationCategory.EDITOR_GM:
				return "Editor / GM";
			case SCR_ENotificationCategory.WEATHER_ENVIRONMENT:
				return "Weather";
		}
		
		return string.Empty;
	}
}
