[BaseContainerProps(), SCR_BaseContainerCustomTitleUIInfo("m_UIInfo")]
class ATH_FactionOverrideAttribute : SCR_BaseValueListEditorAttribute
{
	override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
	{
		SCR_EditableEntityComponent editableEntity = SCR_EditableEntityComponent.Cast(item);
		if (!editableEntity) return null;
		
		IEntity entity = editableEntity.GetOwner();
		if (!entity) return null;
		
		FactionKey currentFactionKey = "";
		
		// Handle players
		int playerID = GetPlayerID(entity);
		if (playerID > 0)
		{
			PlayerController pc = GetGame().GetPlayerManager().GetPlayerController(playerID);
			if (pc)
			{
				SCR_PlayerFactionAffiliationComponent playerFaction = SCR_PlayerFactionAffiliationComponent.Cast(pc.FindComponent(SCR_PlayerFactionAffiliationComponent));
				if (playerFaction && playerFaction.GetAffiliatedFaction())
				{
					currentFactionKey = playerFaction.GetAffiliatedFaction().GetFactionKey();
				}
			}
		}
		else
		{
			// Handle standard entities / AI
			FactionAffiliationComponent factionComp = FactionAffiliationComponent.Cast(entity.FindComponent(FactionAffiliationComponent));
			if (factionComp && factionComp.GetAffiliatedFaction())
			{
				currentFactionKey = factionComp.GetAffiliatedFaction().GetFactionKey();
			}
		}
		
		FactionManager factionManager = GetGame().GetFactionManager();
		if (!factionManager) return null;
		
		int selectedIndex = 0;
		array<Faction> outFactions = {};
		factionManager.GetFactionsList(outFactions);
		
		for (int i = 0; i < outFactions.Count(); i++)
		{
			if (outFactions[i].GetFactionKey() == currentFactionKey)
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
		
		FactionManager factionManager = GetGame().GetFactionManager();
		if (!factionManager) return;
		
		array<Faction> outFactions = {};
		factionManager.GetFactionsList(outFactions);
		
		int selectedIndex = var.GetInt();
		if (selectedIndex < 0 || selectedIndex >= outFactions.Count()) return;
		
		Faction selectedFaction = outFactions[selectedIndex];
		
		// Handle players
		int targetPlayerID = GetPlayerID(entity);
		if (targetPlayerID > 0)
		{
			PlayerController pc = GetGame().GetPlayerManager().GetPlayerController(targetPlayerID);
			if (pc)
			{
				SCR_PlayerFactionAffiliationComponent playerFaction = SCR_PlayerFactionAffiliationComponent.Cast(pc.FindComponent(SCR_PlayerFactionAffiliationComponent));
				if (playerFaction)
				{
					// For forcing player factions on the server
					playerFaction.SetAffiliatedFaction(selectedFaction);
				}
			}
		}
		else
		{
			// Handle standard entities / AI
			FactionAffiliationComponent factionComp = FactionAffiliationComponent.Cast(entity.FindComponent(FactionAffiliationComponent));
			if (factionComp)
			{
				factionComp.SetAffiliatedFactionByKey(selectedFaction.GetFactionKey());
			}
		}
	}
	
	override int GetEntries(notnull array<ref SCR_BaseEditorAttributeEntry> outEntries)
	{
		FactionManager factionManager = GetGame().GetFactionManager();
		if (!factionManager) return 0;
		
		array<Faction> outFactions = {};
		factionManager.GetFactionsList(outFactions);
		
		for (int i = 0; i < outFactions.Count(); i++)
		{
			Faction faction = outFactions[i];
			
			UIInfo uiInfo = faction.GetUIInfo();
			if (uiInfo)
			{
				SCR_BaseEditorAttributeEntryText entry = new SCR_BaseEditorAttributeEntryText(uiInfo.GetName());
				outEntries.Insert(entry);
			}
			else
			{
				SCR_BaseEditorAttributeEntryText entry = new SCR_BaseEditorAttributeEntryText(faction.GetFactionKey());
				outEntries.Insert(entry);
			}
		}
		
		return outEntries.Count();
	}
	
	protected int GetPlayerID(IEntity entity)
	{
		int playerID = 0;
		if (EntityUtils.IsPlayer(entity))
		{
			playerID = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(entity);
		}
		return playerID;
	}
}
