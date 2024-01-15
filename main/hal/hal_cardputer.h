/**
 * @file hal_cardputer.h
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-09-22
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "hal.h"

namespace HAL
{
    class HalCardputer : public Hal
    {
        private:
            void _display_init();
            void _keyboard_init();
            void _mic_init();
            void _speaker_init();
            void _button_init();
            void _bat_init();
            void _sdcard_init();

        public:
            std::string type() override { return "cardputer"; }
            void init() override;
            void playKeyboardSound() override { _speaker->setVolume(72); _speaker->tone(5000, 20); }
            void playLastSound() override { _speaker->setVolume(32); _speaker->tone(6000, 20); }
            void playNextSound() override { _speaker->setVolume(64); _speaker->tone(7000, 20); }
            uint8_t getBatLevel() override;

        public:
            static void MicTest(HalCardputer* hal);
            static void SpeakerTest(HalCardputer* hal);
            static void LcdBgLightTest(HalCardputer* hal);
    };
}
