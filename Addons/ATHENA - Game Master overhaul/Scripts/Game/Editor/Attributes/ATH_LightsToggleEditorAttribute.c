[BaseContainerProps(), SCR_BaseContainerCustomTitleUIInfo("m_UIInfo")]
class ATH_LightsToggleEditorAttribute : SCR_BasePresetsEditorAttribute
{
	override int GetEntries(notnull array<ref SCR_BaseEditorAttributeEntry> outEntries)
	{
		array<ref SCR_EditorAttributeFloatStringValueHolder> aValues = {};
		
		SCR_EditorAttributeFloatStringValueHolder val0 = new SCR_EditorAttributeFloatStringValueHolder();
		val0.SetName("Auto");
		val0.SetFloatValue(0);
		aValues.Insert(val0);
		
		SCR_EditorAttributeFloatStringValueHolder val1 = new SCR_EditorAttributeFloatStringValueHolder();
		val1.SetName("Off");
		val1.SetFloatValue(1);
		aValues.Insert(val1);
		
		SCR_EditorAttributeFloatStringValueHolder val2 = new SCR_EditorAttributeFloatStringValueHolder();
		val2.SetName("Headlights");
		val2.SetFloatValue(2);
		aValues.Insert(val2);
		
		SCR_EditorAttributeFloatStringValueHolder val3 = new SCR_EditorAttributeFloatStringValueHolder();
		val3.SetName("HiBeams");
		val3.SetFloatValue(3);
		aValues.Insert(val3);

		SCR_EditorAttributeFloatStringValueHolder val4 = new SCR_EditorAttributeFloatStringValueHolder();
		val4.SetName("Hazards");
		val4.SetFloatValue(4);
		aValues.Insert(val4);

		outEntries.Insert(new SCR_EditorAttributePresetEntry(1, false, "", false, false, "", -1));
		outEntries.Insert(new SCR_BaseEditorAttributeFloatStringValues(aValues));
		return outEntries.Count();
	}
	
	override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
	{
		SCR_EditableEntityComponent editableEntity = SCR_EditableEntityComponent.Cast(item);
		if (!editableEntity) return null;
		
		IEntity entity = editableEntity.GetOwner();
		if (!entity) return null;
		
		Vehicle vehicle = Vehicle.Cast(entity);
		if (!vehicle) return null;
		
		VehicleControllerComponent controller = VehicleControllerComponent.Cast(vehicle.FindComponent(VehicleControllerComponent));
		if (!controller) return null;
		
		return SCR_BaseEditorAttributeVar.CreateInt(controller.ATH_GetLightsForcedState());
	}
	
	override void WriteVariable(Managed item, SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, int playerID)
	{
		if (!var) return;
		
		SCR_EditableEntityComponent editableEntity = SCR_EditableEntityComponent.Cast(item);
		if (!editableEntity) return;
		
		IEntity entity = editableEntity.GetOwner();
		if (!entity) return;
		
		Vehicle vehicle = Vehicle.Cast(entity);
		if (!vehicle) return;
		
		VehicleControllerComponent controller = VehicleControllerComponent.Cast(vehicle.FindComponent(VehicleControllerComponent));
		if (!controller) return;
		
		controller.ATH_SetLightsForcedState(var.GetInt());
	}
}
