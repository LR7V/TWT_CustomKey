class TWT_KeyClientCache
{
    protected static string s_AdminKeyTypeClient = "";
    protected static ref TStringArray s_AllowedTypesClient;

    static void InitIfNeeded()
    {
        if (!s_AllowedTypesClient) s_AllowedTypesClient = new TStringArray();
    }

    static void SetFromServer(string adminType, TStringArray types)
    {
        InitIfNeeded();

        // admin
        s_AdminKeyTypeClient = "";
        if (adminType && adminType != string.Empty)
        {
            s_AdminKeyTypeClient = adminType;
            s_AdminKeyTypeClient.Trim();
        }

        // types
        s_AllowedTypesClient.Clear();
        if (types)
        {
            for (int i = 0; i < types.Count(); i++)
            {
                string t = types[i];
                if (t && t != string.Empty)
                {
                    t.Trim();
                    s_AllowedTypesClient.Insert(t);
                }
            }
        }
        Print("[KEYSYNC] client received types count=" + s_AllowedTypesClient.Count().ToString() + " admin=" + s_AdminKeyTypeClient);
    }

    static bool IsAdminKeyClient(string typeName)
    {
        if (!typeName || typeName == string.Empty) return false;
        if (s_AdminKeyTypeClient == "") return false;

        string lhs = typeName; lhs.Trim(); lhs.ToLower();
        string rhs = s_AdminKeyTypeClient; rhs.Trim(); rhs.ToLower();
        return (lhs == rhs);
    }

    static bool IsAllowedTypeClient(string typeName)
    {
        InitIfNeeded();
        if (!typeName || typeName == string.Empty) return false;

        string needle = typeName; needle.Trim(); needle.ToLower();

        for (int i = 0; i < s_AllowedTypesClient.Count(); i++)
        {
            string cur = s_AllowedTypesClient[i];
            if (cur && cur != string.Empty)
            {
                string norm = cur; norm.Trim(); norm.ToLower();
                if (norm == needle) return true;
            }
        }
        return false;
    }

    // Optional: Fallback, falls RPC noch nicht angekommen ist
    static bool HasData()
    {
        InitIfNeeded();
        return (s_AdminKeyTypeClient != "" || s_AllowedTypesClient.Count() > 0);
    }
}
