//------------------------------------------------------------------------------------------------
//! ATH_Init - Mod Initialization
//! Registers Athena settings to the ODIN Core centralized settings manager.
//! Follows the same pattern as ATLAS_Init.c.
//------------------------------------------------------------------------------------------------

modded class SCR_BaseGameMode
{
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		// Wait a frame to ensure ODIN_SettingsManager is ready
		GetGame().GetCallqueue().CallLater(RegisterAthenaSettings, 1000, false);
	}
	
	protected void RegisterAthenaSettings()
	{
		if (ODIN_SettingsManager.GetInstance())
		{
			ODIN_SettingsManager.GetInstance().RegisterMod("ATHENA", "Athena - Game Master", "{69FB32E4A1B2C3D4}UI/layouts/AthenaSettings.layout");
		}
	}
}
