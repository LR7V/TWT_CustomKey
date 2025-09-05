class TWT_KeySyncService
{
    protected static ref TWT_KeySyncService s_Instance;

    static void InitRPC()
    {
        if (!s_Instance) s_Instance = new TWT_KeySyncService();

        GetRPCManager().AddRPC("TWT_KeySync", "ClientReceiveAllowedTypes", s_Instance, SingeplayerExecutionType.Client);
        GetRPCManager().AddRPC("TWT_KeySync", "ClientNotify",             s_Instance, SingeplayerExecutionType.Client);

    }

    void ClientReceiveAllowedTypes(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Client) return;

        Param2<string, ref TStringArray> data;
        if (!ctx.Read(data)) {
            GetTWT_CustomKeyLogger().LogDebug("[KEYSYNC] RPC read failed");
            return;
        }

        string adminType   = data.param1;
        TStringArray types = data.param2;

        TWT_KeyClientCache.SetFromServer(adminType, types);

        int cnt = 0;
        if (types) {
            cnt = types.Count();
        }

        GetTWT_CustomKeyLogger().LogDebug("[KEYSYNC] client received types count=" + cnt.ToString() + " admin=" + adminType);
    }

}
