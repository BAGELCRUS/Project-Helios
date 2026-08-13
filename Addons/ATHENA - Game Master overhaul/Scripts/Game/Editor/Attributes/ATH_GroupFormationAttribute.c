[BaseContainerProps(), SCR_BaseContainerCustomTitleUIInfo("m_UIInfo")]
class ATH_GroupFormationAttribute : SCR_BaseValueListEditorAttribute
{
	protected static const ref array<string> FORMATION_NAMES = {
		"Wedge",
		"Line",
		"Column",
		"Staggered Column",
		"Vee",
		"Echelon Left",
		"Echelon Right"
	};
	
	protected static const ref array<string> FORMATION_VALUES = {
		"Wedge",
		"Line",
		"Column",
		"StaggeredColumn",
		"Vee",
		"EchelonLeft",
		"EchelonRight"
	};

	override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
	{
		SCR_EditableEntityComponent editableEntity = SCR_EditableEntityComponent.Cast(item);
		if (!editableEntity) return null;
		
		IEntity entity = editableEntity.GetOwner();
		if (!entity) return null;
		
		AIFormationComponent formationComp = AIFormationComponent.Cast(entity.FindComponent(AIFormationComponent));
		if (!formationComp) return null;
		
		AIFormationDefinition formationDef = formationComp.GetFormation();
		string currentFormation = "";
		if (formationDef)
		{
			currentFormation = formationDef.GetName();
		}
		
		int selectedIndex = 0;
		for (int i = 0; i < FORMATION_VALUES.Count(); i++)
		{
			if (FORMATION_VALUES[i] == currentFormation)
			{
				selectedIndex = i;
				break;
			}
		}
		
		return SCR_BaseEditorAttributeVar.CreateInt(selectedIndex);
	}
	
	override void WriteVariable(Managed item, SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, int playerID)
	{
		if (!var) return;
		
		SCR_EditableEntityComponent editableEntity = SCR_EditableEntityComponent.Cast(item);
		if (!editableEntity) return;
		
		IEntity entity = editableEntity.GetOwner();
		if (!entity) return;
		
		AIFormationComponent formationComp = AIFormationComponent.Cast(entity.FindComponent(AIFormationComponent));
		if (!formationComp) return;
		
		int selectedIndex = var.GetInt();
		if (selectedIndex >= 0 && selectedIndex < FORMATION_VALUES.Count())
		{
			formationComp.SetFormation(FORMATION_VALUES[selectedIndex]);
		}
	}
	
	override int GetEntries(notnull array<ref SCR_BaseEditorAttributeEntry> outEntries)
	{
		for (int i = 0; i < FORMATION_NAMES.Count(); i++)
		{
			SCR_BaseEditorAttributeEntryText entry = new SCR_BaseEditorAttributeEntryText(FORMATION_NAMES[i]);
			outEntries.Insert(entry);
		}
		
		return outEntries.Count();
	}
}
