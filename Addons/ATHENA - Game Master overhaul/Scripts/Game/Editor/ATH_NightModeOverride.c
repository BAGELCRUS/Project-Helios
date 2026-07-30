//------------------------------------------------------------------------------------------------
//! ATH_NightModeOverride
//! Athena - Game Master Overhaul
//!
//! Overrides the vanilla SCR_NightModeGameModeComponent to make the
//! "Brighter" GM toolbar setting slightly brighter.
//! Vanilla default: EV 2  →  Athena override: EV 4
//------------------------------------------------------------------------------------------------

modded class SCR_NightModeGameModeComponent : SCR_BaseGameModeComponent
{
	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		// Let vanilla initialize everything first
		super.EOnInit(owner);
		
		// Override the night mode EV to be brighter than vanilla (2 → 4)
		m_iEVValueNightMode = 4;
	}
}
