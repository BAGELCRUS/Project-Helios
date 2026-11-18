[ComponentEditorProps(category: "GameScripted/Intel", description: "Component that opens a custom UI to configure and spawn intel when placed in Game Master.")]
class ATHENA_IntelSpawnerComponentClass : ScriptComponentClass
{
}

class ATHENA_IntelSpawnerComponent : ScriptComponent
{
	[Attribute("{6AE58B21CE701D23}UI/layouts/Intel/IntelSpawnerDialog.layout", UIWidgets.ResourcePickerThumbnail, desc: "The UI layout opened when spawning intel in GM.", params: "layout", category: "Intel Spawner")]
	protected ResourceName m_sUI;

	[Attribute("{5256290A5C99C729}Prefabs/Props/Military/AmmoBoxes/EquipmentBoxStack/US/EquipmentBoxStack_US_01__intel_Test.et", UIWidgets.ResourcePickerThumbnail, desc: "Fallback Intel prefab if none is selected in config.", params: "et", category: "Intel Spawner")]
	protected ResourceName m_DefaultFallbackPrefab;

	[Attribute("Confidential Intel", UIWidgets.EditBox, desc: "Default Title shown in the Spawner UI.", category: "Intel Defaults")]
	protected string m_sDefaultTitle;

	[Attribute("Document", UIWidgets.EditBox, desc: "Default Type shown in the Spawner UI.", category: "Intel Defaults")]
	protected string m_sDefaultType;

	[Attribute("Top secret description...", UIWidgets.EditBoxMultiline, desc: "Default Description shown in the Spawner UI.", category: "Intel Defaults")]
	protected string m_sDefaultDescription;

	[Attribute("Read Intel", UIWidgets.EditBox, desc: "Default Action Text shown in the Spawner UI.", category: "Intel Defaults")]
	protected string m_sDefaultActionText;

	[Attribute("1", UIWidgets.ComboBox, desc: "Default Share Type in the Spawner UI.", enums: ParamEnumArray.FromEnum(EAthenaIntelShareType), category: "Intel Defaults")]
	protected EAthenaIntelShareType m_DefaultShareType;

	[Attribute("false", UIWidgets.CheckBox, desc: "Default Ping GM setting in the Spawner UI.", category: "Intel Defaults")]
	protected bool m_bDefaultPingGM;

	[Attribute("false", UIWidgets.CheckBox, desc: "Default Delete on Completion setting in the Spawner UI.", category: "Intel Defaults")]
	protected bool m_bDefaultDeleteOnCompletion;

	[Attribute("0", UIWidgets.Slider, desc: "Default Action Duration (seconds) in the Spawner UI.", params: "0 60 1", category: "Intel Defaults")]
	protected float m_fDefaultActionDuration;

	protected bool m_bDialogOpened = false;

