modded class MissionGameplay extends MissionBase
{
	void MissionGameplay()
	{
		Print("TWT_CustomKey wurde geladen!");
		GetTWT_CustomKeyLogger().LogDebug("TWT_CustomKey wurde geladen!");
	}

	override void OnInit()
    {
        super.OnInit();
        // Registriert den Client-RPC-Handler
        TWT_KeySyncService.InitRPC();
    }

};
