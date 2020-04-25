#define MIN_WINDOWNAME_CHARS 8
#define MAX_WINDOWNAME_CHARS 16

using namespace std::chrono;

extern bool g_IsPanorama;

//include json
#include "Include/nlohmann/json.hpp"

using json = nlohmann::json;

//include utils
#include "config.h"
#include "helper.h"
#include "console.h"

//include BSPParser
#include "Include/ValveBSP/BSPParser.hpp"

//include math
#include "vector.h"
#include "matrix.h"
#include "game_math.h"

//include memory
#include "module.h"
#include "memory.h"
#include "offsets.h"

//include game
#include "definitions.h"
#include "netvars.h"
#include "game.h"
#include "entity.h"

//include main
#include "entity_updater.h"
#include "visibility.h"
#include "shooter.h"

//include profiler
#include "entity_profile.h"
#include "feature_data.h"
#include "profiler.h"
#include "monitor.h"
#include "analyzer.h"

//include hacks
#include "assistant.h"
#include "aimbot.h"
#include "triggerbot.h"

//include overlay
#include "overlay_window.h"
#include "d3d.h"
#include "esp.h"
