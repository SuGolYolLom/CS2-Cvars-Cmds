#include "icvar.h"
#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <iostream>
#include <iomanip>

static_assert(sizeof(ConCommandHandle) == 8, "unexpected size for ConCommandHandle");
static_assert(sizeof(ConVarHandle) == 8, "unexpected size for ConVarHandle");

static ConCommandRefAbstract cvar_unhide_ref;
static void cvar_unhide_callback(const CCommand& args);
ConCommand cvar_unhide(&cvar_unhide_ref, "zcvar_unhide", cvar_unhide_callback, "Removes flags from commands");
//ConCommand cvarlist_md(&cvarlist_md_ref, "cvarlist_md", cvarlist_md_callback, "List all convars/concmds in Markdown format. Format: [hidden]");
static void cvar_unhide_callback(const CCommand& args)
{
	uint64 flagsToRemove = (FCVAR_HIDDEN | FCVAR_DEVELOPMENTONLY | FCVAR_MISSING3);
	//uint64 flagsToRemove = (FCVAR_NONE | FCVAR_LINKED_CONCOMMAND | FCVAR_DEVELOPMENTONLY | FCVAR_GAMEDLL | FCVAR_CLIENTDLL | FCVAR_HIDDEN | FCVAR_PROTECTED | FCVAR_SPONLY | FCVAR_ARCHIVE | FCVAR_NOTIFY | FCVAR_USERINFO | FCVAR_MISSING0 | FCVAR_UNLOGGED | FCVAR_MISSING1 | FCVAR_REPLICATED | FCVAR_CHEAT | FCVAR_PER_USER | FCVAR_DEMO | FCVAR_DONTRECORD | FCVAR_MISSING2 | FCVAR_RELEASE | FCVAR_MENUBAR_ITEM | FCVAR_MISSING3 | FCVAR_NOT_CONNECTED | FCVAR_VCONSOLE_FUZZY_MATCHING | FCVAR_SERVER_CAN_EXECUTE | FCVAR_CLIENT_CAN_EXECUTE | FCVAR_SERVER_CANNOT_QUERY | FCVAR_VCONSOLE_SET_FOCUS | FCVAR_CLIENTCMD_CAN_EXECUTE | FCVAR_EXECUTE_PER_TICK);

	int unhiddenConCmds = 0;
	int unhiddenConVars = 0;

	ConCommandHandle cmdHandle{};
	auto invalidConcmd = g_pCVar->GetCommand(cmdHandle);
	int cmdIdx = 0;
	for (;;)
	{
		cmdHandle.Set(cmdIdx++);
		auto concmd = g_pCVar->GetCommand(cmdHandle);
		if (concmd == invalidConcmd)
			break;

		if (concmd->GetFlags() & flagsToRemove)
		{
			Msg("- %s\n", concmd->GetName());
			concmd->RemoveFlags(flagsToRemove);
			unhiddenConCmds++;
		}
	}
	ConColorMsg(Color(255, 0, 255, 255), "Removed hidden flags from %d concommands\n", unhiddenConCmds);

	ConVarHandle cvarHandle{};
	auto invalidCvar = g_pCVar->GetConVar(cvarHandle);
	int cvarIdx = 0;
	for (;;)
	{
		cvarHandle.Set(cvarIdx++);
		auto convar = g_pCVar->GetConVar(cvarHandle);
		if (convar == invalidCvar)
			break;

		if (convar->flags & flagsToRemove)
		{
			Msg("- %s\n", convar->m_pszName);
			convar->flags &= ~flagsToRemove;
			unhiddenConVars++;
		}
	}
	ConColorMsg(Color(255, 0, 255, 255), "Removed hidden flags from %d cvars\n", unhiddenConVars);
}

void string_replace(std::string& data, std::string toSearch, std::string replaceStr)
{
	// Get the first occurrence
	size_t pos = data.find(toSearch);

	// Repeat till end is reached
	while (pos != std::string::npos)
	{
		// Replace this occurrence of Sub String
		data.replace(pos, toSearch.size(), replaceStr);
		// Get the next occurrence from the current position
		pos = data.find(toSearch, pos + replaceStr.size());
	}
}
struct ConEntry_t
{
	ConEntry_t() : pszName(nullptr), defaultValue(nullptr), conVarType(EConVarType_Invalid), flags(0), pszDescription(nullptr)
	{
	}

