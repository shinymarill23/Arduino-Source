/*  Shiny Hunt - Custom Path
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include "CommonFramework/Notifications/ProgramNotifications.h"
#include "CommonFramework/Tools/StatsTracking.h"
#include "NintendoSwitch/NintendoSwitch_Settings.h"
#include "NintendoSwitch/Commands/NintendoSwitch_Commands_PushButtons.h"
#include "PokemonLA/PokemonLA_Settings.h"
#include "PokemonLA/Programs/PokemonLA_GameEntry.h"
#include "PokemonLA/Programs/PokemonLA_RegionNavigation.h"
#include "PokemonLA/Programs/PokemonLA_MountChange.h"
#include "PokemonLA_ShinyHunt-CustomPath.h"
#include "CommonFramework/InferenceInfra/InferenceRoutines.h"
#include "PokemonLA/Inference/PokemonLA_ShinySoundDetector.h"

#include <iostream>

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonLA{


ShinyHuntCustomPath_Descriptor::ShinyHuntCustomPath_Descriptor()
    : RunnableSwitchProgramDescriptor(
        "PokemonLA:ShinyHunt-CustomPath",
        STRING_POKEMON + " LA", "Shiny Hunt - Custom Path",
        "ComputerControl/blob/master/Wiki/Programs/PokemonLA/ShinyHunt-CustomPath.md",
        "Repeatedly travel on a custom path to shiny hunt " + STRING_POKEMON + " around it.",
        FeedbackType::REQUIRED, false,
        PABotBaseLevel::PABOTBASE_12KB
    )
{}


ShinyHuntCustomPath::ShinyHuntCustomPath(const ShinyHuntCustomPath_Descriptor& descriptor)
    : SingleSwitchProgramInstance(descriptor)
    , TEST_PATH(
        "<b>Test Path:</b><br>Run the path immediately on the map to test it.",
        false
    )
    , NOTIFICATION_STATUS("Status Update", true, false, std::chrono::seconds(3600))
    , NOTIFICATIONS({
        &NOTIFICATION_STATUS,
        &SHINY_DETECTED.NOTIFICATIONS,
        &NOTIFICATION_PROGRAM_FINISH,
//        &NOTIFICATION_ERROR_RECOVERABLE,
        &NOTIFICATION_ERROR_FATAL,
    })
{
    PA_ADD_OPTION(TRAVEL_LOCATION);
    PA_ADD_OPTION(CUSTOM_PATH_TABLE);
    PA_ADD_OPTION(TEST_PATH);
    PA_ADD_OPTION(SHINY_DETECTED);
    PA_ADD_OPTION(NOTIFICATIONS);
}

class ShinyHuntCustomPath::Stats : public StatsTracker, public ShinyStatIncrementer{
public:
    Stats()
        : attempts(m_stats["Attempts"])
        , errors(m_stats["Errors"])
        , shinies(m_stats["Shinies"])
    {
        m_display_order.emplace_back("Attempts");
        m_display_order.emplace_back("Errors", true);
        m_display_order.emplace_back("Shinies", true);
    }
    virtual void add_shiny() override{
        shinies++;
    }

    std::atomic<uint64_t>& attempts;
    std::atomic<uint64_t>& errors;
    std::atomic<uint64_t>& shinies;
};

std::unique_ptr<StatsTracker> ShinyHuntCustomPath::make_stats() const{
    return std::unique_ptr<StatsTracker>(new Stats());
}

void ShinyHuntCustomPath::do_non_listen_action(SingleSwitchProgramEnvironment& env, size_t action_index){
    const auto& row = CUSTOM_PATH_TABLE.get_action(action_index);
    env.log("Execute action " + PathAction_NAMES[(size_t)row.action]);
    switch(row.action){
        case PathAction::CHANGE_MOUNT:
        {
            MountState mountState = MountState::NOTHING;
            switch(row.mount){
            case PathMount::WYRDEER:
                mountState = MountState::WYRDEER_ON;
                break;
            case PathMount::URSALUNA:
                mountState = MountState::URSALUNA_ON;
                break;
            case PathMount::BASCULEGION:
                mountState = MountState::BASCULEGION_ON;
                break;
            case PathMount::SNEASLER:
                mountState = MountState::SNEASLER_ON;
                break;
            case PathMount::BRAVIARY:
                mountState = MountState::BRAVIARY_ON;
                break;
            default:
                break;
            }

            if (mountState == MountState::NOTHING){
                dismount(env.console);
            } else{
                change_mount(env.console, mountState);
            }
            break;
        }
        case PathAction::ROTATE_CAMERA:
        {
            if (row.camera_turn_ticks > 0){
                pbf_move_right_joystick(env.console, 255, 128, uint16_t(row.camera_turn_ticks), 0);
            } else if (row.camera_turn_ticks < 0){
                pbf_move_right_joystick(env.console, 0, 128, uint16_t(-row.camera_turn_ticks), 0);
            }
            break;
        }
        case PathAction::MOVE_FORWARD:
        {
            switch(row.move_speed){
            case PathSpeed::NORMAL_SPEED:
                pbf_move_left_joystick(env.console, 128, 0, row.move_forward_ticks, 0);
                break;
            case PathSpeed::SLOW_SPEED:
                pbf_move_left_joystick(env.console, 128, 64, row.move_forward_ticks, 0);
                break;
            case PathSpeed::RUN:
                pbf_controller_state(env.console, BUTTON_LCLICK, DPAD_NONE, 128, 0, 128, 128, row.move_forward_ticks);
                break;
            case PathSpeed::DASH:
                pbf_press_button(env.console, BUTTON_B, row.move_forward_ticks, 0);
                break;
            case PathSpeed::DASH_B_SPAM:
                pbf_mash_button(env.console, BUTTON_B, row.move_forward_ticks);
                break;
            case PathSpeed::DIVE:
                pbf_press_button(env.console, BUTTON_Y, row.move_forward_ticks, 0);
                break;
            }
            break;
        }
        case PathAction::JUMP:
        {
            pbf_press_button(env.console, BUTTON_Y, 10, row.jump_wait_ticks);
            break;
        }
        case PathAction::WAIT:
        {
            pbf_wait(env.console, row.wait_ticks);
            break;
        }
        default:   
            break;
    } // end switch action
    env.console.botbase().wait_for_all_requests();
}


void ShinyHuntCustomPath::run_path(SingleSwitchProgramEnvironment& env){
    Stats& stats = env.stats<Stats>();

    for(size_t action_index = 0; action_index < CUSTOM_PATH_TABLE.num_actions(); action_index++){
        const auto& row = CUSTOM_PATH_TABLE.get_action(action_index);
        if (row.action != PathAction::START_LISTEN){
            do_non_listen_action(env, action_index);
        } else{
            env.log("Start Listen, build sound detector");
            // Build shiny sound detector and start listens:
            ShinySoundDetector shiny_detector(env.console, SHINY_DETECTED.stop_on_shiny());
            // TODO: run_until() is not designed to pass `env` inside. run_until() relies on the usage
            // of the passed in const BotBaseContext& context to stop the code inside the lambda function,
            // but the code using the passed in `env` may still runs. This will delay the program stop
            // on shiny sound but should be genearally OK in this use case.
            run_until(
                env, env.console,
                [&env, &action_index, this](const BotBaseContext& context){
                    for(; action_index < CUSTOM_PATH_TABLE.num_actions(); action_index++){
                        const auto& listened_row = CUSTOM_PATH_TABLE.get_action(action_index);
                        if (listened_row.action != PathAction::END_LISTEN){
                            do_non_listen_action(env, action_index);
                        } else{
                            env.log("End Listen, exit sound detector");
                            break;
                        }
                    }
                    context.wait_for_all_requests();
                },
                { &shiny_detector });
            if (shiny_detector.detected()){
                stats.shinies++;
                on_shiny_sound(env, env.console, SHINY_DETECTED, shiny_detector.results());
                break;
            }
        }
    } // end for loop on each action
}


void ShinyHuntCustomPath::program(SingleSwitchProgramEnvironment& env){
    Stats& stats = env.stats<Stats>();

    //  Connect the controller.
    pbf_press_button(env.console, BUTTON_LCLICK, 5, 5);

    if (TEST_PATH){
        // Run the test path immediately
        env.log("Testing path...");
        run_path(env);
        return;
    }

    //  Check whether the user has set shiny sound listen action:
    {
        bool has_listen_action = false;
        for(size_t i = 0; i < CUSTOM_PATH_TABLE.num_actions(); i++){
            if (CUSTOM_PATH_TABLE.get_action(i).action == PathAction::START_LISTEN){
                has_listen_action = true;
                break;
            }
        }
        if (has_listen_action == false){
            throw OperationFailedException(env.console, "No START LISTEN action specified.");
        }
    }

    while (true){
        env.update_stats();
        send_program_status_notification(
            env.logger(), NOTIFICATION_STATUS,
            env.program_info(),
            "",
            stats.to_str()
        );
        try{
//            Stats& stats = env.stats<Stats>();

            goto_camp_from_jubilife(env, env.console, TRAVEL_LOCATION);
            run_path(env);

            stats.attempts++;

            pbf_press_button(env.console, BUTTON_HOME, 20, GameSettings::instance().GAME_TO_HOME_DELAY);
            reset_game_from_home(env, env.console, ConsoleSettings::instance().TOLERATE_SYSTEM_UPDATE_MENU_FAST);
        }catch (OperationFailedException&){
            stats.errors++;
            pbf_press_button(env.console, BUTTON_HOME, 20, GameSettings::instance().GAME_TO_HOME_DELAY);
            reset_game_from_home(env, env.console, ConsoleSettings::instance().TOLERATE_SYSTEM_UPDATE_MENU_FAST);
        }

    }

    env.update_stats();
    send_program_finished_notification(
        env.logger(), NOTIFICATION_PROGRAM_FINISH,
        env.program_info(),
        "",
        stats.to_str()
    );
}





}
}
}
