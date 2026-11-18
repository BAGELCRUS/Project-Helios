[BaseContainerProps(configRoot: true)]
class ATHENA_IntelConfig
{
	static const ResourceName DEFAULT_CONFIG_PATH = "{6AE58B21CE701D22}Configs/Intel/ATHENA_IntelConfig.conf";
	static const ResourceName FALLBACK_PREFAB = "{5256290A5C99C729}Prefabs/Props/Military/AmmoBoxes/EquipmentBoxStack/US/EquipmentBoxStack_US_01__intel_Test.et";

	[Attribute(desc: "Available intel prefabs to spawn.", params: "et")]
	ref array<ResourceName> m_aAvailablePrefabs;

	[Attribute(desc: "Available intel types (e.g. Document, Map, Radio).")]
	ref array<string> m_aIntelTypes;

	[Attribute(desc: "Available action sounds.")]
	ref array<ref ATHENA_IntelSoundSetting> m_aActionSounds;

	//------------------------------------------------------------------------------------------------
	static ATHENA_IntelConfig LoadConfig()
	{
		Resource res = BaseContainerTools.LoadContainer(DEFAULT_CONFIG_PATH);
		if (!res || !res.IsValid())
			res = BaseContainerTools.LoadContainer("Configs/Intel/ATHENA_IntelConfig.conf");

		ATHENA_IntelConfig config;
		if (res && res.IsValid() && res.GetResource())
		{
			BaseContainer container = res.GetResource().ToBaseContainer();
			if (container)
				config = ATHENA_IntelConfig.Cast(BaseContainerTools.CreateInstanceFromContainer(container));
		}

		if (!config)
			config = new ATHENA_IntelConfig();

		if (!config.m_aAvailablePrefabs || config.m_aAvailablePrefabs.IsEmpty())
		{
			config.m_aAvailablePrefabs = new array<ResourceName>();
			config.m_aAvailablePrefabs.Insert(FALLBACK_PREFAB);
		}

		if (!config.m_aIntelTypes || config.m_aIntelTypes.IsEmpty())
		{
			config.m_aIntelTypes = new array<string>();
			config.m_aIntelTypes.Insert("Document");
			config.m_aIntelTypes.Insert("Map");
			config.m_aIntelTypes.Insert("Radio");
			config.m_aIntelTypes.Insert("Computer");
			config.m_aIntelTypes.Insert("Photograph");
		}

		if (!config.m_aActionSounds || config.m_aActionSounds.IsEmpty())
		{
			config.m_aActionSounds = new array<ref ATHENA_IntelSoundSetting>();

			ATHENA_IntelSoundSetting noneSound = new ATHENA_IntelSoundSetting();
			noneSound.m_sName = "None";
			noneSound.m_ActionSound = "";
			noneSound.m_sActionSoundEventStart = "";
			noneSound.m_sActionSoundEventEnd = "";
			config.m_aActionSounds.Insert(noneSound);

			ATHENA_IntelSoundSetting defaultSound = new ATHENA_IntelSoundSetting();
			defaultSound.m_sName = "Default Interaction";
			defaultSound.m_ActionSound = "{951A2351637B9445}Sounds/Destruction/Buildings/Destruction_Buildings_Brick_Generic.acp";
			defaultSound.m_sActionSoundEventStart = "SOUND_BUILDING_DESTRUCTION_PRESTART";
			defaultSound.m_sActionSoundEventEnd = "SOUND_BUILDING_DESTRUCTION_END";
			config.m_aActionSounds.Insert(defaultSound);
		}

		return config;
	}

	//------------------------------------------------------------------------------------------------
	static string GetPrefabDisplayName(ResourceName prefab)
	{
		if (prefab.IsEmpty())
			return "None";

		string path = prefab.GetPath();
		if (path.IsEmpty())
			path = prefab;

		return FilePath.StripExtension(FilePath.StripPath(path));
	}
}

[BaseContainerProps()]
class ATHENA_IntelSoundSetting
{
	[Attribute()]
	string m_sName;

	[Attribute(params: "acp")]
	ResourceName m_ActionSound;

	[Attribute()]
	string m_sActionSoundEventStart;

	[Attribute()]
	string m_sActionSoundEventEnd;
}

