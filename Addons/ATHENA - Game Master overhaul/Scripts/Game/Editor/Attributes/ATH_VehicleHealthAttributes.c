[BaseContainerProps(), SCR_BaseContainerCustomTitleUIInfo("m_UIInfo")]
class ATH_VehicleOverallHealthAttribute : SCR_BaseEditorAttribute
{
	override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
	{
		IEntity entity = IEntity.Cast(item);
		if (!entity) return null;
		
		SCR_VehicleDamageManagerComponent dmgMgr = SCR_VehicleDamageManagerComponent.Cast(entity.FindComponent(SCR_VehicleDamageManagerComponent));
		if (!dmgMgr) return null;
		
		return SCR_BaseEditorAttributeVar.CreateFloat(dmgMgr.GetHealthScaled());
	}
	
	override void WriteVariable(Managed item, SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, int playerID)
	{
		if (!var) return;
		
		IEntity entity = IEntity.Cast(item);
		if (!entity) return;
		
		SCR_VehicleDamageManagerComponent dmgMgr = SCR_VehicleDamageManagerComponent.Cast(entity.FindComponent(SCR_VehicleDamageManagerComponent));
		if (!dmgMgr) return;
		
		dmgMgr.SetHealthScaled(var.GetFloat());
	}
}

// Base class for component health attributes
class ATH_VehicleComponentHealthAttributeBase : SCR_BaseEditorAttribute
{
	protected float GetHitzonesHealthScaled(IEntity entity, array<string> hitzoneNames)
	{
		SCR_VehicleDamageManagerComponent dmgMgr = SCR_VehicleDamageManagerComponent.Cast(entity.FindComponent(SCR_VehicleDamageManagerComponent));
		if (!dmgMgr) return 1.0;
		
		float totalHealth = 0;
		float totalMax = 0;
		
		foreach (string hzName : hitzoneNames)
		{
			HitZone hz = dmgMgr.GetHitZoneByName(hzName);
			if (hz)
			{
				totalHealth += hz.GetHealth();
				totalMax += hz.GetMaxHealth();
			}
		}
		
		if (totalMax == 0) return 1.0;
		return totalHealth / totalMax;
	}
	
	protected void SetHitzonesHealthScaled(IEntity entity, array<string> hitzoneNames, float scale)
	{
		SCR_VehicleDamageManagerComponent dmgMgr = SCR_VehicleDamageManagerComponent.Cast(entity.FindComponent(SCR_VehicleDamageManagerComponent));
		if (!dmgMgr) return;
		
		foreach (string hzName : hitzoneNames)
		{
			HitZone hz = dmgMgr.GetHitZoneByName(hzName);
			if (hz)
			{
				hz.SetHealthScaled(scale);
			}
		}
	}
}

[BaseContainerProps(), SCR_BaseContainerCustomTitleUIInfo("m_UIInfo")]
class ATH_VehicleEngineHealthAttribute : ATH_VehicleComponentHealthAttributeBase
{
	override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
	{
		return SCR_BaseEditorAttributeVar.CreateFloat(GetHitzonesHealthScaled(IEntity.Cast(item), {"Engine"}));
	}
	
	override void WriteVariable(Managed item, SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, int playerID)
	{
		if (var) SetHitzonesHealthScaled(IEntity.Cast(item), {"Engine"}, var.GetFloat());
	}
}

[BaseContainerProps(), SCR_BaseContainerCustomTitleUIInfo("m_UIInfo")]
class ATH_VehicleWheelsHealthAttribute : ATH_VehicleComponentHealthAttributeBase
{
	override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
	{
		// Common wheel names in Reforger
		return SCR_BaseEditorAttributeVar.CreateFloat(GetHitzonesHealthScaled(IEntity.Cast(item), {
			"Wheel_L01", "Wheel_R01", "Wheel_L02", "Wheel_R02", 
			"Wheel_L03", "Wheel_R03", "Wheel_L04", "Wheel_R04"
		}));
	}
	
	override void WriteVariable(Managed item, SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, int playerID)
	{
		if (var) SetHitzonesHealthScaled(IEntity.Cast(item), {
			"Wheel_L01", "Wheel_R01", "Wheel_L02", "Wheel_R02", 
			"Wheel_L03", "Wheel_R03", "Wheel_L04", "Wheel_R04"
		}, var.GetFloat());
	}
}

[BaseContainerProps(), SCR_BaseContainerCustomTitleUIInfo("m_UIInfo")]
class ATH_VehicleRotorHealthAttribute : ATH_VehicleComponentHealthAttributeBase
{
	override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
	{
		return SCR_BaseEditorAttributeVar.CreateFloat(GetHitzonesHealthScaled(IEntity.Cast(item), {"MainRotor", "TailRotor"}));
	}
	
	override void WriteVariable(Managed item, SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, int playerID)
	{
		if (var) SetHitzonesHealthScaled(IEntity.Cast(item), {"MainRotor", "TailRotor"}, var.GetFloat());
	}
}

[BaseContainerProps(), SCR_BaseContainerCustomTitleUIInfo("m_UIInfo")]
class ATH_VehicleFuelTankHealthAttribute : ATH_VehicleComponentHealthAttributeBase
{
	override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
	{
		return SCR_BaseEditorAttributeVar.CreateFloat(GetHitzonesHealthScaled(IEntity.Cast(item), {"FuelTank", "FuelTank_L", "FuelTank_R"}));
	}
	
	override void WriteVariable(Managed item, SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, int playerID)
	{
		if (var) SetHitzonesHealthScaled(IEntity.Cast(item), {"FuelTank", "FuelTank_L", "FuelTank_R"}, var.GetFloat());
	}
}
