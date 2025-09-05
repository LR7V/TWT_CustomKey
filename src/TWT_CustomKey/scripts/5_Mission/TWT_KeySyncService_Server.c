class TWT_KeySyncService_Server
{
    static void SendAllowedTypesToClient(PlayerIdentity id)
    {
        if (!GetGame() || !GetGame().IsServer() || !id) return;

        TStringArray types = TWT_KeyConfig.GetAllTypes(); 
        string adminType   = TWT_KeyConfig.GetAdminKeyType();

        GetTWT_CustomKeyLogger().LogDebug("[KEYSYNC] sending types count=" + types.Count().ToString() + " admin=" + adminType + " to " + id.GetId());

        GetRPCManager().SendRPC("TWT_KeySync","ClientReceiveAllowedTypes",new Param2<string, ref TStringArray>(adminType, types),true,id);
    }

    static void BroadcastAllowedTypes()
    {
        if (!GetGame() || !GetGame().IsServer()) return;

        array<Man> players = new array<Man>();
        GetGame().GetPlayers(players);

        for (int i = 0; i < players.Count(); i++)
        {
            PlayerBase pb = PlayerBase.Cast(players[i]);
            if (pb && pb.GetIdentity())
            {
                SendAllowedTypesToClient(pb.GetIdentity());
            }
        }
    }

}
