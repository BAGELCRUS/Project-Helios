[ComponentEditorProps(category: "GameScripted/Intel", description: "Component on PlayerController to track collected intel.")]
class ATHENA_PlayerIntelComponentClass : ScriptComponentClass
{
}

class ATHENA_PlayerIntelComponent : ScriptComponent
{
	protected ref array<ref ATHENA_IntelData> m_aCollectedIntel = new array<ref ATHENA_IntelData>();

	// Add Intel locally
	void AddIntelLocal(string title, string type, string description, string method = "Collected", EAthenaIntelShareType shareType = EAthenaIntelShareType.Individual)
	{
		// Avoid duplicates by checking title
		foreach (ATHENA_IntelData intel : m_aCollectedIntel)
		{
			if (intel.m_sTitle == title)
				return;
		}
		
		string timeStr = GetCurrentGameTimeFormatted();
		
		ATHENA_IntelData newIntel = new ATHENA_IntelData(title, type, description, shareType, method, timeStr);
		m_aCollectedIntel.Insert(newIntel);
	}
	
	void MarkIntelAsSharedLocal(string title)
	{
		foreach (ATHENA_IntelData intel : m_aCollectedIntel)
		{
			if (intel.m_sTitle == title)
			{
				intel.m_ShareType = EAthenaIntelShareType.Everyone; // Triggers UI SHARED icon
				return;
			}
		}
	}

	void ShareIntelNearby(string title, string type, string description)
	{
		Print("ATHENA_PlayerIntelComponent: Requesting server to share Intel nearby. Title: " + title, LogLevel.NORMAL);
		Rpc(RpcAsk_ShareIntelNearby, title, type, description);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_ShareIntelNearby(string title, string type, string description)
	{
		IEntity instigatorEntity = PlayerController.Cast(GetOwner()).GetControlledEntity();
		if (!instigatorEntity) 
		{
			Print("ATHENA_PlayerIntelComponent Server: Instigator entity is null!", LogLevel.WARNING);
			return;
		}
		
		vector instigatorPos = instigatorEntity.GetOrigin();
		Print("ATHENA_PlayerIntelComponent Server: Processing ShareIntelNearby request for Title: " + title + " from position: " + instigatorPos.ToString(), LogLevel.NORMAL);

		PlayerManager playerManager = GetGame().GetPlayerManager();
		if (!playerManager) return;
		
		array<int> players = new array<int>();
		playerManager.GetPlayers(players);
		int sharedCount = 0;

		foreach (int playerId : players)
		{
			PlayerController pc = playerManager.GetPlayerController(playerId);
			if (!pc || pc == GetOwner()) continue; // Skip self

			IEntity targetEntity = pc.GetControlledEntity();
			if (!targetEntity) continue;

			float dist = vector.Distance(instigatorPos, targetEntity.GetOrigin());
			if (dist <= 10.0) // 10m radius
			{
				Print("ATHENA_PlayerIntelComponent Server: Player " + playerId + " is in range (" + dist + "m). Sending intel.", LogLevel.NORMAL);
				ATHENA_PlayerIntelComponent targetPic = ATHENA_PlayerIntelComponent.Cast(pc.FindComponent(ATHENA_PlayerIntelComponent));
				if (targetPic)
				{
					targetPic.Rpc(targetPic.RpcDo_ReceiveSharedIntel, title, type, description);
					sharedCount++;
				}
			}
			else
			{
				Print("ATHENA_PlayerIntelComponent Server: Player " + playerId + " is out of range (" + dist + "m).", LogLevel.VERBOSE);
			}
		}
		
		Print("ATHENA_PlayerIntelComponent Server: Shared intel with " + sharedCount + " nearby players.", LogLevel.NORMAL);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_ReceiveSharedIntel(string title, string type, string description)
	{
		Print("ATHENA_PlayerIntelComponent Client: Received shared Intel. Title: " + title, LogLevel.NORMAL);
		AddIntelLocal(title, type, description, "Received", EAthenaIntelShareType.Individual);
		SCR_HintManagerComponent.GetInstance().ShowCustomHint(title, "Intel Shared by Nearby Player", 10.0, false);
	}

	string GetCurrentGameTimeFormatted()
	{
		ChimeraWorld world = ChimeraWorld.CastFrom(GetGame().GetWorld());
		if (!world)
			return "";
			
		TimeAndWeatherManagerEntity timeManager = world.GetTimeAndWeatherManager();
		if (!timeManager)
			return "";

		int year = timeManager.GetYear();
		int month = timeManager.GetMonth();
		int day = timeManager.GetDay();

		int hours, minutes, seconds;
		timeManager.GetHoursMinutesSeconds(hours, minutes, seconds);

		string monthStr = "JAN";
		switch (month)
		{
			case 1: monthStr = "JAN"; break;
			case 2: monthStr = "FEB"; break;
			case 3: monthStr = "MAR"; break;
			case 4: monthStr = "APR"; break;
			case 5: monthStr = "MAY"; break;
			case 6: monthStr = "JUN"; break;
			case 7: monthStr = "JUL"; break;
			case 8: monthStr = "AUG"; break;
			case 9: monthStr = "SEP"; break;
			case 10: monthStr = "OCT"; break;
			case 11: monthStr = "NOV"; break;
			case 12: monthStr = "DEC"; break;
		}

		string dayStr = day.ToString();
		if (day < 10) dayStr = "0" + dayStr;

		string yearStr = year.ToString();
		if (yearStr.Length() >= 2)
			yearStr = yearStr.Substring(yearStr.Length() - 2, 2);

		string hourStr = hours.ToString();
		if (hours < 10) hourStr = "0" + hourStr;

		string minStr = minutes.ToString();
		if (minutes < 10) minStr = "0" + minStr;

		return string.Format("%1%2%3 %4%5 Local", dayStr, monthStr, yearStr, hourStr, minStr);
	}
	
	// Get all intel for UI
	array<ref ATHENA_IntelData> GetCollectedIntel()
	{
		return m_aCollectedIntel;
	}

	// -------------------------------------------------------------------------
	// PERSISTENCE HOOKS (Currently unused, ready for future expansion)
	// -------------------------------------------------------------------------
	
	bool OnSave(ScriptBitWriter writer)
	{
		// To be implemented later for reconnects
		return true;
	}
	
	bool OnLoad(ScriptBitReader reader)
	{
		// To be implemented later for reconnects
		return true;
	}

	// -------------------------------------------------------------------------
	// MAP TOOLBAR INTEGRATION
	// -------------------------------------------------------------------------

	[Attribute("{092FA51684BDBF6D}UI/Textures/Map/Map_Tools/Tool_Marker_UI.imageset", UIWidgets.ResourcePickerThumbnail, desc: "Map toolbar icon imageset", params: "imageset")]
	protected ResourceName m_sIconImageset;

	[Attribute("Marker", UIWidgets.EditBox, desc: "Name of the quad within the imageset")]
	protected string m_sIconName;

	protected SCR_MapToolEntry m_IntelToolEntry;

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		SetEventMask(owner, EntityEvent.INIT);
	}

	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		// Wait a bit to ensure map system is ready and player is initialized
		GetGame().GetCallqueue().CallLater(RegisterMapHook, 1000, false);
	}

