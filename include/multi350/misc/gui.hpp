#ifndef PROJCONTROL_GUI_HPP
#define PROJCONTROL_GUI_HPP

#include "al/ui/al_ParameterGUI.hpp"
#include "al/ui/al_PresetHandler.hpp"
#include "multi350/controller.hpp"
#include <array>
#include <string>
#include <vector>

using namespace al;

struct Multi350GUI {
  multi350::Controller multi350;

  std::array<multi350::PatternSequence, 4> patternSequences;
  int patternSequenceIndex{0};

  multi350::VarExpPatSequence varExpPatSequences;

  Trigger device_list{"device_list", "multi350"};
  Trigger device_open{"device_open", "multi350"};
  Trigger device_close{"device_close", "multi350"};

  ParameterInt proj0_index{"proj0_index", "multi350", 0, 0, 3};
  ParameterInt proj1_index{"proj1_index", "multi350", 1, 0, 3};
  ParameterInt proj2_index{"proj2_index", "multi350", 2, 0, 3};
  ParameterInt proj3_index{"proj3_index", "multi350", 3, 0, 3};
  std::vector<unsigned int> usb_idx;
  std::vector<std::string> usb_names;
  Trigger apply_indices{"apply_indices", "multi350"};
  Trigger save_indices{"save_indices", "multi350"};

  ParameterBool proj0_control{"proj0", "multi350", true};
  ParameterBool proj1_control{"proj1", "multi350", true};
  ParameterBool proj2_control{"proj2", "multi350", true};
  ParameterBool proj3_control{"proj3", "multi350", true};

  Trigger reset{"reset", "multi350"};
  Trigger print_status{"print_status", "multi350"};

  Trigger test_start{"test_start", "multi350"};
  Trigger test_stop{"test_stop", "multi350"};

  Trigger power_normal{"power_normal", "multi350"};
  Trigger power_standby{"power_standby", "multi350"};

  Trigger video_mode{"video_mode", "multi350"};
  Trigger varExpPat_start{"varExpPat_start", "multi350"};
  Trigger pattern_start{"pattern_start", "multi350"};
  Trigger pattern_stop{"pattern_stop", "multi350"};

  Trigger apply_led{"apply_led", "multi350"};
  Trigger save_led{"save_led", "multi350"};

  ParameterInt proj0_red{"proj0_red", "multi350", 40, 0, 255};
  ParameterInt proj0_green{"proj0_green", "multi350", 90, 0, 255};
  ParameterInt proj0_blue{"proj0_blue", "multi350", 255, 0, 255};
  ParameterInt proj1_red{"proj1_red", "multi350", 40, 0, 255};
  ParameterInt proj1_green{"proj1_green", "multi350", 90, 0, 255};
  ParameterInt proj1_blue{"proj1_blue", "multi350", 255, 0, 255};
  ParameterInt proj2_red{"proj2_red", "multi350", 40, 0, 255};
  ParameterInt proj2_green{"proj2_green", "multi350", 90, 0, 255};
  ParameterInt proj2_blue{"proj2_blue", "multi350", 255, 0, 255};
  ParameterInt proj3_red{"proj3_red", "multi350", 40, 0, 255};
  ParameterInt proj3_green{"proj3_green", "multi350", 90, 0, 255};
  ParameterInt proj3_blue{"proj3_blue", "multi350", 255, 0, 255};

  // TODO: change file to std::file to handle paths
  PresetHandler preset_currents{"presets/currents"};
  PresetHandler preset_projectors{"presets/projectors"};

  void init() {
    preset_currents << proj0_red << proj0_green << proj0_blue << proj1_red
                    << proj1_green << proj1_blue << proj2_red << proj2_green
                    << proj2_blue << proj3_red << proj3_green << proj3_blue;
    preset_currents.recallPresetSynchronous("currents");

    preset_projectors << proj0_index << proj1_index << proj2_index
                      << proj3_index;
    preset_projectors.recallPresetSynchronous("projectors");

    multi350.init();

    setupProjectorIndices();
    setupPatternSequences();
    setupCallbacks();
  }

  void shutdown() {
    multi350.close();
    multi350.exit();
  }