	ConEntry_t(const char* pszName, const CVValue_t* defaultValue, EConVarType conVarType, int flags, const char* pszDescription) : pszName(pszName), defaultValue(defaultValue), conVarType(conVarType), flags(flags), pszDescription(pszDescription)
	{
	}

	const char* pszName;
	const CVValue_t* defaultValue;
	EConVarType conVarType;
	int flags;
	const char* pszDescription;
};

std::string FormatCVValue(const CVValue_t& value, EConVarType type) {
	std::ostringstream oss;

	switch (type) {
	case EConVarType_Bool:
		return value.m_bValue ? "true" : "false";
	case EConVarType_Int16:
		oss << value.m_i16Value;
		break;
	case EConVarType_UInt16:
		oss << value.m_u16Value;
		break;
	case EConVarType_Int32:
		oss << value.m_i32Value;
		break;
	case EConVarType_UInt32:
		oss << value.m_u32Value;
		break;
	case EConVarType_Int64:
		oss << value.m_i64Value;
		break;
	case EConVarType_UInt64:
		oss << value.m_u64Value;
		break;
	case EConVarType_Float32:
		oss << value.m_flValue;
		break;
	case EConVarType_Float64:
		oss << value.m_dbValue;
		break;
	case EConVarType_String:
		oss << (value.m_szValue != nullptr ? value.m_szValue : "");
		break;
	case EConVarType_Color:
		oss << value.m_clrValue.r() << " " << value.m_clrValue.g() << " "
			<< value.m_clrValue.b() << " " << value.m_clrValue.a();
		break;
	case EConVarType_Vector2:
		oss << value.m_vec2Value.x << " " << value.m_vec2Value.y;
		break;
	case EConVarType_Vector3:
		oss << value.m_vec3Value.x << " " << value.m_vec3Value.y << " " << value.m_vec3Value.z;
		break;
	case EConVarType_Vector4:
		oss << value.m_vec4Value.x << " " << value.m_vec4Value.y << " " << value.m_vec4Value.z
			<< " " << value.m_vec4Value.w;
		break;
	case EConVarType_Qangle:
		oss << value.m_angValue.x << " " << value.m_angValue.y << " " << value.m_angValue.z;
		break;
	default:
		return "(unknown type)";
	}

	return oss.str();
}

