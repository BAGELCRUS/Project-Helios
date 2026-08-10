[BaseContainerProps(), SCR_BaseContainerCustomTitleUIInfo("m_UIInfo")]
class ATH_VehicleOverallHealthAttribute : SCR_BaseValueListEditorAttribute
{
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
	protected float GetHitzonesHealthScaled(Managed item, typename hitzoneType)
	{
		SCR_EditableEntityComponent editableEntity = SCR_EditableEntityComponent.Cast(item);
		if (!editableEntity) return -1.0;
		IEntity entity = editableEntity.GetOwner();
		if (!entity) return -1.0;
		
		SCR_VehicleDamageManagerComponent dmgMgr = SCR_VehicleDamageManagerComponent.Cast(entity.FindComponent(SCR_VehicleDamageManagerComponent));
		if (!dmgMgr) return -1.0;
		
		array<HitZone> hitzones = {};
		dmgMgr.GetAllHitZones(hitzones);
		
		float totalHealth = 0;
		float totalMax = 0;
		
		foreach (HitZone hz : hitzones)
		{
			if (hz.IsInherited(hitzoneType))
			{
				totalHealth += hz.GetHealth();
				totalMax += hz.GetMaxHealth();
			}
		}
		
		if (totalMax == 0) return -1.0;
		return totalHealth / totalMax;
	}
	
	protected void SetHitzonesHealthScaled(Managed item, typename hitzoneType, float scale)
	{
		SCR_EditableEntityComponent editableEntity = SCR_EditableEntityComponent.Cast(item);
		if (!editableEntity) return;
		IEntity entity = editableEntity.GetOwner();
		if (!entity) return;
		
		SCR_VehicleDamageManagerComponent dmgMgr = SCR_VehicleDamageManagerComponent.Cast(entity.FindComponent(SCR_VehicleDamageManagerComponent));
		if (!dmgMgr) return;
		
		array<HitZone> hitzones = {};
		dmgMgr.GetAllHitZones(hitzones);
		
		foreach (HitZone hz : hitzones)
		{
			if (hz.IsInherited(hitzoneType))
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
		float val = GetHitzonesHealthScaled(item, SCR_EngineHitZone);
		if (val < 0) return null;
		return SCR_BaseEditorAttributeVar.CreateFloat(val);
	}
	
	override void WriteVariable(Managed item, SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, int playerID)
	{
		if (var) SetHitzonesHealthScaled(item, SCR_EngineHitZone, var.GetFloat());
	}
}

[BaseContainerProps(), SCR_BaseContainerCustomTitleUIInfo("m_UIInfo")]
class ATH_VehicleWheelsHealthAttribute : ATH_VehicleComponentHealthAttributeBase
{
	override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
	{
		float val = GetHitzonesHealthScaled(item, SCR_WheelHitZone);
		if (val < 0) return null;
		return SCR_BaseEditorAttributeVar.CreateFloat(val);
	}
	
	override void WriteVariable(Managed item, SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, int playerID)
	{
		if (var) SetHitzonesHealthScaled(item, SCR_WheelHitZone, var.GetFloat());
	}
}

[BaseContainerProps(), SCR_BaseContainerCustomTitleUIInfo("m_UIInfo")]
class ATH_VehicleRotorHealthAttribute : ATH_VehicleComponentHealthAttributeBase
{
	override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
	{
		float val = GetHitzonesHealthScaled(item, SCR_RotorHitZone);
		if (val < 0) return null;
		return SCR_BaseEditorAttributeVar.CreateFloat(val);
	}
	
	override void WriteVariable(Managed item, SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, int playerID)
	{
		if (var) SetHitzonesHealthScaled(item, SCR_RotorHitZone, var.GetFloat());
	}
}

[BaseContainerProps(), SCR_BaseContainerCustomTitleUIInfo("m_UIInfo")]
class ATH_VehicleFuelTankHealthAttribute : ATH_VehicleComponentHealthAttributeBase
{
	override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
	{
		float val = GetHitzonesHealthScaled(item, SCR_FuelHitZone);
		if (val < 0) return null;
		return SCR_BaseEditorAttributeVar.CreateFloat(val);
	}
	
	override void WriteVariable(Managed item, SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, int playerID)
	{
		if (var) SetHitzonesHealthScaled(item, SCR_FuelHitZone, var.GetFloat());
	}
}