	//------------------------------------------------------------------------------------------------
	string GetDefaultTitle() { return m_sDefaultTitle; }
	string GetDefaultType() { return m_sDefaultType; }
	string GetDefaultDescription() { return m_sDefaultDescription; }
	string GetDefaultActionText() { return m_sDefaultActionText; }
	EAthenaIntelShareType GetDefaultShareType() { return m_DefaultShareType; }
	bool GetDefaultPingGM() { return m_bDefaultPingGM; }
	bool GetDefaultDeleteOnCompletion() { return m_bDefaultDeleteOnCompletion; }
	float GetDefaultActionDuration() { return m_fDefaultActionDuration; }
	ResourceName GetDefaultFallbackPrefab() { return m_DefaultFallbackPrefab; }

	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		SetEventMask(owner, EntityEvent.INIT);
	}

	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);

		if (!GetGame().InPlayMode())
			return;

		// Wait briefly for Game Master placement state to finish initializing
		GetGame().GetCallqueue().CallLater(CheckGMSpawn, 150, false, owner);
	}

	//------------------------------------------------------------------------------------------------
	void CheckGMSpawn(IEntity owner)
	{
		if (!owner || m_bDialogOpened)
			return;

		// Ignore ghost preview entities while dragging in Game Master
		SCR_EditablePreviewComponent previewComp = SCR_EditablePreviewComponent.Cast(owner.FindComponent(SCR_EditablePreviewComponent));
		if (previewComp)
			return;

		SCR_EditorManagerEntity editorManager = SCR_EditorManagerEntity.GetInstance();
		if (!editorManager || !editorManager.IsOpened())
			return;

		OpenIntelSpawnerUI(owner);
	}

	//------------------------------------------------------------------------------------------------
	void OpenIntelSpawnerUI(IEntity owner)
	{
		if (m_bDialogOpened)
			return;

		m_bDialogOpened = true;
		Print("ATHENA_IntelSpawnerComponent: Opening Intel Spawner Dialog UI.", LogLevel.NORMAL);
		ATHENA_IntelSpawnerDialogUI.Open(this);
	}

	//------------------------------------------------------------------------------------------------
	void SpawnConfiguredIntel(
		ResourceName prefab,
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
		ResourceName prefabToSpawn = prefab;
		if (prefabToSpawn.IsEmpty())
			prefabToSpawn = m_DefaultFallbackPrefab;
		if (prefabToSpawn.IsEmpty())
			prefabToSpawn = ATHENA_IntelConfig.FALLBACK_PREFAB;

		// Pack shareType (bits 0..7), pingGM (bit 8), deleteOnCompletion (bit 9), and soundIndex (bits 16..31)
		// to stay within Enfusion's 8-parameter RPC limit.
		int packedFlags = (shareType & 0xFF);
		if (pingGM)
			packedFlags = packedFlags | (1 << 8);
		if (deleteOnCompletion)
			packedFlags = packedFlags | (1 << 9);
		if (soundIndex > 0)
			packedFlags = packedFlags | ((soundIndex & 0xFFFF) << 16);

		if (Replication.IsServer())
		{
			RpcDo_SpawnConfiguredIntel(
				prefabToSpawn,
				title,
				intelType,
				description,
				actionText,
				packedFlags,
				actionDuration
			);
		}
		else
		{
			Rpc(
				RpcDo_SpawnConfiguredIntel,
				prefabToSpawn,
				title,
				intelType,
				description,
				actionText,
				packedFlags,
				actionDuration
			);
		}
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcDo_SpawnConfiguredIntel(
		ResourceName prefab,
		string title,
		string intelType,
		string description,
		string actionText,
		int packedFlags,
		float actionDuration
	)
	{
		IEntity owner = GetOwner();
		if (!owner)
			return;

		EAthenaIntelShareType shareType = (packedFlags & 0xFF);
		bool pingGM = ((packedFlags & (1 << 8)) != 0);
		bool deleteOnCompletion = ((packedFlags & (1 << 9)) != 0);
		int soundIndex = ((packedFlags >> 16) & 0xFFFF);

		ResourceName actionSound = "";
		string soundStart = "";
		string soundEnd = "";

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

		vector mat[4];
		owner.GetTransform(mat);

		EntitySpawnParams params = new EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform = mat;

		Resource res = Resource.Load(prefab);
		if (!res || !res.IsValid())
		{
			Print("ATHENA_IntelSpawnerComponent: Failed to load prefab resource: " + prefab, LogLevel.ERROR);
			return;
		}

		IEntity newIntel = GetGame().SpawnEntityPrefab(res, owner.GetWorld(), params);
		if (newIntel)
		{
			ATHENA_IntelComponent intelComp = ATHENA_IntelComponent.Cast(newIntel.FindComponent(ATHENA_IntelComponent));
			if (intelComp)
			{
				intelComp.SetTitle(title);
				intelComp.SetType(intelType);
				intelComp.SetDescription(description);
				intelComp.SetActionText(actionText);
				intelComp.SetShareType(shareType);
				intelComp.SetPingGM(pingGM);
				intelComp.SetDeleteOnCompletion(deleteOnCompletion);
				intelComp.SetActionSound(actionSound);
				intelComp.SetActionSoundEventStart(soundStart);
				intelComp.SetActionSoundEventEnd(soundEnd);
				intelComp.SetActionDuration(actionDuration);
			}
			else
			{
				Print("ATHENA_IntelSpawnerComponent: Spawned prefab does not have an ATHENA_IntelComponent!", LogLevel.WARNING);
			}

			// Delete the temporary spawner module
			SCR_EntityHelper.DeleteEntityAndChildren(owner);
		}
	}
}