static std::string ConvarFlagsString(int unFlags)
{
	std::vector<std::string> flags;

	if (unFlags & FCVAR_NONE)
	flags.push_back("FCVAR_NONE");
	if (unFlags & FCVAR_LINKED_CONCOMMAND)
	flags.push_back("FCVAR_LINKED_CONCOMMAND");
	if (unFlags & FCVAR_DEVELOPMENTONLY)
	flags.push_back("FCVAR_DEVELOPMENTONLY");
	if (unFlags & FCVAR_GAMEDLL)
	flags.push_back("FCVAR_GAMEDLL");
	if (unFlags & FCVAR_CLIENTDLL)
	flags.push_back("FCVAR_CLIENTDLL");
	if (unFlags & FCVAR_HIDDEN)
	flags.push_back("FCVAR_HIDDEN");
	if (unFlags & FCVAR_PROTECTED)
	flags.push_back("FCVAR_PROTECTED");
	if (unFlags & FCVAR_SPONLY)
	flags.push_back("FCVAR_SPONLY");
	if (unFlags & FCVAR_ARCHIVE)
	flags.push_back("FCVAR_ARCHIVE");
	if (unFlags & FCVAR_NOTIFY)
	flags.push_back("FCVAR_NOTIFY");
	if (unFlags & FCVAR_USERINFO)
	flags.push_back("FCVAR_USERINFO");
	if (unFlags & FCVAR_MISSING0)
	flags.push_back("FCVAR_MISSING0");
	if (unFlags & FCVAR_UNLOGGED)
	flags.push_back("FCVAR_UNLOGGED");
	if (unFlags & FCVAR_MISSING1)
	flags.push_back("FCVAR_MISSING1");
	if (unFlags & FCVAR_REPLICATED)
	flags.push_back("FCVAR_REPLICATED");
	if (unFlags & FCVAR_CHEAT)
	flags.push_back("FCVAR_CHEAT");
	if (unFlags & FCVAR_PER_USER)
	flags.push_back("FCVAR_PER_USER");
	if (unFlags & FCVAR_DEMO)
	flags.push_back("FCVAR_DEMO");
	if (unFlags & FCVAR_DONTRECORD)
	flags.push_back("FCVAR_DONTRECORD");
	if (unFlags & FCVAR_MISSING2)
	flags.push_back("FCVAR_MISSING2");
	if (unFlags & FCVAR_RELEASE)
	flags.push_back("FCVAR_RELEASE");
	if (unFlags & FCVAR_MENUBAR_ITEM)
	flags.push_back("FCVAR_MENUBAR_ITEM");
	if (unFlags & FCVAR_MISSING3)
	flags.push_back("FCVAR_MISSING3");
	if (unFlags & FCVAR_NOT_CONNECTED)
	flags.push_back("FCVAR_NOT_CONNECTED");
	if (unFlags & FCVAR_VCONSOLE_FUZZY_MATCHING)
	flags.push_back("FCVAR_VCONSOLE_FUZZY_MATCHING");
	if (unFlags & FCVAR_SERVER_CAN_EXECUTE)
	flags.push_back("FCVAR_SERVER_CAN_EXECUTE");
	if (unFlags & FCVAR_CLIENT_CAN_EXECUTE)
	flags.push_back("FCVAR_CLIENT_CAN_EXECUTE");
	if (unFlags & FCVAR_SERVER_CANNOT_QUERY)
	flags.push_back("FCVAR_SERVER_CANNOT_QUERY");
	if (unFlags & FCVAR_VCONSOLE_SET_FOCUS)
	flags.push_back("FCVAR_VCONSOLE_SET_FOCUS");
	if (unFlags & FCVAR_CLIENTCMD_CAN_EXECUTE)
	flags.push_back("FCVAR_CLIENTCMD_CAN_EXECUTE");
	if (unFlags & FCVAR_EXECUTE_PER_TICK)
	flags.push_back("FCVAR_EXECUTE_PER_TICK");

	std::string result;
	bool bFirst = true;

	for (auto flag : flags)
	{
		if (bFirst)
			bFirst = false;
		else
			//result += ", ";
			result += "<br>";

		result += flag;
	}

	return result;
}

