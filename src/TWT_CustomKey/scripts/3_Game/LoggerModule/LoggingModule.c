
static ref TWT_CustomKeyLoggingModule GetTWT_CustomKeyLogger()
{
    return TWT_CustomKeyLoggingModule.Cast(CF_ModuleCoreManager.Get(TWT_CustomKeyLoggingModule));
}

[CF_RegisterModule(TWT_CustomKeyLoggingModule)]
class TWT_CustomKeyLoggingModule : CF_ModuleGame
{
    private int networkSync_LogLevel;

    ref TWT_CustomKeyLoggingSettings settings;

    FileHandle fileHandle;

    float dtime = 0;


    override void OnInit()
    {
        super.OnInit();
        
        EnableUpdate();
        EnableMissionStart();
    }

    override void OnMissionStart(Class sender, CF_EventArgs args)
    {
        super.OnMissionStart(sender, args);

        fileHandle = CreateNewLogFile();

        if(GetGame().IsServer())
        {
            settings = TWT_CustomKeyLoggingSettings.Load();

            networkSync_LogLevel = settings.logLevel;
            SynchLogLevel();       
        }
        else
        {
            AddLegacyRPC("GetLogLevelResponse", SingleplayerExecutionType.Client);
        }
    }

    void GetLogLevelResponse(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		 if (type != CallType.Client)
			 return;

		 Param1<int> data;
		 if (!ctx.Read(data))
            return;

		 networkSync_LogLevel = data.param1;
	}

    void SynchLogLevel()
    {
        GetRPCManager().SendRPC(ClassName(), "GetLogLevelResponse",  new Param1<int>(networkSync_LogLevel), true, NULL);
    }

    override void OnUpdate(Class sender, CF_EventArgs args)
    {
        auto update = CF_EventUpdateArgs.Cast(args);

        if(!GetGame().IsServer())
            return;

        if(!settings)
            return;

        dtime += update.DeltaTime;
        if(dtime >= settings.refreshRateInSeconds)
        {
            dtime = 0;

            settings = TWT_CustomKeyLoggingSettings.Load();

            networkSync_LogLevel = settings.logLevel;
            SynchLogLevel();
        }
    }


    void MakeDirectoryIfNotExists()
    {
        if(!FileExist(TWT_CustomKey_ROOT_FOLDER))
            MakeDirectory(TWT_CustomKey_ROOT_FOLDER);

        if(!FileExist(TWT_CustomKey_LOG_FOLDER))
            MakeDirectory(TWT_CustomKey_LOG_FOLDER);

        if(!FileExist(TWT_CustomKey_LOGGER_CONFIG_DIR))
            MakeDirectory(TWT_CustomKey_LOGGER_CONFIG_DIR);
        
        if(!FileExist(TWT_CustomKey_LOGGER_LOG_DIR))
            MakeDirectory(TWT_CustomKey_LOGGER_LOG_DIR);
    }
    
    string GenerateShortDateString()
    {
        int year, month, day;
        GetYearMonthDay(year, month, day);
        return "" + year + "_" + month + "_" + day;
    }

    string GenerateShortTimeString()
    {
        int hour, minute, second;
        GetHourMinuteSecond(hour, minute, second);
        return "" + hour + "_" + minute + "_" + second;
    }

    string GenerateFullTimestamp()
    {
        string dateStr = GenerateShortDateString();
        string timeStr = GenerateShortTimeString();

        return dateStr + "-" + timeStr;
    }

    static string GetTimestampForLog()
    {
        int year, month, day, hour, minute, second;
        GetYearMonthDay(year, month, day);
        GetHourMinuteSecond(hour, minute, second);

        string dateStr = day.ToStringLen(2) + "." + month.ToStringLen(2) + "." + year.ToString();
        string timeStr = hour.ToStringLen(2) + ":" + minute.ToStringLen(2);
        return dateStr + " | " + timeStr;
    }

    FileHandle CreateNewLogFile()
    {
        MakeDirectoryIfNotExists();

        string filePath = string.Format(TWT_CustomKey_LOGGER_LOG_FILE, GenerateFullTimestamp());

        fileHandle = OpenFile(filePath, FileMode.WRITE);

        if(fileHandle != 0)
        {
            FPrintln(fileHandle, "Creation Time: " + GenerateFullTimestamp());
            return fileHandle;
        }

        return null;
    }

    void Log(string content, TWT_CustomKeyLogLevel logLevel)
    {
        if(logLevel < networkSync_LogLevel)
            return;
        
        string timestamp = GetTimestampForLog();
        FPrintln(fileHandle, timestamp + " | " + GetLogLevelString(logLevel) + " | " + content);
    }

    void LogInfo(string content)
    {
        Log(content, TWT_CustomKeyLogLevel.Info);
    }

    void LogWarning(string content)
    {
        Log(content, TWT_CustomKeyLogLevel.Warn);
    }

    void LogError(string content)
    {
        Log(content, TWT_CustomKeyLogLevel.Error);
    }

    void LogDebug(string content)
    {
        Log(content, TWT_CustomKeyLogLevel.Debug);
    }

    string GetLogLevelString(TWT_CustomKeyLogLevel logLevel)
    {
        switch(logLevel)
        {
             case TWT_CustomKeyLogLevel.Debug:
                return "DEBUG";
            case TWT_CustomKeyLogLevel.Info:
                return "INFO";
            case TWT_CustomKeyLogLevel.Warn:
                return "WARNING";
            case TWT_CustomKeyLogLevel.Error:
                return "ERROR";
            default:
                return "";
        }

        return "";
    }

}