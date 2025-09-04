class CfgPatches
{
	class TWT_CustomKey_Script
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]={};
	};
};
class CfgMods
{
	class TWT_CustomKey
	{
		dir = "TWT_CustomKey";
		name = "TWT_CustomKey";
		credits = "Mr.Monki, LRTV";
		author = "Mr.Monki, LRTV";
		version = "1.0";
		type = "mod";
		dependencies[]=
		{
			"Game",
			"World",
			"Mission"
		};
		class defs
		{
			class gameScriptModule
			{
				value="";
				files[]=
				{
					"TWT_CustomKey/scripts/Common",
					"TWT_CustomKey/scripts/3_Game"
				};
			};
			class worldScriptModule
			{
				value="";
				files[]=
				{
					"TWT_CustomKey/scripts/Common",
					"TWT_CustomKey/scripts/4_World"
				};
			};
			class missionScriptModule
			{
				value="";
				files[]=
				{
					"TWT_CustomKey/scripts/Common",
					"TWT_CustomKey/scripts/5_Mission"
				};
			};
		};
	};
};
class CfgVehicles
{

};
