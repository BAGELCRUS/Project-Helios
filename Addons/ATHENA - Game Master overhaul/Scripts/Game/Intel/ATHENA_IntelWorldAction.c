class ATHENA_IntelWorldAction : ScriptedUserAction
{
	[Attribute("Read Intel", UIWidgets.EditBox, desc: "Name of the action shown to the player.")]
	protected string m_sActionName;

	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		Print("ATHENA_IntelWorldAction: PerformAction CALLED", LogLevel.NORMAL);
		
		ATHENA_IntelComponent intelComp = ATHENA_IntelComponent.Cast(pOwnerEntity.FindComponent(ATHENA_IntelComponent));
		if (!intelComp)
		{
			Print("ATHENA_IntelWorldAction: No ATHENA_IntelComponent found on entity!", LogLevel.ERROR);
			return;
		}

		Print("ATHENA_IntelWorldAction: Component found, title=" + intelComp.GetTitle(), LogLevel.NORMAL);

		// 1. Open the UI via the static factory
		string title = intelComp.GetTitle();
		string description = intelComp.GetDescription();
		
		ATHENA_IntelPlayerUI uiInstance = ATHENA_IntelPlayerUI.Open(title, description);

		// 2. Handle Pings (Notifications) via RPC
		if (intelComp.ShouldPingGM() || intelComp.ShouldPingEveryone())
		{
			intelComp.RequestPing(intelComp.ShouldPingGM(), intelComp.ShouldPingEveryone(), title);
		}

		// 3. Handle Deletion
		if (intelComp.ShouldDeleteAfterOpen())
		{
			// We must ask the server to delete it.
			// Since ScriptedUserAction doesn't have an RPC directly built-in for this without extending,
			// we call a method on the component. If the component is replicated, it should ideally use an RPC.
			// For now, we assume the component has the delete logic.
			intelComp.DeleteIntelEntity();
		}
	}

	override bool GetActionNameScript(out string outName)
	{
		outName = m_sActionName;
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
