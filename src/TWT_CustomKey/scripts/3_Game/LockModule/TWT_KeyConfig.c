class TWT_KeyEntry
{
    string type;
    ref array<string> steamIds;

    void TWT_KeyEntry()
    {
        steamIds = new array<string>();
    }
}

class TWT_KeyConfigData
{
    ref array<string> items;
    string adminKeyType;
    ref array<ref TWT_KeyEntry> keys;

    ref array<string> admins;

    void TWT_KeyConfigData()
    {
        items = new array<string>();
        adminKeyType = "";
        keys = new array<ref TWT_KeyEntry>();
        admins = new array<string>(); 
    }
}

class TWT_KeyConfig
{
    protected static const string DIR       = "$profile:TWT_CustomKey";
    protected static const string KEYS_FILE = "$profile:TWT_CustomKey/config.json";

    protected static ref TWT_KeyConfigData s_Data;
    protected static bool s_Loaded = false;

    protected static ref TStringArray s_Default = {"Apple"};

    protected static void EnsureLoaded()
    {
        if (s_Loaded) return;

        if (GetGame() && GetGame().IsMultiplayer() && !GetGame().IsServer())
        {
            s_Data = new TWT_KeyConfigData();
            s_Loaded = true;
            GetTWT_CustomKeyLogger().LogDebug("[KEYCONFIG] Client context detected -> skip loading local $profile");
            return;
        }

        if (!FileExist(DIR)) MakeDirectory(DIR);
        s_Data = new TWT_KeyConfigData();

        if (FileExist(KEYS_FILE))
        {
            JsonFileLoader<TWT_KeyConfigData>.JsonLoadFile(KEYS_FILE, s_Data);
            GetTWT_CustomKeyLogger().LogDebug("[KEYCONFIG] Loaded (JsonFileLoader): " + KEYS_FILE);

            if (!s_Data.items)  s_Data.items  = new array<string>();
            if (!s_Data.keys)   s_Data.keys   = new array<ref TWT_KeyEntry>();
            if (!s_Data.admins) s_Data.admins = new array<string>(); // NEU

            foreach (TWT_KeyEntry e : s_Data.keys)
            {
                if (e && !e.steamIds) e.steamIds = new array<string>();
            }
        }
        else
        {
            s_Data.items.Insert("Apple");
            s_Data.adminKeyType = "TWT_AdminKey";
            s_Data.admins = new array<string>();

            foreach (string t2 : s_Data.items)
            {
                TWT_KeyEntry e2 = new TWT_KeyEntry();
                e2.type = t2;
                s_Data.keys.Insert(e2);
            }
            JsonFileLoader<TWT_KeyConfigData>.JsonSaveFile(KEYS_FILE, s_Data);
            GetTWT_CustomKeyLogger().LogDebug("[KEYCONFIG] Wrote default (JsonFileLoader): " + KEYS_FILE);
        }

        s_Loaded = true;
    }

    static string GetAdminKeyType()
    {
        EnsureLoaded();
        return s_Data.adminKeyType;
    }

    static bool IsAdminKey(string typeName)
    {
        EnsureLoaded();
        if (!typeName || typeName == string.Empty) return false;
        if (!s_Data.adminKeyType || s_Data.adminKeyType == string.Empty) return false;

        string t = typeName; t.Trim(); t.ToLower();
        string a = s_Data.adminKeyType; a.Trim(); a.ToLower();
        return (t == a);
    }

    static bool IsAdminSteamId(string steamId)
    {
        EnsureLoaded();
        if (!steamId || steamId == string.Empty) return false;
        if (!s_Data.admins) return false;

        string sid = steamId;
        sid.Trim();

        for (int i = 0; i < s_Data.admins.Count(); i++)
        {
            string a = s_Data.admins[i];
            if (a && a != string.Empty)
            {
                string t = a;
                t.Trim();
                if (t == sid) return true;
            }
        }
        return false;
    }

