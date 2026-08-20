enum EAthenaIntelShareType
{
	Nobody,
	Group,
	Faction
}

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

	[Attribute("0", UIWidgets.ComboBox, desc: "Who should receive a notification when this is opened?", enums: ParamEnumArray.FromEnum(EAthenaIntelShareType), category: "Intel Settings")]
	protected EAthenaIntelShareType m_ShareType;

	[Attribute("false", UIWidgets.CheckBox, desc: "Should picking up this intel ping the Game Master?", category: "Intel Settings")]
	protected bool m_bPingGM;

	[Attribute("false", UIWidgets.CheckBox, desc: "Should this intel item be deleted from the world after it is opened?", category: "Intel Settings")]
	protected bool m_bDeleteOnCompletion;

	[Attribute("Read Intel", UIWidgets.EditBox, desc: "Action text shown to the player.", category: "Intel Settings")]
	protected string m_sActionText;

	[Attribute("", UIWidgets.ResourcePickerThumbnail, desc: "Action sound played upon completion", params: "acp", category: "Intel Settings")]
	protected ResourceName m_ActionSound;

	[Attribute("", UIWidgets.EditBox, desc: "The specific event name to START the sound (e.g., SOUND_PICKUP_START)", category: "Intel Settings")]
	protected string m_sActionSoundEventStart;

	[Attribute("", UIWidgets.EditBox, desc: "The specific event name to END the sound (e.g., SOUND_PICKUP_STOP)", category: "Intel Settings")]
	protected string m_sActionSoundEventEnd;

	[Attribute("0", UIWidgets.Slider, desc: "How long the action takes (seconds)", params: "0 60 1", category: "Intel Settings")]
	protected float m_fActionDuration;

	// Getters
	string GetTitle()
	{
		return m_sIntelTitle;
	}

	string GetDescription()
	{
		return m_sIntelDescription;
	}
	
	EAthenaIntelShareType GetShareType()
	{
		return m_ShareType;
	}

	bool ShouldPingGM()
	{
		return m_bPingGM;
	}

	bool ShouldDeleteOnCompletion()
	{
		return m_bDeleteOnCompletion;
	}
	
	string GetActionText()
	{
		return m_sActionText;
	}
	
	ResourceName GetActionSound()
	{
		return m_ActionSound;
	}
	
	string GetActionSoundEventStart()
	{
		return m_sActionSoundEventStart;
	}

	string GetActionSoundEventEnd()
	{
		return m_sActionSoundEventEnd;
	}
	
	float GetActionDuration()
	{
		return m_fActionDuration;
	}
	
	// Server-side function to handle deleting the entity
	void DeleteIntelEntity()
	{
		if (Replication.IsServer())
		{
			IEntity owner = GetOwner();
			if (owner)
			{
				SCR_EntityHelper.DeleteEntityAndChildren(owner);
			}
		}
		else
		{
			Rpc(RpcDo_DeleteIntelEntity);
		}
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
	void RequestPing(bool pingGM, EAthenaIntelShareType shareType, string title, int playerId)
	{
		Rpc(RpcDo_PingServer, pingGM, shareType, title, playerId);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcDo_PingServer(bool pingGM, EAthenaIntelShareType shareType, string title, int playerId)
	{
		// Notify clients depending on ShareType
		// Currently only basic support for Nobody/Group/Faction
		if (shareType != EAthenaIntelShareType.Nobody)
		{
			// Simplification: In a full implementation, we'd iterate over faction/group members.
			// For now, we will broadcast and let clients filter.
			Rpc(RpcDo_ShowHintClient, title, false, shareType, playerId);
		}
		
		if (pingGM)
		{
			// Ping GM separately (true flag)
			Rpc(RpcDo_ShowHintClient, title, true, EAthenaIntelShareType.Nobody, playerId);
		}
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void RpcDo_ShowHintClient(string title, bool gmOnly, EAthenaIntelShareType shareType, int sourcePlayerId)
	{
		int localPlayerId = SCR_PlayerController.GetLocalPlayerId();
		
		if (gmOnly)
		{
			SCR_EditorManagerEntity editorManager = SCR_EditorManagerEntity.GetInstance();
			if (!editorManager || !editorManager.IsOpened())
				return;
			
			SCR_HintManagerComponent.GetInstance().ShowCustomHint(string.Format("Intel '%1' was collected", title), "Intel Collected", 10.0, false);
			return;
		}
		
		// For Faction/Group logic:
		if (shareType == EAthenaIntelShareType.Faction)
		{
			SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
			if (factionManager)
			{
				Faction sourceFaction = factionManager.GetPlayerFaction(sourcePlayerId);
				Faction localFaction = factionManager.GetPlayerFaction(localPlayerId);
				if (sourceFaction != localFaction || !sourceFaction)
					return; // Not same faction
			}
		}
		else if (shareType == EAthenaIntelShareType.Group)
		{
			SCR_GroupsManagerComponent groupManager = SCR_GroupsManagerComponent.GetInstance();
			if (groupManager)
			{
				SCR_AIGroup sourceGroup = groupManager.GetPlayerGroup(sourcePlayerId);
				SCR_AIGroup localGroup = groupManager.GetPlayerGroup(localPlayerId);
				if (sourceGroup != localGroup || !sourceGroup)
					return; // Not same group
			}
		}

		SCR_HintManagerComponent.GetInstance().ShowCustomHint(title, "Intel Discovered", 10.0, false);
	}
}
