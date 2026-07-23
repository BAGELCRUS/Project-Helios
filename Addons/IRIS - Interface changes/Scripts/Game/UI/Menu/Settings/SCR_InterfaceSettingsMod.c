//------------------------------------------------------------------------------------------------
//! Extends SCR_InterfaceSettings with per-category notification toggles.
//! Each bool defaults to true (shown). When a category is disabled,
//! notifications of that type are suppressed client-side.
modded class SCR_InterfaceSettings : ModuleGameSettings
{
	[Attribute("1")]
	bool m_bShowNotif_PlayerJoinLeave;
	
	[Attribute("1")]
	bool m_bShowNotif_Killfeed;
	
	[Attribute("1")]
	bool m_bShowNotif_Objectives;
	
	[Attribute("1")]
	bool m_bShowNotif_BaseCapture;
	
	[Attribute("1")]
	bool m_bShowNotif_FactionChanges;
	
	[Attribute("1")]
	bool m_bShowNotif_EditorGM;
	
	[Attribute("1")]
	bool m_bShowNotif_Weather;
}
