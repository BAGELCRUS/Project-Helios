[BaseContainerProps(), SCR_BaseContainerCustomTitleUIInfo("m_UIInfo")]
class ATH_LightsToggleEditorAttribute : SCR_BaseEditorAttribute
{
	override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
	{
		IEntity entity = IEntity.Cast(item);
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
		
		IEntity entity = IEntity.Cast(item);
		if (!entity) return;
		
		Vehicle vehicle = Vehicle.Cast(entity);
		if (!vehicle) return;
		
		VehicleControllerComponent controller = VehicleControllerComponent.Cast(vehicle.FindComponent(VehicleControllerComponent));
		if (!controller) return;
		
		controller.ATH_SetLightsForcedState(var.GetInt());
	}
}
