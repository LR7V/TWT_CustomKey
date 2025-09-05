class TWT_CustomKeyLoggingSettings 
{
    int logLevel = 1;
    int refreshRateInSeconds = 60;

    void MakeDirectoryIfNotExists()
    {
        if(!FileExist( TWT_CustomKey_ROOT_FOLDER))
            MakeDirectory( TWT_CustomKey_ROOT_FOLDER);

        if(!FileExist( TWT_CustomKey_LOG_FOLDER))
            MakeDirectory( TWT_CustomKey_LOG_FOLDER);

        if(!FileExist( TWT_CustomKey_LOGGER_CONFIG_DIR))
            MakeDirectory( TWT_CustomKey_LOGGER_CONFIG_DIR);
        
        if(!FileExist( TWT_CustomKey_LOGGER_LOG_DIR))
            MakeDirectory( TWT_CustomKey_LOGGER_LOG_DIR);
    }

    void Save()
    {
        JsonFileLoader<TWT_CustomKeyLoggingSettings>.JsonSaveFile( TWT_CustomKey_LOGGER_CONFIG_FILE, this);
    }

    static ref TWT_CustomKeyLoggingSettings Load()
    {
        TWT_CustomKeyLoggingSettings settings = new TWT_CustomKeyLoggingSettings();

        settings.MakeDirectoryIfNotExists();

        if(FileExist( TWT_CustomKey_LOGGER_CONFIG_FILE))
        {
            JsonFileLoader<TWT_CustomKeyLoggingSettings>.JsonLoadFile( TWT_CustomKey_LOGGER_CONFIG_FILE, settings);
            return settings;
        }

        settings.Save();
        return settings;
    }

}