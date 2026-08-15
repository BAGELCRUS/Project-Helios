[ComponentEditorProps(category: "GameScripted/Intel", description: "Component to store intel information and settings.")]
class ATHENA_IntelComponentClass : ScriptComponentClass
{
}

class ATHENA_IntelComponent : ScriptComponent
{
	[Attribute("Confidential Intel", UIWidgets.EditBox, desc: "Title of the Intel displayed in the UI.", category: "Intel Settings")]
	protected string m_sIntelTitle;

	[Attribute("Top secret description...", UIWidgets.EditBoxMultiline, desc: "Description of the Intel displayed in the UI.", category: "Intel Settings")]
	protected string m_sIntelDescription;

	[Attribute("false", UIWidgets.CheckBox, desc: "Should reading this intel ping the Game Master?", category: "Intel Settings")]
	protected bool m_bPingGM;

	[Attribute("false", UIWidgets.CheckBox, desc: "Should reading this intel ping everyone on the server?", category: "Intel Settings")]
	protected bool m_bPingEveryone;

	[Attribute("false", UIWidgets.CheckBox, desc: "Should this intel item be deleted from the world after it is opened?", category: "Intel Settings")]
	protected bool m_bDeleteAfterOpen;

	// Getters
	string GetTitle()
	{
		return m_sIntelTitle;
	}

	string GetDescription()
	{
		return m_sIntelDescription;
	}

	bool ShouldPingGM()
	{
		return m_bPingGM;
	}

	bool ShouldPingEveryone()
	{
		return m_bPingEveryone;
	}

	bool ShouldDeleteAfterOpen()
	{
		return m_bDeleteAfterOpen;
	}
	
	// Server-side function to handle deleting the entity
	void DeleteIntelEntity()
	{
		Rpc(RpcDo_DeleteIntelEntity);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcDo_DeleteIntelEntity()
	{
		IEntity owner = GetOwner();
		if (owner)
		{
			SCR_EntityHelper.DeleteEntityAndChildren(owner);
		}
	}
	
	// Networking logic for pings
	void RequestPing(bool pingGM, bool pingEveryone, string title)
	{
		Rpc(RpcDo_PingServer, pingGM, pingEveryone, title);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcDo_PingServer(bool pingGM, bool pingEveryone, string title)
	{
		// On the server, we dispatch the notification to clients.
		// For dynamic strings, vanilla notifications don't easily support it without custom enums.
		// So we use a multicast RPC to show a hint on all clients.
		if (pingEveryone)
		{
			Rpc(RpcDo_ShowHintClient, title, false);
		}
		else if (pingGM)
		{
			Rpc(RpcDo_ShowHintClient, title, true);
		}
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void RpcDo_ShowHintClient(string title, bool gmOnly)
	{
		// If it's GM only, check if local player is GM (requires SCR_EditorManagerEntity check, keeping simple for now)
		if (gmOnly)
		{
			SCR_EditorManagerEntity editorManager = SCR_EditorManagerEntity.GetInstance();
			if (!editorManager || !editorManager.IsOpened())
				return;
		}

		SCR_HintManagerComponent.GetInstance().ShowCustomHint(title, "Intel Discovered", 10.0, false);
	}
}
