enum EAthenaIntelShareType
{
	Nobody,
	Individual,
	Group,
	Faction,
	Everyone
}

[ComponentEditorProps(category: "GameScripted/Intel", description: "Component to store intel information and settings.")]
class ATHENA_IntelComponentClass : ScriptComponentClass
{
}

class ATHENA_IntelComponent : ScriptComponent
{
	[Attribute("Confidential Intel", UIWidgets.EditBox, desc: "Title of the Intel displayed in the UI.", category: "Intel Settings"), RplProp()]
	protected string m_sIntelTitle;

	[Attribute("Document", UIWidgets.EditBox, desc: "Type of the Intel (e.g. Document, Map, Radio).", category: "Intel Settings"), RplProp()]
	protected string m_sIntelType;

	[Attribute("Top secret description...", UIWidgets.EditBoxMultiline, desc: "Description of the Intel displayed in the UI.", category: "Intel Settings"), RplProp()]
	protected string m_sIntelDescription;

	[Attribute("0", UIWidgets.ComboBox, desc: "Who should receive a notification when this is opened?", enums: ParamEnumArray.FromEnum(EAthenaIntelShareType), category: "Intel Settings"), RplProp()]
	protected EAthenaIntelShareType m_ShareType;

	[Attribute("false", UIWidgets.CheckBox, desc: "Should picking up this intel ping the Game Master?", category: "Intel Settings"), RplProp()]
	protected bool m_bPingGM;

	[Attribute("false", UIWidgets.CheckBox, desc: "Should this intel item be deleted from the world after it is opened?", category: "Intel Settings"), RplProp()]
	protected bool m_bDeleteOnCompletion;

	[Attribute("Read Intel", UIWidgets.EditBox, desc: "Action text shown to the player.", category: "Intel Settings"), RplProp()]
	protected string m_sActionText;

	[Attribute("", UIWidgets.ResourcePickerThumbnail, desc: "Action sound played upon completion", params: "acp", category: "Intel Settings"), RplProp()]
	protected ResourceName m_ActionSound;

	[Attribute("", UIWidgets.EditBox, desc: "The specific event name to START the sound (e.g., SOUND_PICKUP_START)", category: "Intel Settings"), RplProp()]
	protected string m_sActionSoundEventStart;

	[Attribute("", UIWidgets.EditBox, desc: "The specific event name to END the sound (e.g., SOUND_PICKUP_STOP)", category: "Intel Settings"), RplProp()]
	protected string m_sActionSoundEventEnd;

	[Attribute("0", UIWidgets.Slider, desc: "How long the action takes (seconds)", params: "0 60 1", category: "Intel Settings"), RplProp()]
	protected float m_fActionDuration;

	// Getters
	string GetTitle()
	{
		return m_sIntelTitle;
	}