  void configureGUI() {
    ImGui::Begin("MULTI350 Control");

    ParameterGUI::draw(&device_list);
    ImGui::SameLine();
    ParameterGUI::draw(&device_open);
    ImGui::SameLine();
    ParameterGUI::draw(&device_close);

    if (!usb_names.empty() &&
        ImGui::CollapsingHeader("Adjust Projector Order",
                                ImGuiTreeNodeFlags_CollapsingHeader)) {
      ImGui::Indent();
      ParameterGUI::draw(&apply_indices);
      ImGui::SameLine();
      ParameterGUI::draw(&save_indices);

      ImGui::NewLine();

      for (int idx = 0; idx < usb_names.size(); ++idx) {
        ImGui::Text("Proj %d: ", idx);
        ImGui::SameLine();
        ImGui::Selectable(usb_names[idx].c_str(), true, 0, ImVec2(30, 15));

        if (ImGui::IsItemActive() && !ImGui::IsItemHovered()) {
          int next_idx = idx + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
          if (next_idx >= 0 && next_idx < usb_names.size()) {
            std::string temp_name(usb_names[idx]);
            usb_names[idx] = usb_names[next_idx];
            usb_names[next_idx] = temp_name;
            int temp_idx(usb_idx[idx]);
            usb_idx[idx] = usb_idx[next_idx];
            usb_idx[next_idx] = temp_idx;
            ImGui::ResetMouseDragDelta();
          }
        }
      }
      ImGui::Unindent();

      proj0_index.setNoCalls(usb_idx[0]);
      proj1_index.setNoCalls(usb_idx[1]);
      proj2_index.setNoCalls(usb_idx[2]);
      proj3_index.setNoCalls(usb_idx[3]);
    }

    ImGui::NewLine();

    if (multi350.isConnected()) {
      ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Connected: %u",
                         multi350.deviceNum());
    } else {
      ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Not Connected");
    }

    int device_num = multi350.deviceNum();
    if (device_num < 4) {
      proj3_control.setHint("hide", true);
      if (device_num < 3) {
        proj2_control.setHint("hide", true);
        if (device_num < 2) {
          proj1_control.setHint("hide", true);
          if (device_num < 1) {
            proj0_control.setHint("hide", true);
          } else {
            proj0_control.setHint("hide", false);
          }
        } else {
          proj1_control.setHint("hide", false);
        }
      } else {
        proj2_control.setHint("hide", false);
      }
    } else {
      proj3_control.setHint("hide", false);
      proj2_control.setHint("hide", false);
      proj1_control.setHint("hide", false);
      proj0_control.setHint("hide", false);
    }

    ParameterGUI::draw(&proj0_control);
    ImGui::SameLine();
    ParameterGUI::draw(&proj1_control);
    ImGui::SameLine();
    ParameterGUI::draw(&proj2_control);
    ImGui::SameLine();
    ParameterGUI::draw(&proj3_control);

    ImGui::NewLine();

    ParameterGUI::draw(&test_start);
    ImGui::SameLine();
    ParameterGUI::draw(&test_stop);

    ParameterGUI::draw(&reset);
    ImGui::SameLine();
    ParameterGUI::draw(&print_status);

    ImGui::NewLine();

    ParameterGUI::draw(&power_normal);
    ImGui::SameLine();
    ParameterGUI::draw(&power_standby);

    ImGui::NewLine();

    ParameterGUI::draw(&video_mode);
    ImGui::SameLine();
    ParameterGUI::draw(&varExpPat_start);

    ImGui::RadioButton("8b", &patternSequenceIndex, 0);
    ImGui::SameLine();
    ImGui::RadioButton("7b", &patternSequenceIndex, 1);
    ImGui::SameLine();
    ImGui::RadioButton("4b", &patternSequenceIndex, 2);
    ImGui::SameLine();
    ImGui::RadioButton("2b", &patternSequenceIndex, 3);

    ParameterGUI::draw(&pattern_start);
    ImGui::SameLine();
    ParameterGUI::draw(&pattern_stop);

    ImGui::NewLine();

    ParameterGUI::draw(&apply_led);