    static bool CanUseKey(string typeName, string steamId)
    {
        EnsureLoaded();

        GetTWT_CustomKeyLogger().LogDebug("[KEYCONFIG] CanUseKey");

        if (typeName && typeName != string.Empty) typeName.Trim(); else typeName = "";
        if (steamId  && steamId  != string.Empty) steamId.Trim();  else steamId  = "";

        if (typeName == "")
        {
            GetTWT_CustomKeyLogger().LogDebug("[KEYCONFIG] CanUseKey: typeName leer -> false");
            return false;
        }

        string typeNorm = typeName; typeNorm.ToLower();

        string adminNorm = "";
        if (s_Data.adminKeyType && s_Data.adminKeyType != string.Empty)
        {
            adminNorm = s_Data.adminKeyType;
            adminNorm.Trim();
            adminNorm.ToLower();
        }

        if (adminNorm != "" && typeNorm == adminNorm)
        {
            if (IsAdminSteamId(steamId))
            {
                GetTWT_CustomKeyLogger().LogDebug("[KEYCONFIG] CanUseKey: AdminKey von Admin " + steamId + " -> true");
                return true;
            }
            GetTWT_CustomKeyLogger().LogDebug("[KEYCONFIG] CanUseKey: AdminKey aber kein Admin (" + steamId + ") -> false");
            return false;
        }

        if (IsAdminSteamId(steamId))
        {
            GetTWT_CustomKeyLogger().LogDebug("[KEYCONFIG] CanUseKey: " + steamId + " ist Admin -> Bypass für Typ " + typeName + " -> true");
            return true;
        }

        if (s_Data && s_Data.keys)
        {
            foreach (TWT_KeyEntry e : s_Data.keys)
            {
                if (!e) continue;

                string eTypeNorm = "";
                if (e.type && e.type != string.Empty)
                {
                    eTypeNorm = e.type;
                    eTypeNorm.Trim();
                    eTypeNorm.ToLower();
                }

                if (eTypeNorm == typeNorm)
                {
                    int listCount = 0;
                    if (e.steamIds) listCount = e.steamIds.Count();
                    GetTWT_CustomKeyLogger().LogDebug("[KEYCONFIG] Entry type=" + typeName + " ids.count=" + listCount.ToString());

                    if (steamId == "")
                    {
                        GetTWT_CustomKeyLogger().LogDebug("[KEYCONFIG] CanUseKey: Spieler hat keine SteamID -> false");
                        return false;
                    }

                    if (!e.steamIds || e.steamIds.Count() == 0)
                    {
                        GetTWT_CustomKeyLogger().LogDebug("[KEYCONFIG] CanUseKey: Whitelist leer -> false");
                        return false;
                    }

                    string steamNorm = steamId; steamNorm.ToLower();
                    foreach (string raw : e.steamIds)
                    {
                        string s = "";
                        if (raw && raw != string.Empty)
                        {
                            s = raw;
                            s.Trim();
                            s.ToLower();
                        }
                        if (s == steamNorm)
                        {
                            GetTWT_CustomKeyLogger().LogDebug("[KEYCONFIG] CanUseKey: match " + steamId + " in " + typeName + " -> true");
                            return true;
                        }
                    }

                    return false;
                }
            }
        }

        if (s_Data.items && s_Data.items.Count() > 0)
        {
            foreach (string legacy : s_Data.items)
            {
                string legacyNorm = "";
                if (legacy && legacy != string.Empty)
                {
                    legacyNorm = legacy;
                    legacyNorm.Trim();
                    legacyNorm.ToLower();
                }

                if (legacyNorm == typeNorm)
                {
                    GetTWT_CustomKeyLogger().LogDebug("[KEYCONFIG] Legacy fallback: " + typeName + " in items[] -> true (jeder darf)");
                    return true;
                }
            }
        }

        GetTWT_CustomKeyLogger().LogDebug("[KEYCONFIG] Kein Key-Entry für " + typeName + " -> false");
        return false;
    }

    static bool IsAllowedType(string typeName)
    {
        EnsureLoaded();

        Print("[KEYCONFIG] IsAllowedType check for " + typeName);
        if (!typeName || typeName == string.Empty) return false;

        string tn = typeName;
        tn.Trim();
        tn.ToLower();
        if (tn == string.Empty) return false;

        if (s_Data && s_Data.adminKeyType && s_Data.adminKeyType != string.Empty) {
            string an = s_Data.adminKeyType;
            an.Trim();
            an.ToLower();
            if (tn == an) return true;
        }

        if (s_Data && s_Data.keys) {
            foreach (TWT_KeyEntry e : s_Data.keys) {
                if (!e) continue;
                if (e.type && e.type != string.Empty) {
                    string en = e.type;
                    en.Trim();
                    en.ToLower();
                    if (tn == en) return true;
                }
            }
        }

        if (s_Data && s_Data.items) {
            foreach (string t : s_Data.items) {
                if (t && t != string.Empty) {
                    string ln = t;
                    ln.Trim();
                    ln.ToLower();
                    if (tn == ln) return true;
                }
            }
        }

        return false;
    }


    static TStringArray GetAllTypes()
    {
        EnsureLoaded();
        TStringArray outArr = new TStringArray();

        foreach (ref TWT_KeyEntry e : s_Data.keys)
        {
            if (e && e.type != string.Empty)
                outArr.Insert(e.type);
        }

        foreach (string t : s_Data.items)
        {
            bool exists = false;
            for (int i = 0; i < outArr.Count(); i++)
            {
                if (outArr[i] == t) { exists = true; break; }
            }
            if (!exists) outArr.Insert(t);
        }
        return outArr;
    }
}
