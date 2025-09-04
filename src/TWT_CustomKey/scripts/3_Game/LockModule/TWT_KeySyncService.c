// CF-basiertes Server->Client Sync der erlaubten Key-Typen (ohne SteamIDs)

class TWT_KeySyncService
{
    protected static ref TWT_KeySyncService s_Instance;

    static void InitRPC()
    {
        if (!s_Instance) s_Instance = new TWT_KeySyncService();

        // Richtig: SingleplayerExecutionType (mit 'l')
        GetRPCManager().AddRPC("TWT_KeySync", "ClientReceiveAllowedTypes", s_Instance, SingleplayerExecutionType.Client);
    }

    void ClientReceiveAllowedTypes(CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target)
    {
        if (type != CallType.Client) return;

        Param2<string, ref TStringArray> data;
        if (!ctx.Read(data)) { Print("[KEYSYNC] RPC read failed"); return; }

        string adminType = data.param1;
        TStringArray types = data.param2;

        TWT_KeyClientCache.SetFromServer(adminType, types);
    }

}
