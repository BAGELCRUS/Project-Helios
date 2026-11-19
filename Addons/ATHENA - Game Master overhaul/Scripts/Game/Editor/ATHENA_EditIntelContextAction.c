[BaseContainerProps(), SCR_BaseContainerCustomTitleUIInfo("m_Info")]
class ATHENA_EditIntelContextAction : SCR_BaseContextAction
{
	//------------------------------------------------------------------------------------------------
	void InitDefault()
	{
		m_bEnabled = true;
		m_Info = SCR_UIInfo.CreateInfo(
			"Edit Intel",
			"Reopen the Intel configuration dialog to edit this Intel entity.",
			"{FDDA5068A208FC4E}UI/Textures/Editor/ContextMenu/ContextAction_Properties.edds"
		);
		m_ActionType = EEditorActionType.ACTION;
		m_ActionGroup = EEditorActionGroup.NONE;
		m_iOrder = 0;
		m_fCooldownTime = 0;
	}

	//------------------------------------------------------------------------------------------------
	static ATHENA_EditIntelContextAction CreateDefault()
	{
		ATHENA_EditIntelContextAction action = new ATHENA_EditIntelContextAction();
		action.InitDefault();
		return action;
	}

	//------------------------------------------------------------------------------------------------
	protected ATHENA_IntelComponent FindIntelOnEditable(SCR_EditableEntityComponent editableEntity)
	{
		if (!editableEntity)
			return null;

		IEntity owner = editableEntity.GetOwner();
		if (!owner)
			return null;

		ATHENA_IntelComponent intelComp = ATHENA_IntelComponent.Cast(owner.FindComponent(ATHENA_IntelComponent));
		if (intelComp)
			return intelComp;

		IEntity parent = owner.GetParent();
		if (parent)
			return ATHENA_IntelComponent.Cast(parent.FindComponent(ATHENA_IntelComponent));

		return null;
	}

	//------------------------------------------------------------------------------------------------
	protected ATHENA_IntelComponent GetTargetIntel(SCR_EditableEntityComponent hoveredEntity, notnull set<SCR_EditableEntityComponent> selectedEntities)
	{
		ATHENA_IntelComponent intelComp = FindIntelOnEditable(hoveredEntity);
		if (intelComp)
			return intelComp;

		if (selectedEntities.Count() == 1)
			return FindIntelOnEditable(selectedEntities.Get(0));

		return null;
	}

	//------------------------------------------------------------------------------------------------
	override bool IsServer()
	{
		return false;
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBeShown(SCR_EditableEntityComponent hoveredEntity, notnull set<SCR_EditableEntityComponent> selectedEntities, vector cursorWorldPosition, int flags)
	{
		return GetTargetIntel(hoveredEntity, selectedEntities) != null;
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBePerformed(SCR_EditableEntityComponent hoveredEntity, notnull set<SCR_EditableEntityComponent> selectedEntities, vector cursorWorldPosition, int flags)
	{
		return GetTargetIntel(hoveredEntity, selectedEntities) != null;
	}

	//------------------------------------------------------------------------------------------------
	override void Perform(SCR_EditableEntityComponent hoveredEntity, notnull set<SCR_EditableEntityComponent> selectedEntities, vector cursorWorldPosition, int flags, int param = -1)
	{
		ATHENA_IntelComponent intelComp = GetTargetIntel(hoveredEntity, selectedEntities);
		if (!intelComp)
			return;

		// Defer by 1ms so the context menu closes cleanly before our dialog takes focus
		GetGame().GetCallqueue().CallLater(OpenEditDialogDeferred, 1, false, intelComp);
	}

	//------------------------------------------------------------------------------------------------
	protected void OpenEditDialogDeferred(ATHENA_IntelComponent intelComp)
	{
		if (!intelComp || !intelComp.GetOwner())
			return;

		ATHENA_IntelSpawnerDialogUI.OpenForEdit(intelComp);
	}
}

//------------------------------------------------------------------------------------------------
// Automatically register "Edit Intel" in the Game Master Edit-mode context menu so no manual
// TempEdit.conf override is required (and avoid duplicates if also added via config).
modded class SCR_ContextActionsEditorComponent
{
	protected ref ATHENA_EditIntelContextAction m_ATHENA_EditIntelAction;

	override int GetActions(out notnull array<SCR_BaseEditorAction> actions)
	{
		int count = super.GetActions(actions);

		SCR_EditorModeEntity modeEntity = SCR_EditorModeEntity.Cast(GetOwner());
		if (modeEntity && modeEntity.GetModeType() != EEditorMode.EDIT)
			return count;

		// Avoid adding a duplicate if already configured in an action list .conf
		foreach (SCR_BaseEditorAction existing : actions)
		{
			if (ATHENA_EditIntelContextAction.Cast(existing))
				return count;
		}

		if (!m_ATHENA_EditIntelAction)
			m_ATHENA_EditIntelAction = ATHENA_EditIntelContextAction.CreateDefault();

		if (m_ATHENA_EditIntelAction)
		{
			actions.InsertAt(m_ATHENA_EditIntelAction, 0);
			count++;
		}

		return count;
	}
}