	protected void RegisterMapHook()
	{
		// Only register on the local player's controller
		PlayerController pc = PlayerController.Cast(GetOwner());
		if (pc && pc.GetPlayerId() == SCR_PlayerController.GetLocalPlayerId())
		{
			if (SCR_MapEntity.GetOnMapOpen())
				SCR_MapEntity.GetOnMapOpen().Insert(OnMapOpen);
		}
	}

	protected void OnMapOpen(MapConfiguration config)
	{
		SCR_MapEntity mapEnt = SCR_MapEntity.GetMapInstance();
		if (!mapEnt) return;

		SCR_MapToolMenuUI toolMenu = SCR_MapToolMenuUI.Cast(mapEnt.GetMapUIComponent(SCR_MapToolMenuUI));
		if (toolMenu)
		{
			// Prevent duplicates when map is reopened
			array<ref SCR_MapToolEntry> entries = toolMenu.GetMenuEntries();
			if (entries)
			{
				foreach (SCR_MapToolEntry entry : entries)
				{
					if (entry == m_IntelToolEntry)
						return; // Already registered
				}
			}

			// 100 is sort priority
			m_IntelToolEntry = toolMenu.RegisterToolMenuEntry(m_sIconImageset, m_sIconName, 100);
			if (m_IntelToolEntry)
			{
				m_IntelToolEntry.m_OnClick.Insert(OnIntelButtonClicked);
			}
		}
	}

	protected void OnIntelButtonClicked()
	{
		ATHENA_IntelListDialogUI.OpenIntelList();
	}

	// Static helper to get the local player's intel component
	static ATHENA_PlayerIntelComponent GetLocalInstance()
	{
		PlayerController pc = GetGame().GetPlayerController();
		if (!pc) return null;
		
		return ATHENA_PlayerIntelComponent.Cast(pc.FindComponent(ATHENA_PlayerIntelComponent));
	}
}