    if (ImGui::CollapsingHeader("LED Currents",
                                ImGuiTreeNodeFlags_CollapsingHeader)) {
      ImGui::Indent();
      ParameterGUI::draw(&proj0_red);
      ParameterGUI::draw(&proj0_green);
      ParameterGUI::draw(&proj0_blue);
      ImGui::NewLine();
      ParameterGUI::draw(&proj1_red);
      ParameterGUI::draw(&proj1_green);
      ParameterGUI::draw(&proj1_blue);
      ImGui::NewLine();
      ParameterGUI::draw(&proj2_red);
      ParameterGUI::draw(&proj2_green);
      ParameterGUI::draw(&proj2_blue);
      ImGui::NewLine();
      ParameterGUI::draw(&proj3_red);
      ParameterGUI::draw(&proj3_green);
      ParameterGUI::draw(&proj3_blue);

      ImGui::NewLine();

      ParameterGUI::draw(&save_led);
      ImGui::Unindent();
    }
    ImGui::End();
  }

  void setupCallbacks() {
    device_list.registerChangeCallback(
        [&](float value) { multi350.printDevices(); });

    device_open.registerChangeCallback([&](float value) { multi350.open(); });

    reset.registerChangeCallback(
        [&](float value) { multi350.softwareReset(); });

    print_status.registerChangeCallback([&](float value) {
      multi350.updateStatus();
      multi350.printStatus();
    });

    test_start.registerChangeCallback([&](float value) {
      multi350.startTestPattern(multi350::TestPattern::COLOR_BARS);
    });

    test_stop.registerChangeCallback(
        [&](float value) { multi350.stopTestPattern(); });

    apply_led.registerChangeCallback([&](float value) {
      std::vector<multi350::LEDCurrent> currents;
      currents.emplace_back(static_cast<uint8_t>(proj0_red.get()),
                            static_cast<uint8_t>(proj0_green.get()),
                            static_cast<uint8_t>(proj0_blue.get()));
      currents.emplace_back(static_cast<uint8_t>(proj1_red.get()),
                            static_cast<uint8_t>(proj1_green.get()),
                            static_cast<uint8_t>(proj1_blue.get()));
      currents.emplace_back(static_cast<uint8_t>(proj2_red.get()),
                            static_cast<uint8_t>(proj2_green.get()),
                            static_cast<uint8_t>(proj2_blue.get()));
      currents.emplace_back(static_cast<uint8_t>(proj3_red.get()),
                            static_cast<uint8_t>(proj3_green.get()),
                            static_cast<uint8_t>(proj3_blue.get()));

      multi350.setLEDCurrent(currents);
    });

    save_led.registerChangeCallback(
        [&](float value) { preset_currents.storePreset("currents"); });

    varExpPat_start.registerChangeCallback([&](float value) {
      multi350.startVarExpPatSequence(varExpPatSequences);
    });

    pattern_start.registerChangeCallback([&](float value) {
      multi350.startPatternSequence(patternSequences[patternSequenceIndex]);
    });

    pattern_stop.registerChangeCallback(
        [&](float value) { multi350.stopPatternSequence(); });

    video_mode.registerChangeCallback(
        [&](float value) { multi350.startVideoMode(); });

    device_close.registerChangeCallback([&](float value) { multi350.close(); });

    proj0_control.registerChangeCallback([&](float value) {
      auto &projector = multi350.getProjector(0);
      projector.controlled = value;
    });

    proj1_control.registerChangeCallback([&](float value) {
      auto &projector = multi350.getProjector(1);
      projector.controlled = value;
    });

    proj2_control.registerChangeCallback([&](float value) {
      auto &projector = multi350.getProjector(2);
      projector.controlled = value;
    });

    proj3_control.registerChangeCallback([&](float value) {
      auto &projector = multi350.getProjector(3);
      projector.controlled = value;
    });

    power_normal.registerChangeCallback([&](float value) {
      multi350.setPowerMode(multi350::PowerMode::NORMAL);
    });

    power_standby.registerChangeCallback([&](float value) {
      multi350.setPowerMode(multi350::PowerMode::STANDBY);
    });

    apply_indices.registerChangeCallback(
        [&](float value) { multi350.updateIndices(usb_idx); });

    save_indices.registerChangeCallback(
        [&](float value) { preset_projectors.storePreset("projectors"); });
  }

  void setupPatternSequences() {
    using namespace multi350;
    // TODO: check if insert black is needed on final pattern
    patternSequences[0].addPattern<Pattern::Pattern8bit>(
        Pattern::TriggerType::EXTERNAL_POSITIVE,
        Pattern::Pattern8bit::G7G6G5G4G3G2G1G0, 8, Pattern::LEDSelect::GREEN);
    patternSequences[0].addPattern<Pattern::Pattern8bit>(
        Pattern::TriggerType::NO_TRIGGER,
        Pattern::Pattern8bit::R7R6R5R4R3R2R1R0, 8, Pattern::LEDSelect::RED);
    patternSequences[0].addPattern<Pattern::Pattern8bit>(
        Pattern::TriggerType::NO_TRIGGER,
        Pattern::Pattern8bit::B7B6B5B4B3B2B1B0, 8, Pattern::LEDSelect::BLUE,
        false, true, false);
    patternSequences[0].setExposure(8333);
    patternSequences[0].setPeriod(8333);

    patternSequences[1].addPattern<Pattern::Pattern7bit>(
        Pattern::TriggerType::EXTERNAL_POSITIVE,
        Pattern::Pattern7bit::G7G6G5G4G3G2G1, 7, Pattern::LEDSelect::GREEN);
    patternSequences[1].addPattern<Pattern::Pattern7bit>(
        Pattern::TriggerType::NO_TRIGGER, Pattern::Pattern7bit::R7R6R5R4R3R2R1,
        7, Pattern::LEDSelect::RED);
    patternSequences[1].addPattern<Pattern::Pattern7bit>(
        Pattern::TriggerType::NO_TRIGGER, Pattern::Pattern7bit::B7B6B5B4B3B2B1,
        7, Pattern::LEDSelect::BLUE, false, true, false);
    patternSequences[1].setExposure(4500);
    patternSequences[1].setPeriod(4500);

    patternSequences[2].addPattern<Pattern::Pattern4bit>(
        Pattern::TriggerType::EXTERNAL_POSITIVE, Pattern::Pattern4bit::G7G6G5G4,
        4, Pattern::LEDSelect::GREEN);
    patternSequences[2].addPattern<Pattern::Pattern4bit>(
        Pattern::TriggerType::NO_TRIGGER, Pattern::Pattern4bit::R7R6R5R4, 4,
        Pattern::LEDSelect::RED);
    patternSequences[2].addPattern<Pattern::Pattern4bit>(
        Pattern::TriggerType::NO_TRIGGER, Pattern::Pattern4bit::B7B6B5B4, 4,
        Pattern::LEDSelect::BLUE, false, true, false);
    patternSequences[2].setExposure(1700);
    patternSequences[2].setPeriod(1700);

    patternSequences[3].addPattern<Pattern::Pattern2bit>(
        Pattern::TriggerType::EXTERNAL_POSITIVE, Pattern::Pattern2bit::G7G6, 2,
        Pattern::LEDSelect::GREEN);
    patternSequences[3].addPattern<Pattern::Pattern2bit>(
        Pattern::TriggerType::NO_TRIGGER, Pattern::Pattern2bit::R7R6, 2,
        Pattern::LEDSelect::RED);
    patternSequences[3].addPattern<Pattern::Pattern2bit>(
        Pattern::TriggerType::NO_TRIGGER, Pattern::Pattern2bit::B7B6, 2,
        Pattern::LEDSelect::BLUE, false, true, false);
    patternSequences[3].setExposure(700);
    patternSequences[3].setPeriod(700);

    varExpPatSequences.addVarExpPat<Pattern::Pattern4bit>(
        1700, 1700, Pattern::TriggerType::EXTERNAL_POSITIVE,
        Pattern::Pattern4bit::G7G6G5G4, 4, Pattern::LEDSelect::GREEN);
    varExpPatSequences.addVarExpPat<Pattern::Pattern4bit>(
        4250, 4250, Pattern::TriggerType::NO_TRIGGER,
        Pattern::Pattern4bit::R7R6R5R4, 4, Pattern::LEDSelect::RED);
    varExpPatSequences.addVarExpPat<Pattern::Pattern4bit>(
        1700, 1700, Pattern::TriggerType::NO_TRIGGER,
        Pattern::Pattern4bit::B7B6B5B4, 4, Pattern::LEDSelect::BLUE, false,
        true, false);
  }

  void setupProjectorIndices() {
    // // TODO: adjust for less than 4 projectors
    // if (multi350.deviceNum() != 4) {
    //   std::cout << "[Controller] Only " << multi350.deviceNum()
    //             << " projectors are connected. Using "
    //                "default values"
    //             << std::endl;
    //   return;
    // }

    usb_idx.push_back(proj0_index.get());
    usb_idx.push_back(proj1_index.get());
    usb_idx.push_back(proj2_index.get());
    usb_idx.push_back(proj3_index.get());

    for (int i = 0; i < 4; ++i) {
      usb_names.push_back("USB" + std::to_string(usb_idx[i]));
    }

    // multi350.updateIndices(usb_idx);
  }
};

#endif