static std::string ConvarFlagsStringShort(int unFlags)
{
	std::vector<std::string> flags;

	if (unFlags & FCVAR_NONE)
		flags.push_back("");
	if (unFlags & FCVAR_LINKED_CONCOMMAND)
		flags.push_back("linked_concommand");
	if (unFlags & FCVAR_DEVELOPMENTONLY)
		flags.push_back("devonly");
	if (unFlags & FCVAR_GAMEDLL)
		flags.push_back("sv");
	if (unFlags & FCVAR_CLIENTDLL)
		flags.push_back("cl");
	if (unFlags & FCVAR_HIDDEN)
		flags.push_back("hidden");
	if (unFlags & FCVAR_PROTECTED)
		flags.push_back("prot");
	if (unFlags & FCVAR_SPONLY)
		flags.push_back("sp");
	if (unFlags & FCVAR_ARCHIVE)
		flags.push_back("a");
	if (unFlags & FCVAR_NOTIFY)
		flags.push_back("nf");
	if (unFlags & FCVAR_USERINFO)
		flags.push_back("user");
	if (unFlags & FCVAR_MISSING0)
		flags.push_back("missing0");
	if (unFlags & FCVAR_UNLOGGED)
		flags.push_back("unlogged");
	if (unFlags & FCVAR_MISSING1)
		flags.push_back("missing1");
	if (unFlags & FCVAR_REPLICATED)
		flags.push_back("rep");
	if (unFlags & FCVAR_CHEAT)
		flags.push_back("cheat");
	if (unFlags & FCVAR_PER_USER)
		flags.push_back("per_user");
	if (unFlags & FCVAR_DEMO)
		flags.push_back("demo");
	if (unFlags & FCVAR_DONTRECORD)
		flags.push_back("norecord");
	if (unFlags & FCVAR_MISSING2)
		flags.push_back("missing2");
	if (unFlags & FCVAR_RELEASE)
		flags.push_back("release");
	if (unFlags & FCVAR_MENUBAR_ITEM)
		flags.push_back("menubar_item");
	if (unFlags & FCVAR_MISSING3)
		flags.push_back("missing3");
	if (unFlags & FCVAR_NOT_CONNECTED)
		flags.push_back("notconnected");
	if (unFlags & FCVAR_VCONSOLE_FUZZY_MATCHING)
		flags.push_back("vconsole_fuzzy_matching");
	if (unFlags & FCVAR_SERVER_CAN_EXECUTE)
		flags.push_back("server_can_execute");
	if (unFlags & FCVAR_CLIENT_CAN_EXECUTE)
		flags.push_back("client_can_execute");
	if (unFlags & FCVAR_SERVER_CANNOT_QUERY)
		flags.push_back("server_cannot_query");
	if (unFlags & FCVAR_VCONSOLE_SET_FOCUS)
		flags.push_back("vconsole_set_focus");
	if (unFlags & FCVAR_CLIENTCMD_CAN_EXECUTE)
		flags.push_back("clientcmd_can_execute");
	if (unFlags & FCVAR_EXECUTE_PER_TICK)
		flags.push_back("per_tick");

	std::string result;
	bool bFirst = true;

	for (auto flag : flags)
	{
		if (bFirst)
			bFirst = false;
		else
			result += ", ";
			//result += "<br>";

		result += flag;
	}

	return result;
}


std::string MarkdownEscape(const std::string& str)
{
	std::string escaped{ str };

	string_replace(escaped, "\\", "\\\\");
	string_replace(escaped, "<", "&lt;");
	string_replace(escaped, ">", "&gt;");
	string_replace(escaped, "[", "\\[");
	string_replace(escaped, "]", "\\]");
	string_replace(escaped, "\n", "<br>");
	string_replace(escaped, "|", "\\|");

	return escaped;
}

std::string NewLineEscape(const std::string& str) {
	
	std::string escaped{ str };

	string_replace(escaped, "\n", "\\n");

	return escaped;
}

