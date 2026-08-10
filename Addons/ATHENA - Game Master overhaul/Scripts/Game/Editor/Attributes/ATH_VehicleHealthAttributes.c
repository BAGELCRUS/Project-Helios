[BaseContainerProps(), SCR_BaseContainerCustomTitleUIInfo("m_UIInfo")]
class ATH_VehicleOverallHealthAttribute : SCR_BaseValueListEditorAttribute
{
	override int GetEntries(notnull array<ref SCR_BaseEditorAttributeEntry> outEntries)
	{
		SCR_EditorAttributeBaseValues values = new SCR_EditorAttributeBaseValues();
		values.m_fMin = 0;
		values.m_fMax = 1;
		values.m_fStep = 0.05;
		outEntries.Insert(new SCR_BaseEditorAttributeEntrySlider(values));
		return outEntries.Count();
	}
	
	override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
	{
		SCR_EditableEntityComponent editableEntity = SCR_EditableEntityComponent.Cast(item);
		if (!editableEntity) return null;
		
		IEntity entity = editableEntity.GetOwner();
		if (!entity) return null;
		
		SCR_VehicleDamageManagerComponent dmgMgr = SCR_VehicleDamageManagerComponent.Cast(entity.FindComponent(SCR_VehicleDamageManagerComponent));
		if (!dmgMgr) return null;
		
		return SCR_BaseEditorAttributeVar.CreateFloat(dmgMgr.GetHealthScaled());
	}
	
	override void WriteVariable(Managed item, SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, int playerID)
	{
		if (!var) return;
		
		SCR_EditableEntityComponent editableEntity = SCR_EditableEntityComponent.Cast(item);
		if (!editableEntity) return;
		
		IEntity entity = editableEntity.GetOwner();
		if (!entity) return;
		
		SCR_VehicleDamageManagerComponent dmgMgr = SCR_VehicleDamageManagerComponent.Cast(entity.FindComponent(SCR_VehicleDamageManagerComponent));
		if (!dmgMgr) return;
		
		dmgMgr.SetHealthScaled(var.GetFloat());
	}
}

// Base class for component health attributes
class ATH_VehicleComponentHealthAttributeBase : SCR_BaseValueListEditorAttribute
{
	override int GetEntries(notnull array<ref SCR_BaseEditorAttributeEntry> outEntries)
	{
		SCR_EditorAttributeBaseValues values = new SCR_EditorAttributeBaseValues();
		values.m_fMin = 0;
		values.m_fMax = 1;
		values.m_fStep = 0.05;
		outEntries.Insert(new SCR_BaseEditorAttributeEntrySlider(values));
		return outEntries.Count();
	}
	
	protected float GetHitzonesHealthScaled(Managed item, array<string> hitzoneNames)
	{
		SCR_EditableEntityComponent editableEntity = SCR_EditableEntityComponent.Cast(item);
		if (!editableEntity) return 1.0;
		IEntity entity = editableEntity.GetOwner();
		if (!entity) return 1.0;
		
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
	
	protected void SetHitzonesHealthScaled(Managed item, array<string> hitzoneNames, float scale)
	{
		SCR_EditableEntityComponent editableEntity = SCR_EditableEntityComponent.Cast(item);
		if (!editableEntity) return;
		IEntity entity = editableEntity.GetOwner();
		if (!entity) return;
		
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
		return SCR_BaseEditorAttributeVar.CreateFloat(GetHitzonesHealthScaled(item, {"Engine"}));
	}
	
	override void WriteVariable(Managed item, SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, int playerID)
	{
		if (var) SetHitzonesHealthScaled(item, {"Engine"}, var.GetFloat());
	}
}

[BaseContainerProps(), SCR_BaseContainerCustomTitleUIInfo("m_UIInfo")]
class ATH_VehicleWheelsHealthAttribute : ATH_VehicleComponentHealthAttributeBase
{
	override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
	{
		// Common wheel names in Reforger
		return SCR_BaseEditorAttributeVar.CreateFloat(GetHitzonesHealthScaled(item, {
			"Wheel_L01", "Wheel_R01", "Wheel_L02", "Wheel_R02", 
			"Wheel_L03", "Wheel_R03", "Wheel_L04", "Wheel_R04"
		}));
	}
	
	override void WriteVariable(Managed item, SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, int playerID)
	{
		if (var) SetHitzonesHealthScaled(item, {
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
		return SCR_BaseEditorAttributeVar.CreateFloat(GetHitzonesHealthScaled(item, {"MainRotor", "TailRotor"}));
	}
	
	override void WriteVariable(Managed item, SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, int playerID)
	{
		if (var) SetHitzonesHealthScaled(item, {"MainRotor", "TailRotor"}, var.GetFloat());
	}
}

[BaseContainerProps(), SCR_BaseContainerCustomTitleUIInfo("m_UIInfo")]
class ATH_VehicleFuelTankHealthAttribute : ATH_VehicleComponentHealthAttributeBase
{
	override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
	{
		return SCR_BaseEditorAttributeVar.CreateFloat(GetHitzonesHealthScaled(item, {"FuelTank", "FuelTank_L", "FuelTank_R"}));
	}
	
	override void WriteVariable(Managed item, SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, int playerID)
	{
		if (var) SetHitzonesHealthScaled(item, {"FuelTank", "FuelTank_L", "FuelTank_R"}, var.GetFloat());
	}
}