	string GetType()
	{
		return m_sIntelType;
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

	// Setters
	void SetTitle(string val) { m_sIntelTitle = val; Replication.BumpMe(); }
	void SetType(string val) { m_sIntelType = val; Replication.BumpMe(); }
	void SetDescription(string val) { m_sIntelDescription = val; Replication.BumpMe(); }
	void SetShareType(EAthenaIntelShareType val) { m_ShareType = val; Replication.BumpMe(); }
	void SetPingGM(bool val) { m_bPingGM = val; Replication.BumpMe(); }
	void SetDeleteOnCompletion(bool val) { m_bDeleteOnCompletion = val; Replication.BumpMe(); }
	void SetActionText(string val) { m_sActionText = val; Replication.BumpMe(); }
	void SetActionSound(ResourceName val) { m_ActionSound = val; Replication.BumpMe(); }
	void SetActionSoundEventStart(string val) { m_sActionSoundEventStart = val; Replication.BumpMe(); }
	void SetActionSoundEventEnd(string val) { m_sActionSoundEventEnd = val; Replication.BumpMe(); }
	void SetActionDuration(float val) { m_fActionDuration = val; Replication.BumpMe(); }

	//------------------------------------------------------------------------------------------------
	void ApplyIntelSettings(
		string title,
		string intelType,
		string description,
		string actionText,
		EAthenaIntelShareType shareType,
		bool pingGM,
		bool deleteOnCompletion,
		ResourceName actionSound,
		string soundStart,
		string soundEnd,
		float actionDuration
	)
	{
		m_sIntelTitle = title;
		m_sIntelType = intelType;
		m_sIntelDescription = description;
		m_sActionText = actionText;
		m_ShareType = shareType;
		m_bPingGM = pingGM;
		m_bDeleteOnCompletion = deleteOnCompletion;
		m_ActionSound = actionSound;
		m_sActionSoundEventStart = soundStart;
		m_sActionSoundEventEnd = soundEnd;
		m_fActionDuration = actionDuration;
		Replication.BumpMe();
	}

	//------------------------------------------------------------------------------------------------
	// Called by the Game Master Edit Intel dialog to update an existing Intel entity (or swap its prefab) on the server.
	void RequestUpdateIntel(
		ResourceName newPrefab,
		string title,
		string intelType,
		string description,
		string actionText,
		EAthenaIntelShareType shareType,
		bool pingGM,
		bool deleteOnCompletion,
		int soundIndex,
		float actionDuration
	)
	{
		// Pack shareType (0..15), pingGM (16), deleteOnCompletion (32),
		// soundIndex (0..1023 * 64), and actionDuration in tenths of a second (0..1023 * 65536)
		// so RPCs stay strictly within Enfusion's parameter limit.
		int packedFlags = (shareType & 0x0F);
		if (pingGM)
			packedFlags = packedFlags | 16;
		if (deleteOnCompletion)
			packedFlags = packedFlags | 32;
		if (soundIndex > 0)
			packedFlags = packedFlags + ((soundIndex & 0x3FF) * 64);

		int durationTenths = Math.ClampInt(Math.Round(actionDuration * 10.0), 0, 1023);
		packedFlags = packedFlags + ((durationTenths & 0x3FF) * 65536);

		if (Replication.IsServer())
		{
			ServerApplyPackedUpdate(
				newPrefab,
				title,
				intelType,
				description,
				actionText,
				packedFlags
			);
			return;
		}

		// On a dedicated server, world Intel entities are server-owned, so relay the update
		// via the local client's PlayerController-owned ATHENA_PlayerIntelComponent.
		ATHENA_PlayerIntelComponent playerIntel = ATHENA_PlayerIntelComponent.GetLocalInstance();
		RplId intelId = Replication.FindItemId(this);
		if (playerIntel && intelId.IsValid())
		{
			playerIntel.RequestUpdateIntelOnServer(
				intelId,
				newPrefab,
				title,
				intelType,
				description,
				actionText,
				packedFlags
			);
		}
		else
		{
			Rpc(
				RpcDo_UpdateIntel,
				newPrefab,
				title,
				intelType,
				description,
				actionText,
				packedFlags
			);
		}
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcDo_UpdateIntel(
		ResourceName newPrefab,
		string title,
		string intelType,
		string description,
		string actionText,
		int packedFlags
	)
	{
		ServerApplyPackedUpdate(
			newPrefab,
			title,
			intelType,
			description,
			actionText,
			packedFlags
		);
	}

	//------------------------------------------------------------------------------------------------
	void ServerApplyPackedUpdate(
		ResourceName newPrefab,
		string title,
		string intelType,
		string description,
		string actionText,
		int packedFlags
	)
	{
		IEntity owner = GetOwner();
		if (!owner)
			return;

		EAthenaIntelShareType shareType = (packedFlags & 0x0F);
		bool pingGM = ((packedFlags & 16) != 0);
		bool deleteOnCompletion = ((packedFlags & 32) != 0);
		int soundShifted = packedFlags / 64;
		int soundIndex = (soundShifted & 0x3FF);
		int durationShifted = packedFlags / 65536;
		int durationTenths = (durationShifted & 0x3FF);
		float actionDuration = durationTenths * 0.1;

		ResourceName actionSound = m_ActionSound;
		string soundStart = m_sActionSoundEventStart;
		string soundEnd = m_sActionSoundEventEnd;

		ATHENA_IntelConfig config = ATHENA_IntelConfig.LoadConfig();
		if (config && config.m_aActionSounds && soundIndex >= 0 && soundIndex < config.m_aActionSounds.Count())
		{
			ATHENA_IntelSoundSetting soundSetting = config.m_aActionSounds[soundIndex];
			if (soundSetting)
			{
				actionSound = soundSetting.m_ActionSound;
				soundStart = soundSetting.m_sActionSoundEventStart;
				soundEnd = soundSetting.m_sActionSoundEventEnd;
			}
		}

		ResourceName currentPrefab = ResourceName.Empty;
		EntityPrefabData prefabData = owner.GetPrefabData();
		if (prefabData)
			currentPrefab = prefabData.GetPrefabName();

		// If the GM switched to a different prefab, respawn at the same transform with the updated settings
		if (!newPrefab.IsEmpty() && newPrefab != currentPrefab)
		{
			Resource res = Resource.Load(newPrefab);
			if (res && res.IsValid())
			{
				vector mat[4];
				owner.GetTransform(mat);

				EntitySpawnParams params = new EntitySpawnParams();
				params.TransformMode = ETransformMode.WORLD;
				params.Transform = mat;

				IEntity newIntel = GetGame().SpawnEntityPrefab(res, owner.GetWorld(), params);
				if (newIntel)
				{
					ATHENA_IntelComponent newIntelComp = ATHENA_IntelComponent.Cast(newIntel.FindComponent(ATHENA_IntelComponent));
					if (newIntelComp)
					{
						newIntelComp.ApplyIntelSettings(
							title,
							intelType,
							description,
							actionText,
							shareType,
							pingGM,
							deleteOnCompletion,
							actionSound,
							soundStart,
							soundEnd,
							actionDuration
						);
					}

					SCR_EntityHelper.DeleteEntityAndChildren(owner);
					return;
				}
			}
			else
			{
				Print("ATHENA_IntelComponent: Failed to load replacement prefab: " + newPrefab + ", updating in-place instead.", LogLevel.WARNING);
			}
		}

		// Update this Intel entity in-place
		ApplyIntelSettings(
			title,
			intelType,
			description,
			actionText,
			shareType,
			pingGM,
			deleteOnCompletion,
			actionSound,
			soundStart,
			soundEnd,
			actionDuration
		);
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
	void RequestPing(bool pingGM, EAthenaIntelShareType shareType, string title, string type, string description, int playerId)
	{
		if (Replication.IsServer())
		{
			RpcDo_PingServer(pingGM, shareType, title, type, description, playerId);
		}
		else
		{
			Rpc(RpcDo_PingServer, pingGM, shareType, title, type, description, playerId);
		}
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcDo_PingServer(bool pingGM, EAthenaIntelShareType shareType, string title, string type, string description, int playerId)
	{
		// Notify clients depending on ShareType
		if (shareType == EAthenaIntelShareType.Group || shareType == EAthenaIntelShareType.Faction || shareType == EAthenaIntelShareType.Everyone)
		{
			Rpc(RpcDo_ShowHintClient, title, type, description, false, shareType, playerId);
		}
		
		if (pingGM)
		{
			string pName = GetGame().GetPlayerManager().GetPlayerName(playerId);
			Print("ATHENA_IntelComponent Server: Broadcasting GM Ping for player " + pName, LogLevel.NORMAL);
			// Ping GM separately (true flag)
			Rpc(RpcDo_ShowHintClient, title, type, description, true, EAthenaIntelShareType.Nobody, playerId);
		}
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void RpcDo_ShowHintClient(string title, string type, string description, bool gmOnly, EAthenaIntelShareType shareType, int sourcePlayerId)
	{
		int localPlayerId = SCR_PlayerController.GetLocalPlayerId();
		
		if (gmOnly)
		{
			SCR_EditorManagerEntity editorManager = SCR_EditorManagerEntity.GetInstance();
			if (!editorManager)
				return;
			
			string playerName = GetGame().GetPlayerManager().GetPlayerName(sourcePlayerId);
			if (playerName == string.Empty) playerName = "Unknown Player";
			
			string msg = string.Format("%1 Has collected Intel", playerName);
			Print("ATHENA_IntelComponent GM Client: " + msg + " (" + title + ")", LogLevel.NORMAL);
			
			SCR_HintManagerComponent.GetInstance().ShowCustomHint(msg, title, 10.0, false);
			return;
		}
		
		// For Faction/Group/Everyone logic:
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
		else if (shareType == EAthenaIntelShareType.Everyone)
		{
			// Everyone gets it, no filtering needed
		}
		else 
		{
			// Nobody or Individual shouldn't trigger this broadcast
			return;
		}

		ATHENA_PlayerIntelComponent pic = ATHENA_PlayerIntelComponent.GetLocalInstance();
		if (pic)
		{
			pic.AddIntelLocal(title, type, description, "Received", shareType);
		}

		SCR_HintManagerComponent.GetInstance().ShowCustomHint(title, "Intel Discovered", 10.0, false);
	}
}