static ConCommandRefAbstract cvarlist_md_ref;
static void cvarlist_md_callback(const CCommand& args);
ConCommand cvarlist_md(&cvarlist_md_ref, "zcvarlist_md", cvarlist_md_callback, "List all convars/concmds in Markdown format. Format: [hidden]");
static void cvarlist_md_callback(const CCommand& args)
{
	std::map<std::string, ConEntry_t> allEntries;

	ConCommandHandle cmdHandle{};
	auto invalidConcmd = g_pCVar->GetCommand(cmdHandle);
	int cmdIdx = 0;
	for (;;)
	{
		cmdHandle.Set(cmdIdx++);
		auto concmd = g_pCVar->GetCommand(cmdHandle);
		if (concmd == invalidConcmd)
			break;

		allEntries[concmd->GetName()] = ConEntry_t(concmd->GetName(), nullptr, EConVarType_Invalid, concmd->GetFlags(), concmd->GetHelpText());
	}

	ConVarHandle cvarHandle{};
	auto invalidCvar = g_pCVar->GetConVar(cvarHandle);
	int cvarIdx = 0;
	for (;;)
	{
		cvarHandle.Set(cvarIdx++);
		auto convar = g_pCVar->GetConVar(cvarHandle);
		if (convar == invalidCvar)
			break;

		allEntries[convar->m_pszName] = ConEntry_t(convar->m_pszName, convar->m_cvvDefaultValue, convar->m_eVarType, convar->flags, convar->m_pszHelpString);
	}

	auto bShowHidden = !V_stricmp(args.Arg(1), "hidden");

	CUtlString outputPath(Plat_GetGameDirectory());
	outputPath.Append("\\csgo\\cvarlist.md");
	outputPath.FixSlashes();

	CUtlString outputPath2(Plat_GetGameDirectory());
	outputPath2.Append("\\csgo\\cvarlist.log");
	outputPath2.FixSlashes();

	std::ofstream file(outputPath.GetForModify());
	std::ofstream file2(outputPath2.GetForModify());

	file << "Name | Flags | Description\n";
	file << "---- | ----- | -----------\n";

	file2 << "cvar list\n";
	file2 << "--------------\n";

	int nWritten = 0;
	for (auto& pair : allEntries)
	{
		const std::string& name = pair.first;
		auto cmd = pair.second;

		//if (!bShowHidden && (cmd.flags & (FCVAR_DEVELOPMENTONLY | FCVAR_HIDDEN | FCVAR_MISSING3)))
		//	continue;
		//if (!bShowHidden && (cmd.flags & (FCVAR_NONE | FCVAR_LINKED_CONCOMMAND | FCVAR_DEVELOPMENTONLY | FCVAR_GAMEDLL | FCVAR_CLIENTDLL | FCVAR_HIDDEN | FCVAR_PROTECTED | FCVAR_SPONLY | FCVAR_ARCHIVE | FCVAR_NOTIFY | FCVAR_USERINFO | FCVAR_MISSING0 | FCVAR_UNLOGGED | FCVAR_MISSING1 | FCVAR_REPLICATED | FCVAR_CHEAT | FCVAR_PER_USER | FCVAR_DEMO | FCVAR_DONTRECORD | FCVAR_MISSING2 | FCVAR_RELEASE | FCVAR_MENUBAR_ITEM | FCVAR_MISSING3 | FCVAR_NOT_CONNECTED | FCVAR_VCONSOLE_FUZZY_MATCHING | FCVAR_SERVER_CAN_EXECUTE | FCVAR_CLIENT_CAN_EXECUTE | FCVAR_SERVER_CANNOT_QUERY | FCVAR_VCONSOLE_SET_FOCUS | FCVAR_CLIENTCMD_CAN_EXECUTE | FCVAR_EXECUTE_PER_TICK)))
		//	continue;

		auto flagsStr = ConvarFlagsString(cmd.flags);
		auto flagsStr_ = ConvarFlagsStringShort(cmd.flags);
		auto helpText = MarkdownEscape(cmd.pszDescription);
		auto helpText_ = NewLineEscape(cmd.pszDescription);

		int name_setw = (82 - name.length() >= 0) ? (82 - name.length()) : 0;
		int flags_setw = (38 - flagsStr_.length() >= 0) ? (38 - flagsStr_.length()) : 0;

		//file << name << " | " << flagsStr << " | ";
		
		if (cmd.defaultValue != nullptr) // there is a def value
		{
			auto defaultValue = MarkdownEscape(FormatCVValue(*cmd.defaultValue, cmd.conVarType));
			auto defaultValue_ = NewLineEscape(FormatCVValue(*cmd.defaultValue, cmd.conVarType));
			//file << "Default: " << defaultValue << "<br>";
			//file << name << " " << defaultValue << " | " << flagsStr << " | " << helpText << "\n";
			int def_setw = (20 - defaultValue.length() >= 0) ? (20 - defaultValue.length()) : 0;

			file << name << " | " << flagsStr << " | " << "**Default:** " << defaultValue << "<br>" << helpText << "\n";
			file2 << name << std::setw(name_setw) << ": " << defaultValue_ << std::setw(def_setw) << " : " << flagsStr_ << std::setw(flags_setw) << " : " << helpText_ << "\n";
		}
		else { // no def value
			file << name <<  " | " << flagsStr << " | " << helpText << "\n";
			//file2 << name << "\t\t\t\t\t\t\t: " << "cmd" << "\t\t: " << flagsStr_ << "\t\t\t\t\t: " << helpText_ << "\n";
			int name_setw2 = name_setw + 3;
			file2 << name << std::setw(name_setw2) << ": cmd" << std::setw(17) << " : " << flagsStr_ << std::setw(flags_setw) << " : " << helpText_ << "\n";
		}
		
		//file << helpText << "\n";
		nWritten++;
	}

	file.close();
	file2.close();

	Msg("Wrote %d entries to %s\n", nWritten, outputPath.GetForModify());
}
