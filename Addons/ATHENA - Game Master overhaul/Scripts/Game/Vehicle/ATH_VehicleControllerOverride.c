modded class VehicleControllerComponent : BaseVehicleControllerComponent
{
	[RplProp(onRplName: "OnEngineOverrideChanged")]
	protected bool m_bATH_EngineForcedOn;

	[RplProp(onRplName: "OnLightsOverrideChanged")]
	protected int m_iATH_LightsForcedState; // 0 = Auto, 1 = Off, 2 = Headlights, 3 = HiBeam, 4 = Hazard
	
	//------------------------------------------------------------------------------------------------
	// Engine Override
	//------------------------------------------------------------------------------------------------
	bool ATH_IsEngineForcedOn()
	{
		return m_bATH_EngineForcedOn;
	}

	void ATH_SetEngineForcedOn(bool forced)
	{
		m_bATH_EngineForcedOn = forced;
		Replication.BumpMe();
		OnEngineOverrideChanged();
	}
	
	protected void OnEngineOverrideChanged()
	{
		if (m_bATH_EngineForcedOn && !IsEngineOn())
		{
			StartEngine();
		}
	}

	override void OnEngineStop()
	{
		super.OnEngineStop();
		
		if (m_bATH_EngineForcedOn)
		{
			// The engine was stopped (likely by the auto-shutoff when players left).
			// We force it back on immediately.
			StartEngine();
		}
	}

	//------------------------------------------------------------------------------------------------
	// Lights Override
	//------------------------------------------------------------------------------------------------
	int ATH_GetLightsForcedState()
	{
		return m_iATH_LightsForcedState;
	}

	void ATH_SetLightsForcedState(int state)
	{
		m_iATH_LightsForcedState = state;
		Replication.BumpMe();
		OnLightsOverrideChanged();
	}

	protected void OnLightsOverrideChanged()
	{
		ATH_ApplyForcedLights();
	}

	protected void ATH_ApplyForcedLights()
	{
		BaseLightManagerComponent lightManager = GetLightManager();
		if (!lightManager) 
			return;
		
		// If Auto, we do nothing and let the game handle it
		if (m_iATH_LightsForcedState == 0) 
			return;
			
		bool bOff = m_iATH_LightsForcedState == 1;
		bool bHeadlights = m_iATH_LightsForcedState == 2;
		bool bHiBeam = m_iATH_LightsForcedState == 3;
		bool bHazard = m_iATH_LightsForcedState == 4;
		
		if (bOff)
		{
			lightManager.SetLightsState(ELightType.AllLights, false);
		}
		else
		{
			// Reset all first
			lightManager.SetLightsState(ELightType.AllLights, false);
			
			if (bHeadlights)
			{
				lightManager.SetLightsState(ELightType.Head, true);
				lightManager.SetLightsState(ELightType.Rear, true);
				lightManager.SetLightsState(ELightType.Dashboard, true);
			}
			else if (bHiBeam)
			{
				lightManager.SetLightsState(ELightType.Head, true);
				lightManager.SetLightsState(ELightType.Rear, true);
				lightManager.SetLightsState(ELightType.Dashboard, true);
				lightManager.SetLightsState(ELightType.HiBeam, true);
			}
			else if (bHazard)
			{
				lightManager.SetLightsState(ELightType.Hazard, true);
			}
		}
	}
	
	// When compartment is left, the game natively shuts off lights after a timeout.
	// We can hook into OnCompartmentLeft to continually enforce our light state.
	// But it might be delayed. An alternative is to enforce it in OnPostInit with a timer, 
	// but hooking into OnCompartmentLeft might be enough.
	override void OnCompartmentLeft(IEntity vehicle, BaseCompartmentManagerComponent mgr, IEntity occupant, int managerId, int slotID)
	{
		super.OnCompartmentLeft(vehicle, mgr, occupant, managerId, slotID);
		
		if (m_iATH_LightsForcedState != 0)
		{
			// Enforce it next frame to override any vanilla shutoff that happens in this exact call stack
			GetGame().GetCallqueue().CallLater(ATH_ApplyForcedLights, 50, false);
		}
	}
}
