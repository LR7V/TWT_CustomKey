modded class MissionServer extends MissionBase
{
	void ~MissionServer() {
	}

	void MissionServer()
	{
		Print("TWT_CustomKey wurde geladen!");
		GetTWT_CustomKeyLogger().LogDebug("TWT_CustomKey wurde geladen!");
	}

	override void OnInit()
    {
        super.OnInit();
        TWT_KeySyncService.InitRPC();
    }
	

	override void OnMissionStart()
    {
        super.OnMissionStart();

        TWT_KeyConfig.GetAllTypes();

        auto db = GetTWT_DoorLockDB(); 
        db.Save();                      
        db.ApplyWithRetries(10, 3000);  
    }


	override void OnClientReadyEvent(PlayerIdentity identity, PlayerBase player)
    {
        super.OnClientReadyEvent(identity, player);
        if (identity)
        {
            GetTWT_CustomKeyLogger().LogDebug("[KEYSYNC] send types to " + identity.GetId());
            TWT_KeySyncService_Server.SendAllowedTypesToClient(identity);
        }
    }

};
