class TWT_DoorLockData
{
    ref map<string, ref array<int>> items;
    ref map<string, ref map<int, string>> types;

    void TWT_DoorLockData()
    {
        items = new map<string, ref array<int>>();
        types = new map<string, ref map<int, string>>();
    }
}

class TWT_DoorLockDB
{
    protected const string DIR  = "$profile:TWT_CustomKey";
    protected const string FILE = "$profile:TWT_CustomKey/doorlocks.json";

    protected ref TWT_DoorLockData m_Data;

    void TWT_DoorLockDB()
    {
        m_Data = new TWT_DoorLockData();
        Load();
    }

    string MakeKey(BuildingBase bld)
    {
        vector p = bld.GetPosition();
        int x = Math.Round(p[0]);
        int z = Math.Round(p[2]);
        return bld.GetType() + ":" + x.ToString() + ":" + z.ToString();
    }

    void Load()
    {
        if (!m_Data) m_Data = new TWT_DoorLockData();
        if (FileExist(FILE)) {
            JsonFileLoader<TWT_DoorLockData>.JsonLoadFile(FILE, m_Data);
        }
        if (!m_Data.items) m_Data.items = new map<string, ref array<int>>();
        if (!m_Data.types) m_Data.types = new map<string, ref map<int, string>>();
    }

    void Save()
    {
        MakeDirectory(DIR);
        JsonFileLoader<TWT_DoorLockData>.JsonSaveFile(FILE, m_Data);
    }


    void SetLocked(BuildingBase bld, int doorIndex, bool locked)
    {
        SetLockedWithKey(bld, doorIndex, locked, "");
    }


    void SetLockedWithKey(BuildingBase bld, int doorIndex, bool locked, string keyType)
    {
        if (!bld || doorIndex < 0) return;

        string bkey = MakeKey(bld);


        ref array<int> doors;
        if (!m_Data.items.Find(bkey, doors)) {
            doors = new array<int>();
            m_Data.items.Insert(bkey, doors);
        }

        ref map<int, string> tmap;
        if (!m_Data.types.Find(bkey, tmap)) {
            tmap = new map<int, string>();
            m_Data.types.Insert(bkey, tmap);
        }

        int pos = doors.Find(doorIndex);

        if (locked) {
            if (pos == -1) doors.Insert(doorIndex);

            string kt = keyType;
            if (kt && kt != string.Empty) kt.Trim(); else kt = "";

            string dummy;
            if (tmap.Find(doorIndex, dummy)) tmap.Remove(doorIndex);
            tmap.Insert(doorIndex, kt);
        } else {
            if (pos != -1) doors.Remove(pos);

            string _dump;
            if (tmap.Find(doorIndex, _dump)) tmap.Remove(doorIndex);

            if (doors.Count() == 0) m_Data.items.Remove(bkey);
            if (tmap.Count()   == 0) m_Data.types.Remove(bkey);
        }

        Save();
    }

    string GetLastKeyType(BuildingBase bld, int doorIndex)
    {
        if (!bld || doorIndex < 0) return "";
        string bkey = MakeKey(bld);

        ref map<int, string> tmap;
        if (!m_Data.types.Find(bkey, tmap)) return "";

        string outType;
        if (tmap.Find(doorIndex, outType)) {
            if (outType && outType != string.Empty) {
                outType.Trim();
                return outType;
            }
        }
        return "";
    }

    BuildingBase FindBuildingByKey(string key, float radius = 1.25)
    {
        TStringArray parts = new TStringArray();
        key.Split(":", parts);
        if (parts.Count() != 3) return null;

        string type = parts[0];
        int x = parts[1].ToInt();
        int z = parts[2].ToInt();

        vector pos = Vector(x, GetGame().SurfaceY(x, z), z);
        array<Object> objs = {};
        array<CargoBase> dummy = {};
        GetGame().GetObjectsAtPosition(pos, radius, objs, dummy);

        foreach (Object o : objs) {
            BuildingBase b = BuildingBase.Cast(o);
            if (b && b.GetType() == type) return b;
        }
        return null;
    }

    int ApplyAllOnce()
    {
        int applied = 0;
        foreach (string key, ref array<int> lockedDoors : m_Data.items) {
            BuildingBase b = FindBuildingByKey(key);
            if (!b) continue;

            foreach (int di : lockedDoors) {
                if (!b.IsDoorLocked(di)) {
                    b.LockDoor(di);
                    applied++;
                }
            }
        }
        return applied;
    }

    void ApplyWithRetries(int tries = 10, int intervalMs = 3000)
    {
        Param2<int,int> state = new Param2<int,int>(tries, intervalMs);
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.OnRetryTick, intervalMs, true, state);
    }

    void OnRetryTick(Param2<int,int> state)
    {
        int triesLeft = state.param1;
        int interval = state.param2;

        int applied = ApplyAllOnce();

        if (applied > 0 || triesLeft <= 1) {
            GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(this.OnRetryTick);
            return;
        }
        state.param1 = triesLeft - 1;
    }
}

ref TWT_DoorLockDB g_TWT_DoorLockDB;
TWT_DoorLockDB GetTWT_DoorLockDB()
{
    if (!g_TWT_DoorLockDB) g_TWT_DoorLockDB = new TWT_DoorLockDB();
    return g_TWT_DoorLockDB;
}
