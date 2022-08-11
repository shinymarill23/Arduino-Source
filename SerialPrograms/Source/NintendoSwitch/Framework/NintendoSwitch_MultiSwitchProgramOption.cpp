/*  Multi-Switch Program Option
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include "Common/Cpp/Json/JsonValue.h"
#include "Common/Cpp/Json/JsonObject.h"
#include "NintendoSwitch_MultiSwitchProgramOption.h"
#include "NintendoSwitch_MultiSwitchProgramWidget.h"

namespace PokemonAutomation{
namespace NintendoSwitch{



MultiSwitchProgramOption::MultiSwitchProgramOption(const MultiSwitchProgramDescriptor& descriptor)
    : PanelInstance(descriptor)
    , m_descriptor(descriptor)
    , m_system(
        descriptor.min_pabotbase_level(),
        descriptor.feedback(),
        descriptor.allow_commands_while_running(),
        descriptor.min_switches(),
        descriptor.max_switches(),
        descriptor.default_switches()
    )
    , m_instance(descriptor.make_instance())
{}

void MultiSwitchProgramOption::from_json(const JsonValue& json){
    const JsonObject* obj = json.get_object();
    if (obj == nullptr){
        return;
    }
    const JsonValue* value = obj->get_value("SwitchSetup");
    if (value){
        m_system.load_json(*value);
    }
    m_instance->from_json(json);
}
JsonValue MultiSwitchProgramOption::to_json() const{
    JsonObject obj = std::move(*m_instance->to_json().get_object());
    obj["SwitchSetup"] = m_system.to_json();
    return obj;
}

ConfigOption& MultiSwitchProgramOption::options(){
    return m_instance->m_options;
}

std::string MultiSwitchProgramOption::check_validity() const{
    return m_instance->check_validity();
}
void MultiSwitchProgramOption::restore_defaults(){
    m_instance->restore_defaults();
}


QWidget* MultiSwitchProgramOption::make_widget(QWidget& parent, PanelHolder& holder){
    return new MultiSwitchProgramWidget2(parent, *this, holder);
}




}
}