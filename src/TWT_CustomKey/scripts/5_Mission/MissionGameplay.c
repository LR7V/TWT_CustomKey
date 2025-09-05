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
        TWT_KeySyncService.InitRPC();
    }

};
