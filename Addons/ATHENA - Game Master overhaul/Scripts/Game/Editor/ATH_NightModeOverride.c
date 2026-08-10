//------------------------------------------------------------------------------------------------
//! ATH_NightModeOverride
//! Athena - Game Master Overhaul
//!
//! Overrides the vanilla SCR_NightModeGameModeComponent to make the
//! "Brighter" GM toolbar setting slightly brighter ONLY for the local Game Master.
//! Vanilla default: EV 2  →  Athena GM override: EV 4
//------------------------------------------------------------------------------------------------

modded class SCR_NightModeGameModeComponent : SCR_BaseGameModeComponent
{
	protected int m_iPreviousTargetEV = -1;

	override void EOnFrame(IEntity owner, float timeSlice)
	{
		// Determine the desired EV target based on active modes.
		// Local Editor Night Mode (the GM shortcut "Brighter") takes precedence.
		int desiredEV = 2; // Default vanilla night mode EV
		
		if (IsLocalEditorNightModeEnabled())
		{
			desiredEV = 4; // Athena brighter mode
		}
		
		// If the target EV state changed, update the internal variable and force a recalculation
		if (m_iPreviousTargetEV != desiredEV)
		{
			m_iEVValueNightMode = desiredEV;
			m_iPreviousTargetEV = desiredEV;
			
			// Force a recalculation by invalidating the alpha so the vanilla lerp updates immediately
			m_fNightModeAlpha = -1;
		}

		// Let vanilla handle the actual application of EV and sun tracking
		super.EOnFrame(owner, timeSlice);
	}
}
