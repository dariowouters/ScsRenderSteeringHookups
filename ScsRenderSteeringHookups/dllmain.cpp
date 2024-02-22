#define WIN32_LEAN_AND_MEAN

#include "Windows.h"
#include "memoryapi.h"
#include "memory.h"

#include "scssdk_telemetry.h"

uintptr_t g_steering_render_hookups_check_address = NULL;
uintptr_t game_base;

scs_log_t scs_log = nullptr;

DWORD old_acc_protect = NULL;

void cleanup()
{
    if (g_steering_render_hookups_check_address == 0)
    {
        return;
    }

    VirtualProtect(reinterpret_cast<LPVOID>(g_steering_render_hookups_check_address),
                   3,
                   PAGE_EXECUTE_READWRITE,
                   &old_acc_protect);
    *reinterpret_cast<uint8_t*>(g_steering_render_hookups_check_address) = 0x85;
    *reinterpret_cast<uint8_t*>(g_steering_render_hookups_check_address + 2) = 0x74;
    VirtualProtect(reinterpret_cast<LPVOID>(g_steering_render_hookups_check_address), 3, old_acc_protect, nullptr);
    scs_log(SCS_LOG_TYPE_message, "[ScsRenderSteeringHookups] Unpatched 'r_steering_wheel' check for hookups");
}

bool bypass_type_check()
{
    if (g_steering_render_hookups_check_address == 0)
    {
        return false;
    }

    VirtualProtect(reinterpret_cast<LPVOID>(g_steering_render_hookups_check_address),
                   3,
                   PAGE_EXECUTE_READWRITE,
                   &old_acc_protect);
    *reinterpret_cast<uint8_t*>(g_steering_render_hookups_check_address) = 0x33;
    *reinterpret_cast<uint8_t*>(g_steering_render_hookups_check_address + 2) = 0x75;
    VirtualProtect(reinterpret_cast<LPVOID>(g_steering_render_hookups_check_address), 3, old_acc_protect, nullptr);
    scs_log(SCS_LOG_TYPE_message, "[ScsRenderSteeringHookups] Patched 'r_steering_wheel' check for hookups");
    return true;
}

SCSAPI_RESULT scs_telemetry_init(const scs_u32_t version, const scs_telemetry_init_params_t* const params)
{
    // We currently support only one version.
    if (version != SCS_TELEMETRY_VERSION_1_01)
    {
        return SCS_RESULT_unsupported;
    }

    const auto version_params = reinterpret_cast<const scs_telemetry_init_params_v101_t*>(params);
    scs_log = version_params->common.log;

    std::stringstream ss;
    ss << "[ScsRenderSteeringHookups] Found 'r_steering_wheel' check for hookups address @ &" << std::hex <<
        g_steering_render_hookups_check_address << " "
        << (strcmp(version_params->common.game_id, "eut2") == 0 ? "eurotrucks2" : "amtrucks") << ".exe+"
        << (g_steering_render_hookups_check_address - game_base);
    scs_log(SCS_LOG_TYPE_message, ss.str().c_str());

    if (!bypass_type_check())
    {
        version_params->common.log(SCS_LOG_TYPE_error,
                                   "[ScsRenderSteeringHookups] Could not bypass 'r_steering_wheel' check for hookups");
        return SCS_RESULT_invalid_parameter;
    }

    scs_log(SCS_LOG_TYPE_message, "[ScsRenderSteeringHookups] Plugin Loaded");

    return SCS_RESULT_ok;
}


/**
 * @brief Telemetry API deinitialization function.
 *
 * See scssdk_telemetry.h
 */
SCSAPI_VOID scs_telemetry_shutdown(void)
{
    cleanup();
}

BOOL __stdcall DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        game_base = reinterpret_cast<uintptr_t>(GetModuleHandleA(nullptr));
        const auto header = reinterpret_cast<const IMAGE_DOS_HEADER*>(game_base);
        const auto nt_header = reinterpret_cast<const IMAGE_NT_HEADERS64*>(reinterpret_cast<const uint8_t*>(header) + header->e_lfanew);
        const auto total_size = nt_header->OptionalHeader.SizeOfImage;

        g_steering_render_hookups_check_address = pattern::scan("85 c0 74 ? 48 8b 4b ? 48 8b d6 48 8b 01 ff",
                                                                game_base,
                                                                total_size,
                                                                2);
    }

    return TRUE;
}
