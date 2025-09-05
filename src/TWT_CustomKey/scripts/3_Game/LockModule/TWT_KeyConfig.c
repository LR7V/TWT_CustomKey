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

    void TWT_KeyConfigData()
    {
        items = new array<string>();
        adminKeyType = "";
        keys = new array<ref TWT_KeyEntry>();
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

            if (!s_Data.items) s_Data.items = new array<string>();
            if (!s_Data.keys)  s_Data.keys  = new array<ref TWT_KeyEntry>();
            foreach (TWT_KeyEntry e : s_Data.keys)
            {
                if (e && !e.steamIds) e.steamIds = new array<string>();
            }
        }
        else
        {
            s_Data.items.Insert("");
            s_Data.adminKeyType = "TWT_AdminKey";

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
        return (typeName == s_Data.adminKeyType && s_Data.adminKeyType != string.Empty);
    }

    static bool CanUseKey(string typeName, string steamId)
    {
        EnsureLoaded();


        if (GetGame().IsServer()) DebugDumpRaw();

        GetTWT_CustomKeyLogger().LogDebug("[KEYCONFIG] CanUseKey");


        if (typeName && typeName != string.Empty) {
            typeName.Trim();
        } else {
            typeName = "";
        }

        if (steamId && steamId != string.Empty) {
            steamId.Trim();
        } else {
            steamId = "";
        }

        if (typeName == "") {
            GetTWT_CustomKeyLogger().LogDebug("[KEYCONFIG] CanUseKey: typeName leer -> false");
            return false;
        }

        string typeNorm = typeName;
        typeNorm.ToLower();

        string adminNorm = "";
        if (s_Data.adminKeyType && s_Data.adminKeyType != string.Empty) {
            adminNorm = s_Data.adminKeyType;
            adminNorm.Trim();
            adminNorm.ToLower();
        }

        if (adminNorm != "" && typeNorm == adminNorm) {
            GetTWT_CustomKeyLogger().LogDebug("[KEYCONFIG] CanUseKey: " + typeName + " ist AdminKey -> true");
            return true;
        }


        if (s_Data && s_Data.keys) {
            foreach (TWT_KeyEntry e : s_Data.keys) {
                if (!e) continue;

                string eTypeNorm = "";
                if (e.type && e.type != string.Empty) {
                    eTypeNorm = e.type;
                    eTypeNorm.Trim();
                    eTypeNorm.ToLower();
                }

                if (eTypeNorm == typeNorm) {
                    int listCount = 0;
                    if (e.steamIds) listCount = e.steamIds.Count();
                    GetTWT_CustomKeyLogger().LogDebug("[KEYCONFIG] Entry type=" + typeName + " ids.count=" + listCount.ToString());

                    if (steamId == "") {
                        GetTWT_CustomKeyLogger().LogDebug("[KEYCONFIG] CanUseKey: Spieler hat keine SteamID -> false");
                        return false;
                    }

                    if (!e.steamIds || e.steamIds.Count() == 0) {
                        GetTWT_CustomKeyLogger().LogDebug("[KEYCONFIG] CanUseKey: Whitelist leer -> false");
                        return false;
                    }

                    string steamNorm = steamId;
                    steamNorm.ToLower();

                    foreach (string raw : e.steamIds) {
                        string s = "";
                        if (raw && raw != string.Empty) {
                            s = raw;
                            s.Trim();
                            s.ToLower();
                        }

                        if (s == steamNorm) {
                            GetTWT_CustomKeyLogger().LogDebug("[KEYCONFIG] CanUseKey: match " + steamId + " in " + typeName + " -> true");
                            return true;
                        }
                    }


                    string sample = "";
                    int shown = 0;
                    foreach (string raw2 : e.steamIds) {
                        string t = "null";
                        if (raw2 && raw2 != string.Empty) {
                            t = raw2;
                            t.Trim();
                        }
                        if (shown == 0) sample = t;
                        else sample = sample + ", " + t;
                        shown++;
                        if (shown >= 5) {
                            sample = sample + " ...";
                            break;
                        }
                    }

                    GetTWT_CustomKeyLogger().LogDebug("[KEYCONFIG] CanUseKey: " + steamId + " nicht in Whitelist [" + sample + "] -> false");
                    return false;
                }
            }
        }

        if (s_Data.items && s_Data.items.Count() > 0) {
            foreach (string legacy : s_Data.items) {
                string legacyNorm = "";
                if (legacy && legacy != string.Empty) {
                    legacyNorm = legacy;
                    legacyNorm.Trim();
                    legacyNorm.ToLower();
                }

                if (legacyNorm == typeNorm) {
                    GetTWT_CustomKeyLogger().LogDebug("[KEYCONFIG] Legacy fallback: " + typeName + " in items[] -> true (jeder darf)");
                    return true;
                }
            }
        }

        GetTWT_CustomKeyLogger().LogDebug("[KEYCONFIG] Kein Key-Entry für " + typeName + " -> false");
        return false;
    }




    static void DebugDumpRaw()
    {
        if (!(GetGame() && GetGame().IsServer())) return;
        FileHandle fh = OpenFile(KEYS_FILE, FileMode.READ);
        if (fh != 0) {
            string line; string all = "";
            while (FGets(fh, line) > 0) all = all + line;
            CloseFile(fh);
            GetTWT_CustomKeyLogger().LogDebug("[KEYCONFIG] RAW=" + all);
        } else {
            GetTWT_CustomKeyLogger().LogDebug("[KEYCONFIG] RAW failed: " + KEYS_FILE);
        }
    }



    static bool IsAllowedType(string typeName)
    {
        EnsureLoaded();
        if (!typeName || typeName == string.Empty) return false;


        if (IsAdminKey(typeName)) return true;

        foreach (ref TWT_KeyEntry e : s_Data.keys)
        {
            if (e && e.type == typeName) return true;
        }

        foreach (string t : s_Data.items)
        {
            if (t == typeName) return true;
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
