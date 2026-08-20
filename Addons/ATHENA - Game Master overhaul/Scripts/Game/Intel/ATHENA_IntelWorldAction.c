class ATHENA_IntelWorldAction : ScriptedUserAction
{
	protected ATHENA_IntelComponent m_IntelComp;

	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_IntelComp = ATHENA_IntelComponent.Cast(pOwnerEntity.FindComponent(ATHENA_IntelComponent));
		
		// Note: SetActionDuration() causes a compile error in this context.
		// Instead, we dynamically scale the duration by overriding GetActionProgressScript.
		// The prefab MUST have a Duration > 0 (e.g., 1.0) for it to be treated as a continuous action.
	}



	protected bool m_bStartSoundPlayed = false;

	override float GetActionProgressScript(float fProgress, float timeSlice)
	{
		if (m_IntelComp)
		{
			// Play start sound on the very first frame of interaction
			if (fProgress == 0 && !m_bStartSoundPlayed)
			{
				m_bStartSoundPlayed = true;
				
				ResourceName soundProject = m_IntelComp.GetActionSound();
				string soundEventStart = m_IntelComp.GetActionSoundEventStart();
				if (!soundProject.IsEmpty() && !soundEventStart.IsEmpty())
				{
					IEntity owner = GetOwner();
					if (owner)
					{
						vector mat[4];
						owner.GetTransform(mat);
						AudioSystem.PlayEvent(soundProject, soundEventStart, mat);
					}
				}
			}

			float desiredDuration = m_IntelComp.GetActionDuration();
			float prefabDuration = GetActionDuration();
			
			// Scale the time slice so it completes in exactly `desiredDuration` seconds
			if (desiredDuration > 0 && prefabDuration > 0)
			{
				return fProgress + ((timeSlice / desiredDuration) * prefabDuration);
			}
		}
		
		return fProgress + timeSlice;
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
		
		// Check if we are the local player who interacted
		PlayerController pc = GetGame().GetPlayerController();
		bool isLocalPlayer = false;
		if (pc && pc.GetControlledEntity() == pUserEntity)
			isLocalPlayer = true;

		if (isLocalPlayer)
		{
			ATHENA_IntelPlayerUI uiInstance = ATHENA_IntelPlayerUI.Open(title, description);

			// Play END sound if configured
			ResourceName soundProject = m_IntelComp.GetActionSound();
			string soundEventEnd = m_IntelComp.GetActionSoundEventEnd();
			if (!soundProject.IsEmpty() && !soundEventEnd.IsEmpty())
			{
				vector mat[4];
				pOwnerEntity.GetTransform(mat);
				AudioSystem.PlayEvent(soundProject, soundEventEnd, mat);
			}

			// Reset start sound for the next interaction
			m_bStartSoundPlayed = false;
		}

		if (Replication.IsServer())
		{
			// Handle Pings (Notifications) via RPC
			EAthenaIntelShareType shareType = m_IntelComp.GetShareType();
			bool pingGM = m_IntelComp.ShouldPingGM();
			
			if (pingGM || shareType != EAthenaIntelShareType.Nobody)
			{
				// Get the correct player ID of the user on the server
				int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(pUserEntity);
				m_IntelComp.RequestPing(pingGM, shareType, title, playerId);
			}

			// Handle Deletion
			if (m_IntelComp.ShouldDeleteOnCompletion())
			{
				m_IntelComp.DeleteIntelEntity();
			}
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
		// Returning false means PerformAction runs on the Client who interacted AND the Server.
		// This is required so the Server can handle deleting the entity.
		return false; 
	}
}
