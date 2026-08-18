class ATHENA_IntelWorldAction : ScriptedUserAction
{
	protected ATHENA_IntelComponent m_IntelComp;

	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_IntelComp = ATHENA_IntelComponent.Cast(pOwnerEntity.FindComponent(ATHENA_IntelComponent));
		
		// Note: Action Duration cannot be dynamically populated into the action from the component in script.
		// It must be set directly in the prefab's ActionsManager component on this ScriptedUserAction.
	}

	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!m_IntelComp)
		{
			Print("ATHENA_IntelWorldAction: No ATHENA_IntelComponent found on entity!", LogLevel.ERROR);
			return;
		}

		string title = m_IntelComp.GetTitle();
		string description = m_IntelComp.GetDescription();
		
		ATHENA_IntelPlayerUI uiInstance = ATHENA_IntelPlayerUI.Open(title, description);

		// Handle Pings (Notifications) via RPC
		EAthenaIntelShareType shareType = m_IntelComp.GetShareType();
		bool pingGM = m_IntelComp.ShouldPingGM();
		
		if (pingGM || shareType != EAthenaIntelShareType.Nobody)
		{
			// We pass the local player ID as the source of the interaction
			int playerId = SCR_PlayerController.GetLocalPlayerId();
			m_IntelComp.RequestPing(pingGM, shareType, title, playerId);
		}

		// Handle Deletion
		if (m_IntelComp.ShouldDeleteOnCompletion())
		{
			m_IntelComp.DeleteIntelEntity();
		}
	}

	override bool GetActionNameScript(out string outName)
	{
		if (m_IntelComp)
			outName = m_IntelComp.GetActionText();
		else
			outName = "Read Intel";
		
		return true;
	}

	override bool CanBeShownScript(IEntity user)
	{
		return true;
	}

	override bool CanBePerformedScript(IEntity user)
	{
		return true;
	}
	
	override bool HasLocalEffectOnlyScript()
	{
		// Returning true means PerformAction runs ONLY on the Client who interacted.
		// This is required so the Client can open their local UI.
		// The ping and delete logic are handled via RPCs to the server inside PerformAction.
		return true; 
	}
}
