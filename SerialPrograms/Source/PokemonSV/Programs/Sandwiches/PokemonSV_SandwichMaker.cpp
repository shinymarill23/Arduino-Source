/*  Sandwich Maker
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include "Common/Cpp/PrettyPrint.h"
#include "CommonFramework/Notifications/ProgramNotifications.h"
#include "CommonFramework/Tools/VideoResolutionCheck.h"
#include "NintendoSwitch/Commands/NintendoSwitch_Commands_PushButtons.h"
#include "Pokemon/Pokemon_Strings.h"
#include "Common/Cpp/Exceptions.h"
#include "PokemonSV/Programs/PokemonSV_SaveGame.h"
#include "PokemonSV/Programs/Sandwiches/PokemonSV_SandwichRoutines.h"
#include "PokemonSV/Inference/Overworld/PokemonSV_OverworldDetector.h"
#include "PokemonSV/Resources/PokemonSV_Ingredients.h"
#include "PokemonSV/Programs/Sandwiches/PokemonSV_IngredientSession.h"
#include "PokemonSV_SandwichMaker.h"

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonSV{

using namespace Pokemon;

SandwichMaker_Descriptor::SandwichMaker_Descriptor()
    : SingleSwitchProgramDescriptor(
        "PokemonSV:SandwichMaker",
        STRING_POKEMON + " SV", "Sandwich Maker",
        "ComputerControl/blob/master/Wiki/Programs/PokemonSV/SandwichMaker.md",
        "Make a sandwich of your choice.",
        FeedbackType::REQUIRED,
        AllowCommandsWhenRunning::DISABLE_COMMANDS,
        PABotBaseLevel::PABOTBASE_12KB
    )
{}

SandwichMaker::SandwichMaker()
    : GO_HOME_WHEN_DONE(false)
    , NOTIFICATIONS({
        &NOTIFICATION_PROGRAM_FINISH,
        &NOTIFICATION_ERROR_FATAL,
        })
{
    PA_ADD_OPTION(SANDWICH_OPTIONS);
    PA_ADD_OPTION(GO_HOME_WHEN_DONE);
    PA_ADD_OPTION(NOTIFICATIONS);
}

void SandwichMaker::execute_action(ConsoleHandle& console, BotBaseContext& context, const SandwichIngredientsTableRow& row) {
   
}

void SandwichMaker::program(SingleSwitchProgramEnvironment& env, BotBaseContext& context) {
    assert_16_9_720p_min(env.logger(), env.console);


    //Player must be on default sandwich menu
    //enter_custom_sandwich_mode(env.program_info(), env.console, context);
    //if (SANDWICH_OPTIONS.LANGUAGE == Language::None) {
    //    throw UserSetupError(env.console.logger(), "Must set game langauge option to read ingredient lists.");
    //}

    int num_fillings = 0;
    int num_condiments = 0;

    //std::map<std::string, uint8_t> fillings;// = { {"lettuce", (uint8_t)1} };
    //std::map<std::string, uint8_t> condiments;// = { {"sweet-herba-mystica", (uint8_t)1} };
    std::vector<std::string> fillings;
    std::vector<std::string> condiments;

    //Build maps of the selected ingredients if set to custom
    if (SANDWICH_OPTIONS.SANDWICH_RECIPE == SandwichMakerOption::SandwichRecipe::custom) {
        env.log("Custom sandwich selected. Validating ingredients.", COLOR_BLACK);

        std::vector<std::unique_ptr<SandwichIngredientsTableRow>> table = SANDWICH_OPTIONS.SANDWICH_INGREDIENTS.copy_snapshot();

        for (const std::unique_ptr<SandwichIngredientsTableRow>& row : table) {
            if (std::find(ALL_SANDWICH_FILLINGS_SLUGS().begin(), ALL_SANDWICH_FILLINGS_SLUGS().end(), row->item.slug()) != ALL_SANDWICH_FILLINGS_SLUGS().end()) {
                fillings.push_back(row->item.slug());
                num_fillings++;
            }
            else {
                condiments.push_back(row->item.slug());
                num_condiments++;
            }
            //execute_action(env.console, context, *row);
            //context.wait_for_all_requests();
        }

        if (num_fillings == 0 || num_condiments == 0) {
            throw UserSetupError(env.console.logger(), "Must have at least one filling and at least one condiment.");
        }

        if (num_fillings > 6 || num_condiments > 4) {
            throw UserSetupError(env.console.logger(), "Number of fillings exceed 6 and/or number of condiments exceed 4.");
        }
        //add_sandwich_ingredients(dispatcher, console, context, SANDWICH_OPTIONS.LANGUAGE, std::move(fillings), std::move(condiments));

        env.log("Ingredients validated.", COLOR_BLACK);
    }

    //SANDWICH_OPTIONS.SANDWICH_INGREDIENTS.
    //use pla custom path as example?


    GO_HOME_WHEN_DONE.run_end_of_program(context);
    send_program_finished_notification(env, NOTIFICATION_PROGRAM_FINISH);
}


}
}
}

